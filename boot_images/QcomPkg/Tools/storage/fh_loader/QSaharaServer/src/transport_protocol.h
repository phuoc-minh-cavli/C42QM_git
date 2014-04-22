#ifndef TRANSPORT_PROTOCOL_H
#define TRANSPORT_PROTOCOL_H

/*=============================================================================

                               Boot Transport
                               Header File
GENERAL DESCRIPTION
  This file provides the APIs to enter the Packet layer Protocol.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2018-20 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "common_protocol_defs.h"

#define PACKET_MINOR_ID 0
#define PACKET_MAJOR_ID 1
#define UART_MAX_PACKET_SIZE_IN_BYTES 4092
#define UART_MAX_READ_RETRIES 50
#define UART_LARGE_PACKETS_WAIT_THRESHOLD 400

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
	uint8_t       packet_id;
	uint8_t       seq_no;
};

/* Version Packet header */
struct version_packet_header
{
	uint8_t       packet_id;	
	uint8_t       seq_no;
	uint8_t       major_id;
	uint8_t       minor_id;
};

bool boot_transport_layer_init(void);
void boot_transport_layer_deinit(void);

uint32_t boot_transport_protocol_pkt_tx(uint8_t* sahara_packet_data, uint32_t length);

uint32_t boot_transport_protocol_pkt_rx(uint8_t* sahara_packet_data, uint32_t length, uint32_t * temp_bytes_read);

void print_statistics(void);

#endif /* TRANSPORT_PROTOCOL_H */ 
