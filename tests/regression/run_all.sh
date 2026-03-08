#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"

"${ROOT_DIR}/tests/regression/run_regression.sh"
"${ROOT_DIR}/tests/regression/run_cache_tools_smoke.sh" "${1:-}"
"${ROOT_DIR}/tests/regression/run_crash_regression.sh"

echo "regression-all: passed"
