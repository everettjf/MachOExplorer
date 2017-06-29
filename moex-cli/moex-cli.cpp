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

int main(int argc, char* argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("compression,c", po::value<int>(), "set compression level")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("compression")) {
        cout << "Compression level was set to "
             << vm["compression"].as<int>() << ".\n";
    } else {
        cout << "Compression level was not set.\n";
    }

    return 0;
}