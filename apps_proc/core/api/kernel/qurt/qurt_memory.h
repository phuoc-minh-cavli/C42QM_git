#ifndef QURT_MEMORY_H
#define QURT_MEMORY_H
/**
  @file qurt_memory.h
  @brief  Prototypes of Kernel memory API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016-2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/


#ifdef __cplusplus
extern "C" {
#endif


#include "qurt_types.h"
#include "txm_module.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 
#define memory_barrier() qurt_mem_barrier()
#define QURT_INVALID_ADDR (~0UL)
#define QURT_MAX_ADDR 0xFFFFFFFF
#define SIZE_16M                   0x01000000
#define SIZE_1M                    0x100000
#define SIZE_512K                  0x80000
#define SIZE_256K                  0x40000
#define SIZE_128K                  0x20000
#define SIZE_64K                   0x10000
#define SIZE_4K                    0x1000

/*=============================================================================
                                    TYPEDEFS
=============================================================================*/
/** @addtogroup memory_management_types
@{ */
typedef uint32 qurt_mem_region_t;
typedef uint32 qurt_size_t;
typedef uint32 qurt_mem_pool_t;       /**< Memory pools are represented in QuRT as objects of this type.*/
typedef uint32 qurt_addr_t;
typedef unsigned long long qurt_long_addr_t;

extern qurt_mem_pool_t qurt_mem_default_pool;


typedef enum {
    QURT_MEM_MAPPING_VIRTUAL=0,  /**< Default mode. The system picks up the
                                    available range of the virtual address, and maps it to
                                    available contiguous physical addresses.*/
    QURT_MEM_MAPPING_PHYS_CONTIGUOUS = 1,  /**< The physical address of the
                                               memory region is guaranteed to be contiguous
                                               starting at the provided address, and it is required
                                               to provide a fixed physical address.*/
    QURT_MEM_MAPPING_IDEMPOTENT=2,  /**< Allocates physical-to-virtual memory 1:1. */
    QURT_MEM_MAPPING_VIRTUAL_FIXED=3,      /**< Not supported yet, work in progress */
    QURT_MEM_MAPPING_NONE=4,               /**< Used to reserve a Virtual memory area. No physical memory
                                                is reserved or mapped to this virtual space */
    QURT_MEM_MAPPING_INVALID=10,        /**< Reserved as an invalid mapping type. */
} qurt_mem_mapping_t;

typedef enum {  /* If any new cache attributes are added, update the function: is_valid_cache_attr() */
        QURT_MEM_CACHE_STRONGLY_ORDERED = 0x0,         /**Strongly ordered memory, which is always shareable*/
        QURT_MEM_CACHE_DEVICE = 0x10,                  /**Device-type memory, that is not shareable*/
        QURT_MEM_CACHE_DEVICE_SHARED = 0x2,            /**Device-type memory, that is shareable*/
        QURT_MEM_CACHE_NONE = 0x8,                     /**< normal uncached memory that cannot be shared with other subsystems*/
        QURT_MEM_CACHE_NONE_SHARED = 0x9,              /**< normal uncached memory that can be shared with other subsystems*/
        
        QURT_MEM_CACHE_WRITEBACK = 0xE,                /**< L1 and L2 cacheable write-back memory, non-shared, write-allocate */
        QURT_MEM_CACHE_WRITEBACK_SHARED = 0xF,         /**< L1 and L2 cacheable write-back memory, shared, write-allocate */
        QURT_MEM_CACHE_WRITETHROUGH = 0x4,             /**< L1 and L2 cacheable write-through, non-shared no-write-allocate*/
        QURT_MEM_CACHE_WRITETHROUGH_SHARED = 0x5,      /**< L1 and L2 cacheable write-through, shared no-write-allocate*/
        
        QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE = 0x22,            /**< L1 write-back, L2 uncached, non-shared*/
        QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE_SHARED = 0x23,     /**< L1 write-back, L2 uncached, shared*/
        QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE = 0x24,         /**< L1 write-through, L2 uncached, non-shared*/
        QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE_SHARED = 0x25,  /**< L1 write-through, L2 uncached, non-shared*/
        
        QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE=QURT_MEM_CACHE_WRITEBACK,  /**< */
        QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE=QURT_MEM_CACHE_WRITETHROUGH,  /**< */
        
        QURT_MEM_CACHE_OUTER_INNER_UNCACHED=QURT_MEM_CACHE_NONE,
        QURT_MEM_CACHE_OUTER_INNER_WB_NO_ALLOC=QURT_MEM_CACHE_WRITEBACK,
         
        QURT_MEM_CACHE_INVALID = 0x3F,  /**< Reserved as an invalid cache type. */
} qurt_mem_cache_mode_t;

/** Cache types are specified as values of this type, which specify data cache or instruction cache. */
typedef enum {
    QURT_MEM_ICACHE, /**< */
    QURT_MEM_DCACHE  /**< */
} qurt_mem_cache_type_t;

/**  Cache operation codes are specified as values of this type. */
typedef enum {
    QURT_MEM_CACHE_FLUSH, /**< */
    QURT_MEM_CACHE_INVALIDATE, /**< */
    QURT_MEM_CACHE_FLUSH_INVALIDATE, /**< */
    QURT_MEM_CACHE_FLUSH_ALL, /**< */
    QURT_MEM_CACHE_FLUSH_INVALIDATE_ALL, /**< */
    QURT_MEM_CACHE_TABLE_FLUSH_INVALIDATE, /**< */
} qurt_mem_cache_op_t;

typedef enum {
    QURT_PERM_NONE=0x0, /**< */
    QURT_PERM_EXECUTE=0x1,  /**< */
    QURT_PERM_WRITE=0x6,  /**< */
    QURT_PERM_READ_WRITE=QURT_PERM_WRITE,
    QURT_PERM_READ=0x4, /**< */
    QURT_PERM_FULL=QURT_PERM_READ_WRITE|QURT_PERM_EXECUTE,  /**< */
} qurt_perm_t;

/** Memory region types are stored as values of this type. */
typedef enum {
    QURT_MEM_REGION_DEVICE = 0,
    QURT_MEM_REGION_DEVICE_SHARED,
    QURT_MEM_REGION_STRONGLY_ORDERED,
    QURT_MEM_REGION_NORMAL,
    QURT_MEM_REGION_INVALID = 10,
} qurt_mem_region_type_t;

typedef struct {
    /** @cond */
    qurt_mem_mapping_t     mapping_type;
    qurt_mem_region_type_t region_type;
    qurt_mem_cache_mode_t  cache_attribs;
    qurt_perm_t            permission;  // Access permissions 
    qurt_addr_t            physaddr;    // Physical Address
    qurt_addr_t            virtaddr;    // Virtual Address
    qurt_size_t            size;        // memory region size
    uint32                 asid;		// Address space ID
    /** @endcond */
} qurt_mem_region_attr_t;


/** @} */ /* end_addtogroup memory_management_types */
/*=====================================================================
 Functions
======================================================================*/
/**@ingroup func_qurt_mem_region_attr_init
  @xreflabel{sec:qurt_mem_region_attr_init}
  Initializes the specified memory region attribute structure with default attribute values: \n
  - Mapping -- QURT_MEM_MAPPING_IDEMPOTENT \n
  - Cache mode -- QURT_MEM_CACHE_OUTER_WB_ALLOC \n
  - Physical address -- -1 \n
  - Virtual address -- -1 \n
  - Memory type -- QURT_MEM_REGION_NORMAL \n
  - Size -- -1
 
  @datatypes
  #qurt_mem_region_attr_t
 
  @param[in,out] attr  Pointer to the destination structure for the memory region attributes.
 
  @return
  None.
 
  @dependencies
  None.
 */
void qurt_mem_region_attr_init(qurt_mem_region_attr_t *attr);

/**@ingroup func_qurt_mem_region_create
  @xreflabel{sec:mem_region_create}
  Creates a memory region with the specified attributes.

  @note1hang If attr is specified as NULL, the memory region is created with default
             attribute values 
             QuRT predefines the memory pool object qurt_mem_default_pool
  @datatypes
  #qurt_mem_region_t \n
  #qurt_size_t \n
  #qurt_mem_pool_t \n
  qurt_mem_region_attr_t

  @param[out] region Pointer to the memory region object.
  @param[in]  size   Memory region size (in bytes). If size is not an integral multiple of 4K,
                     it is rounded up to a 4K boundary.
  @param[in]  pool   Memory pool of the region. Use *qurt_mem_default_pool* as default value
  @param[in]  attr   Pointer to the memory region attribute structure.

  @return
  QURT_EOK -- Memory region successfully created.\n
  QURT_EMEM -- Not enough memory to create region.

  @dependencies
  None.
*/
int qurt_mem_region_create(qurt_mem_region_t *region, qurt_size_t size, qurt_mem_pool_t pool, qurt_mem_region_attr_t *attr);


/**@ingroup func_qurt_mem_region_delete
  Deletes the specified memory region.

  If the memory region was created by the caller application, it is removed and its
  assigned memory reclaimed by the system.

  If the memory region was created by a different application (and shared with the caller
  application), then only the local memory mapping to the region is removed; the memory
  itself is not reclaimed by the system.

  @datatypes
  #qurt_mem_region_t

  @param[in] region Memory region object.

  @returns
  QURT_EOK -- Region successfully deleted.

  @dependencies
  None.
*/
int qurt_mem_region_delete(qurt_mem_region_t region);


/**@ingroup func_qurt_mem_region_attr_get
  @xreflabel{sec:mem_region_attr_get}
  Gets the memory attributes of the specified message region.
  After a memory region is created, its attributes cannot be changed.

  @datatypes
  #qurt_mem_region_t \n
  #qurt_mem_region_attr_t

  @param[in] region     Memory region object.
  @param[out] attr      Pointer to the destination structure for memory region attributes.

  @return
  QURT_EOK -- Operation successfully performed. \n
  Error code -- Failure.

  @dependencies
  None.
*/
int qurt_mem_region_attr_get(qurt_mem_region_t region, qurt_mem_region_attr_t *attr);


/**@ingroup func_qurt_mapping_create
  Creates a new memory mapping in the page table.

  @datatypes
  #qurt_addr_t \n
  #qurt_size_t \n
  #qurt_mem_cache_mode_t \n
  #qurt_perm_t
 
  @param vaddr			Virtual address.
  @param paddr			Physical address.
  @param size			Size (4K-aligned) of the mapped memory page.
  @param cache_attribs		Cache mode (writeback, etc.).
  @param perm			Access permissions.

  @return			
  QURT_EOK -- Mapping created. \n
  QURT_EMEM -- Failed to create mapping. 	

  @dependencies
  None.
*/
int qurt_mapping_create(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size,
                         qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm);

                         
/**@ingroup func_qurt_mapping_remove
  Deletes the specified memory mapping from the page table.
 
  @datatypes
  #qurt_addr_t \n
  #qurt_size_t

  @param[in] vaddr			Virtual address.
  @param[in] paddr			Physical address.
  @param[in] size			Size of the mapped memory page (4K-aligned).

  @return 			
  QURT_EOK -- Mapping created successfully.

  @dependencies
  None.
  		
 */ 		
int qurt_mapping_remove(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size);


/**@ingroup func_qurt_mem_cache_clean
  Performs a cache clean operation on the data stored in the specified memory area.

  @note1hang The flush all operation can be performed only on the data cache.

  @datatypes
  #qurt_addr_t \n
  #qurt_size_t \n
  #qurt_mem_cache_op_t \n
  #qurt_mem_cache_type_t

  @param[in] addr      Address of data to be flushed.
  @param[in]  size     Size (in bytes) of data to be flushed.
  @param[in] opcode    Type of cache clean operation: \n QURT_MEM_CACHE_FLUSH,\n QURT_MEM_CACHE_INVALIDATE,\n 
                       QURT_MEM_CACHE_FLUSH_INVALIDATE,\n QURT_MEM_CACHE_FLUSH_ALL.\n
                       @note1 QURT_MEM_CACHE_FLUSH_ALL is valid only when the type is QURT_MEM_DCACHE
  @param type          Cache type: QURT_MEM_ICACHE, QURT_MEM_DCACHE.
 
  @return
  QURT_EOK -- Cache operation performed successfully.\n
  QURT_EVAL -- Invalid cache type.\n
  QURT_EALIGN -- Aligning data or address failed.\n
  
  @dependencies
  None.
*/
int qurt_mem_cache_clean(qurt_addr_t addr, qurt_size_t size, qurt_mem_cache_op_t opcode, qurt_mem_cache_type_t type);

/**@ingroup func_qurt_mem_map_static_query
  Determines if a memory page is statically mapped with the specified page attributes: \n
  physical address, page size, cache mode, and memory permissions:
  - If the specified page is statically mapped, vaddr returns the page's virtual address.\n
  - If the page is not statically mapped (or if it does not exist as specified), vaddr
     returns QURT_INVALID_ADDR as the virtual address value.\n
  QuRT memory maps are defined in the system configuration file.
 
  @datatypes
  #qurt_addr_t \n
  #qurt_mem_cache_mode_t \n
  #qurt_perm_t
  
  @param[out]  vaddr             Virtual address corresponding to paddr.
  @param[in]   paddr             Physical address.  
  @param[in]   page_size         Size of the mapped memory page.
  @param[in]   cache_attribs     Cache mode (writeback, etc.).
  @param[in]   perm              Access permissions.

  @return
  QURT_EOK -- Specified page is statically mapped, virtual address is returned in vaddr. \n
  QURT_EMEM -- Specified page is not statically mapped, QURT_INVALID_ADDR is returned in vaddr. \n
  QURT_EVAL -- Page attribute(s) do not match, QURT_INVALID_ADDR is returned in vaddr. \n
  QURT_EINVALID -- Invalid attributes.

  @dependencies
  None.
 */
int qurt_mem_map_static_query(qurt_addr_t *vaddr, qurt_addr_t paddr, qurt_size_t page_size, qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm);


/**@ingroup func_qurt_mem_region_query
  Queries a memory region. \n
  This function determines whether a dynamically-created memory region (Section @xref{sec:mem_region_create}) exists for the
  specified virtual or physical address.  
  Once a memory region has been determined to exist, its attributes can be
  accessed (Section @xref{sec:mem_region_attr_get}).

  @note1hang This function returns QURT_EFATAL if QURT_EINVALID is passed to both
             vaddr and paddr (or to neither). 

  @datatypes
  #qurt_mem_region_t \n
  #qurt_paddr_t 
   
  @param[out] region_handle    Pointer to the memory region object (if it exists).
  @param[in]  vaddr            Virtual address to query; if vaddr is specified, paddr must be set to
                               the value QURT_EINVALID.
  @param[in]  paddr            Physical address to query; if paddr is specified, vaddr must be set to
                               the value QURT_EINVALID.

  @return 
  QURT_EOK -- Query successfully performed. \n
  QURT_EMEM -- Region not found for the specified address. \n
  QURT_EFATAL -- Invalid input parameters.

  @dependencies
  None.
 */
int qurt_mem_region_query(qurt_mem_region_t *region_handle, qurt_addr_t vaddr, qurt_addr_t paddr);


/**@ingroup func_qurt_lookup_physaddr
  Translates a virtual memory address to the physical memory address it is mapped to.

  @datatypes
  #qurt_addr_t
  #qurt_paddr_t

  @param[in] vaddr   Virtual address.

  @return
  Nonzero -- Physical address the virtual address is mapped to.\n
  0 -- Virtual address not mapped.

  @dependencies
  None.
*/
qurt_addr_t qurt_lookup_physaddr (qurt_addr_t vaddr);


/**@ingroup func_qurt_mem_pool_attach 
  Initializes a memory pool object to be attached to a pool predefined in the system
  configuration file.

  Memory pool objects are used to assign memory regions to physical memory in different
  Hexagon memory units. They are specified in memory region create operations
  (Section @xref{sec:mem_region_create}).

  @note1hang QuRT predefines the memory pool object qurt_mem_default_pool
             (Section @xref{dox:mem_management}) for allocation memory regions in SMI memory. The pool attach
             operation is necessary only when allocating memory regions in nonstandard
             memory units such as TCM.
 
  @datatypes
  #qurt_mem_pool_t

  @param[in] name   Pointer to the memory pool name. 
  @param[out] pool  Pointer to the memory pool object.

  @return
  QURT_EOK -- Attach operation successful.
  QURT_EVAL -- Specified pool does not exist

  @dependencies
  None.
*/
int qurt_mem_pool_attach(char *name, qurt_mem_pool_t *pool);


/**@ingroup func_qurt_mem_barrier
  This instruction ensures that all memory accesses in program order before the
  barrier are observed in the system before any explicit memory accesses that
  appear in program order after the barrier. It does not affect the ordering of any
  other instructions executing on the processor, or of instruction fetches.
 
  @return 			
  None

  @dependencies
  None.  		
 */ 

INLINE void qurt_mem_barrier() {
   __asm ( "dmb" );
}

/**@ingroup func_qurt_check_if_kernel_address
   Returns the status whether specifeid address belongs to Kernel space or User space.
  
  @datatypes
  #qurt_addr_t

  @param[in] address:     Address for which kernel or user space ownership to be determined
			 
  @return
  TRUE  -- Specified address belongs to Kernel space
  FALSE -- Specified address belongs belongs to User space or invalid address

  @dependencies
  None.
*/
int qurt_check_if_kernel_address(qurt_addr_t *address);

/**@ingroup func_qurt_check_if_module_address
   Checks if a given memory region belongs to the current Module memory region or not. 
   This is applicable only in synchronous context (in the DAM thread context).
  
  @datatypes
  #qurt_addr_t \n
  #uint32 len

  @param[in] address:     Start address of the region that is to be validated for current module address range
  @param[in] len    :     Length of memory region that is to be validated for current module address range
			 
  @return
  TRUE  -- Specified address belongs to current module's address range. \n
  FALSE -- Invalid address or Specified address range is outside of module's address range. 
  
  @dependencies
  None.
*/
int qurt_check_if_module_address(qurt_addr_t address, uint32 len);
					  
/**@ingroup func_qurt_copy_to_user 
   Copies data (minimum of user space and kernel space buffer sizes)
   from kernel space buffer to user space buffer, if destination address is actually user space address.
   If destination address is kernel space buffer, it does not copy the data.

  @datatypes
  #void \n
  #qurt_size_t

  @param[in] uspace_addr:     Destination (user space) buffer address
  @param[in] uspace_buf_size: Destination (user space) buffer size
  @param[in] kspace_addr:     Source (kernel space) buffer address
  @param[in] kspace_buf_size: Source (kernel space) buffer size
			 
  @return
  QURT_EOK -- Data successfully copied to destination address.
  QURT_ENOTALLOWED -- Copy not allowed because destination address is not a valid user space address.
  QURT_EINVALID -- One of the input arguments found to be invalid.

  @dependencies
  None.
*/
int qurt_copy_to_user(void *uspace_addr, qurt_size_t uspace_buf_size, 
                      void *kspace_addr, qurt_size_t kspace_buf_size);

/**@ingroup func_qurt_copy_from_user 
   user here implies caller. It can be either user/kernel space.
   Copies data (minimum of kernel space and caller space buffer sizes)
   from caller space buffer to kernel space buffer, if destination address is actually kernel space address.

  @datatypes
  #void \n
  #qurt_size_t

  @param[in] dst		:	Destination (kernel space) buffer address
  @param[in] dst_size	:	Destination (kernel space) buffer size
  @param[in] src		:	Source (user/kernel space) buffer address
  @param[in] src_size	:	Source (user/kernel space) buffer size
			 
  @return
  QURT_EOK -- Data successfully copied to destination address.
  QURT_ENOTALLOWED -- Copy not allowed because source address is not a valid address/no permission.
  QURT_EINVALID -- One of the input arguments found to be invalid.

  @dependencies
  None.
*/

int qurt_copy_from_user(
    void *dst, qurt_size_t dst_size,
    const void *src, qurt_size_t src_size
    );

/**@ingroup func_qurt_copy_to_user_ext
   Copies the data (minimum of uspace_buf_size and kspace_buf_size sizes) from 
   kspace_addr to uspace_addr after performing validation based on input param 
   module_instance.
   
   Copies the specified amount of data (min of source and destination buffer sizes) 
   if module_instance is a Valid User or Kernel space Module pointer and destination 
   address space falls within given Module's boundary.
   
   Copies the specified amount of data (min of source and destination buffer sizes) 
   if module_instance is a valid Kernel thread Magic ID.

   @datatypes
   #TXM_MODULE_INSTANCE \n
   #void \n
   #qurt_size_t
 
   @param[in] module_instance  Module instance pointer or Kernel thread Magic ID.
   @param[in] uspace_addr  	   Destination buffer start address (could be in Module or resident Kernel thread).
   @param[in] uspace_buf_size  Destination buffer size.
   @param[in] kspace_addr  	   Source buffer start address.
   @param[in] kspace_buf_size  Source buffer size.

   @return
   QURT_EOK:         Data successfully copied to destination address. \n
   QURT_ENOTALLOWED: Copy not allowed because of condition check failure. \n
   QURT_EINVALID:    One of the input arguments found to be invalid. 
   
   @dependencies
   Note: Client can call qurt_txm_module_instance_get API and pass the pointer/value 
         returned by that API as module_instance param to this API.
*/
int qurt_copy_to_user_ext(TXM_MODULE_INSTANCE *module_instance, void *uspace_addr, qurt_size_t uspace_buf_size, void *kspace_addr, qurt_size_t kspace_buf_size);
						  
/**@ingroup func_qurt_copy_from_user_ext 
   Copies the data (minimum of uspace_buf_size and kspace_buf_size sizes) from 
   uspace_addr to kspace_addr after performing validation based on input param 
   module_instance.
   
   Copies the specified amount of data (min of source and destination buffer sizes) 
   if module_instance is a Valid User or Kernel space Module pointer and source 
   address space falls within given Module's boundary.
   
   Copies the specified amount of data (min of source and destination buffer sizes) 
   if module_instance is a valid Kernel thread Magic ID.

   @datatypes
   #TXM_MODULE_INSTANCE \n
   #void \n
   #qurt_size_t

   @param[in] module_instance : Module instance pointer or Kernel thread Magic ID
   @param[in] kspace_addr	  :	Destination buffer start address
   @param[in] kspace_buf_size :	Destination buffer size
   @param[in] uspace_addr	  :	Source buffer start address (could be in Module or resident Kernel thread)
   @param[in] uspace_buf_size :	Source buffer size
			 
   @return
   QURT_EOK -- Data successfully copied to destination address. \n
   QURT_ENOTALLOWED -- Copy not allowed because of condition check failure. \n
   QURT_EINVALID -- One of the input arguments found to be invalid.

   @dependencies
   Note: Client can call qurt_txm_module_instance_get API and pass the pointer/value 
         returned by that API as module_instance param to this API.
*/						  
int qurt_copy_from_user_ext(TXM_MODULE_INSTANCE *module_instance, void *kspace_addr, qurt_size_t kspace_buf_size, void *uspace_addr, qurt_size_t uspace_buf_size);

/**@ingroup func_qurt_txm_check_if_valid_module_code_addr 
   Checks if passed address belongs to code area of caller module

  @param[in] addr		:	Address to be verified
			 
  @return
  TRUE  : if address valid and falls under caller module's code area
  FALSE : invalid address or address outside caller module's code area
  @dependencies
  None.
*/
int qurt_txm_check_if_valid_module_code_addr(void *addr);

/**@ingroup func_qurt_txm_check_if_valid_module_data_addr 
   Checks if passed address belongs to data area of caller module

  @param[in] addr		:	Address to be verified
			 
  @return
  TRUE  : if address valid and falls under caller module's data area
  FALSE : invalid address or address outside caller module's data area
  @dependencies
  None.
*/
int qurt_txm_check_if_valid_module_data_addr(void *addr);

/*** Changes related to configurable memory region query framework ***/

/* IDs for different memory regions
   Mem regions can be added on need basis.
   Region ID 0 is reserved.
*/
#define MEM_REGION_ID_RESERVED	   	0
#define MEM_REGION_ID_OEM_POOL		MEM_REGION_ID_RESERVED + 1
#define MEM_REGION_ID_TZ_APPS		MEM_REGION_ID_OEM_POOL + 1

#define MEM_REGION_ID_MAX			MEM_REGION_ID_TZ_APPS + 1

/**@ingroup func_qurt_get_mem_region_info 
   Function to query Memory region info based on client supplied region ID.
   If input region ID is valid and present in region list, it would return 
   region start address and region size to client.

  @datatypes
  #uint32 \n
  #qurt_addr_t \n
  #qurt_size_t

  @param[in]  region_id     : ID of the Memory region for which info is requested
  @param[out] pRegion_start : Client supplied pointer, to which, region start address would be updated.
  @param[out] pRegion_size  : Client supplied pointer, to which, region size would be updated.

  @return
  QURT_EOK      -- Success. Supplied pointers are updated with respective info.
  QURT_EINVALID -- Invalid input param.
  QURT_EFAILED  -- Failure due to internal error.

  @dependencies
  None.
*/
int qurt_get_mem_region_info(uint32 region_id, qurt_addr_t *pRegion_start, qurt_size_t *pRegion_size);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_MEMORY_H */
