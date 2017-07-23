//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_LOADCOMMANDSVIEWNODE_H
#define MOEX_LOADCOMMANDSVIEWNODE_H

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

class LoadCommandsViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;

    std::vector<LoadCommandViewNodePtr> cmds_;
public:
    void Init(MachHeaderPtr mh);

    std::string GetDisplayName()override { return "Load Commands";}
    void ForEachChild(std::function<void(ViewNode*)> callback) override ;
};
using LoadCommandsViewNodePtr = std::shared_ptr<LoadCommandsViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_LOADCOMMANDSVIEWNODE_H
