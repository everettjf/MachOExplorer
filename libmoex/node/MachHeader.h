//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MOEXMACHHEADER_H
#define MOEXMACHHEADER_H

#include "Node.h"
#include "LoadCommand.h"
#include "Magic.h"
#include "MachSection.h"


MOEX_NAMESPACE_BEGIN

// Wrapper for mach_header
// Derived from NodeData because some data should be swapped
class MachHeaderInternal : public NodeData<mach_header>{
public:
    void Init(void *offset,NodeContextPtr&ctx) override;
};
using MachHeaderInternalPtr = std::shared_ptr<MachHeaderInternal>;

// Wrapper for mach_header
// Derived from NodeData because some data should be swapped
class MachHeader64Internal : public NodeData<mach_header_64>{
public:
    void Init(void *offset,NodeContextPtr&ctx) override;
};
using MachHeader64InternalPtr = std::shared_ptr<MachHeader64Internal>;


// Cache info when parsing
struct ParsingCacheInfo{
    uint64_t base_addr=0LL;
};

// Wrapper class
class MachHeader : public Node{
private:
    // Whether it is 64bit
    bool is64_;

    // Pointing to mach_header and mach_header_64 (swapped)
    mach_header * header_;

    // Load commands
    std::vector<LoadCommandPtr> loadcmds_;

    MachHeaderInternalPtr mh_;
    MachHeader64InternalPtr mh64_;

    Magic magic_;
    NodeContextPtr ctx_;

    // Offset of the beginning header
    void *header_start_;

    ParsingCacheInfo cache_;
private:
    // Internal parse
    void Parse(void *offset,NodeContextPtr& ctx);

public:
    // Whether it is 64bit
    bool Is64()const{return is64_;}

    // Pointing to swapped header
    mach_header * data_ptr(){return header_;}

    // Get all load commands
    std::vector<LoadCommandPtr> &loadcmds_ref(){return loadcmds_;}

    // Get context
    NodeContextPtr & ctx(){return ctx_;}

    // Getter for the offset of the begging header
    void * header_start(){return header_start_;}

    ParsingCacheInfo & cache(){return cache_;}

    // Get offset from the beginning of the file
    uint64_t GetRAW(const void * addr);

    // Get current header size in bytes
    std::size_t DATA_SIZE();

    // Init function from the begining of MachO header
    void Init(void *offset,NodeContextPtr&ctx);

    // Getter mach header
    MachHeaderInternalPtr & mh(){return mh_;}

    // Getter mach header 64
    MachHeader64InternalPtr & mh64(){return mh64_;}

    // Getter arch in string
    std::string GetArch();

    // Get file type in string
    std::string GetFileTypeString();

    // Get flags in string array
    std::vector<std::tuple<uint32_t,std::string>> GetFlagsArray();

    // Get magic in string
    std::string GetMagicString();

    // Get cpu type in string
    std::string GetCpuTypeString();

    // Get cpu sub type in string
    std::string GetCpuSubTypeString();

    // Get cpu sub type detailed array
    std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> GetCpuSubTypeArray();
};
using MachHeaderPtr = std::shared_ptr<MachHeader>;

MOEX_NAMESPACE_END

#endif // MOEXMACHHEADER_H
