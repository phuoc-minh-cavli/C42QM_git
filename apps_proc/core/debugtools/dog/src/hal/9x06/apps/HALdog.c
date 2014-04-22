/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    W A T C H D O G     H A L

GENERAL DESCRIPTION
  These functions implement hardware specific instructions in watchdog

FUNCTIONS
  HAL_dogEnable
  HAL_dogKick
  HAL_dogAutokick
  HAL_dogEnableBark
  HAL_dogDisableBark
  HAL_dogRegisterBark
  HAL_dogIsAutokicking


  

Copyright (c) 2002 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/debugtools/dog/src/hal/9x06/apps/HALdog.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "HALhwio.h"
#include "HALdogHWIO.h"


#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"

#include "err.h"

#define DOG_BARK_ENABLE_TIMEOUT_SLCK    0x3000  /* 12288 slcks */
#define DOG_BITE_ENABLE_TIMEOUT_SLCK    0x31F3
#define DOG_BARK_DISABLE_TIMEOUT_SLCK    0x3FFF 

#define DOG_ISR                         19

#include "msmhwiobase.h"            


void dog_bark_isr(uint32 client_param);
volatile int dog_enable_hw_wdog = 1;

DalDeviceHandle * hInthandle;

volatile boolean dog_intentional_timeout_flag = FALSE;

/** =====================================================================
 * Function:
 *     HAL_dogEnable
 * 
 * Description:
 *     This function enables the hardware dog
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnable( void )
{
    HWIO_OUT(WDOG_CTL, dog_enable_hw_wdog);
    HWIO_OUT(WDOG_BITE_TIME, DOG_BITE_ENABLE_TIMEOUT_SLCK);
}

/** =====================================================================
 * Function:
 *     HAL_dogDisable
 * 
 * Description:
 *     This function disables the hardware dog
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisable( void )
{
    if (!dog_intentional_timeout_flag)
    {
        HWIO_OUT(WDOG_CTL, 0);
    }
}

/** =====================================================================
 * Function:
 *     HAL_dogKick
 * 
 * Description:
 *     This function kicks the hardware dog
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogKick( void )
{
    if (!dog_intentional_timeout_flag)
    {
        HWIO_OUT( WDOG_RESET, 1 );
    }
}

/** =====================================================================
 * Function:
 *     HAL_dogAutokick
 * 
 * Description:
 *     This function turns on dog autokick
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogAutokick( void )
{
    if (!dog_intentional_timeout_flag)
    {
        HWIO_OUT(WDOG_CTL, 0);
    }
}

/** =====================================================================
 * Function:
 *     HAL_dogEnableBark
 * 
 * Description:
 *       Enable the watchdog bark timer. 
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnableBark( void )
{
    HWIO_OUT( WDOG_BARK_TIME, DOG_BARK_ENABLE_TIMEOUT_SLCK );
}

/** =====================================================================
 * Function:
 *     HAL_dogDisableBark
 * 
 * Description:
 *       Enable the watchdog bark timer. 
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisableBark( void )
{
    HWIO_OUT( WDOG_BARK_TIME, DOG_BARK_DISABLE_TIMEOUT_SLCK );
}

/** =====================================================================
 * Function:
 *     HAL_dogRegisterBark
 * 
 * Description:
 *     Register an ISR for the watchdog bark interrupt.
 * 
 * Parameters:
 *     none
 * 
 * 
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogRegisterBark( void )
{
    if ((DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hInthandle) != DAL_SUCCESS) || (hInthandle == NULL))
        ERR_FATAL("Dog cannot register with DAL", 0, 0, 0);
        
    DalInterruptController_RegisterISR(hInthandle, DOG_ISR, (DALIRQ)dog_bark_isr, 0, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);
}

 /** =====================================================================
 * Function:
 *     HAL_dogIsAutokicking
 * 
 * Description:
 *     Check if dog is autokicking 
 * 
 * Parameters:
 *     uint32 status pointer
 * 
 * Returns:
 *     pointer set to 1 if autokicking, 0 otherwise
 * =====================================================================  */
 void HAL_dogIsAutokicking(uint32 *status)
 {
	if(HWIO_INF(WDOG_STATUS,FROZEN))
        *status = 1;
 	else
		*status = 0;
	return;
 }

 /** =====================================================================
 * Function:
 *     HAL_dogForceBite
 *
 * Description:
 *       Force the watchdog to bite
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogForceBite( void )
{
	/* Enable the hw watchdog before doing a force bite,
	 * just to be sure that it will be enabled 
	 */
	HWIO_OUT(WDOG_CTL, 1);

	/* Do a force bite */
    HWIO_OUT(WDOG_BITE_TIME, 0x1);
}

