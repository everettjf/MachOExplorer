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


void FatHeaderViewNode::InitViewDatas(){
    using namespace moex::util;

    // Table
    {
        TableViewDataPtr  t = std::make_shared<TableViewData>();
        const fat_header * h = d_->offset();

        t->AddRow(d_->GetRAW(&(h->magic)),h->magic,"Magic Number",d_->GetMagicString());
        t->AddRow(d_->GetRAW(&(h->nfat_arch)),h->nfat_arch,"Number of Architecture",AsString(d_->data().nfat_arch));

        t->AddSeparator();

        for(auto & arch : d_->archs()){
            const fat_arch * a = arch->offset();
            t->AddRow(d_->GetRAW(&(a->cputype)),a->cputype,"CPU Type",arch->GetCpuTypeString());
            t->AddRow(d_->GetRAW(&(a->cpusubtype)),a->cpusubtype,"CPU SubType",arch->GetCpuSubTypeString());
            t->AddRow(d_->GetRAW(&(a->offset)),a->offset,"Offset",AsString(arch->data().offset));
            t->AddRow(d_->GetRAW(&(a->size)),a->size,"Size",AsString(arch->data().size));
            t->AddRow(d_->GetRAW(&(a->align)),a->align,"Align",AsString(1 << arch->data().align)); // why 1<<

            t->AddSeparator();
        }
        AddViewData(t);
    }

    // Binary
    {
        BinaryViewDataPtr b = std::make_shared<BinaryViewData>();
        b->offset = (char*)d_->offset();
        b->size = d_->DATA_SIZE();
        AddViewData(b);
    }
}

MOEX_NAMESPACE_END
