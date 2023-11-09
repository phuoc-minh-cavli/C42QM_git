/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#ifndef __QCA4004_UART_H__
#define __QCA4004_UART_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qapi_types.h"
#include "qapi_uart.h"

#include "qurt.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define QCA4004_RX_BUFFER_SIZE	(2048)
#define QCA4004_TX_BUFFER_SIZE	(128)

#define QCA4004_RECEIVE_EVENT_MASK	(0x00000001)
#define QCA4004_DAEMON_EVENT_MASK	(0x00000002)


#define DELIMITER_COMMAND_CHARACTER		'\n'
#define START_OF_COMMAND_STRING			"ATM="
#define SIZE_OF_COMMAND_OTHER			sizeof(START_OF_COMMAND_STRING) + 1

#define COMMAND_SCAN_STRING				"SCAN"

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
typedef struct QCA4004_UART_Ctx_s
{
	uint8_t 			buffer[QCA4004_RX_BUFFER_SIZE];
	volatile uint32_t	bufferFree;

	qapi_UART_Handle_t	uartHandle;
	qurt_signal_t 		event;
} QCA4004_UART_Ctx_t;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function initializes the UART port.

   @return
    - 0 if Port open was successful.
    - 1 if Port open failed.
*/
uint8_t qca4004_uart_open(void);

/**
   @brief This function Closes the UART port for QCA4004.

   @return
    - 0 if Port close was successful.
    - 1 if Port close failed.
*/
uint8_t qca4004_uart_close(void);

/**
   @brief This function is the main thread , 
   		it monitor the data which come from QCA4004 and process.
*/
void qca4004_thread(void *Param);

/**
   @brief This function transmits data from a specified buffer to QCA4004.

   @param[in] length is Bytes of data to transmit.  
   @param[in] buffer is Buffer with data for transmit.

   @return
    - 0 if Queuing of the transmit buffer was successful.
    - 1 if Queuing of the transmit buffer failed. 
*/
uint8_t qca4004_write(uint32_t length, const uint8_t *buffer);

/**
   @brief This function send a formatted command to QCA4004.

   @param[in] length is Bytes of command to transmit.  
   @param[in] buffer is Buffer with command for transmit.

   @return
    - 0 if send command successful.
    - 1 if send command failed. 
*/
uint8_t qca4004_send_command(uint32_t length, const char *buffer);

#endif

