#ifndef __PMAPP_PON_H__
#define __PMAPP_PON_H__
/*===========================================================================


               P M A P P_ P O N  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations for the PMIC PON application
  
Copyright (c) 2013, 2023            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pmapp_pon.h#4 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
2/26/14    rh      Created file
===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"
#include "DDISpmi.h"
#include "timer.h"

/*===========================================================================

                PM APP PON TYPE DEFINITIONS

===========================================================================*/

//Function callback definition to be triggered after timer expiry
typedef void (*pmapp_pon_fn_cb) (void);

#ifdef  FEATURE_TX_SP_AWARE_TRACKER
typedef void (*pmapp_pon_key_event_cb) (uint8);
#endif
//Data to be passed through PON ISR
typedef struct pmapp_pon_isr_data
{
    timer_ptr_type timer;
    uint32 duration;
    boolean* timer_status;
}pmapp_pon_isr_data;

/*! \enum pm_app_misc_cfg_type
   \brief :to configure the PMIC misc register which controls the 
   	Tiny/full Modem loading and PSM mode.
 */

typedef enum
{
  PM_APP_MISC_CFG_PSM,
  PM_APP_MISC_CFG_MAX
}pm_app_misc_cfg_type;

typedef enum
{
  PM_APP_KEY_PRESS = 1,
  PM_APP_KEY_RELEASE_SHORT,
  PM_APP_KEY_RELEASE_LONG
}pm_app_key_event;

/*===========================================================================

                PM APP PON FUNCTION PROTOTYPES

===========================================================================*/

/*=========================================================================== */
/*                     pmapp_pon_cblpwr_register_cb                 	      */
/*=========================================================================== */
/**
  This function registers a callback that will be triggered once the CBLPWR_N
  signal(For 9205: KPDPWR Key ) is held for a pre-configured amount of time

  PARAMETERS
 * @param[in]  cb: Callback to be triggered
  @return      pm_err_flag_type - PMIC error flag status 

*/
#ifdef  FEATURE_TX_SP_AWARE_TRACKER
pm_err_flag_type pmapp_pon_cblpwr_register_cb(pmapp_pon_key_event_cb cb);
#else
pm_err_flag_type pmapp_pon_cblpwr_register_cb(pmapp_pon_fn_cb cb);
#endif
/**
 * @brief gets the Configurations of PMIC MISC_MISC_PSM , register 
 *        
 * @details  to get the pmic misc_psm data
 *           MISC_MISC_PSM is one bit register [0]
 * @param pm_app_misc_cfg_type 
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__INVALID
 *          PM_ERR_FLAG_SUCCESS
 */

pm_err_flag_type
pm_app_misc_get_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 *data);

/**
 * @brief Configures PMIC MISC_MISC_PSM,  register  
 *        
 * @details  to set the pmic misc_psm data
 *           MISC_MISC_PSM is one bit register [0]
 * @param pm_app_misc_cfg_type
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__INVALID
 *          PM_ERR_FLAG_SUCCESS
 */

pm_err_flag_type
pm_app_misc_set_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 mask, uint8 data);


#endif /* __PMAPP_PON_H__ */
