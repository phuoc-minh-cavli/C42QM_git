/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SMD

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SMD.

Copyright (c) 2001-2015, 2018-2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.mpss/3.10/services/diag/DCM/common/src/diagcomm_smd.c#4 $ $DateTime: 2019/09/30 01:26:23 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/19   kdey    Porting of Qsockets
04/23/18   ph      Separation of data and DCI send buffer macros.
11/23/15   ph      WP logging stopped issue fixes.
08/26/15   ph      Buffering mode revamp to make it functional.
06/11/15   sa      Fixed Compiler Warnings.
05/15/15   sr      Changed the SMDL FIFO size to 8k
04/08/15   sr      Diag over sockets and DSM removal
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
05/31/13   is      Support for peripheral buffering mode
01/03/13   sg      Command Response Channel separation
08/31/12   ra      Use accessor macros instead of accessing private task
                   info directly; accomodated changes for User space
                   task priority changes
05/25/12   sa      Replaced DIAG_REQ_FWD with DIAG_MP_MASTER
05/10/12   sa      Removed DIAG_MP_SLAVE_LAST
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
04/28/11   is      Add missing sleep vote
04/28/11   is      Fix compilation warnings
03/29/11   sg      Central Routing changes for Q6
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite
12/10/10   mad     cleanup: Got rid of init_done in diagcomm_smd_open()
12/08/10   mad     do not open SMD port if already open: do not reset
                   diagcomm_smd_connected
10/20/10   is      Resolve compilation issue in Apps only Diag
09/28/10   mad     Fixed compile error regarding  DIAGCOMM_SMD_1 and DIAGCOMM_SMD_2
09/14/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/25/10   sg      Fixed compiler warnings
05/25/10   JV      New task on master to process slave traffic
05/13/10   JV      Merge functionality in diagcomm_smd.c and diagcomm_smd2.c
                   in a single file
10/29/09   JV      Specify DSM_DIAG_SIO_TX_ITEM_POOL as pool ID for slave last
                   processor.
10/29/09   JV      Pass the DSM pool ID to SMD
09/23/09   JV      To initialize the smd_tx_wmq, use the sizes and counts from the
                   SIO_TX pool. This is the DSM pool it uses.
09/23/09   JV      Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
08/11/09   mad     flow-control is now independent of port open/close status
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/23/09   JV      Removed inclusion of task.h
06/23/09   JV      Included diagdsm_v.h instead of diagdsm.h. Featurized
                   inclusion of customer.h and target.h for wm.
05/28/09   mad     Mainlined item-level flow-control for SMD_TX pool
05/12/09   JV      Introduced the OS abstraction layer.
05/07/09   vk      changed includes from external to internal headers
04/10/09   mad     modified flow-control for SMD_TX pool, added SMD2 channel
03/20/09   mad     Changed DSM_MEM_LEVEL_MANY for SMD_TX flow-control,to keep
                   up with change in MANY level defined in diagdsmi_wce.h
02/19/09   mad     Inbound traffic from SMD is now processed in diag
                   task context, by handling DIAG_TX_SLAVE_SIG signal.
10/30/08   mad     Introduced flow-control for DSM_DIAG_SMD_TX_ITEM_POOL, at
                   item-level granularity. Took out watermark Q method of
                   flow-control.
08/14/08   sj      Added flow control function for higher threshold
05/17/07   jc      Proxy by tv.  Revise low water mark to 5 * DSM_DIAG_SMD_TX_ITEM_SIZ
12/15/06   as      Fixed compiler warnings.
11/21/06   as      Modified code to use diag internal features.
01/04/06   ptm     Change sio port name for SMD.
07/20/05   pjb     Changes to support DSM 3
06/15/05   cr      Fix CR#: 66743 - Surf crashes when diag is changed from
                   USB to UART1. Don't dequeue dsm_item or call sio_transmit
                   unless diagcomm is connected.
06/01/05   as      Added data transfer mode and transfer flow type to sio
                   open params under FEATURE_SMD.
05/17/05   as      Added Dual processor Diag support.
04/14/05   adm     Added FEATURE SMD check along with FEATURE_SMEM_DS
03/04/05    cr     Delete calls to dsm_reg_mem_event_cb because that was
                   completely breaking outbound flow control by leaving null
                   function pointer in diagcomm start/stop callback pointers
                   leading to DSM item exhausting and QXDM disconnect.
02/17/05   as      Removed async character check for APPS processor.
11/22/04    cr     Only compile diagcomm_smd for modem side, multi-processor
                   build.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
08/14/03   vsk     DSM 2.0 changes
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
05/08/02   lad     Moved dsm_simple_enqueue_isr() call above diagcomm_sio_cb
                   to enqueue data prior to potential context switch in cb.
02/27/02   dwp     Additional changes to use new DSM interface functions for
                   registering/handling memory events.
02/19/02   lad     Made changes to use new DSM interface functions for
                   registering/handling memory events.
01/21/02   as      Changed nv_sio_baud_type to nv_sio_baudrate_type
11/06/01   abp     Added diagcomm_flush_tx() to flush SIO transmit channel
10/30/01   abp     Added diagcomm_sio_close_to_open_cb().
                   In diagcomm_sio_close_done_cb() callback function ptr is
                   checked for NULL.
                   In diagcomm_open() added mechanism to close connection
                   before opening a new connection
10/09/01   ttl     Added second parameter to diagcomm_sio_tx_start_cb() and
                   diagcomm_sio_tx_stop_cb() due the DSM memory pool registration
                   changes.
09/17/01   jal     Fixed diagcomm_close() to notify RDM of completion
                   properly
05/07/01   lad     Cleaned up file.
04/17/01   dwp     Initialize diagcomm_smd_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#endif

#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_smd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "msg.h"
#include "diag_cfg.h"   /* For DIAGBUF_COMMIT_THRESHOLD */
#include "diagdiag_v.h" /* For DIAGDIAG_TX_MODE_CONFIG_APP */
#include "eventi.h"
#include "diagcomm_io.h"
#include "diagdsm_v.h"
#include "diag_mode_v.h" /* For diag_buffering_pool_used_cnt() */
#include "diagdsmi.h" /* for def. of DSMI_DIAG_SMD_TX_ITEM_FEW_MARK etc */
#include "ULogFront.h"

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#include "dsm.h"
#include "osal.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern volatile diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID]; /* From diag_mode.c */

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];      /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern uint32 diag_max_send_buf_len;

/* Queues around which the watermark queues are built */
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
static q_type diagcomm_smd_rx_q[NUM_SMD_PORTS];
#endif

/* Watermark queues used for communications with SIO */
dsm_watermark_type diagcomm_smd_rx_wmq[NUM_SMD_PORTS];
dsm_watermark_type diagcomm_smd_tx_wmq[NUM_SMD_PORTS];
extern dsm_watermark_type diagcomm_sio_buffering_tx_wmq[NUM_SIO_PORTS];

extern osal_timer_cnt_t diagbuf_tx_sleep_time; /* From diagbuf.c */

extern void diag_ctrl_send_drain_status( void );
uint32 diag_buffered_bytes(uint8 stream_index);

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
/* Call-backs for flow control at pool's item-level granularity.
Flow-control using watermark Qs work with the number of bytes,
 and that does not work reliably, because a pool might run out of items,
 but still have enough bytes left. */
static void diagcomm_smd_dsm_low_mem_event_cb (dsm_mempool_id_type,
                                               dsm_mem_level_enum_type,
                                               dsm_mem_op_enum_type);
static void diagcomm_smd_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op);
#endif


/*===========================================================================

FUNCTION DIAG_FWD_NOTIFY

DESCRIPTION

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
#if defined(DIAG_MP_MASTER)
void diag_fwd_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  uint32 dsm_cnt = 0;

  if( diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH )
  {
    dsm_cnt = diag_buffering_pool_used_cnt( DIAG_STREAM_1 );

    if( dsm_cnt >= diag_tx_mode.buffered_many_mark )
      diag_tx_mode.drain = TRUE;
    else if( diag_tx_mode.drain && (dsm_cnt <= diag_tx_mode.buffered_few_mark) )
      diag_tx_mode.drain = FALSE;
  }

  if( (diag_tx_mode.mode == DIAG_TX_MODE_STREAMING) || diag_tx_mode.drain )
  {
    if( diagcomm_sio_tx_wmq_flow_enabled() == TRUE )
    {
      /* Set signal to diag task to process traffic from slave */
      return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
  }

} /* diag_fwd_notify */
#endif /* DIAG_MP_MASTER */

uint32 diag_buffered_bytes(uint8 stream_index)
{
  if(diag_tx_mode[stream_index].buf_head >= diag_tx_mode[stream_index].buf_tail)
  return (diag_tx_mode[stream_index].buf_head - diag_tx_mode[stream_index].buf_tail);
  else
  return (diag_tx_mode[stream_index].buf_size - (diag_tx_mode[stream_index].buf_tail - diag_tx_mode[stream_index].buf_head));
}
/*===========================================================================

FUNCTION DIAGCOMM_BUFFER_ENQUEUE_STREAM

DESCRIPTION

PARAMETERS
  dsm_item_type
  diagcomm_port_type
  stream_id

RETURN VALUE
  None

===========================================================================*/
void diagcomm_update_buffer_params( uint8 stream_id, uint16 length )
{
   uint32 head = 0;
   uint32 tail = 0;
   diag_send_buf_header_type *diag_send_buf_header;
   int pad = 0;
   uint8 stream_index = 0;

   if ((stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID)) {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_update_buffer_params- Invalid stream_id (%d)", stream_id);

      return;
   }

   stream_index = DIAG_ID_TO_INDEX(stream_id);

   head = diag_tx_mode[stream_index].buf_head;
   tail = diag_tx_mode[stream_index].buf_tail;

   if (head == tail) {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_update_buffer_params - head = %d tail = %d", head, tail);
      return;
   }

   if (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR) {

      //If head is greater than tail, and there is not enough space in the buffer to accomodate the pkt, then we
      // zero out the buffer space from head till end of buffer and write the new packet from the start of the buffer.
      // We do this as in the current design, we dont support vector arrays
      if ((head > tail)) {
         memset(diag_tx_mode[stream_index].buf_ptr + head, 0, diag_tx_mode[stream_index].buf_size - head);
         diag_tx_mode[stream_index].curr_data_block_header = (diag_send_buf_header_type *)diag_tx_mode[stream_index].buf_ptr;
         diag_tx_mode[stream_index].curr_data_block_header->pattern = 0xDEAD;
         //diag_tx_mode[stream_index].curr_data_block_header->length = 0;
         head = sizeof(diag_send_buf_header_type);
      }
      while ((head + length > tail) && (tail < diag_tx_mode[stream_index].buf_size)) {
         diag_tx_mode[stream_index].dsm_dropped_cnt += 1;
         /*Move the tail to the next data block*/
         diag_send_buf_header = (diag_send_buf_header_type *)(diag_tx_mode[stream_index].buf_ptr + tail);
         if (diag_send_buf_header->length == 0) {
            ASSERT(0);
         }

         /*Calculate the new tail*/
         tail += diag_send_buf_header->length + sizeof(diag_send_buf_header_type);
         pad = DIAG_WORD_ALIGN(tail) - tail;
         tail += pad;

         /*Verify tail points to a valid data block*/
         if ((tail > diag_tx_mode[stream_index].buf_size) || ((diag_send_buf_header_type *)(diag_tx_mode[stream_index].buf_ptr + tail))->pattern != 0xDEAD) {

            tail = 0;
            head = sizeof(diag_send_buf_header_type);
            diag_tx_mode[stream_index].curr_data_block_header = (diag_send_buf_header_type *)diag_tx_mode[stream_index].buf_ptr;
            diag_tx_mode[stream_index].curr_data_block_header->pattern = 0xDEAD;
            diag_tx_mode[stream_index].curr_data_block_header->length = 0;
            break;
         }
         diag_send_buf_header->length = 0;
      }
   }

   diag_tx_mode[stream_index].buf_head = head;
   diag_tx_mode[stream_index].buf_tail = tail;

   diag_tx_mode[stream_index].buf_used = diag_buffered_bytes(stream_index);
}
/*===========================================================================

FUNCTION DIAG_DRAIN_POOL_CHECK (internal)

DESCRIPTION
  Checks whether Diag needs to set signal to drain the DSM pool. Diag should
  signal draining if we're passed the threshold in threshold buffering mode
  or if we're in streaming mode.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.

RETURN VALUE
  TRUE - Diag can drain the pool now.
  FALSE - Buffering pools are not ready to be drained yet.

===========================================================================*/
boolean diag_drain_pool_check( uint8 stream_id )
  {
  uint8 stream_index = 0;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
    {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return FALSE;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);

  if( diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
  /*Set drain as false if the data remaining to be drained is less than the lower threshold*/
    if( diag_tx_mode[stream_index].buf_used <= diag_tx_mode[stream_index].buffered_few_bytes_mark )
    {
        diag_tx_mode[stream_index].drain = FALSE;
        diag_ctrl_send_drain_status();
    }
  }

  if( diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH )
  {
    if( diag_tx_mode[stream_index].buf_used >= diag_tx_mode[stream_index].buffered_many_bytes_mark )
    {

      diag_tx_mode[stream_index].drain = TRUE;
    }
    else if( diag_tx_mode[stream_index].drain &&
            (diag_tx_mode[stream_index].buf_used <= diag_tx_mode[stream_index].buffered_few_bytes_mark) )
    {
      diag_tx_mode[stream_index].drain = FALSE;
    }
  }

  if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_STREAMING) || diag_tx_mode[stream_index].drain )
  {
    if(TRUE == diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAG_ID_TO_PORT(stream_id)].allow_flow)
    {
      return TRUE;
    }
  }

  return FALSE;

}

/*===========================================================================
FUNCTION DIAG_DRAIN_DSM_BUFFER (internal)

DESCRIPTION
  This function drains the buffering DSM pools per stream.

PARAMETERS
  stream_id - stream_id for buffering DSM pool to drain

RETURN VALUE
  None

===========================================================================*/
void diag_drain_dsm_buffer( uint8 stream_id )
{
  diagcomm_io_tx_params_type tx_params;
  uint8 port_index;
  uint8 stream_index;
  diag_send_buf_header_type *diag_send_buf_header = NULL;


  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
    return;

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  port_index = DIAG_ID_TO_PORT(stream_id);

  diag_send_buf_header = (diag_send_buf_header_type *)(diag_tx_mode[stream_index].buf_ptr + diag_tx_mode[stream_index].buf_tail);


   /* Start drain buffering if buffering checks pass. */
  if( diag_drain_pool_check(stream_id) && diagcomm_status(port_index ) && (diag_tx_mode[stream_index].buf_used >sizeof(diag_send_buf_header_type))
    && diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_index].allow_flow == TRUE)
  {
     if ((diag_send_buf_header->pattern != 0xDEAD) || (diag_send_buf_header->length > diag_max_send_buf_len)) {

        diag_tx_mode[stream_index].buf_tail = 0;
	diag_tx_mode[stream_index].buf_used = diag_tx_mode[stream_index].buf_head;
     } else {
        tx_params.params.buffer = diag_tx_mode[stream_index].buf_ptr + diag_tx_mode[stream_index].buf_tail + sizeof(diag_send_buf_header_type);
        tx_params.params.len = diag_send_buf_header->length;
        tx_params.params.flags = 0;

        diagcomm_io_transmit(&diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_index], &tx_params);

     }

     /* Set drain buffering signal if buffering checks passes and we are in buffering mode. */
     if (diag_drain_pool_check(stream_id) ) {
        if (stream_id == DIAG_STREAM_1) {
           diag_set_internal_sigs(DIAG_INT_DRAIN_DSM_BUFFER_SIG);
        }
     #ifdef DIAG_CONSUMER_API
        else if (stream_id == DIAG_STREAM_2) {
           diag_set_internal_sigs(DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG);
        }
     #endif /* DIAG_CONSUMER_API */
     }
  }
} /* diag_drain_dsm_buffer */

/*===========================================================================

FUNCTION DIAGCOMM_SMD_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
#if defined(DIAG_SIO_SUPPORT)
void
diagcomm_smd_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
#if defined(DIAG_MP_MASTER)
  if( diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    diagcomm_buffer_enqueue_stream( item_ptr, DIAGCOMM_PORT_SMD, DIAG_STREAM_1 );
  }
  else
  {
    diagcomm_buffer_enqueue_stream( item_ptr, DIAGCOMM_PORT_SMD, DIAG_STREAM_1 );
    diag_fwd_notify();
  }
#endif /* DIAG_MP_MASTER */

} /* diagcomm_smd_inbound_pkt_notify */
#endif /* DIAG_SIO_SUPPORT */


/*===========================================================================

FUNCTION DIAGCOMM_SMD2_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
#if defined(DIAG_SIO_SUPPORT) && !defined(DIAG_BUFFERING_SUPPORT)
#ifdef DIAG_CONSUMER_API
void
diagcomm_smd2_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  /* dsm item originated on APPS proc */
  dsm_enqueue (diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2].open_params.sio_params.params.rx_queue, item_ptr);

  /* Set signal to diag task to process slave traffic */
  return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

} /* diagcomm_smd2_inbound_pkt_notify */
 #endif /* #ifdef DIAG_CONSUMER_API */
 #endif /* DIAG_SIO_SUPPORT && !DIAG_BUFFERING_SUPPORT */

void diagcomm_smd_close_done_cb(void)
{
  diag_release ();

} /* diagcomm_smd_close_done_cb */

/* -------------------------------------------------------------------------
** Externalized routines (declared in diagcomm.h).
** ------------------------------------------------------------------------- */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_OPEN

DESCRIPTION
  Opens the communcications stream.  Returns boolean indicating success.

===========================================================================*/
boolean
diagcomm_smd_open (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];

  if( conn->connected )
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
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_smd_open - Couldn't get stream.");
    }
  }
  /* Register DTR callbacks to detect connection change. */
  diag_io_detect_connection( conn );

  return conn->connected;
} /* diagcomm_smd_open */

/*===========================================================================

FUNCTION DIAGCOMM_SMD_CLOSE

DESCRIPTION
  Closes the current connection, if any.

===========================================================================*/
void
diagcomm_smd_close ( diagcomm_enum_port_type port_num )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];

  if (DIAGCOMM_PORT_1 == conn->port_num)
  {
    #if (!defined (DIAG_MP_MASTER) && !defined (DIAG_QDSP6_APPS_PROC))
    return;
    #endif
  }
#ifdef DIAG_CONSUMER_API
  else if (DIAGCOMM_PORT_2 == conn->port_num)
  {
    #if !defined (DIAG_MP_MASTER)
    return;
    #endif
  }
#endif /* #ifdef DIAG_CONSUMER_API */
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
  }

  /* success or not, consider close operation successful  */
  conn->connected = FALSE;

} /* diagcomm_smd_close */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_CONN_CHANGED

DESCRIPTION

===========================================================================*/
void diagcomm_smd_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  /* Nothing needs to be done when SMD conn changed on Slave for now. */
#if defined(DIAG_MP_MASTER)
  uint8 preset_index;

  if( connected )
  {
    conn->channel_connected = TRUE;

    /* If connection just opened, Master needs to update all masks on Slave */
    if( DIAGCOMM_IO_CTRL == conn->channel_type )
    {
      /* Send feature flag mask to remote procs */
      diag_set_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );

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
      diag_set_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );
    }
  }
  else
  {
    conn->channel_connected = FALSE;

    for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
    {
      conn->mask_preset_update[preset_index] = DIAG_UPDATE_MASK_NONE;
    }
  #ifdef DIAG_CONSUMER_API
    conn->mask_dci_update[DIAG_DCI_MASK_1-1] = DIAG_UPDATE_MASK_NONE;
  #endif /* #ifdef DIAG_CONSUMER_API */
  }
#endif

  if( connected )
    conn->channel_connected = TRUE;
  else
    conn->channel_connected = FALSE;

} /* diagcomm_smd_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_STATUS

DESCRIPTION
  This function returns TRUE if the SMD communications layer has been
  opened locally and on the remote end.

PARAMETERS
  port_num - Data SMD channel to check for connection status

RETURN VALUE
  True - The Data SMD communications layer is opened locally and on the
         remote end.
  False - Otherwise.

===========================================================================*/
boolean
diagcomm_smd_status ( diagcomm_enum_port_type port_num )
{
  return (diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num].connected &&
          diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num].channel_connected);

} /* diagcomm_smd_status */



/*===========================================================================

FUNCTION DIAGCOMM_SMD_PROCESS_SLAVE_TX

DESCRIPTION
  This function sends APP diag traffic which came from SMEM to SIO.

===========================================================================*/
void diagcomm_smd_process_slave_tx(diagcomm_enum_port_type port_num)
{
  static dsm_item_type *diagcomm_app_item_ptr = NULL;
  diagcomm_io_tx_params_type tx_params;

  #ifdef DIAG_CONSUMER_API
  if (DIAGCOMM_PORT_2 == port_num)
  {
    #if defined (DIAG_MP_MASTER)
    {}
    #else
    return;
    #endif
  }
  #endif /* #ifdef DIAG_CONSUMER_API */
   
   if (diagcomm_status(DIAGCOMM_PORT_1))
   {
     diagcomm_app_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num] );
     if(diagcomm_app_item_ptr!=NULL)
     {
       tx_params.dsm_ptr = diagcomm_app_item_ptr;
       diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
     }
   }
} /* diagcomm_smd_process_slave_tx */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns forward RX watermark queue count.

===========================================================================*/
uint32 diagcomm_smd_get_rx_wmq_cnt(diagcomm_enum_port_type port_num)
{
  return diagcomm_io_get_rx_wmq_cnt( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num] );

} /* diagcomm_smd_get_rx_wmq_cnt */


#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
static boolean bAllowFlow = TRUE; /* Keep track of state of flow control. */

/*===========================================================================
FUNCTION diagcomm_smd_flow_enabled

DESCRIPTION
   This function tells whether the flow to DSM_DIAG_SMD_TX_ITEM_POOL is enabled
   or not.

===========================================================================*/
boolean diagcomm_smd_flow_enabled(void)
{
  return bAllowFlow;

} /* diagcomm_smd_flow_enabled */


/*===========================================================================
FUNCTION diagcomm_smd_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count
   for DSM_DIAG_SMD_TX_ITEM_POOL is reached.
   (DSM_DIAG_SMD_TX_ITEM_POOL is used by smd, to receive data.)

===========================================================================*/
static void diagcomm_smd_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op)
{
   uint32 flow_cnt = 0;

   /* Check to be defensive */
   if(pool_id != DSM_DIAG_SMD_TX_ITEM_POOL ||
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_NEW)
      return;

   if (TRUE == bAllowFlow)
   {
      diagcomm_io_disable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1] );
      #ifdef DIAG_CONSUMER_API
        diagcomm_io_disable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2] );
      #endif 
	  
      bAllowFlow = FALSE;
      flow_cnt = diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SMD);
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diagcomm_smd_dsm_high_mem_event_cb: disabled flow (cnt=%d)", flow_cnt);
   }

} /* diagcomm_smd_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_smd_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count
   for DSM_DIAG_SMD_TX_ITEM_POOL is reached.
   (DSM_DIAG_SMD_TX_ITEM_POOL is used by smd, to receive data.)

===========================================================================*/
static void diagcomm_smd_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                               dsm_mem_level_enum_type mem_level,
                                               dsm_mem_op_enum_type mem_op)
{
   osal_sigs_t return_sigs;
   int return_val = 0;

   /* check to be defensive */
   if(pool_id != DSM_DIAG_SMD_TX_ITEM_POOL ||
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_FREE)
      return;

   if (FALSE == bAllowFlow)
   {
      diagcomm_io_enable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1] );
      #ifdef DIAG_CONSUMER_API
        diagcomm_io_enable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2] );
      #endif
	  
      bAllowFlow = TRUE;

      diag_tx_notify();
      return_val = osal_set_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_smd_dsm_low_mem_event_cb */


/*===========================================================================
FUNCTION DIAGCOMM_SMD_SEND

DESCRIPTION
  Sends a packet over SMD channel

PARAMETERS
  req_pkt - packet to send
  pkt_len - length of 'req_pkt'
  port_num - channel to send pkt on

RETURN VALUE
  None

===========================================================================*/
void diagcomm_smd_send( void *req_pkt, uint16 pkt_len,
                        diagcomm_enum_port_type port_num )
{
  diagcomm_io_tx_params_type tx_params;
  dsm_item_type *app_req_item_ptr = NULL;
  diagcomm_io_conn_type * conn = NULL;
  dsm_mempool_id_type pool_id;
  uint16 write_len = 0;

    ASSERT(req_pkt != NULL);

  if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
  {
    conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][port_num];
    pool_id = DSM_DIAG_CMD_SMD_TX_ITEM_POOL;
  }
  else
  {
    conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];
    pool_id = DSM_DIAG_SMD_RX_ITEM_POOL;
  }

  if( diagcomm_status() && diagcomm_io_status(conn) )
  {
    write_len = dsm_pushdown_tail(&app_req_item_ptr, req_pkt, pkt_len, pool_id);

#if defined(DIAG_DSM_DEBUG)
    ASSERT( write_len == pkt_len );
#endif

    if( write_len == pkt_len )
    {
      tx_params.dsm_ptr = app_req_item_ptr;
      diagcomm_io_transmit( conn, &tx_params );
      diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
    }
    else
    {
      dsm_free_packet(&app_req_item_ptr);
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "diagcomm_smd_send - Ran out of DSM items on pool %d", pool_id);
    }
  }
} /* diagcomm_smd_send */
#endif /* DIAG_MP && DIAG_MP_MASTER*/

