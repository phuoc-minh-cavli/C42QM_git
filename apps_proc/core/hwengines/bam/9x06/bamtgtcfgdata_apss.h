/**
  @file bamtgtcfgdata_apss.h
  @brief
  This file contains configuration data for the BAM driver for the 
  9x55 apss system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.tx/6.0/hwengines/bam/9x06/bamtgtcfgdata_apss.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/08/15   ss      created

===============================================================================
                   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    USB3.0
    BLSP1
    QPIC
    QDSS
    IPA
    BAM_TGT_CFG_LAST
 */

#define BAM_CNFG_BITS_VAL 0xFFFFF004
/* clear BAM_NO_EXT_P_RST,i.e bit 12, due to HW limitation*/
#define USB_BAM_CNFG_BITS (BAM_CNFG_BITS_VAL & (~(1 << 12)))

const bam_target_config_type  bam_tgt_config[] = {
    {                      // USB30_BAM
     /* .bam_pa     */     0x78C4000,
     /* .options    */     BAM_TGT_CFG_DISABLE_CLK_GATING,
     /* .cfg_bits   */     USB_BAM_CNFG_BITS,
     /* .ee         */     0, 
     /* .sec_config */     NULL,
     /* .size       */     0x15000
    },
    {                      // BLSP1_BAM
     /* .bam_pa     */     0x07884000,
     /* .options    */    (BAM_TGT_CFG_SHARABLE|BAM_TGT_CFG_NO_INIT|BAM_TGT_CFG_USE_DSR),
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,
     /* .sec_config */     NULL,
     /* .size       */     0x2B000
    },
    {                      // QPIC_BAM _LITE
     /* .bam_pa     */     0x7984000,
     /* .options    */     BAM_TGT_CFG_NO_INIT | BAM_TGT_CFG_SHARABLE | BAM_TGT_CFG_USE_NO_SYNC_LOCK,
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,
     /* .sec_config */     NULL,
     /* .size       */     0x1A000
    },
    {                      // QDSS_BAM NDP
     /* .bam_pa     */     0x6084000,
     /* .options    */     0x0,                   
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,                     
     /* .sec_config */     NULL,
     /* .size       */     0x15000
    },
    {                      // A2_BAM NDP
     /* .bam_pa     */     0x4044000,
     /* .options    */     BAM_TGT_CFG_USE_DSR,
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,
     /* .sec_config */     NULL,
     /* .size       */     0x19000
    },
	{                      // CRYPTO BAM
     /* .bam_pa     */     0x00704000,
     /* .options    */     BAM_TGT_CFG_NO_INIT,                   
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     1,                     
     /* .sec_config */     NULL,
     /* .size       */     0x20000
    },
    {                      //dummy config
     /* .bam_pa     */     BAM_TGT_CFG_LAST,
     /* .options    */     0,
     /* .cfg_bits   */     0,
     /* .ee         */     0,
     /* .sec_config */     NULL,
     /* .size       */     0
    },
};


