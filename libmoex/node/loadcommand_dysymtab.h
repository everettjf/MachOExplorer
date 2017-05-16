//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H
#define MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_DYSYMTAB : public LoadCommandImpl<dysymtab_command>{
public:
    std::string GetTypeName() override{ return "dysymtab_command";}
    std::string GetDisplayName() override{ return "dysymtab";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,index to local symbols=%4%, number of local symbols =%5%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->ilocalsym
                          % cmd_->nlocalsym
        );
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H
