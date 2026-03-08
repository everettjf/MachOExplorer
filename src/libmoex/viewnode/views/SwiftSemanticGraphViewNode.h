//
// Swift semantic graph view
//

#ifndef MOEX_SWIFTSEMANTICGRAPHVIEWNODE_H
#define MOEX_SWIFTSEMANTICGRAPHVIEWNODE_H

#include "../ViewNode.h"
#include "libmoex/node/MachHeader.h"

MOEX_NAMESPACE_BEGIN

class SwiftSemanticGraphViewNode : public ViewNode {
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh) { mh_ = mh; }
    std::string GetDisplayName() override { return "Swift Semantic Graph"; }
    void InitViewDatas() override;
};
using SwiftSemanticGraphViewNodePtr = std::shared_ptr<SwiftSemanticGraphViewNode>;

MOEX_NAMESPACE_END

#endif // MOEX_SWIFTSEMANTICGRAPHVIEWNODE_H
