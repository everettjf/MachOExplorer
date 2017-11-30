# MachOExplorer

Explore MachO File (Yet another MachOView)

![MachOExplorerIcon](image/machoexplorer-small.png)

*Icon is designed by [wantline](https://weibo.com/wantline)*

![MachOExplorer](image/screenshot.png)

# Latest Version

`v0.4.0 Alpha` (Not a stable version and there must be lots of bugs now)

# How to install

1. MachOExplorer

(1) [Download MachOExplorer.dmg](https://github.com/everettjf/MachOExplorer/releases)下载最新版本

(2) or use brew cask

```
brew cask install machoexplorer
```

2. Command line tool `moex`

```
brew tap everettjf/tap
brew install moex
moex --help
```

# Group

1. [Telegram Group](https://t.me/joinchat/FBhGpBHmk0r0R73bxYmNdA)
2. QQ Group 259141165

# Feature

1. Basic view macho file
2. macOS support

# Incomplete code

1. Symbol display for address
2. Windows support in the future

# How to build 

这里是从源码编译的步骤，如果仅安装使用，[点击这里下载](https://github.com/everettjf/MachOExplorer/releases)

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

# Icon

![MachOExplorer](image/machoexplorer-small.png)

*Icon is designed by [wantline](https://weibo.com/wantline)*

Thanks for wantline's great design.

# Version History

- 2017-11-05 v0.4.0 Alpha : Command line tool `moex` release and version policy changed
- 2017-11-05 v0.3 Alpha : Bug fix
- 2017-11-05 v0.2 Alpha : Icon born
- 2017-11-04 v0.1 Alpha : First release
