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
    using namespace moex::util;

    // Table
    if(table_.get() == nullptr){
        table_ = std::make_shared<TableViewData>();
        const fat_header * h = d_->offset();

        table_->AddRow(d_->GetRAW(&(h->magic)),h->magic,"Magic Number",d_->GetMagicString());
        table_->AddRow(d_->GetRAW(&(h->nfat_arch)),h->nfat_arch,"Number of Architecture",AsString(d_->data().nfat_arch));

        table_->AddSeparator();

        for(auto & arch : d_->archs()){
            const fat_arch * a = arch->offset();
            table_->AddRow(d_->GetRAW(&(a->cputype)),a->cputype,"CPU Type",arch->GetCpuTypeString());
            table_->AddRow(d_->GetRAW(&(a->cpusubtype)),a->cpusubtype,"CPU SubType",arch->GetCpuSubTypeString());
            table_->AddRow(d_->GetRAW(&(a->offset)),a->offset,"Offset",AsString(arch->data().offset));
            table_->AddRow(d_->GetRAW(&(a->size)),a->size,"Size",AsString(arch->data().size));
            table_->AddRow(d_->GetRAW(&(a->align)),a->align,"Align",AsString(arch->data().align));

            table_->AddSeparator();
        }
    }

    if(binary_.get() == nullptr){

    }

    return {table_.get()};
}

MOEX_NAMESPACE_END
