//
// Created by everettjf on 2017/7/20.
//
#include "LoadCommand_DYLD_INFO.h"
#include "../MachHeader.h"
#include "LoadCommand_SEGMENT.h"


MOEX_NAMESPACE_BEGIN

std::string RebaseOpcodeContext::GetRebaseTypeString()const{
    return LoadCommand_DYLD_INFO::GetRebaseTypeString(type);
}
std::string RebaseOpcodeContext::GetRebaseTypeShortString()const{
    return LoadCommand_DYLD_INFO::GetRebaseTypeShortString(type);
}

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

    BindingOpcodeContext ctx;
    ctx.address = header()->GetBaseAddress();
    ctx.do_bind_location = (uint64_t)header()->header_start() + cmd()->rebase_off;

    bool done = false;
    char * begin = header()->header_start() + cmd()->rebase_off;
    uint32_t size = cmd()->rebase_size;
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

                if(node_type == NodeTypeLazyBind){
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

                // todo
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_TYPE_IMM:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_ADDEND_SLEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_ADD_ADDR_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_DO_BIND:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:{
                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());

                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:{

                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());
                break;
            }
            case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:{

                auto code = std::make_shared<Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM>();
                callback(&ctx,code.get());
                break;
            }
        }
    }
}

MOEX_NAMESPACE_END

