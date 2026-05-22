#!/usr/bin/env bash
set -euo pipefail

# Build MachOExplorer on Linux.
#
# Requirements:
#   - CMake >= 3.16
#   - Qt 6 base + concurrent (e.g. Debian/Ubuntu: qt6-base-dev)
#   - A C++14 compiler (g++ or clang++)
#   - Optional: libcapstone-dev for __TEXT,__text disassembly
#
# Usage:
#   ./build_linux.sh [build_type]
# where build_type defaults to Release.

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
BUILD_TYPE="${1:-Release}"

cmake -S "${ROOT_DIR}/src" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "---------"
echo "built: ${BUILD_DIR}/MachOExplorer"
echo "run:   ${BUILD_DIR}/MachOExplorer"
echo "cli:   ${BUILD_DIR}/MachOExplorer --cli <file>"
echo "---------"
