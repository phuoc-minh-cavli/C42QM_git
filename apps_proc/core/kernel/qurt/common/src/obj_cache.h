#ifndef OBJ_CACHE_H
#define OBJ_CACHE_H
/*=============================================================================
                obj_cache.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  Kernel obj cache functions
			
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


$Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/obj_cache.h#1 $ 
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

#include "qurt_types.h"
#include "qurt_mutex.h"


typedef struct {
    qurt_mutex_t lock;    // lock is used when allocating from kernel heap
    void *free_obj_list;// free object LIFO list
    volatile unsigned int pgnum;    // total number of pages allocated
    void (*init)(void **, void *);  // init objects of specific type in new page
}QURTK_obj_cache_t;


INLINE void QURTK_obj_cache_init(QURTK_obj_cache_t *cache, void (*func_ptr)(void **, void *)) {
    qurt_mutex_init(&(cache->lock));
    cache->free_obj_list = NULL;
    cache->pgnum = 0;
    cache->init = func_ptr;
}

void * QURTK_obj_cache_alloc(QURTK_obj_cache_t *cache); 

void QURTK_obj_cache_free(QURTK_obj_cache_t *cache, void *obj);

#endif /* OBJ_CACHE_H */

