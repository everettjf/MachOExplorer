#include "libmoex/node/Binary.h"
#include <algorithm>
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "usage: moex-cache-list <dyld_shared_cache_file> [path-filter]\n";
        return 2;
    }

    const std::string filter = (argc == 3) ? argv[2] : "";
    try {
        moex::Binary bin(argv[1]);
        if (!bin.IsDyldSharedCache()) {
            std::cerr << "input is not a dyld shared cache file\n";
            return 1;
        }

        auto cache = bin.dyld_cache();
        const auto &header = cache->header();
        std::cout << "magic: " << std::string(header.magic, header.magic + 16) << "\n";
        std::cout << "mappingCount: " << header.mappingCount << " imageCount: " << header.imagesCount << "\n";

        uint32_t shown = 0;
        if (!cache->images().empty()) {
            for (const auto &img : cache->images()) {
                std::string path = cache->ReadPathByOffset(img.pathFileOffset);
                if (!filter.empty() && path.find(filter) == std::string::npos) {
                    continue;
                }
                std::cout << "0x" << moex::util::AsShortHexString(img.address) << " " << path << "\n";
                ++shown;
            }
        } else {
            for (const auto &img : cache->map_images()) {
                if (!filter.empty() && img.second.find(filter) == std::string::npos) {
                    continue;
                }
                std::cout << "0x" << moex::util::AsShortHexString(img.first) << " " << img.second << "\n";
                ++shown;
            }
        }

        std::cout << "shown: " << shown << "\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "moex-cache-list failed: " << ex.what() << "\n";
        return 1;
    }
}
