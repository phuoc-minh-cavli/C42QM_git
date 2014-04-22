/*! \file
*
*  \brief   ----File contains the implementation of the public APIs for GPIO resource type.
*  \details Implementation file for GPIO resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*
*    PMIC code generation Version: 2.0.0.19
*    This file contains code for Target specific settings and modes.
*
*  &copy; Copyright 2018 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/bcl/src/pm_bcl_comp.c#1 $


when           who          what, where, why
--------       ---          ----------------------------------------------------------
01/26/16        RL          Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_bcl_comp_driver.h"
#include "pm_comm.h"
#include "pm_ulog.h"
/*===========================================================================

                        extern variables and functions

===========================================================================*/

extern pm_err_flag_type  pm_comm_register_isr(uint32 slave_id, uint16 addr, uint8 irq_mask, void* isr_ptr, uint32 isr_ctx) ;

/*===========================================================================

                        Definitions

===========================================================================*/


#define BASE_ADDR_BCL (bcl_comp_ptr->bcl_comp_register->base_address)

#define REG_BCL(reg) ((bcl_comp_ptr->bcl_comp_register->base_address) \
                      + (bcl_comp_ptr->bcl_comp_register->reg))

#define SLAVE_ID_BCL (bcl_comp_ptr->comm_ptr->slave_id)

#define GET_BCL_ERR_CODE(ptr)  ptr==NULL ?                           \
                               PM_ERR_FLAG__FEATURE_NOT_SUPPORTED :  \
                               PM_ERR_FLAG__INVALID_PARAMETER

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type
pm_bcl_comp_set_thr_voltage(uint8 pmic_index,
                            pm_bcl_comp_thr_type thr_type,
                            uint32 volt_mV)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type voltage_thr_reg = 0;
  uint8 volt_val;

  if((thr_type>=PM_BCL_COMP_THR_INVALID) ||
     (bcl_comp_ptr == NULL) ||
     (volt_mV < bcl_comp_ptr->limits->min_mv) ||
     (volt_mV > bcl_comp_ptr->limits->max_mv))
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag, "error = %d, pmic=%d, thr_type=%d, mV=%d",
                     err_flag, pmic_index, thr_type, volt_mV);
    return err_flag;
  }

  switch (thr_type)
  {
    case PM_BCL_COMP_LOW1_THR:
      voltage_thr_reg = REG_BCL(vcomp_low1_thr);
      break;
    case PM_BCL_COMP_LOW2_THR:
      voltage_thr_reg = REG_BCL(vcomp_low2_thr);
      break;
    default:
      CORE_VERIFY(0);
      break;
  }

  volt_val = (uint8)((volt_mV - bcl_comp_ptr->limits->min_mv)/
                      bcl_comp_ptr->limits->step_mv);

  err_flag |= pm_comm_write_byte(SLAVE_ID_BCL, voltage_thr_reg, volt_val, 0);

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}

pm_err_flag_type
pm_bcl_comp_irq_enable(uint8 pmic_index,
                       pm_bcl_comp_thr_type thr_type,
                       boolean enabled)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 irq_mask;

  if((thr_type>=PM_BCL_COMP_THR_INVALID) || (bcl_comp_ptr == NULL))
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d, thr_type=%d, enable=%d",
                     err_flag, pmic_index, thr_type, enabled);
    return err_flag;
  }

  //Get Enable Mask from threshold type
  if(bcl_comp_ptr->version_update)
  {
    irq_mask = 2<<thr_type;
  }
  else
  {
    irq_mask = 1<<thr_type;
  }

  err_flag |= pm_comm_write_byte(SLAVE_ID_BCL,
              (enabled==TRUE ? REG_BCL(int_en_set) : REG_BCL(int_en_clr)),
              irq_mask, 0);

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}


pm_err_flag_type
pm_bcl_comp_irq_clear(uint8 pmic_index,
                      pm_bcl_comp_thr_type thr_type)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 irq_mask;

  if((thr_type>=PM_BCL_COMP_THR_INVALID) || (bcl_comp_ptr == NULL))
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d, thr_type=%d",
                     err_flag, pmic_index, thr_type);
    return err_flag;
  }

  //Get Mask from threshold type
  if(bcl_comp_ptr->version_update)
  {
    irq_mask = 2<<thr_type;
  }
  else
  {
    irq_mask = 1<<thr_type;
  }

  err_flag |= pm_comm_write_byte(SLAVE_ID_BCL, REG_BCL(int_latched_clr),
                                 irq_mask, 0);

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}

pm_err_flag_type
pm_bcl_comp_enable_status(uint8 pmic_index,
                          pm_on_off_type *on_off)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 reg_data;

  if((on_off==NULL) || (bcl_comp_ptr == NULL))
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d, on_off=0x%x",
                     err_flag, pmic_index, on_off);
    return err_flag;
  }

  err_flag |= pm_comm_read_byte(SLAVE_ID_BCL, REG_BCL(en_ctl), &reg_data, 0 );

  *on_off=(reg_data & 0x80)==0x80? PM_ON: PM_OFF;

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}


pm_err_flag_type
pm_bcl_comp_enable(uint8 pmic_index,
                   pm_on_off_type on_off)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if((on_off>=PM_INVALID) || (bcl_comp_ptr == NULL))
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d, on_off=%d",
                     err_flag, pmic_index, on_off);
    return err_flag;
  }

  err_flag |= pm_comm_write_byte(SLAVE_ID_BCL, REG_BCL(en_ctl),
                                (on_off==PM_ON ? 0x80 : 0x00), 0 );

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}

pm_err_flag_type
pm_bcl_comp_register_isr(uint8 pmic_index,
                         pm_bcl_comp_thr_type thr_type,
                         void *isr_ptr)
{
  pm_bcl_comp_data_type *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 irq_mask;
  uint32 isr_ctx;

  if((thr_type>=PM_BCL_COMP_THR_INVALID) ||
     (bcl_comp_ptr == NULL) ||
     (isr_ptr==NULL))
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d, thr_type=%d, isr=0x%x",
                     err_flag, pmic_index, thr_type, isr_ptr);
    return err_flag;
  }

  //Get IRQ Mask
  if(bcl_comp_ptr->version_update)
  {
    irq_mask = 2<<thr_type;
  }
  else
  {
    irq_mask = 1<<thr_type;
  }

  //Get CTX save slave id and peripheral id in ctx
  //MSB 16 bits are slave id, LSB 16 bits are peripheral index
  isr_ctx = (((uint32)(SLAVE_ID_BCL) & 0xFFFF)<<16) |
            ((uint32)(BASE_ADDR_BCL) >> 8);

  err_flag=pm_comm_register_isr(SLAVE_ID_BCL, BASE_ADDR_BCL,
                               irq_mask, isr_ptr, isr_ctx);

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}

pm_err_flag_type
pm_bcl_comp_enable_vcomp(uint8 pmic_index)
{
  pm_bcl_comp_data_type     *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type          err_flag = PM_ERR_FLAG__SUCCESS;
  uint8                     vcomp_en_mask = 0;
  pm_register_address_type  reg = 0;

  if(bcl_comp_ptr == NULL)
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d",
                     err_flag, pmic_index);
    return err_flag;
  }

  //Enable VCOMP Based on version type
  if(bcl_comp_ptr->version_update)
  {
    /*Data is same as mask*/
    vcomp_en_mask = 0x6;
    reg = REG_BCL(mode_ctl1);
  }
  else
  {
    /*Data is same as mask*/
    vcomp_en_mask = 0x3;
    reg = REG_BCL(mode_ctl);
  }

  err_flag |= pm_comm_write_byte_mask(SLAVE_ID_BCL, reg, vcomp_en_mask,
                                      vcomp_en_mask, 0);

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}

pm_err_flag_type
pm_bcl_comp_set_irq_polarity(uint8 pmic_index,
                             pm_bcl_comp_thr_type thr_type,
                             pm_irq_trigger_type trigger)
{
  pm_bcl_comp_data_type     *bcl_comp_ptr = pm_bcl_comp_get_data(pmic_index);
  pm_err_flag_type          err_flag      = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  reg_clear     = 0;
  pm_register_address_type  reg_write     = 0;
  uint8                     mask          = 0;

  if(bcl_comp_ptr == NULL ||
     thr_type >= PM_BCL_COMP_THR_INVALID ||
     trigger >= PM_IRQ_TRIGGER_INVALID)
  {
    err_flag = GET_BCL_ERR_CODE(bcl_comp_ptr);
    PM_LOG_MSG_ERROR(err_flag,"error = %d, pmic=%d, thr_type = %d, trig = %d",
                     err_flag, pmic_index, thr_type, trigger);
    return err_flag;
  }

  mask = (bcl_comp_ptr->version_update == TRUE) ? (2<<thr_type) : (1<<thr_type);

  switch(trigger)
  {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
      reg_write = REG_BCL(int_pol_low);
      reg_clear = REG_BCL(int_pol_high);
      break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
      reg_write = REG_BCL(int_pol_high);
      reg_clear = REG_BCL(int_pol_low);
      break;
    default:
      err_flag = PM_ERR_FLAG__INVALID_PARAMETER;
      return err_flag;
      break;
  }

  //Set interrupt to be level triggered
  err_flag |= pm_comm_write_byte_mask(SLAVE_ID_BCL, REG_BCL(int_set_type), 
                                      mask, 0, 0);

  err_flag |= pm_comm_write_byte_mask(SLAVE_ID_BCL, reg_clear, mask, 0, 0);

  err_flag |= pm_comm_write_byte_mask(SLAVE_ID_BCL, reg_write, mask, mask, 0);

  PM_LOG_MSG_ERROR(err_flag,"error = %d", err_flag);

  return err_flag;
}
