//
// Created by everettjf on 2017/7/21.
//
#include "LoadCommand_SEGMENT.h"
#include "../MachHeader.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SEGMENT::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
        section * cur = reinterpret_cast<section*>((char*)offset_ + data_size_cmd + idx * sizeof(section));

        MachSectionPtr section = std::make_shared<MachSection>();
        section->set_header(header_);
        section->Init(cur,ctx);
        sections_.push_back(section);
    }
}

std::string LoadCommand_LC_SEGMENT::GetShortCharacteristicDescription(){
    return segment_name();
}
std::vector<std::tuple<vm_prot_t,std::string>> LoadCommand_LC_SEGMENT::GetMaxProts(){
    return util::ParseProts(cmd_->maxprot);
}
std::vector<std::tuple<vm_prot_t,std::string>> LoadCommand_LC_SEGMENT::GetInitProts(){
    return util::ParseProts(cmd_->initprot);
}



void LoadCommand_LC_SEGMENT_64::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
        section_64 * cur = reinterpret_cast<section_64*>((char*)offset_ + data_size_cmd + idx * sizeof(section_64));
        MachSectionPtr section = std::make_shared<MachSection>();
        section->set_header(header_);
        section->Init(cur,ctx);
        sections_.push_back(section);
    }
}

std::string LoadCommand_LC_SEGMENT_64::GetShortCharacteristicDescription(){
    return segment_name();
}
std::vector<std::tuple<vm_prot_t,std::string>> LoadCommand_LC_SEGMENT_64::GetMaxProts(){
    return util::ParseProts(cmd_->maxprot);
}
std::vector<std::tuple<vm_prot_t,std::string>> LoadCommand_LC_SEGMENT_64::GetInitProts(){
    return util::ParseProts(cmd_->initprot);
}

MOEX_NAMESPACE_END

