#include <iostream>
#include <libmoex/binary.h>

using namespace std;

int main() {
    moex::BinaryPtr bin_;
    std::string filepath = "/Applications/SizeOptDemo";
    try{
        bin_ = std::make_shared<moex::Binary>(filepath);
    }catch(std::exception & ex){
        cout << ex.what() <<endl;
        return 1;
    }

    cout << "succeed" <<endl;


    return 0;
}