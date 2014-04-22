
/*======================================================================
                        qapi_adc.c

GENERAL DESCRIPTION
  Contains Kernel level QAPIs for ADC

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2015-2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2018-01-30   AY      Updated for MDM9205
 * 2017-11-06   KS      QAPI Custon callback changes.
 * 2017-04-05   KS      QAPI changes.
 * 2017-03-13   PC      Create.
 ======================================================================*/
// This should be on top. Do not move

#ifdef QAPI_TXM_SOURCE_CODE
#include "qapi_adc.h"
#include "qurt_txm_qapi_fwk.h"
#include "DALStdErr.h"
#include "DDIAdc.h"
#include "../../adc/dal/src/DalAdc.h"
#include "DALDeviceId.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "qurt.h"
/*======================================================================
                          EXTERNAL
 ======================================================================*/
/*======================================================================
                          MACROS
 ======================================================================*/
#define  ADC_QAPI_MAGIC  0x0599
#define  ADC_CLIENTS      10

#define  ADC_QAPI_ENCODE(index)   (index ^ ADC_QAPI_MAGIC)

#define  ADC_QAPI_DECODE(index)                                 \
    index = (uint32)index ^ ADC_QAPI_MAGIC;                     \
    if (index >= ADC_CLIENTS)                                    \
    {                                                           \
        return (UINT)QAPI_ERR_NOT_SUPPORTED;		                    \
                                                                \
    }
	


/*======================================================================
                          TYPEDEFS
 ======================================================================*/
typedef struct adc_qapi_opener
{
    qurt_mutex_t        mutex;
    void                *c_handle[ADC_CLIENTS];
} adc_qapi_opener; 

static adc_qapi_opener opener;
static boolean created_adc = FALSE;
/*======================================================================
                          GLOBALS
 ======================================================================*/

/*======================================================================
                          FUNCTIONS
=======================================================================*/

/* Function to invoke ADC APIs based on command ID*/
UINT qapi_ADC_Handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e,
                      UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
   UINT retval = 0xffffffff;
   void  *call_back_mgr_1, *call_back_mgr_2;
   qapi_AdcTM_Request_Params_Type_t *param_cb ;
   uint32_t en_decoded_h,client;
   adc_qapi_opener *opener_ptr = &opener;
   boolean found;
   AdcInputPropertiesType nInput_Prop_Ptr;
   AdcResultType nResult_Ptr;
   char *pChannel_Name_Ptr;
  
   if (!created_adc)
   {
       qurt_mutex_init (&opener_ptr->mutex);
       created_adc = TRUE;
   }
  
   switch(id)
   {

   case TXM_QAPI_ADC_OPEN:
         if ((a == NULL))
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        qurt_mutex_lock(&opener_ptr->mutex);
		found = FALSE;
        for (client = 0; client < ADC_CLIENTS; client++)
        {
            if (opener_ptr->c_handle[client] == NULL)
            {
                found = TRUE;
                break;
            }
        }
        if (!found)
        {
            retval = (UINT)QAPI_ERR_NOT_SUPPORTED;
            qurt_mutex_unlock (&opener_ptr->mutex);
            break;
        }
		if(client >= ADC_CLIENTS)
			client = ADC_CLIENTS -1;
        retval = qapi_ADC_Open((qapi_ADC_Handle_t *)&(opener_ptr->c_handle[client]),(uint32_t)b);
        if(retval == QAPI_OK)
		{
			en_decoded_h = ADC_QAPI_ENCODE(client);
            if (QURT_EOK != qurt_copy_to_user((qurt_addr_t *)*((qapi_ADC_Handle_t *)a),
                                   sizeof(uint32_t),
                                   (qurt_addr_t *)&en_decoded_h,
                                   sizeof(uint32_t)))


            {
               
                retval = (UINT)QAPI_ERR_INVALID_PARAM;
                opener_ptr->c_handle[client] = NULL;
            }
		}
		qurt_mutex_unlock (&opener_ptr->mutex);
      break;

   case TXM_QAPI_ADC_GET_INPUT_PROP:
        if ((a == NULL) || (b == NULL) || (c == NULL) || (c > 30)|| (d == NULL))
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        if( QURT_EOK != qurt_copy_from_user((qurt_addr_t *)&en_decoded_h, (qurt_size_t)sizeof(uint32_t),
                (qurt_addr_t *)a, (qurt_size_t)sizeof(uint32_t)))
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        
        ADC_QAPI_DECODE(en_decoded_h);
        pChannel_Name_Ptr = (char *)malloc(c);
        if( QURT_EOK != qurt_copy_from_user((qurt_addr_t *)pChannel_Name_Ptr, (qurt_size_t)c,
                (qurt_addr_t *)b, (qurt_size_t)c))
        {
            free(pChannel_Name_Ptr);
            return (UINT)QAPI_ERR_INVALID_PARAM;
        } 

      retval = qapi_ADC_Get_Input_Properties((qapi_ADC_Handle_t)(opener_ptr->c_handle[en_decoded_h]) ,
                                             (const char *)pChannel_Name_Ptr,
                                             (uint32_t)c,
                                             (qapi_Adc_Input_Properties_Type_t *)&nInput_Prop_Ptr);
        if(retval == QAPI_OK)
		{
            retval = qurt_copy_to_user((qurt_addr_t *)d, (qurt_size_t)sizeof(AdcInputPropertiesType),
                    (qurt_addr_t *)&nInput_Prop_Ptr, (qurt_size_t)sizeof(AdcInputPropertiesType));
        }
        free(pChannel_Name_Ptr);
      break;

   case TXM_QAPI_ADC_READ_CHNL:

      if ((a == NULL) || (b == NULL) || (c == NULL))
      {
         return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      if( QURT_EOK != qurt_copy_from_user((qurt_addr_t *)&en_decoded_h, (qurt_size_t)sizeof(uint32_t),
              (qurt_addr_t *)a, (qurt_size_t)sizeof(uint32_t)))
      {
          return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      ADC_QAPI_DECODE(en_decoded_h);
      if( QURT_EOK != qurt_copy_from_user((qurt_addr_t *)&nInput_Prop_Ptr, (qurt_size_t)sizeof(AdcInputPropertiesType),
              (qurt_addr_t *)b, (qurt_size_t)sizeof(AdcInputPropertiesType)))
      {
          return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      
      retval = qapi_ADC_Read_Channel((qapi_ADC_Handle_t)opener_ptr->c_handle[en_decoded_h],
                                     (qapi_Adc_Input_Properties_Type_t *)&nInput_Prop_Ptr,
                                     (qapi_ADC_Read_Result_t *)&nResult_Ptr);
      if( QURT_EOK != qurt_copy_to_user((qurt_addr_t *)c, (qurt_size_t)sizeof(AdcResultType),
              (qurt_addr_t *)&nResult_Ptr, (qurt_size_t)sizeof(AdcResultType)))
      {
          return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      break;

   case TXM_QAPI_ADC_TM_GET_INPUT_PROP:
      retval = qapi_ADC_TM_Get_Input_Properties((qapi_ADC_Handle_t)a ,
                                             (const char *)b,
                                             (uint32_t)c,
                                             (qapi_AdcTM_Input_Properties_Type_t *)d);
      break;

   case TXM_QAPI_ADC_GET_RANGE:
      retval = qapi_ADC_Get_Range((qapi_ADC_Handle_t)a,
                                     (qapi_AdcTM_Input_Properties_Type_t *)b,
                                     (qapi_AdcTM_Range_Type_t *)c);
      break;

   case TXM_QAPI_ADC_SET_AMP_THRESHOLD:
		param_cb = (qapi_AdcTM_Request_Params_Type_t  *)b;
		
        if(!(qurt_qapi_callback_configure(TXM_QAPI_ADC_AMP_THRESHOLD_CB, 
		                                 (void *)0xDEADCAFE, 
									     0,                 
										 (void *)param_cb->pfnAdcTMThresholdCb,
									     (void *)f,
									     &call_back_mgr_1, 
									     NULL)))
	    {
			if((call_back_mgr_1) == NULL)
			{
			  return retval;
			}
	    }
	    param_cb->pfnAdcTMThresholdCb = (qapi_AdcTM_Threshold_Cb_Type)call_back_mgr_1;
		
      retval = qapi_ADC_Set_Amp_Threshold((qapi_ADC_Handle_t)a ,
                                             (const qapi_AdcTM_Request_Params_Type_t  *)param_cb,
                                             (qapi_ADC_Amp_Threshold_t)c,
                                             (const int32_t *)d,
                                             (int32_t *)e);
      break;
	  
   case TXM_QAPI_ADC_TM_ENABLE_THRESHOLDS:
      retval = qapi_ADC_TM_Enable_Thresholds((qapi_ADC_Handle_t)a,
                                         (uint32_t)b, (qapi_ADC_Amp_Threshold_t)c);
      break;

   case TXM_QAPI_ADC_TM_SET_TOLERANCE:
       param_cb = (qapi_AdcTM_Request_Params_Type_t  *)b;
	   
       if(!(qurt_qapi_callback_configure(TXM_QAPI_ADC_SET_TOLERENCE_CB, 
		                                (void *)0xDEADCAFE, 
									    0,                 
										(void *)param_cb->pfnAdcTMThresholdCb,						        
									    (void *)e,         
									    &call_back_mgr_2, 
									    NULL)))
	    {
			if((call_back_mgr_2) == NULL)
			{
			  return retval;
			}
	    }
	    param_cb->pfnAdcTMThresholdCb = (qapi_AdcTM_Threshold_Cb_Type)call_back_mgr_2;
		
      retval = qapi_ADC_TM_Set_Tolerance((qapi_ADC_Handle_t)a ,
                                             (const qapi_AdcTM_Request_Params_Type_t  *)param_cb,
                                             (const int32_t *)c,
                                             (const int32_t *)d);
      break;

   case TXM_QAPI_ADC_CLOSE:
        if (a == NULL) 
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        if( QURT_EOK != qurt_copy_from_user((qurt_addr_t *)&en_decoded_h, (qurt_size_t)sizeof(uint32_t),
                (qurt_addr_t *)a, (qurt_size_t)sizeof(uint32_t)))
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        ADC_QAPI_DECODE(en_decoded_h);
        qurt_mutex_lock(&opener_ptr->mutex);
       
        retval = qapi_ADC_Close((qapi_ADC_Handle_t)opener_ptr->c_handle[en_decoded_h],(qbool_t)b);
        if (retval == QAPI_OK)
        {
            opener_ptr->c_handle[en_decoded_h] = NULL;
        }
        qurt_mutex_unlock (&opener_ptr->mutex);
      
      break;

   default:
      break;
   }

   return retval;
}

/* Function to convert ADC internal error status to generic status*/
qapi_Status_t  qapi_ADC_Convert_Status(int32_t status)
{
   switch (status)
   {
      case DAL_SUCCESS:
         return QAPI_OK;

      case DAL_ERROR_INVALID_HANDLE:
      case DAL_ERROR_INVALID_POINTER:
      case DAL_ERROR_INVALID_ADDRESS:
      case ADC_ERROR_NULL_POINTER:
      case ADC_ERROR_INVALID_PROPERTY_LENGTH :
         return QAPI_ERR_INVALID_PARAM;

      case DAL_ERROR_INSUFFICIENT_MEMORY:
         return QAPI_ERR_NO_MEMORY;

      case DAL_ERROR_NOT_SUPPORTED:
         return QAPI_ERR_NOT_SUPPORTED;

      case DAL_ERROR_BUSY_RESOURCE:
         return QAPI_ERR_BUSY;

      case ADC_ERROR_INVALID_DEVICE_IDX :
          return QAPI_ERR_INVALID_IDX;

      case ADC_ERROR_INVALID_CHANNEL_IDX :
          return QAPI_ERR_INVALID_CHANNEL_IDX;

      case ADC_ERROR_DEVICE_QUEUE_FULL :
          return QAPI_ERR_QUEUE_FULL;

      case ADC_ERROR_OUT_OF_TM_CLIENTS :
          return QAPI_ERR_OUT_OF_TM_CLIENTS;

      case ADC_ERROR_TM_THRESHOLD_OUT_OF_RANGE :
          return QAPI_ERR_TM_THRESHOLD_OUT_OF_RANGE;

      case ADC_ERROR_TM_BUSY :
          return QAPI_ERR_TM_BUSY;


      default:
         return QAPI_ERROR;
   }
}



/* API for Opening ADC */
qapi_Status_t qapi_ADC_Open(qapi_ADC_Handle_t *Handle, uint32_t Attributes)
{
   DALResult status;

   status = DAL_AdcDeviceAttach(DALDEVICEID_ADC, (DalDeviceHandle **)Handle);

   return qapi_ADC_Convert_Status(status);
}

/* API for closing ADC */
qapi_Status_t qapi_ADC_Close(qapi_ADC_Handle_t Handle,qbool_t keep_enabled)
{
   DALResult status = QAPI_OK;
	
   if (Handle == NULL)
   {
      return QAPI_ERR_INVALID_PARAM;
   }
   //Dummy function
   return qapi_ADC_Convert_Status(status);
}

/* API for reading an ADC channel */
qapi_Status_t qapi_ADC_Read_Channel(qapi_ADC_Handle_t Handle,
                                         const qapi_Adc_Input_Properties_Type_t *Input_Prop_Ptr,
                                         qapi_ADC_Read_Result_t *Result_Ptr)
{
   DALResult status;

   if ((Handle == NULL) || (Input_Prop_Ptr == NULL) || (Result_Ptr == NULL))
   {
      return QAPI_ERR_INVALID_PARAM;
   }

   status = DalAdc_Read((DalDeviceHandle *)Handle,
                        (const AdcInputPropertiesType *)Input_Prop_Ptr,
                        (AdcResultType *)Result_Ptr);

   return qapi_ADC_Convert_Status(status);
}

/* API to get properties of ADC channels */
qapi_Status_t qapi_ADC_Get_Input_Properties(qapi_ADC_Handle_t Handle,
                                                 const char *Channel_Name_Ptr,
                                                 uint32_t Channel_Name_Size,
                                                 qapi_Adc_Input_Properties_Type_t *Properties_Ptr)
{
   DALResult status;

   if ((Handle == NULL) || (Channel_Name_Ptr == NULL) ||
         (Channel_Name_Size == NULL) || (Properties_Ptr == NULL))
   {
      return QAPI_ERR_INVALID_PARAM;
   }

   status = Adc_GetAdcInputProperties(((DalDeviceHandle *)Handle)->pClientCtxt,
                                      Channel_Name_Ptr,
                                      Channel_Name_Size,
                                      (AdcInputPropertiesType *)Properties_Ptr);

   return qapi_ADC_Convert_Status(status);
}


/* API to get properties of ADC TM channels */
qapi_Status_t qapi_ADC_TM_Get_Input_Properties(qapi_ADC_Handle_t Handle,
                                                   const char *Channel_Name_Ptr,
                                                   uint32_t Channel_Name_Size,
                                                   qapi_AdcTM_Input_Properties_Type_t *Properties_Ptr)
{
  
   return QAPI_ERR_NOT_SUPPORTED;
}


/* API to get range of ADC channel */
qapi_Status_t qapi_ADC_Get_Range(qapi_ADC_Handle_t Handle,
                                        const qapi_AdcTM_Input_Properties_Type_t *In_Properties_Ptr,
                                        qapi_AdcTM_Range_Type_t *ADC_TM_Range_Ptr)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/* API to set threshold on ADC TM channels */
qapi_Status_t qapi_ADC_Set_Amp_Threshold(qapi_ADC_Handle_t Handle,
                                             const qapi_AdcTM_Request_Params_Type_t  * ADC_TM_Params_Ptr,
                                             qapi_ADC_Amp_Threshold_t Threshold_Type,
                                             const int32_t *Threshold_Desired_Ptr,
                                             int32_t *TM_Threshold_Set_Ptr)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/* API to enable ADC TM thresholds */
qapi_Status_t qapi_ADC_TM_Enable_Thresholds(qapi_ADC_Handle_t Handle,
                                                 uint32_t Enable, qapi_ADC_Amp_Threshold_t Threshold_Type)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/* API to set tolerance on ADC TM channel */
qapi_Status_t qapi_ADC_TM_Set_Tolerance(
      qapi_ADC_Handle_t Handle,
      const qapi_AdcTM_Request_Params_Type_t  * TM_Params_Type_Ptr,
      const int32_t *Lower_Tolerance_Ptr,
      const int32_t *Higher_Tolerance_Ptr)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

#endif
