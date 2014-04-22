/*! \file pm_diag_mpps.c
*
*  \brief Diag handlers to get PMIC MPPs status.
*  \n
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag_mpps.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------
07/22/15   ps      Fixed compiler warning (CR-878990)
10/28/14   kt      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_mpp_driver.h"
#include "pm_diag.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/*
 * Diag command packet to start/stop mpps status logging.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      status_flag;
} pm_diag_mpps_log_req_type;


/*
 * Diag response packet sent back to the test tool.
 *
 * The status of the mpps log is appended to
 * this structure in memory.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      command_type;
  uint8                      result_type;
  uint8                      num_mpps;
  uint8                      pmic_name_len;
} pm_diag_mpps_log_resp_type;

/*
 * This is a callback function which gets executed upon the reception and
 * routing of a Diag subsystem command/request packet to our driver.
 */
static void * pm_diag_mpps_log_handler(void *req_ptr, uint16 req_len);

/*=========================================================================
      Data
==========================================================================*/

static const diagpkt_user_table_entry_type pm_mpps_diag_table[] =
{
  {PM_DIAG_MPPS_LOG_CMD_ID, PM_DIAG_MPPS_LOG_CMD_ID, pm_diag_mpps_log_handler}
};

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  boolean                    log_enabled_flag;      /**< state variable */
  uint32                     start_resp_size;       /**< size of packet in response to start cmd */
  DalDeviceHandle           *timetick_handle;       /**< handle for getting system time stamp */
  uint8                      num_mpps;              /**< # of mpps being logged */
} pm_mpps_diag;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : pm_diag_mpps_status
** =========================================================================*/
/**
  Gets the status for the MPPs and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the SMPS and fills in the diag packet memory.

  @param **mpp_log_item_ptr [out]  - Storage for MPP logs.
  @param   pmic_index       [in]   - PMIC index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_mpps_status
(
  pm_mpps_diag_log_item_type **mpp_log_item_ptr,
  uint8 pmic_index
)
{
  pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
  pm_mpp_data_type                 *mpp_data_ptr = NULL;
  pm_mpp_status_type               mpp_status;
  DalTimetickTime64Type            current_tick = 0;
  pm_mpps_diag_log_item_type       mpp_log; // local variable to store the status.
  pm_model_type                    pmic_type = PMIC_IS_INVALID;
  const char                       *pmic_model_name;
  pm_register_address_type         base_address = 0;
  pm_register_address_type         status_reg = 0;
  pm_register_address_type         en_ctl_reg = 0;
  pm_on_off_type                   mpp_ok = PM_INVALID;
  pm_on_off_type                   mpp_en = PM_INVALID;
  uint8                            status_reg_data = 0;
  uint8                            en_reg_data = 0;
  uint8                            mpp_val = 0;
  uint8                            num_mpp = 0;
  uint8                            mpp_index = 0;

  mpp_data_ptr = pm_mpp_get_data(pmic_index);

  if ((mpp_data_ptr == NULL) || (mpp_data_ptr->mpp_register == NULL))
  {
    return;
  }
  
  /*
   * Get max number of peripherals for MPPs.
   */
  num_mpp = pm_mpp_get_num_peripherals(pmic_index);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(mpp_index = 0;mpp_index < num_mpp;mpp_index++)
  {
    /*
     * Reset the status values.
     */
    status_reg_data = 0;
    mpp_ok = PM_INVALID;
    mpp_en = PM_INVALID;
    mpp_val = 0;
    mpp_status.mpp_config = PM_MPP_TYPE_INVALID;
    err_flag = PM_ERR_FLAG__SUCCESS;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_mpps_diag.timetick_handle, &current_tick);

    /*
     * Get status for the mpp.
     */

    base_address = mpp_data_ptr->mpp_register->base_address + 
                   (pm_register_address_type)(mpp_data_ptr->mpp_register->peripheral_offset*mpp_index);

    status_reg =  base_address + mpp_data_ptr->mpp_register->status;
    
    err_flag = pm_comm_read_byte(mpp_data_ptr->comm_ptr->slave_id, status_reg, &status_reg_data, 0 );

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      mpp_ok = (status_reg_data & 0x80) ? PM_ON : PM_OFF;
      mpp_val = (status_reg_data & 0x01);
    }

    en_ctl_reg =  base_address + mpp_data_ptr->mpp_register->en_ctl;
    
    err_flag = pm_comm_read_byte(mpp_data_ptr->comm_ptr->slave_id, en_ctl_reg, &en_reg_data, 0 );

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      mpp_en = (en_reg_data & 0x80) ? PM_ON : PM_OFF;
    }

    err_flag = pm_dev_mpp_status_get(pmic_index, (pm_mpp_which_type)mpp_index, &mpp_status);

    /*
     * Save status for the mpp in the local variable.
     */
    mpp_log.timestamp = (uint64)current_tick;
    strlcpy(mpp_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    mpp_log.periph_index = (mpp_index + 1);
    mpp_log.mpp_ok_status = (uint8)mpp_ok;
    mpp_log.sw_enable_status = (uint8)mpp_en;
    mpp_log.mode_select = (uint8)mpp_status.mpp_config;
    mpp_log.input_value = mpp_val;
    mpp_log.status = (uint8)err_flag;

    switch (mpp_status.mpp_config)
    {
    case PM_MPP_DIG_IN:
      mpp_log.config_select = (uint8)mpp_status.mpp_config_settings.mpp_digital_input_status.mpp_dlogic_in_dbus;
      mpp_log.level_select = (uint8)mpp_status.mpp_config_settings.mpp_digital_input_status.mpp_dlogic_lvl;
      break;
    case PM_MPP_DIG_OUT:
      mpp_log.config_select = (uint8)mpp_status.mpp_config_settings.mpp_digital_output_status.mpp_dlogic_out_ctrl;
      mpp_log.level_select = (uint8)mpp_status.mpp_config_settings.mpp_digital_output_status.mpp_dlogic_lvl;
      break;
    case PM_MPP_BI_DIR:
      mpp_log.config_select = (uint8)mpp_status.mpp_config_settings.mpp_bidirectional_status.mpp_dlogic_inout_pup;
      mpp_log.level_select = (uint8)mpp_status.mpp_config_settings.mpp_bidirectional_status.mpp_dlogic_lvl;
      break;
    case PM_MPP_ANALOG_IN:
      mpp_log.config_select = (uint8)mpp_status.mpp_config_settings.mpp_analog_input_status.mpp_ain_ch;
      mpp_log.level_select = 0;
      break;
    case PM_MPP_ANALOG_OUT:
      mpp_log.config_select = (uint8)mpp_status.mpp_config_settings.mpp_analog_output_status.mpp_aout_switch;
      mpp_log.level_select = (uint8)mpp_status.mpp_config_settings.mpp_analog_output_status.mpp_aout_level;
      break;
    case PM_MPP_I_SINK:
      mpp_log.config_select = (uint8)mpp_status.mpp_config_settings.mpp_current_sink_status.mpp_i_sink_switch;
      mpp_log.level_select = (uint8)mpp_status.mpp_config_settings.mpp_current_sink_status.mpp_i_sink_level;
      break;
    default:
      mpp_log.config_select = 0;
      mpp_log.level_select = 0;
      break;
        
    }

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*mpp_log_item_ptr,&mpp_log,sizeof(pm_mpps_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*mpp_log_item_ptr)++;
  }
} /* END pm_diag_mpps_status */

/* =========================================================================
**  Function : pm_diag_mpps_error_response
** =========================================================================*/
/**
  Packages up an error response for the QXDM tool with the requested command.

  @param command [in] -- This specifies the command which resulted in error.

  @return
  pm_diag_mpps_log_resp_type* -- Response packet to Diag.

  @dependencies
  None.
*/
static pm_diag_mpps_log_resp_type* pm_diag_mpps_error_response
(
  pm_diag_command_type command
)
{
  pm_diag_mpps_log_resp_type *error_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Try to allocate memory for error response                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr =
    (pm_diag_mpps_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_MPPS_LOG_CMD_ID,
        sizeof(pm_diag_mpps_log_resp_type));

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
} /* END pm_diag_mpps_error_response */

/* =========================================================================
**  Function : pm_diag_mpps_deinit_logging
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
static DALResult pm_diag_mpps_deinit_logging(void)
{
  pm_mpps_diag.log_enabled_flag = FALSE;
  pm_mpps_diag.num_mpps = 0;
  pm_mpps_diag.start_resp_size = 0;

  /*
   * Detach timetick handler.
   */
  if (DAL_SUCCESS != DAL_DeviceDetach(pm_mpps_diag.timetick_handle))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
} /* END pm_diag_mpps_deinit_logging */

/* =========================================================================
**  Function : pm_diag_mpps_init_logging
** =========================================================================*/
/**
  Init routine for all logging related activities.

  This function is invoked by the Diag log command handler to prepare the
  driver for capturing and reporting mpps status via diag resp packet.

  @param None.

  @return
  DALResult -- Result of logging setup.

  @dependencies
  None.
*/
static DALResult pm_diag_mpps_init_logging(void)
{
  static boolean log_supported_flag = TRUE;
  const char* timetick_dev_str = "SystemTimer";
  pm_mpp_data_type *mpp_data_ptr = NULL;
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

  if (pm_mpps_diag.log_enabled_flag == TRUE)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to DAL Timetick to get the System time stamp later.            */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &pm_mpps_diag.timetick_handle))
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the number of mpps to calculate the log resp packet size.  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    mpp_data_ptr = pm_mpp_get_data(pmic_index);

    if (mpp_data_ptr != NULL)
    {
      pm_mpps_diag.num_mpps = pm_mpps_diag.num_mpps + pm_mpp_get_num_peripherals(pmic_index);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* If there are no mpps we will not support logging.              */
  /*-----------------------------------------------------------------------*/

  if (pm_mpps_diag.num_mpps == 0)
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    if (DAL_SUCCESS != pm_diag_mpps_deinit_logging())
    {
      return DAL_ERROR;
    }

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Compute Diag start log response packet size.                          */
  /*-----------------------------------------------------------------------*/
  pm_mpps_diag.start_resp_size = 
    sizeof(pm_diag_mpps_log_resp_type) + (sizeof(pm_mpps_diag_log_item_type) * pm_mpps_diag.num_mpps);

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : pm_diag_mpps_log_start
** =========================================================================*/
/**
  Enables mpps status logging functionality.

  This function starts the logging of mpps status and sends back
  a Diag packet containing the status of the mpps and their
  respective string names including the corresponding PMIC name.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_mpps_log_start(void)
{
  pm_diag_mpps_log_resp_type *start_resp_ptr = NULL;
  pm_mpps_diag_log_item_type *mpps_log_resp_ptr = NULL;
  char                       *diag_buf_ptr = NULL;
  uint8                      pmic_index = 0;

  /*-----------------------------------------------------------------------*/
  /* Prepare the logging system.                                           */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != pm_diag_mpps_init_logging())
  {
    return pm_diag_mpps_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  diag_buf_ptr = diagpkt_subsys_alloc(DIAG_SUBSYS_PM, PM_DIAG_MPPS_LOG_CMD_ID, 
                                      pm_mpps_diag.start_resp_size);
  if (diag_buf_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Locate fields in newly allocated Diag buffer.                         */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr = (pm_diag_mpps_log_resp_type *)diag_buf_ptr;
  mpps_log_resp_ptr = 
    (pm_mpps_diag_log_item_type *)( diag_buf_ptr + sizeof(pm_diag_mpps_log_resp_type) );

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->command_type = PM_DIAG_LOGGING_START;

  /*-----------------------------------------------------------------------*/
  /* Populate the diag response packet with the mpps array size.    */
  /* This is needed so that QXDM knows how many mpps names to pick  */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->num_mpps = pm_mpps_diag.num_mpps;

  /*-----------------------------------------------------------------------*/
  /* Send the buffer length used for the mpps names.                */
  /* This is needed so that QXDM knows how many bytes/bits are in each of  */
  /* the mpps name array fields.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->pmic_name_len = PM_DIAG_PMIC_NAME_LEN;

  /*-----------------------------------------------------------------------*/
  /* Fill in start response with mpps status data.                  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    pm_diag_mpps_status(&mpps_log_resp_ptr,pmic_index);
  }

  /*-----------------------------------------------------------------------*/
  /* The callback setup was successful.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->result_type = PM_DIAG_SUCCESS;
  pm_mpps_diag.log_enabled_flag = TRUE;

  return start_resp_ptr;

} /* END pm_diag_mpps_log_start */


/* =========================================================================
**  Function : pm_diag_mpps_log_stop
** =========================================================================*/
/**
  Disables mpps status logging functionality.

  This function stops the logging of mpps status and sends back a Diag packet
  with the result.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_mpps_log_stop(void)
{
  pm_diag_mpps_log_resp_type *stop_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr =
    (pm_diag_mpps_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_MPPS_LOG_CMD_ID,
        sizeof(pm_diag_mpps_log_resp_type));

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
  /* Return success is mpps logging is already stopped.             */
  /*-----------------------------------------------------------------------*/

  if (pm_mpps_diag.log_enabled_flag == FALSE)
  {
    stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
  }
  else
  {
    /*
     * Deinitialize logging.
     */
    if (DAL_SUCCESS != pm_diag_mpps_deinit_logging())
    {
      stop_resp_ptr->result_type = PM_DIAG_ERROR;
    }
    else
    {
      stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
    }
  }

  return (void *)stop_resp_ptr;

} /* END pm_diag_mpps_log_stop */

/* =========================================================================
**  Function : pm_diag_mpps_log_handler
** =========================================================================*/
/**
  Handler to start/stop mpps status logging.

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
void * pm_diag_mpps_log_handler
(
  void  *req_ptr, /* pointer to request packet */
  uint16 req_len /* length of request packet  */
)
{
  void                      *resp_ptr;
  pm_diag_mpps_log_req_type *mpp_req_ptr = (pm_diag_mpps_log_req_type*)req_ptr;

  /*-----------------------------------------------------------------------*/
  /* Validate the incoming packet is as expected.                          */
  /*-----------------------------------------------------------------------*/

  if (mpp_req_ptr == NULL)
  {
    return NULL;
  }

  if (req_len != sizeof(pm_diag_mpps_log_req_type))
  {
    resp_ptr = diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, sizeof(pm_diag_mpps_log_req_type));

    return (void *)resp_ptr;
  }

  /*-----------------------------------------------------------------------*/
  /* Start or stop the mpps status logging.                         */
  /*-----------------------------------------------------------------------*/

  if (mpp_req_ptr->status_flag)
  {
    resp_ptr = pm_diag_mpps_log_start();
  }
  else
  {
    resp_ptr = pm_diag_mpps_log_stop();
  }

  /*-----------------------------------------------------------------------*/
  /* Response packet is transmitted by dispatcher upon return.             */
  /*-----------------------------------------------------------------------*/

  return (void *)resp_ptr;

} /* END pm_diag_mpps_log_handler */


/* =========================================================================
**  Function : pm_diag_mpps_init
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
void pm_diag_mpps_init (void)
{
  pm_mpps_diag.log_enabled_flag = FALSE;
  pm_mpps_diag.num_mpps = 0;
  pm_mpps_diag.start_resp_size = 0;
  pm_mpps_diag.timetick_handle = NULL;

  /*-----------------------------------------------------------------------*/
  /* Registers the mpps diag support with the dispatcher.                 */
  /*-----------------------------------------------------------------------*/

  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PM, pm_mpps_diag_table);

} /* END pm_diag_mpps_init */

