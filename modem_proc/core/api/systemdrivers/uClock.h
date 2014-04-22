#ifndef UCLOCK_H
#define UCLOCK_H
/*
===========================================================================
*/
/**
  @file uClock.h
  @brief Public definitions include file for accessing the micro lightweight
         clock device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF.

      This file is the public header file for the MPSS page-only mode clock
      driver implementation. It is meant to be used by a small subset of
      drivers, which cannot call into the DAL clock driver APIs. These APIs
      are only supported page-only mode.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.10/api/systemdrivers/uClock.h#2 $
  $DateTime: 2019/09/19 05:28:49 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  09/19/19   shm     Add clock enable/disable APIs
  01/16/19   shm     First version for page-only mode requirements

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
  Clock handle.
 */
typedef uint32 uClockIdType;


/*=========================================================================
      Function Definitions.
==========================================================================*/


/* ============================================================================
**  Function : uClock_GetClockId
** ============================================================================
*/
/**
  Looks up a clock ID for a specified clock name.

  This function takes the name of a clock and returns a handle that
  can be used for making requests on that clock. Usually, this function
  must be the first to be invoked prior to using any other clock functions.
  The string name for Q6 clock is "clk_q6".

  @param szClock   [in]  String name of the clock.
  @param pnId      [out] Pointer to the clock ID to be filled in.

  @return
  TRUE -- Clock was found and the ID was filled in. \n
  FALSE -- Clock was not found on this target or pointer to clock name or ID is null.

  @dependencies
  None.
*/

boolean uClock_GetClockId
(
  const char   *szClock,
  uClockIdType *pnId
);

/* ============================================================================
**  Function : uClock_SetClockFrequency
** ============================================================================
*/
/**
  Sets the frequency of a clock.

  This function requests a frequency change for a specified clock. The
  driver selects the minimum frequency equal or higher to the requested
  frequency. If the requested frequency is greater than the max supported
  frequency, then the max frequency will be set.
  Following caveats are to be noted:
  - Voltage requests will not be made. Clients are expected to place the voltage
    requirement votes for target source/clock.
  - There is no support for concurrent requests; requests will not be
    aggregated and the last request will be serviced and override any previous
    frequency requests.
  - The source from which the target frequency will run on should already be
    enabled. This API will only perform source slewing, if required.
    
    Currently only supports Q6 clock. Please contact the clock team to discuss
    support for other clocks in future.

  @param nClock         [in]  Clock for which to set the frequency.
  @param nFreqHz        [in]  Minimum clock frequency in Hz
  @param pnResultFreqHz [out] The programmed clock frequency in Hz; can be
                              NULL if the resulting frequency does not need
                              to be checked.

  @return
  TRUE -- Requested clock frequency was found and programmed. \n
  FALSE -- Unable to set the frequency

  @dependencies
  None.

  @sa
  uClock_GetClockId()
*/

boolean uClock_SetClockFrequency
(
  uClockIdType nClockId,
  uint32       nFreqHz,
  uint32      *pnResultFreqHz
);


/* ============================================================================
**  Function : uClock_EnableClock
** ============================================================================
*/
/**
  Enables a clock.

  @param[in]  nClockId  - The ID of the clock to enable.

  @return
  TRUE  - The clock was enabled.
  FALSE - Otherwise.
  
  @dependencies
  None.

*/

boolean uClock_EnableClock
(
  uClockIdType nClockId
);


/* ============================================================================
**  Function : uClock_DisableClock
** ============================================================================
*/
/**
  Disables a clock.

  @param[in]  nClockId  - The ID of the clock to disable.

  @return
  TRUE  - The clock was disabled.
  FALSE - Otherwise.

  @dependencies
  None.

*/

boolean uClock_DisableClock
(
  uClockIdType nClockId
);

#endif /* UCLOCK_H */
