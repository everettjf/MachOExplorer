//
// Created by everettjf on 2017/7/24.
//

#include "DynamicLoaderInfoViewNode.h"

MOEX_NAMESPACE_BEGIN

void DyldInfoViewNodeBase::Init(LoadCommand_DYLD_INFO *info)
{
    info_ = info;
}

void RebaseInfoViewNode::InitViewDatas()
{
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
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = (char*)info_->header()->header_start() + info_->cmd()->rebase_off;
        b->size = info_->cmd()->rebase_size;
        b->start_value = (uint64_t)b->offset - (uint64_t)info_->ctx()->file_start;
        AddViewData(b);
    }
}

void BindingInfoViewNode::InitViewDatas()
{
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
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
}

void WeakBindingInfoViewNode::InitViewDatas()
{
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
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
}

void LazyBindingInfoViewNode::InitViewDatas()
{
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
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
    }
}

void ExportInfoViewNode::InitViewDatas()
{
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
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = 0;
        b->size = 0;
        AddViewData(b);
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
    using namespace moex::util;
    mh_ = mh;

    moex::LoadCommand_DYLD_INFO *info= mh_->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({(int)LC_DYLD_INFO,(int)LC_DYLD_INFO_ONLY});
    if(!info)
        return;

    dyld_info_command *cmd = info->cmd();

    if(cmd->rebase_off * cmd->rebase_size > 0){
        rebase_ = std::make_shared<RebaseInfoViewNode>();
        rebase_->Init(info);
    }
    if(cmd->bind_off * cmd->bind_size > 0){
        binding_ = std::make_shared<BindingInfoViewNode>();
        binding_->Init(info);
    }
    if(cmd->weak_bind_off * cmd->weak_bind_size > 0){
        weak_binding_ = std::make_shared<WeakBindingInfoViewNode>();
        weak_binding_->Init(info);
    }
    if(cmd->lazy_bind_off * cmd->lazy_bind_size > 0){
        lazy_binding_ = std::make_shared<LazyBindingInfoViewNode>();
        lazy_binding_->Init(info);
    }
    if(cmd->export_off * cmd->export_size > 0){
        export_ = std::make_shared<ExportInfoViewNode>();
        export_->Init(info);
    }
}

void DynamicLoaderInfoViewNode::InitViewDatas()
{
    moex::LoadCommand_DYLD_INFO *c= mh_->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({(int)LC_DYLD_INFO,(int)LC_DYLD_INFO_ONLY});
    if(!c)
        return;

    using namespace moex::util;
    {
        auto t = CreateTableViewDataPtr();

        t->AddRow(c->GetRAW(&(c->cmd()->rebase_off)),c->cmd()->rebase_off,"Rebase Info Offset",AsShortHexString(c->cmd()->rebase_off));
        t->AddRow(c->GetRAW(&(c->cmd()->rebase_size)),c->cmd()->rebase_size,"Rebase Info Size",AsShortHexString(c->cmd()->rebase_size));

        t->AddRow(c->GetRAW(&(c->cmd()->bind_off)),c->cmd()->bind_off,"Binding Info Offset",AsShortHexString(c->cmd()->bind_off));
        t->AddRow(c->GetRAW(&(c->cmd()->bind_size)),c->cmd()->bind_size,"Binding Info Size",AsShortHexString(c->cmd()->bind_size));

        t->AddRow(c->GetRAW(&(c->cmd()->weak_bind_off)),c->cmd()->weak_bind_off,"Weak Binding Info Offset",AsShortHexString(c->cmd()->weak_bind_off));
        t->AddRow(c->GetRAW(&(c->cmd()->weak_bind_size)),c->cmd()->weak_bind_size,"Weak Binding Info Size",AsShortHexString(c->cmd()->weak_bind_size));

        t->AddRow(c->GetRAW(&(c->cmd()->lazy_bind_off)),c->cmd()->lazy_bind_off,"Lazy Binding Info Offset",AsShortHexString(c->cmd()->lazy_bind_off));
        t->AddRow(c->GetRAW(&(c->cmd()->lazy_bind_size)),c->cmd()->lazy_bind_size,"Lazy Binding Info Size",AsShortHexString(c->cmd()->lazy_bind_size));

        t->AddRow(c->GetRAW(&(c->cmd()->export_off)),c->cmd()->export_off,"Export Info Offset",AsShortHexString(c->cmd()->export_off));
        t->AddRow(c->GetRAW(&(c->cmd()->export_size)),c->cmd()->export_size,"Export Info Size",AsShortHexString(c->cmd()->export_size));
        AddViewData(t);
    }
}


MOEX_NAMESPACE_END
