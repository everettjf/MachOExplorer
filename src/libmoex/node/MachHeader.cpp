//
// Created by everettjf on 2017/7/10.
//

#include "MachHeader.h"
#include "loadcmd/LoadCommand_SYMTAB.h"
#include "loadcmd/LoadCommand_SEGMENT.h"


MOEX_NAMESPACE_BEGIN

void MachHeaderInternal::Init(void *offset, NodeContextPtr &ctx) {
    NodeData::Init(offset,ctx);
    if(data_.magic == MH_CIGAM) {
        swap_mach_header(&data_, NX_LittleEndian);
    }
}

void MachHeader64Internal::Init(void *offset, NodeContextPtr &ctx) {
    NodeData::Init(offset,ctx);
    if(data_.magic == MH_CIGAM_64){
        swap_mach_header_64(& data_,NX_LittleEndian);
    }
}

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

        // next
        cur_cmd = reinterpret_cast<load_command*>((char*)cur_cmd + cur_cmd->cmdsize);
    }
}

uint64_t MachHeader::GetRAW(const void *addr){
    return (uint64_t)addr - (uint64_t)ctx_->file_start;
}


std::size_t MachHeader::DATA_SIZE(){return is64_?mh64_->DATA_SIZE() : mh_->DATA_SIZE();}

std::string MachHeader::GetArch(){
    return util::GetArchStringFromCpuType(this->data_ptr()->cputype,this->data_ptr()->cpusubtype);
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


uint64_t MachHeader::GetBaseAddress(){
    if(cache_.base_addr > 0)
        return cache_.base_addr;

    ForEachLoadCommand<LoadCommand_LC_SEGMENT>({LC_SEGMENT},[this](LoadCommand_LC_SEGMENT * seg,bool & stop){
        if(seg->cmd()->fileoff == 0 && seg->cmd()->filesize != 0){
            cache_.base_addr = seg->cmd()->vmaddr;
            stop = true;
        }
    });

    ForEachLoadCommand<LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64},[this](LoadCommand_LC_SEGMENT_64 * seg,bool & stop){
        if(seg->cmd()->fileoff == 0 && seg->cmd()->filesize != 0){
            cache_.base_addr = seg->cmd()->vmaddr;
            stop = true;
        }
    });
    return cache_.base_addr;
}
std::vector<LoadCommand_LC_SEGMENT*> & MachHeader::GetSegments(){
    if(cache_.segments.size() > 0)
        return cache_.segments;

    ForEachLoadCommand<LoadCommand_LC_SEGMENT>({LC_SEGMENT},[this](LoadCommand_LC_SEGMENT * seg,bool & stop){
        cache_.segments.push_back(seg);
    });
    return cache_.segments;
}
std::vector<LoadCommand_LC_SEGMENT_64*> & MachHeader::GetSegments64(){
    if(cache_.segments64.size() > 0)
        return cache_.segments64;

    ForEachLoadCommand<LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64},[this](LoadCommand_LC_SEGMENT_64 * seg,bool & stop){
        cache_.segments64.push_back(seg);
    });
    return cache_.segments64;
}

MOEX_NAMESPACE_END
