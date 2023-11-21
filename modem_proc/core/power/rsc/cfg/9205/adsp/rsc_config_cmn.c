/*==============================================================================
  FILE:         rsc_config_cmn.c

  OVERVIEW:     This file contains target specific data/configuration for rsc
                instances in island

  DEPENDENCIES: None
                
                Copyright (c) 2017-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "rsc.h"
#include "rsc_seq_hal.h"
#include "rsc_hal_bsp.h"
#include "rsc_config.h"
#include "rsc_hwio.h"

/*==========================================================================
 *                          EXTERNAL REFERENCES
 *=========================================================================*/
//extern uint32 g_rscParentSequence[];
extern uint32 g_rscChildSequence[];
//extern branch_info  g_rscParentBranch[SSC_LPASS_RSCP_NUMBRANCHES];

/*==========================================================================
 *                          SUBSYSTEM RSC INSTANCES
 *=========================================================================*/
/* QDSP6 mode configurations. */
rsc_modes_t g_rscChildModes[RSC_NUM_CHILD_LOW_POWER_MODES] =
{
  {
    RSC_SW_TRIG_BUS_ISOLATION + RSC_SW_TRIGGER_FLAG_LPM,     // mode_id
    Q6SS_RSCC_LPASS_BUS_ISO_SEQ,                         // start_address
  },
  {
    RSC_SW_TRIG_BUS_DEISOLATION + RSC_SW_TRIGGER_FLAG_LPM,
    Q6SS_RSCC_LPASS_BUS_DEISO_SEQ,
  },
  {
    RSC_MODE_APCR_PLL_ON,
    Q6SS_RSCC_LPASS_SHORT_APCR_SEQ_LPASS,
  },
  {
    RSC_MODE_PWRC_BHS,
    Q6SS_RSCC_LPASS_LONG_APCR_SEQ_LPASS,
  },
 // {
 //   RSC_MODE_APCR_PLL_LPM_NO_SS,
 //   Q6SS_RSCC_SSC_LPASS_LONG_APCR_SEQ_NOTCS,  //swayam: commented because not in Starlord
 // },
};

rsc_t g_rscInstances[] =
{
  {
    g_rscChildModes,                  // Instance modes
    RSC_NUM_CHILD_LOW_POWER_MODES,
    g_rscChildSequence,               // Instance sequence
    Q6SS_RSCC_SSC_LPASS_NUMSEQWORDS,
    NULL,                             // branch info (static)
    Q6SS_RSCC_SSC_LPASS_NUMBRANCHES,
    RSC_SS_BASE + 0x007b0000,         // HW base address
  },
};

/* Variable to indicate rsc instance counts */
uint32 g_rscInstCount = sizeof(g_rscInstances) / sizeof(g_rscInstances[0]);

