//
// Created by everettjf on 2017/7/30.
//

#ifndef MOEX_FILEVIEWNODE_H
#define MOEX_FILEVIEWNODE_H


#include "../ViewNode.h"
#include "libmoex/node/Binary.h"
#include "FatHeaderViewNode.h"
#include "MachHeaderViewNode.h"

MOEX_NAMESPACE_BEGIN

class FileViewNode : public ViewNode{
private:
    BinaryPtr bin_;
    FatHeaderViewNodePtr fat_;
    MachHeaderViewNodePtr mh_;

public:
    void Init(BinaryPtr bin);

    std::string GetDisplayName()override {
        return "File";
    }
    void ForEachChild(std::function<void(ViewNode*)> callback)override;

    void InitViewDatas()override;
};
using FileViewNodePtr = std::shared_ptr<FileViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_FILEVIEWNODE_H
