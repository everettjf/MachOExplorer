# MachOExplorer AGENTS Guide

This guide describes how autonomous contributors should work in this repository safely and effectively.

## Project Snapshot
- App: `MachOExplorer` (Qt/C++)
- Domain: Mach-O / Fat Mach-O / `.a` archive / dyld shared cache analysis
- Current release line: `v2.0.x` (repo currently at `v2.0.1`)
- Platforms: macOS first-class, Windows supported

## Repository Layout
- `src/`:
  - `libmoex/` parser and view-node model
  - `src/` Qt UI/controllers/widgets/dialogs
  - `tools/` CLI tools (`moex-parse`, `moex-cache-list`, `moex-cache-extract`, `moex-diff`)
- `tests/regression/`:
  - `run_all.sh` unified regression entry
  - includes parser regression, cache tool smoke tests, malformed-input crash regression
- `sample/`: reproducible sample binaries
- `scripts/`:
  - `inc_patch_version.sh` patch version bump helper
  - `release_homebrew.sh` Homebrew cask updater
  - `build_windows_installer.ps1` Windows installer pipeline helper
- `packaging/windows/MachOExplorer.iss`: Inno Setup installer script
- `deploy.sh`: one-command release pipeline (bump/build/test/tag/release/homebrew)

## Build & Run (macOS)
```bash
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
./build/MachOExplorer
```

## Regression (required before/after meaningful changes)
```bash
tests/regression/run_all.sh
```

## Release Workflows

### Patch bump only
```bash
scripts/inc_patch_version.sh
```

### Homebrew cask update
- Dry run:
```bash
scripts/release_homebrew.sh --version <x.y.z> --dry-run
```
- Real push (requires DMG):
```bash
scripts/release_homebrew.sh --version <x.y.z> --dmg /path/to/MachOExplorer.dmg
```

### Full deploy pipeline
```bash
./deploy.sh
```
Default behavior:
1. bump patch
2. configure/build
3. run `tests/regression/run_all.sh`
4. package `.app` + `.dmg`
5. commit + push
6. tag + push tag
7. GitHub release upload
8. Homebrew cask update

Flags:
- `--no-homebrew`
- `--no-release`
- `--no-tag`
- `--no-push`

## Windows Packaging Guidance (2026)
- Preferred installer tool for classic Next/Next/Finish: **Inno Setup**
- Build helper:
```powershell
powershell -ExecutionPolicy Bypass -File scripts/build_windows_installer.ps1 `
  -QtBin "C:\Qt\6.9.3\msvc2022_64\bin" `
  -BuildDir "build-win" `
  -Config "Release"
```

## Coding Expectations
- Preserve existing code style and architecture boundaries:
  - parser logic in `src/libmoex`
  - UI logic in `src/src`
- Avoid broad formatting-only churn.
- Keep changes scoped and reversible.
- Add/adjust regression coverage for parser hardening changes when possible.

## Safety Rules
- Do not use destructive git commands (`reset --hard`, forced checkout) unless explicitly requested.
- Do not revert unrelated local changes.
- If unexpected external modifications appear, pause and confirm before continuing.

## Quick QA Checklist for PRs
- [ ] Project builds successfully
- [ ] `tests/regression/run_all.sh` passes
- [ ] Relevant docs updated (`README*`, packaging notes, this file when workflow changes)
- [ ] No unrelated refactors included
