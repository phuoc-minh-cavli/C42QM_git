/*! \file pm_pbs_client_driver.c
*  \n
*  \brief PBS Client driver initialization.
*  \n  
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/13   kt      Initial version
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "CoreVerify.h"
#include "hw_module_type.h"
#include "pm_malloc.h"
#include "device_info.h"
#include "pm_target_information.h"
#include "pm_version.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the PBS data */
static pm_pbs_client_data_type *pm_pbs_client_data_arr[PM_MAX_NUM_PMICS];


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info,uint8 pmic_index)
{	
    pm_pbs_client_data_type *pbs_client_ptr = NULL;
    
	
    if (peripheral_info->peripheral_type != PM_HW_MODULE_PBS || 
        peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT)
    {
        return;
    }
    
    if ((peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT_1TRIG) &&
        (peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT_4TRIG))
    {
        return;
    }
	
    pbs_client_ptr = pm_pbs_client_data_arr[pmic_index];
	
    if (pbs_client_ptr == NULL)
    {
       pm_malloc( sizeof(pm_pbs_client_data_type), (void**)&pbs_client_ptr);
                                                    
        /* Assign Comm ptr */
        pbs_client_ptr->comm_ptr = comm_ptr;
		
        /* PBS Register Info - Obtaining Data through dal config */
        pbs_client_ptr->pbs_client_reg_table = (pm_pbs_client_register_info_type*)pm_target_information_get_common_info(PM_PROP_PBS_CLIENT_REG);
        
        CORE_VERIFY_PTR(pbs_client_ptr->pbs_client_reg_table);
        
        pbs_client_ptr->num_peripherals = pm_target_information_get_periph_count_info(PM_PROP_PBS_CLIENT_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(pbs_client_ptr->num_peripherals != 0);

        pm_pbs_client_data_arr[pmic_index] = pbs_client_ptr;
    }
    
}

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_pbs_client_data_arr[pmic_index];
    }

    return NULL;
}


uint8 pm_pbs_client_get_num_peripherals(uint8 pmic_index)
{
  if( (pmic_index < PM_MAX_NUM_PMICS) && 
      (pm_pbs_client_data_arr[pmic_index] != NULL))
  {
      return pm_pbs_client_data_arr[pmic_index]->num_peripherals;
  }

  return NULL;
}

