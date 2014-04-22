/*==============================================================================

                      Remote Storage Server SIO Interface

DESCRIPTION
   This file contains RMTS interface to the SIO transport layer

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_sio.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     -------------------------------------------------------
2014-12-05       dks     Create.
==============================================================================*/

#include "remotefs_sio.h"
#include "remotefs_msg.h"
#include "remotefs_comm_api.h"
#include "sio.h"
#include "rdevmap.h"
#include "diagdsm_v.h"
#include "rex.h"
#include "assert.h"
#include "stringl/stringl.h"
#include "time_genoff.h"

#define REMOTEFS_SRV_SIO_TIMEOUT_SIG     0x00020000
#define REMOTEFS_SRV_SIO_SIG             0x00040000
#define REMOTEFS_SRV_DTR_SIG             0x00080000
#define REMOTEFS_SRV_SIO_FLUSH_SIG       0x00100000

#define REMOTEFS_DBG_PACKET_BUFF_MAX     20
#define REMOTEFS_DBG_PACKET_MAX          20

struct remotefs_packet_info
{
  uint64 time_stamp;
  uint32 length;
  uint8 operation;
  uint8  data[REMOTEFS_DBG_PACKET_BUFF_MAX];
};

struct remotefs_dbg_info
{
  uint32 dbg_packet_head;
  struct remotefs_packet_info dbg_packet[REMOTEFS_DBG_PACKET_MAX];
};

struct remotefs_sio_rx_info
{
  uint8* sio_rx_buf;
  uint32 sio_rx_buf_size;
  uint32 rx_bytes;
  boolean loop_for_data;
  uint32 loop_rx_data;
  uint32 data_to_loop_for;
};

struct remotefs_sio_if_type
{
  sio_open_type open_properties;
  sio_open_param_type open_param;
  sio_stream_id_type remotefs_sio_handle;
  dsm_watermark_type remotefs_tx_wm_queue;
  q_type remotefs_tx_q;

  rex_tcb_type* sio_tcb;
  rex_timer_type sio_timer;

  struct remotefs_sio_rx_info sio_rx_info;

  struct remotefs_dbg_info debug_info;
};
static struct remotefs_sio_if_type remotefs_sio_info;

static void remotefs_sio_rx_notify (dsm_item_type ** item_ptr);
static void remotefs_dtr_changed_cb (void);
static void remotefs_dbg_add_packet(uint32 len, uint8* buffer, uint8 op);

/* Opens a port on the SIO channel */
static void
remotefs_sio_open (sio_port_id_type port_id)
{
  sio_ioctl_param_type param;

  remotefs_sio_info.open_param.dsm_pool_param.dsm_mempool_id =
                                                  DSM_DS_LARGE_ITEM_POOL;
  remotefs_sio_info.open_properties.stream_mode = SIO_DS_AUTODETECT_MODE;
  remotefs_sio_info.open_properties.port_id = port_id;
  remotefs_sio_info.open_properties.rx_func_ptr = remotefs_sio_rx_notify;
  remotefs_sio_info.open_properties.open_param =
                                         &remotefs_sio_info.open_param;
  remotefs_sio_info.open_properties.tx_queue =
                                 &remotefs_sio_info.remotefs_tx_wm_queue;

  remotefs_sio_info.remotefs_sio_handle =
                         sio_open (&(remotefs_sio_info.open_properties));

  if (remotefs_sio_info.remotefs_sio_handle != SIO_NO_STREAM_ID)
  {
    // Set flow control to allow flow
    sio_ioctl (remotefs_sio_info.remotefs_sio_handle,
               SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);
    // Set CD line
    sio_ioctl (remotefs_sio_info.remotefs_sio_handle,
               SIO_IOCTL_CD_ASSERT, &param);
    // Set DSR line
    sio_ioctl (remotefs_sio_info.remotefs_sio_handle,
               SIO_IOCTL_DSR_ASSERT, &param);
    // Clear RI line
    sio_ioctl (remotefs_sio_info.remotefs_sio_handle,
               SIO_IOCTL_RI_DEASSERT, &param);
    // register DTR callback
    param.enable_dte_ready_event = remotefs_dtr_changed_cb;
    sio_ioctl (remotefs_sio_info.remotefs_sio_handle,
               SIO_IOCTL_ENABLE_DTR_EVENT, &param);

    // Hack for PCie as it doesn't support DTR. PCIe is ready for transfer
    // if sio_open goes through. So no need for DTR.
    if (port_id == SIO_PORT_MHI_EFS)
    {
      rex_set_sigs (remotefs_sio_info.sio_tcb, REMOTEFS_SRV_DTR_SIG);
    }
  }
}

/* Called when RDM wants diag to open a port on the SIO channel */
static void
remotefs_rdm_open_cb (sio_port_id_type port_id)
{
  /*
  ** RDM OPEN logic:
  ** 1. If the port_id given by RDM is valid, try to open that port
  ** 2. If invalid port ID is given or sio_open fails, try MHI, then USB.
  */

  switch (port_id)
  {
    case SIO_PORT_MHI_EFS:
    {
      remotefs_sio_open (port_id);
      if (remotefs_sio_info.remotefs_sio_handle == SIO_NO_STREAM_ID)
      {
        remotefs_sio_open (SIO_PORT_USB_SER4);
      }
    }
    break;

    case SIO_PORT_USB_SER4:
    {
      remotefs_sio_open (port_id);
      if (remotefs_sio_info.remotefs_sio_handle == SIO_NO_STREAM_ID)
      {
        remotefs_sio_open (SIO_PORT_MHI_EFS);
      }
    }
    break;

    /* Unsupported PORT ID */
    default:
    {
      RMTS_MSG_HIGH("unknown port %u", port_id, 0, 0);

      remotefs_sio_open (SIO_PORT_MHI_EFS);
      if (remotefs_sio_info.remotefs_sio_handle == SIO_NO_STREAM_ID)
      {
        remotefs_sio_open (SIO_PORT_USB_SER4);
      }
    }
    break;
  }

  if (remotefs_sio_info.remotefs_sio_handle == SIO_NO_STREAM_ID)
  {
    RMTS_MSG_ERR ("remotefs_sio_open failed",0,0,0);
    /* Optionally can be changed to ERR_FATAL if this is critical service */
    rdm_notify (RDM_RMTS_SRVC, RDM_NOT_ALLOWED_S);
  }
  else
  {
    /* RDM issue open succeeded. Notify to RDM, so RDM can commit the current
    ** mapping to config file in EFS
    */
    rdm_notify (RDM_RMTS_SRVC, RDM_DONE_S);
  }
}

/* Called when RDM wants RMTS to close its port. */
static void
remotefs_rdm_close_cb (void)
{
  if (remotefs_sio_info.remotefs_sio_handle != SIO_NO_STREAM_ID)
  {
    /* Call SIO close with NULL close_func_ptr to discard the TX data
    ** immediately
    */
    sio_close (remotefs_sio_info.remotefs_sio_handle, NULL );
    remotefs_sio_info.remotefs_sio_handle = SIO_NO_STREAM_ID;

    rdm_notify (RDM_RMTS_SRVC, RDM_DONE_S);
  }
}

/* receive data notification sio */
static void
remotefs_sio_rx_notify (dsm_item_type ** item_ptr)
{
  size_t retval;

  if (item_ptr == NULL)
  {
    ERR_FATAL ("Error in remotefs_sio_rx_notify, item_ptr is NULL", 0,0,0);
    return;
  }
  if (*item_ptr == NULL)
  {
    ERR_FATAL ("Error in remotefs_sio_rx_notify, item_ptr contains NULL", 0,0,0);
    return;
  }

  remotefs_sio_info.sio_rx_info.rx_bytes = (*item_ptr)->used;
  RMTS_MSG_MED ("rmts_srv:Received %d bytes",
                 remotefs_sio_info.sio_rx_info.rx_bytes,0,0);

  if ((remotefs_sio_info.sio_rx_info.rx_bytes > 0) &&
      (remotefs_sio_info.sio_rx_info.rx_bytes <=
       remotefs_sio_info.sio_rx_info.sio_rx_buf_size))
  {
    retval = memscpy (remotefs_sio_info.sio_rx_info.sio_rx_buf,
                      remotefs_sio_info.sio_rx_info.sio_rx_buf_size,
                      (*item_ptr)->data_ptr,
                      remotefs_sio_info.sio_rx_info.rx_bytes);
    if (retval < remotefs_sio_info.sio_rx_info.rx_bytes)
    {
      RMTS_MSG_ERR("Warning: data truncated while copying", 0, 0, 0);
    }
    if (!remotefs_sio_info.sio_rx_info.loop_for_data)
    {
      rex_set_sigs (remotefs_sio_info.sio_tcb, REMOTEFS_SRV_SIO_SIG);
    }
    else
    {
      remotefs_sio_info.sio_rx_info.loop_rx_data +=
                  remotefs_sio_info.sio_rx_info.rx_bytes;
      if (remotefs_sio_info.sio_rx_info.loop_rx_data >=
          remotefs_sio_info.sio_rx_info.data_to_loop_for)
      {
        remotefs_sio_info.sio_rx_info.rx_bytes =
              remotefs_sio_info.sio_rx_info.loop_rx_data;
        rex_set_sigs (remotefs_sio_info.sio_tcb, REMOTEFS_SRV_SIO_SIG);
      }
      else
      {
        remotefs_sio_info.sio_rx_info.sio_rx_buf +=
                   remotefs_sio_info.sio_rx_info.rx_bytes;
        remotefs_sio_info.sio_rx_info.sio_rx_buf_size -=
                   remotefs_sio_info.sio_rx_info.rx_bytes;
      }
    }
  }
  else
  {
    //should we move to error state or just ignore the message?
    ERR_FATAL ("Error in remotefs_sio_rx_notify: invalid data size", 0,0,0);
  }

  dsm_free_packet(item_ptr);
}

/* Setup buffers to receive data */
static void
remotefs_sio_rx_setup  (uint8 *rx_buf, uint32 len, boolean loop_for_data)
{
  // Store the original supplied buffer
  remotefs_sio_info.sio_rx_info.sio_rx_buf = rx_buf;
  remotefs_sio_info.sio_rx_info.sio_rx_buf_size = len;
  remotefs_sio_info.sio_rx_info.loop_for_data = loop_for_data;

  if (TRUE == remotefs_sio_info.sio_rx_info.loop_for_data)
  {
    remotefs_sio_info.sio_rx_info.loop_rx_data = 0;
    remotefs_sio_info.sio_rx_info.data_to_loop_for = len;
  }
}

/* Receives bulk data from driver */
remotefs_status_type
remotefs_sio_rx  (uint32 timeout, uint32 *rx_bytes)
{
  rex_sigs_type sigs;
  remotefs_status_type status;
  *rx_bytes = 0;

  if (timeout > 0)
  {
    rex_def_timer (&remotefs_sio_info.sio_timer, remotefs_sio_info.sio_tcb,
                   REMOTEFS_SRV_SIO_TIMEOUT_SIG);
    (void) rex_set_timer (&remotefs_sio_info.sio_timer, timeout);
    sigs = rex_wait (REMOTEFS_SRV_SIO_SIG | REMOTEFS_SRV_SIO_TIMEOUT_SIG);
    rex_clr_timer (&remotefs_sio_info.sio_timer);
  }
  else
  {
    sigs = rex_wait (REMOTEFS_SRV_SIO_SIG);
  }
  rex_clr_sigs (remotefs_sio_info.sio_tcb, sigs);

  if (sigs & REMOTEFS_SRV_SIO_SIG)
  {
    *rx_bytes = remotefs_sio_info.sio_rx_info.rx_bytes;
  }
  else if (sigs & REMOTEFS_SRV_SIO_TIMEOUT_SIG)
  {
    RMTS_MSG_ERR ("Timeout occured waiting for data", 0, 0, 0);
    status = RMFS_RX_TIMEOUT;
    goto End;
  }

  if (*rx_bytes)
  {
    remotefs_dbg_add_packet(*rx_bytes,
                            remotefs_sio_info.sio_rx_info.sio_rx_buf, 1);
  }

  status = RMFS_NO_ERROR;

End:
  return status;
}

static void remotefs_flush_tx_cb (void)
{
  rex_set_sigs (remotefs_sio_info.sio_tcb, REMOTEFS_SRV_SIO_FLUSH_SIG);
}

/* Transmits bulk data to driver */
remotefs_status_type
remotefs_sio_tx (uint8 *tx_buf, uint32 len, uint32 flush_flag,
                 uint32 flush_timeout, uint8 *rx_buf, uint32 rx_len,
                 boolean loop_for_data)
{
  rex_sigs_type sigs;
  dsm_item_type *tx_dsm;
  remotefs_status_type status;

  /* Setup rx buffer before doing a tx since the tx and rx happen on different
     threads and there can be a race-condition */
  remotefs_sio_rx_setup  (rx_buf, rx_len, loop_for_data);

  tx_dsm = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
  if (tx_dsm == NULL)
  {
    status = RMFS_ERROR_NO_MEM;
    goto End;
  }

  tx_dsm->app_field = len;
  tx_dsm->app_ptr = tx_buf;

  sio_transmit (remotefs_sio_info.remotefs_sio_handle, tx_dsm);

  remotefs_dbg_add_packet (len, tx_buf, 0);

  if (flush_flag == 1)
  {
    rex_clr_sigs (remotefs_sio_info.sio_tcb, REMOTEFS_SRV_SIO_FLUSH_SIG);
    sio_flush_tx (remotefs_sio_info.remotefs_sio_handle, remotefs_flush_tx_cb);

    if (flush_timeout > 0)
    {
      rex_def_timer(&remotefs_sio_info.sio_timer, remotefs_sio_info.sio_tcb,
                    REMOTEFS_SRV_SIO_TIMEOUT_SIG);
      (void) rex_set_timer (&remotefs_sio_info.sio_timer, flush_timeout);
      sigs = rex_wait (REMOTEFS_SRV_SIO_FLUSH_SIG |
                       REMOTEFS_SRV_SIO_TIMEOUT_SIG);
      rex_clr_timer (&remotefs_sio_info.sio_timer);
   }
   else
   {
    sigs = rex_wait (REMOTEFS_SRV_SIO_FLUSH_SIG);
   }

  rex_clr_sigs (remotefs_sio_info.sio_tcb, sigs);

  if (sigs & REMOTEFS_SRV_SIO_TIMEOUT_SIG)
  {
    status = RMFS_FLUSH_TIMEOUT;
    goto End;
   }
  }

  status = RMFS_NO_ERROR;

End:
  return status;
}

static void remotefs_dtr_changed_cb (void)
{
  sio_ioctl_param_type ioctl_param;
  boolean dte_ready_asserted;
  ioctl_param.dte_ready_asserted = &dte_ready_asserted;

  sio_ioctl (remotefs_sio_info.remotefs_sio_handle,
             SIO_IOCTL_DTE_READY_ASSERTED,
             (void*)&ioctl_param);

  if (dte_ready_asserted)
  {
    rex_set_sigs (remotefs_sio_info.sio_tcb, REMOTEFS_SRV_DTR_SIG);
  }
}

void
remotefs_sio_init (rex_tcb_type *tcb)
{
  rex_sigs_type sigs;

  memset (&remotefs_sio_info, 0, sizeof(remotefs_sio_info));

  remotefs_sio_info.remotefs_sio_handle = SIO_NO_STREAM_ID;

  /* Initialize watermark queue for SIO */
  dsm_queue_init (&remotefs_sio_info.remotefs_tx_wm_queue,
                  DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ,
                  &remotefs_sio_info.remotefs_tx_q);
  dsm_set_low_wm (&remotefs_sio_info.remotefs_tx_wm_queue,
                  DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ);
  dsm_set_hi_wm (&remotefs_sio_info.remotefs_tx_wm_queue,
                 DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ);

  /* Register the open/close callbacks with RDM */
  rdm_register_open_func (RDM_RMTS_SRVC, remotefs_rdm_open_cb);
  rdm_register_close_func (RDM_RMTS_SRVC, remotefs_rdm_close_cb);

  /* After registering func, let RDM know list of compatible device.
     This will prevent rdm_assign_port() for other device than below */
  rdm_set_compatibility (RDM_RMTS_SRVC, RDM_USB_SER4_DEV);
  rdm_set_compatibility (RDM_RMTS_SRVC, RDM_MHI_EFS_DEV);

  remotefs_sio_info.sio_tcb = tcb;

  /* Wait for DTR to signal device ready */
  sigs = rex_wait (REMOTEFS_SRV_DTR_SIG);
  rex_clr_sigs (remotefs_sio_info.sio_tcb, sigs);
}

static uint64
remotefs_dbg_get_time (void)
{
  time_type now;
  uint64 ret_value;

  time_genoff_args_struct_type genoff_args;

  genoff_args.base = ATS_TOD;
  genoff_args.base_source = TIME_SCLK;
  genoff_args.ts_val = now;
  genoff_args.unit = TIME_MSEC; // unit of time
  genoff_args.operation = T_GET;
  time_genoff_opr (&genoff_args);
  ret_value = QW_CVT_Q2N (now);
  return ret_value;
}

static void
remotefs_dbg_add_packet(uint32 len, uint8* buffer, uint8 operation)
{
  struct remotefs_packet_info *dbg_packet;
  struct remotefs_dbg_info *dbg_info = &remotefs_sio_info.debug_info;
  size_t retval;

  dbg_info->dbg_packet_head =
                    (dbg_info->dbg_packet_head < REMOTEFS_DBG_PACKET_MAX)
                    ? dbg_info->dbg_packet_head : 0;

  memset(&dbg_info->dbg_packet[dbg_info->dbg_packet_head], 0,
         sizeof(struct remotefs_packet_info));

  dbg_packet = &dbg_info->dbg_packet[dbg_info->dbg_packet_head];
  dbg_packet->time_stamp = remotefs_dbg_get_time();
  dbg_packet->length = len;
  dbg_packet->operation = operation;

  if (len < 512)
  {
    len =
      (len > REMOTEFS_DBG_PACKET_BUFF_MAX)? REMOTEFS_DBG_PACKET_BUFF_MAX : len;
    retval = memscpy(&dbg_packet->data[0], REMOTEFS_DBG_PACKET_BUFF_MAX,
                    buffer, len);
    if (retval < len)
    {
      RMTS_MSG_HIGH("Warning: Truncation in debug packet", 0, 0, 0);
    }
  }

  dbg_info->dbg_packet_head++;
}
