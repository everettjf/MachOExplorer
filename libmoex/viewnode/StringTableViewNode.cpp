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

    auto t = CreateTableViewDataPtr();
    t->SetHeaders({"Index","Offset","Length","String"});
    t->SetWidths({10,20,20,200});

    moex::LoadCommand_LC_SYMTAB *seg=nullptr;
    for(auto cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_SYMTAB) {
            seg = static_cast<moex::LoadCommand_LC_SYMTAB*>(cmd.get());
            break;
        }
    }
    if(!seg)
        return;


    char * stroffset = (char*)seg->GetStringTableOffsetAddress();
    uint32_t strsize = seg->GetStringTableSize();

    int lineno = 0;
    char *cur = stroffset;
    char *end = stroffset + strsize;

    while(cur < end){
        if(*cur == 0){
            ++cur;
            continue;
        }
        std::string name(cur);
        t->AddRow({
                AsString(lineno),
                AsHexString(mh_->GetRAW(cur)),
                AsString(name.length()),
                name
                  });

        cur += name.length();
        ++lineno;
    }
    AddViewData(t);

    auto b = CreateBinaryViewDataPtr();
    b->offset = stroffset;
    b->size = strsize;
    AddViewData(b);
}

MOEX_NAMESPACE_END
