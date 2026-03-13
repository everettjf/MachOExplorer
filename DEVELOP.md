# DEVELOP

This document is for contributors and maintainers.

## Prerequisites
- CMake >= 3.16
- Qt 6 (recommended)
- C++14 compiler
- macOS release tooling: `macdeployqt`, `hdiutil`, `gh`

## Build

### macOS
```bash
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
./build/MachOExplorer
```

### Windows
```powershell
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvcXXXX_64"
cmake --build build --config Release
```

## Regression
```bash
tests/regression/run_all.sh
```

Includes:
- parser regression (`moex-parse`)
- dyld cache tools smoke checks
- malformed-input crash regression suite

## Key Scripts
- `scripts/inc_patch_version.sh`: bump patch version in repo files
- `deploy.sh`: one-command release pipeline (bump/build/test/tag/release/homebrew)
- `scripts/release_homebrew.sh`: update Homebrew cask (supports `--dry-run`)
- `scripts/build_windows_installer.ps1`: Windows installer helper
- `scripts/build_windows_release.ps1`: Windows build + installer + GitHub release asset upload

## Packaging
- Windows Inno Setup script: `packaging/windows/MachOExplorer.iss`
- Packaging notes: `docs/release_packaging.md`

## Windows Release Upload
After the macOS release/tag is already published, run this on a Windows machine:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/build_windows_release.ps1 `
  -QtBin "C:\Qt\6.9.3\msvc2022_64\bin"
```

Requirements:
- `cmake`
- `windeployqt`
- `iscc`
- `gh` authenticated with release upload access

## Full Release (macOS)
```bash
./deploy.sh
```

Optional flags:
- `--no-homebrew`
- `--no-release`
- `--no-tag`
- `--no-push`

## Coding & PR Checklist
- Keep changes scoped and avoid unrelated refactors.
- Preserve existing style in touched areas.
- Run build + `tests/regression/run_all.sh` before commit.
- Update docs if workflow/behavior changed.
