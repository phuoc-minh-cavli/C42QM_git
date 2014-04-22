/*===========================================================================

            S M D   L O O P B A C K   S E R V E R   M E M C P Y

===========================================================================*/
/**
  @file smd_loopback_server_memcpy.c
*/
/* 
  Defines the private API and structures used to implement the N-way SMD memcpy 
  loopback server implementations, for those processors that support it.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_server_memcpy.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
10/05/12   bt      Add OS and proc files for std libs and memcpy macros.
07/01/11   bt      Initial implementation.
===========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_os_common.h"
#include "smd_loopback_server.h"
#include "smd_loopback_server_apis.h"
#include "smd.h"
#include "smd_os.h"
#include "smd_proc.h"

/*===========================================================================
                   CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
#define  SMD_LOOPBACK_BUF_SIZE   512

typedef enum
{
  SMD_LOOPBACK_S_TX,
  SMD_LOOPBACK_S_TX_WAIT,
  SMD_LOOPBACK_S_RX,
  SMD_LOOPBACK_S_RX_WAIT,
}smd_memcpy_loopback_state;

struct smd_lb_memcpy_server_struct
{
  smd_port_id_type  port_id;
  uint8             tx_rx_buf[SMD_LOOPBACK_BUF_SIZE]; /* tx/rx buff */
  uint32            size;                             /* data to be sent */

  smd_memcpy_loopback_state state;
  
  /** The index within the loopback command register at which the loopback port
   * information can be found. */
  uint32            idx;
};

/*===========================================================================
                   LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/* The host set the DTR signal for a loopback port where the server uses the 
 * memcpy API. */
static void smd_lb_memcpy_dtr_cb ( void *cb_data )
{
  
}

/* The memcpy API was able to successfully close the loopback port on both 
 * sides. */
static void smd_lb_memcpy_close_cb ( void *cb_data )
{
  smd_lb_mcpy_serv_handle mcpy_handle = (smd_lb_mcpy_serv_handle)cb_data;
  smd_lb_gen_close_cb(mcpy_handle->idx);
  smd_free(mcpy_handle);
}

/* Server memcpy API loopback transfer callback. */
static void smd_lb_memcpy_tx_cb
(
  void     *cb_data,
  byte    **buf_ptr,
  uint32   *size
)
{
  /* cb_data is a pointer to the memcpy server loopback structure. */
  smd_lb_mcpy_serv_handle mcpy_handle = 
      (smd_lb_mcpy_serv_handle)cb_data;

  switch( mcpy_handle->state )
  {
    case SMD_LOOPBACK_S_TX:
      /* transmit what we have just read */
      *buf_ptr = mcpy_handle->tx_rx_buf;
      *size    = mcpy_handle->size;

      mcpy_handle->state = SMD_LOOPBACK_S_TX_WAIT;
      break;

    case SMD_LOOPBACK_S_TX_WAIT:
      /* stop transmitting */
      *buf_ptr = NULL;
      *size = 0;

      mcpy_handle->state = SMD_LOOPBACK_S_RX;

      /* poke to see if there is more to be read */
      smd_notify_read(mcpy_handle->port_id);
      break;

    default:
      /* ignore other states */
      *buf_ptr = NULL;
      *size    = 0;
      break;
  }
}

/* Server memcpy API loopback receive callback. */
static void smd_lb_memcpy_rx_cb
(
  void     *cb_data,
  uint32    bytes_read,
  byte    **buf_ptr,
  uint32   *size
)
{
  /* cb_data is a pointer to the memcpy server loopback structure. */
  smd_lb_mcpy_serv_handle mcpy_handle = 
      (smd_lb_mcpy_serv_handle)cb_data;

  switch( mcpy_handle->state )
  {
    case SMD_LOOPBACK_S_RX:
      /* give the full buffer for receiving */
      *buf_ptr = mcpy_handle->tx_rx_buf;
      *size    = SMD_LOOPBACK_BUF_SIZE;

      mcpy_handle->state = SMD_LOOPBACK_S_RX_WAIT;
      break;

    case SMD_LOOPBACK_S_RX_WAIT:
      /* transmit what is read if any */
      if(bytes_read)
      {
        *buf_ptr = NULL;
        *size    = 0;

        /* store the size to be transmitted */
        mcpy_handle->size = bytes_read;
        mcpy_handle->state = SMD_LOOPBACK_S_TX;
        smd_notify_write(mcpy_handle->port_id);
      }
      else
      {
        /* We were waiting for data, but didn't get any */
        ERR_FATAL( "Invalid loopback state/bytes_read count", 0, 0, 0 );
      }
      break;

    default:
      /* ignore other states */
      *buf_ptr = NULL;
      *size    = 0;
      break;
  }
}

/* Server memcpy API loopback flow change callback.  Not used right now, 
 * although it may be instructive to use it for actual flow control cases 
 * in the future. */
static uint32 smd_lb_memcpy_rx_flow_cb
(
  void     *cb_data,
  uint32    byte_count
)
{
  return byte_count;
}

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_lb_server_memcpy_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the memcpy API.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals for memcpy, which
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
boolean smd_lb_server_memcpy_open
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
  lb_port_info->mcpy = smd_malloc(sizeof(struct smd_lb_memcpy_server_struct));
  if (lb_port_info->mcpy == NULL)
  {
    return FALSE;
  }
  lb_port_info->mcpy->size  = SMD_LOOPBACK_BUF_SIZE;
  lb_port_info->mcpy->state = SMD_LOOPBACK_S_RX;
  lb_port_info->mcpy->idx   = idx;
  
  lb_port_info->mcpy->port_id = smd_open_memcpy_3( lb_cmd_local.name, 
                                  (smd_channel_type)lb_cmd_local.edge, 
                                  (void *)(lb_port_info->mcpy), 
                                  smd_lb_memcpy_tx_cb, smd_lb_memcpy_rx_cb, 
                                  smd_lb_memcpy_rx_flow_cb, &param );
  /* To set the local server dtr callback, need to use IOCTL. */
  ioctl_param.enable_dte_ready_event_ext.cb_func = smd_lb_memcpy_dtr_cb;
  ioctl_param.enable_dte_ready_event_ext.cb_data = 
                                         (void *)(&(lb_port_info->mcpy));
  smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_ENABLE_DTR_EVENT_EXT, 
            &ioctl_param);
  
  /* Set the RS-232 signals in the smd_loopback_command_type to match the 
   * signals automatically set by the memcpy open. */
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_dtr), FALSE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_cts), FALSE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_cd), FALSE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_ri), FALSE);
  
  return TRUE;
}

/*===========================================================================
  FUNCTION  smd_lb_server_memcpy_close
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
void smd_lb_server_memcpy_close
(
  smd_loopback_info_type *lb_port_info
)
{
  smd_close(lb_port_info->mcpy->port_id, smd_lb_memcpy_close_cb, 
            (void *)lb_port_info->mcpy, FALSE);
}

/*===========================================================================
  FUNCTION  smd_lb_server_memcpy_signal
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
void smd_lb_server_memcpy_signal
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info
)
{
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_dtr)))
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_DSR_ASSERT, NULL);
  }
  else
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_DSR_DEASSERT, NULL);
  }
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_cts)))
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_INBOUND_FLOW_ENABLE, 
              NULL);
  }
  else
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_INBOUND_FLOW_DISABLE, 
              NULL);
  }
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_cd)))
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_CD_ASSERT, NULL);
  }
  else
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_CD_DEASSERT, NULL);
  }
  if (SMD_READ_SMEM_1(&(lb_cmd_info->sig_ri)))
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_RI_ASSERT, NULL);
  }
  else
  {
    smd_ioctl(lb_port_info->mcpy->port_id, SMD_IOCTL_RI_DEASSERT, NULL);
  }
}
