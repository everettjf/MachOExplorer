//
// Minimal dyld shared cache parser.
//

#ifndef MOEX_DYLDSHAREDCACHE_H
#define MOEX_DYLDSHAREDCACHE_H

#include "Node.h"

MOEX_NAMESPACE_BEGIN

struct dyld_cache_header_min {
    char magic[16];
    uint32_t mappingOffset;
    uint32_t mappingCount;
    uint32_t imagesOffset;
    uint32_t imagesCount;
    uint64_t dyldBaseAddress;
    uint64_t codeSignatureOffset;
    uint64_t codeSignatureSize;
    uint64_t slideInfoOffsetUnused;
    uint64_t slideInfoSizeUnused;
    uint64_t localSymbolsOffset;
    uint64_t localSymbolsSize;
    uint8_t uuid[16];
    uint64_t cacheType;
};

struct dyld_cache_mapping_info_min {
    uint64_t address;
    uint64_t size;
    uint64_t fileOffset;
    uint32_t maxProt;
    uint32_t initProt;
};

struct dyld_cache_image_info_min {
    uint64_t address;
    uint64_t modTime;
    uint64_t inode;
    uint32_t pathFileOffset;
    uint32_t pad;
};

class DyldSharedCache : public Node {
private:
    NodeContextPtr ctx_;
    char *base_ = nullptr;
    dyld_cache_header_min header_{};
    std::vector<dyld_cache_mapping_info_min> mappings_;
    std::vector<dyld_cache_image_info_min> images_;

public:
    static bool IsSharedCacheMagic(const void *data, std::size_t size);

    void Init(void *offset, std::size_t size, NodeContextPtr &ctx);

    NodeContextPtr ctx() { return ctx_; }
    char *base() { return base_; }
    const dyld_cache_header_min &header() const { return header_; }
    const std::vector<dyld_cache_mapping_info_min> &mappings() const { return mappings_; }
    const std::vector<dyld_cache_image_info_min> &images() const { return images_; }

    std::string ReadPathByOffset(uint32_t file_offset) const;

    uint64_t GetRAW(const void *addr) override {
        return reinterpret_cast<uint64_t>(addr) - reinterpret_cast<uint64_t>(ctx_->file_start);
    }
};
using DyldSharedCachePtr = std::shared_ptr<DyldSharedCache>;

MOEX_NAMESPACE_END

#endif // MOEX_DYLDSHAREDCACHE_H
