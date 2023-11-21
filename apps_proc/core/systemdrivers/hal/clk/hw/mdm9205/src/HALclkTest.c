/*
==============================================================================

FILE:         HALclkTest.c

DESCRIPTION:
  This file contains the implementation for the clock frequency measurement
  for the clock HAL.

==============================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/HALclkTest.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/28/18   shm     First version for 9205

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

/*
 * Chipset specific header files.
 */
#include "HALclkHWIO.h"
#include "HALclkTest.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Defines
** ==========================================================================*/


/*
 * Define the initial count for the ring-oscillator comparator.  Arbitrary
 * but high enough to get good accuracy.
 */
#define HAL_CLK_RO_TCXO_CNT_VAL   0x8000

/*
 * This value defines the minimal valid count for a clock under test. If the
 * original count is at or below this value, there is a good chance that the
 * HW has malfunctioned during the measurement phase. In this case we allow
 * the reset loop to time out and escape the infinite loop.
 */
#define HAL_CLK_MINIMAL_COUNT       0xC0


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_CalcClockFreq
**
** ======================================================================== */

void HAL_clk_CalcClockFreq
(
  HAL_clk_ClockHandleType  pClockHandle,
  uint32                   *pnFreqHz
)
{
  boolean bIsClockSupportedIsOn;
  uint32 nCount;
  uint32 nDenominator;
  uint32 nTestClockSel;
  uint32 nTestClockType;
  uint32 nMultiplier = 1;
  uint32 nDebugClkCtl, nXODIV4CBCR;

  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  /*
   * Sanity
   */
  if (pmClockDesc == NULL || pnFreqHz == NULL)
  {
    return;
  }

  /*
   * Initialize the return value.
   */
  *pnFreqHz = 0;

  /*
   * Before performing the clock frequency calculation try to determine
   * whether the clock is actually "on". If the clock is in the "off" state,
   * return '0' and don't proceed with the computation.
   */
  bIsClockSupportedIsOn = HAL_clk_IsClockSupported (pClockHandle,
                                                    HAL_CLK_CLOCK_API_ISON);

  /*
   * Verify the clock is "on" if possible; otherwise, proceed with the
   * computation. If the clock is "off", then return '0' right away.
   */
  if (bIsClockSupportedIsOn)
  {
    if (!HAL_clk_IsClockOn(pClockHandle))
    {
      return;
    }
  }

  /*
   * Save current values.
   */
  nDebugClkCtl = HWIO_IN(GCC_GCC_DEBUG_CLK_CTL);
  nXODIV4CBCR  = HWIO_IN(GCC_GCC_XO_DIV4_CBCR);

  /*
   * Get the debug clock value.
   */
  nTestClockType = pmClockDesc->nTestClock & HAL_CLK_TEST_TYPE_MASK;
  nTestClockSel  = pmClockDesc->nTestClock & HAL_CLK_TEST_SEL_MASK;

  /*
   * Program the subsystem specific debug clock registers
   */
  switch (nTestClockType)
  {
    /*
     * GCC clocks.
     */
    case HAL_CLK_GCC_TEST:
      HWIO_OUTF(GCC_GCC_DEBUG_CLK_CTL, MUX_SEL, nTestClockSel);
      break;

      /*
       * A5SS subsystem clocks.
       */
    case HAL_CLK_APCS_TEST:
      HWIO_OUTF(APCS_CLK_DIAG, FAST_CLK_SEL, nTestClockSel);
      HWIO_OUTF(GCC_GCC_DEBUG_CLK_CTL, MUX_SEL,
                HAL_CLK_GCC_TEST_APSS_GCC_DBG_CLK & HAL_CLK_TEST_SEL_MASK);
      break;

    /*
     * Unknown
     */
    default:
      *pnFreqHz = 0;
      return;
  }

  /*
   * Enable debug clock control register and divide the input clock since
   * it might be too high for the frequency measurement hardware.
   */
  HWIO_OUTF(GCC_GCC_DEBUG_CLK_CTL, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_GCC_DEBUG_CLK_CTL, POST_DIV, 0x3);
  nMultiplier = 4;

  /*
   * Enable XO DIV4 comparator clock
   */
  HWIO_OUTF(GCC_GCC_XO_DIV4_CBCR, CLK_ENABLE, 1);

  /*
   * Start with counter disabled
   */
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 0);

  /*
   * Program the starting counter value, high enough to get good accuracy
   */
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, XO_DIV4_TERM_CNT, HAL_CLK_RO_TCXO_CNT_VAL);

  /*
   * Start the counting
   */
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 1);

  HAL_clk_BusyWait(5);

  /*
   * Wait for the counters to finish.
   */
  while (HWIO_INF(GCC_CLOCK_FRQ_MEASURE_STATUS, XO_DIV4_CNT_DONE) == 0);

  /*
   * Turn off test clock and read count
   */
  HWIO_OUT(GCC_CLOCK_FRQ_MEASURE_CTL, 0);
  nCount = HWIO_INF(GCC_CLOCK_FRQ_MEASURE_STATUS, MEASURE_CNT);

  /*
   * Restore the registers.
   */
  HWIO_OUT(GCC_GCC_DEBUG_CLK_CTL, nDebugClkCtl);
  HWIO_OUT(GCC_GCC_XO_DIV4_CBCR, nXODIV4CBCR);

  /*
   * Calculate the frequency.  Function is provided by
   * Power Control 42.8.1.2 Measurement technique
   * f ring = f tcxo/4 * (Nring + 1.5) / (TCtcxo + 3.5)
   *
   * The formula below is the equivalent one converted
   * to integer arithmetic.
   *
   * We add half of the denominator value to the dividend
   * to guard against rounding errors inherent to
   * integer arithmetic.
   */
  nDenominator = HAL_CLK_RO_TCXO_CNT_VAL * 2 + 7;

  *pnFreqHz = (uint32)(((9600 * (uint64)nCount + 14400) + nDenominator / 2) / nDenominator * 1000);

  /*
   * Take into account the multiplication factor
   */
  *pnFreqHz *= nMultiplier;

} /* END HAL_clk_CalcClockFreq */

