/**
==============================================================================

  @file ClockBIMC.c

  This file provides clock initialization for the Apps.

==============================================================================

                             Edit History


when      who           what, where, why
--------  ------------  ------------------------------------------------------
03/02/18  avk           Initial QCS405 revision, branched from SDM660.

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockBoot.h"
#include "ClockBIMC.h"
#include "ClockHWIO.h"
#include "ClockCommon.h"
#include "ClockBSP.h"
#include "ddr_drivers.h"
#include "ddr_common.h"
#include "railway.h"
#include "smem.h"
#include "boot_util.h"
#include "busywait.h"
#include "boot_error_if.h"
/*=========================================================================
      Macro Definitions
==========================================================================*/
/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

/*
 * Common root clock command fields/masks (*_CMD_RCGR)
 */
#define HAL_CLK_CMD_RCGR_ROOT_OFF_FMSK                                0x80000000
#define HAL_CLK_CMD_RCGR_ROOT_EN_FMSK                                 0x00000002
#define HAL_CLK_CMD_CFG_UPDATE_FMSK                                   0x00000001
#define HAL_CLK_CMD_CFG_UPDATE_SHFT                                   0

/*
 * Common root clock config fields/masks (*_CFG_RCGR)
 */
#define HAL_CLK_CFG_RCGR_MODE_FMSK                                    0x00003000
#define HAL_CLK_CFG_RCGR_MODE_SHFT                                    0xc
#define HAL_CLK_CFG_RCGR_SRC_SEL_FMSK                                 0x00000700
#define HAL_CLK_CFG_RCGR_SRC_SEL_SHFT                                 0x8
#define HAL_CLK_CFG_RCGR_SRC_DIV_FMSK                                 0x0000001F
#define HAL_CLK_CFG_RCGR_SRC_DIV_SHFT                                 0

#define HAL_CLK_CFG_REG_OFFSET                                        0x4

/*
 * Supported PLL pre/post dividers.
 */
#define CLK_PLL_PREDIV_1      1
#define CLK_PLL_PREDIV_2      2
#define CLK_PLL_POSTDIV_1     1
#define CLK_PLL_POSTDIV_2     2
#define CLK_PLL_POSTDIV_4     4



/*=========================================================================
     Externs
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_SourceMapToBIMC(ClockSourceType eSource, uint32 *nMuxValue);

/*=========================================================================
      Data
==========================================================================*/
#define MAX_DDR_LEVELS 3

/* Min and Max support DDR Index */
static uint8 nDDRMinIndex = 0;
static uint8 nDDRMaxIndex; /*Updated in the Clock_InitBIMCPlan API */




/* The current rate of the DRR clock in kHz */
static uint32 ddr_speed_khz = 168000;


/*=========================================================================
      Function Definitions
=========================================================================*/
  

/* =========================================================
**  Function : Clock_SetDDRMinMax()
** =======================================================*/
/* 
 * Function is used for setting minimum and maximum DDR speed.
 *
 * @param 
 *   nDDRMin - Minimum DDR speed index.
 *   nDDRMax - Maximum DDR speed index.
 * @return 
 *   TRUE  - Min and Max are in supporting range
 *   FALSE - Min and Max are NOT in supporting range.  No setting effective.
 */
boolean Clock_SetDDRMinMax( uint8 nDDRMin, uint8 nDDRMax )
{
  /* Check if Min and Max are in supportive range */
  if ( (nDDRMin > nDDRMax) || (nDDRMin > MAX_DDR_LEVELS) )
  {
    return FALSE;
  }

  /* Over-write Max if it is higher than supportive levels */
  if ( nDDRMax > MAX_DDR_LEVELS )
  {
    nDDRMax = MAX_DDR_LEVELS;
  }

  nDDRMinIndex = nDDRMin;
  nDDRMaxIndex = nDDRMax;

  return TRUE;
}


/* =========================================================
**  Function : Clock_GetDDRMinMax()
** =======================================================*/
/* 
 * Function is used for getting minimum and maximum DDR level index.
 *
 * @param 
 *   pDDRMin - Pointer to Minimum DDR speed index.
 *   pDDRMax - Pointer to Maximum DDR speed index.
 * @return 
 *   None
 */
void Clock_GetDDRMinMax( uint8 *pDDRMin, uint8 *pDDRMax )
{
  *pDDRMin = nDDRMinIndex;
  *pDDRMax = nDDRMaxIndex;
}


/* =========================================================
**  Function : Clock_DDRSpeed()
** =======================================================*/
/* 
 * @param None
 * @return The clock rate of DDR in kHz
 */
uint32 Clock_DDRSpeed()
{
  return ddr_speed_khz;
}

void Clock_StoreDDRFreqKHz ( uint32 nFreqKHz )
{
  ddr_speed_khz = nFreqKHz;
}


/* ============================================================================
**  Function : Clock_BIMCQuery
** ============================================================================
*/
/*!
 
  This function lets the client query the BIMC frequency plan and the number 
  of supported frequencies.
  @return
  NONE
  @dependencies
  None.

*/
void Clock_BIMCQuery(ClockQueryType nQuery, void* pResource)
{
  uint32 nIdx,nCount;
  uint32 *pData;
  ClockPlanType *pBimcClkPlan;
  uint32 *pThresholdFreq;
  ClockSourceType *pDDRSource;
  ClockConfigMuxType DDRclkCfg  =  {HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),   MUX_BIMC, SRC_GPLL2,  2, 0, 0, 0};
  BIMCClockCfgType *BIMC_Cfg;
  /* Return early if no output buffer is given. */
  if ( pResource == NULL )
  {
    return;
  }

  const Clock_ConfigType *cfg = Clock_Config();
  BIMC_Cfg = (BIMCClockCfgType *) cfg->BIMC_Cfg;
  switch ( nQuery )
  {
    case CLOCK_RESOURCE_QUERY_NUM_PERF_LEVELS:
      pData = (uint32 *)pResource;
      if (pData == NULL) 
      {
        return;
      }
      *pData = 0;
       nCount = 0;
       for ( nIdx =0;(BIMC_Cfg[nIdx].nFrequency !=0); nIdx++ )
       {
              nCount ++;
       }

       *pData = nCount;
      break;

    case CLOCK_RESOURCE_QUERY_ALL_FREQ_KHZ:
      pBimcClkPlan = (ClockPlanType*)pResource;

      if (pBimcClkPlan == NULL) 
      {
        return;
      }
      for ( nIdx =0;(BIMC_Cfg[nIdx].nFrequency !=0); nIdx++ )
      {
        pBimcClkPlan[nIdx].nFreqKHz   = BIMC_Cfg[nIdx].nFrequency/1000;
        pBimcClkPlan[nIdx].eVRegLevel = BIMC_Cfg[nIdx].eVRegLevel;
        pBimcClkPlan[nIdx].eMode      = BIMC_Cfg[nIdx].Mux.DDRMux.nM;
      }
      break;

     case CLOCK_RESOURCE_QUERY_THRESHOLD_FREQ_KHZ:
      pThresholdFreq = (uint32*)pResource;
      for ( nIdx =0;(BIMC_Cfg[nIdx].nFrequency !=0); nIdx++ )
      {
        *pThresholdFreq = BIMC_Cfg[nIdx].nFrequency/1000; 
      }
       break;      

     case CLOCK_RESOURCE_QUERY_DDR_SOURCE:
       pDDRSource = (ClockSourceType*)pResource;
       if(!Clock_MuxMapToSource(&DDRclkCfg , HWIO_INF(GCC_BIMC_DDR_CFG_RCGR, SRC_SEL))) return ;
        *pDDRSource = DDRclkCfg.eSource;
        break;
    default:
      return;
  }
}

/* ========================================================================
**  Function : Clock_BIMCConfigFSM
** ======================================================================*/
/*
    Description: Configure BIMC to enable the DDR FSM.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_BIMCConfigFSM( void )
{
  /* Enable the FSM */
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS, 0);

}



/* ========================================================================
**  Function : Clock_BIMCIsFSMConfigured
** ======================================================================*/
/*
    Description: Find out if BIMC is configured enable the DDR FSM.

    @param None
    @return TRUE if FSM is enabled and FALSE otherwise.

    @dependencies
    None.

    @sa None
*/
boolean Clock_BIMCIsFSMConfigured( void )
{
  uint32 nVal;

  /* Enable the FSM */
  nVal = HWIO_INF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS);

  if (nVal == 0) 
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/* =========================================================================
**  Function : Clock_ConfigBIMCMux
** =========================================================================*/
/*!
    Configure a BIMC clock mux. BIMC clocks are configured differently
    compared to the other clocks.

    @param pConfig -  [IN] Clock mux config structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_ConfigBIMCMux (BimcMuxType* pBIMC)
{
  const ClockConfigMuxType *pConfig;

  
  /* Switch APPS AXI clock */
  pConfig = (const ClockConfigMuxType *)&pBIMC->ApssAxiMux;
  if( ! Clock_ConfigMux(pConfig)) return FALSE;
  
  pConfig = (const ClockConfigMuxType *)&pBIMC->DDRMux;
  if( ! Clock_ConfigMux(pConfig)) return FALSE;

  return TRUE;

} /* END Clock_ConfigBIMCMux */


/* =========================================================================
**  Function : Clock_SetBIMCSpeed
** =========================================================================*/
/*!
    Switch DDR and the related BIMC roots to the requested frequency

    @param -  nFreqHz [in] Frequency to search for in the table
    @return - FALSE on fail, TRUE on success

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetBIMCSpeed(uint32 nFreqKHz )
{
  uint32 nIdx;
  uint32 nFreqHz;
  boolean bResult;
  Clock_RailwayType *pClockRailway;
  ClockConfigMuxType CurrentCfg;
  const Clock_ConfigType *cfg = Clock_Config();
  const ClockConfigPLLType *pCurrentPLLCfg;
  BIMCClockCfgType BimcCfg;
  BIMCClockCfgType *BIMC_Cfg;
  ClockSourceConfigType NewSrcCfg;
  pClockRailway = Clock_RailwayConfig();
  nFreqHz = nFreqKHz*1000;
  pCurrentPLLCfg = NULL;
  bResult = TRUE;
  
   /* SPARE0 bit Field in FEATURE_CONFIG5 Register i.e. SECURITY_CONTROL.FEATURE_CONFIG5[31] 
   = 0::Feerolite Or 1::Feerolite-pro.*/
  
  /* Feerolite clock Plan :: Max DDR freq. :: 672MHz */
  BIMC_Cfg = (BIMCClockCfgType *) cfg->BIMC_Cfg;
  

  for ( nIdx =0;(BIMC_Cfg[nIdx].nFrequency !=0); nIdx++ )
  { 
    if(nFreqHz == BIMC_Cfg[nIdx].nFrequency)
    {
      /* 
       * Vote for higher voltage before the switch if the new frequency is more
       * than the current frequency.
       */
      if (ddr_speed_khz < nFreqKHz) 
      {

        // setup the vote for corner voltage with railway.
       railway_corner_vote(pClockRailway->CxVoter, BIMC_Cfg[nIdx].eVRegLevel);
        // Always need to transition the rail after updating the votes for them to be effective.
       railway_transition_rails();
      }

      BimcCfg = BIMC_Cfg[nIdx];
      
      CurrentCfg.nCMDRCGRAddr = HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR);
      if(!Clock_DetectMuxCfg(&(BIMC_Cfg[nIdx].Mux.DDRMux),&CurrentCfg))
      {
        return FALSE;
      }

      /* If frequency to be configured is not GPLL0 then we need to toggle between GPLL2 and GPLL3 */
      if ((BIMC_Cfg[nIdx].pSourceConfig != NULL) &&  (BIMC_Cfg[nIdx].pSourceConfig->ePLL != SRC_GPLL0) && (BIMC_Cfg[nIdx].pSourceConfig->ePLL != SRC_CXO))
      {
        /* 
         * We want to configure the source based on which pll is configured for bimc.
         */
        if(CurrentCfg.eSource == SRC_GPLL2)
        {
          /* 
           * New config is SRC_BIMCPLL
           */
          NewSrcCfg.PLLCfg               =  BIMC_Cfg[nIdx].pSourceConfig->PLLCfg;
          NewSrcCfg.ePLL                 =  SRC_BIMCPLL;
          NewSrcCfg.PLLCfg.nPLLModeAddr  =  HWIO_ADDR(GCC_BIMC_PLL_MODE); 
          NewSrcCfg.PLLCfg.nVoteAddr     =  0; 
          NewSrcCfg.PLLCfg.nVoteMask     =  0;
          BimcCfg.pSourceConfig          =  &NewSrcCfg;
          BimcCfg.Mux.DDRMux.eSource     =  SRC_BIMCPLL;
         
        }
        else
        {
          /* 
           * New config is GPLL2
           */
          NewSrcCfg.PLLCfg               =  BIMC_Cfg[nIdx].pSourceConfig->PLLCfg;
          NewSrcCfg.ePLL                 =  SRC_GPLL2;
          NewSrcCfg.PLLCfg.nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL2_MODE); 
          NewSrcCfg.PLLCfg.nVoteAddr     =  0; 
          NewSrcCfg.PLLCfg.nVoteMask     =  0;
          BimcCfg.pSourceConfig          =  &NewSrcCfg;
          BimcCfg.Mux.DDRMux.eSource     =  SRC_GPLL2;
         
        }
      }
      
      if(CurrentCfg.eSource == SRC_GPLL0)
      {
        pCurrentPLLCfg = &cfg->PLL0_Cfg;
      }
      else if (CurrentCfg.eSource == SRC_GPLL2) 
      {
        pCurrentPLLCfg = &cfg->PLL2_Cfg;
      }
      else if(CurrentCfg.eSource == SRC_BIMCPLL)
      {
        pCurrentPLLCfg = &cfg->BIMCPLL_Cfg;
      }
      
      if((BimcCfg.pSourceConfig != NULL) && (!Clock_IsPLLEnabled(&BimcCfg.pSourceConfig->PLLCfg)))
      {
        bResult = Clock_ConfigurePLL(&BimcCfg.pSourceConfig->PLLCfg); 
        if (bResult == TRUE) 
        {
          bResult = Clock_EnablePLL(&BimcCfg.pSourceConfig->PLLCfg);   
        }  
      }
      
      if (bResult == FALSE) 
      {
        return bResult;
      }
      if (!Clock_BIMCIsFSMConfigured()) 
      {
        /* Configure BIMC */
        Clock_BIMCConfigFSM();
      }
      

      //ddr_pre_clock_switch( ddr_speed_khz, nFreqKHz, SDRAM_INTERFACE_0 );
      //ddr_pre_clock_switch( ddr_speed_khz, nFreqKHz, SDRAM_INTERFACE_1 );
       
       
      if(!Clock_ConfigBIMCMux(&BimcCfg.Mux)) 
      {
        return FALSE;
      }
      if ((pCurrentPLLCfg != NULL) && ((CurrentCfg.eSource != SRC_GPLL0) && (CurrentCfg.eSource != SRC_CXO)) && (pCurrentPLLCfg != NULL))
      {
        Clock_DisablePLL(pCurrentPLLCfg);
      }
         
      /* Inform BIMC that new clock already switched */
      //ddr_post_clock_switch( ddr_speed_khz, nFreqKHz, SDRAM_INTERFACE_0 );
      //ddr_post_clock_switch( ddr_speed_khz, nFreqKHz, SDRAM_INTERFACE_1 );
     
      /* 
       * Vote for lower voltage after the switch if the new frequency is less
       * than the current frequency.
       */
      if (ddr_speed_khz > nFreqKHz) 
      {

        // setup the vote for corner voltage with railway.
       railway_corner_vote(pClockRailway->CxVoter, BIMC_Cfg[nIdx].eVRegLevel);

        // Always need to transition the rail after updating the votes for them to be effective.
       railway_transition_rails();
      }
      ddr_speed_khz = nFreqKHz;
      return TRUE;
    }
  }
  return FALSE;
}

