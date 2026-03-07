#include "XrefViewNode.h"
#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include <unordered_map>

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

}

void XrefViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!mh_) return;

    auto t = CreateTableView();
    t->SetHeaders({"Target", "Symbol", "Ref Count", "Referrer"});
    t->SetWidths({180, 360, 120, 460});

    std::unordered_map<uint64_t, std::string> symbols;
    auto *symtab = mh_->FindLoadCommand<LoadCommand_LC_SYMTAB>({LC_SYMTAB});
    if (symtab != nullptr) {
        for (auto &n : symtab->nlists_ref()) {
            if (n->n_strx() == 0) continue;
            const uint64_t addr = n->Is64() ? n->n_value64() : n->n_value();
            if (addr == 0) continue;
            std::string name = symtab->GetStringByStrX(n->n_strx());
            if (!name.empty() && symbols.count(addr) == 0) {
                symbols[addr] = name;
            }
        }
    }

    struct RefItem {
        std::string source;
        std::string kind;
        uint64_t ref_raw = 0;
        uint64_t ref_vm = 0;
    };
    std::unordered_map<uint64_t, std::vector<RefItem>> refs;

    auto collect_from_sections = [&](auto *seg_cmd) {
        for (auto &sect : seg_cmd->sections_ref()) {
            if (!IsPointerLikeSection(sect->sect())) continue;
            const std::string sec_name = sect->sect().segment_name() + "/" + sect->sect().section_name();
            if (sect->Is64()) {
                for (auto *cur : util::ParsePointerAsType<uint64_t>(sect->GetOffset(), sect->GetSize())) {
                    const uint64_t target = *cur;
                    if (target == 0) continue;
                    refs[target].push_back({sec_name, "pointer", sect->GetRAW(cur), 0});
                }
            } else {
                for (auto *cur : util::ParsePointerAsType<uint32_t>(sect->GetOffset(), sect->GetSize())) {
                    const uint64_t target = *cur;
                    if (target == 0) continue;
                    refs[target].push_back({sec_name, "pointer", sect->GetRAW(cur), 0});
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
            for (size_t i = 0; i < count; ++i) {
                const auto &ci = insn[i];
                bool is_branch = false;
                bool is_call = false;
                for (uint8_t g = 0; g < ci.detail->groups_count; ++g) {
                    const uint8_t grp = ci.detail->groups[g];
                    if (grp == CS_GRP_JUMP) is_branch = true;
                    if (grp == CS_GRP_CALL) is_call = true;
                }
                if (!is_branch && !is_call) continue;
                if (arch == CS_ARCH_X86) {
                    for (uint8_t o = 0; o < ci.detail->x86.op_count; ++o) {
                        const auto &op = ci.detail->x86.operands[o];
                        if (op.type == X86_OP_IMM) {
                            refs[static_cast<uint64_t>(op.imm)].push_back(
                                    {"__TEXT/__text", is_call ? "call" : "jump",
                                     sect->GetRAW(sect->GetOffset() + (ci.address - base)), ci.address});
                        }
                    }
                }
                if (arch == CS_ARCH_ARM64) {
                    for (uint8_t o = 0; o < ci.detail->arm64.op_count; ++o) {
                        const auto &op = ci.detail->arm64.operands[o];
                        if (op.type == ARM64_OP_IMM) {
                            refs[static_cast<uint64_t>(op.imm)].push_back(
                                    {"__TEXT/__text", is_call ? "call" : "jump", sect->GetRAW(sect->GetOffset() + (ci.address - base)), ci.address});
                        }
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
        if (sym_it != symbols.end()) symbol = sym_it->second;
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
