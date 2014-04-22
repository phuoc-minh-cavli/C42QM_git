/*==============================================================================

                             QCMAP_CMDQ.C

GENERAL DESCRIPTION
  This file deals with the Command Queue related operations

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Command Queue must me initialized

Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

#include "qcmap.h"
#include "qcmap_api.h"

static qcmap_task_cmd_q                   qcmap_cmdq;
static qcmap_config_info_t                *qcmap_info = NULL;
boolean qti_client_enabled = FALSE;
extern qcmap_connection_manager *qc_mgr;
extern client_data_s *client_data_t;

/*===========================================================================

FUNCTION QCMAP_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  qcmap_config_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void qcmap_cmdq_init (qcmap_config_info_t * qcmap_state)
{

  if (qcmap_state == NULL)
  {
    QCMAP_LOG_MSG_ERROR ("QCMAP State is NULL ",0, 0, 0 );
    return;
  }

  QCMAP_LOG_MSG_HIGH ("qcmap_cmdq_init ",0, 0, 0 );

  qcmap_info = qcmap_state;

  q_init(&qcmap_cmdq);

  qcmap_info->cmd_q_ptr = &qcmap_cmdq;
}


/*===========================================================================

FUNCTION QCMAP_GET_CMD_BUFFER

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
qcmap_cmd_t* qcmap_get_cmd_buffer()
{
  qcmap_cmd_t*                 cmd_ptr = NULL;


  cmd_ptr = (qcmap_cmd_t*)malloc(sizeof(qcmap_cmd_t));

  if (cmd_ptr == NULL) 
  {
    QCMAP_LOG_MSG_ERROR("Command buffer allocation faled", 0, 0, 0);
  }

  return cmd_ptr;
}


/*===========================================================================

FUNCTION  QCMAP_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the QCMAP queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void qcmap_enqueue_cmd(qcmap_cmd_t* cmd_ptr)
{
  QCMAP_LOG_MSG_HIGH("Cmd_ptr: 0x%x with cmd_id: %d, posted to int Q," 
                             "num cmds already in Q: %d",
                             cmd_ptr, cmd_ptr->cmd_hdr.cmd_id, 
                             q_cnt(&qcmap_cmdq));

  q_put(&qcmap_cmdq, &(cmd_ptr->cmd_hdr.link));

  return;
}

/*===========================================================================

FUNCTION QCMAP_GET_NEXT_CMD

DESCRIPTION
  This function de-queues and returns a command from the QCMAP task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static qcmap_cmd_t* qcmap_get_next_cmd( void )
{
  qcmap_cmd_t *cmd_ptr = NULL;


  cmd_ptr = (qcmap_cmd_t*)q_get( &qcmap_cmdq );

  QCMAP_LOG_MSG_HIGH("Cmd ptr: 0x%x, num rem cmds: QCMAP Q count: %d",
                           cmd_ptr, q_cnt(&qcmap_cmdq), 0);

  return cmd_ptr;
}

/*==============================================================================
FUNCTION QCMAP_PROCESS_COMMANDS()

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
void qcmap_process_commands(void)
{
  qcmap_cmd_t                    *qcmap_cmd_ptr = NULL;
  qcmap_nas_buffer_t             *qcmap_nas_buffer = NULL;
  qcmap_dss_buffer_t             *qcmap_dss_buffer = NULL;
  qcmap_qmi_service_buffer_t *qcmap_qmi_service_buffer = NULL;

  while ( (qcmap_cmd_ptr = qcmap_get_next_cmd()) != NULL ) 
  {
    QCMAP_LOG_MSG_HIGH("Command ptr: 0x%x, cmd_id: %d", 
                             qcmap_cmd_ptr, 
                             qcmap_cmd_ptr->cmd_hdr.cmd_id, 0);
/*--------------------------------------------------------------------------
      Bound check to ensure command id is bounded
--------------------------------------------------------------------------*/
    if (!QCMAP_CMD_ID_RANGE_CHECK(qcmap_cmd_ptr->cmd_hdr.cmd_id))
    {
      QCMAP_LOG_MSG_ERROR("Command ID Range check failed",0,0,0); 
      continue;
    }

    switch (qcmap_cmd_ptr->cmd_hdr.cmd_id)
    {
      case QCMAP_MOBILEAP_ENABLE_REQ_EVT:
        qcmap_mobile_ap_enable(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_DISABLE_REQ_EVT:
        qcmap_mobile_ap_disable(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_BRING_UP_WWAN_REQ_EVT:
        qcmap_bring_up_wwan(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_TEAR_DOWN_WWAN_REQ_EVT:
        qcmap_tear_down_wwan(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_GET_AUTOCONNECT_REQ_EVT:
        qcmap_get_auto_connect(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_SET_AUTOCONNECT_REQ_EVT:
        qcmap_set_auto_connect(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_GET_ROAMING_REQ_EVT:
        qcmap_get_roaming(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_SET_ROAMING_REQ_EVT:
        qcmap_set_roaming(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_SET_DMZIP_REQ_EVT:
        qcmap_set_dmzip(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_MOBILEAP_DELETE_DMZIP_REQ_EVT:
        qcmap_delete_dmzip(qcmap_cmd_ptr->cmd_data.data,
                               qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_QTI_EVT:
        qcmap_update_dtr_reg(qcmap_cmd_ptr->cmd_data.connection_handle);
        break;

      case QCMAP_DTR_CHANGE_EVT:
        if (qcmap_get_line_state() == TRUE)
        {
          qcmap_get_mac_address();
          qcmap_bring_up_std_ecm();
          qti_client_enabled = TRUE;//Enable the ecm0 interface functionality
        }
        else
        {
          if(qti_client_enabled == TRUE)
          {
            qcmap_bring_down_std_ecm();
            qti_client_enabled = FALSE;//Disable ecm0 interface functionality
          }
        }
        break;

      case QCMAP_QMI_NAS_IN_SERVICE_EVT:
        qcmap_qmi_service_buffer = (qcmap_qmi_service_buffer_t *)qcmap_cmd_ptr->cmd_data.data;
        
        if(qcmap_qmi_service_buffer != NULL)
        {
          qcmap_cm_process_qmi_service_availabilty(qcmap_qmi_service_buffer->qmi_service_id);
          free(qcmap_qmi_service_buffer);
          qcmap_qmi_service_buffer = NULL;
        }
        break;

      case QCMAP_QMI_NAS_IND_EVT:
        qcmap_nas_buffer = (qcmap_nas_buffer_t *)qcmap_cmd_ptr->cmd_data.data;
        qcmap_cm_process_qmi_nas_ind( qcmap_nas_buffer);

        break;

      case QCMAP_DSS_IND_EVT:
        QCMAP_LOG_MSG_MED("QCMAP_DSS_IND_EVT", 0, 0, 0);
        qcmap_dss_buffer = (qcmap_dss_buffer_t *)qcmap_cmd_ptr->cmd_data.data;
        qcmap_process_dss_net_evt( qcmap_dss_buffer);
        break;

      case QCMAP_V4_TIMEOUT_EVT:
      case QCMAP_V6_TIMEOUT_EVT:
        qcmap_auto_connect_timeout(qcmap_cmd_ptr->cmd_hdr.cmd_id);
        break;

      case QCMAP_MOBILEAP_SET_DHCP_REQ_EVT:
	qcmap_set_dhcp_command(qcmap_cmd_ptr->cmd_data.data,
							 qcmap_cmd_ptr->cmd_data.connection_handle);
	break;

    }
    free(qcmap_cmd_ptr);
  } /* Loop to dequeue all outstanding commands*/

  return;
} /* qcmap_process_commands */ 

