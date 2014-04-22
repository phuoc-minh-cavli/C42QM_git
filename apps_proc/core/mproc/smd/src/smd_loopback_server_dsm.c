/*===========================================================================

                S M D   L O O P B A C K   S E R V E R   D S M

===========================================================================*/
/**
  @file smd_loopback_server_dsm.c
*/
/* 
  Defines the private API and structures used to implement the N-way SMD DSM
  loopback server implementations, for those processors that support it.
*/

/*===========================================================================
    Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_server_dsm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
07/24/13   bt      Make SMD use secure SMEM partitions.
05/01/13   bm      Remove usage of DSM_DIAG_SIO_RX_ITEM_POOL.
04/05/12   bt      Remove smd_target.h, move processor defs to smd_proc.h.
02/23/12   bt      Resolve pointer-casting compiler warning.
10/31/11   bt      Changed DSM pool to DSM_DIAG_SIO_RX_ITEM_POOL, since SMD
                   pool is no longer present.
07/06/11   bt      Initial implementation.
===========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_os_common.h"
#include "smd_loopback_server.h"
#include "smd_loopback_server_apis.h"
#include "smd_dsmi.h"
#include "smd_proc.h"
#include "queue.h"

#ifdef SMD_LPASS_PROC
#include "diagdsm.h"
#endif

/*===========================================================================
                   CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
struct smd_lb_dsm_server_struct
{
  /** The port id on which loopback is transmitting/reading data */
  smd_port_id_type      port_id;
  dsm_watermark_type    tx_queue;
  q_type                tx_queue_queue;
  /** The index within the loopback command register at which the loopback port
   * information can be found. */
  uint32                idx;
};

/*===========================================================================
                   LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/* The host set the DTR signal for a loopback port where the server uses the 
 * DSM API. */
static void smd_lb_dsm_dtr_cb ( void *cb_data )
{
  
}

/* The DSM API was able to successfully close the loopback port on both 
 * sides. */
static void smd_lb_dsm_close_cb ( void *cb_data )
{
  smd_lb_dsm_serv_handle dsm_handle = (smd_lb_dsm_serv_handle)cb_data;
  smd_lb_gen_close_cb(dsm_handle->idx);
  smd_free(dsm_handle);
}

/* Server DSM API loopback receive callback. */
static void smd_lb_dsm_rx_cb
(
  smd_port_id_type port_id,
  dsm_item_type   *packet,
  uint32           cb_data
)
{
  smd_lb_dsm_serv_handle dsm_handle = (smd_lb_dsm_serv_handle)cb_data;

  /* Echo the packet back, now that we have a full packet */
  dsm_enqueue(&dsm_handle->tx_queue, &packet);
  smd_wm_notify_write(dsm_handle->port_id);
}

/* Low watermark event: Enable inbound flow on the receive port */
static void smd_lb_low_watermark(dsm_watermark_type *queue, void *data)
{
  smd_lb_dsm_serv_handle dsm_handle = (smd_lb_dsm_serv_handle)data;

  if(smd_is_port_id_valid(dsm_handle->port_id))
  {
    smd_ioctl_param_type param;
    smd_wm_ioctl(dsm_handle->port_id, SMD_IOCTL_INBOUND_FLOW_ENABLE, &param);
  }
}

/* high watermark event: Disable inbound flow on the receive port */
static void smd_lb_high_watermark(dsm_watermark_type *queue, void *data)
{
  smd_lb_dsm_serv_handle dsm_handle = (smd_lb_dsm_serv_handle)data;

  if(smd_is_port_id_valid(dsm_handle->port_id))
  {
    smd_ioctl_param_type param;
    smd_wm_ioctl(dsm_handle->port_id, SMD_IOCTL_INBOUND_FLOW_DISABLE, &param);
  }
}

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_lb_server_dsm_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the SMD DSM API.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals for SMD DSM, which
  is all starting at FALSE.

  @param[in/out] lb_cmd_info   The loopback command structure for this port.
  @param[out]    lb_port_info  The internal server data structures for this 
                               port.
  @param[in]     idx           The index at which to find the loopback command 
                               structure for the port to be opened (used in 
                               callbacks).

  @return    TRUE if the open was successful.
             FALSE if the open fails for any reason, based on the arguments 
             provided in the loopback command structure.

  @sideeffects
  Allocates an internal data structure from heap memory.
*/
/*=========================================================================*/
boolean smd_lb_server_dsm_open
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info,
  uint32                     idx
)
{
  smd_open_param_type param;
  smd_ioctl_param_type ioctl_param;
  smd_loopback_command_type lb_cmd_local;
  SMD_MEMCPY_FROM_SMEM(&lb_cmd_local, sizeof(smd_loopback_command_type),
                       lb_cmd_info, sizeof(smd_loopback_command_type));
  
  param.fields = SMD_OPEN_PARAM_FIFO_SIZE | SMD_OPEN_PARAM_XFRFLOW;
  param.fifo_size = lb_cmd_local.fifo_size;
  param.xfrflow = (smd_xfrflow_type)lb_cmd_local.xfer_type;
  lb_port_info->dsm = smd_malloc(sizeof(struct smd_lb_dsm_server_struct));
  if (lb_port_info->dsm == NULL)
  {
    return FALSE;
  }
  lb_port_info->dsm->port_id = SMD_PORT_INVALID;
  
  q_init( &lb_port_info->dsm->tx_queue_queue );
  dsm_queue_init(&lb_port_info->dsm->tx_queue, 1024*64, 
                 &lb_port_info->dsm->tx_queue_queue);

  /* Set the low watermark equal to the FIFO size */
  lb_port_info->dsm->tx_queue.lo_watermark = SMD_STANDARD_FIFO;
  lb_port_info->dsm->tx_queue.lowater_func_ptr = 
      (wm_cb_type)smd_lb_low_watermark;
  lb_port_info->dsm->tx_queue.lowater_func_data = lb_port_info->dsm;

  /* Set the high watermark equal to twice the low watermark (two times the
   * FIFO size) */
  lb_port_info->dsm->tx_queue.hi_watermark = SMD_STANDARD_FIFO * 2;
  lb_port_info->dsm->tx_queue.hiwater_func_ptr = 
      (wm_cb_type)smd_lb_high_watermark;
  lb_port_info->dsm->tx_queue.hiwater_func_data = lb_port_info->dsm;
  
  lb_port_info->dsm->port_id = smd_wm_open_3( lb_cmd_local.name, 
                           (smd_channel_type)lb_cmd_local.edge, 
                           &lb_port_info->dsm->tx_queue, NULL, 
                           smd_lb_dsm_rx_cb, (uint32)lb_port_info->dsm, 
                           SMD_MEMCPY_MODE, 
                           DSM_DS_LARGE_ITEM_POOL, 
                           &param );
  lb_port_info->dsm->idx = idx;
  /* To set the local server dtr callback, need to use IOCTL. */
  ioctl_param.enable_dte_ready_event_ext.cb_func = smd_lb_dsm_dtr_cb;
  ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)idx;
  smd_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_ENABLE_DTR_EVENT_EXT, 
            &ioctl_param);
  /* Set the RS-232 signals in the smd_loopback_command_type to match the 
   * signals automatically set by the DSM open. */
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_dtr), FALSE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_cts), FALSE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_cd), FALSE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_ri), FALSE);
  
  return TRUE;
}

/*===========================================================================
  FUNCTION  smd_lb_server_dsm_close
===========================================================================*/
/**
  Closes an SMD loopback port, associated with the indicated internal data 
  structure.

  When the server has finished the close operation, the local close callback 
  will run, which sets the server-side loopback state to 
  SMD_LOOPBACK_STATE_CLOSED, notifies the client, and does any needed cleanup.

  @param[in] lb_port_info  The internal server data structure for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_dsm_close
(
  smd_loopback_info_type *lb_port_info
)
{
  smd_wm_close(lb_port_info->dsm->port_id, smd_lb_dsm_close_cb, 
               (void *)lb_port_info->dsm, FALSE);
}

/*===========================================================================
  FUNCTION  smd_lb_server_dsm_signal
===========================================================================*/
/**
  Sets all RS-232 signals of an SMD loopback port, associated with the 
  indicated loopback command structure and internal server structure.  The 
  values to set the signals to are obtained from this loopback command 
  structure, which may not necessarily be different from the port's current 
  values.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_dsm_signal
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info
)
{
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_dtr)))
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_DSR_ASSERT, NULL);
  }
  else
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_DSR_DEASSERT, NULL);
  }
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_cts)))
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_INBOUND_FLOW_ENABLE, 
                 NULL);
  }
  else
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_INBOUND_FLOW_DISABLE, 
                 NULL);
  }
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_cd)))
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_CD_ASSERT, NULL);
  }
  else
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_CD_DEASSERT, NULL);
  }
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_ri)))
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_RI_ASSERT, NULL);
  }
  else
  {
    smd_wm_ioctl(lb_port_info->dsm->port_id, SMD_IOCTL_RI_DEASSERT, NULL);
  }
}
