/*
==============================================================================

FILE:         HALclkHFPLL.c

DESCRIPTION:
  This file contains the generic clock HAL code for the PLL control.
  The PLL of HF PLL variety.

==============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
02/24/11   gfr     Created

==============================================================================
  Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"
#include "HALclkHFPLL.h"
#include "HALclkGenericPLL.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * HAL_CLK_PLL_MODE_*
 *
 * Definitions for configuring the PLLn_MODE register.
 */
#define HAL_CLK_PLL_MODE_ACTIVE  \
  (HWIO_FMSK(PLL_MODE, PLL_OUTCTRL)  | \
   HWIO_FMSK(PLL_MODE, PLL_BYPASSNL) | \
   HWIO_FMSK(PLL_MODE, PLL_RESET_N))

/*
 * HAL_CLK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define HAL_CLK_PLL_INIT_TIME_US 5

/*
 * HAL_CLK_PLL_WARMUP_TIME_US
 *
 * Warm-up time in microseconds after turning on the PLL.
 */
#define HAL_CLK_PLL_WARMUP_TIME_US 50

/*
 * HAL_CLK_LOCK_TIMEOUT_US
 *
 * Timeout in usec for PLL_LOCK_DET to get set.
 */
#define HAL_CLK_LOCK_TIMEOUT_US 200


/* ============================================================================
**    Data Declarations
** ==========================================================================*/

/*
 * HAL_clk_HFPLLControl
 *
 * Control function array for the PLLs.
 */
HAL_clk_SourceControlType HAL_clk_HFPLLControl =
{
  /* .Enable          = */ HAL_clk_HFPLLEnable,
  /* .Disable         = */ HAL_clk_HFPLLDisable,
  /* .IsEnabled       = */ HAL_clk_HFPLLIsEnabled,
  /* .Config          = */ HAL_clk_HFPLLConfig,
  /* .ConfigPLL       = */ HAL_clk_HFPLLConfigPLL,
  /* .DetectPLLConfig = */ HAL_clk_HFPLLDetectPLLConfig,
  /* .EnableVote      = */ NULL,
  /* .DisableVote     = */ NULL,
  /* .IsVoteEnabled   = */ NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

/* ===========================================================================
**  HAL_clk_HFPLLEnable
**
** ======================================================================== */

static boolean HAL_clk_HFPLLEnable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  uint32 nModeVal;
  HAL_clk_PLLContextType *pCtxt;
  boolean                 bResult;

  /*
   * Check if PLL is already enabled and return
   */
  if (HAL_clk_HFPLLIsEnabled(pSource))
  {
    return TRUE;
  }

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current mode value.
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Put the PLL in reset mode
   */
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_RESET_N);
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_BYPASSNL);
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_OUTCTRL);
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_PLLTEST); /*Added newly*/
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

  /*
   * De-assert PLL bypass
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, PLL_BYPASSNL);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

  /*
   * Wait for PLL init time
   */
  HAL_clk_BusyWait(HAL_CLK_PLL_INIT_TIME_US);

  /*
   * De-assert PLL reset
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, PLL_RESET_N);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

  /*
   * Wait for the PLL warm-up time
   */
  HAL_clk_BusyWait(HAL_CLK_PLL_WARMUP_TIME_US);

  /*
   * Wait for the PLL to lock within the timeout period.
   */
  bResult =
    HAL_clk_ValidateRegSetTimeout(
      HAL_CLK_PLL_HWIO_ADDR(PLL_STATUS, pCtxt),
      HWIO_FMSK(PLL_STATUS, LOCK_DET),
      HAL_CLK_LOCK_TIMEOUT_US);

  /*
   * Return immediately if the lock detect failed.
   */
  if (!bResult)
  {
    ERR_FATAL("HAL_clk_HFPLLEnable Lock Failure", 0, 0, 0);
    return FALSE;
  }

  /*
   * Put the PLL in active mode
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, PLL_OUTCTRL);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

  return TRUE;

} /* END HAL_clk_HFPLLEnable */


/* ===========================================================================
**  HAL_clk_HFPLLDisable
**
** ======================================================================== */

static void HAL_clk_HFPLLDisable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  uint32  nModeVal;
  HAL_clk_PLLContextType *pCtxt;

  if (eMode != HAL_CLK_SOURCE_DISABLE_MODE_NORMAL)
  {
    ERR_FATAL("HAL_clk_HFPLLDisable Invalid Parameter", 0, 0, 0);
    return;
  }

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current mode value.
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Put the PLL in reset mode
   */
  nModeVal &= ~(HWIO_FMSK(PLL_MODE, PLL_RESET_N) |
                HWIO_FMSK(PLL_MODE, PLL_BYPASSNL) |
                HWIO_FMSK(PLL_MODE, PLL_OUTCTRL) |
                HWIO_FMSK(PLL_MODE, PLL_PLLTEST));  /*Added newly*/
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

} /* END HAL_clk_HFPLLDisable */


/* ===========================================================================
**  HAL_clk_HFPLLIsEnabled
**
** ======================================================================== */

static boolean HAL_clk_HFPLLIsEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32 nModeVal;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current mode value.
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Check whether the PLL is enabled.
   */
  return ((nModeVal & HAL_CLK_PLL_MODE_ACTIVE) == HAL_CLK_PLL_MODE_ACTIVE);

} /* END HAL_clk_HFPLLIsEnabled */


/* ===========================================================================
**  HAL_clk_HFPLLConfig
**
** ======================================================================== */

static void HAL_clk_HFPLLConfig
(
  HAL_clk_SourceDescType   *pSource,
  HAL_clk_SourceConfigType  eConfig
)
{
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Take action.
   */
  switch (eConfig)
  {
    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_AUX_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_AUX_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, LVEARLY_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, LVEARLY_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, LVMAIN_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, LVMAIN_EN, 0);
      break;

    default:
      ERR_FATAL("HAL_clk_HFPLLConfig Invalid Parameter", 0, 0, 0);
      break;
  }

} /* END HAL_clk_HFPLLConfig */


/* ===========================================================================
**  HAL_clk_HFPLLConfigPLL
**
** ======================================================================== */

static boolean HAL_clk_HFPLLConfigPLL
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig,
  HAL_clk_SourceConfigMode     eMode
)
{
  uint32 nUserVal;
  boolean bPLLEnabled = FALSE;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * The HF PLL only support full config mode.
   */
  if (eMode != HAL_CLK_SOURCE_CONFIG_MODE_NORMAL)
  {
    ERR_FATAL("HAL_clk_HFPLLConfigPLL Invalid Parameter", 0, 0, 0);
    return FALSE;
  }

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  bPLLEnabled = HAL_clk_HFPLLIsEnabled(pSource);

  /*
   * Disable the PLL.
   */
  if (bPLLEnabled)
  {
    HAL_clk_HFPLLDisable(pSource, HAL_CLK_SOURCE_DISABLE_MODE_NORMAL, NULL);
  }

  /*
   * Program L/M/N value.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pCtxt, pConfig->nL);
  HAL_CLK_PLL_HWIO_OUT(PLL_M_VAL, pCtxt, pConfig->nM);
  HAL_CLK_PLL_HWIO_OUT(PLL_N_VAL, pCtxt, pConfig->nN);

  /*
   * Get the CONFIG value and clear out fields we will configure.
   */
  nUserVal  =  HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pCtxt);
  nUserVal &= ~(HWIO_FMSK(PLL_USER_CTL, MN_ACC)                       |
                HWIO_FMSK(PLL_USER_CTL, CLK_REF_PRE_DIV)              |
                HWIO_FMSK(PLL_USER_CTL, CLK_POST_DIV));

  /*
   * Program the pre-div value (div-2).
   */
  if (pConfig->nPreDiv <= 2 && pConfig->nPreDiv > 0)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, CLK_REF_PRE_DIV, (pConfig->nPreDiv - 1));
  }

  /*
   * Program the post-div value (div-3 not supported)
   */
  if (pConfig->nPostDiv == 2)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, CLK_POST_DIV, 1);
  }
  else if (pConfig->nPostDiv == 4)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, CLK_POST_DIV, 3);
  }

  /*
   *  Check fractional output or integer output.
   */
  if (pConfig->nM != 0 && pConfig->nM < pConfig->nN)
  {
    nUserVal |= HWIO_FMSK(PLL_USER_CTL, MN_ACC);
  }

  /*
   * Enable MAIN_OUT_ENA bit.
   */
  nUserVal |= HWIO_FMSK(PLL_USER_CTL, LVMAIN_EN);

  /*
   * Finally program the USER_CTL register.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL, pCtxt, nUserVal);

  return TRUE;

} /* END HAL_clk_HFPLLConfigPLL */


/* ===========================================================================
**  HAL_clk_HFPLLDetectPLLConfig
**
** ======================================================================== */

static void HAL_clk_HFPLLDetectPLLConfig
(
  HAL_clk_SourceDescType *pSource,
  HAL_clk_PLLConfigType  *pConfig
)
{
  uint32 nConfigVal;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Clear all config values not modified in this function.
   */
  pConfig->nAlpha = 0;
  pConfig->nAlphaU = 0;
  pConfig->eVCO = 0;
  pConfig->nVCOMultiplier =  0;
  pConfig->bFsmMode = 0;

  nConfigVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pCtxt);
  /*
   * Read current L, M and N values.
   */
  pConfig->nL = HAL_CLK_PLL_HWIO_IN(PLL_L_VAL, pCtxt);
  pConfig->nM = HAL_CLK_PLL_HWIO_IN(PLL_M_VAL, pCtxt);
  pConfig->nN = HAL_CLK_PLL_HWIO_IN(PLL_N_VAL, pCtxt);

  /*
   * Determine the oscillator source.
   */
  pConfig->eSource = HAL_CLK_SOURCE_XO;

  /*
   * Get the pre and post dividers.
   */
  pConfig->nPreDiv = ((nConfigVal & HWIO_FMSK(PLL_USER_CTL, CLK_REF_PRE_DIV))
                         >> (HWIO_SHFT(PLL_USER_CTL, CLK_REF_PRE_DIV))) + 1;
  pConfig->nPostDiv = ((nConfigVal & HWIO_FMSK(PLL_USER_CTL, CLK_POST_DIV))
                >> HWIO_SHFT(PLL_USER_CTL, CLK_POST_DIV)) + 1;

} /* END HAL_clk_HFPLLDetectPLLConfig */

