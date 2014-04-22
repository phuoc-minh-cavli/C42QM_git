/**
@file PrngCL_DALIntf.c 
@brief Prng Engine Core Library source file 
*/

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       S o u r c e  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW PRNG DAL interface specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/securemsm/cryptodrivers/prng/chipset/src/PrngCL_DALIntf.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2011-09-05   nk      Initial version
============================================================================*/

#include "comdef.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "PrngCL_DALIntf.h"
#include "PrngCL.h"

uint32 prng_clk_init = NULL;
DalDeviceHandle *hClock = NULL;
ClockIdType      nClockID;
/**
 * @brief   This function enables the PRNG Engine Clock 
 *          The function uses DAL interface to enable clock.
 *
 * @param None
 *
 * @return Prng_Result_Type
 *
 * @see PrngCL_DAL_Clock_Disable
 */

PrngCL_Result_Type PrngCL_DAL_Clock_Enable( void )
{

  DALResult           eResult;
  PrngCL_Result_Type    ePrngStatus = PRNGCL_ERROR_NONE;
  
  if (prng_clk_init == 0){
    DALSYS_InitMod(NULL);

    /* Get the handle for clock device */
    eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hClock);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
      return ePrngStatus;
    }

    /* Get the clock ID */
    eResult = DalClock_GetClockId(hClock, "gcc_prng_ahb_clk", &nClockID);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
     return ePrngStatus;
    }

    eResult = DalClock_EnableClock(hClock,nClockID);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
      return ePrngStatus;
    }
  }
  prng_clk_init++;

   return ePrngStatus;
}

/* TODO: Multi- threadsafe implementation of enable and disable. 
   The following disable implementation may not work.
   Since only enable is used for now. we need  to change the 
   implementation once we start using disable.
*/

/**
 * @brief   This function disables the PRNG Engine Clock 
 *          The function uses DAL interface to disable clock.
 *
 * @param None
 *
 * @return Prng_Result_Type
 *
 * @see PrngEL_DAL_Clock_Enable
 */
PrngCL_Result_Type PrngCL_DAL_Clock_Disable( void )
{
  DALResult           eResult;
  PrngCL_Result_Type    ePrngStatus = PRNGCL_ERROR_NONE;
  
  if(prng_clk_init!=0)
    prng_clk_init--;
  if (prng_clk_init == 0){
    eResult = DalClock_DisableClock(hClock,nClockID);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
      return ePrngStatus;
    }
  }
 
   return ePrngStatus;
}

