/*! \file
*  \n
*  \brief  pm_vbus_driver.c
*  \details  
*  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/vbus/src/pm_vbus_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/15   vk      Added new vbus driver support
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_vbus_driver.h"
#include "CoreVerify.h"
#include "pm_target_information.h"
#include "pm_version.h"
#include "device_info.h"
#include "pm_malloc.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the vbus data */
static pm_vbus_data_type *pm_vbus_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_vbus_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
   pm_vbus_data_type *vbus_ptr = NULL;
	
    vbus_ptr = pm_vbus_data_arr[pmic_index];
	
    if (vbus_ptr == NULL)
    {
        pm_malloc( sizeof(pm_vbus_data_type), (void**)&vbus_ptr);
                                                    
        /* Assign Comm ptr */
        vbus_ptr->comm_ptr = comm_ptr;
		
        /* LRegister Info - Obtaining Data through dal config */
        vbus_ptr->vbus_register = (pm_vbus_register_info_type*)pm_target_information_get_common_info(PM_PROP_VBUS_REG);
		
        CORE_VERIFY_PTR(vbus_ptr->vbus_register);
		
        vbus_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_VBUS_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(vbus_ptr->num_of_peripherals != 0);
		
         pm_vbus_data_arr[pmic_index] = vbus_ptr;
    }
}

pm_vbus_data_type* pm_vbus_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_vbus_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_vbus_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_vbus_data_arr[pmic_index] != NULL))
  {
      return pm_vbus_data_arr[pmic_index]->num_of_peripherals;
    }

    return NULL;
}
