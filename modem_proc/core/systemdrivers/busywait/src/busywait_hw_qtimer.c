/*=========================================================================

                         Busywait Hardware QTIMER

GENERAL DESCRIPTION
  This file contains the implementation of the busywait() function for
  hardware based blocking waits that use the QTIMER as a
  reference.

EXTERNALIZED FUNCTIONS
  busywait

INITIALIZATION AND SEQUENCING REQUIREMENTS
 None

      Copyright (c) 2018 Qualcomm Technologies Incorporated.
      All rights reserved.
==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/busywait/src/busywait_hw_qtimer.c#2 $
  $DateTime: 2018/09/25 03:25:32 $
  $Author: pwbldsvc $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
09/11/18   shm     First version for 9205 codebase.

==========================================================================*/ 

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIHWIO.h"
#include "HALhwio.h"
#include "DDITimetick.h"

/*==========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==========================================================================*/

/*=========================================================================
      Constants and Macros 
==========================================================================*/

#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 19200

/*=========================================================================
      Typedefs
==========================================================================*/


/*=========================================================================
      Variables
==========================================================================*/

/*==========================================================================

                         FUNCTIONS FOR MODULE

==========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None

  RETURN VALUE  None.

  SIDE EFFECTS  The UGPT will be enabled and left running to support
                nested busywait calls, i.e. to support multithreading
                and/or ISRs.

==========================================================================*/

void busywait (uint32 pause_time_us)
{
  uint64 start_count = 0;
  uint64 delay_count = 0;

  uint64 utimer = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));

  start_count = utimer;
  /*
   * Perform the delay and handle potential overflows of the timer.
   */

  delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ)/1000;

  while ((utimer - start_count) < delay_count)
  {
    __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  }

}
