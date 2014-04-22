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

      Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/core.tx/6.0/systemdrivers/busywait/src/busywait_hw_qtimer.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
  $Change: 22400605 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
11/23/11   pbitra     Initial version.

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
   static uint32                           QTimerBase = 0;

/*==========================================================================

                         FUNCTIONS FOR MODULE

==========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT_INIT

  DESCRIPTION   This function attaches to timetick dal and gets the
                Qtimer Base address from timetick properties file.

  PARAMETERS    None

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None

==========================================================================*/

void busywait_init(void)
{

  static DalDeviceHandle *phDalTimetickHandle = NULL;
  static DALSYSPropertyVar system_property_var; 
  DALResult  result = DAL_SUCCESS;
  /*declare property handle variable */
  static DALSYS_PROPERTY_HANDLE_DECLARE(timetick_property); 
  /*Handle for communicating with the HWIO DAL.*/
  DalDeviceHandle                 *phDalHWIO;     

  if(DAL_SUCCESS == DalTimetick_Attach("SystemTimer", &phDalTimetickHandle))
  {
    /* Get the QTimer Base address from timetick properties */
    DALSYS_GetDALPropertyHandleStr("SystemTimer",timetick_property);
    result = DALSYS_GetPropertyValue(timetick_property,
                                   "QTIMER_BASE",
                           0,&system_property_var); 
    if(result == DAL_SUCCESS)
    {
      /*-----------------------------------------------------------------------*/
      /* Attach to the HWIO DAL.                                               */
      /*-----------------------------------------------------------------------*/
      result = DAL_DeviceAttach(DALDEVICEID_HWIO, &phDalHWIO);
      if (result == DAL_SUCCESS)
      {
        result = DalHWIO_MapRegionByAddress(phDalHWIO, 
                                  (uint8*)(system_property_var.Val.dwVal),
                                  (uint8 **)&(QTimerBase));
      }
      else
      {
        QTimerBase = system_property_var.Val.dwVal;
      }

      result = DALSYS_GetPropertyValue(timetick_property,
                                       "QTIMER_OFFSET",
                                       0,&system_property_var); 
      if(result == DAL_SUCCESS)
      {
        QTimerBase = QTimerBase + system_property_var.Val.dwVal;
      }
    }
  }
}

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
  uint32 start_count = 0;
  uint64 delay_count = 0;

  if (QTimerBase != NULL )
  {
    start_count = in_dword(QTimerBase);
    /*
     * Perform the delay and handle potential overflows of the timer.
     */
  
    delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ)/1000;
    while ((in_dword(QTimerBase) - start_count) < delay_count);
  }
}
