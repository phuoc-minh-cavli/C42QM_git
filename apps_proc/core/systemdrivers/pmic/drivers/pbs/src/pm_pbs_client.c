/*! \file pm_pbs_client.c
*  \n
*  \brief This file contains the implementation of the public APIs for PBS Client module.
*  \n
*  \details Implementation file for PBS Client module. Each of the APIs checks for
*           access and then if necessary directs the call to Driver implementation.
*  \n  
*  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================
EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/04/16   kc	   Fix KW errors.
06/16/13   kt      Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "pm_pbs_client.h"
#include "pm_irq.h"


#define PMIO_PBS_CORE_MEM_INTF_CFG_RMSK 0x80
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_pbs_client_enable(uint8 pmic_chip, uint8 pbs_client_index, boolean enable)
{
    uint8 data = 0;
    uint8 mask = 0x80;
    pm_err_flag_type 		 err_flag        = PM_ERR_FLAG__SUCCESS;
	pm_register_address_type en_ctl1		 = 0x0;
	pm_pbs_client_data_type *pbs_client_ptr  = pm_pbs_client_get_data(pmic_chip);

	if (pbs_client_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {   
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    en_ctl1 = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->en_ctl1;

    if (enable == TRUE) 
    {
        /* Enable PBS Client module */
        data = 0xFF;
    }
    else
    {
        /* Disable PBS Client module */
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, en_ctl1, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_enable_status(uint8 pmic_chip, uint8 pbs_client_index, boolean *status)
{
    uint8 mask = 0x80;
    uint8 data = 0;
    pm_err_flag_type 		 err_flag 		 = PM_ERR_FLAG__SUCCESS;
	pm_register_address_type en_ctl1		 = 0x0;
    pm_pbs_client_data_type *pbs_client_ptr  = pm_pbs_client_get_data(pmic_chip);

	if (pbs_client_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {   
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }
    
	en_ctl1 = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->en_ctl1;

    /* Read PBS Client module enable status */
    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, en_ctl1, mask, &data,0);

    *status = data ? TRUE : FALSE;

    return err_flag;
}

pm_err_flag_type pm_pbs_client_trigger_cfg(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_trigger_type trigger)
{
    uint8 data = 0;
    pm_err_flag_type 				err_flag 		= PM_ERR_FLAG__SUCCESS;
    pm_register_address_type 		TRIG_CFG 		= 0x0;
    pm_pbs_client_data_type 		*pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);


	if (pbs_client_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {   
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }
    
	TRIG_CFG = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->trig_cfg;

    switch (trigger)
    {
    case PM_PBS_CLIENT_TRIGGER_RISING_EDGE:
      data = 0x80;
      break;
    case PM_PBS_CLIENT_TRIGGER_FALLING_EDGE:
      data = 0x40;
      break;
    case PM_PBS_CLIENT_TRIGGER_DUAL_EDGE:
      data = 0xC0;
      break;
    default:
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
	
    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, TRIG_CFG, data, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_enable(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, boolean enable)
{
    uint8 mask = 1<<irq;
    pm_err_flag_type 		 err_flag 		= PM_ERR_FLAG__SUCCESS;
	pm_register_address_type INT_EN_CLR     = 0x0;
	pm_register_address_type INT_EN_SET 	= 0x0;  
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);

	if (pbs_client_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
	if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {   
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }
	
    INT_EN_SET = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_en_set;
    INT_EN_CLR = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_en_clr;
   
    if (enable == TRUE)
    {
        err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, INT_EN_SET, mask, 0);
    }
    else
	{
        err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, INT_EN_CLR, mask, 0);
    }

	return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_clear(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq)
{
    uint8 mask = 1<<irq;
	pm_err_flag_type 		 err_flag 		 = PM_ERR_FLAG__SUCCESS;
	pm_register_address_type INT_LATCHED_CLR = 0x0;
    pm_pbs_client_data_type *pbs_client_ptr  = pm_pbs_client_get_data(pmic_chip);

	if (pbs_client_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {   
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
	}
    
    INT_LATCHED_CLR = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_latched_clr;

    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, INT_LATCHED_CLR, mask, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_set_trigger(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, pm_irq_trigger_type trigger)
{
    uint8 mask          = 1<<irq;
    uint8 set_type      = 0;
    uint8 polarity_high = 0;
    uint8 polarity_low  = 0;
    pm_err_flag_type 		 err_flag          = PM_ERR_FLAG__SUCCESS;
	pm_register_address_type INT_SET_TYPE      = 0x0;
    pm_register_address_type INT_POLARITY_HIGH = 0x0;
	pm_register_address_type INT_POLARITY_LOW  = 0x0;
	pm_pbs_client_data_type  *pbs_client_ptr   = pm_pbs_client_get_data(pmic_chip);
    
	if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }
	
	INT_SET_TYPE      = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_set_type;
    INT_POLARITY_HIGH = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_high;
    INT_POLARITY_LOW  = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_low;
    
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

    err_flag  = pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_SET_TYPE, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, mask, polarity_low, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_get_trigger(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, pm_irq_trigger_type *trigger)
{
    uint8 mask 	        = 1<<irq;
    uint8 set_type      = 0;
    uint8 polarity_high = 0;
    uint8 polarity_low  = 0;
	pm_register_address_type INT_SET_TYPE      = 0x0;
    pm_register_address_type INT_POLARITY_HIGH = 0x0;
	pm_register_address_type INT_POLARITY_LOW  = 0x0;
    pm_err_flag_type         err_flag          = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type  *pbs_client_ptr   = pm_pbs_client_get_data(pmic_chip);

	if (pbs_client_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }
	
    INT_SET_TYPE      = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_set_type;
    INT_POLARITY_HIGH = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_high;
    INT_POLARITY_LOW  = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_low;
   
    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_SET_TYPE, mask, &set_type,0);
    err_flag |= pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, mask, &polarity_high,0);
    err_flag |= pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, mask, &polarity_low,0);

    if ((set_type == 0) && (polarity_high == 0) && (polarity_low == mask))
    {
        *trigger = PM_IRQ_TRIGGER_ACTIVE_LOW;
    }
    else if ((set_type == 0) && (polarity_high == mask) && (polarity_low == 0))
    {
        *trigger = PM_IRQ_TRIGGER_ACTIVE_HIGH;
    }
    else if ((set_type == mask) && (polarity_high == mask) && (polarity_low == 0))
    {
        *trigger = PM_IRQ_TRIGGER_RISING_EDGE;
    }
    else if ((set_type == mask) && (polarity_high == 0) && (polarity_low == mask))
    {
        *trigger = PM_IRQ_TRIGGER_FALLING_EDGE;
    }
    else if ((set_type == mask) && (polarity_high == mask) && (polarity_low == mask))
    {
        *trigger = PM_IRQ_TRIGGER_DUAL_EDGE;
    }
    else
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_status(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, pm_irq_status_type type, boolean *status)
{
    uint8 mask = 1<<irq;
    uint8 data = 0;
    pm_register_address_type INT_STS		= 0x0;
    pm_err_flag_type 		err_flag   	    = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);

	if (pbs_client_ptr == NULL) 
	{
	return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}
	
    if (pbs_client_index >= pbs_client_ptr->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        INT_STS = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_rt_status;
        break;
    case PM_IRQ_STATUS_LATCHED:
        INT_STS = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_latched_status;
        break;
    case PM_IRQ_STATUS_PENDING:
        INT_STS = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_pending_status;
        break;
    case PM_IRQ_STATUS_ENABLE:
        INT_STS = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_en_set;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_STS, mask, &data,0);
    *status = data ? TRUE : FALSE;
    
    return err_flag;
}

pm_err_flag_type pm_pbs_client_sw_trigger(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger trigger, 
                                          uint8 *trigger_argument, uint8 *trigger_return_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 data = 0x80;
    pm_register_address_type base_addr = 0;
    pm_register_address_type trig_ctl = 0;
    pm_register_address_type scratch1 = 0;
    pm_register_address_type scratch2 = 0;

    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((pbs_client_index >= pbs_client_ptr->num_peripherals) || (pbs_client_index >= PM_PBS_CLIENT_INVALID) || 
        (trigger >= PM_PBS_CLIENT_TRIGGER_INVALID))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (trigger == PM_PBS_CLIENT_SW_TRIGGER)
    {
      data = 0x80;
    }
    else
    {
        data = 1<<trigger;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*pbs_client_ptr->pbs_client_reg_table->peripheral_offset);

    if (trigger_argument != NULL)
    {
        scratch1 = base_addr + pbs_client_ptr->pbs_client_reg_table->scratch1;

        // Write to SCRATCH1 register if any argument value to be used with the PBS sequence
        err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, scratch1, *trigger_argument, 0);
    }

    trig_ctl = base_addr + pbs_client_ptr->pbs_client_reg_table->trig_ctl;

    // Trigger the PBS sequence
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, trig_ctl, data, 0);

    if (trigger_return_value != NULL)
    {
        scratch2 = base_addr + pbs_client_ptr->pbs_client_reg_table->scratch2;

        // Read from SCRATCH2 register if any return value expected from the PBS sequence
        err_flag |= pm_comm_read_byte(pbs_client_ptr->comm_ptr->slave_id, scratch2, trigger_return_value, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_pbs_client_hw_trigger_enable(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger hw_trigger, pm_pbs_client_trigger_type trigger_edge, boolean enable)
{
    pm_err_flag_type err_flag                      = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type base_addr             = 0;
    pm_register_address_type en_ctl_addr           = 0;
    pm_register_address_type hw_trig_re_en_addr    = 0;
    pm_register_data_type hw_trig_re_fe_en_mask    = 0;
    pm_register_address_type hw_trig_fe_en_addr    = 0;
    pm_register_address_type trig_latch_clr_addr   = 0;
    pm_register_data_type    hw_trigger_data       = 0; 
    pm_register_data_type    hw_trigger_re_data    = 0; 
    pm_register_data_type    hw_trigger_fe_data    = 0; 
    pm_register_address_type 		TRIG_CFG 		= 0x0;
     
    pm_pbs_client_data_type *pbs_client_ptr        = pm_pbs_client_get_data(pmic_chip);
    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
  
  //Disabling HW trigger for PBS client 0 is not allowed as it used for PON, also failing if SW trigger is given as argument
    if ( (pbs_client_index >= pbs_client_ptr->num_peripherals) || 
         (pbs_client_index == PM_PBS_CLIENT_0)       || 
         (pbs_client_index >= PM_PBS_CLIENT_INVALID) || 
         (hw_trigger >= PM_PBS_CLIENT_SW_TRIGGER)    || 
         (trigger_edge >= PM_PBS_CLIENT_TRIGGER_EDGE_INVALID)) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*pbs_client_ptr->pbs_client_reg_table->peripheral_offset);
    en_ctl_addr         =  base_addr + pbs_client_ptr->pbs_client_reg_table->en_ctl1;
    hw_trig_re_en_addr  =  base_addr + pbs_client_ptr->pbs_client_reg_table->hw_trig_re_en;
    hw_trig_fe_en_addr  =  base_addr + pbs_client_ptr->pbs_client_reg_table->hw_trig_fe_en;
    trig_latch_clr_addr =  base_addr + pbs_client_ptr->pbs_client_reg_table->trig_latched_clr;
    TRIG_CFG = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->trig_cfg;
    hw_trig_re_fe_en_mask = 0x01 << hw_trigger;
    if(enable == TRUE)  //Enable triggering
{
       hw_trigger_data = hw_trig_re_fe_en_mask;
}
    else //Disable triggering
{
       hw_trigger_data = 0x00;
}

    switch (trigger_edge)
    {
    case PM_PBS_CLIENT_TRIGGER_RISING_EDGE:
      hw_trigger_re_data = hw_trigger_data;
      hw_trigger_fe_data = 0x00;
      break;
    case PM_PBS_CLIENT_TRIGGER_FALLING_EDGE:
      hw_trigger_re_data = 0x00;
      hw_trigger_fe_data = hw_trigger_data;
      break;
    case PM_PBS_CLIENT_TRIGGER_DUAL_EDGE:
      hw_trigger_re_data = hw_trigger_data;
      hw_trigger_fe_data = hw_trigger_data;
      break;
    default:
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, en_ctl_addr, 0x00, 0);  
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, hw_trig_re_en_addr, hw_trig_re_fe_en_mask, hw_trigger_re_data, 0);  
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, hw_trig_fe_en_addr, hw_trig_re_fe_en_mask, hw_trigger_fe_data, 0);

    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, trig_latch_clr_addr, 0xFF, 0);
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, en_ctl_addr, 0x80, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, TRIG_CFG, PMIO_PBS_CORE_MEM_INTF_CFG_RMSK, 0x80, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, TRIG_CFG, PMIO_PBS_CORE_MEM_INTF_CFG_RMSK, 0x00, 0);

   return err_flag;
}

