//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H
#define MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H

#include "LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_VERSION_MIN: public LoadCommandImpl<version_min_command>{
public:
    std::string GetTypeName() override{ return "version_min_command";}
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H
