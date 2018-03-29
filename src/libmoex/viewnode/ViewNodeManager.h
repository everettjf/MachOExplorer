//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODEMANAGER_H
#define MOEX_VIEWNODEMANAGER_H


#include "ViewNode.h"
#include "libmoex/node/Binary.h"
#include "views/FileViewNode.h"

MOEX_NAMESPACE_BEGIN

class ViewNodeManager{
private:
    BinaryPtr bin_;
    FileViewNodePtr file_;
private:
    void ConstructNode();

public:
    bool Init(const std::string &filepath, std::string &error);
    void Init(BinaryPtr bin);

    ViewNode * GetRootNode();
    bool IsFat();
};


MOEX_NAMESPACE_END


#endif //MOEX_VIEWNODEMANAGER_H
