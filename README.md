# MachOExplorer

Explore Mach-O binaries with a modern Qt interface on macOS and Windows (yet another MachOView).

![MachOExplorerIcon](image/machoexplorer-small.png)

![MachOExplorer Screenshot](image/screenshot.png)

`v1.0 Alpha` is the latest release (still unstable and likely to contain bugs).

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

- Inspect Mach-O files through a dedicated desktop UI.
- Works on both macOS and Windows (Qt-based cross-platform UI).
- Ships with a native icon set and screenshot previews to understand layout quickly.

## Quick Start

1. Download the latest `MachOExplorer.dmg` (macOS) or `MachOExplorer_Windows.zip` (Windows) from the [releases page](https://github.com/everettjf/MachOExplorer/releases).
2. Install or unzip the application.
3. Launch MachOExplorer and open any Mach-O binary to explore its structure.

## Installation

- **macOS:** Install via the packaged [`MachOExplorer.dmg`](https://github.com/everettjf/MachOExplorer/releases). Drag the app into `/Applications` (or run from the DMG) just like any other macOS app.
- **Windows:** Download [`MachOExplorer_Windows.zip`](https://github.com/everettjf/MachOExplorer/releases), extract its contents, and run `MachOExplorer.exe`.

## Usage

- Launch the app and use the file picker (or drag & drop) to load a Mach-O file.
- Navigate through headers, sections, and symbols using the provided dock widgets.
- Use the included **sample binaries** (see [Examples](#examples)) to experiment if you do not have your own files handy.

![MachOExplorer UI](image/screenshot.png)

## Configuration

- No runtime configuration is required for the packaged binaries.
- Building from source requires Qt SDK `>= 5.10.1` (the project scripts assume Qt 5.11.2). On macOS the development workflow symlinks Qt into `/opt/qt` (`sudo ln -s ~/qt/5.11.2 /opt/qt`) and installs `appdmg` globally (`npm install -g appdmg`). On Windows you need Inno Setup plus a Qt installation under `C:\Qt\Qt5.11.2\5.11.2`.

## Examples

- The `sample/` directory contains minimal Mach-O files (`simple`, `complex`, and `simple.c`) that can be opened in MachOExplorer to verify functionality or for debugging demonstrations.

## Development

MachOExplorer is a Qt project. You can open `src/MachOExplorer.pro` directly in Qt Creator or use the provided scripts:

- **macOS build:**
  1. Ensure Qt 5.11.2 (or newer) is installed and available under `/opt/qt/5.11.2`.
  2. Run `./build_macos.sh` to configure with `qmake`, compile via `make`, and package a DMG using `macdeployqt` and `appdmg`.
  3. Resulting artifacts are copied into `dist/MachOExplorer.dmg`.
- **Windows build:**
  1. Install Qt 5.11.2 for MSVC 2017 64-bit and ensure `qmake.exe`, `nmake`, and `windeployqt.exe` are available.
  2. Run `build_windows.bat` to build, bundle dependencies with `windeployqt`, and copy the distributable to `dist/MachOExplorer_Windows`.
- **Dependencies:** Qt, [fmt](https://github.com/fmtlib/fmt), [cpp-mmaplib](https://github.com/yhirose/cpp-mmaplib), and [nlohmann/json](https://github.com/nlohmann/json) are vendorized within the repo.

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
