//
// Created by everettjf on 2017/7/21.
//
#include "LoadCommand_SEGMENT.h"
#include "../MachHeader.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SEGMENT::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    // Section structs live inside this load command; bound nsects to what
    // cmdsize (already validated against the file) can actually hold so a
    // crafted nsects cannot create sections that point past the command/file.
    uint64_t avail = cmd_->cmdsize > data_size_cmd ? (cmd_->cmdsize - data_size_cmd) : 0;
    uint64_t count = avail / sizeof(qv_section);
    if(count > cmd_->nsects) count = cmd_->nsects;
    for(uint64_t idx = 0; idx < count; ++idx){
        qv_section * cur = reinterpret_cast<qv_section*>((char*)offset_ + data_size_cmd + idx * sizeof(qv_section));

        MachSectionPtr section = std::make_shared<MachSection>();
        section->set_header(header_);
        section->Init(cur,ctx);
        sections_.push_back(section);
    }
}

std::string LoadCommand_LC_SEGMENT::GetShortCharacteristicDescription(){
    return segment_name();
}
std::vector<std::tuple<qv_vm_prot_t,std::string>> LoadCommand_LC_SEGMENT::GetMaxProts(){
    return util::ParseProts(cmd_->maxprot);
}
std::vector<std::tuple<qv_vm_prot_t,std::string>> LoadCommand_LC_SEGMENT::GetInitProts(){
    return util::ParseProts(cmd_->initprot);
}



void LoadCommand_LC_SEGMENT_64::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    // See LoadCommand_LC_SEGMENT::Init: bound nsects to what cmdsize can hold.
    uint64_t avail = cmd_->cmdsize > data_size_cmd ? (cmd_->cmdsize - data_size_cmd) : 0;
    uint64_t count = avail / sizeof(qv_section_64);
    if(count > cmd_->nsects) count = cmd_->nsects;
    for(uint64_t idx = 0; idx < count; ++idx){
        qv_section_64 * cur = reinterpret_cast<qv_section_64*>((char*)offset_ + data_size_cmd + idx * sizeof(qv_section_64));
        MachSectionPtr section = std::make_shared<MachSection>();
        section->set_header(header_);
        section->Init(cur,ctx);
        sections_.push_back(section);
    }
}

std::string LoadCommand_LC_SEGMENT_64::GetShortCharacteristicDescription(){
    return segment_name();
}
std::vector<std::tuple<qv_vm_prot_t,std::string>> LoadCommand_LC_SEGMENT_64::GetMaxProts(){
    return util::ParseProts(cmd_->maxprot);
}
std::vector<std::tuple<qv_vm_prot_t,std::string>> LoadCommand_LC_SEGMENT_64::GetInitProts(){
    return util::ParseProts(cmd_->initprot);
}

MOEX_NAMESPACE_END

