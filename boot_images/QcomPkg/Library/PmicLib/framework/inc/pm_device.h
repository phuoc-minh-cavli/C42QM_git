#ifndef __PM_DEVICE_H__
#define __PM_DEVICE_H__

/*! \file pm_device.h
*  \n
*  \brief This header file contains enums and helper function declarations needed
*  \n during PMIC device initialization .
*  \n
*  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/18   sv      Added SPMI config API's
08/03/16   akm     Created
 
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_boot.h"
#include "pm_version.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*
 * pm_periph_owner
 *
 * A type to choose the interrupt and peripehral owner.
 */
typedef enum 
{
  PM_APPS_HLOS_OWNER = 0,
  PM_APPS_TZ_OWNER,
  PM_MSS_OWNER,
  PM_LPASS_OWNER,
  PM_RPM_OWNER,
  PM_SSC_OWNER,
  PM_SECPROC_OWNER
} pm_periph_owner;

/*===========================================================================

                        FUNCTION PROTOTYPES 

===========================================================================*/

/*! \brief This function executes the pmic device setup needed to communicate with PMICs
 *   \brief  It initilizes SPMI bus and detects PMIC on target
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_device_setup(void);


/*! \brief This function executes the pmic device setup needed to communicate with PMICs
 *   \brief  It initilizes for dynamic SPMI channel configuration
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_spmi_dynamic_chnl_init(void);


/*! \brief This function executes the pmic device setup needed to communicate with PMICs
 *   \brief  It initilizes for static SPMI channel, PVC Port and VRM Port configuration.
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_spmi_static_chnl_init(void);

/** 
 * @name pm_is_pmic_present 
 *  
 * @brief This function check PMIC presence
 * 
 * @param[in] pmic_index 
 *
 * @return 
 *     TRUE : PMIC is Present
 *     FALSE: PMIC is NOT Present
 *
 */
boolean 
pm_is_pmic_present(uint8 pmic_index);

/** 
 * @name pm_is_target_pre_silicon 
 *  
 * @brief This function checks if target is pre-silicon 
 * 
 * @param[in] pmic_index 
 *
 * @return 
 *    TRUE  : Tartet is Pre-Silicon
 *    FALSE : Target is NOT Pre-Silicon
 *
 */
boolean 
pm_is_target_pre_silicon(void);


#endif // __PM_DEVICE_H__
