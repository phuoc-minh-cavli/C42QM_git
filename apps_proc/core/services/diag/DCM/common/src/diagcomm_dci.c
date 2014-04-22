#ifdef DIAG_CONSUMER_API
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications
                   for Diag Consumer Interface

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when transmitting Consumer Interface messages.

Copyright (c) 2014-2017, 2019-2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * 
 *
 *  
 */
/*===========================================================================

                               Edit History

 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
04/28/17   gn      Initialized dte_ready_asserted in diagcomm_dci_dtr_changed_cb
07/20/16   nk      Changes to support ThreadX
05/29/15   rh      Now only flow-control channels which have been opened.
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
11/5/14    rs      Replaced EQUIP_ID_MAX with LOG_EQUIP_ID_MAX.
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/07/14   xy      Created 
===========================================================================*/

#include "diag_v.h"
#include "diagdsm.h" /*for DSM_DIAG_SMD_DCI_TX_ITEM_POOL*/
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_smd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "msg.h"
#include "dsm.h"
#include "sio.h"
#include "diagcomm_io.h"
#include "diagdsm_v.h"
#include "osal.h"
#include "diagcomm_dci.h"
#include "diag_fwd_v.h"    
#include "stringl/stringl.h"
#include "eventi.h" /* for event_ctrl_stale_timer */
#include "diag_cfg.h" /*for DIAG_TX_SLEEP_THRESHOLD_NRT*/
#include "diagcomm_io_sio.h"
#include "diagcomm_cmd.h"



#if defined (FEATURE_DIAG_STUB)
#include"diagstub.h"
#else
#include "assert.h"
#endif

#include "diagdsmi.h" /* for def. of DSMI_DIAG_SMD_TX_ITEM_FEW_MARK etc */
#include <stdio.h>
/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

diagcomm_io_conn_type diagcomm_io_dci_conn[NUM_SMD_PORTS];
diagcomm_io_conn_type diagcomm_io_dci_cmd_conn[NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern volatile diag_tx_mode_type diag_tx_mode;                    /* From diagcomm_smd.c */
boolean send_dci_conn_status;  /*indicate that we failed to send dci conn status because of flow control
                                  and need to send it again when we have available dsm items*/

static uint8 diag_req_dci_inbound[DIAG_MAX_RX_PKT_SIZ];
static q_type diagcomm_smd_dci_tx_q[NUM_SMD_PORTS];
static q_type diagcomm_smd_dci_rx_q[NUM_SMD_PORTS];
static dsm_item_type *diagcomm_sio_inbound_fusion_item_ptr = NULL;
static boolean
diag_get_rx_pkt_fusion (uint8 * dest_ptr,  /* Buffer for received packet. */
         unsigned int size, /* Maximum size for request packet. */
         unsigned int *pkt_len  /* Packet length when it is constructed. */
  );
static void diagcomm_smd_dci_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                               dsm_mem_level_enum_type mem_level,
                                               dsm_mem_op_enum_type mem_op);
static void diagcomm_smd_dci_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op);
extern osal_timer_t diag_sio_tout_timer;

/*Watermark queues used for dci communications on TN apps side*/
dsm_watermark_type diagcomm_smd_dci_rx_wmq[NUM_SMD_PORTS];
dsm_watermark_type diagcomm_smd_dci_tx_wmq[NUM_SMD_PORTS];

extern dsm_watermark_type diagcomm_sio_rx_wmq[NUM_SIO_PORTS];  /* From diagcomm_sio.c */
extern osal_tcb_t diag_fwd_task_tcb;
extern   diagpkt_slave_table_type * diagpkt_slave_table[NUM_SMD_PORTS][DIAGPKT_MSTR_TBL_SIZE];
boolean dci_AllowFlow = TRUE; /* Keep track of state of flow control. */
void diag_rx_dci_notify(void);
extern boolean diagcomm_smd_flow_enabled(void);

char diag_data_pkt_inbound_dci[DIAG_READ_BUF_SIZE*2];

static dsm_item_type *diagcomm_sio_outbound_item_ptr_data_dci = NULL;


/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
/*===========================================================================
FUNCTION DIAG_RX_DCI_NOTIFY

DESCRIPTION
  This callback routine is called when DCI data has been received from APQ
  side for Fusion DCI
  
===========================================================================*/
void
diag_rx_dci_notify (void)
{
  osal_sigs_t return_sigs; 
  int return_val = 0;
  return_val =osal_set_sigs(&diag_task_tcb, DIAG_RX_CTRL_DCI_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}

/*===========================================================================

FUNCTION DIAG_PROCESS_RX_FUSION_DCI

DESCRIPTION
  This routine processes the packet received from APX to apps on the 
  HSIC port for fusion.      
         
  Packet Structure       
         
Field Start version Length cmd_code  pkt_ID  Command_Packet         7E       
         
Bytes   1     1       2       1         4   (Length - (1+2+4+1))     1 
  
===========================================================================*/
void
diag_process_rx_fusion_dci (void)
{
    unsigned int pkt_len = 0;
    /* Disable diag sleep so packet can be processed w/out interruption */
    diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);
   if (diag_get_rx_pkt_fusion ((uint8 *) & diag_req_dci_inbound,
                       sizeof (diag_req_dci_inbound), (unsigned int *)&pkt_len) == TRUE)
  {
    /* If we RX good packets, we must be connected. */
    DIAG_SET_MASK (diag_cx_state, DIAG_CX_DCI_S);

    diagpkt_process_request_fusion_dci (&diag_req_dci_inbound, pkt_len);

  }
    /* Go back to sleep after processing Rx packet */
    diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
}

/*===========================================================================

FUNCTION DIAG_DCI_DTR_CHANGED_CB

DESCRIPTION
  Invoked when DTR changes. This is intended to flip a variable that causes
  Diag to vote/not vote for sleep based on connectivity status.

PARAMETERS
  cb_data - Diag I/O connection handle
  
RETURN VALUE
  None
  
  
===========================================================================*/

void diagcomm_dci_dtr_changed_cb(void* param)
{
  diagcomm_io_conn_type *conn = (diagcomm_io_conn_type *)param;
  if (conn)
  {
    sio_ioctl_param_type param;
    boolean dte_ready_asserted = FALSE;

    param.dte_ready_asserted = &dte_ready_asserted;
    if ( conn->channel_type == DIAGCOMM_IO_DCI ) 
    {
     // Get DTR using sio_ioctl
    sio_ioctl ( conn->handle.sio_handle, SIO_IOCTL_DTE_READY_ASSERTED, &param );
    }
    diagcomm_smd_dci_conn_changed( conn, dte_ready_asserted );
  }
}


/*===========================================================================

FUNCTION DIAG_DCI_DTR_ENABLE

DESCRIPTION
  Invoked when Diag initializes. This registers a callback with SIO that is
  intended to report when there are DTR changes of dci channel between TN Apps
  and peripherals

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_dci_dtr_enable( diagcomm_io_conn_type * conn )
{
  sio_ioctl_param_type ioctl_param;
  if (conn == NULL) {
     return;
  }
  // Set up call back for smd dtr event for MPSS and LPASS
  ioctl_param.enable_dte_ready_event_ext.cb_func = diagcomm_dci_dtr_changed_cb;

  //Set up call back data - since we are interested in knowing SIO stread id when
  //call back comes we are passing port info to SIO for MPSS
  ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)conn;

  sio_ioctl(conn->handle.sio_handle,
		  SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
		  (void*)&ioctl_param);

  diagcomm_dci_dtr_changed_cb(conn);
}


/*===========================================================================

FUNCTION DIAGCOMM_SIO_FUSION_DCI_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback registered with SIO for receiving Fusion DCI data from APQ.

  This function enqueues the data onto the diag's watermark.  
 
PARAMETERS
  item_ptr - A pointer point to the dsm item receive on TN apps side from APQ
  
RETURN VALUE
  None

===========================================================================*/
void
diagcomm_sio_fusion_dci_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  dsm_enqueue (&diagcomm_sio_rx_wmq[DIAGCOMM_PORT_FUSION_DCI], item_ptr);

  /* Notify diag that there is inbound data available to be read */
   diag_rx_dci_notify();
} /* diagcomm_sio_fusion_dci_inbound_pkt_notify */

/*===========================================================================

FUNCTION DIAG_FWD_NOTIFY_DCI

DESCRIPTION
Set signal when TN apps received packets from modem through DCI channel 
 
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_fwd_notify_dci( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
    
  /* Set signal to diag task to process slave traffic */
  return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DCI_SLAVE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

} /* diag_fwd_notify_dci */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_DCI_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SMD calls when data is received from modem DCI channel.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.
 
PARAMETERS
  item_ptr - The dsm item received on TN apps side from modem
  
RETURN VALUE
  None

===========================================================================*/
void
diagcomm_smd_dci_inbound_pkt_notify (dsm_item_type ** item_ptr)
{

  dsm_enqueue (diagcomm_io_dci_conn[DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr);
  diag_fwd_notify_dci();


} /* diagcomm_smd_dci_inbound_pkt_notify */


/*===========================================================================

FUNCTION DIAGCOMM_DCI_OPEN

DESCRIPTION
  Opens the DCI port on TN apps side.
 
PARAMETERS
  port_num - The port number for DCI channel
             DIAGCOMM_PORT_1 stands for modem
             DIAGCOMM_PORT_2 stands for ADSP
  
RETURN VALUE
  None
===========================================================================*/
boolean
diagcomm_dci_open (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = NULL;

  if (port_num >= NUM_SMD_PORTS || port_num < DIAGCOMM_PORT_1) 
  return FALSE; 


  conn = &diagcomm_io_dci_conn[port_num];
  if( conn->connected )
  {
    return conn->connected;
  }

  diagcomm_io_set_port(conn);

  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
      diag_io_detect_connection(conn);
    }
    else
    {
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_dci_open - Couldn't get stream.");

    }
  }
  return conn->connected;

} /* diagcomm_dci_open */

/*===========================================================================

FUNCTION DIAGCOMM_DCI_CMD_OPEN

DESCRIPTION
  Opens the DCI port on TN apps side.
 
PARAMETERS
  port_num - The port number for DCI channel
             DIAGCOMM_PORT_1 stands for modem
             DIAGCOMM_PORT_2 stands for ADSP
  
RETURN VALUE
  None
===========================================================================*/
boolean
diagcomm_dci_cmd_open (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = NULL;

  if (port_num >= NUM_SMD_PORTS || port_num < DIAGCOMM_PORT_1) 
  return FALSE; 


  conn = &diagcomm_io_dci_cmd_conn[port_num];
  if( conn->connected )
  {
    return conn->connected;
  }

  diagcomm_io_set_port(conn);

  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
      diag_io_detect_connection(conn);
    }
    else
    {
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_dci_cmd_open - Couldn't get stream.");

    }
  }
  return conn->connected;

} /* diagcomm_dci_cmd_open */


/*===========================================================================

FUNCTION DIAGCOMM_DCI_CLOSE

DESCRIPTION
  Closes the current DCI connection, between TN apps and peripherals.
 
PARAMETERS
  port_num - The port number for DCI channel
             DIAGCOMM_PORT_1 stands for modem
             DIAGCOMM_PORT_2 stands for ADSP
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_dci_close (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = NULL;

  if (port_num >= NUM_SMD_PORTS || port_num < DIAGCOMM_PORT_1) 
   return; 

  conn = &diagcomm_io_dci_conn[port_num];
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
    diag_block ();
  }

  /* success or not, consider close operation successful  */
  conn->connected = FALSE;

} /* diagcomm_dci_close */

/*===========================================================================

FUNCTION DIAGCOMM_SMD_DCI_CONN_CHANGED

DESCRIPTION
   This function will send the connection status of the DCI channels between TN
   apps and peripherals to APQ when the connection status is changed
 
PARAMETERS
  conn - The channel that has changes in connection status
  connected -  The connection status of the channel
  
RETURN VALUE
  None
===========================================================================*/
void diagcomm_smd_dci_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
#if defined(DIAG_MP_MASTER)
  uint8 preset_index;

  if( connected )
  {
    conn->channel_connected = TRUE;
    DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_F3);
    DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_LOGS);
    DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_EVENTS);
      
    diag_set_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );
  }
  else
  {
    conn->channel_connected = FALSE;
    
   for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
    {
      conn->mask_preset_update[preset_index] = DIAG_UPDATE_MASK_NONE;
    }
    conn->mask_dci_update[DIAG_DCI_MASK_1-1] = DIAG_UPDATE_MASK_NONE;
  }

  
#endif

  if( connected )
  {
    conn->channel_connected = TRUE;  
  }
  else
  {
    conn->channel_connected = FALSE;
  }

   /*Setting signal to send connection status change to Fusion DCI on APQ*/
   (void)diag_set_internal_sigs( DIAG_INT_CONN_STATUS_SEND_SIG );

} /* diagcomm_smd_dci_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_GET_DCI_RX_WMQ_CNT

DESCRIPTION
  This function returns the DCI RX watermark queue count.
 
PARAMETERS
  port_num - Port number for the channel
  
RETURN VALUE
  RX watermark queue count
===========================================================================*/
uint32 diagcomm_smd_get_dci_rx_wmq_cnt(diagcomm_enum_port_type port_num)
{
  if (port_num >= NUM_SMD_PORTS || port_num < DIAGCOMM_PORT_1) 
     return 0;
   
   return diagcomm_io_get_wmq_cnt( &diagcomm_smd_dci_rx_wmq[port_num] );
}
/* diagcomm_smd_get_rx_wmq_cnt */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_DCI_INIT

DESCRIPTION
  This function initializes the dci communication.
 
PARAMETERS
  port_num - Port number for the channel
  
RETURN VALUE
  None
===========================================================================*/
void
diagcomm_smd_dci_init (diagcomm_enum_port_type port_num)
{

  diagcomm_io_conn_type * conn = NULL;

  if (port_num >= NUM_SMD_PORTS || port_num < DIAGCOMM_PORT_1) 
  return; 

  conn = &diagcomm_io_dci_conn[port_num];

  /* Make sure we are closed */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
    diag_block ();
  }

  /* Initialize watermark structures */
  memset (&diagcomm_smd_dci_rx_wmq[conn->port_num], 0, sizeof (dsm_watermark_type));


#if  defined (DIAG_MP_MASTER)
  if (DIAGCOMM_PORT_1 == conn->port_num)
  {
     /* Adding flow control at pool's item-level granularity for streaming mode.
     Lower flow control levels prevents Diag from unable to catch up with draining 
     data if SIO outbound is slow. */
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SMD_DCI_TX_ITEM_POOL,                                /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_MANY,                                       /* dsm_mem_level_enum_type level */
    DSMI_DIAG_SMD_DCI_TX_ITEM_MANY_MARK /* uint32 avail_item_count */
  );
  
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SMD_DCI_TX_ITEM_POOL,                               /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_FEW,                                       /* dsm_mem_level_enum_type level */
    DSMI_DIAG_SMD_DCI_TX_ITEM_FEW_MARK /* uint32 avail_item_count */
  );

    dsm_reg_mem_event_cb
  (
   DSM_DIAG_SMD_DCI_TX_ITEM_POOL,                                 /* dsm_mempool_id_type      pool_id*/
   DSM_MEM_LEVEL_FEW,                                        /* dsm_mem_level_enum_type  level */
   DSM_MEM_OP_NEW,                                            /* dsm_mem_op_enum_type     mem_op */
   diagcomm_smd_dci_dsm_high_mem_event_cb                         /* pointer to call-back function */
  );
  
  dsm_reg_mem_event_cb
  (
   DSM_DIAG_SMD_DCI_TX_ITEM_POOL,                                 /* dsm_mempool_id_type      pool_id*/
   DSM_MEM_LEVEL_MANY,                                         /* dsm_mem_level_enum_type  level */
   DSM_MEM_OP_FREE,                                           /* dsm_mem_op_enum_type     mem_op */
   diagcomm_smd_dci_dsm_low_mem_event_cb                          /* pointer to call-back function */
  );

  /* Set up Queue for receiving data from SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_dci_rx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_SMD_DCI_TX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_dci_rx_q[conn->port_num]                        /* Queue to hold the items */
  );

  conn->rx_wmq_ptr = &diagcomm_smd_dci_rx_wmq[conn->port_num];

  /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_smd_dci_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_SMD_DCI_TX_ITEM_SIZ;
 
    dsm_set_low_wm(&diagcomm_smd_dci_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_SMD_DCI_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_dci_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_SMD_DCI_TX_ITEM_SIZ));


  /* Set up Queue for sending data via SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_dci_tx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,          /* Do not exceed count */
    &diagcomm_smd_dci_tx_q[conn->port_num]                        /* Queue to hold the items */
  );

  /* Set up watermarks for WaterMark Queue. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm(&diagcomm_smd_dci_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_dci_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
  }

#endif

} /* diagcomm_smd_dci_init */


/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST_FUSION_DCI

DESCRIPTION
The function process the ctrl packet received from APQ side through HSIC port
 
PARAMETERS
  req_pkt - The packet received from APQ to TN apps
  pkt_len - The length of the received packet
  
RETURN VALUE
  None
===========================================================================*/
#if defined(DIAG_MP_MASTER)
void 
diagpkt_process_request_fusion_dci (void *req_pkt, uint16 pkt_len)
{
  int h, i, j;
  uint16 packet_id;     /* Command code for std or subsystem */
  diag_dci_pkt_type *rx_dci = NULL;
  diag_ctrl_msg_type * rev_ctrl_pkt = NULL;
  boolean skip_slave_table = FALSE;
  boolean found_slave = FALSE;
  const diagpkt_slave_table_type *slave_tbl_entry = NULL;
  uint8 subsys_id = DIAGPKT_NO_SUBSYS_ID;
  uint16 cmd_code = 0xFF;
  const diagpkt_user_table_entry_type *tbl_entry = NULL;
  int tbl_entry_count = 0;
  boolean is_sent = FALSE;
  uint8 equip_id = 0;
  boolean * mask_status_ptr = NULL;

  /*the length of the packet should at least be 5:(start(byte),version(byte), length(uint32),cmd_code(byte))*/
  if (req_pkt == NULL || pkt_len < 5) {
     return;
  }

      /* If we RX good packets, we must be connected. */
      DIAG_SET_MASK (diag_cx_state, DIAG_CX_DCI_S);

      rx_dci = (diag_dci_pkt_type *)(req_pkt);
  
      /* Check if the packet is well-formatted */
      if ((rx_dci->start) != DIAG_ASYNC_CONTROL_CHAR) {
         return;
      }

      /* Check the version */
     if((rx_dci->version) != DIAG_CONSUMER_API_VER_1){
        return;
     }

     if ((rx_dci->dci_cmd_code) == DIAG_DCI_CONTROL_PACKET)
     {
        rev_ctrl_pkt = (diag_ctrl_msg_type *)(&(rx_dci->type));
        /* Length Sanity Check. Length returned by diag_get_rx_pkt_slave is the entire     
         length from 7E to 7E (see description). Length field embedded in the packet  
         is from Cmd_code to Command_Packet. */

     switch( rev_ctrl_pkt->cmd_type )
     {   
         case DIAG_CTRL_DCI_HANDSHAKE:
         {
           if (((diag_dci_ctrl_pkt_type *)(&(rx_dci->type)))->data.dci_handshake_ctrl_pkt.version != DIAG_CONSUMER_API_VER_1)
           {
             return;
           }
           diag_send_handshake_fusion_dci(req_pkt,pkt_len);
           /*MDM will send whatever APQ sent to it back and APQ will check the magic number on its side*/
           if ( ((diag_dci_ctrl_pkt_type *)(&(rx_dci->type)))->data.dci_handshake_ctrl_pkt.magic_num == 0xAABB1122)
	   {
		     /*we set channel_connected here because we received handshake indicating APQ Fusion DCI port is opened*/
		     diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].channel_connected = TRUE;
		     (void)diag_set_internal_sigs( DIAG_INT_CONN_STATUS_SEND_SIG );
	   }
           break;
         }
         case DIAG_CTRL_MSG_EQUIP_LOG_MASK: /*Set log mask control packet:9*/
         {
            /*Update log mask of the master*/
            diag_ctrl_update_log_mask( rev_ctrl_pkt );

            /*Send the log mask from master to slave*/

            mask_status_ptr = diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].log_mask_dci_update[DIAG_DCI_MASK_1-1];

            if (diagcomm_ctrl_send_mult_pkt( (PACK(void *))rev_ctrl_pkt,
                                            rx_dci->dci_pkt_length-sizeof(rx_dci->dci_cmd_code), 
                                            NULL, 0,
                                            DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1, TRUE, &is_sent )>0) 
            {
                  for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
                     {
                        mask_status_ptr[equip_id] = FALSE;
                  }
            }
           else
           {
              MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "FAILED FORWARDING LOG MASK UPDATE CONTROL PACKET TO MODEM");
           }
           break;
         }
         case DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3:
         {
           /*Update log mask of the master*/
            diag_ctrl_update_log_preset_mask( rev_ctrl_pkt );

            /*Send the log mask from master to slave*/
            diagcomm_ctrl_send_mult_pkt( (PACK(void *))rev_ctrl_pkt,
                                         rx_dci->dci_pkt_length-1, 
                                         NULL, 0,
                                         DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1, 1, &is_sent);

           break;
         }
         case DIAG_CTRL_MSG_EVENT_MASK_V2:/*Set event mask control packet 15*/
         {  
             /*update the event mask on master side*/
             diag_ctrl_update_event_mask(rev_ctrl_pkt);

             /*Send the event mask from master to slave*/
             diagcomm_ctrl_send_mult_pkt( (PACK(void *))rev_ctrl_pkt,
                                         rx_dci->dci_pkt_length-1, 
                                         NULL, 0,
                                         DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1, 1, &is_sent);

             break;
         }
         case DIAG_CTRL_MSG_EVENT_MASK_V3:/*Set event mask control packet 15*/
         {  
              /*update the event mask on master side*/
              diag_ctrl_update_event_preset_mask(rev_ctrl_pkt);

              /*Send the event mask from master to slave*/
             diagcomm_ctrl_send_mult_pkt( (PACK(void *))rev_ctrl_pkt,
                                         rx_dci->dci_pkt_length-1, 
                                         NULL, 0,
                                         DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1, 1, &is_sent);

              break;
         }
         case DIAG_CTRL_MSG_DIAGMODE:  //3
         {
             /*Update the variables on TN side and propagate the NRT control packet to slave*/
           diagpkt_process_ctrl_msg( (void *)rev_ctrl_pkt, (uint32)(rx_dci->dci_pkt_length-1),
                          DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1 );
            break;
         }
     }
    }
   else if ((rx_dci->dci_cmd_code) == DIAG_DCI_CMD_REQ) 
   {
      /* Length Sanity Check. Length returned by diag_get_rx_pkt_slave is the entire     
         length from 7E to 7E (see description). Length field embedded in the packet  
         is from Cmd_code to Command_Packet. */
      if((rx_dci->dci_pkt_length) != (pkt_len-sizeof(rx_dci->start)-sizeof(rx_dci->version)-sizeof(rx_dci->dci_pkt_length)-sizeof(rx_dci->dci_cmd_code))){
         return;
      }
      if (diagcomm_io_dci_conn[DIAGCOMM_PORT_1].connected != 1) {
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Received CMD_REQ FROM APQ, BUT DCI CHENNEL IS NOT OPENED");
         return;
      }
         skip_slave_table = FALSE;
         cmd_code = diagpkt_get_cmd_code((void *)&(rx_dci->type.diag_dci_cmd_req_pkt.command[0]));  
         packet_id = diagpkt_subsys_get_cmd_code((void *)&(rx_dci->type.diag_dci_cmd_req_pkt.command[0]));
         subsys_id = diagpkt_subsys_get_id ((void *)&(rx_dci->type.diag_dci_cmd_req_pkt.command[0]));
         
  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */

       for ( h = 0; !found_slave && h < NUM_SMD_PORTS; h++)
      {
      // Search slave table for cmd registration
      if (diagcomm_io_dci_conn[h].connected == 1) 
      {
      
         for( i = 0; !found_slave && !skip_slave_table && i < DIAGPKT_MSTR_TBL_SIZE; i++ )
         {
           slave_tbl_entry = diagpkt_slave_table[h][i];

           if( (slave_tbl_entry != NULL) && (slave_tbl_entry->mstr_data.subsysid == subsys_id)
               && ((slave_tbl_entry->mstr_data.cmd_code == cmd_code)||(slave_tbl_entry->mstr_data.cmd_code == 255) ))
           {
             tbl_entry = slave_tbl_entry->mstr_data.user_table;
             tbl_entry_count = (tbl_entry) ? slave_tbl_entry->mstr_data.count : 0;
             
             for( j = 0; (tbl_entry != NULL)  && (j < tbl_entry_count); j++ )
             {
               if( (packet_id >= tbl_entry->cmd_code_lo) && 
                   (packet_id <= tbl_entry->cmd_code_hi) )
               {
                 /* If the entry has no func, ignore it. */
                 if( tbl_entry->func_ptr )
                 {
                   found_slave = TRUE;
                 }
               }
               tbl_entry++;
             } /* end 'j' for-loop */
           }
         } /* end 'i' for-loop */
       }
      }/* end of 'h' for-loop */
      if( found_slave == TRUE )
      {

        // Pkt ID was found; Send to slave proc which registered the CMD
        diagcomm_send_cmd( req_pkt, pkt_len, slave_tbl_entry->channel, TRUE );
      }
      else
      {
         /*We are not sending any error back to app side when packet not found*/
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "CAN'T FIND PKT IS IN SLAVE TABLE");
      }
   } 
}
/*===========================================================================

FUNCTION DIAG_SEND_HANDSHAKE_FUSION_DCI

DESCRIPTION
   Send the handshake ctrl pkt through fusion dci port from TN apps to APQ
 
PARAMETERS
  rev_pkt - The data received from APQ side
  pkt_len - The length of the packet received from APQ
  
RETURN VALUE
  None
===========================================================================*/
void diag_send_handshake_fusion_dci(void * rev_pkt, uint16 pkt_len)
{  
  diag_dci_pkt_type * rsp_pkt = NULL;
  diagcomm_io_tx_params_type tx_params;
  uint8 * cur_data_ptr = NULL;
  dsm_item_type * dsm_item_ptr_sav = NULL;

  if (rev_pkt == NULL) 
  {
    return;
  }
  if(pkt_len != (FPOS(diag_dci_pkt_type, type)+FPOS(diag_dci_ctrl_pkt_type, data)+sizeof(diag_fusion_dci_handshake_ctrl_pkt)))
  {
    return;
  }
  if( diagcomm_sio_tx_flow_enabled() == FALSE )
  {
    /*Drop the packet if flow control is enabled, APQ will keep sending handshake 
      packets for five times and close the channel if it gets no response*/
    return;
  }
  rsp_pkt = (diag_dci_pkt_type *)rev_pkt;

  if (dsm_item_ptr_sav == NULL) 
    dsm_item_ptr_sav = diagcomm_new_dsm( DSM_DIAG_SIO_TX_ITEM_POOL , FALSE ); 
      
  if( dsm_item_ptr_sav )
  {
    /* Append ctrl_pkt to DSM */
    cur_data_ptr = dsm_item_ptr_sav->data_ptr + dsm_item_ptr_sav->used;
    /*length of the dci ctrl pkt =size of diag_fusion_dci_handshake_ctrl_pkt(from version to end) 
     + DCI_CMD_CODE(uint8) + CTRL_PKT_ID(uint32) + CTRL_PKT_DATA_LEN(uint32)+DCI_PACKET_LENGTH(uint16) +DCI_VERSION(uint8) +START(uint8)*/
    memscpy( cur_data_ptr, (dsm_item_ptr_sav->size - dsm_item_ptr_sav->used), (void *)rsp_pkt,  pkt_len);
    dsm_item_ptr_sav->used += pkt_len;
    tx_params.dsm_ptr = dsm_item_ptr_sav;
    diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI], &tx_params ); 
    dsm_item_ptr_sav = NULL;  
  }
  else
  {
    /*Quit and wait for the retry from APQ*/
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Out of dsm item, not able to send handshake ACK back to APQ");
    return;
  }
}

/*===========================================================================

FUNCTION DIAG_SEND_CONN_STATUS_FUSION_DCI

DESCRIPTION
   Send the connection status of DCI channel connected to peripherals to APQ 
   through HSIC port
 
PARAMETERS
  connected - Connection status of the DCI channel 
  
RETURN VALUE
  None
===========================================================================*/
void diag_send_conn_status_fusion_dci(boolean connected)
{  
  diag_dci_pkt_type dci_pkt;
  diagcomm_io_tx_params_type tx_params;
  uint8 * cur_data_ptr = NULL;
  dsm_item_type * dsm_item_ptr_sav = NULL;
  diag_fusion_dci_conn_status_ctrl_pkt conn_status_pkt;

  dci_pkt.start = DIAG_ASYNC_CONTROL_CHAR;
  dci_pkt.version = DIAG_CONSUMER_API_VER_1;
  /*dci_pkt_length = size of diag_fusion_dci_conn_status_ctrl_pkt(from version to end) - end(uint8) + DCI_CMD_CODE(uint8) + CTRL_PKT_ID(uint32) + CTRL_PKT_DATA_LEN(uint32)*/
  dci_pkt.dci_pkt_length = sizeof(diag_fusion_dci_conn_status_ctrl_pkt)+sizeof(uint32)+sizeof(uint32);
  dci_pkt.dci_cmd_code = DIAG_DCI_CONTROL_PACKET;
  dci_pkt.type.diag_dci_ctrl_pkt.ctrl_pkt_id = DIAG_CTRL_MSG_DCI_CONN_STATUS;
  dci_pkt.type.diag_dci_ctrl_pkt.ctrl_pkt_data_len = sizeof(diag_fusion_dci_conn_status_ctrl_pkt);

  conn_status_pkt.version = DIAG_CONSUMER_API_VER_1;
  conn_status_pkt.pair_count = 1;         /*Currently we only have connection with modem*/
  conn_status_pkt.peripheral_id = DIAG_MODEM_PROC;  //Peripheral ID         
  conn_status_pkt.status = connected;             /*0 DCI channel down; 1 DCI channel up*/
  conn_status_pkt.end = DIAG_ASYNC_CONTROL_CHAR; 
  memscpy( (void*)&dci_pkt.type.diag_dci_ctrl_pkt.data, sizeof(diag_fusion_dci_conn_status_ctrl_pkt),
          &conn_status_pkt, 
          sizeof(diag_fusion_dci_conn_status_ctrl_pkt) );

  if( diagcomm_sio_tx_flow_enabled() == FALSE )
  {
   /*Set it to true to indicate that we need to send dci conn status when we have available dsm item*/
   send_dci_conn_status = TRUE;
   return;
  }

  if (dsm_item_ptr_sav == NULL) 
  dsm_item_ptr_sav = diagcomm_new_dsm( DSM_DIAG_SIO_TX_ITEM_POOL , FALSE ); 
      
  if( dsm_item_ptr_sav )
  {
     send_dci_conn_status = FALSE;
             /* Append ctrl_pkt to DSM */
     cur_data_ptr = dsm_item_ptr_sav->data_ptr + dsm_item_ptr_sav->used;
     /*length of the dci ctrl pkt =size of diag_fusion_dci_conn_status_ctrl_pkt(from version to end) 
       + DCI_CMD_CODE(uint8) + CTRL_PKT_ID(uint32) + CTRL_PKT_DATA_LEN(uint32)+DCI_PACKET_LENGTH(uint16) +DCI_VERSION(uint8) +START(uint8)*/
     memscpy( cur_data_ptr, (dsm_item_ptr_sav->size - dsm_item_ptr_sav->used), (void *)&dci_pkt,  
              (sizeof(diag_fusion_dci_conn_status_ctrl_pkt)+sizeof(uint32)+sizeof(uint32)+sizeof(byte)
               +sizeof(byte)+sizeof(uint16)+sizeof(byte)) );
     dsm_item_ptr_sav->used += (uint16)(sizeof(diag_fusion_dci_conn_status_ctrl_pkt)+sizeof(uint32)+sizeof(uint32)+sizeof(byte)
               +sizeof(byte)+sizeof(uint16)+sizeof(byte));
     tx_params.dsm_ptr = dsm_item_ptr_sav;
     if(diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].channel_connected == TRUE && 
        diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].connected == TRUE) 
     {
       diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI], &tx_params ); 
     }
     dsm_item_ptr_sav = NULL;  
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Out of dsm item, will try send connection status again");
    return;
  }
}
#endif /* DIAG_MP_MASTER*/


/*===========================================================================

FUNCTION DIAG_GET_RX_PKT_FUSION

DESCRIPTION
  This function will retrieve the next DM packet sent by APQ to TN apps.
  The incoming packets can be a chained. This function calls dsm_pullup_tail
  and extracts the whole packet.

DEPENDENCIES
  This procedure can only be called when a full DM packet is available on
  queue from SIO.  Further, if multiple packets are enqueued, then this will
  only process the first packet.  The remaining packets are left on the queue.

RETURN VALUE
  Returns TRUE if full packet recd, else returns FALSE.

===========================================================================*/
static boolean
diag_get_rx_pkt_fusion (uint8 * dest_ptr,  /* Buffer for received packet. */
         unsigned int size, /* Maximum size for request packet. */
         unsigned int *pkt_len  /* Packet length when it is constructed. */
  )
{
  unsigned int used;
  unsigned int total_req_bytes;
  dsm_item_type *item_ptr = NULL;
  /* TRUE if found a packet boundary in HDLC stream. */
  boolean pkt_bnd = FALSE;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  item_ptr = (dsm_item_type *) diagcomm_inbound_fusion (&used);
  
  if( item_ptr != NULL)
  {
    total_req_bytes = dsm_length_packet(item_ptr);
    if(total_req_bytes > size)
    {
     dsm_free_packet(&item_ptr);
     return FALSE;
    }
    *pkt_len = dsm_pullup_tail(&item_ptr,dest_ptr,total_req_bytes);
    item_ptr = NULL;
    pkt_bnd = TRUE;
  }

  if(diagcomm_get_rx_wmq_cnt(DIAGCOMM_PORT_FUSION_DCI) > 0)
  {
    diag_rx_dci_notify ();
  }

  return pkt_bnd;
}    /* diag_get_rx_pkt_fusion */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_DCI_CLOSE_DONE_CB

DESCRIPTION
  This function is the callback function when the dci channel closed
===========================================================================*/
void diagcomm_smd_dci_close_done_cb(void)
{
 (void)diag_set_internal_sigs( DIAG_INT_CONN_STATUS_SEND_SIG );
  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diagcomm_smd_dci_close_done_cb:close dci channel", 0);
  diag_release (); 
} /* diagcomm_smd_dci_close_done_cb */

/*===========================================================================

FUNCTION DIAGCOMM_SMD_FUSION_PROCESS_SLAVE_TX

DESCRIPTION
  This function is used to forward the dci packet received from peripherals to APQ
 
PARAMETERS 
port_num Indicates the port number from which the DCI data has to be sent/dequeued 
===========================================================================*/

void diagcomm_smd_fusion_process_slave_tx(diagcomm_enum_port_type port_num)
{
  static dsm_item_type *diagcomm_app_item_ptr = NULL;
  diagcomm_io_tx_params_type tx_params;
  int return_val = 0;
  osal_sigs_t return_sigs;

    if (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].connected &&
	diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].channel_connected )
   {
         diagcomm_app_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_dci_conn[port_num] );
         tx_params.dsm_ptr = diagcomm_app_item_ptr;
         diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI], &tx_params );
         if( diagcomm_smd_get_dci_rx_wmq_cnt(DIAGCOMM_PORT_1) )
         {    
            /* Set signal to diag task to process slave traffic */
            return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DCI_SLAVE_SIG, &return_sigs);
            ASSERT(OSAL_SUCCESS == return_val);
         }

   }
}


/*===========================================================================
FUNCTION diagcomm_smd_dci_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count 
   for DSM_DIAG_SMD_DCI_TX_ITEM_POOL is reached. (DSM_DIAG_SMD_DCI_TX_ITEM_POOL is 
   used by DCI channel, to receive data.)
   
   This callback is registered with lower watermark level for streaming mode,
   so we will only disable flow when this callback is triggered while in 
   streaming mode.
   
===========================================================================*/
static void diagcomm_smd_dci_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op)
{
   /* Check to be defensive */
   if(pool_id != DSM_DIAG_SMD_DCI_TX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_NEW)
      return;

   if( diag_tx_mode.mode != DIAG_TX_MODE_STREAMING )
     return;
   
   if (TRUE == dci_AllowFlow)
   {
      dci_AllowFlow = FALSE;
      /*increment the # of times that flow control is triggered for the dci pool*/
      diagcomm_incr_flow_ctrl_count((diagcomm_port_type)2);
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dci_dsm_high_mem_event_cb: disabled flow");
   }

} /* diagcomm_smd_dci_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_smd_dci_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count 
   for DSM_DIAG_SMD_DCI_TX_ITEM_POOL is reached. (DSM_DIAG_SMD_DCI_TX_ITEM_POOL 
   is used by DCI channel, to receive data.)
   
   This callback is registered with lower watermark level for streaming mode,
   so we will only enable flow when this callback is triggered while in 
   streaming mode.
   
===========================================================================*/
static void diagcomm_smd_dci_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                               dsm_mem_level_enum_type mem_level,
                                               dsm_mem_op_enum_type mem_op)
{
#if defined(DIAG_BUFFERING_SUPPORT)	
   osal_sigs_t return_sigs;
   int return_val = 0;
#endif
  
   /* check to be defensive */
   if(pool_id != DSM_DIAG_SMD_DCI_TX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_FREE)
      return;
   
   /* Allow this to re-enable flow in all modes, in case we miss the 
      re-enable wm in buffering mode and we've switched back to 
      streaming mode. */
     
   if (FALSE == dci_AllowFlow)
   {
      dci_AllowFlow = TRUE;
	  
#if defined(DIAG_BUFFERING_SUPPORT)		  
	  diag_tx_notify();
      return_val = osal_set_sigs(&diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
#endif
	  
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dci_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_smd_dci_dsm_low_mem_event_cb */


/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_FUSION

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound
  data is available, NULL is returned.

===========================================================================*/
void *
diagcomm_inbound_fusion (unsigned int *stream_len)
{
  void *return_ptr = NULL;

  ASSERT (stream_len);
  if (diagcomm_sio_inbound_fusion_item_ptr != NULL)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to get more than one inbound buffer.");

    *stream_len = 0;
  }
  else
  {
    diagcomm_sio_inbound_fusion_item_ptr = diagcomm_io_dequeue_rx_wmq( diagcomm_get_sio_conn(DIAGCOMM_PORT_FUSION_DCI) );

    if (diagcomm_sio_inbound_fusion_item_ptr != NULL)
    {
      ASSERT (diagcomm_sio_inbound_fusion_item_ptr->data_ptr != NULL);

      *stream_len = diagcomm_sio_inbound_fusion_item_ptr->used;

      return_ptr = diagcomm_sio_inbound_fusion_item_ptr;
      diagcomm_sio_inbound_fusion_item_ptr = NULL;
 
    }
  }

  return return_ptr;
  
} /* diagcomm_inbound */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_DCI_SET_PORT

DESCRIPTION
  This function set the port id for dci channel

===========================================================================*/
void diagcomm_smd_dci_set_port(diagcomm_io_conn_type * conn)
{
   if (conn->port_num == DIAGCOMM_PORT_1) 
   {
     conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG2;
   }
}

/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_DATA_DCI

DESCRIPTION
  This retrieves the incoming data over DCI channel.

===========================================================================*/

void diagcomm_process_data_dci( void )
{
 if(diagcomm_smd_flow_enabled())
 {
  diagcomm_process_inbound_data_dci(DIAGCOMM_PORT_1);
 }
}
/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_INBOUND_DATA_DCI

DESCRIPTION
  This retrieves the incoming data over DCI channel and formulate a DSM
  item and enqueue it into the watermark queue.

===========================================================================*/

void diagcomm_process_inbound_data_dci( diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = &diagcomm_io_dci_conn[port_num];
  uint32 num_used = 0;
  dsm_item_type **dsm_item_ptr= NULL;

  osal_lock_mutex(&conn->buf_mutex);
  num_used = conn->used;
  memscpy(&diag_data_pkt_inbound_dci[0], sizeof(diag_data_pkt_inbound_dci), conn->buf,conn->used);
  conn->used = 0;
  osal_unlock_mutex(&conn->buf_mutex);

  if((DIAGCOMM_PORT_SMD == conn->port_type) &&  (DIAGCOMM_IO_DCI == conn->channel_type))
  {
	dsm_item_ptr = diagcomm_sio_data_to_dsm_dci((uint8 *)&(diag_data_pkt_inbound_dci[0]), num_used);
	diagcomm_enqueue_dsm_dci(dsm_item_ptr);
  }
}

/*===========================================================================

FUNCTION DIAGCOMM_ENQUEUE_DSM_DCI

DESCRIPTION
	This enqueues the dsm item into appropriate RX WMQ.

===========================================================================*/

void
diagcomm_enqueue_dsm_dci (dsm_item_type ** item_ptr)
{

  /* Enqueue the dsm items in the rx queue */
  dsm_enqueue (&diagcomm_smd_dci_rx_wmq[DIAGCOMM_PORT_1], item_ptr);
  diag_fwd_notify_dci();

} /*diagcomm_enqueue_dsm_dci  */

/*===========================================================================

FUNCTION DIAGCOMM_SIO_DATA_TO_DSM_DCI

DESCRIPTION
	This enqueues takes the data from given buffer and copies into
	the dsm item and returns the dsm item pointer back.(It takes care
	of chaining if packet is huge)

===========================================================================*/

dsm_item_type ** diagcomm_sio_data_to_dsm_dci(uint8* pkt_ptr, uint32 pkt_len)
{
  /* temp pointer for working */
  dsm_item_type * dsm_item_ptr_sav = NULL;
  uint8 * diag_send_buf_pointer = NULL;
  uint32 length = 0;
  uint32 writelen = 0;


  /* Master sends to Slaves with SMD pools */
  dsm_item_ptr_sav = diagcomm_new_dsm( DSM_DIAG_SMD_DCI_TX_ITEM_POOL, FALSE );
  
  if(dsm_item_ptr_sav)
  {

	diag_send_buf_pointer = (uint8*)pkt_ptr;
	length = pkt_len;
	writelen = dsm_pushdown_tail(&dsm_item_ptr_sav, diag_send_buf_pointer, length,
					  DSM_DIAG_SMD_DCI_TX_ITEM_POOL);
	
	if(writelen == length)
	{
		diagcomm_sio_outbound_item_ptr_data_dci = dsm_item_ptr_sav;
	}
  }

  return &diagcomm_sio_outbound_item_ptr_data_dci;

} /* diagcomm_sio_data_to_dsm_dci */
#endif /* DIAG_CONSUMER_API */

