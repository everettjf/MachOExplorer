//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MACHHEADER_H
#define MACHHEADER_H

#include "Node.h"
#include "LoadCommand.h"
#include "Magic.h"

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


// rva - vmaddr,vmsize
using SegmentInfoMap = std::map<uint64_t,std::pair<uint64_t,uint64_t>>;

class MachHeader : public Node{
private:
    bool is64_;
    mach_header * header_; // points to mach_header and mach_header_64 (swapped)
    std::vector<LoadCommandPtr> loadcmds_;

    MachHeaderInternalPtr mh_;
    MachHeader64InternalPtr mh64_;
    Magic magic_;
    NodeContextPtr ctx_;
    void *header_start_;

    SegmentInfoMap segment_info_;
private:
    void Parse(void *offset,NodeContextPtr& ctx);
public:
    bool Is64()const{return is64_;}
    mach_header * data_ptr(){return header_;}
    std::vector<LoadCommandPtr> &loadcmds_ref(){return loadcmds_;}
    NodeContextPtr & ctx(){return ctx_;}
    void * header_start(){return header_start_;}

    uint64_t GetRAW(const void * addr){
        return (uint64_t)addr - (uint64_t)ctx_->file_start;
    }

    uint64_t FileOffsetToRVA(uint64_t fileoff){
        auto seg = segment_info_.upper_bound(fileoff);
        if(seg == segment_info_.begin()){
            // error
            return 0;
        }
        --seg;
        uint64_t seg_offset = seg->first;
        uint64_t seg_addr = seg->second.first;
        return fileoff - seg_offset + seg_addr;
    }
    std::string FindSymbolAtRVA(uint64_t rva){
        // extend external symbols represented in 32bit to 64bit
        if ((int32_t)rva < 0)
        {
            rva |= 0xffffffff00000000LL;
        }
        return boost::str(boost::format("0x%X")%rva);
    }
    std::string FileOffsetToSymbol(uint64_t fileoff){
        return FindSymbolAtRVA(FileOffsetToRVA(fileoff));
    }

    std::size_t DATA_SIZE(){return is64_?mh64_->DATA_SIZE() : mh_->DATA_SIZE();}

    void Init(void *offset,NodeContextPtr&ctx);

    MachHeaderInternalPtr & mh(){return mh_;}
    MachHeader64InternalPtr & mh64(){return mh64_;}

    std::string GetArch();
    std::string GetFileTypeString();
    std::vector<std::tuple<uint32_t,std::string>> GetFlagsArray();
    std::string GetMagicString();

    std::string GetCpuTypeString();
    std::string GetCpuSubTypeString();
    std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> GetCpuSubTypeArray();

    const SegmentInfoMap &segment_info()const {return segment_info_;}
    void AddSegmentInfo(uint32_t fileoff,uint64_t vmaddr,uint64_t vmsize){
        segment_info_[fileoff + (uint64_t)header_start_] = std::make_pair(vmaddr,vmsize);
    }
};
using MachHeaderPtr = std::shared_ptr<MachHeader>;




MOEX_NAMESPACE_END

#endif // MACHHEADER_H
