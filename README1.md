# MOEX - MachO EXplorer

- MOEX means explore MachO file format.
- Yet another MachOView, and maybe a better one.

# Feature

- Basic view MachO file structure (like MachOView).
- Insert a dylib (like optool).
- Class dump (like class-dump).


# Future

- Symbolicate static initializers : just drop dSYM into `mod_func_init` section.
- Restore Symbol : one key to restore symbol.
- Compare MachO : compare two macho files.


# How to build

```
brew install cmake
brew install boost --c++11
```

