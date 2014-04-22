/*! \file pm_rpm_utilities.c
 *
 *  \brief This file contains RPM PMIC utility functions for trans apply aggregation layer.
 *
 *  &copy; Copyright 2012 - 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/pmic/npa/src/pm_rpm_utilities.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/15   akt     Updated waiting delay to 2 times for ignored rails  (CR-787855)
11/03/14   mr      Removed KW Errors from PMIC SW (CR-735987)
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
01/29/13   aks     Adding support for Boost as separate peripheral
05/18/12   wra     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include <stringl/stringl.h>
#include "rpmserver.h"
#include "pm_rpm_utilities.h"
#include "CoreVerify.h"
#include "pm_rpm_smps_trans_apply.h"
#include "pm_rpm_ldo_trans_apply.h"

/*===========================================================================

                        MACROS AND TYPE DEFINITIONS

===========================================================================*/

#define RPM_LDO_REQ         0x6F646C    // 'ldo'  in little endian
#define RPM_SMPS_REQ        0x706D73 // 'smp' in little endian

#define PM_NUM_OF_STATUS_REGS 2
#define PM_NUM_OF_VOLT_REGS   8
#define PM_STATUS1_REG_OFFSET 0x08
#define PM_VSET_LB_REG_OFFSET 0x40

/* Rail status register dump info */
typedef struct
{
	pm_err_flag_type err_flag;
    uint8 status_reg_dump[PM_NUM_OF_STATUS_REGS];
    uint8 vset_lb_base_reg_dump[PM_NUM_OF_VOLT_REGS];
}pm_rail_status_info_type;


/* Settling errors log info */
typedef struct
{
    uint32 actual_time_us;
    uint32 estimated_time_us;
    uint8 periph_type;
    uint8 periph_index;
    uint8 pmic_index;
    pm_rail_status_info_type reg_dump;
}pm_settle_info_type;

pm_settle_info_type pm_settle_vreg_ok_err_info;

pm_settle_info_type pm_settle_stepper_done_err_info;


pm_err_flag_type pm_rpm_status_reg_dump(uint32 slave_id, uint32 base_address, pm_rail_status_info_type* rail_status);


/*===========================================================================

                     FUNCTION IMPLEMENTATION

===========================================================================*/

int pm_rpm_int_copy(void *source, void *destination, size_t num)
{
    int change_detected = 0;

    change_detected = memcmp(destination, source, num);

    if(change_detected != 0)
    {
        memscpy(destination, num, source, num);
    }

    return change_detected;
}

boolean pm_rpm_check_vreg_settle_status(uint64 settle_start_time, uint32 estimated_settling_time_us,
                                        pm_pwr_data_type *pwr_res, pm_comm_info_type *comm_ptr,
                                        uint8 resource_index, boolean settling_err_en)
{
    boolean vreg_status = FALSE;
    uint64 current_time = 0;
    uint64 settle_end_time = 0;
    uint32 max_settling_time_us = 0;
    uint32 actual_time_us = 0;

    // calculate the time at which software should end the polling/settling for the rail
    // which is after maximum allowed settling time for the rail has elapsed (10 times the
    // estimated settling time); for the rails to ignore the ERROR fatal, wait two times
    if(settling_err_en == TRUE)
    {
        max_settling_time_us = PM_RPM_MAX_SETTLING_TIME_MULTIPLIER * estimated_settling_time_us;
    }
    else
    {
        max_settling_time_us = PM_RPM_MIN_SETTLING_TIME_MULTIPLIER * estimated_settling_time_us;
    }
    settle_end_time = settle_start_time + pm_convert_time_to_timetick(max_settling_time_us);

    /* Get the current time and check if the maximum allowed settle time on the rail has elapsed */
    current_time = time_service_now();

    pm_pwr_sw_enable_vreg_ok_status_alg(pwr_res, comm_ptr, resource_index, &vreg_status);

    while(vreg_status == FALSE)
    {
        /* Log and abort if the rail has not finished settling after maximum allowed settle time on the rail has elapsed */
        if(current_time > settle_end_time)
        {
            actual_time_us = pm_convert_timetick_to_time(current_time - settle_start_time);

            /* saving the settling info for the error case */
            pm_settle_vreg_ok_err_info.actual_time_us = actual_time_us;
            pm_settle_vreg_ok_err_info.estimated_time_us = estimated_settling_time_us;  
            pm_settle_vreg_ok_err_info.periph_type = pwr_res->pwr_specific_info[resource_index].periph_type;
            pm_settle_vreg_ok_err_info.periph_index = resource_index;  
            pm_settle_vreg_ok_err_info.pmic_index = comm_ptr->pmic_index;

			pm_rpm_status_reg_dump(comm_ptr->slave_id, pwr_res->pwr_specific_info[resource_index].periph_base_address,
											  &pm_settle_vreg_ok_err_info.reg_dump);


            PM_LOG_MSG_ERROR(PMIC_RPM_VREG_SETTLING_ERROR, comm_ptr->pmic_index, pwr_res->pwr_specific_info[resource_index].periph_type, resource_index);
          
            if(settling_err_en == TRUE)
            {
              CORE_VERIFY(0); // abort
            }
            break;
        }

        DALSYS_BusyWait(PM_RPM_SETTLING_TIME_POLL_INTERVAL); // us

        /* Get the current time and check if the maximum allowed settle time on the rail has elapsed */
        current_time = time_service_now();

        pm_pwr_sw_enable_vreg_ok_status_alg(pwr_res, comm_ptr, resource_index, &vreg_status);
    }

    return vreg_status;
}


boolean pm_rpm_check_stepper_settle_status(uint64 settle_start_time, uint32 estimated_settling_time_us,
                                           pm_pwr_data_type *pwr_res, pm_comm_info_type *comm_ptr,
                                           uint8 resource_index, boolean settling_err_en)
{
    boolean stepper_status = FALSE;
    uint64 current_time = 0;
    uint64 settle_end_time = 0;
    uint32 max_settling_time_us = 0;
    uint32 actual_time_us = 0;

    // calculate the time at which software should end the polling/settling for the rail
    // which is after maximum allowed settling time for the rail has elapsed (10 times the
    // estimated settling time); for the rails to ignore the ERROR fatal, wait two times
    if(settling_err_en == TRUE)
    {
        max_settling_time_us = PM_RPM_MAX_SETTLING_TIME_MULTIPLIER * estimated_settling_time_us;
    }
    else
    {
        max_settling_time_us = PM_RPM_MIN_SETTLING_TIME_MULTIPLIER * estimated_settling_time_us;
    }
    settle_end_time = settle_start_time + pm_convert_time_to_timetick(max_settling_time_us);

    /* Get the current time and check if the maximum allowed settle time on the rail has elapsed */
    current_time = time_service_now();

    pm_pwr_volt_level_stepper_done_status_alg(pwr_res, comm_ptr, resource_index, &stepper_status);

    while(stepper_status == FALSE)
    {
        /* Log and abort if the rail has not finished settling after maximum allowed settle time on the rail has elapsed */
        if(current_time >= settle_end_time)
        {
            actual_time_us = pm_convert_timetick_to_time(current_time - settle_start_time);

            /* saving the settling info for the error cases */
            pm_settle_stepper_done_err_info.actual_time_us = actual_time_us;
            pm_settle_stepper_done_err_info.estimated_time_us = estimated_settling_time_us; 
            pm_settle_stepper_done_err_info.periph_type = pwr_res->pwr_specific_info[resource_index].periph_type;
            pm_settle_stepper_done_err_info.periph_index = resource_index;  
            pm_settle_stepper_done_err_info.pmic_index = comm_ptr->pmic_index;

			pm_rpm_status_reg_dump(comm_ptr->slave_id, pwr_res->pwr_specific_info[resource_index].periph_base_address,
											  &pm_settle_stepper_done_err_info.reg_dump);

            PM_LOG_MSG_ERROR(PMIC_RPM_STEPPER_SETTLING_ERROR, comm_ptr->pmic_index, pwr_res->pwr_specific_info[resource_index].periph_type, resource_index);
         
            if(settling_err_en == TRUE)
            {
              CORE_VERIFY(0); // abort
            }
            break;
        }

        DALSYS_BusyWait(PM_RPM_SETTLING_TIME_POLL_INTERVAL); // us

        /* Get the current time and check if the maximum allowed settle time on the rail has elapsed */
        current_time = time_service_now();

        pm_pwr_volt_level_stepper_done_status_alg(pwr_res, comm_ptr, resource_index, &stepper_status);
    }

    return stepper_status;
}

pm_err_flag_type pm_rpm_calculate_vset(rpm_resource_type rsrc_type, uint32 rsrc_index, uint32 volt_uv, uint32* vset)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 pmic_chip = 0;
  uint8 periph_index = rsrc_index - 1;

  if(vset == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Calculate vset */
  switch(rsrc_type & 0x00FFFFFF)
  {
    case RPM_LDO_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_LDO(rsrc_type);
      err_flag = pm_ldo_calculate_vset(pmic_chip, periph_index, volt_uv, vset);
    }
    break;
    case RPM_SMPS_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_SMPS(rsrc_type);
      err_flag = pm_smps_calculate_vset(pmic_chip, periph_index, volt_uv, vset);
    }
    break;
  }

  return err_flag;
}

pm_err_flag_type pm_rpm_volt_level_status(rpm_resource_type rsrc_type, uint32 rsrc_index, uint32* volt_uv)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 pmic_chip = 0;
  uint8 periph_index = rsrc_index - 1;

  if(volt_uv == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Calculate vset */
  switch(rsrc_type & 0x00FFFFFF)
  {
    case RPM_LDO_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_LDO(rsrc_type);
      err_flag = pm_ldo_volt_level_status(pmic_chip, periph_index, volt_uv);
    }
    break;
    case RPM_SMPS_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_SMPS(rsrc_type);
      err_flag = pm_smps_volt_level_status(pmic_chip, periph_index, volt_uv);
    }
    break;
  }

  return err_flag;
}

pm_err_flag_type pm_rpm_sw_mode_status_raw(rpm_resource_type rsrc_type, uint32 rsrc_index, uint8* mode)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 pmic_chip = 0;
  uint8 periph_index = rsrc_index - 1;

  if(mode == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Calculate vset */
  switch(rsrc_type & 0x00FFFFFF)
  {
    case RPM_LDO_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_LDO(rsrc_type);
      err_flag = pm_ldo_sw_mode_status_raw(pmic_chip, periph_index, mode);
    }
    break;
    case RPM_SMPS_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_SMPS(rsrc_type);
      err_flag = pm_smps_sw_mode_status_raw(pmic_chip, periph_index, mode);
    }
    break;
  }

  return err_flag;
}

pm_err_flag_type pm_rpm_is_periph_unified_reg(rpm_resource_type rsrc_type, uint32 rsrc_index, boolean* is_periph_unified_reg)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 pmic_chip = 0;
  uint8 periph_index = rsrc_index - 1;
  pm_ldo_data_type *ldo_ptr = NULL;
  pm_smps_data_type *smps_ptr = NULL;
  pm_pwr_data_type *pwr_data = NULL;

  if(is_periph_unified_reg == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Calculate vset */
  switch(rsrc_type & 0x00FFFFFF)
  {
    case RPM_LDO_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_LDO(rsrc_type);
      ldo_ptr = pm_ldo_get_data(pmic_chip);
      if(ldo_ptr != NULL)
      {
        pwr_data = &(ldo_ptr->pm_pwr_data);
      }
    }
    break;
    case RPM_SMPS_REQ:
    {
      pmic_chip = GET_PMIC_INDEX_SMPS(rsrc_type);
      smps_ptr = pm_smps_get_data(pmic_chip);
      if(smps_ptr != NULL)
      {
        pwr_data = &(smps_ptr->pm_pwr_data);
      }
    }
    break;
  }

  if (pwr_data == NULL)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  *is_periph_unified_reg = pwr_data->pwr_specific_info[periph_index].is_periph_unified_reg;

  return err_flag;
}

pm_err_flag_type pm_rpm_status_reg_dump(uint32 slave_id, uint32 base_address, pm_rail_status_info_type* rail_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0x0;

    reg = (pm_register_address_type)(base_address + PM_STATUS1_REG_OFFSET);
    err_flag |= pm_comm_read_byte_array(slave_id, reg, PM_NUM_OF_STATUS_REGS, rail_status->status_reg_dump, 0);

    reg = (pm_register_address_type)(base_address + PM_VSET_LB_REG_OFFSET);
    err_flag |= pm_comm_read_byte_array(slave_id, reg, PM_NUM_OF_VOLT_REGS, rail_status->vset_lb_base_reg_dump, 0);

    rail_status->err_flag = err_flag;

    return err_flag;
}



