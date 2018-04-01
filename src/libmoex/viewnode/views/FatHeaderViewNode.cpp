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
        auto t = CreateTableView();
        const fat_header * h = d_->offset();

        t->AddRow(h->magic,"Magic Number",d_->GetMagicString());
        t->AddRow(h->nfat_arch,"Number of Architecture",AsString(d_->data().nfat_arch));

        t->AddSeparator();

        for(auto & arch : d_->archs()){
            const fat_arch * a = arch->offset();
            t->AddRow(a->cputype,"CPU Type",arch->GetCpuTypeString());
            t->AddRow(a->cpusubtype,"CPU SubType",arch->GetCpuSubTypeString());
            t->AddRow(a->offset,"Offset",AsString(arch->data().offset));
            t->AddRow(a->size,"Size",AsString(arch->data().size));
            t->AddRow(a->align,"Align",AsString(1 << arch->data().align)); // why 1<<

            t->AddSeparator();
        }
    }

    // Binary
    {
        auto b = CreateBinaryView();
        b->offset = (char*)d_->offset();
        b->size = d_->DATA_SIZE();
        for(auto & arch: d_->archs()){
            b->size += arch->DATA_SIZE();
        }
        b->start_value = (uint64_t)b->offset - (uint64_t)d_->ctx()->file_start;
    }
}

MOEX_NAMESPACE_END
