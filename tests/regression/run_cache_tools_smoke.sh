#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
CACHE_DEFAULT="/System/Volumes/Preboot/Cryptexes/OS/System/Library/dyld/dyld_shared_cache_arm64e"
CACHE_FILE="${1:-$CACHE_DEFAULT}"

if [ ! -x "${BUILD_DIR}/moex-cache-list" ] || [ ! -x "${BUILD_DIR}/moex-cache-extract" ]; then
  echo "cache-smoke: missing tools in ${BUILD_DIR}; build first"
  exit 2
fi

"${BUILD_DIR}/moex-cache-list" --help >/dev/null 2>&1
"${BUILD_DIR}/moex-cache-extract" --help >/dev/null 2>&1

if [ ! -f "${CACHE_FILE}" ]; then
  echo "cache-smoke: skipped (cache file not found: ${CACHE_FILE})"
  exit 0
fi

"${BUILD_DIR}/moex-cache-list" --json --limit=1 "${CACHE_FILE}" libobjc >/dev/null
"${BUILD_DIR}/moex-cache-extract" --dry-run --compact --all --max=1 "${CACHE_FILE}" libswift "/tmp/moex-cache-smoke" >/dev/null

echo "cache-smoke: passed"
