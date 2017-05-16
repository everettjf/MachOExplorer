//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MACHHEADER_H
#define MACHHEADER_H

#include "node.h"
#include "loadcommand_factory.h"

MOEX_NAMESPACE_BEGIN



template <typename T>
class MachHeaderImpl : public NodeData<T>{
protected:
    mach_header * header_; // points to mach_header and mach_header_64
    std::vector<LoadCommandPtr> loadcmds_;

    static constexpr bool is64 = std::is_same<T,mach_header_64>::value;

protected:
    // should called after swap
    void Parse(){
        header_ = reinterpret_cast<mach_header*>(NodeData<T>::offset_);

        // commands
        const uint32_t cmd_count = header_->ncmds;
        const uint32_t sizeofcmds = header_->sizeofcmds;

        uint32_t index = 0;
        load_command *first_cmd = reinterpret_cast<load_command*>((char*)NodeData<T>::offset_ + NodeData<T>::DATA_SIZE());
        load_command *cur_cmd = first_cmd;
        for(uint32_t index = 0; index < cmd_count; ++index){
            // current
            LoadCommandPtr cmd = LoadCommandFactory::Create(cur_cmd,NodeData<T>::ctx_);
            loadcmds_.push_back(cmd);

            // next
            cur_cmd = reinterpret_cast<load_command*>((char*)cur_cmd + cur_cmd->cmdsize);
        }
    }


public:
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
};



class MachHeader : public MachHeaderImpl<mach_header>{
public:
    void init(void *offset,NodeContextPtr&ctx) override {
        MachHeaderImpl::init(offset,ctx);

        if(data_.magic == MH_CIGAM) {
            swap_mach_header(&data_, NX_LittleEndian);
        }

        Parse();
    }

    std::string GetTypeName() override{ return "mach_header";}
    std::string GetDisplayName() override{
        return "mach_header";
    }

};
using MachHeaderPtr = std::shared_ptr<MachHeader>;


class MachHeader64 : public MachHeaderImpl<mach_header_64>{
public:
    void init(void *offset,NodeContextPtr&ctx) override {
        MachHeaderImpl::init(offset,ctx);

        if(data_.magic == MH_CIGAM_64){
            swap_mach_header_64(& data_,NX_LittleEndian);
        }

        Parse();
    }
    std::string GetTypeName() override{ return "mach_header_64";}
    std::string GetDisplayName() override{
        return "mach_header_64";
    }

};
using MachHeader64Ptr = std::shared_ptr<MachHeader64>;

MOEX_NAMESPACE_END

#endif // MACHHEADER_H
