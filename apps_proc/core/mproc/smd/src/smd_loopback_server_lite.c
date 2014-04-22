/*===========================================================================

            S M D   L O O P B A C K   S E R V E R   S M D L I T E

===========================================================================*/
/**
  @file smd_loopback_server_lite.c
*/
/* 
  Defines the private API and structures used to implement the N-way SMD Lite 
  loopback server implementations.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_server_lite.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
07/31/12   pa      Moved smdl_loop_type to header file.  Added smd_internal.h
07/01/11   bt      Initial implementation.
===========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_os_common.h"
#include "smd_internal.h"
#include "smd_loopback_server.h"
#include "smd_loopback_server_apis.h"
#include "smdl_debug.h"

/*===========================================================================
                   CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/


/*===========================================================================
                   LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_lb_lite_alloc_handle
===========================================================================*/
/**
  Allocate an SMD Lite loopback handle and buffer for received data
  
  @param[in] buffer_len   Length of the buffer to use for data received by
                          the echo port. Should be equal to the FIFO size.

  @return Pointer to an SMD Lite loopback handle, or NULL if the memory could 
          not be allocated.
*/
/*==========================================================================*/
static smdl_loop_type * smd_lb_lite_alloc_handle
(
  uint32 buffer_len
)
{
  smdl_loop_type *handle;

  handle = smd_malloc(sizeof(smdl_loop_type));
  if(handle == NULL)
  {
    return NULL;
  }

  memset(handle, 0, sizeof(smdl_loop_type));
  handle->buffer_len = buffer_len;

  handle->buffer = smd_malloc(handle->buffer_len);
  if(handle->buffer == NULL)
  {
    smd_free(handle);
    return NULL;
  }

  return handle;
}

/*===========================================================================
  FUNCTION  smd_lb_lite_free_handle
===========================================================================*/
/**
  Frees an SMD Lite echo handle
  
  @param[in] handle  The SMD Lite echo handle. Must have been allocated by
                     smdl_echo_alloc_handle() and must not be NULL.

  @return None.
*/
/*==========================================================================*/
void smd_lb_lite_free_handle
(
  smdl_loop_type *handle
)
{
  smd_free(handle->buffer);
  smd_free(handle);
}

/*===========================================================================
  FUNCTION  smd_lb_lite_read_write
===========================================================================*/
/**
  Reads data "from" and writes data "to"

  Uses the "from"->buffer to store data until it can be written "to".

  @param[in] from  Loopback structure with the buffer to read from.
  @param[in] to    Loopback structure with the buffer to write to.

  @return None.
*/
/*==========================================================================*/
void smd_lb_lite_read_write
(
  smdl_loop_type *from,
  smdl_loop_type *to
)
{
  SMDL_DEBUG("smd_lb_lite_read_write: from=%p to=%p from->data_len=%d",
      from, to, from->data_len);

  /* Try to write leftover data in the loopback buffer to the transfer 
   * smd buffer.  Streaming mode avoids storing leftover data. */
  if(from->data_len > 0)
  {
    int32 fifo_free_space = smdl_tx_peek(to->port);
    int32 wlen = smdl_write(to->port, from->data_len, from->buffer, 0);
    SMDL_DEBUG("SMDL loop(%p): Wrote %d (free %d)", to->port, wlen, 
               fifo_free_space);
    
    if (wlen == 0){
      /* The outgoing port is full; no data could be written; wait until 
       * the next event_write. */
    }
    else if( (to->port->mode.lite.transfer_mode == SMDL_MODE_PACKET) && 
             (wlen != from->data_len) )
    {
      ERR_FATAL("Loop port: smdl_write() failed", 0, 0, 0);
    }
    else
    {
      /* If wlen < from->data_len, then outgoing port is full; all of the data 
       * could not be written; wait until the next event_write. 
       * If wlen == from_data_len, we may be able to loop back more data. */
      from->data_len = from->data_len - wlen;
      if (from->data_len > 0)
      {
        /* Realign the buffer to put the unsent data at the front again. */
        smd_mem_move(from->buffer, from->buffer_len, from->buffer + wlen, 
                     from->data_len);
      }
    }
  }

  /* Try to read more data from the incoming FIFO and write it back to the
   * outgoing FIFO, if the loopback buffer is currently empty. */
  if(from->data_len == 0)
  {
    int32 len;
    int32 wlen;
    if (to->port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
    {
      do {
        len = smdl_read(from->port, from->buffer_len, from->buffer, 0);
        SMDL_DEBUG("smd_lb_lite_read_write: from=%p from->port=%p read=%d",
            from, from->port, len);
        if(len > 0)
        {
          /* The maximum packet size that can be written is the FIFO size minus
           * the packet header (20 bytes) minus the must-be-empty padding
           * (currently 4 bytes). We can *read* messages larger than, if the
           * transmitter is using SMD, that because SMD Lite can read the packet
           * header first, leaving space for a packet up to the full capacity of
           * the FIFO. */
          if((uint32)(len + SMD_PACKET_HEADER_BYTE_SIZE + 4) > from->buffer_len)
          {
            /* Truncate at maximum packet length */
            SMDL_DEBUG("SMDL loop(%p): Truncating %d to %d",
                       from->port, len, from->buffer_len - 
                                        (SMD_PACKET_HEADER_BYTE_SIZE + 4));
            len = from->buffer_len - (SMD_PACKET_HEADER_BYTE_SIZE + 4);
          }
          wlen = smdl_write(to->port, len, from->buffer, 0);
          SMDL_DEBUG("SMDL loop(%p): Read %d, wrote %d", 
                     from->port, len, wlen);
          if(wlen == 0)
          {
            /* Outgoing port full; data could not be written */
            from->data_len = len;
            len = 0;
          }
          else if(wlen != len)
          {
            ERR_FATAL("Loop port: smdl_write() failed", 0, 0, 0);
          }
        }
      } while(len > 0);
    }
    else if (to->port->mode.lite.transfer_mode == SMDL_MODE_STREAMING)
    {
      do {
        len = smdl_read(from->port, from->buffer_len, from->buffer, 0);
        SMDL_DEBUG("smd_lb_lite_read_write: from=%p from->port=%p read=%d",
            from, from->port, len);
        /* This loop will end when there is no more data that can be read 
         * (len == 0), or no more data can be written because the transfer 
         * buffer is full (rlen == 0 and len == -1). */
        if(len > 0)
        {
          wlen = smdl_write(to->port, len, from->buffer, 0);
          from->data_len = len - wlen;
          if (wlen < len)
          {
            /* Realign the buffer to put the unsent data at the front again. */
            smd_mem_move(from->buffer, from->buffer_len, from->buffer + wlen, 
                         from->data_len);
            len = 0;
          }
        }
      } while(len > 0);
    }
  }
}

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_lb_server_lite_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the SMDL API.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals for SMDL, which
  is DTR, CTS, and CD equal to TRUE and RI equal to FALSE.

  @param[in/out] lb_cmd_info   The loopback command structure for this port.
  @param[out]    lb_port_info  The internal server data structures for this 
                               port.
  @param[in]     idx           The index at which to find the loopback command 
                               structure for the port to be opened (used in 
                               callbacks).

  @return    NULL if the open fails for any reason, based on the arguments
             provided in the loopback command structure.
             Non-NULL if the open is successful.

  @sideeffects
  Allocates an internal data structure from heap memory.
*/
/*=========================================================================*/
smdl_loop_type * smd_lb_server_lite_open
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info,
  uint32                     idx
)
{
  uint32 flags;
  smd_loopback_command_type lb_cmd_local;
  smdl_loop_type * handle;

  SMD_MEMCPY_FROM_SMEM(&lb_cmd_local, sizeof(smd_loopback_command_type),
                       lb_cmd_info, sizeof(smd_loopback_command_type));
  switch (lb_cmd_local.xfer_type)
  {
  case SMD_NO_XFRFLOW:
    return NULL; /* SMDL must specify a xfrflow type. */
  case SMD_STREAMING_TYPE:
    flags = SMDL_OPEN_FLAGS_MODE_STREAMING;
    break;
  case SMD_PKTXFR_TYPE:
    flags = SMDL_OPEN_FLAGS_MODE_PACKET;
    break;
  default:
    return NULL;
  }
  
  handle = smd_lb_lite_alloc_handle(lb_cmd_local.fifo_size);
  if(handle == NULL)
  {
    return NULL;
  }
  handle->rx = handle;
  handle->idx = idx;
  
  handle->port =
      smdl_open( lb_cmd_local.name, (smd_channel_type)lb_cmd_local.edge, flags, 
                 lb_cmd_local.fifo_size, smd_lb_lite_cb, handle );
  if (handle->port == NULL)
  {
    smd_lb_lite_free_handle(handle);
    return NULL;
  }
  handle->rx_port = handle->port;
  lb_port_info->lite = handle->port;
  
  /* Set the RS-232 signals in the smd_loopback_command_type to match the 
   * signals automatically set by the SMD Lite open. */
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_dtr), TRUE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_cts), TRUE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_cd), TRUE);
  SMD_WRITE_SMEM_1(&(lb_cmd_info->sig_ri), FALSE);
  
  return handle;
}

/*===========================================================================
  FUNCTION  smd_lb_server_lite_close
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
void smd_lb_server_lite_close
(
  smd_loopback_info_type *lb_port_info
)
{
  if (smdl_close(lb_port_info->lite) == -1)
  {
    /* This should not be possible. */
    ERR_FATAL("smd_lb_server_lite_close: smdl handle was NULL.", 0, 0, 0);
  }
}

/*===========================================================================
  FUNCTION  smd_lb_server_lite_signal
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
void smd_lb_server_lite_signal
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info
)
{
  ASSERT(lb_port_info->lite != NULL);
  /* Interrupts will only be sent if the signal is changed. */
  smdl_sig_set(lb_port_info->lite, SMDL_SIG_DTR_DSR, 
               SMD_READ_SMEM_1(&(lb_cmd_info->sig_dtr)));
  smdl_sig_set(lb_port_info->lite, SMDL_SIG_RTS_CTS, 
               SMD_READ_SMEM_1(&(lb_cmd_info->sig_cts)));
  smdl_sig_set(lb_port_info->lite, SMDL_SIG_CD, 
               SMD_READ_SMEM_1(&(lb_cmd_info->sig_cd)));
  smdl_sig_set(lb_port_info->lite, SMDL_SIG_RI, 
               SMD_READ_SMEM_1(&(lb_cmd_info->sig_ri)));
}
