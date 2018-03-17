//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
#define MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

struct Uleb128Data{
    uint64_t offset;
    uint32_t occupy_size;
    uint64_t data;
};
//-------------------------------------------------


class LoadCommand_LC_CODE_SIGNATURE : public LoadCommandImpl<linkedit_data_command>{
public:
};

//-------------------------------------------------
class LoadCommand_LC_SEGMENT_SPLIT_INFO: public LoadCommandImpl<linkedit_data_command>{
public:
};
//-------------------------------------------------


class LoadCommand_LC_FUNCTION_STARTS: public LoadCommandImpl<linkedit_data_command>{
private:
    std::vector<Uleb128Data> functions_;
public:
    std::vector<Uleb128Data> & GetFunctions();

};

//-------------------------------------------------
class DataInCodeEntry: public NodeOffset<data_in_code_entry>{
public:
    std::string GetKindString();
};
using DataInCodeEntryPtr = std::shared_ptr<DataInCodeEntry>;

class LoadCommand_LC_DATA_IN_CODE: public LoadCommandImpl<linkedit_data_command>{
private:
    std::vector<DataInCodeEntryPtr> dices_;
public:
    std::vector<DataInCodeEntryPtr> &GetDices();
};
//-------------------------------------------------
class LoadCommand_LC_DYLIB_CODE_SIGN_DRS: public LoadCommandImpl<linkedit_data_command>{
public:
};
//-------------------------------------------------
class LoadCommand_LC_LINKER_OPTIMIZATION_HINT: public LoadCommandImpl<linkedit_data_command>{
public:
};
//-------------------------------------------------
MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
