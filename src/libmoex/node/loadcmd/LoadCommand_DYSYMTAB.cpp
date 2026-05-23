//
// Created by everettjf on 2017/7/20.
//
#include "LoadCommand_DYSYMTAB.h"
#include "../MachHeader.h"
#include "LoadCommand_SYMTAB.h"

MOEX_NAMESPACE_BEGIN

namespace {
// Clamp [offset, offset+size) to the mapped file so a truncated or crafted
// table cannot make the iterators below dereference past the end.
uint64_t ClampSizeToFile(const NodeContextPtr &ctx, const char *offset, uint64_t size){
    if(!ctx || ctx->file_start == nullptr)
        return size;
    const char *fstart = static_cast<const char*>(ctx->file_start);
    if(offset < fstart)
        return 0;
    const uint64_t off = static_cast<uint64_t>(offset - fstart);
    if(off >= ctx->file_size)
        return 0;
    const uint64_t avail = ctx->file_size - off;
    return size < avail ? size : avail;
}
} // namespace

std::tuple<bool, uint32_t, uint32_t> LoadCommand_LC_DYSYMTAB::GetDataRange()
{
    if(cmd_->tocoff>0)
        return std::make_tuple(true,cmd_->tocoff,cmd_->ntoc * sizeof(struct qv_dylib_table_of_contents));
    if(cmd_->modtaboff>0){
        if(header_->Is64())
            return std::make_tuple(true,cmd_->modtaboff,cmd_->nmodtab * sizeof(struct qv_dylib_module_64));
        else
            return std::make_tuple(true,cmd_->modtaboff,cmd_->nmodtab * sizeof(struct qv_dylib_module));
    }
    if(cmd_->extrefsymoff>0)
        return std::make_tuple(true,cmd_->extrefsymoff,cmd_->nextrefsyms * sizeof(struct qv_dylib_reference));
    if(cmd_->indirectsymoff>0)
        return std::make_tuple(true,cmd_->indirectsymoff,cmd_->nindirectsyms * sizeof(uint32_t));
    if(cmd_->extreloff>0)
        return std::make_tuple(true,cmd_->extreloff,cmd_->nextrel * sizeof(struct qv_relocation_info));
    if(cmd_->locreloff>0)
        return std::make_tuple(true,cmd_->locreloff,cmd_->nlocrel * sizeof(struct qv_relocation_info));
    return std::make_tuple(false,0,0);
}

void LoadCommand_LC_DYSYMTAB::ForEachIndirectSymbols(std::function<void(uint32_t* indirect_index)> callback){
    char * offset = (char*)header_->header_start() + cmd_->indirectsymoff;
    uint64_t size = static_cast<uint64_t>(cmd_->nindirectsyms) * sizeof(uint32_t);
    size = ClampSizeToFile(ctx(), offset, size);
    auto array = util::ParseDataAsSize(offset,(uint32_t)size,sizeof(uint32_t));
    for(char *cur : array){
        callback((uint32_t*)(void*)cur);
    }
}

void LoadCommand_LC_DYSYMTAB::ForEachExternalRelocations(std::function<void(char *entry, uint32_t index)> callback){
    if(!ExistExternalRelocations()) return;

    char *offset = (char*)header_->header_start() + cmd_->extreloff;
    const uint32_t entry_size = sizeof(struct qv_relocation_info);
    const uint64_t avail = ClampSizeToFile(ctx(), offset, static_cast<uint64_t>(cmd_->nextrel) * entry_size);
    const uint64_t count = avail / entry_size;
    for(uint64_t i = 0; i < count; ++i){
        callback(offset + i * entry_size, (uint32_t)i);
    }
}

void LoadCommand_LC_DYSYMTAB::ForEachLocalRelocations(std::function<void(char *entry, uint32_t index)> callback){
    if(!ExistLocalRelocations()) return;

    char *offset = (char*)header_->header_start() + cmd_->locreloff;
    const uint32_t entry_size = sizeof(struct qv_relocation_info);
    const uint64_t avail = ClampSizeToFile(ctx(), offset, static_cast<uint64_t>(cmd_->nlocrel) * entry_size);
    const uint64_t count = avail / entry_size;
    for(uint64_t i = 0; i < count; ++i){
        callback(offset + i * entry_size, (uint32_t)i);
    }
}

MOEX_NAMESPACE_END
