//
// Created by everettjf on 2017/7/2.
//

#ifndef MOEX_COMMANDPARSER_H
#define MOEX_COMMANDPARSER_H

#include <boost/program_options.hpp>
#include <string>
#include <iostream>

class CommandParser{
private:
    boost::program_options::variables_map vm;
    boost::program_options::options_description desc;
public:
    CommandParser(int argc,char *argv[]){
        using namespace boost::program_options;

        desc.add_options()
                ("help,h", "http://everettjf.com")
                ("file,f",value<std::string>(),"[required] MachO file path")
                ;


        store(parse_command_line(argc, (const char* const *)argv, desc), vm);
        notify(vm);
    }

    bool exist(const char *option){
        return vm.count(option);
    }

    std::string getString(const char *option){
        return vm[option].as<std::string>();
    }

    int getInteger(const char *option){
        return vm[option].as<int>();
    }

    void printHelp(){
        std::cout << desc <<std::endl;
    }
};

#endif //MOEX_COMMANDPARSER_H
