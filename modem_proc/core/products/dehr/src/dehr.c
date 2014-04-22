 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    D E H R     I N I T I A L I Z A T I ON

GENERAL DESCRIPTION

  The DEHR init  will help to set DEHR_VMIDMT_MCR from modem and configuring DEHR_BIMC_DEHR_SEC_CFG enable and lock bit for DEHR sequence in RPM 

INITIALIZATION AND SEQUENCING REQUIREMENTS

  This initialization should happen before entering BIMC power collapse otherwise it leads to crash
  To write in to DEHR_VMIDMT_MCR  register GCC_DEHR_CBCR clock should be enabled which is happening in DEHR_Clock_Enable

Copyright (c) 2002 - 2015,2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who     		what, where, why
--------   ---     		----------------------------------------------------------
05/06/18   ahemanth     Added the DEHR_INIT function ,DEHR_Clock_Enable and DEHR_Clock_Disable
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <HALhwio.h>
#include "dehr_hwio.h"
#include "DALDeviceId.h"
#include "DDIClock.h"

DalDeviceHandle   *Clock_handle = NULL;
ClockIdType       Clock_ID;

/*===========================================================================

FUNCTION DEHR_Clock_Enable

DESCRIPTION
  This Function Initialize the DAL handle and enable GCC_DEHR_CBCR clock .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If DAL didn't return DAL_SUCCESS then it will result in Fatal error

===========================================================================*/

DALResult DEHR_Clock_Enable( void)
{
	DALResult             eResult;
	if (Clock_handle == NULL)
    {
		  DALSYS_InitMod(NULL);
		  /* Get the handle for clock device */
		  eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &Clock_handle);
		  if(eResult != DAL_SUCCESS)
		  {
			 DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR, "Unable to attach to dal_clock_driver");
			return eResult;
		  }
		  /* Get the clock ID */
		  eResult = DalClock_GetClockId(Clock_handle, "gcc_dehr_clk", &Clock_ID);
		  if(eResult != DAL_SUCCESS)
		  {
			DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR, "Unable to get clock id for DEHR_clk");
			return eResult;
		  }
	}  		
	eResult = DalClock_EnableClock(Clock_handle,Clock_ID);
	if(eResult != DAL_SUCCESS)
	{
		DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR, "Error enabling DEHR_clk");
		return eResult;
	}
    return eResult;
}

/*===========================================================================

FUNCTION DEHR_Clock_Disable

DESCRIPTION
  This function will disable GCC_DEHR_CBCR clock only if  DEHR_Clock_Enable returns DAL_SUCCESS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If DAL didn't return DAL_SUCCESS then it will result in Fatal error 

===========================================================================*/

void  DEHR_Clock_Disable( void)
{
	DALResult   eResult; 		
	eResult = DalClock_DisableClock(Clock_handle,Clock_ID);
	if(eResult != DAL_SUCCESS)
	{
		DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR, "Error disabling DEHR_clk");
	}
}




/*===========================================================================

FUNCTION DEHR_INIT

DESCRIPTION
  This Init function will enable the DEHR clock if it is successfull then it will
  set DEHR_VMIDMT_MCR and DEHR_BIMC_DEHR_SEC_CFG to respective values 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If this function didn't initialized then BIMC PC dont work and may result in noc error

===========================================================================*/


void DEHR_init ( void)
{
	DALResult eResult;
	eResult = DEHR_Clock_Enable();
	if (eResult == DAL_SUCCESS )
	{
		HWIO_OUTF(DEHR_VMIDMT_MCR,BPSMSACFG,1);
		HWIO_OUTF(DEHR_BIMC_DEHR_SEC_CFG, DEHR_LOCK, 1);
		HWIO_OUTF(DEHR_BIMC_DEHR_SEC_CFG, DEHR_ENA, 1);
		DEHR_Clock_Disable();
	}
}