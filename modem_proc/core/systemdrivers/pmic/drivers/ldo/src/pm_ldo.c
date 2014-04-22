/*! \file pm_ldo.c
*  
*  \brief Implementation file for LDO resource type.  
*  
*  \details Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation.
*  
*  &copy; Copyright 2012-2015, 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/ldo/src/pm_ldo.c#3 $

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


static pm_err_flag_type pm_ldo_sw_mode_exit_bypass(pm_comm_info_type *comm_ptr, pm_register_address_type reg);

/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_ldo_sw_mode(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    uint8 periph_type = 0;

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(ldo_peripheral_index >= ldo_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (mode >= PM_SW_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type          reg = 0;
        pm_pwr_data_type                  *pwr_data = &(ldo_ptr->pm_pwr_data);

        periph_type = pwr_data->pwr_specific_info[ldo_peripheral_index].periph_type;
       
       boolean is_periph_unified_reg = pwr_data->pwr_specific_info[ldo_peripheral_index].is_periph_unified_reg;
       boolean is_pmic4 = pwr_data->pwr_specific_info[ldo_peripheral_index].pmic4_flag;
         
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[ldo_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: /* Low power mode */
          {
              /* the LDO is already in Bypass mode and will be changed to LPM */
              if (ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                if (is_periph_unified_reg == TRUE)   //YODA
                {
                    /* the LDO is already in Bypass mode and will be changed to NPM
                       before programming LPM mode */
                    err_flag = pm_ldo_sw_mode(pmic_chip, ldo_peripheral_index, PM_SW_MODE_NPM);

                    /* wait 10uS for loop to settle */
                    DALSYS_BusyWait(10);
                }
                else     //PMD9x07
                {
                    /*exit by-pass properly*/
                    err_flag = pm_ldo_sw_mode_exit_bypass(ldo_ptr->comm_ptr, reg);
                }
                  if (err_flag == PM_ERR_FLAG__SUCCESS)
                  {
                      /* update the bypass LUT */
                      ldo_ptr->in_bypass[ldo_peripheral_index] = 0;
                  }
              }

             if (is_periph_unified_reg == TRUE)   //YODA
            {
              if(is_pmic4)
              {  
                err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x5, 0);
              }
              else
              {
                err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x4, 0);  
              }
            }
            else   //PMD9x07
            {
              err_flag |= pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x80, 0, 0);
            }
          }
          break;
        case PM_SW_MODE_NPM: /* normal power mode */
          {
              /* the LDO is already in Bypass mode and will be changed to NPM */
              if (ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                  if (is_periph_unified_reg == TRUE)
                  {
                    /* Configure ACTIVE_BYPASS_NPM */
                    err_flag = pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x2, 0);

                    /* step2: wait 100uS */
                    DALSYS_BusyWait(100);
                  }
                  else
                  {
                    /* exit bypass properly */
                    err_flag = pm_ldo_sw_mode_exit_bypass(ldo_ptr->comm_ptr, reg);
                  }
                  
                  if (err_flag == PM_ERR_FLAG__SUCCESS)
                  {
                      /* update the bypass LUT */
                      ldo_ptr->in_bypass[ldo_peripheral_index] = 0;
                  }
              }
            if (is_periph_unified_reg == TRUE)
            {
                err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x7, 0);
            }
            else
            {
              err_flag |= pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x80, 0x80, 0);
            }
          }
          break;
        case PM_SW_MODE_BYPASS: /* bypass mode */
          {
              if (!ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                  if (is_periph_unified_reg == TRUE)
                  {
                    if(is_pmic4)
                    {
                      err_flag = pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x3, 0);
                    }
                    else
                    {
                      err_flag = pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x2, 0);  
                    }
                  }
                  else
                  {
                    err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x20, 0x20, 0);
                  }
                  if (err_flag == PM_ERR_FLAG__SUCCESS)
                  {
                      /* update the bypass LUT */
                      ldo_ptr->in_bypass[ldo_peripheral_index] = 1;
                  }
              }
          }
          break;
           case PM_SW_MODE_RETENTION: /* Retention mode */   // supported on YODA
          {
              if (is_periph_unified_reg == TRUE)
              {
                  /* the LDO is already in Bypass mode and will be changed to NPM
                     before programming retention mode */
                  if (ldo_ptr->in_bypass[ldo_peripheral_index])
                  {
                      err_flag = pm_ldo_sw_mode(pmic_chip, ldo_peripheral_index, PM_SW_MODE_NPM);

                      /* wait 10uS for loop to settle */
                      DALSYS_BusyWait(10);
                  }
                  
                  if(is_pmic4)
                  {
                    err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x4, 0);    //LPM
                  }
                  else
                  {
                    err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x3, 0);    //LPM  
                  }
              }
              else
              {
                  err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
              }
          }
        default:
          err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
          break;
        }
    }
    return err_flag;
}



pm_err_flag_type pm_ldo_sw_mode_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type* sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_mode_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, sw_mode);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_pin_ctrled_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    uint8 select_pin = 0;

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off, &select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_pin_ctrl_status(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    pm_on_off_type on_off = PM_INVALID;

    if (ldo_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, &on_off, select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_volt_level_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_sw_enable_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_vreg_ok_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_soft_reset_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_soft_reset_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, status);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_soft_reset_exit(uint8 pmic_chip, uint8 ldo_peripheral_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_soft_reset_exit_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_volt_calculate_vset(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type  volt_level, uint32* vset)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_calculate_vset_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level, vset);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_volt_level_stepper_done_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *stepper_done)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_stepper_done_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, stepper_done);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_volt_level(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level);
    }
    return err_flag;
}


pm_err_flag_type pm_ldo_sw_mode_exit_bypass(pm_comm_info_type *comm_ptr, pm_register_address_type reg)
{
     pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
      
    if(comm_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    /* step1: set BYPASS_ACT (bit 6) */
    err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x40, 0x40, 0);

    /* step2: wait 100uS */
    DALSYS_BusyWait(100);

    /* step3: clear BYPASS_ACT and BYPASS_EN (bit 6 and bit 5) */
    err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x60, 0x00, 0);

    return err_flag;
}
