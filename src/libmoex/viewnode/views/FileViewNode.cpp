//
// Created by everettjf on 2017/7/30.
//

#include "FileViewNode.h"

MOEX_NAMESPACE_BEGIN

void FileViewNode::Init(BinaryPtr bin) {
    bin_ = bin;

    if(bin_->IsArchive()){
        archive_ = std::make_shared<ArchiveViewNode>();
        archive_->Init(bin_->archive());
    }else if(bin_->IsFat()){
        fat_ = std::make_shared<FatHeaderViewNode>();
        fat_->Init(bin_->fath());
    }else{
        mh_ = std::make_shared<MachHeaderViewNode>();
        mh_->Init(bin_->mh());
    }
}

void FileViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    if(bin_->IsArchive()){
        callback(archive_.get());
    }else if(bin_->IsFat()){
        callback(fat_.get());
    }else{
        callback(mh_.get());
    }

}

void FileViewNode::InitViewDatas()
{
    auto t = CreateTableView();
    t->SetHeaders({"Information"});
    t->SetWidths({300});
    if(bin_->IsArchive()){
        t->AddRow({"It is an archive file (.a)"});
    }else if(bin_->IsFat()){
        t->AddRow({"It is a fat file"});
    }else{
        t->AddRow({"It is not a fat file"});
    }

//    // Binary
//    {
//        auto b = CreateBinaryViewDataPtr();
//        b->offset = (char*)bin_->memory();
//        b->size = bin_->memorysize();
//        b->start_value = 0;
//        SetViewData(b);
//    }
}
MOEX_NAMESPACE_END
