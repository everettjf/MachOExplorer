//
// Created by everettjf on 2017/7/20.
//
#include "LoadCommand_DYSYMTAB.h"
#include "../MachHeader.h"
#include "LoadCommand_SYMTAB.h"

MOEX_NAMESPACE_BEGIN




std::tuple<bool, uint32_t, uint32_t> LoadCommand_LC_DYSYMTAB::GetDataRange()
{
    if(cmd_->tocoff>0)
        return std::make_tuple(true,cmd_->tocoff,cmd_->ntoc * sizeof(struct dylib_table_of_contents));
    if(cmd_->modtaboff>0){
        if(header_->Is64())
            return std::make_tuple(true,cmd_->modtaboff,cmd_->nmodtab * sizeof(struct dylib_module_64));
        else
            return std::make_tuple(true,cmd_->modtaboff,cmd_->nmodtab * sizeof(struct dylib_module));
    }
    if(cmd_->extrefsymoff>0)
        return std::make_tuple(true,cmd_->extrefsymoff,cmd_->nextrefsyms * sizeof(struct dylib_reference));
    if(cmd_->indirectsymoff>0)
        return std::make_tuple(true,cmd_->indirectsymoff,cmd_->nindirectsyms * sizeof(uint32_t));
    if(cmd_->extreloff>0)
        return std::make_tuple(true,cmd_->extreloff,cmd_->nextrel * sizeof(struct relocation_info));
    if(cmd_->locreloff>0)
        return std::make_tuple(true,cmd_->locreloff,cmd_->nlocrel * sizeof(struct relocation_info));
    return std::make_tuple(false,0,0);
}

void LoadCommand_LC_DYSYMTAB::ForEachIndirectSymbols(std::function<void(uint32_t* indirect_index)> callback){
    char * offset = (char*)header_->header_start() + cmd_->indirectsymoff;
    uint32_t size = cmd_->nindirectsyms * sizeof(uint32_t);
    auto array = util::ParseDataAsSize(offset,size,sizeof(uint32_t));
    for(char *cur : array){
        callback((uint32_t*)(void*)cur);
    }
}

MOEX_NAMESPACE_END

