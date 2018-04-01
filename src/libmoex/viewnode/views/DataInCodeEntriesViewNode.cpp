//
// Created by everettjf on 2017/7/24.
//

#include "DataInCodeEntriesViewNode.h"

MOEX_NAMESPACE_BEGIN

void DataInCodeEntriesViewNode::InitViewDatas(){
    using namespace moex::util;

    auto seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DATA_IN_CODE>({LC_DATA_IN_CODE});
    if(!seg)
        return;

    // Dices
    auto t = CreateTableView(seg);
    for(auto & dice : seg->GetDices()){
        t->AddRow(dice->offset()->offset,"Offset",AsShortHexString(dice->offset()->offset));
        t->AddRow(dice->offset()->length,"Length",AsShortHexString(dice->offset()->length));
        t->AddRow(dice->offset()->kind,"Kind",dice->GetKindString());
        t->AddSeparator();
    }

    auto b = CreateBinaryView();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->header_start();
}

MOEX_NAMESPACE_END
