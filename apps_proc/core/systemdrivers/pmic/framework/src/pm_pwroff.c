/**
  ===========================================================================

  @file pm_pwroff.c
  @brief Implementation file for PMIC power-off API's.

  ===========================================================================

  Copyright (c) 2012,2020 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.tx/6.0/systemdrivers/pmic/framework/src/pm_pwroff.c#2 $
  $DateTime: 2020/07/21 09:43:37 $
  $Author: pwbldsvc $

  when		 who	 what, where, why
  --------	 ---	 ----------------------------------------------------------
  06/27/16	 rk 	 MSM9206: API porting from DALsys to Threadx (CR - 1034553) 

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DALSys.h"
#include "pm_comm.h"
#include "pm_pwroff.h"
#include "busywait.h"
#include "pm_target.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

#define PON_PS_HOLD_RESET_CTL_ADDR  0x85A
#define PON_PON_TRIGGER_EN_ADDR     0x880

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : pm_shutdown
** =========================================================================*/
/**
  See pm_pwroff.h
*/

pm_err_flag_type pm_shutdown (void)
{
  pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;
  
  /*
   * Disable the PS_HOLD trigger and wait at least 10 sleep clock cycles before
   * changing the trigger action.
   */
  err |= pm_comm_write_byte_mask(0,PON_PS_HOLD_RESET_CTL_ADDR, 0x80, 0x00, 0);
  busywait(310);

  /*
   * Configure PS_HOLD to perform a normal shutdown.
   */
  err |= pm_comm_write_byte_mask(0,PON_PS_HOLD_RESET_CTL_ADDR, 0xF, 0x4, 0);

  /*
   * Enable the PS_HOLD trigger.
   */
  err |= pm_comm_write_byte_mask(0,PON_PS_HOLD_RESET_CTL_ADDR, 0x80, 0x80, 0);

  /*
   * Disable SMPL PON trigger.
   */
  err |= pm_comm_write_byte_mask(0,PON_PON_TRIGGER_EN_ADDR, 0x2, 0x0, 0);
  err |= pm_shutdown_target(); // if any target specific requirements
  if (err != PM_ERR_FLAG__SUCCESS)
  {
    return PM_ERR_FLAG__SPMI_OPT_ERR;
  }
  
  return PM_ERR_FLAG__SUCCESS;
}
