//
// Created by everettjf on 2017/4/2.
//

#ifndef MACHOEXPLORER_MACHSECTION_H
#define MACHOEXPLORER_MACHSECTION_H

#include "Node.h"

MOEX_NAMESPACE_BEGIN

class MachHeader;

// Section wrapper : unifiy section and section_64 into one struct
class moex_section{
private:
    section *s=nullptr;
    section_64 *s64=nullptr;
    bool is64=false;
public:
    moex_section(){}

    // Init as section struct
    void Init(section *sect){s=sect;is64=false;}

    // Init as section_64 struct
    void Init(section_64 *sect){s64=sect;is64=true;}

    // Getter for is64
    bool Is64()const{return is64;}

    // Shared for 32bit and 64bit struct
    char   	    (&sectname())[16]{return is64?s64->sectname:s->sectname;}
    char		(&segname())[16]{return is64?s64->segname:s->segname;}
    uint32_t	&offset(){return is64?s64->offset:s->offset;}
    uint32_t	&align(){return is64?s64->align:s->align;}
    uint32_t	&reloff(){return is64?s64->reloff:s->reloff;}
    uint32_t	&nreloc(){return is64?s64->nreloc:s->nreloc;}
    uint32_t	&flags(){return is64?s64->flags:s->flags;}
    uint32_t	&reserved1(){return is64?s64->reserved1:s->reserved1;}
    uint32_t	&reserved2(){return is64?s64->reserved2:s->reserved2;}

    // 32bit
    uint32_t	&addr(){return s->addr;}
    uint32_t	&size(){return s->size;}

    // 64bit
    uint64_t	&addr64(){return s64->addr;}
    uint64_t	&size64(){return s64->size;}
    uint32_t	&reserved3(){return s64->reserved3;}

    // Get segname as string
    std::string segment_name() {return std::string(segname(),16).c_str();}
    // Get sectname as string
    std::string section_name() {return std::string(sectname(),16).c_str();}

    // Return either 32bit or 64bit as bigger type
    uint64_t	addr_both(){return is64?s64->addr:(uint64_t)s->addr;}
    uint64_t	size_both(){return is64?s64->size:(uint64_t)s->size;}
};

// Internal class for section
class MachSectionInternal : public NodeOffset<section>{
public:
};
using MachSectionInternalPtr = std::shared_ptr<MachSectionInternal>;

// Internal class for section_64
class MachSection64Internal : public NodeOffset<section_64>{
public:
};
using MachSection64InternalPtr = std::shared_ptr<MachSection64Internal>;

// Section wrapper
class MachSection : public Node{
private:
    MachSectionInternalPtr section_;
    MachSection64InternalPtr section64_;

    moex_section sect_;

protected:
    // Corresponding MachO header
    MachHeader* header_;
public:
    // Getter and setter for header
    void set_header(MachHeader* header){header_ = header;}
    MachHeader * header(){return header_;}

    bool Is64()const{return sect_.Is64();}

    // Init as section (32bit) section
    void Init(section *offset,NodeContextPtr & ctx);

    // Init as section (64bit) section
    void Init(section_64 *offset,NodeContextPtr & ctx);

    // Data size for current section type
    std::size_t DATA_SIZE(){return Is64()?section64_->DATA_SIZE():section_->DATA_SIZE();}

    // Context for current section
    NodeContextPtr ctx(){return Is64()?section64_->ctx():section_->ctx();}

    // Offset for current section
    void *offset(){return Is64()?(void*)(section64_->offset()):(void*)(section_->offset());}

    // Get wrapper struct for section
    moex_section & sect(){return sect_;}

    // Get addr offset from file beginning
    uint64_t GetRAW(const void * addr) override ;

    // Get RVA offset of current section
    char *GetOffset();

    // Get size of current section
    uint32_t GetSize();

    // str : c style string
    void ForEachAs_S_CSTRING_LITERALS(std::function<void(char* str)> callback);

    // ptr : can be cast into 4byte,8byte,16byte
    void ForEachAs_N_BYTE_LITERALS(std::function<void(void* ptr)> callback, size_t unitsize);
    void ForEachAs_S_4BYTE_LITERALS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_8BYTE_LITERALS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_16BYTE_LITERALS(std::function<void(void* ptr)> callback);

    // ptr : uint64_t when 64bit, uint32_t when 32bit
    void ForEachAs_POINTERS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_LITERAL_POINTERS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_MOD_INIT_FUNC_POINTERS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_MOD_TERM_FUNC_POINTERS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_LAZY_SYMBOL_POINTERS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_NON_LAZY_SYMBOL_POINTERS(std::function<void(void* ptr)> callback);
    void ForEachAs_S_LAZY_DYLIB_SYMBOL_POINTERS(std::function<void(void* ptr)> callback);

    void ForEachAs_S_SYMBOL_STUBS(std::function<void(void* str,size_t unitsize)> callback);

    // ptr : uint64_t when 64bit, uint32_t when 32bit
    void ForEachAs_ObjC2Pointer(std::function<void(void* ptr)> callback);


    void ParseAsObjCImageInfo(std::function<void(objc_image_info* ptr)> callback);
};
using MachSectionPtr = std::shared_ptr<MachSection>;
using MachSectionWeakPtr = std::weak_ptr<MachSection>;

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_MACHSECTION_H
