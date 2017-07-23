//
// Created by everettjf on 2017/7/23.
//

#include "SectionsViewNode.h"

MOEX_NAMESPACE_BEGIN


void SectionsViewNode::Init(MachHeaderPtr mh) {
    mh_ = mh;

}

void SectionsViewNode::ForEachChild(std::function<void(ViewNode*)> callback){

}

MOEX_NAMESPACE_END