/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications for Diag Control Channel

General Description

Copyright (c) 2010-2017, 2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_ctrl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
09/01/17   gn      Fixed invalid send_size issue on control channel
07/20/16   nk      Changes to support ThreadX
02/01/16   is      Do not send control packets on closed channel
01/13/16   is      Send feature mask before processing commands
10/12/15   as      Added NULL check to pointer dsm_item_ptr_sav
07/28/15   rh      Moved persistent data from diagcomm_ctrl_send_mult_pkt to queue
06/25/14   rs      Updated the functions to use msg_mask_preset_status stored in diag_msg_rt_mask_mstr_tbl.
03/12/14   xy      Add support for STM status 
02/21/14   is      Command/response separation
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
06/21/13   rh      Added flow control count for control channel 
06/14/13   rh      Support for detecting when we run low on ctrl channel 
                   memory and resending mask updates if we run out
06/03/13   ph      Fixed a corner case where diagpkt_table_fwd_status was not updated.
04/23/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove() 
11/27/12   rh      Removed DIAG_MP_SLAVE_LAST
11/27/12   rh      Mainlining DIAG_CENTRAL_ROUTING
11/15/12   is      Support for preset masks
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes
08/15/12   vk/ra   Modification for LogOnDemand algorithm change
07/19/12   rh      Set Diag control channel size to 8kB 
03/02/12   sg      Added function to return the channel connected status
                   for the ctrl port, call diagcomm_io_detection() while
				   opening the ctrl channel to register with dtr changes
12/08/11   is      ASSERT() when Diag runs out of DSM items
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/11/11   wjg     Added function to handle processing DIAG_CTRL_MSG_SIG
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite
03/02/11   hm      Support SIO control channel on single processor for
                   dual-mask feature 
12/20/10   mad     Changes for dual mask: Open SIO control port in MASTER DIAG
12/08/10   mad     do not open CTRL port if already open: do not reset 
                   diagcomm_ctrl_conn
10/14/10   mad     Diag now blocks until control port finishes closing
10/20/10   is      Resolve compilation issue in Apps only Diag
09/28/10   mad     Closing control port in diagcomm_ctrl_close
09/14/10   is      New control channel and central routing features 

===========================================================================*/

#include "diag_v.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h" /* For assert */
#endif

#include "diagbuf.h"         // For diagbuf_drain()
#include "diagcomm_ctrl.h"
#include "diagcomm_smd.h"
#include "diagdsmi.h"        // For DSMI_DIAG_SMD_TX_ITEM_CNT, etc.
#include "diagi_v.h"         // For DIAG_FWD_TX_SLAVE_SIG, diagpkt_process_ctrl_msg()
#include "diagpkt.h"         // For DIAGPKT_DISPATCH_TABLE_REGISTER_SLAVE()
#include "msg.h"             // For MSG_1, MSG_2 etc.
#include "osal.h"            // For osal* functions
#include "diagdsm_v.h"
#include "feature.h"         // For DIAG_INT_FEATURE_MASK_LEN
#include "msgi.h"
#include "memheap.h"
#include <stringl/stringl.h>

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */


// Data structures to enable resending of CTRL packets in case of flow control
static q_type diagcomm_ctrl_resend_q[NUM_SMD_PORT_TYPES];
static char diag_ctrl_heap_buf[DIAG_CTRL_RESEND_BUF_SIZE];

mem_heap_type diag_ctrl_heap;
static uint32 diag_ctrl_heap_malloc_count, diag_ctrl_heap_free_count;

typedef struct
{
   q_link_type qlink;
   void* buf;
   uint32 length;
}ctrl_resend_pkt_type;

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
static int diagcomm_ctrl_heap_init(void);
static void * diag_ctrl_heap_malloc (uint32 num_bytes);
static void  diag_ctrl_heap_free(void *mem_ptr);

/*Buffer to copy the control packet */
char diag_ctrl_pkt_inbound[DIAG_READ_BUF_SIZE];

static uint8 diagbuf_send_ctrl[NUM_SMD_PORT_TYPES][DIAG_CTRL_SEND_BUF_SIZE]; /*Buffer used to send out ctrl channel data */
#ifdef DIAG_CONSUMER_API
  static uint16 diag_send_buf_ctrl_index[NUM_SMD_PORTS] = {0, 0, 0}; /*Current ctrl buffer index*/
#else
  static uint16 diag_send_buf_ctrl_index[NUM_SMD_PORTS] = {0};
#endif /* DIAG_CONSUMER_API */
static osal_mutex_arg_t diag_send_ctrl_mutex;

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
#ifdef DIAG_SMD_SUPPORT
void diagcomm_process_inbound_ctrl_msg( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
#endif /* DIAG_SMD_SUPPORT */

/*===========================================================================

FUNCTION DIAG_SEND_CTRL_CS_INIT

DESCRIPTION
    This function initializes the diag_send_ctrl_mutex that serializes the
    control packets sent from APPS to various peripherals so that multiple
    tasks can simultaneously use diagcomm_ctrl_send_mult_pkt().

===========================================================================*/
void diag_send_ctrl_cs_init(void)
{
    int return_val;

    diag_send_ctrl_mutex.name = "MUTEX_DIAG_SEND_CTRL_CS";
    return_val = osal_init_mutex(&diag_send_ctrl_mutex);
  ASSERT(OSAL_SUCCESS == return_val);
}
/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_CTRL

DESCRIPTION
  Process control traffic when DIAG_CTRL_MSG_SIG received.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_process_ctrl( void )
{
#if defined (DIAG_MP_MASTER) 
  diagcomm_process_inbound_ctrl_msg (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
#else
  diagcomm_process_inbound_ctrl_msg (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
#endif // DIAG_MP_MASTER || DIAG_QDSP6_APPS_PROC
}

/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_INBOUND_CTRL_MSG

DESCRIPTION
  Process inbound traffic recieved via control channel.
  
PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_process_inbound_ctrl_msg( diagcomm_port_type port_type, 
                                   diagcomm_enum_port_type ctrl_channel )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][ctrl_channel];
  uint32 num_used = 0;

  osal_lock_mutex(&conn->buf_mutex);
  num_used = conn->used;
  memscpy(&diag_ctrl_pkt_inbound[0], sizeof(diag_ctrl_pkt_inbound), conn->buf,conn->used);
  conn->used = 0;
  osal_unlock_mutex(&conn->buf_mutex);

  diagpkt_process_ctrl_msg( &diag_ctrl_pkt_inbound[0], num_used, port_type, ctrl_channel );
  }
/*===========================================================================

FUNCTION DIAGCOMM_CTRL_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  True if the logical control stream is connected.
  False otherwise.
  
===========================================================================*/
boolean
diagcomm_ctrl_status ( diagcomm_port_type port_type, 
                       diagcomm_enum_port_type ctrl_channel )
{
  return (diagcomm_io_ctrl_conn[port_type][ctrl_channel].connected &&
          diagcomm_io_ctrl_conn[port_type][ctrl_channel].channel_connected);
  
} /* diagcomm_ctrl_status */

#if defined(DIAG_MP_MASTER)
/*===========================================================================

FUNCTION DIAGCOMM_CTRL_MASK_STATUS

DESCRIPTION
  This function checks internal bit-masks to determine if msg/event/logs have
  been updated and needs to be propagated to Slaves. Bit-masks are checked per
  mask type (msg/event/log) and per port/channel.


PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  mask - One of the DIAG_UPDATE_MASK_* bit masks to check
  stream_id - Stream id mask to check
  preset_id - Preset id mask to check
  
RETURN VALUE
  TRUE - Mask is set
  FALSE - Otherwise
  
===========================================================================*/
boolean diagcomm_ctrl_mask_status( diagcomm_port_type port_type, 
                                   diagcomm_enum_port_type ctrl_channel,
                                   dword mask, 
                                   uint8 stream_id,
                                   uint8 preset_id )
{    
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );

  if( stream_id == DIAG_STREAM_1 )
  {
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_ctrl_mask_status port_num=%d stream=%d preset_id=%d cur_mask=%d mask=%d", 
         ctrl_channel, stream_id, preset_id, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
    
    return DIAG_CHECK_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
  }
  #ifdef DIAG_CONSUMER_API
    else if( stream_id == DIAG_STREAM_2 )
    {
      MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_ctrl_mask_status port_num=%d stream=%d preset_id=%d cur_mask=%d mask=%d", 
            ctrl_channel, stream_id, preset_id, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
    
      return DIAG_CHECK_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
  }
  #endif /* #ifdef DIAG_CONSUMER_API */
  else
    return FALSE;
    
} /* diagcomm_ctrl_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLEAR_MASK_STATUS

DESCRIPTION
  Clears a bit mask for a given channel.

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  mask - The bit mask to set
  stream_id - Which stream it is being set for
  preset_id - Which mask preset it is being set for
  
RETURN VALUE
  The resulting mask, or FALSE if parameters were bad

===========================================================================*/
dword diagcomm_ctrl_clear_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type ctrl_channel,
                                       dword mask,
                                       uint8 stream_id,
                                       uint8 preset_id )
{
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
    
  if( stream_id == DIAG_STREAM_1 )
  {
    DIAG_CLR_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
    
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "clear_mask_status port_num=%d stream=%d preset_id=%d clear=%d ret=%d", 
        ctrl_channel, stream_id, preset_id, mask, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1]);
    
    return diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1];
  }
  #ifdef DIAG_CONSUMER_API
    else if( stream_id == DIAG_STREAM_2 )
    {
      DIAG_CLR_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
    
      MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "clear_mask_status port_num=%d stream=%d preset_id=%d clear=%d ret=%d", 
          ctrl_channel, stream_id, preset_id, mask, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1]);
    
      return diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1];
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  else
    return FALSE;
  
} /* diagcomm_ctrl_clear_mask_status */

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SET_MASK_STATUS

DESCRIPTION
  Sets a bitmask for a given channel

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  mask - The bit mask to set
  stream_id - Which stream it is being set for
  preset_id - Which mask preset it is being set for
  
RETURN VALUE
  Whether the set was successful

===========================================================================*/
boolean diagcomm_ctrl_set_mask_status( diagcomm_port_type port_type, 
                                     diagcomm_enum_port_type ctrl_channel,
                                     dword mask,
                                     uint8 stream_id,
                                     uint8 preset_id )
{
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
    
  if( stream_id == DIAG_STREAM_1 )
  {
    DIAG_SET_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
    
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "clear_mask_status port_num=%d stream=%d preset_id=%d clear=%d ret=%d", 
        ctrl_channel, stream_id, preset_id, mask, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1]);
    
    return TRUE;
  }
  #ifdef DIAG_CONSUMER_API
    else if( stream_id == DIAG_STREAM_2 )
    {
      DIAG_SET_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
    
      MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "clear_mask_status port_num=%d stream=%d preset_id=%d clear=%d ret=%d", 
          ctrl_channel, stream_id, preset_id, mask, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1]);
    
      return TRUE;
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  else
    return FALSE;
  
} /* diagcomm_ctrl_clear_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_MSG_MASK_STATUS

DESCRIPTION

PARAMETERS

RETURN VALUE

===========================================================================*/
boolean diagcomm_ctrl_msg_mask_status( diagcomm_enum_port_type port_num,
                                       uint8 stream_id,
                                       uint8 preset_id,
                                       uint8 index )
{
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
  
  if( stream_id == DIAG_STREAM_1 )
    return diag_msg_rt_mask_mstr_tbl[index].msg_mask_preset_status[port_num][preset_id -1];
#if defined(DIAG_STREAM_2_MASK_CHECK)
  else if( stream_id == DIAG_STREAM_2 )
    return diag_msg_rt_mask_mstr_tbl[index].msg_mask_dci_status[port_num][DIAG_DCI_MASK_1 -1];
#endif
  else
    return FALSE;
  
} /* diagcomm_ctrl_msg_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLEAR_MSG_MASK_STATUS

DESCRIPTION

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  port_num - Port num for peripheral
  stream_id - Mask stream ID
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_clear_msg_mask_status( diagcomm_enum_port_type port_num,
                                          uint8 stream_id,
                                          uint8 preset_id )
{
  uint8 index = 0;
  
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
  
  for( index=0; index < diag_msg_mask_mstr_tbl.mask_count; index++ )
  {
    if( stream_id == DIAG_STREAM_1 )
      diag_msg_rt_mask_mstr_tbl[index].msg_mask_preset_status[port_num][preset_id -1] = FALSE;
#if defined(DIAG_STREAM_2_MASK_CHECK)
    else if( stream_id == DIAG_STREAM_2 )
      diag_msg_rt_mask_mstr_tbl[index].msg_mask_dci_status[port_num][DIAG_DCI_MASK_1 -1] = FALSE;
#endif
  }
  
} /* diagcomm_ctrl_clear_msg_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_LOG_MASK_STATUS

DESCRIPTION

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  stream_id - Mask stream ID
  equip_id - Log equipment id
  
RETURN VALUE

===========================================================================*/
boolean diagcomm_ctrl_log_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type ctrl_channel,
                                       uint8 stream_id,
                                       uint8 equip_id )
{
  return diagcomm_io_ctrl_conn[port_type][ctrl_channel].log_mask_update[stream_id-1][equip_id];
  
} /* diagcomm_ctrl_log_mask_status */

#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_PORT_ID

DESCRIPTION
  This function returns the SMD/SMDL port ID 
  
PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  Port ID of the control channel
  
===========================================================================*/
uint16
diagcomm_ctrl_port_id( diagcomm_port_type port_type,
                       diagcomm_enum_port_type ctrl_channel )
{
#if defined(DIAG_SIO_SUPPORT)
  return diagcomm_io_ctrl_conn[port_type][ctrl_channel].open_params.sio_params.params.port_id;
#else
  return 0;
#endif

} /* diagcomm_ctrl_port_id */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_OPEN

DESCRIPTION
  Opens the communcications stream.

PARAMETERS
  ctrl_port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel to open
  
RETURN VALUE
  True if control port opened successfully.
  False otherwise.
  
===========================================================================*/
boolean
diagcomm_ctrl_open( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][ctrl_channel];
  
#if defined(DIAG_MP_MASTER)
  uint8 preset_index;
#endif

  if (conn->connected)
  {
    return conn->connected;
  }

  diagcomm_io_set_port( conn );
  
  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
      diag_io_detect_connection( conn );
      
#if defined(DIAG_MP_MASTER)
      // Master has opened CTRL port to Slave; Need to send mask updates to Slave
      for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
      {
        DIAG_SET_MASK(conn->mask_preset_update[preset_index], DIAG_UPDATE_MASK_F3);
        DIAG_SET_MASK(conn->mask_preset_update[preset_index], DIAG_UPDATE_MASK_LOGS);
        DIAG_SET_MASK(conn->mask_preset_update[preset_index], DIAG_UPDATE_MASK_EVENTS);
      }
    #ifdef DIAG_CONSUMER_API
      DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_F3);
      DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_LOGS);
      DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_EVENTS);
    #endif /* #ifdef DIAG_CONSUMER_API */
#endif
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_ctrl_open - Couldn't get stream.");
    }
  }
  
  return conn->connected;

} /* diagcomm_ctrl_open */


/*===========================================================================

FUNCTION DIAGCOMM_NEW_DSM

DESCRIPTION
  This function requests a new DSM buffer from a specified DSM pool.

PARAMETERS
  pool_id - DSM pool to allocate new DSM buffer from
  priority - If True, sets the DSM priority to high.
             If False, DSM is set to default priority.
  
RETURN VALUE
  Pointer to the new DSM item
  
===========================================================================*/
dsm_item_type *
diagcomm_new_dsm( dsm_mempool_id_type pool_id, boolean priority )
{
  dsm_item_type *item_ptr = NULL;
  
  item_ptr = dsm_new_buffer(pool_id);
  
#if defined(DIAG_DSM_DEBUG)	
  ASSERT( item_ptr != NULL );
#endif
  
  if( item_ptr ) 
  {  
    item_ptr->pkt_ptr = NULL;
    
    if( priority )
    {
      item_ptr->priority = DSM_HIGHEST;
    }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"diagcomm_new_dsm - Ran out of DSM items from pool %d", pool_id);
  }
  
  return item_ptr;
  
} /* diagcomm_new_dsm */

/*===========================================================================

FUNCTION diagcomm_ctrl_add_to_queue

DESCRIPTION
  This function adds the ctrl packet that was not sent to a queue to be sent later

PARAMETERS
  tx_params -The tx parameters which include the buffer and length.

RETURN VALUE
  DIAG_EOK - if adding to the queue succeeds.
  DIAG_EMEM - if the available memory on the heap is exhausted.
  DIAG_EPARAM - if the parameters passed are invalid.

===========================================================================*/
int diagcomm_ctrl_add_to_queue(  diagcomm_io_tx_params_type *tx_params, diagcomm_enum_port_type port_num)
{
   ctrl_resend_pkt_type *q_item;

   if(NULL == tx_params) return DIAG_EPARAM;
   q_item = (ctrl_resend_pkt_type *)diag_ctrl_heap_malloc(sizeof(ctrl_resend_pkt_type));

   if(NULL == q_item)
   {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diag_ctrl_heap_malloc failed for item length %d",sizeof(ctrl_resend_pkt_type));
      return DIAG_EMEM;
   }
   memset(q_item,0,sizeof(ctrl_resend_pkt_type));
   (void) q_link(q_item,&q_item->qlink);
   q_item->buf = (void *)diag_ctrl_heap_malloc(tx_params->smdl_params.len);

   if(NULL == q_item->buf)
   {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diag_ctrl_heap_malloc failed for buffer length: %d",tx_params->smdl_params.len);
      //Free the item pointer allocated earlier.
       diag_ctrl_heap_free(q_item);
      return DIAG_EMEM;
   }
   //Copy the buffer and set the queue links
   memscpy(q_item->buf,tx_params->smdl_params.len, tx_params->smdl_params.buffer,tx_params->smdl_params.len);
   q_item->length = tx_params->smdl_params.len;
   //Put the q_item in the resend queue.
   q_put(&diagcomm_ctrl_resend_q[port_num], &(q_item->qlink));
   return DIAG_EOK;

}

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_MULT_PKT

DESCRIPTION
  This function aggregates multiple ctrl packets to be sent in one DSM and
  transmits it via 'ctrl_channel' when it's full. Client may also opt to send
  the DSM immediately by setting 'send_now' to True.

PARAMETERS
  ctrl_pkt - The data packet to aggregate into the DSM
  ctrl_pktlen - The length of 'ctrl_pkt'
  user_pkt - Additional data packet to aggregate into the DSM.
             Used for additional variable length data appended to 'ctrl_pkt'.
  user_pktlen - The length of 'user_pktlen'
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel to transmit the DSM to
  send_now - If True, aggregate 'ctrl_pkt' into the DSM (if any) and transmit the DSM
               immediately.
             If False, aggregate 'ctrl_pkt' into the DSM and transmit the DSM only
               when full.
  is_sent [out] - Returns True if a DSM item was sent. False otherwise.
  
RETURN VALUE
  The number of bytes newly aggregated into the current DSM
  
 WARNING
  This function may only be called by the diag task.  It is not re-entrant!
  
===========================================================================*/
uint32
diagcomm_ctrl_send_mult_pkt( PACK(void *) ctrl_pkt, 
                             uint32 ctrl_pktlen, 
                             PACK(const void *) user_pkt, 
                             uint32 user_pktlen,
                             diagcomm_port_type port_type,
                             diagcomm_enum_port_type ctrl_channel,
                             boolean send_now,
                             boolean * is_sent )
{
  uint32 added_pkt_len = 0;
  uint8 * cur_data_ptr = NULL;
  boolean retry = FALSE;
  diagcomm_io_tx_params_type tx_params;

  /* Don't attempt to send data if the channel is not connected. */
  if( (diagcomm_io_ctrl_conn[port_type][ctrl_channel].connected == FALSE) || 
     (diagcomm_io_ctrl_conn[port_type][ctrl_channel].channel_connected == FALSE) )
  {
    return 0;
  }

  /* Sanity check */
  if( !is_sent)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_ctrl_send_mult_pkt - Sanity check failed");
    return added_pkt_len;
  }
  *is_sent = FALSE;


  if( !ctrl_pkt || (ctrl_pktlen <= 0))
  {
    osal_lock_mutex(&diag_send_ctrl_mutex);
    if( send_now && (diag_send_buf_ctrl_index[ctrl_channel] != 0) )
    {
      /* Just send immediately if there's no pkt to write */
      tx_params.smdl_params.buffer = diagbuf_send_ctrl[port_type];
      tx_params.smdl_params.len = diag_send_buf_ctrl_index[ctrl_channel];
      tx_params.smdl_params.flags = 0;
      if(DIAG_EOK != diagcomm_io_transmit(&diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params))
      {
         diagcomm_ctrl_add_to_queue(&tx_params, ctrl_channel);
      }
      *is_sent = TRUE;
      diag_send_buf_ctrl_index[ctrl_channel] = 0;
    }
    else
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_ctrl_send_mult_pkt - No pkt to write 0x%x %d", ctrl_pkt, ctrl_pktlen);
    }
    osal_unlock_mutex(&diag_send_ctrl_mutex);
    return added_pkt_len;
  }

  if( diagcomm_io_ctrl_conn[port_type][ctrl_channel].allow_flow == FALSE )
  {
    return added_pkt_len;
  }

  osal_lock_mutex(&diag_send_ctrl_mutex);

  do
  {
    retry = FALSE;

     /*If the data to be sent does not fit in the buffer then send the current buffer*/
     if ((ctrl_pktlen +  user_pktlen + diag_send_buf_ctrl_index[ctrl_channel]) > (DIAG_CTRL_SEND_BUF_SIZE))
    {
        if (diag_send_buf_ctrl_index[ctrl_channel] != 0)
      {
           tx_params.smdl_params.buffer = diagbuf_send_ctrl[port_type];
           tx_params.smdl_params.len = diag_send_buf_ctrl_index[ctrl_channel];
           tx_params.smdl_params.flags = 0;
           if (DIAG_EOK != diagcomm_io_transmit(&diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params)) {
              diagcomm_ctrl_add_to_queue(&tx_params, ctrl_channel);
    }
           *is_sent = TRUE;
           diag_send_buf_ctrl_index[ctrl_channel] = 0;
        }
        }

     if ((diag_send_buf_ctrl_index[ctrl_channel] + ctrl_pktlen + user_pktlen) <= DIAG_CTRL_SEND_BUF_SIZE)
        {
        // Append ctrl_pkt to DSM
        cur_data_ptr = &diagbuf_send_ctrl[port_type][diag_send_buf_ctrl_index[ctrl_channel]];
        memscpy((void *)cur_data_ptr, (DIAG_CTRL_SEND_BUF_SIZE - diag_send_buf_ctrl_index[ctrl_channel]), (void *)ctrl_pkt, ctrl_pktlen);
        diag_send_buf_ctrl_index[ctrl_channel] += ctrl_pktlen;
        added_pkt_len = ctrl_pktlen;
        
        /* Append user_pkt to DSM, if any */
        if( (user_pkt != NULL) && (user_pktlen > 0) )
        {
            cur_data_ptr = &diagbuf_send_ctrl[port_type][diag_send_buf_ctrl_index[ctrl_channel]];
            memscpy((void *)cur_data_ptr, (DIAG_CTRL_SEND_BUF_SIZE - diag_send_buf_ctrl_index[ctrl_channel]),
                  (void *)user_pkt, user_pktlen );
            diag_send_buf_ctrl_index[ctrl_channel] += user_pktlen;
          added_pkt_len += user_pktlen;
        }

        if( send_now )
        {
            /* Just send immediately if there's no pkt to write */
            tx_params.smdl_params.buffer = diagbuf_send_ctrl[port_type];
            tx_params.smdl_params.len = added_pkt_len;
            tx_params.smdl_params.flags = 0;


            if(DIAG_EOK != diagcomm_io_transmit(&diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params))
            {
                diagcomm_ctrl_add_to_queue(&tx_params, ctrl_channel);
            }
            diag_send_buf_ctrl_index[ctrl_channel] = 0;
          *is_sent = TRUE;
        }
      }
      else
      {
        // Error case; the data recieved is greater than the maximum size;
        ASSERT(0);
      }
    }
  while (retry);

  osal_unlock_mutex(&diag_send_ctrl_mutex);

  return added_pkt_len;
  
} /* diagcomm_ctrl_send_mult_pkt */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLOSE

DESCRIPTION
  Closes the current control channel connection, if any.

PARAMETERS
  *close_func_ptr - Pointer to function to execute
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel to close
 
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_ctrl_close( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][ctrl_channel];
  
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
  }

  /* success or not, consider close operation successful  */
  conn->connected = FALSE;

} /* diagcomm_ctrl_close */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer.

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_ctrl_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num )
{  
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][port_num];
  
  /* Initialize conn */
  conn->channel_type = DIAGCOMM_IO_CTRL;
  conn->port_num = port_num;
  conn->port_type = port_type;

  /* Make sure we are closed */
  diagcomm_ctrl_close( port_type, port_num );

  (void) q_init (&diagcomm_ctrl_resend_q[port_num]);

  diagcomm_ctrl_heap_init();

  diag_send_ctrl_cs_init();

} /* diagcomm_ctrl_init */

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_FEATURE_MASK

DESCRIPTION
  Sends copy of the local Diag internal feature mask 'diag_int_feature_mask[]'
  to all processors this communicates with.
  
  ie: Master Diag sends to all Slave Diags and Slave Diag sends to Master Diag.
  
PARAMETERS
  None

DEPENDENCIES
  diag_init_feature_mask() should be called to set up the Diag internal 
  feature mask prior to sending.
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_send_feature_mask( void )
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
#if defined(DIAG_MP_MASTER)
  int port_num = 0;
#endif

  /* Compose DIAG_CTRL_MSG_FEATURE packet */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_FEATURE;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_feature_type);
  ctrl_msg.data.ctrl_msg_feature.mask_len = DIAG_INT_FEATURE_MASK_LEN;
  
  memscpy( (void*)&ctrl_msg.data.ctrl_msg_feature.mask[0], sizeof(ctrl_msg.data.ctrl_msg_feature.mask),
          &diag_int_feature_mask[0], 
          DIAG_INT_FEATURE_MASK_LEN );
    
#if defined(DIAG_MP_MASTER)
  for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_feature_type), 
                                        NULL, 0, DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, TRUE, &is_sent );
                                        
    if( is_sent == TRUE )
    {
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask_sent = TRUE;
    }
  }
#else
  (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_feature_type), 
                                      NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );
                                      
  if( is_sent == TRUE )
  {
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask_sent = TRUE;
  }
#endif
} /* diagcomm_ctrl_send_feature_mask */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_NUM_PRESETS

DESCRIPTION
   
PARAMETERS
  None

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_send_num_presets( void )
{
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  
  /* Compose DIAG_CTRL_MSG_NUM_PRESETS */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_NUM_PRESETS;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_num_presets_type);
  ctrl_msg.data.ctrl_msg_num_presets.num_presets = DIAG_MAX_PRESET_ID;
  
  (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_num_presets_type), 
                                      NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );
#endif

} /* diagcomm_ctrl_send_num_presets */

/*===========================================================================
FUNCTION diagcomm_ctrl_send_buf_retry

DESCRIPTION
   This function is used to retry sending of the control packets that were not sent earlier due to
   flow control on control channel. The CTRL packets are added to the CTRL queue and dequeued here.

RETURN
   TRUE if flow is enabled.
   FALSE if flow is disabled.

===========================================================================*/
void diagcomm_ctrl_send_buf_retry(diagcomm_enum_port_type port_num)
{
   diagcomm_io_tx_params_type tx_params;
   ctrl_resend_pkt_type *q_item;

   q_item = (ctrl_resend_pkt_type *)q_check(&diagcomm_ctrl_resend_q[port_num]);

   if(NULL == q_item)
   {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"q_get returned NULL");
      return;
   }
   while(q_item!=NULL)
   {
      tx_params.smdl_params.buffer = q_item->buf;
      tx_params.smdl_params.len = q_item->length;
      tx_params.smdl_params.flags = 0;

      if(DIAG_EOK != diagcomm_io_transmit( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num], &tx_params ))
      {
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"q_get returned NULL");
         break;
      }
       diag_ctrl_heap_free(q_item->buf);
       q_delete(&diagcomm_ctrl_resend_q[port_num],&q_item->qlink);
       diag_ctrl_heap_free(q_item);
       q_item = (ctrl_resend_pkt_type *)q_check(&diagcomm_ctrl_resend_q[port_num]);
   }
   if(NULL == q_item)
   {
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].diag_prev_tx_pending = FALSE;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].diag_resend_data = FALSE;
   }
} /* diagcomm_ctrl_send_buf_retry */

/*===========================================================================
FUNCTION diagcomm_ctrl_heap_init

DESCRIPTION
   .

RETURN
   DIAG_EOK: If successful

===========================================================================*/
static int diagcomm_ctrl_heap_init(void)
{
   mem_init_heap (&diag_ctrl_heap, diag_ctrl_heap_buf, sizeof (diag_ctrl_heap_buf), NULL);
   return DIAG_EOK;

} /* static int diagcomm_ctrl_heap_init()*/

/*===================================================================*/
static void * diag_ctrl_heap_malloc (uint32 num_bytes)
{
   void *buf = NULL;
   buf = mem_malloc (&diag_ctrl_heap, num_bytes);

   if (buf == NULL)
   {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diag_ctrl_heap_malloc allocation failed: %d",num_bytes);
   }
   else
   {
      diag_ctrl_heap_malloc_count++;
   }
  return buf;
} /* diag_ctrl_heap_malloc */

/*===================================================================*/
static void diag_ctrl_heap_free(void *mem_ptr)
{
   mem_free (&diag_ctrl_heap, mem_ptr);
   diag_ctrl_heap_free_count++;
   return;
} /* END diag_ctrl_heap_free */



