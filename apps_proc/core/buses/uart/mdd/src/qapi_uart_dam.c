
/*==================================================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.tx/6.0/buses/uart/mdd/src/qapi_uart_dam.c#4 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/04/19   ss      Added changes for qapi registration in a common place.
10/06/17   vg      Added UART DAM changes
12/14/16   rc      Initial version

==================================================================================================*/

/*==================================================================================================

                                           INCLUDE FILES
==================================================================================================*/
#include "qurt_txm_qapi_fwk.h"
#include "qapi_uart.h"
#include "comdef.h"
#include "qapi_uart_handler.h"
#include "qurt_mutex.h"
#include "qurt.h"




#define  UART_QAPI_MAGIC  0x807A


#define  UART_QAPI_ENCODE(index)   (index ^ UART_QAPI_MAGIC)

#define  UART_QAPI_DECODE(handle, index, status)                        \
    index = (uint32)handle ^ UART_QAPI_MAGIC;                            \
    if (index >= NUM_UART_DEVICES)                                    \
    {                                                                    \
        status = QAPI_ERROR;		                 \
        return status;                                                            \
    }
	
typedef struct uart_qapi_opener
{
    qurt_mutex_t        mutex;
    void                *c_handle[NUM_UART_DEVICES];
} uart_qapi_opener; 	
	
static uart_qapi_opener opener;
static boolean created_uart = FALSE;
/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

extern uart_tx_dispatcher_cb pGlobUartTxCb;
extern uart_tx_dispatcher_cb pGlobUartRxCb;

/*==================================================================================================
                                        FUNCTION DEFINITIONS
==================================================================================================*/

/**
 * Handles  the kernel switching from the user space UART API's.
 *
 * This function will be invoked by the Threadx DAM layer
 *  as part of user space to kernel space switching
 * 
 * @param[in] request_ID      request_ID passed by userspace QAPI
 * @arg1[in]  arg1            passed by userspace QAPI                         
 * @arg1[in]  arg2            passed by userspace QAPI
 * @arg1[in]  arg3            passed by userspace QAPI
 * @arg1[in]  arg4            passed by userspace QAPI
 * 
 * @return
 * QAPI_OK      Handling of requested ID is successful. \n
 * QAPI_ERROR   Handling of requested ID failed.
 * 
 */

int uart_qapi_handler(int request_ID, int arg1, int arg2, int arg3, int arg4, int arg5)
{
  qapi_Status_t result = QAPI_OK;
  
  int encoded_h;
  uart_qapi_opener *opener_ptr = &opener;
  boolean found;
  uint32 i;
  uint8 handle_idx;
  qapi_UART_Open_Config_t     damConfig;
  
   if (opener_ptr && !created_uart)
  {
     qurt_mutex_init (&opener_ptr->mutex);
     created_uart = TRUE;
  }
  
  switch(request_ID) {
    case TXM_QAPI_UART_OPEN:
    	pGlobUartTxCb = (uart_tx_dispatcher_cb) arg4; 
	    pGlobUartRxCb = (uart_rx_dispatcher_cb) arg5;

	    qurt_mutex_lock(&opener_ptr->mutex);
		found = FALSE;
        for (i = 0; i < NUM_UART_DEVICES; i++)
        {
            if (opener_ptr->c_handle[i] == NULL)
            {
                found = TRUE;
                break;
            }
        }
        if (!found)
        {
            result = QAPI_ERROR;
            qurt_mutex_unlock (&opener_ptr->mutex);
            break;
        }
        if (QURT_EOK != qurt_copy_from_user(&damConfig,
                                            sizeof(qapi_UART_Open_Config_t),
	    								    (qapi_UART_Open_Config_t*) arg3,
                                            sizeof(qapi_UART_Open_Config_t)))
	    {
	      result = QAPI_ERROR;

		  break;
        
	    }		
		
		damConfig.user_mode_client = TRUE;
        result = qapi_UART_Open(&opener_ptr->c_handle[i], (qapi_UART_Port_Id_e) arg2, &damConfig);
		if(result == QAPI_OK)
		{
			encoded_h = UART_QAPI_ENCODE(i);
            if (QURT_EOK != qurt_copy_to_user((void *)arg1,
                                   sizeof(void *),
                                   &encoded_h,
                                   sizeof(void *)))


            {
               
                result = QAPI_ERROR;
                qapi_UART_Close (opener_ptr->c_handle[i]);
                opener_ptr->c_handle[i] = NULL;
            }
		}
		qurt_mutex_unlock (&opener_ptr->mutex);
		break;
	case TXM_QAPI_UART_TRANSMIT:
	
	    UART_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_UART_Transmit(opener_ptr->c_handle[handle_idx], (char*) arg2, (uint32_t ) arg3, (void *) arg4 );
		break;
	case TXM_QAPI_UART_RECIEVE:
	    UART_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_UART_Receive(opener_ptr->c_handle[handle_idx], (char*) arg2, (uint32_t ) arg3, (void *) arg4 );
		break;		
	case TXM_QAPI_UART_POWER_ON:
	    UART_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_UART_Power_On(opener_ptr->c_handle[handle_idx]);
		break;		
	case TXM_QAPI_UART_POWER_OFF:
	    UART_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_UART_Power_Off(opener_ptr->c_handle[handle_idx]);
		break;		
	case TXM_QAPI_UART_IOCTL:
	    UART_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_UART_Ioctl(opener_ptr->c_handle[handle_idx], (qapi_UART_Ioctl_Command_e ) arg2, (void *) arg3);
		break;	
	case TXM_QAPI_UART_CLOSE:
	    UART_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_UART_Close(opener_ptr->c_handle[handle_idx]);
        if (result == QAPI_OK)
        {
            opener_ptr->c_handle[handle_idx] = NULL;
        }
            
		break;				
	default:
	    result = QAPI_ERROR;
	    break;
  }
  return result;
}
