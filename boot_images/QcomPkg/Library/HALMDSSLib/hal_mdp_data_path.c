/*! \file */

/*
===========================================================================

FILE:          hal_mdp_data_path.h

DESCRIPTION: Connects the blocks of the MDP (Source, Mixer and Display Interface) 

===========================================================================
  Copyright (c) 2012-2014, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_DataPath_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the data path
*
* \param [in] eDataPathId         - DATA PATH ID (0, 1, 2, 3, and 4)
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_DataPath_BaseOffset(HAL_MDP_DataPathId  eDataPathId)
{
   uintPtr   uRegOffset  = 0x0;

   if((HAL_MDP_DATA_PATH_NONE         == eDataPathId) ||
      (HAL_MDP_GET_DATA_PAHTS_COUNT() <  (uint32)eDataPathId) )
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPDataPathRegBaseOffset[eDataPathId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Rotator_Config()
*/
/*!
* \brief
*     Set up configurations for a particular data path
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psRotateConfig   - Rotator configuration information
* \param [in]   uFlags           - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_Rotator_Config(HAL_MDP_DataPathId         eDataPathId,
                                            HAL_MDP_RotateConfigType   *psRotateConfig,
                                            uint32                     uFlags)
{
   uintPtr              uRegOffset  = 0x0;
   uint32               uRegValue   = 0X00000000;

   uRegOffset = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

   uRegValue = in_dword(HWIO_MMSS_MDP_WB_0_DST_OP_MODE_ADDR + uRegOffset);

   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, ROT_EN, psRotateConfig->bRotateEnable);
   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, ROT_MODE, psRotateConfig->eRotateType);
   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, BLOCK_SIZE, HAL_MDP_ROTATOR_BLOCK_SIZE_128x128_TILE);   // Always force to 128x128 

/* TODO: Vipertooth : Recheck this part

   if(HAL_MDP_COMPRESSION_MODE_LOSSLESS == psRotateConfig->eCompressionMode)
   {
      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, BWC_ENC_EN, 1);
      // always use lossless quality since this is the only one supported
      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, BWC_ENC_QP, 0);
   }
   else 
   {
      // reset to 0
      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, BWC_ENC_EN, 0);
      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_OP_MODE, BWC_ENC_QP, 0);
   }
 */
   out_dword(HWIO_MMSS_MDP_WB_0_DST_OP_MODE_ADDR + uRegOffset, uRegValue);
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_ColorEncodeConfig()
*/
/*!
* \brief
*     Set up configurations for a particular data path
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psColorEncInfo   - Color encoding configuration information
* \param [in]   uFlags           - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_ColorEncodeConfig(HAL_MDP_DataPathId                  eDataPathId,
                                               HAL_MDP_ColorEncodeConfigType      *psColorEncInfo,
                                               uint32                              uFlags)
{
  uintPtr                 uRegOffset        = 0x0;
  uint32                 *pCSCMatrixCoeff   = NULL;
  uint16                 *pCSCPreClamp      = NULL;
  uint16                 *pCSCPostClamp     = NULL;
  uint16                 *pCSCPreBias       = NULL;
  uint16                 *pCSCPostBias      = NULL;
  HAL_MDSS_ErrorType      eStatus           = HAL_MDSS_STATUS_SUCCESS;

  switch(psColorEncInfo->eColorEncodeType)
  {
    case HAL_MDP_COLOR_ENCODING_DEFAULT:
    case HAL_MDP_COLOR_ENCODING_REC601:            /** ITU-R BT.601 (CCIR601)  */
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PostBias;
      break;

    case HAL_MDP_COLOR_ENCODING_REC709:            /** ITU-R BT.709 (CCIR709)  */
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PostBias;
      break;

    case HAL_MDP_COLOR_ENCODING_FULL_REC601:      /**< ITU.BT-601 SDTV         */   
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PostBias; 
      break;

    case HAL_MDP_COLOR_ENCODING_FULL_REC709:      /**< ITU.BT-709 HDTV         */ 
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PostBias; 
      break; 

    case HAL_MDP_COLOR_ENCODING_sRGB:              /** Standard RGB */     
    case HAL_MDP_COLOR_ENCODING_MAX:
    case HAL_MDP_COLOR_ENCODING_FORCE_32BIT:
    default:
      eStatus     = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
   }
   
   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
     if ((NULL != pCSCMatrixCoeff) && (NULL != pCSCPreClamp) &&
         (NULL != pCSCPostClamp)   && (NULL != pCSCPreBias ) && 
         (NULL != pCSCPostBias))
     {
       uRegOffset = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);
      
       // Matrix Coefficients
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_MATRIX_COEFF_0_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000000));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_MATRIX_COEFF_1_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000001));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_MATRIX_COEFF_2_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000002));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_MATRIX_COEFF_3_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000003));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_MATRIX_COEFF_4_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000004));

       // Preclamp
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP0_PRECLAMP_ADDR + uRegOffset, *(pCSCPreClamp + 0x00000000));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP1_PRECLAMP_ADDR + uRegOffset, *(pCSCPreClamp + 0x00000001));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP2_PRECLAMP_ADDR + uRegOffset, *(pCSCPreClamp + 0x00000002));

       // Postclamp
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP0_POSTCLAMP_ADDR + uRegOffset, *(pCSCPostClamp + 0x00000000));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP1_POSTCLAMP_ADDR + uRegOffset, *(pCSCPostClamp + 0x00000001));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP2_POSTCLAMP_ADDR + uRegOffset, *(pCSCPostClamp + 0x00000002));

       // Prebias
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP0_PREBIAS_ADDR + uRegOffset, *(pCSCPreBias + 0x00000000));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP1_PREBIAS_ADDR + uRegOffset, *(pCSCPreBias + 0x00000001));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP2_PREBIAS_ADDR + uRegOffset, *(pCSCPreBias + 0x00000002));

       // Postbias
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP0_POSTBIAS_ADDR + uRegOffset, *(pCSCPostBias + 0x00000000));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP1_POSTBIAS_ADDR + uRegOffset, *(pCSCPostBias + 0x00000001));
       out_dword(HWIO_MMSS_MDP_WB_0_CSC_COMP2_POSTBIAS_ADDR + uRegOffset, *(pCSCPostBias + 0x00000002));
    }
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_GpuMemoryBankBitConfig()
*/
/*!
* \brief
*     Gpu memory bank bit configuration
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psGpuMemoryBankConfig     - Gpu highest memory bank bit config
* \param [in]   uFlags                    - Reserved.
*
* \retval NONE
*
****************************************************************************/

static void HAL_MDP_DataPath_GpuMemoryBankBitConfig(HAL_MDP_DataPathId              eDataPathId,
                                                    HAL_MDP_GpuMemoryBankConfigType *psGpuMemoryBankConfig,
                                                    uint32                          uFlags)
{
  uintPtr  uRegOffset               = 0x00000000;
  uint32   uDestAddrFetchConfig     = 0;
  uint32   uGpuHighestMemoryBankBit = 0;

  // Extract the base address to program the HW registers
  uRegOffset   = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

  // ADDR FETCH Configuration
  uDestAddrFetchConfig     = in_dword(HWIO_MMSS_MDP_WB_0_DST_WRITE_CONFIG_ADDR + uRegOffset);
  uGpuHighestMemoryBankBit = HWIO_GETVAL(MDP_WB_0_DST_WRITE_CONFIG, uDestAddrFetchConfig, HIGHEST_BANK_BIT);
  if(uGpuHighestMemoryBankBit != psGpuMemoryBankConfig->uGpuHighestMemoryBankBit)
  {
    uDestAddrFetchConfig  = HWIO_OUT_FLD(uDestAddrFetchConfig,
                                         MDP_WB_0_DST_WRITE_CONFIG,
                                         HIGHEST_BANK_BIT,
                                         psGpuMemoryBankConfig->uGpuHighestMemoryBankBit);
  }
  
  out_dword(HWIO_MMSS_MDP_WB_0_DST_WRITE_CONFIG_ADDR + uRegOffset, uDestAddrFetchConfig); // Src Fetch Configuration
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_SurfaceAttrib_Config()
*/
/*!
* \brief
*     Set up surface attribute configurations for the given data path
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   eDataPathMode    - Data path mode. Block mode or line mode
* \param [in]   psSurfaceConfig  - Surface Configuration information
* \param [in]   uFlags           - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_SurfaceAttrib_Config(HAL_MDP_DataPathId        eDataPathId,
                                                  HAL_MDP_DataPath_ModeType eDataPathMode,
                                                  HAL_MDP_SurfaceAttrType   *psSurfaceConfig,
                                                  uint32                    uFlags)
{
   uintPtr           uRegOffset              = 0;              // Register offset
   uint32            uPlane0Address          = 0;              // Plane 0 address (Color#0 - G/Luma)
   uint32            uPlane1Address          = 0;              // Plane 1 address (Color#1 - R/Cr)
   uint32            uPlane2Address          = 0;              // Plane 2 address (Color#2 - B/Cb)
   uint32            uPlane3Address          = 0;              // Plane 3 address (Color#3 - Alpha)
   uint32            uPlane01StrideInBytes   = 0;              // Plane 0 and 1 stride
   uint32            uPlane23StrideInBytes   = 0;              // Plane 2 and 3 stride
   uint32            uDstFormatInfo          = 0;              // Destination format information (#of planes, #colors, #bits/each color)
   uint32            uDstPackInfo            = 0;              // Packing information of the destination buffer 
   uint32            uDstFormatFields        = 0;              // Cache of the current dest format fields that should not be overwritten
   uint32            uDstOpMode              = 0;              // OP MODE register
   uint32            uDitherDepth            = 0;
   uint32            uTempRegInfo            = 0;

   HAL_MDSS_ErrorType   eStatus              = HAL_MDSS_STATUS_SUCCESS;

   // Extract the base address to program the HW registers
   uRegOffset   = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

   uDstOpMode = in_dword(HWIO_MMSS_MDP_WB_0_DST_OP_MODE_ADDR + uRegOffset);

   // Keep the DS_ALPHA_X & DST_DITHER_EN configuration from being overwritten
   uDstFormatFields = in_dword(HWIO_MMSS_MDP_WB_0_DST_FORMAT_ADDR + uRegOffset) & 
                      (HWIO_OUT_FLD(0, MDP_WB_0_DST_FORMAT, DST_ALPHA_X, 1) |
                       HWIO_OUT_FLD(0, MDP_WB_0_DST_FORMAT, DST_DITHER_EN, 1));

   if(0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurfaceConfig->uFlags))
   {
      //BWC compressed surface always has 2 planes, compressed data plane and metadata plane, metadata plane keeps the size of 
      //compressed data every 4 lines. metadata plane stride is always 2.
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sCompressedPlane.sCompressedDataDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurfaceConfig->uPlanes.sCompressedPlane.sMetadataDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sCompressedPlane.uCompressedDataStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST1_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sCompressedPlane.uMetadataStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_INTERLEAVED_PIXEL_FORMAT(psSurfaceConfig->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sRgb.sDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sRgb.uStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurfaceConfig->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.uLumaStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST1_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.uChromaStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(psSurfaceConfig->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurfaceConfig->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.uLowPart;
      uPlane2Address          = psSurfaceConfig->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPlanar.uLumaStrideInBytes);

      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE0,
                                             DST1_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPlanar.uChromaBlueStrideInBytes);

      uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                             MDP_WB_0_DST_YSTRIDE1,
                                             DST2_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPlanar.uChromaRedStrideInBytes);
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      // DSPP OP MODE
      if((HAL_MDP_DATA_PATH_LINE_MODE == eDataPathMode)||(HAL_MDP_DATA_PATH_LINEAR_MODE == eDataPathMode))
      {
         // Enable CSC only if destination format is destination format is YUV mode
         if(TRUE == HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(psSurfaceConfig->ePixelFormat))
         {
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CSC_EN, TRUE);
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CSC_SRC_DATA_FORMAT, 0);         // RGB888
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CSC_DST_DATA_FORMAT, 1);         // YUV
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CHROMA_DWN_SAMPLE_EN, TRUE);     // Chroma Down Sample
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CHROMA_DWN_SAMPLE_FORMAT, 1);    // YUV420
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CSC_EN, TRUE);
         }
         else
         {
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CSC_EN, FALSE);
            uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CHROMA_DWN_SAMPLE_EN, FALSE);    // Chroma Down Sample

            // Dither is only for RGB destination with lower color depth than RGB888
            uTempRegInfo = HAL_MDP_GetSourceFormatInfo(psSurfaceConfig);

            // Dither Depth configuration is required only for RGB565 format
            uDitherDepth = HWIO_OUT_FLD(uDitherDepth,
                                          MDP_WB_0_DST_DITHER_BITDEPTH,
                                          C0_BITS,                                    // GREEN
                                          HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uTempRegInfo, SRCC0));

            uDitherDepth = HWIO_OUT_FLD(uDitherDepth,
                                          MDP_WB_0_DST_DITHER_BITDEPTH,
                                          C1_BITS,                                    // BLUE
                                          HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uTempRegInfo, SRCC1));

            uDitherDepth = HWIO_OUT_FLD(uDitherDepth,
                                          MDP_WB_0_DST_DITHER_BITDEPTH,
                                          C2_BITS,                                    // RED
                                          HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uTempRegInfo, SRCC2));
         }
      }
      else if(HAL_MDP_DATA_PATH_BLOCK_MODE == eDataPathMode)
      {
         // NO support for CSC and downsampling in block mode
         uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CSC_EN, FALSE);                   // CSC
         uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_0_DST_OP_MODE, CHROMA_DWN_SAMPLE_EN, FALSE);     // Chroma Down Sample
      }
      else // Linear mode
      {
         /// how to handle this scenario ?
         // In this case, the dither is in DSPP block. 
      }

      // DSPP_SRC_FORMAT
      uDstFormatInfo    = HAL_MDP_GetSourceFormatInfo(psSurfaceConfig);
      uDstFormatInfo   |= uDstFormatFields; // Keep dithering & const alpha settings

      // Enable macro tile format 
      if(psSurfaceConfig->uFlags & HAL_MDP_FLAGS_SURFACE_ATTR_MACRO_TILE)
      {
        uDstFormatInfo |= (HAL_MDP_DST_FORMAT_FRAME_FORMAT_TILE << HWIO_MMSS_MDP_WB_0_DST_FORMAT_FRAME_FORMAT_SHFT);
      }

      // Pack pattern 
      uDstPackInfo      = pgsMdpHwInfo->pDstPackPatternInfo[psSurfaceConfig->ePixelFormat];

      out_dword(HWIO_MMSS_MDP_WB_0_DST0_ADDR_ADDR + uRegOffset, uPlane0Address);           // Plane-0 address
      out_dword(HWIO_MMSS_MDP_WB_0_DST1_ADDR_ADDR + uRegOffset, uPlane1Address);           // Plane-1 address
      out_dword(HWIO_MMSS_MDP_WB_0_DST2_ADDR_ADDR + uRegOffset, uPlane2Address);           // Plane-2 address
      out_dword(HWIO_MMSS_MDP_WB_0_DST3_ADDR_ADDR + uRegOffset, uPlane3Address);           // Plane-3 address
      out_dword(HWIO_MMSS_MDP_WB_0_DST_YSTRIDE0_ADDR + uRegOffset, uPlane01StrideInBytes); // Plane-0 and 1 stride
      out_dword(HWIO_MMSS_MDP_WB_0_DST_YSTRIDE1_ADDR + uRegOffset, uPlane23StrideInBytes); // Plane-2 and 3 stride
      out_dword(HWIO_MMSS_MDP_WB_0_DST_FORMAT_ADDR + uRegOffset, uDstFormatInfo);          // Dst format information
      out_dword(HWIO_MMSS_MDP_WB_0_DST_PACK_PATTERN_ADDR + uRegOffset, uDstPackInfo);      // Dst unpack information
      out_dword(HWIO_MMSS_MDP_WB_0_DST_OP_MODE_ADDR + uRegOffset, uDstOpMode);             // Op-mode

      if(HAL_MDP_DATA_PATH_LINEAR_MODE != eDataPathMode)
      {
         out_dword(HWIO_MMSS_MDP_WB_0_DST_DITHER_BITDEPTH_ADDR + uRegOffset, uDitherDepth);   // Dither bit-depth
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_ConstAlpha_Config()
*/
/*!
* \brief
*     Set up configurations for constant alpha values in data path
*
* \param [in]   eDataPathId         - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psConstAlphaConfig  - Constant Alpha configuration
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_ConstAlpha_Config(HAL_MDP_DataPathId             eDataPathId,
                                               HAL_MDP_ConstAlphaConfigType   *psConstAlphaConfig,
                                               uint32                         uFlags)
{
   uintPtr   uRegOffset      = 0;              // Register offset
   uint32    uDstFormatInfo;

   // Extract the base address to program the HW registers
   uRegOffset     = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

   uDstFormatInfo = in_dword(HWIO_MMSS_MDP_WB_0_DST_FORMAT_ADDR + uRegOffset);          // Dst format information

   if (psConstAlphaConfig->bConstAlpha)
   {
       uDstFormatInfo = HWIO_OUT_FLD(uDstFormatInfo, MDP_WB_0_DST_FORMAT, DST_ALPHA_X, 1);

       out_dword(HWIO_MMSS_MDP_WB_0_DST_ALPHA_X_VALUE_ADDR + uRegOffset, psConstAlphaConfig->uConstAlphaValue);
   }
   else
   {
       // Disable const alpha
       uDstFormatInfo = HWIO_OUT_FLD(uDstFormatInfo, MDP_WB_0_DST_FORMAT, DST_ALPHA_X, 0);
   }

   out_dword(HWIO_MMSS_MDP_WB_0_DST_FORMAT_ADDR + uRegOffset, uDstFormatInfo);          // Dst format information
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_OutRect_Config()
*/
/*!
* \brief
*     Configures output rectangle for given data path
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psRectConfig     - Output rectangle information
* \param [in]   uFlags           - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_OutRect_Config(HAL_MDP_DataPathId     eDataPathId,
                                            HAL_MDP_RectType      *psRectConfig,
                                            uint32                 uFlags)
{
   uintPtr        uRegOffset         = 0;              // Register offset
   uint32         uRegValue          = 0;

   // Extract the base address to program the HW registers
   uRegOffset   = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_WB_0_OUT_SIZE, DST_H, psRectConfig->uHeightInPixels);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_WB_0_OUT_SIZE, DST_W, psRectConfig->uWidthInPixels);

   out_dword(HWIO_MMSS_MDP_WB_0_OUT_SIZE_ADDR + uRegOffset, uRegValue);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Surface_Config()
*/
/*!
* \brief
*     Set up surface and output rectangle configurations for given data path
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   eDataPathMode    - Data path mode. Block mode or line mode.
* \param [in]   psSurfaceConfig  - Surface and Rectangle Configuration information
* \param [in]   uFlags           - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_Surface_Config(HAL_MDP_DataPathId                  eDataPathId,
                                            HAL_MDP_DataPath_ModeType           eDataPathMode,
                                            HAL_MDP_DataPath_SurfaceConfigType  *psSurfaceConfig,
                                            uint32                              uFlags)
{
   if(NULL != psSurfaceConfig->psOutSurfaceConfig)
   {
      HAL_MDP_DataPath_SurfaceAttrib_Config(eDataPathId, eDataPathMode, psSurfaceConfig->psOutSurfaceConfig, 0x00);
   }

   if(NULL != psSurfaceConfig->psOutputRectConfig)
   {
      HAL_MDP_DataPath_OutRect_Config(eDataPathId, psSurfaceConfig->psOutputRectConfig, 0x00);
   }

   if(NULL != psSurfaceConfig->psConstAlphaConfig)
   {
      HAL_MDP_DataPath_ConstAlpha_Config(eDataPathId, psSurfaceConfig->psConstAlphaConfig, 0x00);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Setup()
*/
/*!
* \brief
*     Configures the data path
*
*
* \param [in]   eDataPathId         - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psDataPathConfig    - Data path configuration data
* \param [in]   uFlags              - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DataPath_Setup(HAL_MDP_DataPathId          eDataPathId,
                                          HAL_MDP_DataPathConfigType  *psDataPathConfig,
                                          uint32                       uFlags)
{
   HAL_MDSS_ErrorType            eStatus     = HAL_MDSS_STATUS_SUCCESS;

   if(NULL != psDataPathConfig->psRotatorConfig)
   {
      HAL_MDP_DataPath_Rotator_Config(eDataPathId, psDataPathConfig->psRotatorConfig, 0x00);
   }

   if(NULL != psDataPathConfig->pSurfaceConfig)
   {
      HAL_MDP_DataPath_Surface_Config(eDataPathId, psDataPathConfig->eDataPathMode, psDataPathConfig->pSurfaceConfig, 0x00);
   }

   /* CSC configuration */
   if(NULL != psDataPathConfig->psColorEncInfo)
   {
      HAL_MDP_DataPath_ColorEncodeConfig(eDataPathId, psDataPathConfig->psColorEncInfo, 0x00);
   }

   if(NULL != psDataPathConfig->psGpuMemoryBankConfig)
   {
      HAL_MDP_DataPath_GpuMemoryBankBitConfig(eDataPathId, psDataPathConfig->psGpuMemoryBankConfig, 0x00);
   }

   return eStatus;
}

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
                           uint32                  uFlags)
{
   if(0x00 != (HAL_MDP_DATAPATH_FEATURE_CSC & uInitFlags))
   {
      HAL_MDP_ColorEncodeConfigType      sColorEncInfo; 
      
      /* Default to limited REC601 - RGB to YUV color space conversion */
      sColorEncInfo.eColorEncodeType   =  HAL_MDP_COLOR_ENCODING_REC601;

      /* Configure the hardware with the default color encoding */
      HAL_MDP_DataPath_ColorEncodeConfig(eDataPathId, &sColorEncInfo, 0x00);
   }
}


#ifdef __cplusplus
}
#endif
