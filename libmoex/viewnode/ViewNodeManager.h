//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODEMANAGER_H
#define MOEX_VIEWNODEMANAGER_H


#include "ViewNode.h"
#include "../node/binary.h"
#include "FatBinaryViewNode.h"
#include "ExecutableViewNode.h"

MOEX_NAMESPACE_BEGIN

class ViewNodeManager{
private:
    BinaryPtr bin_;

    FatBinaryViewNodePtr fat_;
    ExecutableViewNodePtr exe_;

public:
    bool Init(const std::string &filepath, std::string &error);

    ViewNode * GetRootNode();

};


MOEX_NAMESPACE_END


#endif //MOEX_VIEWNODEMANAGER_H
