//
// Created by everettjf on 2017/7/24.
//

#include "CodeSignatureViewNode.h"

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
        }

        t->AddRow({AsAddress(base_off + 12 + static_cast<uint64_t>(i) * 8),
                   fmt::format("[{}] {}", i, SlotName(slot_type)),
                   fmt::format("offset={} magic={} length={}",
                               blob_off, magic_desc, len_desc)});
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
