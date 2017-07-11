# MOEX - MachO EXplorer

**Under Developing**
**Under Developing**
**Under Developing**

- Yet another MachOView, and trying to be a better one.

# Feature

- Basic view MachO file structure (like MachOView).
- Insert a dylib (like optool).
- Class dump (like class-dump).
- Symbolicate static initializers : just drop dSYM into `mod_func_init` section.
- Restore Symbol : one key to restore symbol.


# How to build (macOS only until now)

1. Env

```
brew install cmake
brew install boost --c++11
```

2. moex-cli

- vscode with cmake-tools extension
- Clion

3. moex-gui

- Qt Creator with Qt 5.9.1

