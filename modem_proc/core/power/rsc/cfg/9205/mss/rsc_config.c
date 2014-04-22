/*==============================================================================
  FILE:         rsc_config.c

  OVERVIEW:     This file contains target specific data/configuration for rsc
                instances.

  DEPENDENCIES: None
                
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "rsc.h"
#include "rsc_config.h"
#include "rsc_common.h"
#include "rsc_hwio.h"

/*==========================================================================
 *                        QDSP6 RSC CONFIGURATIONS
 *=========================================================================*/
static uint32 g_rscChildSequence[Q6SS_RSCC_CSS_NUMSEQWORDS] = 
{
//SHORT_APCR_SEQ:
  0x86a601a4 ,  //0x00:  0xa4   	SET1 rsc_clk_halt
				//0x01:  0x1        DELI 52ns
			    //0x02:  0xa6       SET1 rsc_save
				//0x03:  0x86       SET0 rsc_save
  0xa5abe9a9 ,  //0x04:  0xa9       SET1 rsc_mem_periph_slp
				//0x05:  0xe9       WAIT1 rsc_mem_periph_pd_event
				//0x06:  0xab       SET1 rsc_clamp_qmc_mem
  			    //0x07:  0xa5       SET1 rsc_clamp_io
  0xc6aeacad ,	//0x08:  0xad       SET1 rsc_ldo_bypass_open
				//0x09:  0xac       SET1 rsc_bhs_off
				//0x0a:  0xae       SET1 rsc_ldo_pd
				//0x0b:  0xc6       WAIT0 rsc_pwr_on_event
  0xa081e1a1 ,	//0x0c:  0xa1       SET1 rsc_child_parent_shutdown_req
				//0x0d:  0xe1       WAIT1 rsc_parent_child_shutdown_ack
				//0x0e:  0x81       SET0 rsc_child_parent_shutdown_req
				//0x0f:  0xa0       SET1 rsc_seq_wait_wakeup
  0xa3e2a2e0 ,	//0x10:  0xe0       WAIT1 rsc_core_rsc_bringup_req
				//0x11:  0xa2       SET1 rsc_child_parent_bringup_req
				//0x12:  0xe2       WAIT1 rsc_parent_child_bringup_ack
				//0x13:  0xa3       SET1 rsc_ares
  0xe68c8ebf ,	//0x14:  0xbf       SET1 rsc_mem_stagger_ares
				//0x15:  0x8e       SET0 rsc_ldo_pd
				//0x16:  0x8c       SET0 rsc_bhs_off
				//0x17:  0xe6       WAIT1 rsc_pwr_on_event
  0x8587a78d ,	//0x18:  0x8d       SET0 rsc_ldo_bypass_open
				//0x19:  0xa7       SET1 rsc_restore
				//0x1a:  0x87   	SET0 rsc_restore
				//0x1b:  0x85   	SET0 rsc_clamp_io
  0x9eedbe9f ,	//0x1c:  0x9f   	SET0 rsc_mem_stagger_ares
				//0x1d:  0xbe   	SET1 rsc_mem_stagger_restore
				//0x1e:  0xed   	WAIT1 rsc_mem_stagger_restore_event
				//0x1f:  0x9e   	SET0 rsc_mem_stagger_restore
  0x83e4898b ,	//0x20:  0x8b   	SET0 rsc_clamp_qmc_mem
				//0x21:  0x89   	SET0 rsc_mem_periph_slp
				//0x22:  0xe4   	WAIT1 rsc_mem_periph_ready_event
				//0x23:  0x83   	SET0 rsc_ares
  0x208495b5 ,	//0x24:  0xb5   	SET1 rsc_pm_irq
				//0x25:  0x95   	SET0 rsc_pm_irq
				//0x26:  0x84   	SET0 rsc_clk_halt
				//0x27:  0x20   	RETURN

//LONG_APCR_SEQ_NOTCS:
		
  0xa601e3a4 ,	//0x01:  0xa4   	SET1 rsc_clk_halt
				//0x02:  0xe3   	WAIT1 rsc_core_clk_off_event
				//0x03:  0x1   	    DELI 52ns
				//0x04:  0xa6   	SET1 rsc_save
  0xaae9a986 ,	//0x05:  0x86   	SET0 rsc_save
				//0x06:  0xa9   	SET1 rsc_mem_periph_slp
				//0x07:  0xe9   	WAIT1 rsc_mem_periph_pd_event
				//0x08:  0xaa   	SET1 rsc_mem_core_slp
  0xaca5abea ,	//0x09:  0xea   	WAIT1 rsc_mem_core_pd_event
				//0x0a:  0xab   	SET1 rsc_clamp_qmc_mem
				//0x0b:  0xa5   	SET1 rsc_clamp_io
				//0x0c:  0xac   	SET1 rsc_bhs_off
  0xb1b0a8c6 ,	//0x0d:  0xc6   	WAIT0 rsc_pwr_on_event
				//0x0e:  0xa8   	SET1 rsc_ext_xo_enable
				//0x0f:  0xb0   	SET1 rsc_pll_outdis
				//0x10:  0xb1   	SET1 rsc_pll_reset
  0xa1b4b2b3 ,	//0x11:  0xb3   	SET1 rsc_pll_bypass
				//0x12:  0xb2   	SET1 rsc_pll_freeze
				//0x13:  0xb4   	SET1 rsc_clamp_mss2cx
				//0x14:  0xa1   	SET1 rsc_child_parent_shutdown_req
  0xe0a081e1 ,	//0x15:  0xe1   	WAIT1 rsc_parent_child_shutdown_ack
				//0x16:  0x81   	SET0 rsc_child_parent_shutdown_req
				//0x17:  0xa0   	SET1 rsc_seq_wait_wakeup
				//0x18:  0xe0   	WAIT1 rsc_core_rsc_bringup_req
  0xbfa3e2a2 ,	//0x19:  0xa2   	SET1 rsc_child_parent_bringup_req
				//0x1a:  0xe2   	WAIT1 rsc_parent_child_bringup_ack
				//0x1b:  0xa3   	SET1 rsc_ares
				//0x1c:  0xbf   	SET1 rsc_mem_stagger_ares
  0x05939294 ,	//0x1d:  0x94   	SET0 rsc_clamp_mss2cx
				//0x1e:  0x92   	SET0 rsc_pll_freeze
				//0x1f:  0x93   	SET0 rsc_pll_bypass
				//0x20:  0x5 	    DELI 1040ns
  0x05050505 ,	//0x21:  0x5   	    DELI 1040ns
				//0x22:  0x5   	    DELI 1040ns
				//0x23:  0x5   	    DELI 1040ns
				//0x24:  0x5   	    DELI 1040ns
  0xe68c0691 ,	//0x25:  0x91   	SET0 rsc_pll_reset
				//0x26:  0x6   	    DELI 5044ns
				//0x27:  0x8c   	SET0 rsc_bhs_off
				//0x28:  0xe6   	WAIT1 rsc_pwr_on_event 
  0x8587a7eb ,	//0x29:  0xeb   	WAIT1 rsc_mem_refresh_event
				//0x2a:  0xa7   	SET1 rsc_restore
				//0x2b:  0x87   	SET0 rsc_restore
				//0x2c:  0x85   	SET0 rsc_clamp_io
  0x8890e79f ,	//0x2d:  0x9f   	SET0 rsc_mem_stagger_ares
				//0x2e:  0xe7   	WAIT1 rsc_pll_ready_event
				//0x2f:  0x90   	SET0 rsc_pll_outdis
				//0x30:  0x88   	SET0 rsc_ext_xo_enable
  0x8b9eedbe ,	//0x31:  0xbe   	SET1 rsc_mem_stagger_restore
				//0x32:  0xed   	WAIT1 rsc_mem_stagger_restore_event
				//0x33:  0x9e   	SET0 rsc_mem_stagger_restore
				//0x34:  0x8b   	SET0 rsc_clamp_qmc_mem
  0xe489e58a ,	//0x35:  0x8a   	SET0 rsc_mem_core_slp
				//0x36:  0xe5   	WAIT1 rsc_mem_core_ready_event
				//0x37:  0x89   	SET0 rsc_mem_periph_slp
				//0x38:  0xe4   	WAIT1 rsc_mem_periph_ready_event
  0x8495b583 ,	//0x39:  0x83   	SET0 rsc_ares
				//0x3a:  0xb5   	SET1 rsc_pm_irq
				//0x3b:  0x95   	SET0 rsc_pm_irq
				//0x3c:  0x84   	SET0 rsc_clk_halt
  0x00000020 ,  //0x3d:  0x20 	    RETURN  harshal
};              

/* QDSP6 mode configurations. */
static rsc_modes_t g_rscChildModes[] =
{
  {
    RSC_MODE_APCR_PLL_ON,                 // mode_id
    Q6SS_RSCC_CSS_SHORT_APCR_SEQ,     // start_address
  },
  {
    RSC_MODE_PWRC,
    Q6SS_RSCC_CSS_LONG_APCR_SEQ_NOTCS,
  },
};

rsc_t g_rscInstances[] =
{
  {
    RSC_ARRAY(g_rscChildModes),     //Instance modes
    g_rscChildSequence,             //Instance sequence
    Q6SS_RSCC_CSS_NUMSEQWORDS,
    NULL,                           //branch info (static)
    Q6SS_RSCC_CSS_NUMBRANCHES,
    RSC_SS_BASE + 0x00130000,
  }
};

/* Variable to indicate rsc instance counts */
uint32 g_rscInstCount = sizeof(g_rscInstances) / sizeof(g_rscInstances[0]);