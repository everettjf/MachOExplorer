//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODEMANAGER_H
#define MOEX_VIEWNODEMANAGER_H


#include "viewnodeimpl.h"
#include "../node/binary.h"

MOEX_NAMESPACE_BEGIN

class ViewNodeManager{
private:
    BinaryPtr bin_;
public:
    bool Init(const std::string &filepath, std::string &error);

    ViewNode * GetRootNode();

};


MOEX_NAMESPACE_END


#endif //MOEX_VIEWNODEMANAGER_H
