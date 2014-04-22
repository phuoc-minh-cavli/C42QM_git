#ifndef ALLOC_H
#define ALLOC_H
/*=============================================================================

                alloc.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  Kernel allocation functions  
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/alloc.h#1 $ 
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/
#include "memory.h"

#define size_round_up(size,alignment) ((((size) + ((alignment)-1)) & (~((alignment)-1))))

/*
 * General purpose functions.
 */
struct memory *QURTK_mem_alloc(struct mem_pool *pool, qurt_size_t size);
struct memory *QURTK_mem_alloc_aligned(struct mem_pool *pool, qurt_size_t size);
struct memory *QURTK_mem_alloc_fixed(struct mem_pool *pool, qurt_size_t size, qurt_addr_t addr);
void QURTK_mem_free(struct mem_pool *pool, struct memory *mem);

/*
 * Specialised functions.
 */
int mem_add(struct mem_pool *pool, qurt_addr_t addr, qurt_size_t size);
struct memory *mem_add_and_alloc(struct mem_pool *pool, qurt_addr_t addr, qurt_size_t size);
void mem_coalesce(struct mem_pool *pool);
int mem_empty(struct mem_pool *pool);
void static_pool_init(void);

#endif /* ALLOC_H */
