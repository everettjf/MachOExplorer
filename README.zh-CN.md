# MachOExplorer

> 一个聚焦 Apple 二进制的桌面分析工具——支持 Mach-O、Fat/Universal、`.a` Archive 与 dyld shared cache，为逆向与二进制审查而生。

语言：简体中文 | [English](README.md)

![MachOExplorer 截图](image/screenshot.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Linux CI](https://github.com/everettjf/MachOExplorer/actions/workflows/linux.yml/badge.svg)](https://github.com/everettjf/MachOExplorer/actions/workflows/linux.yml)
[![Latest Release](https://img.shields.io/github/v/release/everettjf/MachOExplorer)](https://github.com/everettjf/MachOExplorer/releases/latest)
[![Platforms](https://img.shields.io/badge/platforms-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey)](#安装)

MachOExplorer 把一个晦涩的 Mach-O 文件变成可导航的树。打开一个二进制，从文件结构一路钻取到
load command、section、符号、反汇编，以及 Apple 特有的元数据（Objective-C、Swift、代码签名）——
在一个窗口里完成，也可以完全用命令行脚本化。

## 目录
- [功能](#功能)
- [支持的格式](#支持的格式)
- [安装](#安装)
- [快速开始](#快速开始)
- [CLI / 自动化模式](#cli--自动化模式)
- [从源码构建](#从源码构建)
- [文档](#文档)
- [贡献](#贡献)
- [许可证](#许可证)

## 功能

### 结构与导航
- Layout 树：文件 → header → load commands → section / 符号 / link-edit 数据。
- 搜索框：按节点名过滤布局树，**并且**能匹配你已打开过的表格里的内容。
- Hex 视图与所选节点联动——从任意表格行跳到对应字节。

### Apple 二进制内部结构
- Load commands、section、符号表与字符串表、重定位、间接符号、function starts、data-in-code。
- **代码签名**：解析 embedded-signature SuperBlob（Code Directory、Requirements、CMS），并解码 **entitlements** plist。
- **Objective-C** 元数据：类、方法、ivar、属性、协议。
- **Swift** 语义图（基于 `__swift5_*` 段）。
- **dyld 信息**：rebase / bind / lazy-bind / exports trie 与 chained-fixups 结构。

### 代码与交叉引用
- 可选 [Capstone](https://www.capstone-engine.org/) 反汇编 `__TEXT,__text`（arm64、x86_64、arm、i386）。
- Xref 报告：解析 call / jump 目标并关联回符号。

### dyld shared cache
- 列出镜像、提取单个或全部镜像，并直接钻入已提取的镜像分析。

### 体验与效率
- 解析在 **UI 线程之外**进行，打开大型二进制或数 GB 的 shared cache 也不卡界面。
- 表格：按列筛选、复制行、CSV 导出、键盘导航。
- 内置更新检查与提醒。

### 自动化
- 无界面的 `--cli` 模式以文本或**稳定 JSON** 输出完整分析——适合脚本、diff 与 CI。

### 为可疑文件而设计
解析器全程做了边界与对齐校验——LEB128 流、load command 遍历、字符串表、代码签名 blob——
并有针对畸形输入的崩溃回归套件（对截断和模糊后的二进制运行）。打开恶意或损坏文件时，
程序会优雅地降级而不是崩溃。

## 支持的格式
| 格式 | 说明 |
| --- | --- |
| Mach-O | 32 位与 64 位 |
| Fat / Universal | 多架构 slice |
| 静态库 | 含 Mach-O 成员的 `.a` |
| dyld shared cache | 列出、提取并检视镜像 |

## 安装

### macOS
- 下载最新版本：<https://github.com/everettjf/MachOExplorer/releases/latest>
- 或通过 Homebrew 安装：

```bash
brew update && brew tap everettjf/homebrew-tap && brew install --cask machoexplorer
```

### Linux / Windows
GitHub 上提供 macOS 预编译版本。在 Linux 与 Windows 上请从源码构建（见
[从源码构建](#从源码构建)）。Linux 构建与完整回归套件会在每次改动时由 CI 运行。

## 快速开始
1. 启动 MachOExplorer，或直接打开文件：`MachOExplorer <路径>`。
2. 打开内置示例（`sample/simple`、`sample/complex`）或你自己的二进制。
3. 浏览 **Layout** 树；在搜索框输入即可跳到某个 section、符号或数值；选中节点即可在 Hex 视图看到对应字节。

## CLI / 自动化模式
无需界面即可执行完整分析，并输出到终端或文件：

```bash
./build/MachOExplorer --cli sample/simple
./build/MachOExplorer --cli --format json --output /tmp/simple-analysis.json sample/simple
```

常用参数：

| 参数 | 说明 |
| --- | --- |
| `--format text\|json` | 输出格式（默认 `text`） |
| `--output <path>` | 输出到文件 |
| `--max-rows <N>` | 每个表最多输出 N 行（`0` 表示不限制） |
| `--max-depth <N>` | 限制分析树深度（`0` 表示不限制） |
| `--root-path <path>` | 仅导出某个分析子树 |
| `--name-filter <text>` | 仅保留名称匹配的节点 |
| `--table-mode <full\|headers\|summary>` | 控制表格载荷大小 |
| `--include-empty` | 包含空节点 |

JSON 输出便于自动化且稳定：包含 `schemaVersion`、`summary`、节点级 `path` / `kind` / `childIndex`，
以及行级 `cells` / `rowIndex`；所有字符串都会被净化为合法 UTF-8，即使输入畸形也能保证输出良构。
完整说明见 [CLI.zh-CN.md](CLI.zh-CN.md)。

## 从源码构建
依赖：CMake ≥ 3.16、Qt 6（Core、Gui、Widgets、Network、Concurrent）、C++14 编译器，
反汇编功能可选 [Capstone](https://www.capstone-engine.org/)。

```bash
# macOS
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$HOME/Qt/6.x.x/macos"
cmake --build build -j8

# Linux（Debian/Ubuntu 依赖：qt6-base-dev libgl1-mesa-dev libcapstone-dev）
./build_linux.sh        # 或：cmake -S src -B build && cmake --build build -j"$(nproc)"

# Windows
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvcXXXX_64"
cmake --build build --config Release
```

运行 `./build/MachOExplorer`（macOS 上为 `MachOExplorer.app`）。完整的开发、测试与发布流程见
[DEVELOP.md](DEVELOP.md)。

## 文档
- 开发文档（构建 / 测试 / 发布）：[DEVELOP.md](DEVELOP.md)
- CLI 文档：[CLI.zh-CN.md](CLI.zh-CN.md)
- 打包说明：[docs/release_packaging.md](docs/release_packaging.md)

## 贡献
欢迎提交 Issue 和 PR：
- 项目地址：<https://github.com/everettjf/MachOExplorer>
- 问题反馈：<https://github.com/everettjf/MachOExplorer/issues>

提交 PR 前，请先构建项目并运行 `tests/regression/run_all.sh`。

## 许可证
MIT，详见 [LICENSE](LICENSE)。

## Star History
[![Star History Chart](https://api.star-history.com/svg?repos=everettjf/MachOExplorer)](https://star-history.com/#everettjf/MachOExplorer)
