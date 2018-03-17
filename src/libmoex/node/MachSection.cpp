//
// Created by everettjf on 2017/7/12.
//

#include "MachSection.h"
#include "MachHeader.h"

MOEX_NAMESPACE_BEGIN

void MachSection::Init(section *offset,NodeContextPtr & ctx){
    sect_.Init(offset);
    section_ = std::make_shared<MachSectionInternal>();
    section_->Init(offset,ctx);
}

void MachSection::Init(section_64 *offset,NodeContextPtr & ctx){
    sect_.Init(offset);
    section64_ = std::make_shared<MachSection64Internal>();
    section64_->Init(offset,ctx);
}

uint64_t MachSection::GetRAW(const void *addr){
    if(Is64())
        return (uint64_t)addr - (uint64_t)section64_->ctx()->file_start;
    else
        return (uint64_t)addr - (uint64_t)section_->ctx()->file_start;
}

char *MachSection::GetOffset(){
    char *offset = (char*)header()->header_start() + sect().offset();
    return offset;
}
uint32_t MachSection::GetSize() {
    uint32_t size = (uint32_t) sect().size_both();
    return size;
}

void MachSection::ForEachAs_S_CSTRING_LITERALS(std::function<void(char* str)> callback){
    auto array = util::ParseStringLiteral(GetOffset(),GetSize());
    for(char * cur : array) {
        callback(cur);
    }
}
void MachSection::ForEachAs_N_BYTE_LITERALS(std::function<void(void* ptr)> callback, size_t unitsize){
    auto array = util::ParseDataAsSize(GetOffset(),GetSize(),unitsize);
    for(char *cur : array) {
        callback((void*)cur);
    }
}

void MachSection::ForEachAs_S_4BYTE_LITERALS(std::function<void(void* ptr)> callback){
    ForEachAs_N_BYTE_LITERALS(callback,4);
}

void MachSection::ForEachAs_S_8BYTE_LITERALS(std::function<void(void* ptr)> callback){
    ForEachAs_N_BYTE_LITERALS(callback,8);
}

void MachSection::ForEachAs_S_16BYTE_LITERALS(std::function<void(void* ptr)> callback){
    ForEachAs_N_BYTE_LITERALS(callback,16);
}

void MachSection::ForEachAs_POINTERS(std::function<void(void* ptr)> callback){
    if(Is64()) {
        auto array = util::ParsePointerAsType<uint64_t>(GetOffset(), GetSize());
        for (uint64_t *cur : array) {
            callback(cur);
        }
    }else{
        auto array = util::ParsePointerAsType<uint32_t>(GetOffset(), GetSize());
        for (uint32_t *cur : array) {
            callback(cur);
        }
    }
}
void MachSection::ForEachAs_S_LITERAL_POINTERS(std::function<void(void* ptr)> callback){
    ForEachAs_POINTERS(callback);
}
void MachSection::ForEachAs_S_MOD_INIT_FUNC_POINTERS(std::function<void(void* ptr)> callback){
    ForEachAs_POINTERS(callback);
}
void MachSection::ForEachAs_S_MOD_TERM_FUNC_POINTERS(std::function<void(void* ptr)> callback){
    ForEachAs_POINTERS(callback);
}

void MachSection::ForEachAs_S_LAZY_SYMBOL_POINTERS(std::function<void(void* ptr)> callback){
    ForEachAs_POINTERS(callback);
}
void MachSection::ForEachAs_S_NON_LAZY_SYMBOL_POINTERS(std::function<void(void* ptr)> callback){
    ForEachAs_POINTERS(callback);
}
void MachSection::ForEachAs_S_LAZY_DYLIB_SYMBOL_POINTERS(std::function<void(void* ptr)> callback){
    ForEachAs_POINTERS(callback);
}
void MachSection::ForEachAs_S_SYMBOL_STUBS(std::function<void(void* ptr,size_t unitsize)> callback){
    size_t unitsize = sect().reserved2();
    auto array = util::ParseDataAsSize(GetOffset(),GetSize(),unitsize);
    for(char *cur : array){
        callback(cur,unitsize);
    }
}
void MachSection::ForEachAs_ObjC2Pointer(std::function<void(void* ptr)> callback){
    if(Is64()) {
        auto array = util::ParsePointerAsType<uint64_t>(GetOffset(), GetSize());
        for (uint64_t *cur : array) {
            callback(cur);
        }
    }else{
        auto array = util::ParsePointerAsType<uint32_t>(GetOffset(), GetSize());
        for (uint32_t *cur : array) {
            callback(cur);
        }
    }
}
void MachSection::ParseAsObjCImageInfo(std::function<void(objc_image_info* ptr)> callback){
    callback((objc_image_info*)(void*)GetOffset());
}

MOEX_NAMESPACE_END
