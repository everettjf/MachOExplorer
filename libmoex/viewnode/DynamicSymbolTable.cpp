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

    moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    if(!seg)
        return;

    auto bin = seg->GetDataRange();
    if(std::get<0>(bin)){
        auto b = CreateBinaryViewDataPtr();
        b->offset = (char*)mh_->header_start() + std::get<1>(bin);
        b->size = std::get<2>(bin);
        b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
        AddViewData(b);
    }

    // Indirect Symbols
    if(seg->ExistIndirectSymbols()) {
        auto x = CreateTableViewDataPtr("Indirect Symbols");
        seg->ForEachIndirectSymbols([&](uint32_t * indirect_index){
            x->AddRow(mh_->GetRAW(indirect_index),*indirect_index,"Indirect Index",AsShortHexString(*indirect_index));
        });
        AddViewData(x);
    }

    // Modules64
    if(seg->cmd()->nmodtab > 0)
    {
        auto x = CreateTableViewDataPtr("Modules");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // Table of Contents
    if(seg->cmd()->ntoc > 0)
    {
        auto x = CreateTableViewDataPtr("Table Of Contents");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // External References
    if(seg->cmd()->nextrefsyms > 0)
    {
        auto x = CreateTableViewDataPtr("External References");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // External Relocations
    if(seg->cmd()->nextrel > 0)
    {
        auto x = CreateTableViewDataPtr("External Relocations");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

    // Local RElo Table
    if(seg->cmd()->nlocrel > 0)
    {
        auto x = CreateTableViewDataPtr("Local Relocation Table");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }

}


MOEX_NAMESPACE_END
