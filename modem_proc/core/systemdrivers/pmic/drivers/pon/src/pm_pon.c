/*! \file pm_pon.c
*  \n
*  \brief PMIC PowerON Peripheral driver
*  \n
*  \n This header file contains class implementation of the PON
*  \n peripheral APIs
*  \n
*   Copyright (c) 2012-2019, 2023 Qualcomm Technologies, Inc. All rights reserved.
*/
/*===================================================================
			        EDIT HISTORY FOR MODULE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/25/19   abh     Added file for Tiny Modem image in Twizy
========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "pm_version.h"
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pmio_pon.h"
#include "hw_module_type.h"
#include "device_info.h"
#include "pm_comm.h"
#include "pm_utils.h"
#include "busywait.h"


/*===========================================================================
                 #defines 
===========================================================================*/

/* PMIC A Primary SlaveID */
#define PMIC_A_SLAVEID_PRIM      0x00 

#define PON_RESET_CFG_INVALID_VAL                   0xFF

/* PON_DVDD_RB_SPARE reg Warm Reset indicating bit mask */
#define PON_DVDD_RB_SPARE_WARM_RESET_MASK                 0x01

#define WAIT_FOR_SLEEP_CLK_CYCLE(num_of_sleep_cycle)  busywait((3052*num_of_sleep_cycle)/100) //sleep cycle is 32.7645KHz

/*===========================================================================
                 local global variable 
===========================================================================*/

static boolean pm_pon_initialized = FALSE;

typedef struct
{
  uint16 reset_ctl_addr;
  uint16 reset_ctl2_addr;
  uint16 reset_en_mask;
  uint16 reset_type_mask;
}pon_ps_hold_reg_info_type;

static pon_ps_hold_reg_info_type
  pon_ps_hold_reg =
{
  PMIO_PON_PS_HOLD_RESET_CTL_ADDR,
  PMIO_PON_PS_HOLD_RESET_CTL2_ADDR,
  PMIO_PON_PS_HOLD_RESET_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_PS_HOLD_RESET_CTL_RESET_TYPE_BMSK
};

static pm_register_data_type pon_reset_cfg_val[] =
{
  [PM_PON_RESET_CFG_WARM_RESET]                                         = 0x01,
  [PM_PON_RESET_CFG_HARD_RESET]                                         = 0x07,
  [PM_PON_RESET_CFG_NORMAL_SHUTDOWN]                                    = 0x04,
  [PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET]                       = 0x08,
  [PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN]                         = 0x05,
  [PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN]                    = 0x06,
  [PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET]                  = 0x09,
  [PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN]          = 0x02,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN]         = 0x0A,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN]    = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN]                           = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET]                         = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET]       = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET]  = PON_RESET_CFG_INVALID_VAL,
};

/*===========================================================================
                  Function Prototypes
===========================================================================*/


/*===========================================================================
                  Local Function
===========================================================================*/

/* Internal function to initialize the pon driver */
static pm_err_flag_type pm_pon_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8   periph_type = 0;

  peripheral_info_type  pon_peripheral_info;
  uint8   warm_reset_reason[2] = { 0 };

  if (pm_pon_initialized == TRUE)
  {
    return err_flag;
  }

  memset(&pon_peripheral_info, 0, sizeof(peripheral_info_type));
  pon_peripheral_info.base_address = PON_BASE;

  err_flag |= pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_PERPH_TYPE_ADDR, &periph_type, 0);
  if ((periph_type != PM_HW_MODULE_PON) || (err_flag != PM_ERR_FLAG__SUCCESS))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Read ON reason and write the DVDD_RB_SPARE register bit <0> to 1 if Warm Reset occured */
  err_flag |= pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_GEN2_ON_REASON_ADDR, warm_reset_reason, 0);
  if (warm_reset_reason[0] & PMIO_PON_GEN2_ON_REASON_WARM_SEQ_BMSK)
  {
    err_flag |= pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_PON_DVDD_RB_SPARE_ADDR, PON_DVDD_RB_SPARE_WARM_RESET_MASK, 0x01, 0);
  }

  if (err_flag == PM_ERR_FLAG__SUCCESS)
  {
    pm_pon_initialized = TRUE;
  }

  return err_flag;
}

/* Internal function to return slave ID */
static pm_err_flag_type pm_pon_get_pon_info(uint8 pmic_device_index, uint32 *pon_slave_id)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (NULL == pon_slave_id)
  {
    return PM_ERR_FLAG__INVALID;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (pm_pon_initialized != TRUE)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
  }

  err_flag = pm_get_slave_id(pmic_device_index, 0, pon_slave_id);

  return err_flag;
}


/*===========================================================================
API Implementation
===========================================================================*/

pm_err_flag_type
pm_pon_ps_hold_cfg(uint32 pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 ps_hold_cfg_val = 0;
  uint32 slave_id = 0;

  if (ps_hold_cfg >= PM_PON_RESET_CFG_INVALID)
  {
    err_flag = PM_ERR_FLAG__INVALID;
  }
  else if (pon_reset_cfg_val[ps_hold_cfg] == PON_RESET_CFG_INVALID_VAL)
  {
    err_flag = PM_ERR_FLAG__INVALID;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      ps_hold_cfg_val = pon_reset_cfg_val[ps_hold_cfg];

      /* i.) Disable reset ii.) wait for 10 sleep clock cycles iii.)config and iv.) enable reset */
      err_flag = pm_comm_write_byte(slave_id, pon_ps_hold_reg.reset_ctl2_addr, 0x0, 0);
      WAIT_FOR_SLEEP_CLK_CYCLE(10);
      err_flag |= pm_comm_write_byte(slave_id, pon_ps_hold_reg.reset_ctl_addr, ps_hold_cfg_val, 0);
      WAIT_FOR_SLEEP_CLK_CYCLE(8);
      err_flag |= pm_comm_write_byte(slave_id, pon_ps_hold_reg.reset_ctl2_addr, pon_ps_hold_reg.reset_en_mask, 0);
      WAIT_FOR_SLEEP_CLK_CYCLE(3);
    }
  }

  return err_flag;
}

/* API available for page only image */
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE
pm_err_flag_type 
pm_pon_trigger_enable(uint32 pmic_device_index, pm_pon_trigger_type trigger, boolean enable)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 mask, data;

  data = (enable) ? 0xFF : 0x00;

  if (pmic_device_index > PMIC_A_SLAVEID_PRIM)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }
  else if (trigger >= PM_PON_TRIGGER_INVALID)
  {
    return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }

  switch (trigger)
  {
     case PM_PON_TRIGGER_KPDPWR:
       mask = PMIO_PON_PON_TRIGGER_EN_KPDPWR_N_BMSK;
       break;
     case PM_PON_TRIGGER_CBLPWR:
       mask = PMIO_PON_PON_TRIGGER_EN_CBLPWR_N_BMSK;
       break;
     case PM_PON_TRIGGER_PON1:
       mask = PMIO_PON_PON_TRIGGER_EN_PON1_BMSK;
       break;
     case PM_PON_TRIGGER_USB_CHG:
       mask = PMIO_PON_PON_TRIGGER_EN_USB_CHG_BMSK;
       break;
     case PM_PON_TRIGGER_DC_CHG:
       mask = PMIO_PON_PON_TRIGGER_EN_DC_CHG_BMSK;
       break;
     case PM_PON_TRIGGER_RTC:
       mask = PMIO_PON_PON_TRIGGER_EN_RTC_BMSK;
       break;
     case PM_PON_TRIGGER_SMPL:
       mask = PMIO_PON_PON_TRIGGER_EN_SMPL_BMSK;
       break;
     default:
       return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Requires 3 sleep clock cycles of delay */
  busywait(100);

  err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_PON_PON_TRIGGER_EN_ADDR, mask, data, 0);
  
  /* Only for Twizy, check if AON Periph present on other Target before porting the api */
  switch (trigger)
  {
     case PM_PON_TRIGGER_KPDPWR:
       mask = PMIO_AON_TRIGGER_EN_KPDPWR_N_BMSK;
       break;
     case PM_PON_TRIGGER_PON1:
       mask = PMIO_AON_TRIGGER_EN_PON1_BMSK;
       break;
     case PM_PON_TRIGGER_RTC:
       mask = PMIO_AON_TRIGGER_EN_RTC_BMSK;
       break;
     default:
       return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  
  /* Requires 3 sleep clock cycles of delay */
  busywait(100);

  err_flag |= pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_AON_LPI_PON_TRIGGER_EN_ADDR, mask, data, 0);
  
  return err_flag;
}
#endif

pm_err_flag_type 
pm_pon_irq_set_trigger(uint8 pmic_device_index, pm_pon_irq_type irq, pm_irq_trigger_type trigger)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1<<irq;
  uint8 set_type = 0;
  uint8 polarity_high = 0;
  uint8 polarity_low = 0;
  
  if (irq >= PM_PON_IRQ_INVALID)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get PMIC device Primary slave id value since PON module
     is on the Primary slave id */
  err_flag = pm_get_slave_id(pmic_device_index, 0, &slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }
    
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

  err_flag = pm_comm_write_byte_mask(slave_id, PMIO_PON_INT_SET_TYPE_ADDR, mask, set_type, 0);
  err_flag |= pm_comm_write_byte_mask(slave_id, PMIO_PON_INT_POLARITY_HIGH_ADDR, mask, polarity_high, 0);
  err_flag |= pm_comm_write_byte_mask(slave_id, PMIO_PON_INT_POLARITY_LOW_ADDR, mask, polarity_low, 0);
    
  return err_flag;
}

pm_err_flag_type 
pm_pon_irq_clear(uint8  pmic_device_index, pm_pon_irq_type irq)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1<<irq;
  
  if (irq >= PM_PON_IRQ_INVALID)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get PMIC device Primary slave id value since PON module
     is on the Primary slave id */
  err_flag = pm_get_slave_id(pmic_device_index, 0, &slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }

  err_flag = pm_comm_write_byte(slave_id, PMIO_PON_INT_LATCHED_CLR_ADDR, mask, 0);
    
  return err_flag;
}

pm_err_flag_type 
pm_pon_irq_enable(uint8 pmic_device_index, pm_pon_irq_type irq, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1<<irq;
  
  if (irq >= PM_PON_IRQ_INVALID)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get PMIC device Primary slave id value since PON module
     is on the Primary slave id */
  err_flag = pm_get_slave_id(pmic_device_index, 0, &slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }

  if (enable)
  {
    err_flag = pm_comm_write_byte(slave_id, PMIO_PON_INT_EN_SET_ADDR, mask, 0);
  }
  else
  {
    err_flag = pm_comm_write_byte(slave_id, PMIO_PON_INT_EN_CLR_ADDR, mask, 0);
  }
    
  return err_flag;
}

pm_err_flag_type 
pm_pon_irq_status(uint8 pmic_device_index, pm_pon_irq_type irq, pm_irq_status_type type, boolean *status)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1<<irq;
  uint8 data = 0;
  
  if (status == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }
  
  if (irq >= PM_PON_IRQ_INVALID)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get PMIC device Primary slave id value since PON module
     is on the Primary slave id */
  err_flag = pm_get_slave_id(pmic_device_index, 0, &slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }

  switch (type)
  {
  case PM_IRQ_STATUS_RT:
    err_flag = pm_comm_read_byte(slave_id, PMIO_PON_INT_RT_STS_ADDR, &data, 0);
    break;
  case PM_IRQ_STATUS_LATCHED:
    err_flag = pm_comm_read_byte(slave_id, PMIO_PON_INT_LATCHED_STS_ADDR, &data, 0);
    break;
  case PM_IRQ_STATUS_PENDING:
    err_flag = pm_comm_read_byte(slave_id, PMIO_PON_INT_PENDING_STS_ADDR, &data, 0);
    break;
  default:
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if(err_flag == PM_ERR_FLAG__SUCCESS)
  {
    data = data & mask;
  }

  *status = data ? TRUE : FALSE;
    
  return err_flag;
}

pm_err_flag_type 
pm_pon_reason_debug(uint8 pmic_device_index, pm_pon_debug_type *ptr_debug_data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 slave_id = 0;
  if (ptr_debug_data == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }
  err_flag = pm_get_slave_id(pmic_device_index, 0, &slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }
  err_flag |= pm_comm_read_byte(slave_id, 0x8C0, &ptr_debug_data->pon_data[0], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8C2, &ptr_debug_data->pon_data[1], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8C4, &ptr_debug_data->pon_data[2], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8C5, &ptr_debug_data->pon_data[3], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8C7, &ptr_debug_data->pon_data[4], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8C8, &ptr_debug_data->pon_data[5], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8C9, &ptr_debug_data->pon_data[6], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8CA, &ptr_debug_data->pon_data[7], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x8CB, &ptr_debug_data->pon_data[8], 0);
  
  err_flag |= pm_comm_read_byte(slave_id, 0xa40, &ptr_debug_data->AON_data[0], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0xa41, &ptr_debug_data->AON_data[1], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0xa46, &ptr_debug_data->AON_data[2], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0xa47, &ptr_debug_data->AON_data[3], 0);
  
  err_flag |= pm_comm_read_byte(slave_id, 0x103, &ptr_debug_data->pmic_version[0], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x160, &ptr_debug_data->pmic_version[1], 0);
  err_flag |= pm_comm_read_byte(slave_id, 0x161, &ptr_debug_data->pmic_version[2], 0);
  
  return err_flag;
}

