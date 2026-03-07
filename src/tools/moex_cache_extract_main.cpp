#include "libmoex/node/Binary.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace {

struct SegmentCopyPlan {
    std::string name;
    uint64_t vmaddr = 0;
    uint64_t source_fileoff = 0;
    uint64_t target_fileoff = 0;
    uint64_t size = 0;
};

static bool VmToFileOffset(const moex::DyldSharedCachePtr &cache, uint64_t vmaddr, uint64_t &fileoff) {
    for (const auto &m : cache->mappings()) {
        if (vmaddr >= m.address && vmaddr < m.address + m.size) {
            fileoff = m.fileOffset + (vmaddr - m.address);
            return true;
        }
    }
    return false;
}

static bool PickImage(const moex::DyldSharedCachePtr &cache, const std::string &selector, uint64_t &image_addr, std::string &image_path) {
    auto match = [&](const std::string &path) {
        if (path == selector) return true;
        return path.find(selector) != std::string::npos;
    };

    if (!cache->images().empty()) {
        for (const auto &img : cache->images()) {
            const std::string path = cache->ReadPathByOffset(img.pathFileOffset);
            if (match(path)) {
                image_addr = img.address;
                image_path = path;
                return true;
            }
        }
    } else {
        for (const auto &img : cache->map_images()) {
            if (match(img.second)) {
                image_addr = img.first;
                image_path = img.second;
                return true;
            }
        }
    }
    return false;
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "usage: moex-cache-extract <dyld_shared_cache_file> <image-path-or-substr> <output-macho>\n";
        return 2;
    }

    const std::string cache_path = argv[1];
    const std::string image_selector = argv[2];
    const std::string output_path = argv[3];

    try {
        moex::Binary bin(cache_path);
        if (!bin.IsDyldSharedCache()) {
            std::cerr << "input is not a dyld shared cache file\n";
            return 1;
        }
        auto cache = bin.dyld_cache();

        uint64_t image_addr = 0;
        std::string image_path;
        if (!PickImage(cache, image_selector, image_addr, image_path)) {
            std::cerr << "image not found: " << image_selector << "\n";
            return 1;
        }

        uint64_t header_fileoff = 0;
        if (!VmToFileOffset(cache, image_addr, header_fileoff)) {
            std::cerr << "cannot map image vmaddr to cache file offset\n";
            return 1;
        }

        const char *cache_bytes = reinterpret_cast<const char *>(cache->ctx()->file_start);
        const uint64_t cache_size = cache->ctx()->file_size;
        if (header_fileoff + sizeof(qv_mach_header_64) > cache_size) {
            std::cerr << "header out of range\n";
            return 1;
        }

        const auto *mh64 = reinterpret_cast<const qv_mach_header_64 *>(cache_bytes + header_fileoff);
        const bool is64 = (mh64->magic == MH_MAGIC_64 || mh64->magic == MH_CIGAM_64);
        const bool is32 = (mh64->magic == MH_MAGIC || mh64->magic == MH_CIGAM);
        if (!is64 && !is32) {
            std::cerr << "target image is not Mach-O\n";
            return 1;
        }

        uint32_t ncmds = 0;
        uint32_t sizeofcmds = 0;
        uint64_t mh_size = 0;
        if (is64) {
            ncmds = mh64->ncmds;
            sizeofcmds = mh64->sizeofcmds;
            mh_size = sizeof(qv_mach_header_64);
        } else {
            const auto *mh = reinterpret_cast<const qv_mach_header *>(cache_bytes + header_fileoff);
            ncmds = mh->ncmds;
            sizeofcmds = mh->sizeofcmds;
            mh_size = sizeof(qv_mach_header);
        }
        if (ncmds > 65535 || sizeofcmds > (64u * 1024u * 1024u)) {
            std::cerr << "invalid load commands metadata\n";
            return 1;
        }
        if (header_fileoff + mh_size + sizeofcmds > cache_size) {
            std::cerr << "load commands out of range\n";
            return 1;
        }

        std::vector<SegmentCopyPlan> plans;
        const char *cmd_cur = cache_bytes + header_fileoff + mh_size;
        uint32_t parsed = 0;
        for (uint32_t i = 0; i < ncmds; ++i) {
            if (parsed + sizeof(qv_load_command) > sizeofcmds) break;
            const auto *lc = reinterpret_cast<const qv_load_command *>(cmd_cur);
            if (lc->cmdsize < sizeof(qv_load_command) || parsed + lc->cmdsize > sizeofcmds) break;

            if (lc->cmd == LC_SEGMENT_64 && lc->cmdsize >= sizeof(qv_segment_command_64)) {
                const auto *seg = reinterpret_cast<const qv_segment_command_64 *>(lc);
                if (seg->filesize > 0) {
                    uint64_t src_off = 0;
                    if (VmToFileOffset(cache, seg->vmaddr, src_off)) {
                        plans.push_back({std::string(seg->segname, 16).c_str(), seg->vmaddr, src_off, seg->fileoff, seg->filesize});
                    }
                }
            } else if (lc->cmd == LC_SEGMENT && lc->cmdsize >= sizeof(qv_segment_command)) {
                const auto *seg = reinterpret_cast<const qv_segment_command *>(lc);
                if (seg->filesize > 0) {
                    uint64_t src_off = 0;
                    if (VmToFileOffset(cache, seg->vmaddr, src_off)) {
                        plans.push_back({std::string(seg->segname, 16).c_str(), seg->vmaddr, src_off, seg->fileoff, seg->filesize});
                    }
                }
            }
            parsed += lc->cmdsize;
            cmd_cur += lc->cmdsize;
        }

        if (plans.empty()) {
            std::cerr << "no loadable segments found\n";
            return 1;
        }

        uint64_t out_size = mh_size + sizeofcmds;
        for (const auto &p : plans) {
            out_size = std::max(out_size, p.target_fileoff + p.size);
            if (p.source_fileoff + p.size > cache_size) {
                std::cerr << "segment source out of range: " << p.name << "\n";
                return 1;
            }
        }

        std::fstream out(output_path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!out.good()) {
            std::cerr << "cannot open output file: " << output_path << "\n";
            return 1;
        }
        if (out_size > 0) {
            out.seekp(static_cast<std::streamoff>(out_size - 1));
            char z = 0;
            out.write(&z, 1);
        }

        out.seekp(0);
        out.write(cache_bytes + header_fileoff, static_cast<std::streamsize>(mh_size + sizeofcmds));

        constexpr uint64_t kChunk = 1024 * 1024;
        std::vector<char> buf(kChunk);
        for (const auto &p : plans) {
            uint64_t copied = 0;
            while (copied < p.size) {
                const uint64_t n = std::min<uint64_t>(kChunk, p.size - copied);
                memcpy(buf.data(), cache_bytes + p.source_fileoff + copied, static_cast<size_t>(n));
                out.seekp(static_cast<std::streamoff>(p.target_fileoff + copied));
                out.write(buf.data(), static_cast<std::streamsize>(n));
                copied += n;
            }
        }
        out.close();

        std::cout << "extracted: " << image_path << "\n";
        std::cout << "output: " << output_path << " size=" << out_size << " bytes segments=" << plans.size() << "\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "moex-cache-extract failed: " << ex.what() << "\n";
        return 1;
    }
}
