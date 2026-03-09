# MachOExplorer

> 一个使用 Qt 开发的 Mach-O 与 Archive (`.a`) 结构分析工具。
>
> 语言：简体中文 | [English](README.md)

![MachOExplorer Icon](image/machoexplorer-small.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## 安装

### macOS
- Release 下载：<https://github.com/everettjf/MachOExplorer/releases/latest>
- Homebrew 安装：
```bash
brew update && brew tap everettjf/homebrew-tap && brew install --cask machoexplorer
```

### Windows
- 推荐使用 Inno Setup 生成“下一步安装”安装包。
- 仓库已提供：
  - `packaging/windows/MachOExplorer.iss`
  - `scripts/build_windows_installer.ps1`
- 详细步骤见：`docs/release_packaging.md`

## 功能概览
- 支持解析 Mach-O、Fat Mach-O、`.a` Archive、dyld shared cache。
- 支持查看 load commands、sections、symbols、relocations、dyld 信息。
- 可选 Capstone 反汇编 `__TEXT,__text`，并生成更深层 xref/callgraph 关联。
- 支持 ObjC2 / Swift 元数据浏览（含 `__swift5_*` 语义关系）。
- 支持 dyld shared cache 镜像提取并直接打开分析。
- 支持深浅色主题、表格筛选/导出/快捷键复制。

## 更新检查
- 菜单：`Help -> Check for Updates`
- 启动后自动检查新版本。
- 发现更新后支持：
  - `Download`
  - `Remind in 7 days`
  - `Remind in 30 days`
  - `Later`
- 下载会引导到 GitHub Releases 页面。

## 构建

### 依赖
- CMake `>= 3.16`
- Qt 6（推荐）或 Qt 5（`Core/Gui/Widgets/Network`）
- C++14 编译器

### macOS
```bash
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
./build/MachOExplorer
```

### Windows
```powershell
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvcXXXX_64"
cmake --build build --config Release
```

## 回归测试
```bash
tests/regression/run_all.sh
```

## 相关链接
- 项目主页：<https://github.com/everettjf/MachOExplorer>
- Issues：<https://github.com/everettjf/MachOExplorer/issues>

## 许可证
MIT，详见 [LICENSE](LICENSE)。
