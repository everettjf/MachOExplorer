#include "Archive.h"
#include "Magic.h"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdint>

MOEX_NAMESPACE_BEGIN

namespace {
struct ArHeaderRaw {
    char name[16];
    char mtime[12];
    char uid[6];
    char gid[6];
    char mode[8];
    char size[10];
    char fmag[2];
};
static_assert(sizeof(ArHeaderRaw) == 60, "Invalid ar header layout");

std::string TrimRight(std::string s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\n' || s.back() == '/')) {
        s.pop_back();
    }
    return s;
}

std::string ParseName(const ArHeaderRaw *hdr, const std::string &strtab) {
    std::string raw(hdr->name, sizeof(hdr->name));
    raw = TrimRight(raw);

    if (raw.rfind("/#1/", 0) == 0) {
        return raw;
    }
    if (!raw.empty() && raw[0] == '/' && raw.size() > 1 && std::isdigit(raw[1]) && !strtab.empty()) {
        const std::size_t off = static_cast<std::size_t>(std::strtoul(raw.c_str() + 1, nullptr, 10));
        if (off < strtab.size()) {
            std::size_t end = off;
            while (end < strtab.size() && strtab[end] != '\n' && strtab[end] != '\0') {
                ++end;
            }
            std::string n = strtab.substr(off, end - off);
            return TrimRight(n);
        }
    }
    return raw;
}

uint64_t ParseSize(const ArHeaderRaw *hdr) {
    std::string str(hdr->size, sizeof(hdr->size));
    str = TrimRight(str);
    if (str.empty()) return 0;
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return UINT64_MAX;
        }
    }
    return std::strtoull(str.c_str(), nullptr, 10);
}
}

bool Archive::IsArchiveMagic(const void *offset, std::size_t size) {
    if (size < 8 || offset == nullptr) {
        return false;
    }
    static const char kMagic[] = "!<arch>\n";
    return std::memcmp(offset, kMagic, 8) == 0;
}

void Archive::Init(void *offset, std::size_t file_size, NodeContextPtr &ctx) {
    archive_start_ = offset;
    ctx_ = ctx;
    Parse(offset, file_size);
}

void Archive::Parse(void *offset, std::size_t file_size) {
    members_.clear();

    if (!IsArchiveMagic(offset, file_size)) {
        throw NodeException("Not an archive file");
    }
    if (file_size < 8 + sizeof(ArHeaderRaw)) {
        throw NodeException("Malformed archive file: truncated after global magic");
    }

    std::string strtab;
    uint64_t cur = 8; // Skip global archive magic.
    while (cur + sizeof(ArHeaderRaw) <= file_size) {
        auto *hdr = reinterpret_cast<ArHeaderRaw *>((char *)offset + cur);
        if (hdr->fmag[0] != '`' || hdr->fmag[1] != '\n') {
            throw NodeException("Malformed archive file: invalid member header trailer");
        }

        const uint64_t file_size_member = ParseSize(hdr);
        if (file_size_member == UINT64_MAX) {
            throw NodeException("Malformed archive file: non-decimal member size");
        }
        uint64_t data_offset = cur + sizeof(ArHeaderRaw);
        if (data_offset + file_size_member > file_size) {
            throw NodeException("Malformed archive file: member payload out of range");
        }

        std::string name = ParseName(hdr, strtab);
        uint64_t payload_size = file_size_member;

        if (name == "//") {
            strtab.assign((char *)offset + data_offset, (size_t)file_size_member);
        } else {
            if (name.rfind("/#1/", 0) == 0) {
                const uint64_t ext_name_size = std::strtoull(name.c_str() + 3, nullptr, 10);
                if (ext_name_size <= payload_size && data_offset + ext_name_size <= file_size) {
                    name.assign((char *)offset + data_offset, (size_t)ext_name_size);
                    data_offset += ext_name_size;
                    payload_size -= ext_name_size;
                    name = TrimRight(name);
                }
            }

            if (name != "/") {
                auto m = std::make_shared<ArchiveMember>();
                m->name = name;
                m->header_offset = cur;
                m->data_offset = data_offset;
                m->data_size = payload_size;

                if (payload_size >= sizeof(uint32_t)) {
                    Magic magic((char *)offset + data_offset);
                    if (magic.IsValid() && !magic.IsFat()) {
                        m->is_macho = true;
                        m->mh = std::make_shared<MachHeader>();
                        m->mh->Init((char *)offset + data_offset, ctx_);
                    }
                }
                members_.push_back(m);
            }
        }

        cur = data_offset + payload_size;
        if ((cur & 1ULL) != 0ULL) {
            ++cur;
        }
    }
    if (members_.empty()) {
        throw NodeException("Malformed archive file: no valid members");
    }
}

MOEX_NAMESPACE_END
