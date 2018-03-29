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
    auto t = CreateTableViewDataPtr();
    for(auto & dice : seg->GetDices()){
        t->AddRow(seg->GetRAW(&(dice->offset()->offset)),dice->offset()->offset,"Offset",AsShortHexString(dice->offset()->offset));
        t->AddRow(seg->GetRAW(&(dice->offset()->length)),dice->offset()->length,"Length",AsShortHexString(dice->offset()->length));
        t->AddRow(seg->GetRAW(&(dice->offset()->kind)),dice->offset()->kind,"Kind",dice->GetKindString());
        t->AddSeparator();
    }
    SetViewData(t);

    auto b = CreateBinaryViewDataPtr();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->header_start();
    SetViewData(b);
}

MOEX_NAMESPACE_END
