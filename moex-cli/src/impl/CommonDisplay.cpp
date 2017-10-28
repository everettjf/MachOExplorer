//
// Created by everettjf on 2017/7/5.
//

#include "CommonDisplay.h"
#include <iostream>
#include "../util/Utility.h"
#include <libmoex/node/LoadCommand.h>
#include <string.h>
#include <libmoex/viewnode/ViewNodeManager.h>
#include <libmoex/node/MachHeader.h>

using namespace std;

bool CommonDisplay::Init(const std::string & filepath,bool is_csv){
    print_ = BeautyTextPrinterFactory::CreatePrinter(is_csv);

    try{
        bin_ = std::make_shared<moex::Binary>(filepath);
    }catch(std::exception & ex){
        cout << "Parse failed : " << ex.what() <<endl;
        return false;
    }

    return true;
}

void CommonDisplay::ForEachHeader(std::function<void(moex::MachHeaderPtr)> callback){
    bin_->ForEachHeader([&](moex::MachHeaderPtr header) {
        std::string arch = moex::util::GetArchStringFromCpuType(header->data_ptr()->cputype);
        if(!arch_.empty() && arch != arch_)
            return;
        callback(header);
    });
}

void CommonDisplay::IsFat(){
    cout << (bin_->IsFat() ? "true" : "false") <<endl;
}
void CommonDisplay::FatList(){
    if(!bin_->IsFat())
        return;

    print_->SetHeaders({"cputype","cpusubtype","offset","size","align"});
    print_->SetWidths({15,14,10,10,10});
    print_->Begin();

    for(auto & arch : bin_->fath()->archs()){
        const fat_arch & f = arch->data();
        print_->AddRow({
                moex::util::GetCpuTypeString(f.cputype),
                moex::util::GetCpuSubTypeString(f.cputype,f.cpusubtype),
                ToString(f.offset),
                ToString(f.size),
                ToString(f.align),
                });
    }

    print_->End();
}

void CommonDisplay::HeaderList(){

    print_->SetHeaders({"magic","cputype","cpusubtype","ncmds","sizeofcmds","flags"});
    print_->SetWidths({10,15,14,10,10,10});
    print_->Begin();

    bin_->ForEachHeader([&](moex::MachHeaderPtr header){
        mach_header *h = header->data_ptr();
        print_->AddRow({
                               ToString(h->magic),
                               moex::util::GetCpuTypeString(h->cputype),
                               moex::util::GetCpuSubTypeString(h->cputype,h->cpusubtype),
                               ToString(h->ncmds),
                               ToString(h->sizeofcmds),
                               ToString(h->flags),
                       });
    });

    print_->End();
}

void CommonDisplay::ArchList(){
    print_->SetHeaders({"arch"});
    print_->SetWidths({10});
    print_->Begin();
    bin_->ForEachHeader([&](moex::MachHeaderPtr header){
        mach_header *h = header->data_ptr();
        print_->AddRow({
                        moex::util::GetArchStringFromCpuType(h->cputype)
                       });

    });
    print_->End();
}

void CommonDisplay::LoadCommandList(){
    ForEachHeader([&](moex::MachHeaderPtr header){
        print_->SetHeaders({header->GetArch() + "/ cmd","cmdsize","cmdtype"});
        print_->SetWidths({20,10,25});
        print_->Begin();

        for(auto cmd : header->loadcmds_ref()){
            print_->AddRow({
                    ToString(cmd->offset()->cmd),
                    ToString(cmd->offset()->cmdsize),
                    cmd->GetLoadCommandTypeString()
                           });
        }

        print_->End();
        cout << endl;
    });

}

void CommonDisplay::SegmentList(){
    ForEachHeader([&](moex::MachHeaderPtr header){
        print_->SetHeaders({header->GetArch() + " / cmd","cmdsize","segname","vmaddr","vmsize",
                            "fileoff","filesize","maxprot","initprot","nsects",
                            "flags","cmdtype"});
        print_->SetWidths({10,10,20,15,15,
                           10,10,10,10,10,
                           10,20});
        print_->Begin();

        for(auto cmd : header->loadcmds_ref()){
            if(cmd->offset()->cmd == LC_SEGMENT) {
                moex::LoadCommand_LC_SEGMENT *seg = static_cast<moex::LoadCommand_LC_SEGMENT*>(cmd.get());
                print_->AddRow({
                        ToString(seg->cmd()->cmd),
                        ToString(seg->cmd()->cmdsize),
                        seg->segment_name(),
                        ToString(seg->cmd()->vmaddr),
                        ToString(seg->cmd()->vmsize),

                        ToString(seg->cmd()->fileoff),
                        ToString(seg->cmd()->filesize),
                        ToString(seg->cmd()->maxprot),
                        ToString(seg->cmd()->initprot),
                        ToString(seg->cmd()->nsects),

                        ToString(seg->cmd()->flags),
                        seg->GetLoadCommandTypeString(),
                               });
            }else if(cmd->offset()->cmd == LC_SEGMENT_64) {
                moex::LoadCommand_LC_SEGMENT_64 *seg = static_cast<moex::LoadCommand_LC_SEGMENT_64*>(cmd.get());
                print_->AddRow({
                       ToString(seg->cmd()->cmd),
                       ToString(seg->cmd()->cmdsize),
                       seg->segment_name(),
                       ToString(seg->cmd()->vmaddr),
                       ToString(seg->cmd()->vmsize),

                       ToString(seg->cmd()->fileoff),
                       ToString(seg->cmd()->filesize),
                       ToString(seg->cmd()->maxprot),
                       ToString(seg->cmd()->initprot),
                       ToString(seg->cmd()->nsects),

                       ToString(seg->cmd()->flags),
                       seg->GetLoadCommandTypeString(),
                           });
            }
        }
        print_->End();
    });

}
void CommonDisplay::SectionList(){
    ForEachHeader([&](moex::MachHeaderPtr header){
        print_->SetHeaders({header->GetArch() + " / section","segment","addr","size","offset",
                            "align","reloff","nreloc","flags"});
        print_->SetWidths({20,15,10,10,10,
                           10,10,10,10});
        print_->Begin();

        for(auto & cmd : header->loadcmds_ref()){
            if(cmd->offset()->cmd == LC_SEGMENT) {
                moex::LoadCommand_LC_SEGMENT *seg = static_cast<moex::LoadCommand_LC_SEGMENT*>(cmd.get());
                for(auto & sect : seg->sections_ref()){
                    print_->AddRow({
                                           sect->sect().section_name(),
                                           sect->sect().segment_name(),
                                           ToString(sect->sect().addr()),
                                           ToString(sect->sect().size()),
                                           ToString(sect->sect().offset()),
                                           ToString(sect->sect().align()),
                                           ToString(sect->sect().reloff()),
                                           ToString(sect->sect().nreloc()),
                                           ToString(sect->sect().flags()),
                                   });
                }
            }else if(cmd->offset()->cmd == LC_SEGMENT_64) {
                moex::LoadCommand_LC_SEGMENT_64 *seg = static_cast<moex::LoadCommand_LC_SEGMENT_64*>(cmd.get());
                for(auto & sect : seg->sections_ref()){
                    print_->AddRow({
                           sect->sect().section_name(),
                           sect->sect().segment_name(),
                           ToString(sect->sect().addr()),
                           ToString(sect->sect().size()),
                           ToString(sect->sect().offset()),
                           ToString(sect->sect().align()),
                           ToString(sect->sect().reloff()),
                           ToString(sect->sect().nreloc()),
                           ToString(sect->sect().flags()),
                                   });
                }
            }
        }
        print_->End();
    });

}
void CommonDisplay::SymbolList(){
    ForEachHeader([&](moex::MachHeaderPtr header){
        print_->SetHeaders({header->GetArch() + " / strx","type","sect","desc","value"});
        print_->SetWidths({50,15,10,10,20});
        print_->Begin();

        for(auto cmd : header->loadcmds_ref()){
            if(cmd->offset()->cmd == LC_SYMTAB) {
                moex::LoadCommand_LC_SYMTAB *seg = static_cast<moex::LoadCommand_LC_SYMTAB*>(cmd.get());
                for(auto & item : seg->nlists_ref()){
                    print_->AddRow({
                        seg->GetStringByStrX(item->n_strx()),
//                        ToString(item->n_type()),
//                        ToString(item->n_sect()),
                        "","",
                        ToString(item->n_desc()),
                        ToString(item->Is64() ? item->n_value64() : item->n_value())
                    });
                }
            }
        }
        print_->End();
    });
}
void CommonDisplay::StringTable(){
    ForEachHeader([&](moex::MachHeaderPtr header){

        for(auto cmd : header->loadcmds_ref()){
            if(cmd->offset()->cmd == LC_SYMTAB) {
                moex::LoadCommand_LC_SYMTAB *seg = static_cast<moex::LoadCommand_LC_SYMTAB*>(cmd.get());

                char * stroffset = (char*)seg->GetStringTableOffsetAddress();
                uint32_t strsize = seg->GetStringTableSize();

                print_->SetHeaders({
                    header->GetArch() , 
                    "string"
                });
                print_->SetWidths({10,100});
                print_->Begin();

                int lineno = 0;
                char *cur = stroffset;
                char *end = stroffset + strsize;

                while(cur < end){
                    if(*cur == 0){
                        ++cur;
                        continue;
                    }
                    std::string name(cur);
                    print_->AddRow({
                        ToString(lineno),
                        name
                    });

                    cur += name.length();
                    ++lineno;
                }

                print_->End();
            }
        }
    });
}

void CommonDisplay::CryptInfo(){

    ForEachHeader([&](moex::MachHeaderPtr header) {
        for (auto cmd : header->loadcmds_ref()) {
            if (cmd->offset()->cmd == LC_ENCRYPTION_INFO
                || cmd->offset()->cmd == LC_ENCRYPTION_INFO_64) {
                moex::LoadCommandEncryptionInfo info(cmd.get(),cmd->offset()->cmd == LC_ENCRYPTION_INFO_64);

                print_->SetHeaders({
                                   header->GetArch() + " / cryptoff",
                                   "cryptsize",
                                   "cryptid"
                                   });
                print_->SetWidths({20,20,20});
                print_->Begin();

                print_->AddRow({
                        ToHexString(info.data()->cryptoff),
                        ToString(info.data()->cryptsize),
                        ToString(info.data()->cryptid)
                });

                print_->End();
            }
        }
    });
}

void CommonDisplay::UUID(){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        for (auto cmd : header->loadcmds_ref()) {
            if (cmd->offset()->cmd == LC_UUID) {
                moex::LoadCommand_LC_UUID *one = static_cast<moex::LoadCommand_LC_UUID*>(cmd.get());

                print_->SetHeaders({
                                   header->GetArch() + " / uuid",
                                   });
                print_->SetWidths({50});
                print_->Begin();

                print_->AddRow({
                        one->GetUUIDString()
                });

                print_->End();
            }
        }
    });
}

void CommonDisplay::DylibList(){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        print_->SetHeaders({
                           header->GetArch() + " / name",
                           "flag",
                           "path"
                           });
        print_->SetWidths({25,8,100});
        print_->Begin();

        for (auto cmd : header->loadcmds_ref()) {
            if (cmd->offset()->cmd == LC_LOAD_DYLIB
                || cmd->offset()->cmd == LC_LOAD_WEAK_DYLIB
                || cmd->offset()->cmd == LC_REEXPORT_DYLIB) {
                moex::LoadCommand_DYLIB *one = static_cast<moex::LoadCommand_DYLIB*>(cmd.get());

                std::string prefix;
                if (cmd->offset()->cmd == LC_LOAD_DYLIB) {
                    prefix = "-";
                }else if (cmd->offset()->cmd == LC_LOAD_WEAK_DYLIB) {
                    prefix = "weak";
                }else if (cmd->offset()->cmd == LC_REEXPORT_DYLIB) {
                    prefix = "reexport";
                }

                print_->AddRow({
                        one->dylib_name(),
                        prefix,
                        one->dylib_path()
                               });
            }
        }
        print_->End();
    });
}
void CommonDisplay::Main(){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        for (auto cmd : header->loadcmds_ref()) {
            if (cmd->offset()->cmd == LC_MAIN) {
                moex::LoadCommand_LC_MAIN *one = static_cast<moex::LoadCommand_LC_MAIN*>(cmd.get());

                print_->SetHeaders({
                                           header->GetArch() + " / entryoffset",
                                           "stacksize"
                                   });
                print_->SetWidths({20,10});
                print_->Begin();

                print_->AddRow({
                        ToHexString(one->entryoff()),
                        ToString(one->stacksize())
                               });

                print_->End();
            }
        }
    });
}

void CommonDisplay::Tree(){
    moex::ViewNodeManager manager;
    manager.Init(bin_);

    moex::ViewNode *rootNode = manager.GetRootNode();

    int level = 0;
    DisplayViewNode(rootNode,level);
}
void CommonDisplay::DisplayViewNode(moex::ViewNode *node,int & level){
    if(node == nullptr)return;

    for(int i = 0; i < level; ++i){
        cout << "|-- ";
    }

    cout << node->GetDisplayName() <<endl;

    ++level;
    node->ForEachChild([&](moex::ViewNode* child){
        DisplayViewNode(child,level);
    });
    --level;
}

void CommonDisplay::DataInCodeEntries(){

    ForEachHeader([&](moex::MachHeaderPtr header) {
        for (auto cmd : header->loadcmds_ref()) {
            if (cmd->offset()->cmd == LC_DATA_IN_CODE) {
                moex::LoadCommand_LC_DATA_IN_CODE *one = static_cast<moex::LoadCommand_LC_DATA_IN_CODE*>(cmd.get());

                print_->SetHeaders({
                                           header->GetArch() + " / location",
                                           "length",
                                           "kind"
                                   });
                print_->SetWidths({20,20,20});
                print_->Begin();

                for(auto & dice : one->GetDices()){
                    print_->AddRow({
                                           ToHexString(dice->offset()->offset),
                                           ToHexString(dice->offset()->length),
                                           dice->GetKindString()
                                   });
                }
                print_->End();
            }
        }
    });
}
void CommonDisplay::IndirectSymbols(){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        moex::LoadCommand_LC_DYSYMTAB *t = header->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    });
}
void CommonDisplay::RebaseOpcodes1(){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        print_->SetHeaders({
                                   header->GetArch() + " / offset",
                                   "data",
                                   "description",
                                   "value"
                           });
        print_->SetWidths({20,20,50,40});
        print_->Begin();

        moex::LoadCommand_DYLD_INFO *info = header->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({LC_DYLD_INFO,(int)LC_DYLD_INFO_ONLY});

        uint64_t address = info->header()->GetBaseAddress();

        uint64_t pointer_size = info->header()->Is64() ? sizeof(uint64_t) : sizeof(uint32_t);

        uint64_t do_rebase_location = (uint64_t)info->header()->header_start() + info->cmd()->rebase_off;

        bool done = false;
        char * begin = info->header()->header_start() + info->cmd()->rebase_off;
        uint32_t size = info->cmd()->rebase_size;
        char * cur = begin;
        while(cur < begin + size && !done){
            // read and move next
            uint8_t *pbyte = (uint8_t*)cur;
            cur += sizeof(uint8_t);

            // just read
            uint8_t byte = *pbyte;
            uint8_t opcode = byte & REBASE_OPCODE_MASK;
            uint8_t immediate = byte & REBASE_IMMEDIATE_MASK;
            uint8_t type = 0;

            switch(opcode){
                case REBASE_OPCODE_DONE:{
                    // pbyte / byte / opcode / done
                    done = true;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_DONE",
                                    ""
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_TYPE_IMM:{
                    // pbyte / byte / immediate
                    type = immediate;
                    std::string rebasetype = moex::LoadCommand_DYLD_INFO::GetRebaseTypeString(type);

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_SET_TYPE_IMM",
                                    (boost::format("type (%1%, %2%)") % (int)type % rebasetype).str()
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                    uint32_t segment_index = immediate;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB",
                                    (boost::format("segment (%1%)") % (int)segment_index).str()
                                   });

                    pbyte = (uint8_t*)cur;
                    byte = *pbyte;

                    uint64_t offset=0;
                    uint32_t size=0;
                    moex::util::readUnsignedLeb128(cur,offset,size);
                    cur+=size;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "uleb128",
                                    (boost::format("offset (%1%)") % moex::util::AsHexString(offset)).str()
                                   });

                    if(info->header()->Is64()){
                        assert(segment_index < info->header()->GetSegments64().size());
                        address = info->header()->GetSegments64().at(segment_index)->cmd()->vmaddr;
                    }else{
                        assert(segment_index < info->header()->GetSegments().size());
                        address = info->header()->GetSegments().at(segment_index)->cmd()->vmaddr + offset;
                    }

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_ULEB:{
                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_ADD_ADDR_ULEB",
                                    ""
                                   });

                    pbyte = (uint8_t*)cur;
                    byte = *pbyte;

                    uint64_t offset=0;
                    uint32_t size=0;
                    moex::util::readUnsignedLeb128(cur,offset,size);
                    cur+=size;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "uleb128",
                                    (boost::format("offset (%1%)") % moex::util::AsHexString(offset)).str()
                                   });

                    address += offset;

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_IMM_SCALED:{
                    uint32_t scale = immediate;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_ADD_ADDR_IMM_SCALED",
                                    (boost::format("scale (%1%)") % (int)scale).str()
                                   });

                    address += scale * pointer_size;
                    break;
                }
                case REBASE_OPCODE_DO_REBASE_IMM_TIMES:{
                    uint32_t count = immediate;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_DO_REBASE_IMM_TIMES",
                                    (boost::format("count (%1%)") % (int)count).str()
                                   });

                    // empty line
                    print_->AddRow({"","","",""});

                    for(uint32_t index = 0; index < count; ++index){
                        // address / type / do_rebase_location
                        {
                            uint64_t location = do_rebase_location;
                            uint64_t addr = address;
                            std::string section = ""; // find section by addr
                            std::string rebasetype = moex::LoadCommand_DYLD_INFO::GetRebaseTypeShortString(type);
                            // todo action node
                            // row=location,"", section+addr+rebasetype,""
                        }

                        address += pointer_size;
                    }

                    do_rebase_location = (uint64_t)cur;
                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ULEB_TIMES:{
                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_DO_REBASE_ULEB_TIMES",
                                    ""
                                   });

                    uint64_t start_next_rebase = (uint64_t)cur;

                    pbyte = (uint8_t*)cur;
                    byte = *pbyte;

                    uint64_t count=0;
                    uint32_t size=0;
                    moex::util::readUnsignedLeb128(cur,count,size);
                    cur+=size;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "uleb128",
                                    (boost::format("count (%1%)") % (int)count).str()
                                   });
                    // empty line
                    print_->AddRow({"","","",""});
                    for(uint64_t index = 0; index < count; ++index){
                        // address / type / do_rebase_location
                        {
                            uint64_t location = do_rebase_location;
                            uint64_t addr = address;
                            std::string section = ""; // find section by addr
                            std::string rebasetype = moex::LoadCommand_DYLD_INFO::GetRebaseTypeShortString(type);
                            // todo action node
                            // row=location,"", section+addr+rebasetype,""
                        }

                        address += pointer_size;
                    }
                    do_rebase_location = start_next_rebase;

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:{

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB",
                                    ""
                                   });

                    uint64_t start_next_rebase = (uint64_t)cur;

                    pbyte = (uint8_t*)cur;
                    byte = *pbyte;

                    uint64_t offset=0;
                    uint32_t size=0;
                    moex::util::readUnsignedLeb128(cur,offset,size);
                    cur+=size;

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "uleb128",
                                    (boost::format("offset (%1%)") % moex::util::AsHexString(offset)).str()
                                   });

                    // empty line
                    print_->AddRow({"","","",""});

                    // address / type / do_rebase_location
                    {
                        uint64_t location = do_rebase_location;
                        uint64_t addr = address;
                        std::string section = ""; // find section by addr
                        std::string rebasetype = moex::LoadCommand_DYLD_INFO::GetRebaseTypeShortString(type);
                        // todo action node
                        // row=location,"", section+addr+rebasetype,""
                    }

                    address += pointer_size + offset;
                    do_rebase_location = start_next_rebase;

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:{

                    print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                    "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB",
                                    ""
                                   });

                    uint64_t start_next_rebase = (uint64_t)cur;

                    pbyte = (uint8_t*)cur;
                    byte = *pbyte;

                    uint64_t count=0;
                    {
                        uint32_t size=0;
                        moex::util::readUnsignedLeb128(cur,count,size);
                        cur+=size;

                        print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                        "uleb128",
                                        (boost::format("count (%1%)") % (int)count).str()
                                       });
                    }

                    pbyte = (uint8_t*)cur;
                    byte = *pbyte;

                    uint64_t skip=0;
                    {
                        uint32_t size=0;
                        moex::util::readUnsignedLeb128(cur,skip,size);
                        cur+=size;

                        print_->AddRow({ToHexString(info->header()->GetRAW(pbyte)),ToHexString((int)byte),
                                        "uleb128",
                                        (boost::format("skip (%1%)") % (int)skip).str()
                                       });
                    }

                    for(uint64_t index=0; index < count; index++){
                        // address / type / do_rebase_location
                        {
                            uint64_t location = do_rebase_location;
                            uint64_t addr = address;
                            std::string section = ""; // find section by addr
                            std::string rebasetype = moex::LoadCommand_DYLD_INFO::GetRebaseTypeShortString(type);
                            // todo action node
                            // row=location,"", section+addr+rebasetype,""
                        }

                        address += pointer_size + skip;
                    }
                    do_rebase_location = start_next_rebase;

                    break;
                }
                default:{
                    break;
                }
            }

        }

        print_->End();
    });
}
void CommonDisplay::RebaseOpcodes(){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        print_->SetHeaders({
                                   header->GetArch() + " / offset",
                                   "data",
                                   "description",
                                   "value"
                           });
        print_->SetWidths({20, 20, 50, 40});
        print_->Begin();

        moex::LoadCommand_DYLD_INFO *info = header->FindLoadCommand<moex::LoadCommand_DYLD_INFO>(
                {LC_DYLD_INFO, (int) LC_DYLD_INFO_ONLY});


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
                                    (boost::format("type (%1%, %2%)") % (int)ctx->type % rebasetype).str()
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB*>(codebase);
                    uint32_t segment_index = code->segment_index;

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB",
                                    (boost::format("segment (%1%)") % (int)segment_index).str()
                                   });

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    (boost::format("offset (%1%)") % moex::util::AsHexString(code->offset)).str()
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
                                    (boost::format("offset (%1%)") % moex::util::AsHexString(code->offset)).str()
                                   });

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_IMM_SCALED:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_IMM_SCALED*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_ADD_ADDR_IMM_SCALED",
                                    (boost::format("scale (%1%)") % (int)code->scale).str()
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_IMM_TIMES:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_IMM_TIMES*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_IMM_TIMES",
                                    (boost::format("count (%1%)") % (int)code->count).str()
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
                                    (boost::format("count (%1%)") % (int)code->count).str()
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
                                    (boost::format("offset (%1%)") % moex::util::AsHexString(code->offset)).str()
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
                                    (boost::format("count (%1%)") % (int)code->count).str()
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->skip_addr,code->skip_size),
                                    "uleb128",
                                    (boost::format("skip (%1%)") % (int)code->skip).str()
                                   });
                    break;
                }
                default:{
                    break;
                }
            }
        });

        print_->End();
    });
}

void CommonDisplay::InternalBindInfo(moex::LoadCommand_DYLD_INFO::BindNodeType node_type){
    ForEachHeader([&](moex::MachHeaderPtr header) {
        print_->SetHeaders({
                                   header->GetArch() + " / offset",
                                   "data",
                                   "description",
                                   "value"
                           });
        print_->SetWidths({20, 20, 50, 40});
        print_->Begin();

        moex::LoadCommand_DYLD_INFO *info = header->FindLoadCommand<moex::LoadCommand_DYLD_INFO>(
                {LC_DYLD_INFO, (int) LC_DYLD_INFO_ONLY});

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
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    code->GetName(),
                                    ""
                                   });
                    break;
                }
                case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:{
                    auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    code->GetName(),
                                    (boost::format("dylib (%1%)") % code->lib_oridinal).str()
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
                                    (boost::format("dylib (%1%)") % code->lib_oridinal).str()
                                   });
                    break;
                }
                case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:{
                    auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    code->GetName(),
                                    (boost::format("dylib (%1%)") % code->lib_oridinal).str()
                                   });

                    break;
                }
                case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:{
                    auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    code->GetName(),
                                    (boost::format("flags (%1%)") % code->symbol_flags).str()
                                   });

                    print_->AddRow({ToHexString(info->header()->GetRAW(code->symbol_name_addr)),moex::util::AsHexData(code->symbol_name_addr,code->symbol_name_size),
                                    "string",
                                    (boost::format("name (%1%)") % code->symbol_name).str()
                                   });

                    break;
                }
                case BIND_OPCODE_SET_TYPE_IMM:{

                    break;
                }
                case BIND_OPCODE_SET_ADDEND_SLEB:
                case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
                case BIND_OPCODE_ADD_ADDR_ULEB:
                case BIND_OPCODE_DO_BIND:
                case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
                case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
                case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
                default:
                    break;
            }
        });

        print_->End();
    });
}

void CommonDisplay::BindingInfo(){
    InternalBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeBind);
}
void CommonDisplay::WeakBindingInfo(){
    InternalBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeWeakBind);
}
void CommonDisplay::LazyBindingInfo(){
    InternalBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeLazyBind);
}

