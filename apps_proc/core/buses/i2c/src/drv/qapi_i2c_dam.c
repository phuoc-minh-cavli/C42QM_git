
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
$Header: //components/rel/core.tx/6.0/buses/i2c/src/drv/qapi_i2c_dam.c#4 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/04/19   ss      Added changes for qapi registration in a common place.
09/11/17   vg      Added I2C DAM changes
03/02/17   vg      Created Initial version

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qurt_txm_qapi_fwk.h"
#include "qapi_i2c_master.h"
#include "qurt_memory.h"
#include "qapi_i2c_handler.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "qurt.h"


#define  I2C_QAPI_MAGIC  0x012C


#define  I2C_QAPI_ENCODE(index)   (index ^ I2C_QAPI_MAGIC)

#define  I2C_QAPI_DECODE(handle, index, status)                        \
    index = (uint32)handle ^ I2C_QAPI_MAGIC;                            \
    if (index >= I2C_CLIENTS_PER_DEVICE)                                    \
    {                                                                    \
        status = QAPI_I2CM_ERROR_MEM_MAP_FAILURE;		                 \
        return status;                                                            \
    }
	
typedef struct i2c_qapi_opener
{
    qurt_mutex_t        mutex;
    void                *c_handle[I2C_CLIENTS_PER_DEVICE];
} i2c_qapi_opener; 
	
static i2c_qapi_opener opener;
static boolean created_i2c = FALSE;

//wrapper api for i2c transfer

extern qapi_Status_t
qapi_I2CM_Transfer_user
(
    void *i2c_Handle,
    qapi_I2CM_Config_t *config,
    qapi_I2CM_Descriptor_t *desc,
    uint16_t num_Descriptors,
    qapi_I2CM_Transfer_CB_t CB_Function,
    void *CB_Parameter,
    uint32_t delay_us
);

/*==================================================================================================
                                             FUNCTION DECLARATION
==================================================================================================*/

int i2c_qapi_handler(int request_ID, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8)
{
  qapi_Status_t result = QAPI_OK;
  void *call_back_mgr;
  qapi_I2CM_Descriptor_t *desc = NULL;
  int encoded_h;
  i2c_qapi_opener *opener_ptr = &opener;
  boolean found;
  uint32 i;
  uint8 handle_idx;
  
  if (opener_ptr && !created_i2c)
  {
     qurt_mutex_init (&opener_ptr->mutex);
     created_i2c = TRUE;
  }
  
  switch(request_ID) {
    case TXM_QAPI_I2C_OPEN:
        qurt_mutex_lock(&opener_ptr->mutex);
		found = FALSE;
        for (i = 0; i < I2C_CLIENTS_PER_DEVICE; i++)
        {
            if (opener_ptr->c_handle[i] == NULL)
            {
                found = TRUE;
                break;
            }
        }
        if (!found)
        {
            result = QAPI_I2CM_ERROR_NO_MEM_FAILURE;
            qurt_mutex_unlock (&opener_ptr->mutex);
            break;
        }
		result = qapi_I2CM_Open ((qapi_I2CM_Instance_t)arg1, &(opener_ptr->c_handle[i]));
		if(result == QAPI_OK)
		{
			encoded_h = I2C_QAPI_ENCODE(i);
            if (QURT_EOK != qurt_copy_to_user((void *)arg2,
                                   sizeof(void *),
                                   &encoded_h,
                                   sizeof(void *)))


            {
               
                result = QAPI_I2CM_ERROR_COPY_FAILURE;
                qapi_I2CM_Close (opener_ptr->c_handle[i]);
                opener_ptr->c_handle[i] = NULL;
            }
		}
		qurt_mutex_unlock (&opener_ptr->mutex);
		break;
	case TXM_QAPI_I2C_TRANSFER:
	    
		desc = (qapi_I2CM_Descriptor_t *)arg3;
		if ((NULL == desc) || (desc->length == 0))
		{
			result = QAPI_I2CM_ERR_INVALID_PARAMETER;
            break;
        }
		
	    if(!(qurt_qapi_callback_configure(TXM_QAPI_I2C_NOTIFY_CALLBACK, // type
		                             (void *)arg6, 
									 0,              // id
									 (void *)arg5,  // example_api1 - a2: app_cb
									 (void *)arg8,  // example_api1 - a3: app_cb_dispatcher
									 &call_back_mgr, 
									 NULL)))
	    {
			if((call_back_mgr) == NULL)
			{
			  result = QAPI_ERROR;
			}
	    }					

		if(desc->buffer == NULL || 
		   TRUE == qurt_check_if_kernel_address((qurt_addr_t *)(desc->buffer)) || 
		   TRUE == qurt_check_if_kernel_address(((qurt_addr_t *)((desc->buffer) + desc->length))))
		{
			result = QAPI_I2CM_ERR_INVALID_BUFFER_POINTER;
			break;
		}
		
		I2C_QAPI_DECODE((void *)arg1,handle_idx,result);
		
		result = qapi_I2CM_Transfer_user (opener_ptr->c_handle[handle_idx], (qapi_I2CM_Config_t *) arg2, 
                                     (qapi_I2CM_Descriptor_t *)arg3, (uint32_t) arg4,
								     (qapi_I2CM_Transfer_CB_t)call_back_mgr, 
									 (void *)arg6, (uint32_t) arg7);             
		break;	
	case TXM_QAPI_I2C_POWER_ON:
	    I2C_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_I2CM_Power_On (opener_ptr->c_handle[handle_idx]);
		break;		
	case TXM_QAPI_I2C_POWER_OFF:
	    I2C_QAPI_DECODE((void *)arg1,handle_idx,result);
		result = qapi_I2CM_Power_Off (opener_ptr->c_handle[handle_idx]);
		break;
	case TXM_QAPI_I2C_CLOSE:
	     I2C_QAPI_DECODE((void *)arg1,handle_idx,result);
            result = qapi_I2CM_Close (opener_ptr->c_handle[handle_idx]);
            
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
