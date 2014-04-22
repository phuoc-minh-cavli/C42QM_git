#ifndef DIAGCOMM_V_H
#define DIAGCOMM_V_H
/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer.
  
Copyright (c) 2000-2014, 2016, 2019-2020,2022 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           Edit History
                           
 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_v.h#3 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
02/26/14   sa      Added support for compression in buffering mode.
02/21/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature 
10/01/13   sr      Added flow control checks on fwd channel   
10/01/13   xy      Removed code under FEATURE_WINCE 
02/27/13   sr      Added support to HDLC encode the fwd channel traffic 
                   on APPS 
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
03/02/12   sg      Added prototype for diagcomm_sio_ctrl_conn_changed()
01/31/12   is      Add flow control on diagcomm_sio_tx_wmq
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/04/11   is      Support for Diag over SMD-Lite
02/01/11   sg      Dual mask changes
10/25/10   is      Changes for RDM support in 8960 bring-up
09/28/10   sg      Moved diag_time_get from diagtarget.h to here
09/14/10   is      Added prototype for diagcomm_connect_state()
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/09/10   is      Extern diagpkt_filtered_cmd_set to resolve compilation error
07/23/10   mad     Moved declaration for diagpkt_filtered_cmd_set to this file
07/21/10   sg      Added dsm chaining support for rx response packets 
10/31/10   vs      Changes to support modified diagbuf_send_pkt_single_proc
12/13/09   sg      Added prototype for diagcomm_sio_flow_enabled
12/10/09   sg      Removed diagcomm_register_outbound_flow_ctrl()
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/17/09    mad    Featurized includion of customer.h and target.h for WM.
05/07/09   vk      moved declerations into diagcomm.h that were being referred from there
12/12/08    vg     CMI splip into diagcomm_v.h and diagcomm.h
12/16/07    pc     Added support for diag_dtr_enable and diag_dtr_changed_cb.
12/15/06    as     Fixed compiler warnings.
05/15/05    as     Added support for FEATURE_DATA_SERIALIZER
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag
12/14/04    sl     Added support for DIAG to hold multiple SIO ports
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"
#include "target.h" /* This is prob. not needed in AMSS either */


#include "diagcomm_io.h"     /* For diagcomm_io_conn_type, diagcomm_enum_port_type */

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
typedef void (*diagcomm_notify_fnc_ptr_type)( void );

typedef void (*diagcomm_tx_start_fnc_ptr_type)( void );

typedef void (*diagcomm_tx_stop_fnc_ptr_type)( void );

typedef void (*diagcomm_flush_cb_fnc_ptr_type)( void );


/* Definitions for Half-Duplex Async-HDLC mode.
 * NOTE: Under most circumstances, the communications layer doesn't need to 
 * know about HDLC.  Diag handles this.  However, in the case of routing mode,
 * The communications layer needs to be able to determine the end of a packet
 * in order to forward packets.
 */
#define DIAG_NO_ASYNC_CONTROL_CHAR  0    /* Flag character value to ignore tail 
                                            character check in UART             */
#define DIAG_ASYNC_CONTROL_CHAR     0x7E /* Flag character value.  Corresponds 
                                            to the ~ character                  */
#define DIAG_ASYNC_ESC_CHAR         0x7D /* Escape sequence 1st character value */
#define DIAG_ASYNC_ESC_MASK         0x20 /* Escape sequence complement value    */
#define DIAG_MIN_PKT_SIZE           3    /* Command code and CRC                */
#define DIAG_ESC_EXPAND_SIZE        2    /* Size of an expanded escape code     */
#define DIAG_ASYNC_TAIL_SIZE        3    /* CRC plus tail character             */
#define DIAG_FOOTER_SIZE            5    /* 2 byte CRC plus the possibility of 
                                            expanding per AHDLC protocol plus  
                                            the  ASYNC_FLAG                     */
#define DIAG_ESC_EXPAND_SHIFT       1    /* The number of bits to shift for
                                            expanded size due to escaping.      */


extern boolean diagpkt_filtered_cmd_set; /* Defined in diagpkt.c */

/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
    extern "C" {
#endif

void diagcomm_sio_close_cb ( void );

void diagcomm_sio_fusion_dci_close_cb ( void );

boolean diagcomm_connect_state( void );

void diagcomm_sio_open_init( diagcomm_io_conn_type * conn );

void diagcomm_sio_conn_changed( diagcomm_io_conn_type * conn , boolean connected );

diagcomm_io_conn_type * diagcomm_get_sio_conn( diagcomm_enum_port_type port_num );

/*===========================================================================

FUNCTION DIAGCOMM_SIO_INIT

DESCRIPTION
  This function initializes the diagnostic transport layer.

===========================================================================*/
void diagcomm_sio_init (diagcomm_enum_port_type port_num);

/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This given callback will be called when inbound data is available from 
  the comm layer.

===========================================================================*/
void diagcomm_register_inbound( 
   diagcomm_notify_fnc_ptr_type inbound_pkt_cb
);

/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN

DESCRIPTION
  Opens the communications port.

===========================================================================*/
boolean diagcomm_sio_open (diagcomm_enum_port_type port_num);
/*===========================================================================

FUNCTION DIAGCOMM_FLUSH_RX

DESCRIPTION
  Flushes the communications port's receive channel.

===========================================================================*/
void diagcomm_flush_rx (
  diagcomm_flush_cb_fnc_ptr_type flush_fp
);

/*===========================================================================

FUNCTION DIAGCOMM_FLUSH_TX

DESCRIPTION
  Flushes the communications port's transmit channel.

===========================================================================*/
void diagcomm_flush_tx (
  diagcomm_flush_cb_fnc_ptr_type flush_fp, unsigned int timeoutVal
);

/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the communications port.

===========================================================================*/
void diagcomm_close ( diagcomm_enum_port_type port_num );


/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

===========================================================================*/
boolean diagcomm_status (void);

/*===========================================================================

FUNCTION DIAGCOMM_SIO_FLOW_ENABLED

DESCRIPTION
  This function returns TRUE if the flow of traffic is enabled .

===========================================================================*/
boolean diagcomm_sio_flow_enabled(void);


/*===========================================================================

FUNCTION DIAGCOMM_SIO_TX_FLOW_ENABLED

DESCRIPTION
  This function returns TRUE if the flow of traffic is enabled .

===========================================================================*/
boolean diagcomm_sio_tx_flow_enabled(void);

/*===========================================================================
FUNCTION diagcomm_compression_flow_enabled
DESCRIPTION
  This function returns TRUE if compression is enabled.
===========================================================================*/
boolean diagcomm_compression_flow_enabled (void);
/*===========================================================================
FUNCTION diagcomm_sio_tx_wmq_flow_enabled

DESCRIPTION
   This function tells whether the flow to diagcomm_sio_tx_wmq is enabled 
   or not
===========================================================================*/
boolean diagcomm_sio_tx_wmq_flow_enabled( void );

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC

DESCRIPTION
  These functions requests a buffer from the diag communications layer
  
===========================================================================*/
dsm_item_type *
 diagcomm_outbound_alloc ( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,             /* Requesting a chain if variable is TRUE*/
  boolean priority,
  boolean is_cmd_rsp_type   /* True if allocating for a response packet */
);
/*===========================================================================
FUNCTION DIAGCOMM_OUTBOUND_ALLOC_COMPRESSED
DESCRIPTION
  These functions requests a buffer from the diag communications layer to send the
  compressed data.
  
===========================================================================*/
dsm_item_type *
diagcomm_outbound_alloc_compressed( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,             /* Requesting a chain if variable is TRUE*/
  boolean priority
);

dsm_item_type *
 diagcomm_outbound_alloc_ctrl ( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,         /* Requesting a chain if variable is TRUE*/
  boolean priority
);

/*===========================================================================

FUNCTION DIAGCOMM_MASTER_OUTBOUND_ALLOC_HDLC

DESCRIPTION
  This function requests a buffer from the diag communications layer when HDLC
  encoding has to be performed by APPS on its own data traffic in streaming mode

===========================================================================*/
dsm_item_type *
 diagcomm_master_outbound_alloc_hdlc ( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,         /* Requesting a chain if variable is TRUE */
  boolean priority
);


/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC_HDLC

DESCRIPTION
  This function requests a buffer from the diag communications layer when HDLC
  encoding has to be performed by APPS on forward channel

===========================================================================*/
dsm_item_type *
 diagcomm_outbound_alloc_hdlc ( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,         /* Requesting a chain if variable is TRUE */
  boolean priority
);

/*===========================================================================

FUNCTION DIAGCOMM_SEND_MASTER_TX

DESCRIPTION
  This function drains the apps data traffic when tx mode is streaming. 

===========================================================================*/
uint32 diagcomm_send_master_tx(
	diagcomm_enum_port_type port_num
);


/*===========================================================================

FUNCTION DIAGCOMM_SEND

DESCRIPTION
  This function sends the DSM item for the given stream. 

===========================================================================*/
void diagcomm_send ( 
  dsm_item_type *ptr, 
  unsigned int length,
  boolean is_cmd_rsp_type
);

/*===========================================================================

FUNCTION DIAGCOMM_SEND_CTRL
  
DESCRIPTION
  Sends the given data stream for control packets if the connection is open.
  
===========================================================================*/
void diagcomm_send_ctrl ( 
  dsm_item_type *ptr, 
  unsigned int length
);
/*===========================================================================
FUNCTION DIAGCOMM_SEND_COMPRESSED
DESCRIPTION
 This function sends the compressed chunk to the SIO layer
 
===========================================================================*/
void diagcomm_send_compressed ( 
  dsm_item_type *ptr, 
  unsigned int length
);

/*===========================================================================

FUNCTION DIAGCOMM_SEND_MASTER_HDLC

DESCRIPTION
    This function is called only when APPS HDLC encodes the data on forward
  channel. Sends the given data stream if the connection is open.
 
===========================================================================*/
void
diagcomm_send_master_hdlc (dsm_item_type * ptr, unsigned int length);


/*===========================================================================

FUNCTION DIAGCOMM_SEND_HDLC

DESCRIPTION
    This function is called only when APPS HDLC encodes the data on forward
  channel. Sends the given data stream if the connection is open.
 
===========================================================================*/
void
diagcomm_send_hdlc (dsm_item_type * ptr, unsigned int length);

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound 
  data is available, NULL is returned.

  This data stream is not packet delimited.  The stream may contain more 
  than one packet, or a parital packet.

===========================================================================*/
void * diagcomm_inbound (
  unsigned int *stream_len, /* number of bytes in the stream (if non-NULL)  */
  diagcomm_enum_port_type port_num/* Port on which packet is received */
);
/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void diagcomm_inbound_complete ( 
  void *ptr
);

/*===========================================================================

FUNCTION DIAGCOMM_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns the rx watermark queue count.

===========================================================================*/
uint32 diagcomm_get_rx_wmq_cnt(diagcomm_enum_port_type port_num);

void diagcomm_sio_tx_flush( void);

/*===========================================================================
FUNCTION DIAGCOMM_RESET_FLOW_CTRL_COUNT

DESCRIPTION
  This function resets the internal variable diagcomm_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.

===========================================================================*/
void diagcomm_reset_flow_ctrl_count( void );

/*===========================================================================
FUNCTION DIAGCOMM_GET_FLOW_CTRL_COUNT

DESCRIPTION
  This function returns the internal variable diagcomm_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.

===========================================================================*/
uint32 diagcomm_get_flow_ctrl_count( diagcomm_port_type port_type );

/*===========================================================================
FUNCTION DIAGCOMM_INCR_FLOW_CTRL_COUNT

DESCRIPTION
    This function increments the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.
    
===========================================================================*/
uint32 diagcomm_incr_flow_ctrl_count( diagcomm_port_type port_type );

void diagcomm_inbound_pkt_notify (dsm_item_type ** item_ptr);

#ifdef __cplusplus
    }
#endif
#endif /*DIAGCOMM_V_H*/
