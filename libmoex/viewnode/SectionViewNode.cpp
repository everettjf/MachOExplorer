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


//    InitViewDatas();

}
std::string SectionViewNode::GetDisplayName(){
    return boost::str(boost::format("Section(%1%,%2%)")%d_->sect().segment_name()%d_->sect().section_name());
}

void SectionViewNode::InitViewDatas(){

    // SpecialView accourding to section type or name etc.
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

    // section type
    switch(sect.flags() & SECTION_TYPE){
    case S_CSTRING_LITERALS:{
        InitCStringView("C String Literals");
        break;
    }

    case S_4BYTE_LITERALS:{
        InitLiteralsView("Floating Point Literals",4);
        break;
    }
    case S_8BYTE_LITERALS:{
        InitLiteralsView("Floating Point Literals",8);
        break;
    }
    case S_16BYTE_LITERALS:{
        InitLiteralsView("Floating Point Literals",16);
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
        InitIndirectStubsView("Symbol Stubs");
        break;
    }
    default:break;
    }

    // section name
    std::string unique_name = d_->sect().segment_name() + "/" + d_->sect().section_name();
    std::string section_name = d_->sect().section_name();

    bool is_objc_2_0 = true; // objc version detector

    if(unique_name == "__OBJC/__module_info"){
        is_objc_2_0 = false;

    }

    if(unique_name == "__OBJC/__class_ext"){

    }

    if(unique_name == "__OBJC/__protocol_ext"){

    }

    if(unique_name == "__OBJC/__image_info" || unique_name == "__DATA/__objc_imageinfo"){

    }

    if(section_name == "__cfstring"){
        InitCFStringView("ObjC CFStrings");
    }

    if(is_objc_2_0){

        if(unique_name == "__OBJC2/__category_list" || unique_name == "__DATA/__objc_catlist"){

        }


        if(unique_name == "__OBJC2/__class_list" || unique_name == "__DATA/__objc_classlist"){

        }

        if(unique_name == "__OBJC2/__class_refs" || unique_name == "__DATA/__objc_classrefs"){

        }

        if(unique_name == "__OBJC2/__super_refs" || unique_name == "__DATA/__objc_superrefs"){

        }

        if(unique_name == "__OBJC2/__protocol_list" || unique_name == "__DATA/__objc_protolist"){

        }

        if(unique_name == "__OBJC2/__message_refs" || unique_name == "__DATA/__objc_msgrefs"){

        }

    }


}

void SectionViewNode::InitCStringView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);
    t->SetHeaders({"Index","Offset","Data","Length","String"});
    t->SetWidths({80,100,100,80,400});

    int lineno = 0;
    auto array = util::ParseStringLiteral(GetOffset(),GetSize());
    for(char * cur : array){
        std::string name(cur);
        t->AddRow({
                AsString(lineno),
                AsHexString(d_->GetRAW(cur)),
                AsHexData(cur,name.length()),
                AsString(name.length()),
                name
                  });

        std::string symbolname = boost::str(boost::format("0x%1$X:\"%2%\"")
                                            % (uint64_t)cur
                                            % name
                                            );
        d_->header()->AddSymbolNameByMemoryOff((uint64_t)cur,symbolname);

        ++lineno;
    }

    AddViewData(t);
}

void SectionViewNode::InitLiteralsView(const std::string &title,size_t unitsize)
{
    auto t = CreateTableViewDataPtr(title);

    auto array = util::ParseDataAsSize(GetOffset(),GetSize(),unitsize);
    for(char *cur : array){
        std::string name = AsHexData(cur,unitsize);
        t->AddRow(AsString(d_->GetRAW(cur)),AsHexData(cur,unitsize),"Floating Point Number",name);

        std::string symbolname = boost::str(boost::format("0x%1%X:%2%")
                                            % cur
                                            % name
                                            );
        d_->header()->AddSymbolNameByMemoryOff((uint64_t)cur,symbolname);
    }
    AddViewData(t);
}

void SectionViewNode::InitPointersView(const std::string &title)
{

    auto t = CreateTableViewDataPtr(title);

    if(d_->Is64()){
        auto array = util::ParsePointerAsType<uint64_t>(GetOffset(),GetSize());
        for(uint64_t *cur : array){
            std::string symbolname = boost::str(boost::format("%1%->%2%")
                                                % d_->header()->FindSymbolAtFileOffset((uint64_t)cur)
                                                % d_->header()->FindSymbolAtRVA(*cur)
                                                );
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",symbolname);

            d_->header()->AddSymbolNameByMemoryOff((uint64_t)cur,symbolname);
        }
    }else{
        auto array = util::ParsePointerAsType<uint32_t>(GetOffset(),GetSize());
        for(uint32_t *cur : array){
            std::string symbolname = boost::str(boost::format("%1%->%2%")
                                                % d_->header()->FindSymbolAtFileOffset((uint64_t)cur)
                                                % d_->header()->FindSymbolAtRVA(*cur)
                                                );
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",symbolname);

            d_->header()->AddSymbolNameByMemoryOff((uint64_t)cur,symbolname);
        }
    }

    AddViewData(t);
}

void SectionViewNode::InitIndirectPointersView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);

    if(d_->Is64()){
        auto array = util::ParsePointerAsType<uint64_t>(GetOffset(),GetSize());
        for(uint64_t *cur : array){
            t->AddRow(d_->GetRAW(cur),*cur,"Indirect Pointer",AsShortHexString(*cur));
        }
    }else{
        auto array = util::ParsePointerAsType<uint32_t>(GetOffset(),GetSize());
        for(uint32_t *cur : array){
            t->AddRow(d_->GetRAW(cur),*cur,"Indirect Pointer",AsShortHexString(*cur));
        }
    }

    AddViewData(t);
}

void SectionViewNode::InitIndirectStubsView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);

    size_t unitsize = d_->sect().reserved2();
    auto array = util::ParseDataAsSize(GetOffset(),GetSize(),unitsize);
    for(char *cur : array){
        t->AddRow(AsString(d_->GetRAW(cur)),AsHexData(cur,unitsize),"Indirect Stub",AsHexData(cur,unitsize));
    }

    AddViewData(t);

}

void SectionViewNode::InitCFStringView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);

    if(d_->Is64()){
        auto results = util::ParsePointerAsType<cfstring64_t>(GetOffset(),GetSize());
        for(auto *cur : results){
            const char *pstr = (const char*)cur->cstr;
            t->AddRow(d_->GetRAW(&cur->ptr),(uint64_t)cur->ptr,"CFString Ptr",d_->header()->FindSymbolAtRVA(cur->ptr));
            t->AddRow(d_->GetRAW(&cur->data),(uint64_t)cur->data,"Data",d_->header()->FindSymbolAtRVA(cur->data));
            t->AddRow(d_->GetRAW(&cur->cstr),(uint64_t)cur->cstr,"String",d_->header()->FindSymbolAtRVA(cur->cstr));
            t->AddRow(d_->GetRAW(&cur->size),(uint64_t)cur->size,"Size",AsHexString(cur->size));

            t->AddSeparator();
        }
    }else{
        auto results = util::ParsePointerAsType<cfstring_t>(GetOffset(),GetSize());
        for(auto *cur : results){
            const char *pstr = (const char*)cur->cstr;
            t->AddRow(d_->GetRAW(&cur->ptr),(uint32_t)cur->ptr,"CFString Ptr",d_->header()->FindSymbolAtRVA(cur->ptr));
            t->AddRow(d_->GetRAW(&cur->data),(uint32_t)cur->data,"Data",d_->header()->FindSymbolAtRVA(cur->data));
            t->AddRow(d_->GetRAW(&cur->cstr),(uint32_t)cur->cstr,"String",d_->header()->FindSymbolAtRVA(cur->cstr));
            t->AddRow(d_->GetRAW(&cur->size),(uint32_t)cur->size,"Size",AsHexString(cur->size));

            t->AddSeparator();
        }
    }

    AddViewData(t);
}


MOEX_NAMESPACE_END
