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

## Add New Cases

Put crashing/minimized samples in `tests/regression/cases/`.

Current repository ships seed cases from `sample/` via `cases.txt`.
