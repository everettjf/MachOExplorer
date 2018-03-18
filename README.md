# MachOExplorer

Explore MachO File (Yet another MachOView)

![MachOExplorerIcon](image/machoexplorer-small.png)

*Icon is designed by [wantline](https://weibo.com/wantline)*

![MachOExplorer](image/screenshot.png)

# Latest Version

`v0.4.0 Alpha` (Not a stable version and there must be lots of bugs now)

# How to install


(1) [Download MachOExplorer.dmg](https://github.com/everettjf/MachOExplorer/releases)

(2) or use brew cask

```
brew cask install machoexplorer
```

# Feature

1. Basic view macho file
2. macOS support

# Incomplete code

1. Symbol display for address
2. Windows support in the future

# How to build 

macOS only until now, Windows support in the near future.

```
Qt SDK >= 5.10.1
CMake (>=3.9)
```

# Libraries

1. Qt
2. fmt : Formating https://github.com/fmtlib/fmt
3. cpp-mmaplib : mmap/CreateFileMapping Wrapper https://github.com/yhirose/cpp-mmaplib
4. json : https://github.com/nlohmann/json

# Icon

![MachOExplorer](image/machoexplorer-small.png)

*Icon is designed by [wantline](https://weibo.com/wantline)*

Thanks for wantline's great design.

# Version History

- 2017-11-05 v0.4.0 Alpha : Command line tool `moex` release and version policy changed
- 2017-11-05 v0.3 Alpha : Bug fix
- 2017-11-05 v0.2 Alpha : Icon born
- 2017-11-04 v0.1 Alpha : First release
