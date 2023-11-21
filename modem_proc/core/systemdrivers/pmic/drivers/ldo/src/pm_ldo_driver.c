/*! \file pm_ldo_driver.c
*  \n
*  \brief
*  \n
*  \details
*  \n &copy; Copyright 2012-2013, 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/ldo/src/pm_ldo_driver.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/17   kcs     Added CoreVerify.h file to supress unused varaibles warnings (CR-2000426)
08/26/13   rk      removing pin configuration functionality.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_ldo_driver.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the LDO driver data */
static pm_ldo_data_type *pm_ldo_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     LOCAL FUNCTION PROTOTYPE

===========================================================================*/

static pm_pwr_volt_info_type* pm_ldo_get_volt_setting_info(peripheral_info_type *peripheral_info , boolean *is_stepper);

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS

===========================================================================*/

void pm_ldo_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_ldo_data_type *ldo_ptr = NULL;
    uint32 ldo_index = 0;
    pm_register_address_type base_address = 0;
    pm_register_address_type periph_offset = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0x0;
    pm_register_data_type data = 0;
    pm_register_data_type mode_data = 0;
	pm_register_data_type max_volt[2] = {0};
    boolean is_stepper = FALSE;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    ldo_ptr = pm_ldo_data_arr[pmic_index];

    if (ldo_ptr == NULL)
    {
        pm_malloc( sizeof(pm_ldo_data_type), (void**)&ldo_ptr);

        /* Assign Comm ptr */
        ldo_ptr->comm_ptr = comm_ptr;

        /* LDO Register Info - Obtaining Data through dal config */
        ldo_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type*)pm_target_information_get_common_info(PM_PROP_LDO_REG);

        CORE_VERIFY_PTR(ldo_ptr->pm_pwr_data.pwr_reg_table);

        ldo_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(ldo_ptr->pm_pwr_data.num_of_peripherals != 0);

        /* LDO pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
        pm_malloc(sizeof(pm_pwr_specific_info_type)*(ldo_ptr->pm_pwr_data.num_of_peripherals), (void**)&(ldo_ptr->pm_pwr_data.pwr_specific_info));

        /* Save first LDO peripheral's base address */
        ldo_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;

         /* LDO Subtype pointer malloc */
        pm_malloc(sizeof(uint8)*(ldo_ptr->pm_pwr_data.num_of_peripherals), (void**)&(ldo_ptr->periph_subtype));
        
        /* LDO Bypass pointer malloc */
        pm_malloc(sizeof(uint8)*(ldo_ptr->pm_pwr_data.num_of_peripherals), (void**)&(ldo_ptr->in_bypass));

        pm_ldo_data_arr[pmic_index] = ldo_ptr;
    }

    if (ldo_ptr != NULL)
    {
        base_address = ldo_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
        /* Peripheral base_address should be >= first peripheral's base addr */
        CORE_VERIFY(peripheral_info->base_address >= base_address);

        periph_offset = ldo_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

        /* Calculate LDO peripheral index */
        ldo_index = ((peripheral_info->base_address - base_address)/periph_offset);

        /* Peripheral Index should be less than max number of peripherals */
        CORE_VERIFY(ldo_index < ldo_ptr->pm_pwr_data.num_of_peripherals);

        /* Save LDO's Peripheral Subtype value */
        ldo_ptr->periph_subtype[ldo_index] = peripheral_info->peripheral_subtype;

        /* Save LDO's Peripheral Type value */
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type = peripheral_info->peripheral_type;

        /* Save each LDO peripheral's base address */
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_base_address = peripheral_info->base_address;
        
         /* Read current LDO's mode  to check LDO is in bypass mode*/
        reg = (pm_register_address_type)(peripheral_info->base_address + ldo_ptr->pm_pwr_data.pwr_reg_table->MODE_CTL);
        err_flag = pm_comm_read_byte(ldo_ptr->comm_ptr->slave_id, reg, &mode_data, 0);
        CORE_VERIFY (err_flag == PM_ERR_FLAG__SUCCESS);

        if (PM_HW_MODULE_ULT_LDO == ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type)
        {
            ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range = 0;
			/* LDO Vset Info - Obtaining Data through dal config */
			ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset = pm_ldo_get_volt_setting_info(peripheral_info , &is_stepper);
        }
		/*PMIC5*/
		else if ((ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type == PM_HW_MODULE_LDO) &&
            (ldo_ptr->periph_subtype[ldo_index] >= PM_HW_MODULE_LDO_HT_N300_STEPPER))
		{
            if (ldo_ptr->periph_subtype[ldo_index] >= PM_HW_MODULE_LDO_500_N300_100)
            {
                ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pmic4_flag = FALSE;
				reg = (pm_register_address_type)(peripheral_info->base_address + ldo_ptr->pm_pwr_data.pwr_reg_table->ULS_VSET_LB);
                if (mode_data == 0x2)
                {
                    ldo_ptr->in_bypass[ldo_index] = TRUE;
                }
            }
			else
			{
			    ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pmic4_flag = TRUE;
				reg = (pm_register_address_type)(peripheral_info->base_address + ldo_ptr->pm_pwr_data.pwr_reg_table->PMIC4_ULS_VSET_LB);
                if (mode_data == 0x3)
                {
                    ldo_ptr->in_bypass[ldo_index] = TRUE;
                }
			}
			
            /* Flag this rail peripheral as unified register map peripheral */
             ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].is_periph_unified_reg = TRUE;
			 
			 err_flag = pm_comm_read_byte_array(ldo_ptr->comm_ptr->slave_id, reg, 2, max_volt, 0);
             
             CORE_VERIFY (err_flag == PM_ERR_FLAG__SUCCESS);
             
            /* LDO Vset Info - 1mV step for HT rails */
			pm_malloc(sizeof(pm_pwr_range_info_type), (void**)&(ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset));
			
            ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset->RangeMin = 0;
            ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset->RangeMax = ((max_volt[1]<<8) | (max_volt[0]))*1000;
            ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset->VStep = 1000;
            
            ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range =0;
            
            pm_ldo_get_volt_setting_info(peripheral_info , &is_stepper);
		}
        else
        {
            /* Read current LDO's range value and save it */
            reg = (pm_register_address_type)(peripheral_info->base_address + ldo_ptr->pm_pwr_data.pwr_reg_table->VOLTAGE_CTRL1);

            err_flag = pm_comm_read_byte(ldo_ptr->comm_ptr->slave_id, reg, &data, 0);
            if (PM_ERR_FLAG__SUCCESS != err_flag)
            {
                ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range = PM_VOLT_INVALID_RANGE;
            }
            else
            {
                ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range = data;
            }
			/* LDO Vset Info - Obtaining Data through dal config */
			ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset = pm_ldo_get_volt_setting_info(peripheral_info , &is_stepper);
        }
        
        if ((mode_data&0x20) == 0x20)
        {
            ldo_ptr->in_bypass[ldo_index] = TRUE;
        }
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].is_periph_stepper = is_stepper;
        CORE_VERIFY_PTR(ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset);
    }
}

pm_pwr_volt_info_type* pm_ldo_get_volt_setting_info(peripheral_info_type *peripheral_info  , boolean *is_stepper)
{
    pm_pwr_volt_info_type *volt_info = NULL;

    switch ((pm_hw_module_type)peripheral_info->peripheral_type)
    {
        case PM_HW_MODULE_LDO:
            switch((pm_hw_module_ldo_subtype)peripheral_info->peripheral_subtype)
            {
                case PM_HW_MODULE_LDO_N50:
                case PM_HW_MODULE_LDO_N150:
                case PM_HW_MODULE_LDO_N300:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_NMOS_VOLT);
                    break;

                case PM_HW_MODULE_LDO_N600:
                     volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_N600_VOLT);
                case PM_HW_MODULE_LDO_N600_STEPPER:
                case PM_HW_MODULE_LDO_N900_STEPPER:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_N600_VOLT);
                    *is_stepper = TRUE;
                    break;

                case PM_HW_MODULE_LDO_N1200:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_N1200_VOLT);
                case PM_HW_MODULE_LDO_N1200_STEPPER:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_N1200_VOLT);
                    *is_stepper = TRUE;
                    break;

                case PM_HW_MODULE_LDO_P50:
                case PM_HW_MODULE_LDO_P150:
                case PM_HW_MODULE_LDO_P300:
                case PM_HW_MODULE_LDO_P450:
                case PM_HW_MODULE_LDO_P600:
                case PM_HW_MODULE_LDO_P1200:
                case PM_HW_MODULE_LDO_LV_P50:
                case PM_HW_MODULE_LDO_LV_P150:
                case PM_HW_MODULE_LDO_LV_P300:
                case PM_HW_MODULE_LDO_LV_P600:
                case PM_HW_MODULE_LDO_LV_P1200:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_PMOS_VOLT);
                    break;

                case PM_HW_MODULE_LDO_USB_LDO:
                case PM_HW_MODULE_LDO_LN_LDO:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_LN_VOLT);
                    break;

                 case PM_HW_MODULE_LDO_500_N300_100:
                case PM_HW_MODULE_LDO_500_N600_100:
                case PM_HW_MODULE_LDO_500_N1200_100:
                case PM_HW_MODULE_LDO_500_N300_160:
                case PM_HW_MODULE_LDO_500_N600_160:
                case PM_HW_MODULE_LDO_500_N1200_160:
                case PM_HW_MODULE_LDO_510_N300_100:
                case PM_HW_MODULE_LDO_510_N600_100:
                case PM_HW_MODULE_LDO_510_N1200_100:
                case PM_HW_MODULE_LDO_510_N300_160:
                case PM_HW_MODULE_LDO_510_N600_160:
                case PM_HW_MODULE_LDO_510_N1200_160:
                  *is_stepper = TRUE;
                  break;

                default:
                    volt_info = NULL;
                    break;
            }
            break;

        case PM_HW_MODULE_ULT_LDO:
            switch((pm_hw_module_ldo_subtype)peripheral_info->peripheral_subtype)
            {
                case PM_HW_MODULE_LDO_N300_STEPPER:
                case PM_HW_MODULE_LDO_N600_STEPPER:
                case PM_HW_MODULE_LDO_N1200_STEPPER:
                case PM_HW_MODULE_LDO_N900_STEPPER:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_NMOS_VOLT);
                    *is_stepper = TRUE;
                    break;

                case PM_HW_MODULE_LDO_P50:
                case PM_HW_MODULE_LDO_P150:
                case PM_HW_MODULE_LDO_P300:
                case PM_HW_MODULE_LDO_P450:
                case PM_HW_MODULE_LDO_P600:
                case PM_HW_MODULE_LDO_P1200:
                case PM_HW_MODULE_LDO_LV_P50:
                case PM_HW_MODULE_LDO_LV_P150:
                case PM_HW_MODULE_LDO_LV_P300:
                    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_PMOS_VOLT);
                    break;
                    
                default:
                    /* we should not reach this point */
                    volt_info = NULL;
                    break;
            }
            break;

        default:
            volt_info = NULL;
    }

    return volt_info;
}

pm_ldo_data_type* pm_ldo_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_ldo_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_ldo_get_num_peripherals(uint8 pmic_index)
{
    if( (pmic_index < PM_MAX_NUM_PMICS) &&
        (NULL != pm_ldo_data_arr[pmic_index]) )
    {
        return pm_ldo_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
    }

    return NULL;
}
