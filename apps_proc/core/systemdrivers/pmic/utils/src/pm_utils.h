#ifndef PM_UTILS_H
#define PM_UTILS_H

/*! \file
 *  
 *  \brief  pm_utils.c ----This file contains the implementation of utils functions.
 *  \details This file contains the implementation of utils functions.
 *  
 *    &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/utils/src/pm_utils.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/15   vk      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
uint8
pm_utils_get_upper_idx(uint32 data_value, uint32 *data_list, uint8 list_count);

pm_err_flag_type pm_utils_get_battery_id(uint32* battery_id);       /* get bat id */
pm_err_flag_type pm_utils_get_battery_volt_level(uint32* battery_voltage); //get battery voltage

#endif // PM_UTILS_H
