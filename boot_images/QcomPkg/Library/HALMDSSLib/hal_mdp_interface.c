﻿/*! \file */

/*
===========================================================================

FILE:         hal_mdp_interface.c

DESCRIPTION: Manages the MDP (Display) Interface

===========================================================================
Copyright (c) 2012-2013, 2016, 2018 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================
*/

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _HAL_MDP_MISR_Status_Id
{
   HAL_MDP_MISR_STATUS_NONE              = 0x00000000,
   HAL_MDP_MISR_STATUS_READ_SIGNATURE,                  /**< Reads the CRC value                     */
   HAL_MDP_MISR_STATUS_READ_STATE,                      /**< Returns CRC generation complete status  */
   HAL_MDP_MISR_STATUS_MAX,
   HAL_MDP_MISR_STATUS_FORCE_32BIT       = 0x7FFFFFFF

} HAL_MDP_MISR_Status_Id;


/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_PhyInterface_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the data path
*
* \param [in] eIntefaceId         - INTERFACE ID (0, 1, 2, and 3)
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_PhyInterface_BaseOffset(HAL_MDP_PhyInterfaceId  ePhyInterfaceId)
{
   uintPtr   uRegOffset  = 0x0;

   if((HAL_MDP_PHYSICAL_INTERFACE_ID_NONE      == ePhyInterfaceId) ||
      (HAL_MDP_PHYSICAL_INTERFACE_MAX          <= ePhyInterfaceId))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPPhyInterfaceRegBaseOffset[ePhyInterfaceId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_MISR_Status()
*/
/*!
* \brief
*     Returns MISR status (or) CRC value
*
* \param [in]   eInterfaceId      - Interface id
* \param [out]  pIntfMisrStatus   - MISR status 
* \param [in]   eMisrStatusId     - MISR status id to read signature or to read CRC generate complete status
* \param [in]   eConnectType      - Interface connect type
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_MISR_Status(HAL_MDP_InterfaceId              eInterfaceId,
                                          void                             *pIntfMisrStatus,
                                          HAL_MDP_MISR_Status_Id           eMisrStatusId,
                                          HAL_MDP_InterfaceConnectionType  eConnectType)
{
#if 0 //TODO : Vipertooth check again
   HAL_MDP_Interface_MISRInfoType   *pMISR_CRC_Info         = NULL;
   HAL_MDP_Interface_MISRStatusType *pMISR_Status_Info      = NULL;
   uint32                           uMisrStatusRegValue     = 0x00000000;
   uint32                           uMisrSignatureRegValue  = 0x00000000;
   uintPtr                          uRegAddress             = 0x00000000;
   uint32                           uRegValue               = 0x00000000;


   switch(eConnectType)
   {
   case HAL_MDP_INTERFACE_CONNECTION_DSI0:
      {
         uMisrStatusRegValue     = in_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_DSI0_ADDR);
         uMisrSignatureRegValue  = in_dword(HWIO_MMSS_MDP_LP_MISR_SIGN_DSI0_ADDR);
         uRegAddress             = HWIO_MMSS_MDP_LP_MISR_CTRL_DSI0_ADDR;
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_DSI1:
      {
         uMisrStatusRegValue     = in_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_DSI1_ADDR);
         uMisrSignatureRegValue  = in_dword(HWIO_MMSS_MDP_LP_MISR_SIGN_DSI1_ADDR);
         uRegAddress             = HWIO_MMSS_MDP_LP_MISR_CTRL_DSI1_ADDR;
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_HDMI:
      {
         uMisrStatusRegValue     = in_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_HDMI_ADDR);
         uMisrSignatureRegValue  = in_dword(HWIO_MMSS_MDP_LP_MISR_SIGN_HDMI_ADDR);
         uRegAddress             = HWIO_MMSS_MDP_LP_MISR_CTRL_HDMI_ADDR;
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_eDP:
      {
         uMisrStatusRegValue     = in_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_EDP_ADDR);
         uMisrSignatureRegValue  = in_dword(HWIO_MMSS_MDP_LP_MISR_SIGN_EDP_ADDR);
         uRegAddress             = HWIO_MMSS_MDP_LP_MISR_CTRL_EDP_ADDR;
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_FB_LINEAR_MODE_WRITEBACK:
   case HAL_MDP_INTERFACE_CONNECTION_FB_LINE_MODE_WRITEBACK:
   case HAL_MDP_INTERFACE_CONNECTION_FB_BLK_MODE_WRITEBACK:
   case HAL_MDP_INTERFACE_CONNECTION_NONE:
   case HAL_MDP_INTERFACE_CONNECTION_LVDS:
   default:
      {
         HAL_MDP_ASSERT();
      }
      break;
   }

      // Read signature
      if(HAL_MDP_MISR_STATUS_READ_SIGNATURE == eMisrStatusId) 
      {
      if (0 !=uRegAddress)
      {
         pMISR_CRC_Info = (HAL_MDP_Interface_MISRInfoType *)pIntfMisrStatus;
         // Return the signature
         pMISR_CRC_Info->uCRCValue = uMisrSignatureRegValue;

         // Reset/Clear the CRC after signature read
         uRegValue      = HWIO_OUT_FLD(uRegValue,
                                           MDP_LP_MISR_CTRL_DSI0,
                                           MISR_STATUS_CLEAR, 
                                           0x01);
         out_dword(uRegAddress, uRegValue);
      }
   }
      // Read CRC completion status
      else if(HAL_MDP_MISR_STATUS_READ_STATE == eMisrStatusId)
      {
         pMISR_Status_Info = (HAL_MDP_Interface_MISRStatusType *)pIntfMisrStatus;

         if(0x00 != (uMisrStatusRegValue & HWIO_MMSS_MDP_LP_MISR_CTRL_DSI0_MISR_STATUS_BMSK))
         {
            pMISR_Status_Info->bIsCompleted = TRUE;
         }
         else
         {
            pMISR_Status_Info->bIsCompleted = FALSE;
         }
      }
#endif
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_MISR_Config()
*/
/*!
* \brief
*     Configure the interface MISR (CRC - Generator) Block
*
* \param [in]   eInterfaceId      - Interface id
* \param [in]   pIntfMisrConfig   - MISR configuration information
* \param [in]   uSetupFlags       - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_MISR_Config(HAL_MDP_InterfaceId                eInterfaceId,
                                          HAL_MDP_Interface_MISRConfigType   *pIntfMisrConfig,
                                          uint32                             uSetupFlags)
{
#if 0 //TODO: Vipertooth check 
   uint32      uMisrRegValue  = 0x00000000;


   // #1. Program the frame count and enable CRC generation. Maximum frame count supported is 64
   uMisrRegValue  = HWIO_OUT_FLD(uMisrRegValue,
                                 MDP_LP_MISR_CTRL_DSI0,
                                 MISR_FRAME_COUNT, 
                                 pIntfMisrConfig->uFrameCnt);
       
   if (TRUE == pIntfMisrConfig->bEnable)
   {
       uMisrRegValue  = HWIO_OUT_FLD(uMisrRegValue,
                                     MDP_LP_MISR_CTRL_DSI0,
                                     MISR_ENABLE, 
                                     TRUE);
   }
   else 
   {
       uMisrRegValue  = HWIO_OUT_FLD(uMisrRegValue,
                                     MDP_LP_MISR_CTRL_DSI0,
                                     MISR_ENABLE, 
                                     FALSE);
   }

   // The reset and signature clear will be performed after CRC read operation.
   switch(pIntfMisrConfig->eConnectType)
   {
   case HAL_MDP_INTERFACE_CONNECTION_DSI0:
      {
         out_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_DSI0_ADDR, uMisrRegValue);
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_DSI1:
      {
         out_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_DSI1_ADDR, uMisrRegValue);
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_HDMI:
      {
         out_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_HDMI_ADDR, uMisrRegValue);
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_eDP:
      {
         out_dword(HWIO_MMSS_MDP_LP_MISR_CTRL_EDP_ADDR, uMisrRegValue);
      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_FB_LINEAR_MODE_WRITEBACK:
      {

      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_FB_LINE_MODE_WRITEBACK:
      {

      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_FB_BLK_MODE_WRITEBACK:
      {

      }
      break;

   case HAL_MDP_INTERFACE_CONNECTION_NONE:
   case HAL_MDP_INTERFACE_CONNECTION_LVDS:
   default:
      break;
   }
#endif
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_RefreshInfo()
*/
/*!
* \brief
*     Returns the frame refresh information of the interface
*
* \param [in]   eInterfaceId        - Interface id
* \param [out]  pFrameRefreshInfo   - Frame refresh information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_RefreshInfo(HAL_MDP_InterfaceId                       eInterfaceId,
                                          HAL_MDP_Interface_FrameRefreshInfoType    *pFrameRefreshInfo,
                                          uint32                                    uFlags)
{
   uintPtr  uRegOffset        = 0x0;
   uint32   uFrameCount       = 0x00000000;
   uint32   uLineCount        = 0x00000000;

   uRegOffset = HAL_MDP_Get_PhyInterface_BaseOffset((HAL_MDP_PhyInterfaceId)eInterfaceId);

   // Support only for physical interfaces
   if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > eInterfaceId)
   {
      uFrameCount = in_dword(HWIO_MMSS_MDP_INTF_0_FRAME_COUNT_ADDR + uRegOffset);     // Frame counter
      uLineCount  = in_dword(HWIO_MMSS_MDP_INTF_0_LINE_COUNT_ADDR  + uRegOffset);      // Line counter
   }

   pFrameRefreshInfo->uCurrentFrameCnt    = uFrameCount;
   pFrameRefreshInfo->uCurrentLineCnt     = uLineCount;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_EngineStatus()
*/
/*!
* \brief
*     Returns the engine status
*
* \param [in]   eInterfaceId        - Interface id
* \param [out]  pEngineStatus       - Frame refresh information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_EngineStatus(HAL_MDP_InterfaceId                 eInterfaceId,
                                           HAL_MDP_Interface_EngineStatusType  *pEngineStatus,
                                           uint32                              uFlags)
{
   uintPtr   uRegOffset        = 0x0;

   uRegOffset = HAL_MDP_Get_PhyInterface_BaseOffset((HAL_MDP_PhyInterfaceId)eInterfaceId);

   // Support only for physical interfaces
   if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > eInterfaceId)
   {
      pEngineStatus->bIsEngineOn = (bool32)in_dword(HWIO_MMSS_MDP_INTF_0_TIMING_ENGINE_EN_ADDR + uRegOffset);     // Interface On/Off state
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_TimingGenInfo()
*/
/*!
* \brief
*     Returns the timing generator configuration parameters
*
* \param [in]   eInterfaceId        - Interface id
* \param [out]  pTimingGenInfo      - Frame refresh information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_TimingGenInfo(HAL_MDP_InterfaceId                  eInterfaceId,
                                            HAL_MDP_PhyIntf_TimingGenConfigType  *pTimingGenInfo,
                                            uint32                               uFlags)
{
   uintPtr                 uRegOffset           = 0x0;
   uint32                  uDispIntfSel         = 0x00000000;
   uint32                  uTimingRegValue      = 0x00000000;
   uint32                  uHCycle              = 0x00000000;
   uint32                  uVCycle              = 0x00000000;
   uint32                  uHsyncStartX         = 0x00000000;
   uint32                  uHsyncEndX           = 0x00000000;
   uint32                  uVsyncStart          = 0x00000000;
   uint32                  uVsyncEnd            = 0x00000000;
   uint32                  uActiveVStart        = 0x00000000;
   uint32                  uActiveVEnd          = 0x00000000;
   uint32                  uInterfaceConfig     = 0x00000000;

   // Extract the interface standard
   uDispIntfSel   = HWIO_MMSS_MDP_DISP_INTF_SEL_IN;

   switch(eInterfaceId)
   {
   case HAL_MDP_INTERFACE_0:
      {
         pTimingGenInfo->eInterfaceStandard = (HAL_MDP_InterfaceStandardType)HWIO_GETVAL(MDP_DISP_INTF_SEL, uDispIntfSel, INTF_0);
      }
      break;

   case HAL_MDP_INTERFACE_1:
      {
         pTimingGenInfo->eInterfaceStandard = (HAL_MDP_InterfaceStandardType)HWIO_GETVAL(MDP_DISP_INTF_SEL, uDispIntfSel, INTF_1);
      }
      break;

   case HAL_MDP_INTERFACE_2:
      {
         pTimingGenInfo->eInterfaceStandard = (HAL_MDP_InterfaceStandardType)HWIO_GETVAL(MDP_DISP_INTF_SEL, uDispIntfSel, INTF_2);
      }
      break;

   case HAL_MDP_INTERFACE_3:
      {
         pTimingGenInfo->eInterfaceStandard = (HAL_MDP_InterfaceStandardType)HWIO_GETVAL(MDP_DISP_INTF_SEL, uDispIntfSel, INTF_3);
      }
      break;

   case HAL_MDP_INTERFACE_4:
   case HAL_MDP_INTERFACE_5:
   case HAL_MDP_INTERFACE_NONE:
   case HAL_MDP_INTERFACE_MAX:
   case HAL_MDP_INTERFACE_FORCE_32BIT:
   default:
      break;
   }

   // @@ TODO @@ How to extract eInterfaceMode information i.e Video Mode/Command Mode
   // Does HAL driver need to handle this eInterfaceMode ??
   pTimingGenInfo->eInterfaceMode            = HAL_MDP_INTERFACE_MODE_VIDEO;                    // Interface Mode
   pTimingGenInfo->bInterlacedMode           = FALSE;                                           // No support for interlaced
   pTimingGenInfo->uVsyncHeightInPixelsF1    = 0x00;
   pTimingGenInfo->uVsyncFrontPorchInLinesF1 = 0x00;
   pTimingGenInfo->uVsyncBackPorchInLinesF1  = 0x00;
   pTimingGenInfo->uVsyncPulseInLinesF1      = 0x00;

   uRegOffset = HAL_MDP_Get_PhyInterface_BaseOffset((HAL_MDP_PhyInterfaceId)eInterfaceId);

   // Extract video engine timing parameters

   // Signal Polarity Configuration
   uTimingRegValue = in_dword(HWIO_MMSS_MDP_INTF_0_POLARITY_CTL_ADDR + uRegOffset);

   pTimingGenInfo->eDataEnableSignal = (HAL_MDP_SignalPolarityType) HWIO_GETVAL(MDP_INTF_0_POLARITY_CTL, uTimingRegValue, DEN);        // DATA_EN Polarity
   pTimingGenInfo->eVsyncSignal      = (HAL_MDP_SignalPolarityType) HWIO_GETVAL(MDP_INTF_0_POLARITY_CTL, uTimingRegValue, VSYNC);      // VSYNC Polarity
   pTimingGenInfo->eHsyncSignal      = (HAL_MDP_SignalPolarityType) HWIO_GETVAL(MDP_INTF_0_POLARITY_CTL, uTimingRegValue, HSYNC);      // HSYNC Polarity

   // HSYNC SKEW
   pTimingGenInfo->uHysncSkewInPixels = in_dword(HWIO_MMSS_MDP_INTF_0_HSYNC_SKEW_ADDR + uRegOffset);

   // Border Color
   pTimingGenInfo->uBorderColorInRGB888 = in_dword(HWIO_MMSS_MDP_INTF_0_BORDER_COLOR_ADDR + uRegOffset);

   // Underflow color
   pTimingGenInfo->uUnderflowColorInRGB888 = in_dword(HWIO_MMSS_MDP_INTF_0_UNDERFLOW_COLOR_ADDR + uRegOffset);

   // HSYNC related parameters
   uTimingRegValue   = in_dword(HWIO_MMSS_MDP_INTF_0_HSYNC_CTL_ADDR + uRegOffset);                                           // HSYNC_CTL Register

   uHCycle                              = HWIO_GETVAL(MDP_INTF_0_HSYNC_CTL, uTimingRegValue, HSYNC_PERIOD);             // H-CYCLE
   pTimingGenInfo->uHsyncPulseInPixels  = HWIO_GETVAL(MDP_INTF_0_HSYNC_CTL, uTimingRegValue, PULSE_WIDTH);              // HSYNC Pulse Width

   uTimingRegValue   = in_dword(HWIO_MMSS_MDP_INTF_0_DISPLAY_HCTL_ADDR + uRegOffset);                                        // DISPLAY_HCTL Register
   uHsyncStartX      = HWIO_GETVAL(MDP_INTF_0_DISPLAY_HCTL, uTimingRegValue, DISPLAY_START_X);                          // HSYNC Start X
   uHsyncEndX        = (HWIO_GETVAL(MDP_INTF_0_DISPLAY_HCTL, uTimingRegValue, DISPLAY_END_X) + 1);                      // HSYNC End X

   uTimingRegValue   = in_dword(HWIO_MMSS_MDP_INTF_0_ACTIVE_HCTL_ADDR + uRegOffset);                                         // ACTIVE_HCTL
   uInterfaceConfig  = in_dword(HWIO_MMSS_MDP_INTF_0_INTF_CONFIG_ADDR + uRegOffset);

   //if(TRUE == HWIO_GETVAL(MDP_INTF_0_ACTIVE_HCTL, uTimingRegValue, ACTIVE_H_EN)) //TODO: Vipertooth check change
   if(TRUE == HWIO_GETVAL(MDP_INTF_0_INTF_CONFIG, uInterfaceConfig, ACTIVE_H_EN))
   {
      // Right Border Pixels = (uHsyncEndX - ACTIVE_END_X register field value)
      pTimingGenInfo->uHRightBorderInPixels  = (uHsyncEndX - (HWIO_GETVAL(MDP_INTF_0_ACTIVE_HCTL,
                                                              uTimingRegValue,
                                                              ACTIVE_END_X)));                                          // Right Border Pixels

      // Left Border Pixels = (ACTIVE_START_X register field value - uHysncStartX)
      pTimingGenInfo->uHLeftBorderInPixels   = ((HWIO_GETVAL(MDP_INTF_0_ACTIVE_HCTL,
                                                             uTimingRegValue,
                                                             ACTIVE_START_X)) - uHsyncStartX);                          // Left Border Pixels
   }
   else
   {
      pTimingGenInfo->uHRightBorderInPixels  = 0x00;
      pTimingGenInfo->uHLeftBorderInPixels   = 0x00;
   }

   pTimingGenInfo->uHsyncBackPorchInPixels   = (uHsyncStartX - pTimingGenInfo->uHsyncPulseInPixels);                    // HSYNC BackPorch
   pTimingGenInfo->uVisibleWidthInPixels     = (uHsyncEndX - (pTimingGenInfo->uHsyncBackPorchInPixels  +
                                                              pTimingGenInfo->uHsyncPulseInPixels      +
                                                              pTimingGenInfo->uHLeftBorderInPixels     +
                                                              pTimingGenInfo->uHRightBorderInPixels));                  // Visible Width

   pTimingGenInfo->uHsyncFrontPorchInPixels  = (uHCycle - uHsyncEndX);                                                  // HSYNC FrontPorch

   // VSYNC related parameters
   if (0==uHCycle)
   {
     pTimingGenInfo->uVsyncPulseInLines      = 0;
     pTimingGenInfo->uVTopBorderInLines      = 0;
     pTimingGenInfo->uVsyncBackPorchInLines  = 0;
     pTimingGenInfo->uVsyncFrontPorchInLines = 0;

   }
   else
   {
      uVCycle                            = (in_dword(HWIO_MMSS_MDP_INTF_0_VSYNC_PERIOD_F0_ADDR + uRegOffset)/uHCycle);          // V-CYCLE in lines
      pTimingGenInfo->uVsyncPulseInLines = (in_dword(HWIO_MMSS_MDP_INTF_0_VSYNC_PULSE_WIDTH_F0_ADDR + uRegOffset)/uHCycle);     // VSYNC Pulse Width in lines
      uVsyncStart                        = (in_dword(HWIO_MMSS_MDP_INTF_0_DISPLAY_V_START_F0_ADDR + uRegOffset)/uHCycle);       // V-Start in lines
      uVsyncEnd                          = ((in_dword(HWIO_MMSS_MDP_INTF_0_DISPLAY_V_END_F0_ADDR + uRegOffset) + 1)/uHCycle);   // V-End in lines

      //if(TRUE == (HWIO_GETVAL(MDP_INTF_0_ACTIVE_V_START_F0, uActiveVStart, ACTIVE_V_EN)))
	  if(TRUE == (HWIO_GETVAL(MDP_INTF_0_INTF_CONFIG, uInterfaceConfig, ACTIVE_V_EN)))     // TODO: Vipertooth check
      {
         uActiveVStart                       = (in_dword(HWIO_MMSS_MDP_INTF_0_ACTIVE_V_START_F0_ADDR + uRegOffset)/uHCycle);    // Active-V-Start in lines
         uActiveVEnd                         = (in_dword(HWIO_MMSS_MDP_INTF_0_ACTIVE_V_END_F0_ADDR + uRegOffset)/uHCycle);      // Active-V-End in lines
      }
      else
      {
         uActiveVStart                       = uVsyncStart;
         uActiveVEnd                         = uVsyncEnd;
      }

      pTimingGenInfo->uVTopBorderInLines     = (uActiveVStart - uVsyncStart);                                              // VSYNC Top Border in lines
      pTimingGenInfo->uVBottomBorderInLines  = (uVsyncEnd - uActiveVEnd);                                                  // VSYNC Bottom Border in lines

      pTimingGenInfo->uVsyncBackPorchInLines = (uVsyncStart - pTimingGenInfo->uVsyncPulseInLines);                         // VSYNC BackPorch in lines
      pTimingGenInfo->uVisibleHeightInPixels = (uActiveVEnd - (pTimingGenInfo->uVsyncPulseInLines     +
                                                               pTimingGenInfo->uVsyncBackPorchInLines +
                                                               pTimingGenInfo->uVTopBorderInLines) );                      // Visual Height in pixels
      pTimingGenInfo->uVsyncFrontPorchInLines= (uVCycle - uVsyncEnd);                                                      // VSYNC Front Port in lines
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_GetDualInterfaceInfo()
*/
/*!
* \brief
*     Get Split Display Mode
*
* \param [in]   eInterfaceId              - Interface id
* \param [out]  pbDualInterfaceEnabled    - TRUE: Dual interface (Split Display)is enabled
* \param [in]   uFlags                    - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_GetDualInterfaceInfo(HAL_MDP_InterfaceId                eInterfaceId,
                                                   bool32                            *pbDualInterfaceEnabled,
                                                   uint32                             uFlags)
{
  uint32 uReg = in_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_EN_ADDR);
  if (0 != uReg)
  {
    *pbDualInterfaceEnabled = TRUE;
  }
  else
  {
    *pbDualInterfaceEnabled = FALSE;
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_GetStateInfo()
*/
/*!
* \brief
*     Returns the current state of the given interface id.
*
* \param [in]   eInterfaceId        - Interface id
* \param [out]  pbActive            - TRUE : If module is active
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_GetStateInfo(HAL_MDP_InterfaceId    eInterfaceId,
                                           bool32                *pbActive,
                                           uint32                 uFlags)
{
   uint32                        uRegValue         = 0x00;
   uint32                        uWBIntfId         = 0x00;
   HAL_MDP_ModuleStatusRegInfo  *psModuleRegInfo   = NULL;

   *(pbActive)        = FALSE;          // Default state

   if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE < eInterfaceId)    // Valid only for write-back interfaces
   {
      psModuleRegInfo = (HAL_MDP_ModuleStatusRegInfo  *)pgsMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_WRITEBACK_INTERFACE];

      if(NULL != psModuleRegInfo)
      {
         uWBIntfId   = (eInterfaceId - HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE);
         uRegValue   = in_dword(MMSS_MDP_REG_BASE + psModuleRegInfo[uWBIntfId].uRegAddress);

         if(0x00 != (uRegValue & psModuleRegInfo[uWBIntfId].uBitFieldMask) )
         {
            *(pbActive)  = TRUE;        // Module is in active state
         }
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_TPG_Config()
*/
/*!
* \brief
*     Configurs the test pattern
*
* \param [in]   eInterfaceId     - Interface id
* \param [in]   pTPGConfig       - TPG configuration information
* \param [in]   uSetupFlags      - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_TPG_Config(HAL_MDP_PhyInterfaceId             eInterfaceId,
                                         HAL_MDP_Interface_TPGConfigType    *pTPGConfig,
                                         uint32                             uSetupFlags)
{
   uintPtr  uRegOffset           = 0x0;
   uint32   uTPGContrlRegValue   = 0x00000000;
   uint32   uTempRegValue        = 0x00000000;

   uRegOffset = HAL_MDP_Get_PhyInterface_BaseOffset(eInterfaceId);

   if(TRUE == pTPGConfig->bEnable)
   {
      // Required to verify the programming sequence. Not specified in HPG

      uTPGContrlRegValue  = (1 << (pTPGConfig->eTestPatternType));
      uTPGContrlRegValue |= HWIO_MMSS_MDP_INTF_0_TPG_MAIN_CONTROL_TPG_SW_RESET_BMSK;     // Soft reset

      out_dword(HWIO_MMSS_MDP_INTF_0_TPG_MAIN_CONTROL_ADDR + uRegOffset, uTPGContrlRegValue);

      // Wait for reset complete
      uTempRegValue = HWIO_MMSS_MDP_HW_VERSION_IN;
      uTempRegValue += HWIO_MMSS_MDP_HW_VERSION_IN;

      uTPGContrlRegValue &= ~HWIO_MMSS_MDP_INTF_0_TPG_MAIN_CONTROL_TPG_SW_RESET_BMSK;     // Release reset
      out_dword(HWIO_MMSS_MDP_INTF_0_TPG_MAIN_CONTROL_ADDR + uRegOffset, uTPGContrlRegValue);

      if(HAL_MDP_TPG_TYPE_FIXED_PATTERN == pTPGConfig->eTestPatternType)         // Only Fixed Test Pattern is supported for now
      {
         out_dword(HWIO_MMSS_MDP_INTF_0_TPG_INITIAL_VALUE_ADDR + uRegOffset, pTPGConfig->sTestPatternConfigInfo.uFixedPattern);
      }
   }
   out_dword(HWIO_MMSS_MDP_INTF_0_TPG_ENABLE_ADDR + uRegOffset, pTPGConfig->bEnable); // Enable/Disable TPG
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_TimingGen_Config()
*/
/*!
* \brief
*     Set up timing generation for given interface
*
* \param [in]   eInterfaceId      - Interface id
* \param [in]   pInterfaceConfig  - Configuration information
* \param [in]   uSetupFlags       - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interface_TimingGen_Config(HAL_MDP_PhyInterfaceId                eInterfaceId,
                                               HAL_MDP_PhyIntf_TimingGenConfigType   *pInterfaceConfig,
                                               uint32                                uSetupFlags)
{
   uintPtr                 uRegOffset           = 0x0;
   uint32                  uDispIntfSel         = 0x00000000;
   uint32                  uTimingRegValue      = 0x00000000;
   uint32                  uHCycle              = 0x00000000;  // To prepare the register value
   uint32                  uVCycle              = 0x00000000;
   uint32                  uHsyncStartX         = 0x00000000;
   uint32                  uHsyncEndX           = 0x00000000;
   uint32                  uVsyncStart          = 0x00000000;
   uint32                  uVsyncEnd            = 0x00000000;
   uint32                  uActiveVStart        = 0x00000000;
   uint32                  uActiveVEnd          = 0x00000000;
   uint32                  uPanelFormat         = 0x00000000;
   uint32                  uPanelIntfConfig     = 0x00000000;

   uDispIntfSel   = HWIO_MMSS_MDP_DISP_INTF_SEL_IN;

   switch(eInterfaceId)
   {
   case HAL_MDP_PHYSICAL_INTERFACE_0:

      uDispIntfSel   = HWIO_OUT_FLD(uDispIntfSel, 
                                    MDP_DISP_INTF_SEL, 
                                    INTF_0, 
                                    pInterfaceConfig->eInterfaceStandard);
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_1:

      uDispIntfSel   = HWIO_OUT_FLD(uDispIntfSel, 
                                    MDP_DISP_INTF_SEL, 
                                    INTF_1, 
                                    pInterfaceConfig->eInterfaceStandard);
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_2:

      uDispIntfSel   = HWIO_OUT_FLD(uDispIntfSel, 
                                    MDP_DISP_INTF_SEL, 
                                    INTF_2, 
                                    pInterfaceConfig->eInterfaceStandard);
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_3:

      uDispIntfSel   = HWIO_OUT_FLD(uDispIntfSel, 
                                    MDP_DISP_INTF_SEL, 
                                    INTF_3, 
                                    pInterfaceConfig->eInterfaceStandard);
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_4:
   case HAL_MDP_PHYSICAL_INTERFACE_5:
   case HAL_MDP_PHYSICAL_INTERFACE_ID_NONE:
   case HAL_MDP_PHYSICAL_INTERFACE_MAX:
   case HAL_MDP_PHYSICAL_INTERFACE_FORCE_32BIT:
   default:
      break;
   }

   // Select the interface standard
   HWIO_MMSS_MDP_DISP_INTF_SEL_OUT(uDispIntfSel);

   // Configure the video engine timing parameter if inteface is active display type
   if(HAL_MDP_INTERFACE_MODE_VIDEO == pInterfaceConfig->eInterfaceMode)
   {
      uRegOffset = HAL_MDP_Get_PhyInterface_BaseOffset(eInterfaceId);

      // HSYNC_CTL (HSYNC PERIO and HSYNC PULSE WIDTH)
      uTimingRegValue      = 0x00000000;

      uHCycle              = (pInterfaceConfig->uHsyncBackPorchInPixels    +
                              pInterfaceConfig->uHsyncFrontPorchInPixels   +
                              pInterfaceConfig->uHsyncPulseInPixels        +
                              pInterfaceConfig->uVisibleWidthInPixels      +
                              pInterfaceConfig->uHLeftBorderInPixels       +
                              pInterfaceConfig->uHRightBorderInPixels);

      uVCycle        = ((pInterfaceConfig->uVisibleHeightInPixels     +
                         pInterfaceConfig->uVsyncBackPorchInLines     +
                         pInterfaceConfig->uVsyncPulseInLines         +
                         pInterfaceConfig->uVsyncFrontPorchInLines    +
                         pInterfaceConfig->uVTopBorderInLines         +
                         pInterfaceConfig->uVBottomBorderInLines) * uHCycle);

      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue, 
                                          MDP_INTF_0_HSYNC_CTL, 
                                          HSYNC_PERIOD, 
                                          uHCycle);

      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue, 
                                          MDP_INTF_0_HSYNC_CTL, 
                                          PULSE_WIDTH, 
                                          pInterfaceConfig->uHsyncPulseInPixels);

      out_dword(HWIO_MMSS_MDP_INTF_0_HSYNC_CTL_ADDR + uRegOffset, uTimingRegValue);            // HSYNC_CTL

      // VSYNC PERIOD
      out_dword(HWIO_MMSS_MDP_INTF_0_VSYNC_PERIOD_F0_ADDR + uRegOffset, uVCycle);  // VSYNC_PERIOD_F0

      // VSYNC_PULSE_WIDTH (F0)
      uTimingRegValue = pInterfaceConfig->uVsyncPulseInLines * uHCycle;

      out_dword(HWIO_MMSS_MDP_INTF_0_VSYNC_PULSE_WIDTH_F0_ADDR + uRegOffset, uTimingRegValue);  // VSYNC_PULSE_WIDTH_F0

      // DISPLAY_V_START
      uVsyncStart    = ((pInterfaceConfig->uVsyncBackPorchInLines    +
                         pInterfaceConfig->uVsyncPulseInLines ) * uHCycle);
      
      uVsyncEnd      = (((pInterfaceConfig->uVsyncBackPorchInLines    +
                          pInterfaceConfig->uVsyncPulseInLines        +
                          pInterfaceConfig->uVisibleHeightInPixels    +
                          pInterfaceConfig->uVBottomBorderInLines     +
                          pInterfaceConfig->uVTopBorderInLines ) * uHCycle) - 1);
      
      if(HAL_MDP_INTERFACE_STANDARD_eDP == pInterfaceConfig->eInterfaceStandard)
      {
         uVsyncStart    += pInterfaceConfig->uHsyncBackPorchInPixels    +
                           pInterfaceConfig->uHsyncPulseInPixels;
         uVsyncEnd      -= pInterfaceConfig->uHsyncFrontPorchInPixels;
      }
      out_dword(HWIO_MMSS_MDP_INTF_0_DISPLAY_V_START_F0_ADDR + uRegOffset, uVsyncStart);   // DISPLAY_V_START_F0

      // DISPLAY_V_END
      out_dword(HWIO_MMSS_MDP_INTF_0_DISPLAY_V_END_F0_ADDR + uRegOffset, uVsyncEnd);     // DISPLAY_V_END_F0

      // ACTIVE_V_START 
      uActiveVStart      = 0x00;
      uActiveVEnd        = 0x00;

      if((0x00 != pInterfaceConfig->uVBottomBorderInLines) || 
         (0x00 != pInterfaceConfig->uVTopBorderInLines) )
      {
         uActiveVStart  =  HWIO_OUT_FLD(uActiveVStart, 
                                        MDP_INTF_0_ACTIVE_V_START_F0,
                                        ACTIVE_START_Y,
                                        (uVsyncStart + (pInterfaceConfig->uVTopBorderInLines * uHCycle)));
#if 0
         uActiveVStart  =  HWIO_OUT_FLD(uActiveVStart, 
                                        MDP_INTF_0_ACTIVE_V_START_F0,
                                        ACTIVE_V_EN,
                                        TRUE);
#else
         uPanelIntfConfig  =  HWIO_OUT_FLD(uPanelIntfConfig, 
                                           MDP_INTF_0_INTF_CONFIG,
                                           ACTIVE_V_EN,
                                           TRUE);
#endif
         uActiveVEnd    = (uVsyncEnd - (pInterfaceConfig->uVBottomBorderInLines * uHCycle));
      }

      out_dword(HWIO_MMSS_MDP_INTF_0_ACTIVE_V_START_F0_ADDR + uRegOffset, uActiveVStart);     // ACTIVE_V_START_F0

      // ACTIVE_V_END
      out_dword(HWIO_MMSS_MDP_INTF_0_ACTIVE_V_END_F0_ADDR + uRegOffset, uActiveVEnd);       // ACTIVE_V_END_F0

      // DISPLAY_HCTL
      uHsyncStartX         = (pInterfaceConfig->uHsyncBackPorchInPixels +
                              pInterfaceConfig->uHsyncPulseInPixels);

      uTimingRegValue      = 0x00;
      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue, 
                                          MDP_INTF_0_DISPLAY_HCTL,
                                          DISPLAY_START_X,
                                          uHsyncStartX);

      uHsyncEndX           = (pInterfaceConfig->uHsyncBackPorchInPixels  +
                              pInterfaceConfig->uHsyncPulseInPixels      +
                              pInterfaceConfig->uHLeftBorderInPixels     +
                              pInterfaceConfig->uHRightBorderInPixels    +
                              pInterfaceConfig->uVisibleWidthInPixels    - 1);

      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue, 
                                          MDP_INTF_0_DISPLAY_HCTL,
                                          DISPLAY_END_X,
                                          uHsyncEndX);

      out_dword(HWIO_MMSS_MDP_INTF_0_DISPLAY_HCTL_ADDR + uRegOffset, uTimingRegValue);       // DISPLAY_HCTL

      // ACTIVE_HCTL
      uTimingRegValue      = 0x00;

      if ((0 != pInterfaceConfig->uHLeftBorderInPixels ) || 
          (0 != pInterfaceConfig->uHRightBorderInPixels) )
      {
         uTimingRegValue   = HWIO_OUT_FLD(uTimingRegValue, 
                                          MDP_INTF_0_ACTIVE_HCTL,
                                          ACTIVE_END_X,
                                          (uHsyncEndX - pInterfaceConfig->uHRightBorderInPixels));

         uTimingRegValue   = HWIO_OUT_FLD(uTimingRegValue,
                                          MDP_INTF_0_ACTIVE_HCTL,
                                          ACTIVE_START_X,
                                          (uHsyncStartX + pInterfaceConfig->uHLeftBorderInPixels));
#if 0
         uTimingRegValue   = HWIO_OUT_FLD(uTimingRegValue,
                                          MDP_INTF_0_ACTIVE_HCTL,
                                          ACTIVE_H_EN,
                                          TRUE);
#else
         uPanelIntfConfig  = HWIO_OUT_FLD(uPanelIntfConfig,
                                          MDP_INTF_0_INTF_CONFIG,
                                          ACTIVE_H_EN,
                                          TRUE);
#endif
      }

      out_dword(HWIO_MMSS_MDP_INTF_0_ACTIVE_HCTL_ADDR + uRegOffset, uTimingRegValue);       // ACTIVE_HCTL

      // Border Color
      out_dword(HWIO_MMSS_MDP_INTF_0_BORDER_COLOR_ADDR + uRegOffset, pInterfaceConfig->uBorderColorInRGB888);

      // Underflow color
      out_dword(HWIO_MMSS_MDP_INTF_0_UNDERFLOW_COLOR_ADDR + uRegOffset, pInterfaceConfig->uUnderflowColorInRGB888);

      // HSYNC SKEW
      out_dword(HWIO_MMSS_MDP_INTF_0_HSYNC_SKEW_ADDR + uRegOffset, pInterfaceConfig->uHysncSkewInPixels);

      // Signal Polarity Selection
      uTimingRegValue      = 0x00000000;
      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue,
                                          MDP_INTF_0_POLARITY_CTL,
                                          DEN,
                                          pInterfaceConfig->eDataEnableSignal);

      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue,
                                          MDP_INTF_0_POLARITY_CTL,
                                          VSYNC,
                                          pInterfaceConfig->eVsyncSignal);

      uTimingRegValue      = HWIO_OUT_FLD(uTimingRegValue,
                                          MDP_INTF_0_POLARITY_CTL,
                                          HSYNC,
                                          pInterfaceConfig->eHsyncSignal);

      out_dword(HWIO_MMSS_MDP_INTF_0_POLARITY_CTL_ADDR + uRegOffset, uTimingRegValue);

      uPanelFormat         = in_dword(HWIO_MMSS_MDP_INTF_0_PANEL_FORMAT_ADDR + uRegOffset);
      uPanelFormat        |= guPanelFormatInfo[pInterfaceConfig->eInterfacePixelFormat];

      out_dword(HWIO_MMSS_MDP_INTF_0_PANEL_FORMAT_ADDR + uRegOffset, uPanelFormat);
#if 0
      out_dword(HWIO_MMSS_MDP_INTF_0_INTF_CONFIG_ADDR + uRegOffset, uPanelIntfConfig);
#endif
   }
}

HAL_MDSS_ErrorType HAL_MDP_Interface_SplitDisplay_Config(HAL_MDP_Interface_SplitDisplayType *pSplitDisplayConfig,
                                                         uint32                              uSetupFlags)
{
  HAL_MDSS_ErrorType  eStatus       = HAL_MDSS_STATUS_SUCCESS;
  uint32              uSplitCTRLVal = 0;

  if(pSplitDisplayConfig->bEnableSplitDisplay)
  {
    if (HAL_MDP_INTERFACE_MODE_VIDEO == pSplitDisplayConfig->eInterfaceMode )
    {
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, SPLIT_DISPLAY_TYPE,   0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, SMART_PANEL_FREE_RUN, 0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, INTF_1_SW_TRG_MUX,    0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, INTF_2_SW_TRG_MUX,    0x1);
      out_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL_ADDR, uSplitCTRLVal);

      uSplitCTRLVal = 0;
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, SPLIT_DISPLAY_TYPE,         0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, SMART_PANEL_FREE_RUN,       0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, INTF_1_TIM_GEN_SYNC_MODE,   0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, INTF_2_TIM_GEN_SYNC_MODE,   0x1);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, TE_LINE_INTERVAL_WATERMARK, 0x0);
      out_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL_ADDR, uSplitCTRLVal);

    }
    else
    {
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, SPLIT_DISPLAY_TYPE,   0x1);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, SMART_PANEL_FREE_RUN, ((HAL_MDP_FLAGS_PP_SPLIT_ENABLED & uSetupFlags) ? 0x1 : 0x0));
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, INTF_1_SW_TRG_MUX,    0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL, INTF_2_SW_TRG_MUX,    0x1);
      out_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_UPPER_PIPE_CTRL_ADDR, uSplitCTRLVal);

      uSplitCTRLVal = 0;
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, SPLIT_DISPLAY_TYPE,         0x1);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, SMART_PANEL_FREE_RUN,       ((HAL_MDP_FLAGS_PP_SPLIT_ENABLED & uSetupFlags) ? 0x1 : 0x0));
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, INTF_1_TIM_GEN_SYNC_MODE,   0x0);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, INTF_2_TIM_GEN_SYNC_MODE,   0x1);
      uSplitCTRLVal = HWIO_OUT_FLD(uSplitCTRLVal,MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL, TE_LINE_INTERVAL_WATERMARK, 0x0);
      out_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_LOWER_PIPE_CTRL_ADDR, uSplitCTRLVal);
    }
    out_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_EN_ADDR, 0x1);
  }
  else
  {
    out_dword(HWIO_MMSS_MDP_SPLIT_DISPLAY_EN_ADDR, 0x0);
  }

  return eStatus;
}
/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular interface
*
* \param [in]   eInterfaceId      - Interface id
* \param [in]   pInterfaceConfig  - Configuration for the selected interface
* \param [in]   uSetupFlags       - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interface_Setup(HAL_MDP_InterfaceId           eInterfaceId,
                                           HAL_MDP_InterfaceConfigType   *pInterfaceConfig,
                                           uint32                        uSetupFlags)
{
   HAL_MDSS_ErrorType      eStatus  = HAL_MDSS_STATUS_SUCCESS;

   // Interface Timing Generator Configuration
   if(NULL != pInterfaceConfig->pIntrTimingGenConfig)
   {
      if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > eInterfaceId) // Physical interface
      {
         HAL_MDP_Interface_TimingGen_Config((HAL_MDP_PhyInterfaceId)eInterfaceId,
                                            pInterfaceConfig->pIntrTimingGenConfig,
                                            0x00);
      }
      // NO timing generator support for write-back interfaces
   }

   // Interface CRC configuration
   if(NULL != pInterfaceConfig->pIntrMISRConfig)
   {
      HAL_MDP_Interface_MISR_Config(eInterfaceId,
                                    pInterfaceConfig->pIntrMISRConfig,
                                    0x00);
   }

   // TPG - Test Pattern Generation
   if(NULL != pInterfaceConfig->pIntrTPGConfig)
   {
      HAL_MDP_Interface_TPG_Config((HAL_MDP_PhyInterfaceId)eInterfaceId,
                                   pInterfaceConfig->pIntrTPGConfig,
                                   0x00);
   }

   // SplitDisplay
   if (NULL != pInterfaceConfig->pSplitDisplayConfig)
   {
     HAL_MDP_Interface_SplitDisplay_Config(pInterfaceConfig->pSplitDisplayConfig, uSetupFlags);
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_GetProperty()
*/
/*!
* \brief
*     Retrieve information for a particular interface
*
* \param [in]   eInterfaceId          - Interface id
* \param [out]  pInterfaceInfoData    - Info data
* \param [in]   uGetPropertyFlags     - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interface_GetProperty(HAL_MDP_InterfaceId             eInterfaceId,
                                                 HAL_MDP_InterfaceInfoParamsType *pInterfaceInfoData,
                                                 uint32                          uGetPropertyFlags)
{
   HAL_MDSS_ErrorType   eStatus  = HAL_MDSS_STATUS_SUCCESS;

   // MISR Complete status
   if(NULL != pInterfaceInfoData->pMISR_Status_Info)
   {
      HAL_MDP_Interface_MISR_Status(eInterfaceId,
                                    (void *)pInterfaceInfoData->pMISR_Status_Info,
                                    HAL_MDP_MISR_STATUS_READ_STATE,
                                    pInterfaceInfoData->pMISR_Status_Info->eConnectType);
   }


   // MISR Signature
   if(NULL != pInterfaceInfoData->pMISR_CRC_Info)
   {
      HAL_MDP_Interface_MISR_Status(eInterfaceId,
                                    (void *)pInterfaceInfoData->pMISR_CRC_Info,
                                    HAL_MDP_MISR_STATUS_READ_SIGNATURE,
                                    pInterfaceInfoData->pMISR_CRC_Info->eConnectType
                                    );
   }    

   // Interface Frame Rate Information
   if(NULL != pInterfaceInfoData->pFrameRefreshInfo)
   {
      HAL_MDP_Interface_RefreshInfo(eInterfaceId,
                                    pInterfaceInfoData->pFrameRefreshInfo,
                                    0x00);
   }

   // Interface Engine status
   if(NULL != pInterfaceInfoData->pEngineStatus)
   {
      HAL_MDP_Interface_EngineStatus(eInterfaceId,
                                     pInterfaceInfoData->pEngineStatus,
                                     0x00);
   }

   // Interface Timing Generation Parameters
   if(NULL != pInterfaceInfoData->pTimingGenInfo)
   {
      HAL_MDP_Interface_TimingGenInfo(eInterfaceId,
                                      pInterfaceInfoData->pTimingGenInfo,
                                      0x00);
   }

   // Interface status
   if(NULL != pInterfaceInfoData->pbActive)
   {
      HAL_MDP_Interface_GetStateInfo(eInterfaceId,
                                     pInterfaceInfoData->pbActive,
                                     0x00);
   }

   // Dual Interface (split display) status
   if(NULL != pInterfaceInfoData->pbDualInterfaceEnabled)
   {
     HAL_MDP_Interface_GetDualInterfaceInfo(eInterfaceId,
       pInterfaceInfoData->pbDualInterfaceEnabled,
       0x00);
   }
   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_Enable()
*/
/*!
* \brief
*     Enable/Disable a particular interface
*
* \param [in]   eInterfaceId            - Interface id
* \param [in]   eInterfaceMode          - Interface mode (CMD/VIDEO)
* \param [in]   bEnable                 - True for enable, false for disable 
* \param [in]   uEnableFlags            - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interface_Enable(HAL_MDP_InterfaceId          eInterfaceId,
                                            HAL_MDP_InterfaceModeType    eInterfaceMode,
                                            bool32                       bEnable,
                                            uint32                       uEnableFlags)
{
   HAL_MDSS_ErrorType      eStatus              = HAL_MDSS_STATUS_SUCCESS;
   uintPtr                 uRegOffset           = 0x0;
   
   if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > eInterfaceId) // Physical interface
   {
      uRegOffset = HAL_MDP_Get_PhyInterface_BaseOffset((HAL_MDP_PhyInterfaceId)eInterfaceId);

      out_dword(HWIO_MMSS_MDP_INTF_0_TIMING_ENGINE_EN_ADDR + uRegOffset, bEnable);
   }
   return eStatus;
}

#ifdef __cplusplus
}
#endif
