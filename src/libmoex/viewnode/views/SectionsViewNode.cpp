//
// Created by everettjf on 2017/7/23.
//

#include "SectionsViewNode.h"

MOEX_NAMESPACE_BEGIN

void SectionsViewNode::Init(MachHeaderPtr mh) {
    mh_ = mh;

    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_SEGMENT) {
            moex::LoadCommand_LC_SEGMENT *seg = static_cast<moex::LoadCommand_LC_SEGMENT*>(cmd.get());
            for(auto & sect : seg->sections_ref()){
                SectionViewNodePtr node = SectionViewNodeFactory::Create(sect);
                sections_.push_back(node);
            }
        }else if(cmd->offset()->cmd == LC_SEGMENT_64) {
            moex::LoadCommand_LC_SEGMENT_64 *seg = static_cast<moex::LoadCommand_LC_SEGMENT_64*>(cmd.get());
            for(auto & sect : seg->sections_ref()){
                SectionViewNodePtr node = SectionViewNodeFactory::Create(sect);
                sections_.push_back(node);
            }
        }
    }
}

void SectionsViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & section : sections_){
        callback(section.get());
    }
}

void SectionsViewNode::InitViewDatas(){
    using namespace moex::util;

    // Table
    {
        auto t = CreateTableView();
        t->SetHeaders({"Information"});
        t->SetWidths({300});
        t->AddRow({fmt::format("Number of sections : {}", sections_.size())});
    }
}
MOEX_NAMESPACE_END