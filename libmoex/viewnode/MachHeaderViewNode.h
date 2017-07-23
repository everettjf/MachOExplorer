//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_MACHHEADERVIEWNODE_H
#define MOEX_MACHHEADERVIEWNODE_H

#include "ViewNode.h"
#include "../node/machheader.h"

MOEX_NAMESPACE_BEGIN

class MachHeaderViewNode : public ViewNode{
private:
    MachHeaderPtr d_;
public:
    MachHeaderViewNode(MachHeaderPtr d);

    std::string GetDisplayName()override { return "Mach Header";}
};
using MachHeaderViewNodePtr = std::shared_ptr<MachHeaderViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_MACHHEADERVIEWNODE_H
