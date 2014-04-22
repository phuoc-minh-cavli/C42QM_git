/** 
    @file  qapi_spi_master.c
    @brief SPI Master QAPI implementation
 */
/*=============================================================================
            Copyright (c) 2017-2018,2019 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/
/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when         who     what, where, why
----------   ---     -----------------------------------------------------------------
10/23/2019   ss     Fixed Phase for SPI Modes. 
07/08/2018   ska    Removed code to call qapi_SPIM_Close incase of qapi_SPIM_Open failure
03/23/2017   ska    Fixed KW issues
02/20/2017   ska    Initial version
==================================================================================*/
#include "qapi_spi_master.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "comdef.h"
#include "DALSys.h"
#include "ddispi.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "SpiLog.h"
#include "SpiDeviceOsSvc.h"
#include "qapi_spi_handler.h"
#include "qurt_memory.h"
#include "qurt_error.h"


static DALDEVICEID spi_get_deviceid(qapi_SPIM_Instance_t instance)
{
   DALDEVICEID dal_device_id = 0;

   switch (instance)
   {
      case QAPI_SPIM_INSTANCE_1_E:
         dal_device_id = DALDEVICEID_SPI_DEVICE_1;
         break;
      case QAPI_SPIM_INSTANCE_2_E:
         dal_device_id = DALDEVICEID_SPI_DEVICE_2;
         break;
      case QAPI_SPIM_INSTANCE_3_E:
         dal_device_id = DALDEVICEID_SPI_DEVICE_3;
         break;
      case QAPI_SPIM_INSTANCE_4_E:
         dal_device_id = DALDEVICEID_SPI_DEVICE_4;
         break;
      case QAPI_SPIM_INSTANCE_5_E:
         dal_device_id = DALDEVICEID_SPI_DEVICE_5;
         break;
      case QAPI_SPIM_INSTANCE_6_E:
         dal_device_id = DALDEVICEID_SPI_DEVICE_6;
         break;
   }

   return dal_device_id;
}

static qapi_Status_t set_spi_device_params (const qapi_SPIM_Config_t *p_config, SpiDeviceInfoType *p_spi_info)
{
   qapi_Status_t qapi_status = QAPI_OK;
   
   p_spi_info->deviceParameters.eClockMode = SPI_CLK_NORMAL;
   switch(p_config->SPIM_Mode)
   {
      case QAPI_SPIM_MODE_0_E : /*CPOL = 0, CPHA = 0*/
          p_spi_info->deviceParameters.eClockPolarity = SPI_CLK_IDLE_LOW;
          p_spi_info->deviceParameters.eShiftMode = SPI_INPUT_FIRST_MODE;
		  break;
	   case QAPI_SPIM_MODE_1_E : /*CPOL = 0, CPHA = 1*/
          p_spi_info->deviceParameters.eClockPolarity = SPI_CLK_IDLE_LOW;
          p_spi_info->deviceParameters.eShiftMode = SPI_OUTPUT_FIRST_MODE;
		  break;
       case QAPI_SPIM_MODE_2_E :  /*CPOL = 1, CPHA = 0*/
		  p_spi_info->deviceParameters.eClockPolarity = SPI_CLK_IDLE_HIGH;
          p_spi_info->deviceParameters.eShiftMode = SPI_INPUT_FIRST_MODE;
		  break;
       case QAPI_SPIM_MODE_3_E :  /*CPOL = 1, CPHA = 1*/
		  p_spi_info->deviceParameters.eClockPolarity = SPI_CLK_IDLE_HIGH;
          p_spi_info->deviceParameters.eShiftMode = SPI_OUTPUT_FIRST_MODE;
		  break;
	   default:
		  qapi_status = QAPI_SPIM_ERROR_INVALID_PARAM;
		  return qapi_status;
   }
   p_spi_info->deviceParameters.u32DeassertionTime = p_config->CS_Clk_Delay_Cycles;
   p_spi_info->deviceParameters.u32MinSlaveFrequencyHz = 0;
   p_spi_info->deviceParameters.u32MaxSlaveFrequencyHz = p_config->Clk_Freq_Hz;
   p_spi_info->deviceParameters.eCSPolarity = (SpiCSPolarityType)p_config->SPIM_CS_Polarity;
   p_spi_info->deviceParameters.eCSMode = (SpiCSModeType)p_config->SPIM_CS_Mode;
   p_spi_info->deviceBoardInfo.nSlaveNumber = p_config->SPIM_Slave_Index;
   p_spi_info->transferParameters.nNumBits = p_config->SPIM_Bits_Per_Word;
   p_spi_info->transferParameters.eTransferMode = SPI_TRANSFER_MODE_DEFAULT;
   p_spi_info->transferParameters.eLoopbackMode = (SpiLoopbackModeType)p_config->loopback_Mode; 
   p_spi_info->transferParameters.eInputPacking = SPI_INPUT_PACKING_DISABLED;
   p_spi_info->transferParameters.eOutputUnpacking = SPI_OUTPUT_UNPACKING_DISABLED;
   p_spi_info->transferParameters.slaveTimeoutUs = 0;
   return qapi_status;
}


qapi_Status_t qapi_SPIM_Open (qapi_SPIM_Instance_t instance, void **spi_Handle)
{
	unsigned int result;
	qapi_Status_t qapi_status = QAPI_OK;
	DalDeviceHandle *hDalDevice = NULL;
	DALDEVICEID dal_device_id;
	
	do 
	{
		if (instance >= QAPI_SPIM_INSTANCE_MAX_E || instance <= 0 || spi_Handle == NULL)
		{
			qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_INVALID_PARAM;
			break;
		}
		
		dal_device_id = spi_get_deviceid(instance);
		
	    if(DAL_SUCCESS != DAL_DeviceAttach(dal_device_id,&hDalDevice))
		{
			qapi_status = QAPI_SPIM_ERROR_OPEN_FAILURE;
			break;
		}
        *spi_Handle = (void *)hDalDevice;	
		
	}
	while(0);

    return qapi_status;
}

qapi_Status_t qapi_SPIM_Power_On (void *spi_Handle)
{
    qapi_Status_t qapi_status = QAPI_OK;
	DalDeviceHandle *hDalDevice = NULL;

    do {
		
		if(!spi_Handle)
	    {
		    qapi_status = QAPI_SPIM_ERROR_INVALID_PARAM;
			break;
	    }
		
		hDalDevice = spi_Handle;
	    if( DAL_SUCCESS != DalDevice_Open(hDalDevice,DAL_OPEN_SHARED) )
      	{
			qapi_status = QAPI_SPIM_ERROR_POWER_ON_FAILURE;
			break;
      	}

    }while(0);
    if (qapi_status != QAPI_OK)
	{
	 /* Clean up everything that we can. Ignore the return value from
	  * qapi_SPIM_Power_Off() as we want to return the reason the qapi_SPIM_Power_On()
	  * failed */
	  SPIDEVICE_LOG(ERROR, "qapi_SPIM_Power_On: failed ");
	 (void)qapi_SPIM_Power_Off(spi_Handle);
	}

    return qapi_status;
}

qapi_Status_t qapi_SPIM_Power_Off(void *spi_Handle)
{
    qapi_Status_t qapi_status = QAPI_OK;
	static DalDeviceHandle *hDalDevice = NULL;

    do {
		if(!spi_Handle)
		{
			qapi_status = QAPI_SPIM_ERROR_INVALID_PARAM;
			break;
		}
		hDalDevice = spi_Handle;
		if(DAL_SUCCESS != DalDevice_Close(hDalDevice))
		{
			qapi_status = QAPI_SPIM_ERROR_POWER_OFF_FAILURE;
			break;
		}

    }while(0);

    return qapi_status;

}

qapi_Status_t qapi_SPIM_Full_Duplex_user (void *spi_Handle, qapi_SPIM_Config_t *config, 
	                                          qapi_SPIM_Descriptor_t *desc,uint32_t num_Descriptors,
	                                          qapi_SPIM_Callback_Fn_t c_Fn, void *c_Ctxt, qbool_t get_timestamp)
{
	qapi_SPIM_Config_t damConfig;
	qapi_SPIM_Descriptor_t damDesc;
	qapi_Status_t qapi_status = QAPI_OK;
	
	
	if (QURT_EOK != qurt_copy_from_user(&damConfig,
                                       sizeof(qapi_SPIM_Config_t),
								       config,
                                       sizeof(qapi_SPIM_Config_t)))
	   {
	    qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_COPY_FAILURE;
		return qapi_status;
	   } 
	   
	if (QURT_EOK != qurt_copy_from_user(&damDesc,
                                       sizeof(qapi_SPIM_Descriptor_t),
								       desc,
                                       sizeof(qapi_SPIM_Descriptor_t)))
	   {
	    qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_COPY_FAILURE;
		return qapi_status;
	   }    
	   
	if (TRUE != qurt_check_if_module_address((qurt_addr_t)damDesc.tx_buf, damDesc.len))
	    {
	     qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_COPY_FAILURE;
		 return qapi_status;
	    }
		
	if (TRUE != qurt_check_if_module_address((qurt_addr_t)damDesc.rx_buf, damDesc.len))
	    {
	     qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_COPY_FAILURE;
		 return qapi_status;
	    }	
	qapi_status = qapi_SPIM_Full_Duplex(spi_Handle, &damConfig, &damDesc, num_Descriptors, c_Fn, c_Ctxt, get_timestamp);
	return qapi_status;
}	

qapi_Status_t qapi_SPIM_Full_Duplex (void *spi_Handle, qapi_SPIM_Config_t *config, 
	                                          qapi_SPIM_Descriptor_t *desc,uint32_t num_Descriptors,
	                                          qapi_SPIM_Callback_Fn_t c_Fn, void *c_Ctxt, qbool_t get_timestamp)
{
	qapi_Status_t qapi_status = QAPI_OK;
	unsigned int result;
	SpiDeviceInfoType spi_info;
	SpiTransferType spiTransfer;
	DalDeviceHandle *hDalDevice = NULL;
	uint32 transferId;
    DALResult dalResult;
	int inputDataLen;
	SpiDataAddrType writeBuf = {NULL,NULL};
    SpiDataAddrType readBuf= {NULL,NULL};
		   
	   
	do
	{
      /* Check for valid input parameters */
		if ((config == NULL) || (desc == NULL) ||
		    (desc->len == 0) || (desc->tx_buf == 0 && desc->rx_buf == 0))
		{
			SPIDEVICE_LOG(ERROR, "qapi_SPIM_Full_Duplex() invalid input parameters");
			qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_INVALID_PARAM;
			break;
		}
		hDalDevice = spi_Handle;
		
		IBUSOS_Memset(&spi_info,0,sizeof(spi_info));
		
		/*set SPI config*/
		qapi_status = set_spi_device_params(config, &spi_info);
		if(QAPI_OK != qapi_status)
		{
		   break;
		}
		
		if(DAL_ERROR == DalSpi_ConfigureDevice(hDalDevice, &spi_info))
		{
			qapi_status = QAPI_SPIM_ERROR_TRANSFER_CONFIG_FAIL;
			break;
		}

		/*Set up SPI buffers*/
		writeBuf.virtualAddr =  writeBuf.physicalAddr = desc->tx_buf;
		readBuf.virtualAddr = readBuf.physicalAddr = desc->rx_buf;
	  
		spiTransfer.u32NumOutputTransfers = desc->len;
		spiTransfer.u32NumInputTransfers  = desc->len;
		spiTransfer.dalSysEvent = NULL;
		spiTransfer.queueIfBusy = TRUE;
		spiTransfer.transferMode = spi_info.transferParameters.eTransferMode;

		if(c_Fn == NULL)
		{
		   dalResult = DalSpi_StartTransfer(hDalDevice, &spiTransfer, &writeBuf, desc->len,
									   &readBuf, desc->len, &inputDataLen, &transferId);
		}
		else
		{
		   dalResult = DalSpi_SendAndReceiveAsync(hDalDevice, &writeBuf, desc->len, spiTransfer.u32NumInputTransfers,
									   &readBuf, desc->len, &inputDataLen, c_Fn, c_Ctxt);
		}

		switch(dalResult)
        {
           case SPI_QUEUED :
		   case SPI_COMPLETE :
		   case SPI_IN_PROGRESS :
		   	  break;
		   default:
		   	  qapi_status = QAPI_SPIM_ERROR_TRANSFER_FAILURE;
			  break;
		}
	}
	while (0);

	return qapi_status;
}

qapi_Status_t qapi_SPIM_Close (void *spi_Handle)
{
	qapi_Status_t qapi_status = QAPI_OK;
	unsigned int result;
	DalDeviceHandle *hDalDevice = NULL;
	
	do 
	{
		if (spi_Handle == NULL)
		{
			qapi_status = (qapi_Status_t)QAPI_SPIM_ERROR_INVALID_PARAM;
			break;
		}

        hDalDevice = spi_Handle;

		if(DAL_SUCCESS != DAL_DeviceDetach(hDalDevice))
		{
			qapi_status = QAPI_SPIM_ERROR_CLOSE_FAILURE;
			break;
		}
	}
	while(0);

	return qapi_status;

}

