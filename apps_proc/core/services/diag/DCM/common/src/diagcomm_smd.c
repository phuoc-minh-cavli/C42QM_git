/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SMD

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SMD.

Copyright (c) 2001-2016, 2019-2020 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_smd.c#2 $ $DateTime: 2020/04/15 23:22:31 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
03/02/16   nk      Add a mechanism to stop compression when alloc fails along 
                   with correction of total bytes in compression bufferin circular buffering mode.
03/23/16   ph      Use separate pool for TN traffic.
02/12/16   ph      Remove dead code with HDLC removal.
01/13/16   is      Send feature mask before processing commands and
                   propagate STM info in Diag context
09/25/15   sr      Added apps_field check for MPSS traffic on fwd channel
05/04/15   rh      Standardized mutex around total_bytes_in_compressed_buffer
04/22/15   sa      Fixed race conditions which may impact the draining from compression buffer.
04/16/15   rh      Added mutex around diagcomm_smd_process_slave_tx
01/27/15   sa      Trigger compression only if the internal buffer doesnt have space to copy 
                   the incoming DSM item.
10/07/14   sa      Protect updating the diag current preset id and diag tx mode drain variables.
09/10/14   xy      Fixed KW issue 
08/13/14   xy      Fixed 0x0FF8 size log packet HDLC encoding issue 
06/11/14   xy      Fixed 8.5K log issue  
05/19/14   sa      Added support for the new query command.
03/25/14   sa      Removed unused flow control callback functions.
03/12/14   xy      Add support for STM status   
03/10/14   sr      Fixed KW warnings
03/07/14   sa      Renamed DIAG_TX_SLAVE_SIG to DIAG_FWD_TX_SLAVE_SIG
02/26/14   sa      Added support for compression in buffering mode
02/05/14   xy      Fixed KW issue.  
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/27/14   is      Command/response separation
01/23/14   xy      Add support for enable/disable STM command  
12/16/13   is      Resolve race condition with changing DSM mem levels
12/05/13   sr      Added drain timer for forward channel  
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
11/01/13   xy      Store HDLC-encoding info in app_field for slave data on TN apps side 
                   when we receive data on forward channel  
10/01/13   xy      Removed code under FEATURE_WINCE 
05/24/13   rh      We now keep dropping DSM packets until we are under the
                   limit, when we are in circular buffering mode.
02/27/13   sr      Added support to HDLC encode the fwd channel traffic 
                   on APPS  
12/14/12   is      Resolve issues with DSM optimizations in buffering mode
11/27/12   rh      Replaced DIAG_REQ_FWD with DIAG_MP_MASTER
11/27/12   rh      Removed DIAG_MP_SLAVE_LAST
11/27/12   rh      Mainlining DIAG_CENTRAL_ROUTING
11/07/12   sr      Do not vote against sleep when in circular/threshold buffering mode 
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes 
06/20/12   rh      Set outgoing DSM pool to have a smaller high-usage threshold 
                   in streaming mode than in buffering mode. 
05/15/12   is      Allow "build time" mask queries from Master Diag 
02/08/12   is      Add health statistics for buffered threshold mode
01/31/12   is      Add flow control on diagcomm_sio_tx_wmq
01/24/12   is      Support for buffered circular mode
01/23/12   is      Apps flow control on SMD_RX pool instead of SIO_TX pool
12/08/11   is      ASSERT() when Diag runs out of DSM items
11/04/11   is      Support for buffered threshold mode
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

#include "comdef.h"
#include "customer.h"
#include "target.h" /* This is prob. not needed even on AMSS */

#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_smd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "msg.h"
#include "diagdiag_v.h" /* For DIAGDIAG_TX_MODE_CONFIG_APP */
#include "diag_cfg.h"   /* For DIAGBUF_COMMIT_THRESHOLD */
#include "eventi.h"
#include "dsm.h"
#include "diag.h"
#include "diagdsm_v.h"
#include "diagcompress_bufferi.h"

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#include "diagcomm_io.h"
#include "osal.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include"diagstub.h"
#else
#include "assert.h"
#endif

#include "diagdsmi.h" /* for def. of DSMI_DIAG_SMD_TX_ITEM_FEW_MARK etc */

#include "diag_compress_v.h"
#include "diag_fwd_v.h"

/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */

extern osal_mutex_arg_t diag_tx_mode_drain_mutex;

#if defined(DIAG_BUFFERING_SUPPORT)

/* Map Diag buffering mode levels to arbitrary DSM memory levels. 
   Only diag will register memory levels for Diag owned pools, so this 
   is okay. */
#define DIAG_DSM_MEM_LEVEL_BUF_FEW  DSM_MEM_LEVEL_WLAN_FEW
#define DIAG_DSM_MEM_LEVEL_BUF_MANY DSM_MEM_LEVEL_WLAN_MANY

volatile diag_tx_mode_type diag_tx_mode;
volatile uint8 diag_tx_mode_internal;
extern uint32 total_bytes_in_compressed_buffer;
extern uint32 total_length_to_compress;
extern uint32 num_dsm_items_to_process;
extern osal_mutex_arg_t diag_compression_mutex;

#endif

extern uint64 diagbuf_tx_sleep_threshold;
extern osal_timer_cnt_t diagbuf_tx_sleep_time;

#define DIAG_HDLC_CRC_PAD (3) /* 2 bytes CRC. 1 byte 7E terminator. */
static uint8 diagbuf_hdlc_pad_len = DIAG_HDLC_PAD_LEN;

extern unsigned int diag_fwd_drain_timer_len; /*From diag_fwd.c*/

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];      /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];  /* From diagcomm_cfg.c */

/* Queues around which the watermark queues are built */
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
static q_type diagcomm_smd_rx_q[NUM_SMD_PORTS];
#endif
static q_type diagcomm_smd_tx_q[NUM_SMD_PORTS];

diag_send_state_enum_type desc_state = DIAG_SEND_STATE_START;

/* Watermark queues used for communications with SIO */
dsm_watermark_type diagcomm_smd_rx_wmq[NUM_SMD_PORTS];
dsm_watermark_type diagcomm_smd_tx_wmq[NUM_SMD_PORTS];

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
                                                
                                               
#endif /* DIAG_MP && DIAG_MP_MASTER */

#if defined(DIAG_BUFFERING_SUPPORT)
PACK(void *) diagcomm_tx_mode_config( PACK(void *) req_pkt_ptr, uint16 pkt_len );

static const diagpkt_user_table_entry_type diagdiag_subsys_tbl_tx_mode_app[] =
{
  {DIAGDIAG_TX_MODE_CONFIG_APP, DIAGDIAG_TX_MODE_CONFIG_APP, diagcomm_tx_mode_config}
};
void diag_update_mode_info (uint8 mode);
#endif
extern boolean diag_hdlc_disabled;

/*===========================================================================

FUNCTION DIAGCOMM_SMD_TX_MODE_INIT

DESCRIPTION
  Initializes global variable for keeping track of streaming mode vs buffering mode.

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
#if defined(DIAG_BUFFERING_SUPPORT)
void diagcomm_smd_tx_mode_init( void )
{
  diag_tx_mode.mode = (uint8)DIAG_TX_MODE_STREAMING;
  diag_tx_mode_internal = diag_tx_mode.mode;
  diag_tx_mode.drain = FALSE;
  diag_tx_mode.buffered_upper_threshold = DIAG_COMPRESSION_UPPER_THRESHOLD;
  diag_tx_mode.buffered_lower_threshold = DIAG_COMPRESSION_LOWER_THRESHOLD;
  /* Calculate the number of bytes in the compressed buffer based on the upper and low  percentage levels */
  diag_tx_mode.buffered_many_mark = ((DIAGBUFC_SIZE * DIAG_COMPRESSION_UPPER_THRESHOLD)/100);
  diag_tx_mode.buffered_few_mark = ((DIAGBUFC_SIZE * DIAG_COMPRESSION_LOWER_THRESHOLD)/100);
    
  diag_health_reset_dropped_cnt();
  
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC( DIAG_APP_PROC, 
                                        DIAG_SUBSYS_DIAG_SERV, 
										diagdiag_subsys_tbl_tx_mode_app );

} /* diagcomm_smd_tx_mode_init */
#endif /* DIAG_BUFFERING_SUPPORT */


/*===========================================================================

FUNCTION DIAG_FWD_NOTIFY

DESCRIPTION

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_fwd_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  uint8 tx_mode = diag_get_tx_mode();
  boolean bDrainSet = diag_tx_mode.drain;
  
    
#if defined(DIAG_BUFFERING_SUPPORT)
  
/* If mode is changed to streaming mode from buffering, drain data (if any) in the compressed buffer */
  if( tx_mode == DIAG_TX_MODE_STREAMING )
  {
      return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    /*Reset the slave tx signal used in streaming mode */
    return_val = osal_reset_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);

    return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }
#else
  /* Set signal to diag task to process slave traffic */
  return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
#endif

} /* diag_fwd_notify */


/*===========================================================================

FUNCTION DIAGCOMM_BUFFER_ENQUEUE

DESCRIPTION

PARAMETERS
  dsm_item_type
  
RETURN VALUE
  None
  
===========================================================================*/
#if 0
#if defined(DIAG_BUFFERING_SUPPORT)
void diagcomm_buffer_enqueue( dsm_item_type ** item_ptr )
{
  dsm_item_type * dsm_dequeue_item_ptr = NULL;
  dsm_item_type * dsm_chained_ptr = NULL;
  uint32 dsm_cnt = 0;
  uint32 dsm_dropped_cnt = 0;
  uint32 bytes_dropped_cnt = 0;
  uint32 dsm_alloc_cnt = 0;
  uint32 bytes_alloc_cnt = 0;
    
  /* Dequeue and count dropped DSMs only in Buffered Cir mode. */
  if( diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    dsm_cnt = DSM_DIAG_SMD_TX_USED_CNT();
    diag_tx_mode.smd_cur_dsm_cnt = dsm_cnt; //Save dsm count read

    while( dsm_cnt >= diag_tx_mode.buffered_many_mark )
    {
      dsm_dequeue_item_ptr = diagcomm_io_dequeue_rx_wmq(&diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1]);
      ASSERT(dsm_dequeue_item_ptr != NULL);

      //bytes_dropped_cnt += dsm_length_packet( dsm_dequeue_item_ptr );
      dsm_chained_ptr = dsm_dequeue_item_ptr;
      do
      {
        dsm_dropped_cnt += 1;
        bytes_dropped_cnt += dsm_chained_ptr->used;

        // Point to next DSM in the chain
        dsm_chained_ptr = dsm_chained_ptr->pkt_ptr;

        // Continue to count chained DSMs
      } while( dsm_chained_ptr != NULL );
      //} while( (dsm_chained_ptr != NULL) && (dsm_chained_ptr->data_ptr != NULL) && (dsm_chained_ptr->used > 0) );

      // Update dropped count health statistics
      if( (diag_tx_mode.dsm_dropped_cnt + dsm_dropped_cnt) >= MAX_VALUE_UINT32 )
        diag_tx_mode.dsm_dropped_cnt = MAX_VALUE_UINT32;
      else
        diag_tx_mode.dsm_dropped_cnt += dsm_dropped_cnt;

      if( (diag_tx_mode.byte_dropped_cnt + bytes_dropped_cnt) >= MAX_VALUE_UINT32 )
        diag_tx_mode.byte_dropped_cnt = MAX_VALUE_UINT32;
      else
        diag_tx_mode.byte_dropped_cnt += bytes_dropped_cnt;

      // Free the dequeued DSM item, including it's chained items
      dsm_free_packet(&dsm_dequeue_item_ptr);
      dsm_cnt = DSM_DIAG_SMD_TX_USED_CNT();
      diag_tx_mode.smd_cur_dsm_cnt = dsm_cnt; //Save dsm count read
    }
  }

  /* We don't drop/dequeue DSMs in Buffered Threshold mode. Just count alloc
     counts and enqueue. */
  if( (diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH) || 
      (diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
	//bytes_alloc_cnt += dsm_length_packet( *item_ptr );
	dsm_chained_ptr = *item_ptr;
	do
	{
	   dsm_alloc_cnt += 1;
	   bytes_alloc_cnt += dsm_chained_ptr->used;
	   
	   // Point to next DSM in the chain 
       dsm_chained_ptr = dsm_chained_ptr->pkt_ptr;
	   
	  // Continue to count chained DSMs 
	} while( dsm_chained_ptr != NULL );
	
	// Update alloc count health statistics 
	if( (diag_tx_mode.dsm_alloc_cnt + dsm_alloc_cnt) >= MAX_VALUE_UINT32 )
	  diag_tx_mode.dsm_alloc_cnt = MAX_VALUE_UINT32;
	else
	  diag_tx_mode.dsm_alloc_cnt += dsm_alloc_cnt;
	  
	if( (diag_tx_mode.byte_alloc_cnt + bytes_alloc_cnt) >= MAX_VALUE_UINT32 )
	  diag_tx_mode.byte_alloc_cnt = MAX_VALUE_UINT32;
	else
	  diag_tx_mode.byte_alloc_cnt += bytes_alloc_cnt;
	
	dsm_enqueue (diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr);
  }
  else
  {
    /* Streaming mode; Just enqueue it. */
    dsm_enqueue (diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr);
  }

} /* diagcomm_buffer_enqueue */
#endif /* DIAG_BUFFERING_SUPPORT */
#endif 

/*===========================================================================

FUNCTION DIAGCOMM_ENQUEUE_DSM

DESCRIPTION

  This function enqueues the data onto the diag's watermark queue.

===========================================================================*/
void
diagcomm_enqueue_dsm (dsm_item_type ** item_ptr)
{

  /* Enqueue the dsm items in the rx queue */
  dsm_enqueue (&diagcomm_smd_rx_wmq[DIAGCOMM_PORT_1], item_ptr);
  diag_fwd_notify();

} /*diagcomm_enqueue_dsm  */


/*===========================================================================

FUNCTION DIAG_CMD_CODE_CHECK

DESCRIPTION
  This function returns TRUE when the data being
  sent is not event/log/F3

===========================================================================*/
boolean
diag_cmd_code_check( uint8 pkt_cmd_type )
{
  if( (pkt_cmd_type != DIAG_LOG_F) && (pkt_cmd_type != DIAG_MSG_F) && 
      (pkt_cmd_type != DIAG_EVENT_REPORT_F) && ( pkt_cmd_type != DIAG_EXT_MSG_F) && 
      ( pkt_cmd_type != DIAG_EXT_MSG_CONFIG_F) && ( pkt_cmd_type != DIAG_EXT_MSG_TERSE_F) && 
      ( pkt_cmd_type != DIAG_QSR_EXT_MSG_TERSE_F) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}/* diag_cmd_code_check */

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
  return( diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num].connected &&
          diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num].channel_connected );

} /* diagcomm_smd_status */


/* mutex added to fix concurrency bugs in diagcomm_smd_process_slave_tx */
extern osal_crit_sect_t diag_slave_tx_mutex;

/*===========================================================================

FUNCTION DIAGCOMM_SMD_PROCESS_SLAVE_TX

DESCRIPTION
  This function sends APP diag traffic which came from SMEM to SIO.
  If required, this function HDLC encodes the data before sending it out.

PARAMETER
port_num   Indicates the port number from which the data has to be sent/dequeued
           If this is -1, then any pending data is flushed out
===========================================================================*/
#define DIAGCOMM_SMD_READ_UINT16(ptr) (((uint8*)(ptr))[0] | (((uint8*)(ptr))[1] << 8))

uint32 diagcomm_smd_process_slave_tx(diagcomm_enum_port_type port_num)
{
  static dsm_item_type *diagcomm_app_item_ptr = NULL;
  static dsm_item_type *diagcomm_app_item_ptr_initial = NULL;
  diagcomm_io_tx_params_type tx_params;
  static dsm_item_type *item_ptr = NULL;
  static dsm_item_type *item_ptr_compressed = NULL;
  static diag_hdlc_dest_type enc = {NULL, NULL, 0};
  uint16 *pkt_length = NULL;
  uint8 hdlc_pad = 0;
  int move_data_ptr = 0;
  uint8 *data_ptr = NULL;
  uint8 *data_ptr_end = NULL;
  unsigned int outbound_used = 0;
  static uint64 tx_cnt_sav = 0;
  uint8 *pkt_length_msb = NULL;
  uint8 *pkt_length_lsb = NULL;
  static byte* outbound = NULL;
  boolean chained_dsm = FALSE;
  boolean dsm_chain = FALSE;
  static uint16 pkt_length_bytes = 0;
  uint16 bytes_remaining = 0;
  static boolean first_packet = TRUE;
  static boolean new_chain = FALSE;
  boolean dsm_full = FALSE;
  uint64 tx_cnt = tx_cnt_sav;
  diag_send_desc_type send_desc_data = {NULL, NULL, DIAG_SEND_STATE_START, FALSE, FALSE};
  diag_send_desc_type *send_desc = &send_desc_data;
  uint8* data_to_be_compressed = NULL;
  uint16 length_to_be_compressed = 0;
  uint32 num_dsm_items_send = 0;
  uint32 dsm_item_count= 0;
  static uint32 diag_num_chained_dsm = 0;
  uint32 dsm_item_length = 0;
  boolean bDrain = FALSE;
  uint8 mode;
  boolean move_flag = FALSE;
  uint32 debug_slave_tx_len = 0;  /*Track count received from modem */
  uint32 debug_slave_parse_len = 0; /* Track the count parsed by APPS */

  #ifdef DIAG_CONSUMER_API
    if (DIAGCOMM_PORT_2 == port_num)
    {
      #if defined (DIAG_MP_MASTER) && defined (FEATURE_DIAG_8K_QDSP6)
        {}
      #else
        return;
      #endif
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  osal_enter_crit_sect(&diag_slave_tx_mutex);

  /* Flush the buffer to the comm layer. */
  /* The DIAGCOMM_PORT_NONE case needs to be here to flush the Apps-HDLC encoded DSM item
     because the static variable pointing to the DSM item to flush is in this function*/

  if((port_num == DIAGCOMM_PORT_NONE ) && (diagcomm_status()))
  {
    if( (outbound != NULL) && (item_ptr != NULL) )
    {
      outbound_used = (uint32)enc.dest - (uint32)outbound;
      item_ptr->used = outbound_used;
      diagcomm_send_hdlc(item_ptr, outbound_used);
      tx_cnt += outbound_used;

      outbound = NULL;
      item_ptr = NULL;
    }
    osal_exit_crit_sect(&diag_slave_tx_mutex);
    return 0;
  }
  if (port_num < DIAGCOMM_PORT_NUM_FIRST || port_num > DIAGCOMM_PORT_NUM_LAST)
  {
    osal_exit_crit_sect(&diag_slave_tx_mutex);
    return 0;
  }
  /* Check the mode to which the target is configured */
  mode = diag_get_tx_mode();


  if( mode == DIAG_TX_MODE_BUFFERED_THRESH || mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    do
    {
      diagcomm_app_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num] );
      if(diagcomm_app_item_ptr!=NULL)
      {
        item_ptr_compressed = diagcomm_app_item_ptr;
        /* Get the length of the DSM item */
        dsm_item_length = dsm_length_packet(item_ptr_compressed);

        /* If the internal buffer cannot accomodate current DSM item, trigger compression */
        if ( (DIAG_COMPRESSION_INT_BUF_SIZE - total_length_to_compress) < dsm_item_length )
        {
          /* if there is any data in the internal buffer, trigger compression for the same*/
          if(total_length_to_compress > 0)
          {
            bDrain = TRUE;
            /*
              If we are unable to drain from buffer_to_compress even though bDrian is set
              (due to not able to allocate from diagcompressed_buf), then we don't make
              buffer_to_compress index to 0. So don't send into buffer_to_compress unless
              we confirm it is sent to diagcompressed buffer
            */
            diag_compress(NULL, 0, bDrain);
            /*
              Total_length_to_compress will be 0 if diag_compress() succeeds,
              and partial compression does not take place
            */
            if(total_length_to_compress > 0)
            {
               break;
            }
          }
        }
        do
        {
          bDrain = FALSE;
          data_to_be_compressed = item_ptr_compressed->data_ptr;
          length_to_be_compressed = item_ptr_compressed->used;
          num_dsm_items_send++;
          ++dsm_item_count;          
          /* Compress the data in buffering mode */
          diag_compress(data_to_be_compressed, length_to_be_compressed, bDrain);
          diag_compress_kick_watchdog();

          /* Get the chained DSM item (if any) */
          item_ptr_compressed = item_ptr_compressed->pkt_ptr;

          if(item_ptr_compressed)
            diag_num_chained_dsm++;
        }while(item_ptr_compressed != NULL);
        dsm_free_packet(&diagcomm_app_item_ptr);
      }
      else
      {
        break;
      }
      //TODO: else break and remove the app item ptr check at the end of main do-while
      /* check the mode again and exit from the loop if moved to streaming mode */
      mode = diag_get_tx_mode();

      if(mode == DIAG_TX_MODE_STREAMING)
        break;
    }
    while(dsm_item_count < num_dsm_items_to_process);
    // There are 4 conditions in which loop can exit. In any case just compress the leftover data
    //1. dsm_item_count>num_dsm_items_to_process 2. item_ptr_compressed ==NULL 
    //3. Mode is changed to streaming 4.Partial compression happened.
	if(total_length_to_compress!=0)
        {
     	 //Call diag_compress just to send the data in the intermediate compression buffer. 
      	   diag_compress(NULL,0,TRUE);
	 /* Total_length_to_compress will be 0 if diag_compress() succeeds, and partial compression does not take place    */
           if(total_length_to_compress > 0)
           { 
              
	      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Data not fully compressed total_length_to_compress=%d \n",total_length_to_compress);
           }
	}
  }
  else if(mode == DIAG_TX_MODE_STREAMING)
  {
    if (diagcomm_status() && diagcomm_sio_tx_flow_enabled())
    {
        diagcomm_app_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num] );
        diagcomm_app_item_ptr_initial = diagcomm_app_item_ptr;

        if(diagcomm_app_item_ptr!=NULL)
        {
          if(diagcomm_app_item_ptr->pkt_ptr!= NULL)
          {
            dsm_chain = TRUE;
          }

          //Count bytes coming from slave in non-hdlc format
          debug_slave_tx_len += dsm_length_packet(diagcomm_app_item_ptr);

          /*Check if the data is HDLC encoded or not*/
          if(diag_hdlc_disabled)
          {
            tx_params.dsm_ptr = diagcomm_app_item_ptr;
            diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
          }
          else
          {
            data_ptr = diagcomm_app_item_ptr->data_ptr;
            if( diagcomm_app_item_ptr->used <= 0 )
            {
              osal_exit_crit_sect(&diag_slave_tx_mutex);
              return 0;
            }
            data_ptr_end = data_ptr + diagcomm_app_item_ptr->used - 1;

            /*check if data_ptr is NULL*/
            ASSERT(data_ptr != NULL);

            while(data_ptr <= data_ptr_end)
            {
              /*Second chained DSM item*/
              if( chained_dsm )
              {
                send_desc->pkt = data_ptr;
                send_desc->priority = diagcomm_app_item_ptr->priority;
                send_desc->state = DIAG_SEND_STATE_BUSY;
                send_desc->terminate = TRUE;
                send_desc->last = data_ptr + bytes_remaining - 1;
                data_ptr = data_ptr + bytes_remaining + 1;
                debug_slave_parse_len+= (bytes_remaining + 1);

                first_packet = FALSE;
                chained_dsm = FALSE;
                bytes_remaining = 0;
              }
              else /*Not a chained DSM item*/
              {
                /*The packet is of the format: |7E(1 byte)|01(1 byte)|length(2 bytes)|payload(variable)|7E(1 byte)| */

                /*The entire packet header is present in the first dsm*/
                if(data_ptr+3 < data_ptr_end)
                {
                  /*There are two cases here-
                  1)The entire packet is in the DSM item
                  2)The packet is split in two dsm items */

                  //Move the data pointer to point to length field
                  data_ptr += 2;
                  pkt_length = (uint16 *)data_ptr;
                  pkt_length_bytes = *pkt_length;
                  /*Move the data pointer to point to the payload*/
                  data_ptr += 2;

                  debug_slave_parse_len+= 4;
                  //The packet is split in two dsm items
                  if(data_ptr + pkt_length_bytes > data_ptr_end + 1)
                  {
                    ASSERT(diagcomm_app_item_ptr->pkt_ptr != NULL);
                    send_desc->pkt = data_ptr;
                    send_desc->terminate = FALSE;
                    send_desc->last = data_ptr_end;
                    send_desc->state = DIAG_SEND_STATE_START;
                    send_desc->priority = diagcomm_app_item_ptr->priority;
                    bytes_remaining =  pkt_length_bytes - (data_ptr_end - data_ptr + 1);
                    debug_slave_parse_len+= (data_ptr_end - data_ptr + 1);

                  }
                  else /*The entire packet is in the DSM item */
                  {
                    send_desc->pkt = data_ptr;
                    send_desc->last = data_ptr+pkt_length_bytes-1;
                    send_desc->terminate = TRUE;
                    send_desc->state = DIAG_SEND_STATE_START;
                    send_desc->priority = diagcomm_app_item_ptr->priority;
                    data_ptr = data_ptr + pkt_length_bytes+1;

                    debug_slave_parse_len+= (pkt_length_bytes + 1);

                    //The packet exactly fits into DSM or terminating 0x7E is present in chained DSM.
                    if ( (data_ptr ==(data_ptr_end + 1)) || (data_ptr == (data_ptr_end + 2)))
                    {
                      if( data_ptr == (data_ptr_end + 2))
                      {
                        /*Terminating 0x7E is present in chained DSM */
                        move_flag = TRUE;
                      }

                      if(dsm_chain)
                      {
                        ASSERT(diagcomm_app_item_ptr->pkt_ptr != NULL);
                        /*Update the data_ptr to point to the chained dsm*/
                        diagcomm_app_item_ptr = diagcomm_app_item_ptr->pkt_ptr;
                        data_ptr = diagcomm_app_item_ptr->data_ptr;
                        data_ptr_end = data_ptr + (diagcomm_app_item_ptr->used)-1;

                        if(move_flag)
                        {
                          /*Move past the previous packet terminating 0x7E character*/
                          data_ptr++;
                        }
                        /*Mark False to avoid parsing again */
                        dsm_chain = FALSE;
                      }
                      else
                      {
                        /*No chaining, do nothing as the current DSM will be sent */
                      }
                    }
                  }
                }
                /*Only the packet header is present in the current dsm*/
                else if((data_ptr+3 == data_ptr_end ))
                {
                  ASSERT(diagcomm_app_item_ptr->pkt_ptr != NULL);
                  /*Move the data pointer to point to length field*/
                  data_ptr += 2;
                  pkt_length = (uint16 *)data_ptr;
                  pkt_length_bytes = *pkt_length;
                  debug_slave_parse_len+= 4;


                  /*Update the data_ptr to point to the chained dsm*/
                  diagcomm_app_item_ptr = diagcomm_app_item_ptr->pkt_ptr;
                  data_ptr = diagcomm_app_item_ptr->data_ptr;
                  data_ptr_end = data_ptr + diagcomm_app_item_ptr->used-1;
                  send_desc->priority = diagcomm_app_item_ptr->priority;
                  send_desc->pkt = data_ptr;
                  send_desc->last = data_ptr + pkt_length_bytes-1;
                  send_desc->state = DIAG_SEND_STATE_START;
                  send_desc->terminate = TRUE;
                  data_ptr = data_ptr + pkt_length_bytes+1;

                  /*diagcomm_app_item_ptr is set to chained DSM, so
                  mark dsm_chain as false to avoid parsing again */
                  dsm_chain = FALSE;
                  debug_slave_parse_len+= (pkt_length_bytes +1);

                }
                /*The first three bytes of the packet header is present in the current dsm*/
                else if(data_ptr + 2 == data_ptr_end )
                {
                  //length1 and length2 are in different DSM items
                  ASSERT(diagcomm_app_item_ptr->pkt_ptr != NULL);
                  pkt_length_lsb = data_ptr + 2;

                  /*Update the data_ptr to point to the chained dsm*/
                  diagcomm_app_item_ptr = diagcomm_app_item_ptr->pkt_ptr;
                  data_ptr = diagcomm_app_item_ptr->data_ptr;
                  data_ptr_end = data_ptr + diagcomm_app_item_ptr->used-1;
                  pkt_length_msb = data_ptr;
                  data_ptr++;
                  pkt_length_bytes = (uint16)*pkt_length_msb;
                  pkt_length_bytes = (pkt_length_bytes << 8) | *pkt_length_lsb;
                  send_desc->priority = diagcomm_app_item_ptr->priority;
                  send_desc->pkt = data_ptr;
                  send_desc->last = data_ptr + pkt_length_bytes-1;
                  send_desc->state = DIAG_SEND_STATE_START;
                  send_desc->terminate = TRUE;
                  data_ptr = data_ptr + pkt_length_bytes+1;
                  /*diagcomm_app_item_ptr is set to chained DSM, so
                  mark dsm_chain as false to avoid parsing again */
                  dsm_chain = FALSE;

                  debug_slave_parse_len+= 4;
                  debug_slave_parse_len+= (pkt_length_bytes +1);

                }
                /*The first two (or one) bytes of the packet header is present in the first dsm*/
                else if((data_ptr+1 == data_ptr_end) || (data_ptr == data_ptr_end))
                {
                  if(data_ptr == data_ptr_end)
                  {
                    /*Move the data pointer to point to length field*/
                    move_data_ptr = 1;
                  }
                     //Only 7E is present in the first DSM item
                  ASSERT(diagcomm_app_item_ptr->pkt_ptr != NULL);
                  /*Update the data_ptr to point to the chained dsm*/
                  diagcomm_app_item_ptr = diagcomm_app_item_ptr->pkt_ptr;
                  data_ptr = diagcomm_app_item_ptr->data_ptr;
                  data_ptr_end = data_ptr + (diagcomm_app_item_ptr->used)-1;
                  if(move_data_ptr)
                  {
                    data_ptr++;
                  }
                  pkt_length = (uint16 *)data_ptr;
                  pkt_length_bytes = *pkt_length;

                  /*diagcomm_app_item_ptr is set to chained DSM, so
                  mark dsm_chain as false to avoid parsing again */
                  dsm_chain = FALSE;

                  /*Move the data pointer to point to the payload*/
                  data_ptr += 2;

                  debug_slave_parse_len+= 4;

                  send_desc->pkt = data_ptr;
                  send_desc->last = data_ptr+pkt_length_bytes-1;
                  send_desc->terminate = TRUE;
                  send_desc->state = DIAG_SEND_STATE_START;
                  send_desc->priority = diagcomm_app_item_ptr->priority;
                  data_ptr = data_ptr + pkt_length_bytes+1;
                  debug_slave_parse_len+= (pkt_length_bytes +1);
                }
              }

              if (outbound == NULL)
              {

                /* Allocate memory from the communications layer */
                item_ptr = diagcomm_outbound_alloc_hdlc(&outbound_used, FALSE, send_desc->priority);
                new_chain = FALSE;

                if (item_ptr)
                  outbound = item_ptr->data_ptr;
                else
                  outbound = NULL;


                enc.dest =  (void*)outbound;
                enc.dest_last = ( void*)((byte*)outbound + outbound_used - 1);

              }

              if (outbound == NULL)
              {
                /* In case this is the start state, call encode to transition the
                state to "busy".  Some callers use this state to detect a need
                for other initialization.  Forcing state transition avoids
                multiple initialization. */
                enc.dest = enc.dest_last = NULL;
                diag_hdlc_encode (send_desc, &enc);
              }
              else
              {

                outbound_used = (uint32) enc.dest - (uint32) outbound;

                if( send_desc->terminate )
                {
                  /* If terminal packet, allow for minimal padding. */
                  hdlc_pad = MIN(((int)send_desc->last - (int)send_desc->pkt) + DIAG_HDLC_CRC_PAD, diagbuf_hdlc_pad_len);
                }
                else
                {
                  /* If non-terminal packet, add more padding to account for the rest of the packet needing to fit in
                  same DSM chain. */
                  hdlc_pad = diagbuf_hdlc_pad_len;
                }

                /* The condition below checks if the packet is a packet whose source size won't
                fit in the existing DSM item size AND that the packet is not a large packet
                which wouldn't fit in the DSM - if the condition is TRUE send the existing data
                right away and allocate a new buffer for this packet to avoid chaining.

                If first_packet!=TRUE, then this is part of the previous packet and we need to allow
                it to be chained even if the calculations will exceed DSM size here.  The previous
                packet must not have been a terminal packet. */

              if (((((int)send_desc->last - (int)send_desc->pkt) + outbound_used + hdlc_pad) >= DIAGBUF_SIO_TX_ITEM_SIZ)
                    && (TRUE == first_packet) && ((int)send_desc->last - (int)send_desc->pkt) <= DIAGBUF_SIO_TX_ITEM_SIZ && (!send_desc->priority))
              {
                // We are assuming that this check always passes, but,
                // if it doesn't, any existing data will get discarded.
                if (item_ptr)
                {
                  item_ptr->used = outbound_used;
                  diagcomm_send_hdlc(item_ptr, outbound_used);
                  tx_cnt += outbound_used;
                  /*Reset the forward drain timer here if its set*/
                  diag_clr_fwd_drain_timer();
                }

                outbound = NULL;
                item_ptr = diagcomm_outbound_alloc_hdlc(&outbound_used, FALSE, FALSE);

                if (item_ptr) {
                  outbound = item_ptr->data_ptr;
                  enc.dest = (void *) outbound;
                  enc.dest_last = (void *) ((byte *) outbound + outbound_used - 1);
                }
                else
                  outbound = NULL;
              }

              if (outbound) diag_hdlc_encode (send_desc, &enc);

              /* Check for case if exactly DSM SIZE or DSM_SIZE - 1 bytes are in dsm, then packet is full,
                  needs to be sent right away */
              if ((enc.dest >= enc.dest_last) && (send_desc->pkt > send_desc->last ) && send_desc->state == DIAG_SEND_STATE_COMPLETE)
              {
                dsm_full = TRUE;
              }
              else
              {
                  /* Check to see if the packet fit, if not loop through and chain the end of packet*/
                  while  ((enc.dest >= enc.dest_last) && (send_desc->state != DIAG_SEND_STATE_COMPLETE)
                        && (item_ptr)) {

                  outbound_used = (uint32) enc.dest - (uint32) outbound;
                  tx_cnt += outbound_used;
                  /* Update the used field for the current DSM item */
                  item_ptr->used = outbound_used;

                  item_ptr = diagcomm_outbound_alloc_hdlc (&outbound_used, TRUE, send_desc->priority);
                  if (item_ptr) {
                          outbound = item_ptr->data_ptr;
                          new_chain = TRUE;
                          enc.dest = (void *) outbound;
                          enc.dest_last = (void *) ((byte *) outbound + outbound_used - 1);
                          /* Fit the remainder of the packet */
                          diag_hdlc_encode (send_desc, &enc);
                  }

                  else outbound = NULL;
                }
              }

              /* If it is a new chain and a terminal packet - send the data OR
              if it is a priority packet send the data */
              if( ((send_desc->terminate) && (new_chain || dsm_full) && (item_ptr)) || ((send_desc->priority) && (item_ptr)) )
              {

                outbound_used = (uint32)enc.dest - (uint32)outbound;
                item_ptr->used = outbound_used;

                diagcomm_send_hdlc(item_ptr, outbound_used);

                tx_cnt += outbound_used;
                outbound = NULL;
                new_chain = FALSE;
                dsm_full = FALSE;

                /*Reset the forward drain timer here if its set*/
                diag_clr_fwd_drain_timer();
              }
              else if((send_desc->terminate) && (item_ptr))
              {
                /*Set the timer to send the DSM item after the timer expires.
                This takes care of the case when there is less traffic in
                fwd channel and diag is waiting to accumulate more pkts in
                DSM but no more pkts are received*/
                diag_set_fwd_drain_timer();

              }

              if (send_desc->terminate)
              {
                first_packet = TRUE;
              }
              else
              {
                first_packet = FALSE;
              }

            }
            if((bytes_remaining != 0) && (diagcomm_app_item_ptr->pkt_ptr != NULL))
            {
              diagcomm_app_item_ptr = diagcomm_app_item_ptr->pkt_ptr;
              chained_dsm = TRUE;

              /*diagcomm_app_item_ptr is set to chained DSM, so
              mark dsm_chain as false to avoid parsing again */
              dsm_chain = FALSE;
              data_ptr = diagcomm_app_item_ptr->data_ptr;
              data_ptr_end = data_ptr + diagcomm_app_item_ptr->used - 1;
            }

          }

          if(debug_slave_parse_len != debug_slave_tx_len)
          {
            ASSERT(0);
          }
          dsm_free_packet(&diagcomm_app_item_ptr_initial);
        }
      }
    }
    osal_exit_crit_sect(&diag_slave_tx_mutex);
    return 1;
  }
  osal_exit_crit_sect(&diag_slave_tx_mutex);
  return num_dsm_items_send;
} /* diagcomm_smd_process_slave_tx */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns forward RX watermark queue count.

===========================================================================*/
uint32 diagcomm_smd_get_rx_wmq_cnt(diagcomm_enum_port_type port_num)
{
  return diagcomm_io_get_wmq_cnt( &diagcomm_smd_rx_wmq[port_num] );

} /* diagcomm_smd_get_rx_wmq_cnt */



/*===========================================================================

FUNCTION DIAGCOMM_SMD_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_smd_init (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type *conn = NULL;

  if( (port_num < DIAGCOMM_PORT_NUM_FIRST) || (port_num > DIAGCOMM_PORT_NUM_LAST) )
  {
    return;
  }

conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];
  /* Make sure we are closed */
  diagcomm_smd_close( port_num );

  /* Initialize watermark structures */
  memset (&diagcomm_smd_rx_wmq[conn->port_num], 0, sizeof (dsm_watermark_type));
  

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
  if (DIAGCOMM_PORT_1 == conn->port_num)
  {
  /* Adding flow control at pool's item-level granularity for streaming mode.
     Lower flow control levels prevents Diag from unable to catch up with draining 
     data if SIO outbound is slow. */
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SMD_TX_ITEM_POOL,                                /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_MANY,                                       /* dsm_mem_level_enum_type level */
    DSMI_DIAG_SMD_TX_ITEM_MANY_MARK /* uint32 avail_item_count */
  );
  
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SMD_TX_ITEM_POOL,                               /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_FEW,                                       /* dsm_mem_level_enum_type level */
    DSMI_DIAG_SMD_TX_ITEM_FEW_MARK /* uint32 avail_item_count */
  );

  dsm_reg_mem_event_cb
  (
   DSM_DIAG_SMD_TX_ITEM_POOL,                                 /* dsm_mempool_id_type      pool_id*/
   DSM_MEM_LEVEL_FEW,                                        /* dsm_mem_level_enum_type  level */
   DSM_MEM_OP_NEW,                                            /* dsm_mem_op_enum_type     mem_op */
   diagcomm_smd_dsm_high_mem_event_cb                         /* pointer to call-back function */
  );
  
  dsm_reg_mem_event_cb
  (
   DSM_DIAG_SMD_TX_ITEM_POOL,                                 /* dsm_mempool_id_type      pool_id*/
   DSM_MEM_LEVEL_MANY,                                         /* dsm_mem_level_enum_type  level */
   DSM_MEM_OP_FREE,                                           /* dsm_mem_op_enum_type     mem_op */
   diagcomm_smd_dsm_low_mem_event_cb                          /* pointer to call-back function */
  );

  /* Set up Queue for receiving data from SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_rx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_rx_q[conn->port_num]                        /* Queue to hold the items */
  );

  conn->rx_wmq_ptr = &diagcomm_smd_rx_wmq[conn->port_num];

  /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_smd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ;
 
    dsm_set_low_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));


  /* Set up Queue for sending data via SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_tx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_tx_q[conn->port_num]                        /* Queue to hold the items */
  );

  /* Set up watermarks for WaterMark Queue. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
  }
  #ifdef DIAG_CONSUMER_API
  else if (DIAGCOMM_PORT_2 == conn->port_num)
  {
    /* Set up Queue for receiving data from Q6. */
    dsm_queue_init
    (
      &diagcomm_smd_rx_wmq[conn->port_num],                   /* Watermark queue structure */
      DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,    /* Do not exceed count */
      &diagcomm_smd_rx_q[conn->port_num]                      /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_smd_rx_wmq[conn->port_num].lo_watermark = DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ;
    diagcomm_smd_rx_wmq[conn->port_num].hi_watermark = DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ;
    diagcomm_smd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ;

    dsm_set_low_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));

    dsm_set_hi_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));

    /* Set up Queue for sending data via SIO. */
    dsm_queue_init
    (
      &diagcomm_smd_tx_wmq[conn->port_num],                   /* Watermark queue structure */
      DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,    /* Do not exceed count */
      &diagcomm_smd_tx_q[conn->port_num]                      /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue.  Also, set-up functions
     which will be called when the watermarks are reached. */

    /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
  }
  #endif /* #ifdef DIAG_CONSUMER_API */
#else

  /* Set up Queue for sending data via SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_tx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_tx_q[conn->port_num]                        /* Queue to hold the items */
  );

  /* Set up watermarks for WaterMark Queue. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
  dsm_set_low_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ));
  dsm_set_hi_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ));
#endif

} /* diagcomm_smd_init */


/*===========================================================================
FUNCTION DIAGCOMM_SMD_OPEN_INIT

DESCRIPTION

===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void diagcomm_smd_open_init( diagcomm_io_conn_type * conn )
{
  if( DIAGCOMM_PORT_SMD != conn->port_type )
    return;
    
  /* Now prepare parameters for opening a port */
  conn->open_params.sio_params.params.stream_mode = SIO_DM_MODE;
  conn->open_params.sio_params.params.tx_queue = &(diagcomm_smd_tx_wmq[conn->port_num]);

#if defined(DIAG_BUFFERING_SUPPORT)
  conn->open_params.sio_params.params.rx_queue = &diagcomm_smd_rx_wmq[DIAGCOMM_PORT_1];
#else
  conn->open_params.sio_params.params.rx_queue = &diagcomm_smd_rx_wmq[conn->port_num];
#endif
  
  /* Normally, this layer doesn't know about the HDLC encoding of the diag
     protocol.  However, SIO is configured to look for tail character. */
  conn->open_params.sio_params.params.tail_char_used = FALSE;
  conn->open_params.sio_params.params.tail_char = 0;

  /* Use whatever the flow control method is currently. */
  conn->open_params.sio_params.params.rx_flow = SIO_CTSRFR_FCTL;
  conn->open_params.sio_params.params.tx_flow = SIO_FCTL_OFF;

  /* Default DM bit rate is 38.4Kbps unless specified in NV.
     Now, it uses 115.2 (the max) as the default. */
  conn->open_params.sio_params.params.rx_bitrate = SIO_BITRATE_115200;
  conn->open_params.sio_params.params.tx_bitrate = SIO_BITRATE_115200;

  /* Setting callbacks */
  if( DIAGCOMM_PORT_1 == conn->port_num )
  {
    if( conn->open_params.sio_params.params.port_id == SIO_PORT_SMD_DIAG_APPS )
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_inbound_pkt_notify;
    else
      conn->open_params.sio_params.params.rx_func_ptr = NULL;
  }

  conn->open_params.sio_params.close_func_ptr = diagcomm_smd_close_done_cb;
  
  /* Passing the pool ID to SMD */
#if defined(DIAG_MP_MASTER)
  conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SMD_TX_ITEM_POOL;
#else
  conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_TX_ITEM_POOL;
#endif
  
  conn->open_params.sio_params.open_param.dsm_pool_param.mempool_set = 0;
  conn->open_params.sio_params.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_LEGACY;

  conn->open_params.sio_params.params.open_param = &(conn->open_params.sio_params.open_param);
  
} /* diagcomm_smd_open_init */
#endif /* DIAG_SIO_SUPPORT */


#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
boolean bAllowFlow = TRUE; /* Keep track of state of flow control. */

/*===========================================================================
FUNCTION diagcomm_smd_flow_enabled

DESCRIPTION
   This function tells whether the flow to DSM_DIAG_SMD_TX_ITEM_POOL is enabled
   or not.

===========================================================================*/
#if defined(DIAG_BUFFERING_SUPPORT)
boolean diagcomm_smd_flow_enabled(void)
{
  return bAllowFlow;

} /* diagcomm_smd_flow_enabled */
#endif


/*===========================================================================
FUNCTION diagcomm_smd_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count 
   for DSM_DIAG_SMD_TX_ITEM_POOL is reached. (DSM_DIAG_SMD_TX_ITEM_POOL is 
   used by smd, to receive data.)
   
   This callback is registered with lower watermark level for streaming mode,
   so we will only disable flow when this callback is triggered while in 
   streaming mode.
   
===========================================================================*/
static void diagcomm_smd_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op)
{
   /* Check to be defensive */
   if(pool_id != DSM_DIAG_SMD_TX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_NEW)
      return;

  // if( diag_tx_mode.mode != DIAG_TX_MODE_STREAMING )
   //  return;
   
   if (TRUE == bAllowFlow)
   {

      bAllowFlow = FALSE;
      diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SMD);
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dsm_high_mem_event_cb: disabled flow");
   }

} /* diagcomm_smd_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_smd_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count 
   for DSM_DIAG_SMD_TX_ITEM_POOL is reached. (DSM_DIAG_SMD_TX_ITEM_POOL 
   is used by smd, to receive data.)
   
   This callback is registered with lower watermark level for streaming mode,
   so we will only enable flow when this callback is triggered while in 
   streaming mode.
   
===========================================================================*/
static void diagcomm_smd_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                               dsm_mem_level_enum_type mem_level,
                                               dsm_mem_op_enum_type mem_op)
{
   osal_sigs_t return_sigs;
   int return_val = 0;

   /* check to be defensive */
   if(pool_id != DSM_DIAG_SMD_TX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_FREE)
      return;
   
   /* Allow this to re-enable flow in all modes, in case we miss the 
      re-enable wm in buffering mode and we've switched back to 
      streaming mode. */
     
   if (FALSE == bAllowFlow)
   {

      bAllowFlow = TRUE;

       return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_READ_SIG, &return_sigs);
       ASSERT(OSAL_SUCCESS == return_val);

      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_smd_dsm_low_mem_event_cb */

#endif /* DIAG_MP && DIAG_MP_MASTER*/


/*===========================================================================
FUNCTION DIAG_BUFFERED_MODE

DESCRIPTION
  Checks if we're currently in one of the buffering modes.
  
PARAMETERS
  None
  
RETURN VALUE
  True - If we're currently in one of the buffering modes.
  False - Otherwise.
  
===========================================================================*/
boolean diag_buffered_mode()
{
#if defined(DIAG_BUFFERING_SUPPORT)
  if( (diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH) ||
	 (diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
    return TRUE;
  }  
  else
  {
    return FALSE;
  }
#else
  return FALSE;
#endif
} /* diag_buffered_mode */


#if defined(DIAG_BUFFERING_SUPPORT)
/*===========================================================================
FUNCTION DIAG_UPDATE_MODE_INFO 

DESCRIPTION
  Sends a Diag Mode control packet to all the peripherals to enable or
	disable sleep voting based on the mode Diag is in.
 
  It Enables sleep voting for Streaming Mode and disables Sleep voting
  for Buffered Threshold or Circular mode
 
  It also controls the local processor's sleep voting.
  
PARAMETERS
  mode - DIAG_TX_MODE_STREAMING         0
       - DIAG_TX_MODE_BUFFERED_THRESH   1
       - DIAG_TX_MODE_BUFFERED_CIR      2
DEPENDENCIES 
  diag_set_tx_mode()
  We should ensure that this procedure is called from diag_set_tx_mode()
	only -- that sets the Diag Transmission Mode
  
RETURN VALUE
  None

===========================================================================*/
void diag_update_mode_info ( uint8 mode )
{
	diag_ctrl_msg_type ctrl_msg;
	int port_num = 0;
	boolean is_sent = FALSE;
	uint8 sleep_voting_flag = 0;

	if( mode > DIAG_TX_MODE_LAST )
	{
		MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_update_mode_info: Invalid mode (%d)", mode);
		return;
	}
	/* Fill the control message header */
	ctrl_msg.cmd_type = DIAG_CTRL_MSG_DIAGMODE;
	ctrl_msg.data_len = sizeof(diag_ctrl_msg_diagmode_type);
	ctrl_msg.data.ctrl_msg_diagmode.version = DIAG_CTRL_MSG_DIAGMODE_VER;

	if( mode == DIAG_TX_MODE_STREAMING )
	{
	  /* Enable sleep voting for Streaming Mode */
	  sleep_voting_flag = 1;

	  ctrl_msg.data.ctrl_msg_diagmode.sleep_vote = sleep_voting_flag;  /* Setting the sleep vote flag */
	  ctrl_msg.data.ctrl_msg_diagmode.real_time = 1;
	  ctrl_msg.data.ctrl_msg_diagmode.use_nrt_values = 0;
	  ctrl_msg.data.ctrl_msg_diagmode.commit_threshold = 0;
	  ctrl_msg.data.ctrl_msg_diagmode.sleep_threshold = 0;
	  ctrl_msg.data.ctrl_msg_diagmode.sleep_time = 0;
	  ctrl_msg.data.ctrl_msg_diagmode.drain_timer_val = 0;
	  ctrl_msg.data.ctrl_msg_diagmode.event_stale_timer_val = 0;

        /* also update apps */
      diagbuf_ctrl_tx_sleep_parameters( DIAG_TX_SLEEP_THRESHOLD_DEFAULT, DIAG_TX_SLEEP_TIME_DEFAULT );
      diagbuf_ctrl_commit_threshold( DIAGBUF_COMMIT_THRESHOLD );
      diag_ctrl_drain_timer_len( DIAG_DRAIN_TIMER_LEN );
      diag_ctrl_fwd_drain_timer_len( DIAG_FWD_DRAIN_TIMER_LEN_RT );
      event_ctrl_stale_timer( EVENT_TIMER_LEN );
      event_ctrl_report_size( EVENT_RPT_PKT_SIZE, EVENT_SEND_MAX, EVENT_RPT_PKT_LEN_SIZE );

    }
    else if( (mode == DIAG_TX_MODE_BUFFERED_THRESH)
					 || (mode == DIAG_TX_MODE_BUFFERED_CIR) )
    {
      /* Disable Sleep Voting for Buffering Modes */
      sleep_voting_flag = 0;

	  /* Fill the control message */
      ctrl_msg.data.ctrl_msg_diagmode.sleep_vote = sleep_voting_flag;  /* Setting the sleep vote flag */
      ctrl_msg.data.ctrl_msg_diagmode.real_time = 0;   /* real_time = 0 means we use default NRT values */
      ctrl_msg.data.ctrl_msg_diagmode.use_nrt_values = 0;
      ctrl_msg.data.ctrl_msg_diagmode.commit_threshold = 0;
      ctrl_msg.data.ctrl_msg_diagmode.sleep_threshold = 0;
      ctrl_msg.data.ctrl_msg_diagmode.sleep_time = 0;
      ctrl_msg.data.ctrl_msg_diagmode.drain_timer_val = 0;
      ctrl_msg.data.ctrl_msg_diagmode.event_stale_timer_val = 0;

      /* also update apps */
      diagbuf_ctrl_tx_sleep_parameters( DIAG_TX_SLEEP_THRESHOLD_NRT, DIAG_TX_SLEEP_TIME_NRT );
      diagbuf_ctrl_commit_threshold( DIAGBUF_COMMIT_THRESHOLD_NRT );
      diag_ctrl_drain_timer_len( 0 );
      diag_ctrl_fwd_drain_timer_len( DIAG_FWD_DRAIN_TIMER_LEN_NRT );
      event_ctrl_stale_timer( 0 );
      event_ctrl_report_size(EVENT_RPT_PKT_SIZE_NRT, 1024, DIAGBUF_SIO_TX_ITEM_SIZ - DIAG_HDLC_PAD_LEN);
    }

    /* Send the control message to all the peripherals */
    for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
    {
      (void)diagcomm_ctrl_send_mult_pkt((PACK(void *)) &ctrl_msg, 
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_diagmode_type),
                                         NULL, 0, DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, TRUE, &is_sent);
    }

    /* Change the local processor's sleep voting mechanism */
    diag_control_sleep_voting(sleep_voting_flag);

    if( sleep_voting_flag == 0 )
    {
      diag_allow_sleep();
    }
    
} /* diag_update_mode_info */


/*===========================================================================
FUNCTION DIAG_SET_TX_MODE

DESCRIPTION
  Sets the Tx mode. 
  
PARAMETERS
  mode - DIAG_TX_MODE_STREAMING         0
       - DIAG_TX_MODE_BUFFERED_THRESH   1
       - DIAG_TX_MODE_BUFFERED_CIR      2

RETURN VALUE
  The current Tx mode.
  
===========================================================================*/
uint8 diag_set_tx_mode( uint8 mode )
{
  uint8 prev_mode = diag_tx_mode.mode;

  if ( mode <= DIAG_TX_MODE_LAST )
  {
    diag_tx_mode.mode = mode;
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode: Invalid mode (%d)", mode);
    return diag_tx_mode.mode;
  }

  /* Reset health counters if mode has changed */
  if ( prev_mode != diag_tx_mode.mode )
    diag_health_reset_dropped_cnt();

  if ( diag_tx_mode.mode == DIAG_TX_MODE_STREAMING )
  {

    osal_lock_mutex(&diag_tx_mode_drain_mutex);
    diag_tx_mode.drain = FALSE;
    osal_unlock_mutex(&diag_tx_mode_drain_mutex);
    
    diag_fwd_notify();

    osal_lock_mutex(&diag_compression_mutex);
    if(((prev_mode == DIAG_TX_MODE_BUFFERED_THRESH || prev_mode == DIAG_TX_MODE_BUFFERED_CIR)
    && total_bytes_in_compressed_buffer == 0) || prev_mode == DIAG_TX_MODE_STREAMING )
    {
      diag_tx_mode_internal = diag_tx_mode.mode;
      /* Update prcoessor's sleep voting mechanism */
      diag_update_mode_info(diag_tx_mode.mode);  
    }
    osal_unlock_mutex(&diag_compression_mutex);
  }
  else if ( (diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH) ||
            (diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {

    osal_lock_mutex(&diag_tx_mode_drain_mutex);
    diag_tx_mode.drain = FALSE;
    osal_unlock_mutex(&diag_tx_mode_drain_mutex);    
    
    diag_tx_mode_internal = diag_tx_mode.mode;

    /* Update prcoessor's sleep voting mechanism */
    diag_update_mode_info(diag_tx_mode.mode);  
    diag_fwd_notify();
  }

  return diag_tx_mode.mode;

} /* diag_set_tx_mode */


/*===========================================================================
FUNCTION DIAG_GET_TX_MODE

DESCRIPTION
  Gets the current Tx mode. 
  
PARAMETERS
  None
  
RETURN VALUE
  The current Tx mode.
  
===========================================================================*/
uint8 diag_get_tx_mode( void )
{
  return diag_tx_mode.mode;

} /* diag_get_tx_mode */


/*===========================================================================
FUNCTION DIAG_SET_TX_PARAM_HIGH_WM

DESCRIPTION
  Sets the Tx high watermark parameter in terms of percentage full.
  In buffered threshold mode, when the percentage buffer full reaches
  high wm, we start to drain until we reach the low wm percentage.

  High wm level cannot be less then low wm level. High wm level should
  also not be greater than the level when we enable flow control to
  stop the traffic from the peripherals.

PARAMETERS
  value - High watermark value.

RETURN VALUE
  The current high watermark value.

===========================================================================*/
uint8 diag_set_tx_param_high_wm( uint8 value )
{
    /* Don't allow drain thresholds where Diag would have to be flow-controlled before we drain */
  uint32 many_mark = DIAG_COMPRESSION_UPPER_THRESHOLD;

  if( (value > diag_tx_mode.buffered_lower_threshold) && (value < many_mark) )
  {
    diag_tx_mode.buffered_upper_threshold = value;
    diag_tx_mode.buffered_many_mark = ((DIAGBUFC_SIZE*value)/100);
  }

  return diag_tx_mode.buffered_upper_threshold;

} /* diag_set_tx_param_high_wm */


/*===========================================================================
FUNCTION DIAG_SET_TX_PARAM_LOW_WM

DESCRIPTION
  Sets the Tx low watermark parameter in terms of percentage full.
  In buffered threshold and buffered circular modes, we stop draining
  when the compressed buffer full reaches low wm.

  Low wm level cannot be greater than high wm level.

PARAMETERS
  value - Low watermark value.

RETURN VALUE
  The current low watermark value.

===========================================================================*/
uint8 diag_set_tx_param_low_wm( uint8 value )
{
  if( value < diag_tx_mode.buffered_upper_threshold )
  {
    diag_tx_mode.buffered_lower_threshold = value;
    diag_tx_mode.buffered_few_mark = ((DIAGBUFC_SIZE * value)/100);
  }

  return diag_tx_mode.buffered_lower_threshold;

} /* diag_set_tx_param_low_wm */


/*===========================================================================
FUNCTION DIAG_GET_TX_PARAM_HIGH_WM

DESCRIPTION
  Retrieves the Tx high watermark parameter.

PARAMETERS
  None

RETURN VALUE
  The current high watermark value.

===========================================================================*/
uint8 diag_get_tx_param_high_wm( void )
{
  return diag_tx_mode.buffered_upper_threshold;

} /* diag_get_tx_param_high_wm */


/*===========================================================================
FUNCTION DIAG_GET_TX_PARAM_LOW_WM

DESCRIPTION
  Retrieves the Tx low watermark parameter.

PARAMETERS
  None

RETURN VALUE
  The current low watermark value.

===========================================================================*/
uint8 diag_get_tx_param_low_wm( void )
{
  return diag_tx_mode.buffered_lower_threshold;

} /* diag_get_tx_param_low_wm */


/*===========================================================================
FUNCTION DIAG_DRAIN_TX_BUFFER

DESCRIPTION
  Drains the Tx buffer immediately. If in buffered mode, it will drain until
  Tx low watermark threshold is reached.

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_drain_tx_buffer( void )
{

  osal_lock_mutex(&diag_tx_mode_drain_mutex);
  diag_tx_mode.drain = TRUE;
  osal_unlock_mutex(&diag_tx_mode_drain_mutex);

  diag_fwd_notify();

} /* diag_drain_tx_buffer */


 /*===========================================================================
FUNCTION DIAG_HEALTH_GET_DROPPPED_BYTES_CNT

DESCRIPTION
  Gives the number of bytes overwritten  (hence dropped) in circular
  buffering mode.

PARAMETERS
  None

RETURN VALUE
  Bytes Dropped.

===========================================================================*/
uint32 diag_health_get_dropped_bytes_cnt( void )
{
  return diag_tx_mode.byte_dropped_cnt;

} /* diag_health_get_dropped_bytes_cnt */

 /*===========================================================================
FUNCTION DIAG_HEALTH_GET_ALLOC_BYTES_CNT

DESCRIPTION
  Gives the number of bytes allocated in the compressed buffer.

PARAMETERS
  None

RETURN VALUE
  Bytes Allocated.

===========================================================================*/

uint32 diag_health_get_alloc_bytes_cnt( void )
{
  return diag_tx_mode.byte_alloc_cnt;

} /* diag_health_get_alloc_bytes_cnt */


 /*===========================================================================
FUNCTION DIAG_HEALTH_RESET_DROPPPED_CNT
DESCRIPTION
  Resets the  dropped and alloc count

PARAMETERS
  None

RETURN VALUE
  None.

===========================================================================*/
void diag_health_reset_dropped_cnt( void )
{
  diag_tx_mode.prev_mode = diag_tx_mode.mode;
  diag_tx_mode.prev_byte_dropped_cnt = diag_tx_mode.byte_dropped_cnt;
  diag_tx_mode.prev_byte_alloc_cnt = diag_tx_mode.byte_alloc_cnt;


  diag_tx_mode.byte_dropped_cnt = 0;
  diag_tx_mode.byte_alloc_cnt = 0;

} /* diag_health_reset_dropped_cnt */


/*===========================================================================
FUNCTION DIAGCOMM_TX_MODE_CONFIG

DESCRIPTION
  Command handler function for buffered mode commands.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet

RETURN VALUE
  Pointer to response packet.

===========================================================================*/
PACK(void *) diagcomm_tx_mode_config( PACK(void *) req_pkt_ptr, uint16 pkt_len )
{
  diagcomm_tx_mode_config_req_type * req_ptr = (diagcomm_tx_mode_config_req_type *)req_pkt_ptr;
  diagcomm_tx_mode_config_rsp_type * rsp_ptr = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;

  if( req_ptr == NULL )
    return rsp_ptr;

  cmd_code = diagpkt_subsys_get_cmd_code( req_ptr );

  switch( req_ptr->op_code )
  {
    case DIAG_TX_MODE_CONFIG_SET_MODE:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_MODE;
        rsp_ptr->data.mode = diag_set_tx_mode( req_ptr->data.mode );
      }
      break;

    case DIAG_TX_MODE_CONFIG_GET_MODE:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_MODE;
        rsp_ptr->data.mode = diag_get_tx_mode();
      }
      break;

    case DIAG_TX_MODE_CONFIG_SET_PARAM_HIGH_WM:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                 FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_PARAM_HIGH_WM;
        rsp_ptr->data.wm_value = diag_set_tx_param_high_wm( req_ptr->data.wm_value );
      }
      break;

    case DIAG_TX_MODE_CONFIG_GET_PARAM_HIGH_WM:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                   FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_PARAM_HIGH_WM;
        rsp_ptr->data.wm_value = diag_get_tx_param_high_wm();
      }
      break;

    case DIAG_TX_MODE_CONFIG_SET_PARAM_LOW_WM:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_PARAM_LOW_WM;
        rsp_ptr->data.wm_value = diag_set_tx_param_low_wm( req_ptr->data.wm_value );
      }
      break;

    case DIAG_TX_MODE_CONFIG_GET_PARAM_LOW_WM:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_PARAM_LOW_WM;
        rsp_ptr->data.wm_value = diag_get_tx_param_low_wm();
      }
      break;

    case DIAG_TX_MODE_CONFIG_DRAIN:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_rsp_type, data) );
      if( rsp_ptr )
      {
        diag_drain_tx_buffer();
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_DRAIN;
      }
      break;




    case DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(uint32) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT;
        rsp_ptr->data.byte_dropped_cnt = diag_tx_mode.byte_dropped_cnt;
      }
      break;

  case DIAG_TX_MODE_CONFIG_QUERY:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_rsp_type, data) + sizeof(diagcomm_query_rsp_type));
      if( rsp_ptr )
      {
      rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_QUERY;
      rsp_ptr->data.query.per_compression_buf_used = diag_per_compression_buffer_full();
      rsp_ptr->data.query.byte_to_compress_cnt = diag_compression_bytes_internal_buffer();
      }
      break;

    case DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT:
      rsp_ptr = (diagcomm_tx_mode_config_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_rsp_type, data) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT;
        diag_health_reset_dropped_cnt();
      }
      break;

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
  }

  return rsp_ptr;

} /* diagcomm_tx_mode_config */
#endif /* DIAG_BUFFERING_SUPPORT */

