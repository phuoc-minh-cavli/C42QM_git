/*! \file pm_ldo.c
*  \n
*  \brief Implementation file for LDO public APIs.
*  \n
*  &copy; Copyright 2017-2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/0.2/QcomPkg/Library/PmicLib/drivers/ldo/src/pm_ldo.c#2 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   aab     Added pm_ldo_ocp_latched_status_clear()
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture  
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_ldo.h"
#include "pm_ldo_driver.h"
#include "pm_comm.h"


/*===========================================================================

                     LOCAL FUNCTION PROTOTYPE

===========================================================================*/

static pm_err_flag_type pm_ldo_sw_mode_exit_bypass(pm_comm_info_type *comm_ptr, pm_register_address_type reg);

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_ldo_sw_mode(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    //uint8 periph_type = 0;

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
		pm_register_data_type             data = 0;
        boolean is_ret_present = pwr_data->pwr_specific_info[ldo_peripheral_index].is_ret_present;
        boolean is_periph_unified_reg = pwr_data->pwr_specific_info[ldo_peripheral_index].is_periph_unified_reg;
		boolean is_pmic4 = pwr_data->pwr_specific_info[ldo_peripheral_index].pmic4_flag;
        //periph_type = pwr_data->pwr_specific_info[ldo_peripheral_index].periph_type;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[ldo_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: /* Low power mode */
          {
              /* if LDO is already in Bypass mode, exit bypass properly */
              if (ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                  if (is_periph_unified_reg == TRUE)
                  {
                      /* the LDO is already in Bypass mode and will be changed to NPM
                         before programming LPM mode */
                      err_flag = pm_ldo_sw_mode(pmic_chip, ldo_peripheral_index, PM_SW_MODE_NPM);

                      /* wait 10uS for loop to settle */
                      DALSYS_BusyWait(10);
					if(is_pmic4)
					{
						data = 0x5;
					}
					else
					{
						data = 0x4;
					}
					
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
              err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, data, 0);
              }
              else
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
                  /* exit bypass properly */
                  if (is_periph_unified_reg == TRUE)
                  {
                      /* Configure ACTIVE_BYPASS_NPM */
                      err_flag = pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x2, 0);

                      /* step2: wait 100uS */
                      DALSYS_BusyWait(100);
                  }
                  else
                  {
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
						data = 0x3;
					  }
					  else
					  {
						data = 0x2;
					  }
                      err_flag = pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, data, 0);
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
        case PM_SW_MODE_RETENTION: /* Retention mode */
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
                   if(is_ret_present)
                   {
                    err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x3, 0);   //Retention if STATUS3__RETENTION_PRESENT == 1. Otherwise, NPM.
                   }
                   else
                   {
                    err_flag |= pm_comm_write_byte(ldo_ptr->comm_ptr->slave_id, reg, 0x4, 0);    
                   }
              }
              else
              {
                  err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
              }
          }
          break;
        default:
          err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
          break;
        }
    }

    if (PM_ERR_FLAG__SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_MODE, pmic_chip, periph_type, ldo_peripheral_index, mode);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_MODE_ERROR, pmic_chip, periph_type, ldo_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_ldo_sw_mode_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type *mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, mode);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_sw_mode_status_raw(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 *mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_raw_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, mode);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_sw_enable(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
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

pm_err_flag_type pm_ldo_pull_down(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pull_down_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}


pm_err_flag_type pm_ldo_pull_down_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pull_down_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}
pm_err_flag_type pm_ldo_vreg_ready_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean* vreg_ready)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_is_vreg_ready_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, vreg_ready);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_pin_ctrled(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, select_pin);
    }

    return err_flag;
}

pm_err_flag_type pm_ldo_pin_ctrled_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
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



pm_err_flag_type 
pm_ldo_ocp_latched_status_clear(uint8 pmic_chip, uint8 peripheral_index, boolean *ocp_occured)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag |= PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(peripheral_index >= ldo_ptr->pm_pwr_data.num_of_peripherals)
    {
       err_flag |= PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else
    {
        err_flag |= pm_pwr_ocp_latched_status_clear_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, peripheral_index, ocp_occured);
    }

    return err_flag;
}


pm_err_flag_type pm_ldo_ocp_broadcast(uint8 pmic_chip, uint8  ldo_peripheral_index, pm_on_off_type  on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_ocp_broadcast_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}



/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/

pm_err_flag_type pm_ldo_sw_mode_exit_bypass(pm_comm_info_type *comm_ptr, pm_register_address_type reg)
{
     pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   
    CORE_VERIFY_PTR(comm_ptr);

    /* step1: set BYPASS_ACT (bit 6) */
    err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x40, 0x40, 0);

    /* step2: wait 100uS */
    DALSYS_BusyWait(100);

    /* step3: clear BYPASS_ACT and BYPASS_EN (bit 6 and bit 5) */
    err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x60, 0x00, 0);

    return err_flag;
}

