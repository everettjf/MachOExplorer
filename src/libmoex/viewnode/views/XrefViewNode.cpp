#include "XrefViewNode.h"
#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"

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
        std::string section;
        uint64_t ref_raw = 0;
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
                    refs[target].push_back({sec_name, sect->GetRAW(cur)});
                }
            } else {
                for (auto *cur : util::ParsePointerAsType<uint32_t>(sect->GetOffset(), sect->GetSize())) {
                    const uint64_t target = *cur;
                    if (target == 0) continue;
                    refs[target].push_back({sec_name, sect->GetRAW(cur)});
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
            t->AddRow({AsAddress(target), symbol, "", fmt::format("{} @ 0x{}", item.section, AsShortHexString(item.ref_raw))});
        }
        t->AddSeparator();
    }

    if (targets.empty()) {
        t->AddRow({"-", "-", "0", "No pointer-based xrefs found"});
    }
}

MOEX_NAMESPACE_END
