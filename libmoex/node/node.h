//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef NODE_H
#define NODE_H

#include "common.h"
#include "helper.h"
#include "nodeview.h"

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
    virtual std::string GetDisplayName() = 0;
    virtual std::string GetDescription() = 0;
    virtual void ForEachChild(std::function<void(Node*)>) = 0;
    virtual NodeViewArray GetViews() = 0;
};

template <typename T>
class NodeOffset : public Node{
protected:
    T *offset_;
    NodeContextPtr ctx_;
public:
    static constexpr std::size_t DATA_SIZE(){return sizeof(T);}

    const T * offset()const{ return offset_;}

    virtual void init(void *offset,NodeContextPtr &ctx) {
        offset_ = static_cast<T*>(offset);
        ctx_ = ctx;
    }

    std::string GetTypeName() override{ return "node";}
    std::string GetDisplayName() override{ return "unknown";}
    std::string GetDescription() override{ return "empty";}
    void ForEachChild(std::function<void(Node*)>) override{}
    NodeViewArray GetViews() override{ return { std::make_shared<AddressNodeView>(offset_,DATA_SIZE())};}
};

template <typename T>
class NodeData : public NodeOffset<T>{
protected:
    T data_;
public:
    const T & data()const { return data_;}

    void init(void *offset,NodeContextPtr & ctx){
        NodeOffset<T>::init(offset,ctx);
        memcpy(&data_,offset,NodeOffset<T>::DATA_SIZE());
    }

};


MOEX_NAMESPACE_END

#endif // NODE_H
