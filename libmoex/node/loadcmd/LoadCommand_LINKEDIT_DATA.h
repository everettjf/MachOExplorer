//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
#define MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_CODE_SIGNATURE : public LoadCommandImpl<linkedit_data_command>{
public:
};


class LoadCommand_LC_SEGMENT_SPLIT_INFO: public LoadCommandImpl<linkedit_data_command>{
public:
};

class LoadCommand_LC_FUNCTION_STARTS: public LoadCommandImpl<linkedit_data_command>{
public:
};

class LoadCommand_LC_DATA_IN_CODE: public LoadCommandImpl<linkedit_data_command>{
public:
};

class LoadCommand_LC_DYLIB_CODE_SIGN_DRS: public LoadCommandImpl<linkedit_data_command>{
public:
};

class LoadCommand_LC_LINKER_OPTIMIZATION_HINT: public LoadCommandImpl<linkedit_data_command>{
public:
};
MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
