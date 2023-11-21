#ifndef SMD_INTERNAL_H
#define SMD_INTERNAL_H

/*===========================================================================

                    Shared Memory Driver Internal Header File


-----------------------------------------------------------------------------
Copyright (c) 2005-2013 Qualcomm Technologies Incorporated
-----------------------------------------------------------------------------
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/14   bm      Add stream_open callback in SMD buffer layer
01/10/14   bc      Modified Context struct. cmd_queue is changed to pointer
                   and cmd_queue_cs is now sharing one (per priority) lock
                   for FEATURE_SMD
12/12/13   rv      SMEM Partition backwards compatibility is removed.
12/09/13   bt      Fix BWcompat table entries for Static Ports.
12/06/13   an      Add cache management support.
10/30/13   bc      Added functions that checks whether processors 
                   in given edge is supported
                   Added Macros representing bit mask for each processor
07/24/13   bt      Make SMD use secure SMEM partitions.
07/02/13   an      optimizations for accessing the shared info.
05/30/13   bt      Add cmd_queue_cs to smd_context_type.
05/10/13   bt      Fix inclusion dependency order for feature defines.
04/03/13   bt      Add smdi_close_channels() to cleanup open ports after SSR, 
                   and open_pending field to handle handshaking for reopen.
02/22/13   sm      + export smd_get_other_host()
                   + Update prototype for smd_send_interrupt()
                   + Added other_host in smd_stream_info_type struct.   
02/14/13   rs      Removed dependency of queue functions for SMDL 
09/05/12   pa      Fixing compile errors due to use of deprecated SMD_RPM.
08/23/12   bt      Add copy_fn to smd_info_type to signify correct copy func.
08/02/12   pa      Change prototype of smdi_get_channel_id to return int so
                   that negative num can indicate port not found.
07/18/12   bt      Add disabled field to smd_interrupt_table. 
06/28/12   bm      Replace smd_host_type with smem_host_type
06/25/12   pa      Minor fix to switch stream_info to void * type for alignment
05/29/12   pa      Add packet_header_prev to stream_info_type.
03/22/12   bm      Define new macros to access SMD channel header struct.
04/05/12   bt      Replace smd_target.h with smd_proc.h, smd_internal.h.
01/23/12   hwu     Removed IMAGE_APPS_PROC.
01/22/12   bm      Added read_indx_internal to smd_lite_stream_info_type to keep
                   track of the read pointer for smdl_read/free_buffered_msgs().
                   + SMD_ADJUST_INDEX macro.
11/30/11   bt      Added event_mutex for locking in SMDL interrupt context.
11/08/11   bm      Added functionality in smd_memcpy_stream_info_type struct
                   to allow for registration of client level Stream Reset cb.
10/31/11   bm      Moved SMD_CHANNEL_NAME_SIZE_MAX to smd_type.h to 
                   expose it as a public macro.
08/23/11   bt      Added smdi_get_loopback_host/server for N-way loopback.
07/25/11   rs      Added support for SSR Phase 2 
07/12/11   bm      Use smd_os_proc/intr_type, abstraced by smd_os.h
06/28/11   tl      Integrated WoA support: Don't include DALIPC on WoA
05/03/11   tl      Include smd.h for types required by this header
4/26/11    bm      Removed dependency on smd_target.h by using flag 
                   FEATURE_MODEM_FIRMWARE instead of SMD_Q6FW_PROC for 
                   proper code inclusion/exclusion.
04/01/11   bm      Updated prototypes for smdi_alloc_channel_info() and 
                   smdi_add_channel_info() which now take smd_xfrflow_type as
                   an argument as well.
04/07/11   rs      Make SMD channel definitions compatible across targets 
03/25/11   hwu     Merge in 8960 change. 
02/14/11   tl      Added data structures for long-message SMD Lite read
02/11/11   bt      Removed old unused SMD profiling code.
02/08/11   tl      Added data structures for long-message SMD Lite write
01/24/11   tl      Made smdl_read_packet_header() available for vector read/
                   write
01/06/11   bt      Removed SMD Block and Data Mover protocols.
12/22/10   bt      Added transfer_mode field to smd_lite_stream_info_type.
11/17/10   rs      Made smd_register_interrupt static to OS abstraction file
10/21/10   rs      Cleaned up redundancy in function calls when sending and 
                   receiving interrupts 
10/04/10   tl      When receiving data, send an interrupt only when necessary
09/08/10   rs      Cleaned up smd_interrupt_info_type
07/21/10   tl      Moved smd_priority_type into public API
06/21/10   rs      SMD Lite V2 8660 target build: 
                     + Added processor field to smd_interrupt_type to support
                       DALIPC
06/08/10   tl      Split SMD task into multiple tasks with different priorities
05/21/10   rs      Added SMD index validation based on FIFO size
05/03/10   tl      Use critical sections instead of INTLOCK
04/21/10   tl      Implemented SMD Lite API
03/11/10   rs      Refactored SMD interrupts support
12/08/09   esh     Added checks to fix Klocwork warnings
12/07/09   rs      CMI updates, featurization removal.
11/19/09   sa      warning cleanup, sync-up with symbian
09/29/09   bfc/rs  Added variable sized FIFO API support.
09/24/09   rs      Fixed typo.
07/13/09   tl      Added CTS/RTS callback
07/08/09   ih      Reworked SMEM/SMD for WM7
05/05/09   bfc     Acounted for new SMD callback signatures in packet layer.
02/12/09   hwu     added smd_get_other_host().
02/04/09    sa     cleanup msg macros
01/26/09   bfc     Added the dataxfr mode to the CID entry.
01/26/09   jlk     removed redundent smd version
08/20/08   bfc     Changed the SMD BLOCK interface.
06/23/08   bfc     Fix for variable sized SMD FIFOs.
06/19/08   bfc     Support variable sized SMD FIFOs.
06/10/08   sa      Smd_target.h cleanup. 
02/04/08   bfc     Added a function header.
02/21/08   bfc     First SMD Block port check in.
01/14/08   bfc     Created a SMD base struct.
01/14/08   bfc     Started adding hooks for SMD block ports.
11/14/07   jlk     bug fix for smd profiling
10/25/07   hwu     Added client callback data for each callback functions.
10/08/07   hwu     Added functions to allocate/delete/update SMD channel
                   allocation table.
10/18/07   bfc     Added tramsmit and receive abort ioctls
10/15/07   jlk     Added smd profiling
08/02/07   bfc     Added a state machine to packet mode.
08/02/07   bfc     Moved packet macros to smd_internal.h.
07/05/07   hwu     Changed to use smd_channel_type. 
06/12/07   bfc     Added a low level packet mode.
05/22/07   bfc     Added new api.
04/26/07   ptm     Add new shared memory channel data structure and prototype
                   for routine to allocate shared memory channels.
02/07/07   ptm     Change SMD_MSM_H to smd_target.h and add new logging events.
01/09/07   bfc     Added SMD using DM
01/04/06   bfc     Broke SMD MEMCPY and DMOV out into components.
12/14/06   hwu     Added the following to support remote processor restart.
                     - SMD_SS_RESET and SMD_SS_RESET_OPENING states.
                     - SMD_EVENT_REMOTE_RESET events
11/29/06   taw     Include SMD_MSM_H and use macros to access shared memory 
                   data.
11/13/06   hwu     removed smd_opening_list and merged it to smd_open_list.
11/01/06   ptm     Add read_pending and transmit_pending flag to fix a race
                   condition that existed between opening a port and then
                   calling smd_notify_read/write before SMD had a chance to
                   process the open. Rename dsm_iov to smd_iov_type.
10/10/06   ptm     Move power collapse code to smd_task.c and rename variable.
10/03/06   bfc     Added capability for the rcv operation to only return full
                   buffers to the buffer layer.
10/03/06   ptm     Rename interrupt related symbols and remove apps wakeup code.
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
09/27/06   bfc     Added an ioctl to support additional functionality while
                   a port is in the Opening state
09/05/06   ptm     Remove references to REX.
09/05/06   ptm     Added max_queued_data field to smd_stream_info_type.
08/22/06   ptm     Merge WinMobile changes to main line.
05/24/06   bfc     Added RI callback functionality
05/08/06   ptm     Change IOCTL lock from rex critical section to INTLOCK.
05/05/06   ptm     Make interface signals synchronous and add pending IOCTL
                   counter.
04/28/06   ptm     Make interface signals asynchronous.
04/28/06   ptm     Added flow control callback.
04/27/06   bfc     Added support for a dtr cb critacal section
03/27/06   bfc     Modified SMD to use the new sio extended funcion callback
                   parameters
01/26/06   bfc     Added fields for the Carrier Detect callback functionality.
01/20/06   ptm     Added support for apps processor power collapse.
08/28/05   ptm     General task cleanup.
06/01/05   adm     Increased the number of SMD streams to 4
05/18/05   ptm     General clean up of data structures.
04/26/05   adm     Merged Phil's SMSM related changes and Tom's RDM related
                   changes
02/07/05   adm     Initial version. 
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smd_os.h"
#ifdef FEATURE_SMD
#include "queue.h"
#endif
#include "smd.h"
#include "smd_lite.h"
#include "smem_v.h"
#include "smd_proc.h"
#include "smem_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        FEATURE DEFINITIONS
===========================================================================*/

#define FEATURE_SMD_ACTIVATE_DEBUG_COUNTERS
/*#define FEATURE_SMD_UNIT_TEST*/

#define SMD_FEATURE_FIFO_CACHING          1

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------------
 *    SMD Protocol Version
 *    For interprocesor communication
 *-------------------------------------------------------------------------*/
#define SMD_PROTOCOL_VERSION 0x00020000

/*-------------------------------------------------------------------------
 *    SMD API Version
 *-------------------------------------------------------------------------*/
#define SMD_API_VERSION          0x0000001e

#define SMEM_LOG_EVENT_CB            (SMEM_LOG_SMEM_EVENT_BASE +  0)
#define SMEM_LOG_EVENT_START         (SMEM_LOG_SMEM_EVENT_BASE +  1)
#define SMEM_LOG_EVENT_INIT          (SMEM_LOG_SMEM_EVENT_BASE +  2)
#define SMEM_LOG_EVENT_RUNNING       (SMEM_LOG_SMEM_EVENT_BASE +  3)
#define SMEM_LOG_EVENT_STOP          (SMEM_LOG_SMEM_EVENT_BASE +  4)
#define SMEM_LOG_EVENT_RESTART       (SMEM_LOG_SMEM_EVENT_BASE +  5)
#define SMEM_LOG_EVENT_SS            (SMEM_LOG_SMEM_EVENT_BASE +  6)
#define SMEM_LOG_EVENT_READ          (SMEM_LOG_SMEM_EVENT_BASE +  7)
#define SMEM_LOG_EVENT_WRITE         (SMEM_LOG_SMEM_EVENT_BASE +  8)
#define SMEM_LOG_EVENT_SIGS1         (SMEM_LOG_SMEM_EVENT_BASE +  9)
#define SMEM_LOG_EVENT_SIGS2         (SMEM_LOG_SMEM_EVENT_BASE + 10)
#define SMEM_LOG_EVENT_ISR           (SMEM_LOG_SMEM_EVENT_BASE + 15)
#define SMEM_LOG_EVENT_TASK          (SMEM_LOG_SMEM_EVENT_BASE + 16)
#define SMEM_LOG_EVENT_RS            (SMEM_LOG_SMEM_EVENT_BASE + 17)

#define SMD_IF_SIG_DTR 0x01
#define SMD_IF_SIG_RTS 0x02
#define SMD_IF_SIG_DSR 0x04
#define SMD_IF_SIG_CTS 0x08
#define SMD_IF_SIG_CD  0x10
#define SMD_IF_SIG_RI  0x20

/* Stream related states */
#define SMD_SS_CLOSED            0x00000000 /* Stream is closed - must be 0 */
#define SMD_SS_OPENING           0x00000001 /* Stream is opening */
#define SMD_SS_OPENED            0x00000002 /* Stream is opened */
#define SMD_SS_FLUSHING          0x00000003 /* Stream is flushing */
#define SMD_SS_CLOSING           0x00000004 /* Stream is closing */
#define SMD_SS_RESET             0x00000005 /* Stream is resetting */
#define SMD_SS_RESET_OPENING     0x00000006 /* Stream reset on local */

/* Stream related events */
#define SMD_EVENT_CLOSE          0x00000000 /* must be 0 */
#define SMD_EVENT_OPEN           0x00000001
#define SMD_EVENT_REMOTE_OPEN    0x00000002
#define SMD_EVENT_REMOTE_CLOSE   0x00000003
#define SMD_EVENT_FLUSH          0x00000004
#define SMD_EVENT_FLUSH_COMPLETE 0x00000005
#define SMD_EVENT_REMOTE_REOPEN  0x00000006
#define SMD_EVENT_REMOTE_RESET   0x00000007

#define SMD_STREAM_OPEN( SI ) \
  ( ( SMD_READ_CH_HDR( SI, SMD_TX_HDR, stream_state) ) != \
    SMD_SS_CLOSED )

#define SMD_STREAM_FLUSHING( SI ) \
  ( ( SMD_READ_CH_HDR( SI, SMD_TX_HDR, stream_state ) ) == \
    SMD_SS_FLUSHING )

/* Compare index to FIFO size */ 
#define SMD_VALIDATE_INDEX( IDX, SIZE ) \
  do { \
    if( ( IDX ) >= ( SIZE ) ) \
    { \
      ERR_FATAL( "Invalid index %x", IDX, 0, 0 ); \
    } \
  } while( 0 )

/* Adjust the index to be within FIFO boundary.
 * The index should be of type int */ 
#define SMD_ADJUST_INDEX( IDX, SIZE ) \
  do { \
    if( (IDX) < 0 ) \
    { \
      (IDX) += (SIZE); \
    } \
    else if( (IDX) >= (SIZE) ) \
    { \
      (IDX) -= (SIZE); \
    } \
  } while( 0 )

/* Translate an SIO port into an index into the table of SMD ports */
#define SMD_PORT_TO_IDX( PID ) ( (PID) - SMD_PORT_FIRST )

#define SMD_CALL_FLUSH_CB(SI) \
  do { \
    if( SI->flush_cb ) \
    { \
      SI->flush_cb( SI->flush_cb_data ); \
    } \
  } while(0)

/** Macros to decode the port_id for easier manipulation. */
#define PORT_GET_IDX( PORT_ID )           \
  ((PORT_ID) & 0x00ff)
#define PORT_GET_REMOTE( PORT_ID )        \
  ((smem_host_type)(((PORT_ID) & 0xff00) >> 8))

#define PORT_SET( PORT_ID, IDX, REMOTE )  \
  ((PORT_ID) = ((((REMOTE) & 0xff) << 8) | ((IDX) & 0xff)))
#define PORT_SET_IDX( PORT_ID, VAL )      \
  ((PORT_ID) = (((PORT_ID) & 0xff00) | ((VAL) & 0xff)))
#define PORT_SET_REMOTE( PORT_ID, VAL )   \
  ((PORT_ID) = (((PORT_ID) & 0x00ff) | (((VAL) & 0xff) << 8)))

/* See if an SMD port is valid. */
#define SMD_VALIDATE_PORT_ID( PORT_ID ) \
  do { \
    if( !smd_is_port_id_valid( PORT_ID ) ) \
    { \
      ERR_FATAL( "Invalid SMD port id, idx:%d, remote_host:%d", \
                  PORT_GET_IDX(PORT_ID), PORT_GET_REMOTE(PORT_ID), 0 ); \
    } \
  } while(0)

/*-------------------------------------------------------------------------
 *        Define the interface signals for streaming ports
 *
 * We use an array of uint8's, with length a multiple of 4, so that we
 * guarantee the alignment and size across various compilers and linkers.
 *
 *-------------------------------------------------------------------------*/
#define SMD_NUM_IF_SIGS     8       /* Must be multiple of 4 for alignment */

#define SMD_IF_SIG_DSR_ID            0
#define SMD_IF_SIG_DTR_ID            0
#define SMD_IF_SIG_CTS_ID            1
#define SMD_IF_SIG_RTS_ID            1
#define SMD_IF_SIG_CD_ID             2
#define SMD_IF_SIG_RI_ID             3
#define SMD_IF_SIG_DATA_WRITTEN_ID   4
#define SMD_IF_SIG_DATA_READ_ID      5
#define SMD_IF_SIG_STATE_UPDATED_ID  6
#define SMD_IF_SIG_MASK_RECV_INTR_ID 7

#define dsr_sig             if_sigs[SMD_IF_SIG_DSR_ID]   /* transmit side */
#define dtr_sig             if_sigs[SMD_IF_SIG_DTR_ID]   /* receive side */

#define cts_sig             if_sigs[SMD_IF_SIG_CTS_ID]   /* transmit side */
#define rts_sig             if_sigs[SMD_IF_SIG_RTS_ID]   /* receive side */

#define cd_sig              if_sigs[SMD_IF_SIG_CD_ID]

#define ri_sig              if_sigs[SMD_IF_SIG_RI_ID]

/* flags to reduce overhead on signal processing */
#define data_written        if_sigs[SMD_IF_SIG_DATA_WRITTEN_ID]
#define data_read           if_sigs[SMD_IF_SIG_DATA_READ_ID]
#define state_updated       if_sigs[SMD_IF_SIG_STATE_UPDATED_ID]

/** Flag specifying the behavior of SMD interrupts when reading data from the
 * SMD fifo.
 *
 * This flag is owned by the transmitter, and is set to 1 when the transmitter
 * doesn't need an interrupt to be sent when the receiver reads data from the
 * port. A value of 0 requests that an interrupt be sent, which is backward-
 * compatible with earlier SMD protocols and implementations. The transmitter
 * should set this flag to 1 when no data is pending that has not already been
 * copied into the SMD fifo, and clear this flag when additional data is
 * pending. 
 *
 * When 0: Send interrupt on receive
 * When 1: Don't send interrupt on receive
 */
#define mask_recv_intr      if_sigs[SMD_IF_SIG_MASK_RECV_INTR_ID]

#if SMD_NUM_IF_SIGS != 8
#error "SMD_NUM_IF_SIGS must match number of signals defined here"
#endif

#define SMD_DTR_LOCK()         smd_dtr_lock( TRUE )
#define SMD_DTR_UNLOCK()       smd_dtr_lock( FALSE )

/* The "critical section" used for keeping track of IOCTL commands */
#define SMD_IOCTL_LOCK()       smd_ioctl_lock( TRUE )
#define SMD_IOCTL_UNLOCK()     smd_ioctl_lock( FALSE )

/* The "critical section" for WM_Q access protection */
#define SMD_DSM_LOCK()         smd_dsm_lock( TRUE )
#define SMD_DSM_UNLOCK()       smd_dsm_lock( FALSE )

/*---------------------------------------------------------------------------
 * Packet mode definitions
 *---------------------------------------------------------------------------*/

/** The number of 32-bit words in the SMD packet header. */
#define SMD_PACKET_HEADER_SIZE                    5

/** The number of bytes in the SMD packet header. */
#define SMD_PACKET_HEADER_BYTE_SIZE               (sizeof(uint32) \
                                                  * SMD_PACKET_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * When operating in packet mode, a packet header is sent before the data.
 * The following macros are used to set/get the values from the packet header
 * buffer. If the packet header is modified, the macros below should be
 * updated as required. The buffer should not be directly accessed.
 *---------------------------------------------------------------------------*/
#define SMD_SET_PACKET_SIZE( BUF, VALUE )    (BUF)[0] = (VALUE)
#define SMD_SET_APP_FIELD( BUF, VALUE )      (BUF)[1] = (uint32)(VALUE)
#define SMD_SET_APP_PTR( BUF, VALUE )        (BUF)[2] = (uint32)(VALUE)
#define SMD_SET_KIND( BUF, VALUE )           (BUF)[3] = (uint32)(VALUE)
#define SMD_SET_PRIORITY( BUF, VALUE )       (BUF)[4] = (uint32)(VALUE)

#define SMD_GET_PACKET_SIZE( BUF )           ((BUF)[0])
#define SMD_GET_APP_FIELD( BUF )             ((BUF)[1])
#define SMD_GET_APP_PTR( BUF )               ((BUF)[2])
#define SMD_GET_KIND( BUF )                  ((BUF)[3])
#define SMD_GET_PRIORITY( BUF )              ((BUF)[4])

/* Profiling features */
#undef SMD_PROFILE
/*#define SMD_PROFILE*/
#ifdef SMD_PROFILE
/* the true definitions of the types and prototypes used in */
#include "smd_profile.h"
#else

/*=============================================================================
if profiling is turned off, use dummy functions and typedefs
=============================================================================*/
typedef void smd_profile_type;

#define SMD_PROFILE_ALL_INIT 
#define SMD_PROFILE_INIT(a)                      NULL
#define SMD_PROFILE_INIT2(a)                     NULL
#define SMD_PROFILE_MEMCOPY_UPDATE(a,b,c,d)
#define SMD_PROFILE_WATERMARK_START_PACKET(a,b) 
#define SMD_PROFILE_WATERMARK_END_PACKET(a) 
#define SMD_PROFILE_UPDATE_DSM(a,b)

#endif /*SMD_PROFILE*/

/*===========================================================================
   MACROS for reading/writing interface signals and other header elements
===========================================================================*/


/*===========================================================================
MACRO         SMD_IS_SHARED_INFO_ALIGNED

DESCRIPTION   Determine if an aligned shared info should be used for 
              the stream edge (those having RPM as one of the hosts).

ARGUMENTS     info - stream info strcture (smd_stream_info_type).

RETURN VALUE  TRUE if the edge required the aligned sshared info
===========================================================================*/
#define SMD_IS_SHARED_INFO_ALIGNED(info) (info->is_shared_info_algn == TRUE)

/*===========================================================================
MACRO         SMD_READ_IF_SIG

DESCRIPTION   Read the value of the desired interface signal, taking care of
              RPM edge case.

ARGUMENTS     info - stream info strcture (smd_stream_info_type).

              ch_hdr - the header strcture to modify(SMD_TX_HDR/SMD_RX_HDR)

              signal - macro representing which interface signal to read.

RETURN VALUE  The value of the signal
===========================================================================*/
#define SMD_READ_IF_SIG(info, ch_hdr, signal)  \
    ( SMD_IS_SHARED_INFO_ALIGNED(info) ?  \
      ( (ch_hdr == SMD_TX_HDR) ? \
        SMD_READ_SMEM_4(&(info->tx_shared_info_ptr_algn->signal)) : \
        SMD_READ_SMEM_4(&(info->rx_shared_info_ptr_algn->signal)) \
      ) : \
      ( (ch_hdr == SMD_TX_HDR) ? \
        SMD_READ_SMEM_1(&(info->tx_shared_info_ptr->signal)) : \
        SMD_READ_SMEM_1(&(info->rx_shared_info_ptr->signal)) \
      ) \
    )

/*===========================================================================
MACRO         SMD_WRITE_IF_SIG

DESCRIPTION   Write the value to the desired interface signal, taking care of
              RPM edge case.

ARGUMENTS     info - stream info strcture (smd_stream_info_type).

              ch_hdr - the header strcture to modify(SMD_TX_HDR/SMD_RX_HDR)

              signal - macro representing which interface signal to write.

              val - value to be written.

RETURN VALUE  None
===========================================================================*/
#define SMD_WRITE_IF_SIG(info, ch_hdr, signal, val) do {\
    if(SMD_IS_SHARED_INFO_ALIGNED(info)) { \
      if(ch_hdr == SMD_TX_HDR) { \
        SMD_WRITE_SMEM_4(&(info->tx_shared_info_ptr_algn->signal), val); \
      } else { \
        SMD_WRITE_SMEM_4(&(info->rx_shared_info_ptr_algn->signal), val); \
      } \
    } else { \
      if(ch_hdr == SMD_TX_HDR) { \
        SMD_WRITE_SMEM_1(&(info->tx_shared_info_ptr->signal), val); \
      } else { \
        SMD_WRITE_SMEM_1(&(info->rx_shared_info_ptr->signal), val); \
      } \
    } \
  } while(0);

/*===========================================================================
MACRO         SMD_READ_CH_HDR

DESCRIPTION   Read the specified channel header field, taking care of
              RPM edge case.

ARGUMENTS     info - stream info strcture (smd_stream_info_type).

              ch_hdr - the header strcture to modify(SMD_TX_HDR/SMD_RX_HDR)

              element - literal structure element to read. It should be one of
                        stream_state/read_index/write_index
 
RETURN VALUE  Value of the element.
===========================================================================*/
#define SMD_READ_CH_HDR(info, ch_hdr, element) \
    ((ch_hdr == SMD_TX_HDR) ? \
      ((&(info->tx_shared_info_ptr->element) == &(info->tx_shared_info_ptr->write_index)) ? \
        info->tx_write_index : \
        (SMD_IS_SHARED_INFO_ALIGNED(info) ?  \
          SMD_READ_SMEM_4(&(info->tx_shared_info_ptr_algn->element)) : \
          SMD_READ_SMEM_4(&(info->tx_shared_info_ptr->element)) \
        ) \
      ) : \
      ((&(info->rx_shared_info_ptr->element) == &(info->rx_shared_info_ptr->read_index)) ? \
        info->rx_read_index : \
        (SMD_IS_SHARED_INFO_ALIGNED(info) ?  \
          SMD_READ_SMEM_4(&(info->rx_shared_info_ptr_algn->element)) : \
          SMD_READ_SMEM_4(&(info->rx_shared_info_ptr->element)) \
        ) \
      ) \
    )

/*===========================================================================
MACRO         SMD_WRITE_CH_HDR

DESCRIPTION   Write the given val to specified channel header field, taking 
              care of RPM edge case.

ARGUMENTS     info - stream info strcture (smd_stream_info_type).

              ch_hdr - the header strcture to modify(SMD_TX_HDR/SMD_RX_HDR)

              element - literal structure element to read. It should be one of
                        stream_state/read_index/write_index

              val - value to be written
 
RETURN VALUE  None
===========================================================================*/
#define SMD_WRITE_CH_HDR(info, ch_hdr, element, val) do {\
    if (ch_hdr == SMD_TX_HDR) { \
      if(SMD_IS_SHARED_INFO_ALIGNED(info)) { \
        SMD_WRITE_SMEM_4(&(info->tx_shared_info_ptr_algn->element), val); \
      } else { \
        SMD_WRITE_SMEM_4(&(info->tx_shared_info_ptr->element), val); \
      } \
      if (&(info->tx_shared_info_ptr->element) == &(info->tx_shared_info_ptr->write_index)) { \
        info->tx_write_index = val; \
      } \
    } else { \
      if(SMD_IS_SHARED_INFO_ALIGNED(info)) { \
        SMD_WRITE_SMEM_4(&(info->rx_shared_info_ptr_algn->element), val); \
      } else { \
        SMD_WRITE_SMEM_4(&(info->rx_shared_info_ptr->element), val); \
      } \
      if (&(info->rx_shared_info_ptr->element) == &(info->rx_shared_info_ptr->read_index)) { \
        info->rx_read_index = val; \
      } \
    } \
  } while(0);

/*===========================================================================
MACRO         SMD_CLR_IF_SIGS

DESCRIPTION   Clear the channel header's interface signals(if_sigs[]), taking 
              care of RPM edge case.

ARGUMENTS     info - stream info strcture (smd_stream_info_type).

              ch_hdr - the header strcture to modify(SMD_TX_HDR/SMD_RX_HDR)

RETURN VALUE  None
===========================================================================*/
#define SMD_CLR_IF_SIGS(info, ch_hdr) do { \
    if(SMD_IS_SHARED_INFO_ALIGNED(info)) { \
      if(ch_hdr == SMD_TX_HDR) { \
        SMD_MEMSET_SMEM((void*)info->tx_shared_info_ptr_algn->if_sigs, 0, \
            sizeof(info->tx_shared_info_ptr_algn->if_sigs)); \
      } else { \
        SMD_MEMSET_SMEM((void*)info->rx_shared_info_ptr_algn->if_sigs, 0, \
            sizeof(info->rx_shared_info_ptr_algn->if_sigs)); \
      } \
    } else { \
      if(ch_hdr == SMD_TX_HDR) { \
        SMD_MEMSET_SMEM((void*)info->tx_shared_info_ptr->if_sigs, 0, \
            sizeof(info->tx_shared_info_ptr->if_sigs)); \
      } else { \
        SMD_MEMSET_SMEM((void*)info->rx_shared_info_ptr->if_sigs, 0, \
            sizeof(info->rx_shared_info_ptr->if_sigs)); \
      } \
    } \
  } while(0);

/*===========================================================================
   MACROS for processor position bit mask
===========================================================================*/
/* 
  Supported processors are represented as bits. 

  These bits are used in:
		1. Populating smd_interrupt_table at interrupt initialization
		2. Checking whether processors in given edge are supported
  
  Specific bit position for each processor is represented below 
		
		APPS	= 0x1	(0000 0001)
		MODEM	= 0x2	(0000 0010)
		ADSP	= 0x4	(0000 0100)
		RSVD3	= 0x8	(0000 1000)
		WCNSS	= 0x10	(0001 0000)
		RSVD5	= 0x20	(0010 0000)
		RPM		= 0x40	(0100 0000)
*/

#define SMD_APPS_BIT_MASK	0x1
#define SMD_MODEM_BIT_MASK	0x2
#define SMD_ADSP_BIT_MASK	0x4
#define SMD_RSVD3_BIT_MASK	0x8
#define	SMD_WCNSS_BIT_MASK	0x10
#define SMD_RSVD5_BIT_MASK	0x20
#define SMD_RPM_BIT_MASK	0x40

/*===========================================================================
                              ENUMS
===========================================================================*/

/* SMD channel protocol type */
typedef enum
{
  SMD_STREAMING_BUFFER
} smd_channel_protocol_type;

/* SMD channel header type */
typedef enum
{
  SMD_TX_HDR,
  SMD_RX_HDR
} smd_channel_header_type;

/*===========================================================================
           DEFINE SHARED MEMORY STREAMING DATA CHANNELS
===========================================================================*/
typedef struct
{
  uint32                   stream_state; /* see SMD_SS_... */
  uint8                    if_sigs[SMD_NUM_IF_SIGS];

  uint32                   read_index;
  uint32                   write_index;
} smd_shared_stream_info_type;

/* New SMD channel structure created with all elements word aligned */
typedef struct
{
  uint32                   stream_state; /* see SMD_SS_... */
  uint32                   if_sigs[SMD_NUM_IF_SIGS];

  uint32                   read_index;
  uint32                   write_index;
} smd_shared_stream_info_algn_type;

typedef struct
{
  smd_shared_stream_info_type      proc0_region;
  smd_shared_stream_info_type      proc1_region;
} smd_shared_stream_channel_type;

typedef struct
{
  smd_shared_stream_info_algn_type      proc0_region;
  smd_shared_stream_info_algn_type      proc1_region;
} smd_shared_stream_channel_algn_type;

/*===========================================================================
 *        DEFINE PROCESSOR LOCAL MEMORY DATA STRUCTURES
 *===========================================================================*/

/*-------------------------------------------------------------------------
 *        Define MEMCPY local data structures
 *-------------------------------------------------------------------------*/
typedef struct
{
  /* Transmit complete callback function pointer */
  smd_memcpy_tx_callback_fn  tx_callback_fn;
  void                      *tx_callback_data;
  
  /* Receive complete callback function pointer */
  smd_memcpy_rx_callback_fn  rx_callback_fn;
  void                      *rx_callback_data;
  
  /* Flow control callback function pointer */
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn;
  void                      *rx_flowctl_data;

  /** Stream Reset callback function pointer */
  smd_memcpy_reset_callback_fn  reset_callback_fn;
  void                         *reset_callback_data;

  /* Application transmit buffer */
  byte    *tx_buf_ptr;
  uint32   tx_buf_size;

  /* Application receive buffer */
  byte    *rx_buf_ptr;
  uint32   rx_buf_size;

  /* Bytes read from the current buffer */
  uint32 bytes_remaining;
} smd_memcpy_stream_info_type;

/* The mode of transfer to be used by the SMD Lite channel.
 * Packet and streaming modes are supported. */
typedef enum
{
  SMDL_MODE_PACKET = 0,
  SMDL_MODE_STREAMING,
} smdl_tx_mode_type;

/**
 * SMD Lite internal data structure holding port-specific information.
 */
typedef struct
{
  /** The transfer mode of the smd lite port.
   * 
   * Packet as well as streaming mode is supported.
   */
  smdl_tx_mode_type transfer_mode;
  
  /** The SMD critical section used to protect receive operations.
   *
   * This mutex must be held when performing receive operations on the port,
   * from either the SMD task context or user task context.
   */
  void             *rx_mutex;

  /** The SMD critical section used to protect transmit operations.
   *
   * This mutex must be held when performing transmit operations on the port,
   * from either the SMD task context or user task context.
   */
  void             *tx_mutex;
  
  /** The SMD critical section used to protect structures that could be 
   * modified in interrupt context, or both interrupt and thread context.
   * 
   * This mutex must be held to protect read-modify-write of the port's read 
   * and write indices (like smdl_read_packet_header() and other places where 
   * the read index is modified).  It can also be used for protecting against 
   * any other race conditions for code that may be run from interrupt context.
   */
  void             *event_mutex;

  /** The user callback to be called when the port is opened or closed and
   * when data is read or written. */
  smdl_callback_t   notify;

  /** The user context passed to \c notify. */
  void             *cb_data;

  /** Flag indicating whether to call the write callback.
   *
   * This flag is set in smdl_write() when the user data can not be written to
   * the port because the port is not yet open or because the outbound FIFO is
   * full. When this flag is set, read events from the other processor will
   * generate SMDL_EVENT_WRITE events (see smd_lite_process_write()).
   *
   * This flag is cleared in smdl_write() when a packet was successfully
   * written to the outbound FIFO.
   */
  int32             notify_on_write;

  /** The number of bytes of the packet header that have been read from the
   * FIFO into \c packet_header.
   *
   * When this is equal to SMD_PACKET_HEADER_BYTE_SIZE, a packet may available
   * to be read in the FIFO.
   */
  uint32            packet_header_len;

  /** The packet header of the next packet to be received.
   *
   * As the packet header is read from the FIFO, \c packet_header_len is
   * updated with the number of bytes of the packet header that have been
   * read.
   */
  uint32            packet_header[SMD_PACKET_HEADER_SIZE];

  /** The previously received packet.  For debug use. */
  uint32            packet_header_prev[SMD_PACKET_HEADER_SIZE];

  /** The number of bytes to skip before reading the next packet header.
   *
   * This is non-zero when the other end of this connection is using SMD and
   * has written a packet too large to fit in the FIFO at once. All SMD Lite
   * read and writes must be atomic, so SMD Lite will truncate the packet,
   * reading only as much as will fit in the FIFO at one time, and discard the
   * remainder of the packet (\c skip bytes).
   */
  uint32            skip;

  /** The number of bytes that have already been written to the FIFO in a
   * continued write.
   */
  uint32            continue_write;

  /** The number of bytes that have already been read from the FIFO in a
   * continued read. */
  uint32            continue_read;

  /** The read index maintained internally by the the smdl_get_buffered_msgs()
   * routine */
  uint32            read_indx_internal;

} smd_lite_stream_info_type;

/*-------------------------------------------------------------------------
 *        Define memcpy packet local data structures
 *-------------------------------------------------------------------------*/
typedef struct{
  smd_memcpy_tx_callback_fn tx_callback_fn;
  void                     *tx_callback_data;
  smd_memcpy_rx_callback_fn rx_callback_fn;
  void                     *rx_callback_data;
  smd_memcpy_rx_flowctl_fn  rx_flowctl_fn;
  void                     *rx_flowctl_data;

  /* used to restore the flow ctl cb if reverting back to streaming 
     mode */
  smd_memcpy_rx_flowctl_fn saved_flowctl_fn;

  /* indicates the rx state */
  byte rx_state;

  /* pointer to the aligned rx packet header buffer */
  uint32 *rx_packet_header;

  /* pointer to the client supplied rcv buffer */
  byte *rx_buf_ptr;

  /* stores the size of the client rx buffer */
  uint32 rx_buf_sz;

  /* store the amount of unread data in a rcv packet */
  uint32 rx_bytes_remaining;

  /* pointer to a buffer that is used for truncation */
  byte *rx_scratch_buf;

  /* Indicates the TX STATE */
  byte tx_state;

  /* pointer to the aligned transmit packet header */
  uint32 *tx_packet_header;

  /* pointer to the client transmit buffer */
  byte *tx_buf;

  /* size of the client transmit buffer */
  uint32 tx_size;

  /* Size of the packet headers */
  uint32 packet_header_size;
}smd_memcpy_packet_info_type;

/*-------------------------------------------------------------------------
 *        Define Stream local data structures
 *-------------------------------------------------------------------------*/
typedef struct smd_stream_info_struct smd_stream_info_type;

typedef void (*smd_stream_api_fn)( smd_stream_info_type * );
typedef uint32 (*smd_stream_api_uint32_fn)( smd_stream_info_type * );

struct smd_stream_info_struct
{
  smd_port_id_type       port_id;    /* Associated SMD port */
  smd_channel_type       channel_type; /* SMD connection type */
  
  /* Shared memory stream info */
  volatile smd_shared_stream_info_type  *tx_shared_info_ptr;
  volatile smd_shared_stream_info_type  *rx_shared_info_ptr;

  /* Shared memory stream info word-aligned structure*/
  volatile smd_shared_stream_info_algn_type  *tx_shared_info_ptr_algn;
  volatile smd_shared_stream_info_algn_type  *rx_shared_info_ptr_algn;

  /* local cached copy of TX write and RX read indices */
  uint32 tx_write_index;
  uint32 rx_read_index;

  /* flow control methods */
  smd_flow_ctl_type      tx_flow_control_method;
  smd_flow_ctl_type      rx_flow_control_method;

  /* flag indicating whether the aligned shared info is used */
  boolean                is_shared_info_algn;

  /* Flow control */
  boolean                tx_fctl_enabled;
  boolean                rx_fctl_enabled;

  /* opening port xmit control */
  boolean                tx_opening_drop_enabled;

  /* If enabled, only return full memory buffers to the buffer layer */
  boolean                ret_full_rcv_bufs_en;

  /* DTR changed callback function pointer */
  smd_vv_func_ptr_type   dtr_callback_fn;

  /* extended DTR changed callback function pointer and data */
  smd_vu_func_ptr_type   dtr_callback_ext_fn;
  void                   *dtr_callback_ext_data;

  /* extended CD changed callback function pointer and data */
  smd_vu_func_ptr_type   cd_callback_ext_fn;
  void                   *cd_callback_ext_data;

  /* extended RI changed callback function pointer and data */
  smd_vu_func_ptr_type   ri_callback_ext_fn;
  void                  *ri_callback_ext_data;

  /* extended CTS changed callback function pointer and data */
  smd_vu_func_ptr_type   cts_callback_ext_fn;
  void                  *cts_callback_ext_data;

  /* Cached values of dtr, cd and rts used for checking for state changes */
  uint8 prev_dtr;
  uint8 prev_cd;
  uint8 prev_rts;
  uint8 prev_ri;

  /* Stream mode always memcpy for now */
  smd_dataxfr_mode_type  dataxfr_mode;

  /* transfer flow whether Streaming mode or Packet mode */
  smd_xfrflow_type       xfrflow;

  /* Stream state machine fields */
  boolean open_pending;
  boolean close_pending;
  boolean flush_pending;
  boolean read_pending;
  boolean transmit_pending;
  uint32  prev_remote_state;

  /* Close call back info */
  smd_close_callback_fn  close_cb;
  void                   *close_cb_data;

  /* Flush call back info */
  smd_flush_callback_fn  flush_cb;
  void                   *flush_cb_data;

  /* Functions associated with dataxfr mode */
  smd_stream_api_fn         stream_init;
  smd_stream_api_fn         stream_read;
  smd_stream_api_fn         stream_write;
  smd_stream_api_fn         stream_flush_tx;
  smd_stream_api_fn         stream_dtr;
  smd_stream_api_fn         stream_drop;
  smd_stream_api_fn         stream_close;
  smd_stream_api_fn         stream_open;
  smd_stream_api_fn         stream_reset;
  smd_stream_api_uint32_fn  stream_tx_abort;
  smd_stream_api_uint32_fn  stream_rx_abort;
  
  /* Count of pending IOCTL commands for this stream */
  uint32 pending_ioctl_cnt;

  /*************************************************************
   * Maximum amount of data that can be queued in the shared
   * memory buffer. Default value is fifo_sz - 4.
   * - 4 because write_index == read_index => buffer is empty
   * - 4 instead of - 1 to try to keep transfers word aligned
   *************************************************************/
  uint32 max_queued_data;

  /* Private data associated with dataxfr mode */
  union {
    smd_memcpy_stream_info_type  memcpy;
    smd_lite_stream_info_type    lite;
  } mode;

  union {
    smd_memcpy_packet_info_type  memcpy;
  } packet;

  /* The size of the SMD FIFO */
  uint32 fifo_sz;

  /* the rx and tx FIFOs in SMEM */
  volatile uint32 *tx_shared_fifo;
  volatile uint32 *rx_shared_fifo;
  
  /* This is the function SMD will use to copy data to the SMD buffers
   * for this port. */
  void *(*copy_to_fifo_fn)(void *, const void *, unsigned int);
  
  /* This is the function SMD will use to copy data from the SMD buffers
   * for this port. */
  void *(*copy_from_fifo_fn)(void *, const void *, unsigned int);

};

/*-------------------------------------------------------------------------
 *        Determine to open ports on given edge
 *-------------------------------------------------------------------------*/
/* each bit position represents different processor.
   smd_supported_proc is initialized in the OS specific file based on 
   target information available.
 */
extern uint32 smd_supported_proc; 

/*-------------------------------------------------------------------------
 *        Define the general SMD Port local data structure
 *-------------------------------------------------------------------------*/

typedef struct smd_info_struct smd_info_type;

/**
 * SMD context structure for each SMD task
 *
 * SMD ports may be divided among multiple SMD tasks, each running at a
 * different task priority, to allow SMD to read and write data, and call user
 * callbacks, at an appropriate task priority. This structure contains all the
 * information that an SMD task needs to process the ports that have been
 * assigned to it.
 */
typedef struct
{
  /** The priority associated with this context */
  smd_priority_type      priority;

#ifdef FEATURE_SMD
  /** The command queue associated with this context */
  q_type                *cmd_queue;
#endif

  /** The mutex protecting iteration and manipulation of the cmd_queue. */
  void                  *cmd_queue_cs;

  /** Pointer to the first open port to be handled by this context */
  smd_info_type         *open_list;

  /** OS-specific information for this context
   *
   * This structure abstracts the information the OS-specific implementation
   * needs to use to notify the OS thread that work needs to be done.
   */
  smd_os_context_type    os;
  
  /** Determines whether this task should process nway loopback port updates.
   * This should only be true in the lowest-priority task (or only task). */
  boolean                process_loopback;
} smd_context_type;

/**
 * Top-level structure for all open SMD ports.
 */
struct smd_info_struct
{
  /* Link needed for use with list APIs.  Must be at the head of the struct */
  smem_list_link_type    link;

  /* For linked list of opening and opened streams */
  smd_info_type  *next;

  /** Pointer to the context for the SMD task associated with this port */
  smd_context_type      *context;

  smd_port_id_type       port_id;

  smd_channel_type       channel_type; /* SMD connection type */

  char                   port_name[SMD_CHANNEL_NAME_SIZE_MAX]; /* port name */

  void(*handle_event)(smd_info_type *);

  union
  {
    smd_stream_info_type stream;
  }info;

};

struct smd_ist_info_type
{
  uint32 irq;
  smd_channel_type type;
};

/*-------------------------------------------------------------------------
 *        Define the SMD interrupt entry structure
 *-------------------------------------------------------------------------*/
typedef struct
{
  smem_host_type           to;
  smd_os_proc_type        processor;
  smd_os_intr_type        irq_out;
  uint32                  irq_out_mask;
  uint32                  irq_in;
  boolean                 disabled;
} smd_interrupt_info_type;

/** Table defining interrupts for each channel in the target */
extern smd_interrupt_info_type smd_interrupt_table[];


/*===========================================================================
                           FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smdi_init

DESCRIPTION   Initializes the smd info structure and the SMD port number to
              smd info mapping structure.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smdi_init( void );

/*===========================================================================
FUNCTION      smdi_alloc_info

DESCRIPTION   Allocates an smd info for the given SMD port id.

ARGUMENTS     port_id - SMD port to be used

RETURN VALUE  Pointer to smd info struct.

SIDE EFFECTS  None
===========================================================================*/
smd_info_type *smdi_alloc_info( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smdi_free_info

DESCRIPTION   Frees an smd info

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smdi_free_info( smd_info_type *info );

/*===========================================================================
FUNCTION      smdi_get_info

DESCRIPTION   Find an smd info allocated to the given port_id

ARGUMENTS     port_id - SMD port to use as a search key

RETURN VALUE  Pointer to smd info struct.

SIDE EFFECTS  
===========================================================================*/
smd_info_type *smdi_get_info( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smdi_get_stream_info

DESCRIPTION   Find an smd stream allocated to the given port_id

ARGUMENTS     port_id - SMD port to use as a search key

RETURN VALUE  Pointer to smd stream info struct.

SIDE EFFECTS  
===========================================================================*/
smd_stream_info_type *smdi_get_stream_info( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smdi_is_stream_open

DESCRIPTION   Indicates if a stream is already opened for the port_id

ARGUMENTS     port_id - SMD port

RETURN VALUE  TRUE if this SMD port is open
              FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean smdi_is_stream_open( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smdi_event_send_update

DESCRIPTION   Set state updated flag, do memory barrier and then send update
              event to other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_event_send_update( smd_stream_info_type *info );

/*===========================================================================
FUNCTION      smdi_event_send_transmit

DESCRIPTION   Set data read flag, do memory barrier and then send
              transmit event to other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_event_send_transmit( smd_stream_info_type *info );

/*===========================================================================
FUNCTION      smdi_event_send_receive

DESCRIPTION   Set data written flag, do memory barrier and then send
              receive event to other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_event_send_receive( smd_stream_info_type *info );

/*===========================================================================
FUNCTION      smdi_stream_state_machine

DESCRIPTION   Handle events which occur while in any state. Calls the
              appropriate handler based on the current state. It continues
              calling handlers until no new event is generated.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_stream_state_machine( smd_stream_info_type *info, uint32 event );

/*===========================================================================
FUNCTION      smdi_stream_state_update

DESCRIPTION   Handle update event from other processor. Generates an stream
              state machine event based on the change in the stream state on
              the other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_stream_state_update( smd_stream_info_type *info );

/*===========================================================================
FUNCTION      smdi_allocate_stream_channel

DESCRIPTION   Allocate a streaming shared memory channel and connect the 
              tx and rx connects to be consistent with the other processor.

ARGUMENTS     port_id - port id associated with the channel
              info - pointer to stream info structure

RETURN VALUE  SMD_STATUS_SUCCESS if successful
              SMD_STATUS_SIZE_MISMATCH if port already exists with different
              size. Updates info->fifo_sz to the actual size.
===========================================================================*/
int32 smdi_allocate_stream_channel
(
  smd_port_id_type port_id,
  smd_stream_info_type *info
);

/*===========================================================================
FUNCTION      smdi_get_port_name_ex

DESCRIPTION   maps the port id to the port name.

ARGUMENTS     port id 

RETURN VALUE  Returns the port name
===========================================================================*/
const char *smdi_get_port_name_ex(smd_port_id_type port_id);

/*===========================================================================
FUNCTION      smdi_get_port_name

DESCRIPTION   This function translates the port id into a port name.

ARGUMENTS     port_id   port id.

RETURN VALUE  Returns the port name. 
              
SIDE EFFECTS  None
===========================================================================*/
const char *smdi_get_port_name(smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smdi_add_channel_info

DESCRIPTION   This function updates the smd_channel_alloc_tbl for the
              specified channel.

ARGUMENTS     cid            channel id
              name           channel name 
              channel_type   channel connection type
              xfrflow        channel xfrflow

RETURN VALUE  None 
              
SIDE EFFECTS  None
===========================================================================*/
void smdi_add_channel_info
(
  uint32                     cid,
  const char                *name,
  smd_channel_type           channel_type,
  smd_xfrflow_type           xfrflow,
  smem_host_type             remote_host
);

/*===========================================================================
FUNCTION      smdi_delete_channel_info

DESCRIPTION   This function deletes the channel information

ARGUMENTS     port_id  port id structure

RETURN VALUE  None

SIDE EFFECTS  Leave the entry in the smd_channel_alloc_tbl even if the
              channel is closed.
===========================================================================*/
void smdi_delete_channel_info
(
  smd_port_id_type port_id
);

/*===========================================================================
FUNCTION      smdi_alloc_channel_info

DESCRIPTION   This function allocates/populates the channel info in the
              smd_channel_alloc_tbl for the specified channel. 

ARGUMENTS     name           channel name 
              channel_type   channel connection type
              xfrflow        channel xfrflow

RETURN VALUE  it returns the port id object.

SIDE EFFECTS  None
===========================================================================*/
smd_port_id_type smdi_alloc_channel_info
(
  const char                *name,
  smd_channel_type           channel_type,
  smd_xfrflow_type           xfrflow
);

/*===========================================================================
FUNCTION      smdi_set_channel_xfrflow

DESCRIPTION   Sets the channel xfrflow status if it has not already been
              set.  If the value has been set, then this function will
              compare the set value to the inputted value.

ARGUMENTS     port_id        port id structure
              xfrflow        The xfrflow input
              
RETURN VALUE  SMD_DONE if it sets the xfrflow or if the xfrflow was already
                       set to this value
              SMD_NON_CRIT_MISMATCH if the inputted xfrflow does not match
                                    the xfrflow already in the CID table
              SMD_INVALID_INPUT if the cid is not allocated
===========================================================================*/
smd_api_return_type smdi_set_channel_xfrflow
(
  smd_port_id_type  port_id,
  smd_xfrflow_type  xfrflow
);

/*===========================================================================
FUNCTION      smdi_get_channel_id

DESCRIPTION   Look up the channel id from the smd_channel_alloc_tbl
              using port name and connection type.  

ARGUMENTS     name           channel name 
              channel_type   channel connection type
              
DEPENDENCIES  None

RETURN VALUE  returns the channel id if found.
              returns -1 if not found.

SIDE EFFECTS  None
===========================================================================*/
int32 smdi_get_channel_id
(
  const char       *name,
  smd_channel_type  channel_type
);

/*===========================================================================
FUNCTION      smd_memcpy_from_sm
===========================================================================*/
/**
@brief        Copies data from an SMD FIFO to local memory.

              This function copies data from a shared memory buffer to a
              destination buffer. It uses memcpy for performance and handles
              wrap around for the shared memory buffer.
              
@param[in]    info         The SMD channel info structure

@param[in]    dst          pointer to destination buffer

@param[in]    src          pointer to shared memory buffer

@param[in]    count        number of bytes to copy

@param[in]    read_index   start index in source buffer

@param[in]    src_size     size of source buffer

@return       The next read index for the shared memory buffer.
*/
/*=========================================================================*/
uint32 smd_memcpy_from_sm
(
  smd_stream_info_type *info,
  uint8 *dst,
  const uint8 *src,
  uint32 count,
  uint32 read_index
);

/*===========================================================================
FUNCTION      smd_memcpy_to_sm
===========================================================================*/
/**
@brief        Copies data from local memory to an SMD FIFO.

              This function copies data from a source buffer to a shared memory
              buffer. It uses memcpy for performance and handles wrap around
              for the shared memory buffer.
              
@param[in]    info         The SMD channel info structure

@param[in]    dst           pointer to shared memory buffer

@param[in]    src           pointer to source buffer

@param[in]    count         number of bytes to copy

@param[in]    write_index   start index in destination buffer

@param[in]    dst_size      size of destination buffer

@return       The next write index for the shared memory buffer.
*/
/*=========================================================================*/
uint32 smd_memcpy_to_sm
(
  smd_stream_info_type *info,
  uint8 *dst,
  const uint8 *src,
  uint32 count,
  uint32 write_index
);

/*===========================================================================
FUNCTION      smd_lite_open
===========================================================================*/
/**
 * This routine handles an open command for a SMD Lite stream.
 *
 * This function allocates and initializes a shared memory structure, finishes
 * initializing the stream info structure and sends the stream state machine
 * an OPEN event.
 *
 * @param[in]    port_id   SMD port to open
 *
 * @return       This function will always return SMD_DONE
 *
 * @sideeffects  None
 */
/*=========================================================================*/
smd_api_return_type smd_lite_open( smd_port_id_type port_id );

/*===========================================================================
FUNCTION      smdl_read_packet_header
===========================================================================*/
/**
 * Reads the SMD packet header from the FIFO and updates the read_index. If a
 * full packet header could be read, and the full packet is available in the
 * FIFO (or if the packet is too big to fit in the FIFO), returns the number
 * of bytes in the packet payload.
 *
 * This function must be called with the channel's receive mutex held.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return  If a full packet header was read from the shared memory FIFO, the
 *          number of bytes in the packet. If no packet is available, or the
 *          full packet header was not available, 0.
 */
uint32 smdl_read_packet_header( smd_stream_info_type *si );

/*===========================================================================
  FUNCTION  smd_send_interrupt
===========================================================================*/
/**
  Send an interrupt to the target processor. 

  @param[in] host         Remote host to which the interrupt is to be sent.

  @return
  NA.

  @dependencies
  None.

  @sideeffects
  Interrupts the target processor.
*/
/*=========================================================================*/
void smd_send_interrupt( smem_host_type host );

/*===========================================================================
  FUNCTION  smd_get_interrupt_info
===========================================================================*/
/**
  Returns the interrupt information for the specified channel in the target. 

  @param[in] channel         Channel of interest in the target

  @return
  The interrupt information for the specified channel.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smd_interrupt_info_type *smd_get_interrupt_info
(
  smd_channel_type channel
);

/*===========================================================================
  FUNCTION  smd_host_exists_in_channel
===========================================================================*/
/**
  Checks if a given host is part of the specified channel (edge).

  @param[in] channel         Channel of interest
 
  @param[in] host            SMD host ID to look for

  @return
  TRUE is host exists, FALSE otherwsie.

*/
/*=========================================================================*/
boolean smd_host_exists_in_channel( smd_channel_type channel, uint32 host );

/*===========================================================================
  FUNCTION  smdi_get_loopback_client
===========================================================================*/
/**
  This function returns loopback client from channel type.

  @param[in] channel         Channel of interest in the target

  @return
  The client for the specified channel, in the case of a loopback port.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smem_host_type smdi_get_loopback_client( smd_channel_type channel );

/*===========================================================================
  FUNCTION  smdi_get_loopback_server
===========================================================================*/
/**
  This function returns loopback server from channel type.

  @param[in] channel         Channel of interest in the target

  @return
  The server for the specified channel, in the case of a loopback port.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smem_host_type smdi_get_loopback_server( smd_channel_type channel );

/*===========================================================================
  FUNCTION  smd_target_init
===========================================================================*/
/**
  This function initializes target specific configuration.
  
  @return None

  @sideeffect   Initializes target specific variables

  @dependencies None
*/
/*==========================================================================*/
void smd_target_init( void );

/*===========================================================================
  FUNCTION  smd_target_init_interrupts
===========================================================================*/
/**
  This function initializes the interprocessor interrupts for this target.
  
  @param[in]  this_host   source of the interrupts

  @return None

  @sideeffect   Initializes the interrupt table

  @dependencies None
*/
/*==========================================================================*/
void smd_target_init_interrupts( smem_host_type this_host );

/*===========================================================================
  FUNCTION  smd_get_other_host
===========================================================================*/
/**
  This function translates destination host from channel type.
 
  @param[in] this_host       The source of the connection
  @param[in] channel         Channel of interest in the target

  @return
  The destination processor for the specified channel.
  SMEM_INVALID_HOST if source processor does not exist in channel.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smem_host_type smd_get_other_host( smem_host_type this_host, smd_channel_type channel );

/*===========================================================================
  FUNCTION  smdi_processors_exist
===========================================================================*/
/**
 * Check whether processors requesting port opening exist in this target.
 *
 * @param[in]	edge, to find out processors requesting port opening
 * 
 * @return  	FALSE,	one of processors does not exist or edge is invalid
 * @return 		TRUE,	both processors exist. 
 */ 
/*=========================================================================*/ 
boolean smdi_processors_exist(smd_channel_type edge);

/*===========================================================================
  FUNCTION  smdi_get_proc_bit_mask
===========================================================================*/
/**
 * Get corresponding bit mask for this processor (host)
 *
 * @param[in]	processor (host)
 * 
 * @return		bit mask MACRO for this processor (host)
 *				0,	if host is invalid
 */
/*=========================================================================*/ 
uint32 smdi_get_proc_bit_mask(smem_host_type host);

/*===========================================================================
  FUNCTION  smdi_close_channels
===========================================================================*/
/**
  Close all SMD ports coincident with the local edge, if they are not already
  closed.
  
  This function requires outgoing interrupts to be initialized and ready, but 
  it should be called before incoming interrupts are enabled, in order to 
  avoid processing state updates, reads, or writes until the port has been 
  safely closed.

  @return
  None

  @sideeffects
  Sends an interrupt to each remote processor that is coincident with a port
  that was updated by this function.
*/
/*=========================================================================*/
void smdi_close_channels( void );

#ifdef __cplusplus
}
#endif

#endif /* SMD_INTERNAL_H */
