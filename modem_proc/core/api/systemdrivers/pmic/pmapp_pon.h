#ifndef __PMAPP_PON_H__
#define __PMAPP_PON_H__
/*===========================================================================


               P M A P P_ P O N  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations for the PMIC PON application
  
Copyright (c) 2013, 2019, 2023 Qualcomm Technologies, Inc. All rights reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/api/systemdrivers/pmic/pmapp_pon.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/19/19    ks      handling KYPD USB INT on TINY modem
===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"
#include "DDISpmi.h"
#include "pm_pon.h"

/*===========================================================================

                PM APP PON TYPE DEFINITIONS

===========================================================================*/

//Function callback definition to be triggered for both kypd press and release events
typedef void (*pmapp_pon_fn_cb) (boolean Key_press);

//Data to be passed through PON ISR
typedef struct pmapp_pon_isr_data
{
    pmapp_pon_fn_cb cb;
}pmapp_pon_isr_data;

/*===========================================================================

                PM APP PON FUNCTION PROTOTYPES

===========================================================================*/

/*=========================================================================== */
/*                     pmapp_pon_kypd_register_cb                 	      */
/*=========================================================================== */
/**
  This function registers a callback that will be triggered once the CBLPWR_N
  signal(For 9205: KPDPWR Key ) is pressed or released

  PARAMETERS
 * @param[in]  cb: Callback to be triggered-see its declaration for more details 
  @return      pm_err_flag_type - PMIC error flag status 
  @dependency : call this api only after RCINIT1

*/
pm_err_flag_type pmapp_pon_kypd_register_cb(pmapp_pon_fn_cb cb);

/**
 * @brief This function for debug PON reason 
 * 
 * @param[out] debug_data:  
 *                pm_pon_debug_type type structpointer. holds pon reason, AON register and PMIC version details.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pmapp_pon_reason_debug(pm_pon_debug_type *debug_data);

#endif /* __PMAPP_PON_H__ */
