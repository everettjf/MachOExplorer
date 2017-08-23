//
// Created by everettjf on 2017/7/24.
//

#include "DynamicLoaderInfoViewNode.h"

MOEX_NAMESPACE_BEGIN


void RebaseInfoViewNode::InitViewDatas()
{
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
    {
        auto x = CreateTableViewDataPtr("Opcodes");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
    {
        auto x = CreateTableViewDataPtr("Actions");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}

void BindingInfoViewNode::InitViewDatas()
{

    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
    {
        auto x = CreateTableViewDataPtr("Opcodes");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
    {
        auto x = CreateTableViewDataPtr("Actions");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}

void WeakBindingInfoViewNode::InitViewDatas()
{

    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
    {
        auto x = CreateTableViewDataPtr("Opcodes");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
    {
        auto x = CreateTableViewDataPtr("Actions");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}

void LazyBindingInfoViewNode::InitViewDatas()
{

    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
    {
        auto x = CreateTableViewDataPtr("Opcodes");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
    {
        auto x = CreateTableViewDataPtr("Actions");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}

void ExportInfoViewNode::InitViewDatas()
{

    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
    {
        auto x = CreateTableViewDataPtr("Opcodes");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
    {
        auto x = CreateTableViewDataPtr("Actions");
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}

void DynamicLoaderInfoViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    callback(rebase_.get());
    callback(binding_.get());
    callback(weak_binding_.get());
    callback(lazy_binding_.get());
    callback(export_.get());
}

void DynamicLoaderInfoViewNode::Init(MachHeaderPtr mh){
    mh_ = mh;

    rebase_ = std::make_shared<RebaseInfoViewNode>();
    binding_ = std::make_shared<BindingInfoViewNode>();
    weak_binding_ = std::make_shared<WeakBindingInfoViewNode>();
    lazy_binding_ = std::make_shared<LazyBindingInfoViewNode>();
    export_ = std::make_shared<ExportInfoViewNode>();
}

void DynamicLoaderInfoViewNode::InitViewDatas()
{
    {
        auto x = CreateTableViewDataPtr();
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}


MOEX_NAMESPACE_END
