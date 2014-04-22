#ifndef __PMAPP_PWR_H__
#define __PMAPP_PWR_H__
/*===========================================================================


               P M A P P_ P W R  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations to support conversions between voltage values and PMIC
  register values
  
Copyright (c) 2013            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pmapp_pwr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/20/14   rk      Provide PMIC API in MPSS to set voltage for Vdd_MSS Rail (CR - 668036)
10/07/13   rh      File created
===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


typedef struct
{
  boolean valid;	 /**<True if data is valid. */
  uint8 data;        /**< Data. */
} pm_rail_cfg_info_type;

typedef struct
{
  uint8                  nNumConfigs;		/**<Total no. of configs being sent to MVC*/
  pm_rail_cfg_info_type *pm_rail_cfg_info;  /**<Pointer to structure of for command data and validity*/
} pm_rail_configs_type;




typedef struct
{
  pm_on_off_type            prev_rail_status;                                 /**< prev_ON/OFF rail_status */
  pm_on_off_type            current_rail_status;                              /**< current_ON/OFF rail_status */
  uint32                    prev_voltage;                                     /**< previous voltage */
  uint32                    current_voltage;                                  /**<Current Voltage */
  uint32                    settling_time;                                    /**< mss time taken to settle in usec */      
}pmapp_rail_status_type;


/** @addtogroup pmapp_pwr
@{ */

/*=========================================================================== */
/*                     pmapp_pwr_set_vdd_mss                                  */
/*=========================================================================== */

/**
  This function sets the MSS rail to the voltage specified by the voltage input
  parameter (in uV)

  PARAMETERS
 * @param[in]  voltage: Voltage to set MSS (in uV)

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS if successful, otherwise
                                  appropriate PMIC error flag is returned

*/
pm_err_flag_type pmapp_pwr_set_vdd_apps(uint32 voltage);

/*=========================================================================== */
/*                     pmapp_pwr_get_vdd_mss_status                           */
/*=========================================================================== */
/**
  This function gets the MSS rail  voltage(in uV) and MSS rail On or OFF Status

  PARAMETERS
 * @param[out]  voltage: Pointer to get Voltage
   @param[out]  on_off: Pointer to get on/off status
   @param[out]  get_status: Pointer to get status structure

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS if successful, otherwise
                                  appropriate PMIC error flag is returned

*/
/*=========================================================================== */

pm_err_flag_type pmapp_pwr_get_vdd_apps_status(uint32 *voltage, pm_on_off_type *on_off, pmapp_rail_status_type *get_status);

/*=========================================================================== */
/*                     pmapp_pwr_get_vdd_mss_status                           */
/*=========================================================================== */
/**
  This function Sets a flag if MSS Rail is managed by some external entity 

  PARAMETERS
  @param[in]  flag: Flag to indicate external entity wants to manage the rail 
   

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS if successful, otherwise
                                  appropriate PMIC error flag is returned

*/
/*=========================================================================== */
pm_err_flag_type pmapp_pwr_vdd_apps_external_manage (boolean flag);

/*=========================================================================== */
/*                     pmapp_pwr_mss_volt_level_info                           */
/*=========================================================================== */
/**
  Converts given voltage(in uV) to register value for Voltage_CTL2 register

  PARAMETERS
  @param[in]  voltage: value of the voltage 
  @param[in]  pm_mss_rail_configs : Ref. Pointer to structure array for passing
									 appropriate configuration

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS if successful, otherwise
                                  appropriate PMIC error flag is returned

*/
/*=========================================================================== */
pm_err_flag_type pmapp_pwr_apps_volt_level_info(uint32 voltage, pm_rail_configs_type *pm_mss_rail_configs);

/** @} */ /* end_addtogroup pmapp_pwr */

#endif /* __PMAPP_VSET_H__ */
