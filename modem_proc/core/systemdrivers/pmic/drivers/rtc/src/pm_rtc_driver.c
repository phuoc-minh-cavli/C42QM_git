/*! \file
 *  \n
 *  \brief  pm_rtc_driver.c
 *  \details
 *  &copy;  Copyright (c) 2010-2016 Qualcomm Technologies, Inc. All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/rtc/src/pm_rtc_driver.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/17   kcs     Added CoreVerify.h file to supress unused varaibles warnings (CR-2000426)
02/18/11   wra     mega clocks module V6 Initial Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_rtc_driver.h"
#include "CoreVerify.h"
#include "pm_err_flags.h"

/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the RTC data */

#define RTC_DEPRECATED 0

#if RTC_DEPRECATED
static pm_rtc_data_type *pm_rtc_data_arr[PM_MAX_NUM_PMICS];
#endif
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
	
#if RTC_DEPRECATED

    pm_rtc_data_type *rtc_ptr = NULL;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    rtc_ptr =  pm_rtc_data_arr[pmic_index];

    if(rtc_ptr == NULL)
    {
        pm_malloc(sizeof(pm_rtc_data_type),(void**)&rtc_ptr);
        pm_rtc_data_arr[pmic_index] = rtc_ptr;
    }

    if (rtc_ptr != NULL)
    {
        /* Assign Comm ptr */
        rtc_ptr->comm_ptr = comm_ptr;

        /* RTC Register Info - Obtaining Data through dal config */
        rtc_ptr->megartc_register = (pm_rtc_register_info_type*)pm_target_information_get_common_info(PM_PROP_RTC_REG);

        CORE_VERIFY_PTR(rtc_ptr->megartc_register);

        rtc_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_RTC_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(rtc_ptr->num_of_peripherals != 0);
    }
#endif
}

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index)
{
#if RTC_DEPRECATED
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_rtc_data_arr[pmic_index];
    }
#endif
    return NULL;

}

uint8 pm_rtc_get_num_peripherals(uint8 pmic_index)
{
	
#if RTC_DEPRECATED
  if((pmic_index < PM_MAX_NUM_PMICS) &&
      (pm_rtc_data_arr[pmic_index] != NULL))
  {
      return pm_rtc_data_arr[pmic_index]->num_of_peripherals;
  }
#endif
  return NULL;

}

