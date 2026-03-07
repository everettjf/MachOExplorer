//
// Cross-reference report by pointer references in sections.
//

#ifndef MOEX_XREFVIEWNODE_H
#define MOEX_XREFVIEWNODE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class XrefViewNode : public ViewNode {
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh) { mh_ = mh; }
    std::string GetDisplayName() override { return "Xref Report"; }
    void InitViewDatas() override;
};
using XrefViewNodePtr = std::shared_ptr<XrefViewNode>;

MOEX_NAMESPACE_END

#endif // MOEX_XREFVIEWNODE_H
