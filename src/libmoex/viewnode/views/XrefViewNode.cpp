#include "XrefViewNode.h"
#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(MOEX_HAS_CAPSTONE) && MOEX_HAS_CAPSTONE
#include <capstone/capstone.h>
#endif

MOEX_NAMESPACE_BEGIN

namespace {

static bool IsPointerLikeSection(moex_section &sect) {
    const uint32_t stype = sect.flags() & SECTION_TYPE;
    switch (stype) {
        case S_LITERAL_POINTERS:
        case S_NON_LAZY_SYMBOL_POINTERS:
        case S_LAZY_SYMBOL_POINTERS:
        case S_LAZY_DYLIB_SYMBOL_POINTERS:
        case S_MOD_INIT_FUNC_POINTERS:
        case S_MOD_TERM_FUNC_POINTERS:
            return true;
        default:
            break;
    }
    const std::string name = sect.section_name();
    if (name.find("__objc_") == 0) return true;
    if (name == "__objc_classrefs" || name == "__objc_superrefs" || name == "__objc_selrefs") return true;
    return false;
}

static bool VmToRawPtr(MachHeader *mh, uint64_t vmaddr, std::size_t size, char *&raw) {
    raw = nullptr;
    for (auto *seg : mh->GetSegments64()) {
        const uint64_t seg_vm = seg->cmd()->vmaddr;
        const uint64_t seg_size = seg->cmd()->vmsize;
        if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_size) continue;
        const uint64_t delta = vmaddr - seg_vm;
        if (delta > seg->cmd()->filesize || size > (seg->cmd()->filesize - delta)) return false;
        raw = reinterpret_cast<char *>(seg->ctx()->file_start) + seg->cmd()->fileoff + delta;
        return true;
    }
    for (auto *seg : mh->GetSegments()) {
        const uint64_t seg_vm = seg->cmd()->vmaddr;
        const uint64_t seg_size = seg->cmd()->vmsize;
        if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_size) continue;
        const uint64_t delta = vmaddr - seg_vm;
        if (delta > seg->cmd()->filesize || size > (seg->cmd()->filesize - delta)) return false;
        raw = reinterpret_cast<char *>(seg->ctx()->file_start) + seg->cmd()->fileoff + delta;
        return true;
    }
    return false;
}

static bool ReadPointerAtVm(MachHeader *mh, uint64_t vmaddr, bool is64, uint64_t &value) {
    value = 0;
    char *raw = nullptr;
    const std::size_t sz = is64 ? sizeof(uint64_t) : sizeof(uint32_t);
    if (!VmToRawPtr(mh, vmaddr, sz, raw) || raw == nullptr) return false;
    if (is64) {
        value = *reinterpret_cast<uint64_t *>(raw);
        return true;
    }
    value = *reinterpret_cast<uint32_t *>(raw);
    return true;
}

}

void XrefViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!mh_) return;

    auto t = CreateTableView();
    t->SetHeaders({"Target", "Symbol", "Ref Count", "Referrer"});
    t->SetWidths({180, 360, 120, 460});

    std::unordered_map<uint64_t, std::string> symbols;
    std::vector<std::pair<uint64_t, std::string>> sorted_symbols;
    auto *symtab = mh_->FindLoadCommand<LoadCommand_LC_SYMTAB>({LC_SYMTAB});
    if (symtab != nullptr) {
        for (auto &n : symtab->nlists_ref()) {
            if (n->n_strx() == 0) continue;
            const uint64_t addr = n->Is64() ? n->n_value64() : n->n_value();
            if (addr == 0) continue;
            std::string name = symtab->GetStringByStrX(n->n_strx());
            if (!name.empty() && symbols.count(addr) == 0) {
                symbols[addr] = name;
                sorted_symbols.push_back({addr, name});
            }
        }
    }
    std::sort(sorted_symbols.begin(), sorted_symbols.end(),
              [](const std::pair<uint64_t, std::string> &a, const std::pair<uint64_t, std::string> &b) {
                  return a.first < b.first;
              });

    struct RefItem {
        std::string source;
        std::string kind;
        uint64_t ref_raw = 0;
        uint64_t ref_vm = 0;
    };
    std::unordered_map<uint64_t, std::vector<RefItem>> refs;
    std::unordered_map<uint64_t, std::unordered_set<std::string>> ref_dedup;
    auto add_ref = [&](uint64_t target, const RefItem &item) {
        const std::string key = item.source + "|" + item.kind + "|" +
                                std::to_string(item.ref_raw) + "|" + std::to_string(item.ref_vm);
        if (ref_dedup[target].insert(key).second) {
            refs[target].push_back(item);
        }
    };

    auto collect_from_sections = [&](auto *seg_cmd) {
        for (auto &sect : seg_cmd->sections_ref()) {
            if (!IsPointerLikeSection(sect->sect())) continue;
            const std::string sec_name = sect->sect().segment_name() + "/" + sect->sect().section_name();
            if (sect->Is64()) {
                for (auto *cur : util::ParsePointerAsType<uint64_t>(sect->GetOffset(), sect->GetSize())) {
                    const uint64_t target = *cur;
                    if (target == 0) continue;
                    add_ref(target, {sec_name, "pointer", sect->GetRAW(cur), 0});
                }
            } else {
                for (auto *cur : util::ParsePointerAsType<uint32_t>(sect->GetOffset(), sect->GetSize())) {
                    const uint64_t target = *cur;
                    if (target == 0) continue;
                    add_ref(target, {sec_name, "pointer", sect->GetRAW(cur), 0});
                }
            }
        }
    };

    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT>({LC_SEGMENT}, [&](LoadCommand_LC_SEGMENT *seg, bool &stop) {
        collect_from_sections(seg);
    });
    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64}, [&](LoadCommand_LC_SEGMENT_64 *seg, bool &stop) {
        collect_from_sections(seg);
    });

#if defined(MOEX_HAS_CAPSTONE) && MOEX_HAS_CAPSTONE
    auto scan_text_disasm = [&](auto *seg_cmd) {
        for (auto &sect : seg_cmd->sections_ref()) {
            if (sect->sect().segment_name() != "__TEXT" || sect->sect().section_name() != "__text") continue;
            uint32_t arch = 0, mode = 0;
            switch (mh_->data_ptr()->cputype) {
                case CPU_TYPE_I386: arch = CS_ARCH_X86; mode = CS_MODE_32; break;
                case CPU_TYPE_X86_64: arch = CS_ARCH_X86; mode = CS_MODE_64; break;
                case CPU_TYPE_ARM: arch = CS_ARCH_ARM; mode = CS_MODE_ARM; break;
                case CPU_TYPE_ARM64: arch = CS_ARCH_ARM64; mode = CS_MODE_ARM; break;
                default: return;
            }
            csh handle;
            if (cs_open(static_cast<cs_arch>(arch), static_cast<cs_mode>(mode), &handle) != CS_ERR_OK) return;
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

            const uint8_t *code = reinterpret_cast<const uint8_t *>(sect->GetOffset());
            const size_t code_size = sect->GetSize();
            const uint64_t base = sect->Is64() ? sect->sect().addr64() : sect->sect().addr();
            cs_insn *insn = nullptr;
            const size_t count = cs_disasm(handle, code, code_size, base, 0, &insn);
            std::unordered_map<unsigned int, uint64_t> arm64_reg_targets;
            for (size_t i = 0; i < count; ++i) {
                const auto &ci = insn[i];
                bool is_branch = false;
                bool is_call = false;
                for (uint8_t g = 0; g < ci.detail->groups_count; ++g) {
                    const uint8_t grp = ci.detail->groups[g];
                    if (grp == CS_GRP_JUMP) is_branch = true;
                    if (grp == CS_GRP_CALL) is_call = true;
                }

                if (arch == CS_ARCH_ARM64) {
                    uint8_t regs_read[36] = {0};
                    uint8_t regs_write[36] = {0};
                    uint8_t read_count = 0;
                    uint8_t write_count = 0;
                    if (cs_regs_access(handle, &ci, regs_read, &read_count, regs_write, &write_count) == 0) {
                        for (uint8_t ri = 0; ri < write_count; ++ri) {
                            arm64_reg_targets.erase(regs_write[ri]);
                        }
                    }

                    const auto &arm = ci.detail->arm64;
                    if ((ci.id == ARM64_INS_ADR || ci.id == ARM64_INS_ADRP) &&
                        arm.op_count >= 2 &&
                        arm.operands[0].type == ARM64_OP_REG &&
                        arm.operands[1].type == ARM64_OP_IMM) {
                        arm64_reg_targets[arm.operands[0].reg] = static_cast<uint64_t>(arm.operands[1].imm);
                    } else if (ci.id == ARM64_INS_ADD &&
                               arm.op_count >= 3 &&
                               arm.operands[0].type == ARM64_OP_REG &&
                               arm.operands[1].type == ARM64_OP_REG &&
                               arm.operands[2].type == ARM64_OP_IMM) {
                        auto hit = arm64_reg_targets.find(arm.operands[1].reg);
                        if (hit != arm64_reg_targets.end()) {
                            arm64_reg_targets[arm.operands[0].reg] = static_cast<uint64_t>(static_cast<int64_t>(hit->second) + arm.operands[2].imm);
                        }
                    } else if (ci.id == ARM64_INS_MOV &&
                               arm.op_count >= 2 &&
                               arm.operands[0].type == ARM64_OP_REG &&
                               arm.operands[1].type == ARM64_OP_REG) {
                        auto hit = arm64_reg_targets.find(arm.operands[1].reg);
                        if (hit != arm64_reg_targets.end()) {
                            arm64_reg_targets[arm.operands[0].reg] = hit->second;
                        }
                    } else if ((ci.id == ARM64_INS_LDR || ci.id == ARM64_INS_LDUR || ci.id == ARM64_INS_LDRSW) &&
                               arm.op_count >= 2 &&
                               arm.operands[0].type == ARM64_OP_REG &&
                               arm.operands[1].type == ARM64_OP_MEM) {
                        const auto base_reg = arm.operands[1].mem.base;
                        auto hit = arm64_reg_targets.find(base_reg);
                        if (hit != arm64_reg_targets.end()) {
                            const uint64_t mem_vmaddr = static_cast<uint64_t>(static_cast<int64_t>(hit->second) + arm.operands[1].mem.disp);
                            uint64_t pointed = 0;
                            if (ReadPointerAtVm(mh_, mem_vmaddr, true, pointed) && pointed != 0) {
                                arm64_reg_targets[arm.operands[0].reg] = pointed;
                            }
                        }
                    }
                }

                if (!is_branch && !is_call) continue;

                if (arch == CS_ARCH_X86) {
                    for (uint8_t o = 0; o < ci.detail->x86.op_count; ++o) {
                        const auto &op = ci.detail->x86.operands[o];
                        if (op.type == X86_OP_IMM) {
                            add_ref(static_cast<uint64_t>(op.imm),
                                    {"__TEXT/__text", is_call ? "call" : "jump",
                                     sect->GetRAW(sect->GetOffset() + (ci.address - base)), ci.address});
                        } else if (op.type == X86_OP_MEM &&
                                   (op.mem.base == X86_REG_RIP || op.mem.base == X86_REG_EIP)) {
                            const uint64_t mem_vmaddr = static_cast<uint64_t>(static_cast<int64_t>(ci.address) + ci.size + op.mem.disp);
                            uint64_t target = 0;
                            const bool is64ptr = (mode == CS_MODE_64);
                            if (ReadPointerAtVm(mh_, mem_vmaddr, is64ptr, target) && target != 0) {
                                add_ref(target, {"__TEXT/__text", is_call ? "call-ripmem" : "jump-ripmem",
                                                 sect->GetRAW(sect->GetOffset() + (ci.address - base)), ci.address});
                            }
                        }
                    }
                }
                if (arch == CS_ARCH_ARM64) {
                    for (uint8_t o = 0; o < ci.detail->arm64.op_count; ++o) {
                        const auto &op = ci.detail->arm64.operands[o];
                        if (op.type == ARM64_OP_IMM) {
                            add_ref(static_cast<uint64_t>(op.imm),
                                    {"__TEXT/__text", is_call ? "call" : "jump",
                                     sect->GetRAW(sect->GetOffset() + (ci.address - base)), ci.address});
                        }
                    }

                    for (uint8_t o = 0; o < ci.detail->arm64.op_count; ++o) {
                        const auto &op = ci.detail->arm64.operands[o];
                        if (op.type != ARM64_OP_REG) continue;
                        auto hit = arm64_reg_targets.find(op.reg);
                        if (hit == arm64_reg_targets.end()) continue;
                        const uint64_t target = hit->second;
                        add_ref(target, {"__TEXT/__text", is_call ? "call-reg" : "jump-reg",
                                         sect->GetRAW(sect->GetOffset() + (ci.address - base)), ci.address});
                    }
                }
            }
            if (insn) cs_free(insn, count);
            cs_close(&handle);
        }
    };
    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT>({LC_SEGMENT}, [&](LoadCommand_LC_SEGMENT *seg, bool &stop) { scan_text_disasm(seg); });
    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64}, [&](LoadCommand_LC_SEGMENT_64 *seg, bool &stop) { scan_text_disasm(seg); });
#endif

    std::vector<uint64_t> targets;
    targets.reserve(refs.size());
    for (auto &it : refs) targets.push_back(it.first);
    std::sort(targets.begin(), targets.end());

    for (auto target : targets) {
        auto &items = refs[target];
        std::string symbol = "";
        auto sym_it = symbols.find(target);
        if (sym_it != symbols.end()) {
            symbol = sym_it->second;
        } else if (!sorted_symbols.empty()) {
            auto up = std::upper_bound(
                    sorted_symbols.begin(), sorted_symbols.end(), target,
                    [](uint64_t value, const std::pair<uint64_t, std::string> &item) {
                        return value < item.first;
                    });
            if (up != sorted_symbols.begin()) {
                --up;
                if (target > up->first) {
                    symbol = fmt::format("{}+0x{}", up->second, AsShortHexString(target - up->first));
                }
            }
        }
        t->AddRow({AsAddress(target), symbol, AsString(items.size()), ""});
        for (const auto &item : items) {
            std::string ref = fmt::format("{} [{}] @ raw 0x{}", item.source, item.kind, AsShortHexString(item.ref_raw));
            if (item.ref_vm != 0) {
                ref += fmt::format(" vm 0x{}", AsShortHexString(item.ref_vm));
            }
            t->AddRow({AsAddress(target), symbol, "", ref});
        }
        t->AddSeparator();
    }

    if (targets.empty()) {
        t->AddRow({"-", "-", "0", "No pointer-based xrefs found"});
    }
}

MOEX_NAMESPACE_END
