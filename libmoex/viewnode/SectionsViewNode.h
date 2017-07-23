//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_SECTIONSVIEWNODE_H
#define MOEX_SECTIONSVIEWNODE_H

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SectionViewNode : public ViewNode{
private:
    MachSectionPtr d_;
public:
    void Init(MachSectionPtr d);
    std::string GetDisplayName()override;

};
using SectionViewNodePtr = std::shared_ptr<SectionViewNode>;

class SectionsViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
    std::vector<SectionViewNodePtr> sections_;
public:
    void Init(MachHeaderPtr mh);

    std::string GetDisplayName()override { return "Sections";}
    void ForEachChild(std::function<void(ViewNode*)> callback)override;
};
using SectionsViewNodePtr = std::shared_ptr<SectionsViewNode>;

MOEX_NAMESPACE_END


#endif //MOEX_SECTIONSVIEWNODE_H
