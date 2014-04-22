/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SIO

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SIO.

Copyright (c) 2001-2016, 2019-2020, 2022 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_sio.c#6 $ $DateTime: 2022/11/30 21:57:01 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
11/02/16   ph      Remove q_init() before dsm_queue_init().
07/20/16   nk      Changes to support ThreadX
03/23/16   ph      Use separate pool for TN traffic.
07/13/15   rh      Re-signal to send out command responses when SIO reconnects
05/18/15   rh      diagcomm_send() now always sends data to the SMD queue if buffering is supported.
05/13/15   rh      Call diag_fwd_notify() when SIO pool comes out of flow control
                   instead of setting signals by hand
05/04/15   rh      Standardized mutex around total_bytes_in_compressed_buffer
04/16/15   sa      Continue Compression as long as there is memory in the Compression Buffer.
04/16/15   sa      Fixed isses with setting a signal handled in streaming mode while in 
                   buffering mode
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
11/06/14   is      Do not flush on first sio open and debug code
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
10/07/14   ph      Replaced MSG_HIGH call with appropriate MSG macro.
07/25/14   sa      Added support to send an event to debug diag drops
05/22/14   sr      Fixed diag stall issue  
04/17/14   xy      Fixed Diag does not send notification to RDM in diagcomm_close problem 
03/25/14   sa      Updated the flow control callback functions for SIO TX Pool.
03/07/14   sa      Renamed DIAG_TX_SLAVE_SIG to DIAG_FWD_TX_SLAVE_SIG
02/26/14   sa      Added support for compression in buffering mode
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/27/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature 
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
05/02/13   ph      Add DIAG over MBIM support
10/04/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
10/01/13   sr      Added flow control checks on fwd channel  
10/01/13   xy      Removed code under FEATURE_WINCE 
09/25/13   xy      Handle sending out feature mask on control channel first before 
                   sending out cmd reg tables in diag_handle_internal_sigs()  
02/27/13   sr      Added support to HDLC encode the fwd channel traffic 
                   on APPS  
02/15/13   is      Send QDSS event when Diag is ready
01/25/13   sr      Fixed compiler warnings   
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
12/05/12   is      Save osal_set_sigs() return value in diagcomm_sio_wmq_low_mem_event_cb(
11/27/12   rh      Mainlining DIAG_CENTRAL_ROUTING
11/15/12   is      Support for preset masks
10/17/12   rh      Moved defines for TX/RX Max Chain Size to diagi_v.h
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes 
03/04/12   sg      set the signal to forward the regsitration tables to the apps
                   proc only when the ctrl channel is opened on both the sides
02/08/12   is      Fix missing DSMs issue
01/31/12   is      Add flow control on diagcomm_sio_tx_wmq
01/24/12   is      Support for buffered circular mode
01/23/12   is      Apps flow control on SMD_RX pool instead of SIO_TX pool
11/04/11   is      Support for buffered threshold mode
10/07/11   is      Support sending log mask per equipment id
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
04/28/11   mad     Fixed featurization for diag_fwd_task access
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite
02/28/11   sg      Power saving changes 
12/20/10   mad     Changes for dual mask
12/16/10   vs      Open SDIO port based on feature
10/20/10   is      Bypass RDM to set default to UART1 for 8960 Apps only bringup  
10/14/10   is      Featurize diagcomm_send_dtr_msg() for multi-proc only
10/05/10   mad     Register for DTR only if port open is successful
09/28/10   mad     Included diaglogi.h, eventi.h to fix build error 
09/14/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
09/01/10   sg      Call sio_transmit() with smd stream id on Q6 to put the 
                   tx traffic into diagcomm_smd_tx_wmq.
08/06/10   sg      Featurized diagcomm_sio_close_done_cb(),diagcomm_sio_close_cb
                   to fix compiler warnings
07/23/10   mad     Turning on command-filtering before sio_open() call, for 
                   on-device diag port 
07/21/10   sg      Added chaining support for rx response packets
07/15/10   sg      Cleaning up the setting and resetting of drain timer in
                   diag_dtr_changed_cb()
06/21/10   vs      Added ASSERT to catch dropping of partially filled DSM item.
05/10/10   sg      mainlined setting and clearing of drain timer
06/07/10   is      Removed support for FEATURE_DIAG_MULTI_PORTS, FEATURE_SIO_USB_ETHR
                   FEATURE_SIO_USB_COMPOSITE
05/25/10   JV      New task on master to process slave traffic
05/06/10   as      De-coupled Diag and ondevice modules. Mainlined the code for 
                   retry count when the open port fails. Fixed the assign port
                   logic when Diag port is configured to ondevice.
05/03/10   sg      fixing corner case where we can expect triple chaining 
                   with 6k logs
04/29/10   is      Removed support for DEBUG_TX_EFS
04/20/10   sg      Set Event Drian Signal After Flow control is turned off
03/25/10   mad     Support for on-device logging for WM7 devices
03/15/10   vs      Fixed unchecked pointer access in diagcomm_outbound_alloc
03/03/10   as      Added support RDM assign port needed for Ondevice retry.
01/04/10   vs      Modifications to support diagbuf_send_pkt_single_proc
12/30/09   sg      Fixed compiler warning
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
11/30/09   ps      Fixed compiler warnings.
11/05/09   vs      Mainlined functions diagbuf_outbound_alloc and diagbuf_send
10/29/09   JV      Pass the DSM pool ID to SIO-USB
09/11/09   sg      Made changes to have dedicated pools in each Tx and RX direction
                   for single processor targets
08/11/09   mad     Signal Diag with DIAG_TX_SLAVE_SIG, when DTR changes
08/06/09   sg      Added support for DSM chaining in diagcomm_sio_inbound_pkt_notify(),
                   diagcomm_inbound()
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
08/04/09   JV      Diag does not vote for/against sleep based on dtr change events.
07/31/09   JV      Removed inclusion of task.h. Removed warnings.
07/31/09   JV      Merged Q6 diag code back to mainline
06/23/09   JV      Introduced the OS abstraction layer
05/07/09   vk      changed includes from external to internal headers
04/21/09   ps	   branch collapse of 6k and 7k DIAG
11/06/08   mad     modified diagcomm_status() to return only the port 
                   open/close status. diagcomm_status() is not affected by usb 
                   cable detach anymore.
10/03/08   vg      Updated code to use PACK() vs. PACKED
1/28/08    as      Fixed tail char setup for single and modem only builds.
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
05/17/07   jc      Proxy tv.  FEATURE_SIO_USB_COMPOSITE
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
09/09/05   sl      Added support for multiple tx/rx queues for the case of
                   FEATURE_DIAG_MULTI_PORTS
05/15/05   as      Added support for FEATURE_DATA_SERIALIZER
03/28/05   sl      Added an array of watermark queues for the multi port feature
03/24/05   as      Added check in UART/USB notify functions to check if the
                   connected accessory detected by AAM is UART and USB
03/05/05   sl      Corrected BT Featurization for FEATURE_DIAG_MULTI_PORTS
02/20/05   sl      Added support for DIAG over BT via RDM under
                   FEATURE_DIAG_MULTI_PORTS
02/12/05   sl      Fixed a bug that caused the close/open port bit masks
                   not being cleared
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag  
01/25/04   sl      Added support for callback notifications from Accessory
                   Attachment Manager (AAM) for USB and UART cable attach and
                   detach under FEATURE_ACCESSORY_ATTACHMENT_MANAGER.
12/14/04   sl      Added support for DIAG to hold multiple SIO ports
                   controlled by FEATURE_DIAG_MULTI_PORTS
04/20/04   pn      Added support for DIAG over Bluetooth.
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
04/17/01   dwp     Initialize diagcomm_sio_open_params.port_id to NULL. RDM
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
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "msg.h"
#include "msgi.h" /* For msg_set_all_rt_masks_adv () */
#include "diagdsmi.h"
#include "diagtune.h"
#include "diagdiag_v.h"
#include "diag_cfg.h"
#include "diagdsm.h"
#include "dsm.h"

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#if defined(DIAG_QDSS_SUPPORT)
#include "tracer.h"
#include "tracer_event_ids.h"
#endif

#if ((defined(DIAG_RUNTIME_DEVMAP))||(defined(FEATURE_DIAG_SWITCH)))
  #include "rdevmap.h"
#endif



#include "diagcomm_smd.h"
#include "diagcomm_ctrl.h"
#include "diagcomm_io.h"

#ifdef DIAG_SIO_SUPPORT
#include "diagcomm_io_sio.h"
#endif /* DIAG_SIO_SUPPORT */

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include "diagcomm_dci.h"

#include "diagdsm_v.h"
#include "diag_compress_v.h"
#include "diag_fwd_v.h"

#include "diaglogi.h" /* for log_config_mask */
#include "eventi.h" /* for event_set_all_rt_masks */
#include "event.h"
#include "stringl.h"
/*lint -save -e751 */
/* Supress local typedef 'diag_config_comm_enum_type' not referenced */


/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
#define DIAG_HDLC_CRC_PAD (3) /* 2 bytes CRC. 1 byte 7E terminator. */
static uint8 diagbuf_hdlc_pad_len = DIAG_HDLC_PAD_LEN;

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
#ifdef DIAG_CONSUMER_API
  extern diagcomm_io_conn_type diagcomm_io_dci_conn[NUM_SMD_PORTS]; /* From diagcomm_io.c */
#endif /* #ifdef DIAG_CONSUMER_API */
extern boolean diag_first_sio_DTR_check;
extern boolean diag_hdlc_disabled;
extern osal_crit_sect_t diag_master_tx_mutex;


extern uint8 diag_per_count_dropped;  /* Percentage Drop Count */

/* Variables to store drop count for DIAG Health */
extern uint32 drop_count_log;
extern uint32 drop_count_f3;

/* Internal variables to store the DSM item pool drop count */
static uint32 drop_count_sio = 0;
static uint32 drop_count_sio_cmd = 0;

/* Variables to store allocation requests for DIAG Health */
extern uint32 alloc_count_log;
extern uint32 alloc_count_f3;

#if defined(DIAG_BUFFERING_SUPPORT)
extern volatile diag_tx_mode_type diag_tx_mode;                    /* From diagcomm_smd.c */
extern osal_mutex_arg_t diag_compression_mutex;
/* TODO: Need to be revisited for backward compatibility i.e
         buffering without compression. Commenting out for now */
#if 0
extern void diagcomm_buffer_enqueue( dsm_item_type ** item_ptr );  /* From diagcomm_smd.c */
#endif
extern uint32 total_bytes_in_compressed_buffer;
#endif

#ifdef FEATURE_DIAG_SWITCH
  extern int diag_on_single_proc;
#endif

#ifdef DIAG_CONSUMER_API
  uint32 diagcomm_flow_ctrl_count[NUM_SMD_PORTS] = {0, 0, 0}; /* Diag Health counter - Keeps track of # of times flow ctrl is triggered on SMD path */
  extern boolean send_dci_conn_status;
#else
  uint32 diagcomm_flow_ctrl_count[NUM_SMD_PORT_TYPES] = {0,0};
#endif /* DIAG_CONSUMER_API */

boolean diagcomm_apps_only_config = FALSE;

/* Queues around which the watermark queues are built */
static q_type diagcomm_sio_rx_q[NUM_SIO_PORTS];
static q_type diagcomm_sio_tx_q[NUM_SIO_PORTS];

/* Watermark queues used for communications with SIO */
dsm_watermark_type diagcomm_sio_rx_wmq[NUM_SIO_PORTS];
dsm_watermark_type diagcomm_sio_tx_wmq[NUM_SIO_PORTS];



/* To avoid extra copies between layers in the embedded system, we pass
   pointers from SIO and wait for the caller to release them. */
static dsm_item_type *diagcomm_sio_inbound_item_ptr = NULL;

static dsm_item_type *diagcomm_sio_outbound_item_ptr_hdlc = NULL;
static dsm_item_type *diagcomm_sio_master_outbound_item_ptr_hdlc = NULL;
static dsm_item_type *diagcomm_sio_outbound_item_ptr = NULL;
static dsm_item_type *diagcomm_sio_outbound_item_ptr_compressed = NULL;
static dsm_item_type *diagcomm_sio_outbound_item_ptr_ctrl = NULL;
static dsm_item_type *diagcomm_sio_cmd_outbound_item_ptr = NULL;

/* Configuration information for the diagnostic communications layer.
*/
static struct
{
  void (*inbound_cb) (void);

  /* !!! Note: if using a system with separate threads
     ** for RX and TX, these should be NULL.
   */
  void (*tx_start_cb) (void);
  void (*tx_stop_cb) (void);
}
diagcomm_params;

diagcomm_io_conn_type * diagcomm_get_sio_conn( diagcomm_enum_port_type port_num );

static boolean allowflow = TRUE; /* Keep track of state of DSM flow control. */

static void diagcomm_sio_dsm_low_mem_event_cb (dsm_mempool_id_type,
                                                  dsm_mem_level_enum_type,
                                                  dsm_mem_op_enum_type);
static void diagcomm_sio_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                   dsm_mem_level_enum_type mem_level,
                                                   dsm_mem_op_enum_type mem_op);

#if defined(DIAG_BUFFERING_SUPPORT)
extern boolean diagcomm_smd_flow_enabled(void);
#endif

#if defined (DIAG_RUNTIME_DEVMAP)||  defined (FEATURE_DIAG_SWITCH)
static uint32 diagcomm_retry_count = 0;
#endif 

extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern dsm_watermark_type diagcomm_smd_rx_wmq[NUM_SMD_PORTS];


#define DIAGCOMM_RETRY_MAX_CNT 5
extern boolean bAllowFlow;

char diag_data_pkt_inbound[DIAG_READ_BUF_SIZE*2];
static dsm_item_type *diagcomm_sio_outbound_item_ptr_data = NULL;

#if defined(DIAG_FLASH_LOGGING_SUPPORT)
  extern boolean diag_flash_logging_enabled;
#endif

/*===========================================================================================

FUNCTION DIAGCOMM_SEND_MASTER_TX

DESCRIPTION
  This function drains apps data traffic in streaming mode. 
  
@param[in] port_num   Indicates the port number from which the data has to be sent/dequeued
                      If this is -1, then any pending data is flushed out


RETURN VALUE
  Number of dsm items drained

==============================================================================================*/
uint32 diagcomm_send_master_tx(diagcomm_enum_port_type port_num)
{
        
		static dsm_item_type *diagcomm_app_item_ptr = NULL;
		static dsm_item_type *diagcomm_app_item_ptr_initial = NULL;
		diagcomm_io_tx_params_type tx_params;
		static dsm_item_type *item_ptr = NULL;
		static diag_hdlc_dest_type enc = {NULL, NULL, 0};
		uint16 *pkt_length = NULL;
		uint8 hdlc_pad = 0;
		int move_data_ptr = 0;
		uint8 *data_ptr = NULL;
		uint8 *data_ptr_end = NULL;
		unsigned int outbound_used = 0;
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
		diag_send_desc_type send_desc_data = {NULL, NULL, DIAG_SEND_STATE_START, FALSE, FALSE};
		diag_send_desc_type *send_desc = &send_desc_data;
		uint32 num_dsm_items_send = 0;
		uint32 dsm_item_count= 0;
		static uint32 diag_num_chained_dsm = 0;
		uint32 dsm_item_length = 0;
		boolean move_flag = FALSE;

        #ifdef DIAG_CONSUMER_API
        if (DIAGCOMM_PORT_2 == port_num)
        {
          #if defined (DIAG_MP_MASTER) && defined (FEATURE_DIAG_8K_QDSP6)
           {}
          #else
           return 0;
          #endif
        }
        #endif /* #ifdef DIAG_CONSUMER_API */

		osal_enter_crit_sect(&diag_master_tx_mutex);
		
		if((port_num == DIAGCOMM_PORT_NONE ) && (diagcomm_status()))
        {
          if( (outbound != NULL) && (item_ptr != NULL) )
          {
            outbound_used = (uint32)enc.dest - (uint32)outbound;
            item_ptr->used = outbound_used;
            diagcomm_send_master_hdlc(item_ptr, outbound_used);
            outbound = NULL;
            item_ptr = NULL;
          }
		  osal_exit_crit_sect(&diag_master_tx_mutex);
          return 0;
        }

		if (port_num < DIAGCOMM_PORT_NUM_FIRST || port_num > DIAGCOMM_PORT_NUM_LAST)
        {
          osal_exit_crit_sect(&diag_master_tx_mutex);
          return 0;
        }
		
		diagcomm_app_item_ptr = diagcomm_sio_outbound_item_ptr;
        diagcomm_app_item_ptr_initial = diagcomm_app_item_ptr;

        if(diagcomm_app_item_ptr!=NULL)
        {
          if(diagcomm_app_item_ptr->pkt_ptr!= NULL)
          {
            dsm_chain = TRUE;
          }

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
              osal_exit_crit_sect(&diag_master_tx_mutex);
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
                  }
                  else /*The entire packet is in the DSM item */
                  {
                    send_desc->pkt = data_ptr;
                    send_desc->last = data_ptr+pkt_length_bytes-1;
                    send_desc->terminate = TRUE;
                    send_desc->state = DIAG_SEND_STATE_START;
                    send_desc->priority = diagcomm_app_item_ptr->priority;
                    data_ptr = data_ptr + pkt_length_bytes+1;

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

                  send_desc->pkt = data_ptr;
                  send_desc->last = data_ptr+pkt_length_bytes-1;
                  send_desc->terminate = TRUE;
                  send_desc->state = DIAG_SEND_STATE_START;
                  send_desc->priority = diagcomm_app_item_ptr->priority;
                  data_ptr = data_ptr + pkt_length_bytes+1;
                }
              }

              if (outbound == NULL)
              {

                /* Allocate memory from the communications layer */
                item_ptr = diagcomm_master_outbound_alloc_hdlc(&outbound_used, FALSE, send_desc->priority);
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
                  diagcomm_send_master_hdlc(item_ptr, outbound_used);
                  /*Reset the forward drain timer here if its set*/
                  diag_clr_fwd_drain_timer();
                }

                outbound = NULL;
                item_ptr = diagcomm_master_outbound_alloc_hdlc(&outbound_used, FALSE, FALSE);

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
                  /* Update the used field for the current DSM item */
                  item_ptr->used = outbound_used;

                  item_ptr = diagcomm_master_outbound_alloc_hdlc (&outbound_used, TRUE, send_desc->priority);
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

                diagcomm_send_master_hdlc(item_ptr, outbound_used);

                outbound = NULL;
                new_chain = FALSE;
                dsm_full = FALSE;

                /*Reset the forward drain timer here if its set*/
                diag_clr_fwd_drain_timer();
              }
              else if((send_desc->terminate) && (item_ptr))
              {
                /*Set the timer to send the DSM item after the timer expires.
                This takes care of the case when there is less master traffic 
                and diag is waiting to accumulate more pkts in DSM but no more 
                pkts are received.*/
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

          dsm_free_packet(&diagcomm_app_item_ptr_initial);
        }

		diagcomm_sio_outbound_item_ptr=NULL;
      }
	  osal_exit_crit_sect(&diag_master_tx_mutex);
	  return 1;
    }



/*===========================================================================================

FUNCTION DIAG_GET_DSM_ITEM_PTR

DESCRIPTION
  This function checks whether the send descriptor is for cmd response or data and 
  returns the dsm item on whose data pointer the next packet should be written.

@param[in] desc    A boolean value which indicates if the send descriptor is for cmd response
                   or not. It is TRUE for cmd response and FALSE for data.

RETURN VALUE
  Returns the dsm item on whose data pointer the next packet should be written.

==============================================================================================*/

dsm_item_type* diag_get_dsm_item_ptr(boolean is_cmd_rsp_type)
{
  dsm_item_type *tmp = NULL;
  
  if(is_cmd_rsp_type)
    tmp = diagcomm_sio_cmd_outbound_item_ptr;
  else
  {
    osal_enter_crit_sect(&diag_master_tx_mutex);
    tmp = diagcomm_sio_outbound_item_ptr;
  }	
  
  if(tmp)
  {
    //Condition for chained dsm items
   while(tmp->pkt_ptr)
   {
     tmp = tmp->pkt_ptr;
   }

   if(!is_cmd_rsp_type)
   	osal_exit_crit_sect(&diag_master_tx_mutex);
   
   return tmp;
  }
  else
  {
    if(!is_cmd_rsp_type)
		osal_exit_crit_sect(&diag_master_tx_mutex);
    return NULL;
  }	
}

/*===========================================================================

FUNCTION DIAGCOMM_GET_SIO_CONN

DESCRIPTION
  Given a Diag SIO port number, returns the connection data structure.

PARAMETERS
  port_num - SIO port number to retrieve

RETURN VALUE
  Pointer to connection data

===========================================================================*/
diagcomm_io_conn_type * diagcomm_get_sio_conn( diagcomm_enum_port_type port_num )
{
  if( (port_num < DIAGCOMM_PORT_NUM_FIRST) || (port_num > DIAGCOMM_PORT_NUM_LAST) )
    return NULL;

#if defined(DIAG_MP_MASTER) || !defined(DIAG_MP)
  return &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
#else
  if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
    return &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][port_num];
  else
    return &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
#endif

} /* diagcomm_get_sio_conn */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
void
diagcomm_sio_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  #if defined(DIAG_FLASH_LOGGING_SUPPORT)
  if(DIAG_UNLIKELY(diag_flash_logging_enabled == TRUE))
  {
    /* Flash logging is enabled, free the dsm item and return
       without enqueing the packet. */
    dsm_free_packet(item_ptr);
    return;
  }
  else
  #endif // defined(DIAG_FLASH_LOGGING_SUPPORT) && defined(DIAG_FEATURE_EFS2)
  {
    dsm_enqueue (&diagcomm_sio_rx_wmq[DIAGCOMM_PORT_1], item_ptr);

    /* Call the inbound callback to tell the client
       that we have inbound data available. */
    if (diagcomm_params.inbound_cb)
    {
      diagcomm_params.inbound_cb();
    }
  }
} /* diagcomm_sio_inbound_notify */
/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
void
diagcomm_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  dsm_enqueue (&diagcomm_sio_rx_wmq[DIAGCOMM_PORT_1], item_ptr);

  /* Call the inbound callback to tell the client
     that we have inbound data available. */
  if (diagcomm_params.inbound_cb)
  {
    diagcomm_params.inbound_cb ();
  }
  diagcomm_apps_only_config = TRUE;

} /* diagcomm_inbound_pkt_notify */



#if defined (DIAG_RUNTIME_DEVMAP)|| defined (FEATURE_DIAG_SWITCH)
/*===========================================================================

FUNCTION DIAGCOMM_SIO_CLOSE_CB

DESCRIPTION
  This callback is called when RDM wants diag to close its port.

===========================================================================*/
uint32 diag_sio_close_cb_call_count = 0; /* DEBUG */
void
diagcomm_sio_close_cb ()
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  diagcomm_retry_count = 0;

  diag_sio_close_cb_call_count++;

  return_val = osal_set_sigs(&diag_task_tcb, DIAG_COMM_CLOSE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}


/*===========================================================================

FUNCTION DIAGCOMM_SIO_FUSION_DCI_CLOSE_CB

DESCRIPTION
  This callback is called when RDM wants diag to close its port.

===========================================================================*/
#ifdef DIAG_CONSUMER_API
  void
  diagcomm_sio_fusion_dci_close_cb ()
  {
    (void)diag_set_internal_sigs( DIAG_INT_FUSION_CLOSE_SIG );
  }
#endif /* #ifdef DIAG_CONSUMER_API */

/*===========================================================================

FUNCTION DIAGCOMM_SIO_CLOSE_DONE_CB

DESCRIPTION
  This callback is called when sio_close is completed.

===========================================================================*/
uint32 diag_sio_close_done_cb_call_count = 0; /* DEBUG */
static void
diagcomm_sio_close_done_cb (void)
{
  diag_sio_close_done_cb_call_count++;
  diag_release ();
}
#endif  /* defined (DIAG_RUNTIME_DEVMAP)||defined (FEATURE_DIAG_SWITCH)*/

/* -------------------------------------------------------------------------
** Externalized routines (declared in diagcomm.h).
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This function configures the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_register_inbound (void (*inbound_pkt_cb) (void))
{
  diagcomm_params.inbound_cb = inbound_pkt_cb;
}               /* diagcomm_register_inbound */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN

DESCRIPTION
  Opens the communcications stream.  Returns boolean indicating success.

===========================================================================*/
boolean
diagcomm_sio_open (diagcomm_enum_port_type port_num)
{
#if defined(FEATURE_DIAG_NV) && defined(DIAG_DEFAULT_BITRATE_NV)
  nv_sio_baudrate_type nv_default_baud;
  nv_stat_enum_type nv_status;
#endif /* FEATURE_DIAG_NV && DIAG_DEFAULT_BITRATE_NV */
  diagcomm_io_conn_type * conn = NULL;

#ifdef DIAG_CONSUMER_API
  if ((port_num != DIAGCOMM_PORT_1) && (port_num != DIAGCOMM_PORT_FUSION_DCI))
#else
  if (port_num != DIAGCOMM_PORT_1)
#endif /*#ifdef DIAG_CONSUMER_API*/
  {
     return FALSE;
  }
  conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
  diagpkt_filtered_cmd_set = FALSE;

  if (conn->connected)
  {
    #if defined (DIAG_RUNTIME_DEVMAP)
      /* As requested, we will now close. */
      diagcomm_io_close( conn );
      diag_block ();
    #else
      #ifdef FEATURE_DIAG_SWITCH
      if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
      {
        /* As requested, we will now close. */
        diagcomm_io_close( conn );
        diag_block ();
      }
      #endif /* FEATURE_DIAG_SWITCH */
    #endif /* DIAG_RUNTIME_DEVMAP  */

    /* success or not, consider close operation successful */
    conn->connected = FALSE;
  }

  if(DIAGCOMM_PORT_1 == port_num)
  {
    #if defined(FEATURE_DIAG_NV) && defined(DIAG_DEFAULT_BITRATE_NV)
      nv_status = diag_nv_read (NV_DIAG_DEFAULT_BAUDRATE_I,
                (nv_item_type *) & nv_default_baud);

      switch (nv_status)
      {
        case NV_DONE_S:
          conn->open_params.sio_params.params.rx_bitrate = (sio_bitrate_type) nv_default_baud;
          conn->open_params.sio_params.params.tx_bitrate = (sio_bitrate_type) nv_default_baud;
          break;

        case NV_NOTACTIVE_S:
          nv_default_baud = NV_SIO_BAUDRATE_115200;
          (void) diag_nv_write (NV_DIAG_DEFAULT_BAUDRATE_I, &nv_default_baud);
          break;

        default:
          MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Failed to read sio bitrate");
          break;
      } /* end switch */
    #endif /* FEATURE_DIAG_NV && DIAG_DEFAULT_BITRATE_NV */

    diagcomm_io_set_port( conn );

    if( !conn->connected )
    {
      if( diagcomm_io_open(conn) == TRUE )
      {
        conn->connected = TRUE;
      }
      else
      {
        diagpkt_filtered_cmd_set = FALSE;
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_sio_open - Couldn't get stream.");
      }
    }

#if defined (DIAG_RUNTIME_DEVMAP)
    if (conn->connected)
    {
      rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
    }
    else
    {
      rdm_notify (RDM_DIAG_SRVC, RDM_NOT_ALLOWED_S);
#ifdef FEATURE_QBI
      if (diagcomm_retry_count == 0)
      {
        diagcomm_retry_count++;
        diag_suspend (DIAG_TOUT_TIME);
        rdm_assign_port ( RDM_DIAG_SRVC,
                          RDM_QBI_DIAG_DEV,
                          NULL);
      }
      else
#endif
      if (diagcomm_retry_count < DIAGCOMM_RETRY_MAX_CNT)
      {
        diagcomm_retry_count++;
        diag_suspend (DIAG_TOUT_TIME);

       /*rdm_assign_port ( RDM_DIAG_SRVC,
                         RDM_ONDEVICE_DIAG_DEV,
                         NULL );*/
      }
    }
#endif /* DIAG_RUNTIME_DEVMAP */

    diag_io_detect_connection( conn );

    if( diagcomm_is_ondevice_mode(conn) )
    {
      diag_control_sleep_voting(0);
      diag_allow_sleep();
    }
  }

  #ifdef DIAG_CONSUMER_API
  else if (DIAGCOMM_PORT_FUSION_DCI == port_num)
  {
    if( !conn->connected )
    {
    if( diagcomm_io_open(conn)== TRUE )
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diagcomm_sio_open (DIAGCOMM_PORT_FUSION_DCI) - CHANNEL OPENED.");
        conn->connected = TRUE;
      }
    }

    #if defined (DIAG_RUNTIME_DEVMAP)
    if (conn->connected)
    {
      rdm_notify (RDM_DIAG_DCI_SRVC, RDM_DONE_S);
    }
    else
    {
      rdm_notify (RDM_DIAG_DCI_SRVC, RDM_NOT_ALLOWED_S);
    }
#endif /* DIAG_RUNTIME_DEVMAP */

  }
  #endif /* #ifdef DIAG_CONSUMER_API */
  return conn->connected;

} /* diagcomm_sio_open */



/*===========================================================================

FUNCTION DIAGCOMM_SIO_CONN_CHANGED

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  connected - True: SIO data channel has just changed to connected
              False: SIO data channel has just changed to dis-connected

RETURN VALUE
  None

===========================================================================*/
void diagcomm_sio_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

#if defined(DIAG_QDSS_SUPPORT)
  static boolean event_sent = FALSE;
#endif

  ASSERT( conn !=NULL );

  if (!connected)
  {
    conn->channel_connected = FALSE;

    /* Data SIO channel closed */
    //if( DIAGCOMM_IO_DATA == conn->channel_type )
    //{
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
   if(diag_tx_mode.mode == DIAG_TX_MODE_STREAMING)
   {
      return_val = osal_reset_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
   }
#endif

      if( (conn->port_type == DIAGCOMM_PORT_SIO) && (conn->port_num == DIAGCOMM_PORT_1) && (DIAGCOMM_IO_DATA == conn->channel_type) )
      {
         if( diag_first_sio_DTR_check == FALSE )
         {
           return_val = osal_set_sigs(&diag_task_tcb, DIAG_COMM_SIO_TX_FLUSH_SIG , &return_sigs);
           ASSERT(OSAL_SUCCESS == return_val);
         }
      }
      else
      {
         return_val = osal_set_sigs(&diag_task_tcb, DIAG_COMM_SIO_TX_FLUSH_SIG , &return_sigs);
         ASSERT(OSAL_SUCCESS == return_val);
      }
    //}
  }
  else
  {
    conn->channel_connected = TRUE;

#if defined(DIAG_QDSS_SUPPORT)
    /* SIO, Port 1, Data is Apps <-> USB or Modem <-> Apps */
    if( (conn->port_num == DIAGCOMM_PORT_1) &&
        (conn->channel_type == DIAGCOMM_IO_DATA) &&
        (event_sent == FALSE) )
    {
      tracer_event_simple(DIAG_SWE_RX_READY);
      event_sent = TRUE;
    }
#endif

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
    if(diagcomm_status())
    {
      diag_rx_rsp_notify();
      if(diag_tx_mode.mode == DIAG_TX_MODE_STREAMING)
      {
      return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
      }
    }
#endif
  }
} /* diagcomm_sio_conn_changed */

/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the current connection, if any.

===========================================================================*/
void
diagcomm_close ( diagcomm_enum_port_type port_num )
{
  if ((port_num >= DIAGCOMM_PORT_NUM_FIRST) && (port_num <= DIAGCOMM_PORT_NUM_LAST))
  {
     diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];

     if (conn->connected)
     {
       /* Flush the diagnostic subsystem's output buffering system. */
       diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

   #if defined (DIAG_RUNTIME_DEVMAP)
       conn->connected = FALSE;
       diagcomm_io_close( conn );
   #endif /* DIAG_RUNTIME_DEVMAP  */

       diagpkt_filtered_cmd_set = FALSE;  /* Reset filtered set */

       diag_control_sleep_voting(1);
   #if defined (DIAG_RUNTIME_DEVMAP)
       if (port_num == DIAGCOMM_PORT_1)
       {
         rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
       }
     #ifdef DIAG_CONSUMER_API
       else if(port_num == DIAGCOMM_PORT_FUSION_DCI)
       {
         rdm_notify (RDM_DIAG_DCI_SRVC, RDM_DONE_S);
       }
     #endif /* #ifdef DIAG_CONSUMER_API */

   #endif  /* DIAG_RUNTIME_DEVMAP */

       /* success or not, consider close operation successful */
       conn->connected = FALSE;
     }
  }
} /* diagcomm_close */

/*===========================================================================

 FUNCTION diagcomm_sio_tx_flush

 DESCRIPTION
   This function flushes outbound water mark queue and turns off all the masks
   to not generate any more traffic

===========================================================================*/
void diagcomm_sio_tx_flush( void )
{
 /*Turn off all message,log,event masks  */
  msg_set_all_rt_masks_adv (0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mstr_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  event_set_all_rt_masks(0, 0);

  /* Clear the diag connected status */
  DIAG_CLR_MASK (diag_cx_state, DIAG_CX_FLUSH_S | DIAG_CX_COMM_S);

  /* Empty the water mark queue if there are any items pending since smd is not going to free
     them as the port is closed */
  dsm_empty_queue(&diagcomm_sio_tx_wmq[DIAGCOMM_PORT_1]);
  dsm_empty_queue(&diagcomm_sio_rx_wmq[DIAGCOMM_PORT_1]);

} /* diagcomm_sio_tx_flush */


/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_RX

 DESCRIPTION
   Flushes inbound queue.  This is used when a security timeout occurs,
   though the capability is generic.

===========================================================================*/
void
diagcomm_flush_rx (void (*flush_cb) (void))
{
  dsm_empty_queue (&diagcomm_sio_rx_wmq[DIAGCOMM_PORT_1]);

  if (flush_cb)
  {
    flush_cb ();
  }
} /* diagcomm_flush_rx */

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX_CB

 DESCRIPTION
   Called by SIO to ack the successful completion of a TX flush.

===========================================================================*/
static void
diagcomm_flush_tx_cb (void)
{
  diag_release ();

} /* diagcomm_flush_tx_cb */

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX

 DESCRIPTION
   Flushes the transmit channel on current connection.

===========================================================================*/
void
diagcomm_flush_tx (void (*flush_cb) (void), unsigned int timeoutVal)
{
  if (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].connected)
  {
    diagcomm_io_flush_tx( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], diagcomm_flush_tx_cb,timeoutVal );
  }

  if (flush_cb)
  {
    flush_cb ();
  }

} /* diagcomm_flush_tx */

/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

===========================================================================*/
boolean
diagcomm_status (void)
{
  return diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].connected;
}


/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound
  data is available, NULL is returned.

PARAMETERS
  stream_len: pointer to an unsigned int for storing the length of the inbound data
  port_num: Indicate the port number to get the inbound data from

RETURN VALUE
  NONE

===========================================================================*/
void *
diagcomm_inbound (unsigned int *stream_len, diagcomm_enum_port_type port_num)
{
  void *return_ptr = NULL;

  ASSERT (stream_len);
  #ifdef DIAG_CONSUMER_API
    ASSERT (port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_FUSION_DCI);
  #else
    ASSERT (port_num == DIAGCOMM_PORT_1);
  #endif /* #ifdef DIAG_CONSUMER_API */
  if (diagcomm_sio_inbound_item_ptr != NULL)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempt to get more than one inbound buffer.");

    *stream_len = 0;
  }
  else
  {
    diagcomm_sio_inbound_item_ptr = diagcomm_io_dequeue_rx_wmq( diagcomm_get_sio_conn(port_num) );

    if (diagcomm_sio_inbound_item_ptr != NULL)
    {
      ASSERT (diagcomm_sio_inbound_item_ptr->data_ptr != NULL);

      *stream_len = diagcomm_sio_inbound_item_ptr->used;

      return_ptr = diagcomm_sio_inbound_item_ptr;
      diagcomm_sio_inbound_item_ptr = NULL;

    }
  }

  return return_ptr;

} /* diagcomm_inbound */

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void
diagcomm_inbound_complete (void *ptr)
{
  ASSERT (ptr != NULL);

  if (diagcomm_sio_inbound_item_ptr != NULL &&
      ptr == diagcomm_sio_inbound_item_ptr->data_ptr)
  {
    (void) dsm_free_packet (&diagcomm_sio_inbound_item_ptr);
    diagcomm_sio_inbound_item_ptr = NULL;
  }
} /* diagcomm_inbound_complete */


/*===========================================================================

FUNCTION DIAGCOMM_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns the rx watermark queue count.

PARAMETERS
  port_num: Indicate the port number of the rx matermark queue

RETURN VALUE
  Returns the number of items in the rx watermark queue


===========================================================================*/
uint32 diagcomm_get_rx_wmq_cnt(diagcomm_enum_port_type port_num)
{
  #ifdef DIAG_CONSUMER_API
    ASSERT(port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_FUSION_DCI); /* Removed assert for || port_num == DIAGCOMM_PORT_FUSION_DCI */
  #else
    ASSERT(port_num == DIAGCOMM_PORT_1);
  #endif /* #ifdef DIAG_CONSUMER_API */
  return (*(diagcomm_sio_rx_wmq[port_num].q_ptr)).cnt;

} /* diagcomm_get_rx_wmq_cnt */


/*===========================================================================

FUNCTION DIAGCOMM_CONNECT_STATE

DESCRIPTION

===========================================================================*/
boolean diagcomm_connect_state()
{
  return diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].channel_connected;
}


/*===========================================================================

FUNCTION DIAGCOMM_SIO_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communications layer.

PARAMETERS
  port_num: Indicate the port number of the channel

RETURN VALUE
  NONE
===========================================================================*/
void
diagcomm_sio_init (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];

  diagpkt_filtered_cmd_set = FALSE;

  /* Init callbacks, etc */
  diagcomm_params.inbound_cb = NULL;
  diagcomm_params.tx_start_cb = NULL;
  diagcomm_params.tx_stop_cb = NULL;

  /* Make sure we are closed */
  diagcomm_close(port_num);

  /* Initialize watermark structures */
  memset (&diagcomm_sio_rx_wmq[port_num], 0, sizeof (dsm_watermark_type));
  memset (&diagcomm_sio_tx_wmq[port_num], 0, sizeof (dsm_watermark_type));

  if(DIAGCOMM_PORT_1 == port_num)
  {
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SIO_TX_ITEM_POOL, /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_MANY,        /*dsm_mem_level_enum_type level */
    DSMI_DIAG_SIO_TX_ITEM_MANY_MARK /* uint32 avail_item_count */
  );
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SIO_TX_ITEM_POOL, /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_FEW,        /*dsm_mem_level_enum_type level */
    DSMI_DIAG_SIO_TX_ITEM_FEW_MARK/* uint32 avail_item_count */
  );

  /* Register the call-backs for the MANY and FEW levels */
  dsm_reg_mem_event_cb
  (
    DSM_DIAG_SIO_TX_ITEM_POOL, /* dsm_mempool_id_type      pool_id*/
    DSM_MEM_LEVEL_FEW,        /*dsm_mem_level_enum_type  level */
    DSM_MEM_OP_NEW,             /* dsm_mem_op_enum_type     mem_op */
    diagcomm_sio_dsm_high_mem_event_cb  /* pointer to call-back function */
  );
  dsm_reg_mem_event_cb
  (
    DSM_DIAG_SIO_TX_ITEM_POOL, /* dsm_mempool_id_type      pool_id*/
    DSM_MEM_LEVEL_MANY,        /*dsm_mem_level_enum_type  level */
    DSM_MEM_OP_FREE,             /* dsm_mem_op_enum_type     mem_op */
    diagcomm_sio_dsm_low_mem_event_cb  /* pointer to call-back function */
  );

  dsm_queue_init
  (
    &diagcomm_sio_rx_wmq[port_num], /* watermark queue */
    DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ, /* dont_exceed_cnt */
    &diagcomm_sio_rx_q[port_num]  /* queue */
  );

  conn->rx_wmq_ptr = &diagcomm_sio_rx_wmq[port_num];

  /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
#ifdef FEATURE_DIAG_SWITCH
  diagcomm_sio_rx_wmq.lo_watermark = DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ;
  diagcomm_sio_rx_wmq.hi_watermark = DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ;
  diagcomm_sio_rx_wmq.dont_exceed_cnt = DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ;
#endif /* FEATURE_DIAG_SWITCH */

  dsm_set_low_wm
  (
    &diagcomm_sio_rx_wmq[port_num],
    DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ
  );
  dsm_set_hi_wm
  (
    &diagcomm_sio_rx_wmq[port_num],
    DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ
  );

    dsm_queue_init
    (
      &diagcomm_sio_tx_wmq[port_num],                                /* watermark queue */
      DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ, /* dont_exceed_cnt */
      &diagcomm_sio_tx_q[port_num]                                   /* queue */
    );


  #if !defined(DIAG_BUFFERING_SUPPORT)

  dsm_set_low_wm
  (
    &diagcomm_sio_tx_wmq[port_num],
    DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ
  );

  dsm_set_hi_wm
  (
    &diagcomm_sio_tx_wmq[port_num],
    DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ
  );
#endif

  diagcomm_io_open_init( conn );

#if defined(DIAG_SMDL_SUPPORT) && defined(DIAG_IMAGE_RIVA_PROC)
  {
    osal_sigs_t return_sigs;
    (void)osal_set_sigs(&diag_task_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);
  }
#endif
  }
  #ifdef DIAG_CONSUMER_API
  else if (DIAGCOMM_PORT_FUSION_DCI == port_num)
  {
    dsm_queue_init
    (
      &diagcomm_sio_rx_wmq[port_num], /* watermark queue */
      DSM_DIAG_SIO_RX_ITEM_CNT * DSM_DIAG_SIO_RX_ITEM_SIZ, /* dont_exceed_cnt */
      &diagcomm_sio_rx_q[port_num]  /* queue */
    );
    dsm_queue_init
    (
      &diagcomm_sio_tx_wmq[port_num],                                /* watermark queue */
      DSM_DIAG_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_SMD_DCI_TX_ITEM_SIZ,           /* dont_exceed_cnt */
      &diagcomm_sio_tx_q[port_num]                                   /* queue */
    );

    conn->rx_wmq_ptr = &diagcomm_sio_rx_wmq[port_num];

      diagcomm_io_open_init( conn );
#if defined(DIAG_SMDL_SUPPORT) && defined(DIAG_IMAGE_RIVA_PROC)
  {
    osal_sigs_t return_sigs;
    (void)osal_set_sigs(&diag_task_tcb, DIAG_FUSION_OPEN_SIG, &return_sigs);
  }
#endif
  }
  #endif /* #ifdef DIAG_CONSUMER_API */ 

} /* diagcomm_sio_init */


/*===========================================================================
FUNCTION DIAGCOMM_SIO_OPEN_INIT

DESCRIPTION

===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void diagcomm_sio_open_init( diagcomm_io_conn_type * conn )
{
  ASSERT( conn !=NULL );

  if( DIAGCOMM_PORT_SIO != conn->port_type )
    return;

  /* Now prepare parameters for opening a port */
  conn->open_params.sio_params.params.stream_mode = SIO_DM_MODE;
  conn->open_params.sio_params.params.rx_queue = &diagcomm_sio_rx_wmq[conn->port_num];
  conn->open_params.sio_params.params.tx_queue = &(diagcomm_sio_tx_wmq[conn->port_num]);

  /* Normally, this layer doesn't know about the HDLC encoding of the diag
     protocol.  However, SIO is configured to look for tail character. */
#if !defined (DIAG_FRAMING)
#ifdef FEATURE_DIAG_SWITCH
  if(diag_on_single_proc != DIAG_FTM_SWITCH_VAL)
  {
#endif
    conn->open_params.sio_params.params.tail_char_used = FALSE;
    conn->open_params.sio_params.params.tail_char = DIAG_NO_ASYNC_CONTROL_CHAR;
#ifdef FEATURE_DIAG_SWITCH
  }
  else if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
  {
    conn->open_params.sio_params.params.tail_char_used = TRUE;
    conn->open_params.sio_params.params.tail_char = DIAG_ASYNC_CONTROL_CHAR;
  }
#endif
#else
#ifdef FEATURE_DIAG_SWITCH
  if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
  {
    conn->open_params.sio_params.params.tail_char_used = FALSE;
    conn->open_params.sio_params.params.tail_char = DIAG_NO_ASYNC_CONTROL_CHAR;
  }
  else
  {
#endif
  /* The below code gets executed for both DIAG OVER USB as well as DIAG OVER UART.
     Though in case of USB, tail_char is not used by the USB driver.tail_char_used 
	 is set to FALSE so that DIAG receives data sent by tool unfiltered from SIO. */
    conn->open_params.sio_params.params.tail_char_used = FALSE;
    conn->open_params.sio_params.params.tail_char = DIAG_NO_ASYNC_CONTROL_CHAR;
#ifdef FEATURE_DIAG_SWITCH
  }
#endif
#endif

  /* Use whatever the flow control method is currently. */
  conn->open_params.sio_params.params.rx_flow = SIO_FCTL_OFF;
  conn->open_params.sio_params.params.tx_flow = SIO_FCTL_OFF;

  /* Default DM bit rate is 38.4Kbps unless specified in NV.
     Now, it uses 115.2 (the max) as the default. */
  conn->open_params.sio_params.params.rx_bitrate = SIO_BITRATE_115200;
  conn->open_params.sio_params.params.tx_bitrate = SIO_BITRATE_115200;

  /* Setting callbacks */
  if(DIAGCOMM_PORT_1 == conn->port_num)
  {
    /* Setting callbacks */
    conn->open_params.sio_params.params.rx_func_ptr = diagcomm_sio_inbound_pkt_notify;

  #if defined (DIAG_RUNTIME_DEVMAP)|| defined (FEATURE_DIAG_SWITCH)
    conn->open_params.sio_params.close_func_ptr = diagcomm_sio_close_done_cb;
  #else
    conn->open_params.sio_params.close_func_ptr = NULL;
  #endif

  }
  #ifdef DIAG_CONSUMER_API
    else if(DIAGCOMM_PORT_FUSION_DCI == conn->port_num)
    {
      /* Set callback functions */
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_sio_fusion_dci_inbound_pkt_notify;
      conn->open_params.sio_params.close_func_ptr = diagcomm_sio_close_done_cb;
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
#if defined (DIAG_RUNTIME_DEVMAP)||  defined (FEATURE_DIAG_SWITCH)
  conn->open_params.sio_params.close_func_ptr = diagcomm_sio_close_done_cb;
#else
  conn->open_params.sio_params.close_func_ptr = NULL;
#endif

  /* Passing the pool ID to SMD */
  if (DIAGCOMM_PORT_1 == conn->port_num)
  {
     conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_RX_ITEM_POOL;
  }
  #ifdef DIAG_CONSUMER_API
    else if (DIAGCOMM_PORT_FUSION_DCI == conn->port_num)
    {
       conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_FUSION_RX_ITEM_POOL;
    }
  #endif /* #ifdef DIAG_CONSUMER_API */

  conn->open_params.sio_params.open_param.dsm_pool_param.mempool_set = 0;
  conn->open_params.sio_params.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_LEGACY;

  conn->open_params.sio_params.params.open_param = &(conn->open_params.sio_params.open_param);

} /* diagcomm_sio_open_init */
#endif /* DIAG_SIO_SUPPORT */

/*lint -restore */

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC

DESCRIPTION
  This function requests a buffer from the diag communications layer.

===========================================================================*/
static uint32 diag_outbound_alloc_compressed_fail = 0;
static uint32 diag_outbound_alloc_ctrl_fail = 0;

dsm_item_type *
diagcomm_outbound_alloc (unsigned int *count, boolean is_chain, boolean priority, boolean is_cmd_rsp_type)
{
  dsm_item_type *item_ptr = NULL;
  dsm_item_type *tmp = NULL;
  unsigned int chain_size = 0;
  ASSERT (count);

  if (is_cmd_rsp_type)
  {
     item_ptr = dsm_new_buffer(DSM_DIAG_CMD_SIO_TX_ITEM_POOL);
     tmp = diagcomm_sio_cmd_outbound_item_ptr;
  }
  else
  {
     item_ptr = dsm_new_buffer(DSM_DIAG_SIO_TX_ITEM_POOL);
     osal_enter_crit_sect(&diag_master_tx_mutex);
     tmp = diagcomm_sio_outbound_item_ptr;
  }
  if (item_ptr)
  {
    item_ptr->pkt_ptr = NULL;
    if (tmp == NULL)
    {
       if (is_cmd_rsp_type)
       {
           item_ptr->priority = DSM_HIGHEST;
           *count = item_ptr->size;
           diagcomm_sio_cmd_outbound_item_ptr = item_ptr;
       }
       else
       {
          *count = item_ptr->size;
          diagcomm_sio_outbound_item_ptr = item_ptr;
          osal_exit_crit_sect(&diag_master_tx_mutex);
        }
    }
    else if (is_chain)
    {
        diagbuf_incr_dsm_chained_count();

        while(tmp->pkt_ptr != NULL)
        {
          chain_size += tmp->size;

          if (is_cmd_rsp_type)
          {
            ASSERT(chain_size <= (DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ));
          }
          else
          {
            ASSERT(chain_size <= (DIAG_TX_MAX_CHAINED_ITEMS_SIZ));
          }

          tmp = tmp->pkt_ptr;
        }

        tmp->pkt_ptr = item_ptr;

        if(!is_cmd_rsp_type)
          osal_exit_crit_sect(&diag_master_tx_mutex);

     }
     else
     {
       if(!is_cmd_rsp_type)
         osal_exit_crit_sect(&diag_master_tx_mutex);
       ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
     }
  }
  else
  {
    if(is_cmd_rsp_type)
    {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Ran out of CMD SIO TX Item Pool DSM items");
        drop_count_sio_cmd++;
        *count = 0;
    }
    else
    {
        osal_exit_crit_sect(&diag_master_tx_mutex);
        drop_count_sio++;
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Ran out of SIO TX Item Pool DSM items");
        *count = 0;
    }
  }
   diagbuf_incr_dsm_count();

  return item_ptr;
}

/*===========================================================================
FUNCTION DIAGCOMM_OUTBOUND_ALLOC_COMPRESSED
DESCRIPTION
  These functions requests a buffer from the diag communications layer to send the
  compressed data.

===========================================================================*/
dsm_item_type *
diagcomm_outbound_alloc_compressed (unsigned int *count, boolean is_chain, boolean priority)
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);
    item_ptr = dsm_new_buffer (DSM_DIAG_SIO_TX_ITEM_POOL);
    if (item_ptr)
    {
     *count = item_ptr->size;
     item_ptr->pkt_ptr = NULL;
    }
    else
    {
       MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Ran out of SIO Tx Item Pool DSM items");
       if( diag_outbound_alloc_compressed_fail != MAX_VALUE_UINT32 )
      {
         diag_outbound_alloc_compressed_fail++;
      }
        *count = 0;
#if defined(DIAG_DSM_DEBUG)
       ASSERT(0);
#endif
    }
  if (item_ptr)
  {
    if ( diagcomm_sio_outbound_item_ptr_compressed== NULL)
    {
      diagcomm_sio_outbound_item_ptr_compressed= item_ptr;
    }
    else if (is_chain)
    {
      unsigned int chain_size = 0;
      dsm_item_type *tmp = diagcomm_sio_outbound_item_ptr_compressed;
      diagbuf_incr_dsm_chained_count();
      while(tmp->pkt_ptr != NULL)
      {
         chain_size += tmp->size;
         if (priority)
         {
            ASSERT(chain_size <= DIAG_RX_RSP_MAX_CHAIN_SIZ);
         }
         else
         {
            ASSERT(chain_size < DIAG_TX_MAX_CHAIN_SIZ);
         }
         tmp = tmp->pkt_ptr;
      }
      tmp->pkt_ptr = item_ptr;
   }
   else
   {
      ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
   }
  }
  return item_ptr;
} /* diagcomm_outbound_alloc_compressed */

/*===========================================================================

FUNCTION DIAGCOMM_MASTER_OUTBOUND_ALLOC_HDLC

DESCRIPTION
  This function requests a buffer from the diag communications layer when HDLC
  encoding has to be performed by APPS on its own traffic in streaming mode.

===========================================================================*/
dsm_item_type *
diagcomm_master_outbound_alloc_hdlc (unsigned int *count, boolean is_chain, boolean priority)
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);

    item_ptr = dsm_new_buffer (DSM_DIAG_SIO_TX_ITEM_POOL);

    if (item_ptr)
    {
      *count = item_ptr->size;
      item_ptr->pkt_ptr = NULL;
    }

    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Ran out of SIO Tx Item Pool DSM items");
      *count = 0;
#if defined(DIAG_DSM_DEBUG)
      ASSERT(0);
#endif
    }

  if (item_ptr)
  {
   
    if ( diagcomm_sio_master_outbound_item_ptr_hdlc== NULL)
    {
      diagcomm_sio_master_outbound_item_ptr_hdlc= item_ptr;
    }

    else if (is_chain)
    {
        unsigned int chain_size = 0;
        dsm_item_type *tmp = diagcomm_sio_master_outbound_item_ptr_hdlc;

        diagbuf_incr_dsm_chained_count();

        while(tmp->pkt_ptr != NULL)
        {
          chain_size += tmp->size;

          if (priority)
          {
             ASSERT(chain_size <= DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ);
          }
          else
          {
            ASSERT(chain_size <= DIAG_TX_MAX_CHAINED_ITEMS_SIZ);
          }

          tmp = tmp->pkt_ptr;
        }

        tmp->pkt_ptr = item_ptr;

    }

    else
    {
          ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
    }
  }

  return item_ptr;

} /* diagcomm_master_outbound_alloc_hdlc */


/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC_HDLC

DESCRIPTION
  This function requests a buffer from the diag communications layer when HDLC
  encoding has to be performed by APPS on forward channel

===========================================================================*/
dsm_item_type *
diagcomm_outbound_alloc_hdlc (unsigned int *count, boolean is_chain, boolean priority)
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);

    item_ptr = dsm_new_buffer (DSM_DIAG_SIO_TX_ITEM_POOL);

    if (item_ptr)
    {
      *count = item_ptr->size;
      item_ptr->pkt_ptr = NULL;
    }

    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Ran out of SIO Tx Item Pool DSM items");
      *count = 0;
#if defined(DIAG_DSM_DEBUG)
      ASSERT(0);
#endif
    }

  if (item_ptr)
  {
   
    if ( diagcomm_sio_outbound_item_ptr_hdlc== NULL)
    {
      diagcomm_sio_outbound_item_ptr_hdlc= item_ptr;
    }

    else if (is_chain)
    {
        unsigned int chain_size = 0;
        dsm_item_type *tmp = diagcomm_sio_outbound_item_ptr_hdlc;

        diagbuf_incr_dsm_chained_count();

        while(tmp->pkt_ptr != NULL)
        {
          chain_size += tmp->size;

          if (priority)
          {
             ASSERT(chain_size <= DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ);
          }
          else
          {
            ASSERT(chain_size <= DIAG_TX_MAX_CHAINED_ITEMS_SIZ);
          }

          tmp = tmp->pkt_ptr;
        }

        tmp->pkt_ptr = item_ptr;

    }

    else
    {
          ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
    }
  }

  return item_ptr;

} /* diagcomm_outbound_alloc_hdlc */


/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC_CTRL

DESCRIPTION

===========================================================================*/
dsm_item_type *
diagcomm_outbound_alloc_ctrl (unsigned int *count, boolean is_chain, boolean priority)
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);

    item_ptr = dsm_new_buffer (DSM_DIAG_SIO_TX_ITEM_POOL);

    if (item_ptr)
    {
     *count = item_ptr->size;
     item_ptr->pkt_ptr = NULL;
    }

    else
    {
        if( diag_outbound_alloc_ctrl_fail != MAX_VALUE_UINT32 )
        {
          diag_outbound_alloc_ctrl_fail++;
        }

        MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Ran out of (CTRL) SIO Tx Item Pool DSM items");
        *count = 0;

#if defined(DIAG_DSM_DEBUG)
        ASSERT(0);
#endif
    }

  if (item_ptr)
  {

    if ( diagcomm_sio_outbound_item_ptr_ctrl == NULL)
    {
      diagcomm_sio_outbound_item_ptr_ctrl = item_ptr;
    }

    else if (is_chain)
    {
        unsigned int chain_size = 0;
        dsm_item_type *tmp = diagcomm_sio_outbound_item_ptr_ctrl;

        diagbuf_incr_dsm_chained_count();

        while(tmp->pkt_ptr != NULL)
        {
          chain_size += tmp->size;

          if (priority)
          {
            ASSERT(chain_size <= DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ);
          }
          else
          {
            ASSERT(chain_size <= DIAG_TX_MAX_CHAINED_ITEMS_SIZ);
          }

          tmp = tmp->pkt_ptr;
        }

        tmp->pkt_ptr = item_ptr;

    }

    else
    {
          ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
    }
  }
   diagbuf_incr_dsm_count();

  return item_ptr;
}
 /* diagcomm_outbound_alloc_ctrl */


/*===========================================================================

FUNCTION DIAGCOMM_SEND

DESCRIPTION
  Sends the given data stream if the connection is open.

===========================================================================*/
void
diagcomm_send (dsm_item_type * ptr, unsigned int length, boolean is_cmd_rsp_type)
{
 diagcomm_io_tx_params_type tx_params;
 boolean buffered_mode = FALSE;
 uint8 mode = diag_get_tx_mode();

 ASSERT (ptr);

 if((is_cmd_rsp_type) && (diagcomm_sio_cmd_outbound_item_ptr == NULL))
 {
     MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_sio_cmd_outbound_item_ptr is NULL. Nothing to send");
     return;
 }
 else if ( (!is_cmd_rsp_type) && (diagcomm_sio_outbound_item_ptr == NULL) )
 {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_sio_outbound_item_ptr is NULL. Nothing to send");
      return;
 }
 else
 {
    if (length > 0)
    {
      /* If we're in buffering modes, enqueue the DSM instead of sending to SIO immediately. */
      #if defined(DIAG_BUFFERING_SUPPORT)
        if( is_cmd_rsp_type == TRUE )
        {
          /* Do not buffer responses. */
          buffered_mode = FALSE;
        }
        /* Enqueue data in wmq if mode is not streaming i.e. buffering */
        else if( ptr->priority != DSM_HIGHEST && mode!=DIAG_TX_MODE_STREAMING)
        {
          buffered_mode = TRUE;
          /* Protecting diagcomm_sio_outbound_item_ptr with the mutex. */
          osal_enter_crit_sect(&diag_master_tx_mutex);
          dsm_enqueue (&diagcomm_smd_rx_wmq[DIAGCOMM_PORT_1], &diagcomm_sio_outbound_item_ptr);
          osal_exit_crit_sect(&diag_master_tx_mutex);
          diag_fwd_notify();
        }
      #endif

      if( buffered_mode == FALSE )
      {

        if(is_cmd_rsp_type)
        {
            tx_params.dsm_ptr = diagcomm_sio_cmd_outbound_item_ptr;
            diagcomm_io_transmit( diagcomm_get_sio_conn(DIAGCOMM_PORT_1), &tx_params );
            diagcomm_sio_cmd_outbound_item_ptr = NULL;

        }
        else
        {
            /* diagcomm_io_transmit is directly not called here,
               as it needs to be checked if hdlc encoding is 
               needed or not */
            diagcomm_send_master_tx(DIAGCOMM_PORT_1);

        }

      }
    }
    else
    {
      osal_enter_crit_sect(&diag_master_tx_mutex);
      /* This case can be hit if diag is not connected
         and diagcomm_send is called with length 0. In
         such a case just drop the pending outbound data. */
      (void) dsm_free_packet (&diagcomm_sio_outbound_item_ptr);
      osal_exit_crit_sect(&diag_master_tx_mutex);
    }


 }
} /* diagcomm_send */
/*===========================================================================
FUNCTION DIAGCOMM_SEND_COMPRESSED
DESCRIPTION
 This function sends the compressed chunk to the SIO layer

===========================================================================*/
 void
diagcomm_send_compressed (dsm_item_type * ptr, unsigned int length)
{
 diagcomm_io_tx_params_type tx_params;
 ASSERT (ptr);
 if (diagcomm_sio_outbound_item_ptr_compressed== NULL)
 {
   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "sio_oubound_ptr is NULL. Nothing to send");
 }
 else
 {
    if (length > 0)
    {
#if defined(DIAG_BUFFERING_SUPPORT)
      tx_params.dsm_ptr = diagcomm_sio_outbound_item_ptr_compressed;
      diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
#endif
   }
    else
    {
      if (diagcomm_sio_outbound_item_ptr_compressed->used > 0)
       ASSERT(0);
      (void) dsm_free_packet (&diagcomm_sio_outbound_item_ptr_compressed);
    }
    diagcomm_sio_outbound_item_ptr_compressed= NULL;
 }
} /* diagcomm_send */


/*===========================================================================

FUNCTION DIAGCOMM_SEND_CTRL

DESCRIPTION
  Sends the given data stream for control packets if the connection is open.

===========================================================================*/
void
diagcomm_send_ctrl (dsm_item_type * ptr, unsigned int length)
{
 diagcomm_io_tx_params_type tx_params;

 ASSERT (ptr);
  if (diagcomm_sio_outbound_item_ptr_ctrl == NULL)
 {
    /* Corrupt state.*/
     MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "sio_oubound_ptr is NULL. Nothing to send");
 }
  else
 {
    if (length > 0)
    {
      tx_params.dsm_ptr = diagcomm_sio_outbound_item_ptr_ctrl;
      diagcomm_io_transmit( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
    }
    else
    {
      if (diagcomm_sio_outbound_item_ptr_ctrl->used > 0)
        ASSERT(0);
               /* If empty, just release it */
      (void) dsm_free_packet (&diagcomm_sio_outbound_item_ptr_ctrl);
    }
    diagcomm_sio_outbound_item_ptr_ctrl = NULL;
 }
} /* diagcomm_send_ctrl */

/*===========================================================================

FUNCTION DIAGCOMM_SEND_MASTER_HDLC

DESCRIPTION
  This function is called only when APPS HDLC encodes the data on its
  own data traffic while in streaming mode. Sends the given data stream
  if the connection is open.

===========================================================================*/
void
diagcomm_send_master_hdlc (dsm_item_type * ptr, unsigned int length)
{
 diagcomm_io_tx_params_type tx_params;

 ASSERT (ptr);

 if (diagcomm_sio_master_outbound_item_ptr_hdlc== NULL)
 {
   /* Corrupt state.*/
   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"sio_master_oubound_ptr is NULL. Nothing to send");
 }
 else
 {
    if (length > 0)
    {

          tx_params.dsm_ptr = diagcomm_sio_master_outbound_item_ptr_hdlc;
          diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
    }
    else
    {
      if (diagcomm_sio_master_outbound_item_ptr_hdlc->used > 0)
       ASSERT(0);

      /* If empty, just release it */
      (void) dsm_free_packet (&diagcomm_sio_master_outbound_item_ptr_hdlc);
    }

    diagcomm_sio_master_outbound_item_ptr_hdlc= NULL;
 }
} /* diagcomm_send_master_hdlc */


/*===========================================================================

FUNCTION DIAGCOMM_SEND_HDLC

DESCRIPTION
  This function is called only when APPS HDLC encodes the data on forward
  channel. Sends the given data stream if the connection is open.

===========================================================================*/
void
diagcomm_send_hdlc (dsm_item_type * ptr, unsigned int length)
{
 diagcomm_io_tx_params_type tx_params;

 ASSERT (ptr);

 if (diagcomm_sio_outbound_item_ptr_hdlc== NULL)
 {
   /* Corrupt state.*/
   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"sio_oubound_ptr is NULL. Nothing to send");
 }
 else
 {
    if (length > 0)
    {

          tx_params.dsm_ptr = diagcomm_sio_outbound_item_ptr_hdlc;
          diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
    }
    else
    {
      if (diagcomm_sio_outbound_item_ptr_hdlc->used > 0)
       ASSERT(0);

      /* If empty, just release it */
      (void) dsm_free_packet (&diagcomm_sio_outbound_item_ptr_hdlc);
    }

    diagcomm_sio_outbound_item_ptr_hdlc= NULL;
 }
} /* diagcomm_send_hdlc */

/*===========================================================================
FUNCTION diagcomm_sio_flow_enabled

DESCRIPTION
   This function tells whether the flow to DSM_DIAG_SIO_TX_ITEM_POOL is enabled
   or not.

   ToDo: Rename this function to reflect that this control flow control for
         Tx traffic rather than purely from SIO_TX pool.
         (ie: diagcomm_tx_flow_enabled)
===========================================================================*/
boolean diagcomm_sio_flow_enabled(void)
{
  return allowflow;

} /* diagcomm_sio_flow_enabled */

/*===========================================================================
FUNCTION diagcomm_sio_tx_flow_enabled

DESCRIPTION
   This function tells whether the flow to DSM_DIAG_SIO_TX_ITEM_POOL is enabled
   or not.
===========================================================================*/
boolean diagcomm_sio_tx_flow_enabled(void)
{
  return allowflow;
} /* diagcomm_sio_flow_enabled */

boolean diagcomm_compression_flow_enabled(void)
{
  boolean ret_val = TRUE;
  /* If the flow control is initiated on SIO TX ITEM Pool, stop the compression by returning FALSE
        if there is no space in compression buffer*/
   /* Compress as long as there is memory in compression buffer */
    if(diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH)
    {
      osal_lock_mutex(&diag_compression_mutex);
      ret_val = total_bytes_in_compressed_buffer < DIAG_COMPRESSED_BUFFER_MAX_THRESHOLD;
      osal_unlock_mutex(&diag_compression_mutex);
    }

  return ret_val;
}

/*===========================================================================
FUNCTION diagcomm_sio_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count
   for DSM_DIAG_SIO_TX_ITEM_POOL is reached.
   (DSM_DIAG_SIO_TX_ITEM_POOL is used by sio, to receive data.)

===========================================================================*/
static void diagcomm_sio_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                   dsm_mem_level_enum_type mem_level,
                                                   dsm_mem_op_enum_type mem_op)
{
   /* check to be defensive */
   if(pool_id != DSM_DIAG_SIO_TX_ITEM_POOL ||
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_NEW)
      return;

   if (TRUE == allowflow)
   {
      allowflow = FALSE;
      diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SIO);
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW,"diagcomm_sio_dsm_high_mem_event_cb, disabled flow");
   }
} /* diagcomm_sio_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_sio_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count
   for DSM_DIAG_SIO_TX_ITEM_POOL is reached.
   (DSM_DIAG_SIO_TX_ITEM_POOL is used by sio, to receive data.)

===========================================================================*/
static void diagcomm_sio_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                                  dsm_mem_level_enum_type mem_level,
                                                  dsm_mem_op_enum_type mem_op)
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  int feature_bit_port1 = 0;
  int feature_bit_port2 = 0;

   /* check to be defensive */
   if(pool_id != DSM_DIAG_SIO_TX_ITEM_POOL ||
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_FREE)
      return;

   if (FALSE == allowflow)
   {
      /* Set flow control to allow flow */
      allowflow = TRUE;
  
      #ifdef DIAG_CONSUMER_API
        if (send_dci_conn_status)
        {
          (void)diag_set_internal_sigs( DIAG_INT_CONN_STATUS_SEND_SIG );
        }
      #endif /* #ifdef DIAG_CONSUMER_API */

      diag_tx_notify();

      /*Set the forward signal if HDLC encoding has been turned on in Apps and modem */
      feature_bit_port1 = DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS);
      feature_bit_port2 = DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS);

      if ((feature_bit_port1 && feature_bit_port2))
      {
        if (diagcomm_status() && (diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1)))
        {
          diag_fwd_notify();
        }
      }
      return_val = osal_set_sigs(&diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_sio_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_sio_dsm_low_mem_event_cb */


/*===========================================================================


FUNCTION DIAGCOMM_RESET_FLOW_CTRL_COUNT

DESCRIPTION
    This function resets the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.

===========================================================================*/
void diagcomm_reset_flow_ctrl_count( void )
{
  diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SMD] = 0;
  diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SIO] = 0;

}  /* diagcomm_reset_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_GET_FLOW_CTRL_COUNT

DESCRIPTION
    This function returns the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.

===========================================================================*/
uint32 diagcomm_get_flow_ctrl_count( diagcomm_port_type port_type )
{
  return diagcomm_flow_ctrl_count[port_type];

} /* diagcomm_get_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_INCR_FLOW_CTRL_COUNT

DESCRIPTION
    This function increments the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.

===========================================================================*/
uint32 diagcomm_incr_flow_ctrl_count( diagcomm_port_type port_type )
{
  EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type flow_ctrl_event_payload;

  if( diagcomm_flow_ctrl_count[port_type] == MAX_VALUE_UINT32 )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_flow_ctrl_count has reached max value");
  }
  else
  {
    diagcomm_flow_ctrl_count[port_type]++;
  }

  flow_ctrl_event_payload.alloc_f3_cnt = alloc_count_f3;
  flow_ctrl_event_payload.alloc_log_cnt = alloc_count_log;
  flow_ctrl_event_payload.drop_f3_cnt = drop_count_f3;
  flow_ctrl_event_payload.drop_log_cnt = drop_count_log;
  flow_ctrl_event_payload.drop_cnt_per = diag_per_count_dropped;
  flow_ctrl_event_payload.flow_ctrl_cnt_sio = diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SIO];
  flow_ctrl_event_payload.flow_ctrl_cnt_smd  = diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SMD];
  flow_ctrl_event_payload.flow_ctrl_cnt_dci = 0; /* TODO: Update this when flow control is added for the DCI Channel */
  flow_ctrl_event_payload.pd_id = DIAG_GUEST_OS_PD;
  flow_ctrl_event_payload.proc_id = (uint16) DIAG_MY_PROC_ID;
  flow_ctrl_event_payload.event_source = DIAG_FLOW_CTRL;

  event_report_payload(EVENT_DIAG_DROP_DEBUG, sizeof(EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type), &flow_ctrl_event_payload);

  return diagcomm_flow_ctrl_count[port_type];

} /* diagcomm_incr_flow_ctrl_count */

/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_DATA

DESCRIPTION
  This function processes the incoming data from command channel.

PARAMETERS
    None

RETURN VALUE
    None

===========================================================================*/

void diagcomm_process_data( void )
{
 if(diagcomm_smd_flow_enabled())
 {
  diagcomm_process_inbound_data (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
 }
}
/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_INBOUND_DATA

DESCRIPTION
  This function processes the incoming data from command channel.

PARAMETERS
    port_type       - DIAGCOMM_PORT_SMD OR DIAGCOMM_PORT_SIO
    channel_type    - DIAGCOMM_PORT_1 OR DIAGCOMM_PORT_2

RETURN VALUE
    None

===========================================================================*/

void
diagcomm_process_inbound_data( diagcomm_port_type port_type,
                                   diagcomm_enum_port_type channel_type )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[port_type][channel_type];
  uint32 num_used = 0;
  dsm_item_type **dsm_item_ptr= NULL;

  num_used = conn->used;
  if(num_used >0)
  {
  osal_lock_mutex(&conn->buf_mutex);

  memscpy(&diag_data_pkt_inbound[0], sizeof(diag_data_pkt_inbound), conn->buf,conn->used);
  conn->used = 0;

  osal_unlock_mutex(&conn->buf_mutex);

  if((DIAGCOMM_PORT_SMD == conn->port_type) &&  (DIAGCOMM_IO_DATA == conn->channel_type))
  {
    dsm_item_ptr = diagcomm_sio_data_to_dsm((uint8 *)&(diag_data_pkt_inbound[0]), num_used);
    diagcomm_enqueue_dsm(dsm_item_ptr);
  }
}
  return;
}
/*===========================================================================

FUNCTION DIAGCOMM_SIO_DATA_TO_DSM

DESCRIPTION
    This enqueues takes the data from given buffer and copies into
    the dsm item and returns the dsm item pointer back.(It takes care
    of chaining if packet is huge)

===========================================================================*/

dsm_item_type ** diagcomm_sio_data_to_dsm(uint8* pkt_ptr, uint32 pkt_len)
{
    /* temp pointer for working */
  dsm_item_type * dsm_item_ptr_sav = NULL;
  uint8 * diag_send_buf_pointer = NULL;
  uint32 length = 0;
  uint32 writelen = 0;

  dsm_item_ptr_sav = diagcomm_new_dsm( DSM_DIAG_SMD_TX_ITEM_POOL, FALSE );

  if(dsm_item_ptr_sav)
  {

    diag_send_buf_pointer = (uint8*)pkt_ptr;
    length = pkt_len;
    writelen = dsm_pushdown_tail(&dsm_item_ptr_sav, diag_send_buf_pointer, length,
                      DSM_DIAG_SMD_TX_ITEM_POOL);

    if(writelen == length)
    {
        diagcomm_sio_outbound_item_ptr_data = dsm_item_ptr_sav;
    }
  }

  return &diagcomm_sio_outbound_item_ptr_data;

} /* diagcomm_sio_data_to_dsm */
