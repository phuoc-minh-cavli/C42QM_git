/*! \file
 *  \n
 *  \brief  pm_bcl_comp_driver.c
 *  \details
 *  \n &copy; Copyright 2019 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/bcl/src/pm_bcl_comp_driver.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
02/23/16    rl      Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_bcl_comp_driver.h"
#include "CoreVerify.h"
#include "pm_target_information.h"
//#include "pm_version_p.h"
#include "device_info.h"
#include "pm_malloc.h"
#include "hw_module_type.h"
#include "pm_ulog.h"
/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the BCL data */
static pm_bcl_comp_data_type *pm_bcl_comp_data_arr[PM_MAX_NUM_PMICS];

static pm_bcl_comp_register_info_type bcl_comp_reg =
{
  .base_address     = 0x3F00,
  .status           = 0x08,
  .int_rt_sts       = 0x10,
  .int_set_type     = 0x11,
  .int_pol_high     = 0x12,
  .int_pol_low      = 0x13,
  .int_latched_clr  = 0x14,
  .int_en_set       = 0x15,
  .int_en_clr       = 0x16,
  .int_latched_sts  = 0x18,
  .mode_ctl         = 0x40,
  .mode_ctl1        = 0x00, /* Mode Ctl1 register not present */
  .en_ctl           = 0x46,
  .vcomp_low2_thr   = 0x49,
  .vcomp_low1_thr   = 0x4A,
};

static pm_bcl_comp_register_info_type bcl_comp_reg_v2 =
{
  .base_address     = 0x3F00,
  .status           = 0x08,
  .int_rt_sts       = 0x10,
  .int_set_type     = 0x11,
  .int_pol_high     = 0x12,
  .int_pol_low      = 0x13,
  .int_latched_clr  = 0x14,
  .int_en_set       = 0x15,
  .int_en_clr       = 0x16,
  .int_latched_sts  = 0x18,
  .mode_ctl         = 0x40,
  .mode_ctl1        = 0x41,
  .en_ctl           = 0x46,
  .vcomp_low2_thr   = 0x4A,
  .vcomp_low1_thr   = 0x49,
};

pm_bcl_comp_limits bcl_comp_limits =
{ /* min_mV, max_mV, step_mV */
  2250,    3600,   25
};

pm_bcl_comp_limits bcl_comp_limits_v2 =
{ /* min_mV, max_mV, step_mV */
  2250,    3600,   25
};

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void
pm_bcl_comp_driver_init(pm_comm_info_type *comm_ptr,
                        peripheral_info_type *peripheral_info,
                        uint8 pmic_index)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = NULL;

  bcl_comp_ptr = pm_bcl_comp_data_arr[pmic_index];


  if (bcl_comp_ptr == NULL)
  {
    pm_malloc( sizeof(pm_bcl_comp_data_type), (void**)&bcl_comp_ptr);

    bcl_comp_ptr->comm_ptr = comm_ptr;

    if(peripheral_info->analog_major_version == 0x1)
    {
      bcl_comp_ptr->bcl_comp_register = &bcl_comp_reg_v2;
      bcl_comp_ptr->limits = &bcl_comp_limits_v2;
      bcl_comp_ptr->version_update    = TRUE;
    }
    else
    {
      bcl_comp_ptr->bcl_comp_register = &bcl_comp_reg;
      bcl_comp_ptr->limits = &bcl_comp_limits;
      bcl_comp_ptr->version_update    = FALSE;
    }

    if(bcl_comp_ptr->bcl_comp_register == NULL)
    {
      PM_LOG_MSG_ERR_FATAL("Failed to retrieve register addresses");
    }

    if(bcl_comp_ptr->limits == NULL)
    {
      PM_LOG_MSG_ERR_FATAL("Failed to retrieve limits information");
    }

    pm_bcl_comp_data_arr[pmic_index] = bcl_comp_ptr;
  }
  return;
}

pm_bcl_comp_data_type*
pm_bcl_comp_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
    return pm_bcl_comp_data_arr[pmic_index];
  }

  return NULL;
}
