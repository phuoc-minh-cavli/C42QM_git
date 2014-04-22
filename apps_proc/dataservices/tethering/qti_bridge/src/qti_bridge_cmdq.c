/*==============================================================================

                             QTI_CMDQ.C

GENERAL DESCRIPTION
  This file deals with the Command Queue related operations

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Command Queue must me initialized

Copyright (c) 2015 - 2016, 2019 ,2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

#include "qti_bridge.h"
#include "timer.h"
#include "ds_log.h"


static qti_bridge_task_cmd_q                   qti_bridge_cmdq;

/*===========================================================================

FUNCTION QTI_BRIDGE_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  qti_rmnet_config_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void qti_bridge_cmdq_init ()
{
  ds_iot_log_high ("qti_bridge_cmdq_init ");

  q_init(&qti_bridge_cmdq);
}


/*===========================================================================

FUNCTION QTI_BRIDGE_GET_CMD_BUFFER

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
qti_bridge_cmd_t* qti_bridge_get_cmd_buffer()
{
  qti_bridge_cmd_t*                 cmd_ptr = NULL;

//Need to use Qurt API's for malloc and memory management
//This is still not available on mainline
  cmd_ptr = (qti_bridge_cmd_t*)malloc(sizeof(qti_bridge_cmd_t));

  if (cmd_ptr == NULL) 
  {
    ds_iot_log_err("Command buffer allocation failed");
  }

  return cmd_ptr;
}


/*===========================================================================

FUNCTION  QTI_BRIDGE_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the QTI queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void qti_bridge_enqueue_cmd(qti_bridge_cmd_t* cmd_ptr)
{
  q_put(&qti_bridge_cmdq, &(cmd_ptr->cmd_hdr.link));

  ds_iot_log_high1("qti_bridge_enqueue_cmd: Num commands in internal Q now: %d",
                             q_cnt(&qti_bridge_cmdq));
  return;
}

/*===========================================================================

FUNCTION QTI_BRIDGE_GET_NEXT_CMD

DESCRIPTION
  This function de-queues and returns a command from the QTI task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static qti_bridge_cmd_t* qti_bridge_get_next_cmd( void )
{
  qti_bridge_cmd_t *cmd_ptr = NULL;

  cmd_ptr = (qti_bridge_cmd_t*)q_get( &qti_bridge_cmdq );

  ds_iot_log_high2("Cmd ptr: 0x%x, num rem cmds: QTI Q count: %d",
                           cmd_ptr, q_cnt(&qti_bridge_cmdq));

  return cmd_ptr;
}

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_COMMANDS()

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
void qti_bridge_process_commands(void)
{
  qti_bridge_cmd_t                    *qti_bridge_cmd_ptr = NULL;
 
int counter = 0;

  while ( (qti_bridge_cmd_ptr = qti_bridge_get_next_cmd()) != NULL ) 
  {
    ds_iot_log_high2("Command ptr: 0x%x, cmd_id: %d", 
                             qti_bridge_cmd_ptr, 
                             qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
/*--------------------------------------------------------------------------
      Bound check to ensure command id is bounded
--------------------------------------------------------------------------*/
    if (!QTI_CMD_ID_RANGE_CHECK(qti_bridge_cmd_ptr->cmd_hdr.cmd_id))
    {
      ds_iot_log_err("Command ID Range check failed"); 
      continue;
    }


/* We are sleeping per 30 packets for
   200us to prevent watchdogs when we keep on getting DL 
   data and there is always data present for command queue 
   to process which starves other threads.
*/
	counter ++;
    if(counter >= 30)
    {
     counter = 0;
     timer_sleep(200, T_USEC, TRUE);
    }
    switch (qti_bridge_cmd_ptr->cmd_hdr.cmd_id)
    {
      case QTI_BRIDGE_RDM_OPEN_EVT:
        qti_bridge_sio_open(qti_bridge_cmd_ptr->data.port_id);
        break;

      case QTI_BRIDGE_RDM_CLOSE_EVT:
        qti_bridge_sio_close(qti_bridge_cmd_ptr->data.port_id);
        break;

      case QTI_BRIDGE_SMD_QTI_RECV_EVT:
      case QTI_BRIDGE_PHY_QTI_RECV_EVT:
        qti_bridge_sio_forward(qti_bridge_cmd_ptr->data.bridge_id,
                               qti_bridge_cmd_ptr->cmd_hdr.cmd_id,
                               qti_bridge_cmd_ptr->data.dsm_item_ptr);
        break;

      case QTI_BRIDGE_PHYS_QTI_DTR_EVT:
      case QTI_BRIDGE_SMD_QTI_DTR_EVT:
        qti_bridge_sio_set_dtr(qti_bridge_cmd_ptr->data.bridge_id,
                                qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
        break;

      case QTI_BRIDGE_SMD_QTI_CD_EVT:
        qti_bridge_sio_set_cd(qti_bridge_cmd_ptr->data.bridge_id);
        break;

      case QTI_BRIDGE_SMD_QTI_RI_EVT:
        qti_bridge_sio_set_ri(qti_bridge_cmd_ptr->data.bridge_id);
        break;

      case QTI_BRIDGE_PHY_LOW_WM_EVT:
         qti_bridge_set_flow_ctl(qti_bridge_cmd_ptr->data.bridge_id,
                                 QTI_BRIDGE_PHYS_PORT,
                                 TRUE);
        break;

      case QTI_BRIDGE_PHY_HIGH_WM_EVT:
         qti_bridge_set_flow_ctl(qti_bridge_cmd_ptr->data.bridge_id,
                                 QTI_BRIDGE_PHYS_PORT,
                                 FALSE);
        break;

      case QTI_BRIDGE_SMD_LOW_WM_EVT:
         qti_bridge_set_flow_ctl(qti_bridge_cmd_ptr->data.bridge_id,
                                 QTI_BRIDGE_SMD_PORT,
                                 TRUE);
        break;

      case QTI_BRIDGE_SMD_HIGH_WM_EVT:
         qti_bridge_set_flow_ctl(qti_bridge_cmd_ptr->data.bridge_id,
                                 QTI_BRIDGE_SMD_PORT,
                                 FALSE);
        break;
      case QTI_BRIDGE_ATFWD_EVT:
         qti_bridge_process_at_cmds(qti_bridge_cmd_ptr->data.bridge_id);
        break;

      case QTI_BRIDGE_DS_MUX_SMD_QTI_RECV_EVT:
          qti_bridge_ds_mux_sio_forward(qti_bridge_cmd_ptr->data.bridge_id,
                                        qti_bridge_cmd_ptr->cmd_hdr.cmd_id,
                                        qti_bridge_cmd_ptr->data.dsm_item_ptr);
        break;

      case QTI_BRIDGE_DS_MUX_LOGICAL_LOW_WM_EVT:
        qti_bridge_ds_mux_set_flow_ctl(qti_bridge_cmd_ptr->data.bridge_id,
                                       QTI_BRIDGE_DS_MUX_LOGICAL_PORT,
                                       TRUE);
        break;

      case QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_WM_EVT:
        qti_bridge_ds_mux_set_flow_ctl(qti_bridge_cmd_ptr->data.bridge_id,
                                       QTI_BRIDGE_DS_MUX_LOGICAL_PORT,
                                       FALSE);
        break;

      case QTI_BRIDGE_DS_MUX_SMD_QTI_DTR_EVT:
      case QTI_BRIDGE_DS_MUX_LOGICAL_QTI_DTR_EVT:
        qti_bridge_ds_mux_sio_set_dtr(qti_bridge_cmd_ptr->data.bridge_id,
                                      qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
        break;

      case QTI_BRIDGE_DS_MUX_SMD_QTI_CD_EVT:
      case QTI_BRIDGE_DS_MUX_LOGICAL_QTI_CD_EVT:
        qti_bridge_ds_mux_sio_set_cd(qti_bridge_cmd_ptr->data.bridge_id,
                                      qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
        break;

      case QTI_BRIDGE_DS_MUX_SMD_QTI_RI_EVT:
      case QTI_BRIDGE_DS_MUX_LOGICAL_QTI_RI_EVT:
        qti_bridge_ds_mux_sio_set_ri(qti_bridge_cmd_ptr->data.bridge_id,
                                      qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
        break;

      case QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE:
        qti_bridge_ds_mux_sio_close(qti_bridge_cmd_ptr->data.bridge_id,
                                    qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
		break;

      case QTI_BRIDGE_AT_POST_PROCESS_EVT:
        qti_bridge_post_process_at_cmd();
		break;
      case QTI_BRIDGE_ATFWD_QMI_SERVICE_UP_ENT:
        qti_bridge_reg_at_cmd();
		break;
      case QTI_BRIDGE_CMUX_POST_PROCESS_EVT:
	    qti_bridge_post_process_cmux_cmd();
        break;
      default:
        ds_iot_log_err1("Command ID %d not handled",
                           qti_bridge_cmd_ptr->cmd_hdr.cmd_id);
      break;
    } /* switch based on the command id */
    free(qti_bridge_cmd_ptr);
  } /* Loop to dequeue all outstanding commands*/

  return;
} /* qti_process_commands */ 
