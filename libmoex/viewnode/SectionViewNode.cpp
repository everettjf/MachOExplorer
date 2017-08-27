//
// Created by everettjf on 2017/8/6.
//

#include "SectionViewNode.h"

MOEX_NAMESPACE_BEGIN
using namespace moex::util;


SectionViewNodePtr SectionViewNodeFactory::Create(MachSectionPtr d){
    SectionViewNodePtr view = std::make_shared<SectionViewNode>();
    view->Init(d);
    return view;
}

void SectionViewNode::Init(MachSectionPtr d){
    d_ = d;
}
std::string SectionViewNode::GetDisplayName(){
    return boost::str(boost::format("Section(%1%,%2%)")%d_->sect().segment_name()%d_->sect().section_name());
}

void SectionViewNode::InitViewDatas(){

    // SpecialView accourding to section type
    {
        InitSpecialView();
    }

    // Table
    {
        auto t = CreateTableViewDataPtr();
        moex_section &sect = d_->sect();
        t->AddRow(d_->GetRAW(&(sect.sectname())),(void*)sect.sectname(),sizeof(char)*16,"Section Name",sect.section_name());
        t->AddRow(d_->GetRAW(&(sect.segname())),(void*)sect.segname(),sizeof(char)*16,"Segment Name",sect.segment_name());

        if(d_->Is64()) {
            t->AddRow(d_->GetRAW(&(sect.addr64())), sect.addr64(), "Address", AsShortHexString(sect.addr64()));
            t->AddRow(d_->GetRAW(&(sect.size64())), sect.size64(), "Size", AsShortHexString(sect.size64()));
        }else{
            t->AddRow(d_->GetRAW(&(sect.addr())), sect.addr(), "Address", AsShortHexString(sect.addr()));
            t->AddRow(d_->GetRAW(&(sect.size())), sect.size(), "Size", AsShortHexString(sect.size()));
        }

        t->AddRow(d_->GetRAW(&(sect.offset())),sect.offset(),"Offset",AsShortHexString(sect.offset()));
        t->AddRow(d_->GetRAW(&(sect.align())),sect.align(),"Alignment",AsShortHexString(sect.align()));
        t->AddRow(d_->GetRAW(&(sect.reloff())),sect.reloff(),"Relocations Offset",AsShortHexString(sect.reloff()));
        t->AddRow(d_->GetRAW(&(sect.nreloc())),sect.nreloc(),"Number of Relocations",AsShortHexString(sect.nreloc()));
        t->AddRow(d_->GetRAW(&(sect.flags())),sect.flags(),"Flags",AsShortHexString(sect.flags()));
        t->AddRow(d_->GetRAW(&(sect.reserved1())),sect.reserved1(),"Reserved1",AsShortHexString(sect.reserved1()));
        t->AddRow(d_->GetRAW(&(sect.reserved2())),sect.reserved2(),"Reserved2",AsShortHexString(sect.reserved2()));

        if(d_->Is64()){
            t->AddRow(d_->GetRAW(&(sect.reserved3())),sect.reserved3(),"Reserved3",AsShortHexString(sect.reserved3()));
        }
        AddViewData(t);
    }

    // Binary
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = (char*)d_->offset();
        b->size = d_->DATA_SIZE();
        AddViewData(b);
    }



}

void SectionViewNode::InitSpecialView()
{
    moex_section &sect = d_->sect();

    switch(sect.flags() & SECTION_TYPE){
    case S_CSTRING_LITERALS:{
        InitCStringView("C String Literals");
        break;
    }
    case S_4BYTE_LITERALS:{
        InitLiteralsView("Floating Point Literals");
        break;
    }
    case S_8BYTE_LITERALS:{
        InitLiteralsView("Floating Point Literals");
        break;
    }
    case S_16BYTE_LITERALS:{
        InitLiteralsView("Floating Point Literals");
        break;
    }
    case S_LITERAL_POINTERS:{
        InitPointersView("Literal Pointers");
        break;
    }
    case S_MOD_INIT_FUNC_POINTERS:{
        InitPointersView("Module Init Func Pointers");
        break;
    }
    case S_MOD_TERM_FUNC_POINTERS:{
        InitPointersView("Module Term Func Pointers");
        break;
    }
    case S_LAZY_SYMBOL_POINTERS:{
        InitIndirectPointersView("Lazy Symbol Pointers");
        break;
    }
    case S_NON_LAZY_SYMBOL_POINTERS:{
        InitIndirectPointersView("Non-Lazy Symbol Pointers");
        break;
    }
    case S_LAZY_DYLIB_SYMBOL_POINTERS:{
        InitIndirectPointersView("Lazy Dylib Symbol Pointers");
        break;
    }
    case S_SYMBOL_STUBS:{
        InitIndirectPointersView("Symbol Stubs");
        break;
    }
    default:break;
    }

}

void SectionViewNode::InitCStringView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);
    t->SetHeaders({"Index","Offset","Data","Length","String"});
    t->SetWidths({80,100,100,80,400});

    char *offset = (char*)d_->header()->header_start() + d_->sect().offset();
    uint32_t size = (uint32_t)d_->sect().size_both();

    int lineno = 0;
    auto array = util::ParseStringLiteral(offset,size);
    for(char * cur : array){
        std::string name(cur);
        t->AddRow({
                AsString(lineno),
                AsHexString(d_->GetRAW(cur)),
                AsHexData(cur,name.length()),
                AsString(name.length()),
                name
                  });

        ++lineno;
    }

    AddViewData(t);
}

void SectionViewNode::InitLiteralsView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);
    t->AddRow("//todo","","","");
    AddViewData(t);
}

void SectionViewNode::InitPointersView(const std::string &title)
{
    char *offset = (char*)d_->header()->header_start() + d_->sect().offset();
    uint32_t size = (uint32_t)d_->sect().size_both();

    auto t = CreateTableViewDataPtr(title);

    if(d_->Is64()){
        auto array = util::ParsePointer<uint64_t>(offset,size);
        for(uint64_t *cur : array){
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",AsShortHexString(*cur));
        }
    }else{
        auto array = util::ParsePointer<uint32_t>(offset,size);
        for(uint32_t *cur : array){
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",AsShortHexString(*cur));
        }
    }

    AddViewData(t);
}

void SectionViewNode::InitIndirectPointersView(const std::string &title)
{

    auto t = CreateTableViewDataPtr(title);
    t->AddRow("//todo","","","");
    AddViewData(t);
}


MOEX_NAMESPACE_END
