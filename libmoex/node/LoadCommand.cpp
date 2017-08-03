//
// Created by qiwei on 2017/3/29.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_FACTORY_H
#define MACHOEXPLORER_LOADCOMMAND_FACTORY_H

#include "LoadCommand.h"
#include "loadcmd/LoadCommand_SEGMENT.h"
#include "loadcmd/LoadCommand_DYLIB.h"
#include "loadcmd/LoadCommand_DYLD_INFO.h"
#include "loadcmd/LoadCommand_SYMTAB.h"
#include "loadcmd/LoadCommand_DYSYMTAB.h"
#include "loadcmd/LoadCommand_LOAD_DYLINKER.h"
#include "loadcmd/LoadCommand_UUID.h"
#include "loadcmd/LoadCommand_VERSION_MIN.h"
#include "loadcmd/LoadCommand_SOURCE_VERSION.h"
#include "loadcmd/LoadCommand_MAIN.h"
#include "loadcmd/LoadCommand_ENCRYPTION_INFO.h"
#include "loadcmd/LoadCommand_LINKEDIT_DATA.h"
#include "MachHeader.h"

MOEX_NAMESPACE_BEGIN

bool LoadCommand::is64() {
    return header_->is64();
}

std::string LoadCommand::GetLoadCommandTypeString(){
    return util::GetLoadCommandType(offset()->cmd);
}

#define DECLARE_LOAD_COMMAND_CASE_STATEMENT_CLASS(commandtag,classtag) \
case commandtag: return std::make_shared<LoadCommand_##classtag>();

#define DECLARE_LOAD_COMMAND_CASE_STATEMENT(commandtag) \
case commandtag: return std::make_shared<LoadCommand_##commandtag>();

LoadCommandPtr LoadCommandFactory::GetCommand(uint32_t cmd) {
    switch (cmd) {
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

        default:
            return std::make_shared<LoadCommand>();
    }
}

LoadCommandPtr LoadCommandFactory::Create(void * offset,NodeContextPtr & ctx,MachHeader *header){
    load_command *lc = reinterpret_cast<load_command*>(offset);
    LoadCommandPtr cmd = LoadCommandFactory::GetCommand(lc->cmd);
    cmd->set_header(header);
    cmd->Init(offset,ctx);
    return cmd;
}

MOEX_NAMESPACE_END


#endif //MACHOEXPLORER_LOADCOMMAND_FACTORY_H
