# MachOExplorer

Yet another MachOView, and trying to be a better one.

⚠️ This repo is under development, features are in-complete, code may not compile

⚠️ This repo is under development, features are in-complete, code may not compile

⚠️ This repo is under development, features are in-complete, code may not compile


# Feature

- [X] Basic view MachO file structure (like MachOView).
- [ ] Basic edit.


# Future maybe

- [ ] Insert a dylib (like optool).
- [ ] Class dump (like class-dump).
- [ ] Symbolicate static initializers : just drop dSYM into `mod_func_init` section.
- [ ] Restore Symbol : one key to restore symbol (include blocks).


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

