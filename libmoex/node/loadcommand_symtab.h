//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SYMTAB_H
#define MACHOEXPLORER_LOADCOMMAND_SYMTAB_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_SYMTAB : public LoadCommandImpl<symtab_command>{
public:
    std::string GetTypeName() override{ return "symtab_command";}
    std::string GetDisplayName() override{ return "symtab";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,symbol table offset=%4%, number of symbol entries=%5%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->symoff
                          % cmd_->nsyms
        );
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SYMTAB_H
