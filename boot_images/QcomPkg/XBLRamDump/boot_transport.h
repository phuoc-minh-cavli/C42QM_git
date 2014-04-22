#ifndef BOOT_TRANSPORT_H
#define BOOT_TRANSPORT_H

/*=============================================================================

                               Boot Transport
                               Header File
GENERAL DESCRIPTION
  This file provides the APIs to enter the Packet layer Protocol.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2018-19 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/27/19   rohik      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_sahara.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define PACKET_MINOR_ID 0
#define PACKET_MAJOR_ID 1
#define UART_MAX_PACKET_SIZE_IN_BYTES 4096

enum boot_packet_type 
{
	PROTOCOL_PACKET = 0x0,
	ACK_PACKET      = 0x1,
	VERSION_PACKET  = 0x2,
	NAK_PACKET      = 0x3,
	PACKET_ERROR = 0x7FFFFFFF
};


/*=============================================================================
  Packet protocol packet defintions
=============================================================================*/
/* ***NOTE: all length fields are in bytes */

/* Protocol Packet header */
struct protocol_packet_header
{
	uint8       packet_id;	
	uint8       seq_no;
};

/* Version Packet header */
struct version_packet_header
{
	uint8       packet_id;	
	uint8       seq_no;	
	uint8       major_id;	
	uint8       minor_id;
};


/*=============================================================================
  Sahara protocol public defintions
=============================================================================*/

/* RX callback api for packet layer read api*/
typedef void (* boot_hsuart_read_cb_type) (uint32 * err_code);


/*=============================================================================
  Sahara protocol client defintions
=============================================================================*/
/* NOTE: Unless specified, all length fields/parameters are in bytes */

typedef struct 
{
  uint8       minor_id;
  
  uint8       major_id;
  
  /* Polls the driver for data */
  boolean      (*init)                    (void);
  
  /* Polls the driver for data */
  boolean      (*rx_poll)                    (void);

  /* Receives bulk data from driver */
  uint32       (*packet_rx)                (uint8 *rx_buf,
                                           uint32 len,
                                           boot_hsuart_read_cb_type rx_cb,
                                           uint32 * err_code);

  /* Transmits bulk data to driver */
  uint32       (*packet_tx)                (uint8 *tx_buf,
                                           uint32 len,
                                           sahara_tx_cb_type tx_cb,
                                           uint32 * err_code);

  /* Returns the packet size in bytes supported by driver */
  uint32       (*get_max_packet_size)      (void);

  /* Resets the target */
  uint32         (*get_max_raw_data_tx_size)                   (void);

} boot_packet_interface;


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/* Init api for packet layer*/
uint32 boot_transport_layer_init(boot_packet_interface* packet_layer_interface);

/* Transmit api for packet layer */
uint32 boot_transport_protocol_pkt_tx(uint8* sahara_packet_data, uint32 length, sahara_tx_cb_type tx_cb, uint32 * err_code);

/* Receive api for packet layer */
uint32 boot_transport_protocol_pkt_rx(uint8* sahara_packet_data, uint32 length, sahara_tx_cb_type rx_cb, uint32 * err_code);


#endif /* BOOT_TRANSPORT_H */ 