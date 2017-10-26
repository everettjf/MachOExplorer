//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
#define MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class RebaseOpcodeContext{
public:
    uint8_t *pbyte; // opcode offset
    uint8_t byte; // opcode data

    uint8_t opcode;
    uint8_t immediate;

    uint8_t type;

    uint64_t address;
    uint64_t do_rebase_location;

    std::string GetRebaseTypeString(uint8_t type);
    std::string GetRebaseTypeShortString(uint8_t type);
};
class RebaseOpcodeItem{
public:
    virtual ~RebaseOpcodeItem(){}
    virtual std::string GetName()=0;
};

class Wrap_REBASE_OPCODE_DONE : public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_DONE";};
};
class Wrap_REBASE_OPCODE_SET_TYPE_IMM: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_SET_TYPE_IMM";};
};
class Wrap_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB";};
    uint32_t segment_index=0;

    uint64_t offset=0;// value
    uint8_t *offset_addr=0;
    uint32_t offset_size=0;
};
class Wrap_REBASE_OPCODE_ADD_ADDR_ULEB: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_ADD_ADDR_ULEB";};
    uint64_t offset=0;// value
    uint8_t *offset_addr=0;
    uint32_t offset_size=0;
};
class Wrap_REBASE_OPCODE_ADD_ADDR_IMM_SCALED: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_ADD_ADDR_IMM_SCALED";};
    uint32_t scale = 0;
};
class Wrap_REBASE_OPCODE_DO_REBASE_IMM_TIMES: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_DO_REBASE_IMM_TIMES";};
    uint32_t count = 0;
};
class Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_DO_REBASE_ULEB_TIMES";};

    uint64_t count=0;
    uint8_t *count_addr=0;
    uint32_t count_size=0;
};
class Wrap_REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB";};
    uint64_t offset=0;// value
    uint8_t *offset_addr=0;
    uint32_t offset_size=0;
};
class Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB: public RebaseOpcodeItem{
public:
    std::string GetName()override{ return "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB";};

    uint64_t count=0;
    uint8_t *count_addr=0;
    uint32_t count_size=0;

    uint64_t skip=0;
    uint8_t *skip_addr=0;
    uint32_t skip_size=0;
};

class LoadCommand_DYLD_INFO : public LoadCommandImpl<dyld_info_command>{
public:
    static std::string GetRebaseTypeString(uint8_t type);
    static std::string GetRebaseTypeShortString(uint8_t type);

public:
    void ForEachRebaseOpcode(std::function<void(const RebaseOpcodeContext *ctx,RebaseOpcodeItem*item)> callback);
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
