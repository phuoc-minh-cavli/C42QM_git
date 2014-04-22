/*=============================================================================

                               Boot Transport
                               Source File
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
#include "boot_transport.h"
#include "boot_sahara.h"
#include "boot_util.h"
#include "boot_logger.h"
#include "boot_dload_debug.h"
#include "crc.h"
#include "boot_cache.h"
#include "boot_shared_functions_consumer.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

static void boot_transport_ack_tx(uint32 * err_code);
static void boot_transport_nak_tx(uint32 * err_code);
static boolean boot_wait_for_ack(uint32 * err_code);

static uint8 seq_number = 0;
static uint8 ALIGN(32) transport_packet_buffer[SAHARA_MAX_PACKET_SIZE_IN_BYTES] SECTION(".bss.BOOT_DDR_UNCACHED_ZI_ZONE");
static uint8 ALIGN(32) transport_packet_rx_buffer[SAHARA_MAX_PACKET_SIZE_IN_BYTES] SECTION(".bss.BOOT_DDR_UNCACHED_ZI_ZONE");
static boot_packet_interface* packet_layer_table = NULL;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

uint32 boot_transport_layer_init(boot_packet_interface* packet_layer_interface)
{
  boolean status = FALSE;
  uint16 crc_data = 0;
  uint16 crc_data_rx = 0;
  boolean version_check = BULK_ERR_UNKNOWN;
  packet_layer_table = packet_layer_interface;
  uint32 tx_length, rx_length_exp, rx_length;
  uint32 err_code = 0;
  
  struct version_packet_header* version_pkt_header = 
    (struct version_packet_header*)transport_packet_buffer;
	
	  struct version_packet_header* version_pkt_rx_header = 
    (struct version_packet_header*)transport_packet_rx_buffer;

  rx_length_exp = sizeof(struct version_packet_header) + sizeof(crc_data); 

  if(BULK_SUCCESS != packet_layer_table->init())
  {
     return version_check;
  }  
  
  do
  {
	  err_code = 0;
	  //First receive the version packet
      rx_length = packet_layer_table->packet_rx((uint8 *)transport_packet_rx_buffer, rx_length_exp, NULL, &err_code);  
	  
	  if(rx_length == 0)
	  {
			continue; 
	  }
	  
	  if(rx_length != rx_length_exp)
	  {
			// Send NAK
			boot_transport_nak_tx(&err_code);
			continue; 
	  }
	  
	  //Do CRC check for received packet
      crc_data = crc_16_calc(transport_packet_rx_buffer, sizeof(struct version_packet_header));
	  crc_data_rx = *(uint16 *)(transport_packet_rx_buffer + sizeof(struct version_packet_header));
	  
	  if(crc_data != crc_data_rx)
	  {
		  // Send NAK
		  boot_transport_nak_tx(&err_code);
		  continue;
	  }
	  else
      {
	   
	     if(version_pkt_rx_header->packet_id != VERSION_PACKET)
	     {
	   	     // Send NAK
	         boot_transport_nak_tx(&err_code);
		     continue;
	     }
		 
	     if(seq_number == version_pkt_rx_header->seq_no)	 
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
  if( (version_pkt_rx_header->major_id == packet_layer_table->major_id) && (version_pkt_rx_header->minor_id == packet_layer_table->minor_id))
  {
	  version_check = BULK_SUCCESS;
  }
  else
  {
	  return version_check;
  }
	 
  //Now transmit the version pkt	
 
  
  tx_length = sizeof(struct version_packet_header) + sizeof(crc_data); 
  
  while(status == FALSE)
  {
	  err_code = 0;
	  version_check = BULK_ERR_UNKNOWN;
	  version_pkt_header->packet_id = VERSION_PACKET;
      version_pkt_header->seq_no = seq_number + 1; 
      version_pkt_header->major_id = packet_layer_table->major_id;
      version_pkt_header->minor_id = packet_layer_table->minor_id;
  
      //Do CRC
      crc_data = crc_16_calc(transport_packet_buffer, sizeof(struct version_packet_header));
      qmemcpy((transport_packet_buffer + sizeof(struct version_packet_header)), &crc_data, sizeof(crc_data));
	  err_code = 0;
	  
	  //Pass the packet to UART
	  packet_layer_table->packet_tx((uint8 *)transport_packet_buffer, tx_length, NULL, &err_code);
  
		//Wait for ACK
	  *((uint8 *)transport_packet_rx_buffer) = 0;
	  status = boot_wait_for_ack(&err_code);
	  if(err_code == SAHARA_NAK_INVALID_PACKET_SIZE)
	  {
		  version_check = err_code;
		  break;
	  }
  }
  
  if(status == TRUE)
  {
		seq_number += 1;
		version_check = BULK_SUCCESS;
  }
  //dcache_inval_region(&version_check, sizeof(version_check));
  
  return version_check;
	
}

uint32 boot_transport_protocol_pkt_tx(uint8* sahara_packet_data, uint32 length, sahara_tx_cb_type tx_cb, uint32 * err_code)
{
  boolean status = FALSE;
  uint16 crc_data = 0;
  
  uint32 tx_length, tx_data_length;
  uint32 remaining_data = length;
  
  //Implement this API
  uint32 max_length = packet_layer_table->get_max_raw_data_tx_size();
  
  struct protocol_packet_header* protocol_pkt_header = 
    (struct protocol_packet_header*)transport_packet_buffer;
  
  uint8* data_segment = transport_packet_buffer + sizeof(struct protocol_packet_header);	
  
  do{
	  if(packet_layer_table->rx_poll != NULL)
	  {
		if(packet_layer_table->rx_poll())
		{
			*err_code = BULK_ERR_TIMEOUT;
			break;
		}
	  }
  
	  tx_data_length = max_length - (sizeof(struct protocol_packet_header)+ sizeof(crc_data));
  
	  while( remaining_data>0 )
	  {
	   	protocol_pkt_header->packet_id = PROTOCOL_PACKET;
		protocol_pkt_header->seq_no = seq_number + 1;
	
		if(remaining_data < tx_data_length)
		{
			tx_data_length = remaining_data;
		}
	
		//Copy over Sahara packet Data to data region of protocol pkt
		qmemcpy(data_segment, sahara_packet_data, tx_data_length);
  
		//Do CRC
		crc_data = crc_16_calc(transport_packet_buffer, (tx_data_length + sizeof(struct protocol_packet_header)));
		qmemcpy((data_segment + tx_data_length), &crc_data, sizeof(crc_data));
	
		tx_length = sizeof(struct protocol_packet_header) + tx_data_length + sizeof(crc_data); 
  
		while(status == FALSE)
		{
			//Pass the packet to UART
			packet_layer_table->packet_tx((uint8 *)transport_packet_buffer, tx_length, tx_cb, err_code);
  
			//Wait for ACK
			*((uint8 *)transport_packet_rx_buffer) = 0;
			status = boot_wait_for_ack(err_code);
		
		}
		if(status == TRUE)
		{
			seq_number += 1;
			remaining_data -= tx_data_length;
			sahara_packet_data += tx_data_length;
			status = FALSE;
		}	  
	  }
	}while(0);

  tx_cb((length - remaining_data), *err_code);
  
  return (length - remaining_data);
}

uint32 boot_transport_protocol_pkt_rx(uint8* sahara_packet_data, uint32 length, sahara_rx_cb_type rx_cb, uint32 * err_code)
{
  uint32 rx_length, rx_data_length, rx_data_length_exp, rx_length_exp;
  uint32 remaining_data = length;
  boolean short_packet = FALSE;
  uint16 crc_data = 0;
  uint16 crc_data_rx = 0;
  
  //Implement this API
  uint32 max_length = packet_layer_table->get_max_raw_data_tx_size();
  rx_length_exp = max_length;
  
  struct protocol_packet_header* protocol_pkt_header = 
           (struct protocol_packet_header*)transport_packet_rx_buffer; 
  uint8* data_segment = transport_packet_rx_buffer + sizeof(struct protocol_packet_header);		   

  rx_data_length_exp = max_length - (sizeof(struct protocol_packet_header)+ sizeof(crc_data));		   
  
  while(remaining_data>0)
  {
	*err_code = 0;
	short_packet = FALSE;
	
	if(remaining_data < rx_data_length_exp)
	{
		rx_data_length_exp = remaining_data;
		rx_length_exp = sizeof(struct protocol_packet_header) + rx_data_length_exp + sizeof(crc_data);
	}
	
    rx_length = packet_layer_table->packet_rx((uint8 *)transport_packet_rx_buffer, rx_length_exp, boot_transport_nak_tx, err_code);
	
	
	if(*err_code != 0 && *err_code != BULK_ERR_TIMEOUT)
	{
		 // Send NAK
	    boot_transport_nak_tx(err_code);
	    continue; 
	}
	
	if(rx_length == 0)
	{
		boot_transport_nak_tx(err_code);
		continue;
	}
	
	if(rx_length != rx_length_exp)
	{
		//Consult and decide if this should be reported as a failure
		short_packet = TRUE;
	}
	
	rx_data_length = rx_length - (sizeof(struct protocol_packet_header)+ sizeof(crc_data));
  
    //Do CRC check for received packet
    crc_data = crc_16_calc(transport_packet_rx_buffer, (rx_data_length + sizeof(struct protocol_packet_header)));
	crc_data_rx = *(uint16 *)(transport_packet_rx_buffer + sizeof(struct protocol_packet_header) + rx_data_length);
	if(crc_data != crc_data_rx)
	{
		// Send NAK
	    boot_transport_nak_tx(err_code);
	    continue;  
	}
    else
    {
	   
	   if(protocol_pkt_header->packet_id != PROTOCOL_PACKET)
	   {
	   	   // Send NAK
		   // Have this as a sahara failure and return
	       boot_transport_nak_tx(err_code);
		   continue;
	   }
		 
	   if(seq_number == protocol_pkt_header->seq_no)	 
	   {
		   //Send ACK and return
		   boot_transport_ack_tx(err_code);
		   continue;
	   }
	 
    }
	boot_transport_ack_tx(err_code);
    qmemcpy(sahara_packet_data, data_segment, rx_data_length);
    seq_number += 1;
	remaining_data -= rx_data_length;
	sahara_packet_data += rx_data_length;
	*err_code = 0;
	if( short_packet == TRUE )
	{
		//*err_code = BULK_ERR_READ; 
		break;
	}
  }
  
  rx_cb((length - remaining_data), *err_code);
 
  return (length - remaining_data);	
}

static void boot_transport_ack_tx(uint32 * err_code)
{
  uint32 tx_length;
  uint8 ack_packet =  ACK_PACKET;
  *transport_packet_buffer = ack_packet;
  
  tx_length = sizeof(ack_packet);
  
  *err_code = 0;
  
  //Pass the packet to UART
  packet_layer_table->packet_tx((uint8 *)transport_packet_buffer, tx_length, NULL, err_code); 
}

static void boot_transport_nak_tx(uint32 * err_code)
{
  uint32 tx_length;
  
  uint8 nak_packet =  NAK_PACKET;
  *transport_packet_buffer = nak_packet;
  *err_code = 0;
  
  tx_length = sizeof(nak_packet);
  
  //Pass the packet to UART
  packet_layer_table->packet_tx((uint8 *)transport_packet_buffer, tx_length, NULL, err_code); 
}

static boolean boot_wait_for_ack(uint32 * err_code)
{
  boolean status = FALSE;
  uint8 packet_type;
  uint32 rx_length;
  
  do
  {
	  *err_code = 0;	
	  rx_length = packet_layer_table->packet_rx((uint8 *)transport_packet_rx_buffer, sizeof(packet_type), NULL, err_code);
  
	  if( *err_code!=0 )
	  {
		if(*err_code == BULK_ERR_TIMEOUT)
		{
			continue;
		}
		else
		{
			*err_code = 0;
		}
		/*else
		{
			return(status);
		}*/
	  }
  
	  if(rx_length != sizeof(packet_type))
	  {
		*err_code = SAHARA_NAK_INVALID_PACKET_SIZE;
		return(status);
	  }
  
	  if((uint8)(*transport_packet_rx_buffer) == ACK_PACKET)
	  {
		status = TRUE;
	  }
	  
	  return(status);
  } while(1);
   
}

