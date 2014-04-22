/*
==============================================================================

FILE:         HALclkUSBHS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   USBHS clocks.

   List of clock domains:
    - HAL_clk_mGCCUSBHSSYSTEMClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkUSBHS.c#1 $

==============================================================================
            Copyright (c) 2018 Qualcomm Technologies Incorporated.
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

/* ============================================================================
**    Data
** ==========================================================================*/


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

  {
    /* .szClockName      = */ "gcc_pcnoc_usb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_USB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_USB_CLK
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
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
