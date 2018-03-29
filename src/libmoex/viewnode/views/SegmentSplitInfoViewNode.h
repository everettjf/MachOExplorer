//
// Created by everettjf on 03/11/2017.
//

#ifndef MOEX_GUI_SEGMENTSPLITINFOVIEWNODE_H
#define MOEX_GUI_SEGMENTSPLITINFOVIEWNODE_H

#include "libmoex/viewnode/ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SegmentSplitInfoViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Two Level Hints Table";}
    void InitViewDatas()override;
};
using SegmentSplitInfoViewNodePtr = std::shared_ptr<SegmentSplitInfoViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_GUI_SEGMENTSPLITINFOVIEWNODE_H
