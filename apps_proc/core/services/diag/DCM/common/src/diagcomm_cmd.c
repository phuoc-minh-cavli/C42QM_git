/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications for Diag Command/Response Channel

General Description

Copyright (c) 2014,2016, 2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_cmd.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
04/29/14   sr      Fixed the infinite while loop issue in 
                   diagcomm_cmd_hdlc_encode_to_dsm() 
04/11/14   is      Resolve 4k response when no 7E escaping issue
03/28/14   is      Resolve 4k response issue
02/21/14   is      Command/response separation

===========================================================================*/

#include "diagcomm_cmd.h"
#include "diagdsm_v.h"       /* For DSM_DIAG_CMD_RX_ITEM_SIZ, etc. */
#include "msg.h"             /* For MSG_1, etc. */
#include "assert.h"          /* For ASSERT() */
#include "diagcomm_v.h"
#include "diagbuf_v.h"       /* For diag_send_desc_type */
#include "diagi_v.h"         /* For DIAG_NON_HDLC_ENCODED */
#include "stringl.h"

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
#ifdef DIAG_CONSUMER_API 
  extern diagcomm_io_conn_type diagcomm_io_dci_cmd_conn[NUM_SMD_PORTS];			  /* From diagcomm_dci.c */
#endif /* #ifdef DIAG_CONSUMER_API */

static q_type diagcomm_cmd_rx_q[NUM_SMD_PORTS];
static q_type diagcomm_cmd_tx_q[NUM_SMD_PORTS];

dsm_watermark_type diagcomm_cmd_rx_wmq[NUM_SMD_PORTS];
dsm_watermark_type diagcomm_cmd_tx_wmq[NUM_SMD_PORTS];

/*Counter to track DSM allocation failures in cmd_sio_tx pool*/
uint32 diagcomm_cmd_outbound_alloc_fail = 0;

static dsm_item_type *diagcomm_sio_outbound_item_ptr_cmd_hdlc = NULL;
char diag_cmd_pkt_inbound[DIAG_READ_BUF_SIZE*2];


/*===========================================================================

FUNCTION DIAGCOMM_CMD_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communications layer
  for the command/response channel.

PARAMETERS
  port_type - Cmd channel port type
  port_num - Cmd channel to initilize
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_cmd_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num )
{  
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( (port_num >= DIAGCOMM_PORT_NUM_FIRST) && (port_num <= DIAGCOMM_PORT_NUM_LAST) );
  
  conn = &diagcomm_io_cmd_conn[port_type][port_num];
      
  /* Make sure we are closed */
  diagcomm_cmd_close( port_type, port_num );
  if( conn->port_num == DIAGCOMM_PORT_1 )
  {
    memset (&diagcomm_cmd_rx_wmq[port_num], 0, sizeof (dsm_watermark_type));
    
    dsm_queue_init
    (
      &diagcomm_cmd_rx_wmq[conn->port_num],                     
      DSM_DIAG_CMD_SMD_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_RX_ITEM_SIZ,      
      &diagcomm_cmd_rx_q[conn->port_num]                        
    );
  
    diagcomm_cmd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_RX_ITEM_SIZ;
  
	conn->rx_wmq_ptr = &diagcomm_cmd_rx_wmq[conn->port_num];
  
    dsm_queue_init
    (
      &diagcomm_cmd_tx_wmq[conn->port_num],                     
      DSM_DIAG_CMD_SMD_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_TX_ITEM_SIZ,      
      &diagcomm_cmd_tx_q[conn->port_num]                        
    );
  
    diagcomm_cmd_tx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_TX_ITEM_SIZ;
  }

  #ifdef DIAG_CONSUMER_API
  else if( conn->port_num == DIAGCOMM_PORT_2 )
  {
      memset (&diagcomm_cmd_rx_wmq[port_num], 0, sizeof (dsm_watermark_type));
      memset (&diagcomm_cmd_tx_wmq[port_num], 0, sizeof (dsm_watermark_type));
      
      dsm_queue_init
      (
        &diagcomm_cmd_rx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SMD_DCI_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ,      
        &diagcomm_cmd_rx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_DCI_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ;
      
      dsm_queue_init
      (
        &diagcomm_cmd_tx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ,      
        &diagcomm_cmd_tx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_tx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ;
  }
  #endif /* #ifdef DIAG_CONSUMER_API */
  
} /* diagcomm_cmd_init */

  
/*===========================================================================

FUNCTION DIAGCOMM_CMD_OPEN

DESCRIPTION
  Opens the communications stream for the command/response channel.

PARAMETERS
  port_type - Cmd channel port type
  channel - Cmd channel to open
  
RETURN VALUE
  True if control port opened successfully.
  False otherwise.
  
===========================================================================*/
boolean 
diagcomm_cmd_open( diagcomm_port_type port_type, diagcomm_enum_port_type channel )
{
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( (channel >= DIAGCOMM_PORT_NUM_FIRST) && (channel <= DIAGCOMM_PORT_NUM_LAST) );
  
  conn = &diagcomm_io_cmd_conn[port_type][channel];
  
  if( conn->connected )
  {
    return conn->connected;
  }
  
  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
    }
    else
    {
	  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_cmd_open - Could not open stream (%d, %d)", port_type, channel);
    }
  }
  
  return conn->connected;
  
} /* diagcomm_cmd_open */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_CLOSE

DESCRIPTION
  Closes the communications stream for the command/response channel, if any.

PARAMETERS
  port_type - Cmd channel port type
  channel - Cmd channel to close
 
RETURN VALUE
  None
  
===========================================================================*/
void 
diagcomm_cmd_close( diagcomm_port_type port_type, diagcomm_enum_port_type channel )
{
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( (channel >= DIAGCOMM_PORT_NUM_FIRST) && (channel <= DIAGCOMM_PORT_NUM_LAST) );
  
  conn = &diagcomm_io_cmd_conn[port_type][channel];
  
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
    
  }

  /* Success or not, consider close operation successful */
  conn->connected = FALSE;
  
} /* diagcomm_cmd_close */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns CMD RX watermark queue count.

===========================================================================*/
uint32 diagcomm_cmd_get_rx_wmq_cnt(diagcomm_port_type port_type, diagcomm_enum_port_type port_num)
{
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( (port_num >= DIAGCOMM_PORT_NUM_FIRST) && (port_num <= DIAGCOMM_PORT_NUM_LAST) );
  
  return diagcomm_io_get_wmq_cnt( &diagcomm_cmd_rx_wmq[port_num] );
  
} /* diagcomm_cmd_get_rx_wmq_cnt */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_HDLC_ENCODE_TO_DSM

DESCRIPTION
  This function takes a buffer to a response packet and HDLC encodes it into 
  DSM buffer(s), chaining them if neccessary.
  
PARAMETERS
  pkt_ptr - Pointer to a response packet buffer
  pkt_len - Length of the response packet in 'pkt_ptr'
  
RETURN VALUE
  Pointer to a DSM item which contains the HDLC encoded response packet. 
  This DSM item may be chained.

===========================================================================*/
dsm_item_type ** diagcomm_cmd_hdlc_encode_to_dsm(uint8* pkt_ptr, uint32 pkt_len)
{
  dsm_item_type *diag_cmd_hdlc_item_ptr = NULL;
  unsigned int outbound_used = 0;
  diag_send_desc_type send_desc_data = {NULL, NULL, DIAG_SEND_STATE_START, TRUE, TRUE, TRUE};
  diag_hdlc_dest_type enc = {NULL, NULL, 0};
  diag_send_desc_type *send_desc = &send_desc_data;
  uint8 *src_data_ptr = pkt_ptr;
  uint32 remaining_bytes = pkt_len;
  boolean is_chained = FALSE;
 
  /* Response packet is contiguous, so set end of source ptr once for whole packet. */
  send_desc->last = pkt_ptr + pkt_len - 1;
  
  while( (remaining_bytes > 0) || (send_desc->state != DIAG_SEND_STATE_COMPLETE) )
  {
    /* Allocate a new DSM to HDLC encode the response before sending. */
    diag_cmd_hdlc_item_ptr = diagcomm_cmd_outbound_alloc_hdlc(&outbound_used, is_chained);
          
    if( diag_cmd_hdlc_item_ptr ) 
    {
      diag_cmd_hdlc_item_ptr->app_field = DIAG_HDLC_ENCODED;
      
      if( remaining_bytes <= diag_cmd_hdlc_item_ptr->size )
      {
        /* Remaining bytes fits in a DSM */
        send_desc->terminate = TRUE;
      }
      else
      {
        /* Chaining needed for remaining bytes */
        send_desc->terminate = FALSE;
      }
      
      /* Starting source ptr is either beginning of packet or where HDLC left off for previous 
         DSM chunk. */
      send_desc->pkt = src_data_ptr;
      
      /* Starting dest ptr is always the current DSM data ptr. Ending dest ptr is always end of
         current DSM's size. */
      enc.dest = (void *)diag_cmd_hdlc_item_ptr->data_ptr;
      enc.dest_last = (void *)((byte *)diag_cmd_hdlc_item_ptr->data_ptr + diag_cmd_hdlc_item_ptr->size - 1);
      
      diag_hdlc_encode( send_desc, &enc );
      diag_cmd_hdlc_item_ptr->used = (uint32)enc.dest - (uint32)diag_cmd_hdlc_item_ptr->data_ptr;
            
      /* Subtract # of bytes processed from src to determine bytes remaining to encode */
      remaining_bytes = remaining_bytes - ((uint8*)send_desc->pkt - src_data_ptr);
      
      if( remaining_bytes > 0 )
      {
        send_desc->state = DIAG_SEND_STATE_BUSY;
      }
      
      /* Set next src ptr to where the src left off after HDLC encoding */
      src_data_ptr = (uint8*)send_desc->pkt;
      
      /* Next DSM item allocations for same packet will be chained */
      is_chained = TRUE; 
    }
    else
    {
      if( is_chained )
      {
        /*Free the first DSM item*/
        dsm_free_packet(&diagcomm_sio_outbound_item_ptr_cmd_hdlc);
        diagcomm_sio_outbound_item_ptr_cmd_hdlc = NULL;
      }
      break;
    }
  } //end while
  
  return &diagcomm_sio_outbound_item_ptr_cmd_hdlc;
  
} /* diagcomm_cmd_hdlc_encode_to_dsm */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_OUTBOUND_ALLOC_HDLC

DESCRIPTION
  This function requests a buffer from the diag communications layer for
  a response packet when HDLC encoding has to be performed by APPS on 
  forwarding channel.

===========================================================================*/
dsm_item_type *
diagcomm_cmd_outbound_alloc_hdlc( unsigned int *count, boolean is_chain )
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);

  item_ptr = dsm_new_buffer(DSM_DIAG_CMD_SIO_TX_ITEM_POOL);

  if( item_ptr )
  {
    *count = item_ptr->size;
    item_ptr->pkt_ptr = NULL; 
  }
  else
  {
    diagcomm_cmd_outbound_alloc_fail++;
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Ran out of CMD SIO Tx Item Pool DSM items drop count: %d", diagcomm_cmd_outbound_alloc_fail);
    *count = 0;
#if defined(DIAG_DSM_DEBUG)
    ASSERT(0);
#endif
  }
  
  if( item_ptr )
  {
    if( diagcomm_sio_outbound_item_ptr_cmd_hdlc == NULL )
    {
      diagcomm_sio_outbound_item_ptr_cmd_hdlc = item_ptr;
    }
    else if (is_chain)
    {
      unsigned int chain_size = 0;
      dsm_item_type *tmp = diagcomm_sio_outbound_item_ptr_cmd_hdlc;

      diagbuf_incr_dsm_chained_count();
        
      while( tmp->pkt_ptr != NULL )
      {
        chain_size += tmp->size;
        ASSERT(chain_size <= DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ);
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
  
} /* diagcomm_cmd_outbound_alloc_hdlc */

/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_CMD

DESCRIPTION
  This function processes the incoming data from command channel.
  
PARAMETERS
	None
  
RETURN VALUE
	None 

===========================================================================*/
void diagcomm_process_cmd( void )
{
  diagcomm_process_inbound_cmd (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
}

/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_INBOUND_CMD

DESCRIPTION
  This function processes the incoming data from command channel.
  
PARAMETERS
	port_type 		- DIAGCOMM_PORT_SMD OR DIAGCOMM_PORT_SIO
	channel_type	- DIAGCOMM_PORT_1 OR DIAGCOMM_PORT_2
  
RETURN VALUE
	None 

===========================================================================*/
void
diagcomm_process_inbound_cmd( diagcomm_port_type port_type,
                                   diagcomm_enum_port_type channel_type )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_cmd_conn[port_type][channel_type];
  diagcomm_io_tx_params_type tx_params;
  diag_non_hdlc_type *non_hdlc_ptr = NULL;
  dsm_item_type **dsm_item_ptr= NULL;

  osal_lock_mutex(&conn->buf_mutex);
  memscpy(&diag_cmd_pkt_inbound[0], sizeof(diag_cmd_pkt_inbound), conn->buf,conn->used);
  conn->used = 0;
  osal_unlock_mutex(&conn->buf_mutex);

  if((DIAGCOMM_PORT_SMD == conn->port_type) &&  (DIAGCOMM_IO_CMD == conn->channel_type))
  {
    if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][conn->port_num].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS) &&
      DIAG_IS_INT_FEATURE_BIT_SET(diag_int_feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS) )
    {
        /* HDLC encoding is turned off on peripheral */
        non_hdlc_ptr = (diag_non_hdlc_type *)&diag_cmd_pkt_inbound[0];
        dsm_item_ptr = diagcomm_cmd_hdlc_encode_to_dsm((uint8 *)&(non_hdlc_ptr->pkt), non_hdlc_ptr->cmd_len);

        if((dsm_item_ptr) && (*dsm_item_ptr))
        {
            /* Just fwd the DSM out since already it is HDLC/NHDLC encoded. */
            tx_params.dsm_ptr = *dsm_item_ptr;

            /* Responses and data are sent out same port to USB/UART/etc. DSM will be freed by USB etc. */
            diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );

			diagcomm_sio_outbound_item_ptr_cmd_hdlc = NULL;
        }
    }
  }
}

/*===========================================================================


FUNCTION DIAGCOMM_SEND_CMD

DESCRIPTION
  Sends a packet over SMD channel

PARAMETERS
  req_pkt - packet to send
  pkt_len - length of 'req_pkt'
  port_num - channel to send pkt on

RETURN VALUE
  None

===========================================================================*/
void diagcomm_send_cmd( void *req_pkt, uint16 pkt_len,
                        diagcomm_enum_port_type port_num, boolean send_dci )
{
  diagcomm_io_tx_params_type tx_params;
  diagcomm_io_conn_type *cmd_conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][port_num];
  #ifdef DIAG_CONSUMER_API
    diagcomm_io_conn_type *cmd_conn_dci = &diagcomm_io_dci_cmd_conn[port_num];
  #endif /* #ifdef DIAG_CONSUMER_API */

  ASSERT(req_pkt != NULL);

  if (port_num > DIAGCOMM_PORT_NUM_LAST)
  {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_send: Invalid port_num (%d)", port_num);
      return;
  }
  tx_params.smdl_params.buffer = req_pkt;
  tx_params.smdl_params.len = pkt_len;
  tx_params.smdl_params.flags = 0;
  
  if(!send_dci)
  {
	  if( diagcomm_status() && diagcomm_io_status(cmd_conn) )
	  {
		diagcomm_io_transmit(cmd_conn, &tx_params);		
	  }
  }
  else
  {
      #ifdef DIAG_CONSUMER_API
        if( diagcomm_io_status(cmd_conn_dci) )
        {
          diagcomm_io_transmit(cmd_conn_dci, &tx_params);		
        }
      #endif /* DIAG_CONSUMER_API */
  }
} /* diagcomm_send_cmd */

