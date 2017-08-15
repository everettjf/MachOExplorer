//
// Created by everettjf on 2017/7/5.
//

#include "AppHandler.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "util/ArgvParser.h"
#include "util/BeautyTextPrint.h"
#include "impl/CommonDisplay.h"
#include "impl/TestCode.h"

using namespace std;
using namespace boost::filesystem;

int AppHandler::Run(int argc, char* argv[]){
    argv_ = std::make_shared<ArgvParser>();

    SetupOptions();

    if(!argv_->Setup(argc,argv)){
        return 1;
    }

    if(!Prepare()){
        return 1;
    }

    return 0;
}

bool AppHandler::Prepare(){
    if(argv_->Exist("help")){
        argv_->PrintHelp();
        return false;
    }

    if(argv_->Exist("test")){
        TestCode t;
        t.Test();
        return false;
    }

    // Required option file
    if (!argv_->Exist("file")){
        cout << "Input file must be specified."<<endl;
        return false;
    }
    file_path_ = argv_->GetString("file");
    if(!exists(file_path_)){
        cout << "Input file not exist." <<endl;
        return false;
    }

    if(argv_->Exist("edit")){
        // edit mode options
        GoEditMode();
    }else{
        // default to impl mode
        GoDisplayMode();
    }

    return true;
}


void AppHandler::SetupOptions(){
    argv_->desc().add_options()
            ("help", "impl help message")
            ("file",boost::program_options::value<std::string>(),"[required] macho file path")
            ("csv","CSV format output")

            ("test","test code")

            // parameter
            ("arch",boost::program_options::value<std::string>(),"arch filter")

            ("is_fat","is fat")
            ("fat_list","fat list (less info than headerlist)")
            ("header_list","header list")
            ("loadcommand_list","loadcommand list")

            ("segment_list","segment list")
            ("section_list","section list")

            ("symbol_list","symbol list")
            ("string_table","string table")
            ("crypt_info","encryption info")
            ("uuid","uuid")
            ("main","main (entry offset)")

            ("dylib_list","dylib list")
            ("dices","data in code entries")

            ("indirect_symbols","indirect symbols")

            // helper
            ("arch_list","[impl] arch list")
            ("tree","[impl] node tree")

            ;
}

void AppHandler::GoEditMode(){
    cout << "Edit mode [not implemented yet]" <<endl;

}

void AppHandler::GoDisplayMode(){
    CommonDisplay display;
    if(!display.Init(file_path_,argv_->Exist("csv"))){
        return;
    }

    // Param
    if(argv_->Exist("arch")){
        display.set_arch(argv_->GetString("arch"));
    }

    std::vector<std::tuple<std::string,std::function<void()>>> actions = {
    {"is_fat",[&]{display.IsFat();}},
    {"fat_list",[&]{display.FatList();}},
    {"header_list",[&]{display.HeaderList();}},
    {"arch_list",[&]{display.ArchList();}},
    {"loadcommand_list",[&]{display.LoadCommandList();}},
    {"segment_list",[&]{display.SegmentList();}},
    {"section_list",[&]{display.SectionList();}},
    {"symbol_list",[&]{display.SymbolList();}},
    {"string_table",[&]{display.StringTable();}},
    {"crypt_info",[&]{display.CryptInfo();}},
    {"uuid",[&]{display.UUID();}},
    {"dylib_list",[&]{display.DylibList();}},
    {"main",[&]{display.Main();}},
    {"tree",[&]{display.Tree();}},
    {"dices",[&]{display.DataInCodeEntries();}},
    {"indirect_symbols",[&]{display.IndirectSymbols();}},
    };

    for(auto & action : actions){
        std::string param = std::get<0>(action);
        if(argv_->Exist(param.c_str())){
            std::function<void()> func = std::get<1>(action);
            func();
            return;
        }
    }
    cout << "Please specified valid options" <<endl;
}
