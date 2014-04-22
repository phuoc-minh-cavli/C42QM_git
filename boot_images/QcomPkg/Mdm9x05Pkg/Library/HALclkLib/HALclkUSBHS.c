/*
==============================================================================

FILE:         HALclkUSBHS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   USBHS clocks.

   List of clock domains:
     - HAL_clk_mGCCUSB20MOCKUTMIClkDomain
     - HAL_clk_mGCCUSBHSSYSTEMClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkUSBHS.c#1 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_1;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_1;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mUSB20MOCKUTMIClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mUSB20MOCKUTMIClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_usb20_mock_utmi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB20_MOCK_UTMI_CBCR), HWIO_OFFS(GCC_USB_HS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB20_MOCK_UTMI_CLK
  },
};


/*
 * HAL_clk_mGCCUSB20MOCKUTMIClkDomain
 *
 * USB20MOCKUTMI clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCUSB20MOCKUTMIClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_USB20_MOCK_UTMI_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mUSB20MOCKUTMIClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mUSB20MOCKUTMIClkDomainClks)/sizeof(HAL_clk_mUSB20MOCKUTMIClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mUSBHSSYSTEMClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mUSBHSSYSTEMClkDomainClks[] =
{
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout10_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT10_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT10_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT10_AHB_CLK
  },
#endif
  /* This gcc_pcnoc_usb2_clk is needed to turn on the gcc_usb_hs_system_clk with NOC_HANDSHAKE_FSM=1 
   * This clock is enabled in the loader never turn-off these clocks as it may cause NOC-ERRORs \
   * due to hardware data trnsfer completion may not in sync with turing off the clocks, so moving the pmControl to ReadOnly
   */
  {
    /* .szClockName      = */ "gcc_pcnoc_usb2_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_USB2_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_USB2_CLK
  },
  {
    /* .szClockName      = */ "gcc_usb_hs_system_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB_HS_SYSTEM_CBCR), HWIO_OFFS(GCC_USB_HS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB_HS_SYSTEM_CLK
  },
};


/*
 * HAL_clk_mGCCUSBHSSYSTEMClkDomain
 *
 * USBHSSYSTEM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSSYSTEMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_USB_HS_SYSTEM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mUSBHSSYSTEMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mUSBHSSYSTEMClkDomainClks)/sizeof(HAL_clk_mUSBHSSYSTEMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};

