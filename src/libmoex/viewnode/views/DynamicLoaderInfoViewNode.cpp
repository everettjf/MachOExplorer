//
// Created by everettjf on 2017/7/24.
//

#include "DynamicLoaderInfoViewNode.h"
#include <algorithm>
#include <cstring>
#include <list>
#include <unordered_set>

MOEX_NAMESPACE_BEGIN

namespace {

constexpr uint64_t DYLD_CHAINED_PTR_START_NONE = 0xFFFF;
constexpr uint64_t DYLD_CHAINED_PTR_START_MULTI = 0x8000;
constexpr uint64_t DYLD_CHAINED_PTR_START_LAST = 0x8000;

// dyld_chained_fixups_header
struct dyld_chained_fixups_header {
    uint32_t fixups_version;
    uint32_t starts_offset;
    uint32_t imports_offset;
    uint32_t symbols_offset;
    uint32_t imports_count;
    uint32_t imports_format;
    uint32_t symbols_format;
};

// dyld_chained_starts_in_image
struct dyld_chained_starts_in_image {
    uint32_t seg_count;
    // followed by seg_count uint32_t offsets
};

// dyld_chained_starts_in_segment
struct dyld_chained_starts_in_segment {
    uint32_t size;
    uint16_t page_size;
    uint16_t pointer_format;
    uint64_t segment_offset;
    uint32_t max_valid_pointer;
    uint16_t page_count;
    // followed by page_start[page_count]
};

struct dyld_chained_import {
    uint32_t lib_ordinal : 8;
    uint32_t weak_import : 1;
    uint32_t name_offset : 23;
};

struct dyld_chained_import_addend {
    uint32_t lib_ordinal : 8;
    uint32_t weak_import : 1;
    uint32_t name_offset : 23;
    int32_t addend;
};

struct dyld_chained_import_addend64 {
    uint64_t lib_ordinal : 16;
    uint64_t weak_import : 1;
    uint64_t reserved : 15;
    uint64_t name_offset : 32;
    uint64_t addend;
};

enum : uint16_t {
    DYLD_CHAINED_PTR_ARM64E = 1,
    DYLD_CHAINED_PTR_64 = 2,
    DYLD_CHAINED_PTR_32 = 3,
    DYLD_CHAINED_PTR_32_CACHE = 4,
    DYLD_CHAINED_PTR_32_FIRMWARE = 5,
    DYLD_CHAINED_PTR_64_OFFSET = 6,
    DYLD_CHAINED_PTR_ARM64E_KERNEL = 7,
    DYLD_CHAINED_PTR_64_KERNEL_CACHE = 8,
    DYLD_CHAINED_PTR_ARM64E_USERLAND = 9,
    DYLD_CHAINED_PTR_ARM64E_FIRMWARE = 10,
    DYLD_CHAINED_PTR_X86_64_KERNEL_CACHE = 11,
    DYLD_CHAINED_PTR_ARM64E_USERLAND24 = 12
};

static std::string PointerFormatName(uint16_t fmt) {
    switch (fmt) {
        case DYLD_CHAINED_PTR_ARM64E: return "ARM64E";
        case DYLD_CHAINED_PTR_64: return "64";
        case DYLD_CHAINED_PTR_32: return "32";
        case DYLD_CHAINED_PTR_32_CACHE: return "32_CACHE";
        case DYLD_CHAINED_PTR_32_FIRMWARE: return "32_FIRMWARE";
        case DYLD_CHAINED_PTR_64_OFFSET: return "64_OFFSET";
        case DYLD_CHAINED_PTR_ARM64E_KERNEL: return "ARM64E_KERNEL";
        case DYLD_CHAINED_PTR_64_KERNEL_CACHE: return "64_KERNEL_CACHE";
        case DYLD_CHAINED_PTR_ARM64E_USERLAND: return "ARM64E_USERLAND";
        case DYLD_CHAINED_PTR_ARM64E_FIRMWARE: return "ARM64E_FIRMWARE";
        case DYLD_CHAINED_PTR_X86_64_KERNEL_CACHE: return "X86_64_KERNEL_CACHE";
        case DYLD_CHAINED_PTR_ARM64E_USERLAND24: return "ARM64E_USERLAND24";
        default: return "UNKNOWN";
    }
}

static std::string ImportFormatName(uint32_t fmt) {
    switch (fmt) {
        case 1: return "DYLD_CHAINED_IMPORT";
        case 2: return "DYLD_CHAINED_IMPORT_ADDEND";
        case 3: return "DYLD_CHAINED_IMPORT_ADDEND64";
        default: return "UNKNOWN";
    }
}

static bool Is64BitPointerFormat(uint16_t fmt) {
    switch (fmt) {
        case DYLD_CHAINED_PTR_64:
        case DYLD_CHAINED_PTR_64_OFFSET:
        case DYLD_CHAINED_PTR_ARM64E:
        case DYLD_CHAINED_PTR_ARM64E_KERNEL:
        case DYLD_CHAINED_PTR_64_KERNEL_CACHE:
        case DYLD_CHAINED_PTR_ARM64E_USERLAND:
        case DYLD_CHAINED_PTR_ARM64E_FIRMWARE:
        case DYLD_CHAINED_PTR_X86_64_KERNEL_CACHE:
        case DYLD_CHAINED_PTR_ARM64E_USERLAND24:
            return true;
        default:
            return false;
    }
}

static uint64_t DecodeNext(uint16_t fmt, uint64_t raw) {
    if (!Is64BitPointerFormat(fmt)) {
        return (raw >> 26) & 0x1F;
    }
    return (raw >> 51) & 0x7FF;
}

static std::string DecodePointerDetail(uint16_t fmt, uint64_t raw) {
    if (fmt == DYLD_CHAINED_PTR_64 || fmt == DYLD_CHAINED_PTR_64_OFFSET ||
        fmt == DYLD_CHAINED_PTR_64_KERNEL_CACHE || fmt == DYLD_CHAINED_PTR_X86_64_KERNEL_CACHE) {
        const bool bind = ((raw >> 63) & 0x1) != 0;
        if (bind) {
            const uint64_t ordinal = raw & 0x00FFFFFFULL;
            const uint64_t addend = (raw >> 24) & 0xFF;
            return fmt::format("bind ordinal={} addend={}", ordinal, addend);
        }
        const uint64_t target = raw & 0x0000000FFFFFFFFFULL;
        const uint64_t high8 = (raw >> 36) & 0xFF;
        if (fmt == DYLD_CHAINED_PTR_64_OFFSET) {
            return fmt::format("rebase target-fileoff=0x{} high8=0x{}", util::AsShortHexString(target), util::AsShortHexString(high8));
        }
        return fmt::format("rebase target-vm=0x{} high8=0x{}", util::AsShortHexString(target), util::AsShortHexString(high8));
    }

    if (fmt == DYLD_CHAINED_PTR_ARM64E || fmt == DYLD_CHAINED_PTR_ARM64E_USERLAND ||
        fmt == DYLD_CHAINED_PTR_ARM64E_USERLAND24 || fmt == DYLD_CHAINED_PTR_ARM64E_KERNEL ||
        fmt == DYLD_CHAINED_PTR_ARM64E_FIRMWARE) {
        const bool auth = ((raw >> 63) & 0x1) != 0;
        const bool bind = ((raw >> 62) & 0x1) != 0;
        if (!auth && bind) {
            const uint64_t ordinal = raw & 0xFFFF;
            const uint64_t addend = (raw >> 32) & 0x7FFFF;
            return fmt::format("arm64e bind ordinal={} addend={}", ordinal, addend);
        }
        if (!auth && !bind) {
            const uint64_t target = raw & ((1ULL << 43) - 1);
            const uint64_t high8 = (raw >> 43) & 0xFF;
            return fmt::format("arm64e rebase target=0x{} high8=0x{}", util::AsShortHexString(target), util::AsShortHexString(high8));
        }
        if (auth && bind) {
            const uint64_t ordinal = raw & 0xFFFF;
            const uint64_t diversity = (raw >> 32) & 0xFFFF;
            const uint64_t addr_div = (raw >> 48) & 0x1;
            const uint64_t key = (raw >> 49) & 0x3;
            return fmt::format("arm64e auth-bind ordinal={} key={} addrDiv={} diversity=0x{}",
                               ordinal, key, addr_div, util::AsShortHexString(diversity));
        }
        const uint64_t target = raw & 0xFFFFFFFFULL;
        const uint64_t diversity = (raw >> 32) & 0xFFFF;
        const uint64_t addr_div = (raw >> 48) & 0x1;
        const uint64_t key = (raw >> 49) & 0x3;
        return fmt::format("arm64e auth-rebase target=0x{} key={} addrDiv={} diversity=0x{}",
                           util::AsShortHexString(target), key, addr_div, util::AsShortHexString(diversity));
    }

    return fmt::format("raw=0x{}", util::AsShortHexString(raw));
}

static bool InFile(const NodeContextPtr &ctx, const char *ptr, std::size_t sz) {
    if (ptr == nullptr) return false;
    const uint64_t base = reinterpret_cast<uint64_t>(ctx->file_start);
    const uint64_t p = reinterpret_cast<uint64_t>(ptr);
    if (p < base) return false;
    const uint64_t off = p - base;
    return off <= ctx->file_size && sz <= (ctx->file_size - off);
}

template <typename T>
static bool ReadAt(const NodeContextPtr &ctx, const char *base, uint64_t off, const T *&out) {
    out = nullptr;
    if (off > ctx->file_size) return false;
    const char *p = base + off;
    if (!InFile(ctx, p, sizeof(T))) return false;
    out = reinterpret_cast<const T *>(p);
    return true;
}

} // namespace

void DyldInfoViewNodeBase::Init(LoadCommand_DYLD_INFO *info)
{
    info_ = info;
}
void DyldInfoViewNodeBase::InitBindInfo(moex::LoadCommand_DYLD_INFO::BindNodeType node_type){
    using namespace moex::util;

    // "Opcodes"
    auto print_ = CreateTableView(info_->header());
    moex::LoadCommand_DYLD_INFO *info = info_;

    uint32_t bind_off,bind_size;
    if(node_type == moex::LoadCommand_DYLD_INFO::NodeTypeWeakBind){
        bind_off = info->cmd()->weak_bind_off;
        bind_size = info->cmd()->weak_bind_size;
    }else if(node_type == moex::LoadCommand_DYLD_INFO::NodeTypeLazyBind){
        bind_off = info->cmd()->lazy_bind_off;
        bind_size = info->cmd()->lazy_bind_size;
    }else{
        bind_off = info->cmd()->bind_off;
        bind_size = info->cmd()->bind_size;
    }

    info->ForEachBindingOpcode(node_type,bind_off,bind_size,[&](const moex::BindingOpcodeContext *ctx,moex::BindingOpcodeItem*codebase){
        switch(ctx->opcode){
            case BIND_OPCODE_DONE:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DONE*>(codebase);
                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),
                                ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });
                break;
            }
            case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("dylib ({})", code->lib_oridinal)
                               });
                break;
            }
            case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->lib_oridinal_addr)),moex::util::AsHexData(code->lib_oridinal_addr,code->lib_oridinal_size),
                                "uleb128",
                                fmt::format("dylib ({})", code->lib_oridinal)
                               });
                break;
            }
            case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("dylib ({})", code->lib_oridinal)
                               });

                break;
            }
            case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("flags ({})", code->symbol_flags)
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->symbol_name_addr)),
                                moex::util::AsHexData(code->symbol_name_addr,code->symbol_name_size).substr(0,16),
                                "string",
                                fmt::format("name ({})", code->symbol_name)
                               });

                break;
            }
            case BIND_OPCODE_SET_TYPE_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_TYPE_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("type ({})", ctx->GetBindTypeString())
                               });

                break;
            }
            case BIND_OPCODE_SET_ADDEND_SLEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_ADDEND_SLEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->addend_addr)),moex::util::AsHexData(code->addend_addr,code->addend_size),
                                "sleb128",
                                fmt::format("addend ({})", code->addend)
                               });
                break;
            }
            case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("segment ({})",code->segment_index)
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->offset_addr)),moex::util::AsHexData(code->offset_addr,code->offset_size),
                                "uleb128",
                                fmt::format("offset ({})", code->offset)
                               });
                break;
            }
            case BIND_OPCODE_ADD_ADDR_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_ADD_ADDR_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->offset_addr)),moex::util::AsHexData(code->offset_addr,code->offset_size),
                                "uleb128",
                                fmt::format("offset ({})", code->offset)
                               });
                break;
            }
            case BIND_OPCODE_DO_BIND:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });
                print_->AddRow({"-","-","-","-"});

                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->offset_addr)),moex::util::AsHexData(code->offset_addr,code->offset_size),
                                "uleb128",
                                fmt::format("offset ({})", code->offset)
                               });
                print_->AddRow({"-","-","-","-"});
                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("scaled ({})",code->scale)
                               });
                print_->AddRow({"-","-","-","-"});
                break;
            }
            case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB",
                                ""
                               });
                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->count_addr,code->count_size),
                                "uleb128",
                                fmt::format("count ({})", (int)code->count)
                               });
                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->skip_addr,code->skip_size),
                                "uleb128",
                                fmt::format("skip ({})", (int)code->skip)
                               });
                print_->AddRow({"-","-","-","-"});
                break;
            }
            default:
                break;
        }
    });

}

void RebaseInfoViewNode::InitViewDatas()
{
    using namespace moex::util;
    {
        // "Opcodes"
        auto print_ = CreateTableViewDataPtr();
        moex::LoadCommand_DYLD_INFO *info = info_;

        info->ForEachRebaseOpcode([&](const moex::RebaseOpcodeContext * ctx, moex::RebaseOpcodeItem * codebase){

            switch(ctx->opcode){
                case REBASE_OPCODE_DONE:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DONE*>(codebase);
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DONE",
                                    ""
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_TYPE_IMM:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_SET_TYPE_IMM*>(codebase);
                    std::string rebasetype = ctx->GetRebaseTypeString();
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_SET_TYPE_IMM",
                                    fmt::format("type ({}, {})", (int)ctx->type ,rebasetype)
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB*>(codebase);
                    uint32_t segment_index = code->segment_index;

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB",
                                    fmt::format("segment ({})",(int)segment_index)
                                   });

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    fmt::format("offset ({})", moex::util::AsHexString(code->offset))
                                   });

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_ULEB*>(codebase);
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_ADD_ADDR_ULEB",
                                    ""
                                   });

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    fmt::format("offset ({})",moex::util::AsHexString(code->offset))
                                   });

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_IMM_SCALED:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_IMM_SCALED*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_ADD_ADDR_IMM_SCALED",
                                    fmt::format("scale ({})", (int)code->scale)
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_IMM_TIMES:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_IMM_TIMES*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_IMM_TIMES",
                                    fmt::format("count ({})", (int)code->count)
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ULEB_TIMES:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES*>(codebase);
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_ULEB_TIMES",
                                    ""
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->count_addr,code->count_size),
                                    "uleb128",
                                    fmt::format("count ({})",(int)code->count)
                                   });
                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_ULEB*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB",
                                    ""
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    fmt::format("offset ({})",moex::util::AsHexString(code->offset))
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB",
                                    ""
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->count_addr,code->count_size),
                                    "uleb128",
                                    fmt::format("count ({})", (int)code->count)
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->skip_addr,code->skip_size),
                                    "uleb128",
                                    fmt::format("skip ({})",(int)code->skip)
                                   });
                    break;
                }
                default:{
                    break;
                }
            }
        });
        SetViewData(print_);
    }
    {
        auto b = CreateBinaryView();
        b->offset = (char*)info_->header()->header_start() + info_->cmd()->rebase_off;
        b->size = info_->cmd()->rebase_size;
        b->start_value = (uint64_t)b->offset - (uint64_t)info_->ctx()->file_start;
    }
}

void BindingInfoViewNode::InitViewDatas()
{
    {
        InitBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeBind);
    }
    {
        auto b = CreateBinaryView();
    }
}

void WeakBindingInfoViewNode::InitViewDatas()
{
    {
        InitBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeWeakBind);
    }
    {
        auto b = CreateBinaryView();
    }
}

void LazyBindingInfoViewNode::InitViewDatas()
{
    {
        InitBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeLazyBind);
    }
    {
        auto b = CreateBinaryView();
    }
}

void ExportInfoViewNode::InitViewDatas()
{
    using namespace moex::util;
    {
        // "Opcodes"
        auto print_ = CreateTableView(info_->header());

        moex::LoadCommand_DYLD_INFO *info = info_;

        int item_index = 0;
        info->ForEachExportItem([&](const moex::ExportContext *ctx,moex::ExportItem* item,moex::ExportChildItem* child){
            if(item){
                ++item_index;
                if(item_index > 1)
                    print_->AddRow({"-","-","-","-"});

                print_->AddRow({AsAddress(info->header()->GetRAW(item->terminal_size_addr)),
                                moex::util::AsHexData(item->terminal_size),
                                "Terminal Size",
                                fmt::format("{}",(int)item->terminal_size)
                               });

                if(item->terminal_size > 0){
                    print_->AddRow({AsAddress(info->header()->GetRAW(item->flags_addr)),
                                    moex::util::AsHexData(item->flags_addr,item->flags_size),
                                    "Flags",
                                    ""
                                   });

                    auto flags = item->GetFlags();
                    for(auto & flag : flags){
                        print_->AddRow({"","",std::get<0>(flag),std::get<1>(flag)});
                    }

                    print_->AddRow({AsAddress(info->header()->GetRAW(item->offset_addr)),
                                    moex::util::AsHexData(item->offset_addr,item->offset_size),
                                    "Symbol Offset",
                                    fmt::format("{}",(int)item->offset)
                                   });
                }

                print_->AddRow({AsAddress(info->header()->GetRAW(item->child_count_addr)),
                                moex::util::AsHexData(item->child_count),
                                "Child Count",
                                fmt::format("{}",(int)item->child_count)
                               });
            }
            if(child){
                print_->AddRow({AsAddress(info->header()->GetRAW(child->label_addr)),
                                moex::util::AsHexData(child->label_addr,child->label_size).substr(0,16),
                                "Node Label",
                                fmt::format("{}",child->label)
                               });
                print_->AddRow({AsAddress(info->header()->GetRAW(child->skip_addr)),
                                moex::util::AsHexData(child->skip_addr,child->skip_size),
                                "Next Node",
                                moex::util::AsAddress(child->skip)
                               });
            }

        });
    }
    {
        auto b = CreateBinaryView();
        b->offset = 0;
        b->size = 0;
    }
}

void ModernExportsTrieViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (cmd_ == nullptr || mh_ == nullptr) return;

    auto t = CreateTableView(mh_.get());
    t->SetHeaders({"Offset", "Type", "Name", "Value"});
    t->SetWidths({120, 220, 380, 340});

    const uint32_t dataoff = cmd_->cmd()->dataoff;
    const uint32_t datasize = cmd_->cmd()->datasize;
    char *blob = mh_->header_start() + dataoff;
    const char *blob_begin = blob;
    const char *blob_end = blob + datasize;
    if (!InFile(mh_->ctx(), blob, datasize)) {
        t->AddRow({"-", "error", "exports trie", "blob out of range"});
        return;
    }

    struct NodeEntry {
        const char *node;
        std::string prefix;
    };

    std::list<NodeEntry> work;
    work.push_back({blob, ""});
    std::unordered_set<uint64_t> visited;
    int guard = 0;

    while (!work.empty() && guard < 200000) {
        ++guard;
        NodeEntry cur = work.front();
        work.pop_front();

        if (cur.node < blob_begin || cur.node >= blob_end) continue;
        const uint64_t node_off = static_cast<uint64_t>(cur.node - blob_begin);
        if (visited.count(node_off) != 0) continue;
        visited.insert(node_off);

        uint64_t terminal_size = 0;
        uint32_t ts_size = 0;
        const char *p = util::readUnsignedLeb128(cur.node, terminal_size, ts_size);
        if (p == nullptr || p > blob_end) continue;

        t->AddRow({AsAddress(dataoff + node_off), "node", cur.prefix.empty() ? "<root>" : cur.prefix,
                   fmt::format("terminal_size={}", terminal_size)});

        const char *terminal_end = p + terminal_size;
        if (terminal_end <= blob_end && terminal_size > 0) {
            uint64_t flags = 0;
            uint32_t flags_size = 0;
            const char *q = util::readUnsignedLeb128(p, flags, flags_size);
            if (q != nullptr && q <= terminal_end) {
                t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(p - blob_begin)),
                           "terminal.flags",
                           cur.prefix,
                           fmt::format("0x{}", AsShortHexString(flags))});

                uint64_t value = 0;
                uint32_t value_size = 0;
                const char *r = util::readUnsignedLeb128(q, value, value_size);
                if (r != nullptr && r <= terminal_end) {
                    t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(q - blob_begin)),
                               "terminal.value",
                               cur.prefix,
                               fmt::format("0x{}", AsShortHexString(value))});
                }
            }
        }

        const char *children_ptr = p + terminal_size;
        if (children_ptr >= blob_end) continue;
        uint8_t child_count = static_cast<uint8_t>(*children_ptr++);
        t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(children_ptr - 1 - blob_begin)),
                   "child_count",
                   cur.prefix.empty() ? "<root>" : cur.prefix,
                   AsString(child_count)});

        for (uint8_t i = 0; i < child_count; ++i) {
            if (children_ptr >= blob_end) break;
            const char *label = children_ptr;
            const char *label_end = static_cast<const char *>(memchr(label, '\0', static_cast<size_t>(blob_end - label)));
            if (label_end == nullptr) break;
            std::string edge(label, static_cast<size_t>(label_end - label));
            children_ptr = label_end + 1;

            uint64_t child_delta = 0;
            uint32_t child_size = 0;
            const char *after = util::readUnsignedLeb128(children_ptr, child_delta, child_size);
            if (after == nullptr || after > blob_end) break;
            children_ptr = after;

            std::string fullname = cur.prefix + edge;
            t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(label - blob_begin)),
                       "edge",
                       fullname,
                       fmt::format("next=0x{}", AsShortHexString(child_delta))});

            if (child_delta < datasize) {
                work.push_back({blob_begin + child_delta, fullname});
            }
        }
    }

    auto b = CreateBinaryView();
    b->offset = blob;
    b->size = datasize;
    b->start_value = dataoff;
}

void ModernChainedFixupsViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (cmd_ == nullptr || mh_ == nullptr) return;

    auto t = CreateTableView(mh_.get());
    t->SetHeaders({"Offset", "Type", "Field", "Value"});
    t->SetWidths({120, 240, 240, 420});

    const uint32_t dataoff = cmd_->cmd()->dataoff;
    const uint32_t datasize = cmd_->cmd()->datasize;
    char *blob = mh_->header_start() + dataoff;
    if (!InFile(mh_->ctx(), blob, datasize)) {
        t->AddRow({"-", "error", "chained fixups", "blob out of range"});
        return;
    }

    const dyld_chained_fixups_header *header = nullptr;
    if (!ReadAt(mh_->ctx(), blob, 0, header)) {
        t->AddRow({"-", "error", "header", "truncated"});
        return;
    }

    t->AddRow({AsAddress(dataoff), "header", "fixups_version", AsString(header->fixups_version)});
    t->AddRow({AsAddress(dataoff + 4), "header", "starts_offset", AsShortHexString(header->starts_offset)});
    t->AddRow({AsAddress(dataoff + 8), "header", "imports_offset", AsShortHexString(header->imports_offset)});
    t->AddRow({AsAddress(dataoff + 12), "header", "symbols_offset", AsShortHexString(header->symbols_offset)});
    t->AddRow({AsAddress(dataoff + 16), "header", "imports_count", AsString(header->imports_count)});
    t->AddRow({AsAddress(dataoff + 20), "header", "imports_format", ImportFormatName(header->imports_format)});
    t->AddRow({AsAddress(dataoff + 24), "header", "symbols_format", AsString(header->symbols_format)});

    const dyld_chained_starts_in_image *starts_image = nullptr;
    if (!ReadAt(mh_->ctx(), blob, header->starts_offset, starts_image)) {
        t->AddRow({"-", "error", "starts_in_image", "truncated"});
    } else {
        t->AddRow({AsAddress(dataoff + header->starts_offset), "starts_in_image", "seg_count", AsString(starts_image->seg_count)});

        const char *seg_off_base = reinterpret_cast<const char *>(starts_image) + sizeof(uint32_t);
        for (uint32_t seg_index = 0; seg_index < starts_image->seg_count; ++seg_index) {
            const char *entry_ptr = seg_off_base + seg_index * sizeof(uint32_t);
            if (!InFile(mh_->ctx(), entry_ptr, sizeof(uint32_t))) break;
            const uint32_t seg_info_off = *reinterpret_cast<const uint32_t *>(entry_ptr);
            t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(entry_ptr - blob)), "segment", fmt::format("seg[{}].info_offset", seg_index), AsShortHexString(seg_info_off)});

            if (seg_info_off == 0 || seg_info_off >= datasize) continue;
            const dyld_chained_starts_in_segment *seg = nullptr;
            if (!ReadAt(mh_->ctx(), blob, seg_info_off, seg)) continue;
            t->AddRow({AsAddress(dataoff + seg_info_off), "segment", fmt::format("seg[{}].page_size", seg_index), AsString(seg->page_size)});
            t->AddRow({AsAddress(dataoff + seg_info_off + 6), "segment", fmt::format("seg[{}].pointer_format", seg_index), PointerFormatName(seg->pointer_format)});
            t->AddRow({AsAddress(dataoff + seg_info_off + 8), "segment", fmt::format("seg[{}].segment_offset", seg_index), AsShortHexString(seg->segment_offset)});
            t->AddRow({AsAddress(dataoff + seg_info_off + 20), "segment", fmt::format("seg[{}].page_count", seg_index), AsString(seg->page_count)});

            const char *page_starts = reinterpret_cast<const char *>(seg) + sizeof(dyld_chained_starts_in_segment);
            const char *overflow_starts = page_starts + static_cast<size_t>(seg->page_count) * sizeof(uint16_t);
            auto decode_chain = [&](uint16_t start_offset, uint16_t page_index, int multi_index) {
                uint64_t fixup_off = seg->segment_offset + static_cast<uint64_t>(page_index) * seg->page_size + start_offset;
                for (int chain_index = 0; chain_index < 1024; ++chain_index) {
                    if (!Is64BitPointerFormat(seg->pointer_format)) {
                        if (fixup_off + sizeof(uint32_t) > mh_->ctx()->file_size) break;
                        const uint32_t raw32 = *reinterpret_cast<const uint32_t *>(reinterpret_cast<const char *>(mh_->ctx()->file_start) + fixup_off);
                        const uint64_t next = DecodeNext(seg->pointer_format, raw32);
                        t->AddRow({AsAddress(fixup_off),
                                   "fixup32",
                                   fmt::format("seg[{}] page[{}] chain[{}:{}]", seg_index, page_index, multi_index, chain_index),
                                   fmt::format("raw=0x{} next={}", AsShortHexString(raw32), next)});
                        if (next == 0) break;
                        fixup_off += next * 4;
                        continue;
                    }

                    if (fixup_off + sizeof(uint64_t) > mh_->ctx()->file_size) break;
                    const uint64_t raw = *reinterpret_cast<const uint64_t *>(reinterpret_cast<const char *>(mh_->ctx()->file_start) + fixup_off);
                    const uint64_t next = DecodeNext(seg->pointer_format, raw);
                    const std::string detail = DecodePointerDetail(seg->pointer_format, raw);
                    t->AddRow({AsAddress(fixup_off),
                               "fixup",
                               fmt::format("seg[{}] page[{}] chain[{}:{}]", seg_index, page_index, multi_index, chain_index),
                               fmt::format("raw=0x{} next={} {}", AsShortHexString(raw), next, detail)});
                    if (next == 0) break;
                    fixup_off += next * 4;
                }
            };

            for (uint16_t page_index = 0; page_index < seg->page_count; ++page_index) {
                const char *ps = page_starts + page_index * sizeof(uint16_t);
                if (!InFile(mh_->ctx(), ps, sizeof(uint16_t))) break;
                const uint16_t start = *reinterpret_cast<const uint16_t *>(ps);
                if (start == DYLD_CHAINED_PTR_START_NONE) continue;

                t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(ps - blob)),
                           "page_start",
                           fmt::format("seg[{}] page[{}]", seg_index, page_index),
                           AsShortHexString(start)});

                if ((start & DYLD_CHAINED_PTR_START_MULTI) == 0) {
                    decode_chain(start, page_index, 0);
                    continue;
                }

                const uint16_t overflow_index = static_cast<uint16_t>(start & ~DYLD_CHAINED_PTR_START_LAST);
                t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(ps - blob)),
                           "page_start_multi",
                           fmt::format("seg[{}] page[{}].overflow_index", seg_index, page_index),
                           AsString(overflow_index)});

                for (int overflow_iter = 0; overflow_iter < 1024; ++overflow_iter) {
                    const char *ov = overflow_starts + (overflow_index + overflow_iter) * sizeof(uint16_t);
                    if (!InFile(mh_->ctx(), ov, sizeof(uint16_t))) break;
                    const uint16_t ov_start = *reinterpret_cast<const uint16_t *>(ov);
                    const bool is_last = (ov_start & DYLD_CHAINED_PTR_START_LAST) != 0;
                    const uint16_t start_off = static_cast<uint16_t>(ov_start & ~DYLD_CHAINED_PTR_START_LAST);
                    t->AddRow({AsAddress(dataoff + static_cast<uint64_t>(ov - blob)),
                               "page_start_multi.item",
                               fmt::format("seg[{}] page[{}] overflow[{}]", seg_index, page_index, overflow_iter),
                               fmt::format("start=0x{} last={}", AsShortHexString(start_off), is_last ? "true" : "false")});
                    decode_chain(start_off, page_index, overflow_iter);
                    if (is_last) break;
                }
            }
        }
    }

    if (header->imports_offset < datasize && header->imports_count > 0) {
        t->AddRow({"-", "-", "-", "-"});
        t->AddRow({AsAddress(dataoff + header->imports_offset), "imports", "format", ImportFormatName(header->imports_format)});
        for (uint32_t i = 0; i < header->imports_count && i < 4096; ++i) {
            uint64_t entry_off = 0;
            uint64_t lib_ordinal = 0;
            uint64_t weak = 0;
            uint64_t name_off = 0;
            std::string addend_detail = "-";
            if (header->imports_format == 1) {
                entry_off = header->imports_offset + static_cast<uint64_t>(i) * sizeof(dyld_chained_import);
                if (entry_off + sizeof(dyld_chained_import) > datasize) break;
                const auto *im = reinterpret_cast<const dyld_chained_import *>(blob + entry_off);
                lib_ordinal = im->lib_ordinal;
                weak = im->weak_import;
                name_off = im->name_offset;
            } else if (header->imports_format == 2) {
                entry_off = header->imports_offset + static_cast<uint64_t>(i) * sizeof(dyld_chained_import_addend);
                if (entry_off + sizeof(dyld_chained_import_addend) > datasize) break;
                const auto *im = reinterpret_cast<const dyld_chained_import_addend *>(blob + entry_off);
                lib_ordinal = im->lib_ordinal;
                weak = im->weak_import;
                name_off = im->name_offset;
                addend_detail = AsString(im->addend);
            } else if (header->imports_format == 3) {
                entry_off = header->imports_offset + static_cast<uint64_t>(i) * sizeof(dyld_chained_import_addend64);
                if (entry_off + sizeof(dyld_chained_import_addend64) > datasize) break;
                const auto *im = reinterpret_cast<const dyld_chained_import_addend64 *>(blob + entry_off);
                lib_ordinal = im->lib_ordinal;
                weak = im->weak_import;
                name_off = im->name_offset;
                addend_detail = AsShortHexString(im->addend);
            } else {
                break;
            }

            std::string symbol_name = "<invalid>";
            if (header->symbols_offset + name_off < datasize) {
                const char *sp = blob + header->symbols_offset + name_off;
                const char *end = static_cast<const char *>(memchr(sp, '\0', static_cast<size_t>(blob + datasize - sp)));
                if (end != nullptr) symbol_name.assign(sp, static_cast<size_t>(end - sp));
            }
            t->AddRow({AsAddress(dataoff + entry_off), "import", fmt::format("#{}", i),
                       fmt::format("lib={} weak={} addend={} symbol={}", lib_ordinal, weak, addend_detail, symbol_name)});
        }
    }

    auto b = CreateBinaryView();
    b->offset = blob;
    b->size = datasize;
    b->start_value = dataoff;
}

void DynamicLoaderInfoViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    if(rebase_)callback(rebase_.get());
    if(binding_)callback(binding_.get());
    if(weak_binding_)callback(weak_binding_.get());
    if(lazy_binding_)callback(lazy_binding_.get());
    if(export_)callback(export_.get());
    if(modern_exports_trie_)callback(modern_exports_trie_.get());
    if(modern_chained_fixups_)callback(modern_chained_fixups_.get());
}

void DynamicLoaderInfoViewNode::Init(MachHeaderPtr mh){
    using namespace moex::util;
    mh_ = mh;

    moex::LoadCommand_DYLD_INFO *info= mh_->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({LC_DYLD_INFO,LC_DYLD_INFO_ONLY});

    if(info != nullptr){
        qv_dyld_info_command *cmd = info->cmd();

        if(cmd->rebase_off * cmd->rebase_size > 0){
            rebase_ = std::make_shared<RebaseInfoViewNode>();
            rebase_->Init(info);
        }
        if(cmd->bind_off * cmd->bind_size > 0){
            binding_ = std::make_shared<BindingInfoViewNode>();
            binding_->Init(info);
        }
        if(cmd->weak_bind_off * cmd->weak_bind_size > 0){
            weak_binding_ = std::make_shared<WeakBindingInfoViewNode>();
            weak_binding_->Init(info);
        }
        if(cmd->lazy_bind_off * cmd->lazy_bind_size > 0){
            lazy_binding_ = std::make_shared<LazyBindingInfoViewNode>();
            lazy_binding_->Init(info);
        }
        if(cmd->export_off * cmd->export_size > 0){
            export_ = std::make_shared<ExportInfoViewNode>();
            export_->Init(info);
        }
    }

    auto *modern_export_cmd = mh_->FindLoadCommand<LoadCommand_LC_DYLD_EXPORTS_TRIE>({LC_DYLD_EXPORTS_TRIE});
    if(modern_export_cmd != nullptr && modern_export_cmd->cmd()->datasize > 0){
        modern_exports_trie_ = std::make_shared<ModernExportsTrieViewNode>();
        modern_exports_trie_->Init(mh_, modern_export_cmd);
    }

    auto *modern_fixups_cmd = mh_->FindLoadCommand<LoadCommand_LC_DYLD_CHAINED_FIXUPS>({LC_DYLD_CHAINED_FIXUPS});
    if(modern_fixups_cmd != nullptr && modern_fixups_cmd->cmd()->datasize > 0){
        modern_chained_fixups_ = std::make_shared<ModernChainedFixupsViewNode>();
        modern_chained_fixups_->Init(mh_, modern_fixups_cmd);
    }
}

void DynamicLoaderInfoViewNode::InitViewDatas()
{
    using namespace moex::util;
    {
        auto t = CreateTableView(mh_.get());

        moex::LoadCommand_DYLD_INFO *c= mh_->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({LC_DYLD_INFO,LC_DYLD_INFO_ONLY});
        if(c){
            t->AddRow(c->cmd()->rebase_off,"Rebase Info Offset",AsShortHexString(c->cmd()->rebase_off));
            t->AddRow(c->cmd()->rebase_size,"Rebase Info Size",AsShortHexString(c->cmd()->rebase_size));

            t->AddRow(c->cmd()->bind_off,"Binding Info Offset",AsShortHexString(c->cmd()->bind_off));
            t->AddRow(c->cmd()->bind_size,"Binding Info Size",AsShortHexString(c->cmd()->bind_size));

            t->AddRow(c->cmd()->weak_bind_off,"Weak Binding Info Offset",AsShortHexString(c->cmd()->weak_bind_off));
            t->AddRow(c->cmd()->weak_bind_size,"Weak Binding Info Size",AsShortHexString(c->cmd()->weak_bind_size));

            t->AddRow(c->cmd()->lazy_bind_off,"Lazy Binding Info Offset",AsShortHexString(c->cmd()->lazy_bind_off));
            t->AddRow(c->cmd()->lazy_bind_size,"Lazy Binding Info Size",AsShortHexString(c->cmd()->lazy_bind_size));

            t->AddRow(c->cmd()->export_off,"Export Info Offset",AsShortHexString(c->cmd()->export_off));
            t->AddRow(c->cmd()->export_size,"Export Info Size",AsShortHexString(c->cmd()->export_size));
        }

        auto *modern_export_cmd = mh_->FindLoadCommand<LoadCommand_LC_DYLD_EXPORTS_TRIE>({LC_DYLD_EXPORTS_TRIE});
        if(modern_export_cmd){
            t->AddRow(modern_export_cmd->cmd()->dataoff,"Exports Trie Offset (Modern)",AsShortHexString(modern_export_cmd->cmd()->dataoff));
            t->AddRow(modern_export_cmd->cmd()->datasize,"Exports Trie Size (Modern)",AsShortHexString(modern_export_cmd->cmd()->datasize));
        }

        auto *modern_fixups_cmd = mh_->FindLoadCommand<LoadCommand_LC_DYLD_CHAINED_FIXUPS>({LC_DYLD_CHAINED_FIXUPS});
        if(modern_fixups_cmd){
            t->AddRow(modern_fixups_cmd->cmd()->dataoff,"Chained Fixups Offset (Modern)",AsShortHexString(modern_fixups_cmd->cmd()->dataoff));
            t->AddRow(modern_fixups_cmd->cmd()->datasize,"Chained Fixups Size (Modern)",AsShortHexString(modern_fixups_cmd->cmd()->datasize));
        }
    }
}


MOEX_NAMESPACE_END
