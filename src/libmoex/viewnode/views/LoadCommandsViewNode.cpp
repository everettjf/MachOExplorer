//
// Created by everettjf on 2017/7/23.
//

#include "LoadCommandsViewNode.h"

MOEX_NAMESPACE_BEGIN

void LoadCommandsViewNode::Init(MachHeaderPtr mh){
    mh_ = mh;

    for(auto & cmd : mh_->loadcmds_ref()){
        LoadCommandViewNodePtr o = LoadCommandViewNodeFactory::Create(cmd);
        cmds_.push_back(o);
    }
}

void LoadCommandsViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & cmd : cmds_){
        callback(cmd.get());
    }
}
void LoadCommandsViewNode::InitViewDatas(){

    using namespace moex::util;

    // Table
    {
        auto t = CreateTableView();
        t->SetHeaders({"Information"});
        t->SetWidths({300});
        t->AddRow({fmt::format("Number of commands : {}", cmds_.size())});
    }
    // Binary
    {
        auto b = CreateBinaryView();
        b->offset = (char*)mh_->header_start() + mh_->DATA_SIZE();
        b->size = mh_->data_ptr()->sizeofcmds;
    }
}

MOEX_NAMESPACE_END