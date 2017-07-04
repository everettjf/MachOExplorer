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
