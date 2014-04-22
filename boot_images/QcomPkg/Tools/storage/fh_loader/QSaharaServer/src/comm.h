/*
 * Copyright (C) 2012, 2018-20 Qualcomm Technologies, Inc. All rights reserved.
 *
 * All data and information contained in or disclosed by this document is
 * confidential and proprietary information of Qualcomm Technologies, Inc. and all
 * rights therein are expressly reserved.  By accepting this material the
 * recipient agrees that this material and the information contained therein
 * is held in confidence and in trust and will not be used, copied, reproduced
 * in whole or in part, nor its contents revealed in any manner to others
 * without the express written permission of Qualcomm Technologies, Inc.
 *
 *
 *  comm.h : Defines protocol states, and send/receive functions and parameters.
 * ==========================================================================================
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Tools/storage/fh_loader/QSaharaServer/src/comm.h#6 $
 *   $DateTime: 2021/03/08 04:36:02 $
 *   $Author: pwbldsvc $
 *
 *  Edit History:
 *  YYYY-MM-DD		who		why
 *  -----------------------------------------------------------------------------
 *  2010-09-28       ng      Added command mode support
 *  2010-10-18       ab      Added memory debug mode support
 *
 *  Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 *==========================================================================================
 */


#ifndef COMM_H
#define COMM_H

#include "common_protocol_defs.h"

typedef enum {
    USING_USB,
    USING_UART,
    USING_SDIO,
    USING_PCIE
} transport_medium_t;


typedef enum { 
    SAHARA_PROTOCOL,
    DLOAD_PROTOCOL
} protocol_type_t;

typedef struct  {
    /* name of current port */
    char *port_name;

    /* handle to COM port */
    PORT_HANDLE port_fd;

    size_t total_bytes_sent;

    size_t total_bytes_recd;

    transport_medium_t transport_medium;

    protocol_type_t protocol;

    int rx_timeout;

    unsigned int sleep_btwn_reads_usec;

    size_t MAX_TO_READ;
    size_t MAX_TO_WRITE;
	size_t BYTES_TO_PRINT;

} com_port_t;



extern com_port_t com_port;

void uart_port_disconnect (void);
void usb_port_disconnect (void);
bool uart_port_connect (char *port_name);
bool usb_port_connect (char *port_name);
bool uart_tx_data(byte *buffer, size_t bytes_to_send, size_t *bytes_written);
bool boot_usb_tx_data(byte *buffer, size_t bytes_to_send);
bool uart_rx_data(byte *buffer, uint32_t bytes_to_read, uint32_t *bytes_read);  
bool boot_usb_rx_data(byte *buffer, size_t bytes_to_read, size_t *bytes_read);
uint32_t boot_transport_protocol_pkt_tx(uint8_t* sahara_packet_data, uint32_t length);
uint32_t boot_transport_protocol_pkt_rx(uint8_t* sahara_packet_data, uint32_t length, uint32_t* temp_bytes_read);
void map_function_table(void);
uint16_t crc_16_calc(byte*, uint16_t len);
void uart_transport_init(void);
void usb_transport_init(void);

/* Function table*/
typedef struct {

	void (*initialize_medium)(void);

	bool (*port_connect)(char*);
	void (*port_disconnect)(void);

	uint32_t (*boot_transport_tx)(uint8_t*, uint32_t);            
	uint32_t (*boot_transport_rx)(uint8_t*, uint32_t, uint32_t*);  

} function_table;

extern function_table Medium;
void usb_comm_populate_medium_interface(function_table*);
void uart_comm_populate_medium_interface(function_table*);

#endif
