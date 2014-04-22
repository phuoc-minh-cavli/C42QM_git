/*! \file pm_sbl_pon_target.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2018 - 2019 Qualcomm Technologies Inc, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/19   abh     Added file for Tiny Modem image in Twizy
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_app_pon.h"
#include "device_info.h"

/*===========================================================================

                     Global Variables

===========================================================================*/

#define PM_MAX_NUM_PMICS_TARGET 2

//PSHOlD config:  PON PS Hold config
static const pm_pon_reset_cfg_type
pm_app_pon_pshold_reset_cfg_arr[PM_APP_PON_CFG_MAX][PM_MAX_NUM_PMICS_TARGET] =
{  //  PME9205            NONE
   {PON_WARM_RST,      PON_NO_CFG_NEEDED }, //Warm Reset
   {PON_HARD_RST,      PON_NO_CFG_NEEDED }, //Hard Reset
   {PON_SHDN,          PON_NO_CFG_NEEDED }, //Shutdown
   {PON_DVDD_HARD_RST, PON_NO_CFG_NEEDED }, //Dvdd hard reset
   {PON_DVDD_SHDN,     PON_NO_CFG_NEEDED }  //DVDD Shutdown
};

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type 
pm_tgt_get_pshold_reset_cfg( uint32                      pmic_index,
                             pm_app_pon_reset_cfg_type   app_pshold_cfg,
                             pm_pon_reset_cfg_type*      pshold_cfg)
{
  if( (pmic_index         >=  PM_MAX_NUM_PMICS_TARGET)   ||  
      (app_pshold_cfg     >=  PM_APP_PON_CFG_MAX) ||
      (pshold_cfg         ==  NULL) )
  {
    return PM_ERR_FLAG__INVALID;
  }
  
  *pshold_cfg = pm_app_pon_pshold_reset_cfg_arr[app_pshold_cfg][pmic_index];

  return PM_ERR_FLAG__SUCCESS;
}

/* Dummy API, since there is no target specific pshold config needed for single PMIC PME9205 */
pm_err_flag_type
pm_pon_tgt_specific_pshold_cfg(uint32 pmic_index, pm_app_pon_reset_cfg_type app_pshold_cfg)
{
   return PM_ERR_FLAG__SUCCESS;
}

