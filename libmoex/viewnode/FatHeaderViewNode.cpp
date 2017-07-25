//
// Created by everettjf on 2017/7/23.
//

#include "FatHeaderViewNode.h"

MOEX_NAMESPACE_BEGIN
void FatHeaderViewNode::Init(FatHeaderPtr d){
    d_ = d;
    for(auto & f : d_->archs()){
        MachHeaderViewNodePtr node = std::make_shared<MachHeaderViewNode>();
        node->Init(f->mh());
        headers_.push_back(node);
    }
}

void FatHeaderViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & h : headers_){
        callback(h.get());
    }
}


std::vector<ViewData*> FatHeaderViewNode::GetViewDatas(){
    // Table
    if(vd_table_.get() == nullptr){
        vd_table_ = std::make_shared<TableViewData>();
        vd_table_->SetHeaders({"Offset","Data","Description","Value"});


        vd_table_->AddRow({
                "hello",
                "hello",
                "hello",
                "hello",
                          });
    }

    if(vd_binary_.get() == nullptr){

    }

    return {vd_table_.get()};
}

MOEX_NAMESPACE_END
