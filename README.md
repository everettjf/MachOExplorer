# MachOExplorer

> A modern Mach-O explorer with Qt UI for macOS and Windows. Current major release: `v2.0.0`.

![MachOExplorer Icon](image/machoexplorer-small.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## Highlights
- Native desktop UI with layout tree, table inspector, hex view, log panel, and info panel.
- Robust Mach-O parsing with load-command boundary validation for malformed binaries.
- Expanded load-command coverage (including modern commands).
- Theme switching: `Follow System`, `Light`, `Dark`.
- Cross-platform project files: CMake and Qt `.pro`.

## Load Command Coverage (Selected)
MachOExplorer now supports display/parsing for a broad set of commands, including:
- `LC_SEGMENT`, `LC_SEGMENT_64`
- `LC_SYMTAB`, `LC_DYSYMTAB`, `LC_TWOLEVEL_HINTS`
- `LC_LOAD_DYLIB`, `LC_ID_DYLIB`, `LC_LOAD_WEAK_DYLIB`, `LC_REEXPORT_DYLIB`, `LC_LAZY_LOAD_DYLIB`, `LC_LOAD_UPWARD_DYLIB`
- `LC_LOAD_DYLINKER`, `LC_ID_DYLINKER`, `LC_DYLD_ENVIRONMENT`
- `LC_DYLD_INFO`, `LC_DYLD_INFO_ONLY`
- `LC_RPATH`, `LC_MAIN`, `LC_UUID`, `LC_SOURCE_VERSION`
- `LC_VERSION_MIN_*`, `LC_BUILD_VERSION`
- `LC_CODE_SIGNATURE`, `LC_SEGMENT_SPLIT_INFO`, `LC_FUNCTION_STARTS`, `LC_DATA_IN_CODE`, `LC_DYLIB_CODE_SIGN_DRS`, `LC_LINKER_OPTIMIZATION_HINT`
- `LC_NOTE`, `LC_LINKER_OPTION`, `LC_DYLD_EXPORTS_TRIE`, `LC_DYLD_CHAINED_FIXUPS`, `LC_FILESET_ENTRY`

## Screenshot
![MachOExplorer Screenshot](image/screenshot.png)

## Quick Start
1. Download release artifacts from [GitHub Releases](https://github.com/everettjf/MachOExplorer/releases).
2. Launch the app.
3. Open a Mach-O file (or drag and drop one) to inspect headers, commands, sections, symbols, and raw bytes.

## Build From Source

### Prerequisites
- CMake `>= 3.16`
- Qt 6 (recommended) or Qt 5 with Widgets/Network modules
- A C++14 compiler

### macOS / Linux (CMake)
```bash
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/macos"
cmake --build build -j8
./build/MachOExplorer
```

Example Qt path on this repo's recent setup:
```bash
-DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
```

### Windows (CMake)
```powershell
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvcXXXX_64"
cmake --build build --config Release
```

### Qt Creator / qmake (legacy workflow)
- Open `src/MachOExplorer.pro` in Qt Creator.
- Build and run from IDE.

## Included Samples
`sample/` includes test files:
- `sample/simple`
- `sample/complex`
- `sample/simple.c`

## Project Structure
- `src/libmoex/`: Mach-O parsing + view-node data model
- `src/src/`: Qt UI, controllers, dialogs, widgets
- `sample/`: sample binaries
- `image/`: screenshots and icons used in README

## Roadmap
- Better symbol-to-address correlation in more views.
- Continue widening modern Mach-O coverage and deep field decoding.
- Improve Windows packaging and test matrix.

## Version History
- 2026-03-07 — `v2.0.0`: major renovation release (parser hardening, modern load commands, Qt6-friendly build, theme switching).
- 2018-11-21 — `v1.0 Alpha`: Windows support and redesigned UI.
- 2017-11-05 — `v0.4.0 Alpha`: command line tool `moex` release.

## Contributing
PRs and issues are welcome:
- Project: https://github.com/everettjf/MachOExplorer
- Issues: https://github.com/everettjf/MachOExplorer/issues

## Acknowledgements
- Original inspiration: MachOView
- Icon designed by [wantline](https://weibo.com/wantline)

## License
MIT. See [LICENSE](LICENSE).

## Star History
[![Star History Chart](https://api.star-history.com/svg?repos=everettjf/MachOExplorer&type=Date)](https://star-history.com/#everettjf/MachOExplorer&Date)
