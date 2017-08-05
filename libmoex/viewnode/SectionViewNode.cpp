//
// Created by everettjf on 2017/8/6.
//

#include "SectionViewNode.h"

MOEX_NAMESPACE_BEGIN

void SectionViewNode::Init(MachSectionPtr d){
    d_ = d;
}
std::string SectionViewNode::GetDisplayName(){
    return boost::str(boost::format("Section(%1%,%2%)")%d_->segment_name()%d_->section_name());
}

void SectionViewNode::InitViewDatas(){
    using namespace moex::util;

    // Table
    {
        auto t = CreateTableViewDataPtr();
        if(d_->Is64()){
            const section_64 *sect = d_->offset64();
            t->AddRow(d_->GetRAW(&(sect->sectname)),(void*)sect->sectname,sizeof(char)*16,"Section Name",d_->section_name());
            t->AddRow(d_->GetRAW(&(sect->segname)),(void*)sect->segname,sizeof(char)*16,"Segment Name",d_->segment_name());

            t->AddRow(d_->GetRAW(&(sect->addr)),sect->addr,"Address",AsShortHexString(sect->addr));
            t->AddRow(d_->GetRAW(&(sect->size)),sect->size,"Size",AsShortHexString(sect->size));
            t->AddRow(d_->GetRAW(&(sect->offset)),sect->offset,"Offset",AsShortHexString(sect->offset));
            t->AddRow(d_->GetRAW(&(sect->align)),sect->align,"Alignment",AsShortHexString(sect->align));
            t->AddRow(d_->GetRAW(&(sect->reloff)),sect->reloff,"Relocations Offset",AsShortHexString(sect->reloff));
            t->AddRow(d_->GetRAW(&(sect->nreloc)),sect->nreloc,"Number of Relocations",AsShortHexString(sect->nreloc));
            t->AddRow(d_->GetRAW(&(sect->flags)),sect->flags,"Flags",AsShortHexString(sect->flags));
            t->AddRow(d_->GetRAW(&(sect->reserved1)),sect->reserved1,"Reserved1",AsShortHexString(sect->reserved1));
            t->AddRow(d_->GetRAW(&(sect->reserved2)),sect->reserved2,"Reserved2",AsShortHexString(sect->reserved2));

            t->AddRow(d_->GetRAW(&(d_->offset64()->reserved3)),d_->offset64()->reserved3,"Reserved3",AsShortHexString(d_->offset64()->reserved3));
        }else{
            const section *sect = d_->offset();
            t->AddRow(d_->GetRAW(&(sect->sectname)),(void*)sect->sectname,sizeof(char)*16,"Section Name",d_->section_name());
            t->AddRow(d_->GetRAW(&(sect->segname)),(void*)sect->segname,sizeof(char)*16,"Segment Name",d_->segment_name());

            t->AddRow(d_->GetRAW(&(sect->addr)),sect->addr,"Address",AsShortHexString(sect->addr));
            t->AddRow(d_->GetRAW(&(sect->size)),sect->size,"Size",AsShortHexString(sect->size));
            t->AddRow(d_->GetRAW(&(sect->offset)),sect->offset,"Offset",AsShortHexString(sect->offset));
            t->AddRow(d_->GetRAW(&(sect->align)),sect->align,"Alignment",AsShortHexString(sect->align));
            t->AddRow(d_->GetRAW(&(sect->reloff)),sect->reloff,"Relocations Offset",AsShortHexString(sect->reloff));
            t->AddRow(d_->GetRAW(&(sect->nreloc)),sect->nreloc,"Number of Relocations",AsShortHexString(sect->nreloc));
            t->AddRow(d_->GetRAW(&(sect->flags)),sect->flags,"Flags",AsShortHexString(sect->flags));
            t->AddRow(d_->GetRAW(&(sect->reserved1)),sect->reserved1,"Reserved1",AsShortHexString(sect->reserved1));
            t->AddRow(d_->GetRAW(&(sect->reserved2)),sect->reserved2,"Reserved2",AsShortHexString(sect->reserved2));

        }
        AddViewData(t);
    }

    // Binary
    {
        auto b = CreateBinaryViewDataPtr();
        if(d_->Is64()){
            b->offset = (char*)d_->offset64();
            b->size = d_->section64_ref()->DATA_SIZE();
        }else{
            b->offset = (char*)d_->offset();
            b->size = d_->section_ref()->DATA_SIZE();
        }
        AddViewData(b);
    }
}

SectionViewNodePtr SectionViewNodeFactory::Create(MachSectionPtr d){
    SectionViewNodePtr view = std::make_shared<SectionViewNode>();
    view->Init(d);
    return view;
}
MOEX_NAMESPACE_END