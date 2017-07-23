//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODEMANAGER_H
#define MOEX_VIEWNODEMANAGER_H


#include "ViewNode.h"
#include "../node/binary.h"
#include "FatHeaderViewNode.h"
#include "MachHeaderViewNode.h"

MOEX_NAMESPACE_BEGIN

class ViewNodeManager{
private:
    BinaryPtr bin_;

    std::shared_ptr<FatHeaderViewNode> fat_;
    std::shared_ptr<MachHeaderViewNode> mh_;

private:
    void ConstructNode();

public:
    bool Init(const std::string &filepath, std::string &error);
    void Init(BinaryPtr bin);

    ViewNode * GetRootNode();

};


MOEX_NAMESPACE_END


#endif //MOEX_VIEWNODEMANAGER_H
