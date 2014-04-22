/*==============================================================================
Audio adaptation layers.

GENERAL DESCRIPTION
High Speed USB Audio application

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2016 - 2019 by Qualcomm Technologies, Inc. All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: 
  $DateTime: 


==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#ifndef _HSU_AL_MEM_H
#define _HSU_AL_MEM_H
#include "queue.h"
#include "lists.h"
#include "qurt_memory.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/
struct rb_t;
typedef struct rb_t{
  q_link_type        link;
  void               *buffer_pa;
  void               *buffer_va;
  uint32             used;
  boolean            free;
  boolean            done;
  struct rb_t        *next;
  void               *app_ptr;
  //void               *region_t;
}rb_t;

typedef struct rb_t *rb_ptr;

void free_single_rb(rb_t* rb, boolean free_buf);
rb_t * rb_init(void);
boolean rb_queue_init(rb_t *rb, uint32 size, uint32 num, boolean alloc_buf);
boolean rb_queue_free(rb_t* rb, boolean free_buf);
rb_t * get_free_rb(rb_t *rb_q, uint32 num);
rb_t * find_rb_from_buffer(void * buffer, rb_t *rb_q);

boolean rb_queue_empty(rb_t *rb_q, uint32 num);
boolean usb_alloc_memory(uint32 size, void** vaddr, void** paddr);



#endif



