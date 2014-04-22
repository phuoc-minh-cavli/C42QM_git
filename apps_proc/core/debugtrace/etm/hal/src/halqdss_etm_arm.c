/*=============================================================================

FILE:         halqdss_etm_arm.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_etm_arm_regs.h"
#include "DALSys.h"
#include "qdss_etm_utils.h"
#include "busywait.h"
#include "qurt_memory.h"

volatile uint32 QDSS_ETMARM_REG_BASE;
//extern void memory_barrier(void);

void HAL_qdss_etmarm_HalConfigInit(uint32 dwBaseAddr)
{
   QDSS_ETMARM_REG_BASE  = dwBaseAddr;
}


void  HAL_qdss_etmarm_AccessUnLock(void)
{
   HWIO_OUT(QDSS_ETMARM_LAR,0xC5ACCE55);   
   memory_barrier();
}

void  HAL_qdss_etmarm_AccessLock(void)
{ 
   HWIO_OUT(QDSS_ETMARM_LAR,(~0xC5ACCE55));
}


uint32 HAL_qdss_etmarm_Get_CCR(void)
{
   return HWIO_IN(QDSS_ETMARM_CCR);
}

uint32 HAL_qdss_etmarm_Get_CR(void)
{
   return HWIO_IN(QDSS_ETMARM_CR);
}

uint32 HAL_qdss_etmarm_Get_SR(void)
{
   return HWIO_IN(QDSS_ETMARM_SR);
}


uint32 HAL_qdss_etmarm_Get_IDR(void)
{
   return HWIO_IN(QDSS_ETMARM_IDR);
}

uint32 HAL_qdss_etmarm_Get_PDCR(void)
{
   return HWIO_IN(QDSS_ETMARM_PDCR);
}


void HAL_qdss_etmarm_Set_AUXCR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_AUXCR,val);
}

void HAL_qdss_etmarm_Set_CIDCMR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_CIDCMR,val);
}

void HAL_qdss_etmarm_Set_CR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_CR,val);
}


void HAL_qdss_etmarm_Set_EXTINSELR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_EXTINSELR,val);
}

void HAL_qdss_etmarm_Set_FFLR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_FFLR,val);
}

void HAL_qdss_etmarm_Set_PDCR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_PDCR,val);
}

void HAL_qdss_etmarm_Set_SQ12EVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQ12EVR,val);
}

void HAL_qdss_etmarm_Set_SQ13EVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQ13EVR,val);
}

void HAL_qdss_etmarm_Set_SQ21EVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQ21EVR,val);
}

void HAL_qdss_etmarm_Set_SQ23EVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQ23EVR,val);
}

void HAL_qdss_etmarm_Set_SQ31EVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQ31EVR,val);
}

void HAL_qdss_etmarm_Set_SQ32EVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQ32EVR,val);
}

void HAL_qdss_etmarm_Set_SQR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SQR,val);
}

void HAL_qdss_etmarm_Set_SYNCFR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_SYNCFR,val);
}

void HAL_qdss_etmarm_Set_TECR1(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TECR1,val);
}

void HAL_qdss_etmarm_Set_TECR2(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TECR2,val);
}

void HAL_qdss_etmarm_Set_TEEVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TEEVR,val);
}

void HAL_qdss_etmarm_Set_TRACEIDR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TRACEIDR,val);
}

void HAL_qdss_etmarm_Set_TRIGGER(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TRIGGER,val);
}

void HAL_qdss_etmarm_Set_TSEVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TSEVR,val);
}

void HAL_qdss_etmarm_Set_TSSCR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_TSSCR,val);
}

void HAL_qdss_etmarm_Set_VDCR1(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_VDCR1,val);
}

void HAL_qdss_etmarm_Set_VDCR3(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_VDCR3,val);
}

void HAL_qdss_etmarm_Set_VDEVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_VDEVR,val);
}

void HAL_qdss_etmarm_Set_VMIDCVR(uint32 val)
{
   HWIO_OUT(QDSS_ETMARM_VMIDCVR,val);
}

void HAL_qdss_etmarm_Set_ACTRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_ACTRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_ACVRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_ACVRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_CIDCVRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_CIDCVRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_CNTENRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_CNTENRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_CNTRLDVRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_CNTRLDVRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_CNTVRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_CNTVRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_DCMRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_DCMRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_DCVRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_DCVRn_ADDR(i),val);
}

void HAL_qdss_etmarm_Set_EXTOUTEVRn(uint8 i,uint32 val)
{
   out_dword(HWIO_QDSS_ETMARM_EXTOUTEVRn_ADDR(i),val);
}



void HAL_qdss_etm_set_power_down(void)
{
	uint32 etmcr;

	memory_barrier();
	etmcr = HAL_qdss_etmarm_Get_CR();
	etmcr |= bit(0);
	HAL_qdss_etmarm_Set_CR(etmcr);
}

void HAL_qdss_etm_clear_power_down(void)
{
	uint32 etmcr;

	etmcr = HAL_qdss_etmarm_Get_CR();
	etmcr &= ~bit(0);
	HAL_qdss_etmarm_Set_CR(etmcr);
	memory_barrier();
}

void HAL_qdss_etm_set_power_up(void)
{
	uint32 etmpdcr;

   etmpdcr = HAL_qdss_etmarm_Get_PDCR();
   etmpdcr |= bit(3);
   HAL_qdss_etmarm_Set_PDCR( etmpdcr);
	memory_barrier();
}

void HAL_qdss_etm_clear_power_up(void)
{
	uint32 etmpdcr;

	memory_barrier();

   etmpdcr = HAL_qdss_etmarm_Get_PDCR();
   etmpdcr &= ~bit(3);
   HAL_qdss_etmarm_Set_PDCR( etmpdcr);
}

#define QDSS_ETM_TIMEOUT_US		(200)

void HAL_qdss_etm_wait_prog(void)
{
	int i;

	memory_barrier();
	for (i = QDSS_ETM_TIMEOUT_US; bit_value(HAL_qdss_etmarm_Get_SR(), 1) != 1
           && i > 0; i--) {
			busywait(1);
   }

}
void HAL_qdss_etm_set_prog(void)
{
	uint32 etmcr;

	etmcr = HAL_qdss_etmarm_Get_CR();
	etmcr |= bit(10);
	HAL_qdss_etmarm_Set_CR(etmcr);
   HAL_qdss_etm_wait_prog();
}



void HAL_qdss_etm_clear_prog(void)
{
	uint32 etmcr;

	etmcr = HAL_qdss_etmarm_Get_CR();
	etmcr &= ~bit(10);
	HAL_qdss_etmarm_Set_CR(etmcr);
   HAL_qdss_etm_wait_prog();
}
