/******************************************************************************
  @file  modules/smd/src/smd_packet_memcpy.c
  @brief Shared Memory Driver Packet Memcpy Source File

  DESCRIPTION
  Shared Memory Driver Packet Memcpy Source File

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2007 - 2013 Qualcomm Technologies Incorporated.
  -----------------------------------------------------------------------------
******************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_packet_memcpy.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/13   bt      Make SMD use secure SMEM partitions.
03/05/13   bt      Clean up API-specific structures during stream_init.
01/25/11   rs      + Error fatal when receiving packet with no payload 
                   + Do not transmit 0 sized packets
11/19/09   sa      warning cleanup, sync-up with symbian
05/05/09   bfc     Acounted for new SMD callback signatures in packet layer.
06/10/08   sa      Smd_target.h cleanup. 
08/02/07   bfc     Added a state machine and general cleanup.
03/08/07   bfc     Initial creation

===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_os.h"
#include "smd.h"
#include "smd_internal.h"
#include "smd_packet.h"
#include "smd_os_common.h"

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
#define SMD_PACKET_MEMCPY_RX_READY      0
#define SMD_PACKET_MEMCPY_RX_IN_PACKET  1
#define SMD_PACKET_MEMCPY_RX_DROP       2
#define SMD_PACKET_MEMCPY_RX_SKIP       3

#define SMD_PACKET_MEMCPY_TX_READY      0
#define SMD_PACKET_MEMCPY_IN_TX_PACKET  1

/*===========================================================================
                     LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_packet_memcpy_rx_drop

DESCRIPTION   Responsible for dropping packet data that will not fit into
              a client buffer.

ARGUMENTS     pi - The smd_memcpy_packet_info_type info

              bytes_read - the number of bytes read into the previous buffer,
                           0 means there was not a previous buffer.

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.
              The stream must be set up for full rcv bufs

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_packet_memcpy_rx_drop
(
  smd_memcpy_packet_info_type *pi,
  uint32                       bytes_read,
  byte                       **buf_ptr,
  uint32                      *size
)
{
  ASSERT((pi->rx_bytes_remaining - bytes_read) <= pi->rx_bytes_remaining);

  pi->rx_bytes_remaining -= bytes_read;

  if( pi->rx_bytes_remaining )
  {
    /* there is more data to drop */
    *buf_ptr = pi->rx_scratch_buf;

    *size = MIN( SMD_PACKET_SCRATCH_SZ, pi->rx_bytes_remaining );
  }
  else
  {
    /* truncation is finished, move on to the next packet */
    *buf_ptr = NULL;
    *size = 0;
  }
}/* smd_packet_memcpy_rx_drop */

/*===========================================================================
FUNCTION      smd_packet_memcpy_rx

DESCRIPTION   This routine is called from SMD when it needs a buffer for
              incoming data either because it does not have a buffer or it
              is done with the one it has.

              If bytes_read is 0, SMD did not have a previous buffer,
              otherwise, bytes_read contains the number of bytes that were
              actually transferred into the previous buffer.

              This routine first gets a packet header buffer from SMD, then 
              using the header information, it will use the client supplied
              rx_callback_fn function to get a client buffer for SMD to copy
              into.
              
              It is the client's responsibility to ensure that the packet
              payload will fit in the receive buffer that they supply. 
              Any data that does not fit in the receive buffer will be
              dropped.

ARGUMENTS     data - SMD port needing a read buffer

              bytes_read - the number of bytes read into the previous buffer,
                           0 means there was not a previous buffer.

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.
              The stream must be set up for full rcv bufs

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_packet_memcpy_rx
( 
  void              *data,
  uint32             bytes_read,
  byte             **buf_ptr,
  uint32            *size
)
{
  smd_stream_info_type *info;
  smd_memcpy_packet_info_type *pi;

  info = (smd_stream_info_type *)data;
  pi = &(info->packet.memcpy);

  switch(pi->rx_state)
  {
    case SMD_PACKET_MEMCPY_RX_READY:
      if ( bytes_read == 0 )
      {
        /* nead to read packet header first */
        *buf_ptr = (byte *)pi->rx_packet_header;
        *size = pi->packet_header_size;
      }
      else
      {
        pi->rx_bytes_remaining = SMD_GET_PACKET_SIZE(pi->rx_packet_header);

        /* Check that payload is greater than zero  */
        if( 0 == pi->rx_bytes_remaining )
        {
          /* Fatal error when receiving 0 sized packet */
          ERR_FATAL( "smd_packet_memcpy_rx: Received 0 sized packet in port"
                     " 0x%08x buffer 0x%x", 
                     info->port_id, *buf_ptr, 0 );
        }

        if( pi->rx_buf_ptr == NULL )
        {
		  /* pass back the size of the next packet to the client
			 to provide a buffer large enough to hold it, without
			 this the client has to keep retrying or provide a buffer
			 to hold the maximum size, which in cases like diag can
			 be very wasteful and ineffecient
		  */
          pi->rx_buf_sz = pi->rx_bytes_remaining;
          (pi->rx_callback_fn)( pi->rx_callback_data, 0, 
                                &pi->rx_buf_ptr, &pi->rx_buf_sz);
        }

        *buf_ptr = pi->rx_buf_ptr;

        *size = MIN( pi->rx_buf_sz, pi->rx_bytes_remaining );

        pi->rx_state = SMD_PACKET_MEMCPY_RX_IN_PACKET;
      }
      break;

    case SMD_PACKET_MEMCPY_RX_IN_PACKET:
      if ( bytes_read == 0 )
      {
        /* The client did not give a buffer previously */
        (pi->rx_callback_fn)(pi->rx_callback_data, bytes_read,
                             &pi->rx_buf_ptr, &pi->rx_buf_sz);

        *buf_ptr = pi->rx_buf_ptr;

        *size = MIN( pi->rx_buf_sz, pi->rx_bytes_remaining );
      }
      else
      {
        pi->rx_bytes_remaining -= bytes_read;

        (pi->rx_callback_fn)(pi->rx_callback_data, bytes_read,
                             &pi->rx_buf_ptr, &pi->rx_buf_sz);

        if( pi->rx_bytes_remaining )
        {
          /* no room for the rest of the data */
          pi->rx_state = SMD_PACKET_MEMCPY_RX_DROP;
          smd_packet_memcpy_rx( data, 0, buf_ptr, size );
        }
        else
        {
          /* get the next packet header */
          *buf_ptr = (byte *)pi->rx_packet_header;
          *size = pi->packet_header_size;
          pi->rx_state = SMD_PACKET_MEMCPY_RX_READY;
        }
      }
      break;

    case SMD_PACKET_MEMCPY_RX_DROP:
      smd_packet_memcpy_rx_drop( pi, bytes_read, buf_ptr, size );
      if( *buf_ptr == NULL )
      {
        /* We have finished dropping data...start reading the next packet */
        pi->rx_state = SMD_PACKET_MEMCPY_RX_READY;
        smd_packet_memcpy_rx( data, 0, buf_ptr, size );
      }
      break;

    case SMD_PACKET_MEMCPY_RX_SKIP:
      /* Ran out of memory without flow control...
         skipping the current packet */
      *buf_ptr = NULL;
      *size = 0;
      break;

    default:
      ERR_FATAL("smd_packet_memcpy_rx bad state",0,0,0);
  };
}/* smd_packet_memcpy_rx */

/*===========================================================================
FUNCTION      smd_packet_memcpy_tx

DESCRIPTION   This routine is called from SMD when it needs a new buffer to
              send either because it does not have a buffer or because it is
              done with the previous buffer.

              When SMD is done with a previous buffer then it will call the
              client for a new buffer and size.  It is the responsibility
              of the client to free their previous buffer (if any).

              This layer will send packet information, consisting of the 
              size of the client buffer and some padding,  prior to passing 
              the client transmit buffer to SMD.

ARGUMENTS     data - SMD port needing transmit data

              buf_ptr - the next transmit buffer (output)

              size - the number of bytes in the transmit buffer (output).

DEPENDENCIES  The stream must have been opened.
              It is assumed that SMD will transmit the buffers completely
              before calling back to this function.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_packet_memcpy_tx
(
  void              *data,
  byte             **buf_ptr,
  uint32            *size
)
{
  smd_stream_info_type *info;
  smd_memcpy_packet_info_type *pi;

  info = (smd_stream_info_type *)data;
  pi = &(info->packet.memcpy);


  switch( pi->tx_state)
  {
    case SMD_PACKET_MEMCPY_TX_READY:
      /* get the next transmit buffer from client*/
      (pi->tx_callback_fn)( pi->tx_callback_data, &pi->tx_buf,
                           &pi->tx_size );

      /* Check that packet contains data */
      if( pi->tx_buf && pi->tx_size)
      {
        /* There is data to transmit, send the packet header first */
        SMD_SET_PACKET_SIZE( pi->tx_packet_header, pi->tx_size );

        *buf_ptr = (byte *)pi->tx_packet_header;
        *size = pi->packet_header_size;

        pi->tx_state = SMD_PACKET_MEMCPY_IN_TX_PACKET;
      }
      else
      {
        /* no data to transmit */
        *buf_ptr = NULL;
        *size = 0;
      }
      break;

    case SMD_PACKET_MEMCPY_IN_TX_PACKET:
      /* give the client buffer to SMD */
      *buf_ptr = pi->tx_buf;
      *size = pi->tx_size;

      pi->tx_state = SMD_PACKET_MEMCPY_TX_READY;

      break;
    default:
      ERR_FATAL("smd_packet_memcpy_tx bad state",0,0,0);
  }
}/* smd_packet_memcpy_tx */


/*===========================================================================
FUNCTION      smd_packet_memcpy_flowctl

DESCRIPTION   This routine is called from SMD when it did not get a buffer
              and flow control is not enabled. Flow control disabled means
              throw away data if there's no where to put it. In order to avoid
              losing packet sync, we mark the packet to be skipped and we only
              skip what is required to get to the next packet header.

ARGUMENTS     data - SMD port needing to skip data

              byte_count - the number of bytes currently available in the
              stream

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  The number of bytes to skip.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static uint32 smd_packet_memcpy_flowctl
(
  void   *data,
  uint32  byte_count
)
{
  smd_stream_info_type *info;
  smd_memcpy_packet_info_type *pi;

  info = (smd_stream_info_type *)data;
  pi = &(info->packet.memcpy);

  if( pi->rx_bytes_remaining <= byte_count )
  {
    byte_count = pi->rx_bytes_remaining;
    pi->rx_state = SMD_PACKET_MEMCPY_RX_READY;
  }
  else
  {
    pi->rx_bytes_remaining -= byte_count;
  }

  return byte_count;
} /* smd_packet_memcpy_flowctl */

/*===========================================================================
FUNCTION      smd_packet_memcpy_reset

DESCRIPTION   Resets the packet-mode-specific structure fields for a newly
              opened port, to discard any stale data from when the port was
              (potentially) previously open.

ARGUMENTS     cb_data - the packet info struct, previously registered.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_packet_memcpy_reset(void *cb_data)
{
  smd_memcpy_packet_info_type *pi = (smd_memcpy_packet_info_type *)cb_data;
  
  pi->tx_state = SMD_PACKET_MEMCPY_TX_READY;
  pi->tx_buf = NULL;
  pi->tx_size = 0;
  
  pi->rx_state = SMD_PACKET_MEMCPY_RX_READY;
  pi->rx_buf_ptr = NULL;
  pi->rx_buf_sz = 0;
  pi->rx_bytes_remaining = 0;
}

/*===========================================================================
                     EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_packet_memcpy_status_change

DESCRIPTION   This routine is called to insert or remove the primative
              packet layer when the SMD_IOCTL_SET_PACKET_MODE ioctl is
              invoked.

ARGUMENTS     info - stream_info ptr

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  SMD_UNAVAIL - if smd_memcpy is not supported
              SMD_DONE - if smd_memcpy is support

SIDE EFFECTS  Called in SMD context
===========================================================================*/
smd_api_return_type smd_packet_memcpy_status_change
( 
  smd_stream_info_type *info,
  byte *rx_scratch_buf
)
{
  smd_memcpy_packet_info_type *pi;
  smd_memcpy_stream_info_type *mi;
  smd_ioctl_param_type         param;
  uint32                      *tx_packet_header_ptr;
  uint32                      *rx_packet_header_ptr;

  pi = &(info->packet.memcpy);
  mi = &(info->mode.memcpy);

  if( info->xfrflow == SMD_PKTXFR_TYPE )
  {
    /* Allocate RX/TX packet headers */
    tx_packet_header_ptr = ( uint32 * )smd_malloc( SMD_PACKET_HEADER_BYTE_SIZE );
    rx_packet_header_ptr = ( uint32 * )smd_malloc( SMD_PACKET_HEADER_BYTE_SIZE );
    
    if( !( tx_packet_header_ptr ) || !( rx_packet_header_ptr ) )
    {
      ERR_FATAL("%s: Could not allocate RX/TX packet headers.", 
                __FUNCTION__, 0, 0 );
    }
    
    /* insert the packet layer between the SMD layer and the client */
    pi->tx_callback_fn = mi->tx_callback_fn;
    pi->tx_callback_data = mi->tx_callback_data;
    pi->rx_callback_fn = mi->rx_callback_fn;
    pi->rx_callback_data = mi->rx_callback_data;
    pi->saved_flowctl_fn = mi->rx_flowctl_fn;
    pi->rx_flowctl_data = mi->rx_flowctl_data;

    /* set up some local values */
    pi->rx_packet_header = rx_packet_header_ptr;
    pi->tx_packet_header = tx_packet_header_ptr;
    pi->rx_scratch_buf = rx_scratch_buf;
    pi->packet_header_size = SMD_PACKET_HEADER_BYTE_SIZE;

    /* redirect the SMD callbacks to this layer */
    mi->tx_callback_fn = smd_packet_memcpy_tx;
    mi->tx_callback_data = info;
    mi->rx_callback_fn = smd_packet_memcpy_rx;
    mi->rx_callback_data = info;
    mi->rx_flowctl_fn = smd_packet_memcpy_flowctl;
    mi->rx_flowctl_data = info;
    
     /* Register the Stream Reset callback with memcpy layer */
    param.memcpy_reset.cb_func = smd_packet_memcpy_reset;
    param.memcpy_reset.cb_data = (void*)pi;
    smd_ioctl(info->port_id, SMD_IOCTL_REGISTER_MEMCPY_RESET, &param);
  }
  else if( info->xfrflow == SMD_STREAMING_TYPE )
  {
    /* remove the packet layer */
    mi->tx_callback_fn = pi->tx_callback_fn;
    mi->tx_callback_data = pi->tx_callback_data;
    mi->rx_callback_fn = pi->rx_callback_fn;
    mi->rx_callback_data = pi->rx_callback_data;
    mi->rx_flowctl_fn = pi->saved_flowctl_fn;
    mi->rx_flowctl_data = pi->rx_flowctl_data;
    
    (void)smd_free(pi->rx_packet_header);
    (void)smd_free(pi->tx_packet_header);

    /* clean the struct data */
    memset(pi, 0, sizeof(smd_memcpy_packet_info_type));
  }

  return SMD_DONE;
}
