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


MOEX_NAMESPACE_BEGIN

std::string LoadCommandViewNode::GetDisplayName(){
    std::string charact = d_->GetShortCharacteristicDescription();
    if(charact.length() > 0){
        return boost::str(boost::format("%1%(%2%)")%d_->GetLoadCommandTypeString()%charact);
    }else{
        return d_->GetLoadCommandTypeString();
    }
}

//////////////////////////////////////////////////////////////////////////////////

#define IMPL_LOADCOMMAND_VIEWNODE_BEGIN(classname)\
class LoadCommandViewNode_##classname : public LoadCommandViewNode{ \
public:\
    void InitViewDatas()override{\
        using namespace moex::util;\
        auto t = CreateTableViewDataPtr();\
        auto b = CreateBinaryViewDataPtr();\
        LoadCommand_##classname *c = static_cast<LoadCommand_##classname*>(d_.get());\
        t->AddRow(c->GetRAW(&(c->cmd()->cmd)),c->cmd()->cmd,"Command",c->GetLoadCommandTypeString());\
        t->AddRow(c->GetRAW(&(c->cmd()->cmdsize)),c->cmd()->cmdsize,"Command Size",AsString(c->cmd()->cmdsize));\
        t->AddSeparator();


#define IMPL_LOADCOMMAND_VIEWNODE_END\
        if(!t->IsEmpty()) AddViewData(t);\
        if(!b->IsEmpty()) AddViewData(b);\
    }\
};



#define CASE_LOADCOMMAND_VIEWNODE(segment) \
        case segment: { \
            res = std::make_shared<LoadCommandViewNode_##segment>();\
            break;\
        }

#define CASE_LOADCOMMAND_VIEWNODE_CLASS(segment,classname) \
        case segment: { \
            res = std::make_shared<LoadCommandViewNode_##classname>();\
            break;\
        }

//////////////////////////////////////////////////////////////////////////////////

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SEGMENT)
IMPL_LOADCOMMAND_VIEWNODE_END


IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SEGMENT_64)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(DYLIB)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(DYLD_INFO)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SYMTAB)
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_DYSYMTAB)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_LOAD_DYLINKER)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_UUID)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(VERSION_MIN)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SOURCE_VERSION)
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_MAIN)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_ENCRYPTION_INFO)
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_ENCRYPTION_INFO_64)
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LINKEDIT_DATA)
IMPL_LOADCOMMAND_VIEWNODE_END
//////////////////////////////////////////////////////////////////////////////////

LoadCommandViewNodePtr LoadCommandViewNodeFactory::Create(LoadCommandPtr d){
    LoadCommandViewNodePtr res;
    switch(d->GetCommand()){
        CASE_LOADCOMMAND_VIEWNODE(LC_SEGMENT)
        CASE_LOADCOMMAND_VIEWNODE(LC_SEGMENT_64)

        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_LOAD_DYLIB, DYLIB)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_LOAD_WEAK_DYLIB, DYLIB)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_REEXPORT_DYLIB, DYLIB)

        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_DYLD_INFO, DYLD_INFO)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_DYLD_INFO_ONLY, DYLD_INFO)

        CASE_LOADCOMMAND_VIEWNODE(LC_SYMTAB)
        CASE_LOADCOMMAND_VIEWNODE(LC_DYSYMTAB)

        CASE_LOADCOMMAND_VIEWNODE(LC_LOAD_DYLINKER)

        CASE_LOADCOMMAND_VIEWNODE(LC_UUID)

        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_VERSION_MIN_IPHONEOS, VERSION_MIN)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_VERSION_MIN_MACOSX, VERSION_MIN)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_VERSION_MIN_TVOS, VERSION_MIN)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_VERSION_MIN_WATCHOS, VERSION_MIN)

        CASE_LOADCOMMAND_VIEWNODE(LC_SOURCE_VERSION)
        CASE_LOADCOMMAND_VIEWNODE(LC_MAIN)

        CASE_LOADCOMMAND_VIEWNODE(LC_ENCRYPTION_INFO)
        CASE_LOADCOMMAND_VIEWNODE(LC_ENCRYPTION_INFO_64)

        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_CODE_SIGNATURE, LINKEDIT_DATA)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_SEGMENT_SPLIT_INFO, LINKEDIT_DATA)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_FUNCTION_STARTS, LINKEDIT_DATA)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_DATA_IN_CODE, LINKEDIT_DATA)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_DYLIB_CODE_SIGN_DRS, LINKEDIT_DATA)
        CASE_LOADCOMMAND_VIEWNODE_CLASS(LC_LINKER_OPTIMIZATION_HINT, LINKEDIT_DATA)
        default:{
            res = std::make_shared<LoadCommandViewNode>();
            break;
        }
    }
    res->Init(d);
    return res;
}


MOEX_NAMESPACE_END