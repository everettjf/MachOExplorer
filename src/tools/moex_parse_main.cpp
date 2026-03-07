#include "libmoex/node/Binary.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: moex-parse <file> [file...]\n";
        return 2;
    }

    int failed = 0;
    for (int i = 1; i < argc; ++i) {
        try {
            moex::Binary bin(argv[i]);
            int headers = 0;
            bin.ForEachHeader([&](moex::MachHeaderPtr) { ++headers; });
            std::cout << "[ok] " << argv[i] << " headers=" << headers << "\n";
        } catch (const std::exception &ex) {
            std::cerr << "[fail] " << argv[i] << " error=" << ex.what() << "\n";
            ++failed;
        } catch (...) {
            std::cerr << "[fail] " << argv[i] << " error=unknown\n";
            ++failed;
        }
    }

    return failed == 0 ? 0 : 1;
}
