# Crash Regression Set

This directory contains parser crash-regression inputs and a runner script.

## Run

Build parser CLI first:

```bash
cmake -S src -B build -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
```

Run regression:

```bash
tests/regression/run_regression.sh
```

Run all regression checks:

```bash
tests/regression/run_all.sh
```

Run app CLI smoke test:

```bash
tests/regression/run_cli_smoke.sh
```

Run dyld cache tools smoke test:

```bash
tests/regression/run_cache_tools_smoke.sh
```

Run malformed-input crash regression:

```bash
tests/regression/run_crash_regression.sh
```

Optional cache path:

```bash
tests/regression/run_cache_tools_smoke.sh /path/to/dyld_shared_cache_arm64e
```

## Add New Cases

Put crashing/minimized samples in `tests/regression/cases/`.

Current repository ships seed cases from `sample/` via `cases.txt`.
