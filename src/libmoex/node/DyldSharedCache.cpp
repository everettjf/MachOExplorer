#include "DyldSharedCache.h"
#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>

MOEX_NAMESPACE_BEGIN

bool DyldSharedCache::IsSharedCacheMagic(const void *data, std::size_t size)
{
    if (data == nullptr || size < 16) return false;
    const char *p = reinterpret_cast<const char *>(data);
    return std::strncmp(p, "dyld_v1", 7) == 0;
}

void DyldSharedCache::Init(void *offset, std::size_t size, NodeContextPtr &ctx, const std::string &filepath)
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

    if (header_.mappingCount > 1'000'000 || header_.imagesCount > 10'000'000) {
        throw NodeException("Malformed dyld shared cache: unreasonable mapping/image count");
    }
    if (header_.mappingCount == 0) {
        throw NodeException("Malformed dyld shared cache: empty mapping table");
    }
    if (header_.mappingOffset < sizeof(dyld_cache_header_min)) {
        throw NodeException("Malformed dyld shared cache: mapping offset overlaps header");
    }
    if (header_.imagesCount > 0 && header_.imagesOffset < sizeof(dyld_cache_header_min)) {
        throw NodeException("Malformed dyld shared cache: images offset overlaps header");
    }

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

    if (images_.empty()) {
        const std::string map_path = filepath + ".map";
        std::ifstream fin(map_path);
        if (fin.good()) {
            std::regex text_line("^\\s*__TEXT\\s+0x([0-9A-Fa-f]+)");
            std::string line;
            std::string pending_path;
            while (std::getline(fin, line)) {
                if (line.empty()) continue;
                if (line[0] == '/') {
                    pending_path = line;
                    map_images_.push_back({0, pending_path});
                    continue;
                }
                if (pending_path.empty()) continue;
                std::smatch m;
                if (std::regex_search(line, m, text_line) && m.size() >= 2) {
                    uint64_t addr = 0;
                    std::stringstream ss;
                    ss << std::hex << m[1].str();
                    ss >> addr;
                    if (!map_images_.empty() && map_images_.back().second == pending_path) {
                        map_images_.back().first = addr;
                    }
                    pending_path.clear();
                }
            }
        }
    }
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
