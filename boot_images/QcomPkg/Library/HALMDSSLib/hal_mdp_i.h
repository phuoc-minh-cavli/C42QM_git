/*! \file */

/*
===========================================================================

FILE:         hal_mdp_i.h

DESCRIPTION:  Internal Header file for MDP HAL.  This file should not be
              Used outside of MDP HAL
  

===========================================================================
Copyright (c) 2012- 2018 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef _HAL_MDP_I_H
#define _HAL_MDP_I_H

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp.h"
#include "mdsshwio.h"
/* !! ALERT !!
 * DO NOT INCLUDE ANY HEADER FILES OTHER THAN hal_mdp.h and mdsshwio.h
 */

/* -----------------------------------------------------------------------
** Preprocessor Defintions and Constants
** ----------------------------------------------------------------------- */
#define HAL_MDP_ASSERT()
/* -----------------------------------------------------------------------
** Color Format Selection (MDP_DMA_P_CONFIG and MDP_PPP_OUT_FORMAT)
** ----------------------------------------------------------------------- */

/*
 * The following defines are used to configure the HW registers.
 *
 * The values might be vary for different MDP version.
 *
 *    @@ TODO @@: Required to update the defines based on MDP version i.e HAL_MDP_<major>_<minor>_<step>_XXX
 */ 
#define HAL_MDP_SRC_FORMAT_FRAME_FORMAT_NONE                0
#define HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR              0        /**< Buffer Layer Out - Linear      */
#define HAL_MDP_SRC_FORMAT_FRAME_FORMAT_TILE                1        /**< Buffer Layer Out - Tile        */
#define HAL_MDP_DST_FORMAT_FRAME_FORMAT_TILE                1        /**< Buffer Layer Out - Tile        */


#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_NONE               0
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB                0
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444             0
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1        1
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2        2
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1        3
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N1V2        4

#define HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED         0
#define HAL_MDP_SRC_FORMAT_FETCH_PLANES_PLANAR              1
#define HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR       2

#define HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_NONE             0
#define HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB              0
#define HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_MSB              1

#define HAL_MDP_SRC_FORMAT_UNPACK_NONE                      0
#define HAL_MDP_SRC_FORMAT_UNPACK_LOOSE                     0
#define HAL_MDP_SRC_FORMAT_UNPACK_TIGHT                     1

#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT_NONE      0
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT0          0
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT1          0
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2          1
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3          2
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4          3

#define HAL_MDP_SRC_FORMAT_SRC_NO_BPP                       0
#define HAL_MDP_SRC_FORMAT_SRC_ONE_BPP                      0
#define HAL_MDP_SRC_FORMAT_SRC_TWO_BPP                      1
#define HAL_MDP_SRC_FORMAT_SRC_THREE_BPP                    2
#define HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP                     3

#define HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT                0
#define HAL_MDP_SRC_FORMAT_ALPHA_PRESENT                    1

/**< MDP HW COLOR COMPONENT 3 (ALPHA) */
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0                0
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_1                0
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_4                1
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_6                2
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8                3

/**< MDP HW COLOR COMPONENT 2 (RED/Cr) */
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_0                  0
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_4                  0
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5                  1
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_6                  2
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8                  3

/**< MDP HW COLOR COMPONENT 1 (BLUE/Cb) */
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_0                 0
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_4                 0
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5                 1
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_6                 2
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8                 3

/**< MDP HW COLOR COMPONENT 0 (GREEN/Luma) */
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_0                0
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_4                0
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_5                1
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_6                2
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8                3

#define HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB8888                0
#define HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB1555                2
#define HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB4444                4

#define HAL_MDP_CURSOR_BLEND_CONSTANT                       0
#define HAL_MDP_CURSOR_BLEND_ALPHA                          2


/**< Scale filter Mode */
#define HAL_MDP_SCALAR_NEAREST_NEIGHBOR_FILTER              0
#define HAL_MDP_SCALAR_BILINEAR_FILTER                      1
#define HAL_MDP_SCALAR_PCMN_FILTER                          2
#define HAL_MDP_SCALAR_CAF_FILTER                           3

#define HAL_MDP_ROTATOR_BLOCK_SIZE_64x64_TILE               0
#define HAL_MDP_ROTATOR_BLOCK_SIZE_128x128_TILE             1

/**
 * MDP Features with table programmings
 */
/** MDP SMP Features */
#define     HAL_MDP_SMP_FEATURE_NONE              0x00000000

/** MDP SSPP or Layer Features */
#define     HAL_MDP_SSPP_FEATURE_NONE             0x00000000
#define     HAL_MDP_SSPP_FEATURE_QSEED2           HAL_MDSS_BIT(0)
#define     HAL_MDP_SSPP_FEATURE_CSC_1            HAL_MDSS_BIT(1)
#define     HAL_MDP_SSPP_FEATURE_SPA_CE           HAL_MDSS_BIT(2)
#define     HAL_MDP_SSPP_FEATURE_IGC_LUT          HAL_MDSS_BIT(3)
#define     HAL_MDP_SSPP_FEATURE_RGB_SCALE        HAL_MDSS_BIT(4)

/** MDP Layer Mixer Features */
#define     HAL_MDP_LM_FEATURE_NONE               0x00000000
#define     HAL_MDP_LM_GC_LUT                     HAL_MDSS_BIT(5)

/** MDP DSPP Features */
#define     HAL_MDP_DSPP_FEATURE_NONE             0x00000000
#define     HAL_MDP_DSPP_FEATURE_PA               HAL_MDSS_BIT(6)
#define     HAL_MDP_DSPP_FEATURE_IGC_LUT          HAL_MDSS_BIT(7)
#define     HAL_MDP_DSPP_FEATURE_GAMUT_MAPPING    HAL_MDSS_BIT(8)
#define     HAL_MDP_DSPP_FEATURE_PC_LUT           HAL_MDSS_BIT(9)
#define     HAL_MDP_DSPP_FEATURE_DITHRE           HAL_MDSS_BIT(10)

/** MDP DATA-PATH Features */
#define     HAL_MDP_DATAPATH_FEATURE_NONE         0x00000000
#define     HAL_MDP_DATAPATH_FEATURE_CSC          HAL_MDSS_BIT(0)

/** MDP 3D or DUAL PIPE muxer Features */
#define     HAL_MDP_MUXER_FEATURE_NONE            0x00000000

/** < Helper Macro Definitions */

#define HAL_MDP_INTR_STATUS_PING_PONG_RD_PTR_BMSK (HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_3_RD_PTR_BMSK    | \
                                                   HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_2_RD_PTR_BMSK    | \
                                                   HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_1_RD_PTR_BMSK    | \
                                                   HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_0_RD_PTR_BMSK    )

#define HAL_MDP_INTR_STATUS_PING_PONG_DONE_BMSK   (HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_3_DONE_BMSK      | \
                                                   HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_2_DONE_BMSK      | \
                                                   HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_1_DONE_BMSK      | \
                                                   HWIO_MMSS_MDP_INTR_STATUS_PING_PONG_0_DONE_BMSK      )

/**
 * MDP 1.0.0 Version
 * ===========================================================================================================
 */
#define  HAL_MDP_VIG_LAYER_DEFAULT_FEATURE_INIT         (HAL_MDP_SSPP_FEATURE_CSC_1)

#define  HAL_MDP_RGB_LAYER_DEFAULT_FEATURE_INIT         (HAL_MDP_SSPP_FEATURE_RGB_SCALE | \
                                                         HAL_MDP_SSPP_FEATURE_IGC_LUT )

#define  HAL_MDP_DMA_LAYER_DEFAULT_FEATURE_INIT         0x00000000

#define  HAL_MDP_LM_DEFAULT_FEATURE_INIT                HAL_MDP_LM_FEATURE_NONE

#define  HAL_MDP_DSPP_DEFAULT_FEATURE_INIT              (HAL_MDP_DSPP_FEATURE_NONE           |  \
                                                         HAL_MDP_DSPP_FEATURE_IGC_LUT        |  \
                                                         HAL_MDP_DSPP_FEATURE_DITHRE )

#define  HAL_MDP_MUXER_DEFAULT_FEATURE_INIT             HAL_MDP_MUXER_FEATURE_NONE

#define  HAL_MDP_DATAPATH_DEFAULT_FEATURE_INIT          (HAL_MDP_DATAPATH_FEATURE_CSC)

/** MDP Version Specific */

#define  HAL_MDP_PHASE_STEP_CONSTANT                           21

#define HAL_MDP_QOS_REMAPPER_INFO( ReqPriority0, ReqPriority1, ReqPriority2, ReqPriority3 ) \
                                                              (ReqPriority0 << HWIO_MMSS_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_0_SHFT | \
                                                               ReqPriority1 << HWIO_MMSS_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_1_SHFT | \
                                                               ReqPriority2 << HWIO_MMSS_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_2_SHFT | \
                                                               ReqPriority3 << HWIO_MMSS_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_3_SHFT )

#define HAL_MDP_MAX_RETRIES_FOR_CTRL_PATH_RESET_COMPLETE      32

/*=========================================================================================================== */

/*!
 * \enum HAL_MDP_PhysicalInterfaceId (Only for internal. Shoulb not be used in APIs)
 *
 * The \b HAL_MDP_PhysicalInterfaceId lists the physical interface connections from MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_PHYSICAL_INTERFACES provides the maximum physical interfaces present in the HW
 *
 */
typedef enum _HAL_MDP_PhyInterfaceId
{
   HAL_MDP_PHYSICAL_INTERFACE_ID_NONE        = 0x00000000,
   HAL_MDP_PHYSICAL_INTERFACE_0,             /**< INTF_0 */
   HAL_MDP_PHYSICAL_INTERFACE_1,             /**< INTF_1 */
   HAL_MDP_PHYSICAL_INTERFACE_2,             /**< INTF_2 */
   HAL_MDP_PHYSICAL_INTERFACE_3,             /**< INTF_3 */
   HAL_MDP_PHYSICAL_INTERFACE_4,             /**< INTF_4 */
   HAL_MDP_PHYSICAL_INTERFACE_5,             /**< INTF_5 */
   HAL_MDP_PHYSICAL_INTERFACE_MAX,
   HAL_MDP_PHYSICAL_INTERFACE_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_PhyInterfaceId;

/*!
 * \enum HAL_MDP_WritebackInterfaceId (Only for internal. Shoulb not be used in APIs)
 *
 * The \b HAL_MDP_WritebackInterfaceId lists the write back interface connections in MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_WRITEBACK_INTERFACES provides the maximum physical interfaces present in the HW
 *
 */
typedef enum _HAL_MDP_WritebackInterfaceId
{
   HAL_MDP_WRITEBACK_INTERFACE_ID_NONE        = 0x00000000,
   HAL_MDP_WRITEBACK_INTERFACE_0,             /**< WB0 */
   HAL_MDP_WRITEBACK_INTERFACE_1,             /**< WB1 */
   HAL_MDP_WRITEBACK_INTERFACE_2,             /**< WB2 */
   HAL_MDP_WRITEBACK_INTERFACE_3,             /**< WB3 */
   HAL_MDP_WRITEBACK_INTERFACE_4,             /**< WB4 */
   HAL_MDP_WRITEBACK_INTERFACE_5,             /**< WB5 */
   HAL_MDP_WRITEBACK_INTERFACE_MAX,
   HAL_MDP_WRITEBACK_INTERFACE_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_WritebackInterfaceId;


typedef struct _HAL_MDP_PictureAdjustInfo
{
   int32      iMinHue;
   int32      iMaxHue;
   int32      iMinUnchangeHue;
   int32      iMaxUnchangeHue;
   int32      iMinSaturation;
   int32      iMaxSaturation;
   int32      iUnchangeSaturation;
   int32      iMinIntensity;
   int32      iMaxIntensity;
   int32      iUnchangeIntensity;
   int32      iMinContrast;
   int32      iMaxContrast;
   int32      iUnchangeContrast;
}HAL_MDP_PictureAdjustInfo;

/*
 * This function table contains HAL functions to program different MDP IP core implementations based on MDP versions.
 * The function pointers should be set in hal_mdp_1_x_0.c for specific implementation for MDP 1.x.0.
 */
typedef struct 
{
  /** Configure DSPP histogram */
  void (*HistogramConfig) (uintPtr uRegisterOffset, HAL_MDP_HistogramConfigType *psHistConfig);

  /** Histogram buffer lock */
  void (*HistogramLock) (uintPtr uRegisterOffset, bool32 bLock);

  /** VBIF programming */
  void (*Vbif_HwDefaultConfig) (void);

} HAL_MDP_IpFxnTbl;


/* Color Space Conversion(CSC) encoding info */ 
typedef struct 
{
   uint32            *pCSC_Coeff_Matrix;
   uint16            *pCSC_PreClamp;
   uint16            *pCSC_PostClamp;
   uint16            *pCSC_PreBias;
   uint16            *pCSC_PostBias;
}HAL_MDP_CSCEncodingInfo; 

/**
 * HAL_MDP_ModuleStatusRegInfo
 *
 * MDP has a set of MDP_CLK_STATUSx registers to read the active/idle state of the MDP modules (SSPP, DSPP etc).
 * The following data-structure is used to prepare a mapping table information of MDP_CLK_STATUSx register and BitFieldMask for each MDP module
 *
 *    (a)   gsSSPPModuleStatusRegInfo        --> Mapping table for SSPPx (VIGx, RGBx, CURSORx and DMAx) modules
 *    (b)   gsDSPPStatusRegInfo              --> Mapping table for DSPPx modules
 *    (c)   gsPingPongStatusRegInfo          --> Mapping table for PING-PONGx modules
 *    (c)   gsWBStatusRegInfo                --> Mapping table for WBx modules
 */
typedef struct _HAL_MDP_ModuleStatusRegInfo
{
   uint32            uRegAddress;            /**< Register address offset from MDP_REG_BASE                       */
   uint32            uBitFieldMask;          /**< Bit field mask value to extract the module's status information */

} HAL_MDP_ModuleStatusRegInfo;

/**
 * HAL_MDP_Hw_Private_Info
 *    Holds the private data that is accessible to only HAL MDP driver.
 *
 */
typedef struct _HAL_MDP_Hw_Private_Info
{
   HAL_MDP_HwInfo    sHwInfo;

   /** QSEED */
   uint32             uQSEEDNormFilterCoeffCnt;        /**< QSEED Normal Filter Coefficient count */
   uint32             uQSEEDSmoothFilterCoeffCnt;      /**< QSEED Smooth Filter Coefficient count */
   uint32            *pQSEEDNormFilterCoeff;
   uint32            *pQSEEDSmoothFilterCoeff;
   uint32             uQSEEDSupportedScaleFilters;     /**< QSEED Supported scale filters    */

   /** Color Space Conversion(CSC) encoding */
   const HAL_MDP_CSCEncodingInfo    *pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_MAX];
   const HAL_MDP_CSCEncodingInfo    *pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_MAX];
      
   /** QoS */
   uint32             uMaxMdpCoreClkFreq;
   uint32             uQosReqPriWatermarkRegsCount;
   uint32             uQosRemapperRealTimeClients;
   uint32             uQosRemapperNonRealTimeClients;
   uint32            *pQosWBPathsPriority;
   
   /** VBIF */
   uint32             uVBIFMaxDdrRdBurstSize;
   uint32             uVBIFMaxDdrWrBurstSize;
   uint32             uVBIFRoundRobinQosArb;

   /** Write-Back Destination pack Format */
   uint32            *pDstPackPatternInfo;

   /** Clock Control */
   uint32             uHysterisisValue;
   

   /** Picture adjustment(HSIC) */
   HAL_MDP_PictureAdjustInfo   sPictureAdjustInfo;

   /* Mapping of SMP clients to HW id  */
   uint32                     *pClientToHWClientMap;

   /* AXI port assignement */
   uint32                     *pAxiPortAssignment;

   /* IP core specific functions based on MDP version */
   HAL_MDP_IpFxnTbl           sIpFxnTbl;

   /* Module's status (to detect busy or idle state of the module) register information */
   uint32                     *gpsModuleStatusRegInfo[HAL_MDP_MODULE_MAX];

   HAL_MDP_LayerType           eCursorIntLayerType;
   uint32                      uCursorBlendOrderIndex;

   /** Pixel Extension */
   bool32                      bUseSWPixExt;

} HAL_MDP_Hw_Private_Info;


typedef struct _HAL_MDP_ScalarInfo
{
   uint32            uSrcWidthInPixels;
   uint32            uSrcHeightInPixels;
   uint32            uDstWidthInPixels;
   uint32            uDstHeightInPixels;

   uint32            uPhaseStepX;             /**< Phase step X for Color Component 0,3 (Lume and Alpha) plane     */
   uint32            uPhaseStepY;             /**< Phase step Y for Color Component 0,3 (Lume and Alpha) plane     */
   int32             iInitPhaseX;
   int32             iInitPhaseY;
   uint32            eHorzFilterRegFieldVal;
   uint32            eVertFilterRegFieldVal;

} HAL_MDP_ScalarInfo;

typedef struct _HAL_MDP_DecimationInfo
{
   uint32            uHorzDecimationFactor;  /**< Factor to decimate in the Horizontal direction   */
   uint32            uVertDecimationFactor;  /**< Factor to decimate in the Vertical direction       */

} HAL_MDP_DecimationInfo;

/*------------------------------------------------------------------------
 * Global Data Definitions
 *-----------------------------------------------------------------------*/
extern uintPtr  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_MAX];                // Control path register offsets
extern uintPtr  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_MAX];                      // Data path register offsets
extern uintPtr  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];                   // Destination (DSPP) register offsets 
extern uintPtr  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];         // Physical interface register offsets
extern uintPtr  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_MAX];                  // Layer mixer register offsets
extern uintPtr  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_MAX];        // Layer mixer blending state register offsets
extern uintPtr  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_MAX];                       // PingPong register offsets
extern uintPtr  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX];                        // Source (SSPP) register offsets
extern uintPtr  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX];                       // Source (SSPP) Layers (Scalar) register offsets
extern uintPtr  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_MAX];         // Interface stream-based MISR configuration register offsets
extern uintPtr  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_MAX];                 // PingPong Buffer (PPB) register offsets
extern uintPtr  uMDPSSPPPixExtRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS]; // SSPP Pixel Ext Component Offset

extern HAL_MDP_Hw_Private_Info   gsMdpHwInfo;                              // No direct access to this global variable
extern HAL_MDP_Hw_Private_Info   *pgsMdpHwInfo;                            // HAL driver should use this to extract HAL_MDP_HwInfo
extern uint32                    guSrcFormatInfo[];
extern uint32                    guSrcUnpackInfo[];
extern uint32                    guSrcDecompressUnpackInfo[];
extern uint32                    guDstPackInfo[];
extern uint32                    guPanelFormatInfo[];


/* Limited REC601 YUV to RGB Conversion   */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Limited_Rec601;
/* Limited REC601 RGB to YUV Conversion   */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Limited_Rec601;
/* Full REC601 YUV to RGB Conversion      */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Full_Rec601;
/*  Full REC601 RGB to YUV Conversion     */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Full_Rec601;
/*  Limited REC709 YUV to RGB Conversion  */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Limited_Rec709;
/*  Limited REC709 RGB to YUV Conversion  */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Limited_Rec709;
/*  Full REC709 YUV to RGB Conversion     */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Full_Rec709;
/*  Full REC709 RGB to YUV Conversion     */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Full_Rec709;


extern uint32                    gMDP_Qos_WB_Paths_Priority[];

// Helper defines
#define HAL_MDP_GET_RGB_LAYER_COUNT()                       (pgsMdpHwInfo->sHwInfo.uNumOfRGBLayers)
#define HAL_MDP_GET_VIG_LAYER_COUNT()                       (pgsMdpHwInfo->sHwInfo.uNumOfVIGLayers)
#define HAL_MDP_GET_DMA_LAYER_COUNT()                       (pgsMdpHwInfo->sHwInfo.uNumOfDMALayers)
#define HAL_MDP_GET_CURSOR_LAYER_COUNT()                    (pgsMdpHwInfo->sHwInfo.uNumOfCursorLayers)
#define HAL_MDP_GET_LM_COUNT()                              (pgsMdpHwInfo->sHwInfo.uNumOfLayerMixers)
#define HAL_MDP_GET_DSPP_COUNT()                            (pgsMdpHwInfo->sHwInfo.uNumOfDSPPs)
#define HAL_MDP_GET_CONTROL_PATHS_COUNT()                   (pgsMdpHwInfo->sHwInfo.uNumOfControlPaths)
#define HAL_MDP_GET_DATA_PAHTS_COUNT()                      (pgsMdpHwInfo->sHwInfo.uNumOfDataPaths)
#define HAL_MDP_GET_PHYSICAL_INTERFACE_COUNT()              (pgsMdpHwInfo->sHwInfo.uNumOfPhyInterfaces)
#define HAL_MDP_GET_LM_BLEND_STAGES()                       (pgsMdpHwInfo->sHwInfo.uNumOfLayerMixerBlendStages)
#define HAL_MDP_GET_PINGPONG_COUNT()                        (pgsMdpHwInfo->sHwInfo.uNumOfPingPongs)
#define HAL_MDP_GET_WATCHDOG_COUNT()                        (pgsMdpHwInfo->sHwInfo.uNumOfWatchDogs)
#define HAL_MDP_GET_AXIPORT_COUNT()                         (pgsMdpHwInfo->sHwInfo.uNumOfAxiPorts)
#define HAL_MDP_GET_VBIFCLIENT_COUNT()                      (pgsMdpHwInfo->sHwInfo.uNumOfVbifClients)
/* -----------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_Init()
*/
/*!
* \brief
*     Initializes the MDP SSPP block
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_SSPP_Init(HAL_MDP_SourcePipeId    eSourcePipe, 
                       HAL_MDP_LayerType       eLayerType,
                       uint32                  uInitFlags,
                       uint32                  uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_Init()
*/
/*!
* \brief
*     Initializes the MDP DSPP block
*
* \param [in] eDestPipeId           - Destination Pipe ID
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSPP_Init(HAL_MDP_DestinationPipeId    eDestPipeId,
                                     uint32                       uInitFlags,
                                     uint32                       uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Init()
*/
/*!
* \brief
*     Initializes the layer mixer module
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] uInitFlags            - Internal blocks to be initialized
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_LayerMixer_Init(HAL_MDP_LayerMixerId    eLayerMixerId, 
                                           uint32                  uInitFlags,
                                           uint32                  uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Init()
*/
/*!
* \brief
*     Initializes the Data Path block
*
* \param [in] eDataPathId           - Data Path ID
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DataPath_Init(HAL_MDP_DataPathId      eDataPathId,
                           uint32                  uInitFlags,
                           uint32                  uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_GetSourceFormatInfo()
*/
/*!
* \brief
*     Returns the source format register programming information
*
* \param [in] psSurface           - Surface Information
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_MDP_GetSourceFormatInfo(HAL_MDP_SurfaceAttrType   *psSurface);

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
                                    HAL_MDP_Hw_Private_Info    *psMdpHwInfo);								

/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadHardwareInfo_1_16_0()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDP Version(515)
*
* \param [out]  psHwInfo               - Hardware information
* \param [out]  psMdpHwInfo            - MDP hardware information 
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_ReadHardwareInfo_1_16_0(HAL_MDP_HwInfo             *psHwInfo,
                                     HAL_MDP_Hw_Private_Info    *psMdpHwInfo);


/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramV1_Config()
*/
/*!
* \brief
*     Set up configurations for DSPP Histogram(V1) generation
*
* \param [in]   uRegisterOffset   - Register offset
* \param [in]   psHistConfig      - Histogram Configuration information
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_HistogramV1_Config(uintPtr                        uRegisterOffset,
                                     HAL_MDP_HistogramConfigType   *psHistConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramV2_Config()
*/
/*!
* \brief
*     Set up configurations for DSPP Histogram(V2) generation
*
* \param [in]   uRegisterOffset   - Register offset
* \param [in]   psHistConfig      - Histogram Configuration information
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_HistogramV2_Config(uintPtr                        uRegisterOffset,
                                     HAL_MDP_HistogramConfigType   *psHistConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramV2_Lock()
*/
/*!
* \brief
*     Lock histogram table for SW to read
*
* \param [in]   uintPtr    - Register offset
* \param [out]  bLock      - lock/unlock hist table
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_HistogramV2_Lock(uintPtr  uRegisterOffset,
                                   bool32   bLock);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_DSPP_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of DSPP register block from first DSPP block in HW
*
* \param [in]   eDestPipe     - Destination pipe id
*
* \retval uintPtr
*
****************************************************************************/
uintPtr HAL_MDP_Get_DSPP_BaseOffset(HAL_MDP_DestinationPipeId  eDestPipe);


#endif // _HAL_MDP_I_H
