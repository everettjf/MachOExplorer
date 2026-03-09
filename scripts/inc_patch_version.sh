#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
VER_FILE="${ROOT_DIR}/src/libmoex/ver.h"
README_EN="${ROOT_DIR}/README.md"
README_ZH="${ROOT_DIR}/README.zh-CN.md"

if [[ ! -f "${VER_FILE}" ]]; then
  echo "version file not found: ${VER_FILE}" >&2
  exit 1
fi

current="$(sed -n 's/^#define LIBMOEX_VERSION "\(.*\)"/\1/p' "${VER_FILE}" | head -1)"
if [[ -z "${current}" ]]; then
  echo "failed to parse LIBMOEX_VERSION from ${VER_FILE}" >&2
  exit 1
fi

IFS='.' read -r major minor patch <<<"${current}"
if [[ -z "${major:-}" || -z "${minor:-}" || -z "${patch:-}" ]]; then
  echo "invalid semantic version: ${current}" >&2
  exit 1
fi
if ! [[ "${major}" =~ ^[0-9]+$ && "${minor}" =~ ^[0-9]+$ && "${patch}" =~ ^[0-9]+$ ]]; then
  echo "invalid semantic version: ${current}" >&2
  exit 1
fi

next="${major}.${minor}.$((patch + 1))"

sed -i '' "s/^#define LIBMOEX_VERSION \".*\"/#define LIBMOEX_VERSION \"${next}\"/" "${VER_FILE}"

if [[ -f "${README_EN}" ]]; then
  sed -i '' "s/Current major release: \`v${current}\`/Current major release: \`v${next}\`/" "${README_EN}" || true
  sed -i '' "s/releases\\/tag\\/v${current}/releases\\/tag\\/v${next}/g" "${README_EN}" || true
fi

if [[ -f "${README_ZH}" ]]; then
  sed -i '' "s/当前发布版本：\`v${current}\`/当前发布版本：\`v${next}\`/" "${README_ZH}" || true
  sed -i '' "s/releases\\/tag\\/v${current}/releases\\/tag\\/v${next}/g" "${README_ZH}" || true
fi

echo "${next}"
