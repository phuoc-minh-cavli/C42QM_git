/*
===========================================================================

FILE:         hal_mdp_source.h

===========================================================================
  Copyright (c) 2010-2013, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */
#include "hal_mdp_i.h"
#include "hal_mdp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
 * Static Function Declarations and Defintions
 * ----------------------------------------------------------------------- */
#define  HAL_MDP_SOURCE_FLAGS_EXTRACT_ONLY_PIXEL_FORMAT              0x00000001
/**The HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG is used to indicate the Pre-condition call for 'HAL_MDP_SSPP_SurfaceConfig'*/
#define  HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG                     0x00000002


// Get the pixel source format - Mask solid fill, VC1 reduce and rotate90 
#define  HAL_MDP_GET_SOURCE_PIXEL_FORMAT_MASK         (~((uint32)((HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SOLID_FILL_BMSK)            | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_VC1_REDUCE_BMSK)            | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ROT90_BMSK))))

// Clear the pixel source format - Mask all the fields except solid fill and VC1 reduce 
#define  HAL_MDP_CLEAR_SOURCE_PIXEL_FORMAT_MASK       (~((uint32)((HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_BMSK)          | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_EXTEND_PIXEL_FORMAT_BMSK)   | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_CHROMA_SAMP_BMSK)       | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FETCH_PLANES_BMSK)          | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_ALIGN_BMSK)          | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_TIGHT_BMSK)          | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_COUNT_BMSK)          | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ROT90_BMSK)                 | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_BPP_BMSK)               | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC3_EN_BMSK)              | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC3_BMSK)                 | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC2_BMSK)                 | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC1_BMSK)                 | \
                                                                  (HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC0_BMSK))))

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_SSPP_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of SSPP register block from first layer's SSPP address
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA or Cursor)
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_SSPP_BaseOffset(HAL_MDP_SourcePipeId  eSourcePipe,
                                          HAL_MDP_LayerType     eLayerType)
{
   uintPtr     uRegOffset        = 0;
   uint32      uTotalSourcePipes = 0;
   uint32      uSourcePipeBaseId = 0;

   if((HAL_MDP_SOURCE_PIPE_NONE  == eSourcePipe) ||
      (HAL_MDP_SOURCE_PIPE_MAX   <= eSourcePipe) ||
      (HAL_MDP_LAYER_TYPE_NONE   == eLayerType)  ||
      (HAL_MDP_LAYER_TYPE_MAX    <= eLayerType))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      if(HAL_MDP_LAYER_TYPE_RGB == eLayerType)
      {
         uTotalSourcePipes = HAL_MDP_GET_RGB_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_RGB_BASE;
      }
      else if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)
      {
         uTotalSourcePipes = HAL_MDP_GET_VIG_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_VIG_BASE;
      }
      else if(HAL_MDP_LAYER_TYPE_DMA == eLayerType)
      {
         uTotalSourcePipes = HAL_MDP_GET_DMA_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_DMA_BASE;
      }
      else if((HAL_MDP_LAYER_TYPE_CURSOR == eLayerType) || (HAL_MDP_LAYER_TYPE_SSPP_CURSOR == eLayerType))
      {
         uTotalSourcePipes = HAL_MDP_GET_CURSOR_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_CURSOR_BASE;
      }

      if((uSourcePipeBaseId + uTotalSourcePipes) <= (uint32) eSourcePipe)
      {
         HAL_MDP_ASSERT();
      }
      else
      {
         uRegOffset = uMDPSSPPRegBaseOffset[eSourcePipe];
      }
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_Layer_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of SSPP register block from first layer's SSPP address
*
* \param [in]   eSourcePipe         - Source pipe id
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_Layer_BaseOffset(HAL_MDP_SourcePipeId  eSourcePipe)
{
   uintPtr      uRegOffset        = 0;

   uRegOffset = uMDPLayerRegBaseOffset[eSourcePipe];

   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GetCropRectConfig()
*/
/*!
* \brief
*     Returns the cropping rectangle information for the given piple
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [out]  psRectInfo          - Rectangle configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GetCropRectConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                           HAL_MDP_LayerType            eLayerType,
                                           HAL_MDP_CropRectangleType   *psRectInfo,
                                           uint32                       uFlags)
{
   uintPtr  uRegisterOffset   = 0;
   uint32   uSrcRectXYPos     = 0;
   uint32   uSrcRectSize      = 0;
   uint32   uDstRectXYPos     = 0;
   uint32   uDstRectSize      = 0;

   // Extract the SSPP base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   if(HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)  // Only for RGB/VIG/DMA layers
   {
      // Layer SSPP configuration (Common configuration for RGB, VIG, DMA layers)
      uSrcRectXYPos = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_XY_ADDR + uRegisterOffset);      // Source ROI (x,y)
      uSrcRectSize  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_SIZE_ADDR + uRegisterOffset);    // Source ROI (w,h)
      uDstRectXYPos = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_OUT_XY_ADDR + uRegisterOffset);      // Destination ROI (x,y)
      uDstRectSize  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_ADDR + uRegisterOffset);    // Destination ROI (w,h)

      // SRC ROI (x,y)
      psRectInfo->sSrcRectConfig.uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_XY, uSrcRectXYPos, SRC_X);
      psRectInfo->sSrcRectConfig.uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_XY, uSrcRectXYPos, SRC_Y);

      // SRC ROI (w, h)
      psRectInfo->sSrcRectConfig.uWidthInPixels    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_SIZE, uSrcRectSize, ROI_W);
      psRectInfo->sSrcRectConfig.uHeightInPixels   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_SIZE, uSrcRectSize, ROI_H);

      // DST ROI (x,y)
      psRectInfo->sDstRectConfig.uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY, uDstRectXYPos, DST_X);
      psRectInfo->sDstRectConfig.uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY, uDstRectXYPos, DST_Y);

      // DST ROI (w, h)
      psRectInfo->sDstRectConfig.uWidthInPixels    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE, uDstRectSize, DST_W);
      psRectInfo->sDstRectConfig.uHeightInPixels   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE, uDstRectSize, DST_H);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SurfaceInfo()
*/
/*!
* \brief
*     HSYNC Flip mode configuration
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [out]  psSurfaceInfo       - Surface information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_SurfaceInfo(HAL_MDP_SourcePipeId           eSourcePipe,
                                     HAL_MDP_LayerType              eLayerType,
                                     HAL_MDP_SurfaceAttrType        *psSurfaceInfo,
                                     HAL_MDP_CropRectangleType      *psRectInfo,
                                     uint32                         uFlags)
{
   uintPtr  uRegisterOffset          = 0;
   uint32   uSrcFormatRegValue       = 0;
   uint32   uSrcUnPackRegValue       = 0;
   uint32   uIndex                   = 0;
   uint32   uRegValue                = 0;

   if(NULL != psSurfaceInfo)
   {
      // Clear the pixel format
      psSurfaceInfo->ePixelFormat                  = HAL_MDP_PIXEL_FORMAT_NONE;

      if(HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)
      {
         // Extract the SSPP base address to program the HW registers
         uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

         // Read programmed pixel format from register
         uSrcFormatRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);

         // Original defined pixel formats in guSrcFormatInfo[] don't have tile setting info. Ignore this
         // field value so that we can check corresponding programmed pixel format.
         uSrcFormatRegValue &= ~HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_BMSK;

         // if chroma sample has been changed to H1V2, change it back to H2V1 just for comparison with guSrcFormatInfo
         if (HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2 == HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uSrcFormatRegValue, SRC_CHROMA_SAMP))
         {
            uSrcFormatRegValue = HWIO_OUT_FLD(uSrcFormatRegValue,
                                              MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                              SRC_CHROMA_SAMP,
                                              HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1);    
         }
     
         // Pixel format
         uSrcUnPackRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_UNPACK_PATTERN_ADDR + uRegisterOffset);

         // Mask VC1 reduce, solid fill and rotate 90 bits from the pixel format leaving the remaining bits untouched 
         uSrcFormatRegValue   &=  HAL_MDP_GET_SOURCE_PIXEL_FORMAT_MASK;
        
         for(uIndex = 0; uIndex < HAL_MDP_PIXEL_FORMAT_MAX; uIndex++)
         {
            if((uSrcFormatRegValue == guSrcFormatInfo[uIndex]) && (uSrcUnPackRegValue == guSrcUnpackInfo[uIndex]))
            {
               // Update Pixel Format Information
               psSurfaceInfo->ePixelFormat   = (HAL_MDP_PixelFormatType)uIndex;

               if(0x00 == (HAL_MDP_SOURCE_FLAGS_EXTRACT_ONLY_PIXEL_FORMAT & uFlags))
               {
                  // Update width and height information
                  uRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE_ADDR + uRegisterOffset);    // Image size

                  psSurfaceInfo->uWidthInPixel  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE, uRegValue, SRC_W);
                  psSurfaceInfo->uHeightInPixel = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE, uRegValue, SRC_H);

                  if(TRUE == HAL_MDP_CHECK_INTERLEAVED_PIXEL_FORMAT(psSurfaceInfo->ePixelFormat))  // One plane surface
                  {
                     uRegValue =  in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC0_ADDR_ADDR + uRegisterOffset);   // Address
                     psSurfaceInfo->uPlanes.sRgb.sDeviceAddress.u.uLowPart    = uRegValue;
                     psSurfaceInfo->uPlanes.sRgb.sDeviceAddress.u.iHighPart   = 0x00;

                     uRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset); // Stride
                     psSurfaceInfo->uPlanes.sRgb.uStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, uRegValue, SRCP0_YSTRIDE);
                  }
                  else if(TRUE == HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurfaceInfo->ePixelFormat)) // 2 plane address
                  {
                     uRegValue =  in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC0_ADDR_ADDR + uRegisterOffset);   // Address 1
                     psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.uLowPart    = uRegValue;
                     psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.iHighPart   = 0x00;

                     uRegValue =  in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC1_ADDR_ADDR + uRegisterOffset);   // Address 2
                     psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.uLowPart    = uRegValue;
                     psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.iHighPart   = 0x00;

                     uRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset); // Stride 1
                     psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.uLumaStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                              uRegValue, 
                                                                                              SRCP0_YSTRIDE);

                     psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.uChromaStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                                uRegValue, 
                                                                                                SRCP1_YSTRIDE);   // Stride 2
                  }
                  else // 3 Plane surface
                  {
                     uRegValue =  in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC0_ADDR_ADDR + uRegisterOffset);   // Address 1
                     psSurfaceInfo->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.uLowPart    = uRegValue;
                     psSurfaceInfo->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.iHighPart   = 0x00;

                     uRegValue =  in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC1_ADDR_ADDR + uRegisterOffset);   // Address 2
                     psSurfaceInfo->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.uLowPart    = uRegValue;
                     psSurfaceInfo->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.iHighPart   = 0x00;

                     uRegValue =  in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC2_ADDR_ADDR + uRegisterOffset);   // Address 3
                     psSurfaceInfo->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.uLowPart    = uRegValue;
                     psSurfaceInfo->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.iHighPart   = 0x00;

                     uRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset);
                     psSurfaceInfo->uPlanes.sYuvPlanar.uLumaStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                        uRegValue, 
                                                                                        SRCP0_YSTRIDE); // Stride 1

                     psSurfaceInfo->uPlanes.sYuvPlanar.uChromaBlueStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                              uRegValue, 
                                                                                              SRCP1_YSTRIDE);   // Stride 2

                     uRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1_ADDR + uRegisterOffset);
                     psSurfaceInfo->uPlanes.sYuvPlanar.uChromaRedStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                                                                             uRegValue, 
                                                                                             SRCP2_YSTRIDE);   // Stride 3
                  }
               }
               break;
            }
         }
      }
   }

   // Rectangle Information
   if(NULL != psRectInfo)
   {
      // Clear Rectangle Information
      psRectInfo->sSrcRectConfig.uPosX             = 0;
      psRectInfo->sSrcRectConfig.uPosY             = 0;
      psRectInfo->sSrcRectConfig.uWidthInPixels    = 0;
      psRectInfo->sSrcRectConfig.uHeightInPixels   = 0;

      psRectInfo->sDstRectConfig.uPosX             = 0;
      psRectInfo->sDstRectConfig.uPosY             = 0;
      psRectInfo->sDstRectConfig.uWidthInPixels    = 0;
      psRectInfo->sDstRectConfig.uHeightInPixels   = 0;

      HAL_MDP_SSPP_GetCropRectConfig(eSourcePipe, eLayerType, psRectInfo, uFlags);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GetStateInfo()
*/
/*!
* \brief
*     Returns the currrent state of the given SSPP ID
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [out]  pbActive            - TRUE - If the SSPP is active
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GetStateInfo(HAL_MDP_SourcePipeId     eSourcePipe,
                                      HAL_MDP_LayerType        eLayerType,
                                      bool32                  *pbActive,
                                      uint32                   uFlags)
{
   uint32                        uRegValue         = 0x00;
   HAL_MDP_ModuleStatusRegInfo  *psModuleRegInfo   = NULL;

   *(pbActive)  = FALSE;  // Initial state

   psModuleRegInfo = (HAL_MDP_ModuleStatusRegInfo  *)pgsMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_SOURCE_PIPE];

   if(NULL != psModuleRegInfo)
   {
      uRegValue   = in_dword(MMSS_MDP_REG_BASE + psModuleRegInfo[eSourcePipe].uRegAddress);

      if(0x00 != (uRegValue & psModuleRegInfo[eSourcePipe].uBitFieldMask) )
      {
         *(pbActive)  = TRUE;        // Module is in active state
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SurfaceConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psSurface           - Surface information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_SurfaceConfig(HAL_MDP_SourcePipeId       eSourcePipe,
                                       HAL_MDP_LayerType          eLayerType,
                                       HAL_MDP_SurfaceAttrType   *psSurface,
                                       uint32                     uFlags)
{
   uintPtr           uRegisterOffset         = 0;              // Register offset
   uint32            uImageSizeInPixels      = 0;              // Image size
   uint32            uPlane0Address          = 0;              // Plane 0 address (Color#0 - G/Luma)
   uint32            uPlane1Address          = 0;              // Plane 1 address (Color#1 - R/Cr)
   uint32            uPlane2Address          = 0;              // Plane 2 address (Color#2 - B/Cb)
   uint32            uPlane3Address          = 0;              // Plane 3 address (Color#3 - Alpha)
   uint32            uPlane01StrideInBytes   = 0;              // Plane 0 and 1 stride
   uint32            uPlane23StrideInBytes   = 0;              // Plane 2 and 3 stride
   uint32            uSrcFormat              = 0;              // Source format information (#of planes, #colors, #bits/each color)
   uint32            uSrcCurrentFormatInfo   = 0;
   uint32            uSrcUnpack              = 0;              // Unpacking information of the source buffer 
   uint32            uCursorPixelFormat      = 0;              // Cursor pixel format configuration
   uint32            uOpMode                 = 0;              // OP_MODE register
   uint32            uSrcOpModeInfo          = 0;              // Source OP_MODE register
   uintPtr           uLayerRegBaseOffset     = 0;

   HAL_MDSS_ErrorType   eStatus              = HAL_MDSS_STATUS_SUCCESS;

   uLayerRegBaseOffset = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

   // Height (16bits) : Width (16bits) SSPP_SRC_IMG_SIZE
   uImageSizeInPixels = HWIO_OUT_FLD(uImageSizeInPixels,
                                     MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE,
                                     SRC_H,
                                     psSurface->uHeightInPixel);

   uImageSizeInPixels = HWIO_OUT_FLD(uImageSizeInPixels,
                                     MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE,
                                     SRC_W,
                                     psSurface->uWidthInPixel);
   
   if (0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurface->uFlags))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sCompressedPlane.sCompressedDataDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurface->uPlanes.sCompressedPlane.sMetadataDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sCompressedPlane.uCompressedDataStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP1_YSTRIDE,
                                             psSurface->uPlanes.sCompressedPlane.uMetadataStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_INTERLEAVED_PIXEL_FORMAT(psSurface->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sRgb.sDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sRgb.uStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurface->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sYuvPsuedoPlanar.uLumaStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP1_YSTRIDE,
                                             psSurface->uPlanes.sYuvPsuedoPlanar.uChromaStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurface->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.uLowPart;
      uPlane2Address          = psSurface->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sYuvPlanar.uLumaStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP1_YSTRIDE,
                                             psSurface->uPlanes.sYuvPlanar.uChromaBlueStrideInBytes);
      uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                             SRCP2_YSTRIDE,
                                             psSurface->uPlanes.sYuvPlanar.uChromaRedStrideInBytes);
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      // OP MODE Register configuration
      if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)     // Only for VIG layers
      {
         uOpMode  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_ADDR + uLayerRegBaseOffset);

         if(TRUE == HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(psSurface->ePixelFormat)) // If YUV format
         {
            uOpMode   = HWIO_OUT_FLD(uOpMode, MDP_VP_0_VIG_0_OP_MODE, CSC_1_EN, TRUE);
            uOpMode   = HWIO_OUT_FLD(uOpMode, MDP_VP_0_VIG_0_OP_MODE, CSC_SRC_DATA_FORMAT, TRUE);     // SRC YUV Format and DST is always RGB
         }
         else  // RGB format
         {
            uOpMode   = HWIO_OUT_FLD(uOpMode, MDP_VP_0_VIG_0_OP_MODE, CSC_1_EN, FALSE);               // Bypass CSC
         }
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_ADDR + uLayerRegBaseOffset, uOpMode);
      }

      // SSPP

      // Extract the base address to program the HW registers
      uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);
      
      // SSPP_SRC_FORMAT
      uSrcFormat               = HAL_MDP_GetSourceFormatInfo(psSurface);

      uSrcCurrentFormatInfo    = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
      uSrcCurrentFormatInfo   &= HAL_MDP_CLEAR_SOURCE_PIXEL_FORMAT_MASK;
      uSrcFormat              |= uSrcCurrentFormatInfo;                  // Do not overwrite other register information

      // Enable macro tile format 
      if(psSurface->uFlags & HAL_MDP_FLAGS_SURFACE_ATTR_MACRO_TILE)
      {
        uSrcFormat |= (HAL_MDP_SRC_FORMAT_FRAME_FORMAT_TILE << HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_SHFT);
      }

      if(HAL_MDP_LAYER_TYPE_DMA == eLayerType)        // Configuring rotate 90 path
      {
         if(0x00 != (HAL_MDP_FLAGS_SSPP_SURFACE_ROTATE_90 & psSurface->uFlags))
         {
            uSrcFormat = HWIO_OUT_FLD(uSrcFormat, MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, ROT90, TRUE);               // DMA Layer is going through rotator with 90-degree rotation active
         }
      }

      // Source operation mode and unpack pattern
      uSrcOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);

      if (0x00 == (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurface->uFlags))
      {
         // clear BWC_DEC_EN bit in case it has been set to 1 previously
         uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, BWC_DEC_EN, 0);
         
         uSrcUnpack      = guSrcUnpackInfo[psSurface->ePixelFormat];
      } 
      else 
      {
         // set bandwidth decompression
         uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, BWC_DEC_EN, 1);
         // only support compression quality of LOSSLESS
         uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, BWC_DEC_QP, 0);

         uSrcUnpack      = guSrcDecompressUnpackInfo[psSurface->ePixelFormat];
      }

      if(HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)     // Only for VIG/RGB/DMA layers
      {
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE_ADDR + uRegisterOffset, uImageSizeInPixels);    // Image size
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC0_ADDR_ADDR + uRegisterOffset, uPlane0Address);           // Plane-0 address
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC1_ADDR_ADDR + uRegisterOffset, uPlane1Address);           // Plane-1 address
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC2_ADDR_ADDR + uRegisterOffset, uPlane2Address);           // Plane-2 address
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC3_ADDR_ADDR + uRegisterOffset, uPlane3Address);           // Plane-3 address
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset, uPlane01StrideInBytes); // Plane-0 and 1 stride
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1_ADDR + uRegisterOffset, uPlane23StrideInBytes); // Plane-2 and 3 stride
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcFormat);              // Src format information
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_UNPACK_PATTERN_ADDR + uRegisterOffset, uSrcUnpack);      // Src unpack information
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeInfo);         // Src operation mode
      }
      else     // Cursor Layer Type
      {
         switch(psSurface->ePixelFormat)
         {
         case HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP:
            uCursorPixelFormat   = HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB8888;
            break;
         case HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP:
            uCursorPixelFormat   = HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB1555;
            break;
         case HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP:
            uCursorPixelFormat   = HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB4444;
            break;
         default:
            break;
         }
         out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_IMG_SIZE_ADDR + uRegisterOffset, uImageSizeInPixels);    // Image size
         out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_STRIDE_ADDR + uRegisterOffset, uPlane01StrideInBytes);   // Stride
         out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_BASE_ADDR_ADDR + uRegisterOffset, uPlane0Address);       // Plane-0 address
         out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_FORMAT_ADDR + uRegisterOffset, uCursorPixelFormat);      // Cursor pixel format
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DecimationConfig()
*/
/*!
* \brief
*     Set up the decimator configuration
*
* \param [in]   eSourcePipe               - Source pipe id
* \param [in]   sMDPScaleInfo             - Surface information, it will recalculated for the Scaler
* \param [in]   sMDPDecimationInfo        - Decimation Factors.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DecimationConfig(HAL_MDP_SourcePipeId          eSourcePipe,
                                             HAL_MDP_ScalarInfo*           sMDPScaleInfo,
                                             HAL_MDP_DecimationConfigType* sMDPDecimationInfo)
{
   uintPtr uLayerRegOffset  = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);
   uint32 uDecimationValues = 0;

   /* Set the decimation factors if those are higher than the unit, otherwise we need to disable decimation */

   if (1 < sMDPDecimationInfo->uVertDecimationFactor) // Configure Vertical decimation
   {
      /* Recalculate the Height after decimation, this is the height needed to configure the Scaler */
      sMDPScaleInfo->uSrcHeightInPixels = (int) sMDPScaleInfo->uSrcHeightInPixels / sMDPDecimationInfo->uVertDecimationFactor;

      uDecimationValues = HWIO_OUT_FLD(uDecimationValues,
                                       MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                       VERT_S,
                                       (sMDPDecimationInfo->uVertDecimationFactor - 1));
      
   }

   if (1 < sMDPDecimationInfo->uHorzDecimationFactor) // Configure Horizontal decimation
   {
      /* Recalculate the Width after decimation, this is the width needed to configure the Scaler */
      sMDPScaleInfo->uSrcWidthInPixels = (int) sMDPScaleInfo->uSrcWidthInPixels / sMDPDecimationInfo->uHorzDecimationFactor;

      uDecimationValues = HWIO_OUT_FLD(uDecimationValues,
                                       MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                       HORZ_S,
                                       (sMDPDecimationInfo->uHorzDecimationFactor - 1));
   }

   /* Set Decimation Values: It will enable decimation if factor was higher than the unit, otherwise it will disable decimation */
   out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG_ADDR + uLayerRegOffset, uDecimationValues);

   return;

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ProgressiveScalarRegInfo()
*/
/*!
* \brief
*     Set up phase calculation for progressive stream
*
* \param [in]   pScaleInfo         - Scalar info of the surface
*
* \retval NONE
*
****************************************************************************/

//static void HAL_MDP_ProgressiveScalarRegInfo(HAL_MDP_ScalarInfo     *pScaleInfo)
//{
//   /* Check if the destination width and height are valid */
//   if ((0 != pScaleInfo->uDstWidthInPixels) && (0 != pScaleInfo->uDstHeightInPixels))
//   {
//	   uint64 uLclSrcWidthInPixels  = pScaleInfo->uSrcWidthInPixels;
//	   uint64 uLclSrcHeightInPixels = pScaleInfo->uSrcHeightInPixels;
//
//      /*
//       *    phase_step_x = ((input_width  << constant_phase_step)/output_width))
//       *    phase_step_y = ((input_height << constant_phase_step)/output_height))
//       */
//      pScaleInfo->uPhaseStepX    = (uint32)((uLclSrcWidthInPixels << HAL_MDP_PHASE_STEP_CONSTANT) / pScaleInfo->uDstWidthInPixels);
//      pScaleInfo->uPhaseStepY    = (uint32)((uLclSrcHeightInPixels << HAL_MDP_PHASE_STEP_CONSTANT) / pScaleInfo->uDstHeightInPixels);
//
//      /*
//       *    init_phase_x = ((phase_step_x - (1 << constant_phase_step)) >> 1)
//       *    init_phase_y = ((phase_step_y - (1 << constant_phase_step)) >> 1)
//       */
//      pScaleInfo->iInitPhaseX    = ((pScaleInfo->uPhaseStepX - (1 << HAL_MDP_PHASE_STEP_CONSTANT)) >> 1 );
//      pScaleInfo->iInitPhaseY    = ((pScaleInfo->uPhaseStepY - (1 << HAL_MDP_PHASE_STEP_CONSTANT)) >> 1 );
//
//      // Check for upscale or downscale and select the default filter mode 
//      // Horizontal direction
//      if (pScaleInfo->uSrcWidthInPixels > pScaleInfo->uDstWidthInPixels)    // Downscale
//      {
//         pScaleInfo->iInitPhaseX                = 0x00;    
//         pScaleInfo->eHorzFilterRegFieldVal     = HAL_MDP_SCALAR_PCMN_FILTER;
//      }
//      else  // Upscale or no-scale (unity scale)
//      {
//         pScaleInfo->eHorzFilterRegFieldVal     = HAL_MDP_SCALAR_BILINEAR_FILTER;
//      }
//
//      // Vertical direction
//      if (pScaleInfo->uSrcHeightInPixels >  pScaleInfo->uDstHeightInPixels)    // Downscale or no-scale
//      {
//         pScaleInfo->iInitPhaseY                = 0x00; 
//         pScaleInfo->eVertFilterRegFieldVal     = HAL_MDP_SCALAR_PCMN_FILTER;
//      }
//      else // Upscale or no-scale
//      {
//         pScaleInfo->eVertFilterRegFieldVal     = HAL_MDP_SCALAR_BILINEAR_FILTER;
//      }
//   }   
//}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DeinterlaceScalarRegInfo()
*/
/*!
* \brief
*     Set up phase calculation for deinterlaced stream
*
* \param [in]   pScaleInfo                         - Scalar info of the surface
* \param [in]   eColorComponentInfo        - color component type
* \param [in]   psChromaConfigInfo          - chroma configuration
* \param [in]   psDeinterlaceConfig           - deinterlace info
*
* \retval NONE
*
* Chroma Site Position
*         0  64  128                                                 
*                                                
*  0    - A - D - G -                                        
*
*  64   - B - E - H -                                             
* 
*  128  - C - F - I -                                         
*  
****************************************************************************/

//static void HAL_MDP_DeinterlaceScalarRegInfo(HAL_MDP_ScalarInfo     *pScaleInfo, 
//                                    HAL_MDP_QSEED2_ColorComponentType eColorComponentInfo,
//                                    HAL_MDP_ChromaConfigType *psChromaConfigInfo,
//                                    HAL_MDP_DeinterlaceConfigType *psDeinterlaceConfig)
//{
  ///* Check if the destination width and height are valid */
  //if ((0 != pScaleInfo->uDstWidthInPixels) && (0 != pScaleInfo->uDstHeightInPixels))
  //{
  //  uint64 uLclSrcWidthInPixels  = pScaleInfo->uSrcWidthInPixels;
  //  uint64 uLclSrcHeightInPixels = pScaleInfo->uSrcHeightInPixels;

  //  /*
  //     *    phase_step_x = ((input_width  << constant_phase_step)/output_width))
  //     *    phase_step_y = ((input_height << constant_phase_step)/output_height))
  //     */
  //  pScaleInfo->uPhaseStepX    = (uint32)((uLclSrcWidthInPixels << HAL_MDP_PHASE_STEP_CONSTANT) / pScaleInfo->uDstWidthInPixels);
  //  pScaleInfo->uPhaseStepY    = (uint32)((uLclSrcHeightInPixels << HAL_MDP_PHASE_STEP_CONSTANT) / pScaleInfo->uDstHeightInPixels);

  //  //for color component 0 and 3
  //  if(HAL_MDP_QSEED2_COLOR_COMPONENT_TYPE_0_3 == eColorComponentInfo)
  //  {
  //    pScaleInfo->iInitPhaseX      = 0;
  //    if(!psDeinterlaceConfig->bReferenceOddField)//Top or even
  //    {
  //      pScaleInfo->iInitPhaseY    = 0;
  //    }
  //    else
  //    {
  //      pScaleInfo->iInitPhaseY    = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 1));;
  //    }
  //  }
  //  //for color component 1 and 2
  //  else
  //  {
  //    //Calculate init phase in X direction
  //    if(psChromaConfigInfo->uSubsamplingX == HAL_MDP_CHROMA_SUBSAMPLING_TYPE_NONE) //no subsampling
  //    {
  //      pScaleInfo->iInitPhaseX = 0;
  //    }
  //    else // there is subsampling
  //    {
  //      if(psChromaConfigInfo->uSiteX == 0)//Chroma site location at A,B,C position
  //      {
  //        pScaleInfo->iInitPhaseX = 0;
  //      }
  //      else if(psChromaConfigInfo->uSiteX == (1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)))//Chroma site location at D, E, F position
  //      {
  //        pScaleInfo->iInitPhaseX = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 2));
  //      }
  //      else if(psChromaConfigInfo->uSiteX == (1<<HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION)) //Chroma site location at G, H, I position
  //      {
  //        pScaleInfo->iInitPhaseX = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 1));
  //      }
  //    }

  //    //Calculate init phase in Y direction for top field
  //    if(!psDeinterlaceConfig->bReferenceOddField)//Top or even field
  //    {
  //      if (psChromaConfigInfo->uSubsamplingY == HAL_MDP_CHROMA_SUBSAMPLING_TYPE_NONE) //no subsampling
  //      {
  //        pScaleInfo->iInitPhaseY = 0;
  //      }
  //      else
  //      {
  //        if(psChromaConfigInfo->uSiteY == 0)//Chroma site location at A, D, G position
  //        {
  //          pScaleInfo->iInitPhaseY = 0;
  //        }
  //        else if(psChromaConfigInfo->uSiteY == (1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)))//Chroma site location at B, E, H position
  //        {
  //          pScaleInfo->iInitPhaseY = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 3));
  //        }
  //        else if(psChromaConfigInfo->uSiteY == (1<<HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION))//Chroma site location at C, F, I position
  //        {
  //          pScaleInfo->iInitPhaseY = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 2));
  //        }
  //      }
  //    }
  //    //Calculate init phase in Y direction for bottom field
  //    else //bottom/odd field
  //    {
  //      if (psChromaConfigInfo->uSubsamplingY == HAL_MDP_CHROMA_SUBSAMPLING_TYPE_NONE) //no subsampling
  //      {
  //        pScaleInfo->iInitPhaseY = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 1));
  //      }
  //      else
  //      {
  //        if(psChromaConfigInfo->uSiteY == 0)//Chroma site location at A, D, G position
  //        {
  //          pScaleInfo->iInitPhaseY = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 1));
  //        }
  //        else if(psChromaConfigInfo->uSiteY == (1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)))//Chroma site location at B, E, H position
  //        {
  //          pScaleInfo->iInitPhaseY = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 1))-(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 3));
  //        }
  //        else if(psChromaConfigInfo->uSiteY == (1<<HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION))//Chroma site location at C, F, I position
  //        {
  //          pScaleInfo->iInitPhaseY = -(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 1))-(1<<(HAL_MDP_PHASE_STEP_CONSTANT - 2));
  //        }
  //      }
  //    }
  //  }
  //}

  //// Check for upscale or downscale and select the default filter mode 
  //// Horizontal direction
  //if (pScaleInfo->uSrcWidthInPixels > pScaleInfo->uDstWidthInPixels)    // Downscale
  //{
  //  pScaleInfo->eHorzFilterRegFieldVal = HAL_MDP_SCALAR_PCMN_FILTER;
  //}
  //else  // Upscale or no-scale (unity scale)
  //{
  //  pScaleInfo->eHorzFilterRegFieldVal = HAL_MDP_SCALAR_BILINEAR_FILTER;
  //}
  //// Vertical direction
  //if (pScaleInfo->uSrcHeightInPixels >  pScaleInfo->uDstHeightInPixels)    // Downscale
  //{
  //  pScaleInfo->eVertFilterRegFieldVal = HAL_MDP_SCALAR_PCMN_FILTER;
  //}
  //else // Upscale or no-scale
  //{
  //  pScaleInfo->eVertFilterRegFieldVal = HAL_MDP_SCALAR_BILINEAR_FILTER;
  //}
//}   

static void HAL_MDP_ScalarRegInfo(HAL_MDP_SourcePipeId   eSourcePipe,
                                  HAL_MDP_LayerType      eLayerType,
                                  HAL_MDP_ScalarInfo     *pScaleInfo)
{
   /* Check if the destination width and height are valid */
   if ((0 != pScaleInfo->uDstWidthInPixels) && (0 != pScaleInfo->uDstHeightInPixels))
   {
      uint64 uLclSrcWidthInPixels  = pScaleInfo->uSrcWidthInPixels;
      uint64 uLclSrcHeightInPixels = pScaleInfo->uSrcHeightInPixels;

      /*
       *    phase_step_x = ((input_width  << constant_phase_step)/output_width))
       *    phase_step_y = ((input_height << constant_phase_step)/output_height))
       *    
       *    input_width and input_height are casted to 64 bit to support values that are larger than 2047
       */
      pScaleInfo->uPhaseStepX    = (uint32)((uLclSrcWidthInPixels << HAL_MDP_PHASE_STEP_CONSTANT) / pScaleInfo->uDstWidthInPixels);
      pScaleInfo->uPhaseStepY    = (uint32)((uLclSrcHeightInPixels << HAL_MDP_PHASE_STEP_CONSTANT) / pScaleInfo->uDstHeightInPixels);

      /*
       *    init_phase_x = ((phase_step_x - 1) >> 1)
       *    init_phase_y = ((phase_step_y - 1) >> 1)
       */
      if(HAL_MDP_LAYER_TYPE_RGB == eLayerType)  // iInitPhaseX and iInitPhaseY must be 0x00 for RGB layers
      {
         pScaleInfo->iInitPhaseX    = 0x00;
         pScaleInfo->iInitPhaseY    = 0x00;
      }
      else
      {
         pScaleInfo->iInitPhaseX    = ( (pScaleInfo->uPhaseStepX - 1) >> 1 );
         pScaleInfo->iInitPhaseY    = ( (pScaleInfo->uPhaseStepY - 1) >> 1 );
      }

      // Check for upscale or downscale and select the default filter mode 
      // Horizontal direction
      if (pScaleInfo->uSrcWidthInPixels > pScaleInfo->uDstWidthInPixels)    // Downscale
      {
         pScaleInfo->eHorzFilterRegFieldVal     = HAL_MDP_SCALAR_PCMN_FILTER;
      }
      else  // Upscale or no-scale (unity scale)
      {
         pScaleInfo->eHorzFilterRegFieldVal     = HAL_MDP_SCALAR_BILINEAR_FILTER;
      }

      // Vertical direction
      if (pScaleInfo->uSrcHeightInPixels >  pScaleInfo->uDstHeightInPixels)    // Downscale or no-scale
      {
         pScaleInfo->eVertFilterRegFieldVal     = HAL_MDP_SCALAR_PCMN_FILTER;
      }
      else // Upscale or no-scale
      {
         pScaleInfo->eVertFilterRegFieldVal     = HAL_MDP_SCALAR_BILINEAR_FILTER;
      }
   }   
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_PixelExtensionConfig()
*/
/*!
* \brief
*     Program decimation, scaler, initial phase, phase step and PE registers
*     populated from HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \param [in]   eSourcePipe             - Source pipe id
* \param [in]   eLayerType              - Type of the layer
* \param [in]   psDecimationConfig      - Decimation parameters
* \param [in]   psPixelExtLayerConfig   - Values populated from 
*                                         HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_PixelExtensionConfig(HAL_MDP_SourcePipeId             eSourcePipe,   
                                         HAL_MDP_LayerType                eLayerType,
                                         HAL_MDP_DecimationConfigType    *psDecimationConfig, 
                                         HAL_MDP_PixelExtLayerConfigType *psPixelExtLayerConfig)
{
   uintPtr                 uRegisterOffset     = 0;
   uintPtr                 uLayerRegOffset     = 0;
   uint32                  uQseed2Config       = 0;
   uint32                  uPixExtLR           = 0;
   uint32                  uPixExtTB           = 0;
   uint32                  uReqPixels          = 0;
   uint32                  uOpMode             = 0;
   uint32                  uDecimationRegValue = 0;
   uint32                  uI;

   // Extract the layer's SSPP base address
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);
   uOpMode           = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uLayerRegOffset);
   
   // Extract the layer's scalar config base address
   uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

   if ((HAL_MDP_LAYER_TYPE_RGB == eLayerType) ||
       (HAL_MDP_LAYER_TYPE_VIG == eLayerType))
   {
      // Decimation 
      if (NULL != psDecimationConfig)
      {
            if (1 < psDecimationConfig->uVertDecimationFactor)
            {
                  // Vertical decimation 
                  uDecimationRegValue = HWIO_OUT_FLD(uDecimationRegValue,
                                                        MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                                        VERT_S,
                                                       (psDecimationConfig->uVertDecimationFactor - 1));
            }

            if (1 < psDecimationConfig->uHorzDecimationFactor)
            {
                  // Horizontal decimation 
                  uDecimationRegValue = HWIO_OUT_FLD(uDecimationRegValue,
                                                        MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                                        HORZ_S,
                                                       (psDecimationConfig->uHorzDecimationFactor - 1));
            }

            // Program decimation config register 
            out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG_ADDR + uLayerRegOffset, uDecimationRegValue);
      }
   }

  //scaling
  //ToDo Handle the scaling for other layers
  if (HAL_MDP_LAYER_TYPE_VIG == eLayerType)
  {
     // Init Phase and Phase Step for components 0 and 3
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_PHASE_STEP_X_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.uPhaseStepX); // Phase step X step (color 0-3 )
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_PHASE_STEP_Y_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.uPhaseStepY); // Phase step Y step (color 0-3 )
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_INIT_PHASE_X_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.iInitPhaseX); // Init Phase X (color 0-3 )
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_INIT_PHASE_Y_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.iInitPhaseY); // Init Phase Y (color 0-3 )

     // Filter mode for components 0 and 3
     uQseed2Config = HWIO_OUT_FLD(uQseed2Config,
                                  MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                  COMP0_HORZ_FILTER_MODE,
                                  psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.eHorScaleFilter);

     uQseed2Config = HWIO_OUT_FLD(uQseed2Config,
                                  MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                  COMP0_VERT_FILTER_MODE,
                                  psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.eVerScaleFilter);

     uQseed2Config = HWIO_OUT_FLD(uQseed2Config,
                                  MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                  COMP3_HORZ_FILTER_MODE,
                                  psPixelExtLayerConfig->aPixelExtComponentConfig[3].sScalerPlane.eHorScaleFilter);

     uQseed2Config = HWIO_OUT_FLD(uQseed2Config,
                                  MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                  COMP3_VERT_FILTER_MODE,
                                  psPixelExtLayerConfig->aPixelExtComponentConfig[3].sScalerPlane.eVerScaleFilter);


      
     // Init Phase and Phase Step for components 1 and 2
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_PHASE_STEP_X_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[1].sScalerPlane.uPhaseStepX); // Phase step X step (color 0-3 )
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_PHASE_STEP_Y_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[1].sScalerPlane.uPhaseStepY); // Phase step Y step (color 0-3 )
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_INIT_PHASE_X_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[1].sScalerPlane.iInitPhaseX); // Init Phase X (color 0-3 
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_INIT_PHASE_Y_ADDR + uLayerRegOffset, psPixelExtLayerConfig->aPixelExtComponentConfig[1].sScalerPlane.iInitPhaseY); // Init Phase Y (color 0-3 )

     // Filter mode for components 1 and 2
     uQseed2Config = HWIO_OUT_FLD(uQseed2Config,
                                  MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                  COMP1_2_HORZ_FILTER_MODE,
                                  psPixelExtLayerConfig->aPixelExtComponentConfig[1].sScalerPlane.eHorScaleFilter);

     uQseed2Config = HWIO_OUT_FLD(uQseed2Config,
                                  MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                  COMP1_2_VERT_FILTER_MODE,
                                  psPixelExtLayerConfig->aPixelExtComponentConfig[1].sScalerPlane.eVerScaleFilter);
      

     // Horizontal scaler enable
     if (psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.uPhaseStepX > 0)
     {
        uQseed2Config = HWIO_OUT_FLD(uQseed2Config, MDP_VP_0_VIG_0_QSEED2_CONFIG, SCALEX_EN, TRUE);
     }
     else
     {
        uQseed2Config = HWIO_OUT_FLD(uQseed2Config, MDP_VP_0_VIG_0_QSEED2_CONFIG, SCALEX_EN, FALSE);
     }

     // Vertical scaler enable
     if (psPixelExtLayerConfig->aPixelExtComponentConfig[0].sScalerPlane.uPhaseStepY > 0)
     {
        uQseed2Config = HWIO_OUT_FLD(uQseed2Config, MDP_VP_0_VIG_0_QSEED2_CONFIG, SCALEY_EN, TRUE);
     }
     else
     {
        uQseed2Config = HWIO_OUT_FLD(uQseed2Config, MDP_VP_0_VIG_0_QSEED2_CONFIG, SCALEY_EN, FALSE);
     }
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_ADDR + uLayerRegOffset, uQseed2Config);
  }

  // SW Pixel Extension Coefficients
  for(uI=0; uI<HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS; uI++)
  {
     uPixExtLR      = HWIO_OUT_FLD(uPixExtLR,
                                    MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                    LEFT_RPT,
                                    psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.uLeftRepeat);
     uPixExtLR      = HWIO_OUT_FLD(uPixExtLR,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                   LEFT_OVF,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.iLeftOverFetch);
     uPixExtLR      = HWIO_OUT_FLD(uPixExtLR,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                   RIGHT_RPT,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.uRightRepeat);
     uPixExtLR      = HWIO_OUT_FLD(uPixExtLR,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                   RIGHT_OVF,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.iRightOverFetch);

     uPixExtTB      = HWIO_OUT_FLD(uPixExtTB,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                   TOP_RPT,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.uTopRepeat);
     uPixExtTB      = HWIO_OUT_FLD(uPixExtTB,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                   TOP_OVF,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.iTopOverFetch);
     uPixExtTB      = HWIO_OUT_FLD(uPixExtTB,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                   BOTTOM_RPT,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.uBottomRepeat);
     uPixExtTB      = HWIO_OUT_FLD(uPixExtTB,
                                   MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                   BOTTOM_OVF,
                                   psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.iBottomOverFetch);

     uReqPixels     = HWIO_OUT_FLD(uReqPixels,
                                    MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_REQ_PIXELS,
                                    LEFT_RIGHT,
                                    psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.uLeftRightReqPxls);
     uReqPixels     = HWIO_OUT_FLD(uReqPixels,
                                    MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_REQ_PIXELS,
                                    TOP_BOTTOM,
                                    psPixelExtLayerConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig.uTopBottomReqPxls);

     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_ADDR + uLayerRegOffset + uMDPSSPPPixExtRegBaseOffset[uI], uPixExtLR);
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB_ADDR + uLayerRegOffset + uMDPSSPPPixExtRegBaseOffset[uI], uPixExtTB);
     out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_REQ_PIXELS_ADDR + uLayerRegOffset + uMDPSSPPPixExtRegBaseOffset[uI], uReqPixels);
  }

  // Set the SW PE override bit
  uOpMode     = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE,
                             SW_PIX_EXT_OVERRIDE,
                             TRUE); 
  out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uOpMode);
}
/****************************************************************************
*
** FUNCTION: HAL_MDP_ScalarConfig()
*/
/*!
* \brief
*     Program decimation, scaler, initial phase and phase step registers
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type, Only RGB and ViG are supported
* \param [in]   ePixelFormat        - Surface pixel format information
* \param [in]   psRectInfo          - Source and destination rectangles for scaler programming
* \param [in]   psDecimationConfig  - Decimation config
* \param [in]   uFlags              - Reserved
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_ScalarConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                 HAL_MDP_LayerType                eLayerType,
                                 HAL_MDP_PixelFormatType          ePixelFormat,
                                 HAL_MDP_CropRectangleType       *psRectInfo,
                                 HAL_MDP_DecimationConfigType    *psDecimationConfig,
                                 HAL_MDP_DeinterlaceConfigType   *psDeinterlaceConfig,
                                 uint32                           uFlags)
{
   uintPtr                 uLayerRegOffset    = 0;
   uint32                  uQseed2Config      = 0;
   HAL_MDP_ScalarInfo      sMDPScaleInfo;
   uintPtr                  uSsppBaseOffset   = 0;
   uint32                  uSrcFormatRegValue = 0;
   uint32                  uOpModeRegValue    = 0;
   uint32                  uChromaSampleValue = 0;
   uint32                  uSrcOpModeRegValue = 0;
   // Extract the layer base address
   uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

   sMDPScaleInfo.uPhaseStepX                 = 0;
   sMDPScaleInfo.uPhaseStepY                 = 0;
   sMDPScaleInfo.iInitPhaseX                 = 0;
   sMDPScaleInfo.iInitPhaseY                 = 0;
   sMDPScaleInfo.eHorzFilterRegFieldVal      = 0;
   sMDPScaleInfo.eVertFilterRegFieldVal      = 0;

   // Only for RGB and VIG layer
   if((HAL_MDP_LAYER_TYPE_RGB == eLayerType) || (HAL_MDP_LAYER_TYPE_VIG == eLayerType))
   {
      if(HAL_MDP_PIXEL_FORMAT_NONE != ePixelFormat)
      {
         /* Disable SW pixel extension override */
         uSrcOpModeRegValue = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uLayerRegOffset);
         uSrcOpModeRegValue = HWIO_OUT_FLD(uSrcOpModeRegValue,
                                        MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE,
                                        SW_PIX_EXT_OVERRIDE,
                                        FALSE);
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uLayerRegOffset, uSrcOpModeRegValue); 

         // We need to fill the sMDPScaleInfo struct before decimation, since in the case that decimation is supported
         // and enabled, the Width and Height of the Source must be recalculated with the values after-decimation
         // for the further Scaler configuration.
         
         sMDPScaleInfo.uSrcWidthInPixels     = psRectInfo->sSrcRectConfig.uWidthInPixels;
         sMDPScaleInfo.uSrcHeightInPixels    = psRectInfo->sSrcRectConfig.uHeightInPixels;
         sMDPScaleInfo.uDstWidthInPixels     = psRectInfo->sDstRectConfig.uWidthInPixels;
         sMDPScaleInfo.uDstHeightInPixels    = psRectInfo->sDstRectConfig.uHeightInPixels;

         // Check decimation is supported, so we can access the Decimation registers
         if (NULL != psDecimationConfig)
         {
            // Configure decimation
            HAL_MDP_DecimationConfig(eSourcePipe, &sMDPScaleInfo, psDecimationConfig);
         }

         // Color Components 0 and 3

         if(sMDPScaleInfo.uSrcWidthInPixels != psRectInfo->sDstRectConfig.uWidthInPixels)
         {
            uQseed2Config     = HWIO_OUT_FLD(uQseed2Config,
                                             MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                             SCALEX_EN,
                                             TRUE);
         }

         if(sMDPScaleInfo.uSrcHeightInPixels != psRectInfo->sDstRectConfig.uHeightInPixels)
         {
            uQseed2Config     = HWIO_OUT_FLD(uQseed2Config,
                                             MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                             SCALEY_EN,
                                             TRUE);
         }

         HAL_MDP_ScalarRegInfo(eSourcePipe, eLayerType, &sMDPScaleInfo);

         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_PHASE_STEP_X_ADDR + uLayerRegOffset, sMDPScaleInfo.uPhaseStepX); // Phase step X step (color 0-3 )
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_PHASE_STEP_Y_ADDR + uLayerRegOffset, sMDPScaleInfo.uPhaseStepY); // Phase step Y step (color 0-3 )
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_INIT_PHASE_X_ADDR + uLayerRegOffset, sMDPScaleInfo.iInitPhaseX); // Init Phase X (color 0-3 )
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP0_3_INIT_PHASE_Y_ADDR + uLayerRegOffset, sMDPScaleInfo.iInitPhaseY); // Init Phase Y (color 0-3 )

         uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                           MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                           COMP0_HORZ_FILTER_MODE,
                                           sMDPScaleInfo.eHorzFilterRegFieldVal);
  
         uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                           MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                           COMP0_VERT_FILTER_MODE,
                                           sMDPScaleInfo.eVertFilterRegFieldVal);

         /* Disable CAF for now
         //If both are upscale and format is YUV, use CAF for component 0 only. For other components, use derived one.
         if((sMDPScaleInfo.uDstWidthInPixels > sMDPScaleInfo.uSrcWidthInPixels)   &&
            (sMDPScaleInfo.uDstHeightInPixels > sMDPScaleInfo.uSrcHeightInPixels) &&
            (TRUE == HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(ePixelFormat))               &&
            (HAL_MDP_IS_QSEED_FILTER_SUPPORTED(HAL_MDP_SCALAR_CAF_FILTER)))
         {
            uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                              MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                              COMP0_HORZ_FILTER_MODE,
                                              HAL_MDP_SCALAR_CAF_FILTER);
           
            uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                              MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                              COMP0_VERT_FILTER_MODE,
                                              HAL_MDP_SCALAR_CAF_FILTER);
         }*/

         uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                           MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                           COMP3_HORZ_FILTER_MODE,
                                           sMDPScaleInfo.eHorzFilterRegFieldVal);

         uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                           MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                           COMP3_VERT_FILTER_MODE,
                                           sMDPScaleInfo.eVertFilterRegFieldVal);

         if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)
         {
            // Extract the SSPP base address to program the HW registers
            uSsppBaseOffset      = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

            // Read surface format register and OP_MODE register
            uSrcFormatRegValue   = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uSsppBaseOffset);
            uOpModeRegValue      = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_ADDR + uLayerRegOffset);

            // Extract surface color-space type (RGB or YUV)
            uChromaSampleValue   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uSrcFormatRegValue, SRC_CHROMA_SAMP);

            // Note for decimation in CrCb:
            // If decimation it is supported and enabled, the size of the CrCb (UV) Plane will be fully sampled against the size of the Y Plane after decimation, 
            // this is due the HW support for decimation in the YUV pixel formats, which it is defined as follows:
            // (Y=2, UV=0); (Y=4, UV=2); (Y=8, UV=4); (Y=16, UV=8)
            // The meaning of those factors is: 
            // If HW decimate Y by a factor of 2, it will not decimate UV, 
            // If HW decimate Y by a factor of 4, it will decimate UV by a factor of 2...
            // Given that difference in the decimation for the color components in the YUV is why we do not need to divide by two (right shift) the UV
            // widths\heights for subsampled pixel formats when decimation is enabled.

            if(0x01 == (HWIO_GETVAL(MDP_VP_0_VIG_0_OP_MODE, uOpModeRegValue, CSC_SRC_DATA_FORMAT)))
            {
               // YUV 12-BPP surface
               if(HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1 == uChromaSampleValue) // NV12 and NV21 format (or) H2V2
               {     
                  // If decimation is not used, re-calculate Width in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uHorzDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcWidthInPixels      = (psRectInfo->sSrcRectConfig.uWidthInPixels >> 1);
                  }
                  // If decimation is not used, re-calculate Height in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uVertDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcHeightInPixels     = (psRectInfo->sSrcRectConfig.uHeightInPixels >> 1);
                  }
               }
               else if(HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444  == uChromaSampleValue)     // YUV 32-BPP
               {
                  // If decimation is not used, re-calculate Width in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uHorzDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcWidthInPixels      = (psRectInfo->sSrcRectConfig.uWidthInPixels);
                  }
                  // If decimation is not used, re-calculate Height in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uVertDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcHeightInPixels     = (psRectInfo->sSrcRectConfig.uHeightInPixels);
                  }
               }
               else if (HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2 == uChromaSampleValue) // YUV H1V2
               {
                  // If decimation is not used, re-calculate Width in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uHorzDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcWidthInPixels      = (psRectInfo->sSrcRectConfig.uWidthInPixels);
                  }
                  // If decimation is not used, re-calculate Height in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uVertDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcHeightInPixels     = (psRectInfo->sSrcRectConfig.uHeightInPixels >> 1);
                  }
               }
               else // HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1 (or) YUV 16-BPP
               {
                  // If decimation is not used, re-calculate Width in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uHorzDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcWidthInPixels      = (psRectInfo->sSrcRectConfig.uWidthInPixels >> 1);
                  }
                  // If decimation is not used, re-calculate Height in the CrCb Plane
                  if ((NULL != psDecimationConfig) ? (psDecimationConfig->uVertDecimationFactor <= 1) : TRUE )
                  {
                     sMDPScaleInfo.uSrcHeightInPixels     = (psRectInfo->sSrcRectConfig.uHeightInPixels );
                  }
               }

               HAL_MDP_ScalarRegInfo(eSourcePipe, eLayerType, &sMDPScaleInfo);
            }

            out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_PHASE_STEP_X_ADDR + uLayerRegOffset, sMDPScaleInfo.uPhaseStepX); // Phase step X step (color 1-2 )
            out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_PHASE_STEP_Y_ADDR + uLayerRegOffset, sMDPScaleInfo.uPhaseStepY); // Phase step Y step (color 1-2 )
            out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_INIT_PHASE_X_ADDR + uLayerRegOffset, sMDPScaleInfo.iInitPhaseX); // Init Phase X (color 1-2 )
            out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_COMP1_2_INIT_PHASE_Y_ADDR + uLayerRegOffset, sMDPScaleInfo.iInitPhaseY); // Init Phase Y (color 1-2 )

            uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                              MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                              COMP1_2_HORZ_FILTER_MODE,
                                              sMDPScaleInfo.eHorzFilterRegFieldVal);

            uQseed2Config      = HWIO_OUT_FLD(uQseed2Config,
                                              MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                              COMP1_2_VERT_FILTER_MODE,
                                              sMDPScaleInfo.eVertFilterRegFieldVal);

            if(sMDPScaleInfo.uSrcWidthInPixels != psRectInfo->sDstRectConfig.uWidthInPixels)
            {
               uQseed2Config     = HWIO_OUT_FLD(uQseed2Config,
                                             MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                             SCALEX_EN,
                                             TRUE);
            }

            if(sMDPScaleInfo.uSrcHeightInPixels != psRectInfo->sDstRectConfig.uHeightInPixels)
            {
               uQseed2Config     = HWIO_OUT_FLD(uQseed2Config,
                                             MDP_VP_0_VIG_0_QSEED2_CONFIG,
                                             SCALEY_EN,
                                             TRUE);
            }
             
         }

         // Layer configuration (Specific to RGB and VIG layers)
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_CONFIG_ADDR + uLayerRegOffset, uQseed2Config);               // QSEED2 scale enable configuration
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_CropRectConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psRectInfo          - Rectangle configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_CropRectConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                        HAL_MDP_LayerType            eLayerType,
                                        HAL_MDP_CropRectangleType   *psRectInfo,
                                        uint32                       uFlags)
{
   uintPtr  uRegisterOffset   = 0;
   uint32   uSrcRectXYPos     = 0;
   uint32   uSrcRectSize      = 0;
   uint32   uDstRectXYPos     = 0;
   uint32   uDstRectSize      = 0;

   // Extract the SSPP base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   // SRC ROI (x,y)
   uSrcRectXYPos   = HWIO_OUT_FLD(uSrcRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_SRC_XY,
                                  SRC_X,
                                  psRectInfo->sSrcRectConfig.uPosX);

   uSrcRectXYPos   = HWIO_OUT_FLD(uSrcRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_SRC_XY,
                                  SRC_Y,
                                  psRectInfo->sSrcRectConfig.uPosY);

   // SRC ROI (w, h)
   uSrcRectSize   = HWIO_OUT_FLD(uSrcRectSize,
                                 MDP_VP_0_VIG_0_SSPP_SRC_SIZE,
                                 ROI_W,
                                 psRectInfo->sSrcRectConfig.uWidthInPixels);

   uSrcRectSize   = HWIO_OUT_FLD(uSrcRectSize,
                                 MDP_VP_0_VIG_0_SSPP_SRC_SIZE,
                                 ROI_H,
                                 psRectInfo->sSrcRectConfig.uHeightInPixels);

   // DST ROI (x,y)
   uDstRectXYPos   = HWIO_OUT_FLD(uDstRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                  DST_X,
                                  psRectInfo->sDstRectConfig.uPosX);

   uDstRectXYPos   = HWIO_OUT_FLD(uDstRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                  DST_Y,
                                  psRectInfo->sDstRectConfig.uPosY);

   // DST ROI (w, h)
   uDstRectSize   = HWIO_OUT_FLD(uDstRectSize,
                                 MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                 DST_W,
                                 psRectInfo->sDstRectConfig.uWidthInPixels);

   uDstRectSize   = HWIO_OUT_FLD(uDstRectSize,
                                 MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                 DST_H,
                                 psRectInfo->sDstRectConfig.uHeightInPixels);

   // Configure scale step information


   if(HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)  // Only for RGB/VIG/DMA layers
   {
      // Layer SSPP configuration (Common configuration for RGB, VIG, DMA layers)
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_XY_ADDR + uRegisterOffset, uSrcRectXYPos);    // Source ROI (x,y)
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_SIZE_ADDR + uRegisterOffset, uSrcRectSize);   // Source ROI (w,h)
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_OUT_XY_ADDR + uRegisterOffset, uDstRectXYPos);    // Destination ROI (x,y)
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_ADDR + uRegisterOffset, uDstRectSize);   // Destination ROI (w,h)
   }
   else     // Only for cursors
   {
      out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_XY_ADDR + uRegisterOffset, uSrcRectXYPos);            // Source Rectangle start position
      out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_SIZE_ADDR + uRegisterOffset, uSrcRectSize);           // Source Rectangle size
      out_dword(HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_START_XY_ADDR + uRegisterOffset, uDstRectXYPos);      // Destination Rectangle start position
      // Cursor's destination rectangle width and height are same as source rectangle width and height
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ColorfillConfig()
*/
/*!
* \brief
*     Initializes the constant color fill information for the given pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psColorFillInfo     - Color fill information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_ColorfillConfig(HAL_MDP_SourcePipeId        eSourcePipe,
                                         HAL_MDP_LayerType           eLayerType,
                                         HAL_MDP_ColorFillConfigType *psColorFillInfo,
                                         uint32                      uFlags)
{
   uintPtr  uRegisterOffset         = 0;
   uint32   uSrcCurrentFormatInfo   = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   // Color Fill 
   uSrcCurrentFormatInfo  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
   uSrcCurrentFormatInfo  = HWIO_OUT_FLD(uSrcCurrentFormatInfo,
                                         MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                         SOLID_FILL,
                                         (uint32)psColorFillInfo->bEnable);

   // Expected source, rectangle information configuration is already format

   out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcCurrentFormatInfo);                     // Src format information
   out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_CONSTANT_COLOR_ADDR + uRegisterOffset, psColorFillInfo->uConstantColor);   // Color
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ImmediateFlipConfig()
*/
/*!
* \brief
*     HSYNC Flip mode configuration
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psImmediateFlip     - Immediate flip configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_ImmediateFlipConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                             HAL_MDP_LayerType                eLayerType,
                                             HAL_MDP_ImmediateFlipConfigType  *psImmediateFlip,
                                             uint32                           uFlags)
{
   uintPtr  uRegisterOffset      = 0;
   uint32   uSrcAddrFetchConfig  = 0;

   // Only VIG, RGB and DMA layers support HSYNC flip
   if(HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)
   {
      if(((HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG & uFlags) && !psImmediateFlip->bHsyncFlipEnable) || (0x00 == uFlags))
      {
        // Extract the base address to program the HW registers
        uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

        // ADDR FETCH Configuration
        uSrcAddrFetchConfig  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset);
        uSrcAddrFetchConfig  = HWIO_OUT_FLD(uSrcAddrFetchConfig,
                                          MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG,
                                          ADDR_SYNC_MODE,
                                          (uint32)psImmediateFlip->bHsyncFlipEnable);

        out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset, uSrcAddrFetchConfig); // Src Fetch Configuration
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GpuMemoryBankBitConfig()
*/
/*!
* \brief
*     Gpu memory bank bit configuration
*
* \param [in]   eSourcePipe               - Source pipe id
* \param [in]   eLayerType                - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psGpuMemoryBankConfig     - Gpu highest memory bank bit config
* \param [in]   uFlags                    - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GpuMemoryBankBitConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                                HAL_MDP_LayerType                eLayerType,
                                                HAL_MDP_GpuMemoryBankConfigType  *psGpuMemoryBankConfig,
                                                uint32                           uFlags)
{
   uintPtr  uRegisterOffset          = 0;
   uint32   uSrcAddrFetchConfig      = 0;
   uint32   uGpuHighestMemoryBankBit = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   // ADDR FETCH Configuration
   uSrcAddrFetchConfig  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset);
   uGpuHighestMemoryBankBit = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG, uSrcAddrFetchConfig, HIGHEST_BANK_BIT);
   if(uGpuHighestMemoryBankBit != psGpuMemoryBankConfig->uGpuHighestMemoryBankBit)
   {
     uSrcAddrFetchConfig  = HWIO_OUT_FLD(uSrcAddrFetchConfig,
                                         MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG,
                                         HIGHEST_BANK_BIT,
                                         psGpuMemoryBankConfig->uGpuHighestMemoryBankBit);
   }

   out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset, uSrcAddrFetchConfig); // Src Fetch Configuration
}



/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_VC1RangeLevelConfig()
*/
/*!
* \brief
*     VC1 range level mapping configuration
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psVC1RangeLevel     - VC1 range level configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_VC1RangeLevelConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                             HAL_MDP_LayerType                eLayerType,
                                             HAL_MDP_VC1RangeLevelConfigType  *psVC1RangeLevel,
                                             uint32                           uFlags)
{
   uintPtr  uRegisterOffset         = 0;
   uint32   uSrcCurrentFormatInfo   = 0;
   bool32   bVC1RangeMapEnable      = TRUE;
   uint32   uVC1RangeMapReg         = 0x00;

   if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)     // Only ViG Layer supports VC1 range level mapping
   {
      // Extract the base address to program the HW registers
      uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

      if((HAL_MDP_VC1_RANGE_LEVEL_NONE == psVC1RangeLevel->eRangeMapY)   && 
         (HAL_MDP_VC1_RANGE_LEVEL_NONE == psVC1RangeLevel->eRangeMapUV)  )
      {
         bVC1RangeMapEnable = FALSE;        // Disable if both Luma and Chroma are NONE
      }
      else
      {
         uVC1RangeMapReg   = HWIO_OUT_FLD(uVC1RangeMapReg,
                                          MDP_VP_0_VIG_0_SSPP_VC1_RANGE,
                                          RANGE_MAPY,
                                          ((uint32)psVC1RangeLevel->eRangeMapY - 1));     // Range is (0 to 7)

         uVC1RangeMapReg   = HWIO_OUT_FLD(uVC1RangeMapReg,
                                          MDP_VP_0_VIG_0_SSPP_VC1_RANGE,
                                          RANGE_MAPUV,
                                          ((uint32)psVC1RangeLevel->eRangeMapUV - 1));    // Range is (0 to 7)

         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_VC1_RANGE_ADDR + uRegisterOffset, uVC1RangeMapReg);
      }

      // VC1 range map enable/disable configuration
      uSrcCurrentFormatInfo  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
      uSrcCurrentFormatInfo  = HWIO_OUT_FLD(uSrcCurrentFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            VC1_REDUCE,
                                            (uint32)bVC1RangeMapEnable);

      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcCurrentFormatInfo);    // Src format information
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_FetchModeConfig()
*/
/*!
* \brief
*     Flip/Mirror mode configuration (Vertical or Horizontal mirror mode)
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psFetchInfo         - Fetch configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_FetchModeConfig(HAL_MDP_SourcePipeId        eSourcePipe,
                                         HAL_MDP_LayerType           eLayerType,
                                         HAL_MDP_FetchModeConfigType *psFetchInfo,
                                         uint32                      uFlags)
{
   uintPtr  uRegisterOffset      = 0;
   uint32   uSrcOpModeInfo       = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   uSrcOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);
   uSrcOpModeInfo &= (~HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_FLIP_MODE_BMSK);
   uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo,
                                  MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE,
                                  FLIP_MODE,
                                  psFetchInfo->eFetchConfigType);

   out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeInfo);  // Update fetch information
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_IGCConfig()
*/
/*!
* \brief
*     Enables or disables the IGC feature for the given pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psIGCInfo           - IGC enable/disable information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_IGCConfig(HAL_MDP_SourcePipeId      eSourcePipe,
                                   HAL_MDP_LayerType         eLayerType,
                                   HAL_MDP_IGCConfigType    *psIGCInfo,
                                   uint32                    uFlags)
{
   uintPtr                uRegisterOffset         = 0;
   uint32                 uSrcOpModeInfo          = 0;
   uint32                 uIndex                  = 0;
   uint32                 uLayerId                = 0;
   uint32                 uRegValue               = 0;
   uint32                 uColorIndex             = 0;
   uint32                *pIGCLutColor[3];                 // Color 2, 0 and 1
   uintPtr                uRegisterAddr[3]        = {0};                // Color 2, 0 and 1
   uint32                *pIGCLut                 = NULL;
   HAL_MDSS_ErrorType     eStatus                 = HAL_MDSS_STATUS_SUCCESS ;
   

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   /* Enable/disable inverse gamma correction tables */
   uSrcOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);
   uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, IGC_LUT_EN, (uint32)psIGCInfo->bIGCEnable);
   out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeInfo); 

   if (TRUE == psIGCInfo->bIGCEnable)
   {
      switch(eLayerType)
      {
#if 0 // Todo: Vipertooth      
         case HAL_MDP_LAYER_TYPE_RGB:

            uLayerId                = (eSourcePipe - HAL_MDP_SOURCE_PIPE_RGB_BASE);
            uRegisterAddr[0]        = HWIO_MMSS_MDP_RGB_IGC_COLOR2_RAM_LUTN_ADDR;  /* Red component - Color 2   */
            uRegisterAddr[1]        = HWIO_MMSS_MDP_RGB_IGC_COLOR0_RAM_LUTN_ADDR;  /* Green component - Color 0 */
            uRegisterAddr[2]        = HWIO_MMSS_MDP_RGB_IGC_COLOR1_RAM_LUTN_ADDR;  /* Blue component - Color 1  */
            break;
			*/

         case HAL_MDP_LAYER_TYPE_VIG:
            uLayerId                = (eSourcePipe - HAL_MDP_SOURCE_PIPE_VIG_BASE); 
            uRegisterAddr[0]        = HWIO_MMSS_MDP_VIG_IGC_COLOR2_RAM_LUTN_ADDR;  /* Red component - Color 2   */
            uRegisterAddr[1]        = HWIO_MMSS_MDP_VIG_IGC_COLOR0_RAM_LUTN_ADDR;  /* Green component - Color 0 */
            uRegisterAddr[2]        = HWIO_MMSS_MDP_VIG_IGC_COLOR1_RAM_LUTN_ADDR;  /* Blue component - Color 1  */
            break;

         case HAL_MDP_LAYER_TYPE_DMA:
            uLayerId                = (eSourcePipe - HAL_MDP_SOURCE_PIPE_DMA_BASE);
            uRegisterAddr[0]        = HWIO_MMSS_MDP_DMA_IGC_COLOR2_RAM_LUTN_ADDR;  /* Red component - Color 2   */ 
            uRegisterAddr[1]        = HWIO_MMSS_MDP_DMA_IGC_COLOR0_RAM_LUTN_ADDR;  /* Green component - Color 0 */
            uRegisterAddr[2]        = HWIO_MMSS_MDP_DMA_IGC_COLOR1_RAM_LUTN_ADDR;  /* Blue component - Color 1  */
            break;
#endif
		 case HAL_MDP_LAYER_TYPE_RGB:
			  uLayerId				  = (eSourcePipe - HAL_MDP_SOURCE_PIPE_RGB_BASE);
			  uRegisterAddr[0]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_RGB_IGC_COLOR2_RAM_LUTN_ADDR;  /* Red component	 - Color 2	 */
			  uRegisterAddr[1]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_RGB_IGC_COLOR0_RAM_LUTN_ADDR;  /* Green component - Color 0	 */
			  uRegisterAddr[2]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_RGB_IGC_COLOR1_RAM_LUTN_ADDR;  /* Blue component  - Color 1	 */
			  break;
		 
		  case HAL_MDP_LAYER_TYPE_VIG:
			  uLayerId				  = (eSourcePipe - HAL_MDP_SOURCE_PIPE_VIG_BASE);
			  uRegisterAddr[0]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_VIG_IGC_COLOR2_RAM_LUTN_ADDR;  /* Red component	 - Color 2	 */
			  uRegisterAddr[1]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_VIG_IGC_COLOR0_RAM_LUTN_ADDR;  /* Green component - Color 0	 */
			  uRegisterAddr[2]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_VIG_IGC_COLOR1_RAM_LUTN_ADDR;  /* Blue component  - Color 1	 */
			  break;
		 
		  case HAL_MDP_LAYER_TYPE_DMA:
			  uLayerId				  = (eSourcePipe - HAL_MDP_SOURCE_PIPE_DMA_BASE);
			  uRegisterAddr[0]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_DMA_IGC_COLOR2_RAM_LUTN_ADDR;  /* Red component	 - Color 2	 */
			  uRegisterAddr[1]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_DMA_IGC_COLOR0_RAM_LUTN_ADDR;  /* Green component - Color 0	 */
			  uRegisterAddr[2]		  = HWIO_MMSS_MDP_SSPP_IGC_LUT_DMA_IGC_COLOR1_RAM_LUTN_ADDR;  /* Blue component  - Color 1	 */
			  break;


         case HAL_MDP_LAYER_TYPE_CURSOR:
         case HAL_MDP_LAYER_TYPE_SSPP_CURSOR:
         case HAL_MDP_LAYER_TYPE_NONE:
         case HAL_MDP_LAYER_TYPE_MAX:
         case HAL_MDP_LAYER_TYPE_FORCE_32BIT:
         default:
            eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
         break;
      }

      if ((HAL_MDSS_STATUS_SUCCESS == eStatus) && (NULL != psIGCInfo->pBlueCb) && (NULL != psIGCInfo->pGreenY) && (NULL != psIGCInfo->pRedCr))
      {
         pIGCLutColor[0]         = psIGCInfo->pRedCr;
         pIGCLutColor[1]         = psIGCInfo->pGreenY;
         pIGCLutColor[2]         = psIGCInfo->pBlueCb;
#if 0 //TODO: Vipertooth check
         uRegValue   = HWIO_OUT_FLD(uRegValue,
                                    MDP_RGB_IGC_COLOR0_RAM_LUTN,
                                    RGB_MASK,
                                    (0xF & ~(1 << uLayerId)));

         // Disable auto index update
         uRegValue   = HWIO_OUT_FLD(uRegValue,
                                    MDP_RGB_IGC_COLOR0_RAM_LUTN,
                                    INDEX_UPDATE,
                                    0x01);
#endif
         uRegValue   = HWIO_OUT_FLD(uRegValue,
                                    MDP_SSPP_IGC_LUT_RGB_IGC_COLOR0_RAM_LUTN,
                                    RGB_MASK,
                                    (0xF & ~(1 << uLayerId)));

         // Disable auto index update
         uRegValue   = HWIO_OUT_FLD(uRegValue,
                                    MDP_SSPP_IGC_LUT_RGB_IGC_COLOR0_RAM_LUTN,
                                    INDEX_UPDATE,
                                    0x01);

         // IGC LUT
         for(uColorIndex = 0; uColorIndex < 3; uColorIndex++)
         {
            pIGCLut  = pIGCLutColor[uColorIndex];

            for(uIndex = 0; uIndex < psIGCInfo->uTableSize; uIndex++)
            {
#if 0
               // Update the table index
               uRegValue = HWIO_OUT_FLD(uRegValue,
                                        MDP_RGB_IGC_COLOR0_RAM_LUTN,
                                        INDEX,
                                        uIndex);

               // Update the value to write into the table
               uRegValue = HWIO_OUT_FLD(uRegValue,
                                        MDP_RGB_IGC_COLOR0_RAM_LUTN,
                                        VALUE,
                                        pIGCLut[uIndex]);

#endif
 			// Update the table index
 			  uRegValue = HWIO_OUT_FLD(uRegValue,
 									   MDP_SSPP_IGC_LUT_RGB_IGC_COLOR0_RAM_LUTN,
 									   INDEX,
 									   uIndex);
 			
 			  // Update the value to write into the table
 			  uRegValue = HWIO_OUT_FLD(uRegValue,
 									   MDP_SSPP_IGC_LUT_RGB_IGC_COLOR0_RAM_LUTN,
 									   VALUE,
 									   pIGCLut[uIndex]);


               out_dword(uRegisterAddr[uColorIndex], uRegValue);
            }
         }
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SharpenConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psSharpenInfo       - QSEED Sharpening Information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_SharpenConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                       HAL_MDP_LayerType            eLayerType,
                                       HAL_MDP_SharpenConfigType   *psSharpenInfo,
                                       uint32                       uFlags)
{
   int32                  iSharpeningLevel        = 0;
   uintPtr                uRegisterOffset         = 0;  
   uint32                 uSharpEdgeThresh        = 0;
   uint32                 uSmoothThresh           = 0;
   uint32                 uNoiseThresh            = 0; 
   HAL_MDSS_ErrorType     eStatus                 = HAL_MDSS_STATUS_SUCCESS;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe, eLayerType);

   switch (psSharpenInfo->eSharpeningMode)
   {
      case HAL_MDP_SHARPENING_TYPE_NONE:
      {
         // Default no sharpening/smoothening 
         iSharpeningLevel        =  HAL_MDP_DEFAULT_SHARPENING_LEVEL;
         uSharpEdgeThresh        =  HAL_MDP_DEFAULT_SHARPENING_SHARP_EDGE_THRESH;
         uSmoothThresh           =  HAL_MDP_DEFAULT_SHARPENING_SMOOTH_THRESH;
         uNoiseThresh            =  HAL_MDP_DEFAULT_SHARPENING_NOISE_THRESH;        

         break; 
      }

      case HAL_MDP_SHARPENING_TYPE_SHARPEN:
      { 
         int32 iNormalizeLevel   =  0;

         iNormalizeLevel         =  (255 * psSharpenInfo->uLevel) / 100; 

         // Content-adaptive sharpening 
         iSharpeningLevel        =  iNormalizeLevel;        
         uSharpEdgeThresh        =  HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_SHARP_EDGE_THRESH;
         uSmoothThresh           =  HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_SMOOTH_THRESH;
         uNoiseThresh            =  HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_NOISE_THRESH;    

         break; 
      }

      case HAL_MDP_SHARPENING_TYPE_SMOOTHEN:
      {
         int32 iNormalizeLevel   =  0;

         iNormalizeLevel         =  (256 * psSharpenInfo->uLevel) / 100; 

         // Smoothening 
         iSharpeningLevel        =  (-iNormalizeLevel);
         uSharpEdgeThresh        =  HAL_MDP_DEFAULT_SMOOTHENING_SHARP_EDGE_THRESH;
         uSmoothThresh           =  HAL_MDP_DEFAULT_SMOOTHENING_SMOOTH_THRESH;
         uNoiseThresh            =  HAL_MDP_DEFAULT_SMOOTHENING_NOISE_THRESH;    

         break; 
      }

      default:
         eStatus     = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
         break;
   }

   if (HAL_MDSS_STATUS_SUCCESS == eStatus) 
   {
      // Sharpen edge threshold 
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_SHARP_THRESHOLD_EDGE_ADDR + uRegisterOffset, uSharpEdgeThresh);   
      // Smooth threshold 
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_SHARP_THRESHOLD_SMOOTH_ADDR + uRegisterOffset, uSmoothThresh);   
      // Noise threshold 
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_SHARP_THRESHOLD_NOISE_ADDR + uRegisterOffset, uNoiseThresh);   
      // Sharpening levels (Smoothen : -256 to -1 ; Sharpen : 1 to 255; None : 0)
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_SHARP_SMOOTH_STRENGTH_ADDR + uRegisterOffset, iSharpeningLevel);               
   }

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_HistogramConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psHistogramInfo     - Histogram configuration information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_HistogramConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                         HAL_MDP_LayerType            eLayerType,
                                         HAL_MDP_HistogramConfigType *psHistogramInfo,
                                         uint32                       uFlags)
{

}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_Picture_Adjustment_Calculation
*/
/*!
* \brief
*     Calculates the actual picture adjustment value for the HSIC settings
*
* \param [in]   iPictAdjustValue        - Picture adjustment value between -100% to 100%
* \param [in]   iMinPictAdjustValue     - Minimum actual picture adjustment value 
* \param [in]   iMaxPictAdjustValue     - Maximum actual picture adjustment value 
* \param [in]   iMinUnchPictAdjustValue - Unchange value going to the direction of minimum picture adjustment value
* \param [in]   iMaxUnchPictAdjustValue - Unchange value going to the direction of maximum picture adjustment value
*
* \retval Actual picture adjustment value
*
****************************************************************************/
static int32 HAL_MDP_SSPP_Picture_Adjustment_Calculation(int32         iPictAdjustValue,
                                                         int32         iMinPictAdjustValue,
                                                         int32         iMaxPictAdjustValue,
                                                         int32         iMinUnchPictAdjustValue,
                                                         int32         iMaxUnchPictAdjustValue)
{
   int32 iValue = 0;

   if (iPictAdjustValue > 0)
   {
       iValue = ((iPictAdjustValue * (iMaxPictAdjustValue - iMaxUnchPictAdjustValue))/100) + iMaxUnchPictAdjustValue;
   }
   else
   {
       iValue = ((iPictAdjustValue * (iMinPictAdjustValue - iMinUnchPictAdjustValue))/-100) + iMinUnchPictAdjustValue;
   }

   return iValue; 
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_PictureAdjustmentConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psPictureAdjustInfo - SPA (Source Picture Adjust)/CE (Color Enhancement) information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_PictureAdjustmentConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                                 HAL_MDP_LayerType                eLayerType,
                                                 HAL_MDP_PictureAdjustConfigType  *psPictureAdjustInfo,
                                                 uint32                           uFlags)
{
   int32   iPictAdjValue    = 0;
   int32   iSatAdjVal       = 0;
   uint32  uPictAdjEnable   = 0;
   uintPtr uLayerRegOffset  = 0;
   
   /* Picture adjustment is done only for VG layers */
   if (HAL_MDP_LAYER_TYPE_VIG == eLayerType)
   {
      /* Extract the layer base address */
      uLayerRegOffset = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

      /* Enable/Disable the picture adjustment */
      uPictAdjEnable = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_ADDR + uLayerRegOffset);
      uPictAdjEnable = HWIO_OUT_FLD(uPictAdjEnable, MDP_VP_0_VIG_0_OP_MODE, PA_EN, psPictureAdjustInfo->bEnable);
      out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_OP_MODE_ADDR + uLayerRegOffset, uPictAdjEnable);

      /* Check if the picture adjustment info is enabled */
      if (TRUE == psPictureAdjustInfo->bEnable)
      {
         /* Hue */
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iHueValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinHue,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxHue,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinUnchangeHue,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxUnchangeHue);
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SPA_HUE_ADJUST_ADDR + uLayerRegOffset, iPictAdjValue);              

         /* Contrast */
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iContrastValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinContrast,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxContrast,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast);
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SPA_CONT_ADJUST_ADDR + uLayerRegOffset, iPictAdjValue);

         /* Saturation + Saturation threshold (default 26) */
         iSatAdjVal = in_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SPA_SAT_ADJUST_ADDR + uLayerRegOffset);
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iSaturationValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinSaturation,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxSaturation,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation);
         iSatAdjVal = HWIO_OUT_FLD(iSatAdjVal, MDP_VP_0_VIG_0_SPA_SAT_ADJUST, VALUE, iPictAdjValue);
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SPA_SAT_ADJUST_ADDR + uLayerRegOffset, iSatAdjVal);

         /* Intensity */
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iIntensityValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinIntensity,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxIntensity,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity);
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_SPA_VAL_ADJUST_ADDR + uLayerRegOffset, iPictAdjValue);
      }
   }
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_CSC_1_Init()
*/
/*!
* \brief
*     Initializes the CSC_1 (Color Space Connversion) module of given pipe
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] eColorEncodeType      - Color Encoding Type
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_CSC_1_Init(HAL_MDP_SourcePipeId    eSourcePipe,
                                    HAL_MDP_LayerType       eLayerType,
                                    HAL_MDP_ColorEncodeType eColorEncodeType,
                                    uint32                  uFlags)
{
   uintPtr        uLayerRegOffset   = 0x0;
   uint32        *pCSCMatrixCoeff   = NULL;
   uint16        *pCSCPreClamp      = NULL;
   uint16        *pCSCPostClamp     = NULL;
   uint16        *pCSCPreBias       = NULL;
   uint16        *pCSCPostBias      = NULL;

   HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_STATUS_SUCCESS;

   switch(eLayerType)
   {
   case HAL_MDP_LAYER_TYPE_VIG:

      // Extract the layer base address
      uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

      switch(eColorEncodeType)
      {
      case HAL_MDP_COLOR_ENCODING_DEFAULT:
      case HAL_MDP_COLOR_ENCODING_REC601:            /** ITU-R BT.601 (CCIR601)  */
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_PostBias;
         break;

      case HAL_MDP_COLOR_ENCODING_REC709:            /** ITU-R BT.709 (CCIR709)  */
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_PostBias;
         break;

      case HAL_MDP_COLOR_ENCODING_FULL_REC601:      /**< ITU.BT-601 SDTV         */   
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_PostBias;
         break;

      case HAL_MDP_COLOR_ENCODING_FULL_REC709:      /**< ITU.BT-709 HDTV         */ 
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_PostBias;
         break; 

      case HAL_MDP_COLOR_ENCODING_sRGB:              /** Standard RGB */     
      case HAL_MDP_COLOR_ENCODING_MAX:
      case HAL_MDP_COLOR_ENCODING_FORCE_32BIT:
      default:
         eStatus     = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
         break;
      }
      break;

   case HAL_MDP_LAYER_TYPE_RGB:
   case HAL_MDP_LAYER_TYPE_DMA:
   case HAL_MDP_LAYER_TYPE_CURSOR:
   case HAL_MDP_LAYER_TYPE_SSPP_CURSOR:
   case HAL_MDP_LAYER_TYPE_NONE:
   case HAL_MDP_LAYER_TYPE_MAX:
   case HAL_MDP_LAYER_TYPE_FORCE_32BIT:
   default:
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      if ((NULL != pCSCMatrixCoeff) && (NULL != pCSCPreClamp) &&
          (NULL != pCSCPostClamp)   && (NULL != pCSCPreBias ) && 
          (NULL != pCSCPostBias))
      {
         // Matrix Coefficients
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_0_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000000) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_1_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000001) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_2_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000002) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_3_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000003) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_4_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000004) );

         // Preclamp
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_0_PRE_CLAMP_ADDR + uLayerRegOffset, *(pCSCPreClamp + 0x00000000) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_1_PRE_CLAMP_ADDR + uLayerRegOffset, *(pCSCPreClamp + 0x00000001) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_2_PRE_CLAMP_ADDR + uLayerRegOffset, *(pCSCPreClamp + 0x00000002) );

         // Postclamp
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_0_POST_CLAMP_ADDR + uLayerRegOffset, *(pCSCPostClamp + 0x00000000) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_1_POST_CLAMP_ADDR + uLayerRegOffset, *(pCSCPostClamp + 0x00000001) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_2_POST_CLAMP_ADDR + uLayerRegOffset, *(pCSCPostClamp + 0x00000002) );

         // Prebias
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_0_PRE_BIAS_ADDR + uLayerRegOffset, *(pCSCPreBias + 0x00000000) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_1_PRE_BIAS_ADDR + uLayerRegOffset, *(pCSCPreBias + 0x00000001) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_2_PRE_BIAS_ADDR + uLayerRegOffset, *(pCSCPreBias + 0x00000002) );

         // Postbias
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_0_POST_BIAS_ADDR + uLayerRegOffset, *(pCSCPostBias + 0x00000000) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_1_POST_BIAS_ADDR + uLayerRegOffset, *(pCSCPostBias + 0x00000001) );
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_CSC_1_COMP_2_POST_BIAS_ADDR + uLayerRegOffset, *(pCSCPostBias + 0x00000002) );
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ColorEncodeConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in]   psColorEncInfo      - Color encoding information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_ColorEncodeConfig(HAL_MDP_SourcePipeId            eSourcePipe,
                                           HAL_MDP_LayerType               eLayerType,
                                           HAL_MDP_ColorEncodeConfigType  *psColorEncInfo,
                                           uint32                          uFlags)
{
   HAL_MDP_SSPP_CSC_1_Init(eSourcePipe,
                           eLayerType,
                           psColorEncInfo->eColorEncodeType,
                           0x00);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_QSEED_Init()
*/
/*!
* \brief
*     Initializes the QSEED Normal and Smooth Filter Coefficients of given pipe
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_QSEED_Init(HAL_MDP_SourcePipeId    eSourcePipe, 
                                    HAL_MDP_LayerType       eLayerType,
                                    uint32                  uFlags)
{
   uintPtr        uLayerRegOffset     = 0x0;
   uint32        *pQSEEDFilterCoeff   = NULL;
   uint32         uIndex              = 0;

   HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_STATUS_SUCCESS;

   switch(eLayerType)
   {
   case HAL_MDP_LAYER_TYPE_VIG:

      // Extract the layer base address
      uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);
      break;
   case HAL_MDP_LAYER_TYPE_RGB:
   case HAL_MDP_LAYER_TYPE_DMA:
   case HAL_MDP_LAYER_TYPE_CURSOR:
   case HAL_MDP_LAYER_TYPE_SSPP_CURSOR:
   case HAL_MDP_LAYER_TYPE_NONE:
   case HAL_MDP_LAYER_TYPE_MAX:
   case HAL_MDP_LAYER_TYPE_FORCE_32BIT:
   default:
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      // Normal Filter Coefficients
      pQSEEDFilterCoeff = pgsMdpHwInfo->pQSEEDNormFilterCoeff;

      for(uIndex = 0; uIndex < pgsMdpHwInfo->uQSEEDNormFilterCoeffCnt; uIndex++)
      {
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_NORMAL_FILTER_COEFF_N_ADDR + uLayerRegOffset, *(pQSEEDFilterCoeff + uIndex));
      }

      // Smooth Filter Coefficients
      pQSEEDFilterCoeff = pgsMdpHwInfo->pQSEEDSmoothFilterCoeff;

      for(uIndex = 0; uIndex < pgsMdpHwInfo->uQSEEDSmoothFilterCoeffCnt; uIndex++)
      {
         out_dword(HWIO_MMSS_MDP_VP_0_VIG_0_QSEED2_SMOOTH_FILTER_COEFF_N_ADDR + uLayerRegOffset, *(pQSEEDFilterCoeff + uIndex));
      }
   }
}


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
                       uint32                  uFlags)
{

   if(0x00 != (HAL_MDP_SSPP_FEATURE_IGC_LUT & uInitFlags))
   {
      /* To initialize the layer's inverse gamma tables with default inverse gamma values call 
         HAL_MDP_SSPP_IGCConfig() at this point with HAL_MDP_IGCConfigType structure populated  
         with the default inverse gamma table values. */


   }

   if(0x00 != (HAL_MDP_SSPP_FEATURE_CSC_1 & uInitFlags))
   {
      HAL_MDP_SSPP_CSC_1_Init(eSourcePipe, eLayerType, HAL_MDP_COLOR_ENCODING_REC601, uFlags);
   }

   if(0x00 != (HAL_MDP_SSPP_FEATURE_QSEED2 & uInitFlags))
   {
      HAL_MDP_SSPP_QSEED_Init(eSourcePipe, eLayerType, uFlags);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source Pipe ID
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in]   psSourcePipeConfig  - Configuration for the selected pipe
* \param [in]   uSetupFlags         - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SourcePipe_Setup(HAL_MDP_SourcePipeId            eSourcePipe,
                                            HAL_MDP_LayerType               eLayerType,
                                            HAL_MDP_SourcePipeConfigType   *psSourcePipeConfig,
                                            uint32                          uSetupFlags)
{
   HAL_MDSS_ErrorType         eStatus        = HAL_MDSS_STATUS_SUCCESS;
   HAL_MDP_CropRectangleType  sCropRectInfo  = {{0, 0, 0, 0}, {0, 0, 0, 0}};
   HAL_MDP_SurfaceAttrType    sSurfInfo;

   if(HAL_MDP_LAYER_TYPE_CURSOR == eLayerType)     // Override the the type with HAL driver internal cursor type
   {
      eLayerType  = pgsMdpHwInfo->eCursorIntLayerType;
   }

   /**If disable HsyncFlip is requested:then it has be done before SSPP_Source (HAL_MDP_SSPP_SurfaceConfig),otherwise it may lead to SMMU fault*/
   if(NULL != psSourcePipeConfig->psImmediateFlip)
   {
      HAL_MDP_SSPP_ImmediateFlipConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psImmediateFlip, HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG);
   }

   /** Surface Configuration */
   if(NULL != psSourcePipeConfig->psSurfaceInfo)
   {
      HAL_MDP_SSPP_SurfaceConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psSurfaceInfo, 0x00);
   }

   /** Cropping rectangle configuration */
   if(NULL != psSourcePipeConfig->psCropRectInfo)
   {
      HAL_MDP_SSPP_CropRectConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psCropRectInfo, 0x00);
   }

   // Scalar configuration
   if((NULL != psSourcePipeConfig->psSurfaceInfo) || (NULL != psSourcePipeConfig->psCropRectInfo))
   {
      // Extract both surface and cropping rectangle information
      HAL_MDP_SSPP_SurfaceInfo(eSourcePipe,
                               eLayerType,
                               &sSurfInfo,
                               &sCropRectInfo,
                               HAL_MDP_SOURCE_FLAGS_EXTRACT_ONLY_PIXEL_FORMAT);

      if ((NULL != psSourcePipeConfig->psPixelExtLayerConfig) && 
          (TRUE == pgsMdpHwInfo->bUseSWPixExt))
      {
         // Enable Pixel Extension Software Override, Setup PE, Scaler and Decimator 
         HAL_MDP_PixelExtensionConfig(eSourcePipe,
                                      eLayerType,
                                      psSourcePipeConfig->psDecimationConfig,
                                      psSourcePipeConfig->psPixelExtLayerConfig);
      }
      else
      {
         // Disable Pixel Extension Software Override, Setup the Scaler and Decimator
         HAL_MDP_ScalarConfig(eSourcePipe,
                              eLayerType,
                              sSurfInfo.ePixelFormat,
                              &sCropRectInfo,
                              psSourcePipeConfig->psDecimationConfig,
                              psSourcePipeConfig->psDeinterlaceConfig,
                              uSetupFlags);
      }
   }

   /** Color fill configuration */
   if(NULL != psSourcePipeConfig->psColorFillInfo)
   {
      HAL_MDP_SSPP_ColorfillConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psColorFillInfo, 0x00);
   }

   /** CSC configuration */
   if(NULL != psSourcePipeConfig->psColorEncInfo)
   {
      HAL_MDP_SSPP_ColorEncodeConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psColorEncInfo, 0x00);
   }

   /** Fetch configuration */
   if(NULL != psSourcePipeConfig->psFetchInfo)
   {
      HAL_MDP_SSPP_FetchModeConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psFetchInfo, 0x00);
   }

   /** IGC configuration */
   if(NULL != psSourcePipeConfig->psIGCInfo)
   {
      HAL_MDP_SSPP_IGCConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psIGCInfo, 0x00);
   }

   /** Sharpening Mode Configuration */
   if(NULL != psSourcePipeConfig->psSharpenInfo)
   {
      HAL_MDP_SSPP_SharpenConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psSharpenInfo, 0x00);
   }

   /** Sharpening Mode Configuration */
   if(NULL != psSourcePipeConfig->psHistogramConfig)
   {
      HAL_MDP_SSPP_HistogramConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psHistogramConfig, 0x00);
   }

   /** Picture Adjust Configuration */
   if(NULL != psSourcePipeConfig->psPictureAdjust)
   {
      HAL_MDP_SSPP_PictureAdjustmentConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psPictureAdjust, 0x00);
   }

   /** Immediate Flip Configuration: If you need to enable the HsyncFlip: Do it after you config the SSPP_Source (HAL_MDP_SSPP_SurfaceConfig) */
   if(NULL != psSourcePipeConfig->psImmediateFlip)
   {
      HAL_MDP_SSPP_ImmediateFlipConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psImmediateFlip, 0x00);
   }

   /** VC1 Range Level Mapping Configuration */
   if(NULL != psSourcePipeConfig->psVC1RangeInfo)
   {
      HAL_MDP_SSPP_VC1RangeLevelConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psVC1RangeInfo, 0x00);
   }

   /** Gpu highest memory bank bit configuration */
   if(NULL != psSourcePipeConfig->psGpuMemoryBankConfig)
   {
     HAL_MDP_SSPP_GpuMemoryBankBitConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psGpuMemoryBankConfig, 0x00);
   }
   

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_GetProperty()
*/
/*!
* \brief
*     Returns MDP source pipe property
*
* \param [in]   eSourcePipe         - Source Pipe ID
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [out]  psSourcePipeInfo    - Source pipe information
* \param [in]   uFlags              - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SourcePipe_GetProperty(HAL_MDP_SourcePipeId          eSourcePipe,
                                                  HAL_MDP_LayerType             eLayerType,
                                                  HAL_MDP_SourcePipeInfoType    *psSourcePipeInfo,
                                                  uint32                        uFlags)
{
   HAL_MDSS_ErrorType      eStatus = HAL_MDSS_STATUS_SUCCESS;

   if(HAL_MDP_LAYER_TYPE_CURSOR == eLayerType)     // Override the the type with HAL driver internal cursor type
   {
      eLayerType  = pgsMdpHwInfo->eCursorIntLayerType;
   }

   if(NULL != psSourcePipeInfo->psSurfaceInfo)
   {
      HAL_MDP_SSPP_SurfaceInfo(eSourcePipe, eLayerType, psSourcePipeInfo->psSurfaceInfo, NULL, 0x00);
   }

   // Module state
   if(NULL != psSourcePipeInfo->pbActive)
   {
      HAL_MDP_SSPP_GetStateInfo(eSourcePipe, eLayerType, psSourcePipeInfo->pbActive, 0x00);
   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif
