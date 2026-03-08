#include "libmoex/node/Binary.h"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace {
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
}

int main(int argc, char **argv) {
    auto print_usage = []() {
        std::cerr << "usage: moex-cache-list [--json] [--exact] [--limit=N] [--output=file] <dyld_shared_cache_file> [path-filter]\n";
        std::cerr << "  --json         output JSON payload\n";
        std::cerr << "  --exact        exact path match (default: substring)\n";
        std::cerr << "  --limit=N      stop after N matches (0 means unlimited)\n";
        std::cerr << "  --output=file  write output to file instead of stdout\n";
    };

    bool json_mode = false;
    bool exact_match = false;
    uint32_t limit = 0;
    std::string output_file;
    int arg_index = 1;
    while (arg_index < argc) {
        const std::string opt = argv[arg_index];
        if (opt == "--help" || opt == "-h") {
            print_usage();
            return 0;
        }
        if (opt == "--json") {
            json_mode = true;
            ++arg_index;
            continue;
        }
        if (opt == "--exact") {
            exact_match = true;
            ++arg_index;
            continue;
        }
        if (opt.rfind("--limit=", 0) == 0) {
            limit = static_cast<uint32_t>(std::max(0, std::stoi(opt.substr(8))));
            ++arg_index;
            continue;
        }
        if (opt.rfind("--output=", 0) == 0) {
            output_file = opt.substr(9);
            ++arg_index;
            continue;
        }
        break;
    }

    if ((argc - arg_index) < 1 || (argc - arg_index) > 2) {
        print_usage();
        return 2;
    }

    const std::string filter = (argc - arg_index == 2) ? argv[arg_index + 1] : "";
    try {
        moex::Binary bin(argv[arg_index]);
        if (!bin.IsDyldSharedCache()) {
            std::cerr << "input is not a dyld shared cache file\n";
            return 1;
        }

        auto cache = bin.dyld_cache();
        const auto &header = cache->header();
        auto match_filter = [&](const std::string &path) {
            if (filter.empty()) return true;
            if (exact_match) return path == filter;
            return path.find(filter) != std::string::npos;
        };

        std::ostream *out = &std::cout;
        std::ofstream fout;
        if (!output_file.empty()) {
            fout.open(output_file, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!fout.good()) {
                std::cerr << "cannot open output file: " << output_file << "\n";
                return 1;
            }
            out = &fout;
        }

        if (!json_mode) {
            (*out) << "magic: " << std::string(header.magic, header.magic + 16) << "\n";
            (*out) << "mappingCount: " << header.mappingCount << " imageCount: " << header.imagesCount << "\n";
        } else {
            (*out) << "{";
            (*out) << "\"magic\":\"" << JsonEscape(std::string(header.magic, header.magic + 16)) << "\",";
            (*out) << "\"mappingCount\":" << header.mappingCount << ",";
            (*out) << "\"imageCount\":" << header.imagesCount << ",";
            (*out) << "\"images\":[";
        }

        uint32_t shown = 0;
        bool first_json_item = true;
        if (!cache->images().empty()) {
            for (const auto &img : cache->images()) {
                std::string path = cache->ReadPathByOffset(img.pathFileOffset);
                if (!match_filter(path)) {
                    continue;
                }
                if (!json_mode) {
                    (*out) << "0x" << moex::util::AsShortHexString(img.address) << " " << path << "\n";
                } else {
                    if (!first_json_item) (*out) << ",";
                    first_json_item = false;
                    (*out) << "{";
                    (*out) << "\"address\":\"0x" << moex::util::AsShortHexString(img.address) << "\",";
                    (*out) << "\"path\":\"" << JsonEscape(path) << "\"";
                    (*out) << "}";
                }
                ++shown;
                if (limit > 0 && shown >= limit) break;
            }
        } else {
            for (const auto &img : cache->map_images()) {
                if (!match_filter(img.second)) {
                    continue;
                }
                if (!json_mode) {
                    (*out) << "0x" << moex::util::AsShortHexString(img.first) << " " << img.second << "\n";
                } else {
                    if (!first_json_item) (*out) << ",";
                    first_json_item = false;
                    (*out) << "{";
                    (*out) << "\"address\":\"0x" << moex::util::AsShortHexString(img.first) << "\",";
                    (*out) << "\"path\":\"" << JsonEscape(img.second) << "\"";
                    (*out) << "}";
                }
                ++shown;
                if (limit > 0 && shown >= limit) break;
            }
        }

        if (!json_mode) {
            (*out) << "shown: " << shown << "\n";
        } else {
            (*out) << "],\"shown\":" << shown << "}\n";
        }
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "moex-cache-list failed: " << ex.what() << "\n";
        return 1;
    }
}
