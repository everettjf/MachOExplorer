//
// Created by everettjf on 2017/7/24.
//

#include "CodeSignatureViewNode.h"

MOEX_NAMESPACE_BEGIN


void CodeSignatureViewNode::InitViewDatas() {
    using namespace moex::util;

    moex::LoadCommand_LINKEDIT_DATA *seg=nullptr;
    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_CODE_SIGNATURE) {
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
}


MOEX_NAMESPACE_END
