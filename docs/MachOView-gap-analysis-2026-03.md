# MachOExplorer vs MachOView 差距分析（2026-03）

本文基于本仓库当前实现与 MachOView 源码对比（`/tmp/MachOView_20260307`）给出“要超越还需做什么”。

## 当前已完成（本轮已落地）
- `__TEXT,__text` 指令级反汇编（Capstone，可选依赖）+ 符号联动
- relocation 独立可视化节点 + x86_64/arm64 类型解析
- 按 PID 附加（路径级 attach workflow）
- `.a` Archive 多成员浏览
- ObjC2 元数据树（类/分类/协议/方法/属性）
- Swift `__swift5_*` 基础元数据视图 + 基础符号解码提示
- 现代 dyld：`LC_DYLD_EXPORTS_TRIE`/`LC_DYLD_CHAINED_FIXUPS` 条目级解析
- 边界校验增强 + fuzz target + 崩溃回归 runner

## 对比结论
- 与 MachOView 的核心能力已基本齐平，且在跨平台（Qt）和工程化（CMake/fuzz/regression）上更现代。
- 但要“明确超越”，还缺少几项高价值、可验证的深度能力。

## 仍有差距的关键项（按优先级）

### P0（建议优先冲刺）
1. 实时进程内存附加与导出（真正 task_for_pid 级别）
- 现状：当前仅“按 PID 打开可执行路径”，不是内存镜像分析。
- 对标：MachOView `Attach.h/.mm` 有 `task_for_pid` 级能力。
- 目标：支持从目标进程读取主镜像/共享缓存映射并生成可分析快照。

2. Chained fixups 全指针格式完整解码
- 现状：已支持 headers/starts/imports 和部分链条解码，但仍偏“通用化”。
- 目标：覆盖 `DYLD_CHAINED_PTR_*` 全格式（含 arm64e userland24、auth/rebase/bind 全分支）并给出符号/段落联动。

3. Exports trie 终端信息完整语义化
- 现状：已解码 node/edge/terminal flags/value。
- 目标：完整区分 regular/reexport/stub-resolver/thread-local，并解析 reexport ordinal/name、resolver 地址等语义字段。

### P1（提升“专业逆向工具”体验）
1. 反汇编视图增强
- 增加 Data-in-Code 标注、跳转目标高亮、函数边界恢复、交叉引用（Xref）面板。
- 目标：避免“遇到嵌入数据就断流”的体验问题。

2. ObjC/Swift 交叉跳转与反向索引
- 现状：已有树状解析。
- 目标：新增“方法 -> IMP -> 所在函数 -> 被谁引用”的双向导航；Swift 符号接入更完整 demangler。

3. linkedit 全链路一致性校验
- 把 symtab/dysymtab/indirect symbols/bind/export/fixup 做一致性检查报告（missing symbol、越界、重复、坏索引）。

### P2（形成“超越”标志能力）
1. 安全测试体系产品化
- 引入 CI 模糊测试（ASan/UBSan + corpus 进化）和崩溃样本自动最小化归档。
- 形成“每次提交可验证无回归”的安全基线。

2. 可比较分析（Diff）
- 新增 Mach-O / Archive 对比视图：LoadCommands、symbols、ObjC、dyld fixups 的结构差异。
- 这是 MachOView 没有的“超越型功能”。

3. dyld shared cache 解析（可选）
- 若加入 shared cache 导航与提取，可显著拉开与传统 Mach-O 浏览器差距。

## 建议里程碑（可直接排期）
- M1（1~2 周）：真实 attach + fixups 全格式 + exports trie 语义化
- M2（1~2 周）：反汇编增强 + ObjC/Swift 双向跳转
- M3（1 周）：CI fuzz/regression 自动化 + 差异分析 MVP

## 对标依据（MachOView 代码入口）
- `Attach.h`, `Attach.mm`
- `DyldInfo.h`, `DyldInfo.mm`
- `LinkEdit.h`, `LinkEdit.mm`
- `SectionContents.h`, `SectionContents.mm`
- `ObjC.h`, `ObjC.mm`
