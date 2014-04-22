/*==============================================================================
Audio adaptation layers.

GENERAL DESCRIPTION
High Speed USB Memory management

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
#include "DALSys.h"
#include "hsu_log.h"
#include "hsu_common_int.h"
#include "hsu_al_mem.h"


/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/


/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Global definitions
==============================================================================*/


/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/


/*===========================================================================

FUNCTION 

DESCRIPTION


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
boolean usb_alloc_memory(uint32 size, void** vaddr, void** paddr)
{
  qurt_mem_region_attr_t  mem_attr;
  int                     mem_sts;
  qurt_mem_pool_t         pool = qurt_mem_default_pool;
  qurt_mem_region_t       region;

  qurt_mem_region_attr_init(&mem_attr);
  
  mem_attr.cache_attribs = QURT_MEM_CACHE_NONE;
  mem_attr.permission = QURT_PERM_FULL;
  mem_attr.size = size;
  
  //current TX implementation is returning Physically contiguous memory with 1:1 mapping
  //But its not guaranteed so memset has to be done on virtual
  mem_sts = qurt_mem_region_create(&region, size, pool, &mem_attr);
  if(mem_sts != QURT_EOK)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG,"uncached mem alloc failed size (%d)", size);
    return FALSE;
  }
    
  qurt_mem_region_attr_get(region, &mem_attr);
  
  *vaddr = (void*)mem_attr.virtaddr;
  *paddr = (void*)mem_attr.physaddr;
  memset((void*)mem_attr.virtaddr, 0, size);
  
  HSU_ULOG_3(LOW_MSG, DBG_E_LOG, "vir_addr 0x%X, phy_addr 0x%X, size %u",
    (uint32) (void*) mem_attr.virtaddr,
    (uint32) (void*) mem_attr.physaddr,
    (uint32) size);

  return TRUE;
}


boolean usb_free_memory(void* mem_vaddr)
{
  int mem_sts;
  
  HSU_ASSERT(mem_vaddr);
    
  mem_sts = qurt_mem_region_delete((qurt_mem_region_t)mem_vaddr);
  if (mem_sts != QURT_EOK)
  {
    HSU_ULOG_2(ERROR_MSG, DBG_E_LOG,"free_mem_fail %u, vir_addr 0x%X",
      (uint32) mem_sts,
      (uint32) mem_vaddr);
    return FALSE;
  }
  return TRUE;
}

void free_single_rb(rb_t* rb, boolean free_buf)
{
  
  HSU_ASSERT(rb);

  rb->free = TRUE;
  rb->used = 0;

  if((free_buf == TRUE) && (rb->buffer_pa != NULL))
  {
    if(!usb_free_memory(rb->buffer_pa))
    {
      HSU_ASSERT(FALSE);;
    }
  }

}

rb_t * rb_init()
{
  boolean result;
  rb_t    *rb = NULL;
  void    *dummy = NULL; // For compilation
  
  result = usb_alloc_memory(sizeof(rb_t), (void**)&dummy, (void **)&rb);
  if(FALSE == result)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG,"failed_to_allocate_mem_for_rb");
  }

  rb->free = TRUE;

  return rb;
}

rb_t * find_rb_from_buffer(void * buffer, rb_t *rb_q)
{
   
  while(rb_q != NULL)
  {
    if(rb_q->buffer_va == buffer)
    {
        return rb_q;
    }
    rb_q = rb_q->next;
  }
  return NULL;
}


rb_t * get_free_rb(rb_t *rb_q, uint32 num)
{
  int i;

  HSU_ASSERT(rb_q);
  
  for(i = 0;i < num; i++)
  {
    if(rb_q != NULL)
    {
		if(rb_q->free == TRUE)
		{
			return rb_q;
		}
		rb_q = rb_q->next;
    }
  }
  return NULL;
}

boolean rb_queue_empty(rb_t *rb_q, uint32 num)
{
  int i;

  HSU_ASSERT(rb_q);  
  
  for(i = 0;i < num; i++)
  {
    if(rb_q != NULL)
    {
		if(rb_q->free == FALSE)
		{
			return FALSE;
		}
		rb_q = rb_q->next;
    }
  }
  return TRUE;
}


boolean rb_queue_init(rb_t *rb, uint32 size, uint32 num, boolean alloc_buf)
{
  int    i;
   
  HSU_ASSERT(rb);
  HSU_ASSERT(size!=0);

  for(i = 0;i < num; i++)
  {
    if(TRUE == alloc_buf) 
    {
      if (!usb_alloc_memory(size, (void**)&rb->buffer_va, (void**)&rb->buffer_pa))
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG,"failed_to_allocate_mem_for_buffer");
        return FALSE;
      }
      
    }
	
    rb->free = TRUE;  

    rb->used = 0;
    rb->done = FALSE;

    if(i < (num - 1))
    {
      rb->next = rb+1;
      rb = rb->next;
    }  

  }

  rb->next = NULL;

  return TRUE;
}

boolean rb_queue_free(rb_t* rb, boolean free_buf)
{
  rb_t *curr_rb;
  HSU_ASSERT(rb);
  curr_rb = rb;
  
  do
  {
    curr_rb->free = TRUE;
    curr_rb->used = 0;

    if(free_buf == TRUE)
    {
      if (curr_rb->buffer_pa != NULL)
      {
        if(!usb_free_memory(curr_rb->buffer_pa))
        {
          HSU_ASSERT(FALSE);
        }  
      }  
    }

    curr_rb = curr_rb->next;
  }
  while(curr_rb != NULL);

  return TRUE;
}
