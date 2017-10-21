# MachOExplorer

Explore MachO File Format (Yet another MachOView)

⚠️ This repo is under development, features are in-complete, code may not compile

⚠️ This repo is under development, features are in-complete, code may not compile

⚠️ This repo is under development, features are in-complete, code may not compile


# Feature

- [ ] Basic view MachO file structure (like MachOView).

# Future feature maybe

- [ ] Basic edit.
- [ ] Insert a dylib (like optool).
- [ ] Class dump (like class-dump).
- [ ] Symbolicate static initializers : just drop dSYM into `mod_func_init` section.
- [ ] Restore Symbol : restore symbol include blocks.


# How to build 

macOS only until now, Windows support in the near future.

1. Env

```
brew install cmake
brew install boost --c++11

Qt SDK >= 5.9.1
```

2. moex-cli

- CMake (>=3.9)

3. moex-gui

- CMake (>=3.9)

