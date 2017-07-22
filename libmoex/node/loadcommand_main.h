//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_MAIN_H
#define MACHOEXPLORER_LOADCOMMAND_MAIN_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_MAIN : public LoadCommandImpl<entry_point_command>{
public:
    std::string GetTypeName() override{ return "entry_point_command";}
    uint64_t  entryoff(){return cmd_->entryoff;}
    uint64_t  stacksize(){return cmd_->stacksize;}
};


MOEX_NAMESPACE_END
#endif //MACHOEXPLORER_LOADCOMMAND_MAIN_H
