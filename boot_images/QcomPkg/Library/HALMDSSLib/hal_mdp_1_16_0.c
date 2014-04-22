/*! \file */

/*
===========================================================================

FILE:         hal_mdp_1_16_0.c

DESCRIPTION:  HAL changes like caps, offsets etc, specific to MDSS 1.16.0 
  
===========================================================================
===========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"
#include "mdssreg_1.16.0.h"

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/
/** Format: HAL_MDP_<MAJOR>_<MINOR>_<STEP>_XXX */
#define  HAL_MDP_1_16_0_NUM_OF_RGB_LAYERS                       2     /** RGB Source Surface Pixel Processors         */
#define  HAL_MDP_1_16_0_NUM_OF_VIG_LAYERS                       1     /** VIG Source Surface Pixel Processors          */
#define  HAL_MDP_1_16_0_NUM_OF_DMA_LAYERS                       1     /** DMA Source Surface Pixel Processors         */
#define  HAL_MDP_1_16_0_NUM_OF_CURSOR_LAYERS                    1     /** CURSOR Source Surface Pixel Processors   */
#define  HAL_MDP_1_16_0_NUM_OF_LAYER_MIXERS                     2     /** Layer Mixers                                            */
#define  HAL_MDP_1_16_0_NUM_OF_DSPP                             1     /** Destination Surface Pixel Processor            */
#define  HAL_MDP_1_16_0_NUM_OF_PHYSICAL_INTERFACES              2     /** INTF_1 and INTF_2                                   */
#define  HAL_MDP_1_16_0_NUM_OF_WRITEBACK_INTERFACES             2     /** WB_0 & WB_2                                          */
#define  HAL_MDP_1_16_0_NUM_OF_CONTROL_PATHS                    5     /** MDP_CTL_x (x = 0, 1, 2, 3 and 4)                     */
#define  HAL_MDP_1_16_0_NUM_OF_DATA_PATHS                       3     /** MDP_WB_x (x = 0 and 1)                      */
#define  HAL_MDP_1_16_0_NUM_OF_PINGPONGS                        2     /** Pingpong blocks                                       */
#define  HAL_MDP_1_16_0_NUM_OF_WATCHDOGS                        2     /** Watch Dogs                                             */

/** RGB and VG and DMA IGC LUT Size */
#define  HAL_MDP_1_16_0_SSPP_IGC_LUT_SIZE                       256
#define  HAL_MDP_1_16_0_SSPP_IGC_NUM_OF_COLOR_COMPONENTS        3     /** Color 0, 1 and 2                                       */

/** DSPP IGC LUT Size */
#define  HAL_MDP_1_16_0_DSPP_IGC_LUT_SIZE                       256
#define  HAL_MDP_1_16_0_DSPP_IGC_NUM_OF_COLOR_COMPONENTS        3     /** Color 0, 1 and 2                                        */

/** LAYER MIXER */
#define  HAL_MDP_1_16_0_LAYER_MIXER_MAX_BLEND_STAGES            7     /** Blend Stage #0 and 1                                */

/** QOS */
#define HAL_MDP_1_16_0_MAX_MDPCORE_CLK_FREQ                     320000000   /** MDP core maximum working clock frequency in Hz */

/* QoS priority re-mapping for real time read clients; real time clients are ViG, RGB, and DMA in line mode;*/
#define HAL_MDP_1_16_0_QOS_REMAPPER_REALTIME_CLIENTS            HAL_MDP_QOS_REMAPPER_INFO(                \
                                                                HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                                HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                                HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                                HAL_MDP_TRFCTRL_LATENCY_REALTIME )

/* QoS priority re-mapping for non-real time read clients; non-real time clients are DMA in block mode;*/
#define HAL_MDP_1_16_0_QOS_REMAPPER_NONREALTIME_CLIENTS         HAL_MDP_QOS_REMAPPER_INFO(                \
                                                                HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                                HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                                HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                                HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME )

/** VBIF max burst length */
#define HAL_MDP_1_16_0_VBIF_MAX_DDR_RD_BURST_SIZE              15

/** VBIF arbitration */
#define HAL_MDP_1_16_0_VBIF_ROUND_ROBIN_QOS_ARB                 (HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_RR_QOS_EN_BMSK |\
                                                                 HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_MMU_RR_QOS_EN_BMSK)
  /**QSEED Scale Filter caps */
#define  HAL_MDP_1_16_0_QSEED_FILTER_CAPS                       (HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_NEAREST_NEIGHBOR_FILTER) | \
                                                                 HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_BILINEAR_FILTER)         | \
                                                                 HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_PCMN_FILTER)             | \
                                                                 HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_CAF_FILTER))


/****************************************************************************
*
** FUNCTION: HAL_MDP_SetHWBlockRegOffsets_1_16_0()
*/
/*!
* \brief
*     set MDP HW block register offsets
*
*
****************************************************************************/
static void HAL_MDP_SetHWBlockRegOffsets_1_16_0(void)
{
  /* Control path HW block register offset */
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_NONE]           = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_0]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_1]              = (MMSS_MDP_CTL_1_REG_BASE_OFFS - MMSS_MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_2]              = (MMSS_MDP_CTL_2_REG_BASE_OFFS - MMSS_MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_3]              = (MMSS_MDP_CTL_3_REG_BASE_OFFS - MMSS_MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_4]              = (MMSS_MDP_CTL_4_REG_BASE_OFFS - MMSS_MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_5]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_6]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_7]              = 0x00000000;

  /* Data path HW block register offset */
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_NONE]                 = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_0]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_1]                    = (MMSS_MDP_WB_1_REG_BASE_OFFS - MMSS_MDP_WB_0_REG_BASE_OFFS);
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_2]                    = (MMSS_MDP_WB_2_REG_BASE_OFFS - MMSS_MDP_WB_0_REG_BASE_OFFS);
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_3]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_4]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_5]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_6]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_7]                    = 0x00000000;

  /* Destination (DSPP) HW block register offset */
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_NONE]              = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_0]                 = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_1]                 = (MMSS_MDP_VP_0_DSPP_1_REG_BASE_OFFS - MMSS_MDP_VP_0_DSPP_0_REG_BASE_OFFS);
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_2]                 = (MMSS_MDP_VP_0_DSPP_2_REG_BASE_OFFS - MMSS_MDP_VP_0_DSPP_0_REG_BASE_OFFS);
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_3]                 = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_4]                 = 0x00000000;

  /* Physical interface HW block register offset */
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_ID_NONE] = 0x00000000;
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_0]       = 0x00000000;
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_1]       = (MMSS_MDP_INTF_1_REG_BASE_OFFS - MMSS_MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_2]       = (MMSS_MDP_INTF_2_REG_BASE_OFFS - MMSS_MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_3]       = (MMSS_MDP_INTF_3_REG_BASE_OFFS - MMSS_MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_4]       = (MMSS_MDP_INTF_4_REG_BASE_OFFS - MMSS_MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_5]       = 0x00000000;

  /* Layer mixer HW block register offset */
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_NONE]             = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_0]                = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_1]                = (MMSS_MDP_VP_0_LAYER_1_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_2]                = (MMSS_MDP_VP_0_LAYER_2_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_3]                = (MMSS_MDP_VP_0_LAYER_3_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_4]                = (MMSS_MDP_VP_0_LAYER_4_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_5]                = (MMSS_MDP_VP_0_LAYER_5_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_6]                = 0x00000000;

  /* Layer mixer blending stage register offset */
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_0]      = 0x00000000;
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_1]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND1_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_2]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND2_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_3]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND3_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_4]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND4_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_5]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND5_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_6]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND6_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_CURSOR] = 0x00000000;

  /* PingPong HW block register offset */
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_NONE]                  = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_0]                     = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_1]                     = (MMSS_MDP_PP_1_REG_BASE_OFFS - MMSS_MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_2]                     = (MMSS_MDP_PP_2_REG_BASE_OFFS - MMSS_MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_3]                     = (MMSS_MDP_PP_3_REG_BASE_OFFS - MMSS_MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_4]                     = (MMSS_MDP_PP_4_REG_BASE_OFFS - MMSS_MDP_PP_0_REG_BASE_OFFS);

  /* PingPong Buffer (PPB) HW block register offset */
  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_NONE]            = 0x00000000;
  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_0]               = 0x00000000;
  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_1]               = (HWIO_MMSS_MDP_PPB1_CNTL_OFFS - HWIO_MMSS_MDP_PPB0_CNTL_OFFS);
  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_2]               = 0x00000000;
  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_3]               = 0x00000000;
  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_4]               = 0x00000000;

  /* Source (SSPP) HW block register offset */
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_NONE]                   = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_0]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_1]                  = (MMSS_MDP_VP_0_VIG_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_2]                  = (MMSS_MDP_VP_0_VIG_2_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_3]                  = (MMSS_MDP_VP_0_VIG_3_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_0]                  = (MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_1]                  = (MMSS_MDP_VP_0_RGB_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_2]                  = (MMSS_MDP_VP_0_RGB_2_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_3]                  = (MMSS_MDP_VP_0_RGB_3_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_0]                  = (MMSS_MDP_VP_0_DMA_0_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_1]                  = (MMSS_MDP_VP_0_DMA_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_2]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_3]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_0]               = (MMSS_MDP_VP_0_CURSOR_0_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_1]               = (MMSS_MDP_VP_0_CURSOR_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_2]               = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_3]               = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_4]               = 0x00000000;

  /* Source (SSPP) Layer (Scalar) HW block register offset */
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_NONE]                   = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_0]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_1]                  = (HWIO_MMSS_MDP_VP_0_VIG_1_OP_MODE_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_2]                  = (HWIO_MMSS_MDP_VP_0_VIG_2_OP_MODE_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_3]                  = (HWIO_MMSS_MDP_VP_0_VIG_3_OP_MODE_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_0]                  = (HWIO_MMSS_MDP_VP_0_RGB_0_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_1]                  = (HWIO_MMSS_MDP_VP_0_RGB_1_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_2]                  = (HWIO_MMSS_MDP_VP_0_RGB_2_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_3]                  = (HWIO_MMSS_MDP_VP_0_RGB_3_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_0]                  = (HWIO_MMSS_MDP_VP_0_DMA_0_SSPP_SW_PIX_EXT_C0_LR_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_1]                  = (HWIO_MMSS_MDP_VP_0_DMA_1_SSPP_SW_PIX_EXT_C0_LR_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_2]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_3]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_0]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_1]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_2]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_3]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_4]               = 0x00000000;

  // Interface stream-based MISR configuration register offsets
  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_VIDEO_0]  = 0x00000000;
  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_CMD_0]    = (HWIO_MMSS_MDP_INTF_0_CMD_0_MISR_CTRL_OFFS - HWIO_MMSS_MDP_INTF_0_VIDEO_MISR_CTRL_OFFS);
  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_CMD_1]    = (HWIO_MMSS_MDP_INTF_0_CMD_1_MISR_CTRL_OFFS - HWIO_MMSS_MDP_INTF_0_VIDEO_MISR_CTRL_OFFS);
  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_CMD_2]    = (HWIO_MMSS_MDP_INTF_0_CMD_2_MISR_CTRL_OFFS - HWIO_MMSS_MDP_INTF_0_VIDEO_MISR_CTRL_OFFS);
  
  // SSPP Pixel Ext Component Offset
  uMDPSSPPPixExtRegBaseOffset[0]                                     = HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS   - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS;
  uMDPSSPPPixExtRegBaseOffset[1]                                     = HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C1C2_LR_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS;
  uMDPSSPPPixExtRegBaseOffset[2]                                     = HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C1C2_LR_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS;
  uMDPSSPPPixExtRegBaseOffset[3]                                     = HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C3_LR_OFFS   - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_OFFS;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadHardwareInfo_1_16_0()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDP Version(515)
*
* \param [out]  psHwInfo               - Hardware information
* \param [out]  psMdpHwInfo         - MDP hardware information 
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_ReadHardwareInfo_1_16_0(HAL_MDP_HwInfo             *psHwInfo,
                                     HAL_MDP_Hw_Private_Info    *psMdpHwInfo)
{
   // Assign HW block register base. Values come from auto-generated mdssreg.h 
   
   MMSS_MDP_REG_BASE                                                = (MMSS_MDSS_REG_BASE + 0x00001000); 
   MMSS_MDP_SSPP_TOP0_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x00001000);
   MMSS_MDP_SMP_TOP0_REG_BASE                                       = (MMSS_MDSS_REG_BASE + 0x00001030);
   MMSS_MDP_SSPP_TOP1_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x000011e8);
   MMSS_MDP_SMP_TOP1_REG_BASE                                       = (MMSS_MDSS_REG_BASE + 0x000012fc);
   MMSS_MDP_DSPP_TOP0_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x00001300);
   MMSS_MDP_PERIPH_TOP0_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x00001380);
   MMSS_MDP_SSPP_TOP2_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x000013a8);
   MMSS_MDP_PERIPH_TOP1_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x000013f0);
   MMSS_MDP_CTL_0_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00002000);
   MMSS_MDP_CTL_1_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00002200);
   MMSS_MDP_CTL_2_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00002400);
   MMSS_MDP_CTL_3_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00002600);
   MMSS_MDP_CTL_4_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00002800);
   MMSS_MDP_SSPP_IGC_LUT_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x00003000);
   MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00005000);
   MMSS_MDP_VP_0_VIG_0_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00005000);
   MMSS_MDP_VP_0_VIG_0_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00006780);
   MMSS_MDP_VP_0_VIG_1_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00007000);
   MMSS_MDP_VP_0_VIG_1_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00007000);
   MMSS_MDP_VP_0_VIG_1_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00008780);
   MMSS_MDP_VP_0_VIG_2_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00009000);
   MMSS_MDP_VP_0_VIG_2_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00009000);  
   MMSS_MDP_VP_0_VIG_2_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x0000a780);
   MMSS_MDP_VP_0_VIG_3_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0000b000);
   MMSS_MDP_VP_0_VIG_3_REG_BASE 		  	                        = (MMSS_MDSS_REG_BASE + 0x0000b000);	
   MMSS_MDP_VP_0_VIG_3_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x0000c780);
   MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00015000);
   MMSS_MDP_VP_0_RGB_0_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00015000);
   MMSS_MDP_VP_0_RGB_0_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00015380);
   MMSS_MDP_VP_0_RGB_1_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00017000);
   MMSS_MDP_VP_0_RGB_1_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00017000);
   MMSS_MDP_VP_0_RGB_1_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00017380);
   MMSS_MDP_VP_0_RGB_2_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00019000);
   MMSS_MDP_VP_0_RGB_2_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00019000);
   MMSS_MDP_VP_0_RGB_2_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00019380);
   MMSS_MDP_VP_0_RGB_3_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0001b000);
   MMSS_MDP_VP_0_RGB_3_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x0001b000);
   MMSS_MDP_VP_0_RGB_3_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x0001b380);
   MMSS_MDP_VP_0_DMA_0_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00025000);
   MMSS_MDP_VP_0_DMA_0_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00025200);
   MMSS_MDP_VP_0_DMA_0_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00025380);
   MMSS_MDP_VP_0_DMA_1_SSPP_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00027000);  
   MMSS_MDP_VP_0_DMA_1_REG_BASE                                     = (MMSS_MDSS_REG_BASE + 0x00027200);
   MMSS_MDP_VP_0_DMA_1_PCC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00027380);
   MMSS_MDP_VP_0_CURSOR_0_SSPP_REG_BASE                             = (MMSS_MDSS_REG_BASE + 0x00035000);
   MMSS_MDP_VP_0_CURSOR_1_SSPP_REG_BASE                             = (MMSS_MDSS_REG_BASE + 0x00037000);
   MMSS_MDP_VP_0_LAYER_0_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x00045000);
   MMSS_MDP_VP_0_LAYER_0_PGC_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x000453c0);
   MMSS_MDP_VP_0_LAYER_1_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x00046000);
   MMSS_MDP_VP_0_LAYER_1_PGC_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x000463c0);
   MMSS_MDP_VP_0_LAYER_2_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x00047000); 
   MMSS_MDP_VP_0_LAYER_2_PGC_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x000473c0);
   MMSS_MDP_VP_0_LAYER_3_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x00048000);
   MMSS_MDP_VP_0_LAYER_3_PGC_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x000483c0);
   MMSS_MDP_VP_0_LAYER_4_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x00049000);  
   MMSS_MDP_VP_0_LAYER_4_PGC_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x000493c0);
   MMSS_MDP_VP_0_LAYER_5_REG_BASE		  	                            = (MMSS_MDSS_REG_BASE + 0x0004a000);
   MMSS_MDP_VP_0_LAYER_5_PGC_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x0004a3c0);
   MMSS_MDP_VP_0_DSPP_0_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x00055000);
   MMSS_MDP_VP_0_DSPP_0_GAMUT_REG_BASE                              = (MMSS_MDSS_REG_BASE + 0x00056600);
   MMSS_MDP_VP_0_DSPP_0_PCC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00056700);
   MMSS_MDP_VP_0_DSPP_0_PGC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x000567c0);
   MMSS_MDP_VP_0_DSPP_1_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x00057000);
   MMSS_MDP_VP_0_DSPP_1_GAMUT_REG_BASE                              = (MMSS_MDSS_REG_BASE + 0x00058600);
   MMSS_MDP_VP_0_DSPP_1_PCC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00058700);
   MMSS_MDP_VP_0_DSPP_1_PGC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x000587c0);
   MMSS_MDP_VP_0_DSPP_2_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x00059000);  
   MMSS_MDP_VP_0_DSPP_2_GAMUT_REG_BASE                              = (MMSS_MDSS_REG_BASE + 0x0005a600);
   MMSS_MDP_VP_0_DSPP_2_PCC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0005a700);
   MMSS_MDP_VP_0_DSPP_2_PGC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0005a7c0);
   MMSS_MDP_VP_0_DSPP_3_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x0005b000);
   MMSS_MDP_VP_0_DSPP_3_GAMUT_REG_BASE                              = (MMSS_MDSS_REG_BASE + 0x0005c600);
   MMSS_MDP_VP_0_DSPP_3_PCC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0005c700);
   MMSS_MDP_VP_0_DSPP_3_PGC_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0005c7c0);
   MMSS_MDP_WB_0_REG_BASE                                           = (MMSS_MDSS_REG_BASE + 0x00065000);
   MMSS_MDP_WB_1_REG_BASE                                           = (MMSS_MDSS_REG_BASE + 0x00065800);  
   MMSS_MDP_WB_2_REG_BASE                                           = (MMSS_MDSS_REG_BASE + 0x00066000);   
   MMSS_MDP_INTF_0_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x0006b000);
   MMSS_MDP_INTF_1_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x0006b800);
   MMSS_MDP_INTF_2_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x0006c000);  
   MMSS_MDP_INTF_3_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x0006c800);  
   MMSS_MDP_INTF_4_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x0006d000);
   MMSS_MDP_PP_0_REG_BASE                                           = (MMSS_MDSS_REG_BASE + 0x00071000);
   MMSS_MDP_PP_1_REG_BASE                                           = (MMSS_MDSS_REG_BASE + 0x00071800);
   MMSS_MDP_PP_2_REG_BASE                                           = (MMSS_MDSS_REG_BASE + 0x00072000);  
   MMSS_MDP_PP_3_REG_BASE				  	                                = (MMSS_MDSS_REG_BASE + 0x00072800);	
   MMSS_MDP_PP_4_REG_BASE				  	                                = (MMSS_MDSS_REG_BASE + 0x00073000);	
   MMSS_MDP_CDM_0_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x0007a200);
   MMSS_MDP_CDM_0_CSC_10_REG_BASE                                   = (MMSS_MDSS_REG_BASE + 0x0007a200);
   MMSS_MDP_CDM_0_CDWN2_REG_BASE                                    = (MMSS_MDSS_REG_BASE + 0x0007a300);
   MMSS_MDP_CDM_0_HDMI_PACK_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x0007a400);
   MMSS_MDP_DSC_0_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00081000);
   MMSS_MDP_DSC_1_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00081400);
   MMSS_DSI_0_REG_BASE                                              = (MMSS_MDSS_REG_BASE + 0x00094000);
   MMSS_DSI_0_PHY_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00094400);
   MMSS_DSI_0_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON_REG_BASE         = (MMSS_MDSS_REG_BASE + 0x00094400);
   MMSS_DSI_0_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00094500);
   MMSS_DSI_0_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00094580);
   MMSS_DSI_0_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00094600);
   MMSS_DSI_0_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00094680);   
   MMSS_DSI_0_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00094700);
   MMSS_DSI_0_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_COM_LITE_REG_BASE	      = (MMSS_MDSS_REG_BASE + 0x00094800);
   MMSS_DSI_1_REG_BASE                                              = (MMSS_MDSS_REG_BASE + 0x00096000);
   MMSS_DSI_1_PHY_REG_BASE                                          = (MMSS_MDSS_REG_BASE + 0x00096400);
   MMSS_DSI_1_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON_REG_BASE 	      = (MMSS_MDSS_REG_BASE + 0x00096400);
   MMSS_DSI_1_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN_REG_BASE	      = (MMSS_MDSS_REG_BASE + 0x00096500);
   MMSS_DSI_1_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN_REG_BASE	      = (MMSS_MDSS_REG_BASE + 0x00096580);
   MMSS_DSI_1_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN_REG_BASE	      = (MMSS_MDSS_REG_BASE + 0x00096600);
   MMSS_DSI_1_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN_REG_BASE	      = (MMSS_MDSS_REG_BASE + 0x00096680);
   MMSS_DSI_1_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00096700);
   MMSS_DSI_1_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_COM_LITE_REG_BASE       = (MMSS_MDSS_REG_BASE + 0x00096800);
   MMSS_VBIF_VBIF_MDP_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x000b0000);  
   MMSS_MDSS_SEC_DSI_0_SEC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x000d9000);
   MMSS_MDSS_SEC_DSI_1_SEC_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x000da000);
   MMSS_MDP_AADC_0_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x00079000);
   MMSS_MDP_AADC_1_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x00079800);
   MMSS_MDP_AADC_2_REG_BASE                                         = (MMSS_MDSS_REG_BASE + 0x0007a000);
   MMSS_MDP_ENCR_NS_0_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x00077000);
   MMSS_MDP_ENCR_S_0_REG_BASE                                       = (MMSS_MDSS_REG_BASE + 0x00077400);

 
   HAL_MDP_SetHWBlockRegOffsets_1_16_0();
   /*
    * Notes: This file only contains differences between MDP_1.0.0 and MDP_1.16.0
    */
    if(NULL != psHwInfo)
    {
       psHwInfo->uNumOfRGBLayers              = HAL_MDP_1_16_0_NUM_OF_RGB_LAYERS;
       psHwInfo->uNumOfVIGLayers              = HAL_MDP_1_16_0_NUM_OF_VIG_LAYERS;
       psHwInfo->uNumOfDMALayers              = HAL_MDP_1_16_0_NUM_OF_DMA_LAYERS;
       psHwInfo->uNumOfCursorLayers           = HAL_MDP_1_16_0_NUM_OF_CURSOR_LAYERS;
       psHwInfo->uNumOfLayerMixers            = HAL_MDP_1_16_0_NUM_OF_LAYER_MIXERS;
       psHwInfo->uNumOfDSPPs                  = HAL_MDP_1_16_0_NUM_OF_DSPP;
       psHwInfo->uNumOfBltEngines             = HAL_MDP_1_16_0_NUM_OF_WRITEBACK_INTERFACES;
       psHwInfo->uNumOfControlPaths           = HAL_MDP_1_16_0_NUM_OF_CONTROL_PATHS;
       psHwInfo->uNumOfDataPaths              = HAL_MDP_1_16_0_NUM_OF_DATA_PATHS;
       psHwInfo->uNumOfLayerMixerBlendStages  = HAL_MDP_1_16_0_LAYER_MIXER_MAX_BLEND_STAGES;              
       psHwInfo->uNumOfPhyInterfaces          = HAL_MDP_1_16_0_NUM_OF_PHYSICAL_INTERFACES;
       psHwInfo->uNumOfPingPongs              = HAL_MDP_1_16_0_NUM_OF_PINGPONGS;
       psHwInfo->uNumOfWatchDogs              = HAL_MDP_1_16_0_NUM_OF_WATCHDOGS;
    }
 
    if(NULL != psMdpHwInfo)
    {
       psMdpHwInfo->uMaxMdpCoreClkFreq                 = HAL_MDP_1_16_0_MAX_MDPCORE_CLK_FREQ;
   
       //VBIF       
       psMdpHwInfo->uVBIFMaxDdrRdBurstSize             = HAL_MDP_1_16_0_VBIF_MAX_DDR_RD_BURST_SIZE;
       psMdpHwInfo->uVBIFRoundRobinQosArb              = HAL_MDP_1_16_0_VBIF_ROUND_ROBIN_QOS_ARB;
 
       //QoS
       psMdpHwInfo->uQosRemapperRealTimeClients        = HAL_MDP_1_16_0_QOS_REMAPPER_REALTIME_CLIENTS;
       psMdpHwInfo->uQosRemapperNonRealTimeClients     = HAL_MDP_1_16_0_QOS_REMAPPER_NONREALTIME_CLIENTS;
       
       //DST PACK PATTERN
       psMdpHwInfo->pDstPackPatternInfo                = (uint32 *)&guSrcUnpackInfo;
 
       //VBIF
       psMdpHwInfo->uVBIFRoundRobinQosArb              = HAL_MDP_1_16_0_VBIF_ROUND_ROBIN_QOS_ARB;
 
       // SMP client ID mapping
       psMdpHwInfo->pClientToHWClientMap               = NULL;      
 
       // Histogram config function
       psMdpHwInfo->sIpFxnTbl.HistogramConfig          = HAL_MDP_DSPP_HistogramV2_Config;
 
       // Histogram lock function
       psMdpHwInfo->sIpFxnTbl.HistogramLock            = HAL_MDP_DSPP_HistogramV2_Lock;
   
       // QSEED Scale Filter caps
       psMdpHwInfo->uQSEEDSupportedScaleFilters        = HAL_MDP_1_16_0_QSEED_FILTER_CAPS;

       // SW pixel extension is required for all newer chipsets  
       psMdpHwInfo->bUseSWPixExt                       = TRUE;       
    }
 }

#ifdef __cplusplus
}
#endif
