#include "loadcommand_symtab.h"
#include "machheader.h"


MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_SYMTAB::LazyInit(){
    static std::once_flag f;
    std::call_once(f,[&]{
        if(header_->is64()){
            
        }else{

        }

    });
}

MOEX_NAMESPACE_END
