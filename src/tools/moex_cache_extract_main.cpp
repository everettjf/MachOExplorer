#include "libmoex/node/Binary.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#include <direct.h>
#endif

namespace {

struct SegmentCopyPlan {
    std::string name;
    uint64_t vmaddr = 0;
    uint64_t image_fileoff = 0;
    uint64_t source_fileoff = 0;
    uint64_t target_fileoff = 0;
    uint64_t size = 0;
};

static uint64_t AlignUp(uint64_t value, uint64_t alignment) {
    if (alignment == 0) return value;
    const uint64_t mask = alignment - 1;
    return (value + mask) & ~mask;
}

static bool RemapFileOffset(const std::vector<SegmentCopyPlan> &plans, uint32_t old_off, uint32_t &new_off) {
    for (const auto &p : plans) {
        if (old_off < p.image_fileoff || old_off >= p.image_fileoff + p.size) continue;
        const uint64_t mapped = p.target_fileoff + (old_off - p.image_fileoff);
        if (mapped > std::numeric_limits<uint32_t>::max()) return false;
        new_off = static_cast<uint32_t>(mapped);
        return true;
    }
    return false;
}

static void PatchLoadCommandsForCompaction(char *cmd_data, uint32_t sizeofcmds, const std::vector<SegmentCopyPlan> &plans, bool is64) {
    uint32_t parsed = 0;
    char *cur = cmd_data;
    while (parsed + sizeof(qv_load_command) <= sizeofcmds) {
        auto *lc = reinterpret_cast<qv_load_command *>(cur);
        if (lc->cmdsize < sizeof(qv_load_command) || parsed + lc->cmdsize > sizeofcmds) break;

        if (lc->cmd == LC_SEGMENT_64 && lc->cmdsize >= sizeof(qv_segment_command_64) && is64) {
            auto *seg = reinterpret_cast<qv_segment_command_64 *>(lc);
            for (const auto &p : plans) {
                if (p.vmaddr == seg->vmaddr) {
                    seg->fileoff = p.target_fileoff;
                    break;
                }
            }
        } else if (lc->cmd == LC_SEGMENT && lc->cmdsize >= sizeof(qv_segment_command) && !is64) {
            auto *seg = reinterpret_cast<qv_segment_command *>(lc);
            for (const auto &p : plans) {
                if (p.vmaddr == seg->vmaddr) {
                    seg->fileoff = static_cast<uint32_t>(p.target_fileoff);
                    break;
                }
            }
        } else if (lc->cmd == LC_SYMTAB && lc->cmdsize >= sizeof(qv_symtab_command)) {
            auto *cmd = reinterpret_cast<qv_symtab_command *>(lc);
            uint32_t mapped = 0;
            if (RemapFileOffset(plans, cmd->symoff, mapped)) cmd->symoff = mapped;
            if (RemapFileOffset(plans, cmd->stroff, mapped)) cmd->stroff = mapped;
        } else if (lc->cmd == LC_DYSYMTAB && lc->cmdsize >= sizeof(qv_dysymtab_command)) {
            auto *cmd = reinterpret_cast<qv_dysymtab_command *>(lc);
            uint32_t mapped = 0;
            if (RemapFileOffset(plans, cmd->tocoff, mapped)) cmd->tocoff = mapped;
            if (RemapFileOffset(plans, cmd->modtaboff, mapped)) cmd->modtaboff = mapped;
            if (RemapFileOffset(plans, cmd->extrefsymoff, mapped)) cmd->extrefsymoff = mapped;
            if (RemapFileOffset(plans, cmd->indirectsymoff, mapped)) cmd->indirectsymoff = mapped;
            if (RemapFileOffset(plans, cmd->extreloff, mapped)) cmd->extreloff = mapped;
            if (RemapFileOffset(plans, cmd->locreloff, mapped)) cmd->locreloff = mapped;
        } else if ((lc->cmd == LC_DYLD_INFO || lc->cmd == LC_DYLD_INFO_ONLY) && lc->cmdsize >= sizeof(qv_dyld_info_command)) {
            auto *cmd = reinterpret_cast<qv_dyld_info_command *>(lc);
            uint32_t mapped = 0;
            if (RemapFileOffset(plans, cmd->rebase_off, mapped)) cmd->rebase_off = mapped;
            if (RemapFileOffset(plans, cmd->bind_off, mapped)) cmd->bind_off = mapped;
            if (RemapFileOffset(plans, cmd->weak_bind_off, mapped)) cmd->weak_bind_off = mapped;
            if (RemapFileOffset(plans, cmd->lazy_bind_off, mapped)) cmd->lazy_bind_off = mapped;
            if (RemapFileOffset(plans, cmd->export_off, mapped)) cmd->export_off = mapped;
        } else if ((lc->cmd == LC_FUNCTION_STARTS ||
                    lc->cmd == LC_DATA_IN_CODE ||
                    lc->cmd == LC_CODE_SIGNATURE ||
                    lc->cmd == LC_SEGMENT_SPLIT_INFO ||
                    lc->cmd == LC_DYLIB_CODE_SIGN_DRS ||
                    lc->cmd == LC_LINKER_OPTIMIZATION_HINT ||
                    lc->cmd == LC_DYLD_EXPORTS_TRIE ||
                    lc->cmd == LC_DYLD_CHAINED_FIXUPS) &&
                   lc->cmdsize >= sizeof(qv_linkedit_data_command)) {
            auto *cmd = reinterpret_cast<qv_linkedit_data_command *>(lc);
            uint32_t mapped = 0;
            if (RemapFileOffset(plans, cmd->dataoff, mapped)) cmd->dataoff = mapped;
        } else if (lc->cmd == LC_TWOLEVEL_HINTS && lc->cmdsize >= sizeof(qv_twolevel_hints_command)) {
            auto *cmd = reinterpret_cast<qv_twolevel_hints_command *>(lc);
            uint32_t mapped = 0;
            if (RemapFileOffset(plans, cmd->offset, mapped)) cmd->offset = mapped;
        }

        parsed += lc->cmdsize;
        cur += lc->cmdsize;
    }
}

static bool VmToFileOffset(const moex::DyldSharedCachePtr &cache, uint64_t vmaddr, uint64_t &fileoff) {
    for (const auto &m : cache->mappings()) {
        if (vmaddr >= m.address && vmaddr < m.address + m.size) {
            fileoff = m.fileOffset + (vmaddr - m.address);
            return true;
        }
    }
    return false;
}

static std::vector<std::pair<uint64_t, std::string>> CollectImages(
        const moex::DyldSharedCachePtr &cache,
        const std::string &selector,
        bool exact_match) {
    std::vector<std::pair<uint64_t, std::string>> out;
    auto match = [&](const std::string &path) {
        if (path == selector) return true;
        if (exact_match) return false;
        return path.find(selector) != std::string::npos;
    };

    if (!cache->images().empty()) {
        for (const auto &img : cache->images()) {
            const std::string path = cache->ReadPathByOffset(img.pathFileOffset);
            if (match(path)) {
                out.push_back({img.address, path});
            }
        }
    } else {
        for (const auto &img : cache->map_images()) {
            if (match(img.second)) {
                out.push_back({img.first, img.second});
            }
        }
    }
    return out;
}

static std::string BaseName(const std::string &path) {
    const auto pos = path.find_last_of('/');
    if (pos == std::string::npos) return path;
    if (pos + 1 >= path.size()) return "";
    return path.substr(pos + 1);
}

static bool EnsureDirectory(const std::string &path, std::string &error) {
    if (path.empty()) {
        error = "output directory path is empty";
        return false;
    }
    struct stat st = {};
    if (::stat(path.c_str(), &st) == 0) {
        if ((st.st_mode & S_IFDIR) == 0) {
            error = "output path exists but is not a directory: " + path;
            return false;
        }
        return true;
    }
#if defined(_WIN32)
    if (_mkdir(path.c_str()) == 0) {
#else
    if (::mkdir(path.c_str(), 0755) == 0) {
#endif
        return true;
    }
    error = "cannot create output directory: " + path;
    return false;
}

static std::string JsonEscape(const std::string &s) {
    std::string out;
    out.reserve(s.size() + 16);
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

} // namespace

int main(int argc, char **argv) {
    auto print_usage = []() {
        std::cerr << "usage: moex-cache-extract [--compact] [--all] [--exact] [--dry-run] [--json] [--max=N] <dyld_shared_cache_file> <image-path-or-substr> <output-macho-or-dir>\n";
        std::cerr << "  --compact   rewrite segment file offsets for compact output\n";
        std::cerr << "  --all       extract all matched images into output directory\n";
        std::cerr << "  --exact     exact path match (default: substring)\n";
        std::cerr << "  --dry-run   print extraction plan without writing output files\n";
        std::cerr << "  --json      print machine-readable JSON lines\n";
        std::cerr << "  --max=N     max image count processed in --all mode (0 means unlimited)\n";
    };

    bool compact_mode = false;
    bool extract_all = false;
    bool exact_match = false;
    bool dry_run = false;
    bool json_output = false;
    uint32_t max_extract = 0;
    int arg_index = 1;
    while (arg_index < argc) {
        const std::string opt = argv[arg_index];
        if (opt == "--help" || opt == "-h") {
            print_usage();
            return 0;
        }
        if (opt == "--compact") {
            compact_mode = true;
            ++arg_index;
            continue;
        }
        if (opt == "--all") {
            extract_all = true;
            ++arg_index;
            continue;
        }
        if (opt == "--exact") {
            exact_match = true;
            ++arg_index;
            continue;
        }
        if (opt == "--dry-run") {
            dry_run = true;
            ++arg_index;
            continue;
        }
        if (opt == "--json") {
            json_output = true;
            ++arg_index;
            continue;
        }
        if (opt.rfind("--max=", 0) == 0) {
            max_extract = static_cast<uint32_t>(std::max(0, std::stoi(opt.substr(6))));
            ++arg_index;
            continue;
        }
        break;
    }

    if (argc - arg_index != 3) {
        print_usage();
        return 2;
    }

    const std::string cache_path = argv[arg_index];
    const std::string image_selector = argv[arg_index + 1];
    const std::string output_path = argv[arg_index + 2];

    try {
        moex::Binary bin(cache_path);
        if (!bin.IsDyldSharedCache()) {
            std::cerr << "input is not a dyld shared cache file\n";
            return 1;
        }
        auto cache = bin.dyld_cache();

        auto matched_images = CollectImages(cache, image_selector, exact_match);
        if (matched_images.empty()) {
            std::cerr << "image not found: " << image_selector << "\n";
            return 1;
        }

        const char *cache_bytes = reinterpret_cast<const char *>(cache->ctx()->file_start);
        const uint64_t cache_size = cache->ctx()->file_size;
        auto extract_one = [&](uint64_t image_addr, const std::string &image_path, const std::string &final_output_path) -> bool {
            uint64_t header_fileoff = 0;
            if (!VmToFileOffset(cache, image_addr, header_fileoff)) {
                std::cerr << "cannot map image vmaddr to cache file offset: " << image_path << "\n";
                return false;
            }

            if (header_fileoff + sizeof(qv_mach_header_64) > cache_size) {
                std::cerr << "header out of range: " << image_path << "\n";
                return false;
            }

            const auto *mh64 = reinterpret_cast<const qv_mach_header_64 *>(cache_bytes + header_fileoff);
            const bool is64 = (mh64->magic == MH_MAGIC_64 || mh64->magic == MH_CIGAM_64);
            const bool is32 = (mh64->magic == MH_MAGIC || mh64->magic == MH_CIGAM);
            if (!is64 && !is32) {
                std::cerr << "target image is not Mach-O: " << image_path << "\n";
                return false;
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
                std::cerr << "invalid load commands metadata: " << image_path << "\n";
                return false;
            }
            if (header_fileoff + mh_size + sizeofcmds > cache_size) {
                std::cerr << "load commands out of range: " << image_path << "\n";
                return false;
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
                            plans.push_back({std::string(seg->segname, 16).c_str(), seg->vmaddr, seg->fileoff, src_off, seg->fileoff, seg->filesize});
                        }
                    }
                } else if (lc->cmd == LC_SEGMENT && lc->cmdsize >= sizeof(qv_segment_command)) {
                    const auto *seg = reinterpret_cast<const qv_segment_command *>(lc);
                    if (seg->filesize > 0) {
                        uint64_t src_off = 0;
                        if (VmToFileOffset(cache, seg->vmaddr, src_off)) {
                            plans.push_back({std::string(seg->segname, 16).c_str(), seg->vmaddr, seg->fileoff, src_off, seg->fileoff, seg->filesize});
                        }
                    }
                }
                parsed += lc->cmdsize;
                cmd_cur += lc->cmdsize;
            }

            if (plans.empty()) {
                std::cerr << "no loadable segments found: " << image_path << "\n";
                return false;
            }

            if (compact_mode) {
                std::sort(plans.begin(), plans.end(), [](const SegmentCopyPlan &a, const SegmentCopyPlan &b) {
                    if (a.image_fileoff != b.image_fileoff) return a.image_fileoff < b.image_fileoff;
                    return a.vmaddr < b.vmaddr;
                });
                uint64_t next_off = AlignUp(mh_size + sizeofcmds, 0x1000);
                for (auto &p : plans) {
                    p.target_fileoff = AlignUp(next_off, 0x1000);
                    next_off = p.target_fileoff + p.size;
                }
            }

            uint64_t out_size = mh_size + sizeofcmds;
            for (const auto &p : plans) {
                out_size = std::max(out_size, p.target_fileoff + p.size);
                if (p.source_fileoff + p.size > cache_size) {
                    std::cerr << "segment source out of range: " << p.name << " image=" << image_path << "\n";
                    return false;
                }
            }

            if (dry_run) {
                if (!json_output) {
                    std::cout << "plan: " << image_path << " -> " << final_output_path
                              << " mode=" << (compact_mode ? "compact" : "raw-fileoff")
                              << " size=" << out_size << " bytes segments=" << plans.size() << "\n";
                } else {
                    std::cout << "{\"event\":\"plan\",\"image\":\"" << JsonEscape(image_path) << "\",\"output\":\"" << JsonEscape(final_output_path)
                              << "\",\"mode\":\"" << (compact_mode ? "compact" : "raw-fileoff")
                              << "\",\"size\":" << out_size << ",\"segments\":" << plans.size() << "}\n";
                }
                return true;
            }

            std::fstream out(final_output_path, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!out.good()) {
                std::cerr << "cannot open output file: " << final_output_path << "\n";
                return false;
            }
            if (out_size > 0) {
                out.seekp(static_cast<std::streamoff>(out_size - 1));
                char z = 0;
                out.write(&z, 1);
            }

            std::vector<char> header_blob(static_cast<size_t>(mh_size + sizeofcmds));
            memcpy(header_blob.data(), cache_bytes + header_fileoff, header_blob.size());
            if (compact_mode) {
                PatchLoadCommandsForCompaction(header_blob.data() + mh_size, sizeofcmds, plans, is64);
            }

            out.seekp(0);
            out.write(header_blob.data(), static_cast<std::streamsize>(header_blob.size()));

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

            if (!json_output) {
                std::cout << "extracted: " << image_path << "\n";
                std::cout << "mode: " << (compact_mode ? "compact" : "raw-fileoff") << "\n";
                std::cout << "output: " << final_output_path << " size=" << out_size << " bytes segments=" << plans.size() << "\n";
            } else {
                std::cout << "{\"event\":\"extracted\",\"image\":\"" << JsonEscape(image_path) << "\",\"output\":\"" << JsonEscape(final_output_path)
                          << "\",\"mode\":\"" << (compact_mode ? "compact" : "raw-fileoff")
                          << "\",\"size\":" << out_size << ",\"segments\":" << plans.size() << "}\n";
            }
            return true;
        };

        if (!extract_all) {
            if (matched_images.size() > 1) {
                if (!json_output) {
                    std::cerr << "warning: selector matched " << matched_images.size()
                              << " images, using first: " << matched_images[0].second << "\n";
                } else {
                    std::cerr << "{\"event\":\"warning\",\"message\":\"multiple matches; using first\",\"matched\":"
                              << matched_images.size() << ",\"image\":\"" << JsonEscape(matched_images[0].second) << "\"}\n";
                }
            }
            return extract_one(matched_images[0].first, matched_images[0].second, output_path) ? 0 : 1;
        }

        std::string dir_error;
        if (!dry_run) {
            if (!EnsureDirectory(output_path, dir_error)) {
                std::cerr << dir_error << "\n";
                return 1;
            }
        }

        std::unordered_map<std::string, uint32_t> name_count;
        uint32_t ok_count = 0;
        uint32_t processed = 0;
        for (const auto &m : matched_images) {
            if (max_extract > 0 && processed >= max_extract) break;
            std::string base = BaseName(m.second);
            if (base.empty()) base = "image";
            uint32_t &count = name_count[base];
            std::string final_name = base;
            if (count > 0) {
                final_name += "." + std::to_string(count);
            }
            ++count;
            final_name += ".macho";

            std::string full_output = output_path;
            if (!full_output.empty() && full_output.back() != '/') full_output += "/";
            full_output += final_name;
            if (extract_one(m.first, m.second, full_output)) {
                ++ok_count;
            }
            ++processed;
        }
        if (!json_output) {
            std::cout << "batch: matched=" << matched_images.size() << " processed=" << processed << " extracted=" << ok_count << "\n";
        } else {
            std::cout << "{\"event\":\"batch\",\"matched\":" << matched_images.size()
                      << ",\"processed\":" << processed << ",\"extracted\":" << ok_count << "}\n";
        }
        return ok_count == processed ? 0 : 1;
    } catch (const std::exception &ex) {
        std::cerr << "moex-cache-extract failed: " << ex.what() << "\n";
        return 1;
    }
}
