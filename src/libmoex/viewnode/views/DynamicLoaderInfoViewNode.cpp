//
// Created by everettjf on 2017/7/24.
//

#include "DynamicLoaderInfoViewNode.h"

MOEX_NAMESPACE_BEGIN

void DyldInfoViewNodeBase::Init(LoadCommand_DYLD_INFO *info)
{
    info_ = info;
}
void DyldInfoViewNodeBase::InitBindInfo(moex::LoadCommand_DYLD_INFO::BindNodeType node_type){
    using namespace moex::util;

    // "Opcodes"
    auto print_ = CreateTableView(info_->header());
    moex::LoadCommand_DYLD_INFO *info = info_;

    uint32_t bind_off,bind_size;
    if(node_type == moex::LoadCommand_DYLD_INFO::NodeTypeWeakBind){
        bind_off = info->cmd()->weak_bind_off;
        bind_size = info->cmd()->weak_bind_size;
    }else if(node_type == moex::LoadCommand_DYLD_INFO::NodeTypeLazyBind){
        bind_off = info->cmd()->lazy_bind_off;
        bind_size = info->cmd()->lazy_bind_size;
    }else{
        bind_off = info->cmd()->bind_off;
        bind_size = info->cmd()->bind_size;
    }

    info->ForEachBindingOpcode(node_type,bind_off,bind_size,[&](const moex::BindingOpcodeContext *ctx,moex::BindingOpcodeItem*codebase){
        switch(ctx->opcode){
            case BIND_OPCODE_DONE:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DONE*>(codebase);
                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),
                                ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });
                break;
            }
            case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("dylib ({})", code->lib_oridinal)
                               });
                break;
            }
            case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->lib_oridinal_addr)),moex::util::AsHexData(code->lib_oridinal_addr,code->lib_oridinal_size),
                                "uleb128",
                                fmt::format("dylib ({})", code->lib_oridinal)
                               });
                break;
            }
            case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("dylib ({})", code->lib_oridinal)
                               });

                break;
            }
            case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("flags ({})", code->symbol_flags)
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->symbol_name_addr)),
                                moex::util::AsHexData(code->symbol_name_addr,code->symbol_name_size).substr(0,16),
                                "string",
                                fmt::format("name ({})", code->symbol_name)
                               });

                break;
            }
            case BIND_OPCODE_SET_TYPE_IMM:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_TYPE_IMM*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("type ({})", ctx->GetBindTypeString())
                               });

                break;
            }
            case BIND_OPCODE_SET_ADDEND_SLEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_ADDEND_SLEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->addend_addr)),moex::util::AsHexData(code->addend_addr,code->addend_size),
                                "sleb128",
                                fmt::format("addend ({})", code->addend)
                               });
                break;
            }
            case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("segment ({})",code->segment_index)
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->offset_addr)),moex::util::AsHexData(code->offset_addr,code->offset_size),
                                "uleb128",
                                fmt::format("offset ({})", code->offset)
                               });
                break;
            }
            case BIND_OPCODE_ADD_ADDR_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_ADD_ADDR_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->offset_addr)),moex::util::AsHexData(code->offset_addr,code->offset_size),
                                "uleb128",
                                fmt::format("offset ({})", code->offset)
                               });
                break;
            }
            case BIND_OPCODE_DO_BIND:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });
                print_->AddRow({"-","-","-","-"});

                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                ""
                               });

                print_->AddRow({ToHexString(info->header()->GetRAW(code->offset_addr)),moex::util::AsHexData(code->offset_addr,code->offset_size),
                                "uleb128",
                                fmt::format("offset ({})", code->offset)
                               });
                print_->AddRow({"-","-","-","-"});
                break;
            }
            case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                code->GetName(),
                                fmt::format("scaled ({})",code->scale)
                               });
                print_->AddRow({"-","-","-","-"});
                break;
            }
            case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:{
                auto code = static_cast<moex::Wrap_BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB*>(codebase);

                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB",
                                ""
                               });
                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->count_addr,code->count_size),
                                "uleb128",
                                fmt::format("count ({})", (int)code->count)
                               });
                print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->skip_addr,code->skip_size),
                                "uleb128",
                                fmt::format("skip ({})", (int)code->skip)
                               });
                print_->AddRow({"-","-","-","-"});
                break;
            }
            default:
                break;
        }
    });

}

void RebaseInfoViewNode::InitViewDatas()
{
    using namespace moex::util;
    {
        // "Opcodes"
        auto print_ = CreateTableViewDataPtr();
        moex::LoadCommand_DYLD_INFO *info = info_;

        info->ForEachRebaseOpcode([&](const moex::RebaseOpcodeContext * ctx, moex::RebaseOpcodeItem * codebase){

            switch(ctx->opcode){
                case REBASE_OPCODE_DONE:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DONE*>(codebase);
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DONE",
                                    ""
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_TYPE_IMM:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_SET_TYPE_IMM*>(codebase);
                    std::string rebasetype = ctx->GetRebaseTypeString();
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_SET_TYPE_IMM",
                                    fmt::format("type ({}, {})", (int)ctx->type ,rebasetype)
                                   });
                    break;
                }
                case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB*>(codebase);
                    uint32_t segment_index = code->segment_index;

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB",
                                    fmt::format("segment ({})",(int)segment_index)
                                   });

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    fmt::format("offset ({})", moex::util::AsHexString(code->offset))
                                   });

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_ULEB*>(codebase);
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_ADD_ADDR_ULEB",
                                    ""
                                   });

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    fmt::format("offset ({})",moex::util::AsHexString(code->offset))
                                   });

                    break;
                }
                case REBASE_OPCODE_ADD_ADDR_IMM_SCALED:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_IMM_SCALED*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_ADD_ADDR_IMM_SCALED",
                                    fmt::format("scale ({})", (int)code->scale)
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_IMM_TIMES:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_IMM_TIMES*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_IMM_TIMES",
                                    fmt::format("count ({})", (int)code->count)
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ULEB_TIMES:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES*>(codebase);
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_ULEB_TIMES",
                                    ""
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->count_addr,code->count_size),
                                    "uleb128",
                                    fmt::format("count ({})",(int)code->count)
                                   });
                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_ADD_ADDR_ULEB*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB",
                                    ""
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->offset_addr,code->offset_size),
                                    "uleb128",
                                    fmt::format("offset ({})",moex::util::AsHexString(code->offset))
                                   });

                    break;
                }
                case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:{
                    auto code = static_cast<moex::Wrap_REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB*>(codebase);

                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),ToHexString((int)ctx->byte),
                                    "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB",
                                    ""
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->count_addr,code->count_size),
                                    "uleb128",
                                    fmt::format("count ({})", (int)code->count)
                                   });
                    print_->AddRow({ToHexString(info->header()->GetRAW(ctx->pbyte)),moex::util::AsHexDataPrefix(code->skip_addr,code->skip_size),
                                    "uleb128",
                                    fmt::format("skip ({})",(int)code->skip)
                                   });
                    break;
                }
                default:{
                    break;
                }
            }
        });
        SetViewData(print_);
    }
    {
        auto b = CreateBinaryView();
        b->offset = (char*)info_->header()->header_start() + info_->cmd()->rebase_off;
        b->size = info_->cmd()->rebase_size;
        b->start_value = (uint64_t)b->offset - (uint64_t)info_->ctx()->file_start;
    }
}

void BindingInfoViewNode::InitViewDatas()
{
    {
        InitBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeBind);
    }
    {
        auto b = CreateBinaryView();
    }
}

void WeakBindingInfoViewNode::InitViewDatas()
{
    {
        InitBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeWeakBind);
    }
    {
        auto b = CreateBinaryView();
    }
}

void LazyBindingInfoViewNode::InitViewDatas()
{
    {
        InitBindInfo(moex::LoadCommand_DYLD_INFO::NodeTypeLazyBind);
    }
    {
        auto b = CreateBinaryView();
    }
}

void ExportInfoViewNode::InitViewDatas()
{
    using namespace moex::util;
    {
        // "Opcodes"
        auto print_ = CreateTableView(info_->header());

        moex::LoadCommand_DYLD_INFO *info = info_;

        int item_index = 0;
        info->ForEachExportItem([&](const moex::ExportContext *ctx,moex::ExportItem* item,moex::ExportChildItem* child){
            if(item){
                ++item_index;
                if(item_index > 1)
                    print_->AddRow({"-","-","-","-"});

                print_->AddRow({AsAddress(info->header()->GetRAW(item->terminal_size_addr)),
                                moex::util::AsHexData(item->terminal_size),
                                "Terminal Size",
                                fmt::format("{}",(int)item->terminal_size)
                               });

                if(item->terminal_size > 0){
                    print_->AddRow({AsAddress(info->header()->GetRAW(item->flags_addr)),
                                    moex::util::AsHexData(item->flags_addr,item->flags_size),
                                    "Flags",
                                    ""
                                   });

                    auto flags = item->GetFlags();
                    for(auto & flag : flags){
                        print_->AddRow({"","",std::get<0>(flag),std::get<1>(flag)});
                    }

                    print_->AddRow({AsAddress(info->header()->GetRAW(item->offset_addr)),
                                    moex::util::AsHexData(item->offset_addr,item->offset_size),
                                    "Symbol Offset",
                                    fmt::format("{}",(int)item->offset)
                                   });
                }

                print_->AddRow({AsAddress(info->header()->GetRAW(item->child_count_addr)),
                                moex::util::AsHexData(item->child_count),
                                "Child Count",
                                fmt::format("{}",(int)item->child_count)
                               });
            }
            if(child){
                print_->AddRow({AsAddress(info->header()->GetRAW(child->label_addr)),
                                moex::util::AsHexData(child->label_addr,child->label_size).substr(0,16),
                                "Node Label",
                                fmt::format("{}",child->label)
                               });
                print_->AddRow({AsAddress(info->header()->GetRAW(child->skip_addr)),
                                moex::util::AsHexData(child->skip_addr,child->skip_size),
                                "Next Node",
                                moex::util::AsAddress(child->skip)
                               });
            }

        });
    }
    {
        auto b = CreateBinaryView();
        b->offset = 0;
        b->size = 0;
    }
}

void DynamicLoaderInfoViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    if(rebase_)callback(rebase_.get());
    if(binding_)callback(binding_.get());
    if(weak_binding_)callback(weak_binding_.get());
    if(lazy_binding_)callback(lazy_binding_.get());
    if(export_)callback(export_.get());
}

void DynamicLoaderInfoViewNode::Init(MachHeaderPtr mh){
    using namespace moex::util;
    mh_ = mh;

    moex::LoadCommand_DYLD_INFO *info= mh_->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({LC_DYLD_INFO,LC_DYLD_INFO_ONLY});
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
    moex::LoadCommand_DYLD_INFO *c= mh_->FindLoadCommand<moex::LoadCommand_DYLD_INFO>({LC_DYLD_INFO,LC_DYLD_INFO_ONLY});
    if(!c)
        return;

    using namespace moex::util;
    {
        auto t = CreateTableView(mh_.get());

        t->AddRow(c->cmd()->rebase_off,"Rebase Info Offset",AsShortHexString(c->cmd()->rebase_off));
        t->AddRow(c->cmd()->rebase_size,"Rebase Info Size",AsShortHexString(c->cmd()->rebase_size));

        t->AddRow(c->cmd()->bind_off,"Binding Info Offset",AsShortHexString(c->cmd()->bind_off));
        t->AddRow(c->cmd()->bind_size,"Binding Info Size",AsShortHexString(c->cmd()->bind_size));

        t->AddRow(c->cmd()->weak_bind_off,"Weak Binding Info Offset",AsShortHexString(c->cmd()->weak_bind_off));
        t->AddRow(c->cmd()->weak_bind_size,"Weak Binding Info Size",AsShortHexString(c->cmd()->weak_bind_size));

        t->AddRow(c->cmd()->lazy_bind_off,"Lazy Binding Info Offset",AsShortHexString(c->cmd()->lazy_bind_off));
        t->AddRow(c->cmd()->lazy_bind_size,"Lazy Binding Info Size",AsShortHexString(c->cmd()->lazy_bind_size));

        t->AddRow(c->cmd()->export_off,"Export Info Offset",AsShortHexString(c->cmd()->export_off));
        t->AddRow(c->cmd()->export_size,"Export Info Size",AsShortHexString(c->cmd()->export_size));
    }
}


MOEX_NAMESPACE_END
