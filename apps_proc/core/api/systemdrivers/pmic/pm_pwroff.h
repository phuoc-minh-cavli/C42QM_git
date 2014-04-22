#ifndef __PM_PWROFF_H__
#define __PM_PWROFF_H__

/**
  ===========================================================================

  @file pm_pwroff.h
  @brief Public interface include file for PMIC power-off functions.

  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_pwroff.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_err_flags.h"

/*=========================================================================
      Interface Definitions
==========================================================================*/

/* =========================================================================
**  Function : pm_shutdown
** =========================================================================*/
/**
  Prepares the PMIC for shutdown.

  This function performs any necessary actions to prepare the PMIC for a
  shutdown. This function should be called immediately before dropping PS_HOLD.

  @return
  PM_ERR_FLAG__SUCCESS -- pm_shutdown was successful. \n
  PM_ERR_FLAG__SPMI_OPT_ERR -- Error accessing the PMIC.
*/

pm_err_flag_type pm_shutdown (void);

#endif /* __PM_PWROFF_H__ */

