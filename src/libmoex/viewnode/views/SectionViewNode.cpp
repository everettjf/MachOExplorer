//
// Created by everettjf on 2017/8/6.
//

#include "SectionViewNode.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include "../../node/loadcmd/LoadCommand_LINKEDIT_DATA.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <limits>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(MOEX_HAS_CAPSTONE) && MOEX_HAS_CAPSTONE
#include <capstone/capstone.h>
#endif

MOEX_NAMESPACE_BEGIN
using namespace moex::util;

class SectionViewChildNode_CString : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();

        // title
        auto t = CreateTableView();
        t->SetHeaders({"Index","Offset","Length","String"});
        t->SetWidths({80,100,100,80,400});

        int lineno = 0;
        d_->ForEachAs_S_CSTRING_LITERALS([&](char *cur){
            std::string name(cur);
            t->AddRow((void*)cur,
                      (uint64_t )name.length(),
                      {
                              AsString(lineno),
                              AsHexString(d_->GetRAW(cur)),
                              AsString(name.length()),
                              name
                      });

//            std::string symbolname = fmt::format("{0:#x}:\"{1}\"",(uint64_t)cur,name);

            ++lineno;
        });
    }
};
class SectionViewChildNode_Literals : public SectionViewChildNode{
protected:
    size_t unitsize_ = 0;
public:
    void set_unitsize(size_t sz){ unitsize_ = sz;}

    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView();

        d_->ForEachAs_N_BYTE_LITERALS([&](void *cur){
            std::string name = AsHexData(cur,unitsize_);

            t->AddRow((void*)cur,(uint64_t )unitsize_,"Floating Point Number",name);
//            std::string symbolname = fmt::format("{0:#X}:{1}",cur, name);
        },unitsize_);
    }
};
class SectionViewChildNode_Pointers : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView(d_.get());

        d_->ForEachAs_POINTERS([&](void * ptr){
            if(d_->Is64()){
                uint64_t *cur = static_cast<uint64_t*>(ptr);
                std::string symbolname = fmt::format("{}->{}" , AsShortHexString((uint64_t)cur) ,AsShortHexString(*cur));
                t->AddRow((void*)cur,(uint64_t)sizeof(*cur),"Pointer",symbolname);
            }else{
                uint32_t *cur = static_cast<uint32_t*>(ptr);
                std::string symbolname = fmt::format("{}->{}",AsShortHexString((uint64_t)cur),AsShortHexString(*cur) );
                t->AddRow((void*)cur,(uint64_t)sizeof(*cur),"Pointer",symbolname);
            }
        });
    }
};
class SectionViewChildNode_IndirectPointers : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView(d_.get());

        d_->ForEachAs_POINTERS([&](void * ptr){
            if(d_->Is64()){
                uint64_t *cur = static_cast<uint64_t*>(ptr);
                t->AddRow((void*)cur,(uint64_t)sizeof(*cur),"Indirect Pointer",AsShortHexString(*cur));
            }else{
                uint32_t *cur = static_cast<uint32_t*>(ptr);
                t->AddRow((void*)cur,(uint64_t )sizeof(*cur),"Indirect Pointer",AsShortHexString(*cur));
            }
        });
    }
};
class SectionViewChildNode_IndirectStubs : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView(d_.get());

        d_->ForEachAs_S_SYMBOL_STUBS([&](void * cur,size_t unitsize){
            t->AddRow(cur,(uint64_t )unitsize,"Indirect Stub",AsHexData(cur,unitsize));
        });
    }
};
class SectionViewChildNode_CFString : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView();
        t->SetHeaders({"Index","Offset","Length","String"});
        t->SetWidths({80,100,100,80,400});

        if(d_->Is64()){
            auto results = util::ParsePointerAsType<cfstring64_t>(GetOffset(),GetSize());
            for(auto *cur : results){
                t->AddRow(cur->ptr,"CFString Ptr",AsShortHexString(cur->ptr));
                t->AddRow(cur->data,"Data",AsShortHexString(cur->data));
                t->AddRow(cur->cstr,"String",AsShortHexString(cur->cstr));
                t->AddRow(cur->size,"Size",AsHexString(cur->size));

                t->AddSeparator();
            }
        }else{
            auto results = util::ParsePointerAsType<cfstring_t>(GetOffset(),GetSize());
            for(auto *cur : results){
                t->AddRow(cur->ptr,"CFString Ptr",AsShortHexString(cur->ptr));
                t->AddRow(cur->data,"Data",AsShortHexString(cur->data));
                t->AddRow(cur->cstr,"String",AsShortHexString(cur->cstr));
                t->AddRow(cur->size,"Size",AsHexString(cur->size));

                t->AddSeparator();
            }
        }
    }
};
class SectionViewChildNode_ObjC2Pointer : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView();
        t->SetHeaders({"Index","Offset","Length","String"});
        t->SetWidths({80,100,100,80,400});

        d_->ForEachAs_ObjC2Pointer([&](void * ptr){
            if(d_->Is64()){
                uint64_t *cur = static_cast<uint64_t*>(ptr);
                t->AddRow(*cur,"Pointer",
                          AsShortHexString(*cur));
                // todo rva to symbol
            }else{
                uint32_t *cur = static_cast<uint32_t*>(ptr);
                t->AddRow(*cur,"Pointer",
                          AsShortHexString(*cur));
                // todo rva to symbol
            }
        });
    }
};
class SectionViewChildNode_ObjC2ImageInfo : public SectionViewChildNode{
public:
    void InitViewDatas()override {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView();
        d_->ParseAsObjCImageInfo([&](objc_image_info *info){

            t->AddRow(info->version,"Version",AsShortHexString(info->version));
            t->AddRow(info->flags,"Flags",AsShortHexString(info->flags));

            if(info->flags & OBJC_IMAGE_IS_REPLACEMENT)
                t->AddRow({"","0x1","OBJC_IMAGE_IS_REPLACEMENT"});
            if(info->flags & OBJC_IMAGE_SUPPORTS_GC)
                t->AddRow({"","0x2","OBJC_IMAGE_SUPPORTS_GC"});
            if(info->flags & OBJC_IMAGE_GC_ONLY)
                t->AddRow({"","0x4","OBJC_IMAGE_GC_ONLY"});
        });
    }
};

class SectionViewChildNode_SwiftMetadata : public SectionViewChildNode{
private:
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

    struct SwiftFieldRecord {
        uint32_t flags = 0;
        int32_t mangled_type_name = 0;
        int32_t field_name = 0;
    };

    static bool VmToPtr(MachHeader *header, const NodeContextPtr &ctx, bool is64, uint64_t vmaddr, char *&out) {
        out = nullptr;
        if (is64) {
            for (auto *seg : header->GetSegments64()) {
                const uint64_t seg_vm = seg->cmd()->vmaddr;
                const uint64_t seg_vmsize = seg->cmd()->vmsize;
                if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_vmsize) continue;
                const uint64_t delta = vmaddr - seg_vm;
                if (delta >= seg->cmd()->filesize) return false;
                const uint64_t fileoff = seg->cmd()->fileoff + delta;
                if (fileoff >= ctx->file_size) return false;
                out = reinterpret_cast<char *>(ctx->file_start) + fileoff;
                return true;
            }
            return false;
        }

        for (auto *seg : header->GetSegments()) {
            const uint64_t seg_vm = seg->cmd()->vmaddr;
            const uint64_t seg_vmsize = seg->cmd()->vmsize;
            if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_vmsize) continue;
            const uint64_t delta = vmaddr - seg_vm;
            if (delta >= seg->cmd()->filesize) return false;
            const uint64_t fileoff = seg->cmd()->fileoff + delta;
            if (fileoff >= ctx->file_size) return false;
            out = reinterpret_cast<char *>(ctx->file_start) + fileoff;
            return true;
        }
        return false;
    }

    static bool IsFileRangeValid(const NodeContextPtr &ctx, const void *ptr, std::size_t size) {
        if (ctx == nullptr || ptr == nullptr) return false;
        const uint64_t base = reinterpret_cast<uint64_t>(ctx->file_start);
        const uint64_t p = reinterpret_cast<uint64_t>(ptr);
        if (p < base) return false;
        const uint64_t rel = p - base;
        return rel <= ctx->file_size && size <= (ctx->file_size - rel);
    }

    static bool ResolveRelativeTarget(
            MachHeader *header,
            const NodeContextPtr &ctx,
            bool is64,
            uint64_t rel_field_vmaddr,
            int32_t rel,
            uint64_t &target_vmaddr,
            char *&target_raw) {
        target_vmaddr = 0;
        target_raw = nullptr;
        if (rel == 0) return false;
        target_vmaddr = static_cast<uint64_t>(static_cast<int64_t>(rel_field_vmaddr) + static_cast<int64_t>(rel));
        return VmToPtr(header, ctx, is64, target_vmaddr, target_raw);
    }

    static std::string TryReadCString(MachHeader *header, const NodeContextPtr &ctx, bool is64, uint64_t vmaddr) {
        char *raw = nullptr;
        if (!VmToPtr(header, ctx, is64, vmaddr, raw) || raw == nullptr) return "";
        const uint64_t base = reinterpret_cast<uint64_t>(ctx->file_start);
        const uint64_t pos = reinterpret_cast<uint64_t>(raw) - base;
        if (pos >= ctx->file_size) return "";
        const uint64_t max_len = std::min<uint64_t>(ctx->file_size - pos, 512);
        uint64_t i = 0;
        for (; i < max_len; ++i) {
            if (raw[i] == '\0') break;
            if (static_cast<unsigned char>(raw[i]) < 0x20 || static_cast<unsigned char>(raw[i]) > 0x7e) return "";
        }
        if (i == 0 || i >= max_len) return "";
        return std::string(raw, static_cast<std::size_t>(i));
    }

    static std::string TryReadPrintableAscii(MachHeader *header, const NodeContextPtr &ctx, bool is64, uint64_t vmaddr, uint64_t max_len = 256) {
        char *raw = nullptr;
        if (!VmToPtr(header, ctx, is64, vmaddr, raw) || raw == nullptr) return "";
        const uint64_t base = reinterpret_cast<uint64_t>(ctx->file_start);
        const uint64_t pos = reinterpret_cast<uint64_t>(raw) - base;
        if (pos >= ctx->file_size) return "";
        const uint64_t limit = std::min<uint64_t>(ctx->file_size - pos, max_len);
        uint64_t i = 0;
        for (; i < limit; ++i) {
            const unsigned char c = static_cast<unsigned char>(raw[i]);
            if (c == '\0') break;
            const bool ok = std::isalnum(c) || c == '_' || c == '$' || c == '.' || c == ':' || c == '<' || c == '>' || c == ' ' || c == '?';
            if (!ok) break;
        }
        if (i == 0) return "";
        return std::string(raw, static_cast<std::size_t>(i));
    }

    static std::string DecodeSwiftMangledSymbol(std::string name) {
        if (name.empty()) return "";
        if (!name.empty() && name[0] == '_') name.erase(name.begin());
        if (name.size() < 2 || name[0] != '$') return "";
        if (name[1] != 's' && name[1] != 'S') return "";

        std::vector<std::string> parts;
        std::size_t i = 2;
        while (i < name.size()) {
            if (!std::isdigit(static_cast<unsigned char>(name[i]))) break;
            std::size_t len = 0;
            while (i < name.size() && std::isdigit(static_cast<unsigned char>(name[i]))) {
                len = len * 10 + static_cast<std::size_t>(name[i] - '0');
                ++i;
                if (len > 1024) return "";
            }
            if (len == 0 || i + len > name.size()) break;
            parts.push_back(name.substr(i, len));
            i += len;
            if (parts.size() >= 8) break;
        }
        if (parts.empty()) return "";
        std::string out;
        for (std::size_t idx = 0; idx < parts.size(); ++idx) {
            if (idx > 0) out += ".";
            out += parts[idx];
        }
        return out;
    }

    static const char *SwiftContextKindName(uint32_t kind) {
        switch (kind) {
            case 0: return "module";
            case 1: return "extension";
            case 2: return "anonymous";
            case 3: return "protocol";
            case 4: return "opaque-type";
            case 16: return "class";
            case 17: return "struct";
            case 18: return "enum";
            default: return "unknown";
        }
    }

    static const char *SwiftFieldDescriptorKindName(uint16_t kind) {
        switch (kind) {
            case 0: return "struct";
            case 1: return "class";
            case 2: return "enum";
            case 3: return "multi-payload-enum";
            case 4: return "protocol";
            case 5: return "class-protocol";
            case 6: return "objc-protocol";
            case 7: return "objc-class";
            default: return "unknown";
        }
    }

    static std::string SafeDisplay(const std::string &text) {
        if (text.empty()) return "-";
        return text;
    }

    static std::string DemangleSwiftSymbolCached(const std::string &name) {
        static std::map<std::string, std::string> cache;
        auto hit = cache.find(name);
        if (hit != cache.end()) return hit->second;
        std::string demangled = DemangleSwiftSymbolExternal(name);
        if (demangled.empty()) demangled = DecodeSwiftMangledSymbol(name);
        cache[name] = demangled;
        return demangled;
    }

    static std::string DecodeSwiftLikeText(const std::string &text) {
        if (text.empty()) return "-";
        std::string demangled = DemangleSwiftSymbolCached(text);
        if (!demangled.empty()) return demangled;
        if (text.rfind("_$s", 0) == 0 || text.rfind("$s", 0) == 0 ||
            text.rfind("_$S", 0) == 0 || text.rfind("$S", 0) == 0) {
            return "(mangled) " + text;
        }
        return text;
    }

    static void DecodeContextDescriptor(
            MachHeader *header,
            const NodeContextPtr &ctx,
            bool is64,
            uint64_t descriptor_vmaddr,
            std::string &kind,
            std::string &decoded)
    {
        kind = "swift-context";
        decoded = "unmapped";
        char *descriptor_raw = nullptr;
        if (!VmToPtr(header, ctx, is64, descriptor_vmaddr, descriptor_raw) || descriptor_raw == nullptr) {
            return;
        }
        if (!IsFileRangeValid(ctx, descriptor_raw, sizeof(SwiftContextDescriptorHeader))) {
            decoded = "truncated";
            return;
        }

        const auto *desc = reinterpret_cast<const SwiftContextDescriptorHeader *>(descriptor_raw);
        const uint32_t context_kind = desc->flags & 0x1fU;
        kind = std::string("context/") + SwiftContextKindName(context_kind);

        uint64_t name_vmaddr = 0;
        char *name_raw = nullptr;
        std::string name;
        if (ResolveRelativeTarget(header, ctx, is64, descriptor_vmaddr + 8, desc->name, name_vmaddr, name_raw)) {
            name = TryReadCString(header, ctx, is64, name_vmaddr);
        }

        decoded = fmt::format("flags={} parent_rel={} name={}",
                              AsShortHexString(desc->flags),
                              desc->parent,
                              SafeDisplay(name));
    }

    void ParseSwiftRelativeSection(const TableViewDataPtr &t, const std::string &section_name) {
        const bool is64 = d_->Is64();
        const uint64_t base_addr = is64 ? d_->sect().addr64() : d_->sect().addr();
        char *section_data = GetOffset();
        const uint32_t section_size = GetSize();

        const uint32_t stride = sizeof(int32_t);
        const uint32_t count = section_size / stride;
        for (uint32_t idx = 0; idx < count; ++idx) {
            char *cur = section_data + idx * stride;
            const int32_t rel = *reinterpret_cast<int32_t *>(cur);
            const uint64_t rel_field_vmaddr = base_addr + idx * stride;
            uint64_t target_vmaddr = 0;
            char *target_raw = nullptr;
            ResolveRelativeTarget(d_->header(), d_->ctx(), is64, rel_field_vmaddr, rel, target_vmaddr, target_raw);

            std::string kind = "rel32";
            std::string decoded = "-";
            if (section_name == "__swift5_types" || section_name == "__swift5_protos") {
                if (target_raw != nullptr) {
                    DecodeContextDescriptor(d_->header(), d_->ctx(), is64, target_vmaddr, kind, decoded);
                } else {
                    decoded = "unmapped";
                }
            } else if (section_name == "__swift5_assocty") {
                kind = "assocty/rel32";
                if (target_raw != nullptr) {
                    std::string ctx_kind;
                    DecodeContextDescriptor(d_->header(), d_->ctx(), is64, target_vmaddr, ctx_kind, decoded);
                    if (!ctx_kind.empty()) kind = "assocty->" + ctx_kind;
                } else {
                    decoded = "unmapped";
                }
            } else if (section_name == "__swift5_capture") {
                kind = "capture/rel32";
                if (target_raw != nullptr) {
                    const std::string maybe_cstr = TryReadCString(d_->header(), d_->ctx(), is64, target_vmaddr);
                    if (!maybe_cstr.empty()) {
                        decoded = maybe_cstr;
                    } else {
                        decoded = "capture-record";
                    }
                } else {
                    decoded = "unmapped";
                }
            } else if (section_name == "__swift5_typeref") {
                kind = "typeref/rel32";
                if (target_raw != nullptr) {
                    std::string raw_text = TryReadPrintableAscii(d_->header(), d_->ctx(), is64, target_vmaddr);
                    if (raw_text.empty()) raw_text = TryReadCString(d_->header(), d_->ctx(), is64, target_vmaddr);
                    decoded = DecodeSwiftLikeText(raw_text);
                } else {
                    decoded = "unmapped";
                }
            } else {
                if (target_raw != nullptr) {
                    const std::string maybe_cstr = TryReadCString(d_->header(), d_->ctx(), is64, target_vmaddr);
                    if (!maybe_cstr.empty()) decoded = maybe_cstr;
                }
            }

            t->AddRow(target_raw, 0, {
                    AsString(idx),
                    AsAddress(d_->GetRAW(cur)),
                    kind,
                    AsAddress(target_vmaddr),
                    decoded
            });
        }
    }

    void ParseSwiftReflectionStrings(const TableViewDataPtr &t) {
        char *section_data = GetOffset();
        const uint32_t section_size = GetSize();
        uint32_t cursor = 0;
        uint32_t index = 0;
        while (cursor < section_size) {
            char *cur = section_data + cursor;
            uint32_t len = 0;
            while ((cursor + len) < section_size && cur[len] != '\0') {
                ++len;
            }

            if (len == 0) {
                ++cursor;
                continue;
            }

            bool printable = true;
            for (uint32_t i = 0; i < len; ++i) {
                const auto c = static_cast<unsigned char>(cur[i]);
                if (c < 0x20 || c > 0x7e) {
                    printable = false;
                    break;
                }
            }
            if (!printable) {
                ++cursor;
                continue;
            }

            t->AddRow(cur, len + 1, {
                    AsString(index++),
                    AsAddress(d_->GetRAW(cur)),
                    "reflstr",
                    AsString(len),
                    std::string(cur, len)
            });
            cursor += len + 1;
        }
        if (index == 0) {
            t->AddRow({"-", "-", "reflstr", "-", "no printable reflection strings"});
        }
    }

    void ParseSwiftFieldMetadata(const TableViewDataPtr &t) {
        const bool is64 = d_->Is64();
        const uint64_t base_addr = is64 ? d_->sect().addr64() : d_->sect().addr();
        char *section_data = GetOffset();
        const uint32_t section_size = GetSize();

        uint32_t cursor = 0;
        uint32_t descriptor_index = 0;
        while (cursor + sizeof(SwiftFieldDescriptorHeader) <= section_size) {
            char *desc_raw = section_data + cursor;
            auto *desc = reinterpret_cast<const SwiftFieldDescriptorHeader *>(desc_raw);
            const uint64_t desc_vmaddr = base_addr + cursor;

            if (desc->field_record_size < sizeof(SwiftFieldRecord) || desc->field_record_size > 256 || desc->num_fields > 65535) {
                t->AddRow(desc_raw, sizeof(SwiftFieldDescriptorHeader), {
                        AsString(descriptor_index),
                        AsAddress(d_->GetRAW(desc_raw)),
                        "fieldmd/descriptor?",
                        AsAddress(desc_vmaddr),
                        "invalid field descriptor size/count"
                });
                break;
            }

            std::string type_name = "-";
            std::string super_name = "-";
            uint64_t type_name_vmaddr = 0;
            uint64_t super_vmaddr = 0;
            char *tmp_raw = nullptr;
            if (ResolveRelativeTarget(d_->header(), d_->ctx(), is64, desc_vmaddr, desc->mangled_type_name, type_name_vmaddr, tmp_raw)) {
                const std::string parsed = TryReadCString(d_->header(), d_->ctx(), is64, type_name_vmaddr);
                if (!parsed.empty()) type_name = parsed;
            }
            if (ResolveRelativeTarget(d_->header(), d_->ctx(), is64, desc_vmaddr + 4, desc->superclass, super_vmaddr, tmp_raw)) {
                const std::string parsed = TryReadCString(d_->header(), d_->ctx(), is64, super_vmaddr);
                if (!parsed.empty()) super_name = parsed;
            }

            t->AddRow(desc_raw, sizeof(SwiftFieldDescriptorHeader), {
                    AsString(descriptor_index),
                    AsAddress(d_->GetRAW(desc_raw)),
                    std::string("fieldmd/") + SwiftFieldDescriptorKindName(desc->kind),
                    AsAddress(desc_vmaddr),
                    fmt::format("type={} super={} fields={} record_size={}",
                                type_name, super_name, desc->num_fields, desc->field_record_size)
            });

            uint32_t field_cursor = cursor + sizeof(SwiftFieldDescriptorHeader);
            for (uint32_t i = 0; i < desc->num_fields; ++i) {
                if (field_cursor + desc->field_record_size > section_size) {
                    t->AddRow({"-", "-", "field", "-", "truncated field record table"});
                    cursor = section_size;
                    break;
                }
                char *record_raw = section_data + field_cursor;
                const auto *record = reinterpret_cast<const SwiftFieldRecord *>(record_raw);
                const uint64_t record_vmaddr = base_addr + field_cursor;

                std::string field_name = "-";
                std::string mangled_name = "-";
                uint64_t target_vmaddr = 0;
                if (ResolveRelativeTarget(d_->header(), d_->ctx(), is64, record_vmaddr + 8, record->field_name, target_vmaddr, tmp_raw)) {
                    const std::string parsed = TryReadCString(d_->header(), d_->ctx(), is64, target_vmaddr);
                    if (!parsed.empty()) field_name = parsed;
                }
                if (ResolveRelativeTarget(d_->header(), d_->ctx(), is64, record_vmaddr + 4, record->mangled_type_name, target_vmaddr, tmp_raw)) {
                    const std::string parsed = TryReadCString(d_->header(), d_->ctx(), is64, target_vmaddr);
                    if (!parsed.empty()) mangled_name = parsed;
                }

                t->AddRow(record_raw, static_cast<uint64_t>(desc->field_record_size), {
                        AsString(i),
                        AsAddress(d_->GetRAW(record_raw)),
                        "field",
                        field_name,
                        fmt::format("flags={} type={}", AsShortHexString(record->flags), mangled_name)
                });

                field_cursor += desc->field_record_size;
            }
            if (cursor >= section_size) break;
            if (field_cursor <= cursor) break;
            cursor = field_cursor;
            ++descriptor_index;
        }
    }

    static std::string ShellEscapeSingleQuote(const std::string &s) {
        std::string out = "'";
        for (char c : s) {
            if (c == '\'') out += "'\\''";
            else out += c;
        }
        out += "'";
        return out;
    }

    static std::string DemangleSwiftSymbolExternal(const std::string &name) {
#ifdef __APPLE__
        if (name.empty()) return "";
        const std::string cmd = "xcrun swift-demangle " + ShellEscapeSingleQuote(name) + " 2>/dev/null";
        FILE *fp = popen(cmd.c_str(), "r");
        if (fp == nullptr) return "";
        char buf[4096] = {0};
        std::string output;
        while (fgets(buf, sizeof(buf), fp) != nullptr) {
            output += buf;
        }
        pclose(fp);
        if (output.empty()) return "";
        while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) output.pop_back();
        // expected format: <mangled> ---> <demangled>
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

public:
    void InitViewDatas() override {
        SectionViewChildNode::InitViewDatas();

        auto t = CreateTableView();
        t->SetHeaders({"Index", "Offset", "Kind", "Value", "Decoded"});
        t->SetWidths({80, 140, 220, 260, 420});

        const std::string section_name = d_->sect().section_name();
        if (section_name == "__swift5_reflstr") {
            ParseSwiftReflectionStrings(t);
        } else if (section_name == "__swift5_fieldmd") {
            ParseSwiftFieldMetadata(t);
        } else {
            ParseSwiftRelativeSection(t, section_name);
        }

        auto *symtab = d_->header()->FindLoadCommand<LoadCommand_LC_SYMTAB>({LC_SYMTAB});
        if (symtab == nullptr) return;

        t->AddSeparator();
        t->AddRow({"-", "-", "Swift Symbols", "-", "-"});
        int swift_index = 0;
        for (auto &item : symtab->nlists_ref()) {
            if (item->n_strx() == 0) continue;
            std::string name = symtab->GetStringByStrX(item->n_strx());
            if (name.empty()) continue;
            bool is_swift = false;
            if (name.rfind("_$s", 0) == 0 || name.rfind("$s", 0) == 0 ||
                name.rfind("_$S", 0) == 0 || name.rfind("$S", 0) == 0) {
                is_swift = true;
            }
            if (!is_swift) continue;

            const uint64_t symbol_vmaddr = item->Is64() ? item->n_value64() : item->n_value();
            std::string decoded = DemangleSwiftSymbolCached(name);
            if (decoded.empty()) decoded = "(raw mangled)";
            t->AddRow({AsString(swift_index++), AsAddress(symbol_vmaddr), "symbol", name, decoded});
        }
    }
};

enum class ObjCMetadataKind {
    ClassList,
    CategoryList,
    ProtocolList
};

class SectionViewChildNode_ObjC2Metadata : public SectionViewChildNode{
private:
    ObjCMetadataKind kind_ = ObjCMetadataKind::ClassList;

    struct Resolver {
        MachHeader *header = nullptr;
        NodeContextPtr ctx;
        bool is64 = false;

        bool IsInFile(const void *addr, std::size_t size) const {
            if (addr == nullptr) return false;
            const uint64_t base = reinterpret_cast<uint64_t>(ctx->file_start);
            const uint64_t p = reinterpret_cast<uint64_t>(addr);
            if (p < base) return false;
            const uint64_t rel = p - base;
            return rel <= ctx->file_size && size <= (ctx->file_size - rel);
        }

        bool VmToPtr(uint64_t vmaddr, char *&out, uint64_t *raw = nullptr) const {
            out = nullptr;

            if (is64) {
                for (auto *seg : header->GetSegments64()) {
                    const uint64_t seg_vm = seg->cmd()->vmaddr;
                    const uint64_t seg_vmsz = seg->cmd()->vmsize;
                    if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_vmsz) continue;
                    const uint64_t delta = vmaddr - seg_vm;
                    if (delta >= seg->cmd()->filesize) return false;
                    const uint64_t fileoff = seg->cmd()->fileoff + delta;
                    if (fileoff >= ctx->file_size) return false;
                    out = reinterpret_cast<char *>(ctx->file_start) + fileoff;
                    if (raw != nullptr) *raw = fileoff;
                    return true;
                }
            } else {
                for (auto *seg : header->GetSegments()) {
                    const uint64_t seg_vm = seg->cmd()->vmaddr;
                    const uint64_t seg_vmsz = seg->cmd()->vmsize;
                    if (vmaddr < seg_vm || vmaddr >= seg_vm + seg_vmsz) continue;
                    const uint64_t delta = vmaddr - seg_vm;
                    if (delta >= seg->cmd()->filesize) return false;
                    const uint64_t fileoff = seg->cmd()->fileoff + delta;
                    if (fileoff >= ctx->file_size) return false;
                    out = reinterpret_cast<char *>(ctx->file_start) + fileoff;
                    if (raw != nullptr) *raw = fileoff;
                    return true;
                }
            }
            return false;
        }

        template <typename T>
        bool ReadStruct(uint64_t vmaddr, const T *&value, char *&raw_ptr) const {
            value = nullptr;
            raw_ptr = nullptr;
            if (vmaddr == 0) return false;

            if (!VmToPtr(vmaddr, raw_ptr)) return false;
            if (!IsInFile(raw_ptr, sizeof(T))) return false;
            value = reinterpret_cast<const T *>(raw_ptr);
            return true;
        }

        bool ReadPointer(uint64_t vmaddr, uint64_t &ptr_value, char *&raw_ptr) const {
            ptr_value = 0;
            raw_ptr = nullptr;
            if (vmaddr == 0) return false;
            if (is64) {
                const uint64_t *p = nullptr;
                if (!ReadStruct<uint64_t>(vmaddr, p, raw_ptr)) return false;
                ptr_value = *p;
                return true;
            }
            const uint32_t *p = nullptr;
            if (!ReadStruct<uint32_t>(vmaddr, p, raw_ptr)) return false;
            ptr_value = *p;
            return true;
        }

        std::string ReadCString(uint64_t vmaddr) const {
            if (vmaddr == 0) return "";
            char *raw = nullptr;
            uint64_t fileoff = 0;
            if (!VmToPtr(vmaddr, raw, &fileoff)) return "";
            if (fileoff >= ctx->file_size) return "";
            const auto max_len = std::min<uint64_t>(1024, ctx->file_size - fileoff);
            std::size_t len = 0;
            while (len < max_len && raw[len] != '\0') ++len;
            if (len == max_len) return "";
            return std::string(raw, len);
        }

        static uint64_t NormalizeClassData(uint64_t data, bool is64bit) {
            return is64bit ? (data & ~0x7ULL) : (data & ~0x3ULL);
        }
    };

private:
    static std::string Indent(int depth) {
        return std::string(static_cast<std::size_t>(depth * 2), ' ');
    }

    static std::string SafeName(const std::string &value, const std::string &fallback) {
        return value.empty() ? fallback : value;
    }

    static void AddRowWithVm(
            const TableViewDataPtr &table,
            const Resolver &resolver,
            uint64_t vmaddr,
            std::size_t size,
            const std::string &kind,
            const std::string &name,
            const std::string &detail)
    {
        char *raw = nullptr;
        resolver.VmToPtr(vmaddr, raw);
        table->AddRow(raw, raw != nullptr ? static_cast<uint64_t>(size) : 0, {kind, name, detail, AsAddress(vmaddr)});
    }

    void ParseMethodList(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t list_vmaddr,
            int depth,
            const std::string &method_kind)
    {
        if (list_vmaddr == 0) return;
        if (resolver.is64) {
            const method64_list_t *list = nullptr;
            char *list_raw = nullptr;
            if (!resolver.ReadStruct<method64_list_t>(list_vmaddr, list, list_raw)) {
                AddRowWithVm(table, resolver, list_vmaddr, 0, "MethodList?", Indent(depth) + method_kind, "unmapped");
                return;
            }

            const uint32_t entry_size = list->entsize & ~0x3U;
            if (entry_size < sizeof(method64_t) || list->count > 100000) {
                AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "MethodList?", Indent(depth) + method_kind, "invalid entsize/count");
                return;
            }

            AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "MethodList", Indent(depth) + method_kind,
                         fmt::format("count={} entsize={}", list->count, entry_size));
            char *cur = list_raw + sizeof(*list);
            for (uint32_t i = 0; i < list->count; ++i) {
                if (!resolver.IsInFile(cur, entry_size)) break;
                const auto *m = reinterpret_cast<const method64_t *>(cur);
                const std::string mname = SafeName(resolver.ReadCString(m->name), "<unnamed>");
                const std::string mtype = resolver.ReadCString(m->types);
                std::string detail = mtype;
                if (!detail.empty()) detail += " ";
                detail += fmt::format("imp=0x{}", AsShortHexString(m->imp));
                AddRowWithVm(table, resolver, m->imp, 0, "Method", Indent(depth + 1) + mname, detail);
                cur += entry_size;
            }
            return;
        }

        const method_list_t *list = nullptr;
        char *list_raw = nullptr;
        if (!resolver.ReadStruct<method_list_t>(list_vmaddr, list, list_raw)) {
            AddRowWithVm(table, resolver, list_vmaddr, 0, "MethodList?", Indent(depth) + method_kind, "unmapped");
            return;
        }

        const uint32_t entry_size = list->entsize & ~0x3U;
        if (entry_size < sizeof(method_t) || list->count > 100000) {
            AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "MethodList?", Indent(depth) + method_kind, "invalid entsize/count");
            return;
        }

        AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "MethodList", Indent(depth) + method_kind,
                     fmt::format("count={} entsize={}", list->count, entry_size));
        char *cur = list_raw + sizeof(*list);
        for (uint32_t i = 0; i < list->count; ++i) {
            if (!resolver.IsInFile(cur, entry_size)) break;
            const auto *m = reinterpret_cast<const method_t *>(cur);
            const std::string mname = SafeName(resolver.ReadCString(m->name), "<unnamed>");
            const std::string mtype = resolver.ReadCString(m->types);
            std::string detail = mtype;
            if (!detail.empty()) detail += " ";
            detail += fmt::format("imp=0x{}", AsShortHexString(m->imp));
            AddRowWithVm(table, resolver, m->imp, 0, "Method", Indent(depth + 1) + mname, detail);
            cur += entry_size;
        }
    }

    void ParsePropertyList(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t list_vmaddr,
            int depth)
    {
        if (list_vmaddr == 0) return;
        if (resolver.is64) {
            const objc_property64_list *list = nullptr;
            char *list_raw = nullptr;
            if (!resolver.ReadStruct<objc_property64_list>(list_vmaddr, list, list_raw)) {
                AddRowWithVm(table, resolver, list_vmaddr, 0, "PropertyList?", Indent(depth), "unmapped");
                return;
            }
            const uint32_t entry_size = list->entsize;
            if (entry_size < sizeof(objc_property64) || list->count > 100000) {
                AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "PropertyList?", Indent(depth), "invalid entsize/count");
                return;
            }
            AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "PropertyList", Indent(depth),
                         fmt::format("count={} entsize={}", list->count, entry_size));
            char *cur = list_raw + sizeof(*list);
            for (uint32_t i = 0; i < list->count; ++i) {
                if (!resolver.IsInFile(cur, entry_size)) break;
                const auto *p = reinterpret_cast<const objc_property64 *>(cur);
                const std::string pname = SafeName(resolver.ReadCString(p->name), "<property>");
                const std::string pattr = resolver.ReadCString(p->attributes);
                table->AddRow(cur, static_cast<uint64_t>(entry_size), {"Property", Indent(depth + 1) + pname, pattr, AsAddress(list_vmaddr)});
                cur += entry_size;
            }
            return;
        }

        const objc_property_list *list = nullptr;
        char *list_raw = nullptr;
        if (!resolver.ReadStruct<objc_property_list>(list_vmaddr, list, list_raw)) {
            AddRowWithVm(table, resolver, list_vmaddr, 0, "PropertyList?", Indent(depth), "unmapped");
            return;
        }
        const uint32_t entry_size = list->entsize;
        if (entry_size < sizeof(objc_property) || list->count > 100000) {
            AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "PropertyList?", Indent(depth), "invalid entsize/count");
            return;
        }
        AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "PropertyList", Indent(depth),
                     fmt::format("count={} entsize={}", list->count, entry_size));
        char *cur = list_raw + sizeof(*list);
        for (uint32_t i = 0; i < list->count; ++i) {
            if (!resolver.IsInFile(cur, entry_size)) break;
            const auto *p = reinterpret_cast<const objc_property *>(cur);
            const std::string pname = SafeName(resolver.ReadCString(p->name), "<property>");
            const std::string pattr = resolver.ReadCString(p->attributes);
            table->AddRow(cur, static_cast<uint64_t>(entry_size), {"Property", Indent(depth + 1) + pname, pattr, AsAddress(list_vmaddr)});
            cur += entry_size;
        }
    }

    void ParseIvarList(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t list_vmaddr,
            int depth)
    {
        if (list_vmaddr == 0) return;
        if (resolver.is64) {
            const ivar64_list_t *list = nullptr;
            char *list_raw = nullptr;
            if (!resolver.ReadStruct<ivar64_list_t>(list_vmaddr, list, list_raw)) return;
            const uint32_t entry_size = list->entsize & ~0x3U;
            if (entry_size < sizeof(ivar64_t) || list->count > 100000) return;

            AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "IvarList", Indent(depth),
                         fmt::format("count={} entsize={}", list->count, entry_size));
            char *cur = list_raw + sizeof(*list);
            for (uint32_t i = 0; i < list->count; ++i) {
                if (!resolver.IsInFile(cur, entry_size)) break;
                const auto *iv = reinterpret_cast<const ivar64_t *>(cur);
                const std::string iname = SafeName(resolver.ReadCString(iv->name), "<ivar>");
                const std::string itype = resolver.ReadCString(iv->type);
                uint64_t ivar_offset = 0;
                char *offset_raw = nullptr;
                resolver.ReadPointer(iv->offset, ivar_offset, offset_raw);
                const std::string detail = fmt::format("{} offset={} size={}", itype, ivar_offset, iv->size);
                table->AddRow(cur, static_cast<uint64_t>(entry_size), {"Ivar", Indent(depth + 1) + iname, detail, AsAddress(list_vmaddr)});
                cur += entry_size;
            }
            return;
        }

        const ivar_list_t *list = nullptr;
        char *list_raw = nullptr;
        if (!resolver.ReadStruct<ivar_list_t>(list_vmaddr, list, list_raw)) return;
        const uint32_t entry_size = list->entsize & ~0x3U;
        if (entry_size < sizeof(ivar_t) || list->count > 100000) return;

        AddRowWithVm(table, resolver, list_vmaddr, sizeof(*list), "IvarList", Indent(depth),
                     fmt::format("count={} entsize={}", list->count, entry_size));
        char *cur = list_raw + sizeof(*list);
        for (uint32_t i = 0; i < list->count; ++i) {
            if (!resolver.IsInFile(cur, entry_size)) break;
            const auto *iv = reinterpret_cast<const ivar_t *>(cur);
            const std::string iname = SafeName(resolver.ReadCString(iv->name), "<ivar>");
            const std::string itype = resolver.ReadCString(iv->type);
            uint64_t ivar_offset = 0;
            char *offset_raw = nullptr;
            resolver.ReadPointer(iv->offset, ivar_offset, offset_raw);
            const std::string detail = fmt::format("{} offset={} size={}", itype, ivar_offset, iv->size);
            table->AddRow(cur, static_cast<uint64_t>(entry_size), {"Ivar", Indent(depth + 1) + iname, detail, AsAddress(list_vmaddr)});
            cur += entry_size;
        }
    }

    void ParseProtocol(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t protocol_vmaddr,
            int depth,
            std::unordered_set<uint64_t> &visited)
    {
        if (protocol_vmaddr == 0 || visited.count(protocol_vmaddr) != 0) return;
        visited.insert(protocol_vmaddr);

        if (resolver.is64) {
            const protocol64_t *proto = nullptr;
            char *proto_raw = nullptr;
            if (!resolver.ReadStruct<protocol64_t>(protocol_vmaddr, proto, proto_raw)) {
                AddRowWithVm(table, resolver, protocol_vmaddr, 0, "Protocol?", Indent(depth), "unmapped");
                return;
            }

            const std::string pname = SafeName(resolver.ReadCString(proto->name), "<protocol>");
            table->AddRow(proto_raw, sizeof(*proto), {"Protocol", Indent(depth) + pname, "", AsAddress(protocol_vmaddr)});
            ParseMethodList(resolver, table, proto->instanceMethods, depth + 1, "instance");
            ParseMethodList(resolver, table, proto->classMethods, depth + 1, "class");
            ParseMethodList(resolver, table, proto->optionalInstanceMethods, depth + 1, "optional-instance");
            ParseMethodList(resolver, table, proto->optionalClassMethods, depth + 1, "optional-class");
            ParsePropertyList(resolver, table, proto->instanceProperties, depth + 1);
            ParseProtocolList(resolver, table, proto->protocols, depth + 1, visited);
            return;
        }

        const protocol_t *proto = nullptr;
        char *proto_raw = nullptr;
        if (!resolver.ReadStruct<protocol_t>(protocol_vmaddr, proto, proto_raw)) {
            AddRowWithVm(table, resolver, protocol_vmaddr, 0, "Protocol?", Indent(depth), "unmapped");
            return;
        }

        const std::string pname = SafeName(resolver.ReadCString(proto->name), "<protocol>");
        table->AddRow(proto_raw, sizeof(*proto), {"Protocol", Indent(depth) + pname, "", AsAddress(protocol_vmaddr)});
        ParseMethodList(resolver, table, proto->instanceMethods, depth + 1, "instance");
        ParseMethodList(resolver, table, proto->classMethods, depth + 1, "class");
        ParseMethodList(resolver, table, proto->optionalInstanceMethods, depth + 1, "optional-instance");
        ParseMethodList(resolver, table, proto->optionalClassMethods, depth + 1, "optional-class");
        ParsePropertyList(resolver, table, proto->instanceProperties, depth + 1);
        ParseProtocolList(resolver, table, proto->protocols, depth + 1, visited);
    }

    void ParseProtocolList(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t list_vmaddr,
            int depth,
            std::unordered_set<uint64_t> &visited)
    {
        if (list_vmaddr == 0) return;

        if (resolver.is64) {
            const protocol64_list_t *list = nullptr;
            char *list_raw = nullptr;
            if (!resolver.ReadStruct<protocol64_list_t>(list_vmaddr, list, list_raw)) return;
            if (list->count > 100000) return;

            table->AddRow(list_raw, sizeof(*list), {"ProtocolList", Indent(depth), fmt::format("count={}", list->count), AsAddress(list_vmaddr)});
            char *cur = list_raw + sizeof(*list);
            for (uint64_t i = 0; i < list->count; ++i) {
                if (!resolver.IsInFile(cur, sizeof(uint64_t))) break;
                const uint64_t protocol_ptr = *reinterpret_cast<uint64_t *>(cur);
                table->AddRow(cur, sizeof(uint64_t), {"ProtocolRef", Indent(depth + 1), "", AsAddress(protocol_ptr)});
                ParseProtocol(resolver, table, protocol_ptr, depth + 1, visited);
                cur += sizeof(uint64_t);
            }
            return;
        }

        const protocol_list_t *list = nullptr;
        char *list_raw = nullptr;
        if (!resolver.ReadStruct<protocol_list_t>(list_vmaddr, list, list_raw)) return;
        if (list->count > 100000) return;

        table->AddRow(list_raw, sizeof(*list), {"ProtocolList", Indent(depth), fmt::format("count={}", list->count), AsAddress(list_vmaddr)});
        char *cur = list_raw + sizeof(*list);
        for (uint32_t i = 0; i < list->count; ++i) {
            if (!resolver.IsInFile(cur, sizeof(uint32_t))) break;
            const uint32_t protocol_ptr = *reinterpret_cast<uint32_t *>(cur);
            table->AddRow(cur, sizeof(uint32_t), {"ProtocolRef", Indent(depth + 1), "", AsAddress(protocol_ptr)});
            ParseProtocol(resolver, table, protocol_ptr, depth + 1, visited);
            cur += sizeof(uint32_t);
        }
    }

    void ParseClass(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t class_vmaddr,
            int depth,
            std::unordered_set<uint64_t> &visited_classes,
            std::unordered_set<uint64_t> &visited_protocols)
    {
        if (class_vmaddr == 0 || visited_classes.count(class_vmaddr) != 0) return;
        visited_classes.insert(class_vmaddr);

        if (resolver.is64) {
            const class64_t *cls = nullptr;
            char *cls_raw = nullptr;
            if (!resolver.ReadStruct<class64_t>(class_vmaddr, cls, cls_raw)) {
                AddRowWithVm(table, resolver, class_vmaddr, 0, "Class?", Indent(depth), "unmapped");
                return;
            }
            const uint64_t ro_vmaddr = Resolver::NormalizeClassData(cls->data, true);
            const class64_ro_t *ro = nullptr;
            char *ro_raw = nullptr;
            if (!resolver.ReadStruct<class64_ro_t>(ro_vmaddr, ro, ro_raw)) {
                table->AddRow(cls_raw, sizeof(*cls), {"Class?", Indent(depth), "missing class_ro", AsAddress(class_vmaddr)});
                return;
            }

            const std::string cname = SafeName(resolver.ReadCString(ro->name), "<class>");
            const std::string meta = (ro->flags & RO_META) ? "meta" : "instance";
            table->AddRow(cls_raw, sizeof(*cls), {"Class", Indent(depth) + cname, meta, AsAddress(class_vmaddr)});

            if (cls->superclass != 0) {
                table->AddRow(cls_raw, sizeof(*cls), {"Superclass", Indent(depth + 1), "", AsAddress(cls->superclass)});
            }
            ParseMethodList(resolver, table, ro->baseMethods, depth + 1, "base");
            ParseIvarList(resolver, table, ro->ivars, depth + 1);
            ParsePropertyList(resolver, table, ro->baseProperties, depth + 1);
            ParseProtocolList(resolver, table, ro->baseProtocols, depth + 1, visited_protocols);
            return;
        }

        const class_t *cls = nullptr;
        char *cls_raw = nullptr;
        if (!resolver.ReadStruct<class_t>(class_vmaddr, cls, cls_raw)) {
            AddRowWithVm(table, resolver, class_vmaddr, 0, "Class?", Indent(depth), "unmapped");
            return;
        }
        const uint64_t ro_vmaddr = Resolver::NormalizeClassData(cls->data, false);
        const class_ro_t *ro = nullptr;
        char *ro_raw = nullptr;
        if (!resolver.ReadStruct<class_ro_t>(ro_vmaddr, ro, ro_raw)) {
            table->AddRow(cls_raw, sizeof(*cls), {"Class?", Indent(depth), "missing class_ro", AsAddress(class_vmaddr)});
            return;
        }

        const std::string cname = SafeName(resolver.ReadCString(ro->name), "<class>");
        const std::string meta = (ro->flags & RO_META) ? "meta" : "instance";
        table->AddRow(cls_raw, sizeof(*cls), {"Class", Indent(depth) + cname, meta, AsAddress(class_vmaddr)});

        if (cls->superclass != 0) {
            table->AddRow(cls_raw, sizeof(*cls), {"Superclass", Indent(depth + 1), "", AsAddress(cls->superclass)});
        }
        ParseMethodList(resolver, table, ro->baseMethods, depth + 1, "base");
        ParseIvarList(resolver, table, ro->ivars, depth + 1);
        ParsePropertyList(resolver, table, ro->baseProperties, depth + 1);
        ParseProtocolList(resolver, table, ro->baseProtocols, depth + 1, visited_protocols);
    }

    void ParseCategory(
            const Resolver &resolver,
            const TableViewDataPtr &table,
            uint64_t category_vmaddr,
            int depth,
            std::unordered_set<uint64_t> &visited_classes,
            std::unordered_set<uint64_t> &visited_protocols)
    {
        if (category_vmaddr == 0) return;
        if (resolver.is64) {
            const category64_t *cat = nullptr;
            char *cat_raw = nullptr;
            if (!resolver.ReadStruct<category64_t>(category_vmaddr, cat, cat_raw)) {
                AddRowWithVm(table, resolver, category_vmaddr, 0, "Category?", Indent(depth), "unmapped");
                return;
            }
            const std::string cat_name = SafeName(resolver.ReadCString(cat->name), "<category>");
            table->AddRow(cat_raw, sizeof(*cat), {"Category", Indent(depth) + cat_name, "", AsAddress(category_vmaddr)});
            if (cat->cls != 0) {
                table->AddRow(cat_raw, sizeof(*cat), {"ClassRef", Indent(depth + 1), "", AsAddress(cat->cls)});
                ParseClass(resolver, table, cat->cls, depth + 2, visited_classes, visited_protocols);
            }
            ParseMethodList(resolver, table, cat->instanceMethods, depth + 1, "instance");
            ParseMethodList(resolver, table, cat->classMethods, depth + 1, "class");
            ParsePropertyList(resolver, table, cat->instanceProperties, depth + 1);
            ParseProtocolList(resolver, table, cat->protocols, depth + 1, visited_protocols);
            return;
        }

        const category_t *cat = nullptr;
        char *cat_raw = nullptr;
        if (!resolver.ReadStruct<category_t>(category_vmaddr, cat, cat_raw)) {
            AddRowWithVm(table, resolver, category_vmaddr, 0, "Category?", Indent(depth), "unmapped");
            return;
        }
        const std::string cat_name = SafeName(resolver.ReadCString(cat->name), "<category>");
        table->AddRow(cat_raw, sizeof(*cat), {"Category", Indent(depth) + cat_name, "", AsAddress(category_vmaddr)});
        if (cat->cls != 0) {
            table->AddRow(cat_raw, sizeof(*cat), {"ClassRef", Indent(depth + 1), "", AsAddress(cat->cls)});
            ParseClass(resolver, table, cat->cls, depth + 2, visited_classes, visited_protocols);
        }
        ParseMethodList(resolver, table, cat->instanceMethods, depth + 1, "instance");
        ParseMethodList(resolver, table, cat->classMethods, depth + 1, "class");
        ParsePropertyList(resolver, table, cat->instanceProperties, depth + 1);
        ParseProtocolList(resolver, table, cat->protocols, depth + 1, visited_protocols);
    }

public:
    void set_kind(ObjCMetadataKind kind) { kind_ = kind; }

    void InitViewDatas() override {
        SectionViewChildNode::InitViewDatas();

        auto t = CreateTableView();
        t->SetHeaders({"Kind", "Name", "Details", "VM Address"});
        t->SetWidths({160, 360, 460, 180});

        Resolver resolver;
        resolver.header = d_->header();
        resolver.ctx = d_->ctx();
        resolver.is64 = d_->Is64();

        std::unordered_set<uint64_t> visited_classes;
        std::unordered_set<uint64_t> visited_protocols;

        int index = 0;
        d_->ForEachAs_ObjC2Pointer([&](void *ptr_addr) {
            uint64_t target = 0;
            if (d_->Is64()) {
                target = *reinterpret_cast<uint64_t *>(ptr_addr);
            } else {
                target = *reinterpret_cast<uint32_t *>(ptr_addr);
            }

            t->AddRow(ptr_addr,
                      static_cast<uint64_t>(d_->Is64() ? sizeof(uint64_t) : sizeof(uint32_t)),
                      {"Entry", fmt::format("#{}", index++), "", AsAddress(target)});

            if (target == 0) return;

            switch (kind_) {
                case ObjCMetadataKind::ClassList:
                    ParseClass(resolver, t, target, 1, visited_classes, visited_protocols);
                    break;
                case ObjCMetadataKind::CategoryList:
                    ParseCategory(resolver, t, target, 1, visited_classes, visited_protocols);
                    break;
                case ObjCMetadataKind::ProtocolList:
                    ParseProtocol(resolver, t, target, 1, visited_protocols);
                    break;
            }
        });
    }
};

class SectionViewChildNode_Disassembly : public SectionViewChildNode{
private:
    static bool ResolveCapstoneMode(MachHeader *header, uint32_t &arch, uint32_t &mode)
    {
#if defined(MOEX_HAS_CAPSTONE) && MOEX_HAS_CAPSTONE
        switch (header->data_ptr()->cputype) {
            case CPU_TYPE_I386:
                arch = CS_ARCH_X86;
                mode = CS_MODE_32;
                return true;
            case CPU_TYPE_X86_64:
                arch = CS_ARCH_X86;
                mode = CS_MODE_64;
                return true;
            case CPU_TYPE_ARM:
                arch = CS_ARCH_ARM;
                mode = CS_MODE_ARM;
                return true;
            case CPU_TYPE_ARM64:
                arch = CS_ARCH_ARM64;
                mode = CS_MODE_ARM;
                return true;
            default:
                return false;
        }
#else
        (void)header;
        (void)arch;
        (void)mode;
        return false;
#endif
    }

    static std::string LookupSymbolForAddress(
            uint64_t addr,
            const std::unordered_map<uint64_t, std::string> &exact,
            const std::vector<std::pair<uint64_t, std::string>> &sorted)
    {
        auto hit = exact.find(addr);
        if (hit != exact.end()) {
            return hit->second;
        }
        if (sorted.empty()) {
            return "";
        }

        auto up = std::upper_bound(
                sorted.begin(),
                sorted.end(),
                addr,
                [](uint64_t value, const std::pair<uint64_t, std::string> &item) {
                    return value < item.first;
                });
        if (up == sorted.begin()) {
            return "";
        }

        const auto &prev = *(up - 1);
        const uint64_t delta = addr - prev.first;
        if (delta > 0x100000) {
            return "";
        }
        return fmt::format("{}+0x{}", prev.second, AsShortHexString(delta));
    }

public:
    void InitViewDatas() override
    {
        SectionViewChildNode::InitViewDatas();
        auto t = CreateTableView(d_.get());
        t->SetHeaders({"Address", "Bytes", "Instruction", "Symbol", "Notes"});
        t->SetWidths({160, 220, 420, 260, 240});

        auto *symtab = d_->header()->FindLoadCommand<LoadCommand_LC_SYMTAB>({LC_SYMTAB});
        std::unordered_map<uint64_t, std::string> symbols_exact;
        std::vector<std::pair<uint64_t, std::string>> symbols_sorted;
        std::unordered_set<uint64_t> function_starts;
        struct DataInCodeRange {
            uint64_t begin = 0;
            uint64_t end = 0;
            std::string kind;
        };
        std::vector<DataInCodeRange> data_in_code_ranges;
        if (symtab != nullptr) {
            for (auto &item : symtab->nlists_ref()) {
                const uint64_t value = item->Is64() ? item->n_value64() : item->n_value();
                if (value == 0 || item->n_strx() == 0) {
                    continue;
                }
                std::string name = symtab->GetStringByStrX(item->n_strx());
                if (name.empty()) {
                    continue;
                }
                symbols_exact[value] = name;
                symbols_sorted.emplace_back(value, name);
            }
            std::sort(symbols_sorted.begin(), symbols_sorted.end(),
                      [](const std::pair<uint64_t, std::string> &a, const std::pair<uint64_t, std::string> &b) {
                          return a.first < b.first;
                      });
        }

        auto *function_starts_cmd = d_->header()->FindLoadCommand<LoadCommand_LC_FUNCTION_STARTS>({LC_FUNCTION_STARTS});
        if (function_starts_cmd != nullptr) {
            uint64_t addr = d_->header()->GetBaseAddress();
            for (const auto &entry : function_starts_cmd->GetFunctions()) {
                if (entry.data == 0) break;
                addr += entry.data;
                function_starts.insert(addr);
            }
        }

        auto *data_in_code_cmd = d_->header()->FindLoadCommand<LoadCommand_LC_DATA_IN_CODE>({LC_DATA_IN_CODE});
        if (data_in_code_cmd != nullptr) {
            for (const auto &dice : data_in_code_cmd->GetDices()) {
                DataInCodeRange range;
                range.begin = dice->offset()->offset;
                range.end = range.begin + dice->offset()->length;
                range.kind = dice->GetKindString();
                data_in_code_ranges.push_back(range);
            }
        }

#if !(defined(MOEX_HAS_CAPSTONE) && MOEX_HAS_CAPSTONE)
        t->AddRow({"-", "-", "Capstone not enabled. Reconfigure with Capstone installed.", "-"});
        return;
#else
        uint32_t raw_arch = 0;
        uint32_t raw_mode = 0;
        if (!ResolveCapstoneMode(d_->header(), raw_arch, raw_mode)) {
            t->AddRow({"-", "-", "Unsupported architecture for Capstone disassembly.", "-"});
            return;
        }

        csh handle;
        const cs_arch arch = static_cast<cs_arch>(raw_arch);
        const cs_mode mode = static_cast<cs_mode>(raw_mode);
        if (cs_open(arch, mode, &handle) != CS_ERR_OK) {
            t->AddRow({"-", "-", "Failed to initialize Capstone.", "-"});
            return;
        }
        cs_option(handle, CS_OPT_DETAIL, CS_OPT_OFF);

        const uint8_t *code = reinterpret_cast<const uint8_t *>(GetOffset());
        const std::size_t code_size = static_cast<std::size_t>(GetSize());
        const uint64_t base_addr = d_->Is64() ? d_->sect().addr64() : d_->sect().addr();

        cs_insn *insns = nullptr;
        const std::size_t count = cs_disasm(handle, code, code_size, base_addr, 0, &insns);
        if (count == 0 || insns == nullptr) {
            t->AddRow({"-", "-", fmt::format("Disassembly failed: {}", cs_strerror(cs_errno(handle))), "-"});
            cs_close(&handle);
            return;
        }

        for (std::size_t i = 0; i < count; ++i) {
            const cs_insn &insn = insns[i];
            std::string op = insn.op_str;
            std::string text = op.empty() ? std::string(insn.mnemonic) : fmt::format("{} {}", insn.mnemonic, op);
            std::string symbol = LookupSymbolForAddress(insn.address, symbols_exact, symbols_sorted);
            char *raw = GetOffset() + (insn.address - base_addr);
            const uint64_t raw_off = d_->header()->GetRAW(raw);
            std::string notes;
            if (function_starts.count(insn.address) != 0) {
                notes = "function_start";
            }
            for (const auto &range : data_in_code_ranges) {
                if (raw_off >= range.begin && raw_off < range.end) {
                    if (!notes.empty()) notes += ", ";
                    notes += "data_in_code:" + range.kind;
                    break;
                }
            }

            t->AddRow(raw,
                      static_cast<uint64_t>(insn.size),
                      {
                          AsAddress(insn.address),
                          AsHexData((void *)insn.bytes, static_cast<std::size_t>(insn.size)),
                          text,
                          symbol,
                          notes
                      });
        }

        cs_free(insns, count);
        cs_close(&handle);
#endif
    }
};
//////////////////////////////////////////////////////////////////////////

SectionViewNodePtr SectionViewNodeFactory::Create(MachSectionPtr d){
    SectionViewNodePtr view = std::make_shared<SectionViewNode>();
    view->Init(d);
    return view;
}

void SectionViewNode::Init(MachSectionPtr d){
    d_ = d;

    InitChildView();
}
std::string SectionViewNode::GetDisplayName(){
    return fmt::format("Section({},{})",d_->sect().segment_name(),d_->sect().section_name());
}

void SectionViewNode::InitViewDatas(){
    // Table
    {
        auto t = CreateTableView(d_.get());
        moex_section &sect = d_->sect();
        t->AddRow((void*)sect.sectname(),sizeof(char)*16,"Section Name",sect.section_name());
        t->AddRow((void*)sect.segname(),sizeof(char)*16,"Segment Name",sect.segment_name());

        if(d_->Is64()) {
            t->AddRow( sect.addr64(), "Address", AsShortHexString(sect.addr64()));
            t->AddRow( sect.size64(), "Size", AsShortHexString(sect.size64()));
        }else{
            t->AddRow( sect.addr(), "Address", AsShortHexString(sect.addr()));
            t->AddRow( sect.size(), "Size", AsShortHexString(sect.size()));
        }

        t->AddRow(sect.offset(),"Offset",AsShortHexString(sect.offset()));
        t->AddRow(sect.align(),"Alignment",AsShortHexString(sect.align()));
        t->AddRow(sect.reloff(),"Relocations Offset",AsShortHexString(sect.reloff()));
        t->AddRow(sect.nreloc(),"Number of Relocations",AsShortHexString(sect.nreloc()));
        t->AddRow(sect.flags(),"Flags",AsShortHexString(sect.flags()));
        t->AddRow(sect.reserved1(),"Reserved1",AsShortHexString(sect.reserved1()));
        t->AddRow(sect.reserved2(),"Reserved2",AsShortHexString(sect.reserved2()));

        if(d_->Is64()){
            t->AddRow(sect.reserved3(),"Reserved3",AsShortHexString(sect.reserved3()));
        }
    }

    // Binary
    {
        auto b = CreateBinaryView();
        b->offset = (char*)d_->offset();
        b->size = d_->DATA_SIZE();
        b->start_value = (uint64_t)b->offset - (uint64_t)d_->ctx()->file_start;
    }
}

void SectionViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & item : children_){
        callback(item.get());
    }
}


void SectionViewNode::InitChildView()
{
    moex_section &sect = d_->sect();

    // section type
    switch(sect.flags() & SECTION_TYPE){
    case S_CSTRING_LITERALS:{ InitCStringView("C String Literals"); break; }
    case S_4BYTE_LITERALS:{ InitLiteralsView("Floating Point Literals",4); break; }
    case S_8BYTE_LITERALS:{ InitLiteralsView("Floating Point Literals",8); break; }
    case S_16BYTE_LITERALS:{ InitLiteralsView("Floating Point Literals",16); break; }
    case S_LITERAL_POINTERS:{ InitPointersView("Literal Pointers"); break; }
    case S_MOD_INIT_FUNC_POINTERS:{ InitPointersView("Module Init Func Pointers"); break; }
    case S_MOD_TERM_FUNC_POINTERS:{ InitPointersView("Module Term Func Pointers"); break; }
    case S_LAZY_SYMBOL_POINTERS:{ InitIndirectPointersView("Lazy Symbol Pointers"); break; }
    case S_NON_LAZY_SYMBOL_POINTERS:{ InitIndirectPointersView("Non-Lazy Symbol Pointers"); break; }
    case S_LAZY_DYLIB_SYMBOL_POINTERS:{ InitIndirectPointersView("Lazy Dylib Symbol Pointers"); break; }
    case S_SYMBOL_STUBS:{ InitIndirectStubsView("Symbol Stubs"); break; }
    default:break;
    }

    // section name
    std::string unique_name = d_->sect().segment_name() + "/" + d_->sect().section_name();
    std::string section_name = d_->sect().section_name();

    bool has_module = true; // objc version detector
    if(unique_name == "__OBJC/__module_info") has_module = false;

    if(unique_name == "__OBJC/__class_ext"){ /* todo */ }
    if(unique_name == "__OBJC/__protocol_ext"){ /* todo */ }
    if(unique_name == "__OBJC/__image_info" || unique_name == "__DATA/__objc_imageinfo") InitObjC2ImageInfo("ObjC2 Image Info");
    if(section_name == "__cfstring") InitCFStringView("ObjC CFStrings");
    if(unique_name == "__TEXT/__text") InitDisassemblyView("Disassembly (Capstone)");
    if(section_name.rfind("__swift5_", 0) == 0) InitSwiftMetadataView("Swift Metadata");

    if(has_module){
        if(unique_name == "__OBJC2/__category_list" || unique_name == "__DATA/__objc_catlist") InitObjC2MetadataView("ObjC2 Category Metadata","category");
        if(unique_name == "__OBJC2/__class_list" || unique_name == "__DATA/__objc_classlist") InitObjC2MetadataView("ObjC2 Class Metadata","class");
        if(unique_name == "__OBJC2/__class_refs" || unique_name == "__DATA/__objc_classrefs") InitObjC2PointerView("ObjC2 Class References");
        if(unique_name == "__OBJC2/__super_refs" || unique_name == "__DATA/__objc_superrefs") InitObjC2PointerView("ObjC2 Super References");
        if(unique_name == "__OBJC2/__protocol_list" || unique_name == "__DATA/__objc_protolist") InitObjC2MetadataView("ObjC2 Protocol Metadata","protocol");
        if(unique_name == "__OBJC2/__message_refs" || unique_name == "__DATA/__objc_msgrefs"){ /* todo */ }
    }
}

void SectionViewNode::InitCStringView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_CString>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitLiteralsView(const std::string &title,size_t unitsize)
{
    auto p = std::make_shared<SectionViewChildNode_Literals>();
    p->Init(d_);
    p->set_name(title);
    p->set_unitsize(unitsize);
    children_.push_back(p);
}

void SectionViewNode::InitPointersView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_Pointers>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitIndirectPointersView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_IndirectPointers>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitIndirectStubsView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_IndirectStubs>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitCFStringView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_CFString>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitSwiftMetadataView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_SwiftMetadata>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitDisassemblyView(const std::string &title)
{
    auto p = std::make_shared<SectionViewChildNode_Disassembly>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitObjC2PointerView(const std::string &title){
    auto p = std::make_shared<SectionViewChildNode_ObjC2Pointer>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}

void SectionViewNode::InitObjC2MetadataView(const std::string &title, const std::string &kind){
    auto p = std::make_shared<SectionViewChildNode_ObjC2Metadata>();
    p->Init(d_);
    p->set_name(title);
    if(kind == "category"){
        p->set_kind(ObjCMetadataKind::CategoryList);
    }else if(kind == "protocol"){
        p->set_kind(ObjCMetadataKind::ProtocolList);
    }else{
        p->set_kind(ObjCMetadataKind::ClassList);
    }
    children_.push_back(p);
}

void SectionViewNode::InitObjC2ImageInfo(const std::string &title){
    auto p = std::make_shared<SectionViewChildNode_ObjC2ImageInfo>();
    p->Init(d_);
    p->set_name(title);
    children_.push_back(p);
}


MOEX_NAMESPACE_END
