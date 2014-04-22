/*
===========================================================================

FILE:   SpiDevicePlatSvc.c

DESCRIPTION:

===========================================================================

        Edit History


When     Who    What, where, why
-------- ---    -----------------------------------------------------------
10/18/16 ska    Added changes to phy mem alloc for BAM mode support
10/04/16 ska    Added Fixes for KW issues Threadx
09/20/16 dpk    Ported for ThreadX
03/10/16 dpk    Added PCNOC Vote/Devote in SPI Open/Close path
12/07/15 sg     Added GPIO Low power mode support in SPI close path
11/19/15 dpk    Added BAM support check to de init BAM in SPI close path
12/26/14 vmk    String-based GPIO configuration
10/03/13 vmk    BAM CMD descriptor usage is determined from properties
08/19/13 sg     Added explicit vote for PNOC clock.
05/30/12 ddk    Added support for correct deassert wait calculation.
05/26/12 ddk    Added support for GPIO initialisation.
04/20/12 ddk    Enabled Clock initialisation in driver.
04/20/12 ddk    Added updates to enable logging.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
04/13/12 ddk    Added de-init changes.
03/28/12 ddk    Added QUP physical address info for BAM cmd desc creation.
03/27/12 ddk    Fixed DAL clock API usage.
03/27/12 ag     Added SPI BAM mode requirements.
09/26/11 ag     Created (Based on Spi implementation by LK)

===========================================================================
        Copyright c 2011 - 2016 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential & Proprietary
===========================================================================
*/

#include "SpiDevicePlatSvc.h"
#include "SpiDeviceOsSvc.h"
#include "SpiDeviceError.h"
#include "SpiDevice.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DDITlmm.h"
#include "SpiDalProps.h"

#define SPI_ICB_CLIENT_CNT              1
#define SPI_PNOC_MSTRSLV_PAIRS_NUM      1
#define SPI_DEVICE_BAM_DESC_SUM_THRESHOLD (32*1024)
#define SPI_DEVICE_BAM_MAX_DESC_SIZE     256

typedef enum IBUSCFG_Error
{
   IBUSPLAT_ERROR_ATTACH_TO_CLOCKS = SPIDEVICE_RES_ERROR_CLS_PLATFORM,
   IBUSPLAT_ERROR_GETTING_CLK_ID,
   IBUSPLAT_ERROR_INVALID_POWER_STATE,
   IBUSPLAT_ERROR_FAILED_TO_SET_APPCLK_FREQ,
   IBUSPLAT_ERROR_FAILED_TO_ENABLE_APPCLK,
   IBUSPLAT_ERROR_FAILED_TO_DISABLE_APPCLK,
   IBUSPLAT_ERROR_FAILED_TO_ENABLE_HCLK,
   IBUSPLAT_ERROR_FAILED_TO_DISABLE_HCLK,
   IBUSPLAT_ERROR_TLMM_ATTACH,
   IBUSPLAT_ERROR_TLMM_OPEN,
   IBUSPLAT_ERROR_NULL_PTR,
   IBUSPLAT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT,
   IBUSPLAT_ERROR_FAILED_TO_ISSUE_PNOC_REQ,
   IBUSPLAT_ERROR_INVALID_PNOC_STATE,
   IBUSPLAT_ERROR_CONFIGURING_GPIOS,
   IBUSPLAT_ERROR_DAL_GET_PROPERTY_HANDLE,
   IBUSPLAT_ERROR_DAL_GET_PROPERTY_VALUE,
   IBUSPLAT_ERROR_GET_GPIO_ID,

   /*BAM errors*/
   IBUSPLAT_ERROR_BAM_INIT,
   IBUSPLAT_ERROR_BAM_RESET,
   IBUSPLAT_ERROR_BAM_DEINIT,
} IBUSCFG_Error;

#define QUPBUSOSSVC_NUM_DEVICES_MAX       12
int32 aQupDalDevIds [QUPBUSOSSVC_NUM_DEVICES_MAX] = { 
   DALDEVICEID_SPI_DEVICE_1, DALDEVICEID_SPI_DEVICE_2, DALDEVICEID_SPI_DEVICE_3,
   DALDEVICEID_SPI_DEVICE_4, DALDEVICEID_SPI_DEVICE_5, DALDEVICEID_SPI_DEVICE_6,
   DALDEVICEID_SPI_DEVICE_7, DALDEVICEID_SPI_DEVICE_8, DALDEVICEID_SPI_DEVICE_9,
   DALDEVICEID_SPI_DEVICE_10, DALDEVICEID_SPI_DEVICE_11, DALDEVICEID_SPI_DEVICE_12 };

typedef enum IBUSPLAT_PowerStates
{
   IBUSPLAT_POWER_STATE_0, /**< Bus and clocks off. */
   IBUSPLAT_POWER_STATE_1, /**< Bus on clocks off. */
   IBUSPLAT_POWER_STATE_2, /**< Bus on clocks on. */
} IBUSPLAT_PowerStates;

static DalDeviceHandle *pClkHandle = NULL;
static DalDeviceHandle *pTlmmHandle = NULL;

static uint32 IBUSPLAT_InitBam(SpiDevProp_DevTargetCfgType *);
static uint32 IBUSPLAT_DeInitBam(SpiDevProp_DevTargetCfgType *pDev);
static int32 IBUSPLAT_ClkDeInit(SpiDevProp_DevTargetCfgType *pDevTgtCfg);

static int32 IBUSPLATCFG_ActiveGpioState (SpiDevProp_DevTargetCfgType *pDev)
{
   uint32 i;
   DalTlmm_GpioConfigIdType pUserSettings;
   
   pUserSettings.eDirection = DAL_GPIO_INPUT; //don't care for function GPIOs
   pUserSettings.ePull = DAL_GPIO_PULL_DOWN;  
   pUserSettings.eDriveStrength = DAL_GPIO_8MA; 
   
   
   for (i=0; i < SPI_MAX_NUM_GPIOS; i++)
   {
      if(pDev->nGpioId[i] != NULL)
      { 
		if (DAL_SUCCESS != DalTlmm_ConfigGpioId (pTlmmHandle, pDev->nGpioId[i], &pUserSettings))
		{
			return IBUSPLAT_ERROR_CONFIGURING_GPIOS;
		}
      }
   }
   return SPIDEVICE_RES_SUCCESS;
}

static int32 IBUSPLATCFG_InActiveGpioState (SpiDevProp_DevTargetCfgType *pDev)
{
   uint32 i;
   
   for (i=0; i < SPI_MAX_NUM_GPIOS; i++)
   {
      if(pDev->nGpioId[i] != NULL)
      { 
		if (DAL_SUCCESS != DalTlmm_ConfigGpioIdInactive(pTlmmHandle, pDev->nGpioId[i]))
		{
			return IBUSPLAT_ERROR_CONFIGURING_GPIOS;
		}
      }
   }
   return SPIDEVICE_RES_SUCCESS;
}


uint32 IBUSPLAT_SetGpioState(IBUSPLAT_HANDLE hPlat, uint32 state)
{

   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;

   if (SPIDEVICE_GPIO_STATE_ACTIVE == state)
   {
      return IBUSPLATCFG_ActiveGpioState(pDev);
   }

   else if (SPIDEVICE_GPIO_STATE_INACTIVE == state)
   {
      return IBUSPLATCFG_InActiveGpioState(pDev);
   }
  
   return SPIDEVICE_RES_SUCCESS;
}


static int32 IBUSPLATCFG_DeinitSpiGpio (SpiDevProp_DevTargetCfgType *pDev)
{
   uint32 i;
   for (i=0; i < SPI_MAX_NUM_GPIOS; i++)
   {
      if(pDev->nGpioId[i] != NULL)
      {
         DalTlmm_ReleaseGpioId(pTlmmHandle, pDev->nGpioId[i]);
      }
   }
   return SPIDEVICE_RES_SUCCESS;
}

static int32 IBUSPLAT_InitSpiGpios (SpiDevProp_DevTargetCfgType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   int32 res = SPIDEVICE_RES_SUCCESS;
   
   do
   {
      if (NULL == pTlmmHandle)
      {
         SPIDEVICE_LOG(ERROR,"spi pTlmmHandle not initialised");
         res = DAL_ERROR;
		 break;
      }
   
      if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(pDev->uOsDevId, hProp))
      {
         res = IBUSPLAT_ERROR_DAL_GET_PROPERTY_HANDLE;
		 break;
      }
   
      //Read CLK GPIO
      if (DAL_SUCCESS != DALSYS_GetPropertyValue (hProp, "gpio_spi_clk_str", 0, &PropVar))
      {
         res = IBUSPLAT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
	  
	  if (DAL_SUCCESS != DalTlmm_GetGpioId(pTlmmHandle, PropVar.Val.pszVal, &pDev->nGpioId[0]))
	  {
	     pDev->nGpioId[0] = 0;
		 res = IBUSPLAT_ERROR_GET_GPIO_ID;
		 break;
	  }
   
      //Read MOSI GPIO
      if (DAL_SUCCESS != DALSYS_GetPropertyValue (hProp, "gpio_spi_mosi_str", 0, &PropVar))
      {
         res = IBUSPLAT_ERROR_DAL_GET_PROPERTY_VALUE;
		 break;
      }
      
	  if (DAL_SUCCESS != DalTlmm_GetGpioId(pTlmmHandle, PropVar.Val.pszVal, &pDev->nGpioId[1]))
	  {
	     pDev->nGpioId[1] = 0;
		 res = IBUSPLAT_ERROR_GET_GPIO_ID;
		 break;
	  }
   

      //Read MISO GPIO
      if (DAL_SUCCESS != DALSYS_GetPropertyValue (hProp, "gpio_spi_miso_str", 0, &PropVar))
      {
         res = IBUSPLAT_ERROR_DAL_GET_PROPERTY_VALUE;
		 break;
      }
	  
	  if (DAL_SUCCESS != DalTlmm_GetGpioId(pTlmmHandle, PropVar.Val.pszVal, &pDev->nGpioId[2]))
	  {
	     pDev->nGpioId[2] = 0;
		 res = IBUSPLAT_ERROR_GET_GPIO_ID;
		 break;
	  }
      //Read CS GPIO
      if (DAL_SUCCESS == DALSYS_GetPropertyValue (hProp, "gpio_spi_cs_str", 0, &PropVar))
      {
        if (DAL_SUCCESS != DalTlmm_GetGpioId(pTlmmHandle, PropVar.Val.pszVal, &pDev->nGpioId[3]))
		{
			pDev->nGpioId[3] = 0;
			res = IBUSPLAT_ERROR_GET_GPIO_ID;
			break;
		}
      }
      //else nothing, its OK to not have CS GPIO configured, e.g. for 3-wire SPI.
   }
   while(0);
   return res;
}

static int32 IBUSPLAT_ClkInit(SpiDevProp_DevTargetCfgType *pDevTgtCfg)
{
   DALResult dalRes;
   int32 res = SPIDEVICE_RES_SUCCESS;

   do
   {
      dalRes = DalClock_GetClockId(pClkHandle, pDevTgtCfg->pQupAppClkName, &pDevTgtCfg->QupAppClkId);

      if (DAL_SUCCESS != dalRes)
      {
         res = IBUSPLAT_ERROR_GETTING_CLK_ID;
         break;
      }

      dalRes = DalClock_GetClockId(pClkHandle, pDevTgtCfg->pQupHClkName, &pDevTgtCfg->QupHClkId);

      if (DAL_SUCCESS != dalRes)
      {
         res = IBUSPLAT_ERROR_GETTING_CLK_ID;
         break;
      }

      /* Enable QUP Clock */
      dalRes = DalClock_EnableClock(pClkHandle, pDevTgtCfg->QupAppClkId);
      if (DAL_SUCCESS != dalRes)
      {
         res = IBUSPLAT_ERROR_FAILED_TO_ENABLE_APPCLK;
         break;
      }

      /* Enable HCLK */
      dalRes = DalClock_EnableClock(pClkHandle, pDevTgtCfg->QupHClkId);
      if (DAL_SUCCESS != dalRes)
      {
         res = IBUSPLAT_ERROR_FAILED_TO_ENABLE_HCLK;
         break;
      }
   }
   while (0);

   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR, "IBUSPLAT_ClkInit: Failure err 0x%x\n", (unsigned int)dalRes);
   }

   return res;
}



static uint32 IBUSPLAT_AddPNOCVote (SpiDevProp_DevTargetCfgType *pDev)
{
   ICBArb_RequestType pnocRequest;
   ICBArb_MasterSlaveType icbMasterSlave;
   ICBArb_ErrorType icbReqStatus;

   icbMasterSlave.eMaster =(ICBId_MasterType) (pDev->uPnocMaster);
   icbMasterSlave.eSlave = (ICBId_SlaveType)(pDev->uPnocSlave);

   /*NPA handle for PNOC*/
   pDev->pNpaHandle = icbarb_create_suppressible_client_ex
      ("/node/core/bus/spi/clk",
       &icbMasterSlave,
       SPI_ICB_CLIENT_CNT, NULL);
   if (pDev->pNpaHandle == NULL)
   {
      return IBUSPLAT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT;
   }

   pnocRequest.arbType = ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */

   /* Bandwidth in Bytes/second */
   pnocRequest.arbData.type3.uIb = (uint64)pDev->uPnocIBVal;
   pnocRequest.arbData.type3.uAb = (uint64)pDev->uPnocABVal;

   icbReqStatus = icbarb_issue_request(pDev->pNpaHandle, &pnocRequest, SPI_PNOC_MSTRSLV_PAIRS_NUM);

   if (icbReqStatus != ICBARB_ERROR_SUCCESS)
   {
      return IBUSPLAT_ERROR_FAILED_TO_ISSUE_PNOC_REQ;
   }

   return SPIDEVICE_RES_SUCCESS;
}

static uint32 IBUSPLAT_RemovePNOCVote(SpiDevProp_DevTargetCfgType *pDev)
{
   if (pDev->pNpaHandle == NULL)
   {
      return SPIDEVICE_RES_ERROR_CLS_PLATFORM;
   } else
   {
      icbarb_complete_request(pDev->pNpaHandle);
      icbarb_destroy_client(pDev->pNpaHandle);
      pDev->pNpaHandle = NULL;
   }
   return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSPLAT_InitTarget(void *QupHandle, IBUSPLAT_HANDLE *phPlat)
{
   DALResult dalRes;
   SpiDevProp_DevTargetCfgType *pDev = NULL;
   int32 res = SPIDEVICE_RES_SUCCESS;

   do
   {
      if (FALSE == SpiDevProp_SearchDevCfg((uint32)QupHandle, &pDev))
      {
         break;
      }

      if (NULL == pClkHandle)
      {
         dalRes = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &pClkHandle);
         if ((DAL_SUCCESS != dalRes) || (NULL == pClkHandle))
         {
            res = IBUSPLAT_ERROR_ATTACH_TO_CLOCKS;
            break;
         }
      }
      if (NULL == pTlmmHandle)
      {
         if (DAL_SUCCESS != DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM,
                                               DALTLMM_INTERFACE_VERSION, &pTlmmHandle))
         {
            res = IBUSPLAT_ERROR_TLMM_ATTACH;
            break;
         }

         if (DAL_SUCCESS != DalDevice_Open(pTlmmHandle, DAL_OPEN_SHARED))
         {
            DAL_DeviceDetach(pTlmmHandle);
            pTlmmHandle = NULL;
            res = IBUSPLAT_ERROR_TLMM_OPEN;
            break;
         }
      }
	  res = IBUSPLAT_ClkInit(pDev);

      if (SPIDEVICE_RES_SUCCESS != res)
      {
         break;
      }

	  res = IBUSPLAT_InitSpiGpios(pDev);
	  
      if (SPIDEVICE_RES_SUCCESS != res)
      {
         break;
      }

      if (pDev->bBamSupported)
      {
         res = IBUSPLAT_InitBam(pDev);
      }
   }
   while (0);

   if (res == SPIDEVICE_RES_SUCCESS)
   {
      *phPlat = (IBUSPLAT_HANDLE)pDev;
      pDev->uActiveDevCount++;
      SPIDEVICE_LOG(INFO,"IBUSPLAT_InitTarget: uActiveDevCount 0x%x\n",(unsigned int)pDev->uActiveDevCount);
   } 
   else
   {
      SPIDEVICE_LOG(ERROR,"IBUSPLAT_InitTarget: Failure err 0x%x\n",(unsigned int)res);
   }
   return res;
}

uint32 IBUSPLAT_SetPowerState(IBUSPLAT_HANDLE hPlat, uint32 state)
{

   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;

   /* Bus state is OFF */
   if (IBUSPLAT_POWER_STATE_0 == state)
   {
      return IBUSPLAT_ClkDeInit(pDev);
   }
   /* Bus state is POWERED */
   else if (IBUSPLAT_POWER_STATE_1 == state)
   {
      return IBUSPLAT_ClkDeInit(pDev);
   }
   /* Bus state is ACTIVE */
   else if (IBUSPLAT_POWER_STATE_2 == state)
   {
      return IBUSPLAT_ClkInit(pDev);
   } else
   {
      return IBUSPLAT_ERROR_INVALID_POWER_STATE;
   }
  // return SPIDEVICE_RES_SUCCESS;
}

uint32 IBUSPLAT_DeInitTarget(IBUSPLAT_HANDLE hPlat)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;

   do
   {
      if (pDev == NULL)
      {
         res = IBUSPLAT_ERROR_NULL_PTR;
         break;
      }

      if (pDev->bBamSupported)
      {	  
         res = IBUSPLAT_DeInitBam(pDev);
         if (res != SPIDEVICE_RES_SUCCESS)
         {
            break;
         }
      }
      
      IBUSPLAT_SetPowerState(hPlat, IBUSPLAT_POWER_STATE_2);
      if (pDev->uActiveDevCount == 1)
      {
         res = SpiDevProp_RemoveDevCfg(pDev->uOsDevId);
         SPIDEVICEPLAT_CHK_RESULT_BREAK(res);
      }
      IBUSPLAT_SetPowerState(hPlat, IBUSPLAT_POWER_STATE_0);
	  
	  //release GPIOs
	  IBUSPLATCFG_DeinitSpiGpio(pDev);
      pDev->uActiveDevCount--;
   }
   while (0);

   return res;
}

uint32 IBUSPLAT_SetAppClkHz (IBUSPLAT_HANDLE hPlat, uint32 reqFrequencyHz, uint32 *pFinalFrequencyHz)
{
   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;
   DALResult dalRes;

   dalRes = DalClock_SetClockFrequency(pClkHandle,
                                       pDev->QupAppClkId,
                                       reqFrequencyHz,
                                       CLOCK_FREQUENCY_HZ_AT_MOST,
                                       pFinalFrequencyHz);

   if (DAL_SUCCESS != dalRes)
   {
      return IBUSPLAT_ERROR_FAILED_TO_SET_APPCLK_FREQ;
   }

   return SPIDEVICE_RES_SUCCESS;
}


static int32 IBUSPLAT_ClkDeInit(SpiDevProp_DevTargetCfgType *pDevTgtCfg)
{
   DALResult dalRes;
   int32 res = SPIDEVICE_RES_SUCCESS;

   /* Disable QUP Clock */
   dalRes = DalClock_DisableClock(pClkHandle, pDevTgtCfg->QupAppClkId);
   if (DAL_SUCCESS != dalRes)
   {
      res = IBUSPLAT_ERROR_FAILED_TO_DISABLE_APPCLK;
   }

   /* Disable HCLK */
   dalRes = DalClock_DisableClock(pClkHandle, pDevTgtCfg->QupHClkId);
   if (DAL_SUCCESS != dalRes)
   {
      res = IBUSPLAT_ERROR_FAILED_TO_DISABLE_HCLK;
   }

   return res;
}

uint32 IBUSPLAT_GetPowerState(IBUSPLAT_HANDLE hPlat, uint32 state)
{
   return SPIDEVICE_RES_SUCCESS;
}

static void IBUSPLAT_BamPipeCallback(bam_result_type bam_result)
{
   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)(bam_result.cb_data);
   SpiDevProp_BamDevCfgType *devCfg = (SpiDevProp_BamDevCfgType *)pDev->spiBamDevCfg;
   SpiDevProp_BamPipeCtxt *pipeCtxt = NULL;
   SPIDEVICE_BAM_RESULT pResult;

   if (devCfg != NULL)
   {
      pipeCtxt = bam_result.data.xfer.xfer_cb_data;
      pResult.pUser = pipeCtxt->devBamPipeCallbackCfg.data;

      switch (bam_result.event)
      {
      case BAM_EVENT_EOT:
         pResult.result = SPIDEVICE_RES_SUCCESS;
         break;

      case BAM_EVENT_DESC_DONE:
         pResult.result = SPIDEVICE_RES_SUCCESS;
         break;

      case BAM_EVENT_ERROR:
         pResult.result = IBUSPLAT_ERROR_BAM_TRANSACTION;
         break;

      default:
         pResult.result = IBUSPLAT_ERROR_BAM_TRANSACTION;
         break;
      };

      /* Call the callback */
      {
         SPIDEVICE_BAM_CALLBACK cb = (SPIDEVICE_BAM_CALLBACK)devCfg->devBamCallbackCfg.func;
         cb(&pResult);
      }
   }
   else
   {
      /* TODO: Should we error fatal or indicate some logs and just return ? */
   }
}

static int32 IBUSPLAT_InitBamPipe (SpiDevProp_DevTargetCfgType *pDev, uint32 hBamDev, SpiDevProp_BamPipeCtxt *pipeCtxt, uint32 pipedir)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   bam_pipe_config_type *pipe_cfg = &(pipeCtxt->devBamPipeConfig);
   bam_callback_type *pipe_cb     = &(pipeCtxt->devBamPipeCallbackCfg);
   
   do
   {
      if (NULL == pipeCtxt)
      {
         result = IBUSPLAT_ERROR_NULL_PTR;
         break;
      }

      if (0 == pipeCtxt->hBamPipeDev)
      {
         if (pipeCtxt->devBamPipeDescFifo.phMemHandle == NULL)
         {
            if(SPIDEVICE_RES_SUCCESS != IBUSOS_PhysMemAlloc(SPI_DEVICE_BAM_MAX_DESC_SIZE,
                                                       &(pipeCtxt->devBamPipeDescFifo.phMemHandle),
                                                       (void *)&(pipeCtxt->devBamPipeDescFifo.virtualAddr),
                                                       (void *)&(pipeCtxt->devBamPipeDescFifo.physAddr)))
            {
               result = IBUSPLAT_ERROR_BAM_INIT;
               break;
            }
         }

         pipe_cfg->options       = BAM_O_DESC_DONE|BAM_O_EOT|BAM_O_ERROR;
         pipe_cfg->dir           = (bam_dir_type)pipedir;
         pipe_cfg->mode          = BAM_MODE_SYSTEM;
         pipe_cfg->desc_base_va  = pipeCtxt->devBamPipeDescFifo.virtualAddr;
         pipe_cfg->desc_base_pa  = pipeCtxt->devBamPipeDescFifo.physAddr;
         pipe_cfg->desc_size     = SPI_DEVICE_BAM_MAX_DESC_SIZE;
      
         pipe_cb->func = IBUSPLAT_BamPipeCallback;
         pipe_cb->data = pDev;

         pipeCtxt->hBamPipeDev = bam_pipe_init(hBamDev, pipeCtxt->devBamPipeNum, pipe_cfg, pipe_cb);

         if (0 == pipeCtxt->hBamPipeDev)
         {
            result = IBUSPLAT_ERROR_BAM_INIT;
            break;
         }
      }      
   }while(0);
   
   return result;
}

static uint32 IBUSPLAT_InitBam(SpiDevProp_DevTargetCfgType *pDev)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   SpiDevProp_BamDevCfgType *devCfg = pDev->spiBamDevCfg;
   SpiDevProp_BamPipeCtxt *pipeCtxt = NULL;
   bam_callback_type bam_cb;

   do
   {
      if (NULL == devCfg)
      {
         res = IBUSPLAT_ERROR_NULL_PTR;
         break;
      }

      if (0 == devCfg->hBamDev)
      {
         devCfg->devBamConfig.bam_va = 0; //BAM should remap if this is zero
         devCfg->devBamConfig.bam_irq_mask = BAM_IRQ_HRESP_ERR_EN|BAM_IRQ_ERR_EN;
         devCfg->devBamConfig.sum_thresh = SPI_DEVICE_BAM_DESC_SUM_THRESHOLD;
         devCfg->devBamConfig.options = 0; // Not used
        
         bam_cb.func = IBUSPLAT_BamPipeCallback;
         bam_cb.data = (void *)pDev; 

         devCfg->hBamDev = bam_init(&(devCfg->devBamConfig), &bam_cb);
      }
      
      if (0 == devCfg->hBamDev)
      {
         res = IBUSPLAT_ERROR_BAM_INIT;
         break;
      }

      /* TX pipe configuration */
      pipeCtxt = &(devCfg->devBamTxPipeCtxt);

      if (0 == pipeCtxt->hBamPipeDev)
      {
         res = IBUSPLAT_InitBamPipe(pDev, devCfg->hBamDev, pipeCtxt, BAM_DIR_CONSUMER);
      }

      /* RX pipe configuration */
      pipeCtxt = &(devCfg->devBamRxPipeCtxt);  

      if (0 == pipeCtxt->hBamPipeDev)
      {
         res = IBUSPLAT_InitBamPipe(pDev, devCfg->hBamDev, pipeCtxt, BAM_DIR_PRODUCER);
      }
      
   }while(0);
   
   return res;
}

uint32 IBUSPLAT_ResetBam(IBUSPLAT_HANDLE hPlat)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;
   SpiDevProp_BamDevCfgType *devCfg;

   do
   {
      devCfg = pDev->spiBamDevCfg;
      if (NULL == devCfg)
      {
         result = IBUSPLAT_ERROR_NULL_PTR;
         break;
      }

      if (0 != devCfg->hBamDev)
      {
         if (bam_reset(devCfg->hBamDev) != BAM_SUCCESS)
         {
            result = IBUSPLAT_ERROR_BAM_RESET;
            break;
         }
      }
     
   }while(0);

   return result;
}

/*  @brief Schedule a single BAM transfer.
 *
 *  @param[in] hPlat        platform device handle
 *  @param[in] bWrite       TRUE for write transfers, FALSE for read transfers
 *  @param[in] uAddr        physical start address for the transfer
 *  @param[in] uSize        size of the transfer in bytes
 *  @param[in] pUser        user-defined pointer associated with the transfer
 *  @param[in] uFlags       flags for the transfer
 *  @param[in] pCallBackFn  callback function for transfer completion
 *  @return                 error status
 */
uint32 IBUSPLAT_BamTransfer(IBUSPLAT_HANDLE hPlat,
                            boolean bWrite,
                            void *pAddr,
                            uint32 uSize,
                            void *pUser,
                            uint32 uFlags,
                            SPIDEVICE_BAM_CALLBACK pCallBackFn)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;
   SpiDevProp_BamDevCfgType *devCfg = NULL;
   SpiDevProp_BamPipeCtxt *pipeCtxt = NULL;

   do
   {
      devCfg = pDev->spiBamDevCfg;
      if (NULL == devCfg)
      {
         result = IBUSPLAT_ERROR_NULLPTR;
         break;
      }

      pipeCtxt = bWrite ? &(devCfg->devBamTxPipeCtxt) : &(devCfg->devBamRxPipeCtxt);

      if (0 != pipeCtxt->hBamPipeDev) 
      {
         devCfg->devBamCallbackCfg.func = pCallBackFn;
         pipeCtxt->devBamPipeCallbackCfg.data = pUser;
            
         if (bam_pipe_transfer(pipeCtxt->hBamPipeDev, (DALSYSMemAddr )pAddr, (uint16) uSize, (uint16) uFlags, pipeCtxt) != BAM_SUCCESS)
         {
            result = IBUSPLAT_ERROR_BAM_TRANSACTION;
            break;
         }
      }
      else
      {
         result = IBUSPLAT_ERROR_BAM_TRANSACTION;
         break;       
      }
     
   }while(0);
   return result;
}

uint32 IBUSPLAT_GetQupNum(void *QupHandle)
{
   SpiDevProp_DevTargetCfgType *pDev = NULL;
   if (FALSE == SpiDevProp_SearchDevCfg((uint32)QupHandle, &pDev))
   {
      return (uint32)(-1); 
   }
   return pDev->uQupCoreNum;
}

static int32 IBUSPLAT_DeInitBamPipe (SpiDevProp_BamPipeCtxt *pipeCtxt)
{
   int32 result = SPIDEVICE_RES_SUCCESS;

   do
   {
      if (NULL == pipeCtxt)
      {
         result = IBUSPLAT_ERROR_NULL_PTR;
         break;
      }

      if (0 != pipeCtxt->hBamPipeDev)
      {
         if (bam_pipe_deinit(pipeCtxt->hBamPipeDev) != BAM_SUCCESS)
         {
            result = IBUSPLAT_ERROR_BAM_DEINIT;
            break;
         }

         pipeCtxt->hBamPipeDev = 0;

         if (pipeCtxt->devBamPipeDescFifo.phMemHandle != NULL)
         {
            result = IBUSOS_PhysMemFree(pipeCtxt->devBamPipeDescFifo.phMemHandle);
   
            pipeCtxt->devBamPipeDescFifo.phMemHandle = NULL;
            pipeCtxt->devBamPipeDescFifo.virtualAddr = 0;         
            pipeCtxt->devBamPipeDescFifo.physAddr = 0;         
            if (SPIDEVICE_RES_SUCCESS != result)
	        {
               break;
            }
         }
      }
   }while(0);
  
   return result;
}

static uint32 IBUSPLAT_DeInitBam (SpiDevProp_DevTargetCfgType *pDev)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevProp_BamDevCfgType *devCfg = (SpiDevProp_BamDevCfgType *)(pDev->spiBamDevCfg);

   do
   {
      if (NULL == devCfg)
      {
         result = IBUSPLAT_ERROR_NULL_PTR;
         break;
      }

	  DalClock_EnableClock(pClkHandle, pDev->QupHClkId);
      result = IBUSPLAT_DeInitBamPipe(&(devCfg->devBamTxPipeCtxt));
      if (SPIDEVICE_RES_SUCCESS !=result)
	  {
         break;
      }

      result = IBUSPLAT_DeInitBamPipe(&(devCfg->devBamRxPipeCtxt));
      if (SPIDEVICE_RES_SUCCESS !=result)
	  {
         break;
      }

      DalClock_DisableClock(pClkHandle, pDev->QupHClkId); 
   }while(0);
   return result;
}

uint32 IBUSPLAT_SetPNOCVote(IBUSPLAT_HANDLE hPlat, uint32 state)
{
   uint32 res = SPIDEVICE_RES_SUCCESS;
   SpiDevProp_DevTargetCfgType *pDev = (SpiDevProp_DevTargetCfgType *)hPlat;

   if (TRUE == pDev->bPnocEnable)
   {
      if (SPIDEVICE_PNOC_VOTE_ADD == state)
      {
         res = IBUSPLAT_AddPNOCVote(pDev);
      }
      else if (SPIDEVICE_PNOC_VOTE_REMOVE == state)
      {
         res = IBUSPLAT_RemovePNOCVote(pDev);
      }
      else
      {
         res = IBUSPLAT_ERROR_INVALID_PNOC_STATE;
      }	  
   }
   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR, "IBUSPLAT_SetPNOCVote() failed. state = %d, res = 0x%x\n", state, res);
   }
   return res;
}
