/*! \file */

/*
===========================================================================

FILE:         hal_mdp_smp.c

DESCRIPTION:  MDP Initialization

===========================================================================
Copyright (c) 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
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
** FUNCTION: HAL_MDP_SMP_Setup()
*/
/*!
* \brief
*     Configures the MDP's Shared Memory Pool Memory Macro Blocks
*
* \param [in] psMacroBlkConfig    - Macro Block Configuration Information
* \param [in] uNumOfConfigs       - Number of macro blocks configurations
* \param [in] uFlags              - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SMP_Setup(HAL_MDP_SMP_ConfigType    *psMacroBlkConfig,
                                     uint32                     uNumOfConfigs,
                                     uint32                     uFlags)
{
   HAL_MDSS_ErrorType      eStatus           = HAL_MDSS_STATUS_SUCCESS;
   uint32                  uIndex            = 0x00;
   uint32                  uRegOffset        = 0x00;
   uint32                  uRegWrClientValue = 0x00;
   uint32                  uRegRdClientValue = 0x00;
   uint32                  uRegField         = 0x00;
   HAL_MDP_SMP_ConfigType  *pMMBConfig       = psMacroBlkConfig;
   uint32                  uHWClientId       = 0x00;

   for(uIndex = 0; uIndex < uNumOfConfigs; uIndex++)
   {
      // Map the SMP client index to actual HW ID.
      uHWClientId  = pgsMdpHwInfo->pClientToHWClientMap[pMMBConfig->eClientId];

      // Each register holds 3 macro blocks configuration
      uRegOffset  = (uint32) ((pMMBConfig->uMacroBlockNum / 3) << 2);    // Addr increments are DWORDs
      uRegField   = (uint32) ((pMMBConfig->uMacroBlockNum % 3));

      uRegWrClientValue   = in_dword(HWIO_MMSS_MDP_SMP_ALLOC_W_0_ADDR + uRegOffset);
      uRegRdClientValue   = in_dword(HWIO_MMSS_MDP_SMP_ALLOC_R_0_ADDR + uRegOffset);

      switch(uRegField)
      {
      case 0:
         uRegWrClientValue = HWIO_OUT_FLD(uRegWrClientValue, MDP_SMP_ALLOC_W_0, MMB_0_MUX_W, uHWClientId);
         uRegRdClientValue = HWIO_OUT_FLD(uRegRdClientValue, MDP_SMP_ALLOC_R_0, MMB_0_MUX_R, uHWClientId);
         break;

      case 1:
         uRegWrClientValue = HWIO_OUT_FLD(uRegWrClientValue, MDP_SMP_ALLOC_W_0, MMB_1_MUX_W, uHWClientId);
         uRegRdClientValue = HWIO_OUT_FLD(uRegRdClientValue, MDP_SMP_ALLOC_R_0, MMB_1_MUX_R, uHWClientId);
         break;

      case 2:
         uRegWrClientValue = HWIO_OUT_FLD(uRegWrClientValue, MDP_SMP_ALLOC_W_0, MMB_2_MUX_W, uHWClientId);
         uRegRdClientValue = HWIO_OUT_FLD(uRegRdClientValue, MDP_SMP_ALLOC_R_0, MMB_2_MUX_R, uHWClientId);
         break;

      default:
         break;
      }

      out_dword(HWIO_MMSS_MDP_SMP_ALLOC_W_0_ADDR + uRegOffset, uRegWrClientValue);
      out_dword(HWIO_MMSS_MDP_SMP_ALLOC_R_0_ADDR + uRegOffset, uRegRdClientValue);

      pMMBConfig++;
   }
   return eStatus;
}

#ifdef __cplusplus
}
#endif
