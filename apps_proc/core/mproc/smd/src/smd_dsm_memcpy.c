/*===========================================================================

        Shared Memory Driver Watermark Memcpy Source File


 Copyright (c) 2005 - 2013 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_dsm_memcpy.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/15   db      Added SMD tx/rx packet count for debug purpose    
10/10/13   bt      For streaming mode, clear Rx items ptrs after each read, to
                   avoid freeing that item from smd_wm_reset_cb.
10/04/13   ss      Ensure a non-zero sized DSM allocation request
09/25/13   bt      Change SMD_WM_GET_INFO macro to smd_wm_get_info with checks.
07/24/13   bt      Make SMD use secure SMEM partitions.
05/29/12   pa      Save previous packet header for debugging.
01/06/12   bt      Missing header causes warning->error on 8974.
07/20/11   hwu     Requested buffer based on data left in receive FIFO for 
                   streaming mode. 
02/11/11   bt      Removed old unused SMD profiling code.
12/10/10   rs      Cleaned up compiler warnings 
09/17/10   bt      Added default DSM size if not defined
09/02/10   tl/ih   Fixed DSM size request at beginning of large packet
07/21/10   tl      Implemented SMD API for SMD priority.
06/30/10   tl      Remove unused functions left over from earlier rewrite
05/03/10   tl      Use critical sections instead of INTLOCK
03/30/10   bfc/tl  Scale the assumed size of the pending data on STREAMING
01/11/10   hwu     change to use fifo_sz in smd_wm_memcpy_open_sz. 
12/10/09   hwu     fix 0 lengh packet header issue
10/12/09   bfc/rs  Removed control port muxing.
09/29/09   bfc/rs  Added variable sized FIFO API support.
02/19/09   bfc     New DSM API.
12/02/08   bfc     Added data stacking support.
11/26/08   bfc     Reverted to the old DSM APIs for now.
09/25/08   jlk     New DSM api
06/10/08   sa      Smd_target.h cleanup. 
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
10/25/07   hwu     Changed to use client callback data for client callback
                   functions. 
10/19/07   hwu     Added smd_wm_memcpy_open_2.
10/15/07   jlk     added SMD_PROFILE hooks
07/23/07   ptm     Code optimizations.
07/18/07   hwu     Changed to use smd_target.h for common headers.
02/07/07   ptm     Add code to support DSM priority field.
01/18/07   hwu     Included err.h to compile with ERR_FATAL in WinMob.
01/15/07   bfc     Fixed a packet mode issue
01/04/06   bfc     initial creation - derived from smd_wm.c

===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_os.h"

#include "queue.h"
#include "dsm.h"

#include "smd.h"
#include "smd_internal.h"
#include "smd_os_common.h"
#include "smd_dsm.h"
#include "smd_dsmi.h"
#include "smd_memcpy.h"
#include "dsmutil.h"

/*===========================================================================
                     GLOBAL VARIBAL DEFINITIONS
===========================================================================*/

/*===========================================================================
                     LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_wm_memcpy_tx_buf

DESCRIPTION   This routine is called from SMD when it needs a new buffer to
              send either because it does not have a buffer or because it is
              done with the previous buffer.

              If SMD is done with a previous buffer, then the associated DSM
              item is freed.

              This routine sends a DSM chain one item at a time. A pointer to
              the item currently being sent is saved in tx_item_ptr while a
              pointer to the remaining items is saved in tx_item_ptr_next.

              Once an entire DSM chain has been sent, another chain is
              obtained from the tx_queue.

              If this stream is in packet mode, then before each DSM chain
              is sent, SMD is given a header packet and a pointer the DSM
              chain associated with the packet is saved in tx_item_ptr_next.

ARGUMENTS     cb_data - client provided cb_data pointer, a pointer to smd_wm_info

              buf_ptr - the next transmit buffer (output)

              size - the number of bytes in the transmit buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_memcpy_tx_buf
(
  void              *cb_data,
  byte             **buf_ptr,
  uint32            *size
)
{
  smd_port_id_type  port_id = (smd_port_id_type)cb_data;
  dsm_item_type    *item = NULL;
  smd_wm_info_type *info;

  info = smd_wm_get_info( port_id );

  /* Done with previous buffer (if any) so free it */
  if( info->tx_item_ptr != NULL )
  {
    dsm_free_packet( &info->tx_item_ptr );
  }

  if(info->wait)
  {
    /* if wait is set and there is not too much data queued, just
       return NULL */
    *buf_ptr = NULL;
    *size = 0;
    return;
  }
  
  /* More in the current DSM chain - go on to next item */
  if( info->tx_item_ptr_next != NULL )
  {
    /* Send next buffer from current chain */
    info->tx_item_ptr = info->tx_item_ptr_next;
    info->tx_item_ptr_next = info->tx_item_ptr->pkt_ptr;
    info->tx_item_ptr->pkt_ptr = NULL;

    *buf_ptr = info->tx_item_ptr->data_ptr;
    *size = info->tx_item_ptr->used;
    return;
  }
  
  /* This operation needs to be in a critical section because
     the client can free info->tx_queue in a different task context
     using ioctl SMD_IOCTL_CHANGE_MODE. */
  SMD_DSM_LOCK();
  /* Go on to next packet, if any */
  if( info->tx_queue != NULL )
  {
    item = dsm_dequeue( info->tx_queue );
  }
  SMD_DSM_UNLOCK();

  if( item == NULL )
  {
    *buf_ptr = NULL;
    *size = 0;
    return;
  }

  if( info->xfrflow_type == SMD_PKTXFR_TYPE )
  {
    /* update debug info by increasing tx packet count */
	info->smd_wm_debug_tx_pkt_cnt++;
	
    /* Starting new packet, send header first */
    *buf_ptr = (byte *) info->tx_packet_header;
    *size = SMD_WM_PACKET_HEADER_BYTE_SIZE;
    SMD_SET_PACKET_SIZE( info->tx_packet_header, dsm_length_packet(item) );
    SMD_SET_APP_FIELD( info->tx_packet_header, item->app_field );
    SMD_SET_APP_PTR( info->tx_packet_header, item->app_ptr );
    SMD_SET_KIND( info->tx_packet_header, item->kind );
#ifdef DSM_PRIORITY
    SMD_SET_PRIORITY( info->tx_packet_header, item->priority );
#endif /* DSM_PRIORITY */

    if( (SMD_GET_PACKET_SIZE(info->tx_packet_header)) > 
        info->max_packet_size )
    {
      /* We should never send a packet larger than info->max_packet_size*/
      ERR_FATAL( "Invalid packet size %d",
                 (SMD_GET_PACKET_SIZE(info->tx_packet_header)), 0, 0 );
    }

    /* Setup to start sending data buffers next time */
    info->tx_item_ptr_next = item;
  }
  else if( info->xfrflow_type == SMD_STREAMING_TYPE )
  {
    /* Staring new chain for stream (instead of packet) */
    info->tx_item_ptr = item;
    info->tx_item_ptr_next = info->tx_item_ptr->pkt_ptr;
    info->tx_item_ptr->pkt_ptr = NULL;
  
    *buf_ptr = info->tx_item_ptr->data_ptr;
    *size = info->tx_item_ptr->used;
  }
  return;
} /* smd_wm_memcpy_tx_buf */

/*===========================================================================
FUNCTION      smd_wm_memcpy_rx_header

DESCRIPTION   This routine sets up read operation for reading a packet header.
              The rx buf pointers in info are set to NULL to indicate it is a
              header operation. The rx_packet_header buffer in info is used for
              the read buffer.

ARGUMENTS     info - pointer to stream info structure

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_memcpy_rx_header
(
  smd_wm_info_type *info,
  byte **buf_ptr,
  uint32 *size
)
{
  /* Save previous packet header for debugging, if valid */
  if (info->rx_header_valid)
  {
    smd_mem_copy(info->rx_packet_header_prev, 
                 SMD_WM_PACKET_HEADER_BYTE_SIZE,
                 info->rx_packet_header, 
                 SMD_WM_PACKET_HEADER_BYTE_SIZE);
  }

  /* Read packet header */
  info->rx_head_ptr = NULL;
  info->rx_tail_ptr = NULL;
  info->rx_header_valid = FALSE;
  info->bytes_remaining = SMD_WM_PACKET_HEADER_BYTE_SIZE;

  *buf_ptr = (byte *)info->rx_packet_header;
  *size = SMD_WM_PACKET_HEADER_BYTE_SIZE;
} /* smd_wm_memcpy_rx_header */

/*==========================================================================
FUNCTION      smd_wm_memcpy_rx_first

DESCRIPTION   This routine sets up to read the first DSM item in a packet or
              any DSM item in a stream.

ARGUMENTS     info - pointer to stream info structure

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_memcpy_rx_first
(
  smd_wm_info_type *info,
  byte **buf_ptr,
  uint32 *size
)
{
  dsm_item_type *item;
  uint32 packet_size;
  uint32 size_req;

  if(info->xfrflow_type == SMD_PKTXFR_TYPE)
  {
    packet_size = SMD_GET_PACKET_SIZE(info->rx_packet_header);
    size_req = packet_size;
    if( ( packet_size > info->max_packet_size ) || ( packet_size == 0 ) )
    {
      /* FIXME - the other processor has given us a bogus packet size,
         we want to reset the other processor - not possible at this
         time, so err_fatal. */
      ERR_FATAL( "Invalid packet size %d", packet_size, 0, 0 );
      /* This is the correct code to go after we reset the other processor */
      smd_wm_memcpy_rx_header( info, buf_ptr, size );
      return;
    }
  }
  else
  { 
    info->rx_head_ptr = NULL;
    info->rx_tail_ptr = NULL;
    /* streaming mode: poke to see how many bytes left in the fifo*/
    size_req = smd_rx_peek(info->port_id);
    packet_size = 0xFFFFFFFF;
  }

  info->bytes_remaining = packet_size;

  if (size_req > 0) 
  {
     item = dsm_new_buffer_flexible_by_pool_id(info->dsm_pool_id,
                                            size_req,
                                            TRUE,
                                            DSM_ITEM_SUBSYSTEM_SMD);

     if( item == NULL )
     {
       *buf_ptr = NULL;
       *size = 0;
       return;
     }
  }
  else
  {
    *buf_ptr = NULL;
    *size = 0;
    return;
  }

  /* First DSM item, set packet fields of DSM header */
  item->app_field = SMD_GET_APP_FIELD(info->rx_packet_header);
  item->app_ptr = (void *) SMD_GET_APP_PTR(info->rx_packet_header);
  item->kind = (uint8) SMD_GET_KIND(info->rx_packet_header);
#ifdef DSM_PRIORITY
  item->priority = (uint8) SMD_GET_PRIORITY(info->rx_packet_header);
#endif /* DSM_PRIORITY */

  info->rx_head_ptr = item;
  info->rx_tail_ptr = item;

  *buf_ptr = info->rx_tail_ptr->data_ptr;
  *size = MIN(packet_size, ((uint32) item->size));
} /* smd_wm_memcpy_rx_first */

/*===========================================================================
FUNCTION      smd_wm_memcpy_rx_next

DESCRIPTION   This routine sets up to read the next DSM item in a packet.

ARGUMENTS     info - pointer to stream info structure

              bytes_remaining - number of bytes remaining in packet

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_memcpy_rx_next
(
  smd_wm_info_type *info,
  uint32 bytes_remaining,
  byte **buf_ptr,
  uint32 *size
)
{
  dsm_item_type *item;
  uint32 size_req;

  if (info->xfrflow_type == SMD_PKTXFR_TYPE)
  {
    size_req = bytes_remaining;
  }
  else
  {
    size_req = smd_rx_peek(info->port_id);
  }

  if (size_req > 0)
  {  
     item = dsm_new_buffer_flexible_by_pool_id(info->dsm_pool_id,
                                            size_req,
                                            TRUE,
                                            DSM_ITEM_SUBSYSTEM_SMD);

     if( item == NULL )
     {
       *buf_ptr = NULL;
       *size = 0;
       return;
     }
   }
   else
   {
       *buf_ptr = NULL;
       *size = 0;
       return;
   }

  info->rx_tail_ptr->pkt_ptr = item;
  info->rx_tail_ptr = item;
  *buf_ptr = item->data_ptr;
  *size = MIN(bytes_remaining, ((uint32) item->size));
  return;
} /* smd_wm_memcpy_rx_next */

/*===========================================================================
FUNCTION      smd_wm_memcpy_rx_buf

DESCRIPTION   This routine is called from SMD when it needs a buffer for
              incoming data either because it does not have a buffer or it
              is done with the one it has.

              If bytes_read is 0, SMD did not have a previous buffer,
              otherwise, bytes_read contains the number of bytes that were
              actually transferred in to the previous buffer.

              If the stream is in packet mode, this routine first gives a
              packet header buffer to SMD, then using the header information,
              it creates a DSM item chain one item at a time as each item is
              filled by SMD. When the entire packet has been read, the read
              call back routine is called with the packet.

              If the stream is in stream mode, this routine gives a buffer
              to SMD and then calls the read call back routine when the buffer
              is filled.

              It is assumed that SMD may not fill the buffers completely.

ARGUMENTS     cb_data - client provided cb_data pointer, a pointer to smd_wm_info

              bytes_read - the number of bytes read into the previous buffer,
                           0 means there was not a previous buffer.

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_wm_memcpy_rx_buf
(
  void              *cb_data,
  uint32             bytes_read,
  byte             **buf_ptr,
  uint32            *size
)
{
  smd_port_id_type  port_id = (smd_port_id_type)cb_data;
  smd_wm_info_type *info;

  info = smd_wm_get_info( port_id );
  
  if( bytes_read == 0 )
  {
    /* No previous buffer */
    if(info->xfrflow_type == SMD_PKTXFR_TYPE)
    {
      if( info->rx_header_valid )
      {
        /* Previously ran out of memory - try again */
        if( info->rx_skip_packet )
        {
          /* Ran out of memory without flow control,
             skipping the current packet */
          *buf_ptr = NULL;
          *size = 0;
        }
        else if( info->rx_head_ptr == NULL )
        {
          /* Ran out of memory on the first buffer */
          smd_wm_memcpy_rx_first( info, buf_ptr, size );
        }
        else
        {
          /* Ran out of memory on the "next" buffer */
          smd_wm_memcpy_rx_next( info, info->bytes_remaining, buf_ptr, size );
        }
      }
      else
      {
        /* No previous buffer, startup next packet (read header) */
        smd_wm_memcpy_rx_header( info, buf_ptr, size );
      }
    }
    else
    {
      /* No previous buffer, startup next buffer */
      smd_wm_memcpy_rx_first( info, buf_ptr, size );
    }
    return;
  }

  /* some bytes were read */
  if(info->xfrflow_type == SMD_PKTXFR_TYPE)
  {
    /* Handle packet mode */
    if( info->rx_tail_ptr == NULL )
    {
      /* Got the header, setup for the first buffer */
      info->rx_header_valid = TRUE;
      smd_wm_memcpy_rx_first( info, buf_ptr, size );
    }
    else
    {
      /* in the process of reading the packet */
      info->rx_tail_ptr->used = (uint16) bytes_read;
      info->bytes_remaining -= bytes_read;

      if( info->bytes_remaining != 0 )
      {
        /* still more data in packet, allocate another buffer */
        smd_wm_memcpy_rx_next( info, info->bytes_remaining, buf_ptr, size );
      }
      else
      {
        /* Debug info: Update packet received count */	  
	    info->smd_wm_debug_rx_pkt_cnt++;
		
        /* Packet is complete */
        if( info->read_cb != NULL ) /* use the cb if it was defined */
        {
          info->read_cb( info->port_id, info->rx_head_ptr, info->read_cb_data );
        }

        else if( info->rx_queue != NULL ) /* use the queue if it was defined */
        {
          dsm_enqueue( info->rx_queue, &info->rx_head_ptr );
        }

        else /* data has no where to go */
        {
          /* Free the dsm packet to avoid a memory leak */
          dsm_free_packet( &info->rx_head_ptr );
        }

        smd_wm_memcpy_rx_header( info, buf_ptr, size );
      }
    }
  }
  else
  {
    /* Handle stream mode */
    ASSERT( info->rx_tail_ptr != NULL ) ;

    info->rx_tail_ptr->used = (uint16) bytes_read;
    
    if( info->read_cb != NULL ) /* use the cb if it was defined */
    {
      info->read_cb( info->port_id, info->rx_head_ptr, info->read_cb_data );
    }

    else if( info->rx_queue != NULL ) /* use the queue if it was defined */
    {
      dsm_enqueue( info->rx_queue, &info->rx_head_ptr );
    }

    else /* data has no where to go */
    {
      /* Free the dsm item to avoid a memory leak */
      dsm_free_packet( &info->rx_head_ptr );
    }
    
    smd_wm_memcpy_rx_first( info, buf_ptr, size );
  }
} /* smd_wm_memcpy_rx_buf */

/*===========================================================================
FUNCTION      smd_wm_memcpy_flowctl

DESCRIPTION   This routine is called from SMD when it did not get a buffer
              and flow control is not enabled. Flow control disabled means
              throw away data if there's no where to put. However, we have
              to be careful if this is a packet mode stream because we don't
              want to lose packet sync.

              For packet mode, we mark the packet as being skipped and
              we only skip what is required to get to the next packet header

              For stream mode, we just skip everything that's pending.

ARGUMENTS     cb_data - client provided cb_data pointer, a pointer to smd_wm_info

              byte_count - the number of bytes currently available in the
              stream

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  The number of bytes to skip.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static uint32 smd_wm_memcpy_flowctl
(
  void     *cb_data,
  uint32    byte_count
)
{
  smd_port_id_type  port_id = (smd_port_id_type)cb_data;
  smd_wm_info_type *info;

  info = smd_wm_get_info( port_id );
  
  if( info->xfrflow_type == SMD_PKTXFR_TYPE )
  {
    /* packet mode - mark packet for skipping */
    info->rx_skip_packet = TRUE;

    if( info->rx_head_ptr != NULL )
    {
      /* Free partial packet read */
      info->rx_tail_ptr = NULL;
      dsm_free_packet( &info->rx_head_ptr );
    }

    if( info->bytes_remaining <= byte_count )
    {
      /* Got the rest of the packet - setup to read the next header */
      info->rx_header_valid = FALSE;
      info->rx_skip_packet  = FALSE;

      byte_count = info->bytes_remaining;
    }
    else
    {
      info->bytes_remaining -= byte_count;
    }
  }
  /* else stream mode - skip all bytes */

  return byte_count;
} /* smd_wm_memcpy_flowctl */

/*===========================================================================
                     EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_wm_memcpy_open_3

DESCRIPTION   This function opens a SMD port for memcpy mode

ARGUMENTS     port_name - The name of the port to be opened
              channel_type - The connection type
              cb_data - A pointer to the client callback data.
              param - SMD memcpy open parameters
              
RETURN VALUE  Returns the port id.
===========================================================================*/
smd_port_id_type smd_wm_memcpy_open_3( const char                *port_name,
                                       smd_channel_type           channel_type,
                                       void                      *cb_data,
                                       const smd_open_param_type *param)
{
  smd_port_id_type port_id;
  
  port_id =  smd_open_memcpy_3( port_name,
                                channel_type,
                                cb_data,
                                smd_wm_memcpy_tx_buf,
                                smd_wm_memcpy_rx_buf,
                                smd_wm_memcpy_flowctl,
                                param );

  return port_id;
}
