#ifndef QURT_MMAP_H
#define QURT_MMAP_H
/**
  @file qurt_mmap.h 
  @brief  Prototypes of memory mapping/unmapping APIs.
          The APIs allow user to map, un-map and change permissions
          on memory regions. 

  EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2018-2021  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/**@ingroup func_qurt_mem_mmap
  Creates a memory mapping with the specified attributes. \n

  @note1hang If the specified attributes are not valid an error result is returned.\n
  @note2hang This API allows root process caller to create mapping on behalf of a user
             process. If client_handle belongs to a valid user process, the resulting
             mapping will be created for the process.
             If '-1' is passed in place of client_handle, the API will create mapping
             for the caller's underlying process.
  @note3hang 'pool' is an optional argument that allows user to specify a pool handle
              if user wants memory to be allocated from a specific pool.
              Default value for this argument is 'NULL'.
  @note4hang 'pRegion' is unused argument and default value is 'NULL'.
                
  @param[out]  client_handle  client handle to be used for this mapping. (optional)
  @param[in]   pool           Pool handle. (optional)
  @param[in]   pRegion        map region. (unused)
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  @param[in]   prot           Mapping access permissions. (R/W/X)
  @param[in]   flags          Mapping modes.
  @param[in]   fd             File designator.
  @param[in]   offset         Offset in file.
 
  @return
  Valid virtual address -- API returns a valid virtual address on success.
  QURT_MAP_FAILED -- Mapping creation failed. \n
 */
void *qurt_mem_mmap(int client_handle,
                    qurt_mem_pool_t pool,
                    qurt_mem_region_t *pRegion,
                    void *addr,
                    size_t length,
                    int prot,
                    int flags,
                    int fd,
                    unsigned long long offset);

/**@ingroup func_qurt_mem_mmap2
  Creates a memory mapping with the specified attributes. Returns a more descriptive 
  error code in case of failure. \n

  @note1hang If the specified attributes are not valid an error result is returned.\n
  @note2hang This API allows root process caller to create mapping on behalf of a user
             process. If client_handle belongs to a valid user process, the resulting
             mapping will be created for the process.
             If '-1' is passed in place of client_handle, the API will create mapping
             for the caller's underlying process.
  @note3hang 'pool' is an optional argument that allows user to specify a pool handle
              if user wants memory to be allocated from a specific pool.
              Default value for this argument is 'NULL'.
  @note4hang 'pRegion' is unused argument and default value is 'NULL'.
                
  @param[out]  client_handle  client handle to be used for this mapping. (optional)
  @param[in]   pool           Pool handle. (optional)
  @param[in]   pRegion        map region. (unused)
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  @param[in]   prot           Mapping access permissions.(R/W/X)
                              Cache attributes, Bus attributes, User mode
  @param[in]   flags          Mapping modes.
                              Shared , Private, Anonymous etc.
  @param[in]   fd             File designator.
  @param[in]   offset         Offset in file.
 
  @return
  Valid virtual address -- API returns a valid virtual address on success.
  QURT_EMEM -- physical address not available
  QURT_EFAILED -- either VA is not available or mapping failed.
  QURT_EINVALID -- If any of the arguments passed are wrong (e.g. unaligned VA/PA)
 */
void *qurt_mem_mmap2(int client_handle,
                    qurt_mem_pool_t pool,
                    qurt_mem_region_t *pRegion,
                    void *addr,
                    size_t length,
                    int prot,
                    int flags,
                    int fd,
                    unsigned long long offset);

/**@ingroup func_qurt_mem_mmap_by_name
  Creates a memory mapping for a named-memsection using the specified attributes.
  Named-memsection should be specified in cust_config.xml\n

  @note1hang If the specified attributes are not valid or the named memsection is not found
  an error result is returned.\n
                  
  @param[in]   name           Name of the memsection in cust_config.xml, that specifies 
                              this mapping. Should be less than 25 chars.
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  @param[in]   prot           Mapping access permissions.(R/W/X)
                              Cache attributes, Bus attributes, User mode
  @param[in]   flags          Mapping modes.
                              Shared , Private, Anonymous etc.
  @param[in]   offset         Offset relative to physical address range specified in memsection. 
                              If offset + length exceeds size of memsection, failure will be 
                              returned.
  @return
  Valid virtual address -- API returns a valid virtual address on success.
  QURT_MAP_FAILED -- Mapping creation failed. \n
 */
void *qurt_mem_mmap_by_name(const char* name,
                            void *addr,
                            size_t length,
                            int prot,
                            int flags,
                            unsigned long long offset);

/**@ingroup func_qurt_mem_mprotect2
  Changes access permissions and attributes on an existing mapping based on client_handle. \n

  @note1hang If the specified virtual address is not found or invalid attributes are passed
  the an error code is returned.\n
                 
  @param[in]   client_handle  Obtained from the current invocation function (Section 3.4.1).                   
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  @param[in]   prot           Mapping access permissions.(R/W/X)
                              Cache attributes, Bus attributes, User mode
  @return
  QURT_EOK -- Successfully changes permissions on the mapping.
  QURT_EFATAL -- Failed to chage permissions on the mapping \n
 */
int qurt_mem_mprotect2(int client_handle, const void *addr,
                      size_t length,
                      int prot);

/**@ingroup func_qurt_mem_mprotect
  Changes access permissions and attributes on an existing mapping. \n

  @note1hang If the specified virtual address is not found or invalid attributes are passed
  the an error code is returned.\n
                  
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  @param[in]   prot           Mapping access permissions.(R/W/X)
                              Cache attributes, Bus attributes, User mode
  @return
  QURT_EOK -- Successfully changes permissions on the mapping.
  QURT_EFATAL -- Failed to chage permissions on the mapping \n
 */
int qurt_mem_mprotect(const void *addr,
                      size_t length,
                      int prot);

/**@ingroup func_qurt_mem_munmap
  Remove an existing mapping. \n

  @note1hang If the specified mapping is not found in context of caller's process
  or invalid attributes are passed, an error code is returned.\n
                  
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  
  @return
  QURT_EOK -- Successfully changes permissions on the mapping.
  QURT_EFATAL -- Failed to chage permissions on the mapping \n
 */
int qurt_mem_munmap(void *addr,
                    size_t length);

/**@ingroup func_qurt_mem_munmap2
  Remove an existing mapping for a user process. \n

  @note1hang This API allows root process entity such as a driver, to remove mapping
  that was created for a user process. If the specified mapping is not found in context 
  of client handle or invalid attributes are passed, an error code is returned.\n
             
  @param[out]  client_handle  client handle of the user process that owns this mapping.                
  @param[in]   addr           Virtual memory address.
  @param[in]   length         Size of mapping in bytes.
  
  @return
  QURT_EOK -- Successfully changes permissions on the mapping.
  QURT_EFATAL -- Failed to chage permissions on the mapping \n
 */
int qurt_mem_munmap2(int client_handle,
                     void *addr,
                     size_t length);

/*
|| The macros here follow the style of the standard mmap() macros, but with
||  QURT_ prepended to avoid name conflicts, and to avoid having a dependency
||  on sys/mman.h.
||
|| Wherever possible, any values here which are also present in sys/mman.h
||  should have the same value in both places so that we can accept "mmap"
||  calls without having to remap parameters to new values.
||
|| In the future, it would be desirable to have a regression test which
||  checks, for instance, that these macros match.  Example:
||
||   assert(QURT_MAP_FAILED == MAP_FAILED);
||   ... repeat as needed ...
*/

#define QURT_PROT_NONE                  0x00
#define QURT_PROT_READ                  0x01
#define QURT_PROT_WRITE                 0x02
#define QURT_PROT_EXEC                  0x04
#define QURT_PROT_NODUMP                0x08    /* Skip dumping the mapping. During PD dump, need to skip
                                                   some mappings on host memory to avoid a race condition
                                                   where the memory is removed from host and DSP process
                                                   crashed before the mapping was removed*/

#define QURT_MAP_SHARED                 0x0001
#define QURT_MAP_PRIVATE                0x0002
#define QURT_MAP_NAMED_MEMSECTION       0x0004
#define QURT_MAP_FIXED                  0x0010
#define QURT_MAP_RENAME                 0x0020
#define QURT_MAP_NORESERVE              0x0040
#define QURT_MAP_INHERIT                0x0080
#define QURT_MAP_HASSEMAPHORE           0x0200
#define QURT_MAP_TRYFIXED               0x0400
#define QURT_MAP_WIRED                  0x0800
#define QURT_MAP_FILE                   0x0000
#define QURT_MAP_ANON                   0x1000

/** @cond */                                                   
#define QURT_MAP_ALIGNED(n)             ((n) << QURT_MAP_ALIGNMENT_SHIFT)
#define QURT_MAP_ALIGNMENT_SHIFT        24
/** @endcond */

#define QURT_MAP_ALIGNMENT_MASK         QURT_MAP_ALIGNED(0xff)
#define QURT_MAP_ALIGNMENT_64KB         QURT_MAP_ALIGNED(16)
#define QURT_MAP_ALIGNMENT_16MB         QURT_MAP_ALIGNED(24)
#define QURT_MAP_ALIGNMENT_4GB          QURT_MAP_ALIGNED(32)
#define QURT_MAP_ALIGNMENT_1TB          QURT_MAP_ALIGNED(40)
#define QURT_MAP_ALIGNMENT_256TB        QURT_MAP_ALIGNED(48)
#define QURT_MAP_ALIGNMENT_64PB         QURT_MAP_ALIGNED(56)
#define QURT_MAP_FAILED                 ((void *) -1)

/*
|| The macros below are extensions beyond the standard mmap flags, but follow
||  the style of the mmap flags.
*/
/** @cond */
// Describe bitfields in (prot)
#define QURT_PROT_CACHE_BOUNDS          16,19,7         /* Bits 16 through 19 are cache attribute, default 0 */
#define QURT_PROT_BUS_BOUNDS            20,21,0         /* Bits 20 through 21 are bus attributes, default 0 */
#define QURT_PROT_USER_BOUNDS           22,23,3         /* Bits 22 through 23 are user mode, default 3;
                                                           default=3 means to derive user mode setting from the
                                                           default mode of the client... */

// Describe bitfields in (flags)
#define QURT_MAP_PHYSADDR_BOUNDS        15,15,0         /* Bits 15 through 15 are physaddr, default 0 */
#define QURT_MAP_TYPE_BOUNDS            16,19,0         /* Bits 16 through 19 are mapping type, default 0 */
#define QURT_MAP_REGION_BOUNDS          20,23,0         /* Bits 20 through 23 are region type, default 0 */
/** @endcond */

// These macros get OR'ed into (prot)
#define QURT_PROT_CACHE_MODE(n)         QURT_MMAP_BUILD(QURT_PROT_CACHE_BOUNDS,n)
#define QURT_PROT_BUS_ATTR(n)           QURT_MMAP_BUILD(QURT_PROT_BUS_BOUNDS,n)
#define QURT_PROT_USER_MODE(n)          QURT_MMAP_BUILD(QURT_PROT_USER_BOUNDS,n)

// These macros get OR'ed into (flags)
#define QURT_MAP_PHYSADDR               QURT_MMAP_BUILD(QURT_MAP_PHYSADDR_BOUNDS,1)
#define QURT_MAP_TYPE(n)                QURT_MMAP_BUILD(QURT_MAP_TYPE_BOUNDS,n)
#define QURT_MAP_REGION(n)              QURT_MMAP_BUILD(QURT_MAP_REGION_BOUNDS,n)

/** @cond */
// These macros extract fields from (prot)
#define QURT_PROT_GET_CACHE_MODE(n)     QURT_MMAP_EXTRACT(QURT_PROT_CACHE_BOUNDS,n)
#define QURT_PROT_GET_BUS_ATTR(n)       QURT_MMAP_EXTRACT(QURT_PROT_BUS_BOUNDS,n)
#define QURT_PROT_GET_USER_MODE(n)      QURT_MMAP_EXTRACT(QURT_PROT_USER_BOUNDS,n)

// These macros extract fields from (flags)
#define QURT_MAP_GET_TYPE(n)            QURT_MMAP_EXTRACT(QURT_MAP_TYPE_BOUNDS,n)
#define QURT_MAP_GET_REGION(n)          QURT_MMAP_EXTRACT(QURT_MAP_REGION_BOUNDS,n)

// Macros for bitfield insertion and extraction
#define QURT_MMAP_MASK(lo,hi)           (~((~0u) << ((hi)-(lo)+1)))       /* Mask of same size as [lo..hi] */
#define QURT_MMAP_BUILD_(lo,hi,def,n)   ((((n)^(def))&QURT_MMAP_MASK(lo,hi))<<(lo))
#define QURT_MMAP_EXTRACT_(lo,hi,def,n) ((((n)>>(lo))&QURT_MMAP_MASK(lo,hi))^(def))
#define QURT_MMAP_BUILD(a,b)            QURT_MMAP_BUILD_(a,b)
#define QURT_MMAP_EXTRACT(a,b)          QURT_MMAP_EXTRACT_(a,b)
/** @endcond */

#endif
