//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MACHHEADER_H
#define MACHHEADER_H

#include "node.h"
#include "loadcommand.h"
#include "magic.h"

MOEX_NAMESPACE_BEGIN

class MachHeaderInternal : public NodeData<mach_header>{
public:
    void Init(void *offset,NodeContextPtr&ctx) override {
        NodeData::Init(offset,ctx);
        if(data_.magic == MH_CIGAM) {
            swap_mach_header(&data_, NX_LittleEndian);
        }
    }
};
using MachHeaderInternalPtr = std::shared_ptr<MachHeaderInternal>;

class MachHeader64Internal : public NodeData<mach_header_64>{
public:
    void Init(void *offset,NodeContextPtr&ctx) override {
        NodeData::Init(offset,ctx);
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
    NodeContextPtr ctx_;
    void *header_start_;

private:
    void Parse(void *offset,NodeContextPtr& ctx);
public:
    bool is64()const{return is64_;}
    mach_header * data_ptr(){return header_;}
    std::vector<LoadCommandPtr> &loadcmds_ref(){return loadcmds_;}
    NodeContextPtr & ctx(){return ctx_;}
    void * header_start(){return header_start_;}

    void Init(void *offset,NodeContextPtr&ctx);

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

    std::string GetArch();
};
using MachHeaderPtr = std::shared_ptr<MachHeader>;

MOEX_NAMESPACE_END

#endif // MACHHEADER_H
