/*! \file pm_ldo.c
*  
*  \brief Implementation file for LDO resource type.  
*  
*  \details Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation.
*  
*  &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/ldo/src/pm_ldo.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_ldo_driver.h"
#include "pm_resource_manager.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/


pm_err_flag_type pm_ldo_ecm_local_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{

	pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
        pm_register_address_type    reg, ecm_present_status_reg;
	pm_pwr_data_type    *pwr_data;
        pm_register_data_type       data, ecm_present_status_data = 0;

	
	if ((ldo_ptr == NULL) || (on_off == NULL))
	{
		return PM_ERR_FLAG__INVALID_POINTER;
	}
        if (perph_index >= ldo_ptr->pm_pwr_data.num_of_peripherals)
	{
		return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}
       
        pwr_data = &(ldo_ptr->pm_pwr_data);

        if (pwr_data == NULL)
	{
                return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}

       //Read ECM present bit 
       ecm_present_status_reg = pwr_data->pwr_specific_info[perph_index].periph_base_address + pwr_data->pwr_reg_table->STATUS4;
       err_flag = pm_comm_read_byte_mask(ldo_ptr->comm_ptr->slave_id, ecm_present_status_reg, 0x10, &ecm_present_status_data, 0);

       if ((err_flag != PM_ERR_FLAG__SUCCESS) || (ecm_present_status_data != 0x10))
       {
               return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
       }

       reg = pwr_data->pwr_specific_info[perph_index].periph_base_address + pwr_data->pwr_reg_table->ECM_EN_CTL;
       err_flag = pm_comm_read_byte(ldo_ptr->comm_ptr->slave_id, reg, &data, 0);
       if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

       *on_off = (data & 0x80) ? PM_ON : PM_OFF;

       return err_flag;
}
