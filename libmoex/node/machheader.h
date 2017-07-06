//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MACHHEADER_H
#define MACHHEADER_H

#include "node.h"
#include "loadcommand_factory.h"
#include "magic.h"

MOEX_NAMESPACE_BEGIN

class MachHeaderInternal : public NodeData<mach_header>{
public:
    void init(void *offset,NodeContextPtr&ctx) override {
        NodeData::init(offset,ctx);
        if(data_.magic == MH_CIGAM) {
            swap_mach_header(&data_, NX_LittleEndian);
        }
    }
};
using MachHeaderInternalPtr = std::shared_ptr<MachHeaderInternal>;

class MachHeader64Internal : public NodeData<mach_header_64>{
public:
    void init(void *offset,NodeContextPtr&ctx) override {
        NodeData::init(offset,ctx);
        if(data_.magic == MH_CIGAM_64){
            swap_mach_header_64(& data_,NX_LittleEndian);
        }
    }
};
using MachHeader64InternalPtr = std::shared_ptr<MachHeader64Internal>;


class MachHeader : public Node{
private:
    bool is64_;
    mach_header * header_; // points to mach_header and mach_header_64
    std::vector<LoadCommandPtr> loadcmds_;

    MachHeaderInternalPtr mh_;
    MachHeader64InternalPtr mh64_;
    Magic magic_;

private:
    void Parse(void *offset,NodeContextPtr&ctx) {
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
            LoadCommandPtr cmd = LoadCommandFactory::Create(cur_cmd,ctx);
            loadcmds_.push_back(cmd);

            // next
            cur_cmd = reinterpret_cast<load_command*>((char*)cur_cmd + cur_cmd->cmdsize);
        }
    }
public:
    bool is64()const{return is64_;}

    void init(void *offset,NodeContextPtr&ctx) {
        magic_.Parse(offset);

        if(magic_.Is64()){
            is64_ = true;
            mh64_ = std::make_shared<MachHeader64Internal>();
            mh64_->init(offset,ctx);
        }else{
            is64_ = false;
            mh_ = std::make_shared<MachHeaderInternal>();
            mh_->init(offset,ctx);
        }

        Parse(offset,ctx);
    }
    std::string GetTypeName() override {
        return is64_?"mach_header_64":"mach_header";
    }
    std::string GetDisplayName() override {
        return is64_?"mach_header_64":"mach_header";
    }
    std::string GetDescription() override{
        return boost::str(boost::format(
                "magic=%1%,cputype=%2%,subcputype=%3%,filetype=%4%,ncmds=%5%,sizeofcmds=%6%,flags=%7%")
                          % hp::GetMagicString(header_->magic)
                          % hp::GetCpuTypeString(header_->cputype)
                          % hp::GetCpuSubTypeString(header_->cpusubtype)
                          % hp::GetMachFileType(header_->filetype)
                          % header_->ncmds
                          % header_->sizeofcmds
                          % header_->flags
        );
    }
    void ForEachChild(std::function<void(Node*)> func) override{
        for(auto cmd : loadcmds_){
            func(cmd.get());
        }
    }
    NodeViewArray GetViews()override {
        return {};
    }

};
using MachHeaderPtr = std::shared_ptr<MachHeader>;

MOEX_NAMESPACE_END

#endif // MACHHEADER_H
