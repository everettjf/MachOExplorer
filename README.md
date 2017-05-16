# MOEX - MachO EXplorer

**Still under developing for the first release**

- MOEX means explore MachO file format.
- Yet another MachOView, and maybe a better one.

# Feature

- Platform support : macOS,Windows,Linux
- Basic view MachO file structure (like MachOView).
- Insert a dylib (like optool).
- Class dump (like class-dump).


# Future

- Symbolicate static initializers : just drop dSYM into `mod_func_init` section.
- Restore Symbol : one key to restore symbol.
- Compare MachO : compare two macho files.


# How to build

## macOS

```
brew install boost
qmake
```

## Windows

```
// todo
```


