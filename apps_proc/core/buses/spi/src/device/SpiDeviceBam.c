/*
===========================================================================

FILE: SpiDeviceBam.c

DESCRIPTION:

===========================================================================

        Edit History



When      Who  What, where, why
--------  ---  ------------------------------------------------------------
10/04/16  ska  Added Fixes for KW issues Threadx
09/20/16  dpk  Ported for ThreadX
10/03/13  vmk  BAM CMD descriptor usage is determined from properties
05/30/12  ddk  Added support for common qup hal.
04/20/12  ddk  Added some more logging.
04/20/12  ddk  Added updates to enable logging.
04/19/12  ddk  Added changes for avoiding symbol match with I2C library.
04/07/12  ddk  Updated BAM flags being used.
03/29/12  ddk  Added deinit steps.
12/10/12  ag   Created.
===========================================================================
        Copyright (c) 2011-2016 Qualcomm Technologies Incorporated
                            All Rights Reserved
                         Qualcomm Proprietary/GTDR
===========================================================================
*/

#include "SpiDeviceBam.h"
#include "SpiDeviceOsSvc.h"
#include "SpiDevicePlatSvc.h"
#define SPIDEVICE_BAM_LOG_H
#include "SpiLog.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RESET_CMD_BUFFER_SIZE     16
#define SETTINGS_CMD_BUFFER_SIZE  512

#define SPIDEVICEBAM_CHK_RESULT_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res)\
   {\
      break;\
   }

static void
spiDeviceBamCallBack
(
   SPIDEVICE_BAM_RESULT *platBamRes
)
{
   SpiBamPlatCBArgs *cbArgs = (SpiBamPlatCBArgs *)platBamRes->pUser;
   SpiDeviceBamType *spiBam = cbArgs->spiBamObj;
   SpiDeviceTransferType *transfer = spiBam->transfer;

   if (SPIDEVICE_BAM_WRITE_TAG == cbArgs->bamTags)
   {
      transfer->bWriteTransferDone = TRUE;
      spiBam->result.result = platBamRes->result;
      SPIDEVICE_LOG(INFO,"spiDeviceBamCallBack: Write Tag Callback\n");
   }
   else if (SPIDEVICE_BAM_READ_TAG == cbArgs->bamTags)
   {
      transfer->bReadTransferDone = TRUE;
      spiBam->result.result = platBamRes->result;
      SPIDEVICE_LOG(INFO,"spiDeviceBamCallBack: Read Tag Callback\n");
   }

   if ((transfer->bReadTransferDone) && (transfer->bWriteTransferDone))
   {
      if (spiBam->pCallbackFn)
      {
         spiBam->pCallbackFn(&(spiBam->result));
      }
   }
}

static void
spiDeviceBamCancelTransfer
(
   SpiDeviceBamType *spiBam,
   SpiDeviceTransferType *transfer
)
{
   SPIDEVICE_UNREFERENCED_PARAMETER(transfer);
   IBUSPLAT_ResetBam(spiBam->bam_cfg.PlatHandle);
}

static void
spiDeviceBamSetTransferCallback
(
   SpiDeviceBamType *spiBam,
   SPIDEVICE_BAM_TRANSFER_CB pCallBackFn
)
{
   spiBam->pCallbackFn = pCallBackFn;
}

static void
spiDeviceBamSetTransferCmdParms
(
   SpiDeviceBamType *spiBam,
   SpiDeviceBamCmdParms *cmdParms
)
{
   spiBam->cmdParms = cmdParms;
}

static int32
spiDeviceBamGetResetDesc
(
   SpiDeviceBamType *spiBam
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   HAL_qupe_BamCommandDescType halBamCmdDescInfo;
   uint32 do_while_constant = 0;

   do
   {
      if (NULL == spiBam)
      {
         SPIDEVICE_LOG(ERROR,"spiDeviceBamGetResetDesc: Null ptr error\n");
         result = IBUSPLAT_ERROR_NULLPTR;
         break;
      }

      halBamCmdDescInfo.cmd_element_base = spiBam->resetCmdDesc.virtualAddr;
      halBamCmdDescInfo.cmd_element_index = 0;

      result = HAL_qupe_SetCoreState_CommandElement(spiBam->bam_cfg.QupPhysAddr,
                                                    HAL_QUPE_RUNSTATE_RESET,
                                                    &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         SPIDEVICE_LOG(ERROR,
            "spiDeviceBamGetResetDesc: Cmd element creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSPLAT_ERROR_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      /* Save the command element index */
      spiBam->resetCmdDesc.cmd_element_index = halBamCmdDescInfo.cmd_element_index;
      spiBam->resetCmdDesc.size_in_bytes = halBamCmdDescInfo.cmd_size_in_bytes;

   } while(do_while_constant);

   return result;
}

static int32
SpiDeviceBamGetSettingsDesc
(
   SpiDeviceBamType *spiBam
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   uint8 *PhysicalQupAddr;
   uint8 *VirtualQupAddr;
   HAL_qupe_BamCommandDescType halBamCmdDescInfo;
   uint32 do_while_constant = 0;

   do
   {
      if (NULL == spiBam)
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamGetSettingsDesc: Null ptr error\n");
         result = IBUSPLAT_ERROR_NULLPTR;
         break;
      }

      if (NULL == spiBam->settingsCmdDesc.physAddr)
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamGetSettingsDesc: Null ptr error\n");
         result = IBUSPLAT_ERROR_NULLPTR;
         break;
      }

      PhysicalQupAddr = spiBam->bam_cfg.QupPhysAddr;
      VirtualQupAddr  = spiBam->bam_cfg.QupVirtAddr;

      halBamCmdDescInfo.cmd_element_base = spiBam->settingsCmdDesc.virtualAddr;
      halBamCmdDescInfo.cmd_element_index = 0;

      result = HAL_qupe_SetCoreMode_CommandElement(PhysicalQupAddr,
                                           HAL_QUPE_MINI_CORE_SPI,
                                           &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         SPIDEVICE_LOG(ERROR,
            "SpiDeviceBamGetSettingsDesc: SetCoreMode Cmd element creation failed err 0x%x\n",
            (unsigned int )result);

         result = IBUSPLAT_ERROR_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      result = HAL_qupe_Spi_SetDeviceParameters_CommandElement(
                            PhysicalQupAddr,
                            VirtualQupAddr,
                            &(spiBam->mHalDeviceParameters),
                            &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         SPIDEVICE_LOG(ERROR,
            "SpiDeviceBamGetSettingsDesc:"
            "SetDeviceParameters Cmd element creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSPLAT_ERROR_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      result = HAL_qupe_Spi_SetTransferParameters_CommandElement(
                           PhysicalQupAddr,
                           &(spiBam->mHalTransferParameters),
                           &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         SPIDEVICE_LOG(ERROR,
            "SpiDeviceBamGetSettingsDesc:"
            "SetTransferParameters Cmd element creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSPLAT_ERROR_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      result = HAL_qupe_SetCoreState_CommandElement(PhysicalQupAddr,
                                           HAL_QUPE_RUNSTATE_RUN,
                                           &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         SPIDEVICE_LOG(ERROR,
            "SpiDeviceBamGetSettingsDesc: SetCoreMode Cmd element t creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSPLAT_ERROR_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      /* Save the command element index and calculate the no: of bytes */
      spiBam->settingsCmdDesc.cmd_element_index = halBamCmdDescInfo.cmd_element_index;
      spiBam->settingsCmdDesc.size_in_bytes = halBamCmdDescInfo.cmd_size_in_bytes;

   } while(do_while_constant);

   return result;
}

static int32
spiDeviceBamQueueCmdDescs
(
   SpiDeviceBamType *spiBam
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;

   void *writeCBArgs = (void *)&(spiBam->writeArgs);
   uint32 do_while_constant = 0;

   do
   {
      res = spiDeviceBamGetResetDesc(spiBam);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      res = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                 TRUE, spiBam->resetCmdDesc.physAddr,
                                 spiBam->resetCmdDesc.size_in_bytes, writeCBArgs,
                                 PLAT_BAM_DESC_FLAG_CMD | PLAT_BAM_DESC_FLAG_NWD,
                                 spiDeviceBamCallBack);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      res = SpiDeviceBamGetSettingsDesc(spiBam);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      res = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                 TRUE, spiBam->settingsCmdDesc.physAddr,
                                 spiBam->settingsCmdDesc.size_in_bytes, writeCBArgs,
                                 PLAT_BAM_DESC_FLAG_CMD | PLAT_BAM_DESC_FLAG_NWD,
                                 spiDeviceBamCallBack);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      SPIDEVICE_LOG(INFO,"spiDeviceBamQueueCmdDescs: Cmd Desc Queued\n");
   } while(do_while_constant);

   return res;
}

static int32
spiDeviceBamQueueWriteDescs
(
   SpiDeviceBamType *spiBam
)
{
   int32 bamRes = SPIDEVICE_RES_SUCCESS;
   SpiDeviceTransferDataResult bufAvail, bufNextAvail;
   uint8 *dataBuf;
   uint32 uDataSize;
   uint32 uFlags = PLAT_BAM_DESC_FLAG_NO_SUBMIT;
   SpiDeviceTransferType *transfer = spiBam->transfer;
   void *writeCBArgs = (void *)&(spiBam->writeArgs);

   /*If the buffer is not the last buffer of the list then queue to
    * platform BAM layer*/
   do
   {
      bamRes = SPIDEVICE_RES_SUCCESS;
      bufAvail = transfer->getWriteBufPhys(transfer, &dataBuf);
      uDataSize = transfer->getWriteBufLen(transfer);
      bufNextAvail = transfer->moveWriteNext(transfer);
   } while ((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufNextAvail) &&
            (0 == (bamRes = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                                 TRUE, dataBuf, uDataSize,
                                                 writeCBArgs, uFlags, spiDeviceBamCallBack))));

   /*Check if we got the last buffer, then check if EOT is required
    * then set the flags according before queuing to BAM */
   if ((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufAvail) && !bamRes)
   {
      uFlags = 0;

      if(spiBam->eot)
      {
         uFlags |= PLAT_BAM_DESC_FLAG_EOT |PLAT_BAM_DESC_FLAG_NWD | PLAT_BAM_DESC_FLAG_INT;
      }

      bamRes = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                    TRUE, dataBuf, uDataSize,
                                    writeCBArgs, uFlags, spiDeviceBamCallBack);
   }
#if 0  // disabling this because it causes BAM transfer failures - needs further investigation
   else if (SPIDEVICE_TRANSFER_BUFF_NONE == bufAvail)
   {
      /* Even if there is no output we need to send a Zero Length Packet */
      uFlags = 0;

      if (spiBam->eot)
      {
         uFlags |= PLAT_BAM_DESC_FLAG_EOT | PLAT_BAM_DESC_FLAG_NWD;
      }

      bamRes = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                    TRUE, 0, 0,
                                    writeCBArgs, uFlags, spiDeviceBamCallBack);
   }
#endif

   return bamRes;
}

static uint32
spiDeviceBamQueueReadDescs
(
   SpiDeviceBamType *spiBam
)
{
   uint32 bamRes = SPIDEVICE_RES_SUCCESS;
   SpiDeviceTransferDataResult bufAvail, bufNextAvail;
   uint8 *dataBuf;
   uint32 uDataSize;
   uint32 uFlags = PLAT_BAM_DESC_FLAG_NO_SUBMIT;
   SpiDeviceTransferType *transfer = spiBam->transfer;
   void *readCBArgs = (void *)&(spiBam->readArgs);

   /*If the buffer is not the last buffer of the list then queue to
    * platform BAM layer*/
   do
   {
      bamRes = SPIDEVICE_RES_SUCCESS;
      bufAvail = transfer->getReadBufPhys(transfer, &dataBuf);
      uDataSize = transfer->getReadBufLen(transfer);
      bufNextAvail = transfer->moveReadNext(transfer);
   } while((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufNextAvail) &&
           (0 == (bamRes = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                                FALSE, dataBuf, uDataSize,
                                                readCBArgs, uFlags, spiDeviceBamCallBack))));

   /*Check if we got the last buffer, then check if EOT is required
    * then set the flags according before queuing to BAM */
   if ((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufAvail) && !bamRes)
   {
      /*We always queue (meaning remove the PLAT_BAM_DESC_FLAG_NO_SUBMIT
       * flag) the last read transfer*/
      uFlags = 0;

      if(spiBam->eot)
      {
         uFlags |= PLAT_BAM_DESC_FLAG_EOT | PLAT_BAM_DESC_FLAG_INT;
      }

      bamRes = IBUSPLAT_BamTransfer(spiBam->bam_cfg.PlatHandle,
                                    FALSE, dataBuf, uDataSize,
                                    readCBArgs, uFlags, spiDeviceBamCallBack);
   }

   return bamRes;
}

uint32
spiDeviceBamStartTransfer
(
   SpiDeviceBamType *spiBam,
   SpiDeviceTransferType *transfer,
   void *pUser,
   boolean eot
)
{
   uint32 res = SPIDEVICE_RES_SUCCESS;
   uint32 do_while_constant = 0;

   do
   {
      spiBam->result.pUser = pUser;
      spiBam->transfer = transfer;
      spiBam->eot = eot;

      /*Queue the descriptors to BAM*/
      if (TRUE == spiBam->bam_cfg.UseCmdDesc)
      {
         res = spiDeviceBamQueueCmdDescs(spiBam);
         SPIDEVICEBAM_CHK_RESULT_BREAK(res);
      }

      res = spiDeviceBamQueueReadDescs(spiBam);
      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      res = spiDeviceBamQueueWriteDescs(spiBam);
      SPIDEVICEBAM_CHK_RESULT_BREAK(res);
   } while(do_while_constant);

   return res;
}

static uint32
spiDeviceBamSetTransferParameters
(
   SpiDeviceBamType *spiBam,
   HAL_QUPE_SPI_TransferParametersType *mHalTransferParameters
)
{
   return IBUSOS_Memcpy(&spiBam->mHalTransferParameters,
                 sizeof(spiBam->mHalTransferParameters),
                 mHalTransferParameters,
                 sizeof(spiBam->mHalTransferParameters));
}

static uint32
spiDeviceBamSetDeviceParameters
(
   SpiDeviceBamType *spiBam,
   HAL_QUPE_SPI_DeviceParametersType *mHalDeviceParameters
)
{
  return IBUSOS_Memcpy(&spiBam->mHalDeviceParameters, sizeof(spiBam->mHalDeviceParameters),
                 mHalDeviceParameters,
                 sizeof(spiBam->mHalDeviceParameters));
}

uint32 SpiDeviceBamInit
(
   SpiDeviceBam_Config *bam_cfg,
   SpiDeviceBamType **pSpiBam
)
{
   SpiDeviceBamType *spiBam = NULL;
   uint32 res = SPIDEVICE_RES_SUCCESS;
   uint32 return_value = SPIDEVICE_RES_SUCCESS;
   uint32 do_while_constant = 0;

   do
   {
      res = IBUSOS_Malloc((void **)&spiBam, sizeof(SpiDeviceBamType));
      if (SPIDEVICE_RES_SUCCESS != res)
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: spiBam Malloc failure\n");
         break;
      }

      IBUSOS_Memset((void *)spiBam, 0, sizeof(SpiDeviceBamType));
	  res = IBUSOS_Memcpy(&spiBam->bam_cfg, sizeof(spiBam->bam_cfg), bam_cfg, sizeof(SpiDeviceBam_Config));
      if (SPIDEVICE_RES_SUCCESS != res)
      {
         break;
      }

      spiBam->resetCmdDesc.physMemHandle = NULL;
      spiBam->settingsCmdDesc.physMemHandle = NULL;


	  res = IBUSOS_PhysMemAlloc(SETTINGS_CMD_BUFFER_SIZE,
                                     &(spiBam->settingsCmdDesc.physMemHandle),
                                     &(spiBam->settingsCmdDesc.virtualAddr),
                                     &(spiBam->settingsCmdDesc.physAddr));

      if (SPIDEVICE_RES_SUCCESS != res)
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: Settings cmd desc Malloc failure\n");
         break;
      }

	  res = IBUSOS_PhysMemAlloc(RESET_CMD_BUFFER_SIZE,
								 &(spiBam->resetCmdDesc.physMemHandle),
								 &(spiBam->resetCmdDesc.virtualAddr),
								 &(spiBam->resetCmdDesc.physAddr));

      if (SPIDEVICE_RES_SUCCESS != res)
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: Reset cmd desc Malloc failure\n");
         break;
      }

      spiBam->startBamTransfer = spiDeviceBamStartTransfer;
      spiBam->cancelBamTransfer = spiDeviceBamCancelTransfer;
      spiBam->setBamTransferCallback = spiDeviceBamSetTransferCallback;
      spiBam->setBamTransferCmdParms = spiDeviceBamSetTransferCmdParms;
      spiBam->setQupTransferParms = spiDeviceBamSetTransferParameters;
      spiBam->setQupDeviceParms = spiDeviceBamSetDeviceParameters;
      spiBam->readArgs.spiBamObj = spiBam;
      spiBam->readArgs.bamTags = SPIDEVICE_BAM_READ_TAG;
      spiBam->writeArgs.spiBamObj = spiBam;
      spiBam->writeArgs.bamTags = SPIDEVICE_BAM_WRITE_TAG;
      *pSpiBam = spiBam;

   } while(do_while_constant);

   if (SPIDEVICE_RES_SUCCESS != res)
   {
      return_value = res;

      res = SpiDeviceBamDeInit(spiBam);
   }

   return return_value;
}

uint32 SpiDeviceBamDeInit
(
   SpiDeviceBamType *spiBam
)
{
   uint32 result = SPIDEVICE_RES_SUCCESS;
   uint32 do_while_constant = 0;

   do
   {
      if (NULL == spiBam)
      {
         result = IBUSPLAT_ERROR_NULLPTR;
         break;
      }

      if (NULL != spiBam->resetCmdDesc.physMemHandle)
      {
         result = IBUSOS_PhysMemFree(spiBam->resetCmdDesc.physMemHandle);

         spiBam->resetCmdDesc.physMemHandle = NULL;
         spiBam->resetCmdDesc.virtualAddr = NULL;
         spiBam->resetCmdDesc.physAddr = NULL;

         if (SPIDEVICE_RES_SUCCESS != result)
         {
            SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: Free failed err 0x%x\n",
                          (unsigned int)result);
         }
      }

      if (NULL != spiBam->settingsCmdDesc.physMemHandle)
      {
         result = IBUSOS_PhysMemFree(spiBam->settingsCmdDesc.physMemHandle);

         spiBam->settingsCmdDesc.physMemHandle = NULL;
         spiBam->settingsCmdDesc.virtualAddr = NULL;
         spiBam->settingsCmdDesc.physAddr = NULL;

         if (SPIDEVICE_RES_SUCCESS != result)
         {
            SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: Free failed err 0x%x\n",
                          (unsigned int)result);
         }
      }

      result = IBUSOS_Free(spiBam);
	  spiBam = NULL;

   } while(do_while_constant);

   return result;
}
