//
// Created by everettjf on 2017/7/10.
//

#include "MachHeader.h"
#include "loadcmd/LoadCommand_SYMTAB.h"


MOEX_NAMESPACE_BEGIN

void MachHeader::Init(void *offset,NodeContextPtr&ctx) {
    ctx_ = ctx;
    magic_.Parse(offset);

    if(magic_.Is64()){
        is64_ = true;
        mh64_ = std::make_shared<MachHeader64Internal>();
        mh64_->Init(offset,ctx);
    }else{
        is64_ = false;
        mh_ = std::make_shared<MachHeaderInternal>();
        mh_->Init(offset,ctx);
    }

    Parse(offset,ctx);
}

void MachHeader::Parse(void *offset,NodeContextPtr& ctx) {
    header_start_ = offset;

    int cur_datasize = 0;
    if(is64_){
        header_ = reinterpret_cast<mach_header*>(mh64_->data_ptr());
        cur_datasize = mh64_->DATA_SIZE();
    }else{
        header_ = reinterpret_cast<mach_header*>(mh_->data_ptr());
        cur_datasize = mh_->DATA_SIZE();
    }

    // commands
    const uint32_t cmd_count = header_->ncmds;
    const uint32_t sizeofcmds = header_->sizeofcmds;

    uint32_t index = 0;
    load_command *first_cmd = reinterpret_cast<load_command*>((char*)offset + cur_datasize);
    load_command *cur_cmd = first_cmd;
    for(uint32_t index = 0; index < cmd_count; ++index){
        // current
        LoadCommandPtr cmd = LoadCommandFactory::Create(cur_cmd,ctx,this);
        loadcmds_.push_back(cmd);

        switch(cmd->GetCommand()){
        case LC_FUNCTION_STARTS:{exist_function_starts_ = true;break;}
        case LC_DATA_IN_CODE:{exist_data_in_code_entries_ = true;break;}
        case LC_SYMTAB:{symtab_ = (LoadCommand_LC_SYMTAB*)cmd.get();break;}
        default:break;
        }

        // next
        cur_cmd = reinterpret_cast<load_command*>((char*)cur_cmd + cur_cmd->cmdsize);
    }
}

uint64_t MachHeader::GetRAW(const void *addr){
    return (uint64_t)addr - (uint64_t)ctx_->file_start;
}

uint64_t MachHeader::FileOffsetToRVA(uint64_t fileoff){
    uint64_t memoryoff = fileoff + (uint64_t)header_start_;
    auto seg = segment_info_.upper_bound(memoryoff);
    if(seg == segment_info_.begin()){
        // error
        return 0;
    }
    --seg;
    uint64_t seg_offset = seg->first;
    uint64_t seg_addr = seg->second.first;
    return memoryoff - seg_offset + seg_addr;
}

std::string MachHeader::FindSymbolAtRVA(uint64_t rva){
    // extend external symbols represented in 32bit to 64bit
    if ((int32_t)rva < 0)
    {
        rva |= 0xffffffff00000000LL;
    }
    return boost::str(boost::format("0x%X")%rva);
}

std::string MachHeader::FindSymbolAtFileOffset(uint64_t fileoff)
{
    return FindSymbolAtRVA(FileOffsetToRVA(fileoff));
}

std::string MachHeader::FileOffsetToSymbol(uint64_t fileoff){
    uint64_t rva = FileOffsetToRVA(fileoff);
    return FindSymbolAtRVA(rva);
}

std::size_t MachHeader::DATA_SIZE(){return is64_?mh64_->DATA_SIZE() : mh_->DATA_SIZE();}

std::string MachHeader::GetArch(){
    return util::GetArchStringFromCpuType(this->data_ptr()->cputype);
}
std::string MachHeader::GetFileTypeString(){
    return util::GetMachFileType(this->data_ptr()->filetype);
}
std::vector<std::tuple<uint32_t,std::string>> MachHeader::GetFlagsArray(){
    return util::GetMachFlagsArray(this->data_ptr()->flags);
}

std::string MachHeader::GetMagicString(){
    return util::GetMagicString(this->data_ptr()->magic);
}
std::string MachHeader::GetCpuTypeString()
{
    return util::GetCpuTypeString(this->data_ptr()->cputype);
}

std::string MachHeader::GetCpuSubTypeString()
{
    return util::GetCpuSubTypeString(this->data_ptr()->cputype, this->data_ptr()->cpusubtype);
}

std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> MachHeader::GetCpuSubTypeArray(){
    return util::GetCpuSubTypeArray(this->data_ptr()->cputype, this->data_ptr()->cpusubtype);
}

void MachHeader::AddSegmentInfo(uint32_t fileoff, uint64_t vmaddr, uint64_t vmsize){
    uint64_t memoryoff = fileoff + (uint64_t)header_start_;
    segment_info_[memoryoff] = std::make_pair(vmaddr,vmsize);
}
MOEX_NAMESPACE_END
