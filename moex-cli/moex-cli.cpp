#include <iostream>
#include <libmoex/binary.h>
#include <boost/program_options.hpp>

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

class CommandParser{
private:
    boost::program_options::variables_map vm;
    boost::program_options::options_description desc;
public:
    CommandParser(int argc,char *argv[]){
        using namespace boost::program_options;

        desc.add_options()
                ("help,h", "http://everettjf.com")
                ("file,f",value<string>(),"MachO file path")
                ;


        store(parse_command_line(argc, (const char* const *)argv, desc), vm);
        notify(vm);
    }

    bool exist(const char *option){
        return vm.count(option);
    }

    string getString(const char *option){
        return vm[option].as<string>();
    }

    int getInteger(const char *option){
        return vm[option].as<int>();
    }

    void printHelp(){
        cout << desc <<endl;
    }
};

int main(int argc, char* argv[]) {

    CommandParser cp(argc,argv);
    if(cp.exist("help")){
        cp.printHelp();
        return 1;
    }


    // Required option file
    if (cp.exist("file")){
        cout << "Input file :"
             << cp.getString("file")
             << endl;
        return 1;
    }

    cout << "default param" <<endl;

    return 0;
}