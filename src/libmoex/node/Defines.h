//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef DEFINE_H
#define DEFINE_H

#include <cstdlib>

struct cfstring_t
{
  uint32_t ptr;
  uint32_t data;
  uint32_t cstr;
  uint32_t size;
};

struct cfstring64_t
{
  uint64_t ptr;
  uint64_t data;
  uint64_t cstr;
  uint64_t size;
};

/* masks for objc_image_info.flags */
#define OBJC_IMAGE_IS_REPLACEMENT   (1<<0)
#define OBJC_IMAGE_SUPPORTS_GC      (1<<1)
#define OBJC_IMAGE_GC_ONLY          (1<<2)

/* Values for class_ro_t->flags */
#define RO_META                     (1<<0)
#define RO_ROOT                     (1<<1)
#define RO_HAS_CXX_STRUCTORS        (1<<2)

//--------------------- ObjC ----------------------------------------

struct objc_image_info
{
    uint32_t version;
    uint32_t flags;
};

struct objc_module_t
{
    uint32_t version;
    uint32_t size;
    uint32_t name;              // char * (32-bit pointer)
    uint32_t symtab;            // struct objc_symtab * (32-bit pointer)
};

struct objc_symtab_t
{
    uint32_t sel_ref_cnt;
    uint32_t refs;              // SEL * (32-bit pointer)
    uint16_t cls_def_cnt;
    uint16_t cat_def_cnt;
    uint32_t defs[0];           // void * (32-bit pointer) variable size
};

struct objc_class_t
{
    uint32_t isa;               // struct objc_class * (32-bit pointer)
    uint32_t super_class;       // struct objc_class * (32-bit pointer)
    uint32_t name;              // const char * (32-bit pointer)
    int32_t version;
    int32_t info;
    int32_t instance_size;
    uint32_t ivars;             // struct objc_ivar_list * (32-bit pointer)
    uint32_t methodLists;       // struct objc_method_list ** (32-bit pointer)
    uint32_t cache;             // struct objc_cache * (32-bit pointer)
    uint32_t protocols;         // struct objc_protocol_list * (32-bit pointer)
};

struct objc_category_t
{
    uint32_t category_name;     // char * (32-bit pointer)
    uint32_t class_name;        // char * (32-bit pointer)
    uint32_t instance_methods;	// struct objc_method_list * (32-bit pointer)
    uint32_t class_methods;     // struct objc_method_list * (32-bit pointer)
    uint32_t protocols;         // struct objc_protocol_list * (32-bit ptr)
};

struct objc_ivar_t
{
    uint32_t ivar_name;         // char * (32-bit pointer)
    uint32_t ivar_type;         // char * (32-bit pointer)
    int32_t ivar_offset;
};

struct objc_ivar_list_t
{
    int32_t ivar_count;
    struct objc_ivar_t ivar_list[0];  // variable length structure
};

struct objc_method_t
{
    uint32_t method_name;       // SEL, aka struct objc_selector * (32-bit pointer)
    uint32_t method_types;      // char * (32-bit pointer)
    uint32_t method_imp;        // IMP, aka function pointer, (*IMP)(id, SEL, ...) (32-bit pointer)
};

struct objc_method_list_t
{
    uint32_t obsolete;          // struct objc_method_list * (32-bit pointer)
    int32_t method_count;
    struct objc_method_t method_list[0];  // variable length structure
};

struct objc_protocol_t
{
    uint32_t isa;               // struct objc_class * (32-bit pointer)
    uint32_t protocol_name;     // char * (32-bit pointer)
    uint32_t protocol_list;     // struct objc_protocol_list * (32-bit pointer)
    uint32_t instance_methods;	// struct objc_method_description_list * (32-bit pointer)
    uint32_t class_methods;     // struct objc_method_description_list * (32-bit pointer)
};

struct objc_protocol_list_t
{
    uint32_t next;              // struct objc_protocol_list * (32-bit pointer)
    int32_t count;
    uint32_t list[0];           // Protocol *, aka struct objc_protocol_t * (32-bit pointer)
};

struct objc_method_description_t
{
    uint32_t name;              // SEL, aka struct objc_selector * (32-bit pointer)
    uint32_t types;             // char * (32-bit pointer)
};

struct objc_method_description_list_t
{
    int32_t count;
    struct objc_method_description_t list[0];
};

//--------------------- ObjC2 32bit ----------------------------------------

struct class_t
{
    uint32_t isa;               // class_t * (32-bit pointer)
    uint32_t superclass;        // class_t * (32-bit pointer)
    uint32_t cache;             // Cache (32-bit pointer)
    uint32_t vtable;            // IMP * (32-bit pointer)
    uint32_t data;              // class_ro_t * (32-bit pointer)
};

struct class_ro_t
{
    uint32_t flags;
    uint32_t instanceStart;
    uint32_t instanceSize;
    uint32_t ivarLayout;        // const uint8_t * (32-bit pointer)
    uint32_t name;              // const char * (32-bit pointer)
    uint32_t baseMethods;       // const method_list_t * (32-bit pointer)
    uint32_t baseProtocols;     // const protocol_list_t * (32-bit pointer)
    uint32_t ivars;             // const ivar_list_t * (32-bit pointer)
    uint32_t weakIvarLayout;    // const uint8_t * (32-bit pointer)
    uint32_t baseProperties;    // const struct objc_property_list * (32-bit pointer)
};

struct method_t
{
    uint32_t name;              // SEL (32-bit pointer)
    uint32_t types;             // const char * (32-bit pointer)
    uint32_t imp;               // IMP (32-bit pointer)
};

struct method_list_t
{
    uint32_t entsize;
    uint32_t count;
    //struct method_t first;  These structures follow inline
};

struct ivar_list_t
{
    uint32_t entsize;
    uint32_t count;
    // struct ivar_t first;  These structures follow inline
};

struct ivar_t
{
    uint32_t offset;            // uintptr_t * (32-bit pointer)
    uint32_t name;              // const char * (32-bit pointer)
    uint32_t type;              // const char * (32-bit pointer)
    uint32_t alignment;
    uint32_t size;
};

struct protocol_list_t
{
    uint32_t count;             // uintptr_t (a 32-bit value)
    // struct protocol_t * list[0];  These pointers follow inline
};

struct protocol_t
{
    uint32_t isa;               // id * (32-bit pointer)
    uint32_t name;              // const char * (32-bit pointer)
    uint32_t protocols;         // struct protocol_list_t * (32-bit pointer)
    uint32_t instanceMethods;		// method_list_t * (32-bit pointer)
    uint32_t classMethods;      // method_list_t * (32-bit pointer)
    uint32_t optionalInstanceMethods;	// method_list_t * (32-bit pointer)
    uint32_t optionalClassMethods;	// method_list_t * (32-bit pointer)
    uint32_t instanceProperties;	// struct objc_property_list * (32-bit pointer)
};

struct objc_property_list
{
    uint32_t entsize;
    uint32_t count;
    // struct objc_property first;  These structures follow inline
};

struct objc_property
{
    uint32_t name;              // const char * (32-bit pointer)
    uint32_t attributes;        // const char * (32-bit pointer)
};

struct category_t
{
    uint32_t name;              // const char * (32-bit pointer)
    uint32_t cls;               // struct class_t * (32-bit pointer)
    uint32_t instanceMethods;   // struct method_list_t * (32-bit pointer)
    uint32_t classMethods;      // struct method_list_t * (32-bit pointer)
    uint32_t protocols;         // struct protocol_list_t * (32-bit pointer)
    uint32_t instanceProperties; // struct objc_property_list * (32-bit pointer)
};

struct message_ref
{
    uint32_t imp;               // IMP (32-bit pointer)
    uint32_t sel;               // SEL (32-bit pointer)
};

//--------------------- ObjC2 64bit ----------------------------------------

struct class64_t
{
    uint64_t isa;               // class_t * (64-bit pointer)
    uint64_t superclass;        // class_t * (64-bit pointer)
    uint64_t cache;             // Cache (64-bit pointer)
    uint64_t vtable;            // IMP * (64-bit pointer)
    uint64_t data;              // class_ro_t * (64-bit pointer)
};

struct class64_ro_t
{
    uint32_t flags;
    uint32_t instanceStart;
    uint32_t instanceSize;
    uint32_t reserved;
    uint64_t ivarLayout;        // const uint8_t * (64-bit pointer)
    uint64_t name;              // const char * (64-bit pointer)
    uint64_t baseMethods;       // const method_list_t * (64-bit pointer)
    uint64_t baseProtocols;     // const protocol_list_t * (64-bit pointer)
    uint64_t ivars;             // const ivar_list_t * (64-bit pointer)
    uint64_t weakIvarLayout;    // const uint8_t * (64-bit pointer)
    uint64_t baseProperties;    // const struct objc_property_list * (64-bit pointer)
};

struct method64_list_t
{
    uint32_t entsize;
    uint32_t count;
    // struct method_t first;  These structures follow inline
};

struct method64_t
{
    uint64_t name;              // SEL (64-bit pointer)
    uint64_t types;             // const char * (64-bit pointer)
    uint64_t imp;               // IMP (64-bit pointer)
};

struct ivar64_list_t
{
    uint32_t entsize;
    uint32_t count;
    // struct ivar_t first;  These structures follow inline
};

struct ivar64_t
{
    uint64_t offset;            // uintptr_t * (64-bit pointer)
    uint64_t name;              // const char * (64-bit pointer)
    uint64_t type;              // const char * (64-bit pointer)
    uint32_t alignment;
    uint32_t size;
};

struct protocol64_list_t
{
    uint64_t count;             // uintptr_t (a 64-bit value)
    // struct protocol_t * list[0];  These pointers follow inline
};

struct protocol64_t
{
    uint64_t isa;               // id * (64-bit pointer)
    uint64_t name;              // const char * (64-bit pointer)
    uint64_t protocols;         // struct protocol_list_t * (64-bit pointer)
    uint64_t instanceMethods;		// method_list_t * (64-bit pointer)
    uint64_t classMethods;      // method_list_t * (64-bit pointer)
    uint64_t optionalInstanceMethods;	// method_list_t * (64-bit pointer)
    uint64_t optionalClassMethods;	// method_list_t * (64-bit pointer)
    uint64_t instanceProperties;	// struct objc_property_list * (64-bit pointer)
};

struct objc_property64_list
{
    uint32_t entsize;
    uint32_t count;
    // struct objc_property first;  These structures follow inline
};

struct objc_property64
{
    uint64_t name;              // const char * (64-bit pointer)
    uint64_t attributes;        // const char * (64-bit pointer)
};

struct category64_t
{
    uint64_t name;              // const char * (64-bit pointer)
    uint64_t cls;               // struct class_t * (64-bit pointer)
    uint64_t instanceMethods;   // struct method_list_t * (64-bit pointer)
    uint64_t classMethods;      // struct method_list_t * (64-bit pointer)
    uint64_t protocols;         // struct protocol_list_t * (64-bit pointer)
    uint64_t instanceProperties; // struct objc_property_list * (64-bit pointer)
};

struct message_ref64
{
    uint64_t imp;               // IMP (64-bit pointer)
    uint64_t sel;               // SEL (64-bit pointer)
};

#endif // DEFINE_H
