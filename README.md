# MachOExplorer

> A focused desktop explorer for Mach-O binaries, Fat/Universal binaries, `.a` archives, and the dyld shared cache — built for people who reverse engineer and inspect Apple binaries.

Language: **English** | [简体中文](README.zh-CN.md)

![MachOExplorer Screenshot](image/screenshot.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Linux CI](https://github.com/everettjf/MachOExplorer/actions/workflows/linux.yml/badge.svg)](https://github.com/everettjf/MachOExplorer/actions/workflows/linux.yml)
[![Latest Release](https://img.shields.io/github/v/release/everettjf/MachOExplorer)](https://github.com/everettjf/MachOExplorer/releases/latest)
[![Platforms](https://img.shields.io/badge/platforms-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey)](#install)

MachOExplorer turns an opaque Mach-O file into a navigable tree. Open a binary and drill from the
file structure all the way down to load commands, sections, symbols, disassembly, and Apple-specific
metadata (Objective-C, Swift, code signing) — in one window, or fully scripted from the command line.

## Contents
- [Features](#features)
- [Supported formats](#supported-formats)
- [Install](#install)
- [Quick start](#quick-start)
- [CLI / automation mode](#cli--automation-mode)
- [Build from source](#build-from-source)
- [Docs](#docs)
- [Contributing](#contributing)
- [License](#license)

## Features

### Structure & navigation
- Layout tree: file → header → load commands → sections / symbols / link-edit data.
- Search box that filters the layout tree by node name **and** matches the contents of tables you have already opened.
- Hex view stays in sync with the selected node — jump from any table row to the exact bytes.

### Apple binary internals
- Load commands, sections, symbol & string tables, relocations, indirect symbols, function starts, data-in-code.
- **Code signing**: parses the embedded-signature SuperBlob (Code Directory, Requirements, CMS) and decodes the **entitlements** plist.
- **Objective-C** metadata: classes, methods, ivars, properties, and protocols.
- **Swift** semantic graph over the `__swift5_*` sections.
- **dyld info**: rebase / bind / lazy-bind / exports trie and chained-fixups structures.

### Code & cross-references
- Optional [Capstone](https://www.capstone-engine.org/) disassembly of `__TEXT,__text` (arm64, x86_64, arm, i386).
- Xref report that resolves call / jump targets and ties them back to symbols.

### dyld shared cache
- List images, extract a single image or all of them, and drill straight into an extracted image.

### Productivity & UX
- Parsing runs **off the UI thread**, so opening a large binary or a multi-gigabyte shared cache keeps the window responsive.
- Tables: per-column filter, copy row(s), CSV export, keyboard navigation.
- Built-in update check with reminder options.

### Automation
- Headless `--cli` mode prints the full analysis as text or **stable JSON** — ideal for scripting, diffing, and CI.

### Built for suspicious files
The parser is bounds- and alignment-checked throughout — LEB128 streams, load-command walking,
string tables, and code-signature blobs — and is covered by a malformed-input crash-regression
suite (run against truncated and fuzzed binaries). Opening a hostile or corrupted file degrades
gracefully instead of crashing.

## Supported formats
| Format | Notes |
| --- | --- |
| Mach-O | 32-bit and 64-bit |
| Fat / Universal | Multiple architecture slices |
| Static archive | `.a` with Mach-O members |
| dyld shared cache | List, extract, and inspect images |

## Install

### macOS
- Download the latest release: <https://github.com/everettjf/MachOExplorer/releases/latest>
- Or install via Homebrew:

```bash
brew update && brew tap everettjf/homebrew-tap && brew install --cask machoexplorer
```

### Linux / Windows
Prebuilt macOS releases are published on GitHub. On Linux and Windows, build from source
(see [Build from source](#build-from-source)). The Linux build and the full regression suite
run in CI on every change.

## Quick start
1. Launch MachOExplorer, or open a file directly: `MachOExplorer <path>`.
2. Open a bundled sample (`sample/simple`, `sample/complex`) or your own binary.
3. Walk the **Layout** tree; type in the search box to jump to a section, symbol, or value; select a
   node to see its table and the matching bytes in the hex view.

## CLI / automation mode
Run a full analysis without the GUI and write the result to stdout or a file:

```bash
./build/MachOExplorer --cli sample/simple
./build/MachOExplorer --cli --format json --output /tmp/simple-analysis.json sample/simple
```

Common options:

| Option | Description |
| --- | --- |
| `--format text\|json` | Output format (default `text`) |
| `--output <path>` | Write analysis output to a file |
| `--max-rows <N>` | Limit rows per table (`0` = unlimited) |
| `--max-depth <N>` | Limit tree depth (`0` = unlimited) |
| `--root-path <path>` | Export only one matching analysis subtree |
| `--name-filter <text>` | Keep nodes whose display names match the text |
| `--table-mode <full\|headers\|summary>` | Control table payload size |
| `--include-empty` | Include empty nodes |

The JSON output is automation-friendly and stable: it includes `schemaVersion`, a `summary`, node
`path` / `kind` / `childIndex`, and row `cells` / `rowIndex`. String values are sanitized to valid
UTF-8 so output stays well-formed even on malformed input. See [CLI.md](CLI.md) for the full guide.

## Build from source
Requirements: CMake ≥ 3.16, Qt 6 (Core, Gui, Widgets, Network, Concurrent), a C++14 compiler, and
optionally [Capstone](https://www.capstone-engine.org/) for disassembly.

```bash
# macOS
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$HOME/Qt/6.x.x/macos"
cmake --build build -j8

# Linux (Debian/Ubuntu deps: qt6-base-dev libgl1-mesa-dev libcapstone-dev)
./build_linux.sh        # or: cmake -S src -B build && cmake --build build -j"$(nproc)"

# Windows
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvcXXXX_64"
cmake --build build --config Release
```

Run `./build/MachOExplorer` (or `MachOExplorer.app` on macOS). See [DEVELOP.md](DEVELOP.md) for the
full developer, testing, and release workflow.

## Docs
- Developer guide (build / test / release): [DEVELOP.md](DEVELOP.md)
- CLI guide: [CLI.md](CLI.md)
- Packaging notes: [docs/release_packaging.md](docs/release_packaging.md)

## Contributing
Issues and pull requests are welcome:
- Project: <https://github.com/everettjf/MachOExplorer>
- Issues: <https://github.com/everettjf/MachOExplorer/issues>

Before opening a PR, please build the project and run `tests/regression/run_all.sh`.

## License
MIT. See [LICENSE](LICENSE).

## Star History
[![Star History Chart](https://api.star-history.com/svg?repos=everettjf/MachOExplorer)](https://star-history.com/#everettjf/MachOExplorer)
