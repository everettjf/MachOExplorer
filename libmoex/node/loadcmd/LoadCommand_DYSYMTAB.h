//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H
#define MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

struct IndirectSymbol{
    uint64_t offset;
    uint32_t data;

    std::string symbol;
    std::vector<std::tuple<uint32_t,std::string>> symbols;

    std::string target;
    std::string section;
    std::string indirect_address;
};
using IndirectSymbolPtr = std::shared_ptr<IndirectSymbol>;

class LoadCommand_LC_DYSYMTAB : public LoadCommandImpl<dysymtab_command>{
private:
    std::vector<IndirectSymbolPtr> indirect_symbols_;
public:
    // valid,offset,size
    std::tuple<bool,uint32_t,uint32_t> GetDataRange();
    std::vector<IndirectSymbolPtr> &GetIndirectSymbols();
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H
