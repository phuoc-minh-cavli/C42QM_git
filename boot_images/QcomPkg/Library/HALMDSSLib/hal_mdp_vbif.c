/*! \file */

/*
===========================================================================

FILE:        hal_mdp_vbif.c

DESCRIPTION: Configures VBIF settings for MDSS.
  
===========================================================================
  Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------------------
 * Local defines 
 * ------------------------------------------------------------------------------------ */



/* -------------------------------------------------------------------------------------
 * Static data declarations and functions
 * ------------------------------------------------------------------------------------ */


/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_Config_ClockGating()
*/
/*!
* \brief
*     Configures clock gating on the VBIF
*
* \param [in] bEnableGating  - TRUE: enable clock gating AXI & Test Bus Domain; otherwise disable;
* \param [in] uFlags         - reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_Vbif_Config_ClockGating(
                              bool32   bEnableGating,
                              uint32   uFlags )
{
   uint32 uRegValue       = 0;
   uint32 uAxiForceOn     = 0;
   uint32 uTestBusForceOn = 0;

   if (FALSE == bEnableGating)
   {
     uAxiForceOn     = 1;
     uTestBusForceOn = 1;
   }
   
   uRegValue = HWIO_OUT_FLD(uRegValue, VBIF, VBIF_CLKON_FORCE_ON, uAxiForceOn);
   uRegValue = HWIO_OUT_FLD(uRegValue, VBIF, VBIF_CLKON_FORCE_ON_TESTBUS, uTestBusForceOn);
   uRegValue = HWIO_OUT_FLD(uRegValue, VBIF, VBIF_CLKON_HYSTERESIS_VALUE, 0);
   
   out_dword(HWIO_MMSS_VBIF_VBIF_CLKON_ADDR, uRegValue);

   return HAL_MDSS_STATUS_SUCCESS;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_BurstLimitsConfig()
*/
/*!
* \brief
*     Configures Vbif burst size limits
*
* \param [in] uRead          - Maximum read burst size
* \param [in] uWrite         - Maximum write burst size
* \param [in] uFlags         - reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_Vbif_BurstLimitsConfig(
                              uint32   uRead,
                              uint32   uWrite,
                              uint32   uFlags )
{
   uint32 uRegValue = 0;

   // Configure VBIF max read/write burst size
   uRegValue = HWIO_OUT_FLD(uRegValue, VBIF_VBIF_DDR_OUT_MAX_BURST, DDR_OUT_MAX_WR_BURST, uWrite);
   uRegValue = HWIO_OUT_FLD(uRegValue, VBIF_VBIF_DDR_OUT_MAX_BURST, DDR_OUT_MAX_RD_BURST, uRead);

   out_dword(HWIO_MMSS_VBIF_VBIF_DDR_OUT_MAX_BURST_ADDR, uRegValue);

   return HAL_MDSS_STATUS_SUCCESS;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_AoooCtrlConfig()
*/
/*!
* \brief
*     Enable/Disable SW controlling AOOOWR/RD signals on AXI protocol.
*
* \param [in] bEnable - TRUE: enable SW controlling AOOOWR/RD siganls; otherwise disable;
* \param [in] uFlags  - reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_Vbif_AoooCtrlConfig(
                              bool32   bEnable,
                              uint32   uFlags )
{
   if (TRUE == bEnable)
   {
      out_dword(HWIO_MMSS_VBIF_VBIF_OUT_AXI_AOOO_EN_ADDR, HWIO_MMSS_VBIF_VBIF_OUT_AXI_AOOO_EN_RMSK);
      out_dword(HWIO_MMSS_VBIF_VBIF_OUT_AXI_AOOO_ADDR, HWIO_MMSS_VBIF_VBIF_OUT_AXI_AOOO_RMSK);
   }
   else
   {
      out_dword(HWIO_MMSS_VBIF_VBIF_OUT_AXI_AOOO_EN_ADDR, 0);
      out_dword(HWIO_MMSS_VBIF_VBIF_OUT_AXI_AOOO_ADDR, 0);
   }
   
   return HAL_MDSS_STATUS_SUCCESS;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_RoundRobinQoSArbiConfig()
*/
/*!
* \brief
*     Enable/Disable Round-Robin Arbiter with QoS signal.
*
* \param [in] bEnable - TRUE: Round-Robin Arbiter with QoS signal; otherwise disable;
* \param [in] uFlags  - reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_Vbif_RoundRobinQoSArbiConfig(
                              bool32   bEnable,
                              uint32   uFlags )
{
   if (TRUE == bEnable)
   {
      out_dword(HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_ADDR, (HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_RR_QOS_EN_BMSK      |
                                                               HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_MMU_RR_QOS_EN_BMSK) );
   }
   else
   {
      out_dword(HWIO_MMSS_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_ADDR, 0);
   }
   
   return HAL_MDSS_STATUS_SUCCESS;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_AxiPortsConfig()
*/
/*!
* \brief
*     Config AXI fixed arbiter port assignement
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_Vbif_AxiPortsConfig(void)
{
#if 0  //TODO: Vipertooth, check it again. 
    if((2 == HAL_MDP_GET_AXIPORT_COUNT()) &&
       (NULL != pgsMdpHwInfo->pAxiPortAssignment))
    {
        uint32             uRegSelVal = 0;
        uint32             uRegEnVal  = 0;
        uint32             uXinIndex  = 0;

        for(uXinIndex=0; uXinIndex<(uint32)HAL_MDP_GET_VBIFCLIENT_COUNT(); uXinIndex++)
        {
          uRegSelVal |= ((pgsMdpHwInfo->pAxiPortAssignment[uXinIndex] & HWIO_MMSS_VBIF_VBIF_FIXED_SORT_SEL0_FIXED_SORT_SEL_C0_BMSK) 
                          << (uXinIndex << 1));
          uRegEnVal  |= (pgsMdpHwInfo->pAxiPortAssignment[uXinIndex] << uXinIndex);
        }
        out_dword(HWIO_MMSS_VBIF_VBIF_FIXED_SORT_SEL0_ADDR, uRegSelVal);
        out_dword(HWIO_MMSS_VBIF_VBIF_FIXED_SORT_EN_ADDR,   uRegEnVal);
    }
#endif
    return HAL_MDSS_STATUS_SUCCESS;
}


/* -------------------------------------------------------------------------------------
 * Public functions
 * ------------------------------------------------------------------------------------ */

/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_Init()
*/
/*!
* \brief
*     Initializes VBIF settings.
*
*
* \param [in] uFlags - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Vbif_Init(uint32   uFlags)
{
   /* The settings used here are from recommendations of System Performance.
    * Some of the recommended settings are equal to the power-on defaults, 
    * such as:
    *         VBIF_ARB_CTL = 0x10 (MDP has only 1 AXI port, the setting should be 0x10 instead of 0x30)
    *         VBIF_OUT_AXI_AMEMTYPE_CONF0 = 0x22222222
    *         VBIF_OUT_AXI_AMEMTYPE_CONF1 = 0x00002222
    *         VBIF_IN_RD_LIM_CONF0 = 0x08080808
    *         VBIF_IN_RD_LIM_CONF1 = 0x08080808
    *         VBIF_IN_RD_LIM_CONF2 = 0x08080808
    *         VBIF_IN_WR_LIM_CONF0 = 0x10101010
    *         VBIF_IN_WR_LIM_CONF1 = 0x10101010
    *         VBIF_IN_WR_LIM_CONF2 = 0x10101010
    *         VBIF_OUT_RD_LIM_CONF0 = 0x00000010
    *         VBIF_OUT_WR_LIM_CONF0 = 0x00000010
    * these registers are hence not touched here, and left at their own power-on defaults.
    */
    
   /* configure VBIF clock gating */
   HAL_MDP_Vbif_Config_ClockGating(((HAL_MDP_VBIF_ENABLE_CLOCKGATING & uFlags) ? TRUE : FALSE), 0);
   
   /* configure VBIF burst limits */
   HAL_MDP_Vbif_BurstLimitsConfig(pgsMdpHwInfo->uVBIFMaxDdrRdBurstSize, pgsMdpHwInfo->uVBIFMaxDdrWrBurstSize, 0);

   /* enable SW controlling AOOOWR/RD signals on AXI protocol */
   HAL_MDP_Vbif_AoooCtrlConfig(TRUE, 0);

   /* enable Round-Robin Arbiter with QoS signal */
   HAL_MDP_Vbif_RoundRobinQoSArbiConfig(TRUE, 0);

   /* config AXI ports */
   HAL_MDP_Vbif_AxiPortsConfig();

   // Recommended VBIF settings for Gandalf MDSS
   if(NULL != pgsMdpHwInfo->sIpFxnTbl.Vbif_HwDefaultConfig)
   {
      pgsMdpHwInfo->sIpFxnTbl.Vbif_HwDefaultConfig();
   }

   return HAL_MDSS_STATUS_SUCCESS;
}



#ifdef __cplusplus
}
#endif
