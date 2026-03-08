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
