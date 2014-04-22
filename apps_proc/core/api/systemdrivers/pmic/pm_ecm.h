#ifndef PM_ECM_H
#define PM_ECM_H

/** @file pm_ecm.h
*
*  PMIC-ECM MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*   the PMIC ECM module.
*/
/*
*  Copyright (c) 2016 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_ecm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/24/16   vk     Created
===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"
#include "bare_metal_info.h"
#include "pm_comm.h"

typedef enum
{
    PM_MISC_ECM_CH1,  /**< Channel 1 for 0x980 register */
    PM_MISC_ECM__INVALID
}pm_ecm_ext_channel_type;

/**
   Enables or disables a MISC_ECM_CTL.

    @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] on_off               Turns the local ECM on and off.
                                    See #pm_on_off_type.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ecm_ext_channel_enable_status( pm_ecm_ext_channel_type Ch , pm_on_off_type *on_off);


#endif /* PM_ECM_H */
