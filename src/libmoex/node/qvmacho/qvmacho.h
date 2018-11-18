#ifndef QVMACHO_H
#define QVMACHO_H

#include <cstdint>
#include "qvtype.h"
#include "qvcpu.h"

/*
 * replacement for
 <mach-o/loader.h>
 <mach-o/swap.h>
 <mach-o/qv_nlist.h>

 */


/*
 * The following are used to encode rebasing information
 */
#define REBASE_TYPE_POINTER					1
#define REBASE_TYPE_TEXT_ABSOLUTE32				2
#define REBASE_TYPE_TEXT_PCREL32				3

#define REBASE_OPCODE_MASK					0xF0
#define REBASE_IMMEDIATE_MASK					0x0F
#define REBASE_OPCODE_DONE					0x00
#define REBASE_OPCODE_SET_TYPE_IMM				0x10
#define REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB		0x20
#define REBASE_OPCODE_ADD_ADDR_ULEB				0x30
#define REBASE_OPCODE_ADD_ADDR_IMM_SCALED			0x40
#define REBASE_OPCODE_DO_REBASE_IMM_TIMES			0x50
#define REBASE_OPCODE_DO_REBASE_ULEB_TIMES			0x60
#define REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB			0x70
#define REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB	0x80


/*
 * The following are used to encode binding information
 */
#define BIND_TYPE_POINTER					1
#define BIND_TYPE_TEXT_ABSOLUTE32				2
#define BIND_TYPE_TEXT_PCREL32					3

#define BIND_SPECIAL_DYLIB_SELF					 0
#define BIND_SPECIAL_DYLIB_MAIN_EXECUTABLE			-1
#define BIND_SPECIAL_DYLIB_FLAT_LOOKUP				-2
#define BIND_SPECIAL_DYLIB_WEAK_LOOKUP				-3

#define BIND_SYMBOL_FLAGS_WEAK_IMPORT				0x1
#define BIND_SYMBOL_FLAGS_NON_WEAK_DEFINITION			0x8

#define BIND_OPCODE_MASK					0xF0
#define BIND_IMMEDIATE_MASK					0x0F
#define BIND_OPCODE_DONE					0x00
#define BIND_OPCODE_SET_DYLIB_ORDINAL_IMM			0x10
#define BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB			0x20
#define BIND_OPCODE_SET_DYLIB_SPECIAL_IMM			0x30
#define BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM		0x40
#define BIND_OPCODE_SET_TYPE_IMM				0x50
#define BIND_OPCODE_SET_ADDEND_SLEB				0x60
#define BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB			0x70
#define BIND_OPCODE_ADD_ADDR_ULEB				0x80
#define BIND_OPCODE_DO_BIND					0x90
#define BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB			0xA0
#define BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED			0xB0
#define BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB		0xC0
#define	BIND_OPCODE_THREADED					0xD0
#define	BIND_SUBOPCODE_THREADED_SET_BIND_ORDINAL_TABLE_SIZE_ULEB 0x00
#define	BIND_SUBOPCODE_THREADED_APPLY				 0x01


/*
 * The following are used on the flags byte of a terminal node
 * in the export information.
 */
#define EXPORT_SYMBOL_FLAGS_KIND_MASK				0x03
#define EXPORT_SYMBOL_FLAGS_KIND_REGULAR			0x00
#define EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL			0x01
#define EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE			0x02
#define EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION			0x04
#define EXPORT_SYMBOL_FLAGS_REEXPORT				0x08
#define EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER			0x10



#define	VM_PROT_NONE	((qv_vm_prot_t) 0x00)

#define VM_PROT_READ	((qv_vm_prot_t) 0x01)	/* read permission */
#define VM_PROT_WRITE	((qv_vm_prot_t) 0x02)	/* write permission */
#define VM_PROT_EXECUTE	((qv_vm_prot_t) 0x04)	/* execute permission */

/*
 *	The default protection for newly-created virtual memory
 */

#define VM_PROT_DEFAULT	(VM_PROT_READ|VM_PROT_WRITE)

/*
 *	The maximum privileges possible, for parameter checking.
 */

#define VM_PROT_ALL	(VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)

/*
 *	An invalid protection value.
 *	Used only by memory_object_lock_request to indicate no change
 *	to page locks.  Using -1 here is a bad idea because it
 *	looks like VM_PROT_ALL and then some.
 */

#define VM_PROT_NO_CHANGE	((qv_vm_prot_t) 0x08)

/*
 *      When a caller finds that he cannot obtain write permission on a
 *      mapped entry, the following flag can be used.  The entry will
 *      be made "needs copy" effectively copying the object (using COW),
 *      and write permission will be added to the maximum protections
 *      for the associated entry.
 */

#define VM_PROT_COPY            ((qv_vm_prot_t) 0x10)


/*
 *	Another invalid protection value.
 *	Used only by memory_object_data_request upon an object
 *	which has specified a copy_call copy strategy. It is used
 *	when the kernel wants a page belonging to a copy of the
 *	object, and is only asking the object as a result of
 *	following a shadow chain. This solves the race between pages
 *	being pushed up by the memory manager and the kernel
 *	walking down the shadow chain.
 */

#define VM_PROT_WANTS_COPY	((qv_vm_prot_t) 0x10)


/*
 * 	Another invalid protection value.
 *	Indicates that the other protection bits are to be applied as a mask
 *	against the actual protection bits of the map entry.
 */
#define VM_PROT_IS_MASK		((qv_vm_prot_t) 0x40)

/*
 * Another invalid protection value to support execute-only protection.
 * VM_PROT_STRIP_READ is a special marker that tells mprotect to not
 * set VM_PROT_READ. We have to do it this way because existing code
 * expects the system to set VM_PROT_READ if VM_PROT_EXECUTE is set.
 * VM_PROT_EXECUTE_ONLY is just a convenience value to indicate that
 * the memory should be executable and explicitly not readable. It will
 * be ignored on platforms that do not support this type of protection.
 */
#define VM_PROT_STRIP_READ		((qv_vm_prot_t) 0x80)
#define VM_PROT_EXECUTE_ONLY	(VM_PROT_EXECUTE|VM_PROT_STRIP_READ)


#define	MH_OBJECT	0x1		/* relocatable object file */
#define	MH_EXECUTE	0x2		/* demand paged executable file */
#define	MH_FVMLIB	0x3		/* fixed VM shared library file */
#define	MH_CORE		0x4		/* core file */
#define	MH_PRELOAD	0x5		/* preloaded executable file */
#define	MH_DYLIB	0x6		/* dynamically bound shared library */
#define	MH_DYLINKER	0x7		/* dynamic link editor */
#define	MH_BUNDLE	0x8		/* dynamically bound bundle file */
#define	MH_DYLIB_STUB	0x9		/* shared library stub for static */
                    /*  linking only, no section contents */
#define	MH_DSYM		0xa		/* companion file with only debug */
                    /*  sections */
#define	MH_KEXT_BUNDLE	0xb		/* x86_64 kexts */

/* Constants for the flags field of the mach_header */
#define	MH_NOUNDEFS	0x1		/* the object file has no undefined
                       references */
#define	MH_INCRLINK	0x2		/* the object file is the output of an
                       incremental link against a base file
                       and can't be link edited again */
#define MH_DYLDLINK	0x4		/* the object file is input for the
                       dynamic linker and can't be staticly
                       link edited again */
#define MH_BINDATLOAD	0x8		/* the object file's undefined
                       references are bound by the dynamic
                       linker when loaded. */
#define MH_PREBOUND	0x10		/* the file has its dynamic undefined
                       references prebound. */
#define MH_SPLIT_SEGS	0x20		/* the file has its read-only and
                       read-write segments split */
#define MH_LAZY_INIT	0x40		/* the shared library init routine is
                       to be run lazily via catching memory
                       faults to its writeable segments
                       (obsolete) */
#define MH_TWOLEVEL	0x80		/* the image is using two-level name
                       space bindings */
#define MH_FORCE_FLAT	0x100		/* the executable is forcing all images
                       to use flat name space bindings */
#define MH_NOMULTIDEFS	0x200		/* this umbrella guarantees no multiple
                       defintions of symbols in its
                       sub-images so the two-level namespace
                       hints can always be used. */
#define MH_NOFIXPREBINDING 0x400	/* do not have dyld notify the
                       prebinding agent about this
                       executable */
#define MH_PREBINDABLE  0x800           /* the binary is not prebound but can
                       have its prebinding redone. only used
                                           when MH_PREBOUND is not set. */
#define MH_ALLMODSBOUND 0x1000		/* indicates that this binary binds to
                                           all two-level namespace modules of
                       its dependent libraries. only used
                       when MH_PREBINDABLE and MH_TWOLEVEL
                       are both set. */
#define MH_SUBSECTIONS_VIA_SYMBOLS 0x2000/* safe to divide up the sections into
                        sub-sections via symbols for dead
                        code stripping */
#define MH_CANONICAL    0x4000		/* the binary has been canonicalized
                       via the unprebind operation */
#define MH_WEAK_DEFINES	0x8000		/* the final linked image contains
                       external weak symbols */
#define MH_BINDS_TO_WEAK 0x10000	/* the final linked image uses
                       weak symbols */

#define MH_ALLOW_STACK_EXECUTION 0x20000/* When this bit is set, all stacks
                       in the task will be given stack
                       execution privilege.  Only used in
                       MH_EXECUTE filetypes. */
#define MH_ROOT_SAFE 0x40000           /* When this bit is set, the binary
                      declares it is safe for use in
                      processes with uid zero */

#define MH_SETUID_SAFE 0x80000         /* When this bit is set, the binary
                      declares it is safe for use in
                      processes when issetugid() is true */

#define MH_NO_REEXPORTED_DYLIBS 0x100000 /* When this bit is set on a dylib,
                      the static linker does not need to
                      examine dependent dylibs to see
                      if any are re-exported */
#define	MH_PIE 0x200000			/* When this bit is set, the OS will
                       load the main executable at a
                       random address.  Only used in
                       MH_EXECUTE filetypes. */
#define	MH_DEAD_STRIPPABLE_DYLIB 0x400000 /* Only for use on dylibs.  When
                         linking against a dylib that
                         has this bit set, the static linker
                         will automatically not create a
                         LC_LOAD_DYLIB load command to the
                         dylib if no symbols are being
                         referenced from the dylib. */
#define MH_HAS_TLV_DESCRIPTORS 0x800000 /* Contains a section of type
                        S_THREAD_LOCAL_VARIABLES */

#define MH_NO_HEAP_EXECUTION 0x1000000	/* When this bit is set, the OS will
                       run the main executable with
                       a non-executable heap even on
                       platforms (e.g. i386) that don't
                       require it. Only used in MH_EXECUTE
                       filetypes. */

#define MH_APP_EXTENSION_SAFE 0x02000000 /* The code was linked for use in an
                        application extension. */

#define	MH_NLIST_OUTOFSYNC_WITH_DYLDINFO 0x04000000
                    /* The external symbols listed in the nlist
                       symbol table do not include all the symbols
                       listed in the dyld info. */

#define	MH_SIM_SUPPORT 0x08000000
#define FAT_MAGIC	0xcafebabe
#define FAT_CIGAM	0xbebafeca	/* NXSwapLong(FAT_MAGIC) */

struct qv_fat_header {
    uint32_t	magic;		/* FAT_MAGIC or FAT_MAGIC_64 */
    uint32_t	nfat_arch;	/* number of structs that follow */
};

struct qv_fat_arch {
    qv_cpu_type_t	cputype;	/* cpu specifier (int) */
    qv_cpu_subtype_t	cpusubtype;	/* machine specifier (int) */
    uint32_t	offset;		/* file offset to this object file */
    uint32_t	size;		/* size of this object file */
    uint32_t	align;		/* alignment as a power of 2 */
};

#define FAT_MAGIC_64	0xcafebabf
#define FAT_CIGAM_64	0xbfbafeca	/* NXSwapLong(FAT_MAGIC_64) */


/*
 * The 32-bit mach header appears at the very beginning of the object file for
 * 32-bit architectures.
 */
struct qv_mach_header {
    uint32_t	magic;		/* mach magic number identifier */
    qv_cpu_type_t	cputype;	/* cpu specifier */
    qv_cpu_subtype_t	cpusubtype;	/* machine specifier */
    uint32_t	filetype;	/* type of file */
    uint32_t	ncmds;		/* number of load commands */
    uint32_t	sizeofcmds;	/* the size of all the load commands */
    uint32_t	flags;		/* flags */
};

/* Constant for the magic field of the qv_mach_header (32-bit architectures) */
#define	MH_MAGIC	0xfeedface	/* the mach magic number */
#define MH_CIGAM	0xcefaedfe	/* NXSwapInt(MH_MAGIC) */

/*
 * The 64-bit mach header appears at the very beginning of object files for
 * 64-bit architectures.
 */
struct qv_mach_header_64 {
    uint32_t	magic;		/* mach magic number identifier */
    qv_cpu_type_t	cputype;	/* cpu specifier */
    qv_cpu_subtype_t	cpusubtype;	/* machine specifier */
    uint32_t	filetype;	/* type of file */
    uint32_t	ncmds;		/* number of load commands */
    uint32_t	sizeofcmds;	/* the size of all the load commands */
    uint32_t	flags;		/* flags */
    uint32_t	reserved;	/* reserved */
};

/* Constant for the magic field of the qv_mach_header_64 (64-bit architectures) */
#define MH_MAGIC_64 0xfeedfacf /* the 64-bit mach magic number */
#define MH_CIGAM_64 0xcffaedfe /* NXSwapInt(MH_MAGIC_64) */


struct qv_load_command {
    uint32_t cmd;		/* type of load command */
    uint32_t cmdsize;	/* total size of command in bytes */
};

struct qv_section { /* for 32-bit architectures */
    char		sectname[16];	/* name of this section */
    char		segname[16];	/* segment this section goes in */
    uint32_t	addr;		/* memory address of this section */
    uint32_t	size;		/* size in bytes of this section */
    uint32_t	offset;		/* file offset of this section */
    uint32_t	align;		/* section alignment (power of 2) */
    uint32_t	reloff;		/* file offset of relocation entries */
    uint32_t	nreloc;		/* number of relocation entries */
    uint32_t	flags;		/* flags (section type and attributes)*/
    uint32_t	reserved1;	/* reserved (for offset or index) */
    uint32_t	reserved2;	/* reserved (for count or sizeof) */
};

struct qv_section_64 { /* for 64-bit architectures */
    char		sectname[16];	/* name of this section */
    char		segname[16];	/* segment this section goes in */
    uint64_t	addr;		/* memory address of this section */
    uint64_t	size;		/* size in bytes of this section */
    uint32_t	offset;		/* file offset of this section */
    uint32_t	align;		/* section alignment (power of 2) */
    uint32_t	reloff;		/* file offset of relocation entries */
    uint32_t	nreloc;		/* number of relocation entries */
    uint32_t	flags;		/* flags (section type and attributes)*/
    uint32_t	reserved1;	/* reserved (for offset or index) */
    uint32_t	reserved2;	/* reserved (for count or sizeof) */
    uint32_t	reserved3;	/* reserved */
};



enum NXByteOrder {
    NX_UnknownByteOrder,
    NX_LittleEndian,
    NX_BigEndian
};


#define OSSwapConstInt32(x) \
    ((uint32_t)((((uint32_t)(x) & 0xff000000) >> 24) | \
                (((uint32_t)(x) & 0x00ff0000) >>  8) | \
                (((uint32_t)(x) & 0x0000ff00) <<  8) | \
                (((uint32_t)(x) & 0x000000ff) << 24)))

#define OSSwapInt32(x) OSSwapConstInt32(x)


inline void
qv_swap_fat_arch(
struct qv_fat_arch *fat_archs,
uint32_t nfat_arch,
enum NXByteOrder target_byte_sex)
{
    uint32_t i;

    for(i = 0; i < nfat_arch; i++){
        fat_archs[i].cputype    = OSSwapInt32(fat_archs[i].cputype);
        fat_archs[i].cpusubtype = OSSwapInt32(fat_archs[i].cpusubtype);
        fat_archs[i].offset     = OSSwapInt32(fat_archs[i].offset);
        fat_archs[i].size       = OSSwapInt32(fat_archs[i].size);
        fat_archs[i].align      = OSSwapInt32(fat_archs[i].align);
    }
}

inline
void
qv_swap_fat_header(
struct qv_fat_header *fat_header,
enum NXByteOrder target_byte_sex)
{
    fat_header->magic     = OSSwapInt32(fat_header->magic);
    fat_header->nfat_arch = OSSwapInt32(fat_header->nfat_arch);
}

inline
void
qv_swap_mach_header(
struct qv_mach_header *mh,
enum NXByteOrder target_byte_sex)
{
    mh->magic = OSSwapInt32(mh->magic);
    mh->cputype = OSSwapInt32(mh->cputype);
    mh->cpusubtype = OSSwapInt32(mh->cpusubtype);
    mh->filetype = OSSwapInt32(mh->filetype);
    mh->ncmds = OSSwapInt32(mh->ncmds);
    mh->sizeofcmds = OSSwapInt32(mh->sizeofcmds);
    mh->flags = OSSwapInt32(mh->flags);
}

inline
void
qv_swap_mach_header_64(
struct qv_mach_header_64 *mh,
enum NXByteOrder target_byte_sex)
{
    mh->magic = OSSwapInt32(mh->magic);
    mh->cputype = OSSwapInt32(mh->cputype);
    mh->cpusubtype = OSSwapInt32(mh->cpusubtype);
    mh->filetype = OSSwapInt32(mh->filetype);
    mh->ncmds = OSSwapInt32(mh->ncmds);
    mh->sizeofcmds = OSSwapInt32(mh->sizeofcmds);
    mh->flags = OSSwapInt32(mh->flags);
    mh->reserved = OSSwapInt32(mh->reserved);
}

struct qv_segment_command { /* for 32-bit architectures */
    uint32_t	cmd;		/* LC_SEGMENT */
    uint32_t	cmdsize;	/* includes sizeof section structs */
    char		segname[16];	/* segment name */
    uint32_t	vmaddr;		/* memory address of this segment */
    uint32_t	vmsize;		/* memory size of this segment */
    uint32_t	fileoff;	/* file offset of this segment */
    uint32_t	filesize;	/* amount to map from the file */
    qv_vm_prot_t	maxprot;	/* maximum VM protection */
    qv_vm_prot_t	initprot;	/* initial VM protection */
    uint32_t	nsects;		/* number of sections in segment */
    uint32_t	flags;		/* flags */
};

/*
 * The 64-bit segment load command indicates that a part of this file is to be
 * mapped into a 64-bit task's address space.  If the 64-bit segment has
 * sections then section_64 structures directly follow the 64-bit segment
 * command and their size is reflected in cmdsize.
 */
struct qv_segment_command_64 { /* for 64-bit architectures */
    uint32_t	cmd;		/* LC_SEGMENT_64 */
    uint32_t	cmdsize;	/* includes sizeof section_64 structs */
    char		segname[16];	/* segment name */
    uint64_t	vmaddr;		/* memory address of this segment */
    uint64_t	vmsize;		/* memory size of this segment */
    uint64_t	fileoff;	/* file offset of this segment */
    uint64_t	filesize;	/* amount to map from the file */
    qv_vm_prot_t	maxprot;	/* maximum VM protection */
    qv_vm_prot_t	initprot;	/* initial VM protection */
    uint32_t	nsects;		/* number of sections in segment */
    uint32_t	flags;		/* flags */
};

union qv_lc_str {
    uint32_t	offset;	/* offset to the string */
#ifndef __LP64__
    char		*ptr;	/* pointer to the string */
#endif
};

struct qv_dylib {
    union qv_lc_str  name;			/* library's path name */
    uint32_t timestamp;			/* library's build time stamp */
    uint32_t current_version;		/* library's current version number */
    uint32_t compatibility_version;	/* library's compatibility vers number*/
};
struct qv_dylib_command {
    uint32_t	cmd;		/* LC_ID_DYLIB, LC_LOAD_{,WEAK_}DYLIB,
                       LC_REEXPORT_DYLIB */
    uint32_t	cmdsize;	/* includes pathname string */
    struct qv_dylib	dylib;		/* the library identification */
};


struct qv_dyld_info_command {
   uint32_t   cmd;		/* LC_DYLD_INFO or LC_DYLD_INFO_ONLY */
   uint32_t   cmdsize;		/* sizeof(struct dyld_info_command) */

    /*
     * Dyld rebases an image whenever dyld loads it at an address different
     * from its preferred address.  The rebase information is a stream
     * of byte sized opcodes whose symbolic names start with REBASE_OPCODE_.
     * Conceptually the rebase information is a table of tuples:
     *    <seg-index, seg-offset, type>
     * The opcodes are a compressed way to encode the table by only
     * encoding when a column changes.  In addition simple patterns
     * like "every n'th offset for m times" can be encoded in a few
     * bytes.
     */
    uint32_t   rebase_off;	/* file offset to rebase info  */
    uint32_t   rebase_size;	/* size of rebase info   */

    /*
     * Dyld binds an image during the loading process, if the image
     * requires any pointers to be initialized to symbols in other images.
     * The bind information is a stream of byte sized
     * opcodes whose symbolic names start with BIND_OPCODE_.
     * Conceptually the bind information is a table of tuples:
     *    <seg-index, seg-offset, type, symbol-library-ordinal, symbol-name, addend>
     * The opcodes are a compressed way to encode the table by only
     * encoding when a column changes.  In addition simple patterns
     * like for runs of pointers initialzed to the same value can be
     * encoded in a few bytes.
     */
    uint32_t   bind_off;	/* file offset to binding info   */
    uint32_t   bind_size;	/* size of binding info  */

    /*
     * Some C++ programs require dyld to unique symbols so that all
     * images in the process use the same copy of some code/data.
     * This step is done after binding. The content of the weak_bind
     * info is an opcode stream like the bind_info.  But it is sorted
     * alphabetically by symbol name.  This enable dyld to walk
     * all images with weak binding information in order and look
     * for collisions.  If there are no collisions, dyld does
     * no updating.  That means that some fixups are also encoded
     * in the bind_info.  For instance, all calls to "operator new"
     * are first bound to libstdc++.dylib using the information
     * in bind_info.  Then if some image overrides operator new
     * that is detected when the weak_bind information is processed
     * and the call to operator new is then rebound.
     */
    uint32_t   weak_bind_off;	/* file offset to weak binding info   */
    uint32_t   weak_bind_size;  /* size of weak binding info  */

    /*
     * Some uses of external symbols do not need to be bound immediately.
     * Instead they can be lazily bound on first use.  The lazy_bind
     * are contains a stream of BIND opcodes to bind all lazy symbols.
     * Normal use is that dyld ignores the lazy_bind section when
     * loading an image.  Instead the static linker arranged for the
     * lazy pointer to initially point to a helper function which
     * pushes the offset into the lazy_bind area for the symbol
     * needing to be bound, then jumps to dyld which simply adds
     * the offset to lazy_bind_off to get the information on what
     * to bind.
     */
    uint32_t   lazy_bind_off;	/* file offset to lazy binding info */
    uint32_t   lazy_bind_size;  /* size of lazy binding infs */

    /*
     * The symbols exported by a dylib are encoded in a trie.  This
     * is a compact representation that factors out common prefixes.
     * It also reduces LINKEDIT pages in RAM because it encodes all
     * information (name, address, flags) in one small, contiguous range.
     * The export area is a stream of nodes.  The first node sequentially
     * is the start node for the trie.
     *
     * Nodes for a symbol start with a uleb128 that is the length of
     * the exported symbol information for the string so far.
     * If there is no exported symbol, the node starts with a zero byte.
     * If there is exported info, it follows the length.
     *
     * First is a uleb128 containing flags. Normally, it is followed by
     * a uleb128 encoded offset which is location of the content named
     * by the symbol from the mach_header for the image.  If the flags
     * is EXPORT_SYMBOL_FLAGS_REEXPORT, then following the flags is
     * a uleb128 encoded library ordinal, then a zero terminated
     * UTF8 string.  If the string is zero length, then the symbol
     * is re-export from the specified dylib with the same name.
     * If the flags is EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER, then following
     * the flags is two uleb128s: the stub offset and the resolver offset.
     * The stub is used by non-lazy pointers.  The resolver is used
     * by lazy pointers and must be called to get the actual address to use.
     *
     * After the optional exported symbol information is a byte of
     * how many edges (0-255) that this node has leaving it,
     * followed by each edge.
     * Each edge is a zero terminated UTF8 of the addition chars
     * in the symbol, followed by a uleb128 offset for the node that
     * edge points to.
     *
     */
    uint32_t   export_off;	/* file offset to lazy binding info */
    uint32_t   export_size;	/* size of lazy binding infs */
};


struct qv_nlist {
    union {
#ifndef __LP64__
        char *n_name;	/* for use when in-core */
#endif
        uint32_t n_strx;	/* index into the string table */
    } n_un;
    uint8_t n_type;		/* type flag, see below */
    uint8_t n_sect;		/* section number or NO_SECT */
    int16_t n_desc;		/* see <mach-o/stab.h> */
    uint32_t n_value;	/* value of this symbol (or stab offset) */
};

/*
 * This is the symbol table entry structure for 64-bit architectures.
 */
struct qv_nlist_64 {
    union {
        uint32_t  n_strx; /* index into the string table */
    } n_un;
    uint8_t n_type;        /* type flag, see below */
    uint8_t n_sect;        /* section number or NO_SECT */
    uint16_t n_desc;       /* see <mach-o/stab.h> */
    uint64_t n_value;      /* value of this symbol (or stab offset) */
};
struct qv_symtab_command {
    uint32_t	cmd;		/* LC_SYMTAB */
    uint32_t	cmdsize;	/* sizeof(struct symtab_command) */
    uint32_t	symoff;		/* symbol table offset */
    uint32_t	nsyms;		/* number of symbol table entries */
    uint32_t	stroff;		/* string table offset */
    uint32_t	strsize;	/* string table size in bytes */
};

#define LC_REQ_DYLD 0x80000000

/* Constants for the cmd field of all load commands, the type */
#define	LC_SEGMENT	0x1	/* segment of this file to be mapped */
#define	LC_SYMTAB	0x2	/* link-edit stab symbol table info */
#define	LC_SYMSEG	0x3	/* link-edit gdb symbol table info (obsolete) */
#define	LC_THREAD	0x4	/* thread */
#define	LC_UNIXTHREAD	0x5	/* unix thread (includes a stack) */
#define	LC_LOADFVMLIB	0x6	/* load a specified fixed VM shared library */
#define	LC_IDFVMLIB	0x7	/* fixed VM shared library identification */
#define	LC_IDENT	0x8	/* object identification info (obsolete) */
#define LC_FVMFILE	0x9	/* fixed VM file inclusion (internal use) */
#define LC_PREPAGE      0xa     /* prepage command (internal use) */
#define	LC_DYSYMTAB	0xb	/* dynamic link-edit symbol table info */
#define	LC_LOAD_DYLIB	0xc	/* load a dynamically linked shared library */
#define	LC_ID_DYLIB	0xd	/* dynamically linked shared lib ident */
#define LC_LOAD_DYLINKER 0xe	/* load a dynamic linker */
#define LC_ID_DYLINKER	0xf	/* dynamic linker identification */
#define	LC_PREBOUND_DYLIB 0x10	/* modules prebound for a dynamically */
                /*  linked shared library */
#define	LC_ROUTINES	0x11	/* image routines */
#define	LC_SUB_FRAMEWORK 0x12	/* sub framework */
#define	LC_SUB_UMBRELLA 0x13	/* sub umbrella */
#define	LC_SUB_CLIENT	0x14	/* sub client */
#define	LC_SUB_LIBRARY  0x15	/* sub library */
#define	LC_TWOLEVEL_HINTS 0x16	/* two-level namespace lookup hints */
#define	LC_PREBIND_CKSUM  0x17	/* prebind checksum */

/*
 * load a dynamically linked shared library that is allowed to be missing
 * (all symbols are weak imported).
 */
#define	LC_LOAD_WEAK_DYLIB (0x18 | LC_REQ_DYLD)

#define	LC_SEGMENT_64	0x19	/* 64-bit segment of this file to be
                   mapped */
#define	LC_ROUTINES_64	0x1a	/* 64-bit image routines */
#define LC_UUID		0x1b	/* the uuid */
#define LC_RPATH       (0x1c | LC_REQ_DYLD)    /* runpath additions */
#define LC_CODE_SIGNATURE 0x1d	/* local of code signature */
#define LC_SEGMENT_SPLIT_INFO 0x1e /* local of info to split segments */
#define LC_REEXPORT_DYLIB (0x1f | LC_REQ_DYLD) /* load and re-export dylib */
#define	LC_LAZY_LOAD_DYLIB 0x20	/* delay load of dylib until first use */
#define	LC_ENCRYPTION_INFO 0x21	/* encrypted segment information */
#define	LC_DYLD_INFO 	0x22	/* compressed dyld information */
#define	LC_DYLD_INFO_ONLY (0x22|LC_REQ_DYLD)	/* compressed dyld information only */
#define	LC_LOAD_UPWARD_DYLIB (0x23 | LC_REQ_DYLD) /* load upward dylib */
#define LC_VERSION_MIN_MACOSX 0x24   /* build for MacOSX min OS version */
#define LC_VERSION_MIN_IPHONEOS 0x25 /* build for iPhoneOS min OS version */
#define LC_FUNCTION_STARTS 0x26 /* compressed table of function start addresses */
#define LC_DYLD_ENVIRONMENT 0x27 /* string for dyld to treat
                    like environment variable */
#define LC_MAIN (0x28|LC_REQ_DYLD) /* replacement for LC_UNIXTHREAD */
#define LC_DATA_IN_CODE 0x29 /* table of non-instructions in __text */
#define LC_SOURCE_VERSION 0x2A /* source version used to build binary */
#define LC_DYLIB_CODE_SIGN_DRS 0x2B /* Code signing DRs copied from linked dylibs */
#define	LC_ENCRYPTION_INFO_64 0x2C /* 64-bit encrypted segment information */
#define LC_LINKER_OPTION 0x2D /* linker options in MH_OBJECT files */
#define LC_LINKER_OPTIMIZATION_HINT 0x2E /* optimization hints in MH_OBJECT files */
#define LC_VERSION_MIN_TVOS 0x2F /* build for AppleTV min OS version */
#define LC_VERSION_MIN_WATCHOS 0x30 /* build for Watch min OS version */
#define LC_NOTE 0x31 /* arbitrary data included within a Mach-O file */
#define LC_BUILD_VERSION 0x32 /* build for platform min OS version */

struct qv_dysymtab_command {
    uint32_t cmd;	/* LC_DYSYMTAB */
    uint32_t cmdsize;	/* sizeof(struct qv_dysymtab_command) */

    /*
     * The symbols indicated by symoff and nsyms of the LC_SYMTAB load command
     * are grouped into the following three groups:
     *    local symbols (further grouped by the module they are from)
     *    defined external symbols (further grouped by the module they are from)
     *    undefined symbols
     *
     * The local symbols are used only for debugging.  The dynamic binding
     * process may have to use them to indicate to the debugger the local
     * symbols for a module that is being bound.
     *
     * The last two groups are used by the dynamic binding process to do the
     * binding (indirectly through the module table and the reference symbol
     * table when this is a dynamically linked shared library file).
     */
    uint32_t ilocalsym;	/* index to local symbols */
    uint32_t nlocalsym;	/* number of local symbols */

    uint32_t iextdefsym;/* index to externally defined symbols */
    uint32_t nextdefsym;/* number of externally defined symbols */

    uint32_t iundefsym;	/* index to undefined symbols */
    uint32_t nundefsym;	/* number of undefined symbols */

    /*
     * For the for the dynamic binding process to find which module a symbol
     * is defined in the table of contents is used (analogous to the ranlib
     * structure in an archive) which maps defined external symbols to modules
     * they are defined in.  This exists only in a dynamically linked shared
     * library file.  For executable and object modules the defined external
     * symbols are sorted by name and is use as the table of contents.
     */
    uint32_t tocoff;	/* file offset to table of contents */
    uint32_t ntoc;	/* number of entries in table of contents */

    /*
     * To support dynamic binding of "modules" (whole object files) the symbol
     * table must reflect the modules that the file was created from.  This is
     * done by having a module table that has indexes and counts into the merged
     * tables for each module.  The module structure that these two entries
     * refer to is described below.  This exists only in a dynamically linked
     * shared library file.  For executable and object modules the file only
     * contains one module so everything in the file belongs to the module.
     */
    uint32_t modtaboff;	/* file offset to module table */
    uint32_t nmodtab;	/* number of module table entries */

    /*
     * To support dynamic module binding the module structure for each module
     * indicates the external references (defined and undefined) each module
     * makes.  For each module there is an offset and a count into the
     * reference symbol table for the symbols that the module references.
     * This exists only in a dynamically linked shared library file.  For
     * executable and object modules the defined external symbols and the
     * undefined external symbols indicates the external references.
     */
    uint32_t extrefsymoff;	/* offset to referenced symbol table */
    uint32_t nextrefsyms;	/* number of referenced symbol table entries */

    /*
     * The sections that contain "symbol pointers" and "routine stubs" have
     * indexes and (implied counts based on the size of the section and fixed
     * size of the entry) into the "indirect symbol" table for each pointer
     * and stub.  For every section of these two types the index into the
     * indirect symbol table is stored in the section header in the field
     * reserved1.  An indirect symbol table entry is simply a 32bit index into
     * the symbol table to the symbol that the pointer or stub is referring to.
     * The indirect symbol table is ordered to match the entries in the section.
     */
    uint32_t indirectsymoff; /* file offset to the indirect symbol table */
    uint32_t nindirectsyms;  /* number of indirect symbol table entries */

    /*
     * To support relocating an individual module in a library file quickly the
     * external relocation entries for each module in the library need to be
     * accessed efficiently.  Since the relocation entries can't be accessed
     * through the section headers for a library file they are separated into
     * groups of local and external entries further grouped by module.  In this
     * case the presents of this load command who's extreloff, nextrel,
     * locreloff and nlocrel fields are non-zero indicates that the relocation
     * entries of non-merged sections are not referenced through the section
     * structures (and the reloff and nreloc fields in the section headers are
     * set to zero).
     *
     * Since the relocation entries are not accessed through the section headers
     * this requires the r_address field to be something other than a section
     * offset to identify the item to be relocated.  In this case r_address is
     * set to the offset from the vmaddr of the first LC_SEGMENT command.
     * For MH_SPLIT_SEGS images r_address is set to the the offset from the
     * vmaddr of the first read-write LC_SEGMENT command.
     *
     * The relocation entries are grouped by module and the module table
     * entries have indexes and counts into them for the group of external
     * relocation entries for that the module.
     *
     * For sections that are merged across modules there must not be any
     * remaining external relocation entries for them (for merged sections
     * remaining relocation entries must be local).
     */
    uint32_t extreloff;	/* offset to external relocation entries */
    uint32_t nextrel;	/* number of external relocation entries */

    /*
     * All the local relocation entries are grouped together (they are not
     * grouped by their module since they are only used if the object is moved
     * from it staticly link edited address).
     */
    uint32_t locreloff;	/* offset to local relocation entries */
    uint32_t nlocrel;	/* number of local relocation entries */

};

struct qv_dylinker_command {
    uint32_t	cmd;		/* LC_ID_DYLINKER, LC_LOAD_DYLINKER or
                       LC_DYLD_ENVIRONMENT */
    uint32_t	cmdsize;	/* includes pathname string */
    union qv_lc_str    name;		/* dynamic linker's path name */
};
struct qv_uuid_command {
    uint32_t	cmd;		/* LC_UUID */
    uint32_t	cmdsize;	/* sizeof(struct uuid_command) */
    uint8_t	uuid[16];	/* the 128-bit uuid */
};
struct qv_version_min_command {
    uint32_t	cmd;		/* LC_VERSION_MIN_MACOSX or
                   LC_VERSION_MIN_IPHONEOS or
                   LC_VERSION_MIN_WATCHOS or
                   LC_VERSION_MIN_TVOS */
    uint32_t	cmdsize;	/* sizeof(struct min_version_command) */
    uint32_t	version;	/* X.Y.Z is encoded in nibbles xxxx.yy.zz */
    uint32_t	sdk;		/* X.Y.Z is encoded in nibbles xxxx.yy.zz */
};
struct qv_entry_point_command {
    uint32_t  cmd;	/* LC_MAIN only used in MH_EXECUTE filetypes */
    uint32_t  cmdsize;	/* 24 */
    uint64_t  entryoff;	/* file (__TEXT) offset of main() */
    uint64_t  stacksize;/* if not zero, initial stack size */
};
struct qv_encryption_info_command {
   uint32_t	cmd;		/* LC_ENCRYPTION_INFO */
   uint32_t	cmdsize;	/* sizeof(struct encryption_info_command) */
   uint32_t	cryptoff;	/* file offset of encrypted range */
   uint32_t	cryptsize;	/* file size of encrypted range */
   uint32_t	cryptid;	/* which enryption system,
                   0 means not-encrypted yet */
};

/*
 * The encryption_info_command_64 contains the file offset and size of an
 * of an encrypted segment (for use in x86_64 targets).
 */
struct qv_encryption_info_command_64 {
   uint32_t	cmd;		/* LC_ENCRYPTION_INFO_64 */
   uint32_t	cmdsize;	/* sizeof(struct encryption_info_command_64) */
   uint32_t	cryptoff;	/* file offset of encrypted range */
   uint32_t	cryptsize;	/* file size of encrypted range */
   uint32_t	cryptid;	/* which enryption system,
                   0 means not-encrypted yet */
   uint32_t	pad;		/* padding to make this struct's size a multiple
                   of 8 bytes */
};

struct qv_source_version_command {
    uint32_t  cmd;	/* LC_SOURCE_VERSION */
    uint32_t  cmdsize;	/* 16 */
    uint64_t  version;	/* A.B.C.D.E packed as a24.b10.c10.d10.e10 */
};
struct qv_linkedit_data_command {
    uint32_t	cmd;		/* LC_CODE_SIGNATURE, LC_SEGMENT_SPLIT_INFO,
                                   LC_FUNCTION_STARTS, LC_DATA_IN_CODE,
                   LC_DYLIB_CODE_SIGN_DRS or
                   LC_LINKER_OPTIMIZATION_HINT. */
    uint32_t	cmdsize;	/* sizeof(struct linkedit_data_command) */
    uint32_t	dataoff;	/* file offset of data in __LINKEDIT segment */
    uint32_t	datasize;	/* file size of data in __LINKEDIT segment  */
};
struct qv_data_in_code_entry {
    uint32_t	offset;  /* from mach_header to start of data range*/
    uint16_t	length;  /* number of bytes in data range */
    uint16_t	kind;    /* a DICE_KIND_* value  */
};
#define DICE_KIND_DATA              0x0001
#define DICE_KIND_JUMP_TABLE8       0x0002
#define DICE_KIND_JUMP_TABLE16      0x0003
#define DICE_KIND_JUMP_TABLE32      0x0004
#define DICE_KIND_ABS_JUMP_TABLE32  0x0005



struct qv_twolevel_hints_command {
    uint32_t cmd;	/* LC_TWOLEVEL_HINTS */
    uint32_t cmdsize;	/* sizeof(struct twolevel_hints_command) */
    uint32_t offset;	/* offset to the hint table */
    uint32_t nhints;	/* number of hints in the hint table */
};






/* a table of contents entry */
struct qv_dylib_table_of_contents {
    uint32_t symbol_index;	/* the defined external symbol
                   (index into the symbol table) */
    uint32_t module_index;	/* index into the module table this symbol
                   is defined in */
};

/* a module table entry */
struct qv_dylib_module {
    uint32_t module_name;	/* the module name (index into string table) */

    uint32_t iextdefsym;	/* index into externally defined symbols */
    uint32_t nextdefsym;	/* number of externally defined symbols */
    uint32_t irefsym;		/* index into reference symbol table */
    uint32_t nrefsym;		/* number of reference symbol table entries */
    uint32_t ilocalsym;		/* index into symbols for local symbols */
    uint32_t nlocalsym;		/* number of local symbols */

    uint32_t iextrel;		/* index into external relocation entries */
    uint32_t nextrel;		/* number of external relocation entries */

    uint32_t iinit_iterm;	/* low 16 bits are the index into the init
                   section, high 16 bits are the index into
                       the term section */
    uint32_t ninit_nterm;	/* low 16 bits are the number of init section
                   entries, high 16 bits are the number of
                   term section entries */

    uint32_t			/* for this module address of the start of */
    objc_module_info_addr;  /*  the (__OBJC,__module_info) section */
    uint32_t			/* for this module size of */
    objc_module_info_size;	/*  the (__OBJC,__module_info) section */
};

/* a 64-bit module table entry */
struct qv_dylib_module_64 {
    uint32_t module_name;	/* the module name (index into string table) */

    uint32_t iextdefsym;	/* index into externally defined symbols */
    uint32_t nextdefsym;	/* number of externally defined symbols */
    uint32_t irefsym;		/* index into reference symbol table */
    uint32_t nrefsym;		/* number of reference symbol table entries */
    uint32_t ilocalsym;		/* index into symbols for local symbols */
    uint32_t nlocalsym;		/* number of local symbols */

    uint32_t iextrel;		/* index into external relocation entries */
    uint32_t nextrel;		/* number of external relocation entries */

    uint32_t iinit_iterm;	/* low 16 bits are the index into the init
                   section, high 16 bits are the index into
                   the term section */
    uint32_t ninit_nterm;      /* low 16 bits are the number of init section
                  entries, high 16 bits are the number of
                  term section entries */

    uint32_t			/* for this module size of */
        objc_module_info_size;	/*  the (__OBJC,__module_info) section */
    uint64_t			/* for this module address of the start of */
        objc_module_info_addr;	/*  the (__OBJC,__module_info) section */
};

/*
 * The entries in the reference symbol table are used when loading the module
 * (both by the static and dynamic link editors) and if the module is unloaded
 * or replaced.  Therefore all external symbols (defined and undefined) are
 * listed in the module's reference table.  The flags describe the type of
 * reference that is being made.  The constants for the flags are defined in
 * <mach-o/nlist.h> as they are also used for symbol table entries.
 */
struct qv_dylib_reference {
    uint32_t isym:24,		/* index into the symbol table */
              flags:8;	/* flags to indicate the type of reference */
};


struct qv_relocation_info {
   int32_t	r_address;	/* offset in the section to what is being
                   relocated */
   uint32_t     r_symbolnum:24,	/* symbol index if r_extern == 1 or section
                   ordinal if r_extern == 0 */
        r_pcrel:1, 	/* was relocated pc relative already */
        r_length:2,	/* 0=byte, 1=word, 2=long, 3=quad */
        r_extern:1,	/* does not include value of sym referenced */
        r_type:4;	/* if not 0, machine specific relocation type */
};





/*
 * The flags field of a section structure is separated into two parts a section
 * type and section attributes.  The section types are mutually exclusive (it
 * can only have one type) but the section attributes are not (it may have more
 * than one attribute).
 */
#define SECTION_TYPE		 0x000000ff	/* 256 section types */
#define SECTION_ATTRIBUTES	 0xffffff00	/*  24 section attributes */

/* Constants for the type of a section */
#define	S_REGULAR		0x0	/* regular section */
#define	S_ZEROFILL		0x1	/* zero fill on demand section */
#define	S_CSTRING_LITERALS	0x2	/* section with only literal C strings*/
#define	S_4BYTE_LITERALS	0x3	/* section with only 4 byte literals */
#define	S_8BYTE_LITERALS	0x4	/* section with only 8 byte literals */
#define	S_LITERAL_POINTERS	0x5	/* section with only pointers to */
                    /*  literals */
/*
 * For the two types of symbol pointers sections and the symbol stubs section
 * they have indirect symbol table entries.  For each of the entries in the
 * section the indirect symbol table entries, in corresponding order in the
 * indirect symbol table, start at the index stored in the reserved1 field
 * of the section structure.  Since the indirect symbol table entries
 * correspond to the entries in the section the number of indirect symbol table
 * entries is inferred from the size of the section divided by the size of the
 * entries in the section.  For symbol pointers sections the size of the entries
 * in the section is 4 bytes and for symbol stubs sections the byte size of the
 * stubs is stored in the reserved2 field of the section structure.
 */
#define	S_NON_LAZY_SYMBOL_POINTERS	0x6	/* section with only non-lazy
                           symbol pointers */
#define	S_LAZY_SYMBOL_POINTERS		0x7	/* section with only lazy symbol
                           pointers */
#define	S_SYMBOL_STUBS			0x8	/* section with only symbol
                           stubs, byte size of stub in
                           the reserved2 field */
#define	S_MOD_INIT_FUNC_POINTERS	0x9	/* section with only function
                           pointers for initialization*/
#define	S_MOD_TERM_FUNC_POINTERS	0xa	/* section with only function
                           pointers for termination */
#define	S_COALESCED			0xb	/* section contains symbols that
                           are to be coalesced */
#define	S_GB_ZEROFILL			0xc	/* zero fill on demand section
                           (that can be larger than 4
                           gigabytes) */
#define	S_INTERPOSING			0xd	/* section with only pairs of
                           function pointers for
                           interposing */
#define	S_16BYTE_LITERALS		0xe	/* section with only 16 byte
                           literals */
#define	S_DTRACE_DOF			0xf	/* section contains
                           DTrace Object Format */
#define	S_LAZY_DYLIB_SYMBOL_POINTERS	0x10	/* section with only lazy
                           symbol pointers to lazy
                           loaded dylibs */
/*
 * Section types to support thread local variables
 */
#define S_THREAD_LOCAL_REGULAR                   0x11  /* template of initial
                              values for TLVs */
#define S_THREAD_LOCAL_ZEROFILL                  0x12  /* template of initial
                              values for TLVs */
#define S_THREAD_LOCAL_VARIABLES                 0x13  /* TLV descriptors */
#define S_THREAD_LOCAL_VARIABLE_POINTERS         0x14  /* pointers to TLV
                                                          descriptors */
#define S_THREAD_LOCAL_INIT_FUNCTION_POINTERS    0x15  /* functions to call
                              to initialize TLV
                              values */
#define S_INIT_FUNC_OFFSETS                      0x16  /* 32-bit offsets to
                              initializers */

/*
 * Constants for the section attributes part of the flags field of a section
 * structure.
 */
#define SECTION_ATTRIBUTES_USR	 0xff000000	/* User setable attributes */
#define S_ATTR_PURE_INSTRUCTIONS 0x80000000	/* section contains only true
                           machine instructions */
#define S_ATTR_NO_TOC 		 0x40000000	/* section contains coalesced
                           symbols that are not to be
                           in a ranlib table of
                           contents */
#define S_ATTR_STRIP_STATIC_SYMS 0x20000000	/* ok to strip static symbols
                           in this section in files
                           with the MH_DYLDLINK flag */
#define S_ATTR_NO_DEAD_STRIP	 0x10000000	/* no dead stripping */
#define S_ATTR_LIVE_SUPPORT	 0x08000000	/* blocks are live if they
                           reference live blocks */
#define S_ATTR_SELF_MODIFYING_CODE 0x04000000	/* Used with i386 code stubs
                           written on by dyld */
/*
 * If a segment contains any sections marked with S_ATTR_DEBUG then all
 * sections in that segment must have this attribute.  No section other than
 * a section marked with this attribute may reference the contents of this
 * section.  A section with this attribute may contain no symbols and must have
 * a section type S_REGULAR.  The static linker will not copy section contents
 * from sections with this attribute into its output file.  These sections
 * generally contain DWARF debugging info.
 */
#define	S_ATTR_DEBUG		 0x02000000	/* a debug section */
#define SECTION_ATTRIBUTES_SYS	 0x00ffff00	/* system setable attributes */
#define S_ATTR_SOME_INSTRUCTIONS 0x00000400	/* section contains some
                           machine instructions */
#define S_ATTR_EXT_RELOC	 0x00000200	/* section has external
                           relocation entries */
#define S_ATTR_LOC_RELOC	 0x00000100	/* section has local
                           relocation entries */


#endif // QVMACHO_H
