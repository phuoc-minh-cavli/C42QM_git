/*==================================================================================================

FILE: SpiDalProps.h

DESCRIPTION:
   The file declares structures and functions to fetch device configuration properties 

   ====================================================================================================

  Edit History
   
  $Header: //components/rel/core.tx/6.0/buses/spi/src/device/SpiDalProps.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

When      Who  What, where, why
--------  ---  ------------------------------------------------------------
09/20/16  dpk  Ported for ThreadX
04/01/14  vmk  Initial version

====================================================================================================
                     Copyright (c) 2014-2016 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR
==================================================================================================*/
#include "DDIClock.h"
#include "icbarb.h"
#include "bam.h"
#include "SpiLog.h"
#include "DDITlmm.h"

typedef struct SpiDevProp_BamDescFifoType
{
   void     *phMemHandle;
   uint32   physAddr;
   uint32   virtualAddr;
}SpiDevProp_BamDescFifoType;   

typedef struct SpiDevProp_BamPipeCtxt
{
   uint32                 hBamPipeDev;  
   uint32                 devBamPipeNum;  
   bam_pipe_config_type   devBamPipeConfig;
   bam_callback_type      devBamPipeCallbackCfg;
   SpiDevProp_BamDescFifoType   devBamPipeDescFifo;
} SpiDevProp_BamPipeCtxt;

typedef struct SpiDevProp_BamCallbackType
{
   void *func; 
   void *data;
} SpiDevProp_BamCallbackType;

typedef struct SpiDevProp_BamDevCfgType
{
   uint32                    uBamDevId;
   uint32                    hBamDev;  
   bam_config_type           devBamConfig;
   SpiDevProp_BamCallbackType  devBamCallbackCfg;
   SpiDevProp_BamPipeCtxt      devBamRxPipeCtxt;  
   SpiDevProp_BamPipeCtxt      devBamTxPipeCtxt;  
} SpiDevProp_BamDevCfgType;

#define SPI_MAX_NUM_GPIOS 7
typedef struct SpiDevProp_DevTargetCfgType
{
   char *pQupHClkName;
   char *pQupAppClkName;
   uint32 uOsDevId;
   uint32 uQupCoreNum;
   ClockIdType QupHClkId;
   ClockIdType QupAppClkId;
   uint32 qupVirtBlockAddr;
   uint32 qupPhysBlockAddr;
   uint32 tcsrVirtBlockAddr;
   boolean bInterruptBased;
   boolean bBamSupported;
   boolean bTcsrInterruptRequired;
   boolean bUseCmdDesc;
   uint32 uTcsrInterruptBitMask;
   uint32 uTcsrInterruptBitShift;
   SpiDevProp_BamDevCfgType *spiBamDevCfg;
   uint32 uActiveDevCount;
   boolean bPnocEnable;
   uint64 uPnocIBVal;
   uint64 uPnocABVal;
   uint32 uPnocMaster;
   uint32 uPnocSlave;
   npa_client_handle pNpaHandle;
   DALGpioIdType nGpioId[SPI_MAX_NUM_GPIOS];
}SpiDevProp_DevTargetCfgType;

#define SPIDEVICEPLAT_CHK_RESULT_BREAK(res) if (SPIDEVICE_RES_SUCCESS !=(res)) break;

uint32 SpiDevProp_GetConfig(void *QupHandle, void *conf);
boolean SpiDevProp_SearchDevCfg (uint32 devId, SpiDevProp_DevTargetCfgType **ppDevCfg);
int32 SpiDevProp_RemoveDevCfg (uint32 uOsDevId);
