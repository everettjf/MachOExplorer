# MachOExplorer

> Explore Mach-O binaries with a modern Qt interface on macOS and Windows (yet another MachOView). `v1.0 Alpha` is the latest release and remains unstable while bugs are being fixed.

![MachOExplorer Icon](image/machoexplorer-small.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## Table of Contents
- [Features](#features)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Examples](#examples)
- [Development](#development)
- [Roadmap / TODO](#roadmap--todo)
- [Version History](#version-history)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)
- [Star History](#star-history)

## Features
- Inspect Mach-O files with a dedicated desktop UI built with Qt.
- Works on both macOS and Windows with identical workflows and visuals.
- Ships with native icons, dock widgets, and screenshot previews so you understand the layout quickly.

## Quick Start
1. Download the latest `MachOExplorer.dmg` (macOS) or `MachOExplorer_Windows.zip` (Windows) from the [releases page](https://github.com/everettjf/MachOExplorer/releases).
2. Install or unzip the application just like any other desktop app.
3. Launch MachOExplorer and open any Mach-O binary (try the included samples) to explore its structure immediately.

## Installation
### macOS
- Grab the packaged [`MachOExplorer.dmg`](https://github.com/everettjf/MachOExplorer/releases).
- Drag the app into `/Applications` or run it directly from the DMG.

### Windows
- Download [`MachOExplorer_Windows.zip`](https://github.com/everettjf/MachOExplorer/releases).
- Extract the archive and launch `MachOExplorer.exe`.

## Usage
- Launch the app, then open a Mach-O file via the file picker or by dragging and dropping it onto the window.
- Navigate headers, sections, and symbols using the dock widgets to inspect the full binary layout.
- Use the bundled [sample binaries](#examples) if you need something to experiment with or to verify functionality on a new install.

![MachOExplorer Screenshot](image/screenshot.png)

## Configuration
- **Packaged apps:** No runtime configuration is required beyond running the binary.
- **Building from source:**
  - macOS expects Qt SDK `>= 5.10.1` (scripts assume 5.11.2). Symlink your Qt install with `sudo ln -s ~/qt/5.11.2 /opt/qt` and install `appdmg` globally via `npm install -g appdmg`.
  - Windows builds need Qt 5.11.2 (MSVC 2017 64-bit), `nmake`, `windeployqt`, and Inno Setup (see `res/windows_setup.iss`). Keep Qt under `C:\Qt\Qt5.11.2\5.11.2` so scripts can find it.

## Examples
- The `sample/` directory contains minimal Mach-O files (`simple`, `complex`, and `simple.c`). Open them inside MachOExplorer to verify parsing, demo features, or aid debugging.

## Development
MachOExplorer is a Qt project (`src/MachOExplorer.pro`). Open it in Qt Creator or use the provided build scripts.

### macOS build
1. Install Qt 5.11.2 (or newer) and make sure it is accessible at `/opt/qt/5.11.2`.
2. Run `./build_macos.sh` to configure via `qmake`, build with `make`, and run `macdeployqt`.
3. The script repackages the DMG using `appdmg` and copies the artifact to `dist/MachOExplorer.dmg`.

### Windows build
1. Install Qt 5.11.2 for MSVC 2017 64-bit so `qmake.exe`, `nmake`, and `windeployqt.exe` are on the path, and install Inno Setup when packaging installers.
2. Run `build_windows.bat` from a Visual Studio Developer Command Prompt.
3. The script builds the release target, runs `windeployqt`, and copies the distributable into `dist/MachOExplorer_Windows/`.

### Dependencies
- Qt 5.11.x for the UI and platform integration.
- Vendorized third-party libraries inside `src/libmoex` such as [fmt](https://github.com/fmtlib/fmt), [cpp-mmaplib](https://github.com/yhirose/cpp-mmaplib), and [nlohmann/json](https://github.com/nlohmann/json).

## Roadmap / TODO
- Display symbol names for addresses.
- Improve Windows support (port is present but still unstable).

## Version History
- 2018-11-21 — `v1.0 Alpha`: Windows support and new user interface.
- 2017-11-05 — `v0.4.0 Alpha`: Command line tool `moex` release and version policy change.
- 2017-11-05 — `v0.3 Alpha`: Bug fixes.
- 2017-11-05 — `v0.2 Alpha`: Icon created.
- 2017-11-04 — `v0.1 Alpha`: First release.

## Contributing
Contributions are welcome! Please open an issue or pull request if you spot bugs, want to improve the UI, or can help with feature parity across platforms.

## License
Distributed under the [MIT License](LICENSE).

## Acknowledgements
- Icon designed by [wantline](https://weibo.com/wantline).
- Special thanks to everyone who provided feedback on early alpha builds.

![MachOExplorer Icon](image/machoexplorer-small.png)

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=everettjf/MachOExplorer&type=Date)](https://star-history.com/#everettjf/MachOExplorer&Date)
