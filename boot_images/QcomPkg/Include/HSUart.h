#ifndef __HS_UART_H__
#define __HS_UART_H__
/*==================================================================================================

FILE: HSUart.h

DESCRIPTION: UEFI driver for UARTDM/UARTBAM

                              Copyright (c) 2018-2019 Qualcomm Technologies, Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                       STRUCTURE
==================================================================================================*/
typedef struct
{
   BOOLEAN enable_loopback;
   UINTN baud_rate;
   BOOLEAN enable_flow_control;   
} HSUART_CONFIG;

typedef enum{
	HSUART_SUCCESS = 0,
	HSUART_OVERRUN_ERROR,
	HSUART_PARITY_FRAMING_ERROR,
} HSUART_STATUS;


/*==================================================================================================
                            FUNCTION DECLARATIONS AND DOCUMENTATION
==================================================================================================*/
/**
*  @brief Initialize the HSUart port
*
*  @details Initializes the HSUart port according to the configs present in the settings file.
*           baud rate = 115200 bits per second , 
*           No of bits per character = 8 ,
*           Length of stop bit = 1 ,
*
*  @return
*  RETURN_SUCCESS               -- HSUart Port successfully opened  \n
*  RETURN_UNSUPPORTED           -- HSUart Port Config not present \n
*/

RETURN_STATUS EFIAPI hsuart_initialize(HSUART_CONFIG *cf);

/**
*  @brief Closes the HSUart port
*
*  @details This API will close the HSUart port that was opened 
*
*  @return 
*  VOID                         --Returns nothing
*  
*/
VOID          EFIAPI hsuart_close(); 

/**
*  @brief Poll the Uart bus for data.
*
*  @details This function checks if data is available to be read from the hardware port.
*
*  @attention This function must be called after initializing the uart port.
*
*  @return
*  FALSE              -- If there is no data in the Port or if config not present\n
*  TRUE               -- If data is present in the Port \n
*/
BOOLEAN       EFIAPI hsuart_poll(void);

/**
*  @brief  Reads data from the Hardware Port and stores in Software Buffer.
*
*  @details  This function receives the data bytes requested to be read, from the hardware port 
*            and stores it in the software buffer(user buffer).
*
*  @param[OUT] user buffer       Software buffer of 8 bytes where data is stored after reading from the Hardware Port.
*  @param[IN] bytes_requested    No of bytes requested to be read(received).
*
*  @return
*  0                   --Uart port config not present  \n
*  bytes_read          --Returns the number of bytes successfully read from the Hardware Port through the uart bus \n
*/
UINTN         EFIAPI hsuart_read(OUT UINT8 *user_buffer, IN UINTN bytes_requested, UINT32 *error);

/**
*  @brief Writes data to the Hardware Port from the Software Buffer.
*
*  @details This function transmits the data bytes requested to be sent, from the Software Buffer(User Buffer)
*           and writes it to the hardware Port.
*
*  @param[IN] user buffer       Software buffer from which data is written to the Hardware Port.
*  @param[IN] bytes_to_send     No of bytes requested to be send(transmitted)
*
*  @return
*  0                    -- Uart port config not present \n
*  bytes_to_send        -- Returns the number of bytes successfully written to the Hardware Port through the uart bus \n
*/
UINTN         EFIAPI hsuart_write(IN UINT8 *user_buffer, IN UINTN bytes_to_send);

/**
*  @brief Check Break Detection Condition
*
*  @details Checks if RX break detection condition is satisfied, or cable is connected successfully or not.
*
*  @return
*  FALSE                 -- RX Break not detected or cable is successfully connected \n
*  TRUE                  -- RX Break detected or cable not connected successfully \n
*/
BOOLEAN       EFIAPI hsuart_rxBreakonDetected(void);

/**
*  @brief Returns the size of the maximum packet supported
*
*  @details Returns the size of the maximum packet supported
*
*  @return
*  MAX_PACKET_SIZE        --Size of the maximum packet that is supported.
*/
UINTN         EFIAPI hsuart_get_max_packet_size(void);

/**
*  @brief  Returns the maximum raw data transfer size 
*
*  @details  Returns the maximum raw data transfer size
*
*  @return
*  MAX_PACKET_SIZE        --Size of the maximum packet that is supported.
*/
UINTN         EFIAPI hsuart_max_raw_data_transfer_size(void);

/**
*  @brief  Clears the RX buffer. 
*
*  @details  Flushes/clears the whole RX buffer in case of overrun error caused
*            while reading data from the hardware buffer.
*
*  @return
*  VOID                    --Returns nothing.
*/
VOID           EFIAPI  hsuart_flush_rx_buffer();

#endif  // __HS_UART_H__
