# MachOExplorer Agent Guide

This guide summarizes everything an autonomous contributor needs to know before editing the repository.

## Project Overview

MachOExplorer is a Qt-based desktop application (macOS and Windows) for inspecting Mach-O binaries. The codebase contains a GUI written in C++/Qt, bundles third-party libraries (fmt, cpp-mmaplib, nlohmann/json), and includes packaging scripts for DMG/ZIP releases. The latest public release is `v1.0 Alpha`, and the project is still considered unstable.

## Repository Map

- `src/` — Main Qt project (`MachOExplorer.pro`, Qt resources, icon, and the `libmoex` sources).
- `src/src/` — Application code (widgets, controllers, utility modules, CLI integration, and a lightweight `test/` directory).
- `res/` — Packaging assets such as `appdmg.json` and `background.png` used when producing macOS installers.
- `image/` — Screenshots and icons referenced by the README.
- `sample/` — Sample Mach-O binaries (and `simple.c`) for manual testing.
- `build_macos.sh` / `build_windows.bat` — Helper scripts that compile and package the application on their respective platforms.
- `dev.md` — Notes about per-platform dependencies (Qt locations, `appdmg`, Inno Setup).

## Run Locally

1. Install Qt SDK 5.11.2 (or newer) for your platform.
2. macOS:
   - Symlink your Qt install to `/opt/qt/5.11.2` as hinted in `dev.md`.
   - From the repo root run `./build_macos.sh` **or** open `src/MachOExplorer.pro` in Qt Creator and build/run the app.
3. Windows:
   - Install Qt 5.11.2 (MSVC 2017 64-bit) and ensure `qmake`, `nmake`, and `windeployqt` are in your PATH.
   - Run `build_windows.bat` **or** build from Qt Creator using `MachOExplorer.pro`.

Launching the compiled `MachOExplorer` binary opens the GUI; load Mach-O files using the menu or drag-and-drop.

## Testing

- There is no automated test suite wired into CTest or another runner. Manual testing consists of opening binaries from `sample/` (e.g., `sample/simple`) and verifying key panes (headers, sections, symbols) load correctly.
- If you change parsing logic, consider creating additional sample inputs in the `sample/` folder to reproduce the scenario.

## Linting / Formatting

- No formal lint/format tooling is enforced. Follow Qt/C++ best practices and keep formatting consistent with the existing files.
- When in doubt, use `clang-format` with a conservative LLVM or Qt Creator default profile, but do not introduce sweeping format-only diffs.

## Build and Release

- **macOS:** `build_macos.sh` runs `qmake`, `make`, and `macdeployqt`, then repackages the DMG with `appdmg`. It copies the final `MachOExplorer.dmg` into `dist/`.
- **Windows:** `build_windows.bat` builds the project, runs `windeployqt`, and stages the portable bundle under `dist/MachOExplorer_Windows`.
- Release artifacts (`MachOExplorer.dmg` and `MachOExplorer_Windows.zip`) are uploaded manually to GitHub Releases.

## Coding Style & Conventions

- Default recommendation: follow Qt/C++ idioms already present in the repo. Maintain the header/source split, respect existing namespaces, and avoid introducing new dependencies without discussion.

## Debugging Tips

- Use Qt Creator’s debugger (attach to the `MachOExplorer` run configuration) to inspect widget state.
- Load binaries from `sample/` for deterministic repros.
- The `src/src/test` folder contains simple harness files you can extend when validating parsing utilities.

## Rules for Making Changes

- Keep pull requests focused (UI change, parser fix, packaging tweak, etc.).
- Update documentation when behavior or build steps change.
- Avoid sweeping refactors or dependency upgrades unless coordinated.
- Ensure scripts (`build_macos.sh`, `build_windows.bat`) still run after your changes.

## PR Checklist

- [ ] Code compiles on at least one target platform (macOS or Windows).
- [ ] Manual testing performed with binaries from `sample/`.
- [ ] Documentation (README, dev.md, changelog if applicable) updated as needed.
- [ ] No unrelated formatting-only changes included.
- [ ] New assets or samples are referenced from the README if they affect users.
