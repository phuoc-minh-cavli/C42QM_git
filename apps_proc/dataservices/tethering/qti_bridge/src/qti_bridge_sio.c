/*===============================================================================

                             QTI_BRIDGE_SIO.C

GENERAL DESCRIPTION
  This file deals with the SIO related operations for QTI Bridge

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Command Queue must be initialized

Copyright (c) 2017-2019, 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
#include "qti_bridge.h"
#include "ds_mux.h"
#include "ds_mux_api.h"
#include "fs_public.h"
#ifdef QTI_LOGGING_ENABLED
#include "ULogFront.h"
#endif
#include "ds_log.h"


qti_bridge_ds_mux_info_t *qti_bridge_ds_mux_info = NULL;

extern qti_bridge_info_t qti_bridge_info_table[QTI_BRIDGES_MAX];

extern qti_bridge_qurt_t  qti_bridge_qurt;

extern boolean ds_mux_bridge_active;

extern boolean  at_cmux_handle_in_progress;
#ifdef QTI_LOGGING_ENABLED
extern ULogHandle qti_ulog;
#endif

ds_mux_ext_client_event_info_type event_info;

uint32_t qti_bridge_dne_wm_level;

static wm_cb_type
qti_bridge_enable_phys_on_bridge[QTI_BRIDGES_MAX] =
{
  qti_bridge_enable_phys_0,
  qti_bridge_enable_phys_1
};

const wm_cb_type
qti_bridge_disable_phys_on_bridge[QTI_BRIDGES_MAX] =
{
  qti_bridge_disable_phys_0,
  qti_bridge_disable_phys_1
};

static wm_cb_type
qti_bridge_enable_smd_on_bridge[QTI_BRIDGES_MAX] =
{
  qti_bridge_enable_smd_0,
  qti_bridge_enable_smd_1
};

static wm_cb_type
qti_bridge_disable_smd_on_bridge[QTI_BRIDGES_MAX] =
{
  qti_bridge_disable_smd_0,
  qti_bridge_disable_smd_1
};


static sio_rx_func_ptr_type
  qti_bridge_smd_to_qti_cbs[QTI_BRIDGES_MAX] =
{
  qti_bridge_recv_smd_to_qti_0,
  qti_bridge_recv_smd_to_qti_1
};

static sio_rx_func_ptr_type
  qti_bridge_phys_to_qti_cbs[QTI_BRIDGES_MAX] =
{
  qti_bridge_recv_phys_to_qti_0,
  qti_bridge_recv_phys_to_qti_1
};

static sio_vv_func_ptr_type
  qti_bridge_phys_to_qti_dtr_cbs[QTI_BRIDGES_MAX] =
{
  qti_bridge_dtr_phys_to_qti_0,
  qti_bridge_dtr_phys_to_qti_1
};

static qti_bridge_cb_func_ptr_type
  qti_bridge_close_cbs[QTI_BRIDGES_MAX] =
{
  qti_bridge_disconnect_smd_0,
  qti_bridge_disconnect_smd_1
};


static const wm_cb_type
  qti_bridge_ds_mux_enable_logical[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_enable_logical_0,
  qti_bridge_ds_mux_enable_logical_1,
  qti_bridge_ds_mux_enable_logical_2
};


static const wm_cb_type
  qti_bridge_ds_mux_disable_logical[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_disable_logical_0,
  qti_bridge_ds_mux_disable_logical_1,
  qti_bridge_ds_mux_disable_logical_2
};

static const wm_cb_type
  qti_bridge_ds_mux_logical_low_tx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_logical_low_tx_wm_0,
  qti_bridge_ds_mux_logical_low_tx_wm_1,
  qti_bridge_ds_mux_logical_low_tx_wm_2
};

static const wm_cb_type
  qti_bridge_ds_mux_logical_high_tx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_logical_high_tx_wm_0,
  qti_bridge_ds_mux_logical_high_tx_wm_1,
  qti_bridge_ds_mux_logical_high_tx_wm_2
};

static const wm_cb_type
  qti_bridge_ds_mux_logical_low_rx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_logical_low_rx_wm_0,
  qti_bridge_ds_mux_logical_low_rx_wm_1,
  qti_bridge_ds_mux_logical_low_rx_wm_2
};

static const wm_cb_type
  qti_bridge_ds_mux_logical_high_rx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_logical_high_rx_wm_0,
  qti_bridge_ds_mux_logical_high_rx_wm_1,
  qti_bridge_ds_mux_logical_high_rx_wm_2
};

static const wm_cb_type
  qti_bridge_ds_mux_logical_rx_wm_non_empty_func[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_logical_rx_wm_non_empty_func_0,
  qti_bridge_ds_mux_logical_rx_wm_non_empty_func_1,
  qti_bridge_ds_mux_logical_rx_wm_non_empty_func_2
};


static const sio_vpu_func_ptr_type
  qti_bridge_ds_mux_dtr_smd_to_qti_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_dtr_smd_to_qti_cb_0,
  qti_bridge_ds_mux_dtr_smd_to_qti_cb_1,
  qti_bridge_ds_mux_dtr_smd_to_qti_cb_2
};

static const sio_vpu_func_ptr_type
  qti_bridge_ds_mux_dtr_logical_to_qti_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_dtr_logical_to_qti_cb_0,
  qti_bridge_ds_mux_dtr_logical_to_qti_cb_1,
  qti_bridge_ds_mux_dtr_logical_to_qti_cb_2
};


static const sio_vpu_func_ptr_type
  qti_bridge_ds_mux_cd_smd_to_qti_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_cd_smd_to_qti_cb_0,
  qti_bridge_ds_mux_cd_smd_to_qti_cb_1,
  qti_bridge_ds_mux_cd_smd_to_qti_cb_2
};


static const sio_vpu_func_ptr_type
  qti_bridge_ds_mux_cd_logical_to_qti_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_cd_logical_to_qti_cb_0,
  qti_bridge_ds_mux_cd_logical_to_qti_cb_1,
  qti_bridge_ds_mux_cd_logical_to_qti_cb_2
};


static const sio_vpu_func_ptr_type
  qti_bridge_ds_mux_ri_smd_to_qti_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_ri_smd_to_qti_cb_0,
  qti_bridge_ds_mux_ri_smd_to_qti_cb_1,
  qti_bridge_ds_mux_ri_smd_to_qti_cb_2
};

static const sio_vpu_func_ptr_type
  qti_bridge_ds_mux_ri_logical_to_qti_cb[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_ri_logical_to_qti_cb_0,
  qti_bridge_ds_mux_ri_logical_to_qti_cb_1,
  qti_bridge_ds_mux_ri_logical_to_qti_cb_2
};

static const sio_rx_func_ptr_type
  qti_bridge_ds_mux_smd_to_qti_cbs[QTI_BRIDGES_DS_MUX_CHANNEL_MAX] =
{
  qti_bridge_ds_mux_recv_smd_to_qti_0,
  qti_bridge_ds_mux_recv_smd_to_qti_1,
  qti_bridge_ds_mux_recv_smd_to_qti_2
};


/*==============================================================================

FUNCTION QTI_OPEN_BRIDGE

DESCRIPTION
  Sets up QTI data/cntl bridge b/w Apps-modem for the given
  physical SIO port ID

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_open
(
  qti_bridge_info_t* bridge_info,
  uint32 bridge_id
)
{
  dsm_watermark_type *wm;
  sio_open_type sio_open_params;
  sio_open_param_type open_param;
  uint32              br_id = bridge_id;

  ds_iot_log_high1 ("qti_bridge_open. bridge id:%d",br_id);

  if((bridge_id >= QTI_BRIDGES_MAX) || (NULL == bridge_info))
  {
    ds_iot_log_err2(" Bridge_id is invalid %d or bridge_info is invalid %u\n",
                        bridge_id, bridge_info);
    return;
  }
  memset(&sio_open_params, 0, sizeof(sio_open_params));
  memset(&open_param, 0, sizeof(open_param));

  sio_open_params.stream_mode   = SIO_GENERIC_MODE;
  sio_open_params.stream_type   = SIO_STREAM_RXTX;

  sio_open_params.rx_queue      = NULL; /* Recevice data in Rx callback */

  sio_open_params.tail_char_used  = FALSE;
  sio_open_params.tail_char       = 0;

  sio_open_params.rx_bitrate = SIO_BITRATE_BEST;
  sio_open_params.tx_bitrate = SIO_BITRATE_BEST;

  /*-------------------------------------------------------------------------
   Setup Flow-control
  -------------------------------------------------------------------------*/
  sio_open_params.rx_flow    = SIO_FCTL_BEST;
  sio_open_params.tx_flow    = SIO_FCTL_BEST;

  /*==========================================================================
  Open PHY port if not already open
============================================================================*/
  if(bridge_info->phys_stream_id  == SIO_NO_STREAM_ID)
  {
    /* Setup PHY SIO port */
    wm = &bridge_info->phys_tx_wm;
    dsm_queue_init (wm,
        qti_bridge_dne_wm_level,
        &bridge_info->phys_tx_queue);

/*===========================================================================
  Enable incoming data from SMD bridge when low watermark is hit
=============================================================================*/
    wm->lowater_func_ptr = qti_bridge_enable_smd_on_bridge[bridge_id];
    wm->lowater_func_data = (void *)&br_id;

/*============================================================================
Disable incoming data from SMD bridge when high watermark is hit
==============================================================================*/
    wm->hiwater_func_ptr = qti_bridge_disable_smd_on_bridge[bridge_id];
    wm->hiwater_func_data = (void *)&br_id;

    wm->dont_exceed_cnt = qti_bridge_dne_wm_level;

    if (bridge_info->phys_port_id == QTI_BRIDGE_UART_PORT)
    {
      sio_open_params.stream_mode = SIO_DS_PKT_MODE;
      sio_open_params.rx_bitrate = SIO_BITRATE_1000000;
      sio_open_params.tx_bitrate = SIO_BITRATE_1000000;
    }
    /* Open PHY SIO port */
    sio_open_params.port_id = bridge_info->phys_port_id;
    sio_open_params.rx_func_ptr   = qti_bridge_phys_to_qti_cbs[bridge_id];
    sio_open_params.tx_queue      = wm;

    /* Clear out any stale data in the open params union */
    memset(&open_param, 0, sizeof(open_param));

    if(bridge_info->control)
    {
      wm->lo_watermark = QTI_BRIDGE_CNTL_LO_WM_LEVEL;
      wm->hi_watermark = QTI_BRIDGE_CNTL_HI_WM_LEVEL;
      bridge_info->phys_stream_id = sio_control_open(&sio_open_params);
    }
    else
    {
      wm->lo_watermark = QTI_BRIDGE_DATA_LO_WM_LEVEL;
      wm->hi_watermark = QTI_BRIDGE_DATA_HI_WM_LEVEL;
      bridge_info->phys_stream_id = sio_open(&sio_open_params);
    }
  }

  if (bridge_info->phys_stream_id == SIO_NO_STREAM_ID)
  {
    ds_iot_log_err1(" Physical Stream ID is invalid for Bridge Id %d\n", 
                      bridge_id);
    //Cleaning up the TX Queue for Physical interface
    dsm_queue_destroy(&bridge_info->phys_tx_wm);
#ifndef FEATURE_DSM_Q_DESTROY
    q_destroy( &bridge_info->phys_tx_queue );
#endif

    return;
  }

  /* Open SMD port if not already open */
  if(bridge_info->smd_stream_id  == SIO_NO_STREAM_ID)
  {
    /* Set up the wm queues and associated cb*/
    wm = &bridge_info->smd_tx_wm;
    dsm_queue_init (wm,
        qti_bridge_dne_wm_level,
        &bridge_info->smd_tx_queue);

/*===================================================================
Enable incoming data from PHY device when low watermark is hit
====================================================================*/
    wm->lowater_func_ptr = qti_bridge_enable_phys_on_bridge[bridge_id];
    wm->lowater_func_data = (void *)&br_id;

/*===================================================================
  Disable incoming data from PHY device when high watermark is hit
====================================================================*/

    wm->hiwater_func_ptr = qti_bridge_disable_phys_on_bridge[bridge_id];
    wm->hiwater_func_data = (void *)&br_id;

    wm->dont_exceed_cnt = qti_bridge_dne_wm_level;

    /* Open SMD SIO port */
    sio_open_params.port_id          = bridge_info->smd_port_id;
    sio_open_params.rx_func_ptr      = qti_bridge_smd_to_qti_cbs[bridge_id];
    sio_open_params.tx_queue         = wm;
    sio_open_params.open_param       = &open_param;

    if(bridge_info->control)
    {
/*=========================================================================
  Set fifo size for control port
===========================================================================*/
      open_param.dsm_pool_param.smd_fifo_size = bridge_info->smd_fifo_size;

      wm->lo_watermark = QTI_BRIDGE_CNTL_LO_WM_LEVEL;
      wm->hi_watermark = QTI_BRIDGE_CNTL_HI_WM_LEVEL;
      bridge_info->smd_stream_id  = sio_control_open(&sio_open_params);
    }
    else
    {
/*=========================================================================
  Set fifo size for data port
===========================================================================*/
      wm->lo_watermark = QTI_BRIDGE_DATA_LO_WM_LEVEL;
      wm->hi_watermark = QTI_BRIDGE_DATA_HI_WM_LEVEL;
      bridge_info->smd_stream_id  = sio_open(&sio_open_params);
    }
  }

  if (bridge_info->smd_stream_id == SIO_NO_STREAM_ID)
  {
    ds_iot_log_err1(" SMD Stream ID is invalid for Bridge Id %d\n", 
                      bridge_id);

    //Cleaning up the TX Queue for SMD interface
    dsm_queue_destroy(&bridge_info->smd_tx_wm);
#ifndef FEATURE_DSM_Q_DESTROY
    q_destroy( &bridge_info->smd_tx_queue );
#endif

    return;
  }

  /** Setup signalling when both sides of bridge are open */
  if((bridge_info->phys_stream_id  != SIO_NO_STREAM_ID) &&
      (bridge_info->smd_stream_id  != SIO_NO_STREAM_ID) )
  {
    /* Set the callbacks to mirror DTR, CD and RI */
    if(!bridge_info->control)
    {
      sio_ioctl_param_type         ioctl_param;

      /* set the needed signal call backs via ioctl for the physical port */
      if(bridge_info->phys_port_id == QTI_BRIDGE_UART_PORT)
      {
        ioctl_param.rx_high_event_func_ptr =
          qti_bridge_phys_to_qti_dtr_cbs[bridge_id];
        sio_ioctl(bridge_info->phys_stream_id,
                  SIO_IOCTL_RX_HIGH_EVENT,
                  &ioctl_param);
        bridge_info->uart_stream_id = bridge_info->phys_stream_id;
      }
      else
      {
        ioctl_param.enable_dte_ready_event =
          qti_bridge_phys_to_qti_dtr_cbs[bridge_id];
        sio_ioctl(bridge_info->phys_stream_id,
            SIO_IOCTL_ENABLE_DTR_EVENT,
            &ioctl_param);
      }

      /* set the needed signal call backs via ioctl for the smd port */
      ioctl_param.enable_dte_ready_event_ext.cb_func =
        qti_bridge_dtr_smd_to_qti_cb;
      ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)bridge_info;
      sio_ioctl(bridge_info->smd_stream_id,
          SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
          &ioctl_param);

      /* CD and RI are system output so only SMD->PHYS callbacks need to be
         set up */
      ioctl_param.enable_cd_ready_event_ext.cb_func =
        qti_bridge_cd_smd_to_qti_cb;
      ioctl_param.enable_cd_ready_event_ext.cb_data = bridge_info;
      sio_ioctl(bridge_info->smd_stream_id,
          SIO_IOCTL_ENABLE_CD_EVENT_EXT,
          &ioctl_param);

      ioctl_param.enable_ri_ready_event_ext.cb_func =
        qti_bridge_ri_smd_to_qti_cb;
      ioctl_param.enable_ri_ready_event_ext.cb_data = bridge_info;
      sio_ioctl(bridge_info->smd_stream_id,
          SIO_IOCTL_ENABLE_RI_EVENT_EXT,
          &ioctl_param);
    }
    else
    {
      /* Register for DTR events for control channel */
      sio_ioctl_param_type         ioctl_param;
      /* set the needed signal call backs via ioctl for the physical port */
      ioctl_param.enable_dte_ready_event =
        qti_bridge_phys_to_qti_dtr_cbs[bridge_id];
      sio_control_ioctl(bridge_info->phys_stream_id,
          SIO_IOCTL_ENABLE_DTR_EVENT,
          &ioctl_param);

      /* set the needed signal call backs via ioctl for the smd port */
      ioctl_param.enable_dte_ready_event_ext.cb_func =
        qti_bridge_dtr_smd_to_qti_cb;
      ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)bridge_info;
      sio_control_ioctl(bridge_info->smd_stream_id,
          SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
          &ioctl_param);
    }
    /* turn on flow control for both SMD and PHY ports */
    qti_bridge_set_flow_ctl(bridge_id, QTI_BRIDGE_SMD_PORT, TRUE);
    qti_bridge_set_flow_ctl(bridge_id, QTI_BRIDGE_PHYS_PORT, TRUE);
  }
  return;
}


/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_OPEN()

DESCRIPTION
  This function opens the peripheral and SMD SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_open(sio_port_id_type port_id)
{
  qti_bridge_info_t*         bridge_info;

  ds_iot_log_high1("qti_bridge_sio_open, port_id %d", port_id);

  if (port_id == SIO_PORT_USB_MDM || port_id == QTI_BRIDGE_UART_PORT)
  {
    bridge_info = &qti_bridge_info_table[QTI_BRIDGE_INSTANCE_0];
    bridge_info->phys_port_id   = port_id;
    bridge_info->smd_port_id    = SIO_PORT_SMD_DS;
    bridge_info->control        = 0;
    bridge_info->smd_fifo_size  = 0; /* Use default: 8k */
    bridge_info->usb_sps        = 0;
    /* Setup SMD SIO port */
    qti_bridge_open(bridge_info, QTI_BRIDGE_INSTANCE_0);
  }
  else if (port_id == SIO_PORT_USB_MDM2)
  {
    bridge_info = &qti_bridge_info_table[QTI_BRIDGE_INSTANCE_1];
    bridge_info->phys_port_id   = port_id;
    bridge_info->smd_port_id    = SIO_PORT_SMD_DATA1;
    bridge_info->control        = 0;
    bridge_info->smd_fifo_size  = 0; /* Use default: 8k */
    bridge_info->usb_sps        = 0;
    /* Setup SMD SIO port */
    qti_bridge_open(bridge_info, QTI_BRIDGE_INSTANCE_1);
  }
  else if (port_id == SIO_PORT_DATA_MUX_1 || port_id == SIO_PORT_DATA_MUX_2 ||
           port_id == SIO_PORT_DATA_MUX_3)
  {
    if (port_id == SIO_PORT_DATA_MUX_1)
    {
      qti_bridge_ds_mux_logical_sio_open(0,SIO_PORT_DATA_MUX_1);
    }
    else if (port_id == SIO_PORT_DATA_MUX_2)
    {
      qti_bridge_ds_mux_logical_sio_open(1,SIO_PORT_DATA_MUX_2);
    }
    else if (port_id == SIO_PORT_DATA_MUX_3)
    {
      qti_bridge_ds_mux_logical_sio_open(2,SIO_PORT_DATA_MUX_3);
    }
  }
  else
  {
    rdm_notify(RDM_DUN_SRVC, RDM_NOT_ALLOWED_S);
    return;
  }

  if(port_id == SIO_PORT_USB_MDM || port_id == SIO_PORT_USB_MDM2
     ||port_id == QTI_BRIDGE_UART_PORT )
  {
    /*Even if SIO open for the peripheral ports fail, we always notify RDM 
      saying the service is allowed. This will ensure the RDM will update the
      EFS with correct service to device mapping for subsequent reboots*/
      rdm_notify(RDM_DUN_SRVC, RDM_DONE_S);
  }
  else if (port_id == SIO_PORT_DATA_MUX_1 || port_id == SIO_PORT_DATA_MUX_2 ||
           port_id == SIO_PORT_DATA_MUX_3)
  {
    rdm_notify(RDM_DATA_SRVC, RDM_DONE_S);
  }
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DISCONNECT_PORT

DESCRIPTION
   Disconnects a bridge and closes the physical sio port.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static boolean qti_bridge_disconnect_port
(
  uint32                      bridge_id
)
{
  sio_ioctl_param_type         param;

  ds_iot_log_high1 ("qti_bridge_disconnect_port: BridgeID:%d",bridge_id);

  /* check the bridge id is valid and open */
  if((bridge_id >= QTI_BRIDGES_MAX) ||
     (qti_bridge_info_table[bridge_id].phys_stream_id ==
                                       SIO_NO_STREAM_ID))
  {
    return FALSE;
  }

  /* set the physical port dsr state to not ready */
  param.data_set_ready_deassert = TRUE;

  sio_ioctl(qti_bridge_info_table[bridge_id].phys_stream_id,
            SIO_IOCTL_DSR_DEASSERT,
            &param );

  /* close the physical port */
  sio_close(qti_bridge_info_table[bridge_id].phys_stream_id,
            NULL);

  /* close the SMD port */
  sio_close(qti_bridge_info_table[bridge_id].smd_stream_id,
            qti_bridge_close_cbs[bridge_id]);

  /* clear stream IDs */
  qti_bridge_info_table[bridge_id].smd_stream_id = SIO_NO_STREAM_ID;
  qti_bridge_info_table[bridge_id].phys_stream_id = SIO_NO_STREAM_ID;

  return TRUE;
}/* smd_bridge_server_disconnect */


/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_CLOSE()

DESCRIPTION
  This function Closes the SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_close(sio_port_id_type port_id)
{
  ds_iot_log_high ("qti_bridge_sio_close ");

  /* Disconnect DUN data/control channels */
  /* Make sure the bridge was already setup */

  if((qti_bridge_info_table[QTI_BRIDGE_INSTANCE_0].phys_stream_id ==
                            SIO_NO_STREAM_ID) && (qti_bridge_info_table
                           [QTI_BRIDGE_INSTANCE_1].phys_stream_id ==
                            SIO_NO_STREAM_ID))
  {
    rdm_notify(RDM_DUN_SRVC, RDM_DONE_S);
    return;
  }
  if(port_id == SIO_PORT_USB_MDM)
  {
    qti_bridge_disconnect_port(QTI_BRIDGE_INSTANCE_0);
  }
  else if(port_id == SIO_PORT_USB_MDM2)
  {
    qti_bridge_disconnect_port(QTI_BRIDGE_INSTANCE_1);
  }
}

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_OPEN()

DESCRIPTION
  This function opens the peripheral and SMD SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_forward
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id,
  dsm_item_type *dsm_item_ptr
)
{
  qti_bridge_info_t                *br_info;
  sio_stream_id_type               stream_id = SIO_NO_STREAM_ID;
  dsm_item_type *temp_dsm = NULL;
  dsm_item_type *temp_dsm2 = NULL;

#ifdef QTI_LOGGING_ENABLED
    ULogFront_RealTimePrintf(qti_ulog, 2, "s_f %d %u ", evt_id, bridge_id);
  if(dsm_item_ptr)
  {
  ULogFront_RealTimePrintf(qti_ulog, 6, "%x %x %x %x %x %d", dsm_item_ptr-> data_ptr[1], 
                                                             dsm_item_ptr-> data_ptr[2],
                                                             dsm_item_ptr-> data_ptr[3],
                                                             dsm_item_ptr-> data_ptr[4],
                                                             dsm_item_ptr-> data_ptr[5],
                                                             dsm_item_ptr->used);
  }
  else
  {
	  ULogFront_RealTimePrintf(qti_ulog, 0, "dsm");
  }
  ds_iot_log_high2 ("qti_bridge_sio_forward; evt id %d bridge Id %u ",
                    evt_id, bridge_id);

#endif
  if (ds_mux_bridge_active == FALSE)
  {
#ifdef QTI_LOGGING_ENABLED
    ULogFront_RealTimePrintf(qti_ulog, 0, "baF");
#endif
  br_info = &qti_bridge_info_table[bridge_id];

  if (evt_id == QTI_BRIDGE_SMD_QTI_RECV_EVT)
  {
    stream_id = br_info->phys_stream_id;
  }
  else if (evt_id == QTI_BRIDGE_PHY_QTI_RECV_EVT)
  {
    stream_id = br_info->smd_stream_id;
  }

  if (stream_id == SIO_NO_STREAM_ID)
  {
#ifdef QTI_LOGGING_ENABLED
   	ULogFront_RealTimePrintf(qti_ulog, 0, "clr");
#endif
    dsm_free_packet(&dsm_item_ptr);
      return;
  }
  else
  {
    if(br_info->control)
    {
      //De-chain the DSM Item before transmitting
	  
      for (temp_dsm = dsm_item_ptr; temp_dsm;)
      {
        temp_dsm2 = temp_dsm;
        temp_dsm = temp_dsm->pkt_ptr;
        temp_dsm2->pkt_ptr = NULL;
#ifdef QTI_LOGGING_ENABLED
   		ULogFront_RealTimePrintf(qti_ulog, 0, "sct");
#endif
        sio_control_transmit(stream_id, temp_dsm2);
      }
    }
    else
    {
      //De-chain the DSM Item before transmitting
      for (temp_dsm = dsm_item_ptr; temp_dsm;)
      {
        temp_dsm2 = temp_dsm;
        temp_dsm = temp_dsm->pkt_ptr;
        temp_dsm2->pkt_ptr = NULL;
#ifdef QTI_LOGGING_ENABLED
   		ULogFront_RealTimePrintf(qti_ulog, 0, "sct");
#endif
        sio_transmit(stream_id, temp_dsm2);
      }
      	
    }
  }
  }
  else
  {
    /*DS MUX bridge is active. Put the DSM item in the PHYS RX pointer*/
    ds_mux_update_phys_rx_wmk(dsm_item_ptr);
  }
}

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_SET_DTR()

DESCRIPTION
  This function sets the DTR signal

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_set_dtr
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  qti_bridge_info_t                *br_info;
  sio_stream_id_type               from_stream_id = SIO_NO_STREAM_ID;
  sio_stream_id_type               to_stream_id = SIO_NO_STREAM_ID;
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;
  boolean                          dtr_status;

#ifdef FEATURE_9x07_CAT4
  sio_ioctl_cmd_type               cmd1;
  sio_ioctl_param_type             param1;
  boolean isPhy_to_QTI = false;
#endif
  ds_iot_log_high2 ("qti_bridge_sio_set_dtr; evt id %d bridge Id %u ",
                    evt_id, bridge_id);

  br_info = &qti_bridge_info_table[bridge_id];

  if (evt_id == QTI_BRIDGE_PHYS_QTI_DTR_EVT)
  {
#ifdef FEATURE_9x07_CAT4
	isPhy_to_QTI = true;
#endif
    from_stream_id     = br_info->phys_stream_id;
    to_stream_id       = br_info->smd_stream_id;
  }
  else if (evt_id == QTI_BRIDGE_SMD_QTI_DTR_EVT)
  {
    from_stream_id     = br_info->smd_stream_id;
    to_stream_id       = br_info->phys_stream_id;
  }

  if (br_info->phys_port_id == QTI_BRIDGE_UART_PORT)
  {
    param.rx_line_state = &dtr_status;
  }
  else
  {
    param.dte_ready_asserted = &dtr_status;
  }

  if (br_info->phys_port_id == QTI_BRIDGE_UART_PORT)
  {
    sio_ioctl(from_stream_id, SIO_IOCTL_CHECK_RX_STATE, &param);
  }
  else
  {
    if(br_info->control)
    {
      sio_control_ioctl(from_stream_id,
                        SIO_IOCTL_DTE_READY_ASSERTED,
                        &param );
    }
    else
    {
      sio_ioctl(from_stream_id,
                SIO_IOCTL_DTE_READY_ASSERTED,
                &param );
    }
  }

  if(dtr_status)
  {
    cmd = SIO_IOCTL_DSR_ASSERT;
    param.data_set_ready_assert = TRUE;
#ifdef FEATURE_9x07_CAT4
    if(isPhy_to_QTI)
    {
      cmd1 = SIO_IOCTL_SET_FLOW_CTL;
      param1.flow_ctl.tx_flow = SIO_FCTL_BEST;
      param1.flow_ctl.rx_flow = SIO_CTSRFR_FCTL;
    }
#endif
  }
  else
  {
    cmd = SIO_IOCTL_DSR_DEASSERT;
    param.data_set_ready_deassert = TRUE;
#ifdef FEATURE_9x07_CAT4
    if(isPhy_to_QTI)
    {
      cmd1 = SIO_IOCTL_SET_FLOW_CTL;
      param1.flow_ctl.tx_flow = SIO_FCTL_BEST;
      param1.flow_ctl.rx_flow = SIO_FCTL_OFF;
    }
#endif
  }

  if(br_info->control)
  {
    sio_control_ioctl(to_stream_id, cmd, &param );
  }
  else
  {
    sio_ioctl(to_stream_id, cmd, &param );
  }

#ifdef FEATURE_9x07_CAT4
if(isPhy_to_QTI)
{
  if(br_info->control)
    {
      sio_control_ioctl(to_stream_id, cmd1, &param1 );
    }
    else
    {      
      sio_ioctl(to_stream_id, cmd1, &param1 );
    }
}
#endif
}

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_SET_CD()

DESCRIPTION
  This function sets the CD signal on physical interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_set_cd (uint32 bridge_id)
{
  qti_bridge_info_t                *br_info;
  sio_stream_id_type               from_stream_id;
  sio_stream_id_type               to_stream_id;
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;
  boolean                          cd_status;

  ds_iot_log_high1 ("qti_bridge_sio_set_cd : bridge Id %u ",
					  bridge_id);

  br_info           = &qti_bridge_info_table[bridge_id];
  from_stream_id    = br_info->smd_stream_id;
  to_stream_id      = br_info->phys_stream_id;

  param.cd_asserted = &cd_status;

  sio_ioctl(from_stream_id,
            SIO_IOCTL_GET_CURRENT_CD,
            &param );

  if(cd_status)
  {
    cmd = SIO_IOCTL_CD_ASSERT;
    param.carrier_detect_assert = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_CD_DEASSERT;
    param.carrier_detect_deassert = TRUE;
  }

  sio_ioctl(to_stream_id, cmd, &param );
}

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_SET_RI()

DESCRIPTION
  This function sets the RI signal on physical interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_set_ri (uint32 bridge_id)
{
  qti_bridge_info_t                *br_info;
  sio_stream_id_type               from_stream_id;
  sio_stream_id_type               to_stream_id;
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;
  boolean                          ri_status;

  ds_iot_log_high1 ("qti_bridge_sio_set_ri : bridge Id %u ",
					  bridge_id);

  br_info           = &qti_bridge_info_table[bridge_id];
  from_stream_id    = br_info->smd_stream_id;
  to_stream_id      = br_info->phys_stream_id;

  param.ri_asserted = &ri_status;

  sio_ioctl(from_stream_id,
            SIO_IOCTL_GET_CURRENT_RI,
            &param );

  if(ri_status)
  {
    cmd = SIO_IOCTL_RI_ASSERT;
    param.ring_indicator_assert = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_RI_DEASSERT;
    param.ring_indicator_deassert = TRUE;
  }

  sio_ioctl(to_stream_id, cmd, &param );
}

/*==============================================================================
FUNCTION QTI_BRIDGE_SET_FLOW_CTL

DESCRIPTION
  Set the flow control on the given port

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_set_flow_ctl
(
  uint32                      bridge_id,
  qti_bridge_port_type        port_type,
  boolean                     enable
)
{
  sio_stream_id_type   stream_id = SIO_NO_STREAM_ID;
  sio_ioctl_param_type ioctl_param;
  sio_ioctl_cmd_type   cmd;

  ds_iot_log_high3 ("qti_bridge_set_flow_ctl : bridge Id %u, port type %d,enable %d ",
					  bridge_id, port_type, enable );

  if(port_type == QTI_BRIDGE_PHYS_PORT)
  {
    stream_id = qti_bridge_info_table[bridge_id].phys_stream_id;
  }
  else if (port_type == QTI_BRIDGE_SMD_PORT)
  {
    stream_id = qti_bridge_info_table[bridge_id].smd_stream_id;
  }

  /* the port isn't open, punt */
  if(stream_id == SIO_NO_STREAM_ID)
  {
    return;
  }

  if(enable)
  {
    cmd = SIO_IOCTL_INBOUND_FLOW_ENABLE;
    ioctl_param.inbound_flow_enable = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_INBOUND_FLOW_DISABLE;
    ioctl_param.inbound_flow_disable = TRUE;
  }

  /* Make SIO IOCTL call */
  if(qti_bridge_info_table[bridge_id].control)
  {
    sio_control_ioctl(stream_id, cmd, &ioctl_param );
  }
  else
  {
    sio_ioctl(stream_id, cmd, &ioctl_param );
  }
}

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SMD_SIO_OPEN

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
void qti_bridge_ds_mux_smd_sio_open
(
  uint8_t index,
  sio_port_id_type smd_port_id
)
{
  dsm_watermark_type *wm;
  sio_open_type sio_open_params;
  sio_open_param_type open_param;

  ds_iot_log_high2 ("qti_bridge_ds_mux_smd_sio_open: smd_port_id%d, index %d",
                     smd_port_id,index);

  memset(&sio_open_params, 0, sizeof(sio_open_params));
  memset(&open_param, 0, sizeof(open_param));

  sio_open_params.stream_mode   = SIO_GENERIC_MODE;
  sio_open_params.stream_type   = SIO_STREAM_RXTX;

  sio_open_params.rx_queue      = NULL; /* Receive data in Rx callback */

  sio_open_params.tail_char_used  = FALSE;
  sio_open_params.tail_char       = 0;

  sio_open_params.rx_bitrate = SIO_BITRATE_BEST;
  sio_open_params.tx_bitrate = SIO_BITRATE_BEST;

  /*-------------------------------------------------------------------------
   Setup Flow-control
  -------------------------------------------------------------------------*/
  sio_open_params.rx_flow    = SIO_FCTL_BEST;
  sio_open_params.tx_flow    = SIO_FCTL_BEST;

  /*------------------------------------------------------------------------
   Setup Flow-control
  -------------------------------------------------------------------------*/
  sio_open_params.rx_flow    = SIO_FCTL_BEST;
  sio_open_params.tx_flow    = SIO_FCTL_BEST;

  /* Set up the wm queues and associated cb*/
  wm = &qti_bridge_ds_mux_info->smd_tx_wm[index];
  dsm_queue_init (wm,
      qti_bridge_dne_wm_level,
      &qti_bridge_ds_mux_info->smd_tx_queue[index]);

/*===================================================================
Enable incoming data from PHY device when low watermark is hit
====================================================================*/
  wm->lowater_func_ptr = qti_bridge_ds_mux_enable_logical[index];
  wm->lowater_func_data = (void *)&index;

/*===================================================================
  Disable incoming data from PHY device when high watermark is hit
====================================================================*/

  wm->hiwater_func_ptr = qti_bridge_ds_mux_disable_logical[index];
  wm->hiwater_func_data = (void *)&index;

  wm->dont_exceed_cnt = qti_bridge_dne_wm_level;

  /* Open SMD SIO port */
  sio_open_params.port_id          = smd_port_id;
  sio_open_params.rx_func_ptr      = qti_bridge_ds_mux_smd_to_qti_cbs[index];
  sio_open_params.tx_queue         = wm;
  sio_open_params.open_param       = &open_param;

  /*=========================================================================
  Set fifo size for data port
===========================================================================*/
  wm->lo_watermark = QTI_BRIDGE_DATA_LO_WM_LEVEL;
  wm->hi_watermark = QTI_BRIDGE_DATA_HI_WM_LEVEL;
  qti_bridge_ds_mux_info->smd_stream_id[index]  = sio_open(&sio_open_params);

  //Enable DTR for the SMD port
  qti_bridge_change_dtr(TRUE,qti_bridge_ds_mux_info->smd_stream_id[index]);
}

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_SIO_OPEN

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
void qti_bridge_ds_mux_logical_sio_open
(
  uint8_t index,
  sio_port_id_type logical_sio_port_id
)
{
  dsm_watermark_type *txwm, *rxwm;
  sio_open_type sio_open_params;
  sio_open_param_type open_param;
  sio_ioctl_param_type         ioctl_param;

  ds_iot_log_high2 ("qti_bridge_ds_mux_logical_sio_open: logical_sio_port_id%d, index %d", logical_sio_port_id, index);

  memset(&sio_open_params, 0, sizeof(sio_open_params));
  memset(&open_param, 0, sizeof(open_param));

  sio_open_params.stream_mode   = SIO_GENERIC_MODE;
  sio_open_params.stream_type   = SIO_STREAM_RXTX;

  sio_open_params.rx_queue      = NULL; /* Receive data in Rx callback */

  sio_open_params.tail_char_used  = FALSE;
  sio_open_params.tail_char       = 0;

  sio_open_params.rx_bitrate = SIO_BITRATE_BEST;
  sio_open_params.tx_bitrate = SIO_BITRATE_BEST;

  /*-------------------------------------------------------------------------
   Setup Flow-control
  -------------------------------------------------------------------------*/
  sio_open_params.rx_flow    = SIO_FCTL_BEST;
  sio_open_params.tx_flow    = SIO_FCTL_BEST;

  /*------------------------------------------------------------------------
   Setup the TX WM and associated functions
  -------------------------------------------------------------------------*/
  txwm = &qti_bridge_ds_mux_info->logical_tx_wm[index];
  dsm_queue_init (txwm,
      qti_bridge_dne_wm_level,
      &qti_bridge_ds_mux_info->logical_tx_queue[index]);

  dsm_set_low_wm( &qti_bridge_ds_mux_info->logical_tx_wm[index], QTI_BRIDGE_DATA_LO_WM_LEVEL );
  dsm_set_hi_wm( &qti_bridge_ds_mux_info->logical_tx_wm[index], QTI_BRIDGE_DATA_HI_WM_LEVEL );
  dsm_set_dne( &qti_bridge_ds_mux_info->logical_tx_wm[index], qti_bridge_dne_wm_level );

  qti_bridge_ds_mux_info->logical_tx_wm[index].lowater_func_ptr = qti_bridge_ds_mux_logical_low_tx_wm[index];
  qti_bridge_ds_mux_info->logical_tx_wm[index].hiwater_func_ptr = qti_bridge_ds_mux_logical_high_tx_wm[index];

  /*------------------------------------------------------------------------
   Setup the RX WM and associated functions
  -------------------------------------------------------------------------*/
  rxwm = &qti_bridge_ds_mux_info->logical_rx_wm[index];
  dsm_queue_init (rxwm,
      qti_bridge_dne_wm_level,
      &qti_bridge_ds_mux_info->logical_rx_queue[index]);

  dsm_set_low_wm( &qti_bridge_ds_mux_info->logical_rx_wm[index], QTI_BRIDGE_DATA_LO_WM_LEVEL );
  dsm_set_hi_wm( &qti_bridge_ds_mux_info->logical_rx_wm[index], QTI_BRIDGE_DATA_HI_WM_LEVEL );
  dsm_set_dne( &qti_bridge_ds_mux_info->logical_rx_wm[index], qti_bridge_dne_wm_level );

  qti_bridge_ds_mux_info->logical_rx_wm[index].lowater_func_ptr = qti_bridge_ds_mux_logical_low_rx_wm[index];
  qti_bridge_ds_mux_info->logical_rx_wm[index].hiwater_func_ptr = qti_bridge_ds_mux_logical_high_rx_wm[index];
  qti_bridge_ds_mux_info->logical_rx_wm[index].non_empty_func_ptr = qti_bridge_ds_mux_logical_rx_wm_non_empty_func[index];

  sio_open_params.port_id          = logical_sio_port_id;
  sio_open_params.tx_queue         = txwm;
  sio_open_params.rx_queue         = rxwm;
  sio_open_params.open_param       = &open_param;

  qti_bridge_ds_mux_info->logical_stream_id[index]  = sio_open(&sio_open_params);

  /** Setup signalling when both sides of bridge are open */
  if((qti_bridge_ds_mux_info->smd_stream_id[index]  != SIO_NO_STREAM_ID) &&
      (qti_bridge_ds_mux_info->logical_stream_id[index]  != SIO_NO_STREAM_ID) )
  {
    //Set the DTR for SMD->Logical Channel
    ioctl_param.enable_dte_ready_event_ext.cb_func =
          qti_bridge_ds_mux_dtr_smd_to_qti_cb[index];
    ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)&index;
    sio_ioctl(qti_bridge_ds_mux_info->smd_stream_id[index],
          SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
          &ioctl_param);

    //Set the CD for SMD->Logical Channel
    ioctl_param.enable_cd_ready_event_ext.cb_func =
        qti_bridge_ds_mux_cd_smd_to_qti_cb[index];
    ioctl_param.enable_cd_ready_event_ext.cb_data = (void *)&index;
    sio_ioctl(qti_bridge_ds_mux_info->smd_stream_id[index],
          SIO_IOCTL_ENABLE_CD_EVENT_EXT,
          &ioctl_param);

    //Set the RI for SMD->Logical Channel
    ioctl_param.enable_ri_ready_event_ext.cb_func =
        qti_bridge_ds_mux_ri_smd_to_qti_cb[index];
    ioctl_param.enable_ri_ready_event_ext.cb_data = &index;
    sio_ioctl(qti_bridge_ds_mux_info->smd_stream_id[index],
          SIO_IOCTL_ENABLE_RI_EVENT_EXT,
          &ioctl_param);

    //Set the DTR for Logical->SMD Channel
    ioctl_param.enable_dte_ready_event_ext.cb_func =
          qti_bridge_ds_mux_dtr_logical_to_qti_cb[index];
    ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)&index;
    sio_ioctl(qti_bridge_ds_mux_info->logical_stream_id[index],
          SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
          &ioctl_param);

    //Set the CD for Logical->SMD Channel
    ioctl_param.enable_cd_ready_event_ext.cb_func =
        qti_bridge_ds_mux_cd_logical_to_qti_cb[index];
    ioctl_param.enable_cd_ready_event_ext.cb_data = (void *)&index;
    sio_ioctl(qti_bridge_ds_mux_info->logical_stream_id[index],
          SIO_IOCTL_ENABLE_CD_EVENT_EXT,
          &ioctl_param);

    //Set the RI for Logical->SMD Channel
    ioctl_param.enable_ri_ready_event_ext.cb_func =
        qti_bridge_ds_mux_ri_logical_to_qti_cb[index];
    ioctl_param.enable_ri_ready_event_ext.cb_data = (void *)&index;
    sio_ioctl(qti_bridge_ds_mux_info->logical_stream_id[index],
          SIO_IOCTL_ENABLE_RI_EVENT_EXT,
          &ioctl_param);
  }
  qti_bridge_ds_mux_set_flow_ctl(index,QTI_BRIDGE_SMD_PORT,TRUE);
  qti_bridge_ds_mux_set_flow_ctl(index,QTI_BRIDGE_DS_MUX_LOGICAL_PORT,TRUE);
}

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_FORWARD()

DESCRIPTION
  This function opens the peripheral and SMD SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_forward
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id,
  dsm_item_type *dsm_item_ptr
)
{
  sio_stream_id_type               stream_id;

  ds_iot_log_high ("qti_bridge_ds_mux_sio_forward ");
  stream_id = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];

  if (stream_id == SIO_NO_STREAM_ID)
  {
    ds_iot_log_high ("qti_bridge_ds_mux_sio_forward: stream_id == SIO_NO_STREAM_ID ");
    dsm_free_packet(&dsm_item_ptr);
  }
  else
  {
    sio_transmit(stream_id, dsm_item_ptr);
  }

}


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SET_FLOW_CTL

DESCRIPTION
  Set the flow control on the given port

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_set_flow_ctl
(
  uint32                      bridge_id,
  qti_bridge_port_type        port_type,
  boolean                     enable
)
{
  sio_stream_id_type   stream_id = SIO_NO_STREAM_ID;
  sio_ioctl_param_type ioctl_param;
  sio_ioctl_cmd_type   cmd;

  ds_iot_log_high3 ("qti_bridge_ds_mux_set_flow_ctl : bridge Id %u, port type %d,enable %d ",
					  bridge_id, port_type, enable );

  //TODO:: Fetch streamID of the logical MUX / SMD channel based on bridgeID

  if (port_type == QTI_BRIDGE_SMD_PORT)
  {
    stream_id = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
  }
  else if (port_type == QTI_BRIDGE_DS_MUX_LOGICAL_PORT)
  {
    stream_id = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
  }

  /* the port isn't open, punt */
  if(stream_id == SIO_NO_STREAM_ID)
  {
    return;
  }

  if(enable)
  {
    cmd = SIO_IOCTL_INBOUND_FLOW_ENABLE;
    ioctl_param.inbound_flow_enable = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_INBOUND_FLOW_DISABLE;
    ioctl_param.inbound_flow_disable = TRUE;
  }

  sio_ioctl(stream_id, cmd, &ioctl_param );
}

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_SET_DTR()

DESCRIPTION
  This function sets the DTR signal on the appropriate DS MUX LogicalStream ID

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_set_dtr
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  sio_stream_id_type               from_stream_id = SIO_NO_STREAM_ID;
  sio_stream_id_type               to_stream_id = SIO_NO_STREAM_ID;
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;
  boolean                          dtr_status;

  ds_iot_log_high2 ("qti_bridge_ds_mux_sio_set_dtr; evt id %d bridge Id %u ",
                    evt_id, bridge_id);

  if (evt_id == QTI_BRIDGE_DS_MUX_SMD_QTI_DTR_EVT)
  {
    from_stream_id     = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
    to_stream_id       = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
  }
  else if (evt_id == QTI_BRIDGE_DS_MUX_LOGICAL_QTI_DTR_EVT)
  {
    from_stream_id     = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
    to_stream_id       = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
  }

  param.dte_ready_asserted = &dtr_status;

  sio_ioctl(from_stream_id,
            SIO_IOCTL_DTE_READY_ASSERTED,
            &param );

  if(dtr_status)
  {
    cmd = SIO_IOCTL_DSR_ASSERT;
    param.data_set_ready_assert = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_DSR_DEASSERT;
    param.data_set_ready_deassert = TRUE;
  }

  sio_ioctl(to_stream_id, cmd, &param );
}



/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_SET_CD()

DESCRIPTION
  This function sets the CD signal on DS MUX logical channel interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_set_cd
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  sio_stream_id_type               from_stream_id = 0;
  sio_stream_id_type               to_stream_id = 0;
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;
  boolean                          cd_status;

  ds_iot_log_high1 ("qti_bridge_sio_set_cd : bridge Id %u ",
					  bridge_id);

  if (evt_id == QTI_BRIDGE_DS_MUX_SMD_QTI_CD_EVT)
  {
    from_stream_id    = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
    to_stream_id      = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
  }
  else if (evt_id == QTI_BRIDGE_DS_MUX_LOGICAL_QTI_CD_EVT)
  {
    from_stream_id     = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
    to_stream_id       = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
  }

  param.cd_asserted = &cd_status;

  sio_ioctl(from_stream_id,
            SIO_IOCTL_GET_CURRENT_CD,
            &param );

  if(cd_status)
  {
    cmd = SIO_IOCTL_CD_ASSERT;
    param.carrier_detect_assert = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_CD_DEASSERT;
    param.carrier_detect_deassert = TRUE;
  }

  sio_ioctl(to_stream_id, cmd, &param );
}


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_SET_RI()

DESCRIPTION
  This function sets the RI signal on DS MUX Logical interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_set_ri
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  sio_stream_id_type               from_stream_id = SIO_NO_STREAM_ID;
  sio_stream_id_type               to_stream_id = SIO_NO_STREAM_ID;
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;
  boolean                          ri_status;

  ds_iot_log_high1 ("qti_bridge_sio_set_ri : bridge Id %u ",
					  bridge_id);
  if (evt_id == QTI_BRIDGE_DS_MUX_SMD_QTI_RI_EVT)
  {
    from_stream_id    = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
    to_stream_id      = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
  }
  else if (evt_id == QTI_BRIDGE_DS_MUX_LOGICAL_QTI_RI_EVT)
  {
    from_stream_id     = qti_bridge_ds_mux_info->logical_stream_id[bridge_id];
    to_stream_id       = qti_bridge_ds_mux_info->smd_stream_id[bridge_id];
  }

  param.ri_asserted = &ri_status;

  sio_ioctl(from_stream_id,
            SIO_IOCTL_GET_CURRENT_RI,
            &param );

  if(ri_status)
  {
    cmd = SIO_IOCTL_RI_ASSERT;
    param.ring_indicator_assert = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_RI_DEASSERT;
    param.ring_indicator_deassert = TRUE;
  }

  sio_ioctl(to_stream_id, cmd, &param );
}


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_CLOSE()

DESCRIPTION
  This function handles the SIO close for DS MUX channels Logical/SMD

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_close
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
)
{
  ds_iot_log_high1 ("qti_bridge_ds_mux_sio_close : bridge Id %u ",
                    bridge_id);

  if (evt_id == QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE)
  {
  dsm_queue_destroy (&qti_bridge_ds_mux_info->logical_tx_wm[bridge_id]);
  #ifndef FEATURE_DSM_Q_DESTROY
  q_destroy( &qti_bridge_ds_mux_info->logical_tx_queue[bridge_id] );
  #endif

  dsm_queue_destroy (&qti_bridge_ds_mux_info->logical_rx_wm[bridge_id]);
  #ifndef FEATURE_DSM_Q_DESTROY
  q_destroy( &qti_bridge_ds_mux_info->logical_rx_queue[bridge_id] );
  #endif
  }
}

/*==============================================================================
  FUNCTION QTI_BRIDGE_SET_DNE_WM_LEVEL

  DESCRIPTION
  Function invoked to set the don't exceed water mark level

  DEPENDENCIES
  None.

  RETURN VALUE
  None

  SIDE EFFECTS
  None

  ==============================================================================*/
void qti_bridge_set_dne_wm_level()
{
    int filehandle;
    int val;
    char buf[256] = {0};

    filehandle = efs_open( "/datatx/qti_bridge_dne_wm_config", O_RDONLY );
    if(filehandle < 0 )
    {
        qti_bridge_dne_wm_level = QTI_BRIDGE_DNE_WM_LEVEL;
        ds_iot_log_high1 ("qti_bridge_set_dne_wm_level : default qti_bridge_dne_wm_level %u ",
                qti_bridge_dne_wm_level);
    }
    else
    {
        efs_read(filehandle, buf, sizeof(buf));
        val = atoi(buf);
        if( val > 0 )
        {
            qti_bridge_dne_wm_level = val;
            ds_iot_log_high1 ("qti_bridge_set_dne_wm_level : qti_bridge_dne_wm_level %u ",
                    qti_bridge_dne_wm_level);
        }
        else
        {
            qti_bridge_dne_wm_level = QTI_BRIDGE_DNE_WM_LEVEL;
            ds_iot_log_high1 ("qti_bridge_set_dne_wm_level : default qti_bridge_dne_wm_level %u ",
                    qti_bridge_dne_wm_level);
        }
        efs_close(filehandle);
    }
}

/*==============================================================================

FUNCTION QTI_BRIDGE_POST_PROCESS_CMUX_CMD

DESCRIPTION
  Function invoked to do post processing of CMUX cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void qti_bridge_post_process_cmux_cmd (void)
{
  if((at_cmux_handle_in_progress) && (ds_mux_bridge_active == FALSE))
    {

#ifdef QTI_LOGGING_ENABLED
      ULogFront_RealTimePrintf(qti_ulog, 0, "ip");
#endif
      ds_iot_log_high ("qti_bridge_post_process_cmux_cmd; start CMUX active init ");
      event_info.event_cb_fn = qti_bridge_ds_mux_event_cb;
      event_info.client_data = NULL;
      ds_mux_active_init(qti_bridge_info_table[0].phys_stream_id,&event_info);
      at_cmux_handle_in_progress = FALSE;
    }
}


