# MachOExplorer CLI Guide

This document covers the command-line capabilities in MachOExplorer, including the new no-GUI analysis mode and existing helper tools.

## Overview
MachOExplorer now supports two CLI usage styles:

1. `MachOExplorer --cli ...`
Run full analysis without starting the GUI. This reuses the same internal analysis tree (`ViewNode`) as the desktop app.

2. Dedicated helper tools (`moex-*`)
Focused commands for parser regression, diff, dyld cache listing, and dyld cache extraction.

## 1) No-GUI Full Analysis Mode

### Basic usage

```bash
./build/MachOExplorer --cli [options] <input-file>
```

### Help

```bash
./build/MachOExplorer --cli --help
```

### Options

- `--cli`
  Enable command-line mode and skip UI startup.
- `--format text|json`
  Output format. Default: `text`.
- `--output <path>`
  Write output to file (instead of stdout).
- `--max-rows <N>`
  Limit rows printed per table. `0` means unlimited.
- `--max-depth <N>`
  Limit analysis tree depth. `0` means unlimited.
- `--include-empty`
  Include nodes with no table/binary payload.
- `-h`, `--help`
  Show CLI help.

### Output behavior

- `stdout` is used by default.
- `stderr` is used for errors and invalid arguments.
- JSON format is pretty-printed for readability.

### Exit codes

- `0`: success
- `1`: analysis/runtime failure (parse failure, output file open failure, etc.)
- `2`: invalid CLI arguments

### What is included in full analysis output

The `--cli` analysis mode traverses the full view tree and includes all currently implemented analysis nodes, for example:

- File kind summary (Mach-O / Fat / Archive / dyld shared cache)
- Mach header and load command views
- Section and section-derived views
- Symbol/string-related views
- Dynamic loader info views (including modern dyld exports/fixups views when present)
- Swift/ObjC related views already exposed by current `ViewNode` graph
- Dyld shared cache top-level views (mappings/images)

Notes:

- Coverage is comprehensive with respect to the current `ViewNode` architecture (same backbone as GUI analysis tree).
- UI-only interactions (sorting/filtering/copy operations) are not exported as behaviors; the underlying analyzed data is exported.

### Examples

#### Human-readable output to terminal

```bash
./build/MachOExplorer --cli sample/simple
```

#### JSON output to terminal

```bash
./build/MachOExplorer --cli --format json sample/simple
```

#### JSON output to file

```bash
./build/MachOExplorer --cli --format json --output /tmp/simple-analysis.json sample/simple
```

#### Large file with output limits

```bash
./build/MachOExplorer --cli --max-rows 100 --max-depth 4 sample/complex
```

#### Include empty nodes for schema-like traversal

```bash
./build/MachOExplorer --cli --format json --include-empty sample/simple
```

#### Pipe to jq

```bash
./build/MachOExplorer --cli --format json sample/simple | jq '.analysis.name'
```

## 2) Dedicated CLI Tools (`moex-*`)

These tools are also built in `build/`.

## `moex-parse`

```bash
./build/moex-parse <file> [file...]
```

Purpose:

- Quick parser validity check
- Used by regression scripts

## `moex-diff`

```bash
./build/moex-diff <fileA> <fileB>
```

Purpose:

- Compare first Mach-O header attributes
- Compare section/symbol set differences

## `moex-cache-list`

```bash
./build/moex-cache-list [--json] [--exact] [--limit=N] [--output=file] <dyld_shared_cache_file> [path-filter]
```

Purpose:

- List dyld shared cache images
- Optional exact/substring filtering

## `moex-cache-extract`

```bash
./build/moex-cache-extract [--compact] [--all] [--exact] [--dry-run] [--json] [--max=N] <dyld_shared_cache_file> <image-path-or-substr> <output-macho-or-dir>
```

Purpose:

- Extract one or multiple images from dyld shared cache
- Optional compact rewrite of segment file offsets

## Automation examples

### CI smoke check for CLI mode

```bash
tests/regression/run_cli_smoke.sh
```

### Full regression

```bash
tests/regression/run_all.sh
```

### Batch analyze samples

```bash
for f in sample/simple sample/complex; do
  ./build/MachOExplorer --cli --format json --output "/tmp/$(basename "$f").json" "$f"
done
```

## Build reminder

```bash
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
```

## Troubleshooting

- Error: `expected exactly one input file`
  Cause: `--cli` mode requires one positional input path.
- Error: `unsupported format: ...`
  Cause: only `text` and `json` are supported.
- Error: `analysis failed: ...`
  Cause: parse failed or file is malformed/unsupported for given operation.
- Error: `cannot open output file`
  Cause: invalid path or missing write permission.
