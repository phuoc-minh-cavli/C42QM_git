
/*==================================================================================
                 Copyright (c) 2017 Qualcomm Technologies, Inc.
                          All Rights Reserved.
            Qualcomm Technologies, Inc. Confidential and Proprietary.
==================================================================================*/

/*==================================================================================

                             EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file. Notice that
changes are listed in reverse chronological order.

$Header: //components/dev/core.tx/1.0/c_vsarid.core.tx.1.0.tx_011117/buses/i2c/src/drv/I2cQapiDriver.c

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
04/11/17   vg     gpio & clock enbalement added in qapi_poweron_off
01/11/17   vg     Initial version

==================================================================================*/
#include "I2cDevice.h"
#include "I2cSys.h"
#include "ddii2c.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "qapi_i2c_master.h"
#include "qapi_status.h"
#include "qapi_i2c_handler.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "qurt_mutex.h"


#define I2C_WAIT_FOR_IDLE_TIMEOUT_US      2500

 #define QAPI_I2CDEVID_TO_DALID(A)                                      \
        ((A) == QAPI_I2CM_INSTANCE_001_E)? DALDEVICEID_I2C_DEVICE_1 :   \
        ((A) == QAPI_I2CM_INSTANCE_002_E)? DALDEVICEID_I2C_DEVICE_2 :   \
        ((A) == QAPI_I2CM_INSTANCE_003_E)? DALDEVICEID_I2C_DEVICE_3 :   \
        ((A) == QAPI_I2CM_INSTANCE_004_E)? DALDEVICEID_I2C_DEVICE_4 :   \
        ((A) == QAPI_I2CM_INSTANCE_005_E)? DALDEVICEID_I2C_DEVICE_5 :   \
        ((A) == QAPI_I2CM_INSTANCE_006_E)? DALDEVICEID_I2C_DEVICE_6 :   \
        ((A) == QAPI_I2CM_INSTANCE_007_E)? DALDEVICEID_I2C_DEVICE_7 :   \
        ((A) == QAPI_I2CM_INSTANCE_008_E)? DALDEVICEID_I2C_DEVICE_8 :   \
        ((A) == QAPI_I2CM_INSTANCE_009_E)? DALDEVICEID_I2C_DEVICE_9 :   \
        ((A) == QAPI_I2CM_INSTANCE_010_E)? DALDEVICEID_I2C_DEVICE_10 :  \
        ((A) == QAPI_I2CM_INSTANCE_011_E)? DALDEVICEID_I2C_DEVICE_11 :  \
        ((A) == QAPI_I2CM_INSTANCE_012_E)? DALDEVICEID_I2C_DEVICE_12 :  \
        QAPI_I2CM_INSTANCE_MAX_E


#define I2CDRV_VALIDATE_ASYNC_PARAMS(i2c_handle,config,desc,c_fn,c_ctxt)\
     ( (NULL == i2c_handle)?           QAPI_I2CM_ERR_INVALID_DEV_HANDLE:           \
       (NULL == config)?               QAPI_I2CM_ERR_INVALID_CONFIG_POINTER:     \
       (NULL == desc)?                 QAPI_I2CM_ERR_INVALID_DESC_POINTER:  \
       (NULL == c_fn)?                 QAPI_I2CM_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == c_ctxt)?               QAPI_I2CM_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2C_RES_SUCCESS                                                    \
     )

     
#define I2C_QAPI_SUCCESS(x)  (x == QAPI_OK)
#define I2C_QAPI_ERROR(x)    (x != QAPI_OK)

/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/

qapi_Status_t
qapi_I2CM_Open
(
    qapi_I2CM_Instance_t instance,
    void **i2c_Handle
)
{   
   DALResult result = DAL_SUCCESS;
   qapi_Status_t qapi_status = QAPI_OK;
   DalDeviceHandle *hDalDevice = NULL;
   DALDEVICEID i2cBusId;
   
   do{
      
      if (instance >= QAPI_I2CM_INSTANCE_MAX_E || instance <= 0 || i2c_Handle == NULL)
		{
			qapi_status = (qapi_Status_t)QAPI_I2CM_ERR_UNSUPPORTED_CORE_INSTANCE;
			break;
		}
      
      i2cBusId = QAPI_I2CDEVID_TO_DALID(instance);

	   result = DAL_I2CDeviceAttach(i2cBusId, &hDalDevice);
      if(DAL_SUCCESS != result)
      {
         qapi_status = QAPI_I2CM_ERR_HANDLE_ALLOCATION;
         break;
      }
      
      *i2c_Handle = (void **)hDalDevice;
      
      result = DalDevice_Open(hDalDevice, DAL_OPEN_SHARED);
      if(DAL_SUCCESS != result)
      {
         qapi_status = QAPI_I2CM_ERROR_OPEN_FAILURE;
         DAL_DeviceDetach(hDalDevice);
         break;
      }
   }while(0);      
  
  	if (qapi_status != result)
	{
	 /* Clean up everything that we can. Ignore the return value from
	  * qapi_I2CM_Open() as we want to return the reason the qapi_I2CM_Open()
	  * failed */
	 (void)qapi_I2CM_Close(hDalDevice);
    
	}
   
   return result;
}

qapi_Status_t
qapi_I2CM_Close
(
    void *i2c_Handle
)
{
   DALResult result = DAL_SUCCESS;
   qapi_Status_t qapi_status = QAPI_OK;
   DalDeviceHandle *hDalDevice = NULL;
   
   if(i2c_Handle != NULL)
   { 
      hDalDevice = i2c_Handle;
      
      qapi_status = DalDevice_Close(hDalDevice);
      if(DAL_SUCCESS != result)
      {
        qapi_status = QAPI_I2CM_ERROR_CLOSE_FAILURE;
      }
      
      qapi_status = DAL_DeviceDetach(hDalDevice);
      if(DAL_SUCCESS != qapi_status)
      {
         qapi_status = QAPI_I2CM_ERROR_PLATFORM_DETACH_FAILURE;
      }
   }
   
    return  qapi_status;
}

qapi_Status_t
qapi_I2CM_Transfer_user
(
    void *i2c_Handle,
    qapi_I2CM_Config_t *config,
    qapi_I2CM_Descriptor_t *desc,
    uint16_t num_Descriptors,
    qapi_I2CM_Transfer_CB_t CB_Function,
    void *CB_Parameter,
	 uint32 delay_us
)
{
	qapi_I2CM_Config_t   damConfig;
	qapi_I2CM_Descriptor_t damDesc;
	qapi_Status_t result = QAPI_OK;
	
	
	if (QURT_EOK != qurt_copy_from_user(&damConfig,
                                        sizeof(qapi_I2CM_Config_t),
									    config,
                                        sizeof(qapi_I2CM_Config_t)))
	{
	 result = (qapi_Status_t)QAPI_I2CM_ERROR_COPY_FAILURE;
	 return result;
	} 
	
	if (QURT_EOK != qurt_copy_from_user(&damDesc,
                                        sizeof(qapi_I2CM_Descriptor_t),
									    desc,
                                        sizeof(qapi_I2CM_Descriptor_t)))
	{
	 result = (qapi_Status_t)QAPI_I2CM_ERROR_COPY_FAILURE;
	 return result;
	} 
	
	
	if (TRUE != qurt_check_if_module_address((qurt_addr_t)damDesc.buffer, damDesc.length))
	{
	  result = (qapi_Status_t)QAPI_I2CM_ERROR_COPY_FAILURE;
	  return result;
	}
	
	result = qapi_I2CM_Transfer(i2c_Handle, &damConfig, &damDesc, num_Descriptors, CB_Function, CB_Parameter, delay_us);
	
	return result;

	
}

qapi_Status_t
qapi_I2CM_Transfer
(
    void *i2c_Handle,
    qapi_I2CM_Config_t *config,
    qapi_I2CM_Descriptor_t *desc,
    uint16_t num_Descriptors,
    qapi_I2CM_Transfer_CB_t CB_Function,
    void *CB_Parameter,
	 uint32 delay_us
)
{
  
   DalDeviceHandle    *hDalDevice;  
   I2cTransfer        pTransfer;
   I2cBuffDesc        pI2cBuffDesc;
   I2cClientConfig    pClntCfg; 
   I2cIoResult        pIoRes;
   qapi_Status_t      res = QAPI_OK;
    
	I2cTransferConfig	  tranCfg;		   /**< Transfer configuration. */
	I2cTranDirection 	 eTranDirection;  /**< Transfer direction. */
	I2cTranCtxt			 eTranCtxt ;	  /**< Transfer context. */
	
  
    res= I2CDRV_VALIDATE_ASYNC_PARAMS(i2c_Handle,config,desc,CB_Function,CB_Parameter);
	if ( QAPI_OK != res ) {
      return res;
    }

	hDalDevice = i2c_Handle;
	 
    res = I2CSYS_Memset(&pTransfer, 0, sizeof(I2cTransfer));
    if ( I2C_RES_SUCCESS != res )  {
       I2CSYS_Free(&pTransfer);     
       return res;
    }

    res = I2CSYS_Memset(&pI2cBuffDesc, 0, sizeof(I2cBuffDesc));
    if ( I2C_RES_SUCCESS != res )  {
       I2CSYS_Free(&pI2cBuffDesc);     
       return res;
    }

   
    res = I2CSYS_Memset(&pClntCfg, 0, sizeof(I2cClientConfig));
    if ( I2C_RES_SUCCESS != res )  {
       I2CSYS_Free(&pClntCfg);     
       return res;
    }

    res = I2CSYS_Memset(&pIoRes, 0, sizeof(I2cIoResult));
    if ( I2C_RES_SUCCESS != res )  {
       I2CSYS_Free(&pIoRes);     
       return res;
    }

        // we use clock stretch timeout to wait for bus idle, so have a default
    if (config->slave_Max_Clock_Stretch_Us == 0)
    {
        config->slave_Max_Clock_Stretch_Us = 2500;
    }
    
   //config->SMBUS_Mode =0;
   
	
	


   pI2cBuffDesc.pBuff = desc->buffer;
   pI2cBuffDesc.uBuffSize = desc->length;  

   pTransfer.pI2cBuffDesc =&pI2cBuffDesc;
   
   tranCfg.uSlaveAddr   = config->slave_Address;

   pTransfer.tranCfg = tranCfg ;
   
   pTransfer.eTranCtxt = I2cTrCtxNotASequence;
   pTransfer.uTrSize = desc->length; 
   
   pClntCfg.uByteTransferTimeoutUs =config->slave_Max_Clock_Stretch_Us;
   pClntCfg.uBusFreqKhz =config->bus_Frequency_KHz;
   pClntCfg.uBusSCLNR = config->core_Configuration1;
   pClntCfg.uBusSDANR = config->core_Configuration2;

	

    if (desc->flags & QAPI_I2C_FLAG_READ)
    {
      pTransfer.eTranDirection=I2cTranDirIn;

	  res= DalI2C_AsyncRead(hDalDevice,
	  	                 &pTransfer,
                       &pClntCfg,
                       &pIoRes,
                       (DDIDRV_ASYNC_CB)CB_Function,
                       CB_Parameter);
	  return res;
    }    
    else if (desc->flags & QAPI_I2C_FLAG_WRITE)
    {

	 pTransfer.eTranDirection=I2cTranDirOut;
	 
     res = DalI2C_AsyncWrite(hDalDevice,
	 	                 &pTransfer,
                       &pClntCfg,
                       &pIoRes,
                       (DDIDRV_ASYNC_CB)CB_Function,
                       CB_Parameter);
	 return res;
	 
    }else
    {
        return QAPI_I2CM_ERR_INVALID_PARAMETER;
    }

}

qapi_Status_t qapi_I2CM_Power_On(void *i2c_Handle)
{

  DalDeviceHandle *hDalDevice;
  qapi_Status_t qapi_status = QAPI_OK;
 
  do{
     
        if(!i2c_Handle)
        { 
            qapi_status = QAPI_I2CM_ERR_INVALID_PARAMETER;
            break;
        }
        
        hDalDevice =i2c_Handle;
        
        if(DAL_SUCCESS != DalDevice_Open(hDalDevice, DAL_OPEN_SHARED))
        {
            qapi_status = QAPI_I2CM_ERROR_POWER_ON_FAILURE;
            break;
        }
      
  }while(0);
  
   if (qapi_status != QAPI_OK)
	{
	 /* Clean up everything that we can. Ignore the return value from
	  * qapi_I2CM_Power_Off() as we want to return the reason the qapi_I2CM_Power_On()
	  * failed */
	  DalDevice_Close(i2c_Handle);
	}
   
  return qapi_status;
  
}

qapi_Status_t 
qapi_I2CM_Power_Off 
(
    void *i2c_Handle
)
{
  DalDeviceHandle *hDalDevice;
  qapi_Status_t qapi_status = QAPI_OK;
  
  do{
         
     if(!i2c_Handle)
     { 
         qapi_status = QAPI_I2CM_ERR_INVALID_PARAMETER;
         break;
     }

     hDalDevice =i2c_Handle;
     
     if(DAL_SUCCESS != DalDevice_Close(hDalDevice))
     {
         qapi_status = QAPI_I2CM_ERROR_POWER_OFF_FAILURE;
     }
      
  }while(0);
  
  return qapi_status;
}

