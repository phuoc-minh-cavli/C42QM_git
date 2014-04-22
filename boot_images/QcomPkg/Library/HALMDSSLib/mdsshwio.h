/*! \file */

/*
===========================================================================

FILE:        mdsshwio.h

DESCRIPTION:  
  Top level MDSS HW Access Definition file.  It includes:
  [1] MDSS Register file
  [2] HW Access Macros
  [3] Additionally Defined Fields for HW Access

===========================================================================

                             Edit History



===========================================================================
  Copyright (c) 2010-2013, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef MDSSHWIO_H
#define MDSSHWIO_H



/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

// Basic Register Access Macros
#include "mdsshalhwio.h"

// Database Header file of all MDSS Registers: MDP, DSI, HDMI and eDP
// Static MDP Base Defined Here

extern uintPtr MDSS_BASE_PTR;

/* HW module register base */
extern uintPtr MMSS_MDP_REG_BASE;                                           // Register base of module MMSS_MDP
extern uintPtr MMSS_MDP_SSPP_TOP0_REG_BASE;                                 // Register base of module MMSS_MDP_SSPP_TOP0
extern uintPtr MMSS_MDP_SMP_TOP0_REG_BASE;                                  // Register base of module MMSS_MDP_SMP_TOP0
extern uintPtr MMSS_MDP_SSPP_TOP1_REG_BASE;                                 // Register base of module MMSS_MDP_SSPP_TOP1
extern uintPtr MMSS_MDP_SMP_TOP1_REG_BASE;                                  // Register base of module MMSS_MDP_SMP_TOP1
extern uintPtr MMSS_MDP_DSPP_TOP0_REG_BASE;                                 // Register base of module MMSS_MDP_DSPP_TOP0
extern uintPtr MMSS_MDP_PERIPH_TOP0_REG_BASE;                               // Register base of module MMSS_MDP_PERIPH_TOP0
extern uintPtr MMSS_MDP_SSPP_TOP2_REG_BASE;                                 // Register base of module MMSS_MDP_SSPP_TOP2
extern uintPtr MMSS_MDP_PERIPH_TOP1_REG_BASE;                               // Register base of module MMSS_MDP_PERIPH_TOP1
extern uintPtr MMSS_MDP_CTL_0_REG_BASE;                                     // Register base of module MMSS_MDP_CTL_0
extern uintPtr MMSS_MDP_CTL_1_REG_BASE;                                     // Register base of module MMSS_MDP_CTL_1
extern uintPtr MMSS_MDP_CTL_2_REG_BASE;                                     // Register base of module MMSS_MDP_CTL_2
extern uintPtr MMSS_MDP_CTL_3_REG_BASE;                                     // Register base of module MMSS_MDP_CTL_3
extern uintPtr MMSS_MDP_CTL_4_REG_BASE;                                     // Register base of module MMSS_MDP_CTL_4
extern uintPtr MMSS_MDP_SSPP_IGC_LUT_REG_BASE;                              // Register base of module MMSS_MDP_SSPP_IGC_LUT
extern uintPtr MMSS_MDP_VP_0_CURSOR_0_SSPP_REG_BASE;                        // Register base of module MMSS_MDP_VP_0_CURSOR_0
extern uintPtr MMSS_MDP_VP_0_CURSOR_1_SSPP_REG_BASE;                        // Register base of module MMSS_MDP_VP_0_CURSOR_1
extern uintPtr MMSS_MDP_VP_0_VIG_0_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_VIG_0
extern uintPtr MMSS_MDP_VP_0_VIG_0_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_VIG_0_PCC
extern uintPtr MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_VIG_0_SSPP
extern uintPtr MMSS_MDP_VP_0_VIG_1_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_VIG_1
extern uintPtr MMSS_MDP_VP_0_VIG_1_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_VIG_1_PCC
extern uintPtr MMSS_MDP_VP_0_VIG_1_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_VIG_1_SSPP
extern uintPtr MMSS_MDP_VP_0_VIG_2_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_VIG_2
extern uintPtr MMSS_MDP_VP_0_VIG_2_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_VIG_2_PCC
extern uintPtr MMSS_MDP_VP_0_VIG_2_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_VIG_2_SSPP
extern uintPtr MMSS_MDP_VP_0_VIG_3_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_VIG_3
extern uintPtr MMSS_MDP_VP_0_VIG_3_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_VIG_3_PCC
extern uintPtr MMSS_MDP_VP_0_VIG_3_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_VIG_3_SSPP
extern uintPtr MMSS_MDP_VP_0_RGB_0_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_RGB_0
extern uintPtr MMSS_MDP_VP_0_RGB_0_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_RGB_0_PCC
extern uintPtr MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_RGB_0_SSPP
extern uintPtr MMSS_MDP_VP_0_RGB_1_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_RGB_1
extern uintPtr MMSS_MDP_VP_0_RGB_1_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_RGB_1_PCC
extern uintPtr MMSS_MDP_VP_0_RGB_1_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_RGB_1_SSPP
extern uintPtr MMSS_MDP_VP_0_RGB_2_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_RGB_2
extern uintPtr MMSS_MDP_VP_0_RGB_2_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_RGB_2_PCC
extern uintPtr MMSS_MDP_VP_0_RGB_2_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_RGB_2_SSPP
extern uintPtr MMSS_MDP_VP_0_RGB_3_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_RGB_3
extern uintPtr MMSS_MDP_VP_0_RGB_3_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_RGB_3_PCC
extern uintPtr MMSS_MDP_VP_0_RGB_3_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_RGB_3_SSPP
extern uintPtr MMSS_MDP_VP_0_DMA_0_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DMA_0_SSPP
extern uintPtr MMSS_MDP_VP_0_DMA_0_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_DMA_0
extern uintPtr MMSS_MDP_VP_0_DMA_0_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_DMA_0_PCC
extern uintPtr MMSS_MDP_VP_0_DMA_1_SSPP_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DMA_1_SSPP
extern uintPtr MMSS_MDP_VP_0_DMA_1_REG_BASE;                                // Register base of module MMSS_MDP_VP_0_DMA_1
extern uintPtr MMSS_MDP_VP_0_DMA_1_PCC_REG_BASE;                            // Register base of module MMSS_MDP_VP_0_DMA_1_PCC
extern uintPtr MMSS_MDP_VP_0_LAYER_0_REG_BASE;                              // Register base of module MMSS_MDP_VP_0_LAYER_0
extern uintPtr MMSS_MDP_VP_0_LAYER_0_PGC_REG_BASE;                          // Register base of module MMSS_MDP_VP_0_LAYER_0_PGC
extern uintPtr MMSS_MDP_VP_0_LAYER_1_REG_BASE;                              // Register base of module MMSS_MDP_VP_0_LAYER_1
extern uintPtr MMSS_MDP_VP_0_LAYER_1_PGC_REG_BASE;                          // Register base of module MMSS_MDP_VP_0_LAYER_1_PGC
extern uintPtr MMSS_MDP_VP_0_LAYER_2_REG_BASE;                              // Register base of module MMSS_MDP_VP_0_LAYER_2
extern uintPtr MMSS_MDP_VP_0_LAYER_2_PGC_REG_BASE;                          // Register base of module MMSS_MDP_VP_0_LAYER_2_PGC
extern uintPtr MMSS_MDP_VP_0_LAYER_3_REG_BASE;                              // Register base of module MMSS_MDP_VP_0_LAYER_3
extern uintPtr MMSS_MDP_VP_0_LAYER_3_PGC_REG_BASE;                          // Register base of module MMSS_MDP_VP_0_LAYER_3_PGC
extern uintPtr MMSS_MDP_VP_0_LAYER_4_REG_BASE;                              // Register base of module MMSS_MDP_VP_0_LAYER_4
extern uintPtr MMSS_MDP_VP_0_LAYER_4_PGC_REG_BASE;                          // Register base of module MMSS_MDP_VP_0_LAYER_4_PGC
extern uintPtr MMSS_MDP_VP_0_LAYER_5_REG_BASE;                              // Register base of module MMSS_MDP_VP_0_LAYER_5
extern uintPtr MMSS_MDP_VP_0_LAYER_5_PGC_REG_BASE;                          // Register base of module MMSS_MDP_VP_0_LAYER_5_PGC
extern uintPtr MMSS_MDP_VP_0_DSPP_0_REG_BASE;                               // Register base of module MMSS_MDP_VP_0_DSPP_0
extern uintPtr MMSS_MDP_VP_0_DSPP_0_GAMUT_REG_BASE;                         // Register base of module MMSS_MDP_VP_0_DSPP_0_GAMUT
extern uintPtr MMSS_MDP_VP_0_DSPP_0_PCC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_0_PCC
extern uintPtr MMSS_MDP_VP_0_DSPP_0_PGC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_0_PGC
extern uintPtr MMSS_MDP_VP_0_DSPP_1_REG_BASE;                               // Register base of module MMSS_MDP_VP_0_DSPP_1
extern uintPtr MMSS_MDP_VP_0_DSPP_1_GAMUT_REG_BASE;                         // Register base of module MMSS_MDP_VP_0_DSPP_1_GAMUT
extern uintPtr MMSS_MDP_VP_0_DSPP_1_PCC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_1_PCC
extern uintPtr MMSS_MDP_VP_0_DSPP_1_PGC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_1_PGC
extern uintPtr MMSS_MDP_VP_0_DSPP_2_REG_BASE;                               // Register base of module MMSS_MDP_VP_0_DSPP_2
extern uintPtr MMSS_MDP_VP_0_DSPP_2_GAMUT_REG_BASE;                         // Register base of module MMSS_MDP_VP_0_DSPP_2_GAMUT
extern uintPtr MMSS_MDP_VP_0_DSPP_2_PCC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_2_PCC
extern uintPtr MMSS_MDP_VP_0_DSPP_2_PGC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_2_PGC
extern uintPtr MMSS_MDP_VP_0_DSPP_3_REG_BASE;                               // Register base of module MMSS_MDP_VP_0_DSPP_3
extern uintPtr MMSS_MDP_VP_0_DSPP_3_GAMUT_REG_BASE;                         // Register base of module MMSS_MDP_VP_0_DSPP_3_GAMUT
extern uintPtr MMSS_MDP_VP_0_DSPP_3_PCC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_3_PCC
extern uintPtr MMSS_MDP_VP_0_DSPP_3_PGC_REG_BASE;                           // Register base of module MMSS_MDP_VP_0_DSPP_3_PGC
extern uintPtr MMSS_MDP_WB_0_REG_BASE;                                      // Register base of module MMSS_MDP_WB_0
extern uintPtr MMSS_MDP_WB_1_REG_BASE;                                      // Register base of module MMSS_MDP_WB_1
extern uintPtr MMSS_MDP_WB_2_REG_BASE;                                      // Register base of module MMSS_MDP_WB_2
extern uintPtr MMSS_MDP_WB_3_REG_BASE;                                      // Register base of module MMSS_MDP_WB_3
extern uintPtr MMSS_MDP_WB_4_REG_BASE;                                      // Register base of module MMSS_MDP_WB_4
extern uintPtr MMSS_MDP_INTF_0_REG_BASE;                                    // Register base of module MMSS_MDP_INTF_0
extern uintPtr MMSS_MDP_INTF_1_REG_BASE;                                    // Register base of module MMSS_MDP_INTF_1
extern uintPtr MMSS_MDP_INTF_2_REG_BASE;                                    // Register base of module MMSS_MDP_INTF_2
extern uintPtr MMSS_MDP_INTF_3_REG_BASE;                                    // Register base of module MMSS_MDP_INTF_3
extern uintPtr MMSS_MDP_INTF_4_REG_BASE;                                    // Register base of module MMSS_MDP_INTF_4
extern uintPtr MMSS_MDP_PP_0_REG_BASE;                                      // Register base of module MMSS_MDP_PP_0
extern uintPtr MMSS_MDP_PP_1_REG_BASE;                                      // Register base of module MMSS_MDP_PP_1
extern uintPtr MMSS_MDP_PP_2_REG_BASE;                                      // Register base of module MMSS_MDP_PP_2
extern uintPtr MMSS_MDP_PP_3_REG_BASE;                                      // Register base of module MMSS_MDP_PP_3
extern uintPtr MMSS_MDP_PP_4_REG_BASE;                                      // Register base of module MMSS_MDP_PP_4
extern uintPtr MMSS_MDP_CDM_0_REG_BASE;                                     // Register base of module MMSS_MDP_CDM_0
extern uintPtr MMSS_MDP_CDM_0_CSC_10_REG_BASE;                              // Register base of module MMSS_MDP_CDM_0_CSC_10
extern uintPtr MMSS_MDP_CDM_0_CDWN2_REG_BASE;                               // Register base of module MMSS_MDP_CDM_0_CDWN2
extern uintPtr MMSS_MDP_CDM_0_HDMI_PACK_REG_BASE;                           // Register base of module MMSS_MDP_CDM_0_HDMI_PACK
extern uintPtr MMSS_MDP_DSC_0_REG_BASE;                                     // Register base of module MMSS_MDP_DSC_0
extern uintPtr MMSS_MDP_DSC_1_REG_BASE;                                     // Register base of module MMSS_MDP_DSC_1
extern uintPtr MMSS_HDMI_REG_BASE;                                          // Register base of module MMSS_HDMI
extern uintPtr MMSS_HDMI_PHY_REG_BASE;                                      // Register base of module MMSS_HDMI_PHY
extern uintPtr MMSS_HDMI_PHY_PLL_REG_BASE;                                  // Register base of module MMSS_HDMI_PHY_PLL
extern uintPtr MMSS_HDMI_PHY_QSERDES_COM_QSERDES_COM_HDMI_COM_REG_BASE;     // Register base of module MMSS_HDMI_PHY_QSERDES_COM_QSERDES_COM_HDMI_COM
extern uintPtr MMSS_HDMI_PHY_QSERDES_TX_L0_QSERDES_TX_L0_HDMI_TX_REG_BASE;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L0_QSERDES_TX_L0_HDMI_TX
extern uintPtr MMSS_HDMI_PHY_QSERDES_TX_L1_QSERDES_TX_L1_HDMI_TX_REG_BASE;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L1_QSERDES_TX_L1_HDMI_TX
extern uintPtr MMSS_HDMI_PHY_QSERDES_TX_L2_QSERDES_TX_L2_HDMI_TX_REG_BASE;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L2_QSERDES_TX_L2_HDMI_TX
extern uintPtr MMSS_HDMI_PHY_QSERDES_TX_L3_QSERDES_TX_L3_HDMI_TX_REG_BASE;  // Register base of module MMSS_HDMI_PHY_QSERDES_TX_L3_QSERDES_TX_L3_HDMI_TX
extern uintPtr MMSS_DSI_0_REG_BASE;                                         // Register base of module MMSS_DSI_0
extern uintPtr MMSS_DSI_0_PHY_PLL_REG_BASE;                                 // Register base of module MMSS_DSI_0_PHY_PLL
extern uintPtr MMSS_DSI_0_PHY_REG_BASE;                                     // Register base of module MMSS_DSI_0_PHY
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON_REG_BASE;    // Register base of module MMSS_DSI_0_PHY_CMN_DSIPHY_CMN_DSI_COMMON
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_0_PHY_DLN0_DSIPHY_DLN0_DSI_DATALN
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_0_PHY_DLN1_DSIPHY_DLN1_DSI_DATALN
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_0_PHY_DLN2_DSIPHY_DLN2_DSI_DATALN
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_0_PHY_DLN3_DSIPHY_DLN3_DSI_DATALN
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_0_PHY_CKLN_DSIPHY_CKLN_DSI_DATALN
extern uintPtr MMSS_DSI_0_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_COM_LITE_REG_BASE;  // Register base of module MMSS_DSI_0_PHY_PLL_DSIPHY_PLL_DSI_COM_LITE
extern uintPtr MMSS_DSI_0_PHY_REG_REG_BASE;                                 // Register base of module MMSS_DSI_0_PHY_REG
extern uintPtr MMSS_DSI_1_REG_BASE;                                         // Register base of module MMSS_DSI_1
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON_REG_BASE;    // Register base of module MMSS_DSI_1_PHY_CMN_DSIPHY_CMN_DSI_COMMON
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_1_PHY_DLN0_DSIPHY_DLN0_DSI_DATALN
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_1_PHY_DLN1_DSIPHY_DLN1_DSI_DATALN
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_1_PHY_DLN2_DSIPHY_DLN2_DSI_DATALN
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_1_PHY_DLN3_DSIPHY_DLN3_DSI_DATALN
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN_REG_BASE;  // Register base of module MMSS_DSI_1_PHY_CKLN_DSIPHY_CKLN_DSI_DATALN
extern uintPtr MMSS_DSI_1_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_COM_LITE_REG_BASE;  // Register base of module MMSS_DSI_1_PHY_PLL_DSIPHY_PLL_DSI_COM_LITE
extern uintPtr MMSS_DSI_1_PHY_PLL_REG_BASE;                                 // Register base of module MMSS_DSI_1_PHY_PLL
extern uintPtr MMSS_DSI_1_PHY_REG_BASE;                                     // Register base of module MMSS_DSI_1_PHY
extern uintPtr MMSS_DSI_1_PHY_REG_REG_BASE;                                 // Register base of module MMSS_DSI_1_PHY_REG
extern uintPtr MMSS_EDP_REG_BASE;                                           // Register base of module MMSS_EDP
extern uintPtr MMSS_EDP_PHY_REG_BASE;                                       // Register base of module MMSS_EDP_PHY
extern uintPtr MMSS_EDP_PHY_PLL_REG_BASE;                                   // Register base of module MMSS_EDP_PHY_PLL
extern uintPtr MMSS_VBIF_VBIF_MDP_REG_BASE;                                 // Register base of module MMSS_VBIF_VBIF_MDP
extern uintPtr MMSS_VBIF_VBIF_VBIF_APU0132_13_QR_REG_BASE;                  // Register base of module MMSS_VBIF_VBIF_VBIF_APU0132_13_QR
extern uintPtr MMSS_VBIF_CB0_CB0_MM_MMU_CONTEXT_BANK_REG_BASE;              // Register base of module MMSS_VBIF_CB0_CB0_MM_MMU_CONTEXT_BANK
extern uintPtr MMSS_VBIF_CB1_CB1_MM_MMU_CONTEXT_BANK_REG_BASE;              // Register base of module MMSS_VBIF_CB1_CB1_MM_MMU_CONTEXT_BANK
extern uintPtr MMSS_VBIF_CB2_CB2_MM_MMU_CONTEXT_BANK_REG_BASE;              // Register base of module MMSS_VBIF_CB2_CB2_MM_MMU_CONTEXT_BANK
extern uintPtr MMSS_VBIF_MM_MMU_GLOBAL0_REG_BASE;                           // Register base of module MMSS_VBIF_MM_MMU_GLOBAL0
extern uintPtr MMSS_VBIF_MM_MMU_GLOBAL1_REG_BASE;                           // Register base of module MMSS_VBIF_MM_MMU_GLOBAL1
extern uintPtr MMSS_VBIF_MM_MMU_IMPL_DEF_REG_BASE;                          // Register base of module MMSS_VBIF_MM_MMU_IMPL_DEF
extern uintPtr MMSS_VBIF_MM_MMU_PERF_MON_REG_BASE;                          // Register base of module MMSS_VBIF_MM_MMU_PERF_MON
extern uintPtr MMSS_VBIF_MM_MMU_SSD_REG_BASE;                               // Register base of module MMSS_VBIF_MM_MMU_SSD
extern uintPtr MMSS_MDSS_SEC_DSI_0_SEC_REG_BASE;                            // Register base of module MMSS_MDSS_SEC_DSI_0_SEC
extern uintPtr MMSS_MDSS_SEC_DSI_1_SEC_REG_BASE;                            // Register base of module MMSS_MDSS_SEC_DSI_1_SEC
extern uintPtr MMSS_MDP_AADC_0_REG_BASE;                                    // Register base of module MMSS_MDP_AADC_0
extern uintPtr MMSS_MDP_AADC_1_REG_BASE;                                    // Register base of module MMSS_MDP_AADC_1
extern uintPtr MMSS_MDP_AADC_2_REG_BASE;                                    // Register base of module MMSS_MDP_AADC_2
extern uintPtr MMSS_MDP_ENCR_NS_0_REG_BASE;                                 // Register base of module MMSS_MDP_ENCR_NS_0
extern uintPtr MMSS_MDP_ENCR_S_0_REG_BASE;                                  // Register base of module MMSS_MDP_ENCR_S_0


#define MMSS_MDSS_REG_BASE                                        (MDSS_BASE_PTR)

#include "mdssreg.h"

#endif // MDSSHWIO_H
