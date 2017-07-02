#include <iostream>
#include <libmoex/binary.h>
#include "util/CommandParser.h"
#include "util/BeautyTextPrint.h"

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
    if(cp.Exist("help")){
        cp.PrintHelp();
        return 1;
    }

    // Required option file
    if (!cp.Exist("file")){
        cout << "Input file must be specified."<<endl;
        cp.PrintHelp();
        return 1;
    }

    if(cp.Exist("display")){
        // Display mode

        // fatheader list
        if(cp.Exist("fatheader")){


        }


    }

    return 0;
}