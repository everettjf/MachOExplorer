//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
#define MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H

#include "LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LINKEDIT_DATA : public LoadCommandImpl<linkedit_data_command>{
public:
    std::string GetTypeName() override{ return "linkedit_data_command";}
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
