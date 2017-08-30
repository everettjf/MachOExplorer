//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MACHHEADER_H
#define MACHHEADER_H

#include "Node.h"
#include "LoadCommand.h"
#include "Magic.h"
#include "MachSection.h"


MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_SYMTAB;

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

    // Helper Datas
    SegmentInfoMap segment_info_;

    uint64_t base_addr_=0LL;
    uint64_t seg1addr_ = (uint64_t)-1LL;
    uint64_t segs_read_write_addr_ = (uint64_t)-1LL;

    bool exist_function_starts_ = false;
    bool exist_data_in_code_entries_ = false;

    std::vector<MachSectionWeakPtr> sections_;

    LoadCommand_LC_SYMTAB *symtab_ = nullptr;

    std::unordered_map<uint64_t,std::string> symbol_names_; // <address, symbol name>

private:
    void Parse(void *offset,NodeContextPtr& ctx);
public:
    bool Is64()const{return is64_;}
    mach_header * data_ptr(){return header_;}
    std::vector<LoadCommandPtr> &loadcmds_ref(){return loadcmds_;}
    NodeContextPtr & ctx(){return ctx_;}
    void * header_start(){return header_start_;}

    uint64_t GetRAW(const void * addr);

    uint64_t FileOffsetToRVA(uint64_t fileoff);
    std::string FindSymbolAtRVA(uint64_t rva);
    std::string FileOffsetToSymbol(uint64_t fileoff);

    std::size_t DATA_SIZE();

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
    void AddSegmentInfo(uint32_t fileoff,uint64_t vmaddr,uint64_t vmsize);

    uint64_t base_addr() const{return base_addr_;}
    uint64_t seg1addr()const{return seg1addr_;}
    uint64_t segs_read_write_addr()const{return segs_read_write_addr_;}
    void set_base_addr(uint64_t base_addr){base_addr_ = base_addr;}
    void set_seg1addr(uint64_t seg1addr){seg1addr_ = seg1addr;}
    void set_segs_read_write_addr(uint64_t segs_read_write_addr){segs_read_write_addr_ = segs_read_write_addr;}

    bool exist_function_starts()const{return exist_function_starts_;}
    bool exist_data_in_code_entries()const{return exist_data_in_code_entries_;}

    std::vector<MachSectionWeakPtr> & sections(){return sections_;}
    void AddSection(MachSectionPtr section){
        MachSectionWeakPtr s(section);
        sections_.push_back(s);
    }

    LoadCommand_LC_SYMTAB *symtab(){return symtab_;}

    void AddSymbolName(uint64_t rva,const std::string & name){
        symbol_names_[rva] = name;
    }
    void AddSymbolNameByFileOff(uint64_t fileoffset,const std::string & name){
        symbol_names_[FileOffsetToRVA(fileoffset)] = name;
    }
};
using MachHeaderPtr = std::shared_ptr<MachHeader>;




MOEX_NAMESPACE_END

#endif // MACHHEADER_H
