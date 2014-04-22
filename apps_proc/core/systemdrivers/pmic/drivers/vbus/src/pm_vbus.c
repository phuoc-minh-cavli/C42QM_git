/*! \file
*  
*  \brief  pm_vbus.cpp ----File contains the implementation of the public APIs for vbus resource type.
*  \details Implementation file for vbus resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*  
*    PMIC code generation Version: 2.0.0.19
*    This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2010-2014 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/vbus/src/pm_vbus.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/15   vk      Added new vbus driver support

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_vbus_driver.h"
#include "pm_comm.h"
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pm_vbus_irq_enable(uint8 pmic_chip, uint8 vbus_index, boolean enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_vbus_data_type *vbus_ptr = pm_vbus_get_data(pmic_chip);
    pm_register_address_type IRQ_REG;

    if (vbus_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (vbus_index >= vbus_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    if (enable)
    {
        IRQ_REG = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_en_set;
    }
    else
    {
        IRQ_REG = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_en_clr;
    }
    
    errFlag = pm_comm_write_byte(vbus_ptr->comm_ptr->slave_id, IRQ_REG, 0x01, 0);
    return errFlag;
}

pm_err_flag_type pm_vbus_irq_clear(uint8 pmic_chip, uint8 vbus_index)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_vbus_data_type *vbus_ptr = pm_vbus_get_data(pmic_chip);
    pm_register_address_type INT_LATCHED_CLR;
    
    if (vbus_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (vbus_index >= vbus_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    
    INT_LATCHED_CLR = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_latched_clr;
    
    errFlag = pm_comm_write_byte(vbus_ptr->comm_ptr->slave_id, INT_LATCHED_CLR, 0x01, 0);
    return errFlag;
}


pm_err_flag_type pm_vbus_irq_set_trigger(uint8 pmic_chip, uint8 vbus_index, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_vbus_data_type *vbus_ptr = pm_vbus_get_data(pmic_chip);
    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_register_address_type INT_SET_TYPE;
    pm_register_address_type INT_POLARITY_HIGH;
    pm_register_address_type INT_POLARITY_LOW;
    
    if (vbus_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (vbus_index >= vbus_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    INT_SET_TYPE = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_set_type;
    INT_POLARITY_HIGH = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_polarity_high;
    INT_POLARITY_LOW = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_polarity_low;
       
    switch (trigger)
    {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
        set_type = 0x00;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
        set_type = 0x00;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_RISING_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_FALLING_EDGE:
        set_type = 0xFF;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_DUAL_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0xFF;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    errFlag  = pm_comm_write_byte_mask(vbus_ptr->comm_ptr->slave_id, INT_SET_TYPE, 0x01 , set_type, 0);
    errFlag |= pm_comm_write_byte_mask(vbus_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, 0x01, polarity_high, 0);
    errFlag |= pm_comm_write_byte_mask(vbus_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, 0x01, polarity_low, 0);

    return errFlag;
}

pm_err_flag_type pm_vbus_irq_status(uint8 pmic_chip, uint8 vbus_index, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_vbus_data_type *vbus_ptr = pm_vbus_get_data(pmic_chip);
    pm_register_data_type data;
    pm_register_address_type INT_STS;
    
    if (vbus_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (vbus_index >= vbus_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        INT_STS =vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_rt_sts;
        break;
    case PM_IRQ_STATUS_LATCHED:
        INT_STS = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_latched_sts;
        break;
    case PM_IRQ_STATUS_PENDING:
        INT_STS = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_pending_sts;
        break;
    case PM_IRQ_STATUS_ENABLE:
        INT_STS = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) + vbus_ptr->vbus_register->int_en_set;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    errFlag = pm_comm_read_byte_mask(vbus_ptr->comm_ptr->slave_id, INT_STS, 0x01, &data,0);
    *status = data ? TRUE : FALSE;

    return errFlag;

}


pm_err_flag_type pm_vbus_get_pin_status(uint8 pmic_chip, uint8 vbus_index, pm_pin_status *pin_status)
{
    pm_err_flag_type         errFlag = PM_ERR_FLAG__SUCCESS;
    pm_vbus_data_type *vbus_ptr = pm_vbus_get_data(pmic_chip);
    pm_register_data_type    data;
    pm_register_address_type STATUS;
    
    if (vbus_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }    
    if (vbus_index >= vbus_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    
    STATUS = vbus_ptr->vbus_register->base_address + (pm_register_address_type)(vbus_ptr->vbus_register->peripheral_offset*vbus_index) +
              vbus_ptr->vbus_register->status;
			  
    errFlag = pm_comm_read_byte_mask(vbus_ptr->comm_ptr->slave_id, STATUS, 0xFF, &data,0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    *pin_status = (data & 0x80) ? PM_PIN_STATUS_HIGH : PM_PIN_STATUS_LOW;
    
    return errFlag;
}



