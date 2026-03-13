//
// Created by everettjf on 2017/7/7.
//

#include "Binary.h"
#include <cerrno>
#include <cstring>
#include <string>

#if defined(_WIN32)
#include <sys/stat.h>
#define MOEX_STAT _stat
#else
#include <sys/stat.h>
#define MOEX_STAT stat
#endif

MOEX_NAMESPACE_BEGIN

namespace {

static bool EndsWith(const std::string &value, const std::string &suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static bool IsRegularFilePath(const std::string &filepath, bool &is_directory, std::string &error) {
    is_directory = false;

    struct MOEX_STAT st {};
    if (MOEX_STAT(filepath.c_str(), &st) != 0) {
        error = std::string("cannot stat path: ") + std::strerror(errno);
        return false;
    }

#if defined(_WIN32)
    is_directory = (st.st_mode & _S_IFDIR) != 0;
    return (st.st_mode & _S_IFREG) != 0;
#else
    is_directory = S_ISDIR(st.st_mode);
    return S_ISREG(st.st_mode);
#endif
}

static std::string DescribeUnsupportedPath(const std::string &filepath, bool is_directory) {
    if (is_directory && EndsWith(filepath, ".app")) {
        return "Application bundles are directories; open the Mach-O inside .app/Contents/MacOS instead";
    }
    if (is_directory) {
        return "Path is a directory, not a binary file";
    }
    return "Path is not a regular file";
}

} // namespace


Binary::Binary(const std::string & filepath)
    : filepath_(filepath){

    if(filepath_.length() == 0){
        throw NodeException("invalid file path");
    }

    bool is_directory = false;
    std::string stat_error;
    if (!IsRegularFilePath(filepath_, is_directory, stat_error)) {
        if (!stat_error.empty() && !is_directory) {
            throw NodeException(stat_error);
        }
        throw NodeException(DescribeUnsupportedPath(filepath_, is_directory));
    }

    // Mapping file
    const char * file_name = filepath_.c_str();

    try {
        map_ = std::make_unique<mmaplib::MemoryMappedFile>(file_name);
    }catch (...){
        throw NodeException("Can not open file");
    }

    if (!map_ || !map_->is_open() || map_->data() == nullptr || map_->data() == MAP_FAILED) {
        throw NodeException("Can not map file");
    }

    void *addr = (void*)map_->data();
    std::size_t size = map_->size();

    if (size == 0) {
        throw NodeException("File is empty");
    }

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
