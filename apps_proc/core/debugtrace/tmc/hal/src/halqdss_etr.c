/*=============================================================================

FILE:         halqdss_etr.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/tmc/hal/src/halqdss_etr.c#1 $
==============================================================================*/
#include "halqdss_etr.h"
#include "qdss_etr_regs.h"

volatile uint32 QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE;

void HAL_qdss_etr_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE = nBaseAddr;
}

uint32 HAL_qdss_etr_GetRAMSize(void)
{
   return HWIO_IN(QDSS_ETR_RSZ); 
}


void HAL_qdss_etr_SetRAMSize(uint32 ram_size)
{
   HWIO_OUT(QDSS_ETR_RSZ,ram_size); 
}

/*FUNCTION HAL_qdss_etr_AccessUnLock*/
void  HAL_qdss_etr_AccessUnLock(void)
{
   HWIO_OUT(QDSS_ETR_LAR,0xC5ACCE55);   
}

/*FUNCTION HAL_qdss_etr_AccessLock*/
void  HAL_qdss_etr_AccessLock(void)
{ 
   HWIO_OUT(QDSS_ETR_LAR,(~0xC5ACCE55));   
}

void HAL_qdss_etr_SetMode(uint8 mode)
{
   HWIO_OUTF(QDSS_ETR_MODE,MODE,mode);
}


void HAL_qdss_etr_SetWriteBurstLength(uint8 nLen)
{
   HWIO_OUTF(QDSS_ETR_AXICTL,WRBURSTLEN,nLen);
}

void HAL_qdss_etr_DisableScatterGather(void)
{
   HWIO_OUTF(QDSS_ETR_AXICTL,SCATTERGATHERMODE,0);
}

#define HWIO_QDSS_ETR_AXICTL_CACHEPROTCTRL_SHFT    0x0
#define HWIO_QDSS_ETR_AXICTL_CACHEPROTCTRL_BMSK    0x3F

void HAL_qdss_etr_SetCacheProtCtrl(void)
{
   //non-cacheable, non-bufferable,  non-secure, normal
   HWIO_OUTF(QDSS_ETR_AXICTL,CACHEPROTCTRL,0x2);
}

void HAL_qdss_etr_SetDataBufferAddress(uint32 addr)
{

   HWIO_OUT(QDSS_ETR_DBALO,addr);
   HWIO_OUTF(QDSS_ETR_DBAHI,BUFADDRHI,0);
}


uint32 HAL_qdss_etr_GetDataBufferAddress(void)
{
   return HWIO_IN(QDSS_ETR_DBALO);
}




void HAL_qdss_etr_DisableTraceCapture(void)
{
   HWIO_OUTF(QDSS_ETR_CTL,TRACECAPTEN,0x0);
}


void HAL_qdss_etr_EnableTraceCapture(void)
{
   HWIO_OUTF(QDSS_ETR_CTL,TRACECAPTEN,0x1);
}

void HAL_qdss_etr_SetFFCR(uint32 dwVal)
{
   HWIO_OUT(QDSS_ETR_FFCR,dwVal);
}

void HAL_qdss_etr_Flush(void)
{
   HWIO_OUTF(QDSS_ETR_FFCR,FLUSHMAN_W,1);
}


void HAL_qdss_etr_FlushWait(void)
{
   do {
      //spin until flush is complete.
      //flush is fast, haven't seent it spin more than once.
   }while (HWIO_INF(QDSS_ETR_FFCR,FLUSHMAN_R));
}


void HAL_qdss_etr_EnableFormatting(void)
{
   HWIO_OUTF(QDSS_ETR_FFCR,ENFT,1);
}


void HAL_qdss_etr_SetFFCRETBDefault(void)
{
/*StopOnTrig|FOnTrigEvt|FOnFlIn|EnTI|EnFt*/
/*In agreement with HLOS team*/
   #define ETB_FFCR_DEFAULT 0x2033 
   HWIO_OUT(QDSS_ETR_FFCR,ETB_FFCR_DEFAULT);
}

uint32 HAL_qdss_etr_GetLatchBufferFill(void)
{
   return HWIO_IN(QDSS_ETR_LBUFLEVEL);
}


uint32 HAL_qdss_etr_GetWritePtr(void)
{
   return HWIO_IN(QDSS_ETR_RWP);
}


void HAL_qdss_etr_StopOnFlush(uint8 bEnable) 
{
   HWIO_OUTF(QDSS_ETR_FFCR,STOPONFL,bEnable);
}

void HAL_qdss_etr_WaitForTMCReady() 
{

   do {
      //spin until TMCReady
   }while (!HWIO_INF(QDSS_ETR_STS,TMCREADY));
}

uint32  HAL_qdss_etr_ReadData(void)
{ 
   return HWIO_IN(QDSS_ETR_RRD);   
}

boolean  HAL_qdss_etr_IsEnabled(void)
{
   return HWIO_INF(QDSS_ETR_CTL,TRACECAPTEN);
}
