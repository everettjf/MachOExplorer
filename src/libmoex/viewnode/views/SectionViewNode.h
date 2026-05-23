//
// Created by everettjf on 2017/8/6.
//

#ifndef MOEX_SECTIONVIEWNODE_H
#define MOEX_SECTIONVIEWNODE_H

#include "libmoex/viewnode/ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SectionViewChildNode : public ViewNode{
protected:
    MachSectionPtr d_;
    std::string name_;
public:
    void Init(MachSectionPtr d){
        d_ = d;
    }
    std::string GetDisplayName()override{ return name_; }

    void set_name(const std::string & name){name_ = name;}

    char *GetOffset(){
        char *offset = (char*)d_->header()->header_start() + d_->sect().offset();
        return offset;
    }
    uint32_t GetSize(){
        uint64_t size = d_->sect().size_both();
        // Clamp the section data size to what is actually present in the mapped
        // file: a truncated or crafted section can claim more bytes than exist,
        // and the data readers below would otherwise run off the end.
        auto ctx = d_->ctx();
        if(ctx && ctx->file_start != nullptr){
            const char *off = GetOffset();
            const char *fstart = static_cast<const char*>(ctx->file_start);
            if(off < fstart)
                return 0;
            const uint64_t off_in_file = static_cast<uint64_t>(off - fstart);
            if(off_in_file >= ctx->file_size)
                return 0;
            const uint64_t avail = ctx->file_size - off_in_file;
            if(size > avail)
                size = avail;
        }
        return (uint32_t)size;
    }

    void InitViewDatas()override {
        // Binary
        {
            auto b = CreateBinaryView();
            b->offset = (char*)d_->offset();
            b->size = d_->DATA_SIZE();
            b->start_value = (uint64_t)b->offset - (uint64_t)d_->ctx()->file_start;
        }
    }
};
using SectionViewChildNodePtr = std::shared_ptr<SectionViewChildNode>;


class SectionViewNode : public ViewNode{
private:
    MachSectionPtr d_;
    std::vector<SectionViewChildNodePtr> children_;
public:
    void Init(MachSectionPtr d);
    std::string GetDisplayName()override;

    char *GetOffset(){
        char *offset = (char*)d_->header()->header_start() + d_->sect().offset();
        return offset;
    }
    uint32_t GetSize(){
        uint64_t size = d_->sect().size_both();
        // Clamp the section data size to what is actually present in the mapped
        // file: a truncated or crafted section can claim more bytes than exist,
        // and the data readers below would otherwise run off the end.
        auto ctx = d_->ctx();
        if(ctx && ctx->file_start != nullptr){
            const char *off = GetOffset();
            const char *fstart = static_cast<const char*>(ctx->file_start);
            if(off < fstart)
                return 0;
            const uint64_t off_in_file = static_cast<uint64_t>(off - fstart);
            if(off_in_file >= ctx->file_size)
                return 0;
            const uint64_t avail = ctx->file_size - off_in_file;
            if(size > avail)
                size = avail;
        }
        return (uint32_t)size;
    }

    void InitViewDatas()override;
    void ForEachChild(std::function<void(ViewNode*)>)override;

    void InitChildView();

    void InitCStringView(const std::string & title);
    void InitLiteralsView(const std::string & title,size_t unitsize);
    void InitPointersView(const std::string & title);
    void InitIndirectPointersView(const std::string & title);
    void InitIndirectStubsView(const std::string &title);
    void InitDisassemblyView(const std::string &title);
    void InitCFStringView(const std::string &title);
    void InitSwiftMetadataView(const std::string &title);
    void InitObjC2PointerView(const std::string &title);
    void InitObjC2MetadataView(const std::string &title, const std::string &kind);
    void InitObjC2ImageInfo(const std::string &title);
};
using SectionViewNodePtr = std::shared_ptr<SectionViewNode>;

class SectionViewNodeFactory{
public:
    static SectionViewNodePtr Create(MachSectionPtr d);
};


MOEX_NAMESPACE_END

#endif //MOEX_SECTIONVIEWNODE_H
