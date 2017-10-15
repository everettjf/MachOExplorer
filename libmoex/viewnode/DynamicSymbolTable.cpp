//
// Created by everettjf on 2017/7/24.
//

#include "DynamicSymbolTable.h"

MOEX_NAMESPACE_BEGIN




void DynamicSymbolTable::Init(MachHeaderPtr mh){
    mh_ = mh;
}

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
    {
        auto x = CreateTableViewDataPtr("Indirect Symbols");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // Modules64
    {
        auto x = CreateTableViewDataPtr("Modules");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // Table of Contents
    {
        auto x = CreateTableViewDataPtr("Table Of Contents");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // External References
    {
        auto x = CreateTableViewDataPtr("External References");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // External Relocations
    {
        auto x = CreateTableViewDataPtr("External Relocations");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // Local RElo Table
    {
        auto x = CreateTableViewDataPtr("Local Relocation Table");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

}


MOEX_NAMESPACE_END
