#include "SwiftSemanticGraphViewNode.h"
#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

MOEX_NAMESPACE_BEGIN

namespace {

struct SwiftContextDescriptorHeader {
    uint32_t flags = 0;
    int32_t parent = 0;
    int32_t name = 0;
};

struct SwiftFieldDescriptorHeader {
    int32_t mangled_type_name = 0;
    int32_t superclass = 0;
    uint16_t kind = 0;
    uint16_t field_record_size = 0;
    uint32_t num_fields = 0;
};

static bool VmToPtr(MachHeader *mh, uint64_t vmaddr, char *&out) {
    out = nullptr;
    for (auto *seg : mh->GetSegments64()) {
        const uint64_t seg_vm = seg->cmd()->vmaddr;
        const uint64_t seg_vmsize = seg->cmd()->vmsize;
        if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_vmsize) continue;
        const uint64_t delta = vmaddr - seg_vm;
        if (delta >= seg->cmd()->filesize) return false;
        const uint64_t fileoff = seg->cmd()->fileoff + delta;
        if (fileoff >= mh->ctx()->file_size) return false;
        out = reinterpret_cast<char *>(mh->ctx()->file_start) + fileoff;
        return true;
    }
    for (auto *seg : mh->GetSegments()) {
        const uint64_t seg_vm = seg->cmd()->vmaddr;
        const uint64_t seg_vmsize = seg->cmd()->vmsize;
        if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_vmsize) continue;
        const uint64_t delta = vmaddr - seg_vm;
        if (delta >= seg->cmd()->filesize) return false;
        const uint64_t fileoff = seg->cmd()->fileoff + delta;
        if (fileoff >= mh->ctx()->file_size) return false;
        out = reinterpret_cast<char *>(mh->ctx()->file_start) + fileoff;
        return true;
    }
    return false;
}

static std::string TryReadCString(MachHeader *mh, uint64_t vmaddr, uint64_t max_len = 512) {
    char *raw = nullptr;
    if (!VmToPtr(mh, vmaddr, raw) || raw == nullptr) return "";
    const uint64_t base = reinterpret_cast<uint64_t>(mh->ctx()->file_start);
    const uint64_t pos = reinterpret_cast<uint64_t>(raw) - base;
    if (pos >= mh->ctx()->file_size) return "";
    const uint64_t limit = std::min<uint64_t>(mh->ctx()->file_size - pos, max_len);
    uint64_t i = 0;
    for (; i < limit; ++i) {
        const unsigned char c = static_cast<unsigned char>(raw[i]);
        if (c == '\0') break;
        if (c < 0x20 || c > 0x7e) return "";
    }
    if (i == 0 || i >= limit) return "";
    return std::string(raw, static_cast<std::size_t>(i));
}

static std::string DemangleSwiftSymbolExternal(const std::string &name) {
#ifdef __APPLE__
    if (name.empty()) return "";
    const std::string cmd = "xcrun swift-demangle '" + name + "' 2>/dev/null";
    FILE *fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) return "";
    std::string output;
    char buf[4096] = {0};
    while (fgets(buf, sizeof(buf), fp) != nullptr) output += buf;
    pclose(fp);
    while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) output.pop_back();
    const std::string marker = " ---> ";
    const auto pos = output.find(marker);
    if (pos != std::string::npos && pos + marker.size() < output.size()) {
        return output.substr(pos + marker.size());
    }
    return output;
#else
    (void)name;
    return "";
#endif
}

static std::string ContextKindName(uint32_t kind) {
    switch (kind) {
        case 0: return "module";
        case 1: return "extension";
        case 2: return "anonymous";
        case 3: return "protocol";
        case 16: return "class";
        case 17: return "struct";
        case 18: return "enum";
        default: return "unknown";
    }
}

} // namespace

void SwiftSemanticGraphViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!mh_) return;

    auto t = CreateTableView(mh_.get());
    t->SetHeaders({"Kind", "Name", "Section", "Address", "Details"});
    t->SetWidths({170, 280, 180, 180, 500});

    std::unordered_map<uint64_t, std::string> symbols;
    std::vector<std::pair<uint64_t, std::string>> sorted_symbols;
    auto *symtab = mh_->FindLoadCommand<LoadCommand_LC_SYMTAB>({LC_SYMTAB});
    if (symtab != nullptr) {
        for (auto &n : symtab->nlists_ref()) {
            if (n->n_strx() == 0) continue;
            uint64_t vm = n->Is64() ? n->n_value64() : n->n_value();
            if (vm == 0) continue;
            if (symbols.count(vm) == 0) {
                const std::string s = symtab->GetStringByStrX(n->n_strx());
                if (!s.empty()) {
                    symbols[vm] = s;
                    sorted_symbols.push_back({vm, s});
                }
            }
        }
    }
    std::sort(sorted_symbols.begin(), sorted_symbols.end(),
              [](const std::pair<uint64_t, std::string> &a, const std::pair<uint64_t, std::string> &b) {
                  return a.first < b.first;
              });
    auto nearest_symbol = [&](uint64_t addr) -> std::string {
        auto exact = symbols.find(addr);
        if (exact != symbols.end()) return exact->second;
        if (sorted_symbols.empty()) return "";
        auto up = std::upper_bound(
                sorted_symbols.begin(), sorted_symbols.end(), addr,
                [](uint64_t value, const std::pair<uint64_t, std::string> &item) {
                    return value < item.first;
                });
        if (up == sorted_symbols.begin()) return "";
        --up;
        if (addr == up->first) return up->second;
        return fmt::format("{}+0x{}", up->second, AsShortHexString(addr - up->first));
    };

    struct SwiftSectionRef {
        std::string segname;
        std::string sectname;
        char *offset = nullptr;
        uint32_t size = 0;
        uint64_t vmaddr = 0;
        bool is64 = false;
        MachSection *sect = nullptr;
    };
    std::vector<SwiftSectionRef> sections;

    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT>({LC_SEGMENT}, [&](LoadCommand_LC_SEGMENT *seg, bool &stop) {
        for (auto &s : seg->sections_ref()) {
            const std::string name = s->sect().section_name();
            if (name.rfind("__swift5_", 0) != 0) continue;
            sections.push_back({s->sect().segment_name(), name, s->GetOffset(), s->GetSize(), s->sect().addr(), false, s.get()});
        }
    });
    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64}, [&](LoadCommand_LC_SEGMENT_64 *seg, bool &stop) {
        for (auto &s : seg->sections_ref()) {
            const std::string name = s->sect().section_name();
            if (name.rfind("__swift5_", 0) != 0) continue;
            sections.push_back({s->sect().segment_name(), name, s->GetOffset(), s->GetSize(), s->sect().addr64(), true, s.get()});
        }
    });

    std::sort(sections.begin(), sections.end(), [](const SwiftSectionRef &a, const SwiftSectionRef &b) {
        if (a.segname != b.segname) return a.segname < b.segname;
        return a.sectname < b.sectname;
    });

    std::map<uint64_t, std::string> entity_names;
    std::vector<std::tuple<uint64_t, uint64_t, std::string>> edges;
    std::unordered_set<std::string> edge_dedup;

    for (const auto &sec : sections) {
        const std::string sec_label = sec.segname + "/" + sec.sectname;
        t->AddRow({ "section", sec_label, sec_label, AsAddress(sec.vmaddr), fmt::format("size={}", sec.size) });

        if (sec.sectname == "__swift5_reflstr") {
            uint32_t cursor = 0;
            uint32_t idx = 0;
            while (cursor < sec.size) {
                char *cur = sec.offset + cursor;
                uint32_t len = 0;
                while (cursor + len < sec.size && cur[len] != '\0') ++len;
                if (len == 0) { ++cursor; continue; }
                bool printable = true;
                for (uint32_t i = 0; i < len; ++i) {
                    const unsigned char c = static_cast<unsigned char>(cur[i]);
                    if (c < 0x20 || c > 0x7e) { printable = false; break; }
                }
                if (!printable) { ++cursor; continue; }
                const uint64_t vm = sec.vmaddr + cursor;
                const std::string s(cur, len);
                entity_names[vm] = s;
                t->AddRow(cur, len + 1, {"reflstr", fmt::format("#{}", idx++), sec_label, AsAddress(vm), s});
                cursor += len + 1;
            }
            continue;
        }

        if (sec.sectname == "__swift5_fieldmd") {
            uint32_t cursor = 0;
            while (cursor + sizeof(SwiftFieldDescriptorHeader) <= sec.size) {
                char *raw = sec.offset + cursor;
                auto *fd = reinterpret_cast<const SwiftFieldDescriptorHeader *>(raw);
                const uint64_t vm = sec.vmaddr + cursor;
                t->AddRow(raw, sizeof(SwiftFieldDescriptorHeader), {
                    "fieldmd", "descriptor", sec_label, AsAddress(vm),
                    fmt::format("kind={} fields={} record={}", fd->kind, fd->num_fields, fd->field_record_size)
                });
                if (fd->field_record_size < 12 || fd->field_record_size > 256 || fd->num_fields > 65535) break;
                cursor += sizeof(SwiftFieldDescriptorHeader) + (fd->field_record_size * fd->num_fields);
            }
            continue;
        }

        const uint32_t count = sec.size / sizeof(int32_t);
        for (uint32_t i = 0; i < count; ++i) {
            char *raw = sec.offset + i * sizeof(int32_t);
            const int32_t rel = *reinterpret_cast<int32_t *>(raw);
            const uint64_t from_vm = sec.vmaddr + i * sizeof(int32_t);
            const uint64_t target_vm = static_cast<uint64_t>(static_cast<int64_t>(from_vm) + rel);
            char *target_raw = nullptr;
            VmToPtr(mh_.get(), target_vm, target_raw);

            std::string kind = "rel32";
            std::string detail = "-";
            if (sec.sectname == "__swift5_types" || sec.sectname == "__swift5_protos") {
                kind = (sec.sectname == "__swift5_types") ? "type-ref" : "proto-ref";
                if (target_raw != nullptr) {
                    const auto *desc = reinterpret_cast<const SwiftContextDescriptorHeader *>(target_raw);
                    std::string name;
                    const uint64_t name_vm = target_vm + 8 + static_cast<int64_t>(desc->name);
                    name = TryReadCString(mh_.get(), name_vm);
                    if (name.empty()) name = "<unnamed>";
                    entity_names[target_vm] = name;
                    detail = fmt::format("{} flags=0x{} name={}",
                                         ContextKindName(desc->flags & 0x1fU),
                                         AsShortHexString(desc->flags),
                                         name);
                }
            } else if (sec.sectname == "__swift5_typeref" || sec.sectname == "__swift5_assocty" || sec.sectname == "__swift5_capture") {
                const std::string text = TryReadCString(mh_.get(), target_vm, 256);
                if (!text.empty()) {
                    const std::string dem = DemangleSwiftSymbolExternal(text);
                    detail = dem.empty() ? text : dem;
                    entity_names[target_vm] = detail;
                }
            }

            if (target_vm != 0) {
                const std::string edge_key = fmt::format("{}:{}:{}", sec_label, from_vm, target_vm);
                if (edge_dedup.insert(edge_key).second) {
                    edges.push_back({from_vm, target_vm, sec_label});
                }
            }
            std::string sym = nearest_symbol(target_vm);
            if (!sym.empty()) detail += " symbol=" + sym;
            t->AddRow(raw, sizeof(int32_t), {kind, fmt::format("#{}", i), sec_label, AsAddress(target_vm), detail});
        }
    }

    if (!edges.empty()) {
        t->AddSeparator();
        t->AddRow({"edge", "Swift Metadata Relations", "-", "-", fmt::format("count={}", edges.size())});
        std::size_t idx = 0;
        for (const auto &e : edges) {
            const uint64_t from = std::get<0>(e);
            const uint64_t to = std::get<1>(e);
            const std::string from_name = entity_names.count(from) ? entity_names[from] : AsAddress(from);
            const std::string to_name = entity_names.count(to) ? entity_names[to] : AsAddress(to);
            t->AddRow({ "edge", fmt::format("#{}", idx++), std::get<2>(e), AsAddress(from),
                        fmt::format("{} -> {}", from_name, to_name) });
        }
    } else {
        t->AddSeparator();
        t->AddRow({"-", "No Swift metadata edges found", "-", "-", "-"});
    }
}

MOEX_NAMESPACE_END
