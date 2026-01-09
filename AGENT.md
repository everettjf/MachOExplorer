# MachOExplorer Agent Guide

This document equips autonomous contributors with the context needed to work safely and efficiently in the MachOExplorer repository.

## Project Overview
- MachOExplorer is a Qt-based macOS and Windows desktop application for exploring Mach-O binaries ("yet another MachOView").
- The GUI is written in C++/Qt (`MachOExplorer.pro`) and bundles parsing utilities (`libmoex`) together with vendorized dependencies such as fmt, cpp-mmaplib, and nlohmann/json.
- `v1.0 Alpha` is the most recent public release; the project is still unstable, so regressions are likely if changes are not carefully validated.

## Repository Map
- `src/` — Root Qt project files (`MachOExplorer.pro`, `.qrc`, icons) plus the `libmoex` parsing library.
- `src/src/` — GUI source tree (widgets, controllers, dialogs, docks, utilities, and a lightweight `test/` harness).
- `res/` — Packaging assets (`appdmg.json`, `background.png`, `windows_setup.iss`) consumed during release builds.
- `image/` — Icons, favicons, and screenshots referenced by documentation.
- `sample/` — Minimal Mach-O binaries (`simple`, `complex`, `simple.c`) for manual verification.
- `build_macos.sh` / `build_windows.bat` — Platform-specific scripts to configure, build, and package distributables.
- `dev.md` — Quick notes describing platform prerequisites (Qt locations, `appdmg`, Inno Setup).

## Run Locally
- Install Qt SDK 5.11.2 or newer for your target platform.
- **macOS:**
  1. Symlink Qt to `/opt/qt/5.11.2` as suggested in `dev.md` and install `appdmg` globally (`npm install -g appdmg`).
  2. Either run `./build_macos.sh` from the repo root or open `src/MachOExplorer.pro` inside Qt Creator and press Run.
- **Windows:**
  1. Install Qt 5.11.2 for MSVC 2017 64-bit plus `nmake`, `windeployqt`, and Inno Setup (see `res/windows_setup.iss`).
  2. Run `build_windows.bat` from a Visual Studio Developer Command Prompt or build via Qt Creator.
- Once built, launch the resulting `MachOExplorer` binary and open a Mach-O file (drag & drop or File → Open) to verify the UI comes up.

## Testing
- There is no automated unit-test or CI harness; testing is manual.
- Use the contents of `sample/` (e.g., `sample/simple`, `sample/complex`) to exercise headers, sections, and symbol panes after parser/UI changes.
- For parser work, extend `src/src/test/Test.cpp` or add new binaries to `sample/` so regressions can be reproduced consistently.

## Linting / Formatting
- No formatter is enforced. Preserve the existing Qt/C++ style: header/source pairs, camelCase member names, and indentation established in the repo.
- If tooling is needed, prefer `clang-format` with a conservative LLVM/Qt profile and restrict reformatting to the lines you touch.

## Build & Release
- **macOS pipeline:** `build_macos.sh` creates `build_macos/`, runs `/opt/qt/5.11.2/clang_64/bin/qmake`, builds with `make`, executes `macdeployqt` to create a DMG, then repackages it with `appdmg` before copying `MachOExplorer.dmg` into `dist/`.
- **Windows pipeline:** `build_windows.bat` invokes Qt's `qmake` and `nmake`, runs `windeployqt` inside `build_windows/release/dist`, and stages the portable bundle in `dist/MachOExplorer_Windows/`. The installer script `res/windows_setup.iss` is available if you need to regenerate an Inno Setup installer.
- Release artifacts (`MachOExplorer.dmg`, `MachOExplorer_Windows.zip`) are uploaded manually to GitHub Releases.

## Coding Style & Conventions
- Default recommendation: follow Qt Creator defaults and existing repo idioms (e.g., keep UI classes under `src/src/widget` or `dock`, parser code in `libmoex`).
- Keep dependencies minimal and rely on the vendorized third-party libraries already committed to the tree.
- Document new UI actions or build switches in `README.md`/`dev.md` whenever they affect contributors.

## Debugging
- Run MachOExplorer from Qt Creator with the debugger attached to inspect Qt widgets and signals.
- Load binaries from `sample/` for deterministic repros before experimenting with customer-provided files.
- Use the logging/printing facilities already in `libmoex` or temporary `fmt::print` statements (remove them before merging).

## Rules for Making Changes
- Keep pull requests small and focused (UI change, parser update, packaging tweak) and avoid mixing unrelated refactors.
- Verify at least one platform build before submission and ensure scripts (`build_macos.sh`, `build_windows.bat`) still run when relevant files change.
- Update documentation (README, dev.md, AGENT) whenever user workflows or requirements change.
- Do not remove vendorized third-party code without coordinating with maintainers.

## PR Checklist
- [ ] Builds successfully on the platform(s) impacted by the change (macOS and/or Windows).
- [ ] Manual testing performed with binaries from `sample/` or new repro assets, and results noted in the PR.
- [ ] Scripts/configuration files updated alongside code changes, and documentation reflects new behavior.
- [ ] No drive-by formatting or dependency churn beyond what the PR describes.
- [ ] Added assets/tests/samples are referenced from README or dev docs so others can find them.
