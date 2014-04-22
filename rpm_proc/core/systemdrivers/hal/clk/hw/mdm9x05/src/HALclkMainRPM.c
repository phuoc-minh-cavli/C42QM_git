/*
==============================================================================

FILE:         HALclkMainRPM.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the rpm processor.


==============================================================================

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/hal/clk/hw/mdm9x05/src/HALclkMainRPM.c#2 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include "HALclkInternal.h"
#include "HALclkGeneric.h"
#include "HALclkGenericPLL.h"
#include "HALhwio.h"
#include "HALclkHWIO.h"
#include "assert.h"

//#define CLOCK_PSCBC_TEST


/* List of clocks being turned off during deep sleep */
typedef struct
{
  boolean bUltAudioLPMClk; /*gcc_ultaudio_ahbfabric_ixfabric_lpm_clk*/
  boolean bBootROMClk;     /* gcc_boot_rom_ahb_clk*/
  boolean bDCCClk;         /* gcc_dcc_clk */
  boolean bDEHRClk;        /* gcc_dehr_clk */
  boolean bECCCoreClk;     /* gcc_ecc_core_clk */
  boolean bIMEMClk;        /* gcc_imem_axi_clk */
  boolean bMsgRAMClk;      /* gcc_msg_ram_ahb_clk */
  boolean bQDSSAtClk;      /* gcc_qdss_at_clk */
  boolean bSecCtrlAccClk;  /* gcc_sec_ctrl_acc_clk */
  boolean bSecCtrlClk;     /* gcc_sec_ctrl_clk */
  boolean bSPMIAHBClk;     /* gcc_spmi_ahb_clk */
  boolean bSPMISERClk;     /* gcc_spmi_ser_clk */
  boolean bQZIPClk;        /* gcc_qzip_clk */
  boolean bUSBHSSystemClk; /* gcc_usb_hs_system_clk */
  
} HAL_MiscClkStateType;

/* ============================================================================
**    Prototypes
** ==========================================================================*/

void HAL_clk_PlatformInitSources(void);

/* ============================================================================
**    Externs
** ==========================================================================*/

extern void HAL_clk_PlatformInitRPM(void);
extern void HAL_clk_PlatformInitGCCMain(void);


/* ============================================================================
**    Data
** ==========================================================================*/
#define MAX_RETRY 120
static HAL_MiscClkStateType HAL_MiscClkState;


/*
 * HAL_clk_aInitFuncs
 *
 * Declare array of module initialization functions.
 */
static HAL_clk_InitFuncType HAL_clk_afInitFuncs[] =
{
  /*
   * Sources
   */
  HAL_clk_PlatformInitSources,
  
  /*
   * CPU
   */
  HAL_clk_PlatformInitGCCMain,


  NULL
};


/*
 * Declare the base pointers for HWIO access.
 */
uint32 HAL_clk_nHWIOBaseTop     = CLK_CTL_BASE_PHYS;



/*
 * HAL_clk_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_clk_HWIOBaseType HAL_clk_aHWIOBases[] =
{
  { CLK_CTL_BASE_PHYS,  CLK_CTL_BASE_SIZE, &HAL_clk_nHWIOBaseTop },
  { 0, 0, NULL }
};


/*
 * HAL_clk_Platform;
 * Platform data.
 */
HAL_clk_PlatformType HAL_clk_Platform =
{
  HAL_clk_afInitFuncs,
  HAL_clk_aHWIOBases
};


/*
 * GPLL contexts
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextGPLL[] =
{
  {
    HWIO_OFFS(GCC_GPLL0_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL0),
    HAL_CLK_PLL_STROMER
  },
  {
    HWIO_OFFS(GCC_GPLL1_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL1),
    HAL_CLK_PLL_SR
  },
  {
    HWIO_OFFS(GCC_GPLL2_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL2),
    HAL_CLK_PLL_STROMER
  }, 
  {
    HWIO_OFFS(GCC_BIMC_PLL_MODE),
    HAL_CLK_FMSK(GCC_RPM_GPLL_ENA_VOTE, BIMC_PLL),
    HAL_CLK_PLL_STROMER
  },
  
};


/* ============================================================================
**    Functions
** ==========================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitSources
**
** ======================================================================== */

void HAL_clk_PlatformInitSources (void)
{
  /*
   * Install PLL handlers.
   */
  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL0,   &HAL_clk_aPLLContextGPLL[0], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL1,   &HAL_clk_aPLLContextGPLL[1], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL2,   &HAL_clk_aPLLContextGPLL[2], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_BIMCPLL, &HAL_clk_aPLLContextGPLL[3], CLK_CTL_BASE);

} /* END HAL_clk_PlatformInitSources */


/* ===========================================================================
**  HAL_clk_GCCSourceMapToHW - General Clock Control
**
** ======================================================================== */

uint32 HAL_clk_GCCSourceMapToHW
(
  HAL_clk_SourceType eSource
)
{
  /*
   * Determine the source selection value.
   */
  switch (eSource)
  {
      case HAL_CLK_SOURCE_XO:       return 0;
      case HAL_CLK_SOURCE_GPLL0:    return 1;
      case HAL_CLK_SOURCE_BIMCPLL:  return 2;
     
        
      /* RAW Source clock without any PLL */
      case HAL_CLK_SOURCE_RAW0:     return 0;
      case HAL_CLK_SOURCE_RAW1:     return 1;
      case HAL_CLK_SOURCE_RAW2:     return 2;
    default:
      return 0;
  }

} /* END HAL_clk_GCCSourceMapToHW */


/* ===========================================================================
**  HAL_clk_GCCSourceMapFromHW - General Clock Control
**
** ======================================================================== */

HAL_clk_SourceType HAL_clk_GCCSourceMapFromHW
(
  uint32 nSourceSelect
)
{
  /*
   * Determine the source selection value.
   */
  switch (nSourceSelect)
  {
    case 0: return HAL_CLK_SOURCE_XO;
    case 1: return HAL_CLK_SOURCE_GPLL0;
    case 2: return HAL_CLK_SOURCE_BIMCPLL;
    default:
      return HAL_CLK_SOURCE_XO;
  }
} /* END HAL_clk_GCCSourceMapFromHW */



/* ===========================================================================
**  HAL_clk_Save
**
** ======================================================================== */

void HAL_clk_Save (void)
{
  /*
   * Nothing to save.
   */

} /* END HAL_clk_Save */


/* ===========================================================================
**  HAL_clk_Restore
**
** ======================================================================== */

void HAL_clk_Restore (void)
{
  /*
   * Nothing to restore.
   */
  
} /* END HAL_clk_Restore */


/* ===========================================================================
**  HAL_clk_GFXRailPowerDown
**
** ======================================================================== */

void HAL_clk_GFXRailPowerDown( void )
{
#if 0
  HWIO_OUTF( MMSS_GX_DOMAIN_MISC, OCMEM_FIFO_HALT, 1 );

  HWIO_OUT(MMSS_GX_DOMAIN_MISC, 0x1111);
#endif
}


/* ===========================================================================
**  HAL_clk_GFXRailPowerUp
**
** ======================================================================== */

void HAL_clk_GFXRailPowerUp( void )
{
#if 0
  HWIO_OUT( MMSS_GX_DOMAIN_MISC, 0x1111 );
  HAL_clk_BusyWait( 1 );

  HWIO_OUTF(MMSS_GX_DOMAIN_MISC, OCMEM_FIFO_HALT, 1);
  
  HWIO_OUTF(MMSS_GX_DOMAIN_MISC, OXILI_OCMEM_RESET, 0x0);

  HWIO_OUTF(MMSS_GX_DOMAIN_MISC, OXILI_OCMEM_CLAMP_IO, 0x0);

  HWIO_OUT(MMSS_GX_DOMAIN_MISC, 0x0);
#endif
}

/* ===========================================================================
**  HAL_clk_GPLL0_disable_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL0_disable_deep_sleep(void)
{
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_ENA ,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_OUTCTRL ,0);
}
/* ===========================================================================
**  HAL_clk_GPLL0_FSM_reset_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL0_FSM_reset_deep_sleep(void)
{
     

  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_RESET ,1);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_OUTCTRL ,0);
   
  HAL_clk_BusyWait( 1 );
        
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_ENA ,1);
  
  HAL_clk_BusyWait( 1 );
  
  HWIO_OUTF(GCC_GPLL0_MODE, PLL_VOTE_FSM_RESET ,0);
  
}  

/* ===========================================================================
**  HAL_clk_GPLL1_disable_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL1_disable_deep_sleep(void)
{
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_ENA ,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_OUTCTRL ,0);
}
/* ===========================================================================
**  HAL_clk_GPLL1_FSM_reset_deep_sleep
**
** ======================================================================== */
void HAL_clk_GPLL1_FSM_reset_deep_sleep(void)
{
     

  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_RESET ,1);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_BYPASSNL,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_RESET_N ,0);
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_OUTCTRL ,0);
   
  HAL_clk_BusyWait( 1 );
        
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_ENA ,1);
  
  HAL_clk_BusyWait( 1 );

  HWIO_OUT(GCC_GPLL1_CONFIG_CTL, 0x30031800);
  
  HWIO_OUTF(GCC_GPLL1_MODE, PLL_VOTE_FSM_RESET ,0);
  
  HAL_clk_BusyWait( 30 );
  
  HWIO_OUT(GCC_GPLL1_CONFIG_CTL, 0x31800);

  while (HWIO_INF(GCC_GPLL1_STATUS, PLL_ACTIVE_FLAG) != 1)
  {
    HAL_clk_BusyWait(10);
  }
  
}  

/* ===========================================================================
**  HAL_clk_GetHWParNum
**
** ======================================================================== */
uint32 HAL_clk_GetHWParNum(void)
{
  return HWIO_INF(TLMM_HW_REVISION_NUMBER, PARTNUM);
} 


/* Loop over a table is appropriate to save space.
 * Each of these clocks has a CBCR with a CLK_OFF bit to poll when turned off, and a single
 * bit to enable it. Added QREF Clocks that share ENABLE and STATUS bits with same functionality.
 *
 * pStatusVar  : points to a status value that is used to hold the pre-shutdown state.
 * nCBCRReg    : the address of the CBCR register
 * nEnableReg  : the address of the enable register.
 * nEnableMask : the bit to set(enable) or clear(disable) the clock in the nEnableReg.
 */
const struct {
  boolean* pStatusVar;
  uint32   nCBCRReg;
  uint32   nEnableReg;
  uint32   nEnableMask;
} HAL_clk_DisableDuringSleep[] =
{
  {
    &HAL_MiscClkState.bUltAudioLPMClk,
    HWIO_ADDR(GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR),
    HWIO_ADDR(GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR),
    HWIO_FMSK(GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bBootROMClk,
    HWIO_ADDR(GCC_BOOT_ROM_AHB_CBCR),
    HWIO_ADDR(GCC_RPM_CLOCK_BRANCH_ENA_VOTE),
    HWIO_FMSK(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, BOOT_ROM_AHB_CLK_ENA)
  },
  {
    &HAL_MiscClkState.bDEHRClk,
    HWIO_ADDR(GCC_DEHR_CBCR),
    HWIO_ADDR(GCC_DEHR_CBCR),
    HWIO_FMSK(GCC_DEHR_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bECCCoreClk,
    HWIO_ADDR(GCC_ECC_CORE_CBCR),
    HWIO_ADDR(GCC_ECC_CORE_CBCR),
    HWIO_FMSK(GCC_ECC_CORE_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bIMEMClk,
    HWIO_ADDR(GCC_IMEM_AXI_CBCR),
    HWIO_ADDR(GCC_RPM_CLOCK_BRANCH_ENA_VOTE),
    HWIO_FMSK(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA)
  },
  { 
    &HAL_MiscClkState.bMsgRAMClk,
    HWIO_ADDR(GCC_MSG_RAM_AHB_CBCR),
    HWIO_ADDR(GCC_RPM_CLOCK_BRANCH_ENA_VOTE),
    HWIO_FMSK(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA)
  },
  {
    &HAL_MiscClkState.bSecCtrlAccClk,
    HWIO_ADDR(GCC_SEC_CTRL_ACC_CBCR),
    HWIO_ADDR(GCC_SEC_CTRL_ACC_CBCR),
    HWIO_FMSK(GCC_SEC_CTRL_ACC_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bSecCtrlClk,
    HWIO_ADDR(GCC_SEC_CTRL_CBCR),
    HWIO_ADDR(GCC_SEC_CTRL_CBCR),
    HWIO_FMSK(GCC_SEC_CTRL_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bSPMIAHBClk,
    HWIO_ADDR(GCC_SPMI_AHB_CBCR),
    HWIO_ADDR(GCC_SPMI_AHB_CBCR),
    HWIO_FMSK(GCC_SPMI_AHB_CBCR, CLK_ENABLE),
  },
  {
    &HAL_MiscClkState.bSPMISERClk,
    HWIO_ADDR(GCC_SPMI_SER_CBCR),
    HWIO_ADDR(GCC_SPMI_SER_CBCR),
    HWIO_FMSK(GCC_SPMI_SER_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bQZIPClk,
    HWIO_ADDR(GCC_QZIP_CBCR),
    HWIO_ADDR(GCC_QZIP_CBCR),
    HWIO_FMSK(GCC_QZIP_CBCR, CLK_ENABLE)
  },
  {
    &HAL_MiscClkState.bUSBHSSystemClk,
    HWIO_ADDR(GCC_USB_HS_SYSTEM_CBCR),
    HWIO_ADDR(GCC_USB_HS_SYSTEM_CBCR),
    HWIO_FMSK(GCC_USB_HS_SYSTEM_CBCR, CLK_ENABLE),
  },
};

/* ===========================================================================
**  HAL_clk_MiscClkDisableDeepSleep
** ======================================================================== */
void HAL_clk_MiscClkDisableDeepSleep(void)
{
  int i;
  boolean status;
  uint32 shadow;
  uint32 nRetry = 0;


  
  /* Enable all the clocks */
  for(i=0; i<sizeof( HAL_clk_DisableDuringSleep ) / sizeof( HAL_clk_DisableDuringSleep[0] ); i++)
  {
    status = ( (inpdw(HAL_clk_DisableDuringSleep[i].nCBCRReg) & HWIO_FMSK(GCC_SPMI_SER_CBCR, CLK_OFF)) ? FALSE : TRUE);
    *(HAL_clk_DisableDuringSleep[i].pStatusVar) = status;

    if ( status == TRUE )
    {
      /* Masked write to disable the clock */
      shadow = inpdw(HAL_clk_DisableDuringSleep[i].nEnableReg);
      shadow &= (~HAL_clk_DisableDuringSleep[i].nEnableMask);
      outpdw(HAL_clk_DisableDuringSleep[i].nEnableReg, shadow);
    }
  }
  
  
  /* Now wait for the statuses, this is faster */
  for(i=0; i<sizeof( HAL_clk_DisableDuringSleep ) / sizeof( HAL_clk_DisableDuringSleep[0] ); i++)
  {
    if( *HAL_clk_DisableDuringSleep[i].pStatusVar == TRUE )
    {
    
      // Test to make sure that clock gets turn off
      nRetry  = 0;
      while( ((inpdw(HAL_clk_DisableDuringSleep[i].nCBCRReg) & HWIO_FMSK(GCC_SPMI_SER_CBCR, CLK_OFF)) == 0) &&
             (nRetry < MAX_RETRY) )
      {
        nRetry++;
        HAL_clk_BusyWait( 1 );
      }
      assert (nRetry < MAX_RETRY);
    }
  }
  
  
   /*
   * Need to handle special case for GCC_DCC_CLK,GCC_QDSS_AT_CLK
   * since this clock has HW signal which forces on when JTAG connect
   * so it cannot handle like previous clocks.  Just clear CLK_ENABLE bit
   */
  HAL_MiscClkState.bDCCClk = HWIO_INF( GCC_DCC_CBCR, CLK_ENABLE );
  if ( HAL_MiscClkState.bDCCClk )
  {
    HWIO_OUTF( GCC_DCC_CBCR, CLK_ENABLE, 0 );
  }
  HAL_MiscClkState.bQDSSAtClk = HWIO_INF( GCC_QDSS_AT_CBCR, CLK_ENABLE );
  if ( HAL_MiscClkState.bQDSSAtClk )
  {
    HWIO_OUTF( GCC_QDSS_AT_CBCR, CLK_ENABLE, 0 );
  }
  
}

/* ===========================================================================
**  HAL_clk_MiscClkRestoreDuringWakeup
** ======================================================================== */
void HAL_clk_MiscClkRestoreDuringWakeup(void)
{
  int i;
  uint32 shadow;

   /*
   * Need to handle special case for GCC_DCC_CLK,GCC_QDSS_AT_CLK
   * since this clock has HW signal which force on when JTAG connect,
   * so it cannot handle like previous clocks.  Just set CLK_ENABLE bit back
   * if it was previous enabled
   */
   

  if ( HAL_MiscClkState.bQDSSAtClk )
  {
    HWIO_OUTF( GCC_QDSS_AT_CBCR, CLK_ENABLE, 1 );
  }
  if ( HAL_MiscClkState.bDCCClk )
  {
    HWIO_OUTF( GCC_DCC_CBCR, CLK_ENABLE, 1 );
  }
    
  /* Run them in the opposite order, as this is usually desired in shutdown / restart sequences */
  for(i= (sizeof( HAL_clk_DisableDuringSleep ) / sizeof( HAL_clk_DisableDuringSleep[0]))-1; i>=0; i--)
  {
    if( *HAL_clk_DisableDuringSleep[i].pStatusVar == TRUE )
    {
      /* Masked write to enable the clock */
      shadow = inpdw(HAL_clk_DisableDuringSleep[i].nEnableReg);
      shadow |= HAL_clk_DisableDuringSleep[i].nEnableMask;
      outpdw(HAL_clk_DisableDuringSleep[i].nEnableReg, shadow);
    }
  }
  

  for(i=(sizeof( HAL_clk_DisableDuringSleep ) / sizeof( HAL_clk_DisableDuringSleep[0] ))-1; i>=0; i--)
  {
    if( *HAL_clk_DisableDuringSleep[i].pStatusVar == TRUE )
    {
      /* Wait for clock to enable */
      while( (inpdw(HAL_clk_DisableDuringSleep[i].nCBCRReg) & HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK) != 0 );
    }
  }
}

#ifdef CLOCK_PSCBC_TEST
const uint32 HAL_PSCBC_addrs[] =
{
   HWIO_ADDR(GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR),

   HWIO_ADDR(GCC_BLSP1_AHB_CBCR),
   HWIO_ADDR(GCC_BOOT_ROM_AHB_CBCR),
   HWIO_ADDR(GCC_CRYPTO_CBCR),

   HWIO_ADDR(GCC_DCC_CBCR),
   HWIO_ADDR(GCC_DEHR_CBCR),                                  
   HWIO_ADDR(GCC_ECC_CORE_CBCR),
   
   HWIO_ADDR(GCC_IMEM_AXI_CBCR),                                                            
   HWIO_ADDR(GCC_MSG_RAM_AHB_CBCR),                            

   HWIO_ADDR(GCC_QDSS_AT_CBCR),                                
   HWIO_ADDR(GCC_QPIC_AHB_CBCR),
   HWIO_ADDR(GCC_QPIC_CBCR),                                     
                                    
   HWIO_ADDR(GCC_RPM_BUS_AHB_CBCR),                          
   
   HWIO_ADDR(GCC_SEC_CTRL_ACC_CBCR),                             
   HWIO_ADDR(GCC_SEC_CTRL_CBCR),  
   
   HWIO_ADDR(GCC_SPMI_AHB_CBCR),                                
   HWIO_ADDR(GCC_SPMI_SER_CBCR),                                
                                 
   HWIO_ADDR(GCC_QZIP_CBCR),
   HWIO_ADDR(GCC_USB_HS_SYSTEM_CBCR),


};

/* To prevent storing data in the HAL, the API is as follows:
 * Make the first call as index = HAL_clk_CheckPSCBCs( 0, &badPSCBC)
 * Then loop until index is 0 to get the remaining PSCBCs that are left on
 */
int32 HAL_clk_CheckPSCBCs( uint32 index, uint32 *retPSCBCAddr, uint32 *retPSCBCValue )
{
  int i;
  uint32 nVal;

  for(i=index; i < sizeof(HAL_PSCBC_addrs)/sizeof(HAL_PSCBC_addrs[0]); i++)
  {
    nVal  = inpdw(HAL_PSCBC_addrs[i]);
    if( (nVal & HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK) == 0)
    {
      *retPSCBCAddr = HAL_PSCBC_addrs[i];
      *retPSCBCValue = nVal;
      return i+1;
    }
  }
  // For loop finished with no more problems, return success.
  retPSCBCAddr = 0;
  return 0;
}
#endif



