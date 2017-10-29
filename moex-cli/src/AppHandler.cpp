//
// Created by everettjf on 2017/7/5.
//

#include "AppHandler.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "util/ArgvParser.h"
#include "util/BeautyTextPrint.h"
#include "impl/TestCode.h"

using namespace std;
using namespace boost::filesystem;

void AppHandler::SetupOptions(){
    argv_->desc().add_options()
            ("help", "Help message")
            ("file",boost::program_options::value<std::string>(),"File path")
            ("csv","CSV format output")
            ("test","Test code")
            ("arch",boost::program_options::value<std::string>(),"Arch")
            ;

    actions_ = {
            {"is_fat",
                    "Is fat file",
                    [&]{display_.IsFat();}
            },
            {"fat_list",
                    "List fat header content",
                    [&]{display_.FatList();}
            },
            {"header_list",
                    "List headers",
                    [&]{display_.HeaderList();}
            },
            {"arch_list",
                    "List arch",
                    [&]{display_.ArchList();}
            },
            {"loadcommand_list",
                    "List load commands",
                    [&]{display_.LoadCommandList();}
            },
            {"segment_list",
                    "List segments",
                    [&]{display_.SegmentList();}
            },
            {"section_list",
                    "List sections",
                    [&]{display_.SectionList();}
            },
            {"symbol_list",
                    "List symbols",
                    [&]{display_.SymbolList();}
            },
            {"string_table",
                    "List string table",
                    [&]{display_.StringTable();}
            },
            {"crypt_info",
                    "Show crypt info",
                    [&]{display_.CryptInfo();}
            },
            {"uuid",
                    "Show file uuid",
                    [&]{display_.UUID();}
            },
            {"dylib_list",
                    "List dynamic libraries",
                    [&]{display_.DylibList();}
            },
            {"main",
                    "Show main entry info",
                    [&]{display_.Main();}
            },
            {"tree",
                    "Show file struct as a tree",
                    [&]{display_.Tree();}
            },
            {"dices",
                    "Show data in code entries",
                    [&]{display_.DataInCodeEntries();}
            },
            {"indirect_symbols",
                    "Show indirect symbols",
                    [&]{display_.IndirectSymbols();}
            },
            {"rebase_opcodes",
                    "Show rebase opcodes",
                    [&]{display_.RebaseOpcodes();}
            },
            {"binding_opcodes",
                    "Show binding opcodes",
                    [&]{display_.BindingInfo();}
            },
            {"weak_binding_opcodes",
                    "Show weak binding opcodes",
                    [&]{display_.WeakBindingInfo();}
            },
            {"lazy_binding_opcodes",
                    "Show lazy binding opcodes",
                    [&]{display_.LazyBindingInfo();}
            },
            {"export_info",
                    "Show export info",
                    [&]{display_.ExportInfo();}
            },
    };

    for(auto & action : actions_){
        std::string name = std::get<0>(action);
        std::string desc = std::get<1>(action);

        boost::shared_ptr<boost::program_options::option_description> opt(
                new boost::program_options::option_description(name.c_str(),
                                                               new boost::program_options::untyped_value(true),
                                                               desc.c_str()));
        argv_->desc().add(opt);
    }
}

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

    GoDisplayMode();

    return true;
}



void AppHandler::GoDisplayMode(){
    if(!display_.Init(file_path_,argv_->Exist("csv"))){
        return;
    }

    if(argv_->Exist("arch")){
        display_.set_arch(argv_->GetString("arch"));
    }

    for(auto & action : actions_){
        std::string param = std::get<0>(action);
        if(argv_->Exist(param.c_str())){
            std::function<void()> func = std::get<2>(action);
            func();
            return;
        }
    }
    cout << "Please specified valid options" <<endl;
}
