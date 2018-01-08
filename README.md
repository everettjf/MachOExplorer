# MachOExplorer

Explore MachO File (Yet another MachOView)

![MachOExplorerIcon](image/machoexplorer-small.png)

*Icon is designed by [wantline](https://weibo.com/wantline)*

![MachOExplorer](image/screenshot.png)

# Latest Version

`v0.4.0 Alpha` (Not a stable version and there must be lots of bugs now)

# How to install

1. GUI `MachOExplorer`

(1) [Download MachOExplorer.dmg](https://github.com/everettjf/MachOExplorer/releases)

(2) or use brew cask

```
brew cask install machoexplorer
```

2. Command Line Tool `moex`

```
brew tap everettjf/tap
brew install moex
moex --help
```

# Feature

1. Basic view macho file
2. macOS support

# Incomplete code

1. Symbol display for address
2. Windows support in the future

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



# Libraries

1. Qt
2. boost (Trying to remove)
3. args : Argument Parser https://github.com/Taywee/args
4. fmt : Formating https://github.com/fmtlib/fmt
5. cpp-mmaplib : mmap/CreateFileMapping Wrapper https://github.com/yhirose/cpp-mmaplib

# Icon

![MachOExplorer](image/machoexplorer-small.png)

*Icon is designed by [wantline](https://weibo.com/wantline)*

Thanks for wantline's great design.

# Version History

- 2017-11-05 v0.4.0 Alpha : Command line tool `moex` release and version policy changed
- 2017-11-05 v0.3 Alpha : Bug fix
- 2017-11-05 v0.2 Alpha : Icon born
- 2017-11-04 v0.1 Alpha : First release
