//
// Created by everettjf on 2017/4/2.
//

#ifndef MACHOEXPLORER_MACHSECTION_H
#define MACHOEXPLORER_MACHSECTION_H

#include "Node.h"

MOEX_NAMESPACE_BEGIN

class moex_section{
private:
    section *s=nullptr;
    section_64 *s64=nullptr;
    bool is64=false;
public:
    moex_section(){}

    void Init(section *sect){s=sect;is64=false;}
    void Init(section_64 *sect){s64=sect;is64=true;}

    bool Is64()const{return is64;}

    // shared
    char   	    (&sectname())[16]{return is64?s64->sectname:s->sectname;}
    char		(&segname())[16]{return is64?s64->segname:s->segname;}
    uint32_t	&offset(){return is64?s64->offset:s->offset;}
    uint32_t	&align(){return is64?s64->align:s->align;}
    uint32_t	&reloff(){return is64?s64->reloff:s->reloff;}
    uint32_t	&nreloc(){return is64?s64->nreloc:s->nreloc;}
    uint32_t	&flags(){return is64?s64->flags:s->flags;}
    uint32_t	&reserved1(){return is64?s64->reserved1:s->reserved1;}
    uint32_t	&reserved2(){return is64?s64->reserved2:s->reserved2;}

    // 32
    uint32_t	&addr(){return s->addr;}
    uint32_t	&size(){return s->size;}

    // 64
    uint64_t	&addr64(){return s64->addr;}
    uint64_t	&size64(){return s64->size;}
    uint32_t	&reserved3(){return s64->reserved3;}

    // helper
    std::string segment_name() {return std::string(segname(),16).c_str();}
    std::string section_name() {return std::string(sectname(),16).c_str();}
};


class MachSectionInternal : public NodeOffset<section>{
public:
    std::string GetTypeName() override{ return "section";}
};
using MachSectionInternalPtr = std::shared_ptr<MachSectionInternal>;

class MachSection64Internal : public NodeOffset<section_64>{
public:
    std::string GetTypeName() override{ return "section_64";}
};
using MachSection64InternalPtr = std::shared_ptr<MachSection64Internal>;


class MachSection : public Node{
private:
    MachSectionInternalPtr section_;
    MachSection64InternalPtr section64_;
    moex_section sect_;
    bool is64_;
public:
    bool Is64()const{return is64_;}

    void Init(section *offset,NodeContextPtr & ctx);
    void Init(section_64 *offset,NodeContextPtr & ctx);

    std::string GetTypeName() override{ return is64_ ? section64_->GetTypeName() : section_->GetTypeName();}

    std::size_t DATA_SIZE(){return is64_?section64_->DATA_SIZE():section_->DATA_SIZE();}
    NodeContextPtr ctx(){return is64_?section64_->ctx():section_->ctx();}

    void *offset(){return is64_?(void*)(section64_->offset()):(void*)(section_->offset());}
    moex_section & sect(){return sect_;}

    uint64_t GetRAW(const void * addr){
        if(is64_)
            return (uint64_t)addr - (uint64_t)section64_->ctx()->file_start;
        else
            return (uint64_t)addr - (uint64_t)section_->ctx()->file_start;
    }
};
using MachSectionPtr = std::shared_ptr<MachSection>;

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_MACHSECTION_H
