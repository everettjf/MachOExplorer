//
// Created by everettjf on 2017/7/21.
//
#include "loadcommand_segment.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SEGMENT::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
        section * cur = reinterpret_cast<section*>((char*)offset_ + data_size_cmd + idx * sizeof(section));

        MachSectionPtr section = std::make_shared<MachSection>();
        section->Init(cur,ctx);
        sections_.push_back(section);
    }
}

std::string LoadCommand_LC_SEGMENT::GetShortCharacteristicDescription(){
    return segment_name();
}



void LoadCommand_LC_SEGMENT_64::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
        section * cur = reinterpret_cast<section*>((char*)offset_ + data_size_cmd + idx * sizeof(section_64));

        MachSection64Ptr section = std::make_shared<MachSection64>();
        section->Init(cur,ctx);
        sections_.push_back(section);
    }
}

std::string LoadCommand_LC_SEGMENT_64::GetShortCharacteristicDescription(){
    return segment_name();
}

MOEX_NAMESPACE_END

