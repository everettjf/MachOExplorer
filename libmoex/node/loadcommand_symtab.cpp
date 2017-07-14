#include "loadcommand_symtab.h"
#include "machheader.h"


MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SYMTAB::LazyInit(){
    static std::once_flag f;
    std::call_once(f,[&]{
        char * symbol_offset = (char*)GetSymbolTableOffset();

        if(header_->is64()){
            for(uint32_t idx = 0; idx < GetSymbolTableSize(); ++idx){
                struct nlist64 * cur = reinterpret_cast<struct nlist64*>(symbol_offset + idx * sizeof(struct nlist));
                NList64Ptr symbol = std::make_shared<NList64>();
                symbol->Init(cur,ctx_);
                nlist64s_.push_back(symbol);
            }
        }else{
            for(uint32_t idx = 0; idx < GetSymbolTableSize(); ++idx) {
                struct nlist *cur = reinterpret_cast<struct nlist *>(symbol_offset + idx * sizeof(struct nlist));
                NListPtr symbol = std::make_shared<NList>();
                symbol->Init(cur, ctx_);
                nlists_.push_back(symbol);
            }
        }
    });
}

MOEX_NAMESPACE_END
