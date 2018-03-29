//
// Created by everettjf on 03/11/2017.
//

#ifndef MOEX_GUI_TWOLEVELHINTSTABLEVIEWNODE_H
#define MOEX_GUI_TWOLEVELHINTSTABLEVIEWNODE_H

#include "libmoex/viewnode/ViewNode.h"

MOEX_NAMESPACE_BEGIN

class TwoLevelHintsTableViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Two Level Hints Table";}
    void InitViewDatas()override;
};
using TwoLevelHintsTableViewNodePtr = std::shared_ptr<TwoLevelHintsTableViewNode>;

MOEX_NAMESPACE_END
#endif //MOEX_GUI_TWOLEVELHINTSTABLEVIEWNODE_H
