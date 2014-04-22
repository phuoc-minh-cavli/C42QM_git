/*! \file pm_diag_pwr_rails.c
*
*  \brief Diag handlers to get PMIC power rails status.
*  \n
*  &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag_pwr_rails.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------
12/05/14   mr      Added Dual PMIC support for MSM8909. (CR-764010)
08/26/13   rk      changes for pmic model for pm8916 and remove pin control checking.
01/30/13   kt      Adding pin_ctrled_status API call.
01/28/13   kt      Removing the hack for second PMIC device model type.
01/08/13   kt      Initial creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_pwr_alg.h"
#include "pm_smps_driver.h"
#include "pm_boost_driver.h"
#include "pm_ldo_driver.h"
#include "pm_vs_driver.h"
#include "pm_diag.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/*
 * Power Rails type used in QXDM/Diag communications.
 */
typedef enum
{
  PM_DIAG_PWR_RAILS_SMPS,
  PM_DIAG_PWR_RAILS_LDO,
  PM_DIAG_PWR_RAILS_VS,
  PM_DIAG_PWR_RAILS_BOOST
} pm_diag_pwr_rails_type;

/*
 * Diag command packet to start/stop power rails status logging.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      status_flag;
} pm_diag_pwr_rails_log_req_type;


/*
 * Diag response packet sent back to the test tool.
 *
 * The status of the power rails log is appended to
 * this structure in memory.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      command_type;
  uint8                      result_type;
  uint8                      num_pwr_rails;
  uint8                      pmic_name_len;
} pm_diag_pwr_rails_log_resp_type;

/*
 * This is a callback function which gets executed upon the reception and
 * routing of a Diag subsystem command/request packet to our driver.
 */
static void * pm_diag_pwr_rails_log_handler(void *req_ptr, uint16 req_len);

/*=========================================================================
      Data
==========================================================================*/

static const diagpkt_user_table_entry_type pm_pwr_rails_diag_table[] =
{
  {PM_DIAG_PWR_RAILS_LOG_CMD_ID, PM_DIAG_PWR_RAILS_LOG_CMD_ID, pm_diag_pwr_rails_log_handler}
};

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  boolean                    log_enabled_flag;                                /**< state variable */
  uint8                      num_pmic_devices;                                /**< # of pmic devices */
  uint32                     num_pwr_rails;                                   /**< # of power rails being logged */
  uint32                     start_resp_size;                                 /**< size of packet in response to start cmd */
  DalDeviceHandle           *timetick_handle;                                 /**< handle for getting system time stamp */
} pm_pwr_rails_diag;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : pm_diag_pwr_rails_smps_status
** =========================================================================*/
/**
  Gets the status for the SMPS and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the SMPS and fills in the diag packet memory.

  @param **pwr_rail_log_item_ptr   [out]  - Storage for Power rail's logs.
  @param   pmic_index       [in]   - PMIC device index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_pwr_rails_smps_status
(
  pm_pwr_rails_diag_log_item_type **pwr_rail_log_item_ptr,
  uint8 pmic_index
)
{
  pm_sw_mode_type                   mode = PM_SW_MODE_INVALID;
  boolean                           vreg_ok_status = FALSE;
  pm_on_off_type                    sw_enable_status = PM_INVALID;
  pm_model_type                     pmic_type = PMIC_IS_INVALID;
  pm_volt_level_type                volt = 0;
  DalTimetickTime64Type             current_tick = 0;
  uint8                             num_peripherals = 0;
  uint8                             index = 0;
  uint8                             select_pin = 0;
  const char                       *pmic_model_name;
  pm_pwr_rails_diag_log_item_type      smps_log;//local variable to store the status.
  
  
  /*
   * Get number of peripherals for SMPS rail.
   */
  num_peripherals = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM,pmic_index);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(index = 0;index < num_peripherals;index++)
  {
    /*
     * Reset the status values.
     */
     mode = PM_SW_MODE_INVALID;
     sw_enable_status = PM_INVALID;
     volt = 0;
     vreg_ok_status = FALSE;
     select_pin = 0;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_pwr_rails_diag.timetick_handle, &current_tick);

    /*
     * Get status for the power rail.
     */
    pm_smps_vreg_ok_status(pmic_index,index,&vreg_ok_status);
    pm_smps_sw_enable_status(pmic_index,index,&sw_enable_status);
    pm_smps_pin_ctrl_status(pmic_index,index,&select_pin);
    pm_smps_sw_mode_status(pmic_index,index,&mode);
    pm_smps_volt_level_status(pmic_index,index,&volt);

    /*
     * Save status for the power rail in the local variable.
     */
    smps_log.timestamp = (uint64)current_tick;
    smps_log.volt_level = (uint32)volt;
    strlcpy(smps_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    smps_log.pmic_model_type = (uint8)pmic_type;
    smps_log.rail_type = (uint8)PM_DIAG_PWR_RAILS_SMPS;
    smps_log.periph_index = (index + 1);
    smps_log.enable_status = (uint8)vreg_ok_status;
    smps_log.sw_status = (uint8)sw_enable_status;
    smps_log.pin_ctrl_status = (uint8)select_pin;
    smps_log.mode_type = (uint8)mode;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*pwr_rail_log_item_ptr,&smps_log,sizeof(pm_pwr_rails_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*pwr_rail_log_item_ptr)++;
  }
} /* END pm_diag_pwr_rails_smps_status */

/* =========================================================================
**  Function : pm_diag_pwr_rails_boost_status
** =========================================================================*/
/**
  Gets the status for the BOOST and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the BOOST and fills in the diag packet memory.

  @param **pwr_rail_log_item_ptr   [out]  - Storage for Power rail's logs.
  @param   pmic_index       [in]   - PMIC device index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_pwr_rails_boost_status
(
  pm_pwr_rails_diag_log_item_type **pwr_rail_log_item_ptr,
  uint8 pmic_index
)
{
  pm_sw_mode_type                   mode = PM_SW_MODE_INVALID;
  boolean                           vreg_ok_status = FALSE;
  pm_on_off_type                    sw_enable_status = PM_INVALID;
  pm_model_type                     pmic_type = PMIC_IS_INVALID;
  pm_volt_level_type                volt = 0;
  DalTimetickTime64Type             current_tick = 0;
  uint8                             num_peripherals = 0;
  uint8                             index = 0;
  const char                       *pmic_model_name;
  pm_pwr_rails_diag_log_item_type      boost_log;//local variable to store the status.
  
  /*
   * Get number of peripherals for BOOST rail.
   */
  num_peripherals = pm_target_information_get_periph_count_info(PM_PROP_BOOST_NUM, pmic_index);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(index = 0;index < num_peripherals;index++)
  {
    /*
     * Reset the status values.
     */
     mode = PM_SW_MODE_INVALID;
     sw_enable_status = PM_INVALID;
     volt = 0;
     vreg_ok_status = FALSE;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_pwr_rails_diag.timetick_handle, &current_tick);

    /*
     * Get status for the power rail.
     */
    pm_boost_vreg_ok_status(pmic_index,index,&vreg_ok_status);
    pm_boost_sw_enable_status(pmic_index,index,&sw_enable_status);
    pm_boost_volt_level_status(pmic_index,index,&volt);

    /*
     * Save status for the power rail in the local variable.
     */
    boost_log.timestamp = (uint64)current_tick;
    boost_log.volt_level = (uint32)volt;
    strlcpy(boost_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    boost_log.pmic_model_type = (uint8)pmic_type;
    boost_log.rail_type = (uint8)PM_DIAG_PWR_RAILS_BOOST;
    boost_log.periph_index = (index + 1);
    boost_log.enable_status = (uint8)vreg_ok_status;
    boost_log.sw_status = (uint8)sw_enable_status;
    boost_log.pin_ctrl_status = 0;
    boost_log.mode_type = (uint8)mode;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*pwr_rail_log_item_ptr,&boost_log,sizeof(pm_pwr_rails_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*pwr_rail_log_item_ptr)++;
  }
} /* END pm_diag_pwr_rails_boost_status */

/* =========================================================================
**  Function : pm_diag_pwr_rails_ldo_status
** =========================================================================*/
/**
  Gets the status for the LDOs and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the LDOs and fills in the diag packet memory.

  @param **pwr_rail_log_item_ptr   [out]  - Storage for Power rail's logs.
  @param   pmic_index       [in]   - PMIC device index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_pwr_rails_ldo_status
(
  pm_pwr_rails_diag_log_item_type **pwr_rail_log_item_ptr,
  uint8 pmic_index
)
{
  pm_sw_mode_type                   mode = PM_SW_MODE_INVALID;
  boolean                           vreg_ok_status = FALSE;
  pm_on_off_type                    sw_enable_status = PM_INVALID;
  pm_model_type                     pmic_type = PMIC_IS_INVALID;
  pm_volt_level_type                volt = 0;
  DalTimetickTime64Type             current_tick = 0;
  uint8                             num_peripherals = 0;
  uint8                             index = 0;
  uint8                             select_pin = 0;
  const char                       *pmic_model_name;
  pm_pwr_rails_diag_log_item_type      ldo_log;//local variable to store the status.
  
  /*
   * Get number of peripherals for LDO rail.
   */
  num_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, pmic_index);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(index = 0;index < num_peripherals;index++)
  {
    /*
     * Reset the status values.
     */
     mode = PM_SW_MODE_INVALID;
     sw_enable_status = PM_INVALID;
     volt = 0;
     vreg_ok_status = FALSE;
     select_pin = 0;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_pwr_rails_diag.timetick_handle, &current_tick);

    /*
     * Get status for the power rail.
     */
    pm_ldo_vreg_ok_status(pmic_index,index,&vreg_ok_status);
    pm_ldo_sw_enable_status(pmic_index,index,&sw_enable_status);
    pm_ldo_sw_mode_status(pmic_index,index,&mode);
    pm_ldo_volt_level_status(pmic_index,index,&volt);
    pm_ldo_pin_ctrl_status(pmic_index,index,&select_pin);

    /*
     * Save status for the power rail in the local variable.
     */
    ldo_log.timestamp = (uint64)current_tick;
    ldo_log.volt_level = (uint32)volt;
    strlcpy(ldo_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    ldo_log.rail_type = (uint8)PM_DIAG_PWR_RAILS_LDO;
    ldo_log.periph_index = (index + 1);
    ldo_log.pmic_model_type = (uint8)pmic_type;
    ldo_log.enable_status = (uint8)vreg_ok_status;
    ldo_log.sw_status = (uint8)sw_enable_status;
    ldo_log.pin_ctrl_status = (uint8)select_pin;
    ldo_log.mode_type = (uint8)mode;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*pwr_rail_log_item_ptr,&ldo_log,sizeof(pm_pwr_rails_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*pwr_rail_log_item_ptr)++;
  }
} /* END pm_diag_pwr_rails_ldo_status */


/* =========================================================================
**  Function : pm_diag_pwr_rails_vs_status
** =========================================================================*/
/**
  Gets the status for the Voltage Switchers and appends the log to the diag
  response packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range for
  all the Voltage Switchers and fills in the diag packet memory.

  @param **pwr_rail_log_item_ptr   [out]  - Storage for Power rail's logs.
  @param   pmic_index       [in]   - PMIC device index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_pwr_rails_vs_status
(
  pm_pwr_rails_diag_log_item_type **pwr_rail_log_item_ptr,
  uint8 pmic_index
)
{
  pm_sw_mode_type                   mode = PM_SW_MODE_INVALID;
  boolean                           vreg_ok_status = FALSE;
  pm_on_off_type                    sw_enable_status = PM_INVALID;
  pm_model_type                     pmic_type = PMIC_IS_INVALID;
  pm_volt_level_type                volt = 0;
  DalTimetickTime64Type             current_tick = 0;
  uint8                             num_peripherals = 0;
  uint8                             index = 0;
  uint8                             select_pin = 0;
  const char                       *pmic_model_name;
  pm_pwr_rails_diag_log_item_type      vs_log;//local variable to store the status.
  
  /*
   * Get number of peripherals for VS rail.
   */
  num_peripherals = pm_target_information_get_periph_count_info(PM_PROP_VS_NUM, pmic_index);
  
  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(index = 0;index < num_peripherals;index++)
  {
    /*
     * Reset the status values.
     */
     mode = PM_SW_MODE_INVALID;
     sw_enable_status = PM_INVALID;
     volt = 0;
     vreg_ok_status = FALSE;
     select_pin = 0;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_pwr_rails_diag.timetick_handle, &current_tick);

    /*
     * Get status for the power rail.
     */
    pm_vs_vreg_ok_status(pmic_index,index,&vreg_ok_status);
    pm_vs_sw_enable_status(pmic_index,index,&sw_enable_status);
    pm_vs_sw_mode_status(pmic_index,index,&mode);
    pm_vs_pin_ctrl_status(pmic_index,index,&select_pin);

    /*
     * Save status for the power rail in the local variable.
     */
    vs_log.timestamp = (uint64)current_tick;
    vs_log.volt_level = (uint32)volt;
    strlcpy(vs_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    vs_log.pmic_model_type = (uint8)pmic_type;
    vs_log.rail_type = (uint8)PM_DIAG_PWR_RAILS_VS;
    vs_log.periph_index = (index + 1);
    vs_log.enable_status = (uint8)vreg_ok_status;
    vs_log.sw_status = (uint8)sw_enable_status;
    vs_log.pin_ctrl_status = (uint8)select_pin;
    vs_log.mode_type = (uint8)mode;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*pwr_rail_log_item_ptr,&vs_log,sizeof(pm_pwr_rails_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*pwr_rail_log_item_ptr)++;
  }
} /* END pm_diag_pwr_rails_vs_status */

/* =========================================================================
**  Function : pm_diag_pwr_rails_error_response
** =========================================================================*/
/**
  Packages up an error response for the QXDM tool with the requested command.

  @param command [in] -- This specifies the command which resulted in error.

  @return
  pm_diag_pwr_rails_log_resp_type* -- Response packet to Diag.

  @dependencies
  None.
*/
static pm_diag_pwr_rails_log_resp_type* pm_diag_pwr_rails_error_response
(
  pm_diag_command_type command
)
{
  pm_diag_pwr_rails_log_resp_type *error_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Try to allocate memory for error response                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr =
    (pm_diag_pwr_rails_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_PWR_RAILS_LOG_CMD_ID,
        sizeof(pm_diag_pwr_rails_log_resp_type));

  if (error_resp_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr->command_type = command;

  /*-----------------------------------------------------------------------*/
  /* Include the error status.                                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr->result_type = PM_DIAG_ERROR;

  return error_resp_ptr;
} /* END pm_diag_pwr_rails_error_response */

/* =========================================================================
**  Function : pm_diag_pwr_rails_deinit_logging
** =========================================================================*/
/**
  DeInit routine for all logging related activities.

  This function cleans up the preparation done during logging initialization.

  @param None.

  @return
  DALResult -- Result of logging deinit.

  @dependencies
  None.
*/
static DALResult pm_diag_pwr_rails_deinit_logging(void)
{
  pm_pwr_rails_diag.log_enabled_flag = FALSE;
  pm_pwr_rails_diag.num_pmic_devices = 0;
  pm_pwr_rails_diag.num_pwr_rails = 0;
  pm_pwr_rails_diag.start_resp_size = 0;

  /*
   * Detach timetick handler.
   */
  if (DAL_SUCCESS != DAL_DeviceDetach(pm_pwr_rails_diag.timetick_handle))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
} /* END pm_diag_pwr_rails_deinit_logging */

/* =========================================================================
**  Function : pm_diag_pwr_rails_init_logging
** =========================================================================*/
/**
  Init routine for all logging related activities.

  This function is invoked by the Diag log command handler to prepare the
  driver for capturing and reporting power rails status via diag resp packet.

  @param None.

  @return
  DALResult -- Result of logging setup.

  @dependencies
  None.
*/
static DALResult pm_diag_pwr_rails_init_logging(void)
{
  static boolean               log_supported_flag = TRUE;
  uint8                        pmic_index = 0;
  const char* timetick_dev_str = "SystemTimer";

  
  /*-----------------------------------------------------------------------*/
  /* Prevent multiple attempts to init if it is failing or not supported.  */
  /*-----------------------------------------------------------------------*/

  if (log_supported_flag == FALSE)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Prevent multiple attempts to init if it is already initialized.       */
  /*-----------------------------------------------------------------------*/

  if (pm_pwr_rails_diag.log_enabled_flag == TRUE)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to DAL Timetick to get the System time stamp later.            */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &pm_pwr_rails_diag.timetick_handle))
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the Number of PMIC devices on the target.                         */
  /*-----------------------------------------------------------------------*/

  pm_pwr_rails_diag.num_pmic_devices = PM_MAX_NUM_PMICS;

  /*-----------------------------------------------------------------------*/
  /* Get the number of power rails to calculate the log resp packet size.  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < pm_pwr_rails_diag.num_pmic_devices;pmic_index++)
  {
     pm_pwr_rails_diag.num_pwr_rails = 
        pm_pwr_rails_diag.num_pwr_rails + 
        pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM,pmic_index) +
        pm_target_information_get_periph_count_info(PM_PROP_BOOST_NUM,pmic_index) +
        pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM,pmic_index) +
        pm_target_information_get_periph_count_info(PM_PROP_VS_NUM, pmic_index);
  }

  /*-----------------------------------------------------------------------*/
  /* If there are no power rails we will not support logging.              */
  /*-----------------------------------------------------------------------*/

  if (pm_pwr_rails_diag.num_pwr_rails == 0)
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    if (DAL_SUCCESS != pm_diag_pwr_rails_deinit_logging())
    {
      return DAL_ERROR;
    }

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Compute Diag start log response packet size.                          */
  /*-----------------------------------------------------------------------*/
  pm_pwr_rails_diag.start_resp_size = 
    sizeof(pm_diag_pwr_rails_log_resp_type) + (sizeof(pm_pwr_rails_diag_log_item_type) * pm_pwr_rails_diag.num_pwr_rails);

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : pm_diag_pwr_rails_log_start
** =========================================================================*/
/**
  Enables power rails status logging functionality.

  This function starts the logging of power rails status and sends back
  a Diag packet containing the status of the power rails and their
  respective string names including the corresponding PMIC name.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_pwr_rails_log_start(void)
{
  pm_diag_pwr_rails_log_resp_type *start_resp_ptr = NULL;
  pm_pwr_rails_diag_log_item_type *pwr_rails_log_resp_ptr = NULL;
  char                         *diag_buf_ptr = NULL;
  uint8                         pmic_index = 0;

  /*-----------------------------------------------------------------------*/
  /* Prepare the logging system.                                           */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != pm_diag_pwr_rails_init_logging())
  {
    return pm_diag_pwr_rails_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  diag_buf_ptr =
    diagpkt_subsys_alloc(
      DIAG_SUBSYS_PM,
      PM_DIAG_PWR_RAILS_LOG_CMD_ID,
      pm_pwr_rails_diag.start_resp_size);
  if (diag_buf_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Locate fields in newly allocated Diag buffer.                         */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr = (pm_diag_pwr_rails_log_resp_type *)diag_buf_ptr;
  pwr_rails_log_resp_ptr = 
    (pm_pwr_rails_diag_log_item_type *)( diag_buf_ptr + sizeof(pm_diag_pwr_rails_log_resp_type) );

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->command_type = PM_DIAG_LOGGING_START;

  /*-----------------------------------------------------------------------*/
  /* Populate the diag response packet with the power rails array size.    */
  /* This is needed so that QXDM knows how many power rails names to pick  */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->num_pwr_rails = pm_pwr_rails_diag.num_pwr_rails;

  /*-----------------------------------------------------------------------*/
  /* Send the buffer length used for the power rails names.                */
  /* This is needed so that QXDM knows how many bytes/bits are in each of  */
  /* the power rails name array fields.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->pmic_name_len = PM_DIAG_PMIC_NAME_LEN;

  /*-----------------------------------------------------------------------*/
  /* Fill in start response with power rails status data.                  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < pm_pwr_rails_diag.num_pmic_devices;pmic_index++)
  {
    pm_diag_pwr_rails_smps_status(&pwr_rails_log_resp_ptr,pmic_index);
    pm_diag_pwr_rails_boost_status(&pwr_rails_log_resp_ptr,pmic_index);
    pm_diag_pwr_rails_ldo_status(&pwr_rails_log_resp_ptr,pmic_index);
    pm_diag_pwr_rails_vs_status(&pwr_rails_log_resp_ptr,pmic_index);
  }

  /*-----------------------------------------------------------------------*/
  /* The callback setup was successful.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->result_type = PM_DIAG_SUCCESS;
  pm_pwr_rails_diag.log_enabled_flag = TRUE;

  return start_resp_ptr;

} /* END pm_diag_pwr_rails_log_start */


/* =========================================================================
**  Function : pm_diag_pwr_rails_log_stop
** =========================================================================*/
/**
  Disables power rails status logging functionality.

  This function stops the logging of power rails status and sends back a Diag packet
  with the result.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_pwr_rails_log_stop(void)
{
  pm_diag_pwr_rails_log_resp_type *stop_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr =
    (pm_diag_pwr_rails_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_PWR_RAILS_LOG_CMD_ID,
        sizeof(pm_diag_pwr_rails_log_resp_type));

  if (stop_resp_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr->command_type = PM_DIAG_LOGGING_STOP;

  /*-----------------------------------------------------------------------*/
  /* Return success is power rails logging is already stopped.             */
  /*-----------------------------------------------------------------------*/

  if (pm_pwr_rails_diag.log_enabled_flag == FALSE)
  {
    stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
  }
  else
  {
    /*
     * Deinitialize logging.
     */
    if (DAL_SUCCESS != pm_diag_pwr_rails_deinit_logging())
    {
      stop_resp_ptr->result_type = PM_DIAG_ERROR;
    }
    else
    {
      stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
    }
  }

  return (void *)stop_resp_ptr;

} /* END pm_diag_pwr_rails_log_stop */

/* =========================================================================
**  Function : pm_diag_pwr_rails_log_handler
** =========================================================================*/
/**
  Handler to start/stop power rails status logging.

  This function is invoked by the Diag command dispatcher and handles the
  request sent from the test tool.  The response packet is sent back to the
  Diag command dispatcher for communicating back to the test tool.

  @param *req_ptr     [in] -- Pointer to Diag command packet.
  @param  req_len     [in] -- Size of the Diag command packet.

  @return
  void* -- Diag response packet.

  @dependencies
  None.
*/
void * pm_diag_pwr_rails_log_handler
(
  void  *req_ptr,   /* pointer to request packet */
  uint16 req_len /* length of request packet  */
)
{
  void                        *resp_ptr;
  pm_diag_pwr_rails_log_req_type *pwr_rail_req_ptr = (pm_diag_pwr_rails_log_req_type*)req_ptr;

  /*-----------------------------------------------------------------------*/
  /* Validate the incoming packet is as expected.                          */
  /*-----------------------------------------------------------------------*/

  if (pwr_rail_req_ptr == NULL)
  {
    return NULL;
  }

  if (req_len != sizeof(pm_diag_pwr_rails_log_req_type))
  {
    resp_ptr = diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, sizeof(pm_diag_pwr_rails_log_req_type));

    return (void *)resp_ptr;
  }

  /*-----------------------------------------------------------------------*/
  /* Start or stop the power rails status logging.                         */
  /*-----------------------------------------------------------------------*/

  if (pwr_rail_req_ptr->status_flag)
  {
    resp_ptr = pm_diag_pwr_rails_log_start();
  }
  else
  {
    resp_ptr = pm_diag_pwr_rails_log_stop();
  }

  /*-----------------------------------------------------------------------*/
  /* Response packet is transmitted by dispatcher upon return.             */
  /*-----------------------------------------------------------------------*/

  return (void *)resp_ptr;

} /* END pm_diag_pwr_rails_log_handler */


/* =========================================================================
**  Function : pm_diag_pwr_rails_init
** =========================================================================*/
/**
  Initialization function for the PMIC Power Rails Diag functionality.

  @param
  None.

  @return
  None.
 
  @dependencies
  None.
*/
void pm_diag_pwr_rails_init (void)
{
  pm_pwr_rails_diag.log_enabled_flag = FALSE;
  pm_pwr_rails_diag.num_pmic_devices = 0;
  pm_pwr_rails_diag.num_pwr_rails = 0;
  pm_pwr_rails_diag.start_resp_size = 0;
  pm_pwr_rails_diag.timetick_handle = NULL;

  /*-----------------------------------------------------------------------*/
  /* Registers the power rails diag support with the dispatcher.                 */
  /*-----------------------------------------------------------------------*/

  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PM, pm_pwr_rails_diag_table);

} /* END pm_diag_pwr_rails_init */

