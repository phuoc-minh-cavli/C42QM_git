/**
  @file bamtgtcfgdata_apss.h
  @brief
  This file contains configuration data for the BAM driver for the 
  9x55 apss system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.tx/6.0/hwengines/bam/9x65/bamtgtcfgdata_apss.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/15/16   ss      Branched and updated for 9x65
05/08/15   ss      created

===============================================================================
                   Copyright (c) 2015-2016 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    USB3.0
    BLSP1
    QPIC
    QDSS
    SPMI_FETCHER
    BAM_TGT_CFG_LAST
 */

#define BAM_CNFG_BITS_VAL 0xFFFFF004
/* clear BAM_NO_EXT_P_RST,i.e bit 12, due to HW limitation*/
#define USB_BAM_CNFG_BITS (BAM_CNFG_BITS_VAL & (~(1 << 12)))

const bam_target_config_type  bam_tgt_config[] = {
    {                      // USB30_BAM
     /* .bam_pa     */     0x08b04000,
     /* .options    */     BAM_TGT_CFG_DISABLE_CLK_GATING,
     /* .cfg_bits   */     USB_BAM_CNFG_BITS,
     /* .ee         */     0, 
     /* .sec_config */     NULL,
     /* .size       */     BAM_MAX_MMAP
    },
    {                      // BLSP1_BAM
     /* .bam_pa     */     0x07884000,
     /* .options    */    (BAM_TGT_CFG_SHARABLE|BAM_TGT_CFG_NO_INIT),
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,
     /* .sec_config */     NULL,
     /* .size       */     BAM_MAX_MMAP
    },
    {                      // QPIC_BAM _LITE
     /* .bam_pa     */     0x07984000,
     /* .options    */     BAM_TGT_CFG_NO_INIT | BAM_TGT_CFG_SHARABLE,
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,
     /* .sec_config */     NULL,
     /* .size       */     BAM_MAX_MMAP
    },
    {                      // QDSS_BAM NDP
     /* .bam_pa     */     0x00884000,
     /* .options    */     0x0,                   
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,                     
     /* .sec_config */     NULL,
     /* .size       */     BAM_MAX_MMAP
    },
    {                      // CRYPTO BAM
     /* .bam_pa     */     0x00704000,
     /* .options    */     BAM_TGT_CFG_NO_INIT,                   
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     1,                     
     /* .sec_config */     NULL,
     /* .size       */     BAM_MAX_MMAP
    },
	{                      // SPMI_FETCHER BAM
     /* .bam_pa     */     0xC4000,
     /* .options    */     0,
     /* .cfg_bits   */     BAM_CNFG_BITS_VAL,
     /* .ee         */     0,                     
     /* .sec_config */     NULL,
     /* .size       */     BAM_MAX_MMAP
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


