//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
#define MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

/////////////////////////////Rebase /////////////////////////////////
class RebaseOpcodeContext{
public:
    uint8_t opcode;
    uint8_t immediate;

    uint8_t *pbyte; // opcode offset
    uint8_t byte; // opcode data

    uint8_t type;

    uint64_t address;
    uint64_t do_rebase_location;

    std::string GetRebaseTypeString()const;
    std::string GetRebaseTypeShortString()const;
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

/////////////////////////////Binding /////////////////////////////////

class BindingOpcodeContext{
public:
    uint8_t opcode;
    uint8_t immediate;

    uint8_t *pbyte; // opcode offset
    uint8_t byte; // opcode data

    uint8_t type;

    uint64_t address;
    uint64_t do_bind_location;

    std::string GetBindTypeString()const;
    std::string GetBindTypeShortString()const;
};
class BindingOpcodeItem{
public:
    virtual ~BindingOpcodeItem(){}
    virtual std::string GetName()=0;
};

class Wrap_BIND_OPCODE_DONE : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_DONE";};
};

class Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_DYLIB_ORDINAL_IMM";};

    uint64_t lib_oridinal=0;
};
class Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB";};
    uint64_t lib_oridinal=0;
    uint8_t *lib_oridinal_addr=0;
    uint32_t lib_oridinal_size=0;
};
class Wrap_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_DYLIB_SPECIAL_IMM";};

    uint64_t lib_oridinal=0;
};
class Wrap_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM";};

    uint64_t symbol_flags=0;

    std::string symbol_name;
    uint8_t *symbol_name_addr=0;
    uint32_t symbol_name_size=0;
};
class Wrap_BIND_OPCODE_SET_TYPE_IMM : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_TYPE_IMM";};

};
class Wrap_BIND_OPCODE_SET_ADDEND_SLEB : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_ADDEND_SLEB";};

    int64_t addend=0;
    uint8_t *addend_addr=0;
    uint32_t addend_size=0;
};
class Wrap_BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB";};

    uint32_t segment_index=0;

    uint64_t offset=0;
    uint8_t *offset_addr=0;
    uint32_t offset_size=0;
};
class Wrap_BIND_OPCODE_ADD_ADDR_ULEB : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_ADD_ADDR_ULEB";};

    uint64_t offset=0;
    uint8_t *offset_addr=0;
    uint32_t offset_size=0;
};
class Wrap_BIND_OPCODE_DO_BIND : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_DO_BIND";};

};
class Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB";};

    uint64_t offset=0;
    uint8_t *offset_addr=0;
    uint32_t offset_size=0;
};
class Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED";};
    uint32_t scale = 0;
};
class Wrap_BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB : public BindingOpcodeItem{
public:
    std::string GetName()override{ return "BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB";};

    uint64_t count=0;
    uint8_t *count_addr=0;
    uint32_t count_size=0;

    uint64_t skip=0;
    uint8_t *skip_addr=0;
    uint32_t skip_size=0;
};

/////////////////////////////Export /////////////////////////////////
class ExportContext{
public:
};
class ExportItem{
public:
    uint8_t terminal_size;
    uint8_t *terminal_size_addr;

    // valid if terminal_size > 0
    uint64_t flags;
    uint8_t *flags_addr;
    uint32_t flags_size;

    // valid if terminal_size > 0
    uint64_t offset;
    uint8_t *offset_addr;
    uint32_t offset_size;

    uint8_t child_count;
    uint8_t *child_count_addr;

    std::vector<std::tuple<std::string,std::string>> GetFlags();
};

class ExportChildItem{
public:
    std::string label;
    uint8_t * label_addr;
    uint32_t label_size;

    uint64_t skip;
    uint8_t *skip_addr;
    uint32_t skip_size;
};

/////////////////////////////Command /////////////////////////////////
class LoadCommand_DYLD_INFO : public LoadCommandImpl<dyld_info_command>{
public:
    static std::string GetRebaseTypeString(uint8_t type);
    static std::string GetRebaseTypeShortString(uint8_t type);
    static std::string GetBindTypeString(uint8_t type);
    static std::string GetBindTypeShortString(uint8_t type);

public:
    void ForEachRebaseOpcode(std::function<void(const RebaseOpcodeContext *ctx,RebaseOpcodeItem*item)> callback);

    enum BindNodeType {NodeTypeBind, NodeTypeWeakBind, NodeTypeLazyBind};
    void ForEachBindingOpcode(BindNodeType node_type,uint32_t bind_off,uint32_t bind_size,std::function<void(const BindingOpcodeContext *ctx,BindingOpcodeItem*item)> callback);

    void ForEachExportItem(std::function<void(const ExportContext *ctx,ExportItem* item,ExportChildItem* child)> callback);
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
