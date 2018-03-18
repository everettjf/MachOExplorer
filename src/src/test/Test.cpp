//
// Created by everettjf on 18/03/2018.
//

#include "Test.h"
#include "libmoex/moex-view.h"

void Test::go(){

    fmt::print("int: {0:d};  hex: {0:0>8x}; hex: {0:0>16x}; bin: {0:#b}", 42);


    std::string dylib_path_ = "/Uses/dksdls/sds.dylib";
    std::size_t pos = dylib_path_.find_last_of("/");
    if(pos > 0){
        std::string name = dylib_path_.substr(pos+1,dylib_path_.length());
        fmt::print("{}",name);
    }
}
