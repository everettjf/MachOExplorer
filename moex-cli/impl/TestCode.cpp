//
// Created by everettjf on 2017/7/26.
//

#include "TestCode.h"
#include <boost/format.hpp>
#include <iostream>
#include <libmoex/moex.h>
#include <cstdio>

using namespace std;


void TestCode::Test() {
    uint8_t a8 = 10;
    cout << sizeof(a8) << " "<< boost::format("%02X")%(uint32_t)a8 <<endl;

    uint16_t a16 = 10;
    cout << sizeof(a16) << " "<< boost::format("%04X")%(uint32_t)a16 <<endl;

    uint32_t a32 = 10;
    cout << sizeof(a32) << " " << boost::format("%08X")%(uint32_t)a32 <<endl;

    uint64_t a64 = 10;
    cout << sizeof(a64) << " " << boost::format("%016X")%(uint64_t)a64 <<endl;


    cout << moex::util::AsHexData(&a8)<<endl;
    cout << moex::util::AsHexData(&a16)<<endl;
    cout << moex::util::AsHexData(&a32)<<endl;
    cout << moex::util::AsHexData(&a64)<<endl;

}
