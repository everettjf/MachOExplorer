//
// Created by everettjf on 2017/7/10.
//

#include "machheader.h"


MOEX_NAMESPACE_BEGIN

void MachHeader::Init(void *offset,NodeContextPtr&ctx) {
    magic_.Parse(offset);

    if(magic_.Is64()){
        is64_ = true;
        mh64_ = std::make_shared<MachHeader64Internal>();
        mh64_->Init(offset,ctx);
    }else{
        is64_ = false;
        mh_ = std::make_shared<MachHeaderInternal>();
        mh_->Init(offset,ctx);
    }

    Parse(offset,ctx);
}

void MachHeader::Parse(void *offset,NodeContextPtr& ctx) {
    int cur_datasize = 0;
    if(is64_){
        header_ = reinterpret_cast<mach_header*>(mh64_->data_ptr());
        cur_datasize = mh64_->DATA_SIZE();
    }else{
        header_ = reinterpret_cast<mach_header*>(mh_->data_ptr());
        cur_datasize = mh_->DATA_SIZE();
    }

    // commands
    const uint32_t cmd_count = header_->ncmds;
    const uint32_t sizeofcmds = header_->sizeofcmds;

    uint32_t index = 0;
    load_command *first_cmd = reinterpret_cast<load_command*>((char*)offset + cur_datasize);
    load_command *cur_cmd = first_cmd;
    for(uint32_t index = 0; index < cmd_count; ++index){
        // current
        LoadCommandPtr cmd = LoadCommandFactory::Create(cur_cmd,ctx,this);
        loadcmds_.push_back(cmd);

        // next
        cur_cmd = reinterpret_cast<load_command*>((char*)cur_cmd + cur_cmd->cmdsize);
    }
}

std::string MachHeader::GetArch(){
    return hp::GetArchStringFromCpuType(this->data_ptr()->cputype);
}

MOEX_NAMESPACE_END