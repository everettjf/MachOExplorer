//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_DYNAMICLOADERINFOVIEWNODE_H
#define MOEX_DYNAMICLOADERINFOVIEWNODE_H

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN

class DynamicLoaderInfoViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Dynamic Loader Info";}
    void InitViewDatas()override;
};
using DynamicLoaderInfoViewNodePtr = std::shared_ptr<DynamicLoaderInfoViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_DYNAMICLOADERINFOVIEWNODE_H
