//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H
#define MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_DYSYMTAB : public LoadCommandImpl<qv_dysymtab_command>{
public:
    // valid,offset,size
    std::tuple<bool,uint32_t,uint32_t> GetDataRange();
    bool ExistIndirectSymbols(){return cmd()->nindirectsyms > 0;}
    bool ExistExternalRelocations(){return cmd()->extreloff > 0 && cmd()->nextrel > 0;}
    bool ExistLocalRelocations(){return cmd()->locreloff > 0 && cmd()->nlocrel > 0;}
    void ForEachIndirectSymbols(std::function<void(uint32_t* indirect_index)> callback);
    void ForEachExternalRelocations(std::function<void(char *entry, uint32_t index)> callback);
    void ForEachLocalRelocations(std::function<void(char *entry, uint32_t index)> callback);
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYSYMTAB_H
