#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PARSER_BIN="${ROOT_DIR}/build/moex-parse"
CASE_FILE="${ROOT_DIR}/tests/regression/cases.txt"

if [[ ! -x "${PARSER_BIN}" ]]; then
  echo "missing parser binary: ${PARSER_BIN}"
  echo "build first: cmake --build build -j8"
  exit 2
fi

if [[ ! -f "${CASE_FILE}" ]]; then
  echo "missing cases file: ${CASE_FILE}"
  exit 2
fi

FAIL=0
while IFS= read -r relpath; do
  [[ -z "${relpath}" ]] && continue
  [[ "${relpath}" =~ ^# ]] && continue
  abspath="${ROOT_DIR}/${relpath}"
  if [[ ! -f "${abspath}" ]]; then
    echo "[skip] missing case ${relpath}"
    continue
  fi
  if ! "${PARSER_BIN}" "${abspath}"; then
    FAIL=1
  fi
done < "${CASE_FILE}"

if [[ "${FAIL}" -ne 0 ]]; then
  echo "regression: failed"
  exit 1
fi
echo "regression: passed"
