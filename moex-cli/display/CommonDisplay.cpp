//
// Created by everettjf on 2017/7/5.
//

#include "CommonDisplay.h"
#include <iostream>
#include "../util/Utility.h"

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
                moex::hp::GetCpuTypeString(f.cputype),
                moex::hp::GetCpuSubTypeString(f.cpusubtype),
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

    std::vector<moex::MachHeaderPtr> headers;

    if(bin_->IsFat()){
        for(auto & arch : bin_->fath()->archs()){
            headers.push_back(arch->mh());
        }
    }else{
        headers.push_back(bin_->mh());
    }

    for(auto & header : headers){
        mach_header *h = header->data_ptr();
        print_->AddRow({
                ToString(h->magic),
                moex::hp::GetCpuTypeString(h->cputype),
                moex::hp::GetCpuSubTypeString(h->cpusubtype),
                ToString(h->ncmds),
                ToString(h->sizeofcmds),
                ToString(h->flags),
           });
    }

    print_->End();
}

void CommonDisplay::SegmentList(){
    print_->SetHeaders({"magic","cputype","cpusubtype","ncmds","sizeofcmds","flags"});
    print_->SetWidths({10,15,14,10,10,10});
    print_->Begin();


    print_->End();
}
