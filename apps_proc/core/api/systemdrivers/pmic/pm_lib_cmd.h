#ifndef PM_LIB_CMD_H
#define PM_LIB_CMD_H

/** @file pm_lib_cmd.h
 *  
 *
 * PMIC library command parameters 
 * This file is defines commands used in PMIC library APIs 
 *
 *
 *  Copyright (c) 2003-2013 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_lib_cmd.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/
/** @addtogroup pm_lib_cmd
@{ */

/**
 * Use these commands to enable or disable the different PMIC devices.
 */
typedef enum
{
   /**
    * Command to turn off.
    */
   PM_OFF_CMD,
   /**
    * Command to turn on.
    */
   PM_ON_CMD,
   /**
    * Invalid selection, used for range checking in PMIC APIs.
    */
   PM_INVALID_CMD
}pm_switch_cmd_type;

/** @} */ /* end_addtogroup pm_lib_cmd */

#endif
