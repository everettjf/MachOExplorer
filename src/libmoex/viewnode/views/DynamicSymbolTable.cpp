//
// Created by everettjf on 2017/7/24.
//

#include "DynamicSymbolTable.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include <unordered_map>

MOEX_NAMESPACE_BEGIN

namespace {
constexpr uint32_t R_SCATTERED = 0x80000000;

std::string GetRelocTypeString(qv_cpu_type_t cpu_type, uint32_t type)
{
    if (cpu_type == CPU_TYPE_X86_64) {
        switch (type) {
            case 0: return "X86_64_RELOC_UNSIGNED";
            case 1: return "X86_64_RELOC_SIGNED";
            case 2: return "X86_64_RELOC_BRANCH";
            case 3: return "X86_64_RELOC_GOT_LOAD";
            case 4: return "X86_64_RELOC_GOT";
            case 5: return "X86_64_RELOC_SUBTRACTOR";
            case 6: return "X86_64_RELOC_SIGNED_1";
            case 7: return "X86_64_RELOC_SIGNED_2";
            case 8: return "X86_64_RELOC_SIGNED_4";
            case 9: return "X86_64_RELOC_TLV";
            default: return fmt::format("X86_64_RELOC_UNKNOWN({})", type);
        }
    }
    if (cpu_type == CPU_TYPE_ARM64) {
        switch (type) {
            case 0: return "ARM64_RELOC_UNSIGNED";
            case 1: return "ARM64_RELOC_SUBTRACTOR";
            case 2: return "ARM64_RELOC_BRANCH26";
            case 3: return "ARM64_RELOC_PAGE21";
            case 4: return "ARM64_RELOC_PAGEOFF12";
            case 5: return "ARM64_RELOC_GOT_LOAD_PAGE21";
            case 6: return "ARM64_RELOC_GOT_LOAD_PAGEOFF12";
            case 7: return "ARM64_RELOC_POINTER_TO_GOT";
            case 8: return "ARM64_RELOC_TLVP_LOAD_PAGE21";
            case 9: return "ARM64_RELOC_TLVP_LOAD_PAGEOFF12";
            case 10: return "ARM64_RELOC_ADDEND";
            case 11: return "ARM64_RELOC_AUTHENTICATED_POINTER";
            default: return fmt::format("ARM64_RELOC_UNKNOWN({})", type);
        }
    }
    return fmt::format("RELOC_TYPE({})", type);
}
}

class DynamicSymbolTableChild_IndirectSymbols : public DynamicSymbolTableChild{
public:
    void InitViewDatas() override {
        using namespace moex::util;

        moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
        if(!seg)
            return;

        // Indirect Symbols
        if(seg->ExistIndirectSymbols()) {
            // "Indirect Symbols"
            auto x = CreateTableView(mh_.get());
            x->SetHeaders({"Offset","Data","Description","Value"});
            x->SetWidths({100,100,100,400});

            seg->ForEachIndirectSymbols([&](uint32_t * indirect_index){
                x->AddRow({
                                  AsAddress(mh_->GetRAW(indirect_index)),
                                  AsHexString(*indirect_index),
                                  "Indirect Index",
                                  AsShortHexString(*indirect_index)
                          } );
            });
        }
    }
};

class DynamicSymbolTableChild_Relocations : public DynamicSymbolTableChild{
private:
    std::string ResolveTargetName(
            uint32_t symbol_or_sect,
            bool is_extern,
            const std::unordered_map<uint32_t, std::string> &symbols,
            const std::unordered_map<uint32_t, std::string> &sections) const
    {
        if (is_extern) {
            auto it = symbols.find(symbol_or_sect);
            if (it != symbols.end()) return it->second;
            return fmt::format("symbol#{}", symbol_or_sect);
        }

        if (symbol_or_sect == 0) return "R_ABS";
        auto it = sections.find(symbol_or_sect);
        if (it != sections.end()) return it->second;
        return fmt::format("section#{}", symbol_or_sect);
    }

    void AddRelocationTable(
            moex::TableViewDataPtr &t,
            const char *scope,
            std::function<void(std::function<void(char *, uint32_t)>)> for_each,
            const std::unordered_map<uint32_t, std::string> &symbols,
            const std::unordered_map<uint32_t, std::string> &sections)
    {
        using namespace moex::util;
        for_each([&](char *entry, uint32_t index) {
            const uint32_t word0 = *reinterpret_cast<uint32_t *>(entry);
            const uint32_t word1 = *reinterpret_cast<uint32_t *>(entry + 4);
            const bool scattered = (word0 & R_SCATTERED) != 0;

            if (scattered) {
                const uint32_t r_address = word0 & 0x00FFFFFF;
                const uint32_t r_type = (word0 >> 24U) & 0x0FU;
                const uint32_t r_length = (word0 >> 28U) & 0x03U;
                const uint32_t r_pcrel = (word0 >> 30U) & 0x01U;
                const uint32_t r_value = word1;
                t->AddRow(entry, sizeof(qv_relocation_info), {
                        fmt::format("{}#{}", scope, index),
                        fmt::format("addr=0x{} value=0x{}",
                                    AsShortHexString(r_address),
                                    AsShortHexString(r_value)),
                        GetRelocTypeString(mh_->data_ptr()->cputype, r_type),
                        fmt::format("scattered pcrel={} len={}", r_pcrel, (1U << r_length))
                });
                return;
            }

            const int32_t r_address = static_cast<int32_t>(word0);
            const uint32_t r_symbolnum = word1 & 0x00FFFFFFU;
            const bool r_pcrel = ((word1 >> 24U) & 0x01U) != 0;
            const uint32_t r_length = (word1 >> 25U) & 0x03U;
            const bool r_extern = ((word1 >> 27U) & 0x01U) != 0;
            const uint32_t r_type = (word1 >> 28U) & 0x0FU;

            t->AddRow(entry, sizeof(qv_relocation_info), {
                    fmt::format("{}#{}", scope, index),
                    fmt::format("addr={} target={}",
                                r_address >= 0 ? fmt::format("0x{}", AsShortHexString((uint32_t)r_address))
                                               : fmt::format("-0x{}", AsShortHexString((uint32_t)(-r_address))),
                                ResolveTargetName(r_symbolnum, r_extern, symbols, sections)),
                    GetRelocTypeString(mh_->data_ptr()->cputype, r_type),
                    fmt::format("extern={} pcrel={} len={}", r_extern, r_pcrel, (1U << r_length))
            });
        });
    }

public:
    void InitViewDatas() override {
        auto *dysym = mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
        if(!dysym) return;

        auto t = CreateTableView(mh_.get());
        t->SetHeaders({"Index", "Address/Target", "Type", "Attributes"});
        t->SetWidths({120, 420, 280, 240});

        std::unordered_map<uint32_t, std::string> symbols;
        if (auto *symtab = mh_->FindLoadCommand<moex::LoadCommand_LC_SYMTAB>({LC_SYMTAB})) {
            uint32_t index = 0;
            for (auto &item : symtab->nlists_ref()) {
                symbols[index++] = symtab->GetStringByStrX(item->n_strx());
            }
        }

        std::unordered_map<uint32_t, std::string> sections;
        uint32_t section_index = 1;
        for (auto *seg : mh_->GetSegments()) {
            for (auto &sect : seg->sections_ref()) {
                sections[section_index++] = fmt::format("{}/{}", sect->sect().segment_name(), sect->sect().section_name());
            }
        }
        for (auto *seg : mh_->GetSegments64()) {
            for (auto &sect : seg->sections_ref()) {
                sections[section_index++] = fmt::format("{}/{}", sect->sect().segment_name(), sect->sect().section_name());
            }
        }

        if (dysym->ExistExternalRelocations()) {
            AddRelocationTable(t, "ext", [&](std::function<void(char *, uint32_t)> cb) {
                dysym->ForEachExternalRelocations(cb);
            }, symbols, sections);
        }
        if (dysym->ExistLocalRelocations()) {
            AddRelocationTable(t, "loc", [&](std::function<void(char *, uint32_t)> cb) {
                dysym->ForEachLocalRelocations(cb);
            }, symbols, sections);
        }
        if (!dysym->ExistExternalRelocations() && !dysym->ExistLocalRelocations()) {
            t->AddRow({"-", "No relocation entries in LC_DYSYMTAB", "-", "-"});
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DynamicSymbolTable::Init(MachHeaderPtr mh){
    mh_ = mh;

    moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    if(!seg)
        return;

    // Indirect Symbols
    if(seg->ExistIndirectSymbols()) {
        auto p = std::make_shared<DynamicSymbolTableChild_IndirectSymbols>();
        p->Init(mh_);
        p->set_name("Indirect Symbols");
        children_.push_back(p);
    }
    if(seg->ExistExternalRelocations() || seg->ExistLocalRelocations()) {
        auto p = std::make_shared<DynamicSymbolTableChild_Relocations>();
        p->Init(mh_);
        p->set_name("Relocations");
        children_.push_back(p);
    }
}


void DynamicSymbolTable::InitViewDatas(){
    using namespace moex::util;

    moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    if(!seg)
        return;

    auto bin = seg->GetDataRange();
    if(std::get<0>(bin)){
        auto b = CreateBinaryView();
        b->offset = (char*)mh_->header_start() + std::get<1>(bin);
        b->size = std::get<2>(bin);
        b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
    }

}

void DynamicSymbolTable::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & item : children_){
        callback(item.get());
    }
}

MOEX_NAMESPACE_END
