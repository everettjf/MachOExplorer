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
}

MOEX_NAMESPACE_END
