#ifndef QURT_TYPES_H
#define QURT_TYPES_H
/**
  @file qurt_types.h 
  @brief  Contains types common to all configurations

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013, 2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



/*=============================================================================
												TYPEDEFS
=============================================================================*/
//#include <stddef.h>
#include <qurt_consts.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=====================================================================
 typedefs
======================================================================*/
/** @addtogroup memory_management_types
@{ */
typedef unsigned int qurt_addr_t;          /**< QuRT address type.*/
typedef unsigned int qurt_paddr_t;         /**< QuRT physical memory address type.  */ 
/** @cond rest_reg_dist  */
typedef unsigned long long qurt_paddr_64_t; /**< QuRT 64-bit physical memory address type. */
typedef unsigned int qurt_mem_region_t;    /**< QuRT memory regions type. */
typedef unsigned int qurt_mem_fs_region_t; /**< QuRT memory FS region type. */
/**@endcond */
typedef unsigned int qurt_mem_pool_t;      /**< QuRT memory pool type.*/
typedef unsigned int qurt_size_t;          /**< QuRT size type. */
/** @cond  */
typedef unsigned long long qurt_mmu_entry_t;/**< QuRT MMU entry type. */


/*
 * Mapping type
 *
 * QMEM_MAPPING_VIRTUAL is the default mode, in which the system 
 * picks up the available range of the virtual address, and maps it to 
 * available contiguous physical addresses. Physical-to-virtual
 * is not guaranteed to be 1:1; both virtual and physical memory is 
 * contiguous.
 *
 * In QMEM_MAPPING_IDEMPOTENT mode, the user provides the physical address;
 * the kernel allocates 1:1 physical-to-virtual memory. Primary usage of 
 * of this mapping is to allocate physical-to-virtual memory 1:1.
 *
 * In QMEM_MAPPING_PHYS_CONTIGUOUS mode, the virtual address might
 * not be the same as the physical address. But the physical address of the
 * memory region is guaranteed to be contiguous starting at the provided
 * address, it is REQUIRED to provide a fixed physical address. The primary 
 * use of this mapping is to allocate physical memory from a particular 
 * address, where 1:1 physical-to-virtual is not required.
 *
 * QMEM_MAPPING_NONE mode must be used to reserve a virtual memory
 * Area (VMA); no physical memory is reserved or mapped to this virtual
 * space; all standard qmem_region APIs apply to a VMA, however physical
 * address is always INVALID_ADDR. qmem_region_create() in this mode
 * returns a handle to the VMA, both virt_addr and phys_addr must
 * be set to INVALID_ADDR, kernel allocates any available virtual
 * memory of the specified size. Obtain the starting virtual address 
 * of VMA through qmem_region_attr_getvirtaddr().
 * Primary purpose of this mapping mode is to provide a mechanism for
 * delayed binding in QuRT, for example reserve virtual memory and map it at
 * some later time to possibly discontiguous physical blocks. Thus, a
 * single VMA can be partitioned among several physical-virtual mappings
 * created via qmem_region_create() with QMEM_VIRTUAL_FIXED mapping mode.
 * Each VMA keeps track of associated mapped regions.
 * Deletion of VMA succeeds only if all associated "virtual_fixed"
 * regions are freed prior to VMA deletion.
 *
 * Use QMEM_MAPPING_VIRTUAL_FIXED mode to create a region
 * from virtual space that has been reserved via qmem_region_create()
 * with QMEM_MAPPING_NONE mapping. A valid virt_add is required, if
 * phys_addr is specified, the kernel attempts to map it accordingly,
 * if no phys_addr is specified, kernel maps any available physical
 * memory. All standard qmem_region APIs apply to such region. Remapping
 * a virtual range without prior freeing of the region is not permitted.
 * When such region is deleted its corresponding VMA remains intact.
 *
 * QMEM_MAPPING_PHYS_DISCONTIGUOUS mode can obtain contiguous
 * virtual memory but physical memory can be discontiguous. This method
 * tries to club various small physical memory blocks to obtain requested
 * memory and is useful in case where there is no contiguous full block
 * of requested size. If client does not need contiguous physical memory, 
 * (for example, if client does not use physical addressing), this helps
 * use smaller physical memory blocks rather than using contiguous memory.
 * Note: When memory is allocated through this method, physical address is
 * not returned to the caller using the qurt_mem_region_attr_get() API as there might
 * not be a single physical address.
 *
 */
/**@endcond */
/** QuRT memory region mapping type. */
typedef enum {
        QURT_MEM_MAPPING_VIRTUAL=0,  /**< Default mode. The region virtual address range is mapped to an 
                                          available contiguous area of physical memory. The QuRT system 
										  chooses the base address in physical memory. This makes the
										  most efficient use of virtual memory, and works for most memory 
										  use cases.*/
        QURT_MEM_MAPPING_PHYS_CONTIGUOUS = 1,  /**< The region virtual address space must be mapped to a 
											   contiguous area of physical memory. This is necessary when the
                                               memory region is accessed by external devices that bypass Hexagon
                                               virtual memory addressing. The base address in physical 
											   memory must be explicitly specified.*/
        QURT_MEM_MAPPING_IDEMPOTENT=2,  /**< The region virtual address space is mapped
                                             to the identical area of physical memory. */ 		                                   
        QURT_MEM_MAPPING_VIRTUAL_FIXED=3,  /**< The region virtual address space maps either to the 
										   specified area of physical memory or (if no area is specified)
                                           to any available physical memory. Use this mapping to create
										   regions from virtual space that was reserved by calling 
										   qurt_mem_region_create() with mapping. */										   
        QURT_MEM_MAPPING_NONE=4,  /**< Reserves a virtual memory area (VMA). Remapping a virtual range is not
                                       permitted without first deleting the memory region. When such a region is
                                       deleted, its corresponding virtual memory addressing remains intact. */
        QURT_MEM_MAPPING_VIRTUAL_RANDOM=7,  /**< The system chooses a random virtual address and
                                            maps it to available contiguous physical addresses.*/
        QURT_MEM_MAPPING_PHYS_DISCONTIGUOUS=8, /**< Allocates in discontiguous physical memory blocks
                                                    while virtual memory is contiguous. This helps when 
                                                    there are smaller contiguous blocks than requested size.
                                                    Physical address is not provided as part of get_attr call */
        QURT_MEM_MAPPING_INVALID=10,        /**< Reserved as an invalid mapping type. */
} qurt_mem_mapping_t;  


/** QuRT cache mode type. */
typedef enum {
        QURT_MEM_CACHE_WRITEBACK=7,     /**< Write back. */
        QURT_MEM_CACHE_NONE_SHARED=6,   /**< Normal uncached memory that can be shared with other subsystems.*/
        QURT_MEM_CACHE_WRITETHROUGH=5,  /**< Write through. */
        QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE=0,    /**< Write back non-L2-cacheable.*/
        QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE=1,  /**< Write through non-L2-cacheable. */
        QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE=QURT_MEM_CACHE_WRITEBACK,  /**< Write back L2 cacheable. */
        QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE=QURT_MEM_CACHE_WRITETHROUGH,  /**< Write through L2 cacheable.  */
        QURT_MEM_CACHE_DEVICE = 4,  /**< Volatile memory-mapped device. Access to device memory cannot be cancelled by interrupts, re-ordered, or replayed.*/
        QURT_MEM_CACHE_NONE = 4,  /**< Deprecated -- use #QURT_MEM_CACHE_DEVICE instead. */
        QURT_MEM_CACHE_DEVICE_SFC = 2, /**< Enables placing limitations on the number of outstanding transactions. */
        QURT_MEM_CACHE_INVALID=10,  /**< Reserved as an invalid cache type. */
} qurt_mem_cache_mode_t;

/** Memory access permission. */
typedef enum {
        QURT_PERM_READ=0x1, /**< Read permission. */
        QURT_PERM_WRITE=0x2,  /**< Write permission. */
        QURT_PERM_EXECUTE=0x4,  /**< Execution permission. */
        QURT_PERM_NODUMP=0x8,  /**<  Skip dumping the mapping. During process domain dump, must skip
                                     some mappings on host memory to avoid a race condition
                                     where the memory is removed from the host and DSP process
                                     crashed before the mapping is removed. */
        QURT_PERM_FULL=QURT_PERM_READ|QURT_PERM_WRITE|QURT_PERM_EXECUTE,  /**< Read, write, and execute permission. */
} qurt_perm_t;

/** @cond rest_reg_dist*/
/** QuRT cache type; specifies data cache or instruction cache. */
typedef enum {
        QURT_MEM_ICACHE, /**< Instruction cache.*/
        QURT_MEM_DCACHE  /**< Data cache.*/
} qurt_mem_cache_type_t;

/** QuRT cache operation code type. */
typedef enum {
    QURT_MEM_CACHE_FLUSH, /**< Flush. */
    QURT_MEM_CACHE_INVALIDATE, /**< Invalidate */
    QURT_MEM_CACHE_FLUSH_INVALIDATE, /**< Flush invalidate. */
    QURT_MEM_CACHE_FLUSH_ALL, /**< Flush all. */
    QURT_MEM_CACHE_FLUSH_INVALIDATE_ALL, /**< Flush invalidate all. */
    QURT_MEM_CACHE_TABLE_FLUSH_INVALIDATE, /**< Table flush invalidate. */
    QURT_MEM_CACHE_FLUSH_INVALIDATE_L2, /**< */
} qurt_mem_cache_op_t;

/** QuRT memory region type. */
typedef enum {
        QURT_MEM_REGION_LOCAL=0,  /**< Local. */
        QURT_MEM_REGION_SHARED=1,  /**< Shared.*/
        QURT_MEM_REGION_USER_ACCESS=2,  /**< User access. */
        QURT_MEM_REGION_FS=4,  /**< FS. */
        QURT_MEM_REGION_INVALID=10,  /**< Reserved as an invalid region type. */
} qurt_mem_region_type_t;

/* Cache and bus attributes are combined into a value of this type for convenience,
    and macros for combining and extracting fields are defined here.  */
/** @cond */
struct qurt_pgattr {
   unsigned pga_value; /**< PGA value.*/
};
typedef struct qurt_pgattr qurt_pgattr_t;
#define PGA_BITFIELD_MASK(hi,lo)    (((~0u)>>(31-((hi)-(lo))))<<(lo))
#define PGA_BITFIELD_GET(x,hi,lo)   (((x)&PGA_BITFIELD_MASK(hi,lo))>>(lo))
#define PGA_BITFIELD_INS(hi,lo,v)   (((v)<<(lo))&PGA_BITFIELD_MASK(hi,lo))
#define PGA_BITFIELD_SET(x,hi,lo,v) ((x)=((x)&~PGA_BITFIELD_MASK(hi,lo))|PGA_BITFIELD_INS(hi,lo,v))
#define QURT_PGATTR_C_GET(pga)      PGA_BITFIELD_GET((pga).pga_value, 3, 0)     /* Bits 3-0:  cache */
#define QURT_PGATTR_A_GET(pga)      PGA_BITFIELD_GET((pga).pga_value, 5, 4)     /* Bits 5-4:  bus attr */
#define QURT_PGATTR_C_SET(pga,v)    PGA_BITFIELD_SET((pga).pga_value, 3, 0, v)  /* Bits 3-0:  cache */
#define QURT_PGATTR_A_SET(pga,v)    PGA_BITFIELD_SET((pga).pga_value, 5, 4, v)  /* Bits 5-4:  bus attr */
#define QURT_PGATTR_MKRAW(v)        ((qurt_pgattr_t){.pga_value = (v)})
#define QURT_PGATTR_MK(c,a)         QURT_PGATTR_MKRAW(PGA_BITFIELD_INS(3,0,c)|PGA_BITFIELD_INS(5,4,a))
/** @endcond */
/** QuRT memory region attributes type.*/  
/* QMEM_MAPPING_IDEMPOTENT and QMEM_MAPPING_PHYS_CONTIGUOUS mode can specify physaddr.
   virtaddr cannot be specified for a memory region, it can only be queried by the 
   qmem_attr_getvirtaddr() function.
 */
typedef struct {
    /** @cond */
    qurt_mem_mapping_t    mapping_type; 
    unsigned char          perms;
    unsigned short         owner;
    qurt_pgattr_t          pga;
    unsigned               ppn; //physical page number (physical>>12)
    qurt_addr_t            virtaddr;
    qurt_mem_region_type_t   type;   
    qurt_size_t               size;
    /** @endcond */
} qurt_mem_region_attr_t;


/** QuRT user physical memory pool type. */
typedef struct {
    /** @cond */
    char name[32];
    struct ranges{
        unsigned int start;
        unsigned int size;
    } ranges[MAX_POOL_RANGES];
     /** @endcond */
} qurt_mem_pool_attr_t;

typedef enum {
    HEXAGON_L1_I_CACHE = 0,     /**< Hexagon L1 instruction cache. */
    HEXAGON_L1_D_CACHE = 1,     /**< Hexagon L1 data cache. */
    HEXAGON_L2_CACHE = 2        /**< Hexagon L2 cache. */
} qurt_cache_type_t;

typedef enum {
    FULL_SIZE = 0,                /**< Fully shared cache, without partitioning. */
    HALF_SIZE = 1,                /**< 1/2 for main, 1/2 for auxiliary. */
    THREE_QUARTER_SIZE = 2,       /**< 3/4 for main, 1/4 for auxiliary. */
    SEVEN_EIGHTHS_SIZE = 3        /**< 7/8 for main, 1/8 for auxiliary; for L2 cache only. */
} qurt_cache_partition_size_t;

/** QuRT MMU entry extraction macros on ent: tlbhi_tlblo */
#define PAGE_SHIFT_AMT              12
#define QURT_MMU_EXTRACT_W(ent)     ((unsigned long long)ent & (1<<30))
#define QURT_MMU_EXTRACT_PERM(ent)  (((unsigned long long)ent >> 29) & 0x7)
#define QURT_MMU_EXTRACT_CCCC(ent)  (((unsigned long long)ent >> 24) & 0xF)
#define QURT_MMU_EXTRACT_VPAGE(ent) (((unsigned long long)ent >> 32) & 0xFFFFF)
#define QURT_MMU_EXTRACT_PPAGE(ent) ((((unsigned long long)ent >>  1) & 0x7FFFFF) \
                                     | (((unsigned long long)ent >> 38) & 0x800000))

static inline unsigned qurt_mmu_entry_get_vaddr (qurt_mmu_entry_t entry)
{
    return (QURT_MMU_EXTRACT_VPAGE(entry) << PAGE_SHIFT_AMT);
}

static inline unsigned qurt_mmu_entry_is_writeable (qurt_mmu_entry_t entry)
{
    return QURT_MMU_EXTRACT_W(entry) != 0;
}

static inline unsigned qurt_mmu_entry_is_device_type (qurt_mmu_entry_t entry)
{
    return ( (QURT_MMU_EXTRACT_CCCC(entry) == QURT_MEM_CACHE_DEVICE) || 
             (QURT_MMU_EXTRACT_CCCC(entry) == QURT_MEM_CACHE_DEVICE_SFC) );
}

static inline unsigned qurt_mmu_entry_get_size (unsigned long long ent)
{
    unsigned tmp;

    tmp = ent & 0x7F;
    return (((tmp & -tmp) * (tmp & -tmp)) << PAGE_SHIFT_AMT); 
}

static inline int qurt_mmu_entry_cmp_by_vaddr (const void *a, const void *b)      // Can pass to sorting routines as compare function
{
   return ( (int)QURT_MMU_EXTRACT_VPAGE(*(const qurt_mmu_entry_t *)a) -
            (int)QURT_MMU_EXTRACT_VPAGE(*(const qurt_mmu_entry_t *)b)   );
}

static inline int qurt_mmu_entry_cmp_by_paddr (const void *a, const void *b)      // Can pass to sorting routines as compare function
{
   return ( (int)QURT_MMU_EXTRACT_PPAGE(*(const qurt_mmu_entry_t *)a) -
            (int)QURT_MMU_EXTRACT_PPAGE(*(const qurt_mmu_entry_t *)b)   );
}
/**@endcond*/

/** @} */ /* end_addtogroup memory_management_types */
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_TYPES_H */
