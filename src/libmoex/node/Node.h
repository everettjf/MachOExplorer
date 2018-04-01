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
    const char* what() const _NOEXCEPT override { return error_.c_str();}
};

// Context for parsing process
struct NodeContext{
    // File start address ( beginning of the file )
    void *file_start;
};
using NodeContextPtr = std::shared_ptr<NodeContext>;

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
        memcpy(&data_,offset,NodeOffset<T>::DATA_SIZE());
    }
};


MOEX_NAMESPACE_END

#endif // MOEXNODE_H
