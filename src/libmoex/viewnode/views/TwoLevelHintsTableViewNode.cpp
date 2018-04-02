//
// Created by everettjf on 03/11/2017.
//

#include "TwoLevelHintsTableViewNode.h"

MOEX_NAMESPACE_BEGIN


void TwoLevelHintsTableViewNode::InitViewDatas() {
    using namespace moex::util;

    auto t = CreateTableView();


    auto seg=mh_->FindLoadCommand<moex::LoadCommand_LC_TWOLEVEL_HINTS>({LC_TWOLEVEL_HINTS});
    if(!seg)
        return;

    auto b = CreateBinaryView();
    b->offset = (char*)mh_->header_start() + seg->cmd()->offset;
    b->size = seg->cmd()->nhints * sizeof(uint32_t);
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
}


MOEX_NAMESPACE_END