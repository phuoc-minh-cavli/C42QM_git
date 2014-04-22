#ifndef BOOT_EXTERN_HSUART_INTERFACE_H
#define BOOT_EXTERN_HSUART_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN UART DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external hsuart drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2018-19 by QUALCOMM Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/19   rohik      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_hsuart_interface.h"
#include "boot_sahara.h"
#include "boot_transport.h"
#include "HSUart.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define UART_PACKET_POLL_MAX_LOOP_COUNT 30
#define UART_PACKET_POLL_PAUSE_TIME 100
#define UART_PACKET_READ_MAX_RETRIES 10000

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

**  Function :  boot_hsuart_initialize

** ==========================================================================
*/
/*!
 * 
 * @brief   This function initializes the HSUART driver
 *
 * @details This function initializes the HSUART driver
 * 
 * @param   None
 * 
 * @return  boolean result 
 * 
 */
boolean boot_hsuart_initialize(void);


/*===========================================================================

**  Function :  boot_hsuart_write

** ==========================================================================
*/
/*!
 * 
 * @brief   This function transmits a bulk of data upto the input length.
 *
 * 
 * @param   *tx_buf     -  [IN/OUT] Transmit buffer for data, must be 4 byte aligned 
 * @param   len         -  [IN    ] Length of data to trasmit in bytes
 * @param   tx_cb       -  [IN    ] Transmit complete callback
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes transmitted.
 * 
 */
uint32 boot_hsuart_write(uint8* sahara_packet_data, uint32 length, sahara_tx_cb_type tx_cb, uint32 * err_code);


/*===========================================================================

**  Function :  boot_hsuart_close

** ==========================================================================
*/
/*!
 * 
 * @brief   This function performs a shutdown of the hardware.
 *
 * @details This function performs a shutdown of the hardware.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
void boot_hsuart_close(void);


/*===========================================================================

**  Function :  boot_hsuart_read

** ==========================================================================
*/
/*!
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the receive
 *          URB is not NULL, upon which the data will be copied to the
 *          input receive buffer.
 * 
 * @param   *rx_buf     -  [IN/OUT] Receive buffer for data, must be 4 byte aligned 
 * @param   len         -  [IN    ] Length of buffer, must be multiples of max packet size
 * @param   rx_cb       -  [IN    ] Return callback function
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes receieved.
 * 
 */
uint32 boot_hsuart_read(uint8* sahara_packet_data, uint32 length, boot_hsuart_read_cb_type rx_cb, uint32 * err_code);


/*===========================================================================

**  Function :  boot_hsuart_get_max_raw_data_transfer_size

** ==========================================================================
*/
/*!
 * 
 * @brief    This function returns whether HSUART spec defined max raw data transfer sizes
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 * 
 * @return  Max data transfer sizes
 * 
 */
uint32 boot_hsuart_get_max_raw_data_transfer_size(void);


/*===========================================================================

**  Function :  boot_hsuart_get_max_packet_size

** ==========================================================================
*/
/*!
 * 
 * @brief   This function returns whether HSUART spec defined max packet sizes
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 *          
 * @return  Max packet sizes
 * 
 */
uint32 boot_hsuart_get_max_packet_size(void);


/*===========================================================================

**  Function :  boot_hsuart_poll

** ==========================================================================
*/
/*!
 * 
 * @brief   Returns the booolean result true if there is data in RX pipe, else false
 * 
 *          
 * @return  boolean
 * 
 */
boolean boot_hsuart_poll(void);


#endif /* BOOT_EXTERN_HSUART_INTERFACE_H */
