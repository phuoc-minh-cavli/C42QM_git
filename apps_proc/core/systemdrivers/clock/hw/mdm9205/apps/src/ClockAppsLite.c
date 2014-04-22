/*
===========================================================================
*/
/**
  @file ClockAppsLite.c
  
  This is a lightweight, non-DAL, implementation for clocks that must
  be available prior to the DAL clock driver being initialized.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  ==================================================================== 
  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/hw/mdm9205/apps/src/ClockAppsLite.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIClock.h"
#include "ClockDriver.h"
#include "ClockLiteHWIO.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


#define CLOCK_NAME_PRNG_AHB                 "gcc_prng_ahb_clk"
#define CLOCK_LITE_MAX_ENABLE_READ_ATTEMPTS 150

/*
 * Read interval in microseconds for PRNG Clock enable attempts
 */
#define READ_INTERVAL_PRNG_CLK_US           1

/*
 * Frequency of the QTimer Frame 0 in KHz
 */
#define BUSYWAIT_XO_FREQUENCY_IN_KHZ        19200

/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/*
 * Handle to the clock DAL.
 */
static DalDeviceHandle *hClock;

/*
 * PRNG clock identifier.
 */
static ClockIdType nPRNGClockId;

/*
 * Reference count for prng enable history in lite implementation.
 */
static uint32 nPRNGReferenceCount;


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_EnablePRNG
** =========================================================================*/

DALResult Clock_EnablePRNG(void)
{
  ClockDrvCtxt *pDrvCtxt;
  DALResult     eResult;
  
  /*-----------------------------------------------------------------------*/
  /* ENTER CRITICAL SECTION.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = Clock_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Use DAL API if the clock driver has initialized.                      */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->pImageCtxt != NULL)
  {
    /*
     * Sanity.
     */
    if (hClock == NULL || nPRNGClockId == 0)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Invoke DAL API to enable the PRNG clock.
     */
    eResult = DalClock_EnableClock(hClock, nPRNGClockId);
    if(eResult != DAL_SUCCESS)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Otherwise, enable the PRNG clock directly using HWIO.                 */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (nPRNGReferenceCount == 0)
    {
      uint32 nTimeout = 0;
      uint32 nStartCount, nWaitInterval;

      HWIO_OUTF(PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 1);
      while (HWIO_INF(GCC_PRNG_AHB_CBCR, CLK_OFF))
      {
        /*
         * Return error for timeout.
         */
        if (nTimeout++ == CLOCK_LITE_MAX_ENABLE_READ_ATTEMPTS)
        {
          Clock_ReleaseDrvCtxt();
          return DAL_ERROR;
        }

        /*
         * Wait 1 us between read attempts
         */
        nStartCount = HWIO_IN(APCS_F0_QTMR_V1_CNTPCT_LO);

        /*
         * Number of QTimer ticks in one microsecond
         */
        nWaitInterval = (READ_INTERVAL_PRNG_CLK_US * BUSYWAIT_XO_FREQUENCY_IN_KHZ)/1000;

        while (HWIO_IN(APCS_F0_QTMR_V1_CNTPCT_LO) < nWaitInterval + nStartCount);
      }
    }

    nPRNGReferenceCount++;
  }

  /*-----------------------------------------------------------------------*/
  /* EXIT CRITICAL SECTION.                                                */
  /*-----------------------------------------------------------------------*/

  Clock_ReleaseDrvCtxt();

  return DAL_SUCCESS;

} /* END Clock_EnablePRNG */


/* =========================================================================
**  Function : Clock_DisablePRNG
** =========================================================================*/

DALResult Clock_DisablePRNG(void)
{
  ClockDrvCtxt *pDrvCtxt;
  DALResult     eResult;

  /*-----------------------------------------------------------------------*/
  /* ENTER CRITICAL SECTION.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = Clock_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Use DAL API if the clock driver has initialized.                      */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->pImageCtxt != NULL)
  {
    /*
     * Sanity.
     */
    if (hClock == NULL || nPRNGClockId == 0)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }

    eResult = DalClock_DisableClock(hClock, nPRNGClockId);
    if(eResult != DAL_SUCCESS)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Otherwise, disable the PRNG clock directly using HWIO.                */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (nPRNGReferenceCount == 1)
    {
      HWIO_OUTF(PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 0);
    }

    if (nPRNGReferenceCount > 0)
    {
      nPRNGReferenceCount--;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* EXIT CRITICAL SECTION.                                                */
  /*-----------------------------------------------------------------------*/

  Clock_ReleaseDrvCtxt();

  return DAL_SUCCESS;

} /* END Clock_DisablPRNG */


/* =========================================================================
**  Function : Clock_TransferReferenceCount
** =========================================================================*/

DALResult Clock_TransferReferenceCount(void)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Sanity                                                                */
  /*-----------------------------------------------------------------------*/

  if (hClock && nPRNGClockId)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize handle for DAL API.                                        */
  /*-----------------------------------------------------------------------*/

  eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hClock);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "Unable to attach to clock DAL: %d",
      eResult,0,0);

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize PRNG clock id.                                             */
  /*-----------------------------------------------------------------------*/

  eResult = DalClock_GetClockId(hClock, CLOCK_NAME_PRNG_AHB, &nPRNGClockId);
  if(eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Transfer enable reference counts to the DAL driver.                   */
  /*-----------------------------------------------------------------------*/

  for ( ; nPRNGReferenceCount > 0; nPRNGReferenceCount--)
  {
    eResult = DalClock_EnableClock(hClock, nPRNGClockId);
    if(eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_TransferReferenceCount */

