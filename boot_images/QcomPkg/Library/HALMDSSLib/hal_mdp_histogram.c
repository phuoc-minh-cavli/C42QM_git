/*! \file */

/*
===========================================================================

FILE:         hal_mdp_histogram.c

DESCRIPTION:  Functions for different Histogram versions.

===========================================================================
Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
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
                                     HAL_MDP_HistogramConfigType   *psHistConfig)
{
#if 0 // TODO: Vipertooth check again
  uint32   uDsppOpModeInfo   = 0;
  uint32   uDsppHistTrigger  = 0;
  uint32   uIndex            = 0;
  uint32   uLUTRegValue      = 0;

  uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);

  // Initialization
  if(NULL != psHistConfig->pHistInitConfig)
  {
    if(TRUE == psHistConfig->pHistInitConfig->bEnable)
    {
      // Auto-clear
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_CTL,
                                      (uint32)psHistConfig->pHistInitConfig->bEnableAutoClear);

      // Enable feature
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_EN,
                                      (uint32)TRUE);

      // Frame Count configuration
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_FRAME_CNT_ADDR + uRegisterOffset,
                psHistConfig->pHistInitConfig->uFrameCnt);
    }
    else // Disable the Histogram feature
    {
      // Enable feature
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_EN,
                                      (uint32)FALSE);
    }
  }

  // Start/stop histogram generation process
  if(NULL != psHistConfig->pHistTriggerConfig)
  {
    // Either trigger the reset or trigger the histogram collection
    if(TRUE == psHistConfig->pHistTriggerConfig->bReset)
    {
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_RESET_SEQ_START_ADDR + uRegisterOffset, 1);
    }
    else
    {

      uDsppHistTrigger  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_COLLECT_CTL_ADDR + uRegisterOffset);

      if(TRUE == psHistConfig->pHistTriggerConfig->bTurnOn)
      {
        uDsppHistTrigger  &= ~HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_COLLECT_CTL_STOP_BMSK;    // Clear stop bit. 
        // @@ TODO @@ CHECK WHETHER THIS ACTION REQUIRED
        uDsppHistTrigger  = HWIO_OUT_FLD(uDsppHistTrigger,
                                         MDP_VP_0_DSPP_0_HIST_COLLECT_CTL,
                                         START,
                                         (uint32)TRUE);
      }
      else
      {
        uDsppHistTrigger  &= ~HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_COLLECT_CTL_START_BMSK;    // Clear start bit. 
        uDsppHistTrigger  = HWIO_OUT_FLD(uDsppHistTrigger,
                                         MDP_VP_0_DSPP_0_HIST_COLLECT_CTL,
                                         STOP,
                                         (uint32)TRUE);
      }
      out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_COLLECT_CTL_ADDR + uRegisterOffset, uDsppHistTrigger);
    }
  }

  // HIST LUT configuration
  if(NULL != psHistConfig->pHistLUTConfig)
  {
    if (TRUE == psHistConfig->pHistLUTConfig->bEnable)
    {
      // OP-Mode register
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_LUT_EN,              // LUT Enhancement
                                      (uint32)TRUE);

      // LUT configuration
      if ((TRUE == psHistConfig->pHistLUTConfig->bConfigLUT) &&
        (NULL != psHistConfig->pHistLUTConfig->puLUTData))
      {
        for(uIndex = 0; uIndex < HAL_MDP_GAMMA_LUT_ENTRIES; uIndex++)
        {
          uLUTRegValue = psHistConfig->pHistLUTConfig->puLUTData[uIndex] & HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_LUTN_VALUE_BMSK;

          out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_LUTN_ADDR + uRegisterOffset, uLUTRegValue);
        }
      }

      if (psHistConfig->pHistLUTConfig->bSwap)
      {
        // Swap after finishing writing to the table so it is used by the hardware
        // on the next vsync
        out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_LUT_SWAP_ADDR + uRegisterOffset, 1);
      }
    }
    else
    {
      // OP-Mode register
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_LUT_EN,              // LUT Enhancement
                                      (uint32)FALSE);
    }
  }

  // OP-Mode register
  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);
#endif
}

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
                                     HAL_MDP_HistogramConfigType   *psHistConfig)
{
#if 0 // TODO: Vipertooth check again
  uint32   uDsppOpModeInfo   = 0;
  uint32   uIndex            = 0;
  uint32   uLUTRegValue      = 0;

  uDsppOpModeInfo  = in_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset);

  // Initialization
  if(NULL != psHistConfig->pHistInitConfig)
  {
    if(TRUE == psHistConfig->pHistInitConfig->bEnable)
    {
      // Enable feature
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_EN,
                                      1);
    }
    else // Disable the Histogram feature
    {
      // Enable feature
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      HIST_EN,
                                      0);
    }
  }

  // Start/stop histogram generation process has been removed from HistV2

  // HIST LUT configuration
  if(NULL != psHistConfig->pHistLUTConfig)
  {
    if (TRUE == psHistConfig->pHistLUTConfig->bEnable)
    {
      // OP-Mode register
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      PA_LUTV_EN,              // LUT Enhancement
                                      1);

      // LUT configuration
      if ((TRUE == psHistConfig->pHistLUTConfig->bConfigLUT) &&
          (NULL != psHistConfig->pHistLUTConfig->puLUTData))
      {
        for(uIndex = 0; uIndex < HAL_MDP_GAMMA_LUT_ENTRIES; uIndex++)
        {
          // LUT value
          uLUTRegValue = HWIO_OUT_FLD(0,
                                      MDP_VP_0_DSPP_0_PA_LUTV,
                                      VALUE,
                                      psHistConfig->pHistLUTConfig->puLUTData[uIndex]);
          // LUT index
          uLUTRegValue = HWIO_OUT_FLD(uLUTRegValue,
                                      MDP_VP_0_DSPP_0_PA_LUTV,
                                      INDEX,
                                      uIndex);
          // LUT index update
          uLUTRegValue = HWIO_OUT_FLD(uLUTRegValue,
                                      MDP_VP_0_DSPP_0_PA_LUTV,
                                      INDEX_UPDATE,
                                      1);

          out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_LUTV_ADDR + uRegisterOffset, uLUTRegValue);
        }
      }

      if (psHistConfig->pHistLUTConfig->bSwap)
      {
        // Swap after finishing writing to the table so it is used by the hardware
        // on the next vsync
        out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_PA_LUTV_SWAP_ADDR + uRegisterOffset, 1);
      }
    }
    else
    {
      // OP-Mode register
      uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
                                      MDP_VP_0_DSPP_0_OP_MODE,
                                      PA_LUTV_EN,              // LUT Enhancement
                                      0);
    }
  }

  // OP-Mode register
  out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);
 #endif
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramV2_Lock()
*/
/*!
* \brief
*     Lock histogram table for SW to read
*
* \param [in]   uint32     - Register offset
* \param [out]  bLock      - lock/unlock hist table
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_HistogramV2_Lock(uintPtr  uRegisterOffset,
                                   bool32   bLock)
{
   uint32 uRegValue = 0;

   uRegValue = HWIO_OUT_FLD( 0,
                             MDP_VP_0_DSPP_0_HIST_COLLECT_CTL,
                             LOCK,
                             bLock);
   out_dword(HWIO_MMSS_MDP_VP_0_DSPP_0_HIST_COLLECT_CTL_ADDR + uRegisterOffset, uRegValue);
}

#ifdef __cplusplus
}
#endif
