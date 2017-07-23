//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_MACHHEADERVIEWNODE_H
#define MOEX_MACHHEADERVIEWNODE_H

#include "ViewNode.h"
#include "../node/machheader.h"
#include "LoadCommandsViewNode.h"
#include "SectionsViewNode.h"

MOEX_NAMESPACE_BEGIN

class MachHeaderViewNode : public ViewNode{
private:
    MachHeaderPtr d_;

    LoadCommandsViewNodePtr load_commands_;
    SectionsViewNodePtr sections_;
public:
    void Init(MachHeaderPtr d);

    std::string GetDisplayName()override;

    void ForEachChild(std::function<void(ViewNode*)> callback)override;
};
using MachHeaderViewNodePtr = std::shared_ptr<MachHeaderViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_MACHHEADERVIEWNODE_H
