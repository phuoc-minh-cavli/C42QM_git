/*=============================================================================
 
									obj_cache.c 
 
 GENERAL DESCRIPTION
		 Kernel obj cache functions
		 
 EXTERNAL FUNCTIONS
		None.
 
 INITIALIZATION AND SEQUENCING REQUIREMENTS
		None.
 
			 Copyright (c) 2009
										 by Qualcomm Technologies Incorporated.	All Rights Reserved.
 
 =============================================================================*/
 
 /*=============================================================================
 
												 EDIT HISTORY FOR MODULE
 
	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.
 
 
 $Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/obj_cache.c#1 $ 
 $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
 
 when 			who 		what, where, why
 -------- 	--- 		------------------------------------------------------------
 03/06/11 	op			Add header and history
 =============================================================================*/
#include "mem_utils.h"

void * QURTK_obj_cache_alloc(QURTK_obj_cache_t *cache) {
    void *new_page, *new_obj;
    if (!cache->free_obj_list) {
        // out of memory in cache, request more from kernel heap
        qurt_mutex_lock(&cache->lock);
        new_page = QURTK_heap_alloc();
        cache->init(&cache->free_obj_list, new_page);
        cache->pgnum++;
        qurt_mutex_unlock(&cache->lock);
    }
    new_obj = (void *)QURTK_lifo_pop( &cache->free_obj_list );   
    *(int *)new_obj = -1;  // First field was used as "next" pointer. Reset it "-1".
    return new_obj;
}

void QURTK_obj_cache_free(QURTK_obj_cache_t *cache, void *obj) {
    QURTK_lifo_push(&cache->free_obj_list, obj);
}

