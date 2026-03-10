#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
VER_FILE="${ROOT_DIR}/src/libmoex/ver.h"
BUILD_DIR="${ROOT_DIR}/build"
BUILD_APP_DIR="${BUILD_DIR}/MachOExplorer.app"
BUILD_APP_BIN="${BUILD_APP_DIR}/Contents/MacOS/MachOExplorer"
BUILD_APP_FLAT_BIN="${BUILD_DIR}/MachOExplorer"
APP_DIR="${ROOT_DIR}/dist/macos/MachOExplorer.app"
DMG_PATH="${ROOT_DIR}/dist/macos/MachOExplorer.dmg"
ICON_PATH="${ROOT_DIR}/src/MachOExplorer.icns"
CODE_SIGN_IDENTITY="${CODE_SIGN_IDENTITY:-Developer ID Application: Feng Zhu (YPV49M8592)}"
NOTARYTOOL_PROFILE="${NOTARYTOOL_PROFILE:-}"
APPLE_ID="${APPLE_ID:-}"
APPLE_TEAM_ID="${APPLE_TEAM_ID:-}"
APPLE_APP_SPECIFIC_PASSWORD="${APPLE_APP_SPECIFIC_PASSWORD:-${APPLE_PASSWORD:-${APP_SPECIFIC_PASSWORD:-}}}"

NO_HOMEBREW=0
NO_RELEASE=0
NO_TAG=0
NO_PUSH=0

usage() {
  cat <<EOF
Usage: $0 [--no-homebrew] [--no-release] [--no-tag] [--no-push]

Default behavior:
  1) bump patch version
  2) build app + dmg
  3) sign app with Developer ID
  4) notarize dmg + staple ticket
  5) commit + push
  6) create git tag + push tag
  7) create/update GitHub release + upload dmg
  8) update Homebrew cask

Required env for macOS release:
  CODE_SIGN_IDENTITY              default: Developer ID Application: Feng Zhu (YPV49M8592)
  NOTARYTOOL_PROFILE              preferred keychain profile for xcrun notarytool
  or APPLE_ID + APPLE_TEAM_ID + APPLE_APP_SPECIFIC_PASSWORD
EOF
}

require_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "missing required command: $1" >&2
    exit 1
  fi
}

resolve_qt_prefix() {
  local has_qt_config
  has_qt_config() {
    local prefix="$1"
    [[ -f "${prefix}/lib/cmake/Qt6/Qt6Config.cmake" || -f "${prefix}/lib/cmake/Qt5/Qt5Config.cmake" ]]
  }

  if [[ -n "${CMAKE_PREFIX_PATH:-}" ]]; then
    if has_qt_config "${CMAKE_PREFIX_PATH}"; then
      printf '%s\n' "${CMAKE_PREFIX_PATH}"
      return 0
    fi
  fi

  local cache_file
  local cache_prefix
  local cache_qt_dir
  local candidate

  cache_file="${BUILD_DIR}/CMakeCache.txt"
  if [[ -f "${cache_file}" ]]; then
    cache_prefix="$(sed -n 's/^CMAKE_PREFIX_PATH:.*=//p' "${cache_file}" | head -n1)"
    if [[ -n "${cache_prefix}" && -d "${cache_prefix}" ]] && has_qt_config "${cache_prefix}"; then
      printf '%s\n' "${cache_prefix}"
      return 0
    fi

    cache_qt_dir="$(sed -n 's/^\(Qt[56]_DIR\|QT_DIR\):.*=//p' "${cache_file}" | head -n1)"
    if [[ -n "${cache_qt_dir}" ]]; then
      candidate="${cache_qt_dir%/lib/cmake/Qt[56]}"
      if [[ -d "${candidate}" ]] && has_qt_config "${candidate}"; then
        printf '%s\n' "${candidate}"
        return 0
      fi
    fi
  fi

  for candidate in \
    "/Users/eevv/Qt/6.10.2/macos" \
    "/Users/eevv/Qt/6.9.3/macos"
  do
    if [[ -d "${candidate}" ]] && has_qt_config "${candidate}"; then
      printf '%s\n' "${candidate}"
      return 0
    fi
  done

  candidate="$(find /Users/eevv/Qt -type f \( -name Qt6Config.cmake -o -name Qt5Config.cmake \) 2>/dev/null | head -n1)"
  if [[ -n "${candidate}" ]]; then
    candidate="${candidate%/lib/cmake/Qt[56]/Qt[56]Config.cmake}"
    if [[ -d "${candidate}" ]] && has_qt_config "${candidate}"; then
      printf '%s\n' "${candidate}"
      return 0
    fi
  fi

  return 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --no-homebrew) NO_HOMEBREW=1; shift ;;
    --no-release) NO_RELEASE=1; shift ;;
    --no-tag) NO_TAG=1; shift ;;
    --no-push) NO_PUSH=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) echo "unknown arg: $1" >&2; usage; exit 1 ;;
  esac
done

require_cmd git
require_cmd cmake
require_cmd hdiutil
require_cmd gh
require_cmd codesign
require_cmd security
require_cmd xcrun
require_cmd spctl

if [[ -n "$(git status --porcelain)" ]]; then
  echo "working tree is not clean; commit or stash first" >&2
  exit 1
fi

if ! gh auth status >/dev/null 2>&1; then
  echo "gh not authenticated. run: gh auth login" >&2
  exit 1
fi

if ! security find-identity -v -p codesigning | grep -Fq "\"${CODE_SIGN_IDENTITY}\""; then
  cat >&2 <<EOF
Signing identity not available in keychain:
  ${CODE_SIGN_IDENTITY}

Available code signing identities:
$(security find-identity -v -p codesigning | sed 's/^/  /')
EOF
  exit 1
fi

if [[ -z "${NOTARYTOOL_PROFILE}" ]] && { [[ -z "${APPLE_ID}" ]] || [[ -z "${APPLE_TEAM_ID}" ]] || [[ -z "${APPLE_APP_SPECIFIC_PASSWORD}" ]]; }; then
  cat >&2 <<EOF
Notarization credentials missing.
Set one of:
  1) NOTARYTOOL_PROFILE=<keychain-profile-name>
  2) APPLE_ID + APPLE_TEAM_ID + APPLE_APP_SPECIFIC_PASSWORD
EOF
  exit 1
fi

new_version="$("${ROOT_DIR}/scripts/inc_patch_version.sh")"
tag="v${new_version}"
echo "new version: ${new_version}"

if [[ -f "${VER_FILE}" ]]; then
  echo "version file updated: ${VER_FILE}"
fi

QT_PREFIX="$(resolve_qt_prefix || true)"
if [[ -z "${QT_PREFIX}" ]]; then
  echo "unable to determine Qt CMake prefix; set CMAKE_PREFIX_PATH=/path/to/Qt/<ver>/macos" >&2
  exit 1
fi
echo "using Qt prefix: ${QT_PREFIX}"

cmake -S "${ROOT_DIR}/src" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${QT_PREFIX}"
cmake --build "${BUILD_DIR}" -j8
"${ROOT_DIR}/tests/regression/run_all.sh"

MACDEPLOYQT="${MACDEPLOYQT:-/Users/eevv/Qt/6.10.2/macos/bin/macdeployqt}"
if [[ ! -x "${MACDEPLOYQT}" ]]; then
  MACDEPLOYQT="$(command -v macdeployqt || true)"
fi
if [[ -z "${MACDEPLOYQT}" || ! -x "${MACDEPLOYQT}" ]]; then
  echo "macdeployqt not found; set MACDEPLOYQT=/path/to/macdeployqt" >&2
  exit 1
fi

rm -rf "${APP_DIR}"
mkdir -p "$(dirname "${APP_DIR}")"
if [[ -d "${BUILD_APP_DIR}" ]]; then
  cp -R "${BUILD_APP_DIR}" "${APP_DIR}"
elif [[ -x "${BUILD_APP_FLAT_BIN}" ]]; then
  mkdir -p "${APP_DIR}/Contents/MacOS" "${APP_DIR}/Contents/Resources"
  cp "${BUILD_APP_FLAT_BIN}" "${APP_DIR}/Contents/MacOS/MachOExplorer"
  if [[ -f "${ICON_PATH}" ]]; then
    cp "${ICON_PATH}" "${APP_DIR}/Contents/Resources/MachOExplorer.icns"
  fi
  cat > "${APP_DIR}/Contents/Info.plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleDevelopmentRegion</key><string>en</string>
  <key>CFBundleExecutable</key><string>MachOExplorer</string>
  <key>CFBundleIdentifier</key><string>com.everettjf.machoexplorer</string>
  <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
  <key>CFBundleIconFile</key><string>MachOExplorer.icns</string>
  <key>CFBundleName</key><string>MachOExplorer</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>${new_version}</string>
  <key>CFBundleVersion</key><string>${new_version}</string>
  <key>LSMinimumSystemVersion</key><string>11.0</string>
  <key>NSHighResolutionCapable</key><true/>
  <key>NSRequiresAquaSystemAppearance</key><false/>
</dict>
</plist>
PLIST
else
  echo "built app not found; expected ${BUILD_APP_DIR} or ${BUILD_APP_FLAT_BIN}" >&2
  exit 1
fi

"${MACDEPLOYQT}" "${APP_DIR}" -verbose=0
mkdir -p "${ROOT_DIR}/dist/macos"
codesign --force --deep --options runtime --timestamp --sign "${CODE_SIGN_IDENTITY}" "${APP_DIR}"
codesign --verify --deep --strict --verbose=2 "${APP_DIR}"
spctl --assess -vv "${APP_DIR}"
hdiutil create -volname "MachOExplorer" -srcfolder "${APP_DIR}" -ov -format UDZO "${DMG_PATH}"

echo "Submitting DMG for notarization..."
if [[ -n "${NOTARYTOOL_PROFILE}" ]]; then
  xcrun notarytool submit "${DMG_PATH}" --keychain-profile "${NOTARYTOOL_PROFILE}" --wait
else
  xcrun notarytool submit "${DMG_PATH}" \
    --apple-id "${APPLE_ID}" \
    --team-id "${APPLE_TEAM_ID}" \
    --password "${APPLE_APP_SPECIFIC_PASSWORD}" \
    --wait
fi

echo "Stapling notarization ticket..."
xcrun stapler staple "${DMG_PATH}"
xcrun stapler validate "${DMG_PATH}"

git add "${VER_FILE}" "${ROOT_DIR}/README.md" "${ROOT_DIR}/README.zh-CN.md"
git commit -m "release: bump to ${tag}"

if [[ "${NO_PUSH}" -eq 0 ]]; then
  git push origin master
fi

if [[ "${NO_TAG}" -eq 0 ]]; then
  if git rev-parse -q --verify "refs/tags/${tag}" >/dev/null; then
    echo "tag already exists: ${tag}" >&2
    exit 1
  fi
  git tag "${tag}"
  if [[ "${NO_PUSH}" -eq 0 ]]; then
    git push origin "${tag}"
  fi
fi

if [[ "${NO_RELEASE}" -eq 0 ]]; then
  if gh release view "${tag}" >/dev/null 2>&1; then
    gh release upload "${tag}" "${DMG_PATH}" --clobber
  else
    gh release create "${tag}" "${DMG_PATH}" -t "${tag}" -n "MachOExplorer ${tag}"
  fi
fi

if [[ "${NO_HOMEBREW}" -eq 0 ]]; then
  "${ROOT_DIR}/scripts/release_homebrew.sh" --version "${new_version}" --dmg "${DMG_PATH}"
fi

echo "deploy done: ${tag}"
