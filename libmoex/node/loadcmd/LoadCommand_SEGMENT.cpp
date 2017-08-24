//
// Created by everettjf on 2017/7/21.
//
#include "LoadCommand_SEGMENT.h"
#include "../MachHeader.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SEGMENT::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    header_->AddSegmentInfo(cmd_->fileoff,cmd_->vmaddr,cmd_->vmsize);

    for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
        section * cur = reinterpret_cast<section*>((char*)offset_ + data_size_cmd + idx * sizeof(section));

        MachSectionPtr section = std::make_shared<MachSection>();
        section->set_header(header_);
        section->Init(cur,ctx);
        sections_.push_back(section);

        header_->AddSection(section);
    }

    if(cmd_->fileoff == 0 && cmd_->filesize != 0){
        header_->set_base_addr(cmd_->vmaddr);
    }

    if(cmd_->vmaddr < header_->seg1addr()){
        header_->set_seg1addr(cmd_->vmaddr);
    }

    // Pickup the address of the first read-write segment for MH_SPLIT_SEGS images.
    if((cmd_->initprot & VM_PROT_WRITE) == VM_PROT_WRITE
            && cmd_->vmaddr < header_->segs_read_write_addr()){
        header_->set_segs_read_write_addr(cmd_->vmaddr);
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

    header_->AddSegmentInfo(cmd_->fileoff,cmd_->vmaddr,cmd_->vmsize);

    for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
        section_64 * cur = reinterpret_cast<section_64*>((char*)offset_ + data_size_cmd + idx * sizeof(section_64));
        MachSectionPtr section = std::make_shared<MachSection>();
        section->set_header(header_);
        section->Init(cur,ctx);
        sections_.push_back(section);

        header_->AddSection(section);
    }

    if(cmd_->fileoff == 0 && cmd_->filesize != 0){
        header_->set_base_addr(cmd_->vmaddr);
    }

    if(cmd_->vmaddr < header_->seg1addr()){
        header_->set_seg1addr(cmd_->vmaddr);
    }

    // Pickup the address of the first read-write segment for MH_SPLIT_SEGS images.
    if((cmd_->initprot & VM_PROT_WRITE) == VM_PROT_WRITE
            && cmd_->vmaddr < header_->segs_read_write_addr()){
        header_->set_segs_read_write_addr(cmd_->vmaddr);
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

