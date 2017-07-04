//
// Created by everettjf on 2017/7/5.
//

#include "AppHandler.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "util/ArgvParser.h"
#include "util/BeautyTextPrint.h"
#include "display/CommonDisplay.h"

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
        // default to display mode
        GoDisplayMode();
    }

    return true;
}


void AppHandler::SetupOptions(){
    argv_->desc().add_options()
            ("help", "display help message")
            ("file",boost::program_options::value<std::string>(),"[required] macho file path")
            ("csv","CSV format output")

            // display mode [default]
            ("isfat","[display] is fat")
            ("fatlist","[display] fat list (less info than headerlist)")
            ("headerlist","[display] header list")

            // edit mode
            ("edit", "enter edit mode")
            // ...edit mode options
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

    if(argv_->Exist("isfat")){
        display.IsFat();
        return;
    }

    if(argv_->Exist("fatlist")){
        display.FatList();
        return;
    }

    if(argv_->Exist("header")){
        display.HeaderList();
        return;
    }

    // other...



    cout << "Please specified valid options" <<endl;
}
