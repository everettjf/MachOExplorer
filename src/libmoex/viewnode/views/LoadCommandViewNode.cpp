//
// Created by everettjf on 2017/8/4.
//

#include "LoadCommandViewNode.h"

#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../../node/loadcmd/LoadCommand_DYLIB.h"
#include "../../node/loadcmd/LoadCommand_DYLD_INFO.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include "../../node/loadcmd/LoadCommand_DYSYMTAB.h"
#include "../../node/loadcmd/LoadCommand_LOAD_DYLINKER.h"
#include "../../node/loadcmd/LoadCommand_UUID.h"
#include "../../node/loadcmd/LoadCommand_VERSION_MIN.h"
#include "../../node/loadcmd/LoadCommand_SOURCE_VERSION.h"
#include "../../node/loadcmd/LoadCommand_MAIN.h"
#include "../../node/loadcmd/LoadCommand_ENCRYPTION_INFO.h"
#include "../../node/loadcmd/LoadCommand_LINKEDIT_DATA.h"


MOEX_NAMESPACE_BEGIN

std::string LoadCommandViewNode::GetDisplayName(){
    std::string charact = d_->GetShortCharacteristicDescription();
    if(charact.length() > 0){
        return fmt::format("{}({})",d_->GetLoadCommandTypeString(),charact);
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
        LoadCommand_##classname *c = static_cast<LoadCommand_##classname*>(d_.get());\
        {auto t = CreateTableView(c);\
        t->AddRow(c->cmd()->cmd,"Command",c->GetLoadCommandTypeString());\
        t->AddRow(c->cmd()->cmdsize,"Command Size",AsString(c->cmd()->cmdsize));\
        t->AddSeparator();


#define IMPL_LOADCOMMAND_VIEWNODE_END\
        if(!t->IsEmpty()) SetViewData(t);}\
        {auto b = CreateBinaryView();\
        b->offset = (char*)c->offset();\
        b->size = c->offset()->cmdsize;\
        b->start_value = (uint64_t)b->offset - (uint64_t)c->ctx()->file_start;\
        }\
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
    t->AddRow((void*)c->cmd()->segname,(uint64_t )sizeof(char)*16,"Segment Name",c->segment_name());
    t->AddRow(c->cmd()->vmaddr,"VM Address",AsShortHexString(c->cmd()->vmaddr));
    t->AddRow(c->cmd()->vmsize,"VM Size",AsString(c->cmd()->vmsize));
    t->AddRow(c->cmd()->fileoff,"File Offset",AsShortHexString(c->cmd()->fileoff));
    t->AddRow(c->cmd()->filesize,"File Size",AsString(c->cmd()->filesize));

    t->AddRow(c->cmd()->maxprot,"Maximum VM Protection","");
    for(auto & prot : c->GetMaxProts()){
        t->AddRow("",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->cmd()->initprot,"Initial VM Protection","");
    for(auto & prot : c->GetInitProts()){
        t->AddRow("",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->cmd()->nsects,"Number of Sections",AsString(c->cmd()->nsects));
    t->AddRow(c->cmd()->flags,"Flags",AsString(c->cmd()->flags));
IMPL_LOADCOMMAND_VIEWNODE_END


IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SEGMENT_64)
    t->AddRow((void*)c->cmd()->segname,(uint64_t )sizeof(char)*16,"Segment Name",c->segment_name());
    t->AddRow(c->cmd()->vmaddr,"VM Address",AsShortHexString(c->cmd()->vmaddr));
    t->AddRow(c->cmd()->vmsize,"VM Size",AsString(c->cmd()->vmsize));
    t->AddRow(c->cmd()->fileoff,"File Offset",AsShortHexString(c->cmd()->fileoff));
    t->AddRow(c->cmd()->filesize,"File Size",AsString(c->cmd()->filesize));

    t->AddRow(c->cmd()->maxprot,"Maximum VM Protection","");
    for(auto & prot : c->GetMaxProts()){
        t->AddRow("",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->cmd()->initprot,"Initial VM Protection","");
    for(auto & prot : c->GetInitProts()){
        t->AddRow("",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->cmd()->nsects,"Number of Sections",AsString(c->cmd()->nsects));
    t->AddRow(c->cmd()->flags,"Flags",AsString(c->cmd()->flags));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(DYLIB)
    t->AddRow(c->cmd()->dylib.name.offset,"Str Offset",AsShortHexString(c->cmd()->dylib.name.offset));
    t->AddRow(c->cmd()->dylib.timestamp,"Time Stamp",c->GetTimeStamp());
    t->AddRow(c->cmd()->dylib.current_version,"Current Version",c->GetCurrentVersion());
    t->AddRow(c->cmd()->dylib.compatibility_version,"Compatibility Version",c->GetCompatibilityVersion());
    t->AddSeparator();

    t->AddRow((void*)(c->dylib_path_offset()),(uint64_t)(c->dylib_path().length()),"Path",c->dylib_path());
    t->AddRow("","Name",c->dylib_name());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(DYLD_INFO)
    t->AddRow(c->cmd()->rebase_off,"Rebase Info Offset",AsString(c->cmd()->rebase_off));
    t->AddRow(c->cmd()->rebase_size,"Rebase Info Size",AsString(c->cmd()->rebase_size));

    t->AddRow(c->cmd()->bind_off,"Binding Info Offset",AsString(c->cmd()->bind_off));
    t->AddRow(c->cmd()->bind_size,"Binding Info Size",AsString(c->cmd()->bind_size));

    t->AddRow(c->cmd()->weak_bind_off,"Weak Binding Info Offset",AsString(c->cmd()->weak_bind_off));
    t->AddRow(c->cmd()->weak_bind_size,"Weak Binding Info Size",AsString(c->cmd()->weak_bind_size));

    t->AddRow(c->cmd()->lazy_bind_off,"Lazy Binding Info Offset",AsString(c->cmd()->lazy_bind_off));
    t->AddRow(c->cmd()->lazy_bind_size,"Lazy Binding Info Size",AsString(c->cmd()->lazy_bind_size));

    t->AddRow(c->cmd()->export_off,"Export Info Offset",AsString(c->cmd()->export_off));
    t->AddRow(c->cmd()->export_size,"Export Info Size",AsString(c->cmd()->export_size));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SYMTAB)
    t->AddRow(c->cmd()->symoff,"Symbol Table Offset",AsString(c->cmd()->symoff));
    t->AddRow(c->cmd()->nsyms,"Number of Symbols",AsString(c->cmd()->nsyms));
    t->AddRow(c->cmd()->stroff,"String Table Offset",AsString(c->cmd()->stroff));
    t->AddRow(c->cmd()->strsize,"String Table Size",AsString(c->cmd()->strsize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_DYSYMTAB)
    t->AddRow(c->cmd()->ilocalsym,"Local Symbols Index",AsString(c->cmd()->ilocalsym));
    t->AddRow(c->cmd()->nlocalsym,"Number of Local Symbols",AsString(c->cmd()->nlocalsym));

    t->AddRow(c->cmd()->iextdefsym,"Defined External Symbols Index",AsString(c->cmd()->iextdefsym));
    t->AddRow(c->cmd()->nextdefsym,"Number of Defined External Symbols",AsString(c->cmd()->nextdefsym));

    t->AddRow(c->cmd()->iundefsym,"Undefined External Symbols Index",AsString(c->cmd()->iundefsym));
    t->AddRow(c->cmd()->nundefsym,"Number of Undefined External Symbols",AsString(c->cmd()->nundefsym));

    t->AddRow(c->cmd()->tocoff,"TOC Offset",AsString(c->cmd()->tocoff));
    t->AddRow(c->cmd()->ntoc,"TOC Entries",AsString(c->cmd()->ntoc));

    t->AddRow(c->cmd()->modtaboff,"Module Table Offset",AsString(c->cmd()->modtaboff));
    t->AddRow(c->cmd()->nmodtab,"Module Table Entries",AsString(c->cmd()->nmodtab));

    t->AddRow(c->cmd()->extrefsymoff,"ExtRef Table Offset",AsString(c->cmd()->extrefsymoff));
    t->AddRow(c->cmd()->nextrefsyms,"ExtRef Table Entries",AsString(c->cmd()->nextrefsyms));

    t->AddRow(c->cmd()->indirectsymoff,"Indirect Symbol Table Offset",AsString(c->cmd()->indirectsymoff));
    t->AddRow(c->cmd()->nindirectsyms,"Indirect Symbol Table Entries",AsString(c->cmd()->nindirectsyms));

    t->AddRow(c->cmd()->extreloff,"ExtReloc Table Offset",AsString(c->cmd()->extreloff));
    t->AddRow(c->cmd()->nextrel,"ExtReloc Table Entries",AsString(c->cmd()->nextrel));

    t->AddRow(c->cmd()->locreloff,"LocReloc Table Offset",AsString(c->cmd()->locreloff));
    t->AddRow(c->cmd()->nlocrel,"LocReloc Table Entries",AsString(c->cmd()->nlocrel));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_LOAD_DYLINKER)
    t->AddRow(c->cmd()->name.offset,"Str Offset",AsShortHexString(c->cmd()->name.offset));
    t->AddSeparator();

    t->AddRow((void*)(c->dylinker_path_name_offset()),(uint64_t)(c->dylinker_path_name().length()),"Name",c->dylinker_path_name());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_UUID)
    t->AddRow(c->cmd()->uuid,sizeof(c->cmd()->uuid),"UUID",c->GetUUIDString());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(VERSION_MIN)
    t->AddRow(c->cmd()->version,"Version",c->GetVersion());
    t->AddRow(c->cmd()->sdk,"SDK",c->GetSDK());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SOURCE_VERSION)
    t->AddRow(c->cmd()->version,"Version",c->GetVersion());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_MAIN)
    t->AddRow(c->cmd()->entryoff,"Entry Offset",AsShortHexString(c->cmd()->entryoff));
    t->AddRow(c->cmd()->stacksize,"Stack Size",AsShortHexString(c->cmd()->stacksize));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_ENCRYPTION_INFO)
    t->AddRow(c->cmd()->cryptoff,"Crypt Offset",AsShortHexString(c->cmd()->cryptoff));
    t->AddRow(c->cmd()->cryptsize,"Crypt Size",AsShortHexString(c->cmd()->cryptsize));
    t->AddRow(c->cmd()->cryptid,"Crypt ID",AsShortHexString(c->cmd()->cryptid));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_ENCRYPTION_INFO_64)
    t->AddRow(c->cmd()->cryptoff,"Crypt Offset",AsShortHexString(c->cmd()->cryptoff));
    t->AddRow(c->cmd()->cryptsize,"Crypt Size",AsShortHexString(c->cmd()->cryptsize));
    t->AddRow(c->cmd()->cryptid,"Crypt ID",AsShortHexString(c->cmd()->cryptid));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_CODE_SIGNATURE)
            t->AddRow(c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
            t->AddRow(c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SEGMENT_SPLIT_INFO)
            t->AddRow(c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
            t->AddRow(c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_FUNCTION_STARTS)
            t->AddRow(c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
            t->AddRow(c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_DATA_IN_CODE)
            t->AddRow(c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
            t->AddRow(c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_DYLIB_CODE_SIGN_DRS)
            t->AddRow(c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
            t->AddRow(c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_LINKER_OPTIMIZATION_HINT)
    t->AddRow(c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
    t->AddRow(c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
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

        CASE_LOADCOMMAND_VIEWNODE(LC_CODE_SIGNATURE)
        CASE_LOADCOMMAND_VIEWNODE(LC_SEGMENT_SPLIT_INFO)
        CASE_LOADCOMMAND_VIEWNODE(LC_FUNCTION_STARTS)
        CASE_LOADCOMMAND_VIEWNODE(LC_DATA_IN_CODE)
        CASE_LOADCOMMAND_VIEWNODE(LC_DYLIB_CODE_SIGN_DRS)
        CASE_LOADCOMMAND_VIEWNODE(LC_LINKER_OPTIMIZATION_HINT)
        default:{
            res = std::make_shared<LoadCommandViewNode>();
            break;
        }
    }
    res->Init(d);
    return res;
}


MOEX_NAMESPACE_END