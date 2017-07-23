//
// Created by everettjf on 2017/7/23.
//

#include "LoadCommandsViewNode.h"

MOEX_NAMESPACE_BEGIN


void LoadCommandsViewNode::Init(MachHeaderPtr mh){
    mh_ = mh;

}

void LoadCommandsViewNode::ForEachChild(std::function<void(ViewNode*)> callback){

}

MOEX_NAMESPACE_END