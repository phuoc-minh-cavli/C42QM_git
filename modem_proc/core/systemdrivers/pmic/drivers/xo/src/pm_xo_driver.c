/*! \file
 *  \n
 *  \brief  pm_xo_driver.c
 *  \details
 *  &copy;  Copyright (c) 2010-2016 Qualcomm Technologies, Inc. All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/xo/src/pm_xo_driver.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/17   kcs     Added CoreVerify.h file to supress unused varaibles warnings (CR-2000426)
12/10/13   rh      File created
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_xo_driver.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
#define PM_XO_HW_MAX_VER_SUPPORTED 0x60 // 1st bit are dig_major, 0 bit dig_minor = 6.0

/* Static global variable to store the xo data */
static pm_xo_core_data_type *pm_xo_core_data_arr[PM_MAX_NUM_PMICS];


static pm_xo_core_register_info_type xocore_reg = 
{
  .base_address       = 0x5000,
  .peripheral_offset  = 0x100,
  .peripheral_type    = 0x04,
  .peripheral_subtype = 0x05,
  .voltage_ctl1       = 0x040,
  .mode_ctl1          = 0x044,
  .en_ctl             = 0x046,
  .xo_timer1          = 0x04C,
  .xo_timer2          = 0x04D,
  .xo_dcc_adj         = 0x056,
  .xo_comp            = 0x058,
  .xo_stepper         = 0x05A,
  .xo_adj             = 0x05C,
  .xo_ctl             = 0x05E,
};

static pm_xo_core_register_info_type xocore_reg_v2 =
{
  .base_address       = 0x5000,
  .peripheral_offset  = 0x100,
  .peripheral_type    = 0x04,
  .peripheral_subtype = 0x05,
  .voltage_ctl1       = 0x040,
  .mode_ctl1          = 0x063,
  .en_ctl             = 0x046,
  .xo_dcc_adj         = 0x056,
  .xo_comp            = 0x058,
  .xo_stepper         = 0x05A,
  .xo_adj             = 0x05B,
  .xo_ctl             = 0x05E,
};


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_xo_core_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_xo_core_data_type *xo_ptr = NULL;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    xo_ptr = pm_xo_core_data_arr[pmic_index];

    if (xo_ptr == NULL)
    {
        pm_malloc( sizeof(pm_xo_core_data_type), (void**)&xo_ptr);

        /* Assign Comm ptr */
        xo_ptr->comm_ptr = comm_ptr;
		
		
        if ((peripheral_info->digital_major_version | peripheral_info->digital_minor_version)  > PM_XO_HW_MAX_VER_SUPPORTED )
        {
            CORE_VERIFY(0);
        }

        if(peripheral_info->digital_major_version >= 0x5)
        {
            xo_ptr->xo_core_register = &xocore_reg_v2;
        }
        else
        {
            xo_ptr->xo_core_register = &xocore_reg;
        }

        CORE_VERIFY_PTR(xo_ptr->xo_core_register);

        xo_ptr->xo_core_common = (pm_xo_core_specific_info_type*)pm_target_information_get_specific_info(PM_PROP_XO_SPECIFIC);

        CORE_VERIFY_PTR(xo_ptr->xo_core_common);

        xo_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_MEGAXO_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(xo_ptr->num_of_peripherals != 0);

        pm_xo_core_data_arr[pmic_index] = xo_ptr;
    }
}

pm_xo_core_data_type* pm_xo_core_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_xo_core_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_xo_core_get_num_peripherals(uint8 pmic_index)
{
  if( (pmic_index < PM_MAX_NUM_PMICS) &&
      (pm_xo_core_data_arr[pmic_index] != NULL))
  {
      return pm_xo_core_data_arr[pmic_index]->num_of_peripherals;
  }

  return NULL;
}

