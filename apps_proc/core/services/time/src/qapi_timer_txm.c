
/*==================================================================================================
  Copyright (c) 2017,2019-2020,2022,2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.tx/6.0/services/time/src/qapi_timer_txm.c#5 $

when         who      what, where, why
--------     ---      --------------------------------------------------------
12/14/2016   abhc     Initial version

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qapi_types.h"
#include "qapi_status.h"
#include "qurt_txm_qapi_fwk.h"
#include "qapi_timer.h"
#include "qapi_timer_id.h"
#include "timer.h"
#include "comdef.h"
#include "stringl.h"
#include "err.h"
#include "timer_v.h"
#include "time_svc.h"
#include "tx_event_flags.h"
#include "osal_qurt_types.h"
#include "osal.h"
#include "txm_module.h"
#include "qapi_timer_internal.h"
#include "qurt_txm_uid.h"
#include "time_genoff_v.h"

/*==================================================================================================
                                              MACROS
==================================================================================================*/
/* Assumes two incoming variables are expected to be uint32 and
combines them into 64bit */
#define COMBINE64(a,b) ((((unsigned long long)((unsigned int)(a)))<<32)\
						   |(((unsigned long long)((unsigned int)(b)))<< 0))

/*Signal mask to be set for user space thread*/
#define TIMER_SIGNAL_TO_USER_THREAD 0x1
#define MAX_TIMER_TABLE_SIZE      150
/*===========================================================================
                      GLOBAL STRUCTURE
===========================================================================*/

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

extern uint32 timer_root_handle;
/* This is the head of the list of free node of timers . A free node gets
   added to the list when qapi_Timer_Undef is called.*/
qapi_free_timer_node *head = NULL; 

/* This index tells at which index the incoming timer pointer will be kept 
   in the qapi timer table when qapi_Timer_Def is called */
uint16 qapi_timer_table_index = 0;  // Current value of index in timer table
uint16 timer_expired_table_type_idx =0;
uint16 timer_freed_table_type_idx=0;
/* This is the table which will contain the timer pointer and the corresponding 
   module instance */
qapi_timer_table_type qapi_timer_table[QAPI_MAX_TIMER_TABLE_SIZE]; // Table for storing pointers to qapi timer structs

/* This mutex is used in _qapi_Timer_Def to protect the global variable
   qapi_timer_table_index. */
osal_mutex_arg_t qapi_timer_mutex;

typedef struct
{
  timer_type* timer_exp_ptr; 
  timer_expire_data *timer_exp_data;
  
}timer_expired_table_type;

typedef struct
{
  timer_expire_data  *timer_freed; 
  qapi_TIMER_get_cbinfo_t    timer_expired_cbinfo;
  
}timer_freed_table_type;
timer_expired_table_type timer_expired_table[MAX_TIMER_TABLE_SIZE];
timer_freed_table_type timer_freed_table[MAX_TIMER_TABLE_SIZE];

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/* This function inserts a free node into the free node list of timers
   when qapi_Timer_Undef is called. */
qapi_TIMER_err_type _qapi_insert_timer_free_node(uint16 index);

/* This function removes a free node from the list of free nodes and
   inserts a timer pointer to the corresponding index in the qapi timer
   table when qapi_timer_Def is called  and qapi_timer_table_index reaches
   the maximum limit.*/
int _qapi_remove_timer_free_node(void);

/* This function returns the index in qapi timer table from the unique
   timer handle integer */
int _qapi_get_timer_index(uint32 timer_handle);

/* This function accepts the unique integer id from the client and
   returns the timer pointer from the qapi timer table */
timer_ptr_type _qapi_timer_validate(uint32 temp_id);


/*==================================================================================================
                                        FUNCTION DEFINITIONS
==================================================================================================*/



/*=============================================================================
 * Handles  the notification for user module QAPI timers..
 *
 * This function will be invoked by the kernel ATS Timer DAM driver.
 * 
 * @param[in]   request_ID      request_ID passed by userspace QAPI
 * @arg1/12[in] arg1 to arg12   passed by userspace QAPI                         
 * 
 * @return
 * QAPI_OK      Handling of requested ID is successful. \n
 * QAPI_ERROR   Handling of requested ID failed.
 * 
 * 
 * N.B
 * Handling only the native signalling type of notification right now.
 * It will be chaneged to handle other notifications as well later.
 * For now the callbacks will still get handled in the kernel space
 * timer module only.
 =============================================================================*/
void timer_qapi_notification_handler
(
	timer_ptr_type timer_ptr
)
{
  timer_expire_data *timer_q_data = NULL;

  if(timer_ptr == NULL)
  	return ;
  
  /* Only handling NATIVE OS notification type for now  */
  switch(timer_ptr->notify_type) {

	case TIMER_NATIVE_OS_SIGNAL_TYPE:
		 tx_event_flags_set((TX_EVENT_FLAGS_GROUP *)timer_ptr->sigs_obj, timer_ptr->sigs, TX_OR);
		 break;

	case TIMER_FUNC1_CB_TYPE:
         timer_q_data = (timer_expire_data *)malloc(sizeof(timer_expire_data));

         if(timer_q_data == NULL)
           return;

     	 q_link(timer_q_data,&timer_q_data->link);
		 
		 /*Putting expired timer data over queue*/
		 timer_q_data->timer_cbinfo.func_ptr = timer_ptr->func1;
	     timer_q_data->timer_cbinfo.data = timer_ptr->data; 
		 
		 timer_expired_table[timer_expired_table_type_idx].timer_exp_ptr = timer_ptr;
		 timer_expired_table[timer_expired_table_type_idx].timer_exp_data = timer_q_data;
		 timer_expired_table_type_idx++;
		 if(timer_expired_table_type_idx >= QAPI_MAX_TIMER_TABLE_SIZE)
		 {
			 timer_expired_table_type_idx=0;
		 }
		 
	     q_put(&(((timer_txmod_obj*)(timer_ptr->timer_qapi_proc_hdle))->queue),&timer_q_data->link);
	
	     /*After putting the data on q set signal to user thread.*/
		 qurt_signal_set(&(((timer_txmod_obj*)(timer_ptr->timer_qapi_proc_hdle))->sig_obj), ((timer_txmod_obj*)(timer_ptr->timer_qapi_proc_hdle))->signs);	
	
	case TIMER_INVALID_NOTIFY_TYPE:
	default:
		//NO ACTION ON OTHER NOTIFICATION TYPE FOR NOW
		break;
  	}

  return;
}


/*=============================================================================
_qapi_Timer_Def TXM DRIVER IMPLEMENTATION
*
* This function will allocate a timer pointer and add it to the qapi timer table. 
* If the current index of the table exceeds the max size then it checks the list
* of free indices, removes the head of thr list and puts the timer pointer in the
* index of the qapi timer table which is pointed to by the head. If there are no 
* nodes in the list of free indices, it returns error. It assigns a unique integer 
* to the timer which is sent as argument for the other qapi calls as timer_handle 
* by the client.
*
* 
* @param[in]   1)timer_handle, unique integer for any timer
*              2)timer_attr, timer attributes
*              3)timer_qapi_proc_hdle
*              4)timer_qapi_proc_id, determines whether the invocation 
*                is from kernelspace or userspace
* 
* @return	   1) QAPI_OK: This is returned if the timer got defined successfully and the timer pointer got
                  added to the qapi timer table.
               2) QAPI_ERR_INVALID_PARAM: This is returned if timer_handle is NULL or timer_attr is NULL or
                  timer_qapi_proc_hdle is 0.
               3) QAPI_ERR_NO_MEMORY: Returned when there is no memory in heap to allocate the timer pointer
                  or there is no more space left in the qapi timer table.
               3) QAPI_ERROR: Returned if there is any error other than the ones above.

==============================================================================================================*/
qapi_Status_t _qapi_Timer_Def(
   qapi_TIMER_handle_t          *timer_handle,
   qapi_TIMER_define_attr_t     *timer_attr,
   uint32                       timer_qapi_proc_hdle,
   timer_qapi_process           timer_qapi_proc_id
)
{
  timer_type          *timer_ptr              = NULL;
  timer_group_type    *timer_group_local_ptr  = NULL;
  timer_notify_type   timer_notify_type_local = TIMER_NO_NOTIFY_TYPE;
  qapi_Status_t       status                  = QAPI_ERROR;
  TXM_MODULE_INSTANCE *module_instance;
  int copied_bytes = 0;
  boolean ret_val;
  uint32 timer_id = 0;
  int free_node_index = 0;
  int qurt_ret_val = 0;
    
  if(NULL == timer_handle || NULL == timer_attr || 0 == timer_qapi_proc_hdle)
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  /* Allocate the memory for timer handle*/ 
  	timer_ptr = (timer_type *)malloc(sizeof(timer_type));


  if(NULL == timer_ptr)
  {
    return QAPI_ERR_NO_MEMORY;
  }

  /* Memset the timer ptr to zero */
  memset(timer_ptr,0,sizeof(timer_type));

  /* Set the process ID */
  timer_ptr->timer_qapi_proc_hdle = timer_qapi_proc_hdle;
  timer_ptr->timer_qapi_proc_id   = timer_qapi_proc_id;

  /* Set the group local pointer */
  if (timer_attr->deferrable == TRUE)
  	timer_group_local_ptr = &timer_null_group;
  else
  	timer_group_local_ptr = &timer_non_defer_group;

  /* Get the callback notification type */
  switch (timer_attr->cb_type)
  	{
  	case QAPI_TIMER_NO_NOTIFY_TYPE:
		timer_notify_type_local = TIMER_NO_NOTIFY_TYPE;
		break;

  	case QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE:
		timer_notify_type_local = TIMER_NATIVE_OS_SIGNAL_TYPE;
		break;


	case QAPI_TIMER_FUNC1_CB_TYPE:
		timer_notify_type_local = TIMER_FUNC1_CB_TYPE;
		break;

	case QAPI_TIMER_INVALID_NOTIFY_TYPE:
    default:
		    timer_notify_type_local = TIMER_INVALID_NOTIFY_TYPE;
		    break;

  	}

  if(TE_SUCCESS == timer_def_osal(timer_ptr,
  						timer_group_local_ptr,
	  					timer_notify_type_local,
	  					timer_attr->sigs_func_ptr,
	  					timer_attr->sigs_mask_data))
  {
    osal_lock_mutex(&qapi_timer_mutex); // This is for protecting qapi_timer_table_index
	/* Case where the index has reached max limit. Check if there is any free node. */
    if(qapi_timer_table_index>=QAPI_MAX_TIMER_TABLE_SIZE)
    {
       if(!head) // There are no free nodes
       {
         osal_unlock_mutex(&qapi_timer_mutex); // Unlock mutex before returning
         timer_undef(timer_ptr); // undefining the timer
         free(timer_ptr);
         return QAPI_ERROR; // No space left in table
       }
	   
       free_node_index = _qapi_remove_timer_free_node(); // Get the index of free node at the head
       if(free_node_index==QAPI_TIMER_NO_FREE_NODE)
       {
         osal_unlock_mutex(&qapi_timer_mutex); // Unlock mutex before returning
         timer_undef(timer_ptr); // undefining the timer
         free(timer_ptr);
         return QAPI_ERR_NO_MEMORY;
       }	
       /* Case where there is a free node and we got a valid index */
       // Timer pointer or module instance at the index is not NULL. Return error.
       if(qapi_timer_table[free_node_index].timer_ptr || qapi_timer_table[free_node_index].module_instance) 
       {
         osal_unlock_mutex(&qapi_timer_mutex); // Unlock mutex before returning
         timer_undef(timer_ptr); // undefining the timer
         free(timer_ptr);
         return QAPI_ERROR;
       }

	   qurt_ret_val = qurt_txm_module_instance_get(&module_instance);  // Getting the pointer to the pointer of module instance
       if(qurt_ret_val!=QURT_EOK) // Did not get the module instance id. Return error
       {
         osal_unlock_mutex(&qapi_timer_mutex); // Unlock mutex before returning
         timer_undef(timer_ptr); // undefining the timer
         free(timer_ptr);
         return QAPI_ERROR;
       }	 
	   qapi_timer_table[free_node_index].timer_ptr = timer_ptr; // Storing the timer pointer
       qapi_timer_table[free_node_index].module_instance = (TXM_MODULE_INSTANCE*)module_instance; // Saving the module instance 
       timer_id = QAPI_TIMER_INDEX_MASK+free_node_index; /* This value will be sent to the userspace client
                                                              and will uniquely identify a userspace timer */
    }
    else
    {
      qurt_ret_val = qurt_txm_module_instance_get(&module_instance);  // Getting the pointer to the pointer of module instance

      if(qurt_ret_val!=QURT_EOK) // Did not get the module instance id. Return error
      {
        osal_unlock_mutex(&qapi_timer_mutex); // Unlock mutex before returning
        timer_undef(timer_ptr); // undefining the timer
        free(timer_ptr);
        return QAPI_ERROR;
      }	

	  qapi_timer_table[qapi_timer_table_index].timer_ptr = timer_ptr; // Storing the timer pointer at the index
      qapi_timer_table[qapi_timer_table_index].module_instance = (TXM_MODULE_INSTANCE*)module_instance; // Saving the module instance 
      timer_id = QAPI_TIMER_INDEX_MASK+qapi_timer_table_index; /* This value will be sent to the userspace client
                                                                    and will uniquely identify a userspace timer */
      qapi_timer_table_index++; // Incrementing the index of the qapi_timer_table
    }
    osal_unlock_mutex(&qapi_timer_mutex); // Releasing the mutex here. This is common for both if success case and else */
	
    if(timer_qapi_proc_id == TIMERS_QAPI_FROM_USER) // For userspace
    {
      /* Returning the handle i.e. the unique id to userspace */
      if(QURT_EOK != qurt_copy_to_user((void*)timer_handle, sizeof(qapi_TIMER_handle_t),
                                    (void*)&timer_id, sizeof(uint32)))
        return QAPI_ERROR;
    }
    else if(timer_qapi_proc_id == TIMERS_QAPI_FROM_ROOT)// For kernel space
    {
      copied_bytes = memscpy((void*)timer_handle, sizeof(qapi_TIMER_handle_t),
                              (void*)&timer_id, sizeof(uint32));
      if(copied_bytes!=sizeof(uint32)) // Verifying the number of bytes copied
        return QAPI_ERROR;
    }
    else // Ideally this else should never be hit
    {
      return QAPI_ERROR; 
    }
    status = QAPI_OK; // Timer has been successfully created
  }
  else
  {
        free(timer_ptr);
        timer_ptr = NULL;
        /* Returning timer handle as NULL to user space */
        if(timer_qapi_proc_id == TIMERS_QAPI_FROM_USER)
        {
          if(QURT_EOK != qurt_copy_to_user((void*)timer_handle, sizeof(qapi_TIMER_handle_t),
                                      (void*)&timer_ptr, sizeof(uint32*)))
            return QAPI_ERROR;
        }
        else if(timer_qapi_proc_id == TIMERS_QAPI_FROM_ROOT)// For kernel space
        {
           memscpy((void*)timer_handle, sizeof(qapi_TIMER_handle_t),
                                 (void*)&timer_ptr, sizeof(uint32*)); // Copying NULL to timer_handle in kernel space
           /* Not verifying the length copied as QAPI_ERROR will be returned in any case */
        }

        status = QAPI_ERROR;  // Timer could not be created
  }

  return status; 
}

/*=========================================================================================================================
qapi_Timer_Undef TXM DRIVER IMPLEMENTATION
*
* This function will be invoked after the timer initialization
* from kernel module is completed.This function will be invoked 
* by qapi_Timer_Undef. From the timer handle it will get the 
* index in the qapi timer table, make that entry to be NULL and
* add a node to the list of free indices.
*
* 
* @param[in]   timer_handle
* 
* @return	   1) QAPI_OK: This is returned if the timer was in an active state and got undefined. Happens
                  when timer_undef returns TE_SUCCESS.
               2) QAPI_ERR_INVALID_STATE: This is returned if the timer was already suspended and got undefined. Happens
                  when timer_undef returns TE_TIMER_ALREADY_IN_SAME_STATE.
               3) QAPI_ERR_INVALID_PARAM: This is returned if the timer handle is NULL.
               3) QAPI_ERROR: This is returned when any error code other than TE_SUCCESS and TE_TIMER_ALREADY_IN_SAME_STATE
                  is returned by timer_undef.

============================================================================================================================*/
qapi_Status_t _qapi_Timer_Undef(
    qapi_TIMER_handle_t    timer_handle
)
{
  qapi_Status_t status = QAPI_ERROR;
  timer_error_type return_val;
  timer_ptr_type timer_ptr = NULL;
  qapi_TIMER_err_type insert_node_r_val;
  int qapi_timer_index = 0;
  uint32 temp_id = 0;
  
  
  if(NULL ==  timer_handle) 
  {
    return QAPI_ERR_INVALID_PARAM;  
  }

  temp_id = (uint32)timer_handle; // Type casting to get the unique timer id

  timer_ptr = _qapi_timer_validate(temp_id);
    
  if(!timer_ptr)
  {
    return QAPI_ERROR;
  }	

  return_val = timer_undef((timer_ptr_type)timer_ptr);

  if(TE_SUCCESS == return_val)
  {
    status = QAPI_OK;
  }

  /* This will be true if the timer was already in suspended state. But still the timer will be undefined.
     This is not a failure case. */
  if(TE_TIMER_ALREADY_IN_SAME_STATE == return_val) 
  {
    status = QAPI_ERR_INVALID_STATE;
  }

  free(timer_ptr);

  qapi_timer_index = _qapi_get_timer_index(temp_id); // Getting the index in qapi_timer_table
  qapi_timer_table[qapi_timer_index].timer_ptr = NULL; // Making the timer pointer NULL in the table
  qapi_timer_table[qapi_timer_index].module_instance = NULL; // Making the module instance id pointer as NULL
  insert_node_r_val = _qapi_insert_timer_free_node(qapi_timer_index); /* Inserting a node in the free list to mark that
                                                                         this index is free */
  if(insert_node_r_val == QAPI_TIMER_ERR_INSERT_NODE)
  {
    status = QAPI_ERROR;
  }	
  return status;
}

/*=============================================================================
qapi_Timer_Get_usrhdl TXM DRIVER IMPLEMENTATION
=============================================================================*/
qapi_Status_t _qapi_Timer_Get_usrhdl(
	uint32    *timer_user_handle
)
{
  timer_txmod_obj     *timer_user = NULL;
  
  if (timer_user_handle == NULL)
  	return QAPI_ERR_INVALID_PARAM;

  timer_user = (timer_txmod_obj*)malloc(sizeof(timer_txmod_obj));
  
  if(timer_user == NULL)
   return QAPI_ERR_NO_MEMORY;

  q_init(&timer_user->queue);
  qurt_signal_init(&timer_user->sig_obj);

  timer_user->signs = TIMER_SIGNAL_TO_USER_THREAD;
  
  /*Copying handle to user space*/
  if(QURT_EOK != qurt_copy_to_user((void*)timer_user_handle, 
                       sizeof(uint32), (void*)&timer_user, sizeof(uint32)))
     return QAPI_ERROR;

  return QAPI_OK;
}

/*=============================================================================
qapi_Timer_Get_Cbinfo TXM DRIVER IMPLEMENTATION
=============================================================================*/
qapi_Status_t _qapi_Timer_Get_Cbinfo(
	uint32                     timer_user_handle,
    qapi_TIMER_get_cbinfo_t    *timer_cbinfo
)
{
  timer_expire_data        *temp          = NULL;
  q_type                   *temp_q        = NULL;
  ULONG                    c_actual_flags = 0;
  qapi_TIMER_get_cbinfo_t  temp_cbinfo    = {0};
  timer_txmod_obj*         user_handle;

  /*making a local copy of user handle*/
  user_handle = (timer_txmod_obj* )timer_user_handle;
  
  if (NULL == timer_cbinfo || 0 == timer_user_handle)
  	return QAPI_ERR_INVALID_PARAM;

  temp_q = &user_handle->queue;
  
  if(temp_q->cnt == 0)
    { 
       /*Waiting for expired timer data to be put on queue*/ 
       qurt_signal_wait(&((user_handle)->sig_obj),TIMER_SIGNAL_TO_USER_THREAD,QURT_SIGNAL_ATTR_WAIT_ANY);
    }

  qurt_signal_clear(&((user_handle)->sig_obj), TIMER_SIGNAL_TO_USER_THREAD);
  /*Getting out data out of queue to be processed*/
  temp = (timer_expire_data*)q_get(temp_q);
  
  /* Check for NULL from q_get */
  if ( temp == NULL )
  {
	return QAPI_ERROR;
  }
  
  temp_cbinfo= temp->timer_cbinfo;
  timer_freed_table[timer_freed_table_type_idx].timer_freed = temp;
  timer_freed_table[timer_freed_table_type_idx].timer_expired_cbinfo= temp_cbinfo;
  timer_freed_table_type_idx++;
  if(timer_freed_table_type_idx >= QAPI_MAX_TIMER_TABLE_SIZE)
  {
	timer_freed_table_type_idx=0;
  }
  free(temp);
	 
  /*Copy to user API to copy temp_cbinfo to timer_cbinfo*/
  if(QURT_EOK != qurt_copy_to_user((void*)timer_cbinfo, sizeof(qapi_TIMER_get_cbinfo_t), 
                            (void*)&temp_cbinfo, sizeof(qapi_TIMER_get_cbinfo_t)))
       return QAPI_ERROR;

  return QAPI_OK;
}

/*=============================================================================
qapi_time_get TXM DRIVER IMPLEMENTATION
=============================================================================*/
qapi_Status_t _qapi_time_get(
	qapi_time_unit_type  time_get_unit,
	qapi_time_get_t*     time,
	timer_qapi_process   time_qapi_proc_id
)
{
	qapi_Status_t status = QAPI_ERROR;
	qapi_time_get_t time_local = { 0 };

	/* Perform input validations */ 
	if ( (NULL ==  time) ||
			(time_get_unit < QAPI_TIME_STAMP) ||
			(time_get_unit > QAPI_TIME_JULIAN) ) {
		return QAPI_ERR_INVALID_PARAM;
	}

	// So that we don't have to update in every switch-case
	status = QAPI_OK;

	switch (time_get_unit) {

	case QAPI_TIME_STAMP:
		time_get(time_local.time_ts);
		break;
	case QAPI_TIME_MSECS:
	    time_local.time_msecs=(uint64_t)time_get_ms_native();
		break;
	case QAPI_TIME_SECS:
		time_local.time_secs=(uint64_t)time_get_secs();
		break;
	case QAPI_TIME_JULIAN:
		time_get_julian((time_julian_type *)&time_local.time_julian);
		break;
	default:
		// We shouldn't be reaching here since we already did input validation.
		status = QAPI_ERROR;
	}

	if (time_qapi_proc_id == TIMERS_QAPI_FROM_USER) {
		
        /*Copying data to user space*/
		if(QURT_EOK != qurt_copy_to_user((void*)time, sizeof(qapi_time_get_t),
                                    (void*)&time_local, sizeof(qapi_time_get_t)))
	          return QAPI_ERROR;
     }
	else {
		// Copying from one buffer to another in kernel
		memscpy((void*)time,sizeof(qapi_time_get_t),(void*)&time_local,sizeof(time_local));
	}

	return status;
}

/*=============================================================================
_qapi_get_timer_inaccuracy TXM DRIVER IMPLEMENTATION
=============================================================================*/
qapi_Status_t _qapi_get_timer_inaccuracy(
	qapi_time_genoff_args_ptr    pargs,
	timer_qapi_process   time_qapi_proc_id
)
{
  qapi_Status_t status = QAPI_ERROR;
  time_type ts_val_ms;
  time_genoff_args_struct_type temp_genoff_args;  

  /* Perform input validations */ 
  if ( pargs->base != QAPI_ATS_PSM || pargs->operation != QAPI_T_GET ||  pargs->unit != QAPI_TIME_MSECS)
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  // So that we don't have to update
  status = QAPI_OK;

  /* Get Current Time in Genoff in msecs */
  temp_genoff_args.base                     = ATS_PSM;
  temp_genoff_args.base_source              = TIME_SCLK;
  temp_genoff_args.ts_val                   = ts_val_ms;
  temp_genoff_args.unit                     = TIME_MSEC;
  temp_genoff_args.operation                = T_GET;
  temp_genoff_args.dal_timetick_val         = 0;
  temp_genoff_args.dal_timetick_val_valid   = 0;
  time_genoff_opr( &temp_genoff_args );

  if (time_qapi_proc_id == TIMERS_QAPI_FROM_USER) {
     /*Copying data to user space*/
  	 if(QURT_EOK != qurt_copy_to_user((void*)pargs->ts_val, sizeof(time_type),
                                     (void*)temp_genoff_args.ts_val, sizeof(time_type)))
            return QAPI_ERROR;
  }
  else {
  	// Copying from one buffer to another in kernel
  	memscpy((void*)pargs->ts_val, sizeof(time_type), (void*)temp_genoff_args.ts_val, sizeof(time_type));
  }

  return status;
}

/*==================================================================================
 * This function retrieves the index in qapi_timer_table for the given timer handle
 * 
 * @param[in]   timer_handle   Unique value that identifies a timer
 *                      
 * 
 * @return
 * The index in qapi_timer_table for the timer
 * QAPI_TIMER_INVAL_INDEX if the index is greater than the max number of timers supported 
 ===================================================================================*/

int _qapi_get_timer_index(uint32 timer_handle)
{
  uint32 index = timer_handle - QAPI_TIMER_INDEX_MASK; /* Getting back the index by subtracting the mask*/
  if(index>=QAPI_MAX_TIMER_TABLE_SIZE)
  	return QAPI_TIMER_INVAL_INDEX;
  return index;
}


/*======================================================================================
 * This function inserts a node to the linked list which maintains list of free indices
 *
 * This function will be invoked by _qapi_Timer_Undef
 *  
 * 
 * @param[in]   index     Index of the timer in qapi_timer_table
 *                      
 * 
 * @return
 * QAPI_TIMER_SUCCESS_INSERT_NODE if the timer node got added successfully
 * QAPI_TIMER_ERR_INSERT_NODE otherwise
 =======================================================================================*/
qapi_TIMER_err_type _qapi_insert_timer_free_node(uint16 index)
{
  qapi_free_timer_node *new_node = NULL;  // New node that will be added to list

  new_node = (qapi_free_timer_node*)malloc(sizeof(qapi_free_timer_node));
  if(new_node) // Adding the node at the beginning of the list and making it as head
  {
    new_node->index = index;
    new_node->next = head;
	head = new_node;
	return QAPI_TIMER_SUCCESS_INSERT_NODE;
  }
  return QAPI_TIMER_ERR_INSERT_NODE; // Malloc failed, check for heap exhaustion.
}

/*=============================================================================
 * This function removes the head of the list which maintains the free indices
 *
 *
 * This function will be invoked by _qapi_Timer_Def
 *  
 * 
 * @param[in]   None
 *                      
 * 
 * @return
 * The index of the head node
 * QAPI_TIMER_NO_FREE_NODE if head is NULL
 =============================================================================*/


int _qapi_remove_timer_free_node()
{
  int ret_val = QAPI_TIMER_NO_FREE_NODE;
  if(head)
  {
    qapi_free_timer_node *copy_head = head;
	head = head->next; // Moving head of list to the next node
	ret_val = copy_head->index; // The index of the head will be returned
	free(copy_head); // Freeing head
  }
  return ret_val;
}

/*=============================================================================
 * This function accepts the unique integer which client passes and then gets
 * the actual timer pointer from qapi timer table. It performs checks to get
 * a valid timer pointer
 *
 *
 * This function will be invoked by all the qapi-s
 *  
 * 
 * @param[in]   temp_id, unique integer passed by client which serves as timer
 *              handle
 *                      
 * 
 * @return
 * The timer pointer from qapi timer table
 * NULL if a valid timer pointer cannot be returned
 =============================================================================*/


timer_ptr_type _qapi_timer_validate(uint32 temp_id)
{
  int qapi_timer_index = 0;
  int qurt_ret_val = 0;
  timer_ptr_type timer_ptr = NULL;
  TXM_MODULE_INSTANCE* module_instance;
  
  qapi_timer_index = _qapi_get_timer_index(temp_id);
  
  if(qapi_timer_index==QAPI_TIMER_INVAL_INDEX)
	return NULL;  
  
  qurt_ret_val = qurt_txm_module_instance_get(&module_instance);  // Getting the pointer to the pointer of module instance

  if(qurt_ret_val!=QURT_EOK) // Did not get the module instance id
    return NULL;

  
  /* This is the case for suspected malicious activity. qapi_timer_Set can be called with a timer handle
	 which is within 0 to QAPI_MAX_TIMER_TABLE_SIZE but the client had received a different handle from
	 qapi_timer_Def call. In other words the client is passing a different handle while calling into the
	 qapi rather than the one it received from qapi_Timer_Def. This case is caught here.*/
  if(qapi_timer_table[qapi_timer_index].module_instance != module_instance)
    return NULL;
 

  timer_ptr = (timer_ptr_type)qapi_timer_table[qapi_timer_index].timer_ptr;

  if(!timer_ptr) // There is no timer pointer at this index
    return NULL;
  
  
  return timer_ptr;
}


/*=============================================================================
 * Handles  the kernel switching from the user space TIMER API's.
 *
 * This function will be invoked by the Threadx DAM layer
 *  as part of user space to kernel space switching
 * 
 * @param[in]   request_ID      request_ID passed by userspace QAPI
 * @arg1/12[in] arg1 to arg12   passed by userspace QAPI                         
 * 
 * @return
 * QAPI_OK      Handling of requested ID is successful. \n
 * QAPI_ERROR   Handling of requested ID failed.
 =============================================================================*/
qapi_Status_t timer_qapi_handler(	UINT request_ID, 
											UINT arg1, 
											UINT arg2, 
											UINT arg3, 
											UINT arg4, 
											UINT arg5, 
											UINT arg6, 
											UINT arg7, 
											UINT arg8, 
											UINT arg9, 
											UINT arg10, 
											UINT arg11, 
											UINT arg12)
{
  qapi_Status_t result = QAPI_OK;
  switch(request_ID) {
	case TXM_QAPI_TIMER_DEF:
		result = _qapi_Timer_Def((qapi_TIMER_handle_t*) arg1, (qapi_TIMER_define_attr_t*) arg2,(uint32) arg3, TIMERS_QAPI_FROM_USER);
		break;
	case TXM_QAPI_TIMER_SET:
		result = qapi_Timer_Set((qapi_TIMER_handle_t) arg1, (qapi_TIMER_set_attr_t*) arg2);
		break;
	case TXM_QAPI_TIMER_STOP:
		result = qapi_Timer_Stop((qapi_TIMER_handle_t) arg1);
		break;
	case TXM_QAPI_TIMER_UNDEF:
		result = _qapi_Timer_Undef((qapi_TIMER_handle_t*) arg1);
		break;
	case TXM_QAPI_TIMER_GET_INFO:
		result = qapi_Timer_Get_Timer_Info((qapi_TIMER_handle_t) arg1, (qapi_TIMER_get_info_attr_t*) arg2, (uint64 *) arg3);
		break;
	case TXM_QAPI_TIMER_SLEEP:
		result = qapi_Timer_Sleep((uint64_t)COMBINE64(arg1,arg2), (qapi_TIMER_unit_type)arg3, (qbool_t)arg4);
		break;
	case TXM_QAPI_TIME_GET:
		result = _qapi_time_get((qapi_time_unit_type)arg1, (qapi_time_get_t*)arg2, TIMERS_QAPI_FROM_USER);
		break;
  case TXM_QAPI_GET_TIMER_INACCURACY:
		result = _qapi_get_timer_inaccuracy((qapi_time_genoff_args_ptr)arg1, TIMERS_QAPI_FROM_USER);
		break;
	case TXM_QAPI_TIMER_GET_CBINFO:
		result = _qapi_Timer_Get_Cbinfo((uint32)arg1,(qapi_TIMER_get_cbinfo_t *)arg2);
		break;
	case TXM_QAPI_TIMER_GET_USER_HANDLE:
		result = _qapi_Timer_Get_usrhdl((uint32*)arg1);
		break;
	default:
	    result = QAPI_ERROR;
	    break;
  }
  return result;
}

/*=============================================================================
 * This function returns the QAPI request ID for qapi_timer_sleep.
 * 
 * @param[in]   None                        
 * 
 * @return
 * QAPI request ID for qapi_timer_sleep
 =============================================================================*/
uint32 query_timer_sleep_qapi_id()
{
	return TXM_QAPI_TIMER_SLEEP;
}


