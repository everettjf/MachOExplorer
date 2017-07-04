//
// Created by everettjf on 2017/7/2.
//

#ifndef MOEX_ARGVPARSER_H
#define MOEX_ARGVPARSER_H

#include <boost/program_options.hpp>
#include <string>
#include <iostream>

class ArgvParser{
private:
    boost::program_options::variables_map vm_;
    boost::program_options::options_description desc_;
public:
    ArgvParser(){
    }

    boost::program_options::options_description & desc(){return desc_;}

    bool Setup(int argc,char *argv[]){
        try {
            boost::program_options::store(parse_command_line(argc, (const char* const *)argv, desc_), vm_);
            boost::program_options::notify(vm_);
        }catch(std::exception & ex){
            std::cout << ex.what()<<std::endl;
            return false;
        }
        return true;
    }

    bool Exist(const char *option){
        return vm_.count(option);
    }

    std::string GetString(const char *option){
        return vm_[option].as<std::string>();
    }

    int GetInteger(const char *option){
        return vm_[option].as<int>();
    }

    void PrintHelp(){
        std::cout << "Usage:"<<std::endl;
        std::cout << desc_ <<std::endl;
    }
};

#endif //MOEX_ARGVPARSER_H
