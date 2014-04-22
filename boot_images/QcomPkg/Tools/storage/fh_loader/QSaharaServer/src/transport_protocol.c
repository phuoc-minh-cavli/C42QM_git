/*=============================================================================

                               Boot Transport
                               Source File
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
#include "transport_protocol.h"
#include "comm.h"
#if defined(WINDOWSPC) || defined(WIN32) 
#include "windows_utils.h"
#endif
#include "kickstart_log.h"
#include "sahara_protocol.h"

static bool boot_transport_ack_tx();
static bool boot_transport_nak_tx();
static bool boot_wait_for_ack();

static uint8_t seq_number = 0;
static byte* transport_packet_buffer = NULL;
static byte* transport_packet_rx_buffer = NULL;

uint32_t num_of_nak_tx = 0;
uint32_t num_of_nak_rx = 0;
uint32_t num_of_ack_tx = 0;
uint32_t num_of_ack_rx = 0;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

bool boot_transport_layer_init(void)
{
  bool status = false;
  uint16_t crc_data = 0;
  uint16_t crc_data_rx = 0;
  bool version_check = false;
  uint32_t tx_length, rx_length, rx_length_exp = 0;
  uint32_t err_code = 0;
  uint32_t bytes_read = 0;
  uint32_t bytes_written = 0;

  transport_packet_buffer = malloc(UART_MAX_PACKET_SIZE_IN_BYTES);
  transport_packet_rx_buffer = malloc(UART_MAX_PACKET_SIZE_IN_BYTES);
  struct version_packet_header* version_pkt_header = 
    (struct version_packet_header*)transport_packet_buffer;

  rx_length_exp = sizeof(struct version_packet_header) + sizeof(crc_data); 
  
  tx_length = sizeof(struct version_packet_header) + sizeof(crc_data); 
  
  while(status != true)
  {
	  //Now transmit the version pkt	
	  version_pkt_header->packet_id = VERSION_PACKET;
	  version_pkt_header->seq_no = seq_number + 1;
	  version_pkt_header->major_id = PACKET_MAJOR_ID;
	  version_pkt_header->minor_id = PACKET_MINOR_ID;

	  //Do CRC
	  crc_data = crc_16_calc(transport_packet_buffer, sizeof(struct version_packet_header));
	  memcpy((transport_packet_buffer + sizeof(struct version_packet_header)), &crc_data, (size_t)(sizeof(crc_data)));
	   
	  //Send version packet
	  uart_tx_data((uint8_t *)transport_packet_buffer, tx_length, &bytes_written);
  
	  //Wait for ACK
	  status = boot_wait_for_ack();

  }
  
  if(status == true)
  {
		seq_number += 1;
  }
  
  version_pkt_header = (struct version_packet_header*)transport_packet_rx_buffer;

  do
  {
	  err_code = 0;
	  bytes_read = 0;
	  
	  //First receive the version packet
      rx_length = uart_rx_data((uint8_t *)transport_packet_rx_buffer, rx_length_exp, &bytes_read);

	  if(rx_length_exp != bytes_read)
	  {
			// Send NAK
			boot_transport_nak_tx(&err_code);
			continue;
	  }
	  
	  //Do CRC check for received packet
      crc_data = crc_16_calc(transport_packet_rx_buffer, sizeof(struct version_packet_header));
	  crc_data_rx = *(uint16_t *)(transport_packet_rx_buffer + sizeof(struct version_packet_header));
	  if(crc_data != crc_data_rx)
	  {
		  // Send NAK
		  boot_transport_nak_tx(&err_code);
		  continue;
	  }
	  else
      {
	   
	     if(version_pkt_header->packet_id != VERSION_PACKET)
	     {
	   	     // Send NAK
		     // Have this as a sahara failure and return
			 dbg(LOG_ERROR, "Version packet incorrect");
	         boot_transport_nak_tx(&err_code);
		     continue;
	     }
		 
	     if(seq_number == version_pkt_header->seq_no)	 
	     {
		     //Send ACK and return
		     boot_transport_ack_tx(&err_code);
		     continue;
	     }
	 
      }
	  boot_transport_ack_tx(&err_code);
      seq_number += 1;	
	  break;
  
  }while(1);
	  
  //Check the received data
  if( (version_pkt_header->major_id == PACKET_MAJOR_ID) && (version_pkt_header->minor_id == PACKET_MINOR_ID))
  {
	  version_check = true;
  }
  else
  {
	  dbg(LOG_ERROR, "VERSION packet header mismatch!!");
  }
 
  return version_check;
	
}

void boot_transport_layer_deinit(void)
{
	/*frees up space allocated by initialization */
	free(transport_packet_buffer);
	free(transport_packet_rx_buffer);

	transport_packet_buffer = NULL;
	transport_packet_rx_buffer = NULL;
	
	// print statistics data
	print_statistics();
}

uint32_t boot_transport_protocol_pkt_tx(uint8_t* sahara_packet_data, uint32_t length)
{
  bool status = false;
  uint16_t crc_data = 0;
  uint32_t bytes_read = 0;
  uint32_t tx_length, tx_data_length, bytes_written = 0;
  uint32_t remaining_data = length;
  
  //Implement this API
  uint32_t max_length = UART_MAX_PACKET_SIZE_IN_BYTES;
  
  struct protocol_packet_header* protocol_pkt_header = 
    (struct protocol_packet_header*)transport_packet_buffer;
  uint8_t* data_segment = transport_packet_buffer + sizeof(struct protocol_packet_header);	
  
  do{
   /*if((true == uart_rx_data((uint8_t *)transport_packet_buffer, remaining_data, &bytes_read) && (bytes_read != 0)))
    {
		printf("Data in RX pipe \n");
	  break;
    }
	else
	{
		printf("RX pipe is clean\n");
	}*/

	 

  tx_data_length = max_length - (sizeof(struct protocol_packet_header)+ sizeof(crc_data));
  
  while( remaining_data>0 )
  {
	  

	if(remaining_data < tx_data_length)
	{
		tx_data_length = remaining_data;
	}
	
	status = false;
  
	while(status == false)
	{
		memset(transport_packet_buffer, 0, UART_MAX_PACKET_SIZE_IN_BYTES);
		protocol_pkt_header->packet_id = PROTOCOL_PACKET;
		protocol_pkt_header->seq_no = seq_number + 1;

		//Copy over Sahara packet Data to data region of protocol pkt
		memcpy(data_segment, sahara_packet_data, tx_data_length);

		tx_length = sizeof(struct protocol_packet_header) + tx_data_length + sizeof(crc_data);

		//Do CRC
		crc_data = crc_16_calc(transport_packet_buffer, (tx_data_length + sizeof(struct protocol_packet_header)));
		memcpy((data_segment + tx_data_length), &crc_data, (size_t)(sizeof(crc_data)));
		uart_tx_data((uint8_t *)transport_packet_buffer, tx_length, &bytes_written);
  
		//Wait for ACK
		status = boot_wait_for_ack();
		
	}
	if(status == true)
	{
		seq_number += 1;
		remaining_data -= tx_data_length;
		sahara_packet_data += tx_data_length;
	}	  
  }
 }while(0);
  
  return status;
}

uint32_t boot_transport_protocol_pkt_rx(uint8_t* sahara_packet_data, uint32_t length, uint32_t * temp_bytes_read)
{
  uint32_t rx_data_length;
  uint16_t crc_data = 0;
  uint16_t crc_data_rx = 0;
  uint32_t remaining_data = length;
  uint32_t bytes_read = 0;
  uint32_t max_length = UART_MAX_PACKET_SIZE_IN_BYTES;
  uint32_t rx_length_exp = max_length;
  bool short_packet = false;
  
  struct protocol_packet_header* protocol_pkt_header = 
           (struct protocol_packet_header*)transport_packet_rx_buffer;
  uint8_t* data_segment = transport_packet_rx_buffer + sizeof(struct protocol_packet_header);
  uint32_t rx_data_length_exp = max_length - (sizeof(struct protocol_packet_header) + sizeof(crc_data));

  while(remaining_data>0)
  {
	  short_packet = false;
	  if (remaining_data < rx_data_length_exp)
	  {
		  rx_data_length_exp = remaining_data;
		  rx_length_exp = sizeof(struct protocol_packet_header) + rx_data_length_exp + sizeof(crc_data);
	  }

    if(false == uart_rx_data((uint8_t *)transport_packet_rx_buffer, rx_length_exp, &bytes_read))
    {
		dbg(LOG_ERROR, "RX Data from UART failed");
    	return false;
    }

	rx_data_length = (bytes_read) - (sizeof(struct protocol_packet_header)+ sizeof(crc_data));

	if (bytes_read == 0)
	{
		continue;
	}

	if (bytes_read != rx_length_exp)
	{
		//Consult and decide if this should be reported as a failure
		short_packet = true;
	}
	
    //Do CRC check for received packet
    crc_data = crc_16_calc(transport_packet_rx_buffer, (rx_data_length + sizeof(struct protocol_packet_header)));
	crc_data_rx = *(uint16_t *)(transport_packet_rx_buffer + sizeof(struct protocol_packet_header) + rx_data_length);
	if(crc_data != crc_data_rx)
	{
		dbg(LOG_ERROR, "CRC data incorrect: %d", crc_data);
		// Send NAK
	    if(false == boot_transport_nak_tx())
	    {
			dbg(LOG_ERROR, "NAK TX failed");
	    	return false;
	    }
	    continue;  
	}
    else
    {
	   
	   if(protocol_pkt_header->packet_id != PROTOCOL_PACKET)
	   {
	   	   // Send NAK
		   // Have this as a sahara failure and return
		   dbg(LOG_ERROR, "Packet ID incorrect: %d", protocol_pkt_header->packet_id);
	       if(false == boot_transport_nak_tx())
	       {
	    	   return false;
	       }
		   continue;
	   }
		 
	   if(seq_number == protocol_pkt_header->seq_no)	 
	   {
		   //Send ACK and return
		   if(false == boot_transport_ack_tx())
	       {
			   dbg(LOG_ERROR, "ACK TX failed");
	    	   return false;
	       }
		   continue;
	   }
	 
    }
	boot_transport_ack_tx();
    memcpy(sahara_packet_data, data_segment, (size_t)rx_data_length);
    seq_number += 1;
	remaining_data -= rx_data_length;
	*temp_bytes_read += rx_data_length;
	sahara_packet_data += rx_data_length;
	if (short_packet == true)
	{
		break;
	}
  }

  return true;	
}

static bool boot_transport_ack_tx(void)
{
  uint32_t tx_length;
  uint8_t ack_packet =  ACK_PACKET;
  uint32_t bytes_written = 0;
  *transport_packet_buffer = ack_packet;

  
  tx_length = sizeof(ack_packet);
  num_of_ack_tx += 1;

  //Pass the packet to UART
  return uart_tx_data((uint8_t *)transport_packet_buffer, tx_length, &bytes_written); 
}

static bool boot_transport_nak_tx(void)
{
  uint32_t tx_length;
  uint32_t bytes_written = 0;
  
  uint8_t nak_packet =  NAK_PACKET;
  *transport_packet_buffer = nak_packet;
  
  tx_length = sizeof(nak_packet);
  num_of_nak_tx += 1;

  //Pass the packet to UART
  return uart_tx_data((uint8_t *)transport_packet_buffer, tx_length, &bytes_written); 
}

static bool boot_wait_for_ack(void)
{
  bool status = false;
  uint8_t packet_type;
  uint32_t bytes_read = 0;
  *(uint8_t *)transport_packet_rx_buffer = 0;
  
  if (false == uart_rx_data((uint8_t *)transport_packet_rx_buffer, sizeof(packet_type), &bytes_read))
  {
	  return status;
  }
  
  if (bytes_read == 0)
	return(status);
  

  if((uint8_t)(*transport_packet_rx_buffer) == ACK_PACKET)
  {
	  status = true;
	  num_of_ack_rx += 1;
  }
  else
  {
	  packet_type = *((uint8_t *)transport_packet_rx_buffer);
	  num_of_nak_rx += 1;
  }
  
  return(status); 
}

void print_statistics(void)
{
	printf("\n\nTransport Protocol Statistics \n");
	printf("Number of corrupted packets transmitted:   %d \n", num_of_nak_rx);
	printf("Number of good packets transmitted:        %d \n", num_of_ack_rx);
	printf("Number of corrupted packets received:      %d \n", num_of_nak_tx);
	printf("Number of good packets received:           %d \n", num_of_ack_tx);
}
