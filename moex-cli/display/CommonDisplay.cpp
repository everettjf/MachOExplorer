//
// Created by everettjf on 2017/7/5.
//

#include "CommonDisplay.h"
#include <iostream>
#include "../util/Utility.h"
#include <libmoex/node/loadcommand_factory.h>

using namespace std;


bool CommonDisplay::Init(const std::string & filepath,bool is_csv){
    print_ = BeautyTextPrinterFactory::CreatePrinter(is_csv);

    try{
        bin_ = std::make_shared<moex::Binary>(filepath);
    }catch(std::exception & ex){
        cout << "Parse failed : " << ex.what() <<endl;
        return false;
    }

    return true;
}

void CommonDisplay::ForEachHeader(std::function<void(moex::MachHeaderPtr)> callback){
    bin_->ForEachHeader([&](moex::MachHeaderPtr header) {
        std::string arch = moex::hp::GetArchStringFromCpuType(header->data_ptr()->cputype);
        if(!arch_.empty() && arch != arch_)
            return;
        callback(header);
    });
}

void CommonDisplay::IsFat(){
    cout << (bin_->IsFat() ? "true" : "false") <<endl;
}
void CommonDisplay::FatList(){
    if(!bin_->IsFat())
        return;

    print_->SetHeaders({"cputype","cpusubtype","offset","size","align"});
    print_->SetWidths({15,14,10,10,10});
    print_->Begin();

    for(auto & arch : bin_->fath()->archs()){
        const fat_arch & f = arch->data();
        print_->AddRow({
                moex::hp::GetCpuTypeString(f.cputype),
                moex::hp::GetCpuSubTypeString(f.cpusubtype),
                ToString(f.offset),
                ToString(f.size),
                ToString(f.align),
                });
    }

    print_->End();
}

void CommonDisplay::HeaderList(){

    print_->SetHeaders({"magic","cputype","cpusubtype","ncmds","sizeofcmds","flags"});
    print_->SetWidths({10,15,14,10,10,10});
    print_->Begin();

    bin_->ForEachHeader([&](moex::MachHeaderPtr header){
        mach_header *h = header->data_ptr();
        print_->AddRow({
                               ToString(h->magic),
                               moex::hp::GetCpuTypeString(h->cputype),
                               moex::hp::GetCpuSubTypeString(h->cpusubtype),
                               ToString(h->ncmds),
                               ToString(h->sizeofcmds),
                               ToString(h->flags),
                       });
    });

    print_->End();
}

void CommonDisplay::ArchList(){
    print_->SetHeaders({"arch"});
    print_->SetWidths({10});
    print_->Begin();
    bin_->ForEachHeader([&](moex::MachHeaderPtr header){
        mach_header *h = header->data_ptr();
        print_->AddRow({
                        moex::hp::GetArchStringFromCpuType(h->cputype)
                       });

    });
    print_->End();
}

void CommonDisplay::LoadCommandList(){
    ForEachHeader([&](moex::MachHeaderPtr header){
        std::string arch = moex::hp::GetArchStringFromCpuType(header->data_ptr()->cputype);

        print_->SetHeaders({arch + "/ cmd","cmdsize","cmdtype","description"});
        print_->SetWidths({20,10,25,130});
        print_->Begin();

        header->ForEachLoadCommand([&](moex::LoadCommandPtr cmd){
            print_->AddRow({
                    ToString(cmd->offset()->cmd),
                    ToString(cmd->offset()->cmdsize),
                    cmd->GetTypeName(),
                    cmd->GetDescription()
                           });

        });

        print_->End();
        cout << endl;
    });

}

void CommonDisplay::SegmentList(){
    print_->SetHeaders({"cmd","cmdsize","segname","vmaddr","vmsize",
                        "fileoff","filesize","maxprot","initprot","nsects",
                        "flags","cmdtype"});
    print_->SetWidths({10,10,20,15,15,
                       10,10,10,10,10,
                       10,20});
    print_->Begin();
    ForEachHeader([&](moex::MachHeaderPtr header){
        header->ForEachLoadCommand([&](moex::LoadCommandPtr cmd){
            if(cmd->offset()->cmd == LC_SEGMENT) {
                moex::LoadCommand_LC_SEGMENT *seg = static_cast<moex::LoadCommand_LC_SEGMENT*>(cmd.get());
                print_->AddRow({
                        ToString(seg->cmd()->cmd),
                        ToString(seg->cmd()->cmdsize),
                        seg->segment_name(),
                        ToString(seg->cmd()->vmaddr),
                        ToString(seg->cmd()->vmsize),

                        ToString(seg->cmd()->fileoff),
                        ToString(seg->cmd()->filesize),
                        ToString(seg->cmd()->maxprot),
                        ToString(seg->cmd()->initprot),
                        ToString(seg->cmd()->nsects),

                        ToString(seg->cmd()->flags),
                        seg->GetTypeName(),
                               });
            }else if(cmd->offset()->cmd == LC_SEGMENT_64) {
                moex::LoadCommand_LC_SEGMENT_64 *seg = static_cast<moex::LoadCommand_LC_SEGMENT_64*>(cmd.get());
                print_->AddRow({
                       ToString(seg->cmd()->cmd),
                       ToString(seg->cmd()->cmdsize),
                       seg->segment_name(),
                       ToString(seg->cmd()->vmaddr),
                       ToString(seg->cmd()->vmsize),

                       ToString(seg->cmd()->fileoff),
                       ToString(seg->cmd()->filesize),
                       ToString(seg->cmd()->maxprot),
                       ToString(seg->cmd()->initprot),
                       ToString(seg->cmd()->nsects),

                       ToString(seg->cmd()->flags),
                       seg->GetTypeName(),
                           });
            }
        });
    });

    print_->End();
}
void CommonDisplay::SymbolList(){
    print_->SetHeaders({"magic","cputype","cpusubtype","ncmds","sizeofcmds","flags"});
    print_->SetWidths({10,15,14,10,10,10});
    print_->Begin();


    print_->End();
}
