# MachOExplorer CLI 使用手册

本文档完整介绍 MachOExplorer 的命令行能力，包括新的无界面完整分析模式，以及现有 `moex-*` 工具。

## 总览
MachOExplorer 现在有两类 CLI 用法：

1. `MachOExplorer --cli ...`
无界面执行完整分析，复用桌面版同一套分析树（`ViewNode`）。

2. 专用工具（`moex-*`）
面向特定场景：解析回归、差异对比、dyld cache 列表与提取。

## 1）无界面完整分析模式

### 基本用法

```bash
./build/MachOExplorer --cli [options] <input-file>
```

### 查看帮助

```bash
./build/MachOExplorer --cli --help
```

### 参数说明

- `--cli`
  启用命令行模式，不启动 GUI。
- `--format text|json`
  输出格式，默认 `text`。
- `--output <path>`
  输出到文件（不指定则输出到 stdout）。
- `--max-rows <N>`
  每个表最多输出 N 行，`0` 表示不限制。
- `--max-depth <N>`
  限制分析树深度，`0` 表示不限制。
- `--root-path <path>`
  只导出匹配路径的分析子树，例如 `File/Mach Header (x86_64)`。
- `--name-filter <text>`
  仅保留名称包含指定文本的节点，适合快速收窄输出范围。
- `--table-mode <full|headers|summary>`
  控制表格导出粒度：完整行、仅表头、或仅行数摘要。
- `--include-empty`
  包含无表格/无二进制载荷的空节点。
- `-h` / `--help`
  显示帮助信息。

### 输出行为

- 默认输出到 `stdout`。
- 参数错误或分析失败输出到 `stderr`。
- JSON 输出为可读缩进格式。
- JSON 顶层包含 `schemaVersion`、`summary`、`options` 等稳定元信息。
- 节点包含 `path`、`depth`、`kind`、`childIndex`；表格行包含 `rowIndex` 与 `cells`。

### 退出码

- `0`：执行成功
- `1`：分析或运行失败（例如解析失败、无法写输出文件）
- `2`：命令行参数错误

### “全面”能力范围说明

`--cli` 会递归遍历完整 `ViewNode` 树，导出当前版本中所有已实现分析节点的数据。例如：

- 文件类型总览（Mach-O / Fat / Archive / dyld shared cache）
- Mach Header、Load Commands
- Sections 及派生视图
- Symbol/String 相关视图
- Dynamic Loader Info（含现代 dyld exports/fixups 视图）
- 当前 `ViewNode` 已接入的 Swift/ObjC 相关视图
- dyld shared cache 顶层视图（mappings/images）

说明：

- 就当前架构而言，CLI 与 GUI 共用同一分析树主干，分析覆盖是“全面”的。
- GUI 专属交互行为（排序、筛选、复制操作本身）不作为 CLI 行为导出，但其底层分析数据会导出。

### 示例

#### 终端文本输出

```bash
./build/MachOExplorer --cli sample/simple
```

#### 终端 JSON 输出

```bash
./build/MachOExplorer --cli --format json sample/simple
```

#### 写入 JSON 文件

```bash
./build/MachOExplorer --cli --format json --output /tmp/simple-analysis.json sample/simple
```

#### 大文件限制输出量

```bash
./build/MachOExplorer --cli --max-rows 100 --max-depth 4 sample/complex
```

#### 包含空节点（便于结构遍历）

```bash
./build/MachOExplorer --cli --format json --include-empty sample/simple
```

#### 仅导出某个子树

```bash
./build/MachOExplorer --cli --format json --root-path "File/Mach Header (x86_64)" sample/simple
```

#### 仅关注指定节点并输出表格摘要

```bash
./build/MachOExplorer --cli --format json --name-filter Xref --table-mode summary sample/simple
```

#### 与 jq 联用

```bash
./build/MachOExplorer --cli --format json sample/simple | jq '.analysis.name'
```

## 2）专用命令行工具（`moex-*`）

这些工具同样会在 `build/` 下生成。

## `moex-parse`

```bash
./build/moex-parse <file> [file...]
```

用途：

- 快速验证解析可用性
- 回归脚本依赖该工具

## `moex-diff`

```bash
./build/moex-diff <fileA> <fileB>
```

用途：

- 对比首个 Mach-O Header 关键信息
- 对比 section/symbol 集合差异

## `moex-cache-list`

```bash
./build/moex-cache-list [--json] [--exact] [--limit=N] [--output=file] <dyld_shared_cache_file> [path-filter]
```

用途：

- 列出 dyld shared cache 中的镜像
- 支持精确/子串过滤

## `moex-cache-extract`

```bash
./build/moex-cache-extract [--compact] [--all] [--exact] [--dry-run] [--json] [--max=N] <dyld_shared_cache_file> <image-path-or-substr> <output-macho-or-dir>
```

用途：

- 从 dyld shared cache 提取单个或多个镜像
- 可选 compact 模式重写 segment file offset

## 自动化示例

### CLI 冒烟测试

```bash
tests/regression/run_cli_smoke.sh
```

### 全量回归

```bash
tests/regression/run_all.sh
```

### 批量分析示例

```bash
for f in sample/simple sample/complex; do
  ./build/MachOExplorer --cli --format json --output "/tmp/$(basename "$f").json" "$f"
done
```

## 构建提示

```bash
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
```

## 常见问题

- 报错：`expected exactly one input file`
  原因：`--cli` 模式需要且只接受一个输入文件。
- 报错：`unsupported format: ...`
  原因：当前仅支持 `text` 和 `json`。
- 报错：`analysis failed: ...`
  原因：文件解析失败或输入样本异常。
- 报错：`cannot open output file`
  原因：输出路径不可写或目录不存在。
