//
// Integrity report for parser-visible consistency checks.
//

#ifndef MOEX_INTEGRITYREPORTVIEWNODE_H
#define MOEX_INTEGRITYREPORTVIEWNODE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class IntegrityReportViewNode : public ViewNode {
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh) { mh_ = mh; }
    std::string GetDisplayName() override { return "Integrity Report"; }
    void InitViewDatas() override;
};
using IntegrityReportViewNodePtr = std::shared_ptr<IntegrityReportViewNode>;

MOEX_NAMESPACE_END

#endif // MOEX_INTEGRITYREPORTVIEWNODE_H
