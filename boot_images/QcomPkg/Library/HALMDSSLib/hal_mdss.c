/*! \file */

/*
===========================================================================

FILE:         hal_mdss.c

DESCRIPTION:  MDP Initialization
  

===========================================================================

===========================================================================
  Copyright (c) 2012-2013, 2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdss.h"
#include "mdsshwio.h"

#ifdef __cplusplus
extern "C" {
#endif
uintPtr MMSS_BASE_PTR                                  = 0;
uintPtr MDSS_BASE_PTR                                  = 0;
uintPtr SECURITY_CONTROL_BASE_PTR                      = 0;

/* HW module register base */
uintPtr MMSS_MDP_REG_BASE                                          = 0;  // Register base of module MMSS_MDP
uintPtr MMSS_MDP_SSPP_TOP0_REG_BASE                                = 0;  // Register base of module MMSS_MDP_SSPP_TOP0
uintPtr MMSS_MDP_SMP_TOP0_REG_BASE                                 = 0;  // Register base of module MMSS_MDP_SMP_TOP0
uintPtr MMSS_MDP_SSPP_TOP1_REG_BASE                                = 0;  // Register base of module MMSS_MDP_SSPP_TOP1
uintPtr MMSS_MDP_SMP_TOP1_REG_BASE                                 = 0;  // Register base of module MMSS_MDP_SMP_TOP1
uintPtr MMSS_MDP_DSPP_TOP0_REG_BASE                                = 0;  // Register base of module MMSS_MDP_DSPP_TOP0
uintPtr MMSS_MDP_PERIPH_TOP0_REG_BASE                              = 0;  // Register base of module MMSS_MDP_PERIPH_TOP0
uintPtr MMSS_MDP_SSPP_TOP2_REG_BASE                                = 0;  // Register base of module MMSS_MDP_SSPP_TOP2
uintPtr MMSS_MDP_PERIPH_TOP1_REG_BASE                              = 0;  // Register base of module MMSS_MDP_PERIPH_TOP1
uintPtr MMSS_MDP_CTL_0_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_CTL_0
uintPtr MMSS_MDP_CTL_1_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_CTL_1
uintPtr MMSS_MDP_CTL_2_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_CTL_2
uintPtr MMSS_MDP_CTL_3_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_CTL_3
uintPtr MMSS_MDP_CTL_4_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_CTL_4
uintPtr MMSS_MDP_SSPP_IGC_LUT_REG_BASE                             = 0;  // Register base of module MMSS_MDP_SSPP_IGC_LUT
uintPtr MMSS_MDP_VP_0_CURSOR_0_SSPP_REG_BASE                       = 0;  // Register base of module MMSS_MDP_VP_0_CURSOR_0
uintPtr MMSS_MDP_VP_0_CURSOR_1_SSPP_REG_BASE                       = 0;  // Register base of module MMSS_MDP_VP_0_CURSOR_1
uintPtr MMSS_MDP_VP_0_VIG_0_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_VIG_0
uintPtr MMSS_MDP_VP_0_VIG_0_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_VIG_0_PCC
uintPtr MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_VIG_0_SSPP
uintPtr MMSS_MDP_VP_0_VIG_1_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_VIG_1
uintPtr MMSS_MDP_VP_0_VIG_1_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_VIG_1_PCC
uintPtr MMSS_MDP_VP_0_VIG_1_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_VIG_1_SSPP
uintPtr MMSS_MDP_VP_0_VIG_2_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_VIG_2
uintPtr MMSS_MDP_VP_0_VIG_2_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_VIG_2_PCC
uintPtr MMSS_MDP_VP_0_VIG_2_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_VIG_2_SSPP
uintPtr MMSS_MDP_VP_0_VIG_3_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_VIG_3
uintPtr MMSS_MDP_VP_0_VIG_3_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_VIG_3_PCC
uintPtr MMSS_MDP_VP_0_VIG_3_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_VIG_3_SSPP
uintPtr MMSS_MDP_VP_0_RGB_0_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_RGB_0
uintPtr MMSS_MDP_VP_0_RGB_0_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_RGB_0_PCC
uintPtr MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_RGB_0_SSPP
uintPtr MMSS_MDP_VP_0_RGB_1_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_RGB_1
uintPtr MMSS_MDP_VP_0_RGB_1_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_RGB_1_PCC
uintPtr MMSS_MDP_VP_0_RGB_1_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_RGB_1_SSPP
uintPtr MMSS_MDP_VP_0_RGB_2_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_RGB_2
uintPtr MMSS_MDP_VP_0_RGB_2_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_RGB_2_PCC
uintPtr MMSS_MDP_VP_0_RGB_2_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_RGB_2_SSPP
uintPtr MMSS_MDP_VP_0_RGB_3_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_RGB_3
uintPtr MMSS_MDP_VP_0_RGB_3_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_RGB_3_PCC
uintPtr MMSS_MDP_VP_0_RGB_3_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_RGB_3_SSPP
uintPtr MMSS_MDP_VP_0_DMA_0_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DMA_0_SSPP
uintPtr MMSS_MDP_VP_0_DMA_0_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_DMA_0
uintPtr MMSS_MDP_VP_0_DMA_0_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_DMA_0_PCC
uintPtr MMSS_MDP_VP_0_DMA_1_SSPP_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DMA_1_SSPP
uintPtr MMSS_MDP_VP_0_DMA_1_REG_BASE                               = 0;  // Register base of module MMSS_MDP_VP_0_DMA_1
uintPtr MMSS_MDP_VP_0_DMA_1_PCC_REG_BASE                           = 0;  // Register base of module MMSS_MDP_VP_0_DMA_1_PCC
uintPtr MMSS_MDP_VP_0_LAYER_0_REG_BASE                             = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_0
uintPtr MMSS_MDP_VP_0_LAYER_0_PGC_REG_BASE                         = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_0_PGC
uintPtr MMSS_MDP_VP_0_LAYER_1_REG_BASE                             = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_1
uintPtr MMSS_MDP_VP_0_LAYER_1_PGC_REG_BASE                         = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_1_PGC
uintPtr MMSS_MDP_VP_0_LAYER_2_REG_BASE                             = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_2
uintPtr MMSS_MDP_VP_0_LAYER_2_PGC_REG_BASE                         = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_2_PGC
uintPtr MMSS_MDP_VP_0_LAYER_3_REG_BASE                             = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_3
uintPtr MMSS_MDP_VP_0_LAYER_3_PGC_REG_BASE                         = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_3_PGC
uintPtr MMSS_MDP_VP_0_LAYER_4_REG_BASE                             = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_4
uintPtr MMSS_MDP_VP_0_LAYER_4_PGC_REG_BASE                         = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_4_PGC
uintPtr MMSS_MDP_VP_0_LAYER_5_REG_BASE                             = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_5
uintPtr MMSS_MDP_VP_0_LAYER_5_PGC_REG_BASE                         = 0;  // Register base of module MMSS_MDP_VP_0_LAYER_5_PGC
uintPtr MMSS_MDP_VP_0_DSPP_0_REG_BASE                              = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_0
uintPtr MMSS_MDP_VP_0_DSPP_0_GAMUT_REG_BASE                        = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_0_GAMUT
uintPtr MMSS_MDP_VP_0_DSPP_0_PCC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_0_PCC
uintPtr MMSS_MDP_VP_0_DSPP_0_PGC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_0_PGC
uintPtr MMSS_MDP_VP_0_DSPP_1_REG_BASE                              = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_1
uintPtr MMSS_MDP_VP_0_DSPP_1_GAMUT_REG_BASE                        = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_1_GAMUT
uintPtr MMSS_MDP_VP_0_DSPP_1_PCC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_1_PCC
uintPtr MMSS_MDP_VP_0_DSPP_1_PGC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_1_PGC
uintPtr MMSS_MDP_VP_0_DSPP_2_REG_BASE                              = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_2
uintPtr MMSS_MDP_VP_0_DSPP_2_GAMUT_REG_BASE                        = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_2_GAMUT
uintPtr MMSS_MDP_VP_0_DSPP_2_PCC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_2_PCC
uintPtr MMSS_MDP_VP_0_DSPP_2_PGC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_2_PGC
uintPtr MMSS_MDP_VP_0_DSPP_3_REG_BASE                              = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_3
uintPtr MMSS_MDP_VP_0_DSPP_3_GAMUT_REG_BASE                        = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_3_GAMUT
uintPtr MMSS_MDP_VP_0_DSPP_3_PCC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_3_PCC
uintPtr MMSS_MDP_VP_0_DSPP_3_PGC_REG_BASE                          = 0;  // Register base of module MMSS_MDP_VP_0_DSPP_3_PGC
uintPtr MMSS_MDP_WB_0_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_WB_0
uintPtr MMSS_MDP_WB_1_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_WB_1
uintPtr MMSS_MDP_WB_2_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_WB_2
uintPtr MMSS_MDP_WB_3_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_WB_3
uintPtr MMSS_MDP_WB_4_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_WB_4
uintPtr MMSS_MDP_INTF_0_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_INTF_0
uintPtr MMSS_MDP_INTF_1_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_INTF_1
uintPtr MMSS_MDP_INTF_2_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_INTF_2
uintPtr MMSS_MDP_INTF_3_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_INTF_3
uintPtr MMSS_MDP_INTF_4_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_INTF_4
uintPtr MMSS_MDP_PP_0_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_PP_0
uintPtr MMSS_MDP_PP_1_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_PP_1
uintPtr MMSS_MDP_PP_2_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_PP_2
uintPtr MMSS_MDP_PP_3_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_PP_3
uintPtr MMSS_MDP_PP_4_REG_BASE                                     = 0;  // Register base of module MMSS_MDP_PP_4
uintPtr MMSS_MDP_CDM_0_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_CDM_0
uintPtr MMSS_MDP_CDM_0_CSC_10_REG_BASE                             = 0;  // Register base of module MMSS_MDP_CDM_0_CSC_10
uintPtr MMSS_MDP_CDM_0_CDWN2_REG_BASE                              = 0;  // Register base of module MMSS_MDP_CDM_0_CDWN2
uintPtr MMSS_MDP_CDM_0_HDMI_PACK_REG_BASE                          = 0;  // Register base of module MMSS_MDP_CDM_0_HDMI_PACK
uintPtr MMSS_MDP_DSC_0_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_DSC_0
uintPtr MMSS_MDP_DSC_1_REG_BASE                                    = 0;  // Register base of module MMSS_MDP_DSC_1
uintPtr MMSS_HDMI_REG_BASE                                         = 0;  // Register base of module MMSS_HDMI
uintPtr MMSS_HDMI_PHY_REG_BASE                                     = 0;  // Register base of module MMSS_HDMI_PHY
uintPtr MMSS_HDMI_PHY_PLL_REG_BASE                                 = 0;  // Register base of module MMSS_HDMI_PHY_PLL
uintPtr MMSS_HDMI_PHY_QSERDES_COM_QSERDES_COM_HDMI_COM_REG_BASE    = 0;  // Register base of module MMSS_HDMI_PHY_QSERDES_COM_QSERDES_COM_HDMI_COM
uintPtr MMSS_HDMI_PHY_QSERDES_TX_L0_QSERDES_TX_L0_HDMI_TX_REG_BASE = 0;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L0_QSERDES_TX_L0_HDMI_TX
uintPtr MMSS_HDMI_PHY_QSERDES_TX_L1_QSERDES_TX_L1_HDMI_TX_REG_BASE = 0;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L1_QSERDES_TX_L1_HDMI_TX
uintPtr MMSS_HDMI_PHY_QSERDES_TX_L2_QSERDES_TX_L2_HDMI_TX_REG_BASE = 0;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L2_QSERDES_TX_L2_HDMI_TX
uintPtr MMSS_HDMI_PHY_QSERDES_TX_L3_QSERDES_TX_L3_HDMI_TX_REG_BASE = 0;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L3_QSERDES_TX_L3_HDMI_TX
uintPtr MMSS_DSI_0_REG_BASE                                        = 0;  // Register base of module MMSS_DSI_0
uintPtr MMSS_DSI_0_PHY_PLL_REG_BASE                                = 0;  // Register base of module MMSS_DSI_0_PHY_PLL
uintPtr MMSS_DSI_0_PHY_REG_BASE                                    = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON_REG_BASE   = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_COM_LITE_REG_BASE = 0;  // Register base of module MMSS_DSI_0_PHY
uintPtr MMSS_DSI_0_PHY_REG_REG_BASE                                = 0;  // Register base of module MMSS_DSI_0_PHY_REG
uintPtr MMSS_DSI_1_REG_BASE                                        = 0;  // Register base of module MMSS_DSI_1
uintPtr MMSS_DSI_1_PHY_PLL_REG_BASE                                = 0;  // Register base of module MMSS_DSI_1_PHY_PLL
uintPtr MMSS_DSI_1_PHY_REG_BASE                                    = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON_REG_BASE   = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN_REG_BASE = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_COM_LITE_REG_BASE = 0;  // Register base of module MMSS_DSI_1_PHY
uintPtr MMSS_DSI_1_PHY_REG_REG_BASE                                = 0;  // Register base of module MMSS_DSI_1_PHY_REG
uintPtr MMSS_EDP_REG_BASE                                          = 0;  // Register base of module MMSS_EDP
uintPtr MMSS_EDP_PHY_REG_BASE                                      = 0;  // Register base of module MMSS_EDP_PHY
uintPtr MMSS_EDP_PHY_PLL_REG_BASE                                  = 0;  // Register base of module MMSS_EDP_PHY_PLL
uintPtr MMSS_VBIF_VBIF_MDP_REG_BASE                                = 0;  // Register base of module MMSS_VBIF_VBIF_MDP
uintPtr MMSS_VBIF_VBIF_VBIF_APU0132_13_QR_REG_BASE                 = 0;  // Register base of module MMSS_VBIF_VBIF_VBIF_APU0132_13_QR
uintPtr MMSS_VBIF_CB0_CB0_MM_MMU_CONTEXT_BANK_REG_BASE             = 0;  // Register base of module MMSS_VBIF_CB0_CB0_MM_MMU_CONTEXT_BANK
uintPtr MMSS_VBIF_CB1_CB1_MM_MMU_CONTEXT_BANK_REG_BASE             = 0;  // Register base of module MMSS_VBIF_CB1_CB1_MM_MMU_CONTEXT_BANK
uintPtr MMSS_VBIF_CB2_CB2_MM_MMU_CONTEXT_BANK_REG_BASE             = 0;  // Register base of module MMSS_VBIF_CB2_CB2_MM_MMU_CONTEXT_BANK
uintPtr MMSS_VBIF_MM_MMU_GLOBAL0_REG_BASE                          = 0;  // Register base of module MMSS_VBIF_MM_MMU_GLOBAL0
uintPtr MMSS_VBIF_MM_MMU_GLOBAL1_REG_BASE                          = 0;  // Register base of module MMSS_VBIF_MM_MMU_GLOBAL1
uintPtr MMSS_VBIF_MM_MMU_IMPL_DEF_REG_BASE                         = 0;  // Register base of module MMSS_VBIF_MM_MMU_IMPL_DEF
uintPtr MMSS_VBIF_MM_MMU_PERF_MON_REG_BASE                         = 0;  // Register base of module MMSS_VBIF_MM_MMU_PERF_MON
uintPtr MMSS_VBIF_MM_MMU_SSD_REG_BASE                              = 0;  // Register base of module MMSS_VBIF_MM_MMU_SSD
uintPtr MMSS_MDSS_SEC_DSI_0_SEC_REG_BASE                           = 0;  // Register base of module MMSS_MDSS_SEC_DSI_0_SEC
uintPtr MMSS_MDSS_SEC_DSI_1_SEC_REG_BASE                           = 0;  // Register base of module MMSS_MDSS_SEC_DSI_1_SEC
uintPtr MMSS_MDP_AADC_0_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_AADC_0
uintPtr MMSS_MDP_AADC_1_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_AADC_1
uintPtr MMSS_MDP_AADC_2_REG_BASE                                   = 0;  // Register base of module MMSS_MDP_AADC_2
uintPtr MMSS_MDP_ENCR_NS_0_REG_BASE                                = 0;  // Register base of module MMSS_MDP_ENCR_NS_0
uintPtr MMSS_MDP_ENCR_S_0_REG_BASE                                 = 0;  // Register base of module MMSS_MDP_ENCR_S_0



/****************************************************************************
*
** FUNCTION: HAL_MDSS_IsInterruptActive()
*/
/*!
* \brief
*     HAL_MDSS_IsInterruptActive --- Report MDSS cores that have active interrupts
*
* \param [out]   pModuleIDs            - Reports the list of modules that have active interrupts
* \param [in]    uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDSS_IsInterruptActive(uint32              *pModuleIDs,
                                              uint32               uFlags)
{
  HAL_MDSS_ErrorType   eStatus         = HAL_MDSS_STATUS_SUCCESS;
  uint32               uMDSSIntrStatus = HWIO_MMSS_MDSS_HW_INTR_STATUS_IN;
  uint32               uModuleID       = 0;

  if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, MDP))
  {
    uModuleID |= HAL_MDSS_MODULE_MDP;
  }
  if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, DSI0))
  {
    uModuleID |= HAL_MDSS_MODULE_DSI0;
  }
  if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, DSI1))
  {
    uModuleID |= HAL_MDSS_MODULE_DSI1;
  }
  if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, HDMI))
  {
    uModuleID |= HAL_MDSS_MODULE_HDMI;
  }
  if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, EDP))
  {
    uModuleID |= HAL_MDSS_MODULE_eDP;
  }

  *pModuleIDs = uModuleID;

  return eStatus;
}

/*=========================================================================

HAL_MDSS_SetBaseAddress()

Description:
   Initialize base address pointer 

===========================================================================*/
HAL_MDSS_ErrorType HAL_MDSS_SetBaseAddress (uint32                         uMDSSBaseAddress)
{
   HAL_MDSS_ErrorType  eStatus = HAL_MDSS_STATUS_SUCCESS;

   if (0 != uMDSSBaseAddress)
   {
      MDSS_BASE_PTR = (uintPtr)uMDSSBaseAddress;
   }
   
   return eStatus;
}

#ifdef __cplusplus
}
#endif

