#define	CPU_ARCH_MASK		0xff000000	/* mask for architecture bits */
#define CPU_ARCH_ABI64		0x01000000	/* 64 bit ABI */
#define CPU_ARCH_ABI64_32	0x02000000	/* ABI for 64-bit hardware with 32-bit types; LP32 */

#define CPU_TYPE_ANY		((qv_cpu_type_t) -1)

#define CPU_TYPE_VAX		((qv_cpu_type_t) 1)
/* skip				((qv_cpu_type_t) 2)	*/
/* skip				((qv_cpu_type_t) 3)	*/
/* skip				((qv_cpu_type_t) 4)	*/
/* skip				((qv_cpu_type_t) 5)	*/
#define	CPU_TYPE_MC680x0	((qv_cpu_type_t) 6)
#define CPU_TYPE_X86		((qv_cpu_type_t) 7)
#define CPU_TYPE_I386		CPU_TYPE_X86		/* compatibility */
#define	CPU_TYPE_X86_64		(CPU_TYPE_X86 | CPU_ARCH_ABI64)

/* skip CPU_TYPE_MIPS		((qv_cpu_type_t) 8)	*/
/* skip 			((qv_cpu_type_t) 9)	*/
#define CPU_TYPE_MC98000	((qv_cpu_type_t) 10)
#define CPU_TYPE_HPPA           ((qv_cpu_type_t) 11)
#define CPU_TYPE_ARM		((qv_cpu_type_t) 12)
#define CPU_TYPE_ARM64		(CPU_TYPE_ARM | CPU_ARCH_ABI64)
#define CPU_TYPE_ARM64_32	(CPU_TYPE_ARM | CPU_ARCH_ABI64_32)
#define CPU_TYPE_MC88000	((qv_cpu_type_t) 13)
#define CPU_TYPE_SPARC		((qv_cpu_type_t) 14)
#define CPU_TYPE_I860		((qv_cpu_type_t) 15)
/* skip	CPU_TYPE_ALPHA		((qv_cpu_type_t) 16)	*/
/* skip				((qv_cpu_type_t) 17)	*/
#define CPU_TYPE_POWERPC		((qv_cpu_type_t) 18)
#define CPU_TYPE_POWERPC64		(CPU_TYPE_POWERPC | CPU_ARCH_ABI64)

/*
 *	Machine subtypes (these are defined here, instead of in a machine
 *	dependent directory, so that any program can get all definitions
 *	regardless of where is it compiled).
 */

/*
 * Capability bits used in the definition of cpu_subtype.
 */
#define CPU_SUBTYPE_MASK	0xff000000	/* mask for feature flags */
#define CPU_SUBTYPE_LIB64	0x80000000	/* 64 bit libraries */


/*
 *	Object files that are hand-crafted to run on any
 *	implementation of an architecture are tagged with
 *	CPU_SUBTYPE_MULTIPLE.  This functions essentially the same as
 *	the "ALL" subtype of an architecture except that it allows us
 *	to easily find object files that may need to be modified
 *	whenever a new implementation of an architecture comes out.
 *
 *	It is the responsibility of the implementor to make sure the
 *	software handles unsupported implementations elegantly.
 */
#define	CPU_SUBTYPE_MULTIPLE		((qv_cpu_subtype_t) -1)
#define CPU_SUBTYPE_LITTLE_ENDIAN	((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_BIG_ENDIAN		((qv_cpu_subtype_t) 1)

/*
 *     Machine threadtypes.
 *     This is none - not defined - for most machine types/subtypes.
 */
#define CPU_THREADTYPE_NONE		((cpu_threadtype_t) 0)

/*
 *	VAX subtypes (these do *not* necessary conform to the actual cpu
 *	ID assigned by DEC available via the SID register).
 */

#define	CPU_SUBTYPE_VAX_ALL	((qv_cpu_subtype_t) 0) 
#define CPU_SUBTYPE_VAX780	((qv_cpu_subtype_t) 1)
#define CPU_SUBTYPE_VAX785	((qv_cpu_subtype_t) 2)
#define CPU_SUBTYPE_VAX750	((qv_cpu_subtype_t) 3)
#define CPU_SUBTYPE_VAX730	((qv_cpu_subtype_t) 4)
#define CPU_SUBTYPE_UVAXI	((qv_cpu_subtype_t) 5)
#define CPU_SUBTYPE_UVAXII	((qv_cpu_subtype_t) 6)
#define CPU_SUBTYPE_VAX8200	((qv_cpu_subtype_t) 7)
#define CPU_SUBTYPE_VAX8500	((qv_cpu_subtype_t) 8)
#define CPU_SUBTYPE_VAX8600	((qv_cpu_subtype_t) 9)
#define CPU_SUBTYPE_VAX8650	((qv_cpu_subtype_t) 10)
#define CPU_SUBTYPE_VAX8800	((qv_cpu_subtype_tt) 11)
#define CPU_SUBTYPE_UVAXIII	((qv_cpu_subtype_t) 12)

/*
 * 	680x0 subtypes
 *
 * The subtype definitions here are unusual for historical reasons.
 * NeXT used to consider 68030 code as generic 68000 code.  For
 * backwards compatability:
 * 
 *	CPU_SUBTYPE_MC68030 symbol has been preserved for source code
 *	compatability.
 *
 *	CPU_SUBTYPE_MC680x0_ALL has been defined to be the same
 *	subtype as CPU_SUBTYPE_MC68030 for binary comatability.
 *
 *	CPU_SUBTYPE_MC68030_ONLY has been added to allow new object
 *	files to be tagged as containing 68030-specific instructions.
 */

#define	CPU_SUBTYPE_MC680x0_ALL		((qv_cpu_subtype_t) 1)
#define CPU_SUBTYPE_MC68030		((qv_cpu_subtype_t) 1) /* compat */
#define CPU_SUBTYPE_MC68040		((qv_cpu_subtype_t) 2) 
#define	CPU_SUBTYPE_MC68030_ONLY	((qv_cpu_subtype_t) 3)

/*
 *	I386 subtypes
 */

#define CPU_SUBTYPE_INTEL(f, m)	((qv_cpu_subtype_t) (f) + ((m) << 4))

#define	CPU_SUBTYPE_I386_ALL			CPU_SUBTYPE_INTEL(3, 0)
#define CPU_SUBTYPE_386					CPU_SUBTYPE_INTEL(3, 0)
#define CPU_SUBTYPE_486					CPU_SUBTYPE_INTEL(4, 0)
#define CPU_SUBTYPE_486SX				CPU_SUBTYPE_INTEL(4, 8)	// 8 << 4 = 128
#define CPU_SUBTYPE_586					CPU_SUBTYPE_INTEL(5, 0)
#define CPU_SUBTYPE_PENT	CPU_SUBTYPE_INTEL(5, 0)
#define CPU_SUBTYPE_PENTPRO	CPU_SUBTYPE_INTEL(6, 1)
#define CPU_SUBTYPE_PENTII_M3	CPU_SUBTYPE_INTEL(6, 3)
#define CPU_SUBTYPE_PENTII_M5	CPU_SUBTYPE_INTEL(6, 5)
#define CPU_SUBTYPE_CELERON				CPU_SUBTYPE_INTEL(7, 6)
#define CPU_SUBTYPE_CELERON_MOBILE		CPU_SUBTYPE_INTEL(7, 7)
#define CPU_SUBTYPE_PENTIUM_3			CPU_SUBTYPE_INTEL(8, 0)
#define CPU_SUBTYPE_PENTIUM_3_M			CPU_SUBTYPE_INTEL(8, 1)
#define CPU_SUBTYPE_PENTIUM_3_XEON		CPU_SUBTYPE_INTEL(8, 2)
#define CPU_SUBTYPE_PENTIUM_M			CPU_SUBTYPE_INTEL(9, 0)
#define CPU_SUBTYPE_PENTIUM_4			CPU_SUBTYPE_INTEL(10, 0)
#define CPU_SUBTYPE_PENTIUM_4_M			CPU_SUBTYPE_INTEL(10, 1)
#define CPU_SUBTYPE_ITANIUM				CPU_SUBTYPE_INTEL(11, 0)
#define CPU_SUBTYPE_ITANIUM_2			CPU_SUBTYPE_INTEL(11, 1)
#define CPU_SUBTYPE_XEON				CPU_SUBTYPE_INTEL(12, 0)
#define CPU_SUBTYPE_XEON_MP				CPU_SUBTYPE_INTEL(12, 1)

#define CPU_SUBTYPE_INTEL_FAMILY(x)	((x) & 15)
#define CPU_SUBTYPE_INTEL_FAMILY_MAX	15

#define CPU_SUBTYPE_INTEL_MODEL(x)	((x) >> 4)
#define CPU_SUBTYPE_INTEL_MODEL_ALL	0

/*
 *	X86 subtypes.
 */

#define CPU_SUBTYPE_X86_ALL		((qv_cpu_subtype_t)3)
#define CPU_SUBTYPE_X86_64_ALL		((qv_cpu_subtype_t)3)
#define CPU_SUBTYPE_X86_ARCH1		((qv_cpu_subtype_t)4)
#define CPU_SUBTYPE_X86_64_H		((qv_cpu_subtype_t)8)	/* Haswell feature subset */


#define CPU_THREADTYPE_INTEL_HTT	((cpu_threadtype_t) 1)

/*
 *	Mips subtypes.
 */

#define	CPU_SUBTYPE_MIPS_ALL	((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_MIPS_R2300	((qv_cpu_subtype_t) 1)
#define CPU_SUBTYPE_MIPS_R2600	((qv_cpu_subtype_t) 2)
#define CPU_SUBTYPE_MIPS_R2800	((qv_cpu_subtype_t) 3)
#define CPU_SUBTYPE_MIPS_R2000a	((qv_cpu_subtype_t) 4)	/* pmax */
#define CPU_SUBTYPE_MIPS_R2000	((qv_cpu_subtype_t) 5)
#define CPU_SUBTYPE_MIPS_R3000a	((qv_cpu_subtype_t) 6)	/* 3max */
#define CPU_SUBTYPE_MIPS_R3000	((qv_cpu_subtype_t) 7)

/*
 *	MC98000 (PowerPC) subtypes
 */
#define	CPU_SUBTYPE_MC98000_ALL	((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_MC98601	((qv_cpu_subtype_t) 1)

/*
 *	HPPA subtypes for Hewlett-Packard HP-PA family of
 *	risc processors. Port by NeXT to 700 series. 
 */

#define	CPU_SUBTYPE_HPPA_ALL		((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_HPPA_7100		((qv_cpu_subtype_t) 0) /* compat */
#define CPU_SUBTYPE_HPPA_7100LC		((qv_cpu_subtype_t) 1)

/*
 *	MC88000 subtypes.
 */
#define	CPU_SUBTYPE_MC88000_ALL	((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_MC88100	((qv_cpu_subtype_t) 1)
#define CPU_SUBTYPE_MC88110	((qv_cpu_subtype_t) 2)

/*
 *	SPARC subtypes
 */
#define	CPU_SUBTYPE_SPARC_ALL		((qv_cpu_subtype_t) 0)

/*
 *	I860 subtypes
 */
#define CPU_SUBTYPE_I860_ALL	((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_I860_860	((qv_cpu_subtype_t) 1)

/*
 *	PowerPC subtypes
 */
#define CPU_SUBTYPE_POWERPC_ALL		((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_POWERPC_601		((qv_cpu_subtype_t) 1)
#define CPU_SUBTYPE_POWERPC_602		((qv_cpu_subtype_t) 2)
#define CPU_SUBTYPE_POWERPC_603		((qv_cpu_subtype_t) 3)
#define CPU_SUBTYPE_POWERPC_603e	((qv_cpu_subtype_t) 4)
#define CPU_SUBTYPE_POWERPC_603ev	((qv_cpu_subtype_t) 5)
#define CPU_SUBTYPE_POWERPC_604		((qv_cpu_subtype_t) 6)
#define CPU_SUBTYPE_POWERPC_604e	((qv_cpu_subtype_t) 7)
#define CPU_SUBTYPE_POWERPC_620		((qv_cpu_subtype_t) 8)
#define CPU_SUBTYPE_POWERPC_750		((qv_cpu_subtype_t) 9)
#define CPU_SUBTYPE_POWERPC_7400	((qv_cpu_subtype_t) 10)
#define CPU_SUBTYPE_POWERPC_7450	((qv_cpu_subtype_t) 11)
#define CPU_SUBTYPE_POWERPC_970		((qv_cpu_subtype_t) 100)

/*
 *	ARM subtypes
 */
#define CPU_SUBTYPE_ARM_ALL             ((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_ARM_V4T             ((qv_cpu_subtype_t) 5)
#define CPU_SUBTYPE_ARM_V6              ((qv_cpu_subtype_t) 6)
#define CPU_SUBTYPE_ARM_V5TEJ           ((qv_cpu_subtype_t) 7)
#define CPU_SUBTYPE_ARM_XSCALE		((qv_cpu_subtype_t) 8)
#define CPU_SUBTYPE_ARM_V7		((qv_cpu_subtype_t) 9)
#define CPU_SUBTYPE_ARM_V7F		((qv_cpu_subtype_t) 10) /* Cortex A9 */
#define CPU_SUBTYPE_ARM_V7S		((qv_cpu_subtype_t) 11) /* Swift */
#define CPU_SUBTYPE_ARM_V7K		((qv_cpu_subtype_t) 12)
#define CPU_SUBTYPE_ARM_V6M		((qv_cpu_subtype_t) 14) /* Not meant to be run under xnu */
#define CPU_SUBTYPE_ARM_V7M		((qv_cpu_subtype_t) 15) /* Not meant to be run under xnu */
#define CPU_SUBTYPE_ARM_V7EM		((qv_cpu_subtype_t) 16) /* Not meant to be run under xnu */

#define CPU_SUBTYPE_ARM_V8		((qv_cpu_subtype_t) 13)

/*
 *  ARM64 subtypes
 */
#define CPU_SUBTYPE_ARM64_ALL           ((qv_cpu_subtype_t) 0)
#define CPU_SUBTYPE_ARM64_V8            ((qv_cpu_subtype_t) 1)
#define CPU_SUBTYPE_ARM64E              ((qv_cpu_subtype_t) 2)