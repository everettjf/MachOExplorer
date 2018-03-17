//
// Created by everettjf on 2017/7/20.
//
#include "LoadCommand_DYLD_INFO.h"
#include "../MachHeader.h"
#include "LoadCommand_SEGMENT.h"
#include <list>


MOEX_NAMESPACE_BEGIN

std::string RebaseOpcodeContext::GetRebaseTypeString()const{
    return LoadCommand_DYLD_INFO::GetRebaseTypeString(type);
}
std::string RebaseOpcodeContext::GetRebaseTypeShortString()const{
    return LoadCommand_DYLD_INFO::GetRebaseTypeShortString(type);
}

////////////////////////////////////////////////////////////////////////

std::string LoadCommand_DYLD_INFO::GetRebaseTypeString(uint8_t type){
    switch(type){
        case REBASE_TYPE_POINTER: return "REBASE_TYPE_POINTER";
        case REBASE_TYPE_TEXT_ABSOLUTE32: return "REBASE_TYPE_TEXT_ABSOLUTE32";
        case REBASE_TYPE_TEXT_PCREL32: return "REBASE_TYPE_TEXT_PCREL32";
        default: return "Unknown";
    }
}
std::string LoadCommand_DYLD_INFO::GetRebaseTypeShortString(uint8_t type){
    switch(type){
        case REBASE_TYPE_POINTER: return "POINTER";
        case REBASE_TYPE_TEXT_ABSOLUTE32: return "ABSOLUTE32";
        case REBASE_TYPE_TEXT_PCREL32: return "PCREL32";
        default: return "Unknown";
    }
}

////////////////////////////////////////////////////////////////////////

std::string BindingOpcodeContext::GetBindTypeString()const{
    return LoadCommand_DYLD_INFO::GetBindTypeString(type);
}
std::string BindingOpcodeContext::GetBindTypeShortString()const{
    return LoadCommand_DYLD_INFO::GetBindTypeShortString(type);
}



////////////////////////////////////////////////////////////////////////


std::vector<std::tuple<std::string,std::string>> ExportItem::GetFlags(){
    std::vector<std::tuple<std::string,std::string>> res;
    if ((flags & EXPORT_SYMBOL_FLAGS_KIND_MASK) == EXPORT_SYMBOL_FLAGS_KIND_REGULAR)
        res.push_back(std::make_tuple("00","EXPORT_SYMBOL_FLAGS_KIND_REGULAR"));

    if ((flags & EXPORT_SYMBOL_FLAGS_KIND_MASK) == EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL)
        res.push_back(std::make_tuple("01","EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL"));

    if (flags & EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION)
        res.push_back(std::make_tuple("04","EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION"));

    if (flags & EXPORT_SYMBOL_FLAGS_REEXPORT)
        res.push_back(std::make_tuple("08","EXPORT_SYMBOL_FLAGS_REEXPORT"));
    if (flags & EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER)
        res.push_back(std::make_tuple("10","EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER"));

    return res;
}


////////////////////////////////////////////////////////////////////////


std::string LoadCommand_DYLD_INFO::GetBindTypeString(uint8_t type){
    switch(type){
        case BIND_TYPE_POINTER: return "BIND_TYPE_POINTER";
        case BIND_TYPE_TEXT_ABSOLUTE32: return "BIND_TYPE_TEXT_ABSOLUTE32";
        case BIND_TYPE_TEXT_PCREL32: return "BIND_TYPE_TEXT_PCREL32";
        default: return "Unknown";
    }
}
std::string LoadCommand_DYLD_INFO::GetBindTypeShortString(uint8_t type){
    switch(type){
        case BIND_TYPE_POINTER: return "POINTER";
        case BIND_TYPE_TEXT_ABSOLUTE32: return "ABSOLUTE32";
        case BIND_TYPE_TEXT_PCREL32: return "PCREL32";
        default: return "Unknown";
    }
}
void LoadCommand_DYLD_INFO::ForEachRebaseOpcode(std::function<void(const RebaseOpcodeContext *ctx,RebaseOpcodeItem*item)> callback){
    uint64_t pointer_size = header()->Is64() ? sizeof(uint64_t) : sizeof(uint32_t);

    RebaseOpcodeContext ctx;
    ctx.address = header()->GetBaseAddress();
    ctx.do_rebase_location = (uint64_t)header()->header_start() + cmd()->rebase_off;

    bool done = false;
    char * begin = header()->header_start() + cmd()->rebase_off;
    uint32_t size = cmd()->rebase_size;
    char * cur = begin;
    while(cur < begin + size && !done){
        // read and move next
        ctx.pbyte = (uint8_t*)cur;
        cur += sizeof(uint8_t);

        // just read
        ctx.byte = *ctx.pbyte;
        ctx.opcode = ctx.byte & REBASE_OPCODE_MASK;
        ctx.immediate = ctx.byte & REBASE_IMMEDIATE_MASK;
        ctx.type = 0;

        switch(ctx.opcode){
            case REBASE_OPCODE_DONE:{
                // pbyte / byte / opcode / done
                done = true;
                auto code = std::make_shared<Wrap_REBASE_OPCODE_DONE>();
                callback(&ctx,code.get());
                break;
            }
            case REBASE_OPCODE_SET_TYPE_IMM:{
                // pbyte / byte / immediate
                ctx.type = ctx.immediate;

                auto code = std::make_shared<Wrap_REBASE_OPCODE_SET_TYPE_IMM>();
                callback(&ctx,code.get());
                break;
            }
            case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB>();
                code->segment_index = ctx.immediate;

                code->offset_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->offset,code->offset_size);
                cur+=code->offset_size;

                if(header()->Is64()){
                    assert(code->segment_index < header()->GetSegments64().size());
                    ctx.address = header()->GetSegments64().at(code->segment_index)->cmd()->vmaddr;
                }else{
                    assert(code->segment_index < header()->GetSegments().size());
                    ctx.address = header()->GetSegments().at(code->segment_index)->cmd()->vmaddr + code->offset;
                }

                callback(&ctx,code.get());
                break;
            }
            case REBASE_OPCODE_ADD_ADDR_ULEB:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_ADD_ADDR_ULEB>();

                code->offset_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->offset,code->offset_size);
                cur+=code->offset_size;

                callback(&ctx,code.get());

                ctx.address += code->offset;
                break;
            }
            case REBASE_OPCODE_ADD_ADDR_IMM_SCALED:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_ADD_ADDR_IMM_SCALED>();
                code->scale = ctx.immediate;
                callback(&ctx,code.get());

                ctx.address += code->scale * pointer_size;
                break;
            }
            case REBASE_OPCODE_DO_REBASE_IMM_TIMES:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_DO_REBASE_IMM_TIMES>();
                code->count = ctx.immediate;

                callback(&ctx,code.get());

                for(uint32_t index = 0; index < code->count; ++index){
                    ctx.address += pointer_size;
                }
                ctx.do_rebase_location = (uint64_t)cur;
                break;
            }
            case REBASE_OPCODE_DO_REBASE_ULEB_TIMES:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES>();

                uint64_t start_next_rebase = (uint64_t)cur;

                code->count_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->count,code->count_size);
                cur+=code->count_size;

                callback(&ctx,code.get());

                for(uint64_t index = 0; index < code->count; ++index){
                    ctx.address += pointer_size;
                }

                ctx.do_rebase_location = start_next_rebase;

                break;
            }
            case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB>();

                uint64_t start_next_rebase = (uint64_t)cur;

                code->offset_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->offset,code->offset_size);
                cur+=code->offset_size;

                callback(&ctx,code.get());

                ctx.address += pointer_size + code->offset;
                ctx.do_rebase_location = start_next_rebase;

                break;
            }
            case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:{
                auto code = std::make_shared<Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB>();

                uint64_t start_next_rebase = (uint64_t)cur;

                code->count_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->count,code->count_size);
                cur+=code->count_size;

                code->skip_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->skip,code->skip_size);
                cur+=code->skip_size;

                callback(&ctx,code.get());

                for(uint64_t index=0; index < code->count; index++){
                    ctx.address += pointer_size + code->skip;
                }
                ctx.do_rebase_location = start_next_rebase;

                break;
            }
            default:{
                assert(false);
                break;
            }
        }
    }
}
void LoadCommand_DYLD_INFO::ForEachBindingOpcode(BindNodeType node_type,uint32_t bind_off,uint32_t bind_size,std::function<void(const BindingOpcodeContext *ctx,BindingOpcodeItem*item)> callback){
    uint64_t pointer_size = header()->Is64() ? sizeof(uint64_t) : sizeof(uint32_t);

    BindingOpcodeContext ctx;
    ctx.address = header()->GetBaseAddress();
    ctx.do_bind_location = (uint64_t)header()->header_start() + bind_off;

    bool done = false;
    char * begin = header()->header_start() + bind_off;
    uint32_t size = bind_size;
    char * cur = begin;
    while(cur < begin + size && !done) {
        // read and move next
        ctx.pbyte = (uint8_t *) cur;
        cur += sizeof(uint8_t);

        // just read
        ctx.byte = *ctx.pbyte;
        ctx.opcode = ctx.byte & BIND_OPCODE_MASK;
        ctx.immediate = ctx.byte & BIND_IMMEDIATE_MASK;
        ctx.type = 0;

        switch (ctx.opcode) {
            case BIND_OPCODE_DONE: {
                auto code = std::make_shared<Wrap_BIND_OPCODE_DONE>();
                callback(&ctx,code.get());

                if(node_type != NodeTypeLazyBind){
                    done = true;
                }
                ctx.do_bind_location = (uint64_t)cur;
                break;
            }
            case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                code->lib_oridinal = ctx.immediate;
                callback(&ctx,code.get());
                break;
            }
            case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB >();
                code->lib_oridinal_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->lib_oridinal,code->lib_oridinal_size);
                cur+=code->lib_oridinal_size;

                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM>();
                if(ctx.immediate == 0) {
                    code->lib_oridinal = 0;
                }else{
                    int8_t signExtended = ctx.immediate | BIND_OPCODE_MASK; // This sign extends the value

                    code->lib_oridinal = signExtended;
                }
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM>();
                code->symbol_flags = ctx.immediate;

                char * name = (char*)cur;
                int len = strlen(name);
                code->symbol_name = std::string(name);
                code->symbol_name_addr = (uint8_t*)name;
                code->symbol_name_size = len + 1;

                callback(&ctx,code.get());

                cur += len + 1;

                break;
            }
            case BIND_OPCODE_SET_TYPE_IMM:{
                ctx.type = ctx.immediate;

                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_TYPE_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_ADDEND_SLEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_ADDEND_SLEB>();

                code->addend_addr = (uint8_t*)cur;
                moex::util::readSignedLeb128(cur,code->addend,code->addend_size);
                cur+=code->addend_size;

                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB>();
                code->segment_index = ctx.immediate;

                code->offset_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->offset,code->offset_size);
                cur+=code->offset_size;

                if(header()->Is64()){
                    assert(code->segment_index < header()->GetSegments64().size());
                    ctx.address = header()->GetSegments64().at(code->segment_index)->cmd()->vmaddr;
                }else{
                    assert(code->segment_index < header()->GetSegments().size());
                    ctx.address = header()->GetSegments().at(code->segment_index)->cmd()->vmaddr + code->offset;
                }

                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_ADD_ADDR_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_ADD_ADDR_ULEB>();

                code->offset_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->offset,code->offset_size);
                cur+=code->offset_size;

                ctx.address += code->offset;

                callback(&ctx,code.get());
                break;
            }
            case BIND_OPCODE_DO_BIND:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_DO_BIND>();

                ctx.do_bind_location = (uint64_t)cur;
                ctx.address += pointer_size;

                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB>();

                uint64_t start_next_rebase = (uint64_t)cur;

                code->offset_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->offset,code->offset_size);
                cur+=code->offset_size;

                callback(&ctx,code.get());

                ctx.address += pointer_size + code->offset;
                ctx.do_bind_location = start_next_rebase;
                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED>();
                code->scale = ctx.immediate;
                callback(&ctx,code.get());

                ctx.address += code->scale * pointer_size;
                break;
            }
            case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:{

                auto code = std::make_shared<Wrap_BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB>();

                uint64_t start_next_rebase = (uint64_t)cur;

                code->count_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->count,code->count_size);
                cur+=code->count_size;

                code->skip_addr = (uint8_t*)cur;
                moex::util::readUnsignedLeb128(cur,code->skip,code->skip_size);
                cur+=code->skip_size;

                callback(&ctx,code.get());

                for(uint64_t index=0; index < code->count; index++){
                    ctx.address += pointer_size + code->skip;
                }
                ctx.do_bind_location = start_next_rebase;
                break;
            }
        }
    }
}

void LoadCommand_DYLD_INFO::ForEachExportItem(std::function<void(const ExportContext *ctx,ExportItem* item,ExportChildItem* child)> callback){
    ExportContext ctx;
    char * begin = header()->header_start() + cmd()->export_off;
    uint32_t size = cmd()->export_size;

    // dfs a trie tree

    struct Entry{
        char *addr;
        std::string prefix;
    };
    std::list<Entry> queue;
    {
        Entry entry;
        entry.addr = begin;
        entry.prefix = "";
        queue.push_back(entry);
    }
    while(queue.size() > 0){
        Entry entry = queue.front();
        queue.pop_front();
        char * cur = entry.addr;

        ExportItem item;
        item.terminal_size_addr = (uint8_t*)cur;
        item.terminal_size= *(uint8_t*)cur;
        cur += sizeof(uint8_t);

        if(item.terminal_size > 0){
            item.flags_addr = (uint8_t*)cur;
            moex::util::readUnsignedLeb128(cur,item.flags,item.flags_size);
            cur+= item.flags_size;

            item.offset_addr = (uint8_t*)cur;
            moex::util::readUnsignedLeb128(cur,item.offset,item.offset_size);
            cur+= item.offset_size;
        }

        item.child_count_addr = (uint8_t*)cur;
        item.child_count = *(uint8_t*)cur;
        cur+= sizeof(uint8_t);

        // callback
        callback(&ctx,&item,nullptr);

        for(uint8_t idx = 0; idx < item.child_count; ++idx){

            ExportChildItem child;
            char * name = (char*)cur;
            int len = strlen(name);
            child.label = std::string(name);
            child.label_addr = (uint8_t*)name;
            child.label_size = len + 1;
            cur += child.label_size;

            child.skip_addr = (uint8_t*)cur;
            moex::util::readUnsignedLeb128(cur,child.skip,child.skip_size);
            cur+= child.skip_size;

            // callback
            callback(&ctx,nullptr,&child);

            Entry child_entry;
            child_entry.addr = begin + child.skip;
            child_entry.prefix = entry.prefix + child.label;

            queue.push_back(child_entry);
        }
    }
}

MOEX_NAMESPACE_END

