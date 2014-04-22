/*
===========================================================================
  @file ClockXBL.c

  This file provides clock initialization for the Apps.
===========================================================================

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
#include "ClockBSP.h"
#include "ClockCommon.h"
#include "ClockBoot.h"
#include "ClockBIMC.h"
#include "ClockHWIO.h"
#include "railway.h"
#include "busywait.h"
#include "boot_error_if.h"
#include "boot_build_component.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/
/*=========================================================================
     Externs
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

boolean Clock_InitCrypto(void);
void Clock_BIMCConfigFSM( void );
boolean Clock_BIMCIsFSMConfigured( void );
void Clock_Dummy_Enable_SR_PLL(void);
void Clock_ReplaceUSBBootClockEnable();
boolean Clock_QPICConfig( void );
boolean Clock_PRNGConfig( void );
boolean Clock_InitPLLStatic( void );
extern boolean Clock_CopyBIMCPlanToRPM( void );
/*=========================================================================
      Function Definitions
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/
/*=========================================================================
      Function Definitions
=========================================================================*/
/* ============================================================================
**  Function : Clock_DebugInit
** ============================================================================
*/
/*!

    This function is called very early in boot.  It is used for work-arounds that
    need to be done before JTAG attaches at the SBL "hold focus button" spin loop.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    You cannot use busywait in this function.

*/
boolean Clock_DebugInit(void)
{


  /* Enabling the gcc_pcnoc_usb2_clk for gcc_usb_hs_system_clk
   * This pcnoc_usb2_clk is needed to turn on the gcc_usb_hs_system_clk with NOC_HANDSHAKE_FSM=1 
   * Never turn-off these clocks as it may cause NOC-ERRORs \
   * due to hardware data trnsfer completion may not in sync with turing off the clocks
   */
  //Clock_ToggleClock(HWIO_ADDR(GCC_PCNOC_USB2_CBCR), CLK_TOGGLE_ENABLE);
 // Clock_ToggleClock(HWIO_ADDR(GCC_PCNOC_USB3_CBCR), CLK_TOGGLE_ENABLE);

  /* Enable QDSS register access */
  Clock_ToggleClock(HWIO_GCC_GCC_IM_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  /* Needed for Random Stack Canary */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);

  /* SPMI clocks are already enabled by default on reset */
  /* gcc_spmi_ser_clk default on */
  /* gcc_spmi_pcnoc_ahb_clk default on RPM is voting for this*/
  /* gcc_spmi_ahb_clk default on */

  /*Setting these bits to default value in case warm reset does not clear it*/
  //HWIO_OUT(GCC_LPASS_RESTART, 0x0);
 // HWIO_OUT(GCC_WCSS_RESTART,  0x0);
 // HWIO_OUT(GCC_CDSP_RESTART,  0x0);
 // HWIO_OUT(GCC_WDSP_RESTART,  0x0);
  if( ! Clock_InitPLLStatic()) return FALSE;

  return TRUE;
}

/* ========================================================================
**  Function : Clock_I2CInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for EEPROM to be used Pre DDR.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_I2CInit( void )
{
  /* Enable BLSP1 and BLSP2 AHB CLK */
    HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA, 1);
    HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_SLEEP_CLK_ENA, 1);
    //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_AHB_CLK_ENA, 1);
    //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_SLEEP_CLK_ENA, 1);

    while (HWIO_INF(GCC_BLSP1_AHB_CBCR, CLK_OFF) == 1);
    //while (HWIO_INF(GCC_BLSP2_AHB_CBCR, CLK_OFF) == 1);

  /* Check with Storage Team for the I2C QUP instance for EEPROM access then enable its i2c_apps_clock */
  Clock_ToggleClock(HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CBCR), CLK_TOGGLE_ENABLE);

  /* These clocks can be disabled at Clock_ExitBoot, so keep that function
   * in sync 
   */

  return TRUE;
}

/* ========================================================================
**  Function : Clock_InitVotes
** ======================================================================*/
/*
    Description: This function serves two purposes.  It clears 
    unwanted votes that may be left after watchdog or JTAG reset, which 
    does not actually reset all of GCC.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_InitVotes( void )
{
  uint32 rpm_gpll_votes = 0;

  /* 
   * Correct any left over votes from a watchdog or JTAG induced reset 
   */

  /* RPM needs GPLL0 and maybe GPLL1 if it was configured in PBL */
  if(HWIO_INF(GCC_GPLL0_MODE, PLL_VOTE_FSM_ENA ) > 0) {
    rpm_gpll_votes |= HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL0);
  }

  HWIO_OUT(GCC_RPM_GPLL_ENA_VOTE, rpm_gpll_votes);

  /* APCS needs GPLL0 and maybe GPLL4 if SDCC1 clock is derived from it as SDCC1 clks Initialized before this API
   * Note: Check the ClockBSP.c for PLLs being used in loader and handle the votes appropriately..
   */

 // if(HWIO_INF(GCC_GPLL4_MODE, PLL_VOTE_FSM_ENA ) > 0) {
    //rpm_gpll_votes |= HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL4);
  //}

  /* APCS requires the same PLLs */
  HWIO_OUT(GCC_APCS_GPLL_ENA_VOTE, rpm_gpll_votes);

  /* Everybody else has not booted yet */
  HWIO_OUT(GCC_SPARE_GPLL_ENA_VOTE, 0);
  //HWIO_OUT(GCC_CDSP_GPLL_ENA_VOTE, 0);
  HWIO_OUT(GCC_APCS_HYP_GPLL_ENA_VOTE, 0);
  //HWIO_OUT(GCC_WDSP_GPLL_ENA_VOTE, 0);
  //HWIO_OUT(GCC_LPASS_DSP_GPLL_ENA_VOTE, 0);
}

/* ========================================================================
**  Function : Clock_PreDDRInitEx
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_PreDDRInitEx( uint32 ddr_type )
{
  uint32 mask;
  uint32 value;
  
  /* Twizy using Gen1 PHY, in Gen1 PHY DDR freq = 1/2 (BIMC freq) */
  ClockConfigMuxType DDRclkCfg  =    {HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),  MUX_BIMC, SRC_GPLL2,  4, 0, 0, 0};  

  ClockConfigMuxType apssaxiclkCfg = {HWIO_ADDR(GCC_APSS_AXI_CMD_RCGR),  MUX_GCC, SRC_GPLL0,  8, 0, 0, 0};     //NOM=200


  /* SR PLL Workaround */
  Clock_Dummy_Enable_SR_PLL();


  /* Clean up from watchdog/JTAG reset */
  Clock_InitVotes();

  /* Enable Imem and Msg RAM clock */
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA, 1);

  /* Enable the necessary voteable KPSS and BIMC related clocks */
  //TODO: gcc_sys_noc_axi_clk is default ON, gcc_sys_noc_tprb_clk is not turned on and POR is OFF
  mask = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PCNOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA);

  value = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PCNOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) | 
    ~HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA);

  HWIO_OUTM(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, mask, value);
  
  //HWIO_OUTF(GCC_APCS_CLOCK_SLEEP_ENA_VOTE, BIMC_APSS_AXI_CLK_SLEEP_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA, 0);

  /* Enable all possible BIMC and DDR clocks, (K) means KPSS_Boot_Clk_CTL On.
     bimc_ddr_xo_clk_src, root auto enables. */
  Clock_ToggleClock(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  
  /* gcc_ddr_dim_cfg_clk */
  Clock_ToggleClock(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  /* gcc_ddr_dim_sleep_clk */
  Clock_ToggleClock(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  /* gcc_bimc_xo_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  
  /* gcc_bimc_cfg_ahb_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  
  /* gcc_bimc_sleep_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  
  /* (K)gcc_bimc_sysnoc_axi_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_PCNOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  
  /* (K)gcc_bimc_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_CBCR_ADDR, CLK_TOGGLE_ENABLE); 

  //Enabling PCNOC_XO for PC_NOC Access
  Clock_ToggleClock(HWIO_GCC_PCNOC_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE); 

  if( ! Clock_EnableSource( DDRclkCfg.eSource )) return FALSE;

  HWIO_OUTF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS, 1);
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_DDR_LEGACY_2X_MODE_EN, 1);

  if( ! Clock_ConfigMux(&DDRclkCfg)) return FALSE;
  if( ! Clock_EnableSource( apssaxiclkCfg.eSource ) || ! Clock_ConfigMux(&apssaxiclkCfg)) return FALSE;
  
  /*
   * Trigger the FSM update manually and wait for the frequency to switch.
   */
  if(! Clock_TriggerUpdate(HWIO_ADDR(GCC_BIMC_MISC),
                           HWIO_FMSK(GCC_BIMC_MISC, BIMC_FSM_DIS_DDR_UPDATE))) return FALSE;

 /* Wait for the imem_axi and msg_ram_ahb clocks to be ON */
  while(HWIO_INF(GCC_IMEM_AXI_CBCR , CLK_OFF))
  {
    busywait(1);
  }
  while(HWIO_INF(GCC_MSG_RAM_AHB_CBCR , CLK_OFF))
  {
    busywait(1);
  }
  return TRUE;
}

/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Clock_PreDDRInitEx() was added in 8974 to provide a separate API
    for emergency download (EDL) without including Mxdroop work around. Adding wrapper 
    function to avoid compilation erros in 8x26 and 8x10.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_PreDDRInit( uint32 ddr_type )
{

  /* Init BIMC clock plan for the different types of DDRs. */
 // Clock_InitBIMCPlan();

  
  Clock_RailwayType *pClockRailway;
  /* Initialize CX Rail ID */
  pClockRailway = Clock_RailwayConfig();
  pClockRailway->nCxRailId = rail_id(pClockRailway->CxRail);

  if(RAIL_NOT_SUPPORTED_BY_RAILWAY != pClockRailway->nCxRailId)
  {
    pClockRailway->CxVoter = railway_create_voter(pClockRailway->nCxRailId,RAILWAY_CLOCK_DRIVER_VOTER_ID);                                                            
  }
  else
  {
    return FALSE;
  }

  return Clock_PreDDRInitEx( ddr_type );
}
void  Clock_BringBiasPLLOutOfReset(void)
{
  
  /* Wait for the PLL to be Locked */
    
}
/* ============================================================================
**  Function : Clock_Init
** ============================================================================
*/
/*!

    This function turns on the required clocks and configures
    Fabric and Scorpion speeds depending on the System Fabric and
    CPU boot performance level.

    @param eSysPerfLevel   -  [in] Fabric and DDR performance level to initialize.
    @param eCPUPerfLevel   -  [in] Scropion CPU performance level to initialize.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/

boolean Clock_Init
(
  ClockBootPerfLevelType eSysPerfLevel,
  ClockBootPerfLevelType eCPUPerfLevel
)
{
  Clock_ConfigType *cfg = Clock_Config();
  ClockConfigMuxType *RPMCfg;
  RPMCfg = &cfg->RPM_Cfg;
  
  if( ! Clock_SetCPUPerfLevel(eCPUPerfLevel)) return FALSE;
  //L2 is in sync in each cluster. CCI is for Cache coherency. Configure CCI perf level*/
  //if( ! Clock_SetL2PerfLevel(eCPUPerfLevel)) return FALSE; 
  if( ! Clock_SetSysPerfLevel(eSysPerfLevel)) return FALSE;
  if( ! Clock_InitCrypto()) return FALSE;

  Clock_ReplaceUSBBootClockEnable();
  if( ! Clock_QPICConfig()) return FALSE;
  //if( ! Clock_PRNGConfig()) return FALSE;

  if (!Clock_EnableSource(RPMCfg->eSource)) return FALSE;
  if (!Clock_ConfigMux(RPMCfg))  return FALSE;

  /* Enable the DCD */
  HWIO_OUTF(PCNOC_DCD_QXCLK_CONFIG, DCDEN, 1);

  //HWIO_OUTF(GCC_RPM_MISC, RPM_CLK_AUTO_SCALE_DIS, 0);
  //HWIO_OUTF(GCC_RPM_MISC,RPM_CLK_AUTO_SCALE_DIV, 0xF);

  HWIO_OUTF(GCC_APSS_AHB_MISC , APSS_AHB_CLK_AUTO_SCALE_DIS , 0);
  HWIO_OUTF(GCC_APSS_AHB_MISC , APSS_AHB_CLK_AUTO_SCALE_DIV , 0XF);

  Clock_BringBiasPLLOutOfReset();
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SetSysPerfLevel
** ============================================================================
*/
/**
  Configure System and Application Fabric to a perf level.

  @param eSysPerfLevel [in]  -  System NOC and DDR performance level to configure.

  @return
  TRUE -- Fabric was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetSysPerfLevel
(
  ClockBootPerfLevelType eSysPerfLevel
)
{
  /* Configure SysNOC, PCNOC */
  /* Configure BIMC FSM. BIMC already configured in Clock_PreDDRinit*/
  const ClockConfigMuxType *clkCfg;
  Clock_ConfigType *cfg = Clock_Config();

  if(eSysPerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;
  /* Assert if turbo is passed */
  BL_VERIFY(eSysPerfLevel != CLOCK_BOOT_PERF_TURBO, BL_ERROR_GROUP_CLK);


  if (!Clock_BIMCIsFSMConfigured()) 
  {
    /* Configure BIMC */
    Clock_BIMCConfigFSM();
  }

  /* Configure PCNOC */
  clkCfg = &cfg->PCNOC_Cfg[eSysPerfLevel];
  if( ! Clock_EnableSource( clkCfg->eSource )) return FALSE;
  if( ! Clock_ConfigMux(clkCfg)) return FALSE;

  return TRUE;
}

/* ============================================================================
**  Function : Clock_InitCrypto
** ============================================================================
*/
/*!
    Configure Crypto clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_InitCrypto(void)
{
  const ClockConfigMuxType *CECfg;
  Clock_ConfigType *cfg = Clock_Config();
  CECfg = &cfg->CE_Cfg;

  if( ! Clock_EnableSource( CECfg->eSource )) return FALSE;
  if( ! Clock_ConfigMux(CECfg)) return FALSE;

  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, CRYPTO_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AHB_CLK_ENA, 1);

  /* Revoke APCS CE1 clocks which are enable in PBL, since RPM will manage these clocks */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_CLK_ENA, 0);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AXI_CLK_ENA, 0);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AHB_CLK_ENA, 0 );

  /* Wait for the voted clocks to be ON */
  while(HWIO_INF(GCC_CRYPTO_CBCR, CLK_OFF))
  {
    busywait(1);
  }
  while(HWIO_INF(GCC_CRYPTO_AXI_CBCR, CLK_OFF))
  {
    busywait(1);
  }
  while(HWIO_INF(GCC_CRYPTO_AHB_CBCR, CLK_OFF))
  {
    busywait(1);
  }

  return TRUE;

} /* END Clock_InitCrypto */

/* ============================================================================
**  Function : Clock_InitForDownloadMode
** ============================================================================
*/
/*!
    Configure clocks for download.  Enable every RAM we want to dump.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
void Clock_InitForDownloadMode(void)
{
  uint32 mask;

  /* Enable the necessary voteable KPSS and BIMC related clocks */
  mask = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PCNOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA) |
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BOOT_ROM_AHB_CLK_ENA) |
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, SPMI_PCNOC_AHB_CLK_ENA);
    

  HWIO_OUTM(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, mask, mask);

   /* Enable Imem and Msg RAM clock */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);

  /*Enable the clocks for accessing the DCC_SRAM */
  HWIO_OUTF(GCC_DCC_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_DCC_XO_CBCR, CLK_ENABLE, 1);

}

/* ============================================================================
**  Function : Clock_ExitBoot
** ============================================================================
*/
/*!

    This function turns off clocks that were used during boot, but are not
    needed after boot.  This should be called at the end of boot.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/
boolean Clock_ExitBoot(void)
{
  /* Disable BLSP1 and BLSP2 AHB CLKs */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA, 0);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_SLEEP_CLK_ENA, 0);
  //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_AHB_CLK_ENA, 0);
  //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_SLEEP_CLK_ENA, 0);

  /* Disable PRNG */
  //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 0);
  /*Disable ECC */
  HWIO_OUTF(GCC_ECC_CBCR, CLK_ENABLE, 0);
  HWIO_OUTF(GCC_ECC_CORE_CBCR, CLK_ENABLE, 0);
  /*Enabling the clock(4.8Mhz fixed) for SNOC QOS support.*/
  //HWIO_OUTF(GCC_SNOC_QOSGEN_EXTREF_CTL, ENABLE, 1);

  /*Disabling USB_HS_PHY_PHY_AHB Clock*/
  HWIO_OUTF(GCC_USB_HS_PHY_CFG_AHB_CBCR , CLK_ENABLE, 0);

  return TRUE;
}

extern uint32 clock_apps_speed_khz ;



/* ============================================================================
**  Function : Clock_GetClockFrequency
** ============================================================================
*/
/*!
    Get the requested clock frequency in hertz.

    @param
      eBootLogClock [in]  - targeted clock
      pnFrequencyHz [out] - frequency of clock in hertz

    @return
      True if the requested clock frequency is placed in pnFrequencyHz.

    @dependencies
    None.

    @sa None
*/

boolean Clock_GetClockFrequency(ClockBootLogQueryType eBootLogClock,
                                uint32 *pnFrequencyHz)
{
  boolean ret = TRUE;

  switch(eBootLogClock)
  {
  case CLK_BOOT_LOG_APPS_CLUSTER_0:
    *pnFrequencyHz = (clock_apps_speed_khz * 1000);
    break;

  case CLK_BOOT_LOG_DDR:
    *pnFrequencyHz = (Clock_DDRSpeed() * 1000);
    break;

  default:
    ret = FALSE;
    break;
  }

  return ret;
}

/* =========================================================================
**  Function : Clock_Dummy_Enable_SR_PLL
** =========================================================================*/
void Clock_Dummy_Enable_SR_PLL(void)
{
   /*GPLL1 dummy enable vote */
    HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_ENA , 0x1); /* FSM = Enabled*/

    HWIO_OUTF(GCC_RPM_GPLL_ENA_VOTE, GPLL1, 0x1);

    busywait(50); 

    HWIO_OUTF(GCC_RPM_GPLL_ENA_VOTE, GPLL1, 0x0);

}

boolean Clock_InitRPM( void )
{
    //return Clock_CopyBIMCPlanToRPM();
    return TRUE;
}


/* ========================================================================
**  Function : Clock_QPICConfig
** ======================================================================*/
/*
    Description: Configure QPIC(NAND) clocks at 100MHz on GPLL0.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
boolean Clock_QPICConfig( void )
{
  /* A mux config for QPIC clock to 100MHz*/
  const ClockConfigMuxType QPICCfg =
  {
    .nCMDRCGRAddr = HWIO_ADDR(GCC_QPIC_CMD_RCGR),
    .eMux = MUX_GCC,
    .eSource = SRC_GPLL0,
    .nDiv2x = 16,  /* div8*/
    .nM = 0,
    .nN = 1,
    .n2D = 0
  };
  
  /* A mux config for QPIC clock to 320MHz*/
  const ClockConfigMuxType QPICIOCfg =
  {
    .nCMDRCGRAddr = HWIO_ADDR(GCC_QPIC_IO_MACRO_CMD_RCGR),
    .eMux = MUX_GCC,
    .eSource = SRC_GPLL0,
    .nDiv2x = 5,  /* div2.5*/
    .nM = 0,
    .nN = 1,
    .n2D = 0
  };
    
  /* Configure QPIC clock to 100MHz */
  if(!Clock_ConfigMux( &QPICCfg ) )
    return FALSE;
  
  /* Configure QPIC clock to 400MHz */
  if(!Clock_ConfigMux( &QPICIOCfg ) )
    return FALSE;

#if 0 
//TODO: This is added extra in the 9x45 target do we need for Vipertooth??
  /*
   * Enable the QPIC clocks explicitly for EMMC builds.
   * These clocks will be enabled in PBL for all other images.
   */
  if ( ((HWIO_BOOT_CONFIG_INM(HWIO_BOOT_CONFIG_FAST_BOOT_BMSK) \
         >> HWIO_BOOT_CONFIG_FAST_BOOT_SHFT) & BOOT_CONFIG_EMMC_USB) == BOOT_CONFIG_EMMC_USB)
  {
     result|= Clock_ToggleClock(HWIO_GCC_QPIC_CBCR_ADDR, CLK_TOGGLE_ENABLE);
     result|= Clock_ToggleClock(HWIO_GCC_QPIC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
     result|= Clock_ToggleClock(HWIO_GCC_QPIC_SYSTEM_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  }
#endif
  return TRUE;
}

/* ========================================================================
**  Function : Clock_PRNGConfig
** ======================================================================*/
/*
    Description: Configure PRNG clocks at 80MHz on GPLL0.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/

boolean Clock_PRNGConfig(void)
{
  /* A mux config for QPIC clock to 80MHz*/
  const ClockConfigMuxType PRNGCfg =
  {
    .nCMDRCGRAddr = HWIO_ADDR(GCC_PRNG_CMD_RCGR),
    .eMux = MUX_GCC,
    .eSource = SRC_GPLL0,
    .nDiv2x = 20,  /* div10*/
    .nM = 0,
    .nN = 1,
    .n2D = 0
  };
  
  /* Configure PRNG clock to 100MHz */
  if(!Clock_ConfigMux( &PRNGCfg ) )
    return FALSE;

  return TRUE;
}

/* ============================================================================
**  Function : Clock_InitPLLStatic()
** ============================================================================
*/
/**
  Perform initial static configurations and workarounds
  defined in frequency plan documentation,

  @param
  @return
  TRUE -- Initialization succeeded.
  FALSE -- Initialization failed.

  @dependencies
  None.

*/
boolean Clock_InitPLLStatic( void )
{
 
 /* 
  * Clock_ConfigureSource calling Clock_ConfigurePLL API which is setting config_ctl for all the PLLs.
  * Clock_ConfigureSource API setting the test_ctl for gpll4 only, update other as per HSR
  */

  /* Clock_ConfigureSource will enable  early output for A53PLL, used this early output in the UEFI / HLOS */ 
  Clock_ConfigureSource(SRC_GPLL0);
  Clock_ConfigureSource(SRC_GPLL1);
  Clock_ConfigureSource(SRC_GPLL2);
  Clock_ConfigureSource(SRC_BIMCPLL);
  Clock_ConfigureSource(SRC_A53PERFPLL); 

  return TRUE;

}
