# MachOExplorer

一个聚焦于 Mach-O 生态的桌面分析工具，支持 Mach-O、Archive 与 dyld shared cache。

语言：简体中文 | [English](README.md)

![MachOExplorer 截图](image/screenshot.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## 项目特点
- 一个界面查看 Mach-O / Fat Mach-O / `.a` Archive / dyld shared cache。
- 从结构到代码快速钻取：section、symbol、反汇编、xref。
- 面向 Apple 现代二进制：ObjC 元数据、Swift 元数据、dyld exports/fixups。
- 解析边界校验与回归测试完善，处理异常样本更稳。

## 主要功能
- Load commands、sections、symbols、relocations、strings 等视图。
- 可选 Capstone 支持 `__TEXT,__text` 反汇编。
- Swift 语义图（`__swift5_*`）与 ObjC 元数据浏览。
- dyld shared cache 镜像列表/提取/一键钻取分析。
- 表格交互增强：筛选、复制、CSV 导出、键盘导航。
- 内置更新检查与提醒策略。

## 安装（macOS）
- 最新版本：<https://github.com/everettjf/MachOExplorer/releases/latest>
- Homebrew：

```bash
brew update && brew tap everettjf/homebrew-tap && brew install --cask machoexplorer
```

## 快速开始
1. 启动 MachOExplorer。
2. 打开示例文件（`sample/simple` 或 `sample/complex`）或你自己的 Mach-O。
3. 在左侧 Layout 树中进入 section/symbol/disassembly 等视图。

## CLI 分析模式（无界面）
可直接以命令行方式执行完整分析，并输出到终端或文件：

```bash
./build/MachOExplorer --cli sample/simple
./build/MachOExplorer --cli --format json --output /tmp/simple-analysis.json sample/simple
```

常用参数：
- `--format text|json` 输出格式（默认 `text`）
- `--output <path>` 输出到文件
- `--max-rows <N>` 每个表最多输出 N 行（`0` 表示不限制）
- `--max-depth <N>` 限制分析树深度（`0` 表示不限制）
- `--root-path <path>` 仅导出某个分析子树
- `--name-filter <text>` 仅保留名称匹配的节点
- `--table-mode <full|headers|summary>` 控制表格载荷大小
- `--include-empty` 包含空节点

JSON 输出现在包含更稳定的自动化字段：`schemaVersion`、`summary`、节点级 `path` / `kind` / `childIndex`，以及表格行级 `cells` / `rowIndex`。

## 文档
- 开发文档（构建/测试/发布）：[DEVELOP.md](DEVELOP.md)
- 打包说明：[docs/release_packaging.md](docs/release_packaging.md)
- CLI 文档：[CLI.zh-CN.md](CLI.zh-CN.md)

## 贡献
欢迎提交 Issue 和 PR：
- 项目地址：<https://github.com/everettjf/MachOExplorer>
- 问题反馈：<https://github.com/everettjf/MachOExplorer/issues>

## 许可证
MIT，详见 [LICENSE](LICENSE)。

## Star History
[![Star History Chart](https://api.star-history.com/svg?repos=everettjf/MachOExplorer&type=Date)](https://star-history.com/#everettjf/MachOExplorer&Date)
