/*===============================================================================

                             QTI_BRIDGE_LOGICAL.C

GENERAL DESCRIPTION
  This file deals with the SIO related operations for Logical channels for DS 
  MUX

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Command Queue must be initialized

Copyright (c) 2016, 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
#include "qti_bridge.h"
#include "ds_mux_logical.h"
#include "ds_log.h"


extern qti_bridge_ds_mux_info_t *qti_bridge_ds_mux_info;

extern qti_bridge_info_t qti_bridge_info_table[QTI_BRIDGES_MAX];

extern qti_bridge_qurt_t  qti_bridge_qurt;


static const sio_close_func_ptr_type
  qti_bridge_ds_mux_logical_sio_close_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] = 
{
  qti_bridge_ds_mux_logical_sio_close_cb_0,
  qti_bridge_ds_mux_logical_sio_close_cb_1,
  qti_bridge_ds_mux_logical_sio_close_cb_2
};


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_CH_OPEN

DESCRIPTION
  Open the LOGICAL SIO channels for DS MUX operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_logical_ch_open(void)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_ch_open ");

  if (qti_bridge_ds_mux_info == NULL)
  {
    ds_iot_log_high ("qti_bridge_ds_mux_info is not initialized ");
    return;
  }

  qti_bridge_ds_mux_logical_sio_open(0,SIO_PORT_DATA_MUX_1);
  qti_bridge_ds_mux_logical_sio_open(1,SIO_PORT_DATA_MUX_2);
  qti_bridge_ds_mux_logical_sio_open(2,SIO_PORT_DATA_MUX_3);

}



/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_RECV_SMD_TO_QTI

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

static void qti_bridge_ds_mux_recv_smd_to_qti
(
  dsm_item_type      **item,
  uint32             bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_forward_smd_to_phys() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_DS_MUX_SMD_QTI_RECV_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;
    cmd_ptr->data.dsm_item_ptr     = *item;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}



/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_FORWARD_SMD_TO_QTI_0

DESCRIPTION
  Register callbacks for each bridge channel for communication from SMD to
  DS MUX Logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_ds_mux_recv_smd_to_qti_0(dsm_item_type **item)
{
  qti_bridge_ds_mux_recv_smd_to_qti(item, 0);
}

void qti_bridge_ds_mux_recv_smd_to_qti_1(dsm_item_type **item)
{
  qti_bridge_ds_mux_recv_smd_to_qti(item, 1);
}

void qti_bridge_ds_mux_recv_smd_to_qti_2(dsm_item_type **item)
{
  qti_bridge_ds_mux_recv_smd_to_qti(item, 2);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_LOGICAL

DESCRIPTION
  dsm callback that enables the logical port 
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_enable_logical
(
  uint8              bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_enable_logical() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_DS_MUX_LOGICAL_LOW_WM_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_LOGICAL

DESCRIPTION
  dsm callback that enables the logical port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_ds_mux_enable_logical_0(dsm_watermark_type *item, void* unused)
{
  qti_bridge_enable_logical(0);
}

void qti_bridge_ds_mux_enable_logical_1(dsm_watermark_type *item, void* unused)
{
  qti_bridge_enable_logical(1);
}

void qti_bridge_ds_mux_enable_logical_2(dsm_watermark_type *item, void* unused)
{
  qti_bridge_enable_logical(2);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_LOGICAL

DESCRIPTION
  dsm callback that enables the logical port 
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_disable_logical
(
  uint8              bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_enable_logical() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_WM_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_LOGICAL

DESCRIPTION
  dsm callback that Disable the logical port

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_ds_mux_disable_logical_0(dsm_watermark_type *item, void* unused)
{
  qti_bridge_disable_logical(0);
}

void qti_bridge_ds_mux_disable_logical_1(dsm_watermark_type *item, void* unused)
{
  qti_bridge_disable_logical(1);
}

void qti_bridge_ds_mux_disable_logical_2(dsm_watermark_type *item, void* unused)
{
  qti_bridge_disable_logical(2);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_LOW_TX_WM

DESCRIPTION
  Call back which will be invoked when LOW WM is hit on TX logical MUX channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_low_tx_wm_0(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_low_tx_wm_0 ");
  //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_low_tx_wm_1(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_low_tx_wm_1 ");
   //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_low_tx_wm_2(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_low_tx_wm_2 ");
   //TODO: Functionality to be decided
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_TX_WM

DESCRIPTION
  Call back which will be invoked when HIGH WM is hit on TX logical MUX channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_high_tx_wm_0(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_high_tx_wm_0 ");
  //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_high_tx_wm_1(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_high_tx_wm_1 ");
   //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_high_tx_wm_2(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_high_tx_wm_2 ");
   //TODO: Functionality to be decided
}



/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_LOW_RX_WM

DESCRIPTION
  Call back which will be invoked when LOW WM is hit on RX logical MUX channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_low_rx_wm_0(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_low_rx_wm_0 ");
  //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_low_rx_wm_1(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_low_rx_wm_1 ");
   //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_low_rx_wm_2(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_low_rx_wm_2 ");
   //TODO: Functionality to be decided
}



/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_RX_WM

DESCRIPTION
  Call back which will be invoked when HIGH WM is hit on RX logical MUX channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_high_rx_wm_0(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_high_rx_wm_0 ");
  //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_high_rx_wm_1(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_high_rx_wm_1 ");
   //TODO: Functionality to be decided
}

void qti_bridge_ds_mux_logical_high_rx_wm_2(dsm_watermark_type *item, void* unused)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_high_rx_wm_2 ");
   //TODO: Functionality to be decided
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_RX_WM_NON_EMPTY_FUNC

DESCRIPTION
  Call back which will be invoked when there is a DSM item to be processed in
  the RX direction
  1. De-queue data from the logical channel RX Queue
  2. Send it to appropriate SMD channel
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_rx_wm_non_empty_func_0
(
  dsm_watermark_type *item,
  void* unused
)
{
  dsm_item_type* dsm_item_ptr = NULL;

  ds_iot_log_high ("qti_bridge_ds_mux_logical_rx_wm_non_empty_func_0 ");

  //DATA DLCI starts from 1, 0 is control DLCI
  //This is a tight while loop, if the queue size is too large it may lead to a watchdog in CAT4 

  while ((dsm_item_ptr = ds_mux_logical_get_dsm_item_from_wmk(1)) != NULL)
  {
    sio_transmit(qti_bridge_ds_mux_info->smd_stream_id[0], dsm_item_ptr);
  }
}

void qti_bridge_ds_mux_logical_rx_wm_non_empty_func_1
(
  dsm_watermark_type *item,
  void* unused
)
{
  dsm_item_type* dsm_item_ptr = NULL;

  ds_iot_log_high ("qti_bridge_ds_mux_logical_rx_wm_non_empty_func_1 ");

  while ((dsm_item_ptr = ds_mux_logical_get_dsm_item_from_wmk(2)) != NULL)
  {
    sio_transmit(qti_bridge_ds_mux_info->smd_stream_id[1], dsm_item_ptr);
  }
}

void qti_bridge_ds_mux_logical_rx_wm_non_empty_func_2
(
  dsm_watermark_type *item,
  void* unused
)
{
  dsm_item_type* dsm_item_ptr = NULL;

  ds_iot_log_high ("qti_bridge_ds_mux_logical_rx_wm_non_empty_func_2 ");

  while ((dsm_item_ptr = ds_mux_logical_get_dsm_item_from_wmk(3)) != NULL)
  {
    sio_transmit(qti_bridge_ds_mux_info->smd_stream_id[2], dsm_item_ptr);
  }
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_DTR_LOGICAL_TO_QTI_CB

DESCRIPTION
  Callback to handle change in DTR from DS MUX Logical channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_dtr_logical_to_qti_cb_0 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_dtr_logical_to_qti_cb_0, index %u ",index);
  qti_bridge_ds_mux_recv_dtr_signal(0, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_DTR_EVT);
}

void qti_bridge_ds_mux_dtr_logical_to_qti_cb_1 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_dtr_logical_to_qti_cb_1, index %u ",index);
  qti_bridge_ds_mux_recv_dtr_signal(1, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_DTR_EVT);
}

void qti_bridge_ds_mux_dtr_logical_to_qti_cb_2 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_dtr_logical_to_qti_cb_2, index %u ",index);
  qti_bridge_ds_mux_recv_dtr_signal(2, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_DTR_EVT);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_CD_LOGICAL_TO_QTI_CB

DESCRIPTION
  Callback to handle change in CD from DS MUX Logical channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_cd_logical_to_qti_cb_0 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_cd_logical_to_qti_cb_0, index %u ",index);
  qti_bridge_ds_mux_recv_cd_signal(0, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_CD_EVT);
}

void qti_bridge_ds_mux_cd_logical_to_qti_cb_1 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_cd_logical_to_qti_cb_1, index %u ",index);
  qti_bridge_ds_mux_recv_cd_signal(1, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_CD_EVT);
}

void qti_bridge_ds_mux_cd_logical_to_qti_cb_2 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_cd_logical_to_qti_cb_2, index %u ",index);
  qti_bridge_ds_mux_recv_cd_signal(2, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_CD_EVT);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_RI_LOGICAL_TO_QTI_CB

DESCRIPTION
  Callback to handle change in RI from DS MUX Logical channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_ri_logical_to_qti_cb_0 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_ri_logical_to_qti_cb_0, index %u ",index);
  qti_bridge_ds_mux_recv_ri_signal(0, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_RI_EVT);
}

void qti_bridge_ds_mux_ri_logical_to_qti_cb_1 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_ri_logical_to_qti_cb_1, index %u ",index );
  qti_bridge_ds_mux_recv_ri_signal(1, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_RI_EVT);
}

void qti_bridge_ds_mux_ri_logical_to_qti_cb_2 (void* cb_data)
{
  uint8_t *index_ptr = (uint8_t*)cb_data;
  uint8_t index = *index_ptr;

  ds_iot_log_high1 ("qti_bridge_ds_mux_ri_logical_to_qti_cb_2, index %u ",index);
  qti_bridge_ds_mux_recv_ri_signal(2, QTI_BRIDGE_DS_MUX_LOGICAL_QTI_RI_EVT);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE_CB

DESCRIPTION
  SIO close CB for DS MUX Logical Channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_sio_close(uint32 channel_id)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high1 ("qti_bridge_ds_mux_logical_sio_close, channel id %u ",
                     channel_id);

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE;
    cmd_ptr->data.bridge_id        = channel_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE_CB

DESCRIPTION
  SIO close CB for DS MUX Logical Channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_sio_close_cb_0(void)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_sio_close_cb_0,");
  qti_bridge_ds_mux_logical_sio_close(0);
}

void qti_bridge_ds_mux_logical_sio_close_cb_1(void)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_sio_close_cb_1,");
  qti_bridge_ds_mux_logical_sio_close(1);
}

void qti_bridge_ds_mux_logical_sio_close_cb_2(void)
{
  ds_iot_log_high ("qti_bridge_ds_mux_logical_sio_close_cb_2," );
  qti_bridge_ds_mux_logical_sio_close(2);
}





/*==============================================================================

FUNCTION QTI_BRIDGE_LOGICAL_CHANNEL_CLOSE

DESCRIPTION
  Close the DS MUX logical channels opened by QTI Bridge at time of DS MUX
  active init

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void   qti_bridge_logical_channel_close()
{
  int idx =0;

  for (idx = 0; idx<QTI_BRIDGES_DS_MUX_CHANNEL_MAX; idx++)
  {
    sio_close(qti_bridge_ds_mux_info->logical_stream_id[idx],
              qti_bridge_ds_mux_logical_sio_close_cb[idx]);
    qti_bridge_ds_mux_info->logical_stream_id[idx] = SIO_NO_STREAM_ID;
  }
}

