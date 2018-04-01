//
// Created by everettjf on 2017/7/24.
//

#include "StringTableViewNode.h"

MOEX_NAMESPACE_BEGIN


void StringTableViewNode::Init(MachHeaderPtr mh){
    mh_ = mh;
}


void StringTableViewNode::InitViewDatas()
{
    using namespace moex::util;

    auto t = CreateTableView();
    t->SetHeaders({"Index","Offset","Length","String"});
    t->SetWidths({80,100,80,400});

    moex::LoadCommand_LC_SYMTAB *seg=nullptr;
    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_SYMTAB) {
            seg = static_cast<moex::LoadCommand_LC_SYMTAB*>(cmd.get());
            break;
        }
    }
    if(!seg)
        return;


    char * stroffset = (char*)seg->GetStringTableOffsetAddress();
    uint32_t strsize = seg->GetStringTableSize();

    auto string_results = util::ParseStringLiteral(stroffset,strsize);

    int lineno = 0;
    for(char * cur : string_results){
        std::string name(cur);
        t->AddRow({
                AsString(lineno),
                AsHexString(mh_->GetRAW(cur)),
                AsString(name.length()),
                name
                  });
        ++lineno;
    }


    auto b = CreateBinaryView();
    b->offset = stroffset;
    b->size = strsize;
    b->start_value = (uint64_t)stroffset - (uint64_t)mh_->header_start();
}

MOEX_NAMESPACE_END
