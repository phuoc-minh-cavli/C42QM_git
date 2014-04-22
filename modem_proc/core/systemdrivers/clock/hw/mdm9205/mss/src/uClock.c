/*
===========================================================================
*/
/**
  @file uClock.c
  @brief Implementation of the micro lightweight clock driver

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      This file is source file for the MPSS page-only mode. It is meant to be
      used by a small subset of drivers, which cannot call into the DAL clock
      driver APIs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.10/systemdrivers/clock/hw/mdm9205/mss/src/uClock.c#2 $
  $DateTime: 2019/09/19 05:28:49 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  09/19/19   shm     Add support for UART clocks for FR57613
  01/16/19   shm     First version for page-only mode requirements

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "uClock.h"
#include "uClockHWIO.h"
#include "HALhwio.h"
#include "busywait.h"
#include "string.h"
#include "qurt_mutex.h"


/*=========================================================================
      Macros
==========================================================================*/

#define SIZEOF_ARRAY(p) (sizeof(p) / sizeof(p[0]))

#define UCLOCK_CMD_RCG_ROOT_OFF_FMSK        0x80000000
#define UCLOCK_CMD_RCG_ROOT_EN_FMSK         0x00000002
#define UCLOCK_CMD_RCG_UPDATE_FMSK          0x00000001
#define UCLOCK_CMD_RCG_UPDATE_SHFT          0

#define UCLOCK_CFG_RCG_MODE_FMSK            0x00003000
#define UCLOCK_CFG_RCG_MODE_SHFT            0xC
#define UCLOCK_CFG_RCG_SRC_SEL_FMSK         0x00000700
#define UCLOCK_CFG_RCG_SRC_SEL_SHFT         0x8
#define UCLOCK_CFG_RCG_SRC_DIV_FMSK         0x0000001F
#define UCLOCK_CFG_RCG_SRC_DIV_SHFT         0

#define UCLOCK_CFG_RCG_DUAL_EDGE_MODE_VAL   0x2
#define UCLOCK_CFG_RCG_BYPASS_MODE_VAL      0

#define UCLOCK_PLL_MODE_PLL_LOCK_DET_FMSK     0x80000000
#define UCLOCK_PLL_MODE_PLL_LOCK_DET_SHFT     0x1F
#define UCLOCK_PLL_MODE_PLL_ACTIVE_FLAG_FMSK  0x40000000
#define UCLOCK_PLL_MODE_PLL_ACTIVE_FLAG_SHFT  0x1E
#define UCLOCK_PLL_MODE_PLL_UPDATE_FMSK       0x400000
#define UCLOCK_PLL_MODE_PLL_UPDATE_SHFT       0x16

#define UCLOCK_CBCR_CLK_OFF_FMSK            0x80000000
#define UCLOCK_CBCR_CLK_OFF_SHFT            0x1F
#define UCLOCK_CBCR_CLK_ENABLE_FMSK         0x00000001
#define UCLOCK_CBCR_CLK_ENABLE_SHFT         0x0

/*
 * NOT_2D / NOT_N_MINUS_M
 *
 * Macros to return the inverted value of the 2D field or (N - M)
 * Used to prepare the value for writing to the hardware register field.
 */
#define NOT_2D(cfg)         (~(cfg)->nN)
#define NOT_N_MINUS_M(cfg)  (~((cfg)->nN - (cfg)->nM))

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * uClockSourceEnumType
 *
 */
typedef enum
{
  UCLOCK_SOURCE_XO,
  UCLOCK_SOURCE_MPLL2,
  UCLOCK_SOURCE_GPLL0,
  UCLOCK_SOURCE_TOTAL,

} uClockSourceEnumType;

typedef struct
{
  uint32                     nFreqHz;
  uint8                      nMode;
  uint8                      nPreDiv;
  uint8                      nPostDiv;
  uint8                      nL;
  uint64                     nFracVal;
} uClockSourceFreqConfigType;

typedef struct
{
  uint32                      nFreqHz;
  uint8                       nMuxSel;
  uint8                       nDiv2x;
  uint16                      nM;
  uint16                      nN;
  uClockSourceEnumType        eSource;
  uClockSourceFreqConfigType *pSourceFreqConfig;
} uClockMuxConfigType;

typedef struct
{
  uint32                      nAddr;
  uClockMuxConfigType        *pClockConfig;
  uClockMuxConfigType        *pCurrentConfig;
} uClockDomainType;

typedef struct
{
  char                       *szName;
  uint32                      nCBCAddr;
  uint32                      nVoteAddr;
  uint32                      nVoteMask;
  uClockDomainType           *pDomain;
  uint32                      nRefCnt;
} uClockType;

typedef struct
{
  char                       *szName;
  uClockSourceEnumType        eSource;
  uint32                      nModeAddr;
  uint32                      nVoteAddr;
  uint32                      nVoteMask;
  uClockSourceFreqConfigType *pSourceConfig;
  uint32                      nRefCnt;
} uClockSourceType;

typedef struct
{
  uClockType                 *aClocks;
  uClockSourceType           *aSources;
  uint32                      nNumClocks;
  uint32                      nNumSources;
} uClockBSPType;

/*
 * uClock state context structure
 * Parameters:
 *   Mutex        - Synchronization mutex locking
 *   pBSP         - Pointer to BSP data
 *   bInitialized - Specifies if initialization has taken place
 */
typedef struct
{
  qurt_mutex_t     Mutex;
  uClockBSPType   *pBSP;
  boolean          bInitialized;
} uClockCtxtType;

/*=========================================================================
      Local Variables
==========================================================================*/

static uClockSourceFreqConfigType SourceFreqConfig_MPLL2[] =
{
  { 480000000, 2, 1, 1, 25, 0 },
  { 576000000, 2, 1, 1, 30, 0 },
  { 652800000, 2, 1, 1, 34, 0 },
  { 0 },
};

static uClockSourceFreqConfigType SourceFreqConfig_GPLL0[] =
{
  { 800000000, 1, 1, 1, 41, 0xAAAA000000 },
  { 0 },
};

static uClockMuxConfigType aQ6Config[] =
{
  { 326400000, 1, 4, 0, 0, UCLOCK_SOURCE_MPLL2, &SourceFreqConfig_MPLL2[2]},
  { 480000000, 1, 2, 0, 0, UCLOCK_SOURCE_MPLL2, &SourceFreqConfig_MPLL2[0]},
  { 576000000, 1, 2, 0, 0, UCLOCK_SOURCE_MPLL2, &SourceFreqConfig_MPLL2[1]},
  { 0 }
};

static uClockMuxConfigType aUARTConfig[] =
{
  {  3686400, 1,  2,   72, 15625, UCLOCK_SOURCE_GPLL0, NULL},
  {  7372800, 1,  2,  144, 15625, UCLOCK_SOURCE_GPLL0, NULL},
  { 14745600, 1,  2,  288, 15625, UCLOCK_SOURCE_GPLL0, NULL},
  { 16000000, 1, 20,    1,     5, UCLOCK_SOURCE_GPLL0, NULL},
  { 19200000, 0,  2,    0,     0, UCLOCK_SOURCE_XO,    NULL},
  { 24000000, 1,  2,    3,   100, UCLOCK_SOURCE_GPLL0, NULL},
  { 25000000, 1, 32,    1,     2, UCLOCK_SOURCE_GPLL0, NULL},
  { 32000000, 1,  2,    1,    25, UCLOCK_SOURCE_GPLL0, NULL},
  { 40000000, 1,  2,    1,    20, UCLOCK_SOURCE_GPLL0, NULL},
  { 46400000, 1,  2,   29,   500, UCLOCK_SOURCE_GPLL0, NULL},
  { 48000000, 1,  2,    3,    50, UCLOCK_SOURCE_GPLL0, NULL},
  { 51200000, 1,  2,    8,   125, UCLOCK_SOURCE_GPLL0, NULL},
  { 56000000, 1,  2,    7,   100, UCLOCK_SOURCE_GPLL0, NULL},
  { 58982400, 1,  2, 1152, 15625, UCLOCK_SOURCE_GPLL0, NULL},
  { 60000000, 1,  2,    3,    40, UCLOCK_SOURCE_GPLL0, NULL},
  { 64000000, 1,  2,    2,    25, UCLOCK_SOURCE_GPLL0, NULL},
  { 0 }
};

static uClockMuxConfigType aXOConfig[] =
{
  { 19200000, 0,  2,    0,     0, UCLOCK_SOURCE_XO,    NULL},
  { 0 }
};

/*
 * Current config is hard-coded as we know what frequency Q6 will be running
 * on (from ASM), prior to uClock.
 */
static uClockDomainType Q6Domain =
{
  HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR, aQ6Config, &aQ6Config[1]
};

static uClockDomainType BLSP1UART1Domain =
{
  HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR, aUARTConfig, &aUARTConfig[4]
};

static uClockDomainType BLSP1UART2Domain =
{
  HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR, aUARTConfig, &aUARTConfig[4]
};

static uClockDomainType BLSP1UART3Domain =
{
  HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ADDR, aUARTConfig, &aUARTConfig[4]
};

static uClockDomainType BLSP1UART4Domain =
{
  HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ADDR, aUARTConfig, &aUARTConfig[4]
};

/*
 * Catch-all for uncontrolled domains.
 * Keep current config as XO to bypass source enabling logic.
 */
static uClockDomainType XODomain =
{
  NULL, aXOConfig, &aXOConfig[0]
};

static uClockType aClocks[] =
{
  { "clk_q6",                   0, 0, 0, &Q6Domain      },
  {
    "gcc_blsp1_uart1_apps_clk",
    HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, 0, 0,
    &BLSP1UART1Domain
  },
  {
    "gcc_blsp1_uart2_apps_clk",
    HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, 0, 0,
    &BLSP1UART2Domain
  },
  {
    "gcc_blsp1_uart3_apps_clk",
    HWIO_GCC_BLSP1_UART3_APPS_CBCR_ADDR, 0, 0,
    &BLSP1UART3Domain
  },
  {
    "gcc_blsp1_uart4_apps_clk",
    HWIO_GCC_BLSP1_UART4_APPS_CBCR_ADDR, 0, 0,
    &BLSP1UART4Domain
  },
  {
    "gcc_blsp1_ahb_clk",
    HWIO_GCC_BLSP1_AHB_CBCR_ADDR,
    HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR,
    HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK,
    &XODomain
  },
};

static uClockSourceType aSources[] =
{
  {
    .szName         = "MPLL2",
    .eSource        = UCLOCK_SOURCE_MPLL2,
    .nModeAddr      = HWIO_MSS_QDSP6SS_PLL_MODE_ADDR,
    .nVoteAddr      = 0,
    .nVoteMask      = 0,
    .pSourceConfig  = SourceFreqConfig_MPLL2
  },
  {
    .szName         = "GPLL0",
    .eSource        = UCLOCK_SOURCE_GPLL0,
    .nModeAddr      = HWIO_GCC_GPLL0_MODE_ADDR,
    .nVoteAddr      = HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR,
    .nVoteMask      = HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL0_BMSK,
    .pSourceConfig  = SourceFreqConfig_GPLL0
  },
};

static uClockBSPType uBSP =
{
  .aClocks     = aClocks,
  .aSources    = aSources,
  .nNumClocks  = SIZEOF_ARRAY(aClocks),
  .nNumSources = SIZEOF_ARRAY(aSources),
};

/*
 * Global uClock driver context
 */
static uClockCtxtType uClockDriverCtxt;


/*=========================================================================
      Function Prototypes.
==========================================================================*/

static boolean uClock_SetSourceFrequency   (uClockSourceEnumType, uClockSourceFreqConfigType*);
static boolean uClock_ConfigSource         (uClockSourceType*,    uClockSourceFreqConfigType*);
static boolean uClock_EnableSourceInternal (uClockSourceEnumType, boolean);
static boolean uClock_EnableClockInternal  (uClockIdType,         boolean);
static boolean uClock_IsOn                 (uClockIdType);

/*=============================================================================
 ==============================================================================

  A P P L I C A T I O N   P R O G R A M M E R   I N T E R F A C E

 ==============================================================================
 ============================================================================*/

/* ============================================================================
**  Function : uClock_GetClockId
** ============================================================================
*/
/**
  See uClock.h
*/

boolean uClock_GetClockId
(
  const char   *szClock,
  uClockIdType *pnId
)
{
  uint32  nClockIndex;
  boolean bFound = FALSE;

  /*
   * Sanity checks
   */
  if (szClock == NULL || pnId == NULL)
  {
    return FALSE;
  }

  /*
   * Find the clock from the list of supported clocks
   */
  for (nClockIndex = 0; nClockIndex < uClockDriverCtxt.pBSP->nNumClocks;
       nClockIndex++)
  {
    if (strcmp(szClock, uClockDriverCtxt.pBSP->aClocks[nClockIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  if (bFound)
  {
    *pnId = (uClockIdType)&uClockDriverCtxt.pBSP->aClocks[nClockIndex];
    return TRUE;
  }
  else
  {
    *pnId = NULL;
    return FALSE;
  }

} /* uClock_GetClockId */


/* ============================================================================
**  Function : uClock_SetClockFrequency
** ============================================================================
*/
/**
  See uClock.h
*/

boolean uClock_SetClockFrequency
(
  uClockIdType nClockId,
  uint32       nFreqHz,
  uint32      *pnResultFreqHz
)
{
  uClockType          *pClock;
  boolean              bResult;
  uClockMuxConfigType *pClockConfig;
  uClockMuxConfigType *pCurrentConfig;
  uClockMuxConfigType *pTargetConfig = NULL;
  uint32               nCmdVal, nCfgVal, nTimeout = 100;

  if (pnResultFreqHz != NULL)
  {
    *pnResultFreqHz = 0;
  }

  pClock = (uClockType*)nClockId;

  if (pClock                        == NULL ||
      pClock->pDomain               == NULL ||
      pClock->pDomain->nAddr        == NULL ||
      pClock->pDomain->pClockConfig == NULL)
  {
    return FALSE;
  }

  /*
   * Synchronize
   */
  qurt_mutex_lock(&uClockDriverCtxt.Mutex);

  pClockConfig = pClock->pDomain->pClockConfig;

  while (pClockConfig->nFreqHz != 0)
  {
    if (pClockConfig->nFreqHz >= nFreqHz)
    {
      pTargetConfig = pClockConfig;
      break;
    }
    pClockConfig++;
  }

  /*
   * If the requested frequency exceeded the max supported frequency, set the
   * clock to max frequency.
   */
  if (pTargetConfig == NULL)
  {
    pTargetConfig = pClockConfig - 1;
  }

  pCurrentConfig = pClock->pDomain->pCurrentConfig;

  /*
   * Short-circuit if the target config is already set
   */
  if (pCurrentConfig == pTargetConfig)
  {
    qurt_mutex_unlock(&uClockDriverCtxt.Mutex);
    return TRUE;
  }

  /*
   * Update RCG before slewing the PLL if RCG div is increasing.
   * NOTE: Currently, there is no support to enable the target PLL.
   *       If and when this is added, it will be added along with the
   *       SetSourceFrequency call. Hence, putting a check of same
   *       mux select here.
   *       If target PLL is already enabled, it will be slewed.
   */
  if ((pTargetConfig->nDiv2x > pCurrentConfig->nDiv2x) &&
      (pTargetConfig->nMuxSel == pCurrentConfig->nMuxSel))
  {
    /*
     * Form the CFG_RCGR value and write it
     */
    nCfgVal = in_dword(pClock->pDomain->nAddr + 4);
    nCfgVal &= ~(UCLOCK_CFG_RCG_SRC_SEL_FMSK | UCLOCK_CFG_RCG_SRC_DIV_FMSK |
                 UCLOCK_CFG_RCG_MODE_FMSK);
    nCfgVal |= pTargetConfig->nMuxSel << UCLOCK_CFG_RCG_SRC_SEL_SHFT;
    nCfgVal |= (pTargetConfig->nDiv2x - 1)  << UCLOCK_CFG_RCG_SRC_DIV_SHFT;

    /*
     * Set MND counter mode depending on if it is in use
     */
    if (pTargetConfig->nM != 0 && (pTargetConfig->nM < pTargetConfig->nN))
    {
      /*
       * Set M:N/D value
       */
      outpdw(pClock->pDomain->nAddr + 8, pTargetConfig->nM);
      outpdw(pClock->pDomain->nAddr + 12, NOT_N_MINUS_M(pTargetConfig));
      outpdw(pClock->pDomain->nAddr + 16, NOT_2D(pTargetConfig));

      /*
       * Dual-edge mode
       */
      nCfgVal |= ((UCLOCK_CFG_RCG_DUAL_EDGE_MODE_VAL << UCLOCK_CFG_RCG_MODE_SHFT) &
                  UCLOCK_CFG_RCG_MODE_FMSK);
    }

    out_dword(pClock->pDomain->nAddr + 4, nCfgVal);

    /*
     * Assert UPDATE and wait for it to be cleared
     */
    nCmdVal = in_dword(pClock->pDomain->nAddr);
    nCmdVal |= UCLOCK_CMD_RCG_UPDATE_FMSK;
    out_dword(pClock->pDomain->nAddr, nCmdVal);

    while ((in_dword(pClock->pDomain->nAddr) &
           UCLOCK_CMD_RCG_UPDATE_FMSK) == UCLOCK_CMD_RCG_UPDATE_FMSK)
    {
      nTimeout--;
      busywait(1);
      if (nTimeout == 0)
      {
        qurt_mutex_unlock(&uClockDriverCtxt.Mutex);
        return FALSE;
      }
    }
  }

  /*
   * Enable the target source if its different than the current one
   */
  if (pTargetConfig->nMuxSel != pCurrentConfig->nMuxSel)
  {
    if (pClock->nRefCnt > 0)
    {
      bResult = uClock_EnableSourceInternal(pTargetConfig->eSource, TRUE);
      if (!bResult)
      {
        return FALSE;
      }
    }
  }

  /*
   * Slew the PLL
   */
  if (pTargetConfig->pSourceFreqConfig)
  {
    bResult = uClock_SetSourceFrequency(
                pTargetConfig->eSource, pTargetConfig->pSourceFreqConfig);
    if (bResult != TRUE)
    {
      qurt_mutex_unlock(&uClockDriverCtxt.Mutex);
      return FALSE;
    }
  }

  /*
   * Update RCG after slewing the PLL if RCG div is decreasing or target source
   * is different than the other curent source.
   */
  if ((pTargetConfig->nDiv2x < pCurrentConfig->nDiv2x) ||
      (pTargetConfig->nMuxSel != pCurrentConfig->nMuxSel))
  {
    /*
     * Form the CFG_RCGR value and write it
     */
    nCfgVal = in_dword(pClock->pDomain->nAddr + 4);
    nCfgVal &= ~(UCLOCK_CFG_RCG_SRC_SEL_FMSK | UCLOCK_CFG_RCG_SRC_DIV_FMSK |
                 UCLOCK_CFG_RCG_MODE_FMSK);
    nCfgVal |= pTargetConfig->nMuxSel << UCLOCK_CFG_RCG_SRC_SEL_SHFT;
    nCfgVal |= (pTargetConfig->nDiv2x - 1)  << UCLOCK_CFG_RCG_SRC_DIV_SHFT;

    /*
     * Set MND counter mode depending on if it is in use
     */
    if (pTargetConfig->nM != 0 && (pTargetConfig->nM < pTargetConfig->nN))
    {
      /*
       * Set M:N/D value
       */
      outpdw(pClock->pDomain->nAddr + 8, pTargetConfig->nM);
      outpdw(pClock->pDomain->nAddr + 12, NOT_N_MINUS_M(pTargetConfig));
      outpdw(pClock->pDomain->nAddr + 16, NOT_2D(pTargetConfig));

      /*
       * Dual-edge mode
       */
      nCfgVal |= ((UCLOCK_CFG_RCG_DUAL_EDGE_MODE_VAL << UCLOCK_CFG_RCG_MODE_SHFT) &
                  UCLOCK_CFG_RCG_MODE_FMSK);
    }

    out_dword(pClock->pDomain->nAddr + 4, nCfgVal);

    /*
     * Assert UPDATE and wait for it to be cleared
     */
    nCmdVal = in_dword(pClock->pDomain->nAddr);
    nCmdVal |= UCLOCK_CMD_RCG_UPDATE_FMSK;
    out_dword(pClock->pDomain->nAddr, nCmdVal);

    while ((in_dword(pClock->pDomain->nAddr) &
           UCLOCK_CMD_RCG_UPDATE_FMSK) == UCLOCK_CMD_RCG_UPDATE_FMSK)
    {
      nTimeout--;
      busywait(1);
      if (nTimeout == 0)
      {
        qurt_mutex_unlock(&uClockDriverCtxt.Mutex);
        return FALSE;
      }
    }
  }

  /*
   * Disable the older source if its different than the target one
   */
  if (pTargetConfig->nMuxSel != pCurrentConfig->nMuxSel)
  {
    if (pClock->nRefCnt > 0)
    {
      bResult = uClock_EnableSourceInternal(pCurrentConfig->eSource, FALSE);
      if (!bResult)
      {
        return FALSE;
      }
    }
  }

  /*
   * Update the current config in domain BSP
   */
  pClock->pDomain->pCurrentConfig = pTargetConfig;

  if (pnResultFreqHz != NULL)
  {
    *pnResultFreqHz = pTargetConfig->nFreqHz;
  }

  /*
   * Release lock
   */
  qurt_mutex_unlock(&uClockDriverCtxt.Mutex);
  return TRUE;

} /* uClock_SetClockFrequency */

/* ============================================================================
**  Function : uClock_SetSourceFrequency
** ============================================================================
*/
/**
  This is an internal function to set the frequency of a source.

  This function requests a frequency change for a specified source. The
  driver applies the source config passed to it. Only slewing is supported
  currently.

  @param eSource           [in] Source to be configured
  @param pSourceFreqConfig [in] Source configuration

  @return
  TRUE  -- Requested source was found and programmed. \n
  FALSE -- Unable to find or configure source

  @dependencies
  Source should already be calibrated.

  @sa
  None.
*/

static boolean uClock_SetSourceFrequency
(
  uClockSourceEnumType        eSource,
  uClockSourceFreqConfigType *pSourceFreqConfig
)
{
  uClockSourceType *pSource;
  uint32            nSourceIndex;
  boolean           bFound = FALSE;

  /*
   * Sanity checks
   */
  if (eSource >= UCLOCK_SOURCE_TOTAL || pSourceFreqConfig == NULL)
  {
    return FALSE;
  }

  /*
   * Find the source first.
   */
  for (nSourceIndex = 0; nSourceIndex < uClockDriverCtxt.pBSP->nNumSources;
       nSourceIndex++)
  {
    if (uClockDriverCtxt.pBSP->aSources[nSourceIndex].eSource == eSource)
    {
      pSource = &uClockDriverCtxt.pBSP->aSources[nSourceIndex];
      bFound = TRUE;
      break;
    }
  }

  /*
   * Return immediately if source is not found
   */
  if (!bFound)
  {
    return FALSE;
  }

  /*
   * Apply the passed configuration
   */
  if (uClock_ConfigSource(pSource, pSourceFreqConfig) != TRUE)
  {
    return FALSE;
  }

  return TRUE;

} /* uClock_SetSourceFrequency */

/* ============================================================================
**  Function : uClock_ConfigSource
** ============================================================================
*/
/**
  This is an internal function to slew a source.

  @param pSource [in] Pointer to source entry in the aSources array
  @param pConfig [in] Source configuration

  @return
  TRUE  -- Source was slewed to the requested frequency. \n
  FALSE -- Unable to slew source

  @dependencies
  Source should already be calibrated.

  @sa
  None.
*/
static boolean uClock_ConfigSource
(
  uClockSourceType           *pSource,
  uClockSourceFreqConfigType *pConfig
)
{
  uint32  nVal, nTimeout;

  /*
   * Sanity checks
   */
  if (pSource == NULL || pConfig == NULL)
  {
    return FALSE;
  }

  /*
   * Program L and alpha values.
   */
  out_dword(pSource->nModeAddr + 4, pConfig->nL);
  out_dword(pSource->nModeAddr + 8, pConfig->nFracVal & 0xFFFFFFFF);
  out_dword(pSource->nModeAddr + 12, pConfig->nFracVal & 0xFF00000000 >> 32);

  /*
   * Assert PLL_UPDATE and wait for it to be cleared for at most ~15 us
   */
  nVal = in_dword(pSource->nModeAddr);
  nVal |= UCLOCK_PLL_MODE_PLL_UPDATE_FMSK;
  out_dword(pSource->nModeAddr, nVal);

  nTimeout = 15;
  while ((in_dword(pSource->nModeAddr) &
         UCLOCK_PLL_MODE_PLL_UPDATE_FMSK) == UCLOCK_PLL_MODE_PLL_UPDATE_FMSK)
  {
    nTimeout--;
    busywait(1);
    if (nTimeout == 0)
    {
      return FALSE;
    }
  }

  /*
   * Wait for 11 or more PLL clk_ref ticks for PLL_LOCK_DET to transition to 0.
   * When using a reference clock of XO, the wait requirement is 570 ns.
   */
  busywait(1);

  nTimeout = 200;
  while ((in_dword(pSource->nModeAddr) &
         UCLOCK_PLL_MODE_PLL_LOCK_DET_FMSK) != UCLOCK_PLL_MODE_PLL_LOCK_DET_FMSK)
  {
    nTimeout--;
    busywait(1);
    if (nTimeout == 0)
    {
      return FALSE;
    }
  }

  return TRUE;

} /* END uClock_ConfigSource */

static boolean uClock_EnableSourceInternal
(
  uClockSourceEnumType eSource,
  boolean              bEnable
)
{
  uClockSourceType *pSource;
  uint32            nVal, nSourceIndex, nTimeout = 200;
  boolean           bFound = FALSE;

  /*
   * Sanity checks
   */
  if (eSource >= UCLOCK_SOURCE_TOTAL)
  {
    return FALSE;
  }

  /*
   * XO voting not supported
   */
  if (eSource == UCLOCK_SOURCE_XO)
  {
    return TRUE;
  }

  /*
   * Find the source first
   */
  for (nSourceIndex = 0; nSourceIndex < uClockDriverCtxt.pBSP->nNumSources;
       nSourceIndex++)
  {
    if (uClockDriverCtxt.pBSP->aSources[nSourceIndex].eSource == eSource)
    {
      pSource = &uClockDriverCtxt.pBSP->aSources[nSourceIndex];
      bFound = TRUE;
      break;
    }
  }

  /*
   * Return immediately if source is not found
   */
  if (!bFound)
  {
    return FALSE;
  }

  /*
   * Vote for the PLL if vote-able
   */
  if (pSource->nVoteAddr)
  {
    if (bEnable)
    {
      /*
       * Increase the ref count - if already non-0, then short-cirucit
       */
      if (pSource->nRefCnt++ > 0)
      {
        return TRUE;
      }

      nVal = in_dword(pSource->nVoteAddr);
      nVal |= pSource->nVoteMask;
      out_dword(pSource->nVoteAddr, nVal);

      while ((in_dword(pSource->nModeAddr) &
             UCLOCK_PLL_MODE_PLL_ACTIVE_FLAG_FMSK) != UCLOCK_PLL_MODE_PLL_ACTIVE_FLAG_FMSK)
      {
        nTimeout--;
        busywait(1);
        if (nTimeout == 0)
        {
          return FALSE;
        }
      }
    }
    else
    {
      if (--pSource->nRefCnt > 0)
      {
        return TRUE;
      }

      nVal = in_dword(pSource->nVoteAddr);
      nVal &= ~pSource->nVoteMask;
      out_dword(pSource->nVoteAddr, nVal);
    }
  }
  else
  {
    // Normal enable sequences not supported
    return FALSE;
  }

  return TRUE;
}


/* ============================================================================
**  Function : uClock_IsOn
** ============================================================================
*/
/**
  Internal API which returns whether a clock is on or not.

  @param[in]  nClockId  - The ID of the clock to enable.

  @return
  TRUE  - The clock is on.
  FALSE - Otherwise.

  @dependencies
  None.

*/
static boolean uClock_IsOn
(
  uClockIdType nClockId
)
{
  uint32      nVal;
  uClockType *pClock;

  pClock = (uClockType*)nClockId;

  if (pClock           == NULL ||
      pClock->nCBCAddr == NULL)
  {
    return FALSE;
  }

  nVal = (in_dword(pClock->nCBCAddr) & UCLOCK_CBCR_CLK_OFF_FMSK) >>
           UCLOCK_CBCR_CLK_OFF_SHFT;

 if (nVal != 0)
 {
   return FALSE;
 }
 else
 {
   return TRUE;
 }

} /* uClock_IsOn */

/* ============================================================================
**  Function : uClock_EnableClockInternal
** ============================================================================
*/
/**
  Enables or disables a clock correctly.

  @param[in]  nClockId  - The ID of the clock to enable.
  @param[in]  bEnable   - Boolean on whether to enable or disable the clock.

  @return
  TRUE  - The clock was enabled or disabled correctly.
  FALSE - Otherwise.
  
  @dependencies
  None.

*/
static boolean uClock_EnableClockInternal
(
  uClockIdType nClockId,
  boolean      bEnable
)
{
  uClockType *pClock;
  boolean     bRetVal = TRUE;
  uint32      nVal, nTimeOut = 150;

  pClock = (uClockType*)nClockId;

  if (pClock           == NULL ||
      pClock->nCBCAddr == NULL)
  {
    return FALSE;
  }

  qurt_mutex_lock(&uClockDriverCtxt.Mutex);

  if (bEnable)
  {
    if (pClock->nRefCnt == 0)
    {
      /* Enable the source first */
      if (!uClock_EnableSourceInternal(
             pClock->pDomain->pCurrentConfig->eSource, TRUE))
      {
        qurt_mutex_unlock(&uClockDriverCtxt.Mutex);
        return FALSE;
      }

      if (pClock->nVoteAddr != 0)
      {
        nVal = in_dword(pClock->nVoteAddr);
        nVal |= pClock->nVoteMask;
        out_dword(pClock->nVoteAddr, nVal);
      }
      else
      {
        nVal = in_dword(pClock->nCBCAddr);
        nVal |= UCLOCK_CBCR_CLK_ENABLE_FMSK;
        out_dword(pClock->nCBCAddr, nVal);
      }
    }

    while (nTimeOut-- > 0 && uClock_IsOn(nClockId) != TRUE)
    {
      busywait(2);
    }

    if (nTimeOut != 0)
    {
      pClock->nRefCnt++;
    }
    else
    {
      bRetVal = FALSE;
    }
  }
  else
  {
    if (--pClock->nRefCnt == 0)
    {
      if (pClock->nVoteAddr != 0)
      {
        nVal = in_dword(pClock->nVoteAddr);
        nVal &= ~pClock->nVoteMask;
        out_dword(pClock->nVoteAddr, nVal);
      }
      else
      {
        nVal = in_dword(pClock->nCBCAddr);
        nVal &= ~UCLOCK_CBCR_CLK_ENABLE_FMSK;
        out_dword(pClock->nCBCAddr, nVal);
      }

      uClock_EnableSourceInternal(
        pClock->pDomain->pCurrentConfig->eSource, FALSE);
    }
  }

  qurt_mutex_unlock(&uClockDriverCtxt.Mutex); 

  return bRetVal;

} /* uClock_EnableClockInternal */


/* ============================================================================
**  Function : uClock_EnableClock
** ============================================================================
*/
/**
  See uClock.h
*/
boolean uClock_EnableClock
(
  uClockIdType nClockId
)
{
  return uClock_EnableClockInternal(nClockId, TRUE);
} /* uClock_EnableClock */


/* ============================================================================
**  Function : uClock_DisableClock
** ============================================================================
*/
/**
  See uClock.h
*/
boolean uClock_DisableClock
(
  uClockIdType nClockId
)
{
  return uClock_EnableClockInternal(nClockId, FALSE);
} /* uClock_DisableClock */


/* ============================================================================
**  Function : uClock_Init
** ============================================================================
*/
/**
  Initializes the uClock driver.

  @return
  TRUE - Driver was initialized.
  FALSE - Otherwise.

  @dependencies
  None.

*/
void uClock_Init(void)
{
  qurt_mutex_init(&uClockDriverCtxt.Mutex);
  uClockDriverCtxt.pBSP = &uBSP;

  uClockDriverCtxt.bInitialized = TRUE;

} /* uClock_Init */
