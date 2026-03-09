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
        const qv_fat_header * h = d_->offset();

        t->AddRow(h->magic,"Magic Number",d_->GetMagicString());
        t->AddRow(h->nfat_arch,"Number of Architecture",AsString(d_->data().nfat_arch));

        t->AddSeparator();

        for(auto & arch : d_->archs()){
            if (arch->IsFat64()) {
                const qv_fat_arch_64 *a = arch->offset64();
                t->AddRow(a->cputype,"CPU Type",arch->GetCpuTypeString());
                t->AddRow(a->cpusubtype,"CPU SubType",arch->GetCpuSubTypeString());
                t->AddRow(a->offset,"Offset",AsShortHexString(arch->offset_value()));
                t->AddRow(a->size,"Size",AsShortHexString(arch->size_value()));
                const uint32_t align = arch->align_value();
                const uint64_t align_value = (align < 63) ? (1ULL << align) : 0;
                t->AddRow(a->align,"Align",align_value == 0 ? AsString(align) : AsString(align_value));
            } else {
                const qv_fat_arch *a = arch->offset32();
                t->AddRow(a->cputype,"CPU Type",arch->GetCpuTypeString());
                t->AddRow(a->cpusubtype,"CPU SubType",arch->GetCpuSubTypeString());
                t->AddRow(a->offset,"Offset",AsShortHexString(arch->offset_value()));
                t->AddRow(a->size,"Size",AsShortHexString(arch->size_value()));
                const uint32_t align = arch->align_value();
                const uint64_t align_value = (align < 63) ? (1ULL << align) : 0;
                t->AddRow(a->align,"Align",align_value == 0 ? AsString(align) : AsString(align_value));
            }

            t->AddSeparator();
        }
    }

    // Binary
    {
        auto b = CreateBinaryView();
        b->offset = (char*)d_->offset();
        b->size = d_->DATA_SIZE();
        for(auto & arch: d_->archs()){
            b->size += arch->EntrySize();
        }
        b->start_value = (uint64_t)b->offset - (uint64_t)d_->ctx()->file_start;
    }
}

MOEX_NAMESPACE_END
