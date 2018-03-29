//
// Created by everettjf on 03/11/2017.
//

#include "SegmentSplitInfoViewNode.h"

MOEX_NAMESPACE_BEGIN

void SegmentSplitInfoViewNode::InitViewDatas() {
    using namespace moex::util;

    auto seg=mh_->FindLoadCommand<moex::LoadCommand_LC_SEGMENT_SPLIT_INFO>({LC_SEGMENT_SPLIT_INFO});
    if(!seg)
        return;

    auto b = CreateBinaryViewDataPtr();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
    AddViewData(b);
}


MOEX_NAMESPACE_END