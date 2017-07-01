#include <iostream>
#include <libmoex/binary.h>
#include "util/CommandParser.h"

using namespace std;

//void test(){
//    moex::BinaryPtr bin_;
//    std::string filepath = "/Applications/SizeOptDemo";
//    try{
//        bin_ = std::make_shared<moex::Binary>(filepath);
//    }catch(std::exception & ex){
//        cout << ex.what() <<endl;
//        return;
//    }
//
//    cout << "succeed" <<endl;
//}


int main(int argc, char* argv[]) {
    CommandParser cp(argc,argv);
    if(cp.exist("help")){
        cp.printHelp();
        return 1;
    }

    // Required option file
    if (!cp.exist("file")){
        cout << "Input file must be specified."<<endl;
        cp.printHelp();
        return 1;
    }


    // FatHeader list




    return 0;
}