//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef NODE_H
#define NODE_H

#include "Common.h"
#include "Util.h"

MOEX_NAMESPACE_BEGIN

class NodeException : public std::exception{
private:
    std::string error_;
public:
    NodeException(const std::string & error) { error_ = error;}
    const char* what() const _NOEXCEPT override { return error_.c_str();}
};

struct NodeContext{
    void *file_start;
};
using NodeContextPtr = std::shared_ptr<NodeContext>;

class Node{
public:
    virtual std::string GetTypeName() = 0;
};

template <typename T>
class NodeOffset : public Node{
protected:
    T *offset_;
    NodeContextPtr ctx_;
public:
    static constexpr std::size_t DATA_SIZE(){return sizeof(T);}

    const T * offset()const{ return offset_;}

    virtual void Init(void *offset,NodeContextPtr &ctx) {
        offset_ = static_cast<T*>(offset);
        ctx_ = ctx;
    }

    std::string GetTypeName() override{ return "node";}
};

template <typename T>
class NodeData : public NodeOffset<T>{
protected:
    T data_;
public:
    const T & data()const { return data_;}
    T * data_ptr(){ return &data_;}

    void Init(void *offset,NodeContextPtr & ctx){
        NodeOffset<T>::Init(offset,ctx);
        memcpy(&data_,offset,NodeOffset<T>::DATA_SIZE());
    }

};


MOEX_NAMESPACE_END

#endif // NODE_H
