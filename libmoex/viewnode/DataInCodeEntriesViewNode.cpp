//
// Created by everettjf on 2017/7/24.
//

#include "DataInCodeEntriesViewNode.h"

MOEX_NAMESPACE_BEGIN

void DataInCodeEntriesViewNode::InitViewDatas(){
    using namespace moex::util;

    moex::LoadCommand_LINKEDIT_DATA *seg=nullptr;
    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_DATA_IN_CODE) {
            seg = static_cast<moex::LoadCommand_LINKEDIT_DATA*>(cmd.get());
            break;
        }
    }
    if(!seg)
        return;

    auto b = CreateBinaryViewDataPtr();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    AddViewData(b);


    // Dices

}

MOEX_NAMESPACE_END
