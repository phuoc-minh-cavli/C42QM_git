/*=========================================================================

                       TPDM HAL

GENERAL DESCRIPTION
   This file implements generic HAL functionalities for a TPDM




      Copyright (c) 2015, 2019 Qualcomm Technologies Incorporated.
      All Rights Reserved.
      Qualcomm Confidential and Proprietary
==========================================================================*/

/*==========================================================================
 $Header: //components/rel/core.tx/6.0/debugtrace/tpdm/hal/src/halqdss_tpdm.c#1 $
==========================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/
#include "hal_qdss_tpdm.h"
#include "hwio_qdss_tpdm.h"
#include "HALhwio.h"



static volatile uint32 QDSS_TPDM_BLOCK_BASE;

void HAL_qdss_tpdm_SetBaseAddress(uint32 virtAddr)
{
   QDSS_TPDM_BLOCK_BASE=virtAddr;
}

void HAL_qdss_tpdm_cmb_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_CMB_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_cmb_get_enable_disable(void)
{
   return HWIO_INF(TPDM_CMB_CR,E);
}


void HAL_qdss_tpdm_dsb_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_DSB_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_dsb_get_enable_disable(void)
{
   return HWIO_INF(TPDM_DSB_CR,E);
}


void HAL_qdss_tpdm_bc_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_BC_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_bc_get_enable_disable(void)
{
   return HWIO_INF(TPDM_BC_CR,E);
}


void HAL_qdss_tpdm_tc_set_enable_disable(uint32 enable_bit)
{
   HWIO_OUTF(TPDM_TC_CR,E,enable_bit);
}

uint32 HAL_qdss_tpdm_tc_get_enable_disable(void)
{
   return HWIO_INF(TPDM_TC_CR,E);
}


void HAL_qdss_tpdm_impldef_set_enable_disable(uint32 enable_bit)
{
   /*not implemented*/
   return;
}

void HAL_qdss_tpdm_cmb_set_mode(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_CR,MODE,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_mode(void)
{
   return HWIO_INF(TPDM_CMB_CR,MODE);
}


void HAL_qdss_tpdm_cmb_set_flow_control(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_CR,FLOWCTRL,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_flow_control(void)
{
   return HWIO_INF(TPDM_CMB_CR,FLOWCTRL);
}


void HAL_qdss_tpdm_cmb_set_patt_match(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_TIER,PATT_TSENAB,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_patt_match(void)
{
   return HWIO_INF(TPDM_CMB_TIER,PATT_TSENAB);
}

void HAL_qdss_tpdm_cmb_set_ext_trigger_on_off(uint32 bit)
{
   HWIO_OUTF(TPDM_CMB_TIER,XTRIG_TSENAB,bit);
}

uint32 HAL_qdss_tpdm_cmb_get_ext_trigger_on_off(void)
{
   return HWIO_INF(TPDM_CMB_TIER,XTRIG_TSENAB);
}

void HAL_qdss_tpdm_cmb_set_ts_all(uint32 bEnable)
{
   HWIO_OUTF(TPDM_CMB_TIER,TS_ALL,bEnable);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_all(void)
{
   return HWIO_INF(TPDM_CMB_TIER,TS_ALL);
}


void HAL_qdss_tpdm_cmb_set_ts_patt_val_lsw(uint32 val)
{
   HWIO_OUTI(TPDM_CMB_TPRn,0,val);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_val_lsw(void)
{
   return HWIO_INI(TPDM_CMB_TPRn,0);
}

void HAL_qdss_tpdm_cmb_set_ts_patt_val_msw(uint32 val)
{
   HWIO_OUTI(TPDM_CMB_TPRn,1,val);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_val_msw(void)
{
   return HWIO_INI(TPDM_CMB_TPRn,1);
}


void HAL_qdss_tpdm_cmb_set_ts_patt_mask_lsw(uint32 mask)
{
   HWIO_OUTI(TPDM_CMB_TPMRn,0,mask);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_mask_lsw(void)
{
   return HWIO_INI(TPDM_CMB_TPMRn,0);
}


void HAL_qdss_tpdm_cmb_set_ts_patt_mask_msw(uint32 mask)
{
   HWIO_OUTI(TPDM_CMB_TPMRn,1,mask);
}

uint32 HAL_qdss_tpdm_cmb_get_ts_patt_mask_msw(void)
{
   return HWIO_INI(TPDM_CMB_TPMRn,1);
}

uint32 HAL_qdss_tpdm_cmb_get_mcmb_lanes(void)
{
   return HWIO_INF(TPDM_CMB_CR,E_LN);
}

void HAL_qdss_tpdm_cmb_set_mcmb_lanes(uint32 lanes)
{
   HWIO_OUTF(TPDM_CMB_CR,E_LN,lanes);
}
      
uint32 HAL_qdss_tpdm_cmb_get_mcmb_ca_mode(void)
{
   return HWIO_INF(TPDM_CMB_CR,CA);
}

void HAL_qdss_tpdm_cmb_set_mcmb_ca_mode(uint32 mode)
{
   HWIO_OUTF(TPDM_CMB_CR,CA,mode);
}

void HAL_qdss_tpdm_cmb_set_mcmb_ca_xtrig_lnsel(uint32 xtrig_lnsel)
{
   HWIO_OUTF(TPDM_CMB_CR,XTRIG_LNSEL,xtrig_lnsel);
}

uint32 HAL_qdss_tpdm_cmb_get_mcmb_ca_xtrig_lnsel(void)
{
   return HWIO_INF(TPDM_CMB_CR,XTRIG_LNSEL);
}



void HAL_qdss_tpdm_dsb_set_mode(uint32 mode)
{
   HWIO_OUTF(TPDM_DSB_CR,MODE,mode);
}

uint32 HAL_qdss_tpdm_dsb_get_mode(void)
{
   return HWIO_INF(TPDM_DSB_CR,MODE);
}

void HAL_qdss_tpdm_dsb_set_patt_type(uint8 type)
{
   HWIO_OUTF(TPDM_DSB_TIER,PATT_TYPE,type);
}

uint8 HAL_qdss_tpdm_dsb_get_patt_type(void)
{
   return HWIO_INF(TPDM_DSB_TIER,PATT_TYPE);
}


void HAL_qdss_tpdm_dsb_set_ext_trigger_on_off(uint32 on_off_bit)
{
   HWIO_OUTF(TPDM_DSB_TIER,XTRIG_TSENAB,on_off_bit);
}

uint32 HAL_qdss_tpdm_dsb_get_ext_trigger_on_off(void)
{
   return HWIO_INF(TPDM_DSB_TIER,XTRIG_TSENAB);
}


void HAL_qdss_tpdm_dsb_set_patt_match(uint32 on_off_bit)
{
   HWIO_OUTF(TPDM_DSB_TIER,PATT_TSENAB,on_off_bit);
}

uint32 HAL_qdss_tpdm_dsb_get_patt_match(void)
{
   return HWIO_INF(TPDM_DSB_TIER,PATT_TSENAB);
}

void HAL_qdss_tpdm_dsb_set_edcmr(uint32 index,uint32 val)
{
   HWIO_OUTI(TPDM_DSB_EDCMRn,index,val);
}

uint32 HAL_qdss_tpdm_dsb_get_edcmr(uint32 index)
{
   return HWIO_INI(TPDM_DSB_EDCMRn,index);
}


void HAL_qdss_tpdm_dsb_set_tpmr(uint32 index,uint32 val)
{
   HWIO_OUTI(TPDM_DSB_TPMRn,index,val);
}



#define TPDM_GPR_NUM_REGS 160 //refer arch. spec   

int  HAL_qdss_tpdm_SetGPR(uint32 GPRIndex,uint32 val)
{
   if (GPRIndex >=TPDM_GPR_NUM_REGS ) {
      return -1;
   }
   HWIO_OUTI(TPDM_GPRn,GPRIndex,val);
   return 0;
}

int HAL_qdss_tpdm_GetGPR(uint32 GPRIndex, uint32 *pval)
{
   if (GPRIndex >=TPDM_GPR_NUM_REGS ) {
      return -1;
   }
   *pval=HWIO_INI(TPDM_GPRn,GPRIndex);
   return 0;
}

#define TPDM_DSB_MSR_MAX    32
#define TPDM_DSB_MSR_OFFSET 0x980



int HAL_qdss_tpdm_SetDSBMSR(uint32 num,uint32 val)
{
   uint32 addr;
   uint32 dsb_enabled;

   if (num >= TPDM_DSB_MSR_MAX ) {
      return -1;
   }
   addr = QDSS_TPDM_BLOCK_BASE + TPDM_DSB_MSR_OFFSET + (num * 4);

   //HW erratta requires DSB_CR E bit to be set to be able to set MSR registers
   dsb_enabled = HAL_qdss_tpdm_dsb_get_enable_disable();
   if (!dsb_enabled) {
      HAL_qdss_tpdm_dsb_set_enable_disable(1);
   }
   
   outpdw(addr,val);
   //We loose the value, if E bit is reset after setting MSR register
      if (!dsb_enabled) {
      HAL_qdss_tpdm_dsb_set_enable_disable(0);
   }

   
   return 0;
}

int HAL_qdss_tpdm_GetDSBMSR(uint32 num,uint32 *pval)
{
   uint32 addr;
   if (num >= TPDM_DSB_MSR_MAX ) {
      return -1;
   }
   addr = QDSS_TPDM_BLOCK_BASE + TPDM_DSB_MSR_OFFSET + (num * 4);
   *pval = inpdw(addr);
   return 0;
}


#define TPDM_CMB_MSR_MAX    128
#define TPDM_CMB_MSR_OFFSET 0xA80


int HAL_qdss_tpdm_SetCMBMSR(uint32 num,uint32 val)
{
   uint32 addr;

   if (num >= TPDM_CMB_MSR_MAX ) {
      return -1;
   }
   addr = QDSS_TPDM_BLOCK_BASE + TPDM_CMB_MSR_OFFSET + (num * 4);
   outpdw(addr,val);
   return 0;
}

int HAL_qdss_tpdm_GetCMBMSR(uint32 num,uint32 *pval)
{
   uint32 addr;
   if (num >= TPDM_CMB_MSR_MAX ) {
      return -1;
   }
   addr = QDSS_TPDM_BLOCK_BASE + TPDM_CMB_MSR_OFFSET + (num * 4);
   *pval = inpdw(addr);
   return 0;
}

void HAL_qdss_tpdm_unlock_access(void)
{
   HWIO_OUT(TPDM_LAR,0xC5ACCE55);
}

void HAL_qdss_tpdm_lock_access(void)
{
   HWIO_OUT(TPDM_LAR,~0xC5ACCE55);
}
