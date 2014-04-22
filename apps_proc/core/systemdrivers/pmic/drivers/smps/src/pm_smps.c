/*! \file pm_smps.c
*  
*  \brief Implementation file for SMPS resource type.  
*  
*  \details Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation.
*  
*  &copy; Copyright 2010-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/smps/src/pm_smps.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
04/09/12   hs      Changed the type for internalResourceIndex from int to uint8.
04/04/12   hs      Removed the buck switch size alg.
03/30/12   hs      Removed the dirver size alg.
04/10/11   wra     Removed the ifdef RPM declaration. RPM has it's own routing layer now
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_smps.h"
#include "pm_smps_driver.h"
#include "pm_comm.h"
#include "pm_dal_prop_ids.h"
#include "pm_target_information.h"
/*===========================================================================

                     FUNCTION DECLARATIONS 

===========================================================================*/

/* Internal helper function */
static pm_err_flag_type pm_smps_util_get_ilim_level(uint16 ilim_level, SmpsILimDS *ilimDS, pm_smps_perph_subtype_type smps_perph_subtype, pm_register_data_type *ilim_data);

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_smps_ecm_local_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
	pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type    reg, ecm_present_status_reg;
	pm_pwr_data_type    *pwr_data;
    pm_register_data_type       data, ecm_present_status_data = 0;

	if ((smps_ptr == NULL) || (on_off == NULL))
	{
		return PM_ERR_FLAG__INVALID_POINTER;
	}
        if (perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
	{
		return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}

        pwr_data = &(smps_ptr->pm_pwr_data);

	if (pwr_data == NULL)
	{
		return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}
        if (pwr_data->pwr_specific_info[perph_index].periph_type != PM_HW_MODULE_HF_BUCK)
	{
		return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}

        //Read ECM present bit
        ecm_present_status_reg = pwr_data->pwr_specific_info[perph_index].periph_base_address + 0x100 + pwr_data->pwr_reg_table->STATUS4;
        err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, ecm_present_status_reg, 0x10, &ecm_present_status_data, 0);

        if ((err_flag != PM_ERR_FLAG__SUCCESS) || (ecm_present_status_data != 0x10))
        {
            return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	    }

        reg = pwr_data->pwr_specific_info[perph_index].periph_base_address + 0x100 + pwr_data->pwr_reg_table->ECM_EN_CTL;
        err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, reg, &data, 0);
        if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

        *on_off = (data & 0x80) ? PM_ON : PM_OFF;

        return err_flag;
}

pm_err_flag_type pm_smps_sw_mode_status(uint8 pmic_chip, uint8 perph_index, pm_sw_mode_type* sw_mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, sw_mode);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_pin_ctrled_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    uint8 select_pin = 0;

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, on_off, &select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_pin_ctrl_status(uint8 pmic_chip, uint8 perph_index, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    pm_on_off_type on_off = PM_INVALID;

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, &on_off, select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level_status(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_sw_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_vreg_ok_status(uint8 pmic_chip, uint8 perph_index, boolean *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_is_vreg_ready_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_clk_source(uint8 pmic_chip, pm_clk_src_type  clk)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
	
    if ( clk >= PM_CLK_SOURCE_INVALID )
    {
      err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE ;
    }
    else
    {
        switch ( clk ) 
        {
        case PM_CLK_TCXO: 
          //TODO temp commented out until the clk buff interface is done
          //errFlag = clk->ClkSelectRCorXO(true, internalResourceIndex); // This function sets the Buck Clock Source.
          break;
        case PM_CLK_RC:
          //TODO temp commented out until the clk buff interface is done
          //errFlag = clk->ClkSelectRCorXO(false, internalResourceIndex);
          break;
        default:
          err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        }        
    }
    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq(uint8 pmic_chip, uint8 perph_index, pm_smps_switching_freq_type freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if ( freq >= PM_SWITCHING_FREQ_INVALID )
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type    reg = 0;
        pm_register_address_type    smps_ctrl_base_addr = 0;
        pm_register_address_type    smps_freq_base_addr = 0;
        pm_register_address_type    smps_freq_periph_offset = 0x200;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        boolean is_pmic4 = pwr_data->pwr_specific_info[perph_index].pmic4_flag;  

        /* SMPSx_CTRL peripheral base address */
        smps_ctrl_base_addr = pwr_data->pwr_specific_info[perph_index].periph_base_address;

        /* SMPSx_FREQ peripheral base address */
        smps_freq_base_addr =  smps_ctrl_base_addr + smps_freq_periph_offset;

        /* Implementation is different on FTS and HF bucks */
        if (pwr_data->pwr_specific_info[perph_index].periph_type == PM_HW_MODULE_HF_BUCK)
        {
            /* We need to set the freq in SMPSx_FREQ peripheral for HF Bucks */

            /*PMIC5 does not have SEC_ACCESS*/
            if(is_pmic4)
            {
            /* CLK_DIV needs protected access. Write to SEC_ACCESS register */
            reg =  smps_freq_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;

            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if(err_flag != PM_ERR_FLAG__SUCCESS)
                return err_flag;
            }
            
                /* Write to CLK_DIV register */
                reg =  smps_freq_base_addr + pwr_data->pwr_reg_table->CLK_DIV;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0); // <0:4>
            
        }
        else if (pwr_data->pwr_specific_info[perph_index].periph_type == PM_HW_MODULE_FTS)
        {
            /* We need to set the freq in SMPSx_CTRL peripheral for FTS Bucks */

            /* CLK_DIV needs protected access. Write to SEC_ACCESS register */
            reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;

            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                /* Write to CLK_DIV register */
                reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->CLK_DIV;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0); // <0:4>
            }
        }
        else
        {
            err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
    }

     return err_flag;
}

pm_err_flag_type pm_smps_inductor_ilim_alg(uint8 pmic_chip, uint8 perph_index, uint16 ilim_level,pm_smps_ilim_mode_type smps_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
	
    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if ( perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals )
    {
        err_flag = PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE ;
    }
    else if (smps_mode >= PM_ILIM_SMPS_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE ;
    }
    else
    {
		
        pm_register_data_type  ilim_data = 0x00;
        pm_register_data_type  ilim_data_mask = 0xFF;
        pm_register_data_type  perph_subtype_data_read = 0x00;
        pm_pwr_data_type      *pwr_data = &(smps_ptr->pm_pwr_data);

        pm_register_address_type smps_base_reg_address  = (pm_register_address_type)pwr_data->pwr_reg_table->base_address + 0x0100 +  (perph_index* pwr_data->pwr_reg_table->peripheral_offset);
        pm_register_address_type smps_perph_subtype_reg = smps_base_reg_address + pwr_data->pwr_reg_table->PERPH_SUBTYPE;
        pm_register_address_type smps_ilim_reg          = smps_base_reg_address + pwr_data->pwr_reg_table->PWM_CURRENT_LIM_CTL;

        SmpsILimDS* ilimDS = (SmpsILimDS*)pm_target_information_get_common_info(PM_PROP_SMPS_ILIMIT_LUT);
        
        if(smps_ptr == NULL || ilimDS == NULL)
        {
            err_flag = PM_ERR_FLAG__INVALID_POINTER;
        }
        else
        {
            //Read SMPS PERPH_SUBTYPE
            err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, smps_perph_subtype_reg,
                                               0xFF, &perph_subtype_data_read,0 ); 

            if ( (pm_smps_perph_subtype_type)perph_subtype_data_read == SMPS_PERPH_SUBTYPE_2X)
            {
                err_flag = pm_smps_util_get_ilim_level(ilim_level, ilimDS, SMPS_PERPH_SUBTYPE_2X, &ilim_data);
            }
            else if ( (pm_smps_perph_subtype_type)perph_subtype_data_read == SMPS_PERPH_SUBTYPE_3X)
            {
                err_flag = pm_smps_util_get_ilim_level(ilim_level, ilimDS, SMPS_PERPH_SUBTYPE_3X, &ilim_data);
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }
           
                   
            //Check for for smps operating mode
            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                if(smps_mode == PM_ILIM_SMPS_PWM_MODE)
                {
                     ilim_data_mask = 0x07;
                }
                else if(smps_mode == PM_ILIM_SMPS_AUTO_MODE )
                {
                    ilim_data = ilim_data <<3;
                    ilim_data_mask = 0x38;
                }

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, smps_ilim_reg, ilim_data_mask, ilim_data, 0 ) ;
             }
        }
    }
    return err_flag;
}

pm_err_flag_type pm_smps_inductor_ilim_status( uint8 pmic_chip, uint8 perph_index, uint16* ilim_level, pm_smps_ilim_mode_type smps_mode )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
	
    if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if ( perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals )
    {
     err_flag = PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE ;
    }
    else if (smps_mode >= PM_ILIM_SMPS_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE ;
    }
    else
    {
		
        pm_register_data_type ilim_data_mask = 0xFF;
        pm_register_data_type      data_read = 0x00;
        pm_register_data_type      perph_subtype_data_read = 0x00;
        pm_pwr_data_type      *pwr_data = &(smps_ptr->pm_pwr_data);

        pm_register_address_type smps_base_reg_address  = (pm_register_address_type)pwr_data->pwr_reg_table[perph_index].base_address + 0x0100 +  (perph_index* pwr_data->pwr_reg_table->peripheral_offset);
        pm_register_address_type smps_perph_subtype_reg = smps_base_reg_address + pwr_data->pwr_reg_table->PERPH_SUBTYPE;
        pm_register_address_type smps_ilim_reg          = smps_base_reg_address + pwr_data->pwr_reg_table->PWM_CURRENT_LIM_CTL;

        //Get handle for smps ilimit configuration data (lookup table)
        SmpsILimDS* ilimDS = (SmpsILimDS*)pm_target_information_get_specific_info(PM_PROP_SMPS_ILIMIT_LUT);
        
        if(ilimDS == NULL)
        {
            err_flag = PM_ERR_FLAG__INVALID_POINTER;
        }
        else
        {
            //Read SMPS PERPH_SUBTYPE
            err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, 
                               smps_perph_subtype_reg, 0xFF, &perph_subtype_data_read ,0); 

            if(smps_mode == PM_ILIM_SMPS_PWM_MODE)
            {
                ilim_data_mask = 0x07; 
            }
            else if(smps_mode == PM_ILIM_SMPS_AUTO_MODE )
            {
                ilim_data_mask = 0x38;
            }
            err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, smps_ilim_reg, ilim_data_mask, &data_read,0 );

            if(smps_mode == PM_ILIM_SMPS_AUTO_MODE )
            {
                data_read = data_read >>3;
            }
          
            //Get actual ilim data from DS
            if (perph_subtype_data_read == 0x02)  //2x smps sub type
            {
                *ilim_level = ilimDS[(uint8)data_read].smps_ilimit_2x_lut;

            }
            else if (perph_subtype_data_read == 0x03)  //3x smps sub type)
            {
                *ilim_level = ilimDS[(uint8)data_read].smps_ilimit_3x_lut;
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }
        }
    }
    return err_flag;
}

pm_err_flag_type pm_smps_global_quiet_mode_enable(uint8 pmic_chip, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg_comm = 0xFF;
        pm_register_data_type    data = 0;
        pm_pwr_data_type         *pwr_data = &(smps_ptr->pm_pwr_data);

        reg_comm = 0x1000 + pwr_data->pwr_reg_table->QM_MODE;        
        
        if (PM_ON == on_off)
        {
            data = 0xFF;
         }
         else
         {
            data = 0x00;
         }

       err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg_comm, 0x80, data, 0);

    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_quiet_mode_mask(uint8 pmic_chip, uint8 periph_index, pm_quiet_mode_type mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(periph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (mode >= PM_QUIET_MODE__INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type    base_addr = 0;
        pm_register_address_type    qm_reg = 0;
        pm_register_address_type    sec_reg = 0;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        pm_register_data_type       data = 0;
        pm_register_mask_type       mask = 0x20;
        boolean is_pmic4 = pwr_data->pwr_specific_info[periph_index].pmic4_flag;  

        /* SMPSx_CTRL peripheral base address */
        base_addr = pwr_data->pwr_specific_info[periph_index].periph_base_address;

        sec_reg =  base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;
        
         if(is_pmic4)
        {
            qm_reg = (pm_register_address_type)(base_addr + pwr_data->pwr_reg_table->QMODE_SHADOW);
        }
        else
        {
            qm_reg = (pm_register_address_type)(base_addr + pwr_data->pwr_reg_table->QM_PS_EXIT);
        }

        switch(mode)
        {
        case PM_QUIET_MODE__DISABLE:
            {
                //QM_SHADOW: FOLLOW_QM bit<5> = 0
                mask = 0x20;
                data = 0x0; 
            }
            break;
        case PM_QUIET_MODE__QUIET:
            {
                //QM_SHADOW: FOLLOW_QM bit<5> = 1 
                mask = 0x20;
                data = 0x20; 
            }
            break;
        case PM_QUIET_MODE__SUPER_QUIET:
            {
                //QM_SHADOW: FOLLOW_QM bit<5> = 1
                mask = 0x20;
                data = 0x20; 
            }
            break;
        default:
            err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
            break;
        }

        if (err_flag == PM_ERR_FLAG__SUCCESS)
        {
            /* QM reg needs protected access. Write to SEC_ACCESS register */
            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, sec_reg, 0xA5, 0);
    
            /* Write to QM register */
            err_flag |= pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, qm_reg, mask, data, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type pm_smps_volt_calculate_vset(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type  volt_level, uint32* vset)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_calculate_vset_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level, vset);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level_stepper_done_status(uint8 pmic_chip, uint8 perph_index, boolean *stepper_done)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_stepper_done_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, stepper_done);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_volt_level_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level);
    }
    
    return err_flag;
}

pm_err_flag_type pm_smps_util_get_ilim_level(uint16 ilim_level, SmpsILimDS *ilimDS, pm_smps_perph_subtype_type smps_perph_subtype, pm_register_data_type *ilim_data)
{
    uint8 i;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    //ToDo:  This Alg can be optimized more but requires data structure change on pm_config_register.c
    if ((pm_smps_perph_subtype_type)smps_perph_subtype == SMPS_PERPH_SUBTYPE_2X)  //2x smps sub type
    {
        //check if I limit requested is within limit
        if (ilim_level > ilimDS[0].smps_ilimit_2x_lut)
        {
            *ilim_data = ilimDS[0].smps_ilimit_reg_data_lut; //set the Max limit
        }
        else if(ilim_level < ilimDS[7].smps_ilimit_2x_lut)
        {
            *ilim_data = ilimDS[7].smps_ilimit_reg_data_lut; //set the Min limit
        }
        else
        {
            for (i = 0; i < 8; i++)  //search for closest upper limit
        {
              if ( (ilim_level <= ilimDS[i].smps_ilimit_2x_lut) && (ilim_level > ilimDS[i+1].smps_ilimit_2x_lut) )
            {
                *ilim_data = ilimDS[i].smps_ilimit_reg_data_lut;
             break;
              }
            }
        }
    }
    else if ((pm_smps_perph_subtype_type)smps_perph_subtype == SMPS_PERPH_SUBTYPE_3X ) //3X smps sub type
    {
        if (ilim_level > ilimDS[0].smps_ilimit_3x_lut)
        {
            *ilim_data = ilimDS[0].smps_ilimit_reg_data_lut;
        }
        else if(ilim_level < ilimDS[7].smps_ilimit_3x_lut)
        {
            *ilim_data = ilimDS[7].smps_ilimit_reg_data_lut;
        }
        else
        {
            for (i = 0; i < 8; i++)
            {
              if ( (ilim_level <= ilimDS[i].smps_ilimit_3x_lut) && (ilim_level > ilimDS[i+1].smps_ilimit_3x_lut) )
              {
                *ilim_data = ilimDS[i].smps_ilimit_reg_data_lut;
             break;
            }    
        }
    }
    }
    else
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE ;;
    }
    return err_flag;
}
