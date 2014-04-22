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
#include "boot_shared_functions_consumer.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
static boot_boolean data_received = FALSE;
static void boot_hsuart_clean_buff(uint8* sahara_packet_data, uint32 length, uint32 * err_code);

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
boolean boot_hsuart_initialize(void)
{
	HSUART_CONFIG *cf = NULL;
	if(DAL_SUCCESS != DALSYS_Malloc(sizeof(HSUART_CONFIG), (void *)&cf))
	{
		return(FALSE);
	}
	else
	{
	    cf->enable_loopback = 0;
        cf->baud_rate = 3000000;
        cf->enable_flow_control = TRUE;

		return hsuart_initialize(cf);
	}
}


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
uint32 boot_hsuart_write(uint8* sahara_packet_data, uint32 length, sahara_tx_cb_type tx_cb, uint32 * err_code)
{
	return hsuart_write(sahara_packet_data, length);
}	


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
void boot_hsuart_close(void)
{
	hsuart_close();
}


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
uint32 boot_hsuart_read(uint8* sahara_packet_data, uint32 length, boot_hsuart_read_cb_type rx_cb, uint32 * err_code)
{
	uint32 bytes_read, bytes_left, temp_bytes_read;
	uint32 num_of_tries = 0;
	
	while(!(boot_hsuart_poll()))
	{
	}
	bytes_left = length;
	bytes_read = 0;
	
	while((bytes_read < length) && (num_of_tries < UART_PACKET_READ_MAX_RETRIES))
	{
	  
	  temp_bytes_read = 0;	
	  temp_bytes_read = hsuart_read(sahara_packet_data + bytes_read, bytes_left, err_code);
	  if(*err_code != BULK_SUCCESS && length != 1)
	  {
		  boot_hsuart_clean_buff(sahara_packet_data, length, err_code);
		  if(rx_cb != NULL)
		  {
			  rx_cb(err_code);
		  }
		  memset(sahara_packet_data, 0, length);
		  bytes_read = 0;
		  bytes_left = length;
		  num_of_tries = 0;
		  data_received = FALSE;
		  continue;
	  }
	  else
	  {
		  if(temp_bytes_read != 0)
		  {
			  data_received = TRUE;
		  }
	  }
	  
	  bytes_read += temp_bytes_read;
	  bytes_left -= temp_bytes_read;
	  
	  if(data_received == TRUE)
	    num_of_tries += 1;	    
	}
	data_received = FALSE;
	*err_code = 0;
	return(bytes_read);
}	


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
uint32 boot_hsuart_get_max_raw_data_transfer_size(void)
{
	return hsuart_max_raw_data_transfer_size();
}	


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
uint32 boot_hsuart_get_max_packet_size(void)
{
	return hsuart_get_max_packet_size();
}	


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
boolean boot_hsuart_poll(void)
{
	uint32 poll_count = 0;
	boolean poll_state = FALSE;
	
	do{
		poll_state = hsuart_poll();
		if (poll_state == TRUE)
		{
			break;
		}
		busywait (UART_PACKET_POLL_PAUSE_TIME);
		poll_count++;
	}while((poll_count) < (UART_PACKET_POLL_MAX_LOOP_COUNT));
			
	return (poll_state); 
}

static void boot_hsuart_clean_buff(uint8* sahara_packet_data, uint32 length, uint32 * err_code)
{
	uint32 err_code_temp, bytes_read = 0;
	do
	{
		busywait(UART_PACKET_POLL_PAUSE_TIME);
		bytes_read = hsuart_read(sahara_packet_data, length, &err_code_temp);
	} while(bytes_read != 0);
}