#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
TAP_REPO="${TAP_REPO:-everettjf/homebrew-tap}"
CASK_PATH="${CASK_PATH:-Casks/machoexplorer.rb}"
TMP_DIR="${ROOT_DIR}/tmp"
TAP_DIR="${TMP_DIR}/homebrew-tap"
APP_NAME="MachOExplorer"
DEFAULT_RELEASE_URL_PREFIX="https://github.com/everettjf/MachOExplorer/releases/download"
DRY_RUN=0
VERSION=""
DMG_PATH=""
TAG=""

usage() {
  cat <<EOF
Usage: $0 [--version <x.y.z>] [--dmg <path>] [--tag <vX.Y.Z>] [--dry-run]

Examples:
  $0 --version 2.0.0 --dmg /path/to/MachOExplorer.dmg
  $0 --version 2.0.0 --dmg /path/to/MachOExplorer.dmg --dry-run

Env:
  TAP_REPO    default: everettjf/homebrew-tap
  CASK_PATH   default: Casks/machoexplorer.rb
EOF
}

require_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "Missing required command: $1" >&2
    exit 1
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --version)
      VERSION="$2"
      shift 2
      ;;
    --dmg)
      DMG_PATH="$2"
      shift 2
      ;;
    --tag)
      TAG="$2"
      shift 2
      ;;
    --dry-run)
      DRY_RUN=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown arg: $1" >&2
      usage
      exit 1
      ;;
  esac
done

require_cmd git
require_cmd shasum
require_cmd awk
require_cmd sed

if [[ -z "$VERSION" ]]; then
  VERSION="$(sed -n 's/^#define LIBMOEX_VERSION \"\\(.*\\)\"/\\1/p' "${ROOT_DIR}/src/libmoex/ver.h" | head -1)"
fi
if [[ -z "$VERSION" ]]; then
  echo "Cannot determine version. Pass --version." >&2
  exit 1
fi

if [[ -z "$TAG" ]]; then
  TAG="v${VERSION}"
fi

if [[ -z "$DMG_PATH" && "$DRY_RUN" -eq 0 ]]; then
  echo "Missing --dmg <path> for non-dry-run mode." >&2
  exit 1
fi

SHA256="TO_BE_FILLED_BY_RELEASE_SCRIPT"
if [[ -n "$DMG_PATH" ]]; then
  if [[ ! -f "$DMG_PATH" ]]; then
    echo "DMG not found: $DMG_PATH" >&2
    exit 1
  fi
  SHA256="$(shasum -a 256 "$DMG_PATH" | awk '{print $1}')"
fi

URL="${DEFAULT_RELEASE_URL_PREFIX}/${TAG}/${APP_NAME}.dmg"

echo "Preparing Homebrew cask update:"
echo "  repo    : ${TAP_REPO}"
echo "  cask    : ${CASK_PATH}"
echo "  version : ${VERSION}"
echo "  tag     : ${TAG}"
echo "  url     : ${URL}"
echo "  sha256  : ${SHA256}"
echo "  dry-run : ${DRY_RUN}"

rm -rf "${TAP_DIR}"
mkdir -p "${TMP_DIR}"
git clone "https://github.com/${TAP_REPO}.git" "${TAP_DIR}"

CASK_ABS="${TAP_DIR}/${CASK_PATH}"
mkdir -p "$(dirname "${CASK_ABS}")"

cat > "${CASK_ABS}" <<EOF
cask "machoexplorer" do
  version "${VERSION}"
  sha256 "${SHA256}"

  url "${URL}"
  name "MachOExplorer"
  desc "Mach-O and archive explorer"
  homepage "https://github.com/everettjf/MachOExplorer"

  app "MachOExplorer.app"
end
EOF

cd "${TAP_DIR}"
git add "${CASK_PATH}"

if [[ "$DRY_RUN" -eq 1 ]]; then
  echo
  echo "[dry-run] generated cask content:"
  cat "${CASK_ABS}"
  echo
  echo "[dry-run] git diff --cached:"
  git diff --cached
  exit 0
fi

git commit -m "bump machoexplorer to ${VERSION}"
git push
echo "Done. Updated ${TAP_REPO}/${CASK_PATH}"
