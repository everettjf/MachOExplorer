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
        LoadCommand_##classname *c = static_cast<LoadCommand_##classname*>(d_.get());\
        {auto t = CreateTableViewDataPtr();\
        t->AddRow(c->GetRAW(&(c->cmd()->cmd)),c->cmd()->cmd,"Command",c->GetLoadCommandTypeString());\
        t->AddRow(c->GetRAW(&(c->cmd()->cmdsize)),c->cmd()->cmdsize,"Command Size",AsString(c->cmd()->cmdsize));\
        t->AddSeparator();


#define IMPL_LOADCOMMAND_VIEWNODE_END\
        if(!t->IsEmpty()) AddViewData(t);}\
        {auto b = CreateBinaryViewDataPtr();\
        b->offset = (char*)c->offset();\
        b->size = c->offset()->cmdsize;\
        if(!b->IsEmpty()) AddViewData(b);}\
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
    t->AddRow(c->GetRAW(&(c->cmd()->segname)),(void*)c->cmd()->segname,sizeof(char)*16,"Segment Name",c->segment_name());
    t->AddRow(c->GetRAW(&(c->cmd()->vmaddr)),c->cmd()->vmaddr,"VM Address",AsShortHexString(c->cmd()->vmaddr));
    t->AddRow(c->GetRAW(&(c->cmd()->vmsize)),c->cmd()->vmsize,"VM Size",AsString(c->cmd()->vmsize));
    t->AddRow(c->GetRAW(&(c->cmd()->fileoff)),c->cmd()->fileoff,"File Offset",AsShortHexString(c->cmd()->fileoff));
    t->AddRow(c->GetRAW(&(c->cmd()->filesize)),c->cmd()->filesize,"File Size",AsString(c->cmd()->filesize));

    t->AddRow(c->GetRAW(&(c->cmd()->maxprot)),c->cmd()->maxprot,"Maximum VM Protection","");
    for(auto & prot : c->GetMaxProts()){
        t->AddRow("","",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->GetRAW(&(c->cmd()->initprot)),c->cmd()->initprot,"Initial VM Protection","");
    for(auto & prot : c->GetInitProts()){
        t->AddRow("","",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->GetRAW(&(c->cmd()->nsects)),c->cmd()->nsects,"Number of Sections",AsString(c->cmd()->nsects));
    t->AddRow(c->GetRAW(&(c->cmd()->flags)),c->cmd()->flags,"Flags",AsString(c->cmd()->flags));

IMPL_LOADCOMMAND_VIEWNODE_END


IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SEGMENT_64)
    t->AddRow(c->GetRAW(&(c->cmd()->segname)),(void*)c->cmd()->segname,sizeof(char)*16,"Segment Name",c->segment_name());
    t->AddRow(c->GetRAW(&(c->cmd()->vmaddr)),c->cmd()->vmaddr,"VM Address",AsShortHexString(c->cmd()->vmaddr));
    t->AddRow(c->GetRAW(&(c->cmd()->vmsize)),c->cmd()->vmsize,"VM Size",AsString(c->cmd()->vmsize));
    t->AddRow(c->GetRAW(&(c->cmd()->fileoff)),c->cmd()->fileoff,"File Offset",AsShortHexString(c->cmd()->fileoff));
    t->AddRow(c->GetRAW(&(c->cmd()->filesize)),c->cmd()->filesize,"File Size",AsString(c->cmd()->filesize));

    t->AddRow(c->GetRAW(&(c->cmd()->maxprot)),c->cmd()->maxprot,"Maximum VM Protection","");
    for(auto & prot : c->GetMaxProts()){
        t->AddRow("","",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->GetRAW(&(c->cmd()->initprot)),c->cmd()->initprot,"Initial VM Protection","");
    for(auto & prot : c->GetInitProts()){
        t->AddRow("","",AsShortHexString(std::get<0>(prot)),std::get<1>(prot));
    }

    t->AddRow(c->GetRAW(&(c->cmd()->nsects)),c->cmd()->nsects,"Number of Sections",AsString(c->cmd()->nsects));
    t->AddRow(c->GetRAW(&(c->cmd()->flags)),c->cmd()->flags,"Flags",AsString(c->cmd()->flags));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(DYLIB)
    t->AddRow(c->GetRAW(&(c->cmd()->dylib.name.offset)),c->cmd()->dylib.name.offset,"Str Offset",AsShortHexString(c->cmd()->dylib.name.offset));
    t->AddRow(c->GetRAW(&(c->cmd()->dylib.timestamp)),c->cmd()->dylib.timestamp,"Time Stamp",c->GetTimeStamp());
    t->AddRow(c->GetRAW(&(c->cmd()->dylib.current_version)),c->cmd()->dylib.current_version,"Current Version",c->GetCurrentVersion());
    t->AddRow(c->GetRAW(&(c->cmd()->dylib.compatibility_version)),c->cmd()->dylib.compatibility_version,"Compatibility Version",c->GetCompatibilityVersion());
    t->AddSeparator();

    t->AddRow(c->GetRAW((void*)(c->dylib_path_offset())),AsHexData((void*)(c->dylib_path_offset()),(std::size_t)(c->dylib_path().length())),"Path",c->dylib_path());
    t->AddRow("","","Name",c->dylib_name());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(DYLD_INFO)
    t->AddRow(c->GetRAW(&(c->cmd()->rebase_off)),c->cmd()->rebase_off,"Rebase Info Offset",AsString(c->cmd()->rebase_off));
    t->AddRow(c->GetRAW(&(c->cmd()->rebase_size)),c->cmd()->rebase_size,"Rebase Info Size",AsString(c->cmd()->rebase_size));

    t->AddRow(c->GetRAW(&(c->cmd()->bind_off)),c->cmd()->bind_off,"Binding Info Offset",AsString(c->cmd()->bind_off));
    t->AddRow(c->GetRAW(&(c->cmd()->bind_size)),c->cmd()->bind_size,"Binding Info Size",AsString(c->cmd()->bind_size));

    t->AddRow(c->GetRAW(&(c->cmd()->weak_bind_off)),c->cmd()->weak_bind_off,"Weak Binding Info Offset",AsString(c->cmd()->weak_bind_off));
    t->AddRow(c->GetRAW(&(c->cmd()->weak_bind_size)),c->cmd()->weak_bind_size,"Weak Binding Info Size",AsString(c->cmd()->weak_bind_size));

    t->AddRow(c->GetRAW(&(c->cmd()->lazy_bind_off)),c->cmd()->lazy_bind_off,"Lazy Binding Info Offset",AsString(c->cmd()->lazy_bind_off));
    t->AddRow(c->GetRAW(&(c->cmd()->lazy_bind_size)),c->cmd()->lazy_bind_size,"Lazy Binding Info Size",AsString(c->cmd()->lazy_bind_size));

    t->AddRow(c->GetRAW(&(c->cmd()->export_off)),c->cmd()->export_off,"Export Info Offset",AsString(c->cmd()->export_off));
    t->AddRow(c->GetRAW(&(c->cmd()->export_size)),c->cmd()->export_size,"Export Info Size",AsString(c->cmd()->export_size));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SYMTAB)
    t->AddRow(c->GetRAW(&(c->cmd()->symoff)),c->cmd()->symoff,"Symbol Table Offset",AsString(c->cmd()->symoff));
    t->AddRow(c->GetRAW(&(c->cmd()->nsyms)),c->cmd()->nsyms,"Number of Symbols",AsString(c->cmd()->nsyms));
    t->AddRow(c->GetRAW(&(c->cmd()->stroff)),c->cmd()->stroff,"String Table Offset",AsString(c->cmd()->stroff));
    t->AddRow(c->GetRAW(&(c->cmd()->strsize)),c->cmd()->strsize,"String Table Size",AsString(c->cmd()->strsize));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_DYSYMTAB)
    t->AddRow(c->GetRAW(&(c->cmd()->ilocalsym)),c->cmd()->ilocalsym,"Local Symbols Index",AsString(c->cmd()->ilocalsym));
    t->AddRow(c->GetRAW(&(c->cmd()->nlocalsym)),c->cmd()->nlocalsym,"Number of Local Symbols",AsString(c->cmd()->nlocalsym));

    t->AddRow(c->GetRAW(&(c->cmd()->iextdefsym)),c->cmd()->iextdefsym,"Defined External Symbols Index",AsString(c->cmd()->iextdefsym));
    t->AddRow(c->GetRAW(&(c->cmd()->nextdefsym)),c->cmd()->nextdefsym,"Number of Defined External Symbols",AsString(c->cmd()->nextdefsym));

    t->AddRow(c->GetRAW(&(c->cmd()->iundefsym)),c->cmd()->iundefsym,"Undefined External Symbols Index",AsString(c->cmd()->iundefsym));
    t->AddRow(c->GetRAW(&(c->cmd()->nundefsym)),c->cmd()->nundefsym,"Number of Undefined External Symbols",AsString(c->cmd()->nundefsym));

    t->AddRow(c->GetRAW(&(c->cmd()->tocoff)),c->cmd()->tocoff,"TOC Offset",AsString(c->cmd()->tocoff));
    t->AddRow(c->GetRAW(&(c->cmd()->ntoc)),c->cmd()->ntoc,"TOC Entries",AsString(c->cmd()->ntoc));

    t->AddRow(c->GetRAW(&(c->cmd()->modtaboff)),c->cmd()->modtaboff,"Module Table Offset",AsString(c->cmd()->modtaboff));
    t->AddRow(c->GetRAW(&(c->cmd()->nmodtab)),c->cmd()->nmodtab,"Module Table Entries",AsString(c->cmd()->nmodtab));

    t->AddRow(c->GetRAW(&(c->cmd()->extrefsymoff)),c->cmd()->extrefsymoff,"ExtRef Table Offset",AsString(c->cmd()->extrefsymoff));
    t->AddRow(c->GetRAW(&(c->cmd()->nextrefsyms)),c->cmd()->nextrefsyms,"ExtRef Table Entries",AsString(c->cmd()->nextrefsyms));

    t->AddRow(c->GetRAW(&(c->cmd()->indirectsymoff)),c->cmd()->indirectsymoff,"Indirect Symbol Table Offset",AsString(c->cmd()->indirectsymoff));
    t->AddRow(c->GetRAW(&(c->cmd()->nindirectsyms)),c->cmd()->nindirectsyms,"Indirect Symbol Table Entries",AsString(c->cmd()->nindirectsyms));

    t->AddRow(c->GetRAW(&(c->cmd()->extreloff)),c->cmd()->extreloff,"ExtReloc Table Offset",AsString(c->cmd()->extreloff));
    t->AddRow(c->GetRAW(&(c->cmd()->nextrel)),c->cmd()->nextrel,"ExtReloc Table Entries",AsString(c->cmd()->nextrel));

    t->AddRow(c->GetRAW(&(c->cmd()->locreloff)),c->cmd()->locreloff,"LocReloc Table Offset",AsString(c->cmd()->locreloff));
    t->AddRow(c->GetRAW(&(c->cmd()->nlocrel)),c->cmd()->nlocrel,"LocReloc Table Entries",AsString(c->cmd()->nlocrel));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_LOAD_DYLINKER)
    t->AddRow(c->GetRAW(&(c->cmd()->name.offset)),c->cmd()->name.offset,"Str Offset",AsShortHexString(c->cmd()->name.offset));
    t->AddSeparator();

    t->AddRow(c->GetRAW((void*)(c->dylinker_path_name_offset())),AsHexData((void*)(c->dylinker_path_name_offset()),(std::size_t)(c->dylinker_path_name().length())),"Name",c->dylinker_path_name());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_UUID)
    t->AddRow(c->GetRAW(&(c->cmd()->uuid)),c->cmd()->uuid,sizeof(c->cmd()->uuid),"UUID",c->GetUUIDString());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(VERSION_MIN)
    t->AddRow(c->GetRAW(&(c->cmd()->version)),c->cmd()->version,"Version",c->GetVersion());
    t->AddRow(c->GetRAW(&(c->cmd()->sdk)),c->cmd()->sdk,"SDK",c->GetSDK());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_SOURCE_VERSION)
    t->AddRow(c->GetRAW(&(c->cmd()->version)),c->cmd()->version,"Version",c->GetVersion());
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_MAIN)
    t->AddRow(c->GetRAW(&(c->cmd()->entryoff)),c->cmd()->entryoff,"Entry Offset",AsShortHexString(c->cmd()->entryoff));
    t->AddRow(c->GetRAW(&(c->cmd()->stacksize)),c->cmd()->stacksize,"Stack Size",AsShortHexString(c->cmd()->stacksize));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_ENCRYPTION_INFO)
    t->AddRow(c->GetRAW(&(c->cmd()->cryptoff)),c->cmd()->cryptoff,"Crypt Offset",AsShortHexString(c->cmd()->cryptoff));
    t->AddRow(c->GetRAW(&(c->cmd()->cryptsize)),c->cmd()->cryptsize,"Crypt Size",AsShortHexString(c->cmd()->cryptsize));
    t->AddRow(c->GetRAW(&(c->cmd()->cryptid)),c->cmd()->cryptid,"Crypt ID",AsShortHexString(c->cmd()->cryptid));
IMPL_LOADCOMMAND_VIEWNODE_END
IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LC_ENCRYPTION_INFO_64)
    t->AddRow(c->GetRAW(&(c->cmd()->cryptoff)),c->cmd()->cryptoff,"Crypt Offset",AsShortHexString(c->cmd()->cryptoff));
    t->AddRow(c->GetRAW(&(c->cmd()->cryptsize)),c->cmd()->cryptsize,"Crypt Size",AsShortHexString(c->cmd()->cryptsize));
    t->AddRow(c->GetRAW(&(c->cmd()->cryptid)),c->cmd()->cryptid,"Crypt ID",AsShortHexString(c->cmd()->cryptid));
IMPL_LOADCOMMAND_VIEWNODE_END

IMPL_LOADCOMMAND_VIEWNODE_BEGIN(LINKEDIT_DATA)
    t->AddRow(c->GetRAW(&(c->cmd()->dataoff)),c->cmd()->dataoff,"Data Offset",AsShortHexString(c->cmd()->dataoff));
    t->AddRow(c->GetRAW(&(c->cmd()->datasize)),c->cmd()->datasize,"Data Size",AsShortHexString(c->cmd()->datasize));
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