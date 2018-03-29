//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_CODESIGNATUREVIEWNODE_H
#define MOEX_CODESIGNATUREVIEWNODE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class CodeSignatureViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Code Signature";}
    void InitViewDatas()override;
};
using CodeSignatureViewNodePtr = std::shared_ptr<CodeSignatureViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_CODESIGNATUREVIEWNODE_H
