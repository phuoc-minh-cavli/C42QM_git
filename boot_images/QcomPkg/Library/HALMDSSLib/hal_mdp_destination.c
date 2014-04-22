/*! \file */

/*
===========================================================================

FILE:         hal_mdp_destination.c

DESCRIPTION:  Manages the Destination Pipes of MDP (DSPP and Write Back)

===========================================================================
Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.
All Rights Reserved.
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

#define HAL_MDP_DSPP_PICTURE_ADJUSTMENT_SKIN            0x00000001
#define HAL_MDP_DSPP_PICTURE_ADJUSTMENT_SKY             0x00000002
#define HAL_MDP_DSPP_PICTURE_ADJUSTMENT_FOLIAGE         0x00000003


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
uintPtr HAL_MDP_Get_DSPP_BaseOffset(HAL_MDP_DestinationPipeId  eDestPipe)
{
  uintPtr      uRegOffset        = 0;

  if((HAL_MDP_DESTINATION_PIPE_NONE   == eDestPipe) ||
     (HAL_MDP_DESTINATION_PIPE_MAX    <= eDestPipe))
  {
    HAL_MDP_ASSERT();
  }
  else
  {
    uRegOffset = uMDPDSPPRegBaseOffset[eDestPipe];
  }
  return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_IGCConfig()
*/
/*!
* \brief
*     Configurs DSPP IGC module
*
* \param [in]   eDestPipe      - Destination pipe id
* \param [in]   psIGCConfig    - IGC Configuration information
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_IGCConfig(HAL_MDP_DestinationPipeId  eDestPipe,
                                   HAL_MDP_IGCConfigType      *psIGCConfig)
{
  uintPtr  uRegisterOffset         = 0;
  uint32   uDsppOpModeInfo         = 0;
  uint32   uIndex                  = 0;
  uint32   uRegValue               = 0;
  uint32   uColorIndex             = 0;
  uint32   *pIGCLut                = NULL;
  uint32   *pIGCLutColor[3];   // Color 2, 0 and 1
  uintPtr   uRegisterAddr[3];  // Color 2, 0 and 1
  
  // Extract the base address to program the HW registers
  uRegisterOffset   = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);

  /* Enable/Disable inverse gamma correction */
  uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
  uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo, MDP_VP_0_DSPP_0_OP_MODE, IGC_LUT_EN, (uint32)psIGCConfig->bIGCEnable);
  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);

  /* Setup the IGC tables */
  if ((NULL != psIGCConfig->pBlueCb) && (NULL != psIGCConfig->pGreenY) && (NULL != psIGCConfig->pRedCr))
  {
    uRegisterAddr[0]        = HWIO_MMSS_MDP_DSPP_IGC_COLOR2_RAM_LUTN_ADDR; /* Red component - Color 2 */ 
    uRegisterAddr[1]        = HWIO_MMSS_MDP_DSPP_IGC_COLOR0_RAM_LUTN_ADDR; /* Green component - Color 0 */
    uRegisterAddr[2]        = HWIO_MMSS_MDP_DSPP_IGC_COLOR1_RAM_LUTN_ADDR; /* Blue component - Color 1 */

    pIGCLutColor[0]         = psIGCConfig->pRedCr;
    pIGCLutColor[1]         = psIGCConfig->pGreenY;
    pIGCLutColor[2]         = psIGCConfig->pBlueCb;

    if ((uint32)eDestPipe > HAL_MDP_GET_DSPP_COUNT())
    {
      HAL_MDP_ASSERT();
    }
    else
    {
      uRegValue   = HWIO_OUT_FLD(uRegValue,
                                 MDP_DSPP_IGC_COLOR0_RAM_LUTN,
                                 DSPP_MASK,
                                 (0xF & ~(1 << (uint32)(eDestPipe - 1)))); /* eDestPipeId starts from 1 and DSPP ID selection for LUT starts from 0 */    

      // Disable auto index update
      uRegValue   = HWIO_OUT_FLD(uRegValue,
                                 MDP_DSPP_IGC_COLOR0_RAM_LUTN,
                                 INDEX_UPDATE,
                                 0x01);

      // IGC LUT
      for (uColorIndex = 0; uColorIndex < 3; uColorIndex++)
      {
        pIGCLut  = pIGCLutColor[uColorIndex];

        for (uIndex = 0; uIndex < psIGCConfig->uTableSize; uIndex++)
        {
          // Update the table index
          uRegValue = HWIO_OUT_FLD(uRegValue,
                                   MDP_DSPP_IGC_COLOR0_RAM_LUTN,
                                   INDEX,
                                   uIndex);

          // Update the value to write into the table
          uRegValue = HWIO_OUT_FLD(uRegValue,
                                   MDP_DSPP_IGC_COLOR0_RAM_LUTN,
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
** FUNCTION: HAL_MDP_DSPP_DitherConfig()
*/
/*!
* \brief
*     Set up configurations for DSPP dither module
*
* \param [in]   eDestPipe         - Destination pipe id
* \param [in]   psDitherConfig    - Dither configuration information
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_DitherConfig(HAL_MDP_DestinationPipeId      eDestPipe,
                                      HAL_MDP_DSPP_DitherConfigType  *psDitherConfig)
{
   uintPtr  uRegisterOffset   = 0;
   uint32   uDsppOpModeInfo   = 0;
   uint32   uTempRegInfo      = 0;
   uint32   uDitherBitDepth   = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset  = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);

   uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);

   // Dither is only for RGB destination with lower color depth than RGB888
   if ((TRUE == psDitherConfig->bEnable) && (TRUE == HAL_MDP_IS_MDP_DITHERING_PIXEL_FORMAT(psDitherConfig->eDstPixelFormat))) 
   {
      uTempRegInfo = guSrcFormatInfo[psDitherConfig->eDstPixelFormat];

      uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
                                     MDP_VP_0_DSPP_0_DITHER_BITDEPTH,
                                     C0_BITS,
                                     HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uTempRegInfo, SRCC0));

      uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
                                     MDP_VP_0_DSPP_0_DITHER_BITDEPTH,
                                     C1_BITS,
                                     HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uTempRegInfo, SRCC1));

      uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
                                     MDP_VP_0_DSPP_0_DITHER_BITDEPTH,
                                     C2_BITS,
                                     HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uTempRegInfo, SRCC2));

      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_DITHER_BITDEPTH_ADDR + uRegisterOffset, uDitherBitDepth);

      // Dither matrix
      if (NULL != psDitherConfig->pDitherMatrix)
      {
         out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_DITHER_MATRIX_ROW0_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow0Value);    // ROW#0
         out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_DITHER_MATRIX_ROW1_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow1Value);    // ROW#1
         out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_DITHER_MATRIX_ROW2_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow2Value);    // ROW#2
         out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_DITHER_MATRIX_ROW3_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow3Value);    // ROW#3
      }

      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                     MDP_VP_0_DSPP_0_OP_MODE,
                                     DST_DITHER_EN,
                                     (uint32)TRUE);
   }
   else
   {
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                     MDP_VP_0_DSPP_0_OP_MODE,
                                     DST_DITHER_EN,
                                     (uint32)FALSE);
   }

   out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_GamutMapConfig()
*/
/*!
* \brief
*     Set up configurations for Gamut Mappng Module
*
* \param [in]   eDestPipe         - Destination pipe id
* \param [in]   psGamutMapConfig  - Gamut mapping configuration information
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_GamutMapConfig(HAL_MDP_DestinationPipeId       eDestPipe,
                                        HAL_MDP_GamutMappingConfigType  *psGamutMapConfig)
{
#if 0  // TODO: Vipertooth: Recheck the registers

  uintPtr     uRegisterOffset	= 0;
  uint32      uDsppOpModeInfo	= 0;
  uint32      uCompCntr         = 0;
  uint32      uTableCntr        = 0;
  uint32      uEntryCntr        = 0;
  uint32      uGMTableSizes[ ]  = { HAL_MDP_GM_TABLE_0_ENTRIES,
                                    HAL_MDP_GM_TABLE_1_ENTRIES,
                                    HAL_MDP_GM_TABLE_2_ENTRIES,
                                    HAL_MDP_GM_TABLE_3_ENTRIES,
                                    HAL_MDP_GM_TABLE_4_ENTRIES,
                                    HAL_MDP_GM_TABLE_5_ENTRIES,
                                    HAL_MDP_GM_TABLE_6_ENTRIES,
                                    HAL_MDP_GM_TABLE_7_ENTRIES };
  uint16  *pPGMRegisterConfig[HAL_MDP_GM_MAX_COMPONENTS][HAL_MDP_GM_MAX_COMPONENT_TABLES]; 

  // Extract the base address to program the HW registers
  uRegisterOffset	= HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);
  
  if (TRUE == psGamutMapConfig->bGamutEn)
  {

    // Red component gamut mapping tables register addresses
    pPGMRegisterConfig[0][0] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_0_N_ADDR);
    pPGMRegisterConfig[0][1] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_1_N_ADDR);
    pPGMRegisterConfig[0][2] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_2_N_ADDR);
    pPGMRegisterConfig[0][3] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_3_N_ADDR);
    pPGMRegisterConfig[0][4] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_4_N_ADDR);
    pPGMRegisterConfig[0][5] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_5_N_ADDR);
    pPGMRegisterConfig[0][6] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_6_N_ADDR);
    pPGMRegisterConfig[0][7] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_RED_TABLE_7_N_ADDR);
    
	// Green component gamut mapping tables register addresses
    pPGMRegisterConfig[1][0] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_0_N_ADDR);
    pPGMRegisterConfig[1][1] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_1_N_ADDR);
    pPGMRegisterConfig[1][2] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_2_N_ADDR);
    pPGMRegisterConfig[1][3] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_3_N_ADDR);
    pPGMRegisterConfig[1][4] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_4_N_ADDR);
    pPGMRegisterConfig[1][5] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_5_N_ADDR);
    pPGMRegisterConfig[1][6] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_6_N_ADDR);
    pPGMRegisterConfig[1][7] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_GREEN_TABLE_7_N_ADDR);

	// Blue component gamut mapping tables register addresses
    pPGMRegisterConfig[2][0] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_0_N_ADDR);
    pPGMRegisterConfig[2][1] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_1_N_ADDR);
    pPGMRegisterConfig[2][2] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_2_N_ADDR);
    pPGMRegisterConfig[2][3] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_3_N_ADDR);
    pPGMRegisterConfig[2][4] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_4_N_ADDR);
    pPGMRegisterConfig[2][5] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_5_N_ADDR);
    pPGMRegisterConfig[2][6] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_6_N_ADDR);
    pPGMRegisterConfig[2][7] = (uint16 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_GAMUT_BLUE_TABLE_7_N_ADDR);

	/** Loop for color components R G B*/
    for (uCompCntr = 0; uCompCntr < HAL_MDP_GM_MAX_COMPONENTS; uCompCntr++)
    {
      /** Loop For Number of Tables <0-7>*/
      for (uTableCntr = 0; uTableCntr < HAL_MDP_GM_MAX_COMPONENT_TABLES; uTableCntr++)
      {
        /** Loop for Number of coefficients in the table*/
        for (uEntryCntr = 0; uEntryCntr < uGMTableSizes[uTableCntr]; uEntryCntr++)
        {
          out_dword(pPGMRegisterConfig[uCompCntr][uTableCntr], *(psGamutMapConfig->pGammutTableEntries[uCompCntr][uTableCntr] + uEntryCntr));
        }
      }
    }
  }
  // Enable gamut mapping after programming the coeffs.      Disable, no coeffs will be programmed.
  uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
  uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo, MDP_VP_0_DSPP_0_OP_MODE, GAMUT_EN,(uint32)psGamutMapConfig->bGamutEn);
  // Order of gamut mapping operation(either before or after polynomial color correction)
  uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo, MDP_VP_0_DSPP_0_OP_MODE, GAMUT_PCC_ORDER,(uint32)psGamutMapConfig->bGamutBeforePCC);
  
  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);
  #endif 
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_Picture_Adjustment_Calculation
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
static int32 HAL_MDP_DSPP_Picture_Adjustment_Calculation(int32         iPictAdjustValue,
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
** FUNCTION: HAL_MDP_DSPP_PictureAdjustment_SixZone
*/
/*!
* \brief
*     Six-zone color adjustment
*
* \param [in]   eDestPipe           - Destination pipe ID
* \param [in]   pConfig             - Six-zone color adjustment configuration info
* \param [in]   uFlags              - Reserved
*
* \retval none
*
****************************************************************************/
//static void HAL_MDP_DSPP_PictureAdjustment_SixZone(HAL_MDP_DestinationPipeId     eDestPipe,
//                                                   HAL_MDP_PA_SixZoneConfigType  *pConfig,
//                                                   uint32                        uFlags)
//{
//  uint32           uRegisterOffset = 0;        // register offset for the destination pipe
//  uint32           uRegValue       = 0;
//  uint32           uEnableMask     = 0;
//  uint32           uLutIndex       = 0;
//
//  /* Extract the layer base address */
//  uRegisterOffset = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);
//
//  // Read the op_mode for enable/disable
//  uEnableMask = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
//
//  // Program the thresholds
//  uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SIX_ZONE_THRESHOLDS, SAT_MIN, pConfig->uSaturationMin);
//  uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SIX_ZONE_THRESHOLDS, VAL_MIN, pConfig->uValueMin);
//  uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SIX_ZONE_THRESHOLDS, VAL_MAX, pConfig->uValueMax);
//  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_SIX_ZONE_THRESHOLDS_ADDR + uRegisterOffset, uRegValue);
//
//  // Move the LUT index to 0
//  uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P0, INDEX, 0);
//  uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P0, INDEX_UPDATE, 1);
//  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P0_ADDR + uRegisterOffset, uRegValue);
//
//  // Program the LUT
//  for(uLutIndex=0; uLutIndex<HAL_MDPP_SIX_ZONE_LUT_ENTRIES; uLutIndex++)
//  {
//    uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P1, VALUE, (pConfig->sPwl)[uLutIndex].iValue);
//    uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P1, SAT, (pConfig->sPwl)[uLutIndex].iSaturation);
//    out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P1_ADDR + uRegisterOffset, uRegValue);
//
//    uRegValue = HWIO_OUT_FLD(0, MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P0, HUE, (pConfig->sPwl)[uLutIndex].iHue);
//    out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_SIX_ZONE_ADJ_CURVE_P0_ADDR + uRegisterOffset, uRegValue);
//  }
//
//  // Enable/disable six-zone adjustment
//  uEnableMask = HWIO_OUT_FLD(uEnableMask, MDP_VP_0_DSPP_0_OP_MODE, SIX_ZONE_HUE_MASK, pConfig->bHueEnable);
//  uEnableMask = HWIO_OUT_FLD(uEnableMask, MDP_VP_0_DSPP_0_OP_MODE, SIX_ZONE_SAT_MASK, pConfig->bSaturationEnable);
//  uEnableMask = HWIO_OUT_FLD(uEnableMask, MDP_VP_0_DSPP_0_OP_MODE, SIX_ZONE_VAL_MASK, pConfig->bValueEnable);
//  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uEnableMask);
//}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_PictureAdjustment_MemoryColor
*/
/*!
* \brief
*     Tone adjustment for specific color: skin, sky or foliage
*
* \param [in]   eDestPipe           - Destination pipe ID
* \param [in]   pMemoryColorConfig  - Memory color configuration info
* \param [in]   uColorComponent     - color to be adjusted (skin, sky, foliage)
* \param [in]   uFlags              - Reserved
*
* \retval none
*
****************************************************************************/
//static void HAL_MDP_DSPP_PictureAdjustment_MemoryColor(HAL_MDP_DestinationPipeId        eDestPipe,
//                                                       HAL_MDP_PA_MemoryColorConfigType *pMemoryColorConfig,
//                                                       uint32                           uColorComponent,
//                                                       uint32                           uFlags)
//{
//  uint32           uRegisterOffset = 0;        // register offset for the destination pipe
//  uint32           uEnableMask     = 0;        // bit-mask to enable/disable tone adjustment
//  uint32           uRegRegionHue   = 0;        // register for hue region definition
//  uint32           uRegRegionSat   = 0;        // register for saturation region definition
//  uint32           uRegRegionVal   = 0;        // register for value region definition
//  uint32           uRegAdjustPwl0  = 0;        // register for PWL 0
//  uint32           uRegAdjustPwl1  = 0;        // register for PWL 1
//  uint32           uRegValue       = 0;
//
//  /* Extract the layer base address */
//  uRegisterOffset = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);
//
//  // Read the op_mode for enable/disable
//  uEnableMask = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
//
//  switch(uColorComponent)
//  {
//  case HAL_MDP_DSPP_PICTURE_ADJUSTMENT_SKIN:
//    {
//      // Get all registers related to skin color adjustment
//      uRegRegionHue  = HWIO_MMSS_MDP_VP_0_DSPP_0_SKIN_REGION_HUE_ADDR + uRegisterOffset;
//      uRegRegionSat  = HWIO_MMSS_MDP_VP_0_DSPP_0_SKIN_REGION_SAT_ADDR + uRegisterOffset;
//      uRegRegionVal  = HWIO_MMSS_MDP_VP_0_DSPP_0_SKIN_REGION_VAL_ADDR + uRegisterOffset;
//      uRegAdjustPwl0 = HWIO_MMSS_MDP_VP_0_DSPP_0_SKIN_ADJUST_PWL0_ADDR + uRegisterOffset;
//      uRegAdjustPwl1 = HWIO_MMSS_MDP_VP_0_DSPP_0_SKIN_ADJUST_PWL1_ADDR + uRegisterOffset;
//      uEnableMask    = HWIO_OUT_FLD(uEnableMask, 
//                                    MDP_VP_0_DSPP_0_OP_MODE, 
//                                    PA_MEM_COL_SKIN_MASK, 
//                                    pMemoryColorConfig->bEnable);
//    }
//    break;
//
//  case HAL_MDP_DSPP_PICTURE_ADJUSTMENT_SKY:
//    {
//      // Get all registers related to sky color adjustment
//      uRegRegionHue  = HWIO_MMSS_MDP_VP_0_DSPP_0_SKY_REGION_HUE_ADDR + uRegisterOffset;
//      uRegRegionSat  = HWIO_MMSS_MDP_VP_0_DSPP_0_SKY_REGION_SAT_ADDR + uRegisterOffset;
//      uRegRegionVal  = HWIO_MMSS_MDP_VP_0_DSPP_0_SKY_REGION_VAL_ADDR + uRegisterOffset;
//      uRegAdjustPwl0 = HWIO_MMSS_MDP_VP_0_DSPP_0_SKY_ADJUST_PWL0_ADDR + uRegisterOffset;
//      uRegAdjustPwl1 = HWIO_MMSS_MDP_VP_0_DSPP_0_SKY_ADJUST_PWL1_ADDR + uRegisterOffset;
//      uEnableMask    = HWIO_OUT_FLD(uEnableMask, 
//                                    MDP_VP_0_DSPP_0_OP_MODE, 
//                                    PA_MEM_COL_SKY_MASK, 
//                                    pMemoryColorConfig->bEnable);
//    }
//    break;
//
//  case HAL_MDP_DSPP_PICTURE_ADJUSTMENT_FOLIAGE:
//    {
//      // Get all registers related to foliage color adjustment
//      uRegRegionHue  = HWIO_MMSS_MDP_VP_0_DSPP_0_FOLIAGE_REGION_HUE_ADDR + uRegisterOffset;
//      uRegRegionSat  = HWIO_MMSS_MDP_VP_0_DSPP_0_FOLIAGE_REGION_SAT_ADDR + uRegisterOffset;
//      uRegRegionVal  = HWIO_MMSS_MDP_VP_0_DSPP_0_FOLIAGE_REGION_VAL_ADDR + uRegisterOffset;
//      uRegAdjustPwl0 = HWIO_MMSS_MDP_VP_0_DSPP_0_FOLIAGE_ADJUST_PWL0_ADDR + uRegisterOffset;
//      uRegAdjustPwl1 = HWIO_MMSS_MDP_VP_0_DSPP_0_FOLIAGE_ADJUST_PWL1_ADDR + uRegisterOffset;
//      uEnableMask    = HWIO_OUT_FLD(uEnableMask, 
//                                    MDP_VP_0_DSPP_0_OP_MODE, 
//                                    PA_MEM_COL_FOL_MASK, 
//                                    pMemoryColorConfig->bEnable);
//    }
//    break;
//
//  default:
//    break;
//  }
//
//  if(0 != uRegRegionHue)
//  {
//    if(TRUE == pMemoryColorConfig->bEnable)
//    {
//      // program region for HUE
//      uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SKIN_REGION_HUE, MIN, pMemoryColorConfig->uHueMin);
//      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SKIN_REGION_HUE, MAX, pMemoryColorConfig->uHueMax);
//      out_dword(uRegRegionHue, uRegValue);
//
//      // program region for SAT
//      uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SKIN_REGION_SAT, MIN, pMemoryColorConfig->uSaturationMin);
//      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SKIN_REGION_SAT, MAX, pMemoryColorConfig->uSaturationMax);
//      out_dword(uRegRegionSat, uRegValue);
//
//      // program region for VAL
//      uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SKIN_REGION_VAL, MIN, pMemoryColorConfig->uValueMin);
//      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SKIN_REGION_VAL, MAX, pMemoryColorConfig->uValueMax);
//      out_dword(uRegRegionVal, uRegValue);
//
//      // program PWL_0
//      uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SKIN_ADJUST_PWL0, HUE_MID,  pMemoryColorConfig->uHueMid);
//      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SKIN_ADJUST_PWL0, HUE_GAIN, pMemoryColorConfig->iHueGain);
//      out_dword(uRegAdjustPwl0, uRegValue);
//      
//      // program PWL_1
//      uRegValue = HWIO_OUT_FLD(0,         MDP_VP_0_DSPP_0_SKIN_ADJUST_PWL1, SAT_GAIN, pMemoryColorConfig->iSaturationGain);
//      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_VP_0_DSPP_0_SKIN_ADJUST_PWL1, VAL_GAIN, pMemoryColorConfig->iValueGain);
//      out_dword(uRegAdjustPwl1, uRegValue);
//    }
//
//    // enable/disable the color tone adjustment 
//    out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uEnableMask);
//  }
//}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_PictureAdjustmentConfig()
*/
/*!
* \brief
*     Set up configurations for DSPP picture adjustment generation
*
* \param [in]   eDestPipe              - Destination pipe id
* \param [in]   psPictureAdjustInfo    - Picture adjustment configuration info
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_PictureAdjustmentConfig(HAL_MDP_DestinationPipeId         eDestPipe,
                                                 HAL_MDP_PictureAdjustConfigType   *psPictureAdjustInfo)
{
   int32 iPictAdjValue     = 0;
   int32  iSatAdjVal       = 0;
   uint32 uPictAdjEnable   = 0;
  uintPtr uRegisterOffset  = 0;

   /* Extract the layer base address */
   uRegisterOffset = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);

   /* Enable/Disable the picture adjustment */
   uPictAdjEnable = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
   uPictAdjEnable = HWIO_OUT_FLD(uPictAdjEnable, MDP_VP_0_DSPP_0_OP_MODE, PA_EN, psPictureAdjustInfo->bEnable);
   out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uPictAdjEnable);

   /* Check if the picture adjustment info is enabled */
   if (TRUE == psPictureAdjustInfo->bEnable)
   {
      /* Hue */
      iPictAdjValue = HAL_MDP_DSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iHueValue, 
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMinHue,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMaxHue,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMinUnchangeHue,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMaxUnchangeHue);			
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_HUE_ADJUST_ADDR + uRegisterOffset, iPictAdjValue);              

      /* Contrast */
      iPictAdjValue = HAL_MDP_DSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iContrastValue, 
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMinContrast,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMaxContrast,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast);	
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_CONT_ADJUST_ADDR + uRegisterOffset, iPictAdjValue);

      /* Saturation + Saturation threshold (default 26) */
      iSatAdjVal = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_SAT_ADJUST_ADDR + uRegisterOffset);
      iPictAdjValue = HAL_MDP_DSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iSaturationValue, 
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMinSaturation,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMaxSaturation,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation);	
      iSatAdjVal = HWIO_OUT_FLD(iSatAdjVal, MDP_VP_0_DSPP_0_PA_SAT_ADJUST, VALUE, iPictAdjValue);
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_SAT_ADJUST_ADDR + uRegisterOffset, iSatAdjVal);

      /* Intensity */
      iPictAdjValue = HAL_MDP_DSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iIntensityValue, 
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMinIntensity,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iMaxIntensity,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity,
                                                                  pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity);
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_VAL_ADJUST_ADDR + uRegisterOffset, iPictAdjValue);			
   }
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramConfig()
*/
/*!
* \brief
*     Set up configurations for DSPP Histogram generation
*
* \param [in]   eDestPipe         - Destination pipe id
* \param [in]   psHistConfig      - Histogram Configuration information
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_HistogramConfig(HAL_MDP_DestinationPipeId     eDestPipe,
                                         HAL_MDP_HistogramConfigType   *psHistConfig)
{
   uintPtr uRegisterOffset  = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);

   // IP function call based on MDP version
   if(NULL != pgsMdpHwInfo->sIpFxnTbl.HistogramConfig)
   {
      pgsMdpHwInfo->sIpFxnTbl.HistogramConfig(uRegisterOffset, psHistConfig);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_AreaReducedGCConfig()
*/
/*!
* \brief
*     Configures the AR GC Tables and enable/disables the GC feature
*
* \param [in]   eDestPipe       - Destination pipe id
* \param [in]   psGCConfig      - Gamma correction configuration
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_AreaReducedGCConfig(HAL_MDP_DestinationPipeId        eDestPipe,
                                             HAL_MDP_GammaCorrectConfigType   *psGCConfig)
{
#if 0 // TODO: Vipertooth, check this again
  uintPtr                        uRegisterOffset   = 0;
   uint32                        uDsppOpModeInfo   = 0;
   uint32                        uIndex            = 0;
   uint32                        uComponentIndex   = 0;
   uint32                        uRegValue         = 0;
   HAL_MDP_AreaReductionLutType  *pARGC_Color      = NULL;
   HAL_MDP_AreaReductionLutType  *pAR_Color[3];     // Color 2, 0 and 1
  uintPtr                        uRegisterAddr[3];  // Color 2, 0 and 1

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);

   // Enable/disable panel gamma correction 
   uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
   uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo, MDP_VP_0_DSPP_0_OP_MODE, GC_LUT_EN, (uint32)psGCConfig->bEnable);
   out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);

   /* Setup the panel gamma correction tables */ 
   if ((NULL != psGCConfig->psAR_LUT_Blue) && (NULL != psGCConfig->psAR_LUT_Green) && (NULL != psGCConfig->psAR_LUT_Red))
   {
      uRegisterAddr[0] = HWIO_MMSS_MDP_VP_0_DSPP_0_GC_ARLUT_RED_STARTX_N_ADDR;   /* Red component - color 2 */
      uRegisterAddr[1] = HWIO_MMSS_MDP_VP_0_DSPP_0_GC_ARLUT_GREEN_STARTX_N_ADDR; /* Green component - color 0 */
      uRegisterAddr[2] = HWIO_MMSS_MDP_VP_0_DSPP_0_GC_ARLUT_BLUE_STARTX_N_ADDR;  /* Blue component - color 1 */ 

      pAR_Color[0] = psGCConfig->psAR_LUT_Red;
      pAR_Color[1] = psGCConfig->psAR_LUT_Green;
      pAR_Color[2] = psGCConfig->psAR_LUT_Blue;

      for(uComponentIndex = 0; uComponentIndex < 3; uComponentIndex++)  // 3 component RED, GREEN, BLUE
      {
         pARGC_Color =  pAR_Color[uComponentIndex];

         // StartX
         for(uIndex = 0; uIndex < psGCConfig->uSegmentSize; uIndex++)
         {
            uRegValue = 0;

            uRegValue = HWIO_OUT_FLD(uRegValue,
                                     MDP_VP_0_DSPP_0_GC_ARLUT_RED_STARTX_N,
                                     STARTX,
                                     (uint32)pARGC_Color[uIndex].uGC_Start);

            out_dword(uRegisterAddr[uComponentIndex] + uRegisterOffset, uRegValue);
         }

         // Slope
         uRegisterAddr[uComponentIndex] += 4;    // Move next address offset
         for(uIndex = 0; uIndex < psGCConfig->uSegmentSize; uIndex++)
         {
            uRegValue = 0;

            uRegValue = HWIO_OUT_FLD(uRegValue,
                                     MDP_VP_0_DSPP_0_GC_ARLUT_RED_SLOPE_N,
                                     SLOPE,
                                     (uint32)pARGC_Color[uIndex].uGC_Slope);

            out_dword(uRegisterAddr[uComponentIndex] + uRegisterOffset, uRegValue);
         }

         // Offset
         uRegisterAddr[uComponentIndex] += 4;    // Move next address offset
         for(uIndex = 0; uIndex < psGCConfig->uSegmentSize; uIndex++)
         {
            uRegValue = 0;

            uRegValue = HWIO_OUT_FLD(uRegValue,
                                     MDP_VP_0_DSPP_0_GC_ARLUT_RED_OFFSET_N,
                                     OFFSET,
                                     (uint32)pARGC_Color[uIndex].uGC_Offset);

            out_dword(uRegisterAddr[uComponentIndex] + uRegisterOffset, uRegValue);
         }         
      }
   }
#endif
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_PCCConfig()
*/
/*!
* \brief
*     Set up configurations for DSPP Polynomical Color Correction
*
* \param [in]   eDestPipe         - Destination pipe id
* \param [in]   psPCCConfig       - PCC configuration information
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DSPP_PCCConfig(HAL_MDP_DestinationPipeId                  eDestPipe,
                                   HAL_MDP_PolynomialColorCorrectConfigType   *psPCCConfig)
{
#if 0 // TODO: Vipertooth check it again
  uintPtr                                          uRegisterOffset   = 0;
  uint32                                           uDsppOpModeInfo   = 0;
  uint32                                           uComponentIndex   = 0;
  uint32                                          *pPCCRegisterConfig[HAL_MDP_PCC_CO_EFFICIENT_ENTRIES];
  HAL_MDP_PolynomialColorCorrectCoefficientType   *psPColorCorrect   = NULL; 
  HAL_MDP_PolynomialColorCorrectCoefficientType   *psPColorCorrection[3];  

  // Extract the base address to program the HW registers
  uRegisterOffset   = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);
  
  // Program the tables
  if ((NULL != psPCCConfig->psCC_Blue) && (NULL != psPCCConfig->psCC_Green) && (NULL != psPCCConfig->psCC_Red))
  {
    // Constant
    pPCCRegisterConfig[0]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_CONSTANT_COEFF_ADDR(0));
    // Red Co-efficients
    pPCCRegisterConfig[1]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_R_COEFF_ADDR(0));
    // Green Co-efficients
    pPCCRegisterConfig[2]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_G_COEFF_ADDR(0));
    // Blue Co-efficients
    pPCCRegisterConfig[3]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_B_COEFF_ADDR(0));
    // Red-Green Co-efficients
    pPCCRegisterConfig[4]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_RG_COEFF_ADDR(0));
    // Green-Blue Co-efficients
    pPCCRegisterConfig[5]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_BG_COEFF_ADDR(0));
    // Blue-Red Co-efficients
    pPCCRegisterConfig[6]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_RB_COEFF_ADDR(0));    
    // Red-Red Co-efficients
    pPCCRegisterConfig[7]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_RR_COEFF_ADDR(0));
    // Green-Green Co-efficients
    pPCCRegisterConfig[8]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_GG_COEFF_ADDR(0));
    // Blue-Blue Co-efficients
    pPCCRegisterConfig[9]  = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_BB_COEFF_ADDR(0));
    // RGB_0 Co-efficients
    pPCCRegisterConfig[10] = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_RGB_COEFF_0_ADDR(0));
    // RGB_1 Co-efficients
    pPCCRegisterConfig[11] = (uint32 *)(uRegisterOffset + HWIO_MMSS_MDP_VP_0_DSPP_0_PCC_PLANEc_RGB_COEFF_1_ADDR(0));

    // Color correction - Red component 
    psPColorCorrection[0] = psPCCConfig->psCC_Red;
    // Color correction - Green component 
    psPColorCorrection[1] = psPCCConfig->psCC_Green;
    // Color correction - Blue component 
    psPColorCorrection[2] = psPCCConfig->psCC_Blue;

    for(uComponentIndex = 0; uComponentIndex < HAL_MDP_PCC_NUM_OF_PCC_COLOR_COMPONENTS; uComponentIndex++)
    {
      psPColorCorrect = psPColorCorrection[uComponentIndex];

      // pPCCRegisterConfig is of type uint32 * so each increment automatically increments by 4 bytes 
      out_dword((pPCCRegisterConfig[0]  + uComponentIndex), (uint32)psPColorCorrect->uConstant);
      out_dword((pPCCRegisterConfig[1]  + uComponentIndex), (uint32)psPColorCorrect->uRed);
      out_dword((pPCCRegisterConfig[2]  + uComponentIndex), (uint32)psPColorCorrect->uGreen);
      out_dword((pPCCRegisterConfig[3]  + uComponentIndex), (uint32)psPColorCorrect->uBlue);
      out_dword((pPCCRegisterConfig[4]  + uComponentIndex), (uint32)psPColorCorrect->uRedGreen);
      out_dword((pPCCRegisterConfig[5]  + uComponentIndex), (uint32)psPColorCorrect->uGreenBlue);
      out_dword((pPCCRegisterConfig[6]  + uComponentIndex), (uint32)psPColorCorrect->uBlueRed);
      out_dword((pPCCRegisterConfig[7]  + uComponentIndex), (uint32)psPColorCorrect->uRedRed);
      out_dword((pPCCRegisterConfig[8]  + uComponentIndex), (uint32)psPColorCorrect->uGreenGreen);
      out_dword((pPCCRegisterConfig[9]  + uComponentIndex), (uint32)psPColorCorrect->uBlueBlue);
      out_dword((pPCCRegisterConfig[10] + uComponentIndex), (uint32)psPColorCorrect->uRedGreenBlue_0);
      out_dword((pPCCRegisterConfig[11] + uComponentIndex), (uint32)psPColorCorrect->uRedGreenBlue_1);
    }
  }
  // Enable/disable panel color correction 
  uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);
  uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo, MDP_VP_0_DSPP_0_OP_MODE, PCC_EN, (uint32)psPCCConfig->bEnable);
  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);
#endif
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_GetStateInfo()
*/
/*!
* \brief
*     Returns the current state (active/idle) of the given DSPP ID
*
* \param [in]   eDestPipe           - Destination pipe id
* \param [out]  pbActive            - TRUE : If DSPP is active
* \param [in]   uFlags              - Reserved.
*
* \retval VOID
*
****************************************************************************/
static void HAL_MDP_DestinationPipe_GetStateInfo(HAL_MDP_DestinationPipeId    eDestPipe,
                                                 bool32                      *pbActive,
                                                 uint32                       uFlags)
{
   uint32                        uRegValue         = 0x00;
   HAL_MDP_ModuleStatusRegInfo  *psModuleRegInfo   = NULL;

   *(pbActive)  = FALSE;  // Default state

   psModuleRegInfo = (HAL_MDP_ModuleStatusRegInfo  *)pgsMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_DESTINATION_PIPE];

   if(NULL != psModuleRegInfo)
   {
      uRegValue   = in_dword(MMSS_MDP_REG_BASE + psModuleRegInfo[eDestPipe].uRegAddress);

      if(0x00 != (uRegValue & psModuleRegInfo[eDestPipe].uBitFieldMask) )
      {
         *(pbActive)  = TRUE;        // Module is in active state
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_GetProperty()
*/
/*!
* \brief
*     Retrieve infomation for a particular destination pipe
*
* \param [in]   eDestPipe           - Destination pipe id
* \param [out]  pDsppInfoData       - Info data
* \param [in]   uGetPropertyFlags   - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DestinationPipe_GetProperty(HAL_MDP_DestinationPipeId             eDestPipe,
                                                       HAL_MDP_DestinationPipeInfoParamsType *pDsppInfoData,
                                                       uint32                                uGetPropertyFlags)
{
  HAL_MDSS_ErrorType      eStatus           = HAL_MDSS_STATUS_SUCCESS;
#if 0 // TODO: Vipertooth check it later
   
  uintPtr                 uRegisterOffset   = 0;
   uint32                  uIndex            = 0;
   uint32                  uHistogramData    = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_DSPP_BaseOffset(eDestPipe);

   if(NULL != pDsppInfoData->psDsppHistData)
   {
      // Lock the hist for SW read
      if(NULL != pgsMdpHwInfo->sIpFxnTbl.HistogramLock)
      {
         pgsMdpHwInfo->sIpFxnTbl.HistogramLock(uRegisterOffset, TRUE);
      }

      for(uIndex = 0; uIndex < HAL_MDP_HISTOGRAM_LUT_ENTRIES; uIndex++)
      {
         // Each read increases the hardware index automatically, so keep reading the same register
         uHistogramData = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_V_DATAN_ADDR + uRegisterOffset);

         *(pDsppInfoData->psDsppHistData->puHistData + uIndex) = uHistogramData & HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_V_DATAN_VALUE_BMSK;
      }

      // Unlock the hist for HW update
      if(NULL != pgsMdpHwInfo->sIpFxnTbl.HistogramLock)
      {
         pgsMdpHwInfo->sIpFxnTbl.HistogramLock(uRegisterOffset, FALSE);
      }
   }

   // Module active/idle state property
   if(NULL != pDsppInfoData->pbActive)
   {
      HAL_MDP_DestinationPipe_GetStateInfo(eDestPipe, pDsppInfoData->pbActive, 0x00);
   }
#endif
   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular destination pipe
*
* \param [in]   eDestPipe         - Destination pipe id
* \param [in]   psDestPipeConfig  - Configuration for the selected pipe
* \param [in]   uSetupFlags       - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DestinationPipe_Setup(HAL_MDP_DestinationPipeId                eDestPipe,
                                                 HAL_MDP_DestinationPipeConfigType       *psDestPipeConfig,
                                                 uint32                                   uSetupFlags)
{
   HAL_MDSS_ErrorType      eStatus  = HAL_MDSS_STATUS_SUCCESS;

   // Dither
   if(NULL != psDestPipeConfig->psDither)
   {
      HAL_MDP_DSPP_DitherConfig(eDestPipe, psDestPipeConfig->psDither);
   }

   // Polynomial Color Correction
   if(NULL != psDestPipeConfig->psPolynomialColorCorrect)
   {
      HAL_MDP_DSPP_PCCConfig(eDestPipe, psDestPipeConfig->psPolynomialColorCorrect);
   }

   // AR Gamma Correction
   if(NULL != psDestPipeConfig->psGammaCorrect)
   {
      HAL_MDP_DSPP_AreaReducedGCConfig(eDestPipe, psDestPipeConfig->psGammaCorrect);
   }

   // Gamut Mapping
   if(NULL != psDestPipeConfig->psGamutMapping)
   {
      HAL_MDP_DSPP_GamutMapConfig(eDestPipe, psDestPipeConfig->psGamutMapping);
   }

   // IGC
   if(NULL != psDestPipeConfig->psIGCConfig)
   {
      HAL_MDP_DSPP_IGCConfig(eDestPipe, psDestPipeConfig->psIGCConfig);
   }

   // Histogram configuration
   if(NULL != psDestPipeConfig->psHistogramConfig)
   {
      HAL_MDP_DSPP_HistogramConfig(eDestPipe, psDestPipeConfig->psHistogramConfig);
   }

   // Picture adjustment configuration
   if(NULL != psDestPipeConfig->psPictureAdjust)
   {
      HAL_MDP_DSPP_PictureAdjustmentConfig(eDestPipe, psDestPipeConfig->psPictureAdjust);
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_Init()
*/
/*!
* \brief
*     Initializes the MDP DSPP block
*
* \param [in]   eDestPipeId      - Destination pipe id
* \param [in]   uInitFlags       - Internal modules to be initialized
* \param [in]   uFlags           - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSPP_Init(HAL_MDP_DestinationPipeId    eDestPipeId,
                                     uint32                       uInitFlags,
                                     uint32                       uFlags)
{
   HAL_MDSS_ErrorType      eStatus  = HAL_MDSS_STATUS_SUCCESS;

   return eStatus;
}

#ifdef __cplusplus
}
#endif


