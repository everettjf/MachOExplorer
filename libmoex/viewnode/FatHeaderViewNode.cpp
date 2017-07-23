//
// Created by everettjf on 2017/7/23.
//

#include "FatHeaderViewNode.h"

MOEX_NAMESPACE_BEGIN
FatHeaderViewNode::FatHeaderViewNode(FatHeaderPtr d):d_(d){
    for(auto & f : d_->archs()){
        MachHeaderViewNodePtr node = std::make_shared<MachHeaderViewNode>(f->mh());
        headers_.push_back(node);
    }
}

void FatHeaderViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & h : headers_){
        callback(h.get());
    }
}
MOEX_NAMESPACE_END
