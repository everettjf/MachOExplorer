//
// Created by everettjf on 2017/7/7.
//

#include "Binary.h"

MOEX_NAMESPACE_BEGIN


Binary::Binary(const std::string & filepath)
    : filepath_(filepath){

    if(filepath_.length() == 0){
        throw NodeException("invalid file path");
    }

    // Mapping file
    const char * file_name = filepath_.c_str();

    try {
        map_ = std::make_unique<mmaplib::MemoryMappedFile>(file_name);
    }catch (...){
        throw NodeException("Can not open file");
    }

    void *addr = (void*)map_->data();
    std::size_t size = map_->size();

    // Set member
    memory_ = addr;
    memorysize_ = size;

    // Context
    NodeContextPtr context = std::make_shared<NodeContext>();
    context->file_start = memory_;
    context->file_size = memorysize_;

    // Header
    if (Archive::IsArchiveMagic(memory_, memorysize_)) {
        is_archive_ = true;
        archive_ = std::make_shared<Archive>();
        archive_->Init(memory_, memorysize_, context);
    } else if (DyldSharedCache::IsSharedCacheMagic(memory_, memorysize_)) {
        is_dyld_cache_ = true;
        dyld_cache_ = std::make_shared<DyldSharedCache>();
        dyld_cache_->Init(memory_, memorysize_, context, filepath_);
    } else {
        if (memorysize_ < sizeof(uint32_t)) {
            throw NodeException("Not a MachO file, archive, or dyld shared cache");
        }
        magic_.Parse(memory_);
        if(!magic_.IsValid()){
            throw NodeException("Not a MachO file, archive, or dyld shared cache");
        }

        if(magic_.IsFat()){
        fath_ = std::make_shared<FatHeader>();
        fath_->Init(memory_,context);
        }else{
            mh_ = std::make_shared<MachHeader>();
            mh_->Init(memory_,context);
        }
    }
}


Node *Binary::GetNode(){
    if(is_archive_)
        return archive_.get();
    else if(is_dyld_cache_)
        return dyld_cache_.get();
    else if(magic_.IsFat())
        return fath_.get();
    else
        return mh_.get();
}

void Binary::ForEachHeader(std::function<void (MachHeaderPtr)> callback){
    if(IsArchive()){
        for(auto & member : archive_->members()){
            if(member->is_macho && member->mh){
                callback(member->mh);
            }
        }
    }else if(IsDyldSharedCache()){
        // dyld shared cache is not represented as embedded MachHeader list for now.
        return;
    }else if(IsFat()){
        for(auto & arch : fath()->archs()){
            callback(arch->mh());
        }
    }else{
        callback(mh_);
    }
}
MOEX_NAMESPACE_END
