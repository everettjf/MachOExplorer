#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PARSER_BIN="${ROOT_DIR}/build/moex-parse"

if [[ ! -x "${PARSER_BIN}" ]]; then
  echo "missing parser binary: ${PARSER_BIN}"
  echo "build first: cmake --build build -j8"
  exit 2
fi

TMP_DIR="$(mktemp -d "${TMPDIR:-/tmp}/moex-crash-reg.XXXXXX")"
trap 'rm -rf "${TMP_DIR}"' EXIT

# Empty file.
: > "${TMP_DIR}/empty.bin"

# Truncated Mach-O magic only.
printf '\xcf\xfa\xed' > "${TMP_DIR}/truncated_macho.bin"

# Archive magic without member header.
printf '!<arch>\n' > "${TMP_DIR}/broken_archive.a"

# Archive with invalid member size field (non-decimal).
printf '!<arch>\n' > "${TMP_DIR}/bad_size_archive.a"
# name[16] mtime[12] uid[6] gid[6] mode[8] size[10] fmag[2]
printf '%-16s%-12s%-6s%-6s%-8s%-10s`\n' \
  "bad.o/" "0" "0" "0" "100644" "12x" >> "${TMP_DIR}/bad_size_archive.a"

# dyld magic prefix but too short.
printf 'dyld_v1   arm64e' > "${TMP_DIR}/truncated_dyld.cache"

# dyld header with unreasonable mapping/images count.
# magic[16] + mappingOffset(4)=0x40 + mappingCount(4)=0xFFFFFFFF
# imagesOffset(4)=0x80 + imagesCount(4)=0xFFFFFFFF, then zero padding.
printf '\x64\x79\x6c\x64\x5f\x76\x31\x20\x20\x20\x61\x72\x6d\x36\x34\x65' > "${TMP_DIR}/huge_count_dyld.cache"
printf '\x40\x00\x00\x00\xff\xff\xff\xff\x80\x00\x00\x00\xff\xff\xff\xff' >> "${TMP_DIR}/huge_count_dyld.cache"
dd if=/dev/zero bs=1 count=160 >> "${TMP_DIR}/huge_count_dyld.cache" 2>/dev/null

# dyld header with one mapping entry whose file range is out of bounds.
bad_map="${TMP_DIR}/bad_mapping_range_dyld.cache"
truncate -s 192 "${bad_map}"
printf '\x64\x79\x6c\x64\x5f\x76\x31\x20\x20\x20\x61\x72\x6d\x36\x34\x65' | dd of="${bad_map}" bs=1 seek=0 conv=notrunc 2>/dev/null
printf '\x60\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' | dd of="${bad_map}" bs=1 seek=16 conv=notrunc 2>/dev/null
# mapping: address=0 size=0x2000 fileOffset=0x100000 maxProt=0 initProt=0
printf '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' \
  | dd of="${bad_map}" bs=1 seek=96 conv=notrunc 2>/dev/null

# Valid minimal FAT64 with one x86_64 MH_MAGIC_64 image at offset 0x1000.
valid_fat64="${TMP_DIR}/valid_fat64.bin"
truncate -s 4096 "${valid_fat64}"
printf '\xca\xfe\xba\xbf\x00\x00\x00\x01' \
  | dd of="${valid_fat64}" bs=1 seek=0 conv=notrunc 2>/dev/null
printf '\x01\x00\x00\x07\x00\x00\x00\x03' \
  | dd of="${valid_fat64}" bs=1 seek=8 conv=notrunc 2>/dev/null
printf '\x00\x00\x00\x00\x00\x00\x10\x00' \
  | dd of="${valid_fat64}" bs=1 seek=16 conv=notrunc 2>/dev/null
printf '\x00\x00\x00\x00\x00\x00\x00\x20' \
  | dd of="${valid_fat64}" bs=1 seek=24 conv=notrunc 2>/dev/null
printf '\x00\x00\x00\x0c\x00\x00\x00\x00' \
  | dd of="${valid_fat64}" bs=1 seek=32 conv=notrunc 2>/dev/null
printf '\xcf\xfa\xed\xfe\x07\x00\x00\x01\x03\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' \
  | dd of="${valid_fat64}" bs=1 seek=4096 conv=notrunc 2>/dev/null

FAIL=0
TOTAL=0
REJECTED=0
ACCEPTED=0
for f in "${TMP_DIR}"/*; do
  if [[ "$(basename "${f}")" == "valid_fat64.bin" ]]; then
    continue
  fi
  TOTAL=$((TOTAL + 1))
  set +e
  "${PARSER_BIN}" "${f}" >/dev/null 2>&1
  rc=$?
  set -e

  if [[ "${rc}" -ge 128 ]]; then
    echo "[fail] parser likely crashed (signal exit=${rc}) for: ${f}"
    FAIL=1
    continue
  fi

  if [[ "${rc}" -eq 0 ]]; then
    ACCEPTED=$((ACCEPTED + 1))
    echo "[ok] malformed-ish input handled safely (accepted): $(basename "${f}")"
  else
    REJECTED=$((REJECTED + 1))
    echo "[ok] malformed input rejected safely: $(basename "${f}")"
  fi
done

set +e
"${PARSER_BIN}" "${valid_fat64}" >/dev/null 2>&1
valid_rc=$?
set -e
if [[ "${valid_rc}" -ne 0 ]]; then
  echo "[fail] valid FAT64 should parse successfully: ${valid_fat64}"
  FAIL=1
else
  echo "[ok] valid FAT64 parsed successfully: $(basename "${valid_fat64}")"
fi

if [[ "${FAIL}" -ne 0 ]]; then
  echo "crash-regression: failed"
  exit 1
fi

echo "crash-regression: passed (total=${TOTAL} rejected=${REJECTED} accepted=${ACCEPTED})"
