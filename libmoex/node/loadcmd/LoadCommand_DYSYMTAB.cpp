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

std::vector<IndirectSymbolPtr> &LoadCommand_LC_DYSYMTAB::GetIndirectSymbols()
{
    if(!indirect_symbols_.empty()){
        return indirect_symbols_;
    }

    char * offset = (char*)header_->header_start() + cmd_->indirectsymoff;
    uint32_t curindex = 0;
    for(uint32_t nindsym = 0; nindsym < cmd_->nindirectsyms; ++nindsym){
        auto &sections = header_->sections();
        MachSectionPtr findsection;
        for(auto iter = sections.rbegin(); iter != sections.rend(); iter++){
            MachSectionPtr section(*iter);
            moex_section & sect = section->sect();
            uint32_t type = sect.flags() & SECTION_TYPE;
            if((type != S_SYMBOL_STUBS
                    && type != S_LAZY_SYMBOL_POINTERS
                    && type != S_LAZY_DYLIB_SYMBOL_POINTERS
                    && type != S_NON_LAZY_SYMBOL_POINTERS
               )|| sect.reserved1() > nindsym){
                // what is reserved1
                continue;
            }
            // found the section

            findsection = section;
        }

        if(findsection.get() == nullptr) {
            continue;
        }

        moex_section & sect = findsection->sect();
        // stub or pointer length
        uint32_t length = sect.reserved2() > 0 ? sect.reserved2() : sizeof(uint32_t);

        // indirect value location
        LoadCommand_LC_SYMTAB *symtab = header_->symtab();
        if(sect.Is64()){
            uint64_t indirect_address = sect.addr64() + (nindsym - sect.reserved1()) * length;

            // accumulate search info
            uint32_t *p = (uint32_t*)(offset + curindex * sizeof(uint32_t));
            uint32_t indirect_index = *p;
            if((indirect_index & ( INDIRECT_SYMBOL_LOCAL | INDIRECT_SYMBOL_ABS)) == 0){
                if(indirect_index >= symtab->nlists_ref().size()){
                    // error
                }

                uint32_t str_index = symtab->nlists_ref()[indirect_index]->n_strx();
                std::string symbol_name = symtab->GetStringByStrX(str_index);

            }else{

            }


        }
    }
    return indirect_symbols_;
}

MOEX_NAMESPACE_END

