/**
  @file bamtgtcfgdata_apss.h
  @brief
  This file contains configuration data for the BAM driver for the 
  APSS system.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/2018    cs	   mapped QPIC_BAM physical address
01/2018    pc      Added QPIC support for QCS405
03/3116    ss      updated for Sdm660
27/8/14    rl      initial

===============================================================================
                   Copyright (c) 2013-2018 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    QPIC
    CE0
 */
#include "Include/Library/bamtgtcfg.h"

#define BAM_CNFG_BITS_VAL 0xFFFFF004
#define QPIC_BAM_PA		  0x07984000 
#define CE0_BAM_PA		  0x00704000

const bam_target_config_type  bam_tgt_config[] = {
	{                     
                          // QPIC_BAM _LITE
	 /* .bam_pa     */    QPIC_BAM_PA,
	 /* .options    */    BAM_TGT_CFG_FORCEINIT,
	 /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
	 /* .ee         */    0,
	 /* .sec_config */    NULL,
	 /* .size       */    BAM_QPIC_MMAP
	},
    { 
                         // CE0_BAM 
     /* .bam_pa     */   CE0_BAM_PA,
     /* .options    */   BAM_TGT_CFG_FORCEINIT,
     /* .cfg_bits   */   BAM_CNFG_BITS_VAL,     
     /* .ee         */   0,
     /* .sec_config */   NULL,
     /* .size       */   BAM_CRYPTO_MMAP
    },
    { //dummy config
     /* .bam_pa     */   BAM_TGT_CFG_LAST,
     /* .options    */   0,
     /* .cfg_bits   */   0,
     /* .ee         */   0,
     /* .sec_config */   NULL,
     /* .size       */   0
    },
};




