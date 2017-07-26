//
// Created by everettjf on 2017/7/26.
//

#include "TestCode.h"
#include <boost/format.hpp>
#include <iostream>

using namespace std;


void TestCode::Test() {
    char a8 = 10;
    cout << sizeof(a8) << " "<< boost::format("%02X")%(uint32_t)a8 <<endl;

    uint16_t a16 = 10;
    cout << sizeof(a16) << " "<< boost::format("%04X")%(uint32_t)a16 <<endl;

    uint32_t a32 = 10;
    cout << sizeof(a32) << " " << boost::format("%08X")%(uint32_t)a32 <<endl;

    uint64_t a64 = 10;
    cout << sizeof(a64) << " " << boost::format("%016X")%(uint64_t)a64 <<endl;

    char c20[20] = "Hello World";
    cout << sizeof(c20) << " ";
    for(int i=0;i<sizeof(c20)/sizeof(char);++i){
        cout << boost::format("%02X")%(uint32_t)c20[i];
    }
    cout << endl;
}
