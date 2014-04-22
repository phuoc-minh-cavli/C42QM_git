/*
==============================================================================

FILE:         HALclkQDSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   QDSS clocks.

   List of clock domains:
     - HAL_clk_mGCCQDSSAPBTSCTRClkDomain
     - HAL_clk_mGCCQDSSATClkDomain
     - HAL_clk_mGCCQDSSSTMClkDomain
     - HAL_clk_mGCCQDSSTRACECLKINClkDomain
     - HAL_clk_mGCCQDSSTRIGClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/hal/clk/hw/mdm9x05/src/GCC/HALclkQDSS.c#1 $

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
 *  HAL_clk_mQDSSAPBTSCTRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSAPBTSCTRClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_dap_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_DAP_CBCR), HWIO_OFFS(GCC_QDSS_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, QDSS_DAP_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_DAP_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSAPBTSCTRClkDomain
 *
 * QDSSAPBTSCTR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSAPBTSCTRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_APB_TSCTR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSAPBTSCTRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSAPBTSCTRClkDomainClks)/sizeof(HAL_clk_mQDSSAPBTSCTRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSATClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSATClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_pcnoc_at_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_AT_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_AT_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_at_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_AT_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_AT_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSATClkDomain
 *
 * QDSSAT clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_AT_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSATClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSATClkDomainClks)/sizeof(HAL_clk_mQDSSATClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSSTMClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSSTMClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_pcnoc_qdss_stm_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_QDSS_STM_AXI_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_QDSS_STM_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_stm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_STM_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_STM_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSSTMClkDomain
 *
 * QDSSSTM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSSTMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_STM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSSTMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSSTMClkDomainClks)/sizeof(HAL_clk_mQDSSSTMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSTRACECLKINClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTRACECLKINClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_traceclkin_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TRACECLKIN_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TRACECLKIN_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTRACECLKINClkDomain
 *
 * QDSSTRACECLKIN clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACECLKINClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_TRACECLKIN_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSTRACECLKINClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTRACECLKINClkDomainClks)/sizeof(HAL_clk_mQDSSTRACECLKINClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSTRIGClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTRIGClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_trig_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TRIG_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TRIG_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTRIGClkDomain
 *
 * QDSSTRIG clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRIGClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_TRIG_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSTRIGClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTRIGClkDomainClks)/sizeof(HAL_clk_mQDSSTRIGClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};

