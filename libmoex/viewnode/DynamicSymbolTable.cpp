//
// Created by everettjf on 2017/7/24.
//

#include "DynamicSymbolTable.h"

MOEX_NAMESPACE_BEGIN




void DynamicSymbolTable::InitViewDatas(){
    using namespace moex::util;

    moex::LoadCommand_LC_DYSYMTAB *seg=nullptr;
    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_DYSYMTAB) {
            seg = static_cast<moex::LoadCommand_LC_DYSYMTAB*>(cmd.get());
            break;
        }
    }
    if(!seg)
        return;

    auto bin = seg->GetDataRange();
    if(std::get<0>(bin)){
        auto b = CreateBinaryViewDataPtr();
        b->offset = (char*)mh_->header_start() + std::get<1>(bin);
        b->size = std::get<2>(bin);
        AddViewData(b);
    }

    // Indirect Symbols
    InitIndirectSymbols(seg);

    // Modules64
    // Table of Contents
    // External References
    // External Relocations
    // Local RElo Table

}

void DynamicSymbolTable::InitIndirectSymbols(moex::LoadCommand_LC_DYSYMTAB *seg){
    auto t = CreateTableViewDataPtr();
    t->set_title("Indirect Symbols");

    char * offset = (char*)mh_->header_start() + seg->cmd()->indirectsymoff;

    auto & indirect_symbols = seg->GetIndirectSymbols();

    for(uint32_t nindsym = 0; nindsym < indirect_symbols.size(); ++nindsym){
        IndirectSymbol indirect_symbol = indirect_symbols[nindsym];
        uint32_t indirect_index = indirect_symbol.data;

        // find section
        auto &sections = mh_->sections();
        MachSectionPtr findsection;
        for(auto iter = sections.rbegin(); iter != sections.rend(); iter++){
            MachSectionPtr section(*iter);
            moex_section & sect = section->sect();
            uint32_t type = sect.flags() & SECTION_TYPE;
            if((type != S_SYMBOL_STUBS
                && type != S_LAZY_SYMBOL_POINTERS
                && type != S_LAZY_DYLIB_SYMBOL_POINTERS
                && type != S_NON_LAZY_SYMBOL_POINTERS
               )|| sect.reserved1() > nindsym){
                // what is reserved1
                continue;
            }
            // found the section

            findsection = section;
        }
        if(findsection.get() == nullptr) {
            continue;
        }
        moex_section & sect = findsection->sect();

        // stub or pointer length
        uint32_t length = sect.reserved2() > 0 ? sect.reserved2() : sizeof(uint32_t);

        // indirect value location
        LoadCommand_LC_SYMTAB *symtab = mh_->symtab();
        uint64_t indirect_address = sect.addr_both() + (nindsym - sect.reserved1()) * length;

        // accumulate search info
        if((indirect_index & ( INDIRECT_SYMBOL_LOCAL | INDIRECT_SYMBOL_ABS)) == 0){
            if(indirect_index >= symtab->nlists_ref().size()){
                // error
            }

            uint32_t str_index = symtab->nlists_ref()[indirect_index]->n_strx();
            std::string symbol_name = symtab->GetStringByStrX(str_index);

            t->AddRow(mh_->GetRAW((const void*)indirect_symbol.offset),indirect_index,"Symbol",symbol_name);

        }else{
            t->AddRow(mh_->GetRAW((const void*)indirect_symbol.offset),indirect_index,"Symbol","");
            switch(indirect_index){
                case INDIRECT_SYMBOL_LOCAL:{
                    t->AddRow("","","80000000","INDIRECT_SYMBOL_LOCAL");
                    // todo
                    break;
                }
                case INDIRECT_SYMBOL_ABS:{
                    t->AddRow("","","40000000","INDIRECT_SYMBOL_ABS");
                    break;
                }
                default:{
                    t->AddRow("","","80000000","INDIRECT_SYMBOL_LOCAL");
                    t->AddRow("","","40000000","INDIRECT_SYMBOL_ABS");
                    break;
                }
            }
        }

        t->AddRow("","","Section",boost::str(boost::format("(%1%,%2%)")% sect.segment_name() % sect.section_name()));

        std::string str_ind = boost::str(boost::format("0x%X ($+%u)")
                                         % indirect_address
                                         % (indirect_address - findsection->sect().addr_both())
        );
        t->AddRow("","","Indirect Address",str_ind);
        t->AddSeparator();
    }

    AddViewData(t);
}





MOEX_NAMESPACE_END
