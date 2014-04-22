#ifndef MEMORY_H
#define MEMORY_H
/*=============================================================================
                memory.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  Kernel memory structures and typedefs
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009, 2021 - 2022 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/memory.h#3 $ 
$DateTime: 2022/09/27 06:17:17 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

#include "mem_utils.h"
#include "qurt_memory.h"


/**
 * Cache operation type
 */
 /*
typedef enum { 
    MEM_CACHE_FLUSH,
    MEM_CACHE_INVALIDATE,
    MEM_CACHE_FLUSH_INVALIDATE,
} qurt_mem_cache_op_t;
*/

/**
 * Cache flush operation argument structure
 */
typedef struct {
    qurt_mem_cache_op_t         operation;
    qurt_mem_cache_type_t       type;
    void                   *addr; 
    qurt_size_t            size;
} flush_attr_t;

/**
 * Cache OPT structure is an "ioctl" like structure. It is a union of all
 * possible cache control operations.  Currently it only supports cache flush
 * and will be supporting L2 config in the future 
 */
typedef union{
        flush_attr_t flush;
} mem_cache_ctrl_t;


//#define QUBE_INLINE static inline __attribute__((always_inline))
//#define QUBE_ATTR  __attribute__((warn_unused_result))


struct mem_range {
    qurt_addr_t addr;
    qurt_size_t size;
};

#include "tailq.h"

struct memory {
    qurt_addr_t addr;
    qurt_size_t size;
    TAILQ_ENTRY(memory) mem_list;
    //need a tf bit and a doubly-linked list
    TAILQ_ENTRY(memory) phys_list;
    unsigned int tf_bit;
};

typedef struct memory memory_t;

TAILQ_HEAD(mem_list, memory);
TAILQ_HEAD(phys_list, memory);

typedef struct mem_list mem_list_t;

#define CHAR_BIT        8
#define MEM_MIN_BITS    12
#define MEM_MIN_SIZE    (1UL << MEM_MIN_BITS)
#define MEM_MAX_IDX     4


// This macro is assigned to MAX_POOLS_TOTAL and used in C files
// this change is required to conditionally add more memory pools
#define MAX_POOLS       5
#define MAX_PHYPOOL_RANGES		20

struct mem_pool {
    struct mem_list free_list[MEM_MAX_IDX + 1];
    //struct mem_list allocated;
    struct phys_list global_list; //?
    int coalesced;
    int pad;
};


//
//Memory region
//
typedef struct mem_pool mem_pool_t;
typedef struct memory mem_t;

typedef struct vma_node {
    struct vma_node * next;
    unsigned int magic;
    mem_t * virt_mem;
    uint32  cnt; //book-keeping regions per vma
} vma_node_t;

typedef struct mem_region {
    struct mem_region          * next;     //A global linklist of all memory regions
    unsigned int                 magic;          /* This is needed to verify the type of */
    mem_t                      * virt_mem;
    mem_pool_t                 * phys_pool;
    mem_t                      * phys_mem;
    uint32                       size;
    qurt_mem_mapping_t           mapping_type;
    qurt_mem_cache_mode_t        cache_mode;
    qurt_perm_t                  perm;
    uint32                       owner_thread;
    vma_node_t		            *vma;
    uint32                       asid;
} mem_region_t;

typedef struct phys_pool {
    char name[32];
    struct mem_pool pool;
}
phys_pool_t;


struct phys_mem_pool_config{
    char name[32];
    struct range {
        unsigned int start;
        unsigned int size;
    } ranges[MAX_PHYPOOL_RANGES];
};
 
#endif /* MEMORY_H */
