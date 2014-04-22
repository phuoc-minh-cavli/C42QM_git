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

static multi_app_info_t          multi_app_param_t1;
static multi_app_task_cmd_q      multi_app_cmdq_t1;

extern QCLI_Group_Handle_t qcli_net_handle;         /* Handle for Net Command Group. */

extern QCLI_Group_Handle_t qcli_dss_netapp_handle;  /* Handle for Dss Command Group. */

extern QCLI_Group_Handle_t qcli_atfwd_cli_app_handle;     /* Handle for ATFWD Command Group. */
extern QCLI_Group_Handle_t qcli_version_cli_app_handle;     /* Handle for Version Command Group. */


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
  multi_app_cmd_t                *multi_app_cmd_ptr = NULL;
  QCLI_Command_Status_t                result          = QCLI_STATUS_ERROR_E;
  QCLI_Group_Handle_t qcli_multi_app_handle;
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

      case DSS_LIB_INIT_EVT:
        result = proc_dss_lib_init(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                   multi_app_cmd_ptr->cmd_data.Parameter_List,
                                   multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_GET_CALL_HANDLE_EVT:
        result = proc_dss_get_call_handle(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                          multi_app_cmd_ptr->cmd_data.Parameter_List,
                                          multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_RELEASE_CALL_HANDLE_EVT:
        result = proc_dss_release_call_handle(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                          multi_app_cmd_ptr->cmd_data.Parameter_List,
                                          multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_SET_PARAM_EVT:
        result = proc_dss_test_set_param(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                          multi_app_cmd_ptr->cmd_data.Parameter_List,
                                          multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;
      
      case DSS_TEST_NET_UP_EVT:
        result = proc_dss_test_net_up(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                          multi_app_cmd_ptr->cmd_data.Parameter_List,
                                          multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_NET_DOWN_EVT:
        result =proc_dss_test_net_down(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                       multi_app_cmd_ptr->cmd_data.Parameter_List,
                                       multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_LIST_HANDLE_EVT:
        result = proc_dss_test_list_handle(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_RESET_PKT_STATS_EVT:
        result = proc_dss_test_reset_pkt_stats(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                               multi_app_cmd_ptr->cmd_data.Parameter_List,
                                               multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_PKT_STATS_EVT:
        result = proc_dss_test_get_pkt_stats(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                             multi_app_cmd_ptr->cmd_data.Parameter_List,
                                             multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_CALL_END_REASON_EVT:
        result = proc_dss_test_get_call_end_reason(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                   multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                   multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_CALL_TECH_EVT:
        result = proc_dss_test_get_call_tech(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                             multi_app_cmd_ptr->cmd_data.Parameter_List,
                                             multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_CURRENT_DATA_BEARER_TECH_EVT:
        result = proc_dss_test_get_current_data_bearer_tech(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_DEVICE_NAME_EVT:
        result = proc_dss_test_get_device_name(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                               multi_app_cmd_ptr->cmd_data.Parameter_List,
                                               multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_QMI_PORT_NAME_EVT:
        result = proc_dss_test_get_qmi_port_name(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_IP_ADDR_COUNT_EVT:
        result = proc_dss_test_get_ip_addr_count(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_IP_ADDR_EVT:
        result = proc_dss_test_get_ip_addr(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_GET_LINK_MTU_EVT:
        result = proc_dss_test_get_link_mtu(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_SEND_NIPD_DATA_EVT:
        result = proc_dss_test_send_nipd_data(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                              multi_app_cmd_ptr->cmd_data.Parameter_List,
                                              multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_ADD_FILTERS_EVT:
        result = proc_dss_test_add_filters(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;
    
      case DSS_TEST_REMOVE_FILTERS_EVT:
        result = proc_dss_test_remove_filters(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                              multi_app_cmd_ptr->cmd_data.Parameter_List,
                                              multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_SET_TRANSFER_STAT_EVT:
        result = proc_dss_set_data_transfer_status(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                              multi_app_cmd_ptr->cmd_data.Parameter_List,
                                              multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case DSS_TEST_SET_TRANSFER_STAT_EVT_v2:
        result = proc_dss_set_data_transfer_status_v2(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                              multi_app_cmd_ptr->cmd_data.Parameter_List,
                                              multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_dss_netapp_handle;
        break;

      case ROUTE6_EVT:
        result = proc_route6(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                             multi_app_cmd_ptr->cmd_data.Parameter_List,
                             multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case ROUTE_EVT:
        result = proc_route(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;


      case AUTOIP_EVT:
        result = proc_autoip(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                             multi_app_cmd_ptr->cmd_data.Parameter_List,
                             multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case IFCONFIG_EVT:
        result = proc_ifconfig(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                               multi_app_cmd_ptr->cmd_data.Parameter_List,
                               multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case DNSC_EVT:
        result =proc_dnsc(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                          multi_app_cmd_ptr->cmd_data.Parameter_List,
                          multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case SET_IPV6_ROUTER_PREFIX_EVT:
        result = proc_set_ipv6_router_prefix(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                             multi_app_cmd_ptr->cmd_data.Parameter_List,
                                             multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;
        
      case CFG_NETBUF_EVT:
        result = proc_cfg_netbuf(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                 multi_app_cmd_ptr->cmd_data.Parameter_List,
                                 multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
      break;

      case BENCHTX_EVT:
        result = proc_benchtx(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                multi_app_cmd_ptr->cmd_data.Parameter_List,
                                multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;
      
      case BENCHRX_EVT:
        result = proc_benchrx(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                multi_app_cmd_ptr->cmd_data.Parameter_List,
                                multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case HTTPC_COMMAND_HANDLER_EVT:
        result = proc_httpc_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;
 #ifdef INCLUDE_SSLDEMO
      case CERT_COMMAND_HANDLER_EVT:
        result = proc_cert_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case SSL_COMMAND_HANDLER_EVT:
        result = proc_ssl_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;
#endif
      case MQTT_COMMAND_HANDLER_EVT:
        result = proc_mqtt_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case BENCHTOOL_CONFIG_EVT:
      
      result = proc_benchtoolconfig(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                         multi_app_cmd_ptr->cmd_data.Parameter_List,
                                         multi_app_cmd_ptr->cmd_data.app_id);
      qcli_multi_app_handle = qcli_net_handle;
      break;

      case PING3_EVT:
        result = proc_ping3(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                         multi_app_cmd_ptr->cmd_data.Parameter_List,
                                         multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case PING6_3_EVT:
        result = proc_ping6_3(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                         multi_app_cmd_ptr->cmd_data.Parameter_List,
                                         multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case SOCK_OPT_SET_EVT:
        result = proc_set_sock_opt(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                          multi_app_cmd_ptr->cmd_data.Parameter_List,
                                          multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;
	  	 
      case COAP_COMMAND_HANDLER_EVT:
			  result = proc_coap_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
												 multi_app_cmd_ptr->cmd_data.Parameter_List,
												 multi_app_cmd_ptr->cmd_data.app_id);
			  qcli_multi_app_handle = qcli_net_handle;
	  break;
			  
  #ifdef QTI_BRIDGE_ENABLE
      case ATFWD_EVT:
        result = proc_atfwd(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                            multi_app_cmd_ptr->cmd_data.Parameter_List,
                            multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_atfwd_cli_app_handle;
        break;
  #endif

      case DEVICE_INFO_COMMAND_HANDLER_EVT:
        result = proc_device_info_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;

      case VERSION_COMMAND_HANDLER_EVT:
        result = proc_version_command_handler(multi_app_cmd_ptr->cmd_data.Parameter_Count,
                                           multi_app_cmd_ptr->cmd_data.Parameter_List,
                                           multi_app_cmd_ptr->cmd_data.app_id);
        qcli_multi_app_handle = qcli_version_cli_app_handle;
        break;

      default:
        LOG_ERROR("Command ID %d not handled", multi_app_cmd_ptr->cmd_data.cmd_id);
        qcli_multi_app_handle = qcli_net_handle;
        break;
    } 

    if (flag == false)
    {
      if(result == QCLI_STATUS_SUCCESS_E)
      {
        QCLI_Printf(qcli_multi_app_handle, "Successfully processed the command on App 0\n");
      }
      else if (result == QCLI_STATUS_USAGE_E)
      {
        Display_Usage(multi_app_cmd_ptr->cmd_data.Command_Index, &multi_app_cmd_ptr->cmd_data.Command[multi_app_cmd_ptr->cmd_data.Command_Index - 4]);
      }
      else if (result == QCLI_STATUS_ERROR_E)
      {
        QCLI_Printf(qcli_multi_app_handle, "Failed to process the command on App 0\n");
      }
    }

    dam_is_cmd_running[0] = false;

    tx_byte_release(multi_app_cmd_ptr);

  } /* Loop to dequeue all outstanding commands*/
  return;
} /* multi_app_process_commands */ 

