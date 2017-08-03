//
// Created by everettjf on 2017/8/4.
//

#include "LoadCommandViewNode.h"

#include "../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../node/loadcmd/LoadCommand_DYLIB.h"
#include "../node/loadcmd/LoadCommand_DYLD_INFO.h"
#include "../node/loadcmd/LoadCommand_SYMTAB.h"
#include "../node/loadcmd/LoadCommand_DYSYMTAB.h"
#include "../node/loadcmd/LoadCommand_LOAD_DYLINKER.h"
#include "../node/loadcmd/LoadCommand_UUID.h"
#include "../node/loadcmd/LoadCommand_VERSION_MIN.h"
#include "../node/loadcmd/LoadCommand_SOURCE_VERSION.h"
#include "../node/loadcmd/LoadCommand_MAIN.h"
#include "../node/loadcmd/LoadCommand_ENCRYPTION_INFO.h"
#include "../node/loadcmd/LoadCommand_LINKEDIT_DATA.h"

/*
        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_SEGMENT)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_SEGMENT_64)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_LOAD_DYLIB, DYLIB)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_LOAD_WEAK_DYLIB, DYLIB)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_REEXPORT_DYLIB, DYLIB)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_DYLD_INFO, DYLD_INFO)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_DYLD_INFO_ONLY, DYLD_INFO)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_SYMTAB)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_DYSYMTAB)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_LOAD_DYLINKER)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_UUID)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_VERSION_MIN_IPHONEOS, VERSION_MIN)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_VERSION_MIN_MACOSX, VERSION_MIN)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_VERSION_MIN_TVOS, VERSION_MIN)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_VERSION_MIN_WATCHOS, VERSION_MIN)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_SOURCE_VERSION)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_MAIN)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_ENCRYPTION_INFO)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT(LC_ENCRYPTION_INFO_64)

        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_CODE_SIGNATURE, LINKEDIT_DATA)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_SEGMENT_SPLIT_INFO, LINKEDIT_DATA)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_FUNCTION_STARTS, LINKEDIT_DATA)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_DATA_IN_CODE, LINKEDIT_DATA)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_DYLIB_CODE_SIGN_DRS, LINKEDIT_DATA)
        DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(LC_LINKER_OPTIMIZATION_HINT, LINKEDIT_DATA)
 */


MOEX_NAMESPACE_BEGIN

std::string LoadCommandViewNode::GetDisplayName(){
    std::string charact = d_->GetShortCharacteristicDescription();
    if(charact.length() > 0){
        return boost::str(boost::format("%1%(%2%)")%d_->GetLoadCommandTypeString()%charact);
    }else{
        return d_->GetLoadCommandTypeString();
    }
}


class LoadCommandViewNode_LC_SEGMENT : public LoadCommandViewNode{
public:
    void InitViewDatas()override{
        auto t = CreateTableViewDataPtr();
        auto b = CreateBinaryViewDataPtr();

        if(!t->IsEmpty()) AddViewData(t);
        if(!b->IsEmpty()) AddViewData(b);
    }
};

LoadCommandViewNodePtr LoadCommandViewNodeFactory::Create(LoadCommandPtr d){
    LoadCommandViewNodePtr res = std::make_shared<LoadCommandViewNode>();
    res->Init(d);

    return res;
}


MOEX_NAMESPACE_END