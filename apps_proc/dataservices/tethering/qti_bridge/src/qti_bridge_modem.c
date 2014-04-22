/*===========================================================================
                         QTI_BRIDGE_PERIPHERAL.c

DESCRIPTION
  This  file contains definitions used  by the QTI AT DUN  Module.

  Copyright (c)2016, 2020-2021 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

#include "qti_bridge.h"
#ifdef QTI_LOGGING_ENABLED
#include "ULogFront.h"
#endif
#include "ds_log.h"



extern qti_bridge_info_t  qti_bridge_info_table[QTI_BRIDGES_MAX];
extern qti_bridge_qurt_t  qti_bridge_qurt;
extern qti_bridge_ds_mux_info_t *qti_bridge_ds_mux_info;
extern uint32 at_smd_port;
extern uint8 *input_at_params;
extern char* at_resp_buf;
extern uint8  at_mask;
extern boolean ds_mux_bridge_active;
extern int netmgr_get_apps_emb_port_filter_list(uint16_t * port , uint16_t * range);
#ifdef QTI_LOGGING_ENABLED
extern ULogHandle qti_ulog;
#endif

/*==============================================================================

FUNCTION QTI_BRIDGE_RECV_SMD_TO_QTI

DESCRIPTION
  Receives a dsm packet form the smd port

DEPENDENCIES
  None.

PARAMETER
  The dsm item to be forwarded
  Bridge_id - bridge id of the port

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

static void qti_bridge_recv_smd_to_qti
(
  dsm_item_type      **item,
  uint32             bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_forward_smd_to_phys() ");
#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "s2q");
#endif
  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_SMD_QTI_RECV_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;
    cmd_ptr->data.dsm_item_ptr     = *item;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}



/*==============================================================================

FUNCTION QTI_BRIDGE_FORWARD_SMD_TO_QTI_0

DESCRIPTION
  Register callbacks for each bridge channel for communication from SMD to
  physical interconnect

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_recv_smd_to_qti_0(dsm_item_type **item)
{
  qti_bridge_recv_smd_to_qti(item, QTI_BRIDGE_INSTANCE_0);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_FORWARD_SMD_TO_QTI_1

DESCRIPTION
  Register callbacks for each bridge channel for communication from SMD to
  physical interconnect

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_recv_smd_to_qti_1(dsm_item_type **item)
{
  qti_bridge_recv_smd_to_qti(item, QTI_BRIDGE_INSTANCE_1);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_GET_BRIDGE_ID_FROM_STREAM_ID

DESCRIPTION
  Get the bridge ID from SMD stream ID

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static uint32 qti_bridge_get_bridge_id_from_stream_id
(
  qti_bridge_info_t *bridge_info
)
{
  uint32                     bridge_id = QTI_BRIDGES_MAX;
  sio_stream_id_type         from_stream_id;
  sio_stream_id_type         to_stream_id;

  from_stream_id      = bridge_info->smd_stream_id;
  to_stream_id        = bridge_info->phys_stream_id;

  for(bridge_id = 0; bridge_id < QTI_BRIDGES_MAX; bridge_id++)
  {
    if(qti_bridge_info_table[bridge_id].smd_stream_id == from_stream_id ||
        qti_bridge_info_table[bridge_id].smd_stream_id == to_stream_id)
    {
      /* break if we find a match; this loop is just to find the bridge id
       * for the port on which this operation is being performed */
      break;
    }
  }
  return bridge_id;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_SEND_DTR_SIGNAL

DESCRIPTION
  Process the DTR callback from SMD

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_recv_dtr_signal (uint32 bridge_id)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_recv_dtr_signal ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_SMD_QTI_DTR_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_SMD_TO_QTI_CB

DESCRIPTION
  Callback for handling DTR signal from SMD to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_dtr_smd_to_qti_cb(void *info)
{
  qti_bridge_info_t          *bridge_info = (qti_bridge_info_t*)info;
  uint32                     bridge_id;

  bridge_id = qti_bridge_get_bridge_id_from_stream_id(bridge_info);
#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "sqdtr");
#endif
  if (bridge_id < QTI_BRIDGES_MAX)
  {
    qti_bridge_recv_dtr_signal(bridge_id);
  }
  else
  {
#ifdef QTI_LOGGING_ENABLED
   	ULogFront_RealTimePrintf(qti_ulog, 0, "sqdtrerr");
#endif
    ds_iot_log_err1("qti_bridge_dtr_smd_to_qti_cb: Invalid Bridge ID %u",
                       bridge_id);
  }
}

/*==============================================================================

FUNCTION QTI_BRIDGE_RECV_CD_SIGNAL

DESCRIPTION
  Process the DTR callback from SMD

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_recv_cd_signal (uint32 bridge_id)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_recv_dtr_signal ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_SMD_QTI_CD_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_CD_SMD_TO_QTI_CB

DESCRIPTION
  Callback for handling CD (Carrier Detect) signal from SMD to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_cd_smd_to_qti_cb(void *info)
{
  qti_bridge_info_t          *bridge_info = (qti_bridge_info_t*)info;
  uint32                     bridge_id;

  bridge_id = qti_bridge_get_bridge_id_from_stream_id(bridge_info);

  if (bridge_id < QTI_BRIDGES_MAX)
  {
    qti_bridge_recv_cd_signal(bridge_id);
  }
  else
  {
    ds_iot_log_err1("qti_bridge_cd_smd_to_qti_cb: Invalid Bridge ID %u",
                       bridge_id);
  }
}

/*==============================================================================

FUNCTION QTI_BRIDGE_RECV_RI_SIGNAL

DESCRIPTION
  Process the RI callback from SMD

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_recv_ri_signal (uint32 bridge_id)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_recv_ri_signal ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_SMD_QTI_RI_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_RI_SMD_TO_QTI_CB

DESCRIPTION
  Callback for handling RI (Ring Indicator)signal from SMD to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ri_smd_to_qti_cb(void *info)
{
  qti_bridge_info_t          *bridge_info = (qti_bridge_info_t*)info;
  uint32                     bridge_id;

  bridge_id = qti_bridge_get_bridge_id_from_stream_id(bridge_info);

  if (bridge_id < QTI_BRIDGES_MAX)
  {
    qti_bridge_recv_ri_signal(bridge_id);
  }
  else
  {
    ds_iot_log_err1("qti_bridge_ri_smd_to_qti_cb: Invalid Bridge ID %u",
                       bridge_id);
  }
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DISCONNECT_SMD_0

DESCRIPTION
   This callback function clears out the data for a bridge once it is
   fnished closing.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_clear_port
(
  uint32 bridge_id
)
{
  qti_bridge_info_t *bridge_info;

  ds_iot_log_high ("qti_bridge_clear_port ");

  bridge_info     = &qti_bridge_info_table[bridge_id];

  /* clear out the entry in smd_bridge_server_info_table */
  bridge_info->phys_port_id   = SIO_PORT_NULL;
  bridge_info->smd_port_id    = SIO_PORT_NULL;
  bridge_info->phys_stream_id = SIO_NO_STREAM_ID;
  bridge_info->smd_stream_id  = SIO_NO_STREAM_ID;

  /* call the client cb function */
  if(bridge_info->close_cb != NULL)
  {
    /* Execute callback if in local context */
    bridge_info->close_cb((void*)bridge_id);
  }

  bridge_info->close_cb = NULL;
}/* smd_bridge_server_clear_port */


/*==============================================================================

FUNCTION QTI_BRIDGE_DISCONNECT_SMD_0

DESCRIPTION
  Register callbacks for each bridge channel which is called when the SIO port
  is closed

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_disconnect_smd_0(void)
{
  qti_bridge_clear_port(QTI_BRIDGE_INSTANCE_0);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DISCONNECT_SMD_1

DESCRIPTION
  Register callbacks for each bridge channel which is called when the SIO port
  is closed

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_disconnect_smd_1(void)
{
  qti_bridge_clear_port(QTI_BRIDGE_INSTANCE_1);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_SMD_0

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_enable_smd_0 (  dsm_watermark_type *ignore, void *bridge_id)
{
  qti_bridge_enable_smd(ignore, QTI_BRIDGE_INSTANCE_0);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_SMD_1

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_enable_smd_1 (  dsm_watermark_type *ignore, void *bridge_id)
{
  qti_bridge_enable_smd(ignore, QTI_BRIDGE_INSTANCE_1);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_SMD_0

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_disable_smd_0 (  dsm_watermark_type *ignore, void *bridge_id)
{
  qti_bridge_disable_smd(ignore, QTI_BRIDGE_INSTANCE_0);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_SMD_1

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_disable_smd_1 (  dsm_watermark_type *ignore, void *bridge_id)
{
  qti_bridge_disable_smd(ignore, QTI_BRIDGE_INSTANCE_1);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_SMD

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_enable_smd
(
  dsm_watermark_type *ignore,
  int              bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_enable_smd() ");
#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "esm");
#endif
  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_SMD_LOW_WM_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_SMD

DESCRIPTION
  Dsm callback that disables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disable_smd
(
  dsm_watermark_type *ignore,
  int                 bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

#ifdef NWSTACK_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "dsm");
#endif
 
  ds_iot_log_high ("qti_bridge_disable_smd() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_SMD_HIGH_WM_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}



/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SMD_CH_OPEN

DESCRIPTION
  Open the SMD SIO channels for DS MUX operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_smd_ch_open(void)
{
  ds_iot_log_high ("qti_bridge_ds_mux_smd_ch_open ");

  qti_bridge_ds_mux_info = (qti_bridge_ds_mux_info_t*)malloc(sizeof(qti_bridge_ds_mux_info_t));
  if (qti_bridge_ds_mux_info == NULL)
  {
    ds_iot_log_err("Unable to allocate memory for qti_bridge_ds_mux_smd_ch_open, Assert");
    ASSERT(0);
  }

  memset(qti_bridge_ds_mux_info, 0, sizeof(qti_bridge_ds_mux_info_t));

  qti_bridge_ds_mux_smd_sio_open(0,SIO_PORT_SMD_DATA1);
  qti_bridge_ds_mux_smd_sio_open(1,SIO_PORT_SMD_DATA2);
  qti_bridge_ds_mux_smd_sio_open(2,SIO_PORT_SMD_DATA3);
}



/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_SEND_DTR_SIGNAL

DESCRIPTION
  Process the DTR callback from SMD and send to DS MUX logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_dtr_signal
(
  uint8 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_recv_dtr_signal ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id        = evt_id;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_DTR_SMD_TO_QTI_CB

DESCRIPTION
  Callback to handle change in DTR from SMD channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_dtr_smd_to_qti_cb_0 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_dtr_smd_to_qti_cb_0, index %u ",index);
  qti_bridge_ds_mux_recv_dtr_signal(index, QTI_BRIDGE_DS_MUX_SMD_QTI_DTR_EVT);
}

void qti_bridge_ds_mux_dtr_smd_to_qti_cb_1 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_dtr_smd_to_qti_cb_1, index %u ",index);
  qti_bridge_ds_mux_recv_dtr_signal(index,QTI_BRIDGE_DS_MUX_SMD_QTI_DTR_EVT);
}


void qti_bridge_ds_mux_dtr_smd_to_qti_cb_2 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_dtr_smd_to_qti_cb_2, index %u ",index );
  qti_bridge_ds_mux_recv_dtr_signal(index, QTI_BRIDGE_DS_MUX_SMD_QTI_DTR_EVT);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_SEND_CD_SIGNAL

DESCRIPTION
  Process the CD callback from SMD and send to DS MUX logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_cd_signal
(
  uint8 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_recv_dtr_signal ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id        = evt_id;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_CD_SMD_TO_QTI_CB

DESCRIPTION
  Callback to handle change in CD from SMD channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_cd_smd_to_qti_cb_0 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_cd_smd_to_qti_cb_0, index %u ",index);
  qti_bridge_ds_mux_recv_cd_signal(0, QTI_BRIDGE_DS_MUX_SMD_QTI_CD_EVT);
}

void qti_bridge_ds_mux_cd_smd_to_qti_cb_1 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_cd_smd_to_qti_cb_1, index %u ",index);
  qti_bridge_ds_mux_recv_cd_signal(1, QTI_BRIDGE_DS_MUX_SMD_QTI_CD_EVT);
}

void qti_bridge_ds_mux_cd_smd_to_qti_cb_2 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_cd_smd_to_qti_cb_2 , index %u ",index);
  qti_bridge_ds_mux_recv_cd_signal(2, QTI_BRIDGE_DS_MUX_SMD_QTI_CD_EVT);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_SEND_CD_SIGNAL

DESCRIPTION
  Process the RI callback from SMD and send to DS MUX logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_ri_signal
(
  uint8 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_recv_dtr_signal ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id        = evt_id;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_RI_SMD_TO_QTI_CB

DESCRIPTION
  Callback to handle change in RI from SMD channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_ri_smd_to_qti_cb_0 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_ri_smd_to_qti_cb_0, index %u ",index);
  qti_bridge_ds_mux_recv_ri_signal(0, QTI_BRIDGE_DS_MUX_SMD_QTI_RI_EVT);
}

void qti_bridge_ds_mux_ri_smd_to_qti_cb_1 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_ri_smd_to_qti_cb_1 , index %u ",index);
  qti_bridge_ds_mux_recv_ri_signal(1, QTI_BRIDGE_DS_MUX_SMD_QTI_RI_EVT);
}

void qti_bridge_ds_mux_ri_smd_to_qti_cb_2 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_ri_smd_to_qti_cb_2, index %u ",index);
  qti_bridge_ds_mux_recv_ri_signal(2, QTI_BRIDGE_DS_MUX_SMD_QTI_RI_EVT);
}

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_AT_CMDS

DESCRIPTION
  This function processes the various AT cmds handled in QTI Bridge

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_process_at_cmds(uint32 at_cmd_id)
{

  ds_iot_log_high1 ("qti_bridge_process_at_cmds cmd id %d", at_cmd_id);

  if (at_cmd_id == QTI_BRIDGE_AT_CMD_CMUX)
  {
    qti_bridge_validate_atfwd_cmux_params();
  }
  else if (at_cmd_id == QTI_BRIDGE_AT_CMD_IPR)
  {
    qti_bridge_process_ipr();
  }
  else if (at_cmd_id == QTI_BRIDGE_AT_CMD_QCEMBP)
  {
    qti_bridge_process_qcembp();
  }
}

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_IPR()

DESCRIPTION
  This function processes the AT+IPR command

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_process_ipr(void)
{
  int value = 0;
  int br_inst = 0;
  int baud_rate_value;
  sio_port_id_type                  smd_port_id;
  sio_bitrate_type                  baudrate;
  sio_bitrate_type                  curr_baudrate;
  boolean                           at_resp = FALSE;
  boolean                           is_peripheral_usb = FALSE;
  sio_ioctl_param_type              param;
  at_fwd_resp_result_enum_v01       at_result;

  ds_iot_log_high1 ("qti_bridge_process_ipr for %d", at_smd_port);

/*---------------------------------------------------------------------------
Detect the UART stream ID based on the SMD port ID sent from modem
---------------------------------------------------------------------------*/
  switch (at_smd_port)
  {
    case QTI_BRIDGE_DS_SMD:
      smd_port_id = SIO_PORT_SMD_DS;
    break;

    case QTI_BRIDGE_SMD_DATA1:
      smd_port_id = SIO_PORT_SMD_DATA1;
    break;

    case QTI_BRIDGE_SMD_DATA2:
      smd_port_id = SIO_PORT_SMD_DATA2;
    break;

    case QTI_BRIDGE_SMD_DATA3:
      smd_port_id = SIO_PORT_SMD_DATA3;
    break;

    case QTI_BRIDGE_SMD_DATA4:
      smd_port_id = SIO_PORT_SMD_DATA4;
    break;

    case QTI_BRIDGE_SMD_DATA11:
      smd_port_id = SIO_PORT_SMD_DATA11;
    break;

    default:
      ds_iot_log_err1 ("Incorrect SMD port from modem for IPR %d",
                         at_smd_port );
      at_resp = FALSE;
      goto last;
  }

  for (br_inst =0 ; br_inst < QTI_BRIDGES_MAX; br_inst++)
  {
    if (smd_port_id == qti_bridge_info_table[br_inst].smd_port_id)
    {
      if (qti_bridge_info_table[br_inst].uart_stream_id == SIO_NO_STREAM_ID)
      {
        continue;
      }
      else
      {
        break;
      }
    }
  }

  if(br_inst == QTI_BRIDGES_MAX)
  {
    // This means none of the bridging instance has UART as the peripheral
    // USB will be the peripheral for the bridging instances
    is_peripheral_usb = TRUE;
  }

  if (at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 | QTI_BRIDGE_AT_MASK_QU_V01))
  {
    if(is_peripheral_usb)
    {
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_115200;
    }
    else
    {
        param.cur_bitrate = &curr_baudrate;
        sio_ioctl(qti_bridge_info_table[br_inst].uart_stream_id,
                SIO_IOCTL_GET_CURRENT_BAUD_RATE,
                &param );
    switch (curr_baudrate)
    {
      case SIO_BITRATE_300:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_300;
      break;

      case SIO_BITRATE_600:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_600;
      break;

      case SIO_BITRATE_1200:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1200;
      break;

      case SIO_BITRATE_2400:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_2400;
      break;

      case SIO_BITRATE_4800:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_4800;
      break;

      case SIO_BITRATE_9600:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_9600;
      break;

      case SIO_BITRATE_14400:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_14400;
      break;

      case SIO_BITRATE_19200:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_19200;
      break;

      case SIO_BITRATE_38400:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_38400;
      break;

      case SIO_BITRATE_57600:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_57600;
      break;

      case SIO_BITRATE_115200:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_115200;
      break;

      case SIO_BITRATE_230400:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_230400;
      break;

      case SIO_BITRATE_460800:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_460800;
      break;

      case SIO_BITRATE_921600:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_921600;
      break;

      case SIO_BITRATE_1000000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1000000;
      break;

      case SIO_BITRATE_1200000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1200000;
      break;

      case SIO_BITRATE_1209677:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1209677;
      break;

      case SIO_BITRATE_1250000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1250000;
      break;

      case SIO_BITRATE_1293103:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1293103;
      break;

      case SIO_BITRATE_1339286:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1339286;
      break;

      case SIO_BITRATE_1388889:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1388889;
      break;

      case SIO_BITRATE_1442308:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1442308;
      break;

      case SIO_BITRATE_1500000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1500000;
      break;

      case SIO_BITRATE_1562500:
       baud_rate_value = QTI_BRIDGE_BAUD_RATE_1562500;
      break;

      case SIO_BITRATE_1630435:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1630435;
      break;

      case SIO_BITRATE_1704545:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1704545;
      break;

      case SIO_BITRATE_1785714:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_1785714;
      break;

      case SIO_BITRATE_2000000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_2000000;
      break;

      case SIO_BITRATE_3000000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_3000000;
      break;

      case SIO_BITRATE_3200000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_3200000;
      break;

      case SIO_BITRATE_3686400:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_3686400;
      break;

      case SIO_BITRATE_4000000:
        baud_rate_value = QTI_BRIDGE_BAUD_RATE_4000000;
      break;

      default:
        baud_rate_value = curr_baudrate;
        ds_iot_log_err1 ("Fetched invalid baud rate from UART %d",
                           baud_rate_value);
        at_resp = FALSE;
        goto last;
    }
    }

    at_resp_buf = (char*)malloc(QTI_BRIDGE_MAX_AT_TOKENS);

    if (at_resp_buf == NULL)
    {
      ds_iot_log_err(" Not able to allocate memory for at_resp_buf");
      at_resp = FALSE;
      goto last;
    }

    memset (at_resp_buf,0,QTI_BRIDGE_MAX_AT_TOKENS);

    snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,
           "+IPR:%d \r\n", baud_rate_value);

    at_resp = TRUE;
  }
  else if (at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 | QTI_BRIDGE_AT_MASK_EQ_V01|
                  QTI_BRIDGE_AT_MASK_AR_V01))
  {
    if((is_peripheral_usb) && (ds_mux_bridge_active == FALSE))
    {
      ds_iot_log_high(" IPR Baud rate setting is not supported on USB");
      at_resp = FALSE;
      goto last;
    }
    else
    {
  /*---------------------------------------------------------------------------
    Determine the baud rate value we need to update and then call SIO IOCTL
  ---------------------------------------------------------------------------*/
    value = atoi((const char*)input_at_params);

    switch (value)
    {
      case QTI_BRIDGE_BAUD_RATE_300:
        baudrate = SIO_BITRATE_300;
      break;

      case QTI_BRIDGE_BAUD_RATE_600:
        baudrate = SIO_BITRATE_600;
      break;

      case QTI_BRIDGE_BAUD_RATE_1200:
        baudrate = SIO_BITRATE_1200;
      break;

      case QTI_BRIDGE_BAUD_RATE_2400:
        baudrate = SIO_BITRATE_2400;
      break;

      case QTI_BRIDGE_BAUD_RATE_4800:
        baudrate = SIO_BITRATE_4800;
      break;

      case QTI_BRIDGE_BAUD_RATE_9600:
        baudrate = SIO_BITRATE_9600;
      break;

      case QTI_BRIDGE_BAUD_RATE_14400:
        baudrate = SIO_BITRATE_14400;
      break;

      case QTI_BRIDGE_BAUD_RATE_19200:
        baudrate = SIO_BITRATE_19200;
      break;

      case QTI_BRIDGE_BAUD_RATE_38400:
        baudrate = SIO_BITRATE_38400;
      break;

      case QTI_BRIDGE_BAUD_RATE_57600:
        baudrate = SIO_BITRATE_57600;
      break;

      case QTI_BRIDGE_BAUD_RATE_115200:
        baudrate = SIO_BITRATE_115200;
      break;

      case QTI_BRIDGE_BAUD_RATE_230400:
        baudrate = SIO_BITRATE_230400;
      break;

      case QTI_BRIDGE_BAUD_RATE_460800:
        baudrate = SIO_BITRATE_460800;
      break;

      case QTI_BRIDGE_BAUD_RATE_921600:
        baudrate = SIO_BITRATE_921600;
      break;

      case QTI_BRIDGE_BAUD_RATE_1000000:
        baudrate = SIO_BITRATE_1000000;
      break;

      case QTI_BRIDGE_BAUD_RATE_1200000:
        baudrate = SIO_BITRATE_1200000;
      break;

      case QTI_BRIDGE_BAUD_RATE_1209677:
        baudrate = SIO_BITRATE_1209677;
      break;

      case QTI_BRIDGE_BAUD_RATE_1250000:
        baudrate = SIO_BITRATE_1250000;
      break;

      case QTI_BRIDGE_BAUD_RATE_1293103:
        baudrate = SIO_BITRATE_1293103;
      break;

      case QTI_BRIDGE_BAUD_RATE_1339286:
        baudrate = SIO_BITRATE_1339286;
      break;

      case QTI_BRIDGE_BAUD_RATE_1388889:
        baudrate = SIO_BITRATE_1388889;
      break;

      case QTI_BRIDGE_BAUD_RATE_1442308:
        baudrate = SIO_BITRATE_1442308;
      break;

      case QTI_BRIDGE_BAUD_RATE_1500000:
        baudrate = SIO_BITRATE_1500000;
      break;

      case QTI_BRIDGE_BAUD_RATE_1562500:
       baudrate = SIO_BITRATE_1562500;
      break;

      case QTI_BRIDGE_BAUD_RATE_1630435:
        baudrate = SIO_BITRATE_1630435;
      break;

      case QTI_BRIDGE_BAUD_RATE_1704545:
        baudrate = SIO_BITRATE_1704545;
      break;

      case QTI_BRIDGE_BAUD_RATE_1785714:
        baudrate = SIO_BITRATE_1785714;
      break;

      case QTI_BRIDGE_BAUD_RATE_2000000:
        baudrate = SIO_BITRATE_2000000;
      break;

      case QTI_BRIDGE_BAUD_RATE_3000000:
        baudrate = SIO_BITRATE_3000000;
      break;

      case QTI_BRIDGE_BAUD_RATE_3200000:
        baudrate = SIO_BITRATE_3200000;
      break;

      case QTI_BRIDGE_BAUD_RATE_3686400:
        baudrate = SIO_BITRATE_3686400;
      break;

      case QTI_BRIDGE_BAUD_RATE_4000000:
        baudrate = SIO_BITRATE_4000000;
      break;

      default:
        ds_iot_log_err1 ("Invalid Baud rate from modem for IPR %d",
                           value);
        at_resp = FALSE;
        goto last;
    }

    if((br_inst == QTI_BRIDGES_MAX) && (ds_mux_bridge_active))
    {
        for (br_inst =0 ; br_inst < QTI_BRIDGES_MAX; br_inst++)
        {
            if ((qti_bridge_info_table[br_inst].uart_stream_id !=  0) && 
                    ((qti_bridge_info_table[br_inst].uart_stream_id) ==
                     (qti_bridge_info_table[br_inst].phys_stream_id)))
            {
                break;
            }
        }
        if(br_inst == QTI_BRIDGES_MAX)
        {
            ds_iot_log_high(" IPR Baud rate setting is not supported on USB");
            at_resp = FALSE;
            goto last;
        }
        else
        {
            qti_bridge_info_table[br_inst].uart_baud_rate = baudrate;
        }
    }
    else
    {
        qti_bridge_info_table[br_inst].uart_baud_rate = baudrate;
    }

    at_resp = TRUE;
  }
  }
  else if(at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 |QTI_BRIDGE_AT_MASK_QU_V01|
                      QTI_BRIDGE_AT_MASK_EQ_V01))
  {
    ds_iot_log_high(" Received AT+IPR=?");
    at_resp_buf = (char*)malloc(QTI_BRIDGE_MAX_AT_TOKENS);

    if (at_resp_buf == NULL)
    {
      ds_iot_log_err(" Not able to allocate memory for at_resp_buf");
      at_resp = FALSE;
      goto last;
    }

    memset (at_resp_buf,0,QTI_BRIDGE_MAX_AT_TOKENS);

    snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,
           "+IPR:%d %d %d %d %d %d %d %d %d %d"
            " %d %d %d %d %d %d %d %d %d %d %d"
            " %d %d %d %d %d %d %d %d %d %d %d \r\n", 
            QTI_BRIDGE_BAUD_RATE_300,QTI_BRIDGE_BAUD_RATE_600,
            QTI_BRIDGE_BAUD_RATE_1200,QTI_BRIDGE_BAUD_RATE_2400,
            QTI_BRIDGE_BAUD_RATE_4800,QTI_BRIDGE_BAUD_RATE_9600,
            QTI_BRIDGE_BAUD_RATE_14400, QTI_BRIDGE_BAUD_RATE_19200,
            QTI_BRIDGE_BAUD_RATE_38400, QTI_BRIDGE_BAUD_RATE_57600,
            QTI_BRIDGE_BAUD_RATE_115200,QTI_BRIDGE_BAUD_RATE_230400, 
            QTI_BRIDGE_BAUD_RATE_460800, QTI_BRIDGE_BAUD_RATE_921600, 
            QTI_BRIDGE_BAUD_RATE_1000000, QTI_BRIDGE_BAUD_RATE_1200000,
            QTI_BRIDGE_BAUD_RATE_1209677, QTI_BRIDGE_BAUD_RATE_1250000, 
            QTI_BRIDGE_BAUD_RATE_1293103, QTI_BRIDGE_BAUD_RATE_1339286, 
            QTI_BRIDGE_BAUD_RATE_1388889, QTI_BRIDGE_BAUD_RATE_1442308, 
            QTI_BRIDGE_BAUD_RATE_1500000, QTI_BRIDGE_BAUD_RATE_1562500, 
            QTI_BRIDGE_BAUD_RATE_1630435, QTI_BRIDGE_BAUD_RATE_1704545, 
            QTI_BRIDGE_BAUD_RATE_1785714, QTI_BRIDGE_BAUD_RATE_2000000, 
            QTI_BRIDGE_BAUD_RATE_3000000, QTI_BRIDGE_BAUD_RATE_3200000,
            QTI_BRIDGE_BAUD_RATE_3686400, QTI_BRIDGE_BAUD_RATE_4000000);

    at_resp = TRUE;
  }

last:
  if (at_resp)
  {
    at_result = AT_RESULT_OK_V01;
  }
  else
  {
    at_result = AT_RESULT_ERROR_V01;
  }

  qapi_atfwd_send_resp("+IPR",at_resp_buf,at_result);

  if(at_resp_buf != NULL)
  {
    free(at_resp_buf);
    at_resp_buf = NULL;
  }

  if (input_at_params != NULL)
  {
    free(input_at_params);
    input_at_params = NULL;
  }

  return;
}


/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_QCEMBP()

DESCRIPTION
  This function processes the AT$QCEMBP command

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/


void qti_bridge_process_qcembp(void)
{

  at_fwd_resp_result_enum_v01       at_result;
  boolean                           at_resp = FALSE;
  int                               ret = -1;

  uint16_t  port = 0;
  uint16_t  range = 0; 

  ds_iot_log_high ("qti_bridge_process_qcembp");


  at_resp_buf = (char*)malloc(QTI_BRIDGE_MAX_AT_TOKENS + 1);
  if (at_resp_buf == NULL)
  {
     ds_iot_log_err(" Not able to allocate memory for at_resp_buf");
     at_resp = FALSE;
     goto last;
  }

  memset(at_resp_buf,0,QTI_BRIDGE_MAX_AT_TOKENS + 1);
  
  at_resp_buf[QTI_BRIDGE_MAX_AT_TOKENS] = '\0';

  if (at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 | QTI_BRIDGE_AT_MASK_QU_V01))
  {
     ret =  netmgr_get_apps_emb_port_filter_list(&port, &range);
	 
	 if(ret == 0)
	 {

	     ds_iot_log_high3 ("qti_bridge_process_qcembp: Start Port: %d End Port %d \n",port , port + range , 0 );
         snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,"$QCEMBP:%d-%d \r\n",port,port+range);

         at_resp = TRUE;

	 }
	 else if(ret == -2)
	 {
         snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,"$QCEMBP:ENOENT \r\n");

         at_resp = FALSE;
	 }
     else
     {
        ds_iot_log_err(" Not able to get apps emb port info");
		at_resp = FALSE;
        goto last;

     }

  }

last:
  if (at_resp)
  {
    at_result = AT_RESULT_OK_V01;
  }
  else
  {
    at_result = AT_RESULT_ERROR_V01;
  }

  if(at_resp_buf)
  {
    at_resp_buf[QTI_BRIDGE_MAX_AT_TOKENS] = '\0';
    qapi_atfwd_send_resp("$QCEMBP",at_resp_buf,at_result);
  }
  if(at_resp_buf != NULL)
  {
    free(at_resp_buf);
    at_resp_buf = NULL;
  }

  if (input_at_params != NULL)
  {
    free(input_at_params);
    input_at_params = NULL;
  }

  return;

}

/*==============================================================================

FUNCTION QTI_BRIDGE_POST_PROCESS_AT_CMD

DESCRIPTION
  Function invoked to do post processing of AT cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_post_process_at_cmd (void)
{

  sio_stream_id_type                uart_stream_id = SIO_NO_STREAM_ID;
  sio_ioctl_param_type              param;
  int                               br_inst = 0;

  ds_iot_log_high ("qti_bridge_post_process_at_cmd");

  for (br_inst =0 ; br_inst < QTI_BRIDGES_MAX; br_inst++)
  {
    if ((qti_bridge_info_table[br_inst].uart_stream_id) ==
        (qti_bridge_info_table[br_inst].phys_stream_id))
    {
        uart_stream_id = qti_bridge_info_table[br_inst].phys_stream_id;
      break;
    }
  }

  if(br_inst == QTI_BRIDGES_MAX)
  {
    ds_iot_log_err ("Unable to fetch UART stream ID");
    return;
  }

  param.bitrate = qti_bridge_info_table[br_inst].uart_baud_rate;
  sio_ioctl(uart_stream_id,
            SIO_IOCTL_CHANGE_BAUD_NOW,
            &param );
}


