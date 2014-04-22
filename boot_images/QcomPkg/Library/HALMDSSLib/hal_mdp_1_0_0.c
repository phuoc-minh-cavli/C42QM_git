/*! \file */

/*
===========================================================================

FILE:         hal_mdp_1_0_0.c

DESCRIPTION:  MDP Initialization
  
===========================================================================
===========================================================================
Copyright (c) 2012 - 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"
#include "mdssreg_1.0.0.h"

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/
/** Format: HAL_MDP_<MAJOR>_<MINOR>_<STEP>_XXX */
#define  HAL_MDP_1_0_0_NUM_OF_RGB_LAYERS                       3     /** RGB Source Surface Pixel Processors      */
#define  HAL_MDP_1_0_0_NUM_OF_VIG_LAYERS                       3     /** VIG Source Surface Pixel Processors      */
#define  HAL_MDP_1_0_0_NUM_OF_DMA_LAYERS                       2     /** DMA Source Surface Pixel Processors      */
#define  HAL_MDP_1_0_0_NUM_OF_CURSOR_LAYERS                    3     /** CURSOR Source Surface Pixel Processors   */
#define  HAL_MDP_1_0_0_NUM_OF_LAYER_MIXERS                     5     /** Layer Mixers */
#define  HAL_MDP_1_0_0_NUM_OF_DSPP                             3     /** Destination Surface Pixel Processor      */
#define  HAL_MDP_1_0_0_NUM_OF_PHYSICAL_INTERFACES              4     /** INTF_0, INTF_1, INTF_2 and INTF_3        */
#define  HAL_MDP_1_0_0_NUM_OF_WRITEBACK_INTERFACES             3     /** WB_0, WB_1 and WB_2                      */
#define  HAL_MDP_1_0_0_NUM_OF_CONTROL_PATHS                    5     /** MDP_CTL_x (x = 0, 1, 2, 3, and 4)        */
#define  HAL_MDP_1_0_0_NUM_OF_DATA_PATHS                       5     /** MDP_WB_x (x = 0, 1, 2, 3, and 4)         */
#define  HAL_MDP_1_0_0_NUM_OF_PINGPONGS                        3     /** Pingpong blocks                          */
#define  HAL_MDP_1_0_0_NUM_OF_WATCHDOGS                        0     /** Watch Dogs                               */
#define  HAL_MDP_1_0_0_NUM_OF_AXI_PORTS                        1     /** Number of AXI ports                      */

/** RGB and VG and DMA IGC LUT Size */
#define  HAL_MDP_1_0_0_SSPP_IGC_LUT_SIZE                       256
#define  HAL_MDP_1_0_0_SSPP_IGC_NUM_OF_COLOR_COMPONENTS        3     /** Color 0, 1 and 2                         */

/** DSPP IGC LUT Size */
#define  HAL_MDP_1_0_0_DSPP_IGC_LUT_SIZE                       256
#define  HAL_MDP_1_0_0_DSPP_IGC_NUM_OF_COLOR_COMPONENTS        3     /** Color 0, 1 and 2                         */

/** QSEED */
#define  HAL_MDP_1_0_0_QSEED_NORM_FILTER_COEFF_COUNT           128
#define  HAL_MDP_1_0_0_QSEED_SMOOTH_FILTER_COEFF_COUNT         128
/** QSEED Scale Filter caps */
#define  HAL_MDP_1_0_0_QSEED_FILTER_CAPS                       (HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_NEAREST_NEIGHBOR_FILTER) | \
                                                                HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_BILINEAR_FILTER)         | \
                                                                HAL_MDP_QSEED_FILTER_CAP(HAL_MDP_SCALAR_PCMN_FILTER))
  

/** LAYER MIXER */
#define  HAL_MDP_1_0_0_LAYER_MIXER_MAX_BLEND_STAGES            4     /** Blend Stage #0, 1, 2 and 3               */

/** QOS */
#define HAL_MDP_1_0_0_MAX_MDPCORE_CLK_FREQ                     320000000   /** MDP core maximum working clock frequency in Hz */

#define HAL_MDP_1_0_0_QOS_REQPRI_WATERMARK_REGS_COUNT          3


/* QoS priority re-mapping for real time read clients; real time clients are ViG, RGB, and DMA in line mode;*/
#define HAL_MDP_1_0_0_QOS_REMAPPER_REALTIME_CLIENTS            HAL_MDP_QOS_REMAPPER_INFO(                \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME )
/* QoS priority re-mapping for non-real time read clients; non-real time clients are DMA in block mode;*/
#define HAL_MDP_1_0_0_QOS_REMAPPER_NONREALTIME_CLIENTS         HAL_MDP_QOS_REMAPPER_INFO(                \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME )

/** VBIF max burst length */
#define HAL_MDP_1_0_0_VBIF_MAX_DDR_RD_BURST_SIZE               7
#define HAL_MDP_1_0_0_VBIF_MAX_DDR_WR_BURST_SIZE               7
#define HAL_MDP_1_0_0_VBIF_ROUND_ROBIN_QOS_ARB                 HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_RR_QOS_EN_BMSK

/** Clock control */
#define HAL_MDP_1_0_0_HYSTERISIS_DEFAULT                       3

/** HSIC register range */
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_HUE                  (-768)   // -180 degree
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_HUE                  (-768)   // 180 degree
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_UNCHANGE_HUE          0       // value for unchange hue going to the min hue direction
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_UNCHANGE_HUE         (-1536)  // value for unchange hue going to the max hue direction
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_SATURATION           (-128)   
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_SATURATION            127
#define HAL_MDP_1_0_0_PICTURE_ADJUST_UNCHANGE_SATURATION       0
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_INTENSITY            (-128) 
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_INTENSITY             127 
#define HAL_MDP_1_0_0_PICTURE_ADJUST_UNCHANGE_INTENSITY        0
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_CONTRAST             (-128)
#define HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_CONTRAST              127
#define HAL_MDP_1_0_0_PICTURE_ADJUST_UNCHANGE_CONTRAST         0

/*------------------------------------------------------------------------
 * Global Data Definitions
 *------------------------------------------------------------------------ */

/* SMP client to h/w client id mapping */
const uint32 gMDP_SMPClientMap_1_0_0[HAL_MDP_SMP_MMB_CLIENT_MAX] = 
{
   0x0,     //HAL_MDP_SMP_MMB_CLIENT_NONE  
   0x1,     //HAL_MDP_SMP_MMB_CLIENT_VIG0_FETCH_Y,
   0x2,     //HAL_MDP_SMP_MMB_CLIENT_VIG0_FETCH_CR,
   0x3,     //HAL_MDP_SMP_MMB_CLIENT_VIG0_FETCH_CB,
   0x4,     //HAL_MDP_SMP_MMB_CLIENT_VIG1_FETCH_Y,
   0x5,     //HAL_MDP_SMP_MMB_CLIENT_VIG1_FETCH_CR,
   0x6,     //HAL_MDP_SMP_MMB_CLIENT_VIG1_FETCH_CB,
   0x7,     //HAL_MDP_SMP_MMB_CLIENT_VIG2_FETCH_Y,
   0x8,     //HAL_MDP_SMP_MMB_CLIENT_VIG2_FETCH_CR,
   0x9,     //HAL_MDP_SMP_MMB_CLIENT_VIG2_FETCH_CB,
   0x0,     //HAL_MDP_SMP_MMB_CLIENT_VIG3_FETCH_Y,
   0x0,     //HAL_MDP_SMP_MMB_CLIENT_VIG3_FETCH_CR,
   0x0,     //HAL_MDP_SMP_MMB_CLIENT_VIG3_FETCH_CB,
   0xA,     //HAL_MDP_SMP_MMB_CLIENT_DMA0_FETCH_Y,
   0xB,     //HAL_MDP_SMP_MMB_CLIENT_DMA0_FETCH_CR,
   0xC,     //HAL_MDP_SMP_MMB_CLIENT_DMA0_FETCH_CB,
   0xD,     //HAL_MDP_SMP_MMB_CLIENT_DMA1_FETCH_Y,
   0xE,     //HAL_MDP_SMP_MMB_CLIENT_DMA1_FETCH_CR,
   0xF,     //HAL_MDP_SMP_MMB_CLIENT_DMA1_FETCH_CB,
   0x10,    //HAL_MDP_SMP_MMB_CLIENT_RGB0_FETCH,
   0x11,    //HAL_MDP_SMP_MMB_CLIENT_RGB1_FETCH,
   0x12,    //HAL_MDP_SMP_MMB_CLIENT_RGB2_FETCH,
   0x0,     //HAL_MDP_SMP_MMB_CLIENT_RGB3_FETCH,
};

// Destination pack info mapping for V1
uint32 guDstPackInfo_1_0_0[HAL_MDP_PIXEL_FORMAT_MAX] =
{
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_NONE,
   0x00010001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP
   0x00010001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP
   0x00010001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP
   0x03010001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP
   0x03010001,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP
   0x03010001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP
   0x00008002,                                     // HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP
   0x00008002,                                     // HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP
   0x03008002,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP
   0x03008002,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP
   0x03008002,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGB_OXILI_TILE_4x4_24BPP
   0x03010001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP

   0x00010100,                                     // HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
   0x00008002,                                     // HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
   0x02000100,                                     // HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
   0x01000200,                                     // HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
   0x03000102,                                     // HAL_MDP_PIXEL_FORMAT_AYCBCR_H1V1_32BPP

   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_VENUS_4x4_12BPP

   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
};

/* 
 * MDP has a set of MDP_CLK_STATUSx registers to read the active/idle state of the MDP modules (SSPP, DSPP etc).
 * The following tables provide a mapping information of MDP_CLK_STATUSx register and BitFieldMask for each MDP module
 *
 *    (a)   gsSSPPModuleStatusRegInfo        --> Mapping table for SSPPx (VIGx, RGBx, CURSORx and DMAx) modules
 *    (b)   gsDSPPStatusRegInfo              --> Mapping table for DSPPx modules
 *    (c)   gsPingPongStatusRegInfo          --> Mapping table for PING-PONGx modules
 *    (c)   gsWBStatusRegInfo                --> Mapping table for WBx modules
 */
static HAL_MDP_ModuleStatusRegInfo     gsSSPPModuleStatusRegInfo[HAL_MDP_SOURCE_PIPE_MAX] =
{
 //{ uRegAddrOffset  uBitFieldMask                                         }
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_NONE
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS0_VIG0_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_0
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS1_VIG1_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_1
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS2_VIG2_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_2
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS6_VIG3_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_VIG_4
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS0_RGB0_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_0
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS1_RGB1_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_1
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS2_RGB2_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_2
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS6_RGB3_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_RGB_4
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS0_DMA0_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_DMA_0 
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS1_DMA1_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_DMA_1
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_DMA_2
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_DMA_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_DMA_4
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS3_CURSOR0_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_0 
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS4_CURSOR1_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_1
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS5_CURSOR2_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_2
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS7_CURSOR3_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_CURSOR_4
};

static HAL_MDP_ModuleStatusRegInfo    gsDSPPStatusRegInfo[HAL_MDP_DESTINATION_PIPE_MAX] = 
{
 //{ uRegAddrOffset  uBitFieldMask                                         }
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_DESTINATION_PIPE_NONE
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS3_LM_DSPP0_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_0
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS4_LM_DSPP1_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_1
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS5_LM_DSPP2_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_2
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS7_LM_DSPP3_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_3
};

static HAL_MDP_ModuleStatusRegInfo    gsPingPongStatusRegInfo[HAL_MDP_PINGPONG_MAX] = 
{
 //{ uRegAddrOffset  uBitFieldMask                                         }
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_PINGPONG_NONE
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS3_PP0_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_0
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS4_PP1_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_1
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS5_PP2_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_2
   { 0x00000000,     HWIO_MMSS_MDP_CLK_STATUS7_PP3_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_PINGPONG_4
};

static HAL_MDP_ModuleStatusRegInfo    gsWBStatusRegInfo[HAL_MDP_INTERFACE_MAX - HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE] = 
{
 //{ uRegAddrOffset     uBitFieldMask                                         }
   { 0x00000000,        0x00000000                                            },    // HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE
   { 0x00000000,        HWIO_MMSS_MDP_CLK_STATUS3_ROT0_BLK_CLK_ACTIVE_BMSK    },    // HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_0
   { 0x00000000,        HWIO_MMSS_MDP_CLK_STATUS4_ROT1_BLK_CLK_ACTIVE_BMSK    },    // HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_1
   { 0x00000000,        HWIO_MMSS_MDP_CLK_STATUS3_ROT0_LINE_CLK_ACTIVE_BMSK   },    // HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_0
   { 0x00000000,        HWIO_MMSS_MDP_CLK_STATUS4_ROT1_LINE_CLK_ACTIVE_BMSK   },    // HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_1
   { 0x00000000,        HWIO_MMSS_MDP_CLK_STATUS5_WB2_CLK_ACTIVE_BMSK         },    // HAL_MDP_INTERFACE_MEMORY_WB_LINEAR_MODE
};

/****************************************************************************
*
** FUNCTION: HAL_MDP_SetHWBlockRegOffsets_1_0_0()
*/
/*!
* \brief
*     set MDP HW block register offsets
*
*
****************************************************************************/
static void HAL_MDP_SetHWBlockRegOffsets_1_0_0(void)
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
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_3]                    = (MMSS_MDP_WB_3_REG_BASE_OFFS - MMSS_MDP_WB_0_REG_BASE_OFFS);
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_4]                    = (MMSS_MDP_WB_4_REG_BASE_OFFS - MMSS_MDP_WB_0_REG_BASE_OFFS);
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
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_4]       = 0x00000000;
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_5]       = 0x00000000;

  /* Layer mixer HW block register offset */
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_NONE]             = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_0]                = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_1]                = (MMSS_MDP_VP_0_LAYER_1_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_2]                = (MMSS_MDP_VP_0_LAYER_2_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_3]                = (MMSS_MDP_VP_0_LAYER_3_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_4]                = (MMSS_MDP_VP_0_LAYER_4_REG_BASE_OFFS - MMSS_MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_5]                = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_6]                = 0x00000000;

  /* Layer mixer blending stage register offset */
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_0]      = 0x00000000;
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_1]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND1_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_2]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND2_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_3]      = (HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND3_OP_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_BLEND0_OP_OFFS);
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_4]      = 0x00000000;
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_5]      = 0x00000000;
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_CURSOR] = 0x00000000;

  /* PingPong HW block register offset */
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_NONE]                  = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_0]                     = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_1]                     = (MMSS_MDP_PP_1_REG_BASE_OFFS - MMSS_MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_2]                     = (MMSS_MDP_PP_2_REG_BASE_OFFS - MMSS_MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_3]                     = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_4]                     = 0x00000000;

  /* Source (SSPP) HW block register offset */
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_NONE]                   = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_0]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_1]                  = (MMSS_MDP_VP_0_VIG_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_2]                  = (MMSS_MDP_VP_0_VIG_2_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_3]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_0]                  = (MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_1]                  = (MMSS_MDP_VP_0_RGB_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_2]                  = (MMSS_MDP_VP_0_RGB_2_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_3]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_0]                  = (MMSS_MDP_VP_0_DMA_0_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_1]                  = (MMSS_MDP_VP_0_DMA_1_SSPP_REG_BASE_OFFS - MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_2]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_3]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_0]               = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_1]               = (HWIO_MMSS_MDP_VP_0_LAYER_1_CURSOR_IMG_SIZE_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_IMG_SIZE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_2]               = (HWIO_MMSS_MDP_VP_0_LAYER_2_CURSOR_IMG_SIZE_OFFS - HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_IMG_SIZE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_3]               = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_4]               = 0x00000000;

  /* Source (SSPP) Layer (Scalar) HW block register offset */
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_NONE]                   = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_0]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_1]                  = (HWIO_MMSS_MDP_VP_0_VIG_1_OP_MODE_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_2]                  = (HWIO_MMSS_MDP_VP_0_VIG_2_OP_MODE_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_3]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_0]                  = (HWIO_MMSS_MDP_VP_0_RGB_0_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_1]                  = (HWIO_MMSS_MDP_VP_0_RGB_1_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_2]                  = (HWIO_MMSS_MDP_VP_0_RGB_2_SCALE_CONFIG_OFFS - HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_3]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_0]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_1]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_2]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_3]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_0]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_1]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_2]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_3]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_4]               = 0x00000000;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadHardwareInfo_1_0_0()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDP Version(1.0.0)
*
* \param [out]  psHwInfo            - Hardware information
* \param [out]  psMdpHwInfo         - MDP hardware information 
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_ReadHardwareInfo_1_0_0(HAL_MDP_HwInfo             *psHwInfo,
                                    HAL_MDP_Hw_Private_Info    *psMdpHwInfo)
{
   // Assign HW block register base. Values come from auto-generated mdssreg.h
   MMSS_MDP_CTL_0_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00000600);
   MMSS_MDP_CTL_1_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00000700);
   MMSS_MDP_CTL_2_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00000800);
   MMSS_MDP_CTL_3_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00000900);
   MMSS_MDP_CTL_4_REG_BASE                                = (MMSS_MDSS_REG_BASE + 0x00000a00);
   MMSS_MDP_VP_0_VIG_0_REG_BASE                           = (MMSS_MDSS_REG_BASE + 0x00001200);
   MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00001200);
   MMSS_MDP_VP_0_VIG_1_REG_BASE                           = (MMSS_MDSS_REG_BASE + 0x00001600);
   MMSS_MDP_VP_0_VIG_1_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00001600);
   MMSS_MDP_VP_0_VIG_2_REG_BASE                           = (MMSS_MDSS_REG_BASE + 0x00001a00);
   MMSS_MDP_VP_0_VIG_2_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00001a00);
   MMSS_MDP_VP_0_VIG_3_REG_BASE                           = MMSS_MDP_VP_0_VIG_0_REG_BASE;
   MMSS_MDP_VP_0_VIG_3_SSPP_REG_BASE                      = MMSS_MDP_VP_0_VIG_0_SSPP_REG_BASE;
   MMSS_MDP_VP_0_RGB_0_REG_BASE                           = (MMSS_MDSS_REG_BASE + 0x00001e00);
   MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00001e00);
   MMSS_MDP_VP_0_RGB_1_REG_BASE                           = (MMSS_MDSS_REG_BASE + 0x00002200);
   MMSS_MDP_VP_0_RGB_1_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00002200);
   MMSS_MDP_VP_0_RGB_2_REG_BASE                           = (MMSS_MDSS_REG_BASE + 0x00002600);
   MMSS_MDP_VP_0_RGB_2_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00002600);
   MMSS_MDP_VP_0_RGB_3_REG_BASE                           = MMSS_MDP_VP_0_RGB_0_REG_BASE;
   MMSS_MDP_VP_0_RGB_3_SSPP_REG_BASE                      = MMSS_MDP_VP_0_RGB_0_SSPP_REG_BASE;
   MMSS_MDP_VP_0_DMA_0_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00002a00);
   MMSS_MDP_VP_0_DMA_1_SSPP_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00002e00);
   MMSS_MDP_VP_0_LAYER_0_REG_BASE                         = (MMSS_MDSS_REG_BASE + 0x00003200);
   MMSS_MDP_VP_0_LAYER_1_REG_BASE                         = (MMSS_MDSS_REG_BASE + 0x00003600);
   MMSS_MDP_VP_0_LAYER_2_REG_BASE                         = (MMSS_MDSS_REG_BASE + 0x00003a00);
   MMSS_MDP_VP_0_LAYER_3_REG_BASE                         = (MMSS_MDSS_REG_BASE + 0x00003e00);
   MMSS_MDP_VP_0_LAYER_4_REG_BASE                         = (MMSS_MDSS_REG_BASE + 0x00004200);
   MMSS_MDP_VP_0_LAYER_5_REG_BASE                         = MMSS_MDP_VP_0_LAYER_0_REG_BASE;
   MMSS_MDP_VP_0_DSPP_0_REG_BASE                          = (MMSS_MDSS_REG_BASE + 0x00004600);
   MMSS_MDP_VP_0_DSPP_1_REG_BASE                          = (MMSS_MDSS_REG_BASE + 0x00004a00);
   MMSS_MDP_VP_0_DSPP_2_REG_BASE                          = (MMSS_MDSS_REG_BASE + 0x00004e00);
   MMSS_MDP_VP_0_DSPP_3_REG_BASE                          = MMSS_MDP_VP_0_DSPP_0_REG_BASE;
   MMSS_MDP_WB_0_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00011100);
   MMSS_MDP_WB_1_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00013100);
   MMSS_MDP_WB_2_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00015100);
   MMSS_MDP_WB_3_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00017100);
   MMSS_MDP_WB_4_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00019100);
   MMSS_MDP_INTF_0_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x00021100);
   MMSS_MDP_INTF_1_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x00021300);
   MMSS_MDP_INTF_2_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x00021500);
   MMSS_MDP_INTF_3_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x00021700);
   MMSS_MDP_PP_0_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00021b00);
   MMSS_MDP_PP_1_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00021c00);
   MMSS_MDP_PP_2_REG_BASE                                 = (MMSS_MDSS_REG_BASE + 0x00021d00);
   MMSS_MDP_PP_3_REG_BASE                                 = MMSS_MDP_PP_0_REG_BASE;
   MMSS_EDP_REG_BASE                                      = (MMSS_MDSS_REG_BASE + 0x00023400);
   MMSS_EDP_PHY_REG_BASE                                  = (MMSS_MDSS_REG_BASE + 0x00023800);
   MMSS_EDP_PHY_PLL_REG_BASE                              = (MMSS_MDSS_REG_BASE + 0x00023a00);
   MMSS_VBIF_VBIF_MDP_REG_BASE                            = (MMSS_MDSS_REG_BASE + 0x00024000);
   MMSS_VBIF_VBIF_VBIF_APU0132_13_QR_REG_BASE             = (MMSS_MDSS_REG_BASE + 0x00025000);
   MMSS_VBIF_CB0_CB0_MM_MMU_CONTEXT_BANK_REG_BASE         = (MMSS_MDSS_REG_BASE + 0x00030000);
   MMSS_VBIF_CB1_CB1_MM_MMU_CONTEXT_BANK_REG_BASE         = (MMSS_MDSS_REG_BASE + 0x00031000);
   MMSS_VBIF_CB2_CB2_MM_MMU_CONTEXT_BANK_REG_BASE         = (MMSS_MDSS_REG_BASE + 0x00032000);
   MMSS_VBIF_MM_MMU_GLOBAL0_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00028000);
   MMSS_VBIF_MM_MMU_GLOBAL1_REG_BASE                      = (MMSS_MDSS_REG_BASE + 0x00029000);
   MMSS_VBIF_MM_MMU_IMPL_DEF_REG_BASE                     = (MMSS_MDSS_REG_BASE + 0x0002a000);
   MMSS_VBIF_MM_MMU_PERF_MON_REG_BASE                     = (MMSS_MDSS_REG_BASE + 0x0002b000);
   MMSS_VBIF_MM_MMU_SSD_REG_BASE                          = (MMSS_MDSS_REG_BASE + 0x0002c000);
   MMSS_MDP_AADC_0_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x00013100);
   MMSS_MDP_AADC_1_REG_BASE                               = (MMSS_MDSS_REG_BASE + 0x00013300);
   MMSS_MDP_ENCR_NS_0_REG_BASE                            = (MMSS_MDSS_REG_BASE + 0x00013000);
   MMSS_MDP_ENCR_S_0_REG_BASE                             = (MMSS_MDSS_REG_BASE + 0x00013010);

   HAL_MDP_SetHWBlockRegOffsets_1_0_0();

   if(NULL != psHwInfo)
   {
      psHwInfo->uNumOfRGBLayers              = HAL_MDP_1_0_0_NUM_OF_RGB_LAYERS;
      psHwInfo->uNumOfVIGLayers              = HAL_MDP_1_0_0_NUM_OF_VIG_LAYERS;
      psHwInfo->uNumOfDMALayers              = HAL_MDP_1_0_0_NUM_OF_DMA_LAYERS;
      psHwInfo->uNumOfCursorLayers           = HAL_MDP_1_0_0_NUM_OF_CURSOR_LAYERS;
      psHwInfo->uNumOfLayerMixers            = HAL_MDP_1_0_0_NUM_OF_LAYER_MIXERS;
      psHwInfo->uNumOfDSPPs                  = HAL_MDP_1_0_0_NUM_OF_DSPP;
      psHwInfo->uNumOfBltEngines             = HAL_MDP_1_0_0_NUM_OF_WRITEBACK_INTERFACES;
      psHwInfo->uNumOfControlPaths           = HAL_MDP_1_0_0_NUM_OF_CONTROL_PATHS;
      psHwInfo->uNumOfDataPaths              = HAL_MDP_1_0_0_NUM_OF_DATA_PATHS;
      psHwInfo->uNumOfLayerMixerBlendStages  = HAL_MDP_1_0_0_LAYER_MIXER_MAX_BLEND_STAGES;              // BLEND STAGES
      psHwInfo->uNumOfPhyInterfaces          = HAL_MDP_1_0_0_NUM_OF_PHYSICAL_INTERFACES;
      psHwInfo->uNumOfPingPongs              = HAL_MDP_1_0_0_NUM_OF_PINGPONGS;
      psHwInfo->uNumOfWatchDogs              = HAL_MDP_1_0_0_NUM_OF_WATCHDOGS;
      psHwInfo->uNumOfAxiPorts               = HAL_MDP_1_0_0_NUM_OF_AXI_PORTS;
   }

   if(NULL != psMdpHwInfo)
   {
      /** Color Space Conversion(CSC) - Limited REC601 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]       =  &gMDP_CSC_YuvToRgb_Limited_Rec601;
      /** Color Space Conversion(CSC) - Limited REC601 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]       =  &gMDP_CSC_RgbToYuv_Limited_Rec601;
      /** Color Space Conversion(CSC) - Full REC601 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]  =  &gMDP_CSC_YuvToRgb_Full_Rec601; 
      /** Color Space Conversion(CSC) - Full REC601 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]  =  &gMDP_CSC_RgbToYuv_Full_Rec601;
      /** Color Space Conversion(CSC) - Limited REC709 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]       =  &gMDP_CSC_YuvToRgb_Limited_Rec709;
      /** Color Space Conversion(CSC) - Limited REC709 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]       =  &gMDP_CSC_RgbToYuv_Limited_Rec709;
      /** Color Space Conversion(CSC) - Full REC709 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]  =  &gMDP_CSC_YuvToRgb_Full_Rec709;
      /** Color Space Conversion(CSC) - Full REC709 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]  =  &gMDP_CSC_RgbToYuv_Full_Rec709;    

      // QSEED
      psMdpHwInfo->uQSEEDNormFilterCoeffCnt                = HAL_MDP_1_0_0_QSEED_NORM_FILTER_COEFF_COUNT;
      psMdpHwInfo->uQSEEDSmoothFilterCoeffCnt              = HAL_MDP_1_0_0_QSEED_SMOOTH_FILTER_COEFF_COUNT;
      psMdpHwInfo->pQSEEDNormFilterCoeff                   = NULL;
      psMdpHwInfo->pQSEEDSmoothFilterCoeff                 = NULL;
      psMdpHwInfo->uQSEEDSupportedScaleFilters             = HAL_MDP_1_0_0_QSEED_FILTER_CAPS;

      //QoS
      psMdpHwInfo->uMaxMdpCoreClkFreq                      = HAL_MDP_1_0_0_MAX_MDPCORE_CLK_FREQ;
      psMdpHwInfo->uQosReqPriWatermarkRegsCount            = HAL_MDP_1_0_0_QOS_REQPRI_WATERMARK_REGS_COUNT;
      psMdpHwInfo->uQosRemapperRealTimeClients             = HAL_MDP_1_0_0_QOS_REMAPPER_REALTIME_CLIENTS;
      psMdpHwInfo->uQosRemapperNonRealTimeClients          = HAL_MDP_1_0_0_QOS_REMAPPER_NONREALTIME_CLIENTS;
      psMdpHwInfo->pQosWBPathsPriority                     = (uint32 *)gMDP_Qos_WB_Paths_Priority;

      //VBIF
      psMdpHwInfo->uVBIFMaxDdrRdBurstSize                  = HAL_MDP_1_0_0_VBIF_MAX_DDR_RD_BURST_SIZE;
      psMdpHwInfo->uVBIFMaxDdrWrBurstSize                  = HAL_MDP_1_0_0_VBIF_MAX_DDR_WR_BURST_SIZE;
      psMdpHwInfo->uVBIFRoundRobinQosArb                   = HAL_MDP_1_0_0_VBIF_ROUND_ROBIN_QOS_ARB;

      //DST PACK PATTERN
      psMdpHwInfo->pDstPackPatternInfo                     = (uint32 *)&guDstPackInfo_1_0_0;

      //Picture adjustment(HSIC) 
      psMdpHwInfo->sPictureAdjustInfo.iMinHue              = HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMaxHue              = HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMinUnchangeHue      = HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_UNCHANGE_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMaxUnchangeHue      = HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_UNCHANGE_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMinSaturation       = HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_SATURATION;
      psMdpHwInfo->sPictureAdjustInfo.iMaxSaturation       = HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_SATURATION;
      psMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation  = HAL_MDP_1_0_0_PICTURE_ADJUST_UNCHANGE_SATURATION;
      psMdpHwInfo->sPictureAdjustInfo.iMinIntensity        = HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_INTENSITY;
      psMdpHwInfo->sPictureAdjustInfo.iMaxIntensity        = HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_INTENSITY;
      psMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity   = HAL_MDP_1_0_0_PICTURE_ADJUST_UNCHANGE_INTENSITY;
      psMdpHwInfo->sPictureAdjustInfo.iMinContrast         = HAL_MDP_1_0_0_PICTURE_ADJUST_MIN_CONTRAST;
      psMdpHwInfo->sPictureAdjustInfo.iMaxContrast         = HAL_MDP_1_0_0_PICTURE_ADJUST_MAX_CONTRAST;
      psMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast    = HAL_MDP_1_0_0_PICTURE_ADJUST_UNCHANGE_CONTRAST;

      psMdpHwInfo->uHysterisisValue                        = HAL_MDP_1_0_0_HYSTERISIS_DEFAULT;

      // SMP client ID mapping
      psMdpHwInfo->pClientToHWClientMap                    = (uint32 *)&gMDP_SMPClientMap_1_0_0;

      // Histogram config function
      psMdpHwInfo->sIpFxnTbl.HistogramConfig               = HAL_MDP_DSPP_HistogramV1_Config;

      // Histogram lock function
      psMdpHwInfo->sIpFxnTbl.HistogramLock                 = NULL;

      // Initialize the module's status register details
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_TYPE_NONE]              = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_SOURCE_PIPE]            = (uint32 *)&gsSSPPModuleStatusRegInfo;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_LAYERMIXER]             = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_DESTINATION_PIPE]       = (uint32 *)&gsDSPPStatusRegInfo;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_INTERFACE]              = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_CONTROL_PATH]           = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_DATA_PATH]              = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_SMP]                    = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_TRAFFIC_SHAPER]         = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_VBIF]                   = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_PINGPONG]               = (uint32 *)&gsPingPongStatusRegInfo;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_PHYSICAL_INTERFACE]     = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_WRITEBACK_INTERFACE]    = (uint32 *)&gsWBStatusRegInfo;

      psMdpHwInfo->eCursorIntLayerType    = HAL_MDP_LAYER_TYPE_CURSOR;
   }
}


#ifdef __cplusplus
}
#endif
