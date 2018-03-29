//
// Created by everettjf on 2017/8/6.
//

#ifndef MOEX_SECTIONVIEWNODE_H
#define MOEX_SECTIONVIEWNODE_H

#include "libmoex/viewnode/ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SectionViewNode : public ViewNode{
private:
    MachSectionPtr d_;
public:
    void Init(MachSectionPtr d);
    std::string GetDisplayName()override;

    char *GetOffset(){
        char *offset = (char*)d_->header()->header_start() + d_->sect().offset();
        return offset;
    }
    uint32_t GetSize(){
        uint32_t size = (uint32_t)d_->sect().size_both();
        return size;
    }

    void InitViewDatas()override;

    void InitSpecialView();

    void InitCStringView(const std::string & title);
    void InitLiteralsView(const std::string & title,size_t unitsize);
    void InitPointersView(const std::string & title);
    void InitIndirectPointersView(const std::string & title);
    void InitIndirectStubsView(const std::string &title);

    void InitCFStringView(const std::string &title);

    void InitObjC2PointerView(const std::string &title);
    void InitObjC2ImageInfo(const std::string &title);

    void InitTodoView();
};
using SectionViewNodePtr = std::shared_ptr<SectionViewNode>;

class SectionViewNodeFactory{
public:
    static SectionViewNodePtr Create(MachSectionPtr d);
};


MOEX_NAMESPACE_END

#endif //MOEX_SECTIONVIEWNODE_H
