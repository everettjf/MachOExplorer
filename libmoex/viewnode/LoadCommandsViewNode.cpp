//
// Created by everettjf on 2017/7/23.
//

#include "LoadCommandsViewNode.h"

MOEX_NAMESPACE_BEGIN

void LoadCommandViewNode::Init(LoadCommandPtr d){
    d_ = d;

}

std::string LoadCommandViewNode::GetDisplayName(){
    std::string charact = d_->GetShortCharacteristicDescription();
    if(charact.length() > 0){
        return boost::str(boost::format("%1%(%2%)")%d_->GetLoadCommandTypeString()%charact);
    }else{
        return d_->GetLoadCommandTypeString();
    }
}

// --------------------------------------------------------------
void LoadCommandsViewNode::Init(MachHeaderPtr mh){
    mh_ = mh;

    for(auto & cmd : mh_->loadcmds_ref()){
        LoadCommandViewNodePtr o = std::make_shared<LoadCommandViewNode>();
        o->Init(cmd);
        cmds_.push_back(o);
    }
}

void LoadCommandsViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & cmd : cmds_){
        callback(cmd.get());
    }
}
void LoadCommandsViewNode::InitViewDatas(){

    // Binary
    {
        BinaryViewDataPtr b = std::make_shared<BinaryViewData>();
        b->offset = (char*)mh_->header_start() + mh_->DATA_SIZE();
        b->size = mh_->data_ptr()->sizeofcmds;
        AddViewData(b);
    }
}

MOEX_NAMESPACE_END