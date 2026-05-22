//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MOEXNODE_H
#define MOEXNODE_H

#include "Common.h"
#include "Util.h"
#include "Defines.h"

MOEX_NAMESPACE_BEGIN

// Exception class for parsing process
class NodeException : public std::exception{
private:
    std::string error_;
public:
    NodeException(const std::string & error) { error_ = error;}

#ifdef __APPLE__
    const char* what() const _NOEXCEPT override  { return error_.c_str();}
#else
    const char* what() const noexcept override { return error_.c_str();}
#endif
};

// Context for parsing process
struct NodeContext{
    // File start address ( beginning of the file )
    void *file_start;
    // File size in bytes.
    std::size_t file_size;
};
using NodeContextPtr = std::shared_ptr<NodeContext>;

// True when [addr, addr+size) lies entirely within the mapped file.
static inline bool NodeInFile(const NodeContextPtr &ctx, const void *addr, std::size_t size){
    if(!ctx || ctx->file_start == nullptr) return false;
    const char *p = static_cast<const char*>(addr);
    const char *start = static_cast<const char*>(ctx->file_start);
    const char *end = start + ctx->file_size;
    if(p < start || p > end) return false;
    return size <= static_cast<std::size_t>(end - p);
}

// Base class for each MachO element
class Node{
public:
    virtual ~Node(){}

    virtual uint64_t GetRAW(const void * addr){ return 0; }
};

// Template for every MachO element which hold the offset for the element and context
template <typename T>
class NodeOffset : public Node{
protected:
    // Offset to the MachO element
    T *offset_;
    // Context (which is global to the parsing process)
    NodeContextPtr ctx_;
public:
    // MachO element size (unit:bytes)
    static constexpr std::size_t DATA_SIZE(){return sizeof(T);}

    // Getter for offset
    T * offset(){ return offset_;}

    // Getter for context
    NodeContextPtr ctx(){return ctx_;}

    // Init function which should be called in every child class's Init function
    virtual void Init(void *offset,NodeContextPtr &ctx) {
        offset_ = static_cast<T*>(offset);
        ctx_ = ctx;
    }

    // Utility function : get offset for addr from file beginning
    uint64_t GetRAW(const void * addr) override {
        return (uint64_t)addr - (uint64_t)ctx_->file_start;
    }
};

// Template for some MachO element which hold another copy for the element
// The reason is the copy of the inner struct for the element should be swapped
template <typename T>
class NodeData : public NodeOffset<T>{
protected:
    // Copied inner struct
    T data_;
public:
    // Getter reference for inner struct
    const T & data()const { return data_;}
    // Getter pointer for inner struct
    T * data_ptr(){ return &data_;}

    // Init function which should be called in every child class's Init function
    void Init(void *offset,NodeContextPtr & ctx){
        NodeOffset<T>::Init(offset,ctx);
        if(!NodeInFile(ctx, offset, NodeOffset<T>::DATA_SIZE())){
            throw NodeException("Malformed file: struct read out of bounds");
        }
        memcpy(&data_,offset,NodeOffset<T>::DATA_SIZE());
    }
};


MOEX_NAMESPACE_END

#endif // MOEXNODE_H
