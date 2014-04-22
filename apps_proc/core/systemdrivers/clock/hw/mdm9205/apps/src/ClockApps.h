#ifndef CLOCKAPPS_H
#define CLOCKAPPS_H
/*
===========================================================================
*/
/**
  @file ClockApps.h

  Internal header file for the clock device driver on the apps image.
*/
/*
  ====================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  ==================================================================== 
  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/hw/mdm9205/apps/src/ClockApps.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/08/19   shm     Featurizing stub and test framework code for memory
                     optimization
  10/02/18   shm     First version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DDIClock.h"
#include "ClockAPCSBSP.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*
 * Resource scaling disable flags
 *   BY_USER
 *     Set: At init or runtime via debugger.
 *     Cleared: Never.
 *     Usage: Disable feature (at compile or runtime).
 *   BY_EFS
 *     Set: At init.
 *     Cleared: Clock_LoadNV().
 *     Usage: Disable feature until EFS is loaded.
 *   BY_EFUSE
 *     Set: When efuse reads feature not supported.
 *     Cleared: Never.
 *     Usage: Disable feature until efuse is read.
 *   BY_CPU_TEST:
 *     Set: When \clk\cpu.test is active.
 *     Cleared: When \clk\cpu.test transitions to inactive.
 *     Usage: Disable feature while \clk\cpu.test is active.
 *   BY_BSP:
 *     Set: When the BSP flag is set to disable the feature.
 *     Usage: Set when init reads the BSP data.
 *   HW_LOGIC:
 *     Set: At init or runtime via debugger.
 *     Cleared: Never.
 *     Usage: Disable HAL activity for resource.
 */
#define CLOCK_FLAG_DISABLED_BY_USER                (0x1U << 0)
#define CLOCK_FLAG_DISABLED_BY_EFS                 (0x1U << 1)
#define CLOCK_FLAG_DISABLED_BY_EFUSE               (0x1U << 2)
#define CLOCK_FLAG_DISABLED_BY_CPU_TEST            (0x1U << 3)
#define CLOCK_FLAG_DISABLED_BY_BSP                 (0x1U << 4)
//#define CLOCK_FLAG_DISABLED__x                   (0x1U << 5)
#define CLOCK_FLAG_DISABLED_HW_LOGIC               (0x1U << 31)

/*
 * Clock NPA Node Names
 */
#define CLOCK_NPA_NODE_NAME_CPU                    "/clk/cpu"
#define CLOCK_NPA_NODE_NAME_CPU_TEST               "/clk/cpu.test"
#define CLOCK_NPA_NODE_NAME_CXO                    "/xo/cxo"
#define CLOCK_NPA_NODE_NAME_QDSS                   "/clk/qdss"
#define CLOCK_NPA_NODE_NAME_PCNOC                 "/clk/pcnoc"
#define CLOCK_NPA_NODE_NAME_SNOC                   "/clk/snoc"
#define CLOCK_NPA_NODE_NAME_BIMC                   "/clk/bimc"
#define CLOCK_NPA_NODE_NAME_IPA                    "/clk/ipa"
#define CLOCK_NPA_NODE_NAME_QPIC                   "/clk/qpic"
#define CLOCK_NPA_NODE_NAME_DCVS                   "/clk/dcvs.ena"

/*
 * Other NPA Node Names
 */
#define CLOCK_PMIC_RESOURCE_VDD_CX                "/pmic/client/rail_cx"

/*
 * Clock NPA Client Names
 */
#define CLOCK_NPA_CLIENT_NAME_CLK_CPU_IMPULSE     "/clk/cpu/impulse"

/*
 * Definitions for NPA BIST
 */

/*
 * The measured clock frequency must match the desired frequency within
 * the following error margin expressed in percent of the desired
 * frequency.
 */
#define CLOCK_NPA_BIST_ERROR_MARGIN_PERCENT         1

/*
 * Default size of the NPA BIST ULOG log buffer.
 *
 * NOTE: The log size should be specified in powers of 2.
 */
#define CLOCK_NPA_BIST_DEFAULT_LOG_SIZE   65536

/*
 * Macro to return absolute value of the difference between two values
 */
#define ABS_DIFF(v1, v2)  ((v1 >= v2) ? (v1 - v2) : (v2 - v1))

/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * CPU context.
 *
 *  pClock            - Clock node.
 *  pConfig           - Clock domain configuration.
 *  PerfConfig        - Active clock domain configuration.
 *  nDisableDCS       - Disable flag for clock scaling.
 *  hNPAClkCPUImpulse - NPA handle to impulse client on /clk/cpu.
 *  bNPABISTEnabled   - Flag used to enable the NPA BIST for CPU.
 */
typedef struct
{
  ClockNodeType           *pClock;
  ClockCPUConfigType      *pConfig;
  ClockCPUPerfConfigType   PerfConfig;
  ClockCPUConfigType      *pSavedConfig;
  uint32                   nDisableDCS;
  npa_client_handle        hNPAClkCPUImpulse;
  #ifdef CLOCK_BIST_FWK_ENABLE
  boolean                  bNPABISTEnabled;
  #endif
} ClockCPUCtxtType;


/**
 * Clock driver image context.
 */
typedef struct
{
  ClockCPUCtxtType          CPUCtxt;
  ClockImageBSPConfigType  *pBSPConfig;
  #ifdef CLOCK_BIST_FWK_ENABLE
  ULogHandle                hClockNPABISTLog;
  uint32                    nNPABISTLogSize;
  #endif
} ClockImageCtxtType;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitPLL
** =========================================================================*/
/**
  Initialize the apps PLL management subsystem.

  This function initializes NPA nodes and resources to control apps PLLs.

  @param *pDrvCtxt [in] -- The driver context.

  @return
  DAL_SUCCESS

  @dependencies
  None.
*/

DALResult Clock_InitPLL
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitXO
** =========================================================================*/
/**
  Initialize the XO management subsystem.
 
  This function initializes the XO LPR nodes required for communicating 
  resource requests to the RPM.
 
  @param *pDrvCtxt [in] -- The driver context.

  @return
  DAL_SUCCESS

  @dependencies
  None.
*/ 


DALResult Clock_InitXO
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_SetCPUConfig
** =========================================================================*/
/**
  Sets CPU configuration.

  This function sets CPU configuration based on input parameters.

  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param  pConfig  [in] -- New configuration.

  @return
  DAL_SUCCESS -- CPU clock set successfully.
  DAL_ERROR  --  CPU clock not set successfully.

  @dependencies
  None.
*/

DALResult Clock_SetCPUConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockCPUConfigType *pNewConfig
);


/* =========================================================================
**  Function : Clock_FindCPUMaxConfigAtVoltage
** =========================================================================*/
/**
  Finds the maximum CPU config at the specified voltage level.

  @param *pDrvCtxt[in] -- Pointer to driver context.
  @param **pConfig[in] -- Pointer to CPU config pointer.
  @param nCorner[in]   -- Corner id from pmapp_npa.h

  @return
  DAL_ERROR if configuration was not valid, other DAL_SUCCESS.

  @dependencies
  None.
*/

DALResult Clock_FindCPUMaxConfigAtVoltage
(
  ClockDrvCtxt         *pDrvCtxt,
  ClockCPUConfigType  **pConfig,
  uint32                nCorner
);


#ifdef CLOCK_BIST_FWK_ENABLE
/* =========================================================================
**  Function : Clock_NPA_BIST
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on CPU clocks

  @param *pDrvCtxt[in] -- Pointer to driver context.
  
  @dependencies
  NPA and CPU clock initialization must be complete prior to running the
  NPA BIST.

*/

void Clock_NPA_BIST
(
  ClockDrvCtxt    *pDrvCtxt
);
#endif

#endif /* !CLOCKAPPS_H */
