//
// Created by everettjf on 2017/7/24.
//

#include "DynamicSymbolTable.h"

MOEX_NAMESPACE_BEGIN


class DynamicSymbolTableChild_IndirectSymbols : public DynamicSymbolTableChild{
public:
    void InitViewDatas(){
        using namespace moex::util;

        moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
        if(!seg)
            return;

        // Indirect Symbols
        if(seg->ExistIndirectSymbols()) {
            // "Indirect Symbols"
            auto x = CreateTableView(mh_.get());
            x->SetHeaders({"Offset","Data","Description","Value"});
            x->SetWidths({100,100,100,400});

            seg->ForEachIndirectSymbols([&](uint32_t * indirect_index){
                x->AddRow({
                                  AsAddress(mh_->GetRAW(indirect_index)),
                                  AsHexString(*indirect_index),
                                  "Indirect Index",
                                  AsShortHexString(*indirect_index)
                          } );
            });
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DynamicSymbolTable::Init(MachHeaderPtr mh){
    mh_ = mh;

    moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    if(!seg)
        return;

    // Indirect Symbols
    if(seg->ExistIndirectSymbols()) {
        auto p = std::make_shared<DynamicSymbolTableChild_IndirectSymbols>();
        p->Init(mh_);
        p->set_name("Indirect Symbols");
        children_.push_back(p);
    }
}


void DynamicSymbolTable::InitViewDatas(){
    using namespace moex::util;

    moex::LoadCommand_LC_DYSYMTAB *seg= mh_->FindLoadCommand<moex::LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    if(!seg)
        return;

    auto bin = seg->GetDataRange();
    if(std::get<0>(bin)){
        auto b = CreateBinaryView();
        b->offset = (char*)mh_->header_start() + std::get<1>(bin);
        b->size = std::get<2>(bin);
        b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
    }

}

void DynamicSymbolTable::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto & item : children_){
        callback(item.get());
    }
}

MOEX_NAMESPACE_END
