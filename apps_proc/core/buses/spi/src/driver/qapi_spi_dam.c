
/*==================================================================================================
Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
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

$Header: //components/rel/core.tx/6.0/buses/spi/src/driver/qapi_spi_dam.c#4 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/04/19   ss      Added changes for qapi registration in a common place.
02/25/19   ss      Fixed SPI Sync Transfer Issue.
09/11/17   vg      Added spi DAM changes
02/28/17   ska     Created Initial version

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qurt_txm_qapi_fwk.h"
#include "qapi_spi_master.h"
#include "qurt_memory.h"
#include "qapi_spi_handler.h"
#include "qurt_mutex.h"
#include "qurt.h"



#define  SPI_QAPI_MAGIC  0x0511


#define  SPI_QAPI_ENCODE(index)   (index ^ SPI_QAPI_MAGIC)

#define  SPI_QAPI_DECODE(handle, index, status)                        \
    index = (uint32)handle ^ SPI_QAPI_MAGIC;                            \
    if (index >= SPI_CLIENTS_PER_DEVICE)                                    \
    {                                                                    \
        status = QAPI_SPIM_ERROR_MEM_MAP_FAILURE;		                 \
        return status;                                                            \
    }
	

typedef struct spi_qapi_opener
{
    qurt_mutex_t        mutex;
    void                *c_handle[SPI_CLIENTS_PER_DEVICE];
} spi_qapi_opener; 	

//wrapper api for spi full duplex									 
extern qapi_Status_t qapi_SPIM_Full_Duplex_user (void *spi_Handle, qapi_SPIM_Config_t *config, 
                                     qapi_SPIM_Descriptor_t *desc, uint32_t num_Descriptors,
									 qapi_SPIM_Callback_Fn_t c_Fn, void *c_Ctxt, qbool_t get_timestamp);	
	

static spi_qapi_opener opener;
static boolean created_spi = FALSE;
/*==================================================================================================
                                             ENUMERATIONS
==================================================================================================*/

int spi_qapi_handler(int request_ID, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8)
{
  qapi_Status_t result = QAPI_OK;
  void *manager_cb = NULL;
  qapi_SPIM_Descriptor_t *desc;
  int encoded_h;
  spi_qapi_opener *opener_ptr = &opener;
  boolean found;
  uint32 i;
  uint8 handle_idx;
  
  if (opener_ptr && !created_spi)
  {
     qurt_mutex_init (&opener_ptr->mutex);
     created_spi = TRUE;
  }
  
  switch(request_ID) {
	case TXM_QAPI_SPI_OPEN:
	    qurt_mutex_lock(&opener_ptr->mutex);
		found = FALSE;
        for (i = 0; i < SPI_CLIENTS_PER_DEVICE; i++)
        {
            if (opener_ptr->c_handle[i] == NULL)
            {
                found = TRUE;
                break;
            }
        }
        if (!found)
        {
            result = QAPI_SPIM_ERROR_NO_MEM_FAILURE;
            qurt_mutex_unlock (&opener_ptr->mutex);
            break;
        }
		result = qapi_SPIM_Open ((qapi_SPIM_Instance_t)arg1, &(opener_ptr->c_handle[i]));
		if(result == QAPI_OK)
		{
			encoded_h = SPI_QAPI_ENCODE(i);
            if (QURT_EOK != qurt_copy_to_user((void *)arg2,
                                   sizeof(void *),
                                   &encoded_h,
                                   sizeof(void *)))


            {
               
                result = QAPI_SPIM_ERROR_COPY_FAILURE;
                qapi_SPIM_Close (opener_ptr->c_handle[i]);
                opener_ptr->c_handle[i] = NULL;
            }
		}
		qurt_mutex_unlock (&opener_ptr->mutex);
		break;
	case TXM_QAPI_SPI_FULL_DUPLEX:
		desc = (qapi_SPIM_Descriptor_t *)arg3;
		if ((NULL == desc) || (desc->len == 0))
		{
			result = QAPI_SPIM_ERROR_INVALID_PARAM;
            break;
        }

		if(NULL  == desc->tx_buf ||
		   TRUE == qurt_check_if_kernel_address((qurt_addr_t *)desc->tx_buf) ||
		   TRUE == qurt_check_if_kernel_address((qurt_addr_t *)desc->tx_buf + desc->len) )
		{
			result = QAPI_SPIM_ERROR_INVALID_BUFFER;
			break;
		}
		if(NULL  == desc->rx_buf ||
		   TRUE == qurt_check_if_kernel_address((qurt_addr_t *)desc->rx_buf) ||
		   TRUE == qurt_check_if_kernel_address((qurt_addr_t *)desc->rx_buf + desc->len))
		{
			result = QAPI_SPIM_ERROR_INVALID_BUFFER;
			break;
		}
		if(arg5 != NULL)
		{
		    if(!(qurt_qapi_callback_configure(TXM_QAPI_SPI_CB_NOTIFY, // type
									 (void *)0xDEADCAFE, // dummy obj_ptr
									 0,     // id
									 (void *)arg5, // example_api1 - a2: app_cb
									 (void *)arg8, // example_api1 - a3: app_cb_dispatcher
									 (void**)&manager_cb,
									 NULL)))
	       {
			    if((manager_cb) == NULL)
			    {
			       result = QAPI_ERROR;
			    }
	       }
		   
		   
			SPI_QAPI_DECODE((void *)arg1,handle_idx,result);						 
		    result = qapi_SPIM_Full_Duplex_user (opener_ptr->c_handle[handle_idx], (qapi_SPIM_Config_t *) arg2, 
                                     (qapi_SPIM_Descriptor_t *)arg3, (uint32_t) arg4,
									 (qapi_SPIM_Callback_Fn_t)manager_cb, (void *)arg6, (qbool_t) arg7);
		}
		else 
		{
		    result = QAPI_ERROR;	//Returns error when client doesnot return a callback.
		}
		break;	
	case TXM_QAPI_SPI_POWER_ON:
	    SPI_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_SPIM_Power_On (opener_ptr->c_handle[handle_idx]);
		break;		
	case TXM_QAPI_SPI_POWER_OFF:
	    SPI_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_SPIM_Power_Off (opener_ptr->c_handle[handle_idx]);
		break;
	case TXM_QAPI_SPI_CLOSE:
	    SPI_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_SPIM_Close (opener_ptr->c_handle[handle_idx]);
		qurt_mutex_lock   (&opener_ptr->mutex);
            if (result == QAPI_OK)
            {
                opener_ptr->c_handle[handle_idx] = NULL;
            }
            qurt_mutex_unlock (&opener_ptr->mutex);
        break;				
	default:
	    result = QAPI_ERROR;
	    break;
  }
  return result;
}
