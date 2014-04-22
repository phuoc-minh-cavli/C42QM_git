#ifndef PMAPP_RTC_H
#define PMAPP_RTC_H

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
 * @file pmapp_rtc.h PMIC RTC APPLICATION related declaration.
 *
 * @brief This file contains functions prototypes and variable/type/constant
 *        declarations for the real-time clock application developed for the
 *		  Qualcomm Power Manager Chip Set.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pmapp_rtc.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
02/13/09   vk      Removed feature FEATURE_PMIC_RTC from .h file for RPC. 
06/28/07   cng     Added meta comments to RTC APIs
06/18/07   cng     Added meta comments to pm_rtc_rw_cmd
03/07/07   aab     Added pm_rtc_get_power_reset_status() API
05/31/06   Vish    Fixed LINT warnings.
01/19/05   Vish    Updated function header for pm_rtc_rw_cmd().
11/07/03   Vish    Added the task of clearing the alarm trigger condition
                   within pm_reset_rtc_alarm() so that the master
                   RTC alarm interrupt could be cleared afterward. Also
                   resetting all alarms within pm_rtc_init() to avoid
                   any false alarms.
11/03/03   Vish    Changed signature of pm_rtc_init().
                   Renamed pm_s/get_time_adjust() to pm_rtc_s/get_time_adjust()
                   in order to be consistent with earlier PM chip libraries.
                   Added documentation. Wrapped this file under
                   FEATURE_PMIC_RTC.
10/02/03   Vish    Adding alarm support.
09/24/03   Vish    Added pm_rtc_init().
09/19/03   Vish    Created.
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"
#include "pm_rtc.h"


/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/**
 * @name pmapp_rtc_get_time
 *
 * @brief This function reads the RTC time in second and milliseconds.
 *		  Milliseconds support should be there in PMIC
 *
  *
 * @param [out] time_ptr Current RTC time in Second and Milliseconds
 *								
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_device_init() will be called in SPMI is not in init state.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
 
pm_err_flag_type pmapp_rtc_get_time(pm_rtc_time_type  *time_ptr) ;


/**
 * @name pmapp_rtc_alarm_set_time
 *
 * @brief This function sets the alarm time in second and milliseconds.
 *		  Milliseconds support should be there in PMIC
 *
 *
 * @param [out] time_ptr Current RTC time in Second and Milliseconds
 *								
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_device_init() will be called in SPMI is not in init state.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
pm_err_flag_type pmapp_rtc_alarm_set_time(pm_rtc_time_type  *time_ptr);

#endif /* PMAPP_RTC_H  */
