#include "libmoex/node/Binary.h"
#include <algorithm>
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
    bool json_mode = false;
    bool exact_match = false;
    uint32_t limit = 0;
    int arg_index = 1;
    while (arg_index < argc) {
        const std::string opt = argv[arg_index];
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
        break;
    }

    if ((argc - arg_index) < 1 || (argc - arg_index) > 2) {
        std::cerr << "usage: moex-cache-list [--json] [--exact] [--limit=N] <dyld_shared_cache_file> [path-filter]\n";
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

        if (!json_mode) {
            std::cout << "magic: " << std::string(header.magic, header.magic + 16) << "\n";
            std::cout << "mappingCount: " << header.mappingCount << " imageCount: " << header.imagesCount << "\n";
        } else {
            std::cout << "{";
            std::cout << "\"magic\":\"" << JsonEscape(std::string(header.magic, header.magic + 16)) << "\",";
            std::cout << "\"mappingCount\":" << header.mappingCount << ",";
            std::cout << "\"imageCount\":" << header.imagesCount << ",";
            std::cout << "\"images\":[";
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
                    std::cout << "0x" << moex::util::AsShortHexString(img.address) << " " << path << "\n";
                } else {
                    if (!first_json_item) std::cout << ",";
                    first_json_item = false;
                    std::cout << "{";
                    std::cout << "\"address\":\"0x" << moex::util::AsShortHexString(img.address) << "\",";
                    std::cout << "\"path\":\"" << JsonEscape(path) << "\"";
                    std::cout << "}";
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
                    std::cout << "0x" << moex::util::AsShortHexString(img.first) << " " << img.second << "\n";
                } else {
                    if (!first_json_item) std::cout << ",";
                    first_json_item = false;
                    std::cout << "{";
                    std::cout << "\"address\":\"0x" << moex::util::AsShortHexString(img.first) << "\",";
                    std::cout << "\"path\":\"" << JsonEscape(img.second) << "\"";
                    std::cout << "}";
                }
                ++shown;
                if (limit > 0 && shown >= limit) break;
            }
        }

        if (!json_mode) {
            std::cout << "shown: " << shown << "\n";
        } else {
            std::cout << "],\"shown\":" << shown << "}\n";
        }
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "moex-cache-list failed: " << ex.what() << "\n";
        return 1;
    }
}
