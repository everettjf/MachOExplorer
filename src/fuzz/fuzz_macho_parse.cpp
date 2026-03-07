#include "libmoex/node/Binary.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0 || size > (1u << 20)) {
        return 0;
    }

    char path[] = "/tmp/moex-fuzz-XXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) return 0;

    ssize_t written = write(fd, data, size);
    close(fd);
    if (written != static_cast<ssize_t>(size)) {
        unlink(path);
        return 0;
    }

    try {
        moex::Binary bin(path);
        bin.ForEachHeader([&](moex::MachHeaderPtr) {});
    } catch (...) {
        // Expected for malformed inputs. Only crashes/sanitizer reports matter.
    }

    unlink(path);
    return 0;
}
