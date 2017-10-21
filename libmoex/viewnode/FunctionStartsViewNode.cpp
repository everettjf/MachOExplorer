//
// Created by everettjf on 2017/8/2.
//

#include "FunctionStartsViewNode.h"

MOEX_NAMESPACE_BEGIN




void FunctionStartsViewNode::InitViewDatas()
{
    using namespace moex::util;

    moex::LoadCommand_LC_FUNCTION_STARTS *seg=nullptr;
    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_FUNCTION_STARTS) {
            seg = static_cast<moex::LoadCommand_LC_FUNCTION_STARTS*>(cmd.get());
            break;
        }
    }
    if(!seg)
        return;

    // Functions
    auto t = CreateTableViewDataPtr();
    auto address = 0;//mh_->base_addr(); // todo
    for(auto & func : seg->GetFunctions()){
        address += func.data; // todo : why?

        t->AddRow(mh_->GetRAW((const void *)func.offset),
                  (void*)func.offset,func.occupy_size,
                  "uleb128",
                  AsShortHexString(address));
    }
    AddViewData(t);

    auto b = CreateBinaryViewDataPtr();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
    AddViewData(b);
}

MOEX_NAMESPACE_END
