//
// Created by everettjf on 2017/7/5.
//

#include "CommonDisplay.h"
#include <iostream>

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
    print_->SetWidths({10,10,10,10,10});
    print_->Begin();

    for(auto & arch : bin_->fath()->archs()){
        const fat_arch & f = arch->data();
        print_->AddRow({
                "1",
                "1",
                "1",
                "1",
                "1",
                });
    }

    print_->End();
}

void CommonDisplay::HeaderList(){
    print_->Begin();

    print_->End();
}

