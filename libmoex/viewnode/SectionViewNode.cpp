//
// Created by everettjf on 2017/8/6.
//

#include "SectionViewNode.h"

MOEX_NAMESPACE_BEGIN

void SectionViewNode::Init(MachSectionPtr d){
    d_ = d;
}
std::string SectionViewNode::GetDisplayName(){
    return boost::str(boost::format("Section(%1%,%2%)")%d_->sect().segment_name()%d_->sect().section_name());
}

void SectionViewNode::InitViewDatas(){
    using namespace moex::util;

    // Table
    {
        auto t = CreateTableViewDataPtr();
        moex_section &sect = d_->sect();
        t->AddRow(d_->GetRAW(&(sect.sectname())),(void*)sect.sectname(),sizeof(char)*16,"Section Name",sect.section_name());
        t->AddRow(d_->GetRAW(&(sect.segname())),(void*)sect.segname(),sizeof(char)*16,"Segment Name",sect.segment_name());

        if(d_->Is64()) {
            t->AddRow(d_->GetRAW(&(sect.addr64())), sect.addr64(), "Address", AsShortHexString(sect.addr64()));
            t->AddRow(d_->GetRAW(&(sect.size64())), sect.size64(), "Size", AsShortHexString(sect.size64()));
        }else{
            t->AddRow(d_->GetRAW(&(sect.addr())), sect.addr(), "Address", AsShortHexString(sect.addr()));
            t->AddRow(d_->GetRAW(&(sect.size())), sect.size(), "Size", AsShortHexString(sect.size()));
        }

        t->AddRow(d_->GetRAW(&(sect.offset())),sect.offset(),"Offset",AsShortHexString(sect.offset()));
        t->AddRow(d_->GetRAW(&(sect.align())),sect.align(),"Alignment",AsShortHexString(sect.align()));
        t->AddRow(d_->GetRAW(&(sect.reloff())),sect.reloff(),"Relocations Offset",AsShortHexString(sect.reloff()));
        t->AddRow(d_->GetRAW(&(sect.nreloc())),sect.nreloc(),"Number of Relocations",AsShortHexString(sect.nreloc()));
        t->AddRow(d_->GetRAW(&(sect.flags())),sect.flags(),"Flags",AsShortHexString(sect.flags()));
        t->AddRow(d_->GetRAW(&(sect.reserved1())),sect.reserved1(),"Reserved1",AsShortHexString(sect.reserved1()));
        t->AddRow(d_->GetRAW(&(sect.reserved2())),sect.reserved2(),"Reserved2",AsShortHexString(sect.reserved2()));

        if(d_->Is64()){
            t->AddRow(d_->GetRAW(&(sect.reserved3())),sect.reserved3(),"Reserved3",AsShortHexString(sect.reserved3()));
        }
        AddViewData(t);
    }

    // Binary
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = (char*)d_->offset();
        b->size = d_->DATA_SIZE();
        AddViewData(b);
    }
}

SectionViewNodePtr SectionViewNodeFactory::Create(MachSectionPtr d){
    SectionViewNodePtr view = std::make_shared<SectionViewNode>();
    view->Init(d);
    return view;
}
MOEX_NAMESPACE_END