//
// Created by everettjf on 2017/8/4.
//

#ifndef MOEX_LOADCOMMANDVIEWNODE_H
#define MOEX_LOADCOMMANDVIEWNODE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN


class LoadCommandViewNode : public ViewNode{
protected:
    LoadCommandPtr d_;
public:
    void Init(LoadCommandPtr d){d_ = d;}
    std::string GetDisplayName()override ;
};
using LoadCommandViewNodePtr = std::shared_ptr<LoadCommandViewNode>;

class LoadCommandViewNodeFactory{
public:
    static LoadCommandViewNodePtr Create(LoadCommandPtr d);
};

MOEX_NAMESPACE_END


#endif //MOEX_LOADCOMMANDVIEWNODE_H

