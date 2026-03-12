#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
APP_BUNDLE_BIN="${BUILD_DIR}/MachOExplorer.app/Contents/MacOS/MachOExplorer"
APP_BIN="${APP_BUNDLE_BIN}"
SAMPLE_FILE="${ROOT_DIR}/sample/simple"
OUT_TEXT="/tmp/moex-cli-smoke.txt"
OUT_JSON="/tmp/moex-cli-smoke.json"

if [[ ! -x "${APP_BIN}" ]]; then
  echo "cli-smoke: missing app binary: ${APP_BIN}; build first"
  exit 2
fi

if [[ ! -f "${SAMPLE_FILE}" ]]; then
  echo "cli-smoke: missing sample file: ${SAMPLE_FILE}"
  exit 2
fi

"${APP_BIN}" --cli --max-rows 8 "${SAMPLE_FILE}" >"${OUT_TEXT}"
"${APP_BIN}" --cli --format json --max-rows 8 "${SAMPLE_FILE}" >"${OUT_JSON}"
"${APP_BIN}" --cli --format json --name-filter "Mach Header" --table-mode summary "${SAMPLE_FILE}" >"${OUT_JSON}.filtered"

if command -v rg >/dev/null 2>&1; then
  text_header_cmd=(rg -q "^file:")
  json_field_cmd=(rg -q '"analysis"')
  schema_field_cmd=(rg -q '"schemaVersion"')
  path_field_cmd=(rg -q '"path"')
  summary_mode_cmd=(rg -q '"tableMode": "summary"')
else
  text_header_cmd=(grep -qE "^file:")
  json_field_cmd=(grep -qE '"analysis"')
  schema_field_cmd=(grep -qE '"schemaVersion"')
  path_field_cmd=(grep -qE '"path"')
  summary_mode_cmd=(grep -qE '"tableMode": "summary"')
fi

if ! "${text_header_cmd[@]}" "${OUT_TEXT}"; then
  echo "cli-smoke: missing text output header"
  exit 1
fi

if ! "${json_field_cmd[@]}" "${OUT_JSON}"; then
  echo "cli-smoke: missing json analysis field"
  exit 1
fi

if ! "${schema_field_cmd[@]}" "${OUT_JSON}"; then
  echo "cli-smoke: missing schemaVersion field"
  exit 1
fi

if ! "${path_field_cmd[@]}" "${OUT_JSON}"; then
  echo "cli-smoke: missing node path field"
  exit 1
fi

"${APP_BIN}" --cli --format json --max-rows=4 --output "${OUT_JSON}" "${SAMPLE_FILE}" >/dev/null
if [[ ! -s "${OUT_JSON}" ]]; then
  echo "cli-smoke: output file not written"
  exit 1
fi

if ! "${summary_mode_cmd[@]}" "${OUT_JSON}.filtered"; then
  echo "cli-smoke: filtered summary output missing tableMode"
  exit 1
fi

echo "cli-smoke: passed"
