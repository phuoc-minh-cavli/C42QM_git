/** @file ClockBSP.c

  Definitions of the support clock perf level for the 8974.

  Copyright (c) 2014,2019 by Qualcomm Technologies, Inc. All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/ClockTargetLib/ClockBSP.c#7 $
  $DateTime: 2019/06/21 00:49:54 $
  $Author: pwbldsvc $

when      who           what, where, why
 --------    ---     -----------------------------------------------------------
09/05/12     vph     Set GPLL3 to 200MHz for configure BIMC @ 200MHz with Div-1
08/06/11     vtw     Added SDC clock configurations.
07/15/11     vtw     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"
#include "ClockCommon.h"
#include "ClockHWIO.h"

/*=========================================================================
      Prototypes
==========================================================================*/
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_GCCMuxMapToSource(uint32 nSource, ClockSourceType * pSource);
boolean Clock_SourceMapToAPCS(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_APCSMuxMapToSource(uint32 nSource, ClockSourceType * pSource);
boolean Clock_SourceMapToBIMC(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_BIMCMuxMapToSource(uint32 nSource, ClockSourceType * pSource);
/*=========================================================================
      Data
==========================================================================*/
static Clock_RailwayType ClockRailway =
{
  "vddcx",
  0,
  0
};

ClockSourceConfigType BIMCPLLConfig[] = 
{
  {
    /* Index 0 : BIMC_PLL/GPLL2 @ 672 MHz  */
    .ePLL = SRC_RAW,
    .PLLCfg =
    {
      .nPLLModeAddr  =  0, 
      .nVoteAddr     =  0, 
      .nVoteMask     =  0,
      .nVCO          =  1, 
      .nPreDiv       =  1, 
      .nPostDiv      =  1, 
      .nL            =  35, 
      .nM            =  0,
      .nN            =  1,
      .nConfigCtl    =  0x4001055B,
      .nAlpha        =  0x0,
      .ePLL          =  CLK_SOURCE_PLL_STROMER
      
    },
  },
  {
    /* Index 1 : BIMC_PLL/GPLL2 @ 766.8 MHz  */
    .ePLL = SRC_RAW,
    .PLLCfg =
    {
      .nPLLModeAddr  =  0, 
      .nVoteAddr     =  0, 
      .nVoteMask     =  0,
      .nVCO          =  1, 
      .nPreDiv       =  1, 
      .nPostDiv      =  1, 
      .nL            =  39, 
      .nM            =  0,
      .nN            =  1,
      .nConfigCtl    =  0x4001055B,
      .nAlpha        =  0xF000000000,
      .ePLL          =  CLK_SOURCE_PLL_STROMER
    },
  },
};

static Clock_ConfigType Clock_ConfigData =
{

  /*GPLL0 @ 800MHZ*/
  .PLL0_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL0_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_APCS_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL0),
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x29, 
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0xAAAA000000,
    .nConfigCtl    =  0x4001055B,
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  },
  /*GPLL1 @ 614.4MHZ*/    
  .PLL1_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL1_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_RPM_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL1),
    .nVCO          =  0, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x20, 
    .nM            =  0,
    .nN            =  1,
    .nConfigCtl    =  0x00031000,
    .ePLL          =  CLK_SOURCE_PLL_SR
  },
  /* GPLL2 @ 672MHz */
  .PLL2_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL2_MODE), 
    .nVoteAddr     =  0, 
    .nVoteMask     =  0,
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x23, 
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0x0000000000,
    .nConfigCtl    =  0x4001055B, 
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  },


  /* BIMCPLL @ 840MHz. */
  .BIMCPLL_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_BIMC_PLL_MODE), 
    .nVoteAddr     =  0, 
    .nVoteMask     =  0,
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x2B, 
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0xC000000000,
    .nConfigCtl    =  0x4001055B,
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  },

  /* A7SSPLL @ 998.4MHZ*/
  .A53PERFPLL_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(APCS_CPU_PLL_MODE), 
    .nVoteAddr     =  0,
    .nVoteMask     =  0,
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  2, 
    .nL            =  0x34,
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0x0,
    .nConfigCtl    =  0x4001055B,
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  },
  //TODO: check in ipcat 
  .CPU_Cfg = 
  {
    {0, {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0}},                                        /* PERF NONE */
    {19200, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_CXO,    2, 0, 0, 0}},  /* MIN - 19.2 MHz*/
    {400000, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_GPLL0,  4, 0, 0, 0}},  /* SVS - 400 MHz*/
    {499200, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_A53PERFPLL,  2, 0, 0, 0}},  /* NOM - 499.2 MHz*/
    {499200, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_A53PERFPLL,  2, 0, 0, 0}}   /* DEFAULT - 499.2 MHz */
  },
  .PCNOC_Cfg = 
  {
    {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                        /* PERF NONE */
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_CXO,  1, 0, 0, 0},  /* MIN - 19.2 MHz*/
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_GPLL0, 32, 0, 0, 0},  /* NOM - 50 MHz*/
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_GPLL0, 16, 0, 0, 0},  /* MAX - 100 MHz*/
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}   /* DEFAULT - 100 MHz (Max Nom) */
  },
#if 0
  /* SDC configuration : for backwards compatiblity to the old API */
  .SDC_Cfg =
  {
    0,       /* PERF NONE */
    400,     /* MIN - 400KHz */
    25000,   /* NOMINAL - 25MHz */
    50000,   /* MAX - 50MHz */
    25000    /* DEFAULT - SAME AS NOMINAL */
  }, /* END SDC config */
  /*  SDC extended configurations */
  .SDC1_Ext_Cfg =
  {
    {   400,  {0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, MUX_GCC, SRC_GPLL0, 32, 1, 2, 2}},
    { 50000,  {0, MUX_GCC, SRC_GPLL0, 32, 0, 0, 0}},
    {100000,  {0, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}},
    {177000,  {0, MUX_GCC, SRC_GPLL0,  9, 0, 0, 0}},
    { 0 }
  }, /* END SDC1_Ext_Cfg */
  .SDC2_Ext_Cfg =
  {
    {   400,  {0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, MUX_GCC, SRC_GPLL0, 32, 1, 2, 2}},
    { 50000,  {0, MUX_GCC, SRC_GPLL0, 32, 0, 0, 0}},
    {100000,  {0, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}},
    {200000,  {0, MUX_GCC, SRC_GPLL0, 8, 0,  0, 0}},
    { 0 }
  }, /* END SDC2_Ext_Cfg */
#endif

  /* Crypto configuration CE_Cfg : 160 MHz */
  .CE_Cfg = 
  {
    HWIO_ADDR(GCC_CRYPTO_CMD_RCGR),
    MUX_GCC, SRC_GPLL0,  /* eSource */
    10, /* nDiv2x */
    0,0,0 /* M/N:D */
  },

  /* USB configuration USB_Cfg : 133.33 MHz */
  .USB_Cfg =
  {
    HWIO_ADDR(GCC_USB_HS_SYSTEM_CMD_RCGR),
    MUX_GCC, SRC_GPLL0,  /* eSource */
    12, /* nDiv2x */
    0,0,0 /* M/N:D */
  },

  //TODO:Check
  /*UART configurations UART_cfg : 3.6864 MHz for UART1 to UART6 clocks*/
  .UART_Cfg = 
  {
    {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                                   /* PERF-NONE */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /*MIN - 3.6864 MHz */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /* NOMINAL - 3.6864 MHz MHz */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /* MAX - 3.6864 MHz MHz */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /* DEFAULT - SAME AS NOMINAL */    
  },
  
  /* RPM configuration : 177.777778	 MHz */
  .RPM_Cfg = 
  {
    HWIO_ADDR(GCC_RPM_CMD_RCGR), /*CMD RCGR register*/
    MUX_GCC, SRC_GPLL0,  /* eSource */
    9, /* nDiv2x */
    0,0,0 /* M/N:D */
  },

  .I2C_Cfg = 
  {
     {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                              /* PERF-NONE */
     {HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR), MUX_GCC, SRC_CXO,    1, 0, 0, 0}, /* MIN - 19.2 MHz */
     {HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 32, 0, 0, 0}, /* MAX - 50 MHz */
     {HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR), MUX_GCC, SRC_CXO,    1, 0, 0, 0}, /* DEFAULT - 19.2 MHz */
  },

  /* BLSP*_QUP*_SPI configurations */
  .SPI_Cfg =
  {
    { 19200, {0, MUX_GCC, SRC_CXO,    1, 0,  0,  0}},
    { 25000, {0, MUX_GCC, SRC_GPLL0,  1, 1, 32, 16}},
    { 50000, {0, MUX_GCC, SRC_GPLL0, 32, 0,  0,  0}},
    {0}
  },

    
  /* BIMC configuration BIMC_Cfg */
    .BIMC_Cfg =
    {
      {  /* BIMC configuration BIMC_Cfg : at 19.2 Mhz */
        19200000, RAILWAY_NOMINAL, NULL,
         { /* Separate clock root for BIMC and DDR.  APPS_AXI scaled along with BIMC */
           { HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),  MUX_BIMC, SRC_CXO,     2, 1, 0, 0}, /* GCC DDR Mux, ddr_clk @ 19.2MHz.6MHZ */
           { HWIO_ADDR(GCC_APSS_AXI_CMD_RCGR),  MUX_GCC,  SRC_CXO,     2, 1, 0, 0}  /* GCC APSSAXI Mux, apss_axi_clk @200MHz */
         }
      },
      {  /* BIMC configuration BIMC_Cfg : at 168 Mhz */
        168000000, RAILWAY_NOMINAL, &BIMCPLLConfig[0],
         { /* Single clock root for BIMC and DDR.  BIMC_GPU must be atleast half of BIMC */
           { HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),   MUX_BIMC,    SRC_RAW,     4, 1, 0, 0}, /*  GCC DDR Mux, ddr_clk @ 100.8MHZ */
           { HWIO_ADDR(GCC_APSS_AXI_CMD_RCGR),   MUX_GCC,     SRC_GPLL0,   16, 0, 0, 0}  /* GCC APSSAXI Mux, apss_axi_clk @100MHz */
        }
      },
      {  /* BIMC configuration BIMC_Cfg : at 336 Mhz */
         336000000, RAILWAY_NOMINAL, &BIMCPLLConfig[0],
         { /* Single clock root for BIMC and DDR.  BIMC_GPU must be atleast half of BIMC */
           { HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),  MUX_BIMC,    SRC_RAW,      2, 1, 0, 0}, /*  GCC DDR Mux, ddr_clk @ 211.2MHZ */
           { HWIO_ADDR(GCC_APSS_AXI_CMD_RCGR),  MUX_GCC,     SRC_GPLL0,    8, 0, 0, 0}  /*  GCC APSSAXI Mux, apss_axi_clk @200MHz */
         }
      },
      {  /* BIMC configuration BIMC_Cfg : at 383.4 Mhz */
         383400000, RAILWAY_TURBO, &BIMCPLLConfig[1],
         { /* Single clock root for BIMC and DDR.  BIMC_GPU must be atleast half of BIMC */
           { HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),  MUX_BIMC,    SRC_RAW,     2, 1, 0, 0}, /* GCC DDR Mux, ddr_clk @ 297.6MHZ */
           { HWIO_ADDR(GCC_APSS_AXI_CMD_RCGR),  MUX_GCC,     SRC_GPLL0,   8, 0, 0, 0}  /* GCC APSSAXI Mux, apss_axi_clk @200MHz */
         }
      },
      {0},
    },
};


boolean (*Clock_MuxMap[NUM_MUX_TYPES])(ClockSourceType, uint32 *) =
{
  Clock_SourceMapToGCC,
  Clock_SourceMapToAPCS,
  NULL,
  Clock_SourceMapToBIMC
};

boolean (*Clock_SourceMap[NUM_MUX_TYPES])(ClockSourceType, uint32 *) =
{
  Clock_GCCMuxMapToSource,
  Clock_APCSMuxMapToSource,
  NULL,
  Clock_BIMCMuxMapToSource
};

/*=========================================================================
      Functions
==========================================================================*/

/* ============================================================================
**  Function : Clock_RailwayCfg
** ============================================================================
*/
/*!
    Return a pointer to the Railway configuration data.

   @param  None

   @retval a pointer to the Railway configuration data

*/

Clock_RailwayType *Clock_RailwayConfig( void )
{
  return &ClockRailway;
}

/* ============================================================================
**  Function : Clock_Config
** ============================================================================
*/
/*!
    Return a pointer to the SBL configuration data.

   @param  None

   @retval a pointer to the SBL configuration data

*/
Clock_ConfigType *Clock_Config( void )
{
  return &Clock_ConfigData;
}

/* ============================================================================
**  Function : Clock_SourceMapToMux
** ============================================================================
*/
boolean Clock_SourceMapToMux
(
  const ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
)
{
  if( (pConfig == NULL) ||
      (nMuxValue == NULL) ||
      pConfig->eMux >= NUM_MUX_TYPES )
  {
    return FALSE;
  }

  return Clock_MuxMap[pConfig->eMux](pConfig->eSource, nMuxValue);
}


/* ============================================================================
**  Function : Clock_MuxMapToSource
** ============================================================================
*/
boolean Clock_MuxMapToSource
(
  ClockConfigMuxType *pConfig,
  uint32 nSource
)
{
  if( (pConfig == NULL) ||
      pConfig->eMux >= NUM_MUX_TYPES )
  {
    return FALSE;
  }

  return Clock_SourceMap[pConfig->eMux](nSource,&(pConfig->eSource));
}


/* ============================================================================
**  Function : Clock_SourceMapToGCC
** ============================================================================
*/
/*!
   Map the source enumeration to a physical mux setting for GCC.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the SBL configuration data

*/
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 1;
      break;
    case SRC_BIMCPLL:
      *nMuxValue = 2;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

 /* ============================================================================
 **  Function : Clock_GCCMuxMapToSource
 ** ============================================================================
 */

 /*!
   Map the source enumeration to a physical mux setting for GCC.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the SBL configuration data

*/
boolean Clock_GCCMuxMapToSource(uint32 nSource, ClockSourceType* pSource)
{
  switch( nSource )
  {
    case 0:
      *pSource = SRC_CXO;
      break;
    case 1:
      *pSource = SRC_GPLL0;
      break;
    case 2:
      *pSource = SRC_BIMCPLL;
    default:
      return FALSE;
  }
  return TRUE;
}


/* ============================================================================
**  Function : Clock_SourceMapToAPCS
** ============================================================================
*/
/*!
   Map a ClockSourceType into a physical mux setting for the APCS muxes.

   @param  None

   @retval a pointer to the SBL configuration data

*/
boolean Clock_SourceMapToAPCS(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 1;
       break;
    case SRC_GPLL1:
      *nMuxValue = 2;
      break;
    case SRC_GPLL2:
      *nMuxValue = 3;
      break;
    case SRC_A53PERFPLL:
      *nMuxValue = 5;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

/* ============================================================================
**  Function : Clock_APCSMuxMapToSource
** ============================================================================
*/

/*!
   Map the source enumeration to a physical mux setting for APCS.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the SBL configuration data

*/

boolean Clock_APCSMuxMapToSource(uint32 nSource, ClockSourceType* pSource)
{
  switch( nSource )
  {
    case 0:
      *pSource = SRC_CXO;
      break;
    case 1:
      *pSource = SRC_GPLL0;
      break;
    case 2:
      *pSource = SRC_GPLL1;
      break;
    case 3:
      *pSource = SRC_GPLL2;
      break;
    case 5:
      *pSource = SRC_A53PERFPLL;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}


/* ============================================================================
**  Function : Clock_SourceMapToBIMC
** ============================================================================
*/
/*!
   Map a ClockSourceType into a physical mux setting for the BIMC_DDR and DDR mux.

   @param  None

   @retval a pointer to the SBL configuration data

*/
boolean Clock_SourceMapToBIMC(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 1;
      break;
    case SRC_BIMCPLL:
      *nMuxValue = 2;
      break;
    case SRC_GPLL2:
      *nMuxValue = 3;
      break;
    default:
      return FALSE;
  }
  return TRUE;
} 

/* ============================================================================
**  Function : Clock_BIMCMuxMapToSource
** ============================================================================
*/

/*!
   Map the source enumeration to a physical mux setting for BIMC.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the SBL configuration data

*/

boolean Clock_BIMCMuxMapToSource(uint32 nSource, ClockSourceType* pSource)
{
  switch( nSource )
  {
    case 0:
      *pSource = SRC_CXO;
      break;
    case 1:
      *pSource = SRC_GPLL0;
      break;
    case 2:
      *pSource = SRC_BIMCPLL;
      break; 
    case 3:
      *pSource = SRC_GPLL2;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}


/* ============================================================================
**  Function : Clock_EnableSource
** ============================================================================
*/

boolean Clock_EnableSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;

    case SRC_GPLL0:
      rtrn = Clock_EnablePLL(&Clock_ConfigData.PLL0_Cfg);
      /*Enabling Aux output for QDSS, APSS-TCU*/
      HWIO_OUTF(GCC_GPLL0_USER_CTL, PLLOUT_LV_AUX, 1);
      break;
      
    case SRC_GPLL1:
      rtrn = Clock_EnablePLL(&Clock_ConfigData.PLL1_Cfg);
      /*Enabling Aux output for Cam SS*/
      HWIO_OUTF(GCC_GPLL1_USER_CTL, PLLOUT_LV_AUX, 1);
      break;

    case SRC_GPLL2:
      rtrn = Clock_EnablePLL(&Clock_ConfigData.PLL2_Cfg);
      /*Enabling Aux output for QDSS*/
      /*None of the clocks using, for now enabled will disable after sod*/
      HWIO_OUTF(GCC_GPLL2_USER_CTL, PLLOUT_LV_AUX , 1);
      break; 

    case SRC_BIMCPLL:
      rtrn = Clock_EnablePLL(&Clock_ConfigData.BIMCPLL_Cfg);
      /* Early Output for bimc_ddr_clk_src */
      HWIO_OUTF(GCC_BIMC_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;
   
    case SRC_A53PERFPLL:
      rtrn = Clock_EnablePLL(&Clock_ConfigData.A53PERFPLL_Cfg);
      HWIO_OUTF(APCS_CPU_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      HWIO_OUTF(APCS_CPU_PLL_USER_CTL, PLLOUT_LV_AUX, 1);
      break; 

    default:
      break;
  }
  return rtrn;
}

/* ============================================================================
**  Function : Clock_ConfigureSource
** ============================================================================
*/
boolean Clock_ConfigureSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;
    case SRC_GPLL0:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL0_Cfg);
      /*Enabling Aux output for QDSS, APSS-TCU*/
      HWIO_OUTF(GCC_GPLL0_USER_CTL, PLLOUT_LV_AUX, 1);
      break;

    case SRC_GPLL1:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL1_Cfg);
      /*Enabling Aux output for Cam SS*/
      HWIO_OUTF(GCC_GPLL1_USER_CTL, PLLOUT_LV_AUX, 1);

     break;
    case SRC_GPLL2:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL2_Cfg);
      /*Enabling Aux output for QDSS*/
      /*None of the clocks using, for now enabled will disable after sod*/
      HWIO_OUTF(GCC_GPLL2_USER_CTL, PLLOUT_LV_AUX , 1);
      break; 

    case SRC_BIMCPLL:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.BIMCPLL_Cfg);
      /* Early Output for bimc_ddr_clk_src */
      HWIO_OUTF(GCC_BIMC_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;
    case SRC_A53PERFPLL:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.A53PERFPLL_Cfg);
      HWIO_OUTF(APCS_CPU_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      HWIO_OUTF(APCS_CPU_PLL_USER_CTL, PLLOUT_LV_AUX, 1);
      break;

    default:
      break;
  }
  return rtrn;
}

/* ============================================================================
**  Function : Clock_DisableSource
** ============================================================================
*/
boolean Clock_DisableSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;

    case SRC_GPLL0:
      /* There is currently no use case to disable GPLL0 in XBL Loader. */
      break;

    case SRC_GPLL1:
      rtrn = Clock_DisablePLL(&Clock_ConfigData.PLL1_Cfg);
      break;

    case SRC_BIMCPLL:
      rtrn = Clock_DisablePLL(&Clock_ConfigData.BIMCPLL_Cfg);
      break;
    
    default:
      break;
  }
  return rtrn;
}

