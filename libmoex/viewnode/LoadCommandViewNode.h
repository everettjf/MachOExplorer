//
// Created by everettjf on 2017/8/4.
//

#ifndef MOEX_LOADCOMMANDVIEWNODE_H
#define MOEX_LOADCOMMANDVIEWNODE_H

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN


class LoadCommandViewNode : public ViewNode{
private:
    LoadCommandPtr d_;
public:
    void Init(LoadCommandPtr d);

    std::string GetDisplayName()override ;

};
using LoadCommandViewNodePtr = std::shared_ptr<LoadCommandViewNode>;

MOEX_NAMESPACE_END


#endif //MOEX_LOADCOMMANDVIEWNODE_H

