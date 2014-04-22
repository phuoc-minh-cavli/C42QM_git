/*==============================================================================
  FILE:         q6_lpm_config_v67.c

  OVERVIEW:     This file implements the APIs from q6_lpm_config header file
                on subsystems for specific Q6 versions.

  NOTE:         This file depends on how Q6 core is integrated within a 
                subystem.
                
                Based on HPG_QDSP6SS_v67_1.0.docm (rev 0.6 - March 27, 2018)

  DEPENDENCIES: None

                Copyright (c) 2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/target/q6_lpm_config_v67.c#3 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "rsc.h"
#include "q6_lpm_config.h"
#include "CoreVerify.h"
#include "uSleep_target.h"
#include "uSleep_hwio.h"


/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
/* g_sleepTargetConfig stores target specific config */
extern uSleep_target_config g_uSleepTargetConfig;


/*==============================================================================
 *                             GLOBAL FUNCTIONS
 *============================================================================*/
/*
 * q6LPMConfig_setupBase
 */
void q6LPMConfig_setupBase(void)
{
  /* Configure Q6SS registers that are invariant across all cpu_vdd low-power modes */
  USLEEP_HWIO_OUT(SLPC_CFG, 
                 USLEEP_HWIO_FVAL(SLPC_CFG, MEM_FAST_STAGGER,    0x1) | /* XO dual edge */
                 USLEEP_HWIO_FVAL(SLPC_CFG, IDLE_CORE_CLK_EN,    0x0) |
                 USLEEP_HWIO_FVAL(SLPC_CFG, EXTHW_WAKE_EN,       0x0) | /* Wake from ext hw trig */
                 USLEEP_HWIO_FVAL(SLPC_CFG, MEM_PU_PERI_STAGGER, 0x1) | /* mem periph min in-rush */
                 USLEEP_HWIO_FVAL(SLPC_CFG, MEM_PD_PERI_STAGGER, 0x0) |
                 USLEEP_HWIO_FVAL(SLPC_CFG, MEM_PU_ARRY_STAGGER, 0x1) | /* mem data array min in-runsh */
                 USLEEP_HWIO_FVAL(SLPC_CFG, MEM_PD_ARRY_STAGGER, 0x0) |
                 USLEEP_HWIO_FVAL(SLPC_CFG, WAKEUP_IN_EN,        0x0) | /* Default - mode based */
                 USLEEP_HWIO_FVAL(SLPC_CFG, CLK_GATING_MODE,     0x0)); /* Default - mode based */

  /*USLEEP_HWIO_OUT(CORE_MEM_STAGGER_CTL,
                 USLEEP_HWIO_FVAL( CORE_MEM_STAGGER_CTL,  DIV, 0x03));*/

  USLEEP_HWIO_OUT(WAKEUP_CTL,
                  USLEEP_HWIO_FVAL(WAKEUP_CTL,L2VIC_WAKE_EN, 0x01)); //needed?

  USLEEP_HWIO_OUT(RSCCTL_EN_CLAMP, 0x3);

#ifdef SLEEP_SUPPORT_V67_1_1
  USLEEP_HWIO_OUT(RSCCTL_EN_CLK,   0x3ff);
#else
  USLEEP_HWIO_OUT(RSCCTL_EN_CLK,   0x7ff);
#endif
  
  USLEEP_HWIO_OUT(RSCCTL_EN_MEM_CTL,
                 USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_CTL, CLAMP_QMC_MEM,  0x1) |
                 USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_CTL, MEM_PERIPH,     0x1));

  USLEEP_HWIO_OUT(RSCCTL_EN_MEM_STAGGER,
                 USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_STAGGER, STAGGER_RESTORE,    0x1) |
                 USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_STAGGER, STAGGER_ALT_ARES,   0x0) |
                 USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_STAGGER, STAGGER_FUNC_ARES,  0x1));

  USLEEP_HWIO_OUTF(RSCCTL_EN_BHS, BHS, 0x01);

  USLEEP_HWIO_OUT(RSCCTL_EN_ARES, 0x1FFFF);


  USLEEP_HWIO_OUT(RSCCTL_EN_PLL,
                 USLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_FREEZE,   0x0) | /* pwr down PLL */
                 USLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_OUTDIS,   0x1) |
                 USLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_BYPASS,   0x1) | 
                 USLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_RESET,    0x1)); /* PLL 0=standby/freeze, 1=off */

  USLEEP_HWIO_OUT( RSCCTL_EN_EXT, 
                 USLEEP_HWIO_FVAL( RSCCTL_EN_EXT, XO_ENABLE,  0x1 ) |
                 USLEEP_HWIO_FVAL( RSCCTL_EN_EXT, PLL_OUTDIS, 0x1 ) |
                 USLEEP_HWIO_FVAL( RSCCTL_EN_EXT, PLL_BYPASS, 0x1 ) |
                 USLEEP_HWIO_FVAL( RSCCTL_EN_EXT, PLL_RESET,  0x1 ) ); 

  USLEEP_HWIO_OUT(RSC_EVENT_EXTPLL_OVR,
                 USLEEP_HWIO_FVAL(RSC_EVENT_EXTPLL_OVR, EXT_PLL_OVRRD, 0x0)); 

 /* USLEEP_HWIO_OUT(RSC_EVENT_PLL_OVR,
                 USLEEP_HWIO_FVAL(RSC_EVENT_PLL_OVR, PLL_DYN_OVRRD,  0x1) |
                 USLEEP_HWIO_FVAL(RSC_EVENT_PLL_OVR, PLL_FINE_OVRRD, 0x1) |
                 USLEEP_HWIO_FVAL(RSC_EVENT_PLL_OVR, PLL_OVRRD,      0x0));  Needed?*/

  USLEEP_HWIO_OUT(RSC_EVENT_MEM_OVR, 0x0);

  /* Default to no handshaking with parent / hwsolver */
  uSleepTarget_setRpmHandshake(FALSE);

  /* Always defer non-wakeup qTimer frames (0 and 1) because the local wakeup timer is
   * always programmed */
  //USLEEP_HWIO_OUT(L2VIC_WAKEUP_EN, WAKEUP_EN_QTMR_FRAME_2);
  return;
}

/*
 * q6LPMConfig_setupModeConfig
 */
void q6LPMConfig_setupModeConfig(rsc_low_power_mode_type coreMode, 
                                 boolean                 flag)
{
  uint32 q6WakeHandshake  = 0;
  uint32 wakeupInterrupt  = 0;
  uint32 saveRestore      = 0;
  uint32 configRsc        = 0;
#ifdef SLEEP_IMAGE_CDSP
  uint32 q6vtcm           = 0;
#endif
#ifdef SLEEP_IMAGE_MSS
  uint32 l2tcm            = 0;
#endif

  switch(coreMode)
  {
    case RSC_MODE_APCR_PLL_LPM:
    {
      /* RSC Driver does not support efficient storage of sequences when
       * multiple low-power mode employ the same rsc sequence commands.
       * RSC Long sequence supports both FULL PC and APCR_PLL_LPM modes.  */
      coreMode = RSC_MODE_PWRC_BHS;
    }

    case RSC_MODE_APCR_PLL_ON:
#ifdef SLEEP_IMAGE_SLPI
    case RSC_MODE_APCR_PLL_LPM_NO_SS:
#endif
    {
      q6WakeHandshake   = 0x1;
      saveRestore       = 0x1;

#ifdef SLEEP_IMAGE_MSS
      l2tcm             = 0x03;
#endif
      configRsc         = 0x1; /* Configure RSC low-power mode */
      break;
    }
    
  /*  case RSC_MODE_L2_NRET_BHS:
    {
      coreMode = RSC_MODE_PWRC_BHS;
    } */ 

    case RSC_MODE_PWRC:
    {

#ifdef SLEEP_IMAGE_CDSP
      q6vtcm            = 1;
#endif

      configRsc         = 0x1; /* Configure RSC low-power mode */
      break;
    }

    case RSC_MODE_CLK_GATE:
    {
      /* Clock gating bit overrides all other settings, so just set this one bit.
       * This setting will be in effect until explicitly unset */
      USLEEP_HWIO_OUTF(SLPC_CFG, CLK_GATING_MODE, 1);
	 return;
    }

    default:
    {
      CORE_VERIFY(0);
      break;
    }
  }

  /* Clear clockgate bit for all other modes */
  USLEEP_HWIO_OUTF(SLPC_CFG, CLK_GATING_MODE, 0); 

  wakeupInterrupt = (flag == TRUE) ? 0x1 : 0x0;

  USLEEP_HWIO_OUTF(SLPC_CFG, WAKEUP_IN_EN, q6WakeHandshake); 

#ifdef SLEEP_IMAGE_MSS
      USLEEP_HWIO_OUTF(RSCCTL_EN_L2MEM_RET, L2DATA, l2tcm);
#endif

#ifdef SLEEP_IMAGE_CDSP
  USLEEP_HWIO_OUTF(RSCCTL_EN_VTCMEM_RET, VTCM, q6vtcm);
#endif


  USLEEP_HWIO_OUT(RSCCTL_EN_STATERET,   
#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
               USLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, PROF_IRQ, 0x1)              | /* Always gen profile int */
#endif
               USLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, WAKE_IRQ, wakeupInterrupt)  | /* PMI interrupt */
               USLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, RESTORE,  saveRestore)      | /* APCR */
               USLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, SAVE,     saveRestore));  

  /* Configure RSC if required by lprm */
  if(0 != configRsc)
  { 
    uSleepTarget_setRscLowPowerMode(RSC_CHILD, coreMode); 
  } 

  return;
}

/*
 * q6LPMConfig_setupL2Config
 */
void q6LPMConfig_setupL2Config(mem_state state)
{
  uint32 enMemRet;
  uint32 l2data = USLEEP_HWIO_INF(RSCCTL_EN_L2MEM_RET, L2DATA);
  
  if(MEM_RET == state)
  {
    /* Setup for cache related logic to be retained (All APCR modes & l2.ret) */ 
    l2data &= ~g_uSleepTargetConfig.l2_mask;

    enMemRet = USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2CLADE,   0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2PLRU,    0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2TAG,     0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2STBUF,   0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU_TAG,  0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU,      0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU_TLB64,  0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU_TLB128, 0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU_CAM,    0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, ETB,       0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_TAG,  0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_PDEC, 0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU,      0x00) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_BTB,  0x01);
  }
  else
  {
    /* Setup for cache related logic to be turned off/reset */
    l2data |= g_uSleepTargetConfig.l2_mask;

    enMemRet = USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2CLADE,   0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2PLRU,    0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2TAG,     0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2STBUF,   0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU_TAG,  0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU,      0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU_TLB64,  0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU_TLB128, 0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU_CAM,    0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, ETB,       0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_TAG,  0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_PDEC, 0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU,      0x01) |
               USLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_BTB,  0x01);
  }
 
  USLEEP_HWIO_OUTF(RSCCTL_EN_L2MEM_RET, L2DATA, l2data);
  USLEEP_HWIO_OUT(RSCCTL_EN_MEM_RET, enMemRet);
  
  return;
}
#if 0
/*
 * q6LPMConfig_setupTCMConfig
 */
void q6LPMConfig_setupTCMConfig(mem_state state)
{
  uint32 l2data = SLEEP_HWIO_INF(RSCCTL_EN_L2MEM_RET, L2DATA);
  
  if(MEM_RET == state)
  {
    /* Setup for cache related logic to be retained */ 
    l2data &= ~g_sleepTargetConfig.tcm_mask;
  }
  else
  {
    /* Setup for cache related logic to be turned off/reset */
    l2data |= g_sleepTargetConfig.tcm_mask;
  }
 
  USLEEP_HWIO_OUTF(RSCCTL_EN_L2MEM_RET, L2DATA, l2data);

  return;
}
#endif
/*
 * q6LPMConfig_setRpmhHandshake
 */
void q6LPMConfig_setRpmhHandshake(boolean performHandshake)
{
  uint16  request   = (TRUE == performHandshake) ? 1 : 0;  
  uint16  nRequest  = (TRUE == performHandshake) ? 0 : 1;

  /* Override the child -> parent handshake singnals */
  USLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, BRINGUP_REQ,  request);
  USLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, SHUTDOWN_REQ, request);

  /* If not performing the handshake, we also need to override the ACK signal
   * in the sequence since it is generic */
  USLEEP_HWIO_OUTF(RSC_EVENT_PRSC_OVR, BRINGUP_ACK_OVRRD,  nRequest); 
  USLEEP_HWIO_OUTF(RSC_EVENT_PRSC_OVR, SHUTDOWN_ACK_OVRRD, nRequest);
  
  return;
}
