/*! \file pm_diag_gpios.c
*
*  \brief Diag handlers to get PMIC gpios status.
*  \n
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag_gpios.c#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------
07/22/15   ps      Fixed compiler warning (CR-878990)
10/28/14   kt      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_gpio_driver.h"
#include "pm_diag.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/*
 * Diag command packet to start/stop gpios status logging.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      status_flag;
} pm_diag_gpios_log_req_type;


/*
 * Diag response packet sent back to the test tool.
 *
 * The status of the gpios log is appended to
 * this structure in memory.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      command_type;
  uint8                      result_type;
  uint8                      num_gpios;
  uint8                      pmic_name_len;
} pm_diag_gpios_log_resp_type;

/*
 * This is a callback function which gets executed upon the reception and
 * routing of a Diag subsystem command/request packet to our driver.
 */
static void * pm_diag_gpios_log_handler(void *req_ptr, uint16 req_len);

/*=========================================================================
      Data
==========================================================================*/

static const diagpkt_user_table_entry_type pm_gpios_diag_table[] =
{
  {PM_DIAG_GPIOS_LOG_CMD_ID, PM_DIAG_GPIOS_LOG_CMD_ID, pm_diag_gpios_log_handler}
};

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  boolean                    log_enabled_flag;      /**< state variable */
  uint32                     start_resp_size;       /**< size of packet in response to start cmd */
  DalDeviceHandle           *timetick_handle;       /**< handle for getting system time stamp */
  uint8                      num_gpios;            /**< # of gpios being logged */
} pm_gpios_diag;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : pm_diag_gpios_status
** =========================================================================*/
/**
  Gets the status for the GPIOs and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the SMPS and fills in the diag packet memory.

  @param **gpio_log_item_ptr [out]  - Storage for GPIO logs.
  @param   pmic_index       [in]   - PMIC index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_gpios_status
(
  pm_gpios_diag_log_item_type **gpio_log_item_ptr,
  uint8 pmic_index
)
{
  pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
  pm_gpio_data_type                *gpio_data_ptr = NULL;
  pm_gpio_status_type              gpio_status;
  DalTimetickTime64Type            current_tick = 0;
  pm_gpios_diag_log_item_type      gpio_log; // local variable to store the status.
  pm_model_type                    pmic_type = PMIC_IS_INVALID;
  const char                       *pmic_model_name;
  pm_register_address_type         base_address = 0;
  pm_register_address_type         status_reg = 0;
  pm_on_off_type                   gpio_ok = 0;
  uint8                            status_reg_data = 0;
  uint8                            gpio_val = 0;
  uint8                            num_gpio = 0;
  uint8                            gpio_index = 0;

  gpio_data_ptr = pm_gpio_get_data(pmic_index);

  if ((gpio_data_ptr == NULL) || (gpio_data_ptr->gpio_register == NULL))
  {
    return;
  }
  
  /*
   * Get max number of peripherals for GPIOs.
   */
  num_gpio = pm_gpio_get_num_peripherals(pmic_index);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(gpio_index = 0;gpio_index < num_gpio;gpio_index++)
  {
    /*
     * Reset the status values.
     */
    status_reg_data = 0;
    gpio_ok = PM_INVALID;
    gpio_val = 0;
    gpio_status.gpio_ext_pin_config = PM_GPIO_EXT_PIN_CONFIG_TYPE__INVALID;
    gpio_status.gpio_mode_select = PM_GPIO_MODE_INVALID;
    gpio_status.gpio_src_config = PM_GPIO_SOURCE_CONFIG_TYPE__INVALID;
    gpio_status.gpio_invert_ext_pin = PM_GPIO_INVERT_EXT_PIN_OUTPUT__INVALID;
    gpio_status.gpio_volt_src = PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID;
    gpio_status.gpio_out_buffer_config = PM_GPIO_OUT_BUFFER_CONFIG__INVALID;
    gpio_status.gpio_out_buffer_drv_strength = PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID;
    gpio_status.gpio_current_src_pulls = PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID;
    gpio_status.gpio_dtest_buffer_on_off = PM_GPIO_DTEST_BUFFER_ON_OFF_TYPE__INVALID;
    err_flag = PM_ERR_FLAG__SUCCESS;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_gpios_diag.timetick_handle, &current_tick);

    /*
     * Get status for the gpio.
     */

    base_address = gpio_data_ptr->gpio_register->base_address + 
                   (pm_register_address_type)(gpio_data_ptr->gpio_register->peripheral_offset*gpio_index);

    status_reg =  base_address + gpio_data_ptr->gpio_register->status;
    
    err_flag = pm_comm_read_byte(gpio_data_ptr->comm_ptr->slave_id, status_reg, &status_reg_data, 0 );

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      gpio_ok = (status_reg_data & 0x80) ? PM_ON : PM_OFF;
      gpio_val = (status_reg_data & 0x01);
    }

    err_flag = pm_dev_gpio_status_get(pmic_index, (pm_gpio_perph_index)gpio_index, &gpio_status);

    /*
     * Save status for the gpio in the local variable.
     */
    gpio_log.timestamp = (uint64)current_tick;
    strlcpy(gpio_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    gpio_log.periph_index = (gpio_index + 1);
    gpio_log.gpio_ok_status = (uint8)gpio_ok;
    gpio_log.sw_enable_status = (uint8)gpio_status.gpio_ext_pin_config;
    gpio_log.mode_select = (uint8)gpio_status.gpio_mode_select;
    gpio_log.output_select = (uint8)gpio_status.gpio_src_config;
    gpio_log.output_select_inverted = (uint8)gpio_status.gpio_invert_ext_pin;
    gpio_log.volt_source = (uint8)gpio_status.gpio_volt_src;
    gpio_log.pull_type = (uint8)gpio_status.gpio_current_src_pulls;
    gpio_log.output_drv = (uint8)gpio_status.gpio_out_buffer_drv_strength;
    gpio_log.output_buffer = (uint8)gpio_status.gpio_out_buffer_config;
    gpio_log.dtest_buffer = (uint8)gpio_status.gpio_dtest_buffer_on_off;
    gpio_log.input_value = gpio_val;
    gpio_log.status = (uint8)err_flag;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*gpio_log_item_ptr,&gpio_log,sizeof(pm_gpios_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*gpio_log_item_ptr)++;
  }
} /* END pm_diag_gpios_status */

/* =========================================================================
**  Function : pm_diag_gpios_error_response
** =========================================================================*/
/**
  Packages up an error response for the QXDM tool with the requested command.

  @param command [in] -- This specifies the command which resulted in error.

  @return
  pm_diag_gpios_log_resp_type* -- Response packet to Diag.

  @dependencies
  None.
*/
static pm_diag_gpios_log_resp_type* pm_diag_gpios_error_response
(
  pm_diag_command_type command
)
{
  pm_diag_gpios_log_resp_type *error_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Try to allocate memory for error response                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr =
    (pm_diag_gpios_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_GPIOS_LOG_CMD_ID,
        sizeof(pm_diag_gpios_log_resp_type));

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
} /* END pm_diag_gpios_error_response */

/* =========================================================================
**  Function : pm_diag_gpios_deinit_logging
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
static DALResult pm_diag_gpios_deinit_logging(void)
{
  pm_gpios_diag.log_enabled_flag = FALSE;
  pm_gpios_diag.num_gpios = 0;
  pm_gpios_diag.start_resp_size = 0;

  /*
   * Detach timetick handler.
   */
  if (DAL_SUCCESS != DAL_DeviceDetach(pm_gpios_diag.timetick_handle))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
} /* END pm_diag_gpios_deinit_logging */

/* =========================================================================
**  Function : pm_diag_gpios_init_logging
** =========================================================================*/
/**
  Init routine for all logging related activities.

  This function is invoked by the Diag log command handler to prepare the
  driver for capturing and reporting gpios status via diag resp packet.

  @param None.

  @return
  DALResult -- Result of logging setup.

  @dependencies
  None.
*/
static DALResult pm_diag_gpios_init_logging(void)
{
  static boolean log_supported_flag = TRUE;
  const char* timetick_dev_str = "SystemTimer";
  pm_gpio_data_type *gpio_data_ptr = NULL;
  uint8 pmic_index = 0;
  
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

  if (pm_gpios_diag.log_enabled_flag == TRUE)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to DAL Timetick to get the System time stamp later.            */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &pm_gpios_diag.timetick_handle))
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the number of gpios to calculate the log resp packet size.  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    gpio_data_ptr = pm_gpio_get_data(pmic_index);

    if (gpio_data_ptr != NULL)
    {
      pm_gpios_diag.num_gpios = pm_gpios_diag.num_gpios + pm_gpio_get_num_peripherals(pmic_index);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* If there are no gpios we will not support logging.              */
  /*-----------------------------------------------------------------------*/

  if (pm_gpios_diag.num_gpios == 0)
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    if (DAL_SUCCESS != pm_diag_gpios_deinit_logging())
    {
      return DAL_ERROR;
    }

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Compute Diag start log response packet size.                          */
  /*-----------------------------------------------------------------------*/
  pm_gpios_diag.start_resp_size = 
    sizeof(pm_diag_gpios_log_resp_type) + (sizeof(pm_gpios_diag_log_item_type) * pm_gpios_diag.num_gpios);

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : pm_diag_gpios_log_start
** =========================================================================*/
/**
  Enables gpios status logging functionality.

  This function starts the logging of gpios status and sends back
  a Diag packet containing the status of the gpios and their
  respective string names including the corresponding PMIC name.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_gpios_log_start(void)
{
  pm_diag_gpios_log_resp_type *start_resp_ptr = NULL;
  pm_gpios_diag_log_item_type *gpios_log_resp_ptr = NULL;
  char                        *diag_buf_ptr = NULL;
  uint8                       pmic_index = 0;

  /*-----------------------------------------------------------------------*/
  /* Prepare the logging system.                                           */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != pm_diag_gpios_init_logging())
  {
    return pm_diag_gpios_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  diag_buf_ptr = diagpkt_subsys_alloc(DIAG_SUBSYS_PM, PM_DIAG_GPIOS_LOG_CMD_ID, 
                                      pm_gpios_diag.start_resp_size);
  if (diag_buf_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Locate fields in newly allocated Diag buffer.                         */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr = (pm_diag_gpios_log_resp_type *)diag_buf_ptr;
  gpios_log_resp_ptr = 
    (pm_gpios_diag_log_item_type *)( diag_buf_ptr + sizeof(pm_diag_gpios_log_resp_type) );

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->command_type = PM_DIAG_LOGGING_START;

  /*-----------------------------------------------------------------------*/
  /* Populate the diag response packet with the gpios array size.    */
  /* This is needed so that QXDM knows how many gpios names to pick  */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->num_gpios = pm_gpios_diag.num_gpios;

  /*-----------------------------------------------------------------------*/
  /* Send the buffer length used for the gpios names.                */
  /* This is needed so that QXDM knows how many bytes/bits are in each of  */
  /* the gpios name array fields.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->pmic_name_len = PM_DIAG_PMIC_NAME_LEN;

  /*-----------------------------------------------------------------------*/
  /* Fill in start response with gpios status data.                  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    pm_diag_gpios_status(&gpios_log_resp_ptr,pmic_index);
  }

  /*-----------------------------------------------------------------------*/
  /* The callback setup was successful.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->result_type = PM_DIAG_SUCCESS;
  pm_gpios_diag.log_enabled_flag = TRUE;

  return start_resp_ptr;

} /* END pm_diag_gpios_log_start */


/* =========================================================================
**  Function : pm_diag_gpios_log_stop
** =========================================================================*/
/**
  Disables gpios status logging functionality.

  This function stops the logging of gpios status and sends back a Diag packet
  with the result.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_gpios_log_stop(void)
{
  pm_diag_gpios_log_resp_type *stop_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr =
    (pm_diag_gpios_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_GPIOS_LOG_CMD_ID,
        sizeof(pm_diag_gpios_log_resp_type));

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
  /* Return success is gpios logging is already stopped.             */
  /*-----------------------------------------------------------------------*/

  if (pm_gpios_diag.log_enabled_flag == FALSE)
  {
    stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
  }
  else
  {
    /*
     * Deinitialize logging.
     */
    if (DAL_SUCCESS != pm_diag_gpios_deinit_logging())
    {
      stop_resp_ptr->result_type = PM_DIAG_ERROR;
    }
    else
    {
      stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
    }
  }

  return (void *)stop_resp_ptr;

} /* END pm_diag_gpios_log_stop */

/* =========================================================================
**  Function : pm_diag_gpios_log_handler
** =========================================================================*/
/**
  Handler to start/stop gpios status logging.

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
void * pm_diag_gpios_log_handler
(
  void  *req_ptr,   /* pointer to request packet */
  uint16 req_len /* length of request packet  */
)
{
  void                       *resp_ptr;
  pm_diag_gpios_log_req_type *gpio_req_ptr = (pm_diag_gpios_log_req_type*)req_ptr;

  /*-----------------------------------------------------------------------*/
  /* Validate the incoming packet is as expected.                          */
  /*-----------------------------------------------------------------------*/

  if (gpio_req_ptr == NULL)
  {
    return NULL;
  }

  if (req_len != sizeof(pm_diag_gpios_log_req_type))
  {
    resp_ptr = diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, sizeof(pm_diag_gpios_log_req_type));

    return (void *)resp_ptr;
  }

  /*-----------------------------------------------------------------------*/
  /* Start or stop the gpios status logging.                         */
  /*-----------------------------------------------------------------------*/

  if (gpio_req_ptr->status_flag)
  {
    resp_ptr = pm_diag_gpios_log_start();
  }
  else
  {
    resp_ptr = pm_diag_gpios_log_stop();
  }

  /*-----------------------------------------------------------------------*/
  /* Response packet is transmitted by dispatcher upon return.             */
  /*-----------------------------------------------------------------------*/

  return (void *)resp_ptr;

} /* END pm_diag_gpios_log_handler */


/* =========================================================================
**  Function : pm_diag_gpios_init
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
void pm_diag_gpios_init (void)
{
  pm_gpios_diag.log_enabled_flag = FALSE;
  pm_gpios_diag.num_gpios = 0;
  pm_gpios_diag.start_resp_size = 0;
  pm_gpios_diag.timetick_handle = NULL;

  /*-----------------------------------------------------------------------*/
  /* Registers the gpios diag support with the dispatcher.                 */
  /*-----------------------------------------------------------------------*/

  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PM, pm_gpios_diag_table);

} /* END pm_diag_gpios_init */

