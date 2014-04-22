/*==============================================================================
  FILE:         rsc_config.h

  OVERVIEW:     This file contains target specific data related to the
                RSC sequences

  DEPENDENCIES: None
                
                Copyright (c) 2017-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "comdef.h"

/*==========================================================================
 *                          DEFINITIONS
 *=========================================================================*/
/* Child offsets */
#define Q6SS_RSCC_LPASS_BUS_ISO_SEQ	           0x0
#define Q6SS_RSCC_LPASS_BUS_DEISO_SEQ	       0x09
#define Q6SS_RSCC_LPASS_SHORT_APCR_SEQ_LPASS   0x11
#define Q6SS_RSCC_LPASS_LONG_APCR_SEQ_LPASS	   0x37

#define Q6SS_RSCC_SSC_LPASS_NUMSEQWORDS        30
#define Q6SS_RSCC_SSC_LPASS_NUMBRANCHES        0

#define RSC_NUM_CHILD_LOW_POWER_MODES          4

#if 0
/* Parent offsets */
#define SSC_LPASS_RSCP_LONG_SEQ                   0x0
#define SSC_LPASS_RSCP_EXIT_SLEEP                 0x27

#define SSC_LPASS_RSCP_NUMSEQWORDS           10
#define SSC_LPASS_RSCP_NUMBRANCHES            2

#define RSC_NUM_PARENT_LOW_POWER_MODES      1
#endif