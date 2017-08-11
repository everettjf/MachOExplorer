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

    auto b = CreateBinaryViewDataPtr();
    b->offset = (char*)mh_->header_start() + seg->cmd()->dataoff;
    b->size = seg->cmd()->datasize;
    AddViewData(b);


    // Functions
    auto t = CreateTableViewDataPtr();
    for(auto & func : seg->GetFunctions()){
        t->AddRow(mh_->GetRAW((const void *)func.offset),func.data,"uleb128",mh_->FileOffsetToSymbol(func.data));
    }
    AddViewData(t);
}

MOEX_NAMESPACE_END
