//
// Created by everettjf on 2017/7/12.
//

#include "MachSection.h"

MOEX_NAMESPACE_BEGIN

void MachSection::Init(section *offset,NodeContextPtr & ctx){
    sect_.Init(offset);
    section_ = std::make_shared<MachSectionInternal>();
    section_->Init(offset,ctx);
}

void MachSection::Init(section_64 *offset,NodeContextPtr & ctx){
    sect_.Init(offset);
    section64_ = std::make_shared<MachSection64Internal>();
    section64_->Init(offset,ctx);
}

uint64_t MachSection::GetRAW(const void *addr){
    if(Is64())
        return (uint64_t)addr - (uint64_t)section64_->ctx()->file_start;
    else
        return (uint64_t)addr - (uint64_t)section_->ctx()->file_start;
}


MOEX_NAMESPACE_END
