/*===========================================================================

             Shared Memory Driver Watermark Source File


 Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_dsm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/15   rv      Packet header buffers are freed in close callback.  
12/12/13   rv      SMEM Partition backwards compatibility is removed.
09/25/13   bt      Change SMD_WM_GET_INFO macro to smd_wm_get_info with checks.
08/01/13   bt      Trigger DTR callbacks when they are first registered.
07/24/13   bt      Make SMD use secure SMEM partitions.
11/07/12   bt      Fix DSM header allocations to malloc with byte-size define.
09/04/12   rs      + Dynamically allocate RX/TX packet headers
                   + Removed fields for data mover.
05/29/12   pa      Added smd_wm_rx_packet_header_prev[] to store previous
                   packet headers which are saved for debug use.
11/08/11   bm      Integrated CL 1879286, and added proper SSR handling to 
                   clear DSM queues at SSR.
04/29/11   bt      Resolve compiler warning (unused val in smd_wm_open_3).
04/01/11   bm      Updated function smd_wm_open_3() for handling
                   smd_xfrflow_type param as part of smd_open_param_type input
                   param struct and not as a separate input param.
04/01/11   bt      Removed deprecated smd_wm_open, due to smdi_get_port_name.
02/11/11   bt      Remove SMD_DSM_INIT_MUTEX from smd_dsm.c and smd_os.h files.
02/11/11   bt      Removed old unused SMD profiling code.
01/06/11   bt      Removed SMD Block and Data Mover protocols.
10/25/20   bt      Fixed port_name argument bugs in all smd open functions.
07/21/10   tl      Implemented SMD API for SMD priority.
05/28/10   hwu     Moved ioctl SMD_IOCTL_SET_XFR_TYPE from dsm layer to 
                   buffer layer. 
05/03/10   tl      Use critical sections instead of INTLOCK
12/08/09   esh     Added checks to fix Klocwork warnings
10/12/09   bfc/rs  Removed control port muxing.
09/29/09   bfc/rs  Added variable sized FIFO API support.
06/16/09   hwu     Fixed the bug to let dsm layer support named open.
06/10/09   bfc     CMI updates.
03/31/09   hwu     Merge in the changes to enable data stacking in smd_sio.
01/26/09   bfc     Added the dataxfr mode to the CID entry.
11/26/08   bfc     Added an IOCTL for data stacking.
09/02/08   jlk     changed smd_wm_open to just call smd_wm_open_2
06/10/08   sa      Smd_target.h cleanup. 
04/29/08   ih      Added WM7 support
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
02/21/08   bfc     First SMD Block port check in.
01/14/08   bfc     Created a SMD base struct.
10/25/07   hwu     Changed to pass client callback data to
                   smd_wm_memcpy_open_2 and smd_wm_dmov_open_2.
10/19/07   hwu     Added smd_wm_open_2.
10/18/07   bfc     New SMD IOCTLs
10/08/07   hwu     Changed the order to include smd_os.h and smd.h
                   because smd_channel_type moved to smd.h
07/23/07   ptm     Code optimizations.
07/18/07   hwu     Change to use smd_target.h for common headers.
05/02/07   hwu     Change to use SMD_NUM_CHANNELS.
04/26/07   ptm     Use SMD NUM PORTS in place of SMD NUM STREAMS and
                   SMD PORT LAST
01/11/07   bfc     Fixed a typo in the last checkin
01/09/07   bfc     Added SMD using DM
01/04/06   bfc     Componentized SMD using MEMCPY.
12/18/06   bfc     Moved control port logic to the smd_wm_layer
12/05/06   bfc     Fixed a potential memory leak that could occur if
                   a client did not supply a rx callback or rx_queue.
11/03/06   ptm     Change SMD_SMD_... symbols to SMD_... symbols and rename
                   dsm_iov to smd_iov_type.
10/10/06   ptm     Fix some lint warnings.
10/09/06   bfc     Removed a SMD Tranport layer to DSM dependency
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
09/05/06   ptm     Remove references to REX.
07/19/06   ptm     Fix arg to ERR_FATAL in smd_wm_memcpy_tx_buf.
07/19/06   ptm     Code review comments.
06/22/06   bfc     Added the client capability to specify a receive DSM
                   WM queue
06/21/06   bfc     Fixed a race condition that could occur when the
                   client changes mode
05/25/06   ptm     Set remaining bytes even if unable to get a buffer.
05/11/06   bfc     Increased the maximum size of a packet and fixed a rcv
                   only port merge error.
05/01/06   bfc     Added support for a recv only port
04/28/06   ptm     Added flow control callback.
04/27/06   bfc     Added support for a DTR cb critical section, and modified
                   the flush cb to empty the tx_queue.
03/27/06   bfc     Modified SMD to use the new sio extended funcion callback
                   parameters
02/01/06   ptm     Fix WinCE compiler warnings.
11/08/05   ptm     Initial version
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smd_os.h"

#include "dsm.h"
#include "smd.h"
#include "smd_v.h"
#include "smd_internal.h"
#include "smd_os_common.h"
#include "smd_dsm.h"
#include "smd_dsm_v.h"
#include "smd_dsmi.h"
#include "smd_memcpy.h"

/*===========================================================================
                     CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
smd_wm_info_type *smd_wm_port_to_info[SMD_DSM_NUM_PROCS][SMD_NUM_CHANNELS];

/*===========================================================================
                     LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_wm_get_info_ptr

DESCRIPTION   returns a pointer to the smd_wm_port_to_info pointer

ARGUMENTS     port_id - SMD port to be used by this stream.

RETURN VALUE  Pointer to the associated smd_wm_info_type.

SIDE EFFECTS  Called in client and SMD context.  May Assert if port_id is
              invalid.
===========================================================================*/
static smd_wm_info_type **smd_wm_get_info_ptr
(
  smd_port_id_type port_id
)
{
  smem_host_type remote_host = PORT_GET_REMOTE(port_id);
  uint8 port_idx = PORT_GET_IDX(port_id);
  ASSERT( remote_host < SMD_DSM_NUM_PROCS && port_idx < SMD_NUM_CHANNELS );

  /* We only need to support remote_hosts Apps, Modem, and ADSP, which happen 
   * to be smem_host_type{0:2}. */
  return &smd_wm_port_to_info[remote_host][port_idx];
} /* smd_wm_get_info_ptr */

/*===========================================================================
FUNCTION      smd_wm_alloc_info

DESCRIPTION   Allocates an smd_wm_info_type structure, initializes all of its
              fields to zero and then initializes the link field and sets the
              port_id field to the given port_id.

ARGUMENTS     port_id - SMD port to be used by this stream.

RETURN VALUE  Pointer to allocated info structure.

SIDE EFFECTS  Called in client context
===========================================================================*/
static smd_wm_info_type *smd_wm_alloc_info
(
  smd_port_id_type port_id
)
{
  smd_wm_info_type *info;
  smd_wm_info_type **port_to_info;

  port_to_info = smd_wm_get_info_ptr( port_id );

  ASSERT( *port_to_info == NULL );

  info = (smd_wm_info_type *) smd_malloc(sizeof(smd_wm_info_type));

  if( info != NULL )
  {
    memset( info, 0, sizeof(smd_wm_info_type) );
    info->port_id = port_id;
    *port_to_info = info;
  }

  return info;
} /* smd_wm_alloc_info */

/*===========================================================================
FUNCTION      smd_wm_free_info

DESCRIPTION   Deallocate an smd_wm_info_type structure.

ARGUMENTS     info - pointer to the allocated info structure.

RETURN VALUE  None

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_free_info
(
  smd_wm_info_type *info
)
{
  smd_wm_info_type **port_to_info;
  int32              ret;

  port_to_info = smd_wm_get_info_ptr( info->port_id );

  ASSERT( *port_to_info == info );
  
  /* Need to delete the channel info because the ref_count in
     smd_channel_alloc_tbl is incremented during wm open */
  smdi_delete_channel_info( info->port_id );

  *port_to_info = NULL;
  ret = smd_free(info);
  ASSERT(ret == 0);
} /* smd_wm_free_info */

/*===========================================================================
FUNCTION      smd_wm_close_cb

DESCRIPTION   Close callback that is called when the stream is closed. It is
              registered when the client calls sm_wm_close. It frees the info
              structure associated with the port and then calls the client's
              callback.

ARGUMENTS     port_id - SMD port being closed

              cb_data - a pointer to the info structure associated with the
                        port_id

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_close_cb( void *cb_data )
{
  smd_wm_info_type *info = (smd_wm_info_type *) cb_data;
  dsm_item_type *item;
  smd_close_callback_fn close_cb;
  void *close_cb_data;

  close_cb = info->close_cb;
  close_cb_data = info->close_cb_data;

  if( info->tx_queue != NULL )
  {
    /* Free and pending transmit data */
    while( (item = dsm_dequeue( info->tx_queue )) != NULL )
    {
      dsm_free_packet( &item );
    }
  }

  dsm_free_packet( &info->tx_item_ptr );
  dsm_free_packet( &info->tx_item_ptr_next );

  /* Free any partially read data item */
  dsm_free_packet( &info->rx_head_ptr );

  /* Free the packet header buffers */
  smd_free(info->tx_packet_header);
  smd_free(info->rx_packet_header);
  smd_free(info->rx_packet_header_prev);

  smd_wm_free_info( info );

  if( close_cb != NULL )
  {
    close_cb( close_cb_data );
  }
} /* smd_wm_close_cb */

/*===========================================================================
FUNCTION      smd_wm_flush_cb

DESCRIPTION   Flush callback that is called when the stream is flushed. It is
              registered when the client calls sm_wm_ioctl. It calls the
              client's callback.

              This routine is only called when a client callback was included,
              so it does not need to check if the client callback is NULL.

              Flush any pending transmit data because if there's any left it
              means the other end of the pipe is no longer connected and the
              data will never be received.

ARGUMENTS     port_id - SMD port being flushed

              cb_data - a pointer to the info structure associated with the
                        port_id

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_flush_cb
(
  void *cb_data
)
{
  dsm_item_type *item;
  smd_wm_info_type *info = (smd_wm_info_type *) cb_data;

  if( info->tx_queue != NULL )
  {
    /* Free any pending transmit data that is left over */
    while( (item = dsm_dequeue( info->tx_queue )) != NULL )
    {
      dsm_free_packet( &item );
    }
  }

  info->flush_cb();
} /* smd_wm_flush_cb */

/*===========================================================================
FUNCTION      smd_wm_dtr_cb

DESCRIPTION   DTR toggle callback that is called when the DTR toggles.
              Registered immediately after open.

              When DTR goes away it means that the other processor has crashed,
              so we cleanup the transmit queue and receive queue.

              The Client's callback (if any) is called.

ARGUMENTS     port_id - SMD port being flushed

              cb_data - a pointer to the info structure associated with the
                        port_id

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_dtr_cb( void *cb_data )
{
  smd_wm_info_type *info = (smd_wm_info_type *) cb_data;
  smd_vv_func_ptr_type dtr_cb;
  smd_vu_func_ptr_type dtr_cb_ext;
  void *dtr_cb_ext_data;

  dtr_cb = info->dtr_callback_fn;

  SMD_DTR_LOCK();
  dtr_cb_ext = info->dtr_callback_ext_fn;
  dtr_cb_ext_data = info->dtr_callback_ext_data;
  SMD_DTR_UNLOCK();

  /* Call the "simple" callback */
  if( dtr_cb != NULL )
  {
    dtr_cb();
  }

  /* Call the "extended" callback */
  if( dtr_cb_ext != NULL )
  {
    dtr_cb_ext( dtr_cb_ext_data );
  }
} /* smd_wm_dtr_cb */

/*===========================================================================
FUNCTION      smd_wm_reset_cb
===========================================================================*/
/*!
@brief        Stream Reset callback for smd_wm layer. Called when the smd 
              Memcpy's stream is reset (currently initiated from SSR code).

@param[in]    cb_data    The smd_wm_info_type structure related to this stream.
                         This parameter is passed as a void * argument while
                         registering this callback with the smd_memcpy layer.                        

@return       None

@dependencies None

*/
/*=========================================================================*/
void smd_wm_reset_cb(void *cb_data)
{
  smd_wm_info_type *info = (smd_wm_info_type*) cb_data;
  smd_port_id_type port_id = info->port_id;
  dsm_item_type *item;
  smd_ioctl_param_type param;
  boolean dte_asserted;

  /* Clean up the Rx/Tx queues */
  param.dte_ready_asserted = &dte_asserted;
  smd_ioctl( port_id, SMD_IOCTL_DTE_READY_ASSERTED, &param);

  if( ! dte_asserted )
  {
    if( info->tx_queue != NULL )
    {
      /* Free any pending transmit data */
      while( (item = dsm_dequeue( info->tx_queue )) != NULL )
      {
        dsm_free_packet( &item );
      }
    }

    dsm_free_packet( &info->tx_item_ptr );
    dsm_free_packet( &info->tx_item_ptr_next );

    /* Free any partially read data item and reset read variables */
    dsm_free_packet( &info->rx_head_ptr );
    info->rx_tail_ptr = NULL;
    info->rx_header_valid = FALSE;
  }  
}/* smd_wm_reset_cb */

/*===========================================================================
                     EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_wm_get_info

DESCRIPTION   Returns the smd_wm_port_to_info pointer for the port.

ARGUMENTS     port_id - SMD port to be used by this stream.

RETURN VALUE  Pointer to the associated smd_wm_info_type.

SIDE EFFECTS  Called in client and SMD context.  May Assert if port_id is
              invalid.
===========================================================================*/
smd_wm_info_type *smd_wm_get_info
(
  smd_port_id_type port_id
)
{
  smd_wm_info_type **info_ptr = smd_wm_get_info_ptr(port_id);
  
  ASSERT(*info_ptr != NULL );
  return (*info_ptr);
}

/*===========================================================================
FUNCTION      smd_wm_init

DESCRIPTION   Initialize the data structures internal to this subsystem.
              Must be called before any other APIs in this subsystem are
              called.

ARGUMENTS     None.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
void smd_wm_init( void )
{
  memset( smd_wm_port_to_info, 0, sizeof(smd_wm_port_to_info) );
} /* smd_wm_init */

/*===========================================================================
FUNCTION      smd_wm_open_2

DESCRIPTION   Initialize a shared memory port for use by a client. Must be
              called before other port operations are called.

ARGUMENTS     port_name - SMD port to open. Do not use whitespace in port_name.

              channel_type - Specifies the connection type

              tx_queue - transmit watermark queue

              rx_queue - Client receive watermark queue.  If non-NULL
                         the driver will enqueue data read from the buffer.
                         If xfrflow_type (see below) is packet, then
                         only complete data chains will be enqueued.

              read_cb  - Client read callback.  If non-NULL the driver will
                         call this callback when it is done filling a read
                         buffer. If  xfrflow_type (see below) is packet,
                         then read_cb will be called once for each packet
                         received.

              cb_data - user data passed to read_cb

              dataxfr_mode - the data transfer mode: memcpy.

              xfrflow_type - the data flow type: packet or stream.

              dsm_pool_id - the DSM pool id to use when allocating read buffers.

RETURN VALUE  Returns the port id.

SIDE EFFECTS  Called in client context
===========================================================================*/
smd_port_id_type smd_wm_open_2
(
  char                    *port_name,
  smd_channel_type         channel_type,
  dsm_watermark_type      *tx_queue,
  dsm_watermark_type      *rx_queue,
  smd_wm_read_callback_fn  read_cb,
  uint32                   cb_data,
  smd_dataxfr_mode_type    dataxfr_mode,
  smd_xfrflow_type         xfrflow_type,
  dsm_mempool_id_enum_type dsm_pool_id
)
{
  smd_open_param_type smd_param;
  smd_param.xfrflow = xfrflow_type;
  smd_param.fields = SMD_OPEN_PARAM_XFRFLOW;
  return smd_wm_open_3(  port_name,
                         channel_type,
                         tx_queue,
                         rx_queue,
                         read_cb,
                         cb_data,
                         dataxfr_mode,
                         dsm_pool_id,
                         &smd_param );
}/* smd_wm_open_2 */

/*===========================================================================
FUNCTION      smd_wm_open_sz

DESCRIPTION   Initialize a shared memory port for use by a client. Must be
              called before other port operations are called.

ARGUMENTS     port_name - The name of the SMD port to be opened. Do not use 
                          whitespace in port_name.

              channel_type - Specifies the connection type

              tx_queue - transmit watermark queue

              rx_queue - Client receive watermark queue.  If non-NULL 
                         the driver will enqueue data read from the buffer.
                         If xfrflow_type (see below) is packet, then
                         only complete data chains will be enqueued.

              read_cb  - Client read callback.  If non-NULL the driver will
                         call this callback when it is done filling a read
                         buffer. If  xfrflow_type (see below) is packet,
                         then read_cb will be called once for each packet
                         received.

              cb_data - user data passed to read_cb

              dataxfr_mode - the data transfer mode: memcpy.

              xfrflow_type - the data flow type: packet or stream.

              dsm_pool_id - the DSM pool id to use when allocating read buffers.

              fifo_sz - The FIFO size.  Must be >= SMD_MIN_FIFO and be cached 
                        aligned

RETURN VALUE  Returns the port id.

SIDE EFFECTS  Called in client context
===========================================================================*/  
smd_port_id_type smd_wm_open_sz
(
  char                    *port_name,
  smd_channel_type         channel_type,
  dsm_watermark_type      *tx_queue,
  dsm_watermark_type      *rx_queue,
  smd_wm_read_callback_fn  read_cb,
  uint32                   cb_data,
  smd_dataxfr_mode_type    dataxfr_mode,
  smd_xfrflow_type         xfrflow_type,
  dsm_mempool_id_enum_type dsm_pool_id,
  uint32                   fifo_sz
)
{
  smd_open_param_type param;

  param.fifo_size = fifo_sz;
  param.xfrflow = xfrflow_type;
  param.fields = SMD_OPEN_PARAM_FIFO_SIZE | SMD_OPEN_PARAM_XFRFLOW;

  return smd_wm_open_3( port_name,
                        channel_type,
                        tx_queue,
                        rx_queue,
                        read_cb,
                        cb_data,
                        dataxfr_mode,
                        dsm_pool_id,
                        &param );
}

/*===========================================================================
FUNCTION      smd_wm_open_3
===========================================================================*/
/*!
@brief        Initialize a named shared memory port, with optional parameters,
              for use by a client.

              Must be called before other port operations are called.

@param[in]    port_name   The name of the SMD port to be opened.  Do not use 
                          whitespace in port_name.

@param[in]    channel_type   Specifies the connection type

@param[in]    tx_queue   transmit watermark queue

@param[in]    rx_queue   Client receive watermark queue.  If non-NULL 
                         the driver will enqueue data read from the buffer.
                         If \a xfrflow_type is packet, then only complete data
                         chains will be enqueued.

@param[in]    read_cb    Client read callback.  If non-NULL the driver will
                         call this callback when it is done filling a read
                         buffer. If \a xfrflow_type is packet, then \a read_cb
                         will be called once for each packet received.

@param[in]    cb_data   user data passed to \a read_cb

@param[in]    dataxfr_mode   the data transfer mode: memcpy.

@param[in]    dsm_pool_id   the DSM pool id to use when allocating read buffers.

@param[in]    smd_param     Optional paramaters used to configure the SMD
                            port. If this pointer is NULL, the default values
                            will be used instead. See \a smd_open_param_type
                            for detailed documentation on using the optional
                            paramaters.

@return       The port id of the open SMD DSM port.

@dependencies SMD DSM must have been initialized by calling smd_wm_init().
              This is normally done as part of the platform-specific SMD
              initialization.
*/
/*=========================================================================*/
smd_port_id_type smd_wm_open_3
(
  const char                *port_name,
  smd_channel_type           channel_type,
  dsm_watermark_type        *tx_queue,
  dsm_watermark_type        *rx_queue,
  smd_wm_read_callback_fn    read_cb,
  uint32                     cb_data,
  smd_dataxfr_mode_type      dataxfr_mode,
  dsm_mempool_id_enum_type   dsm_pool_id,
  const smd_open_param_type *smd_param
)
{
  smd_wm_info_type      *info;
  smd_ioctl_param_type   param;
  smd_port_id_type       port_id;
  smd_xfrflow_type       xfrflow = SMD_INVALID_TYPE;
  uint32                *tx_packet_header_ptr;
  uint32                *rx_packet_header_ptr;
  uint32                *rx_packet_header_prev_ptr;


  if(channel_type == SMD_INVALID_CHANNEL_TYPE){
      ERR_FATAL( "Tried to open invalid smd watermark channel type", 0, 0, 0 );
  }

  if (port_name == NULL || !strcmp(port_name, ""))
  {
    ERR_FATAL("%s: Invalid port name.", __FUNCTION__, 0, 0);
  }

  /* Use xfrflow type from the optional parameters if it is specified; otherwise
   * use the default. */
  if((smd_param != NULL) && (smd_param->fields & SMD_OPEN_PARAM_XFRFLOW))
  {
    xfrflow = smd_param->xfrflow;
  }
  ASSERT(xfrflow < SMD_INVALID_TYPE);

  /* Allocate RX/TX packet headers */
  tx_packet_header_ptr = ( uint32 * )smd_malloc( SMD_WM_PACKET_HEADER_BYTE_SIZE );
  rx_packet_header_ptr = ( uint32 * )smd_malloc( SMD_WM_PACKET_HEADER_BYTE_SIZE );
  rx_packet_header_prev_ptr = ( uint32 * )smd_malloc( SMD_WM_PACKET_HEADER_BYTE_SIZE );

  if( !( tx_packet_header_ptr ) || !( rx_packet_header_ptr ) || !( rx_packet_header_prev_ptr ) )
  {
    ERR_FATAL("%s: Could not allocate RX/TX packet headers.", __FUNCTION__, 0, 0 );
  }

  /* Get the port_id and allocate an item for this port id. */
  port_id = smdi_alloc_channel_info( port_name,
                                     channel_type,
                                     xfrflow);

  info = smd_wm_alloc_info( port_id );
  ASSERT(info != NULL);

  info->tx_queue = tx_queue;
  info->rx_queue = rx_queue;
  info->read_cb = read_cb;
  info->read_cb_data = cb_data;
  info->dataxfr_mode = dataxfr_mode;
  info->xfrflow_type = xfrflow;
  info->dsm_pool_id = dsm_pool_id;
  info->max_packet_size = SMD_WM_DEFAULT_MAX_PACKET_SIZE;
  info->tx_packet_header = tx_packet_header_ptr;
  info->rx_packet_header = rx_packet_header_ptr;
  info->rx_packet_header_prev = rx_packet_header_prev_ptr;
  info->wait = FALSE;

  switch( dataxfr_mode )
  {
    case SMD_MEMCPY_MODE:
    {
      smd_port_id_type ret;
      ret = smd_wm_memcpy_open_3( port_name, channel_type,
                                  (void *)port_id, smd_param );
      if( !smd_is_port_id_valid(ret) )
      {
        /* no SMD transport mode is supported */
        ASSERT(0);
      }

      /* Register the Stream Reset callback with memcpy layer */
      param.memcpy_reset.cb_func = smd_wm_reset_cb;
      param.memcpy_reset.cb_data = (void*)info;
      smd_ioctl(port_id, SMD_IOCTL_REGISTER_MEMCPY_RESET, &param);
      break;
    }
    default:
      /* unsupported mode */
      ASSERT(0);
  }

  param.enable_dte_ready_event_ext.cb_func = smd_wm_dtr_cb;
  param.enable_dte_ready_event_ext.cb_data = (void *) info;

  smd_ioctl( port_id, SMD_IOCTL_ENABLE_DTR_EVENT_EXT, &param );

  if( info->xfrflow_type == SMD_PKTXFR_TYPE )
  {
    param.full_bufs_asserted = TRUE;
    smd_ioctl( port_id, SMD_IOCTL_SET_FULL_RCV_BUFS, & param );
  }

  return(port_id);
}/* smd_wm_open_3 */

/*===========================================================================
FUNCTION      smd_wm_close

DESCRIPTION   Close a shared data memory port.

              The callback is called when the port has been closed. The port
              may be reopened in the close callback routine. The port may not
              be reused until the callback has been called.

ARGUMENTS     port_id - SMD port to close

              close_cb - callback that is called when the port is actually
                         closed.

              cb_data - user data passed to the close callback routine.

              flush - flush the port before closing. The close callback will
                      be called when all of the pending data has been received
                      by the other processor.

RETURN VALUE  None.

SIDE EFFECTS  Called in client context
===========================================================================*/
void smd_wm_close
(
  smd_port_id_type      port_id,
  smd_close_callback_fn close_cb,
  void                  *cb_data,
  boolean               flush
)
{
  /* FIX ME - If there is a close pending then we should prevent a second
     close from being initiated.
  */

  smd_wm_info_type *info;

  info = smd_wm_get_info( port_id );

  info->close_cb = close_cb;
  info->close_cb_data = cb_data;

  smd_close( port_id, smd_wm_close_cb, (void *)info, flush );

} /* smd_wm_close */

/*===========================================================================
FUNCTION      smd_wm_ioctl

DESCRIPTION   This function provides general control operations for the
              give port.

ARGUMENTS     port_id - SMD port to control

              cmd - control operation

              param - data for control operation

RETURN VALUE  None.

SIDE EFFECTS  Called in client context
===========================================================================*/
void smd_wm_ioctl
(
  smd_port_id_type      port_id,
  smd_ioctl_cmd_type    cmd,
  smd_ioctl_param_type *param
)
{
  smd_wm_info_type *info;
  dsm_watermark_type *tx_queue;
  boolean last_wait;

  info = smd_wm_get_info( port_id );

  switch( cmd )
  {
    case SMD_IOCTL_ENABLE_DTR_EVENT:/* Register DTR-Toggle callback function */
      info->dtr_callback_fn = param->enable_dte_ready_event;
      ASSERT(info->dtr_callback_fn);
      if (info->dtr_callback_fn)
      {
        info->dtr_callback_fn();
      }
      break;

    case SMD_IOCTL_DISABLE_DTR_EVENT:/*Deregister the DTR-Toggle callback fn*/
      info->dtr_callback_fn = NULL;
      break;

    case SMD_IOCTL_FLUSH_TX:
      if( param->record_flush_func_ptr != NULL )
      {
        info->flush_cb = param->record_flush_func_ptr;
        smd_flush_tx( port_id, smd_wm_flush_cb, (void *)info );
      }
      else
      {
        smd_flush_tx( port_id, NULL, NULL );
      }
      break;

    case SMD_IOCTL_GET_PACKET_SIZE:
      param->packet_size = info->max_packet_size;
      break;

    case SMD_IOCTL_SET_PACKET_SIZE:
      info->max_packet_size = param->packet_size;
      break;

    case SMD_IOCTL_GET_XFR_MODE:
      param->dataxfr_mode = info->dataxfr_mode;
      break;

    case SMD_IOCTL_SET_XFR_MODE:
      info->dataxfr_mode = param->dataxfr_mode;
      smd_ioctl( port_id, cmd, param );
      /* FIXME - need to change callbacks, too */
      ERR_FATAL( "SET_XFR_MODE not completely implemented", 0, 0, 0 );
      break;

    case SMD_IOCTL_GET_XFR_TYPE:
      param->xfrflow_type = info->xfrflow_type;
      break;

    case SMD_IOCTL_SET_XFR_TYPE:
      info->xfrflow_type = param->xfrflow_type;
      smd_ioctl( port_id, cmd, param );
      break;

    case SMD_IOCTL_CHANGE_MODE:
      SMD_DSM_LOCK();

      tx_queue = info->tx_queue;
      info->tx_queue = (dsm_watermark_type *)param->tx_buffer;

      if( tx_queue != NULL && tx_queue->current_cnt > 0 )
      {
        ERR("We are erasing a non-empty watermark %x", tx_queue, 0, 0);
        dsm_empty_queue( tx_queue );
      }

      SMD_DSM_UNLOCK();
      break;

    case SMD_IOCTL_ENABLE_DTR_EVENT_EXT:
      /* these two values are set in client context and read in
         smd_task context */
      SMD_DTR_LOCK();
      info->dtr_callback_ext_fn = param->enable_dte_ready_event_ext.cb_func;
      info->dtr_callback_ext_data = param->enable_dte_ready_event_ext.cb_data;
      SMD_DTR_UNLOCK();

      ASSERT(info->dtr_callback_ext_fn);
      if (info->dtr_callback_ext_fn)
      {
        (param->enable_dte_ready_event_ext.cb_func)(
            param->enable_dte_ready_event_ext.cb_data);
      }
      break;

    case SMD_IOCTL_DISABLE_DTR_EVENT_EXT:
      /* these two values are set in client context and read in
         smd_task context */
      SMD_DTR_LOCK();
      info->dtr_callback_ext_fn = NULL;
      info->dtr_callback_ext_data = 0;
      SMD_DTR_UNLOCK();
      break;

    case SMD_IOCTL_ABORT_TX:
    case SMD_IOCTL_ABORT_RX:
      ERR_FATAL("SMD_IOCTL_ABORT not supported from smd_wm",0,0,0);
      break;

    case SMD_IOCTL_WM_SET_WAIT:
      last_wait = info->wait;
      info->wait = param->wait ? TRUE : FALSE;
      if((param->wait == FALSE)&&(last_wait == TRUE))
      { /* send any waiting data when the flag goes low */
        smd_notify_write(port_id);
      }
      break;
    case SMD_IOCTL_WM_GET_WAIT:
      param->wait = info->wait;
      break;
    default:
      smd_ioctl( port_id, cmd, param );
      break;
  }
} /* smd_wm_ioctl */

/*===========================================================================
FUNCTION      smd_dsm_notify_write

DESCRIPTION   notifies the SMD task that TX data is pending

ARGUMENTS     port_id - the port id

RETURN VALUE  

SIDE EFFECTS  Signal will not be set if TX data aggregation is 
              active.  Called in client context.
===========================================================================*/
void smd_dsm_notify_write
(
  smd_port_id_type port_id
)
{
  smd_wm_info_type *info;

  info = smd_wm_get_info( port_id );

  if( !info->wait )
  {
    smd_notify_write(port_id);
  }
}
