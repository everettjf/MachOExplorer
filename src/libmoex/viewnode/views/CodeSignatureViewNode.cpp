//
// Created by everettjf on 2017/7/24.
//

#include "CodeSignatureViewNode.h"

MOEX_NAMESPACE_BEGIN


void CodeSignatureViewNode::InitViewDatas() {
    using namespace moex::util;

    auto t = CreateTableView();

    auto seg=mh_->FindLoadCommand<moex::LoadCommand_LC_CODE_SIGNATURE>({LC_CODE_SIGNATURE});
    if(!seg)
        return;

    auto b = CreateBinaryView();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
}


MOEX_NAMESPACE_END
