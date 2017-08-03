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
        TableViewDataPtr  t = std::make_shared<TableViewData>();
        t->AddRow("","","Number of commands",AsString(cmds_.size()));
        AddViewData(t);
    }
    // Binary
    {
        BinaryViewDataPtr b = std::make_shared<BinaryViewData>();
        b->offset = (char*)mh_->header_start() + mh_->DATA_SIZE();
        b->size = mh_->data_ptr()->sizeofcmds;
        AddViewData(b);
    }
}

MOEX_NAMESPACE_END