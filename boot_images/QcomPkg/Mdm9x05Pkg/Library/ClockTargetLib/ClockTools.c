/**
==============================================================================

  @file ClockTools.c

  This file provides clock initialization for starting SDCC clocks at boot.

==============================================================================

                             Edit History


when      who           what, where, why
--------  ------------  ------------------------------------------------------
09/22/16    vg          Removed null entry in Clock_RegType 
08/11/15  jcuddihy      Initial SDM660 revision, branched from MSM8996.

==============================================================================
            Copyright (c) 2015-2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockBSP.h"
#include "ClockHWIO.h"
#include "ClockCommon.h"


boolean Clock_UART3ConfigMux (const ClockConfigMuxType *pConfig);

/*=========================================================================
      Data
==========================================================================*/
struct Clock_RegType 
{
  uint32 nCmd;
  uint32 nApps;
};

 /* NOTE: Make sure to switch the Cx rail to turbo if turbo frequency is requested,
  * But here NOM and TURBO both are of same timing closure values, so no rail-switching required.
  */

const struct Clock_RegType Clock_UARTRegs[CLK_BLSP_UART_NUM_CLKS] = 
{
  { 0,0 },    // CLK_BLSP_UART_NONE,
  { 0,0 },    // CLK_BLSP0_UART0_APPS,
  { 0,0 },    // CLK_BLSP0_UART1_APPS,
  { 0,0 },    // CLK_BLSP0_UART2_APPS,
  { 0,0 },    // CLK_BLSP0_UART3_APPS,
  { 0,0 },    // CLK_BLSP0_UART4_APPS,
  { 0,0 },    // CLK_BLSP0_UART5_APPS,
  { 0,0 },    // CLK_BLSP0_UART6_APPS,
  { 0,0 },//{ HWIO_ADDR(GCC_BLSP1_UART0_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_UART0_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_UART1_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_UART2_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_UART2_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_UART3_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_UART3_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_UART4_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_UART4_APPS_CBCR) },
  { 0,0 },    // CLK_BLSP1_UART5_APPS
  { 0,0 },    // CLK_BLSP1_UART6_APPS
  { 0,0 },//{ HWIO_ADDR(GCC_BLSP2_UART0_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP2_UART0_APPS_CBCR) },
  { 0,0 },    // CLK_BLSP2_UART1_APPS
  { 0,0 },    // CLK_BLSP2_UART2_APPS
  { 0,0 },    // CLK_BLSP2_UART3_APPS
  { 0,0 },    // CLK_BLSP2_UART4_APPS
  { 0,0 },    // CLK_BLSP2_UART5_APPS
  { 0,0 },    // CLK_BLSP2_UART6_APPS
};

const struct Clock_RegType Clock_I2CRegs[CLK_BLSP_QUP_I2C_NUM_CLKS] = 
{ 
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP0 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP1 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP2 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP3 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP4 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP5 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP6 is not supported in qcs6200*/
  { 0,  0 },//{ HWIO_ADDR(GCC_BLSP1_QUP0_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP0_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP3_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP4_I2C_APPS_CBCR) },
  { 0,  0 }, /*Added Dummy entry as BLSP1 QUP5 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP1 QUP6 is not supported in qcs6200*/
  { 0,  0 },//{ HWIO_ADDR(GCC_BLSP2_QUP0_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP2_QUP0_I2C_APPS_CBCR) },
  { 0,  0 }, /*Added Dummy entry as BLSP2 QUP1 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP2 QUP2 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP2 QUP3 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP2 QUP4 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP2 QUP5 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP2 QUP6 is not supported in qcs6200*/  
};


struct Clock_SDCCRegAddrType
{
  uint32 nCmd;
  uint32 nApps;
  uint32 nAHB;
};

const struct Clock_SDCCRegAddrType Clock_SDCCRegs[CLK_SDC_NUM_CLKS] = 
{ 
  {0,0,0}, /* CLK_SDC0: */
  /* CLK_SDC1: */
  {0,0,0},//{ HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR), HWIO_ADDR(GCC_SDCC1_APPS_CBCR), HWIO_ADDR(GCC_SDCC1_AHB_CBCR)},
  /* CLK_SDC2: */
  {0,0,0},//{ HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR), HWIO_ADDR(GCC_SDCC2_APPS_CBCR), HWIO_ADDR(GCC_SDCC2_AHB_CBCR)},
  {0,0,0}, /* CLK_SDC3: */
  {0,0,0}, /* CLK_SDC4: */
  {0,0,0}, /* CLK_SDC5: */
};


static const struct Clock_RegType Clock_SPIRegs[CLK_BLSP_QUP_SPI_NUM_CLKS] = 
{ 
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP0 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP1 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP2 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP3 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP4 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP5 is not supported in qcs6200*/
  { 0,  0 }, /*Added Dummy entry as BLSP0_QUP6 is not supported in qcs6200*/
  { 0,  0 },//{ HWIO_ADDR(GCC_BLSP1_QUP0_SPI_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP0_SPI_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP1_SPI_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP2_SPI_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP3_SPI_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP4_SPI_APPS_CBCR) },
  { 0,0 }, /*Added Dummy entry as BLSP1_QUP5 is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP1_QUP6 is not supported in qcs6200*/
  { 0,  0 },//{ HWIO_ADDR(GCC_BLSP2_QUP0_SPI_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP2_QUP0_SPI_APPS_CBCR) },
  { 0,0 }, /*Added Dummy entry as BLSP2_QUP1  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP2_QUP2  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP2_QUP3  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP2_QUP4  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP2_QUP5  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP2_QUP6  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP3_QUP0  is not supported in qcs6200*/
  { 0,0 }, /*Added Dummy entry as BLSP3_QUP1  is not supported in qcs6200*/

};

/* ============================================================================
**  Function : Clock_SetUARTClockFrequency
** ============================================================================
*/
/*!
    Configure UART clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
                     eClock            -  [IN] UART clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetUARTClockFrequency(ClockBootPerfLevelType ePerfLevel,
                                    ClockUARTType eClock)
{
  ClockConfigMuxType UARTCfg = {0};
  const Clock_ConfigType *cfg = Clock_Config();

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;
  if((eClock >= CLK_BLSP_UART_NUM_CLKS) || 
  (Clock_UARTRegs[eClock].nCmd == 0)) return FALSE;

  /* Copy the config to the local so the nCMDRCGRAddr can be modified. */
  UARTCfg = cfg->UART_Cfg[ePerfLevel];

  if( ! Clock_EnableSource( UARTCfg.eSource )) return FALSE;

  UARTCfg.nCMDRCGRAddr = Clock_UARTRegs[eClock].nCmd;


  
  if( ! Clock_ConfigMux(&UARTCfg)) return FALSE;
 
  
  Clock_ToggleClock(Clock_UARTRegs[eClock].nApps, CLK_TOGGLE_ENABLE);
  return TRUE;
}


/* ============================================================================
**	Function : Clock_DisableUARTClock
** ============================================================================
*/
/*!
	Disable  specific UART clock.

	@param    eClock	-	[IN] UART clock to Disable
	@return
	TRUE -- Clock disable was successful.
	FALSE -- Clock disable failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_DisableUARTClock(ClockUARTType eClock)
{
  if((eClock >= CLK_BLSP_UART_NUM_CLKS) || 
  (Clock_UARTRegs[eClock].nApps == 0)) return FALSE;

  Clock_ToggleClock(Clock_UARTRegs[eClock].nApps, CLK_TOGGLE_DISABLE);
  return TRUE;
}

/* ============================================================================
**	Function : Clock_SetI2CClockFrequency
** ============================================================================
*/
/*!
	Configure QUP_I2C clock to a specific perf level.

	@param      ePerfLevel	 -	[IN] UART Clock perf level
		        eClock	 -	[IN] UART clock to configure
	@return
	TRUE -- Initialization was successful.
	FALSE -- Initialization failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_SetI2CClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockQUPI2CType eClock
)
{
  ClockConfigMuxType I2CCfg = {0};
  const Clock_ConfigType *cfg = Clock_Config();

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;
  if((eClock >= CLK_BLSP_QUP_I2C_NUM_CLKS) || 
  (Clock_I2CRegs[eClock].nCmd == 0)) return FALSE;

  /* Copy the config to the local so the nCMDRCGRAddr can be modified. */
  I2CCfg = cfg->I2C_Cfg[ePerfLevel];

  if( ! Clock_EnableSource( I2CCfg.eSource )) return FALSE;

  I2CCfg.nCMDRCGRAddr = Clock_I2CRegs[eClock].nCmd;
  if( ! Clock_ConfigMux(&I2CCfg)) return FALSE;
  Clock_ToggleClock(Clock_I2CRegs[eClock].nApps, CLK_TOGGLE_ENABLE);


  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA, 1);


  return TRUE;
} /* END Clock_SetI2CClockFrequency */



/* ============================================================================
**	Function : Clock_DisableI2CClock
** ============================================================================
*/
/*!
	Disable  specific I2C clock.

	@param    eClock	-	[IN] I2C clock to Disable
	@return
	TRUE -- Clock disable was successful.
	FALSE -- Clock disable failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_DisableI2CClock(ClockQUPI2CType eClock)
{
  if((eClock >= CLK_BLSP_QUP_I2C_NUM_CLKS) ||  (Clock_I2CRegs[eClock].nApps == 0)) return FALSE;

  Clock_ToggleClock(Clock_I2CRegs[eClock].nApps, CLK_TOGGLE_DISABLE);
  
  return TRUE;
}


/* ============================================================================
**  Function : Clock_SetSPIClockFrequency
** ============================================================================
*/
/*!
    Configure the SPI clock to a specified Frquency, rounding up.

    @param nFreqKHz          -  [IN] Frequency in KHz
           eClock            -  [IN] SPI clock to configure
    @return
        freq - Returns the frequency which may be rounded up if the 
          requested frequency was not in the clock plan.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_SetSPIClockFrequency(ClockQUPSPIType eClock,  uint32 nFreqKHz)
{
  ClockConfigMuxType SPICfg = {0};
  const ClockFreqMuxCfgType *pCfg = NULL;
  uint32 n;
  const Clock_ConfigType *cfg = Clock_Config();

  /* Check for a valid Clock */
  if( eClock >= CLK_BLSP_QUP_SPI_NUM_CLKS ) return 0;
  if (Clock_SPIRegs[eClock].nCmd == 0) return 0;

  /* Copy the config to the local so the nCMDRCGRAddr can be modified. */
  pCfg = cfg->SPI_Cfg;

  /* Find the nearest frequency that matches the request */
  for(n = 0; pCfg[n].nFrequency != 0; n++)
  {
    if(pCfg[n].nFrequency >= nFreqKHz) break;
  }
  if(pCfg[n].nFrequency == 0) return 0;


  /* A configuration is found.  Set it */
  SPICfg = pCfg[n].Cfg; /* copy struct and fix it */
  SPICfg.nCMDRCGRAddr = Clock_SPIRegs[eClock].nCmd;



  /* pfab_core_clk defaults to enabled */

  if( SPICfg.eSource != SRC_CXO )
  {
    if( ! Clock_EnableSource( SPICfg.eSource )) return 0;
  }
  if( ! Clock_ConfigMux(&SPICfg)) return 0;
  if( ! Clock_ToggleClock(Clock_SPIRegs[eClock].nApps, CLK_TOGGLE_ENABLE)) return 0;



  return pCfg[n].nFrequency;
}

/* ============================================================================
**  Function : Clock_SetSDCClockFrequencyExt
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_SetSDCClockFrequencyExt
(
  uint32 nFreqKHz,
  ClockSDCType eClock
)
{
    return FALSE;
}


/* ============================================================================
**  Function : Clock_SetSDCClockFrequency
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] SDC Clock perf level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetSDCClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockSDCType eClock
)
{
  return FALSE;
} /* END Clock_SetSDCClockFrequency */


/* ========================================================================
**  Function : Clock_ReplaceUSBBootClockEnable
** ======================================================================*/
/*
    Description: The register GCC_USB_BOOT_CLOCK_CTL turns on all the clocks
    necessary to access USB from the APPS.  This function must be ported
    on every new MSM.  It replaces that single register with all the clock
    enables that it replaced.  This allows for fine grained control of these
    clocks on an individual basis.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_ReplaceUSBBootClockEnable()
{
  //Clock_ToggleClock(HWIO_GCC_PCNOC_QDSS_STM_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  //Clock_ToggleClock(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_PCNOC_USB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  //Clock_ToggleClock(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  //Clock_ToggleClock(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  //Clock_ToggleClock(HWIO_GCC_USB20_MOCK_UTMI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  
  /* Now disable the master boot control, allowing each of those above to replace */
  HWIO_OUTF(GCC_USB_BOOT_CLOCK_CTL, CLK_ENABLE, 0);
}
/* ============================================================================
**  Function : Clock_InitUSB
** ============================================================================
*/
/*!
    Configure USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_InitUSB(void)
{
  /* USBHS configuration USBHS_Cfg : 133.33 MHz */
  ClockConfigMuxType USBHSCfg = {
    HWIO_ADDR(GCC_USB_HS_SYSTEM_CMD_RCGR),
    MUX_GCC, SRC_GPLL0,  /* eSource */
    12,                  /* nDiv2x  */
    0,0,0                /* M/N:D   */
  };


  
  /*Enabling the Access to PCNOC (POR is on only)*/
  //Clock_ToggleClock(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);   

    if( ! Clock_EnableSource( USBHSCfg.eSource )) return FALSE;
    if( ! Clock_ConfigMux(&USBHSCfg)) return FALSE;

  /* Enabling the gcc_pcnoc_usb2_clk for gcc_usb_hs_system_clk
   * This pcnoc_usb2_clk is needed to turn on the gcc_usb_hs_system_clk with NOC_HANDSHAKE_FSM=1 */
    Clock_ToggleClock(HWIO_ADDR(GCC_PCNOC_USB_CBCR), CLK_TOGGLE_ENABLE);

    Clock_ToggleClock(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_PCNOC_USB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, CLK_TOGGLE_ENABLE); 

	/* PHY clocks */
	Clock_ToggleClock(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);


  return TRUE;
}


/* ============================================================================
**  Function : Clock_DisableUSB
** ============================================================================
*/
/*!
    Disable USB clocks.

    @param None.
    @return
    TRUE -- Disable was successful.
    FALSE -- Disable failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisableUSB(void)
{

   /* A mux config for USBHS to CXO */
  ClockConfigMuxType USBHSCfg =
  {
    .nCMDRCGRAddr = HWIO_ADDR(GCC_USB_HS_SYSTEM_CMD_RCGR),
    .eMux = MUX_GCC,
    .eSource = SRC_CXO,
    .nDiv2x = 0,
    .nM = 0,
    .nN = 1,
    .n2D = 0
  };

    /* Swicth USB clock to CXO */
    if( ! Clock_ConfigMux(&USBHSCfg)) return FALSE;
    /*Switch off USB clocks*/
    Clock_ToggleClock(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, CLK_TOGGLE_DISABLE);

    /* This pcnoc_usb2_clk is needed to turn on the gcc_usb_hs_system_clk with NOC_HANDSHAKE_FSM=1 
     * Never turn-off these clocks as it may cause NOC-ERRORs \
     * due to hardware data trnsfer completion may not in sync with turing off the clocks
     */
    //Clock_ToggleClock(HWIO_GCC_PCNOC_USB2_CBCR_ADDR, CLK_TOGGLE_DISABLE);

    Clock_ToggleClock(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, CLK_TOGGLE_DISABLE);
    Clock_ToggleClock(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_DISABLE);
   // Clock_ToggleClock(HWIO_GCC_USB20_MOCK_UTMI_CBCR_ADDR, CLK_TOGGLE_DISABLE);   


  return TRUE;
} /* END Clock_DisableUSB */


/* ============================================================================
**  Function : Clock_USB30_PipeClkSrcSel
** ============================================================================
*/
/*!
    Select USB3 PIPE clock source.

    @param eSrc -  [IN] Clock source choice for the USB3.0 PHY
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.
    @dependencies  None.
    @sa None
*/
boolean Clock_USB30_PipeClkSrcSel(ClockUSB30PipeClkSrcType eSrc)
{
  return TRUE;
}  /* END Clock_USB30_PipeClkSrcSel */


/* ============================================================================
**  Function : Clock_Usb30EnableSWCollapse
** ============================================================================
*/
/*!
    Enable SW Collapse for USB30

    @param Enable/Disable.
    @return
    TRUE always

    @dependencies
    None.

    @sa None
   */
boolean Clock_Usb30EnableSWCollapse(boolean enable)
{
  /* Enable/Disable SW PowerCollapse for USB30 */
  return TRUE;	
}


/* ============================================================================
**  Function : Clock_Usb30GetSWCollapse
** ============================================================================
*/
/*!
    Returns the status of SW Collapse for USB30

    @param None
    @return
    TRUE if enabled
    FALSE if disabled

    @dependencies
    None.

    @sa None
*/
boolean Clock_Usb30GetSWCollapse(void)
{
  return FALSE; /*Vipertooth doesn't have the swcollpase feature*/
}

/* ========================================================================
**  Function : Clock_EnableQPICForDownloadMode
** ======================================================================*/
/*
    Description: Configure QPIC(NAND) clocks at 100MHz on GPLL0 and enable
                 QPIC clocks.
                 The function is used in Download Mode where QPIC clocks are
                 not enabeld by PBL.

    @param None
    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnableQPICForDownloadMode( void )
{

  /*
   * Clocks access to the Peripheral NOC enabled in Clock_ReplaceUSBBootClockEnable.
   */

  /*
   * Enable the QPIC clocks.
   */
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_SYSTEM_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;
  if( ! Clock_ToggleClock(HWIO_GCC_QPIC_IO_MACRO_CBCR_ADDR, CLK_TOGGLE_ENABLE)) return FALSE;
  
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
  
  /* A mux config for QPIC IO clock to 320MHz*/
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
  
   /* Configure QPIC IO clock to 320MHz */
  if(!Clock_ConfigMux( &QPICIOCfg ) )
    return FALSE;

  return TRUE;
}
