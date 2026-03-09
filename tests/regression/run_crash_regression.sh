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

# dyld magic prefix but too short.
printf 'dyld_v1   arm64e' > "${TMP_DIR}/truncated_dyld.cache"

# dyld header with unreasonable mapping/images count.
# magic[16] + mappingOffset(4)=0x40 + mappingCount(4)=0xFFFFFFFF
# imagesOffset(4)=0x80 + imagesCount(4)=0xFFFFFFFF, then zero padding.
printf '\x64\x79\x6c\x64\x5f\x76\x31\x20\x20\x20\x61\x72\x6d\x36\x34\x65' > "${TMP_DIR}/huge_count_dyld.cache"
printf '\x40\x00\x00\x00\xff\xff\xff\xff\x80\x00\x00\x00\xff\xff\xff\xff' >> "${TMP_DIR}/huge_count_dyld.cache"
dd if=/dev/zero bs=1 count=160 >> "${TMP_DIR}/huge_count_dyld.cache" 2>/dev/null

FAIL=0
for f in "${TMP_DIR}"/*; do
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
    echo "[ok] malformed-ish input handled safely (accepted): $(basename "${f}")"
  else
    echo "[ok] malformed input rejected safely: $(basename "${f}")"
  fi
done

if [[ "${FAIL}" -ne 0 ]]; then
  echo "crash-regression: failed"
  exit 1
fi

echo "crash-regression: passed"
