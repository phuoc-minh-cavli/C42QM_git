/*=============================================================================
 
									heap.c 
 
 GENERAL DESCRIPTION
		 Kernel HEAP helper functions
     Kernel HEAP currently mainly used for pagetable, "struct memory", "mem_region_t"
     In future, it may be used to allocate memory of "space_t" and "QURTK_thread_context". 
     It only supports allocation/deallocation of uniform size - 4KB. This can eliminate 
     the kernel heap fragmentation. For objects that with size less than 4KB, object cache 
     (similiar to slab cache) is used to manage free memory in allocated pages for future 
     creation of objects.
		 
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
 
 
 $Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/heap.c#1 $ 
 $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
 
 when 			who 		what, where, why
 -------- 	--- 		------------------------------------------------------------
 03/06/11 	op			Use the same return enum values for kernel and user functions
 =============================================================================*/

/* Kernel HEAP helper functions.
 *
 * Kernel HEAP currently mainly used for pagetable, "struct memory", "mem_region_t"
 * In future, it may be used to allocate memory of "space_t" and "QURTK_thread_context". 
 * It only supports allocation/deallocation of uniform size - 4KB. This can eliminate 
 * the kernel heap fragmentation. For objects that with size less than 4KB, object cache 
 * (similiar to slab cache) is used to manage free memory in allocated pages for future 
 * creation of objects.
 */ 

#include "mem_utils.h"

//#define kprintf(..) 

#define QURTK_HEAP_SIZE 0x10000  // 64K

uint32 QURTK_heap[QURTK_HEAP_SIZE/4];
uint32 max_heap_used = 0;

static void * QURTK_heap_free_list = NULL;

void QURTK_heap_init( ){
    unsigned int addr;
    
    //kprintf("kernel physical start: %p\n", __phys_addr_ram );
    // kprintf("heap init: start %p size 0x%x\n", &QURTK_heap[0], QURTK_HEAP_SIZE );

    for( addr = (unsigned int)&QURTK_heap[0]; addr <= (unsigned int)&QURTK_heap[QURTK_HEAP_SIZE/4 - 1]; addr += 0x1000){
        //kprintf("addr %p\n", addr );
        QURTK_lifo_push(&QURTK_heap_free_list, (void*)addr);
    }
}

void * QURTK_heap_alloc(){
    /* We ignore the size, and only return 4K page */
    void * a = QURTK_lifo_pop( &QURTK_heap_free_list );
    if( !a ) {
        kprintf("Fatal Error: QURT kernel out of heap!\n");
        ASSERT(0);
    }
    max_heap_used++;
    return a;

}

void QURTK_heap_free( void * buf ){
    QURTK_lifo_push( &QURTK_heap_free_list, buf );
}
