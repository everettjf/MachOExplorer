//
// Created by everettjf on 2017/8/2.
//

#include "FunctionStartsViewNode.h"

MOEX_NAMESPACE_BEGIN




void FunctionStartsViewNode::InitViewDatas()
{
    using namespace moex::util;
    auto seg= mh_->FindLoadCommand<moex::LoadCommand_LC_FUNCTION_STARTS>({LC_FUNCTION_STARTS});
    if(!seg)
        return;

    // Functions
    auto t = CreateTableView(mh_.get());
    auto address = mh_->GetBaseAddress();
    for(auto & func : seg->GetFunctions()){
        address += func.data; // todo : why?

        t->AddRow((void*)func.offset,
                  (uint64_t )func.occupy_size,
                  "uleb128",
                  AsShortHexString(address));
    }

    auto b = CreateBinaryView();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
}

MOEX_NAMESPACE_END
