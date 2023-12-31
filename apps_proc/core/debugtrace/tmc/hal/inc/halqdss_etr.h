#ifndef HALQDSS_ETR_H
#define HALQDSS_ETR_H

/*=============================================================================

FILE:         halqdss_etr.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/tmc/hal/inc/halqdss_etr.h#1 $
==============================================================================*/
#include "HALcomdef.h"

void HAL_qdss_etr_SetRAMSize(uint32 ram_size);
uint32 HAL_qdss_etr_GetRAMSize(void);


#define ETR_MODE_CIRCULAR_BUFFER  0x0
#define ETR_MODE_SW_FIFO          0x1
#define ETR_MODE_HW_FIFO          0x2
#define ETR_MODE_RESERVED         0x3

void HAL_qdss_etr_HalConfigInit(uint32 dwBaseAddr);
void  HAL_qdss_etr_AccessUnLock(void);
void  HAL_qdss_etr_AccessLock(void);
void HAL_qdss_etr_SetMode(uint8 mode);
void HAL_qdss_etr_SetWriteBurstLength(uint8 nLen);
void HAL_qdss_etr_DisableScatterGather(void);
void HAL_qdss_etr_SetCacheProtCtrl(void);
void HAL_qdss_etr_SetDataBufferAddress(uint32 addr);
uint32 HAL_qdss_etr_GetDataBufferAddress(void);

void HAL_qdss_etr_DisableTraceCapture(void);
void HAL_qdss_etr_EnableTraceCapture(void);
void HAL_qdss_etr_SetFFCR(uint32 dwVal);
void HAL_qdss_etr_Flush(void);
void HAL_qdss_etr_FlushWait(void);
void HAL_qdss_etr_EnableFormatting(void);
void HAL_qdss_etr_SetFFCRETBDefault(void);
uint32 HAL_qdss_etr_GetLatchBufferFill(void);
uint32 HAL_qdss_etr_GetWritePtr(void);
void HAL_qdss_etr_StopOnFlush(uint8 bEnable);
void HAL_qdss_etr_WaitForTMCReady(void); 
uint32  HAL_qdss_etr_ReadData(void);
boolean  HAL_qdss_etr_IsEnabled(void);


#endif //HALQDSS_ETR_H
