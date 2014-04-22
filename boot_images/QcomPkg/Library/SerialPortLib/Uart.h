#ifndef __UART_H__
#define __UART_H__
/*==================================================================================================

FILE: Uart.h

DESCRIPTION: UEFI driver for UARTDM/UARTBAM

                              Copyright (c) 2013, 2016 ,2019 Qualcomm Technologies, Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                            FUNCTION DECLARATIONS AND DOCUMENTATION
==================================================================================================*/
/**
*  @brief Initialize the uart port
*
*  @details Initializes the debug uart port according to the configs present in the settings file.
*           baud rate = 115200 bits per second , 
*           No of bits per character = 8 ,
*           Length of stop bit = 1 ,
*
*  @return
*  RETURN_SUCCESS               -- Uart Port successfully opened  \n
*  RETURN_UNSUPPORTED           -- Uart Port Config not present \n
*/
RETURN_STATUS EFIAPI uart_initialize(void);

/**
*  @brief Deinitializes the uart port
*
*  @details  This API turns off the serial and AHB clock
*
*  @return
*  VOID             --Returns nothing
*/

VOID        EFIAPI uart_deinitialize(void); 


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
BOOLEAN       EFIAPI uart_poll(void);

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
UINTN         EFIAPI uart_read(OUT UINT8 *user_buffer, IN UINTN bytes_requested);

/**
*  @brief Writes data to the Hardware Port from the Software Buffer.
*
*  @details  This function transmits the data bytes requested to be sent, from the Software Buffer(User Buffer)
*            and writes it to the hardware Port.
*
*  @param[IN] user buffer       Software buffer from which data is written to the Hardware Port.
*  @param[IN] bytes_to_send     No of bytes requested to be send(transmitted)
*
*  @return
*  0                    -- Uart port config not present \n
*  bytes_sent           -- Returns the number of bytes successfully written to the Hardware Port through the uart bus \n
*/
UINTN         EFIAPI uart_write(IN UINT8 *user_buffer, IN UINTN bytes_to_send);

/**
*  @brief Check Break Detection Condition
*
*  @details Checks if RX break detection condition is satisfied, or cable is connected successfully or not.
*
*  @return
*  FALSE                 -- RX Break not detected or cable is successfully connected \n
*  TRUE                  -- RX Break detected or cable not connected successfully \n
*/
BOOLEAN       EFIAPI uart_rxBreakonDetected(void);

#endif  // __UART_H__
