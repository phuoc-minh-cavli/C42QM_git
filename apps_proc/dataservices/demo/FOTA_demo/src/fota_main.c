/*==============================================================================

                             FOTA_MAIN.C

GENERAL DESCRIPTION
  This file deals with the task Initialization to support FOTA object mechanism

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The task init and task start routines do the work necessary to ensure that
  all data structures are in the desired state to continue with normal
  operation.

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------

==============================================================================*/

#include "fota_app.h"


fota_obj_info_t          fota_obj_param;
fota_obj_task_cmd_q      fota_obj_cmdq;
extern qapi_Coap_Session_Hdl_t coap_handle;
/*=============================================================================

FUNCTION FOTA_OBJ_SET_SIGNAL

DESCRIPTION
  Set the fota object signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void fota_obj_set_signal(void)
{
  LOG_INFO("FOTA_LOG: fota_obj_set_signal");

  qurt_mutex_lock(&fota_obj_param.fota_obj_mutex);

  FOTA_OBJ_TASK_SET_SIG(fota_obj_param.fota_obj_signal,FOTA_OBJ_SIGNAL_MASK);

  qurt_mutex_unlock(&fota_obj_param.fota_obj_mutex);

  return;
}

/*=============================================================================

FUNCTION FOTA_OBJ_MUTEX_INIT

DESCRIPTION
  fota object mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void fota_obj_mutex_init(void)
{
  LOG_INFO("FOTA_LOG: fota_obj_mutex_init");

  /* allocate mutex required for thread*/ 
  qurt_mutex_init(&fota_obj_param.fota_obj_mutex);
  return;
}


/*=============================================================================

FUNCTION FOTA_OBJ_SIGNAL_INIT

DESCRIPTION
  FOTA Object Signal Initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void fota_obj_signal_init(void)
{
  LOG_INFO("FOTA_LOG: fota_obj_signal_init");
  /* allocate signal required for thread*/ 
  qurt_signal_init(&fota_obj_param.fota_obj_signal);
  return;
}


/*=============================================================================

FUNCTION FOTA_OBJ_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the FOTA_OBJ signal.
  The FOTA_OBJ signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void fota_obj_signal_wait(void)
{
  uint32_t received_sigs = 0;

  LOG_INFO("FOTA_LOG: fota_obj_signal_wait");

  while (1)
  {
/*============================================================================
  Wait for the FOTA_OBJ Signal to be set
=============================================================================*/
    received_sigs = qurt_signal_wait(&fota_obj_param.fota_obj_signal, FOTA_OBJ_SIGNAL_MASK,
                     QURT_SIGNAL_ATTR_WAIT_ANY| QURT_SIGNAL_ATTR_CLEAR_MASK);

    if (FOTA_OBJ_TASK_IS_SIG_SET(received_sigs, FOTA_OBJ_SIGNAL_MASK))
    {
      // FOTA_OBJ Signal is set Dequeue the FOTA_OBJ Command queue
      fota_obj_process_commands();
    }
  }
}


/*================================================================

FUNCTION FOTA_APP_START

DESCRIPTION
  Fota object Task initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void fota_app_start(void)
{

  LOG_INFO("FOTA_LOG: fota_app_start ");
/*============================================================================
  Reseting the Global SEN_OBJ Structure
=============================================================================*/

  memset(&fota_obj_param, 0x00, sizeof(fota_obj_info_t));

/*----------------------------------------------------------------------------
  Initialize the command queues and set the command queue pointers in the
   fota_obj_info struct to the appropriate values.
----------------------------------------------------------------------------*/

  fota_obj_cmdq_init(&fota_obj_param);

}

/*=============================================================================

FUNCTION FOTA_APP_ENTRY

DESCRIPTION
  This is the entry point for Fota app task
  Initalize the mutex
  Initalize the signal

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void fota_app_entry(void *thread_ctxt)
{
  LOG_INFO("FOTA_LOG: fota_app_entry");

/*============================================================================
  Handshake with RCInit
    Indicate to the Task start framework that we have completed the task start
  procedures.

  If any synchronization is needed with other tasks during the task start time
  we should send an event indicating what we wish to communicate.
=============================================================================*/

  rcinit_handshake_startup();

/*=============================================================================
Initialize Mutex Object
==============================================================================*/

  fota_obj_mutex_init();

/*=============================================================================
Initialize Signal 
==============================================================================*/

  fota_obj_signal_init();

/*=============================================================================
  Register the fota app with LwM2M client
  ==============================================================================*/

  Initialize_Fota_App();

/*=============================================================================
  Wait on the Signal
==============================================================================*/
  fota_obj_signal_wait();

}

/*===========================================================================

FUNCTION FOTA_OBJ_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  fota_obj_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void fota_obj_cmdq_init(fota_obj_info_t *fota_obj_state)
{
  if (fota_obj_state == NULL)
  {
    LOG_ERROR("FOTA_LOG: fota_obj_state is NULL ");
    return;
  }

  LOG_INFO("FOTA_LOG: fota_obj_cmdq_init");

  q_init(&fota_obj_cmdq);

  fota_obj_param.cmd_q_ptr = &fota_obj_cmdq;
}


/*===========================================================================

FUNCTION FOTA_OBJ_GET_CMD_BUFFER

DESCRIPTION
  This function is used to allocate a command buffer that the client can
  then enqueue into the command queue.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the command

SIDE EFFECTS
  None

=========================================================================*/
fota_obj_cmd_t* fota_obj_get_cmd_buffer()
{
  fota_obj_cmd_t*                 cmd_ptr = NULL;

  LOG_INFO("FOTA_LOG:fota_obj_get_cmd_buffer ");

  cmd_ptr = (fota_obj_cmd_t*)malloc(sizeof(fota_obj_cmd_t));

  if (cmd_ptr == NULL) 
  {
    LOG_ERROR("FOTA_LOG: Command buffer allocation failed");
  }
  else
  {
    memset(cmd_ptr, 0x00, sizeof(fota_obj_cmd_t));
  }
  return cmd_ptr;
}


/*===========================================================================

FUNCTION  FOTA_OBJ_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the FOTA_OBJ queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void fota_obj_enqueue_cmd(fota_obj_cmd_t* cmd_ptr)
{
  q_put(&fota_obj_cmdq, &(cmd_ptr->cmd_hdr.link));
  return;
}

/*===========================================================================

FUNCTION FOTA_OBJ_DEQUEUE_CMD

DESCRIPTION
  This function de-queues and returns a command from the FOTA object task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static fota_obj_cmd_t* fota_obj_dequeue_cmd( void )
{
  fota_obj_cmd_t *cmd_ptr = NULL;

  LOG_INFO("FOTA_LOG: fota_obj_dequeue_cmd called");

  cmd_ptr = (fota_obj_cmd_t*)q_get(&fota_obj_cmdq);

  return cmd_ptr;
}

/*==============================================================================
FUNCTION FOTA_OBJ_PROCESS_COMMANDS()

DESCRIPTION
  This function dequeues all outstanding commands from and dispatches the
  processor functions.

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void fota_obj_process_commands(void)
{
  fota_obj_cmd_t                *fota_obj_cmd_ptr = NULL;
  qapi_Status_t                 result          = QAPI_ERROR;

  while ((fota_obj_cmd_ptr = fota_obj_dequeue_cmd()) != NULL ) 
  {
    LOG_INFO("FOTA_LOG: Command ptr: 0x%x, cmd_id: %d", 
                             fota_obj_cmd_ptr, 
                             fota_obj_cmd_ptr->cmd_hdr.cmd_id);
/*--------------------------------------------------------------------------
      Bound check to ensure command id is bounded
--------------------------------------------------------------------------*/
    if (!FOTA_OBJ_CMD_ID_RANGE_CHECK(fota_obj_cmd_ptr->cmd_hdr.cmd_id))
    {
      LOG_INFO("FOTA_LOG: Command ID Range check failed"); 
      free_memory((void **)&fota_obj_cmd_ptr);
      continue;
    }

    switch(fota_obj_cmd_ptr->cmd_hdr.cmd_id)
    {
      case FOTA_OBJ_CALL_BACK_EVT:
      {
        fota_app_data_t *data = (fota_app_data_t *)fota_obj_cmd_ptr->cmd_data.data;
        if(data != NULL && data->app_data.dl_op.lwm2m_data != NULL)
        {
          result = fota_handle_data(data->app_data.dl_op.handler, 
                                    *data->app_data.dl_op.lwm2m_data);
          free_lwm2m_copy_data_(&data->app_data.dl_op.lwm2m_data);
		  free_memory((void **)&data);
        }
      }
      break;

      case FOTA_OBJ_APP_ORIGINATED_EVT:
      {
        fota_app_data_t *data = (fota_app_data_t *)fota_obj_cmd_ptr->cmd_data.data;
        if(data != NULL && data->app_data.coap_op.transaction != NULL)
        {
          result = fota_handle_coap_pkt(data->app_data.coap_op.hdl, 
                                        data->app_data.coap_op.transaction,
                                        data->app_data.coap_op.message);
        
          if(data->app_data.coap_op.transaction)
          {
            free_coap_client_transaction(coap_handle, &data->app_data.coap_op.transaction);
          }
          if(data->app_data.coap_op.message)
          {
            qapi_Coap_Free_Message(coap_handle, data->app_data.coap_op.message);
          }
          free_memory((void **)&data);
		}
      }
      break;
    }
    LOG_INFO("FOTA_LOG: Result is %d",result);
    free_memory((void **)&fota_obj_cmd_ptr);
  } /* Loop to dequeue all outstanding commands*/
  return;
} /* fota_obj_process_commands */ 

void fota_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
)
{
  va_list ap;

  /*-----------------------------------------------------------------------*/

  /* validate input param */
  if( NULL == buf_ptr || buf_size <= 0)
  {
    LOG_ERROR("FOTA_LOG: Bad Param buf_ptr:[%p], buf_size:%d",buf_ptr, buf_size);
    return;
  }

  /*-----------------------------------------------------------------------*/

  va_start( ap, fmt );

  vsnprintf( buf_ptr, (size_t)buf_size, fmt, ap );

  va_end( ap );

}


