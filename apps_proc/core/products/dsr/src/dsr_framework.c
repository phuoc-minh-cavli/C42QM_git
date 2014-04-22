/*======================================================================
                     DSR_Framework.c

GENERAL DESCRIPTION
  DSR (Deferred Service Routine)

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS
  Clients that require deferred service routines must register using DSR APIs

 Copyright (c) 2016-2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 06/20/2016   knaidu  Initial Version
 ======================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "dsr_framework.h"
#include "stdlib.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_interrupt.h"
#include "err.h"
#include "qurt.h"
#include "rcinit.h"
#include "stringl.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
struct dsr_obj_s
{
   dsr_obj_t *next;
   dsr_hdlr_t cb;
   void *cb_ctxt;
   void *payload;
   dsr_priority_t priority;
   boolean pending;
   dsr_context_t context;
};

typedef struct
{
   qurt_thread_attr_t thread_attr;
   qurt_thread_t thread_id;
   boolean       bBusy;
   uint32 index;
   dsr_context_t context;
} dsr_thread_t;

typedef struct
{
   qurt_signal_t signal;
   dsr_thread_t *dsr_thread_list;
   uint32 next_thread;
} dsr_thread_list_t;

// TODO - Clean up if required
typedef struct dsr_thread_attr_s
{
   uint32 thread_stack_size;
   uint8  thread_prio;

}dsr_thread_attr_t;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
/* DSR globals for handling deferred ISR */
uint32 dsr_num_isr_threads;
dsr_thread_list_t *dsr_thread_list_isr;
dsr_obj_t *dsr_obj_list_isr;

/* DSR globals for handling workloop */
uint32 dsr_num_wklp_threads;
dsr_thread_list_t *dsr_thread_list_wklp;
dsr_obj_t *dsr_obj_list_wklp;
qurt_mutex_t dsr_mutex_wklp;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static boolean dsr_init_done = FALSE;
 
/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static void dsr_update_obj_list_head(dsr_context_t context, dsr_obj_t *me)
{
	if(context == DSR_CONTEXT_ISR)
	{
		dsr_obj_list_isr = me;
	}
	else if(context == DSR_CONTEXT_WORKLOOP)
	{
		dsr_obj_list_wklp = me;
	}
	else
	{
		ERR_FATAL("DSR: Invalid object type\n",0,0,0);
	}
}

/* Function to be invoked under lock - either intlock or mutex lock 
 * Primary usage is to get the pending DSR object from the list
 */
static dsr_obj_t* dsr_get_pending_obj(dsr_context_t context)
{
    dsr_obj_t *dsr_obj_ptr, *dsr_obj_list_head;
	
	dsr_obj_list_head = DSR_GET_OBJECT_LIST(context);
    if (dsr_obj_list_head == NULL)
    {
      dsr_obj_ptr = NULL;
    }
    else
    {
      dsr_obj_ptr = dsr_obj_list_head;
	  dsr_obj_list_head = dsr_obj_ptr->next;
      dsr_obj_ptr->next = NULL;
      dsr_obj_ptr->pending = FALSE;
    }
	dsr_update_obj_list_head(context, dsr_obj_list_head);
	return dsr_obj_ptr;
}

/* Process the pending DSRs from the list */
static void dsr_handle_pending_dsrs(dsr_context_t context)
{
  unsigned long interrupt_flag = 0;
  dsr_obj_t *dsr_obj_ptr = NULL;

	do
	{
		switch (context)
		{
			case DSR_CONTEXT_ISR:
				{
					INTLOCK_SAVE(interrupt_flag);
					dsr_obj_ptr = dsr_get_pending_obj(context);
					INTFREE_RESTORE(interrupt_flag);
					break;
				}
			case DSR_CONTEXT_WORKLOOP:
				{
					qurt_mutex_lock(&dsr_mutex_wklp);
					dsr_obj_ptr = dsr_get_pending_obj(context);
					qurt_mutex_unlock(&dsr_mutex_wklp);
					break;
				}
			default:
				{
					ERR_FATAL("DSR: Invalid object type",0,0,0);
					break;
				}
		}
		
		if (dsr_obj_ptr != NULL)
		{
		  dsr_obj_ptr->cb(dsr_obj_ptr, dsr_obj_ptr->cb_ctxt, dsr_obj_ptr->payload);
		}
	} while (dsr_obj_ptr != NULL);
}
 
static void dsr_workloop(void *thread_ctxt)
{
  dsr_thread_list_t *dsr_thread_list;
  dsr_thread_t *dsr_thread;

  dsr_thread = thread_ctxt;
  dsr_thread_list = DSR_GET_THREAD_LIST(dsr_thread->context);
  
  for ( ; ; )
  {
	dsr_thread->bBusy = FALSE;
	
    qurt_signal_wait(&dsr_thread_list->signal,
                     1U << dsr_thread->index,
                     QURT_SIGNAL_ATTR_WAIT_ALL | QURT_SIGNAL_ATTR_CLEAR_MASK);
    
    dsr_thread->bBusy = TRUE;
	
    dsr_handle_pending_dsrs(dsr_thread->context);
  }
}

static boolean dsr_remove_from_list (dsr_obj_t *me_obj)
{
  dsr_obj_t *current_obj, *dsr_obj_list_head;
  dsr_obj_t *last_obj = NULL;
  
  dsr_obj_list_head = DSR_GET_OBJECT_LIST(me_obj->context);
  if (me_obj->pending)
  {
    current_obj = dsr_obj_list_head;
    while (current_obj != me_obj && current_obj != NULL)
    {
      /* Not the node - advance to next */
      last_obj = current_obj;
      current_obj = current_obj->next;
    }

    if (current_obj == NULL)
    {
      ERR_FATAL("DSR: Object not present in List. Invalid object or list corrupted", 0, 0, 0);
    }

    if (last_obj == NULL)
    {
      /* My node was at the head of the list */
	  dsr_update_obj_list_head(me_obj->context, me_obj->next);
    }
    else
    {
      /* Set the previous node's next pointer to my next pointer */
      last_obj->next = me_obj->next;
    }
	return TRUE;
  }
  return FALSE;
  
}

static void dsr_isr_init_framework(void)
{
	dsr_thread_attr_t dsr_thread_attr;
	dsr_thread_t *dsr_thread;
	int32 status;
	uint32 i;
	char *task_name;
	
#ifdef DSR_USES_DAL_PROPERTIES
	DALSYSPropertyHandle dsr_prop_handle;
	DALSYSPropertyVar dsr_prop_val;

	// Properties of ISR Workloop thread
    if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(DALDEVICEID_PLATFORM, prop_handle))
    {
      ERR_FATAL("DSR: prop handle failed", 0, 0, 0);
    }

    if (DAL_SUCCESS != DALSYS_GetPropertyValue(prop_handle, NULL, PLATFORM_NUM_DSR_ID, &prop_val))
    {
      ERR_FATAL("DSR: prop val failed", 0, 0, 0);
    }

    dsr_isr_threads = prop_val.Val.dwVal;
    if (dsr_isr_threads == 0)
    {
      ERR_FATAL("DSR: invalid num threads", 0, 0, 0);
    }
	
	status = dsr_get_thread_attr(DSR_THREAD_ID, &dsr_thread_attr);
    if (status != 0)
    {
      ERR_FATAL("DSR: get thread attr failed", 0, 0, 0);
    }
#else
	dsr_num_isr_threads = DSR_THREAD_ISR_COUNT;
#endif

	//Initializing DSR global list for ISR handling
	dsr_obj_list_isr = NULL;

   dsr_thread_list_isr = (dsr_thread_list_t *)malloc(sizeof(dsr_thread_list_t));
   if (dsr_thread_list_isr == NULL)
   {
     ERR_FATAL("DSR: malloc failed", 0, 0, 0);
   }

   dsr_thread_list_isr->dsr_thread_list = (dsr_thread_t *)malloc(sizeof(dsr_thread_t) * dsr_num_isr_threads);
   if (dsr_thread_list_isr->dsr_thread_list == NULL)
   {
     ERR_FATAL("DSR: malloc failed", 0, 0, 0);
   }

   dsr_thread_list_isr->next_thread = 0;

 	//Initialize signal for ISR Thread execution
   qurt_signal_init(&dsr_thread_list_isr->signal);

	task_name = (char *)malloc((sizeof(unsigned char)*DSR_TASKNAME_SIZE));
	if(task_name == NULL)
	{
		ERR_FATAL("DSR: malloc failed", 0, 0, 0);
	}
	// Creation of ISR Workloop threads
   for (i = 0; i < dsr_num_isr_threads; i++)
   {
      dsr_thread = &dsr_thread_list_isr->dsr_thread_list[i];

      dsr_thread->index = i;
      
      dsr_thread->bBusy = FALSE;
	  
	   dsr_thread->context = DSR_CONTEXT_ISR;
	  
      qurt_thread_attr_init(&dsr_thread->thread_attr);
	   memset(task_name, 0, (sizeof(unsigned char)*DSR_TASKNAME_SIZE));
	   snprintf(task_name, (sizeof(unsigned char)*DSR_TASKNAME_SIZE), "DSR_ISR_%d",(int)i);
      qurt_thread_attr_set_name(&dsr_thread->thread_attr, task_name);

	   dsr_thread_attr.thread_prio = rcinit_lookup_prio(task_name);
      qurt_thread_attr_set_priority(&dsr_thread->thread_attr, dsr_thread_attr.thread_prio);

	   dsr_thread_attr.thread_stack_size = rcinit_lookup_stksz(task_name);
      qurt_thread_attr_set_stack_size(&dsr_thread->thread_attr, dsr_thread_attr.thread_stack_size);

      status = qurt_thread_create(&dsr_thread->thread_id,
                                  &dsr_thread->thread_attr,
                                  dsr_workloop,
                                  dsr_thread);
      if (status != QURT_EOK)
      {
        ERR_FATAL("DSR: thread create failed", 0, 0, 0);
      }
    }
	free(task_name);
}

static void dsr_wklp_init_framework(void)
{
	dsr_thread_attr_t dsr_thread_attr;
	dsr_thread_t *dsr_thread;
	int32 status;
	uint32 i;
	char *task_name;
	
#ifdef DSR_USES_DAL_PROPERTIES
	DALSYSPropertyHandle dsr_prop_handle;
	DALSYSPropertyVar dsr_prop_val;

	// Properties of ISR Workloop thread
    if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(DALDEVICEID_PLATFORM, prop_handle))
    {
      ERR_FATAL("DSR: prop handle failed", 0, 0, 0);
    }

    if (DAL_SUCCESS != DALSYS_GetPropertyValue(prop_handle, NULL, PLATFORM_NUM_DSR_ID, &prop_val))
    {
      ERR_FATAL("DSR: prop val failed", 0, 0, 0);
    }

    dsr_isr_threads = prop_val.Val.dwVal;
    if (dsr_isr_threads == 0)
    {
      ERR_FATAL("DSR: invalid num threads", 0, 0, 0);
    }
	
	status = dsr_get_thread_attr(DSR_THREAD_ID, &dsr_thread_attr);
    if (status != 0)
    {
      ERR_FATAL("DSR: get thread attr failed", 0, 0, 0);
    }
#else
	dsr_num_wklp_threads = DSR_THREAD_WORKLOOP_COUNT;
#endif

	//Initializing DSR global list for ISR handling
	dsr_obj_list_wklp = NULL;

    dsr_thread_list_wklp = (dsr_thread_list_t *)malloc(sizeof(dsr_thread_list_t));
    if (dsr_thread_list_wklp == NULL)
    {
      ERR_FATAL("DSR: malloc failed", 0, 0, 0);
    }

    dsr_thread_list_wklp->dsr_thread_list = (dsr_thread_t *)malloc(sizeof(dsr_thread_t) * dsr_num_wklp_threads);
    if (dsr_thread_list_wklp->dsr_thread_list == NULL)
    {
      ERR_FATAL("DSR: malloc failed", 0, 0, 0);
    }

    dsr_thread_list_wklp->next_thread = 0;

	 // Initialize signal for ISR Thread execution
    qurt_signal_init(&dsr_thread_list_wklp->signal);

	 task_name = (char *)malloc((sizeof(unsigned char)*DSR_TASKNAME_SIZE));
	if(task_name == NULL)
	{
		ERR_FATAL("DSR: malloc failed", 0, 0, 0);
	}

	 // Creation of ISR Workloop threads
    for (i = 0; i < dsr_num_wklp_threads; i++)
    {
      dsr_thread = &dsr_thread_list_wklp->dsr_thread_list[i];

      dsr_thread->index = i;
      
      dsr_thread->bBusy = FALSE;
	  
	   dsr_thread->context = DSR_CONTEXT_WORKLOOP;
	  
      qurt_thread_attr_init(&dsr_thread->thread_attr);
	   memset(task_name, 0, (sizeof(unsigned char)*DSR_TASKNAME_SIZE));
      snprintf(task_name, (sizeof(unsigned char)*DSR_TASKNAME_SIZE), "DSR_WORKLOOP_%d",(int)i);
      qurt_thread_attr_set_name(&dsr_thread->thread_attr, task_name);

	   dsr_thread_attr.thread_prio = rcinit_lookup_prio(task_name);
      qurt_thread_attr_set_priority(&dsr_thread->thread_attr, dsr_thread_attr.thread_prio);

	   dsr_thread_attr.thread_stack_size = rcinit_lookup_stksz(task_name);
      qurt_thread_attr_set_stack_size(&dsr_thread->thread_attr, dsr_thread_attr.thread_stack_size);

      status = qurt_thread_create(&dsr_thread->thread_id,
                                  &dsr_thread->thread_attr,
                                  dsr_workloop,
                                  dsr_thread);
      if (status != QURT_EOK)
      {
        ERR_FATAL("DSR: thread create failed", 0, 0, 0);
      }
    }
	
	free(task_name);
	
	qurt_mutex_init(&dsr_mutex_wklp);
}

static void dsr_add_to_list(dsr_obj_t *me_obj, void *payload)
{
	dsr_obj_t *current_obj, *dsr_obj_list_head;
	dsr_obj_t *last_obj = NULL;

    me_obj->payload = payload;
    me_obj->next = NULL;
    me_obj->pending = TRUE;

	dsr_obj_list_head = DSR_GET_OBJECT_LIST(me_obj->context);
	
    if (dsr_obj_list_head == NULL)
    {
      dsr_update_obj_list_head(me_obj->context, me_obj);
    }
    else
    {
      current_obj = dsr_obj_list_head;

      /* insertion sort */
      while (me_obj->priority <= current_obj->priority &&
             current_obj->next != NULL)
      {
        /* advance to the next node */
        last_obj = current_obj;
        current_obj = current_obj->next;
      }

      if (current_obj->next == NULL &&
          me_obj->priority <= current_obj->priority)
      {
        /* insert the new node AFTER the current node because
           we are at the end of the list and the new node's priority
           is less than or equal to the current node */
        current_obj->next = me_obj;
      }
      else
      {
        /* insert new node right BEFORE current node because the
           priority of the new node is higher than the priority of the
           current node */
        if (last_obj == NULL)
        {
          /* new node is now the head of the list */
		  dsr_update_obj_list_head(me_obj->context, me_obj);
        }
        else
        {
           /* update the previous node to point to the new node */
           last_obj->next = me_obj;
        }
        /* new node points to the current node */
        me_obj->next = current_obj;
      }
    }
}


/* Internal Function to handle Enqueue of DSR for ISR */
static boolean dsr_enqueue_to_isr(dsr_obj_t *me_obj, void *payload)
{
	unsigned long interrupt_flag = 0;
	uint32 next_thread = 0;
	boolean queued = FALSE;
	uint32 sig_mask = 0;
	int    i;
  
	INTLOCK_SAVE(interrupt_flag);

	if (!me_obj->pending)
	{
		/* DSR is not in the last - enqueue it */
		queued = TRUE;
		
		dsr_add_to_list(me_obj, payload);

		if (dsr_thread_list_isr != NULL)
		{
		  for (i = 0; i < dsr_num_isr_threads; i++)
		  {
			if (dsr_thread_list_isr->dsr_thread_list[i].bBusy == FALSE )
			{
			  sig_mask = 1 << i;
			  dsr_thread_list_isr->next_thread =i;
			  break;
			}
		  }
		  
		  if (sig_mask == 0x0)
		  {
			next_thread = dsr_thread_list_isr->next_thread;
			dsr_thread_list_isr->next_thread++;

			if (dsr_thread_list_isr->next_thread >= dsr_num_isr_threads)
			{
			  dsr_thread_list_isr->next_thread = 0;
			}
			
			sig_mask = 1<<next_thread;
		  }
		}
	}
	
	INTFREE_RESTORE(interrupt_flag);

	if (queued && dsr_thread_list_isr != NULL)
	{
		qurt_signal_set(&dsr_thread_list_isr->signal, sig_mask);
	}

	return queued;
}

/* Internal Function to handle Enqueue of DSR for Workloop */
static boolean dsr_enqueue_to_workloop(dsr_obj_t *me_obj, void *payload)
{
	uint32 next_thread = 0;
	boolean queued = FALSE;
	uint32 sig_mask = 0;
	int    i;
  
	qurt_mutex_lock(&dsr_mutex_wklp);

	if (!me_obj->pending)
	{
		/* DSR is not in the last - enqueue it */
		queued = TRUE;
		
		dsr_add_to_list(me_obj, payload);

		if (dsr_thread_list_wklp != NULL)
		{
		  for (i = 0; i < dsr_num_wklp_threads; i++)
		  {
			if (dsr_thread_list_wklp->dsr_thread_list[i].bBusy == FALSE )
			{
			  sig_mask = 1 << i;
			  dsr_thread_list_wklp->next_thread =i;
			  break;
			}
		  }
		  
		  if (sig_mask == 0x0)
		  {
			next_thread = dsr_thread_list_wklp->next_thread;
			dsr_thread_list_wklp->next_thread++;

			if (dsr_thread_list_wklp->next_thread >= dsr_num_wklp_threads)
			{
			  dsr_thread_list_wklp->next_thread = 0;
			}
			
			sig_mask = 1<<next_thread;
		  }
		}
	}
	
	qurt_mutex_unlock(&dsr_mutex_wklp);

	if (queued && dsr_thread_list_wklp != NULL)
	{
		qurt_signal_set(&dsr_thread_list_wklp->signal, sig_mask);
	}

	return queued;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
 
/*This function creates a DSR object that can be enqueued later to defer work to thread context. */ 
boolean dsr_create(dsr_obj_t **dsr_obj, dsr_hdlr_t cb, void *cb_ctxt, dsr_priority_t priority, dsr_context_t context)
{
  dsr_obj_t *dsr_obj_local = NULL;

  if (dsr_obj == NULL || cb == NULL)
  {
    return FALSE;
  }

  dsr_obj_local = (dsr_obj_t *)malloc(sizeof(dsr_obj_t));
  if (dsr_obj_local == NULL)
  {
    return FALSE;
  }

  if (priority >= DSR_NUM_PRIO)
  {
	return FALSE;
  }
  
  dsr_obj_local->next = NULL;
  dsr_obj_local->cb = cb;
  dsr_obj_local->cb_ctxt = cb_ctxt;
  dsr_obj_local->payload = NULL;
  dsr_obj_local->priority = priority;
  dsr_obj_local->pending = FALSE;
  dsr_obj_local->context = context;

  *dsr_obj = dsr_obj_local;

  return TRUE;
}

/* This function dequeues a DSR object from the DSR pending list */
boolean dsr_dequeue(dsr_obj_t *me_obj)
{
  unsigned long interrupt_flag = 0;
  boolean status = FALSE;
  if (me_obj == NULL)
  {
    return FALSE;
  }
   
  switch (me_obj->context)
  {
	case DSR_CONTEXT_ISR:
		{
			INTLOCK_SAVE(interrupt_flag);
			status = dsr_remove_from_list(me_obj);
			INTFREE_RESTORE(interrupt_flag);
			break;
		}
	case DSR_CONTEXT_WORKLOOP:
		{
			qurt_mutex_lock(&dsr_mutex_wklp);
			status = dsr_remove_from_list(me_obj);
			qurt_mutex_unlock(&dsr_mutex_wklp);
			break;
		}
	default:
		{
			ERR_FATAL("DSR: Invalid object type",0,0,0);
			break;
		}
	}
	return status;
}

/* This function destroys a DSR object. If the DSR is currently
 * queued then it will be removed from the queue (callback function
 * will not get invoked).
 */
boolean dsr_destroy(dsr_obj_t *me_obj)
{
	if (me_obj == NULL)
	{
		return FALSE;
	}
	
	// The return value is not checked as the object need to be freed even if it is not in queue.
    dsr_dequeue(me_obj);

	free(me_obj);
	
	return TRUE;
}

void dsr_init_framework(void)
{
	dsr_isr_init_framework();
	dsr_wklp_init_framework();
	dsr_init_done = TRUE;
}

boolean dsr_enqueue(dsr_obj_t *me_obj, void *payload)
{
	boolean status = FALSE;
	
	if (dsr_init_done != TRUE)
	{
		return FALSE;
	}

	if (me_obj == NULL)
	{
		return FALSE;
	}
	
	if (me_obj->context == DSR_CONTEXT_ISR)
	{
		status = dsr_enqueue_to_isr(me_obj, payload);
	}
	else if(me_obj->context == DSR_CONTEXT_WORKLOOP)
	{
		status = dsr_enqueue_to_workloop(me_obj, payload);
	}
	else
	{
		ERR_FATAL("DSR: nvalid object type",0,0,0);
	}
	
	return status;
}

boolean dsr_is_dsr_pending(dsr_context_t context)
{
    dsr_obj_t *dsr_obj_list = NULL;
	unsigned long interrupt_flag = 0;
	
	switch (context)
	{
		case DSR_CONTEXT_ISR:
			{
				INTLOCK_SAVE(interrupt_flag);
				dsr_obj_list = DSR_GET_OBJECT_LIST(context);
				INTFREE_RESTORE(interrupt_flag);
				break;
			}
		case DSR_CONTEXT_WORKLOOP:
			{
				qurt_mutex_lock(&dsr_mutex_wklp);
				dsr_obj_list = DSR_GET_OBJECT_LIST(context);
				qurt_mutex_unlock(&dsr_mutex_wklp);
				break;
			}
		default:
			{
				ERR_FATAL("DSR: Invalid object type",0,0,0);
				break;
			}
	}
  
  return (dsr_obj_list!=NULL);
}

