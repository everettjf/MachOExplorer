# MachOExplorer

A focused desktop explorer for Mach-O binaries, archives, and dyld shared cache.

Language: **English** | [简体中文](README.zh-CN.md)

![MachOExplorer Screenshot](image/screenshot.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## Why MachOExplorer
- Open and inspect Mach-O / Fat Mach-O / `.a` archive / dyld shared cache in one UI.
- Drill from structure to code quickly: sections, symbols, disassembly, xref, call graph hints.
- Understand modern Apple binaries better: ObjC metadata, Swift metadata, dyld exports/fixups.
- Work safely on suspicious files with hardened parser checks and regression coverage.

## Highlights
- Rich binary views: load commands, sections, symbols, relocations, strings.
- Optional Capstone disassembly for `__TEXT,__text`.
- Swift semantic graph (`__swift5_*`) and ObjC metadata browsing.
- Dyld shared cache image listing/extraction and direct drill-in workflow.
- Productive table UX: filter, copy row(s), CSV export, keyboard navigation.
- Built-in update check with reminder options and release-page guidance.

## Install (macOS)
- Latest release: <https://github.com/everettjf/MachOExplorer/releases/latest>
- Homebrew:

```bash
brew update && brew tap everettjf/homebrew-tap && brew install --cask machoexplorer
```

## Quick Start
1. Launch MachOExplorer.
2. Open a sample binary (`sample/simple` or `sample/complex`) or your own Mach-O file.
3. Navigate from `Layout` tree to sections/symbols/disassembly views.

## CLI Analysis Mode (No GUI)
Run full analysis in command-line mode and output to stdout or file:

```bash
./build/MachOExplorer --cli sample/simple
./build/MachOExplorer --cli --format json --output /tmp/simple-analysis.json sample/simple
```

Common options:
- `--format text|json` output format (default `text`)
- `--output <path>` write analysis output to file
- `--max-rows <N>` limit rows per table (`0` means unlimited)
- `--max-depth <N>` limit tree depth (`0` means unlimited)
- `--include-empty` include empty nodes

## Docs
- Developer guide (build/test/release): [DEVELOP.md](DEVELOP.md)
- Packaging notes: [docs/release_packaging.md](docs/release_packaging.md)
- CLI guide: [CLI.md](CLI.md)

## Contributing
Issues and PRs are welcome:
- Project: <https://github.com/everettjf/MachOExplorer>
- Issues: <https://github.com/everettjf/MachOExplorer/issues>

## License
MIT. See [LICENSE](LICENSE).
