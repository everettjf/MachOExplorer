//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_SECTIONSVIEWNODE_H
#define MOEX_SECTIONSVIEWNODE_H

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SectionViewNode : public ViewNode{
private:
public:

};

class SectionsViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh);

    std::string GetDisplayName()override { return "Sections";}
    void ForEachChild(std::function<void(ViewNode*)> callback)override;
};
using SectionsViewNodePtr = std::shared_ptr<SectionsViewNode>;

MOEX_NAMESPACE_END


#endif //MOEX_SECTIONSVIEWNODE_H
