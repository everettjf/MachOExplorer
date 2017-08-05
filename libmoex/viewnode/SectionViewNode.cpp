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
//        t->AddRow("","","Number of sections",AsString(sections_.size()));
        AddViewData(t);
    }

    // Binary
    {
        auto b = CreateBinaryViewDataPtr();

        AddViewData(b);
    }
}

MOEX_NAMESPACE_END