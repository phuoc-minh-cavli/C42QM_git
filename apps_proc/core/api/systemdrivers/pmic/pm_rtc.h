#ifndef PM_RTC_H
#define PM_RTC_H

/**
 * Copyright (c) 2020 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
 *
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
 *
 */

/**
 * @file pm_rtc.h PMIC RTC related declaration.
 *
 * @brief This file contains enums and API definitions for Real Time Clock Driver.
 */

/*===========================================================================

           R T C   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  This file contains functions prototypes and variable/type/constant
  declarations for the RTC services developed for the Qualcomm Power
  Management IC.

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_rtc.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
19/02/20   ks     Created.
===========================================================================*/

/*==========================================================================

                        INCLUDE FILES

========================================================================= */
#include "com_dtypes.h"
#include "pm_err_flags.h"

/*==========================================================================

                         TYPE DEFINITIONS

========================================================================= */

/**
* @struct pm_rtc_time_type
* @brief RTC time structure used for setting and retrieving current time and
*        for setting the alarm time. It supports millisecond resolution on 
*        supported hardware.
*/
typedef struct
{
   uint32  sec;  /*!< RTC time in seconds. */
   uint32  msec; /*!< RTC time in milleseconds */
} pm_rtc_time_type;


/* =========================================================================

                    RTC DRIVER FUNCTION PROTOTYPES

========================================================================= */

/**
 * @name pm_rtc_rw_get_ms_time
 *
 * @brief Gets the RTC RW time in milli second resolution
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] time_ptr Pointer to the time stamp structure in second and milleseconds.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note This function is stand alone and should not depend on other framework init
 */
pm_err_flag_type pm_rtc_rw_get_ms_time(uint8 pmic_chip, pm_rtc_time_type  *time_ptr);


/**
 * @name pm_rtc_alarm_set_ms_time
 *
 * @brief Gets the RTC RW time in milli second resolution
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] time_ptr Pointer to the time stamp structure in second and milleseconds.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note This function is stand alone and should not depend on other framework init
 */
pm_err_flag_type pm_rtc_alarm_set_ms_time(uint8 pmic_chip, pm_rtc_time_type  *time_ptr);


/**
 * @name pm_rtc_alarm_enable
 *
 * @brief enables RTC alarm 
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [in] enable Enable (TRUE) or disable (FALSE) the ALARM.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note This function is stand alone and should not depend on other framework init
 */
pm_err_flag_type pm_rtc_alarm_enable(uint8 pmic_chip, boolean  enable);

#endif    /* PM_RTC_H */

