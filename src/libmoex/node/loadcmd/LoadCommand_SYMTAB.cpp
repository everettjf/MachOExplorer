#include "LoadCommand_SYMTAB.h"
#include "libmoex/node/MachHeader.h"


MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SYMTAB::LazyInit(){
    if(inited_)return;
    inited_ = true;

    // symbol list
    char * symbol_offset = (char*)GetSymbolTableOffsetAddress();
    const bool is64 = header_->Is64();
    const uint64_t entry_size = is64 ? sizeof(struct qv_nlist_64) : sizeof(struct qv_nlist);

    // Bound the entry count to what actually fits in the mapped file: a
    // truncated or crafted symtab can claim more nlist entries than exist, and
    // NList holds raw pointers into the file, so reading past the end crashes.
    uint64_t count = GetSymbolTableSize();
    if(ctx_ && ctx_->file_start != nullptr){
        const char *fstart = static_cast<const char*>(ctx_->file_start);
        if(symbol_offset < fstart){
            count = 0;
        }else{
            const uint64_t off_in_file = static_cast<uint64_t>(symbol_offset - fstart);
            const uint64_t avail = off_in_file <= ctx_->file_size
                                       ? (ctx_->file_size - off_in_file) / entry_size
                                       : 0;
            if(count > avail) count = avail;
        }
    }

    if(is64){
        for(uint64_t idx = 0; idx < count; ++idx){
            struct qv_nlist_64 * cur = reinterpret_cast<struct qv_nlist_64*>(symbol_offset + idx * sizeof(struct qv_nlist_64));
            NListPtr symbol = std::make_shared<NList>();
            symbol->Init(cur,ctx_,true);
            nlists_.push_back(symbol);
        }
    }else{
        for(uint64_t idx = 0; idx < count; ++idx) {
            struct qv_nlist *cur = reinterpret_cast<struct qv_nlist *>(symbol_offset + idx * sizeof(struct qv_nlist));
            NListPtr symbol = std::make_shared<NList>();
            symbol->Init(cur, ctx_,false);
            nlists_.push_back(symbol);
        }
    }

}

void NList::Init(void *offset, NodeContextPtr &ctx, bool is64){
    is64_ = is64;
    if(is64_){
        nlist64_ = std::make_shared<NList64Internal>();
        nlist64_->Init(offset,ctx);
    }else{
        nlist_ = std::make_shared<NListInternal>();
        nlist_->Init(offset,ctx);
    }
}

MOEX_NAMESPACE_END
