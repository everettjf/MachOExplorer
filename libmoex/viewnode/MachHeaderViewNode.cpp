//
// Created by everettjf on 2017/7/23.
//

#include "MachHeaderViewNode.h"
#include <boost/algorithm/string.hpp>

MOEX_NAMESPACE_BEGIN

void MachHeaderViewNode::Init(MachHeaderPtr d){
    d_ = d;

    load_commands_ = std::make_shared<LoadCommandsViewNode>();
    load_commands_->Init(d_);

    sections_ = std::make_shared<SectionsViewNode>();
    sections_->Init(d_);
}

std::string MachHeaderViewNode::GetDisplayName() {
    std::string arch = d_->GetArch();
    boost::to_upper(arch);
    return "Mach Header (" + arch +")";
}

void MachHeaderViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    callback(load_commands_.get());

    callback(sections_.get());
}

void MachHeaderViewNode::InitViewDatas(){

}

MOEX_NAMESPACE_END