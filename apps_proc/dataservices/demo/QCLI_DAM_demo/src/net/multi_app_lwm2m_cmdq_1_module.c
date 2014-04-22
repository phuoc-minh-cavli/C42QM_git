/*==============================================================================

                             MULTI_APP_CMDQ_1.C

GENERAL DESCRIPTION
  This file deals with the task Initialization to support Multi-Thread Functionality 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The task init and task start routines do the work necessary to ensure that
  all data structures are in the desired state to continue with normal
  operation.

Copyright (c) 2017-2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------

==============================================================================*/
#include "multi_app_test_module.h"
#include "ext_object_test_app_module.h"
#include "ext_object_test_app_module_v2.h"

static multi_app_info_t          multi_app_param_t1;
static multi_app_task_cmd_q      multi_app_cmdq_t1;


#ifdef LWM2M_ENABLE
extern QCLI_Group_Handle_t lwm2m_ext_handle;     /* Handle for Lwm2m extensible Command Group. */

extern QCLI_Group_Handle_t lwm2m_ext_handle_v2;     /* Handle for Lwm2m extensible Command Group. */

extern QCLI_Group_Handle_t qcli_m2m_stub_handle;     /* Handle for Lwm2m Stub Command Group. */

extern QCLI_Group_Handle_t qcli_m2m_stub_handle_v2;     /* Handle for Lwm2m Stub Command Group. */
#endif

extern void Display_Usage(uint32_t Command_Index, const QCLI_Command_t *Command);

int dam_thread_id = 0;

extern bool dam_is_cmd_running[NO_OF_APPS];

#define     MULTI_APP_Q_NO_OF_MSGS         100
#define     MULTI_APP_Q_MESSAGE_SIZE       TX_1_ULONG
#define     MULTI_APP_QUEUE_SIZE           MULTI_APP_Q_NO_OF_MSGS*MULTI_APP_Q_MESSAGE_SIZE*sizeof(multi_app_cmd_t)

extern TX_BYTE_POOL *byte_pool_qcli;

/*=============================================================================

FUNCTION MULTI_APP_SET_SIGNAL

DESCRIPTION
  Set the Multi App signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void multi_app_set_signal_dam_t1(void)
{

  LOG_INFO("multi_app_set_signal_dam_t1 \n");

  tx_mutex_get(multi_app_param_t1.multi_app_mutex, TX_WAIT_FOREVER);

  MULTI_APP_TASK_SET_SIG(multi_app_param_t1.multi_app_signal,MULTI_APP_SIGNAL_MASK);

  tx_mutex_put(multi_app_param_t1.multi_app_mutex);

  return;
}

/*=============================================================================

FUNCTION MULTI_APP_MUTEX_INIT

DESCRIPTION
  Multi App mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void multi_app_mutex_init_dam_t1(void)
{

  LOG_INFO("multi_app_mutex_init_dam_t1 \n");

/* allocate mutex required for thread*/ 
  txm_module_object_allocate((void **)(&multi_app_param_t1.multi_app_mutex), sizeof(TX_MUTEX));
  tx_mutex_create(multi_app_param_t1.multi_app_mutex, "multi_app_mutex_t1", TX_NO_INHERIT);
  return;
}


/*=============================================================================

FUNCTION MULTI_APP_SIGNAL_INIT

DESCRIPTION
  Multi App Signal Initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void multi_app_signal_init_dam_t1(void)
{
  LOG_INFO("multi_app_signal_init_dam_t1 \n");

/* allocate signal required for thread*/ 
  txm_module_object_allocate((void **)(&multi_app_param_t1.multi_app_signal), sizeof(TX_EVENT_FLAGS_GROUP));
  tx_event_flags_create(multi_app_param_t1.multi_app_signal, "multi_app_sig_t1");
  return;
}


/*=============================================================================

FUNCTION MULTI_APP_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the signal.
  The Multi App signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void multi_app_signal_wait_dam_t1(void)
{
  uint32_t received_sigs = 0;

  LOG_INFO("multi_app_signal_wait_dam_t1 \n");

  while (1)
  {
/*============================================================================
  Wait for the Signal to be set
=============================================================================*/
    tx_event_flags_get(multi_app_param_t1.multi_app_signal, MULTI_APP_SIGNAL_MASK,
                       TX_OR_CLEAR, (ULONG *)&received_sigs, TX_WAIT_FOREVER);

    if (MULTI_APP_TASK_IS_SIG_SET(received_sigs, MULTI_APP_SIGNAL_MASK))
    {
      // Signal is set Dequeue the Command queue
      multi_app_process_commands_t1();
    }

/*============================================================================
  Clear the Multi App signal **Need To add mutex in the correct place**
=============================================================================*/

  /*  tx_mutex_get(multi_app_param_t1.multi_app_mutex, TX_WAIT_FOREVER);

    MULTI_APP_TASK_CLEAR_SIG(multi_app_param_t1.multi_app_signal, MULTI_APP_SIGNAL_MASK);

    tx_mutex_put(multi_app_param_t1.multi_app_mutex);
*/
  }
}


/*================================================================

FUNCTION MULTI_APP_TASK_INIT

DESCRIPTION
  Multi App Task initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void multi_app_task_init_dam_t1(void)
{

  LOG_INFO("multi_app_task_init_dam_t1 \n");

/*============================================================================
  Reseting the Global Multi App Structure
=============================================================================*/

  memset(&multi_app_param_t1, 0, sizeof (multi_app_info_t));

/*----------------------------------------------------------------------------
  Initialize the command queues and set the command queue pointers in the
   multi_app_info struct to the appropriate values.
----------------------------------------------------------------------------*/
  multi_app_cmdq_init_t1(&multi_app_param_t1);

}

/*=============================================================================

FUNCTION MULTI_APP_TASK_ENTRY

DESCRIPTION
  This is the entry point for Multi App task
  Reset the Global Multi App structure
  Initalize the Multi App Qurt signal

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void multi_app_task_entry_dam_t1(ULONG thread_ctxt)
{

  LOG_INFO("multi_app_task_entry_dam_t1 \n");

  multi_app_task_init_dam_t1();

/*=============================================================================
Initialize Mutex Object
==============================================================================*/

  multi_app_mutex_init_dam_t1();

/*=============================================================================
Initialize Signal 
==============================================================================*/

  multi_app_signal_init_dam_t1 ();

/*=============================================================================
Wait on the Signal
==============================================================================*/
  multi_app_signal_wait_dam_t1();

}

/*===========================================================================

FUNCTION MULTI_APP_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  multi_app_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void multi_app_cmdq_init_t1 (multi_app_info_t * multi_app_state)
{
  void *cmdq_ptr = NULL;

  if(multi_app_state == NULL)
  {
    LOG_INFO("Multi_app State is NULL ");
    return;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&cmdq_ptr, MULTI_APP_QUEUE_SIZE, TX_NO_WAIT);

  LOG_INFO("multi_app_cmdq_init_t1");

  multi_app_param_t1 = *multi_app_state;

/* allocate queue required for thread*/ 
  txm_module_object_allocate((void **)&multi_app_cmdq_t1, sizeof(TX_QUEUE));
  tx_queue_create(multi_app_cmdq_t1, "multi_app_cmdq_t1", MULTI_APP_Q_MESSAGE_SIZE,cmdq_ptr, MULTI_APP_QUEUE_SIZE);

  multi_app_param_t1.cmd_q_ptr = multi_app_cmdq_t1;
}


/*===========================================================================

FUNCTION  MULTI_APP_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void multi_app_enqueue_cmd_t1(multi_app_cmd_t* cmd_ptr)
{

  LOG_INFO("Cmd_ptr: 0x%x with cmd_id: %d, posted to int Q", cmd_ptr, cmd_ptr->cmd_data.cmd_id);

  tx_queue_send(multi_app_cmdq_t1, &cmd_ptr, TX_WAIT_FOREVER);

  return;
}

/*===========================================================================

FUNCTION MULTI_APP_DEQUEUE_CMD

DESCRIPTION
  This function de-queues and returns a command from the Multi app task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static multi_app_cmd_t* multi_app_dequeue_cmd_t1( void )
{
  multi_app_cmd_t *cmd_ptr = NULL;

  LOG_INFO("multi_app_dequeue_cmd_t1 called");

  tx_queue_receive(multi_app_cmdq_t1, &cmd_ptr, TX_WAIT_FOREVER);

  return cmd_ptr;
}

/*==============================================================================
FUNCTION MULTI_APP_PROCESS_COMMANDS()

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
void multi_app_process_commands_t1(void)
{
  

  multi_app_cmd_t				 *multi_app_cmd_ptr = NULL;
  QCLI_Command_Status_t				 result 		 = QCLI_STATUS_ERROR_E;
  QCLI_Group_Handle_t qcli_multi_app_handle;

  #ifdef LWM2M_ENABLE
  qapi_Net_LWM2M_Ext_v2_t      *obj_buffer_v2   = NULL;
  #endif
  qapi_Status_t                lwm2m_result          = QAPI_ERROR;
  bool flag = false;

  while ((multi_app_cmd_ptr = multi_app_dequeue_cmd_t1()) != NULL ) 
  {
    LOG_INFO("Command ptr: 0x%x, cmd_id: %d", multi_app_cmd_ptr, multi_app_cmd_ptr->cmd_data.cmd_id);

/*--------------------------------------------------------------------------
      Bound check to ensure command id is bounded
--------------------------------------------------------------------------*/
    if (!MULTI_APP_CMD_ID_RANGE_CHECK(multi_app_cmd_ptr->cmd_data.cmd_id))
    {
      LOG_INFO("Command ID Range check failed"); 
      continue;
    }

    dam_thread_id = multi_app_cmd_ptr->cmd_data.app_id;

    dam_is_cmd_running[0] = true;

    flag = false;

    switch(multi_app_cmd_ptr->cmd_data.cmd_id)
    {
#ifdef LWM2M_ENABLE
#if 0
      case EXT_OBJ_CALL_BACK_EVT:
      {
        flag= true;
        obj_buffer = (qapi_Net_LWM2M_Ext_t *)multi_app_cmd_ptr->cmd_data.data;
        qcli_multi_app_handle = lwm2m_ext_handle;
        if(obj_buffer != NULL && obj_buffer->app_data.dl_op.lwm2m_srv_data != NULL)
        {
          switch(obj_buffer->app_data.dl_op.lwm2m_srv_data->msg_type)
          {
            case QAPI_NET_LWM2M_READ_REQ_E:
            {
              lwm2m_result = read_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E:
            {
              lwm2m_result = write_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data,
                                           QAPI_COAP_PUT);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E:
            {
              lwm2m_result = write_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data,
                                           QAPI_COAP_POST);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_ATTR_REQ_E:
            {
              lwm2m_result = ext_object_write_attr(obj_buffer->app_data.dl_op.handler, 
                                            *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DISCOVER_REQ_E:
            {
              lwm2m_result = disc_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_EXECUTE_REQ_E:
            {
              lwm2m_result = exec_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_CREATE_REQ_E:
            {
              lwm2m_result = create_object_instance(obj_buffer->app_data.dl_op.handler, 
                                              *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DELETE_REQ_E:
            {
              lwm2m_result = delete_object_instance(obj_buffer->app_data.dl_op.handler, 
                                              *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_OBSERVE_REQ_E:
            {
              lwm2m_result = observe_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                      *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E:
            {
              lwm2m_result = observe_cancel_ext_obj(obj_buffer->app_data.dl_op.handler, 
                                             *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_ACK_MSG_E:
            break;
            case QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E:
            {
              if(obj_buffer->app_data.dl_op.lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_REBOOT_E ||
                 obj_buffer->app_data.dl_op.lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
              {
                // flag = true;
              }
              lwm2m_result = ext_obj_handle_event(obj_buffer->app_data.dl_op.handler, 
                                            *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DELETE_ALL_REQ_E:
            {
              lwm2m_result = ext_obj_delete_all(obj_buffer->app_data.dl_op.handler, 
                                            *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
          }

          if (obj_buffer)
          {
            if (obj_buffer->app_data.dl_op.lwm2m_srv_data)
            {
              if (obj_buffer->app_data.dl_op.lwm2m_srv_data->lwm2m_attr)
              {
                tx_byte_release(obj_buffer->app_data.dl_op.lwm2m_srv_data->lwm2m_attr);
                obj_buffer->app_data.dl_op.lwm2m_srv_data->lwm2m_attr = NULL;
              }
              if(obj_buffer->app_data.dl_op.lwm2m_srv_data->payload)
              {
                tx_byte_release(obj_buffer->app_data.dl_op.lwm2m_srv_data->payload);
                obj_buffer->app_data.dl_op.lwm2m_srv_data->payload = NULL;
              }
              tx_byte_release(obj_buffer->app_data.dl_op.lwm2m_srv_data);
              obj_buffer->app_data.dl_op.lwm2m_srv_data = NULL;
            }

            tx_byte_release(obj_buffer);
            obj_buffer = NULL;
          }

          LOG_INFO("Operation result is %d ", lwm2m_result);
        }
      }
      break;

      case EXT_OBJ_APP_ORIGINATED_EVT:
      {
        obj_buffer = (qapi_Net_LWM2M_Ext_t *)multi_app_cmd_ptr->cmd_data.data;
        qcli_multi_app_handle = lwm2m_ext_handle;
        if(obj_buffer != NULL)
        {
          switch(obj_buffer->app_data.ul_op.msg_type)
          {
            case QAPI_NET_LWM2M_EXT_REGISTER_APP_E:
            {
              lwm2m_result = ext_object_register(obj_buffer->app_data.ul_op.id);
            }
            break;
 
            case QAPI_NET_LWM2M_EXT_CREATE_OBJ_E:
            {
              lwm2m_result = create_object(obj_buffer->app_data.ul_op.id);
            }
            break;

            case QAPI_NET_LWM2M_EXT_CREATE_OBJ_INSTANCE_E:
            {
              lwm2m_result = create_object_instance_app(obj_buffer);
            }
            break;

            case QAPI_NET_LWM2M_EXT_DELETE_OBJ_INSTANCE_E:
            {
              lwm2m_result = delete_object_instance_app(obj_buffer);
            }
            break;

            case QAPI_NET_LWM2M_EXT_DEREGISTER_APP_E:
            {
              lwm2m_result = ext_object_deregister(obj_buffer->app_data.ul_op.id);
            }
            break;

            case QAPI_NET_LWM2M_EXT_NOTIFY_E:
            {
              time_t timeout = 5;
              qapi_time_get_t current_time;
              flag = true;
              qapi_time_get(QAPI_TIME_SECS, &current_time);

              ext_object_notify(0,current_time.time_secs, &timeout);
            }
            break;

            case QAPI_NET_LWM2M_EXT_CONFIG_CLIENT_E:
            {
              lwm2m_result = ext_object_configclient(obj_buffer->app_data.ul_op.id);
              break;
            }

            case QAPI_NET_LWM2M_EXT_SET_OBJ_LINK_E:
            {
              lwm2m_result = set_obj_link_app(obj_buffer);
              break;
            }

            case QAPI_NET_LWM2M_EXT_GET_OBJ_LINK_E:
            {
              lwm2m_result = get_obj_link_app(obj_buffer);
              break;
            }

            case QAPI_NET_LWM2M_EXT_SET_SRV_LIFETIME_E:
            {
              lwm2m_result = set_server_lifetime_app(obj_buffer);
              break;
            }

            case QAPI_NET_LWM2M_EXT_GET_SRV_LIFETIME_E:
            {
              lwm2m_result = get_server_lifetime_app(obj_buffer);
              break;
            }

            default:
            {
              LOG_INFO("Invalid request from application.");
              break;
            }
          }

          if(obj_buffer)
          {
            tx_byte_release(obj_buffer);
            obj_buffer = NULL;
         }
      }
    }
    break;
#endif
      case EXT_OBJ_CALL_BACK_EVT_v2:
      {
        flag= true;
        obj_buffer_v2 = (qapi_Net_LWM2M_Ext_v2_t *)multi_app_cmd_ptr->cmd_data.data;
        qcli_multi_app_handle = lwm2m_ext_handle_v2;
        if(obj_buffer_v2 != NULL && obj_buffer_v2->app_data.dl_op.lwm2m_srv_data != NULL)
        {
          switch(obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->msg_type)
          {
            case QAPI_NET_LWM2M_READ_REQ_E:
            {
              lwm2m_result = read_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                    *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E:
            {
              lwm2m_result = write_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                    *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data,
                                           QAPI_COAP_PUT);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E:
            {
              lwm2m_result = write_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                    *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data,
                                           QAPI_COAP_POST);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_ATTR_REQ_E:
            {
              lwm2m_result = ext_object_write_attr_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                            *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DISCOVER_REQ_E:
            {
              lwm2m_result = disc_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                    *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_EXECUTE_REQ_E:
            {
              lwm2m_result = exec_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                    *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_CREATE_REQ_E:
            {
              lwm2m_result = create_object_instance_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                              *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DELETE_REQ_E:
            {
              lwm2m_result = delete_object_instance_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                              *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_OBSERVE_REQ_E:
            {
              lwm2m_result = observe_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                      *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E:
            {
              lwm2m_result = observe_cancel_ext_obj_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                             *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_ACK_MSG_E:
            break;
            case QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E:
            {
              if(obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_REBOOT_E ||
                 obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
              {
                // flag = true;
              }
              lwm2m_result = ext_obj_handle_event_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                            *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DELETE_ALL_REQ_E:
            {
              lwm2m_result = ext_obj_delete_all_v2(obj_buffer_v2->app_data.dl_op.handler, 
                                            *obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
            }
            break;
          }

          if (obj_buffer_v2)
          {
            if (obj_buffer_v2->app_data.dl_op.lwm2m_srv_data)
            {
              if (obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->lwm2m_attr)
              {
                tx_byte_release(obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->lwm2m_attr);
                obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->lwm2m_attr = NULL;
              }
              if(obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->payload)
              {
                tx_byte_release(obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->payload);
                obj_buffer_v2->app_data.dl_op.lwm2m_srv_data->payload = NULL;
              }
              tx_byte_release(obj_buffer_v2->app_data.dl_op.lwm2m_srv_data);
              obj_buffer_v2->app_data.dl_op.lwm2m_srv_data = NULL;
            }

            tx_byte_release(obj_buffer_v2);
            obj_buffer_v2 = NULL;
          }

          LOG_INFO("Operation result is %d ", lwm2m_result);
        }
      }
      break;

      case EXT_OBJ_APP_ORIGINATED_EVT_v2:
      {
        obj_buffer_v2 = (qapi_Net_LWM2M_Ext_v2_t *)multi_app_cmd_ptr->cmd_data.data;
        qcli_multi_app_handle = lwm2m_ext_handle_v2;
        if(obj_buffer_v2 != NULL)
        {
          switch(obj_buffer_v2->app_data.ul_op.msg_type)
          {
            case QAPI_NET_LWM2M_EXT_REGISTER_APP_v2_E:
            {
              lwm2m_result = ext_object_register_v2(obj_buffer_v2->app_data.ul_op.id);
            }
            break;
 
            case QAPI_NET_LWM2M_EXT_CREATE_OBJ_v2_E:
            {
              lwm2m_result = create_object_v2(obj_buffer_v2->app_data.ul_op.id);
            }
            break;

            case QAPI_NET_LWM2M_EXT_CREATE_OBJ_INSTANCE_v2_E:
            {
              lwm2m_result = create_object_instance_app_v2(obj_buffer_v2);
            }
            break;

            case QAPI_NET_LWM2M_EXT_DELETE_OBJ_INSTANCE_v2_E:
            {
              lwm2m_result = delete_object_instance_app_v2(obj_buffer_v2);
            }
            break;

            case QAPI_NET_LWM2M_EXT_DEREGISTER_APP_v2_E:
            {
              lwm2m_result = ext_object_deregister_v2(obj_buffer_v2->app_data.ul_op.id);
            }
            break;

            case QAPI_NET_LWM2M_EXT_NOTIFY_v2_E:
            {
              time_t timeout = 5;
              qapi_time_get_t current_time;
              flag = true;
              qapi_time_get(QAPI_TIME_SECS, &current_time);

              ext_object_notify_v2(0,current_time.time_secs, &timeout);
            }
            break;

            case QAPI_NET_LWM2M_EXT_SET_OBJ_LINK_v2_E:
            {
              lwm2m_result = set_obj_link_app_v2(obj_buffer_v2);
              break;
            }

            case QAPI_NET_LWM2M_EXT_GET_OBJ_LINK_v2_E:
            {
              lwm2m_result = get_obj_link_app_v2(obj_buffer_v2);
              break;
            }

            case QAPI_NET_LWM2M_EXT_SET_SRV_LIFETIME_v2_E:
            {
              lwm2m_result = set_server_lifetime_app_v2(obj_buffer_v2);
              break;
            }

            case QAPI_NET_LWM2M_EXT_GET_SRV_LIFETIME_v2_E:
            {
              lwm2m_result = get_server_lifetime_app_v2(obj_buffer_v2);
              break;
            }
			
            case QAPI_NET_LWM2M_EXT_CONFIG_BOOTSTRAP_INFO_E:
            {
              lwm2m_result = configclient_bootstrap_info(obj_buffer_v2->app_data.ul_op.id);
              break;
            }
            case QAPI_NET_LWM2M_EXT_START_LWM2M:
            {
              lwm2m_result = configclient_start(obj_buffer_v2->app_data.ul_op.id);
              break;
            }
            case QAPI_NET_LWM2M_EXT_STOP_LWM2M:
            {
              lwm2m_result = configclient_stop(obj_buffer_v2->app_data.ul_op.id, QAPI_NET_LWM2M_STOP);
              break;
            }
            case QAPI_NET_LWM2M_EXT_DEREGISTER_AND_STOP_LWM2M:
            {
              lwm2m_result = configclient_stop(obj_buffer_v2->app_data.ul_op.id, QAPI_NET_LWM2M_DEREGISTER_AND_STOP);
              break;
            }			
		    case QAPI_NET_LWM2M_EXT_FACTORY_RESET_LWM2M:
            {
              lwm2m_result = configclient_stop(obj_buffer_v2->app_data.ul_op.id, QAPI_NET_LWM2M_FACTORY_RESET);
              break;
            }
	
			
            default:
            {
              LOG_INFO("Invalid request from application.");
              break;
            }
          }

          if(obj_buffer_v2)
          {
            tx_byte_release(obj_buffer_v2);
            obj_buffer_v2 = NULL;
         }
      }
    }
    break;

#if 0
      case LWM2M_DEVICECAP_APP_START_EVT:
        result = proc_devicecap_app_start(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_CREATE_INSTANCE_EVT:
        result = proc_devicecap_create_instance(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_DELETE_INSTANCE_EVT:
        result = proc_devicecap_delete_instance(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_OBSERVE_EVT:
        result = proc_devicecap_observe(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_CANCEL_OBSERVE_EVT:
        result = proc_devicecap_cancel_observe(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_SET_NOTIFY_EVT:
        result = proc_devicecap_set_notify(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_APP_STOP_EVT:
        result = proc_devicecap_app_stop(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_SW_MGNT_APP_START_EVT:
        result = proc_sw_mgnt_app_start(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_SW_MGNT_UPDATE_RESULT_EVT:
        result = proc_sw_mgnt_update_result(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_SW_MGNT_DELETE_INSTANCE_EVT:
        result = proc_sw_mgnt_delete_instance(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICE_APP_START_EVT:
        result = proc_device_app_start(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICE_APP_SET_RESOURCE_EVT:
        result = proc_device_app_set_resource(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_SW_MGNT_GET_EVT:
        result = proc_sw_mgnt_get(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;

      case LWM2M_DEVICECAP_GET_EVT:
        result = proc_devicecap_get(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle;
        break;
#endif
      case LWM2M_DEVICECAP_APP_START_EVT_v2:
        result = proc_devicecap_app_start_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_CREATE_INSTANCE_EVT_v2:
        result = proc_devicecap_create_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_DELETE_INSTANCE_EVT_v2:
        result = proc_devicecap_delete_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_OBSERVE_EVT_v2:
        result = proc_devicecap_observe_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_CANCEL_OBSERVE_EVT_v2:
        result = proc_devicecap_cancel_observe_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_SET_NOTIFY_EVT_v2:
        result = proc_devicecap_set_notify_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_APP_STOP_EVT_v2:
        result = proc_devicecap_app_stop_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_SW_MGNT_APP_START_EVT_v2:
        result = proc_sw_mgnt_app_start_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_SW_MGNT_UPDATE_RESULT_EVT_v2:
        result = proc_sw_mgnt_update_result_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_SW_MGNT_DELETE_INSTANCE_EVT_v2:
        result = proc_sw_mgnt_delete_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICE_APP_START_EVT_v2:
        result = proc_device_app_start_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICE_APP_SET_RESOURCE_EVT_v2:
        result = proc_device_app_set_resource_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_SW_MGNT_GET_EVT_v2:
        result = proc_sw_mgnt_get_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_DEVICECAP_GET_EVT_v2:
        result = proc_devicecap_get_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

	 case LWM2M_APN_INFO_APP_START_EVT_v2:
        result = proc_apninfo_app_start_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_APN_INFO_CREATE_INSTANCE_EVT_v2:
        result = proc_apninfo_create_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_APN_INFO_DELETE_INSTANCE_EVT_v2:
        result = proc_apninfo_delete_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_APN_INFO_GET_EVT_v2:
        result = proc_apninfo_get_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                     multi_app_cmd_ptr->cmd_data.Parameter_List,
                                     multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_APN_INFO_APP_STOP_EVT_v2:
        result = proc_apninfo_app_stop_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_APN_INFO_CALL_BACK_EVT:
      {
        std_obj_cb_data_t *cb_data = multi_app_cmd_ptr->cmd_data.data;
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        flag = true;
        lwm2m_result = apn_info_handle_event(cb_data);
        if(cb_data && cb_data->srv_data)
        {
          tx_byte_release(cb_data->srv_data);
          cb_data->srv_data = NULL;
        }
        if(cb_data)
        {
          tx_byte_release(cb_data);
          cb_data = NULL;
        }
      }
      break;

    case LWM2M_CELL_CON_APP_START_EVT_v2:
        result = proc_cell_nw_app_start_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_CELL_CON_CREATE_INSTANCE_EVT_v2:
        result = proc_cell_nw_create_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_CELL_CON_DELETE_INSTANCE_EVT_v2:
        result = proc_cell_nw_delete_instance_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_CELL_CON_GET_EVT_v2:
        result = proc_cell_nw_get_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                     multi_app_cmd_ptr->cmd_data.Parameter_List,
                                     multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_CELL_CON_APP_STOP_EVT_v2:
        result = proc_cell_nw_app_stop_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        break;

      case LWM2M_CELL_CON_CALL_BACK_EVT:
      {
        std_obj_cb_data_t *cb_data = multi_app_cmd_ptr->cmd_data.data;
        qcli_multi_app_handle = qcli_m2m_stub_handle_v2;
        flag = true;
        lwm2m_result = cell_con_handle_event(cb_data);
        if(cb_data && cb_data->srv_data)
        {
          tx_byte_release(cb_data->srv_data);
          cb_data->srv_data = NULL;
        }
        if(cb_data)
        {
          tx_byte_release(cb_data);
          cb_data = NULL;
        }
      }
      break;
   #endif
      default:
        LOG_ERROR("Command ID %d not handled", multi_app_cmd_ptr->cmd_data.cmd_id);
		qcli_multi_app_handle = NULL;
        break;
    } 

    if (flag == false)
    {
      if((result == QCLI_STATUS_SUCCESS_E || lwm2m_result == QAPI_OK) && (NULL != qcli_multi_app_handle ))
      {
        QCLI_Printf(qcli_multi_app_handle, "Successfully processed the command on App 0\n");
      }
      else if (result == QCLI_STATUS_USAGE_E)
      {
        Display_Usage(multi_app_cmd_ptr->cmd_data.Command_Index, &multi_app_cmd_ptr->cmd_data.Command[multi_app_cmd_ptr->cmd_data.Command_Index - 4]);
      }
      else if (result == QCLI_STATUS_ERROR_E && (NULL != qcli_multi_app_handle ))
      {
        QCLI_Printf(qcli_multi_app_handle, "Failed to process the command on App 0\n");
      }
    }

    dam_is_cmd_running[0] = false;

    tx_byte_release(multi_app_cmd_ptr);

  } /* Loop to dequeue all outstanding commands*/
  return;
} /* multi_app_process_commands */ 

