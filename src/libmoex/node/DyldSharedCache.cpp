#include "DyldSharedCache.h"
#include <cstring>

MOEX_NAMESPACE_BEGIN

bool DyldSharedCache::IsSharedCacheMagic(const void *data, std::size_t size)
{
    if (data == nullptr || size < 16) return false;
    const char *p = reinterpret_cast<const char *>(data);
    return std::strncmp(p, "dyld_v1", 7) == 0;
}

void DyldSharedCache::Init(void *offset, std::size_t size, NodeContextPtr &ctx)
{
    if (offset == nullptr || size < sizeof(dyld_cache_header_min)) {
        throw NodeException("Malformed dyld shared cache: header truncated");
    }
    ctx_ = ctx;
    base_ = reinterpret_cast<char *>(offset);
    memcpy(&header_, offset, sizeof(header_));

    auto in_range = [&](uint64_t off, uint64_t sz) -> bool {
        return off <= size && sz <= (size - off);
    };

    const uint64_t map_bytes = static_cast<uint64_t>(header_.mappingCount) * sizeof(dyld_cache_mapping_info_min);
    if (!in_range(header_.mappingOffset, map_bytes)) {
        throw NodeException("Malformed dyld shared cache: mapping array out of range");
    }
    const auto *map_ptr = reinterpret_cast<const dyld_cache_mapping_info_min *>(base_ + header_.mappingOffset);
    mappings_.assign(map_ptr, map_ptr + header_.mappingCount);

    const uint64_t image_bytes = static_cast<uint64_t>(header_.imagesCount) * sizeof(dyld_cache_image_info_min);
    if (!in_range(header_.imagesOffset, image_bytes)) {
        throw NodeException("Malformed dyld shared cache: image array out of range");
    }
    const auto *img_ptr = reinterpret_cast<const dyld_cache_image_info_min *>(base_ + header_.imagesOffset);
    images_.assign(img_ptr, img_ptr + header_.imagesCount);
}

std::string DyldSharedCache::ReadPathByOffset(uint32_t file_offset) const
{
    if (ctx_ == nullptr) return "";
    if (file_offset >= ctx_->file_size) return "";
    const char *s = reinterpret_cast<const char *>(ctx_->file_start) + file_offset;
    const uint64_t remain = ctx_->file_size - file_offset;
    uint64_t len = 0;
    while (len < remain && s[len] != '\0') ++len;
    if (len == remain) return "";
    return std::string(s, static_cast<size_t>(len));
}

MOEX_NAMESPACE_END
