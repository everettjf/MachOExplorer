//
// Created by everettjf on 2017/7/24.
//

#include "CodeSignatureViewNode.h"
#include "libmoex/base/digest/Sha.h"
#include <cstring>

MOEX_NAMESPACE_BEGIN

namespace {

// Code signing blob magics (all big-endian on disk).
constexpr uint32_t kCSMagicEmbeddedSignature    = 0xfade0cc0;
constexpr uint32_t kCSMagicEmbeddedSignatureOld = 0xfade0b02;
constexpr uint32_t kCSMagicCodeDirectory        = 0xfade0c02;
constexpr uint32_t kCSMagicRequirements         = 0xfade0c01;
constexpr uint32_t kCSMagicRequirement          = 0xfade0c00;
constexpr uint32_t kCSMagicEmbeddedEntitlements    = 0xfade7171;
constexpr uint32_t kCSMagicEmbeddedDerEntitlements = 0xfade7172;
constexpr uint32_t kCSMagicBlobWrapper          = 0xfade0b01;

// Slot index types.
constexpr uint32_t kCSSlotEntitlements    = 5;
constexpr uint32_t kCSSlotDerEntitlements = 7;

uint32_t ReadBE32(const uint8_t *p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8)  |
           static_cast<uint32_t>(p[3]);
}

std::string HashTypeName(uint8_t type) {
    switch (type) {
        case 0:  return "none";
        case 1:  return "SHA-1";
        case 2:  return "SHA-256";
        case 3:  return "SHA-256 (truncated)";
        case 4:  return "SHA-384";
        case 5:  return "SHA-512";
        default: return "unknown";
    }
}

std::string CodeDirectoryFlags(uint32_t flags) {
    // Subset of CS_* flags that are useful to surface.
    struct { uint32_t bit; const char *name; } table[] = {
        {0x00000001, "host"},
        {0x00000002, "adhoc"},
        {0x00000004, "force-hard"},
        {0x00000008, "force-kill"},
        {0x00000100, "installer"},
        {0x00000200, "dyld-platform"},
        {0x00000400, "hard"},
        {0x00000800, "kill"},
        {0x00010000, "runtime"},
        {0x00020000, "linker-signed"},
    };
    std::string out;
    for (const auto &e : table) {
        if (flags & e.bit) {
            if (!out.empty()) out += " | ";
            out += e.name;
        }
    }
    return out.empty() ? "(none)" : out;
}

// Read a NUL-terminated string starting at base+off, bounded by datasize.
std::string ReadBlobCString(const uint8_t *base, uint32_t datasize, uint32_t off) {
    if (off == 0 || off >= datasize) return std::string();
    const uint8_t *start = base + off;
    const char *cstart = reinterpret_cast<const char *>(start);
    const char *nul = static_cast<const char *>(memchr(start, '\0', datasize - off));
    if (nul == nullptr) return std::string(cstart, static_cast<std::size_t>(datasize - off));
    return std::string(cstart, nul);
}

std::string MagicName(uint32_t magic) {
    switch (magic) {
        case kCSMagicEmbeddedSignature:       return "Embedded Signature";
        case kCSMagicEmbeddedSignatureOld:    return "Embedded Signature (old)";
        case kCSMagicCodeDirectory:           return "Code Directory";
        case kCSMagicRequirements:            return "Requirements";
        case kCSMagicRequirement:             return "Requirement";
        case kCSMagicEmbeddedEntitlements:    return "Entitlements (XML)";
        case kCSMagicEmbeddedDerEntitlements: return "Entitlements (DER)";
        case kCSMagicBlobWrapper:             return "CMS Signature";
        default:                              return "Unknown";
    }
}

std::string SlotName(uint32_t type) {
    switch (type) {
        case 0:                    return "Code Directory";
        case 1:                    return "Info.plist";
        case 2:                    return "Requirements";
        case 3:                    return "Resource Directory";
        case 4:                    return "Application";
        case kCSSlotEntitlements:  return "Entitlements";
        case 6:                    return "Repspecific";
        case kCSSlotDerEntitlements: return "DER Entitlements";
        case 0x1000:               return "Alternate Code Directory";
        case 0x10000:              return "CMS Signature";
        default:                   return "Slot";
    }
}

} // namespace

void CodeSignatureViewNode::InitViewDatas() {
    using namespace moex::util;

    auto t = CreateTableView();

    auto seg = mh_->FindLoadCommand<moex::LoadCommand_LC_CODE_SIGNATURE>({LC_CODE_SIGNATURE});
    if (!seg)
        return;

    NodeContextPtr ctx = mh_->ctx();
    char *blob = (char *)mh_->header_start() + seg->cmd()->dataoff;
    const uint32_t datasize = seg->cmd()->datasize;

    auto b = CreateBinaryView();
    b->offset = blob;
    b->size = datasize;
    b->start_value = (uint64_t)blob - (uint64_t)ctx->file_start;

    if (!NodeInFile(ctx, blob, datasize) || datasize < 12) {
        t->AddRow({AsAddress(b->start_value), "error", "code signature blob out of range"});
        return;
    }

    const uint8_t *base = reinterpret_cast<const uint8_t *>(blob);
    const uint64_t base_off = b->start_value;
    const uint32_t super_magic = ReadBE32(base);
    const uint32_t super_len = ReadBE32(base + 4);
    const uint32_t super_count = ReadBE32(base + 8);

    t->AddRow({AsAddress(base_off), "Magic",
               fmt::format("0x{} ({})", AsShortHexString(super_magic), MagicName(super_magic))});
    t->AddRow({AsAddress(base_off + 4), "Length", AsString(super_len)});

    if (super_magic != kCSMagicEmbeddedSignature && super_magic != kCSMagicEmbeddedSignatureOld) {
        // Not a super blob; nothing more to enumerate.
        return;
    }

    t->AddRow({AsAddress(base_off + 8), "Blob Count", AsString(super_count)});
    t->AddSeparator();

    // Cap to a sane number of indices and to what the blob can hold.
    const uint64_t max_indices = (static_cast<uint64_t>(datasize) - 12) / 8;
    uint32_t count = super_count;
    if (count > max_indices) count = static_cast<uint32_t>(max_indices);

    uint32_t entitlements_off = 0;
    bool has_entitlements = false;
    uint32_t cd_off = 0;
    bool has_cd = false;

    for (uint32_t i = 0; i < count; ++i) {
        const uint8_t *idx = base + 12 + static_cast<uint64_t>(i) * 8;
        const uint32_t slot_type = ReadBE32(idx);
        const uint32_t blob_off = ReadBE32(idx + 4);

        std::string magic_desc = "-";
        std::string len_desc = "-";
        if (blob_off + 8 <= datasize) {
            const uint32_t bmagic = ReadBE32(base + blob_off);
            const uint32_t blen = ReadBE32(base + blob_off + 4);
            magic_desc = fmt::format("0x{} ({})", AsShortHexString(bmagic), MagicName(bmagic));
            len_desc = AsString(blen);
            if ((slot_type == kCSSlotEntitlements || bmagic == kCSMagicEmbeddedEntitlements) &&
                !has_entitlements) {
                entitlements_off = blob_off;
                has_entitlements = true;
            }
            if (bmagic == kCSMagicCodeDirectory && !has_cd) {
                cd_off = blob_off;
                has_cd = true;
            }
        }

        t->AddRow({AsAddress(base_off + 12 + static_cast<uint64_t>(i) * 8),
                   fmt::format("[{}] {}", i, SlotName(slot_type)),
                   fmt::format("offset={} magic={} length={}",
                               blob_off, magic_desc, len_desc)});
    }

    // Decode the Code Directory: identity, hash type, slot counts, flags.
    if (has_cd && cd_off + 44 <= datasize) {
        const uint8_t *cd = base + cd_off;
        const uint32_t cd_version = ReadBE32(cd + 8);
        const uint32_t cd_flags   = ReadBE32(cd + 12);
        const uint32_t ident_off  = ReadBE32(cd + 20);
        const uint32_t n_special  = ReadBE32(cd + 24);
        const uint32_t n_code     = ReadBE32(cd + 28);
        const uint32_t code_limit = ReadBE32(cd + 32);
        const uint8_t  hash_size  = cd[36];
        const uint8_t  hash_type  = cd[37];
        const uint8_t  page_log2  = cd[39];

        t->AddSeparator();
        t->AddRow({AsAddress(base_off + cd_off), "Code Directory",
                   fmt::format("version=0x{}", AsShortHexString(cd_version))});

        // cdhash: the digest of the whole Code Directory blob (per its hash
        // type), truncated to the canonical 20 bytes.
        const uint32_t cd_len = ReadBE32(cd + 4);
        if (cd_len >= 8 && static_cast<uint64_t>(cd_off) + cd_len <= datasize) {
            std::vector<uint8_t> digest;
            if (hash_type == 1) {
                digest = moex::digest::Sha1(cd, cd_len);
            } else if (hash_type == 2 || hash_type == 3) {
                digest = moex::digest::Sha256(cd, cd_len);
            }
            if (!digest.empty()) {
                if (digest.size() > 20) digest.resize(20);
                t->AddRow({AsAddress(base_off + cd_off), "CD Hash (cdhash)",
                           moex::digest::ToHex(digest)});
            }
        }

        const std::string identifier =
                ReadBlobCString(cd, datasize - cd_off, ident_off);
        if (!identifier.empty()) {
            t->AddRow({AsAddress(base_off + cd_off + ident_off), "Identifier", identifier});
        }

        // Team identifier was added in version 0x20200.
        if (cd_version >= 0x20200 && cd_off + 52 <= datasize) {
            const uint32_t team_off = ReadBE32(cd + 48);
            const std::string team = ReadBlobCString(cd, datasize - cd_off, team_off);
            if (!team.empty()) {
                t->AddRow({AsAddress(base_off + cd_off + team_off), "Team Identifier", team});
            }
        }

        t->AddRow({AsAddress(base_off + cd_off + 37), "Hash Type",
                   fmt::format("{} ({}, {} bytes)", hash_type, HashTypeName(hash_type), hash_size)});
        t->AddRow({AsAddress(base_off + cd_off + 12), "Flags",
                   fmt::format("0x{} ({})", AsShortHexString(cd_flags), CodeDirectoryFlags(cd_flags))});
        t->AddRow({AsAddress(base_off + cd_off + 28), "Code Slots",
                   fmt::format("{} (page size {} bytes)", n_code,
                               page_log2 < 31 ? (1u << page_log2) : 0u)});
        t->AddRow({AsAddress(base_off + cd_off + 24), "Special Slots", AsString(n_special)});
        t->AddRow({AsAddress(base_off + cd_off + 32), "Code Limit", AsString(code_limit)});
    }

    if (!has_entitlements)
        return;

    // Decode the entitlements XML blob: magic(4) + length(4) + payload.
    if (entitlements_off + 8 > datasize)
        return;
    const uint32_t ent_magic = ReadBE32(base + entitlements_off);
    const uint32_t ent_len = ReadBE32(base + entitlements_off + 4);
    if (ent_magic != kCSMagicEmbeddedEntitlements)
        return;
    if (ent_len < 8 || entitlements_off + ent_len > datasize)
        return;

    const char *xml = blob + entitlements_off + 8;
    const uint32_t xml_len = ent_len - 8;

    t->AddSeparator();
    t->AddRow({AsAddress(base_off + entitlements_off), "Entitlements",
               fmt::format("{} bytes (XML)", xml_len)});

    // Emit the plist line by line, capped so a huge blob cannot flood the table.
    const uint32_t kMaxLines = 1000;
    uint32_t line_no = 0;
    uint32_t i = 0;
    while (i < xml_len && line_no < kMaxLines) {
        uint32_t start = i;
        while (i < xml_len && xml[i] != '\n') ++i;
        std::string line(xml + start, xml + i);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        t->AddRow({AsAddress(base_off + entitlements_off + 8 + start), "", line});
        ++line_no;
        if (i < xml_len) ++i; // skip '\n'
    }
    if (i < xml_len) {
        t->AddRow({"", "", fmt::format("... ({} more bytes truncated)", xml_len - i)});
    }
}


MOEX_NAMESPACE_END
