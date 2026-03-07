#ifndef MOEX_ARCHIVEVIEWNODE_H
#define MOEX_ARCHIVEVIEWNODE_H

#include "../ViewNode.h"
#include "libmoex/node/Archive.h"
#include "MachHeaderViewNode.h"

MOEX_NAMESPACE_BEGIN

class ArchiveViewNode : public ViewNode {
private:
    ArchivePtr archive_;
    std::vector<MachHeaderViewNodePtr> headers_;

public:
    void Init(ArchivePtr archive);

    std::string GetDisplayName() override { return "Archive"; }
    void ForEachChild(std::function<void(ViewNode*)> callback) override;
    void InitViewDatas() override;
};
using ArchiveViewNodePtr = std::shared_ptr<ArchiveViewNode>;

MOEX_NAMESPACE_END

#endif // MOEX_ARCHIVEVIEWNODE_H
