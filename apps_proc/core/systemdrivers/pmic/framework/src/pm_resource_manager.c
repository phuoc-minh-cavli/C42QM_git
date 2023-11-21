/*! \file pm_resource_manager.c
 *  
 *  \brief  This file contains the initialization functions for PMIC resource manager.
 *  
 *  &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/framework/src/pm_resource_manager.c#2 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/17/17    kcs     MPP Driver Support on Threadx. (CR-2008263)
09/01/11   hs      Fixed the initialization of OTG500 and HDMI55.
08/18/11   jtn     Added HFS and FTS support
05/18/11   hs      Added MEGA XO support.
03/30/11   hs      Added SMBC support.
03/30/11   hs      Added INT support.
03/23/11   hs      Added off-target testing support.
Removed BTM and LV_AMUX initializations since they should
not be supported in the modem proc.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_target_information.h"

#include "pm_comm.h"
#include "DALSys.h"

/* PMIC Driver includes */
#include "pm_gpio_driver.h"
#include "pm_clk_driver.h"
#include "pm_pbs_client_driver.h"
#include "pm_vbus_driver.h"
#include "pm_rtc_driver.h"
#include "pm_smps_driver.h"
#include "pm_ldo_driver.h"
#include "hw_module_type.h"
#include "device_info.h"
#include "pm_mpp_driver.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*! \brief This function returns the parameter specified pmic resource.
 *
 *  \param[in] comm_ptr The comm object of the pmic resource.
 *
 *  \param[in] peripheral_info The peripheral info of the pmic resource.
 */
static void pm_resource_manager_create_pmic_resource
       (pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

void pm_resource_manager_init(void)
{
    peripheral_info_type peripheral_info;

    uint32 pmic_index = 0;
    uint32 sid_index = 0;
    uint32 peripheral_index = 0;
    
    pm_comm_info_type *comm_ptr = NULL;



    uint8  num_slaves_per_pmic = pm_get_num_slaves_per_pmic();

    memset(&peripheral_info, 0 , sizeof(peripheral_info_type));

    /*  Go through all valid Comm buses and read peripheral info needed for driver initializations */
    for(; pmic_index < PM_MAX_NUM_PMICS ; pmic_index++)
    {
        if(pm_get_pmic_model(pmic_index ) != PMIC_IS_UNKNOWN )
        {
         //every  pmic chip has one or more slaves
        for(sid_index = 0; sid_index < num_slaves_per_pmic ; sid_index++)
            {
            comm_ptr = pm_comm_get_comm_info( pmic_index, sid_index );


                // Go through each of the 256 base addresses and get peripheral info
                for(peripheral_index = 0; peripheral_index < 256; peripheral_index++)
                {
                    peripheral_info.base_address = peripheral_index * 0x0100;
                    //read revision information here
                    if(pm_target_information_read_peripheral_rev(comm_ptr, &peripheral_info) == PM_ERR_FLAG__SUCCESS)
                    {
                        pm_resource_manager_create_pmic_resource(comm_ptr, &peripheral_info, pmic_index);
                    }
                }
                        
            }
        }
    }

}

void pm_resource_manager_create_pmic_resource(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{   
    switch((pm_hw_module_type)peripheral_info->peripheral_type)
    { 
        case PM_HW_MODULE_FTS:
        case PM_HW_MODULE_HF_BUCK:
            pm_smps_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_LDO:
            pm_ldo_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;       
        case PM_HW_MODULE_CLOCK:
            pm_clk_driver_init( comm_ptr, peripheral_info, pmic_index );
            break;
        case PM_HW_MODULE_GPIO:
            pm_gpio_driver_init( comm_ptr, peripheral_info, pmic_index );             
            break;
		case PM_HW_MODULE_MPP:
            pm_mpp_driver_init( comm_ptr, peripheral_info, pmic_index );
            break;			
        case PM_HW_MODULE_PBS:
            pm_pbs_client_driver_init( comm_ptr, peripheral_info, pmic_index );
            break; 
        case PM_HW_MODULE_RTC:
            pm_rtc_driver_init( comm_ptr, peripheral_info, pmic_index );                   
            break;
        case PM_HW_MODULE_MISC:
            switch (peripheral_info->peripheral_subtype)
            {
                case PM_HW_MODULE_MISC_VBUS_DET:
                    pm_vbus_driver_init( comm_ptr, peripheral_info, pmic_index );
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}
