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
        b->start_value = (uint64_t)b->offset - (uint64_t)d_->ctx()->file_start;
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

    bool has_module = true; // objc version detector

    if(unique_name == "__OBJC/__module_info") has_module = false;

    if(unique_name == "__OBJC/__class_ext")
        InitTodoView();

    if(unique_name == "__OBJC/__protocol_ext")
        InitTodoView();

    if(unique_name == "__OBJC/__image_info" || unique_name == "__DATA/__objc_imageinfo")
        InitObjC2ImageInfo("ObjC2 Image Info");

    if(section_name == "__cfstring")
        InitCFStringView("ObjC CFStrings");

    if(has_module){
        if(unique_name == "__OBJC2/__category_list" || unique_name == "__DATA/__objc_catlist")
            InitObjC2PointerView("ObjC2 Category List");

        if(unique_name == "__OBJC2/__class_list" || unique_name == "__DATA/__objc_classlist")
            InitObjC2PointerView("ObjC2 Class List");

        if(unique_name == "__OBJC2/__class_refs" || unique_name == "__DATA/__objc_classrefs")
            InitObjC2PointerView("ObjC2 Class References");

        if(unique_name == "__OBJC2/__super_refs" || unique_name == "__DATA/__objc_superrefs")
            InitObjC2PointerView("ObjC2 Super References");

        if(unique_name == "__OBJC2/__protocol_list" || unique_name == "__DATA/__objc_protolist")
            InitObjC2PointerView("ObjC2 Proto List");

        if(unique_name == "__OBJC2/__message_refs" || unique_name == "__DATA/__objc_msgrefs")
            InitTodoView();
    }
}

void SectionViewNode::InitCStringView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);
    t->SetHeaders({"Index","Offset","Data","Length","String"});
    t->SetWidths({80,100,100,80,400});

    int lineno = 0;
    d_->ForEachAs_S_CSTRING_LITERALS([&](char *cur){
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

        ++lineno;
    });

    AddViewData(t);
}

void SectionViewNode::InitLiteralsView(const std::string &title,size_t unitsize)
{
    auto t = CreateTableViewDataPtr(title);

    d_->ForEachAs_N_BYTE_LITERALS([&](void *cur){
        std::string name = AsHexData(cur,unitsize);
        t->AddRow(AsString(d_->GetRAW(cur)),AsHexData(cur,unitsize),"Floating Point Number",name);

        std::string symbolname = boost::str(boost::format("0x%1%X:%2%")
                                            % cur
                                            % name
        );
    },unitsize);

    AddViewData(t);
}

void SectionViewNode::InitPointersView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);

    d_->ForEachAs_POINTERS([&](void * ptr){
        if(d_->Is64()){
            uint64_t *cur = static_cast<uint64_t*>(ptr);
            std::string symbolname = boost::str(boost::format("%1%->%2%")
                                                % AsShortHexString((uint64_t)cur)
                                                % AsShortHexString(*cur)
            );
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",symbolname);
        }else{
            uint32_t *cur = static_cast<uint32_t*>(ptr);
            std::string symbolname = boost::str(boost::format("%1%->%2%")
                                                % AsShortHexString((uint64_t)cur)
                                                % AsShortHexString(*cur)
            );
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",symbolname);
        }
    });

    AddViewData(t);
}

void SectionViewNode::InitIndirectPointersView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);

    d_->ForEachAs_POINTERS([&](void * ptr){
        if(d_->Is64()){
            uint64_t *cur = static_cast<uint64_t*>(ptr);
            t->AddRow(d_->GetRAW(cur),*cur,"Indirect Pointer",AsShortHexString(*cur));
        }else{
            uint32_t *cur = static_cast<uint32_t*>(ptr);
            t->AddRow(d_->GetRAW(cur),*cur,"Indirect Pointer",AsShortHexString(*cur));
        }
    });

    AddViewData(t);
}

void SectionViewNode::InitIndirectStubsView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);
    d_->ForEachAs_S_SYMBOL_STUBS([&](void * cur,size_t unitsize){
        t->AddRow(AsHexString(d_->GetRAW(cur)),AsHexData(cur,unitsize),"Indirect Stub",AsHexData(cur,unitsize));
    });

    AddViewData(t);
}

void SectionViewNode::InitCFStringView(const std::string &title)
{
    auto t = CreateTableViewDataPtr(title);

    if(d_->Is64()){
        auto results = util::ParsePointerAsType<cfstring64_t>(GetOffset(),GetSize());
        for(auto *cur : results){
            const char *pstr = (const char*)cur->cstr;
            t->AddRow(d_->GetRAW(&cur->ptr),(uint64_t)cur->ptr,"CFString Ptr",AsShortHexString(cur->ptr));
            t->AddRow(d_->GetRAW(&cur->data),(uint64_t)cur->data,"Data",AsShortHexString(cur->data));
            t->AddRow(d_->GetRAW(&cur->cstr),(uint64_t)cur->cstr,"String",AsShortHexString(cur->cstr));
            t->AddRow(d_->GetRAW(&cur->size),(uint64_t)cur->size,"Size",AsHexString(cur->size));

            t->AddSeparator();
        }
    }else{
        auto results = util::ParsePointerAsType<cfstring_t>(GetOffset(),GetSize());
        for(auto *cur : results){
            const char *pstr = (const char*)cur->cstr;
            t->AddRow(d_->GetRAW(&cur->ptr),(uint32_t)cur->ptr,"CFString Ptr",AsShortHexString(cur->ptr));
            t->AddRow(d_->GetRAW(&cur->data),(uint32_t)cur->data,"Data",AsShortHexString(cur->data));
            t->AddRow(d_->GetRAW(&cur->cstr),(uint32_t)cur->cstr,"String",AsShortHexString(cur->cstr));
            t->AddRow(d_->GetRAW(&cur->size),(uint32_t)cur->size,"Size",AsHexString(cur->size));

            t->AddSeparator();
        }
    }

    AddViewData(t);
}
void SectionViewNode::InitObjC2PointerView(const std::string &title){
    auto t = CreateTableViewDataPtr(title);

    d_->ForEachAs_ObjC2Pointer([&](void * ptr){
        if(d_->Is64()){
            uint64_t *cur = static_cast<uint64_t*>(ptr);
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",
                      AsShortHexString(*cur));
            // todo rva to symbol
        }else{
            uint32_t *cur = static_cast<uint32_t*>(ptr);
            t->AddRow(d_->GetRAW(cur),*cur,"Pointer",
                      AsShortHexString(*cur));
            // todo rva to symbol
        }
    });

    AddViewData(t);
}

void SectionViewNode::InitObjC2ImageInfo(const std::string &title){

    auto t = CreateTableViewDataPtr(title);
    d_->ParseAsObjCImageInfo([&](objc_image_info *info){

        t->AddRow(d_->GetRAW(&info->version),AsHexString(info->version),"Version",AsShortHexString(info->version));
        t->AddRow(d_->GetRAW(&info->flags),AsHexString(info->flags),"Flags",AsShortHexString(info->flags));

        if(info->flags & OBJC_IMAGE_IS_REPLACEMENT)t->AddRow({"","","0x1","OBJC_IMAGE_IS_REPLACEMENT"});
        if(info->flags & OBJC_IMAGE_SUPPORTS_GC)t->AddRow({"","","0x2","OBJC_IMAGE_SUPPORTS_GC"});
        if(info->flags & OBJC_IMAGE_GC_ONLY)t->AddRow({"","","0x4","OBJC_IMAGE_GC_ONLY"});
    });

    AddViewData(t);
}
void SectionViewNode::InitTodoView(){

    auto t = CreateTableViewDataPtr("Detail Info");
    t->AddRow({"//todo","","",""});
    AddViewData(t);
}

MOEX_NAMESPACE_END
