//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
#define MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN


class LoadCommand_DYLD_INFO : public LoadCommandImpl<dyld_info_command>{
public:
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
