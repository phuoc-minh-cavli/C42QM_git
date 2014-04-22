/*! \file pm_diag_clocks.c
*
*  \brief Diag handlers to get PMIC clocks status.
*  \n
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag_clocks.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------
10/28/14   kt      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_clk_driver.h"
#include "pm_diag.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/*
 * Clock types used in QXDM/Diag communications.
 */
typedef enum
{
  PM_DIAG_CLOCK_UNKNOWN,
  PM_DIAG_CLOCK_XO,
  PM_DIAG_CLOCK_DIST,
  PM_DIAG_CLOCK_BB_1,
  PM_DIAG_CLOCK_BB_2,
  PM_DIAG_CLOCK_LN_BB,
  PM_DIAG_CLOCK_RF_1,
  PM_DIAG_CLOCK_RF_2,
  PM_DIAG_CLOCK_RF_3,
  PM_DIAG_CLOCK_DIFF_1,
  PM_DIAG_CLOCK_DIV_1,
  PM_DIAG_CLOCK_DIV_2,
  PM_DIAG_CLOCK_DIV_3,
  PM_DIAG_CLOCK_SLEEP_1,
} pm_diag_clock_type;

/*
 * Diag command packet to start/stop clocks status logging.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      status_flag;
} pm_diag_clocks_log_req_type;


/*
 * Diag response packet sent back to the test tool.
 *
 * The status of the clocks log is appended to
 * this structure in memory.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      command_type;
  uint8                      result_type;
  uint8                      num_clocks;
  uint8                      pmic_name_len;
} pm_diag_clocks_log_resp_type;

/*
 * This is a callback function which gets executed upon the reception and
 * routing of a Diag subsystem command/request packet to our driver.
 */
static void * pm_diag_clocks_log_handler(void *req_ptr, uint16 req_len);

/*=========================================================================
      Data
==========================================================================*/

static const diagpkt_user_table_entry_type pm_clocks_diag_table[] =
{
  {PM_DIAG_CLOCKS_LOG_CMD_ID, PM_DIAG_CLOCKS_LOG_CMD_ID, pm_diag_clocks_log_handler}
};

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  boolean                    log_enabled_flag;      /**< state variable */
  uint32                     start_resp_size;       /**< size of packet in response to start cmd */
  DalDeviceHandle           *timetick_handle;       /**< handle for getting system time stamp */
  uint8                      num_clocks;            /**< # of clocks being logged */
} pm_clocks_diag;


/*=========================================================================
      Functions
==========================================================================*/
static pm_diag_clock_type pm_diag_clock_lookup (pm_clk_type clock_type)
{
  pm_diag_clock_type diag_clock_type = PM_DIAG_CLOCK_UNKNOWN;

  /*-------------------------------------------------------------------------*/
  /* Lookup Clock type value used by diag app based on clock type enum value */
  /*-------------------------------------------------------------------------*/
     
  switch(clock_type)
  {
  case PM_CLK_XO:

     diag_clock_type = PM_DIAG_CLOCK_XO;
     break;

  case PM_CLK_DIST:

     diag_clock_type = PM_DIAG_CLOCK_DIST;
     break;

  case PM_CLK_BB_1:

     diag_clock_type = PM_DIAG_CLOCK_BB_1;
     break;

  case PM_CLK_BB_2:

     diag_clock_type = PM_DIAG_CLOCK_BB_2;
     break;

  case PM_CLK_LN_BB:

     diag_clock_type = PM_DIAG_CLOCK_LN_BB;
     break;

  case PM_RF_CLK1:

     diag_clock_type = PM_DIAG_CLOCK_RF_1;
     break;

  case PM_RF_CLK2:

     diag_clock_type = PM_DIAG_CLOCK_RF_2;
     break;

  case PM_RF_CLK3:

     diag_clock_type = PM_DIAG_CLOCK_RF_3;
     break;

  case PM_CLK_DIFF_1:

     diag_clock_type = PM_DIAG_CLOCK_DIFF_1;
     break;

  case PM_CLK_DIV_1:

     diag_clock_type = PM_DIAG_CLOCK_DIV_1;
     break;

  case PM_CLK_DIV_2:

     diag_clock_type = PM_DIAG_CLOCK_DIV_2;
     break;

  case PM_CLK_DIV_3:

     diag_clock_type = PM_DIAG_CLOCK_DIV_3;
     break;

  case PM_SLEEP_CLK1:

     diag_clock_type = PM_DIAG_CLOCK_SLEEP_1;
     break;

  default:
     
     diag_clock_type = PM_DIAG_CLOCK_UNKNOWN;
     break;
  }

  return diag_clock_type;

}

/* =========================================================================
**  Function : pm_diag_clocks_status
** =========================================================================*/
/**
  Gets the status for the Clocks and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the SMPS and fills in the diag packet memory.

  @param **clk_log_item_ptr [out]  - Storage for Clock logs.
  @param   pmic_index       [in]   - PMIC index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_clocks_status
(
  pm_clocks_diag_log_item_type **clk_log_item_ptr,
  uint8 pmic_index
)
{
  pm_err_flag_type                  err_flag = PM_ERR_FLAG__SUCCESS;
  pm_clk_data_type                  *clk_data_ptr = NULL;
  pm_clk_status_type                clk_status;
  DalTimetickTime64Type             current_tick = 0;
  pm_clocks_diag_log_item_type      clk_log; // local variable to store the status.
  pm_model_type                     pmic_type = PMIC_IS_INVALID;
  const char                        *pmic_model_name;
  pm_diag_clock_type                diag_clk_type = 0;
  uint8                             max_num_clks = 0;
  uint8                             clk_index = 0;

  clk_data_ptr = pm_clk_get_data(pmic_index);

  if ((clk_data_ptr == NULL) || (clk_data_ptr->clk_common == NULL))
  {
    return;
  }
  
  /*
   * Get max number of peripherals for Clocks.
   */
  max_num_clks = pm_clk_get_num_peripherals(pmic_index);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  for(clk_index = 0;clk_index < max_num_clks;clk_index++)
  {
    if (clk_data_ptr->periph_subtype[clk_index] == 0)
    {
       continue;
    }

    /*
     * Reset the status values.
     */
    diag_clk_type = PM_DIAG_CLOCK_UNKNOWN;
    clk_status.clk_drv_strength = PM_CLK_DRV_STRENGTH_INVALID;
    clk_status.clk_ok = PM_INVALID;
    clk_status.clk_sw_enable = PM_INVALID;
    clk_status.clk_pin_ctrled = PM_INVALID;
    clk_status.clk_pull_down = PM_INVALID;
    clk_status.clk_out_div = 0;
    err_flag = PM_ERR_FLAG__SUCCESS;

    /*
     * Get logging time stamp in ticks.
     */
    DalTimetick_GetTimetick64(pm_clocks_diag.timetick_handle, &current_tick);

    /*
     * Get status for the clock.
     */
    err_flag = pm_clk_get_status(pmic_index, clk_data_ptr->clk_common[clk_index], &clk_status);

    diag_clk_type = pm_diag_clock_lookup(clk_data_ptr->clk_common[clk_index]);

    /*
     * Save status for the clock in the local variable.
     */
    clk_log.timestamp = (uint64)current_tick;
    strlcpy(clk_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    clk_log.clk_type = (uint8)diag_clk_type;
    clk_log.periph_index = (clk_index + 1);
    clk_log.clk_ok_status = (uint8)clk_status.clk_ok;
    clk_log.sw_enable_status = (uint8)clk_status.clk_sw_enable;
    clk_log.pin_ctrl_status = (uint8)clk_status.clk_pin_ctrled;
    clk_log.drv_strength = (uint8)clk_status.clk_drv_strength;
    clk_log.pull_down_status = (uint8)clk_status.clk_pull_down;
    clk_log.status = (uint8)err_flag;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*clk_log_item_ptr,&clk_log,sizeof(pm_clocks_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*clk_log_item_ptr)++;
  }
} /* END pm_diag_clocks_status */

/* =========================================================================
**  Function : pm_diag_clocks_error_response
** =========================================================================*/
/**
  Packages up an error response for the QXDM tool with the requested command.

  @param command [in] -- This specifies the command which resulted in error.

  @return
  pm_diag_clocks_log_resp_type* -- Response packet to Diag.

  @dependencies
  None.
*/
static pm_diag_clocks_log_resp_type* pm_diag_clocks_error_response
(
  pm_diag_command_type command
)
{
  pm_diag_clocks_log_resp_type *error_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Try to allocate memory for error response                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr =
    (pm_diag_clocks_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_CLOCKS_LOG_CMD_ID,
        sizeof(pm_diag_clocks_log_resp_type));

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
} /* END pm_diag_clocks_error_response */

/* =========================================================================
**  Function : pm_diag_clocks_deinit_logging
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
static DALResult pm_diag_clocks_deinit_logging(void)
{
  pm_clocks_diag.log_enabled_flag = FALSE;
  pm_clocks_diag.num_clocks = 0;
  pm_clocks_diag.start_resp_size = 0;

  /*
   * Detach timetick handler.
   */
  if (DAL_SUCCESS != DAL_DeviceDetach(pm_clocks_diag.timetick_handle))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
} /* END pm_diag_clocks_deinit_logging */

/* =========================================================================
**  Function : pm_diag_clocks_init_logging
** =========================================================================*/
/**
  Init routine for all logging related activities.

  This function is invoked by the Diag log command handler to prepare the
  driver for capturing and reporting clocks status via diag resp packet.

  @param None.

  @return
  DALResult -- Result of logging setup.

  @dependencies
  None.
*/
static DALResult pm_diag_clocks_init_logging(void)
{
  static boolean log_supported_flag = TRUE;
  const char* timetick_dev_str = "SystemTimer";
  pm_clk_data_type *clk_data_ptr = NULL;
  uint8 pmic_index = 0;
  uint8 clk_index = 0;
  uint8 max_num_clks = 0;

  
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

  if (pm_clocks_diag.log_enabled_flag == TRUE)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to DAL Timetick to get the System time stamp later.            */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &pm_clocks_diag.timetick_handle))
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the number of clocks to calculate the log resp packet size.  */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    clk_data_ptr = pm_clk_get_data(pmic_index);

    if (clk_data_ptr != NULL)
    {
      max_num_clks = pm_clk_get_num_peripherals(pmic_index);

      for (clk_index = 0;clk_index < max_num_clks;clk_index++)
      {
        if (clk_data_ptr->periph_subtype[clk_index] != 0)
        {
          pm_clocks_diag.num_clocks = pm_clocks_diag.num_clocks + 1;   
        }
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* If there are no clocks we will not support logging.              */
  /*-----------------------------------------------------------------------*/

  if (pm_clocks_diag.num_clocks == 0)
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    if (DAL_SUCCESS != pm_diag_clocks_deinit_logging())
    {
      return DAL_ERROR;
    }

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Compute Diag start log response packet size.                          */
  /*-----------------------------------------------------------------------*/
  pm_clocks_diag.start_resp_size = 
    sizeof(pm_diag_clocks_log_resp_type) + (sizeof(pm_clocks_diag_log_item_type) * pm_clocks_diag.num_clocks);

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : pm_diag_clocks_log_start
** =========================================================================*/
/**
  Enables clocks status logging functionality.

  This function starts the logging of clocks status and sends back
  a Diag packet containing the status of the clocks and their
  respective string names including the corresponding PMIC name.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_clocks_log_start(void)
{
  pm_diag_clocks_log_resp_type *start_resp_ptr = NULL;
  pm_clocks_diag_log_item_type *clocks_log_resp_ptr = NULL;
  char                         *diag_buf_ptr = NULL;
  uint8                         pmic_index = 0;

  /*-----------------------------------------------------------------------*/
  /* Prepare the logging system.                                           */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != pm_diag_clocks_init_logging())
  {
    return pm_diag_clocks_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  diag_buf_ptr = diagpkt_subsys_alloc(DIAG_SUBSYS_PM, PM_DIAG_CLOCKS_LOG_CMD_ID, 
                                      pm_clocks_diag.start_resp_size);
  if (diag_buf_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Locate fields in newly allocated Diag buffer.                         */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr = (pm_diag_clocks_log_resp_type *)diag_buf_ptr;
  clocks_log_resp_ptr = 
    (pm_clocks_diag_log_item_type *)( diag_buf_ptr + sizeof(pm_diag_clocks_log_resp_type) );

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->command_type = PM_DIAG_LOGGING_START;

  /*-----------------------------------------------------------------------*/
  /* Populate the diag response packet with the clocks array size.         */
  /* This is needed so that QXDM knows how many clocks names to pick       */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->num_clocks = pm_clocks_diag.num_clocks;

  /*-----------------------------------------------------------------------*/
  /* Send the buffer length used for the pmic names.                       */
  /* This is needed so that QXDM knows how many bytes/bits are in each of  */
  /* the pmic name array fields.                                           */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->pmic_name_len = PM_DIAG_PMIC_NAME_LEN;

  /*-----------------------------------------------------------------------*/
  /* Fill in start response with clocks status data.                       */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    pm_diag_clocks_status(&clocks_log_resp_ptr,pmic_index);
  }

  /*-----------------------------------------------------------------------*/
  /* The callback setup was successful.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->result_type = PM_DIAG_SUCCESS;
  pm_clocks_diag.log_enabled_flag = TRUE;

  return start_resp_ptr;

} /* END pm_diag_clocks_log_start */


/* =========================================================================
**  Function : pm_diag_clocks_log_stop
** =========================================================================*/
/**
  Disables clocks status logging functionality.

  This function stops the logging of clocks status and sends back a Diag packet
  with the result.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_clocks_log_stop(void)
{
  pm_diag_clocks_log_resp_type *stop_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr =
    (pm_diag_clocks_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_CLOCKS_LOG_CMD_ID,
        sizeof(pm_diag_clocks_log_resp_type));

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
  /* Return success is clocks logging is already stopped.             */
  /*-----------------------------------------------------------------------*/

  if (pm_clocks_diag.log_enabled_flag == FALSE)
  {
    stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
  }
  else
  {
    /*
     * Deinitialize logging.
     */
    if (DAL_SUCCESS != pm_diag_clocks_deinit_logging())
    {
      stop_resp_ptr->result_type = PM_DIAG_ERROR;
    }
    else
    {
      stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
    }
  }

  return (void *)stop_resp_ptr;

} /* END pm_diag_clocks_log_stop */

/* =========================================================================
**  Function : pm_diag_clocks_log_handler
** =========================================================================*/
/**
  Handler to start/stop clocks status logging.

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
void * pm_diag_clocks_log_handler
(
  void  *req_ptr,   /* pointer to request packet */
  uint16 req_len /* length of request packet  */
)
{
  void                        *resp_ptr;
  pm_diag_clocks_log_req_type *clk_req_ptr = (pm_diag_clocks_log_req_type*)req_ptr;

  /*-----------------------------------------------------------------------*/
  /* Validate the incoming packet is as expected.                          */
  /*-----------------------------------------------------------------------*/

  if (clk_req_ptr == NULL)
  {
    return NULL;
  }

  if (req_len != sizeof(pm_diag_clocks_log_req_type))
  {
    resp_ptr = diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, sizeof(pm_diag_clocks_log_req_type));

    return (void *)resp_ptr;
  }

  /*-----------------------------------------------------------------------*/
  /* Start or stop the clocks status logging.                         */
  /*-----------------------------------------------------------------------*/

  if (clk_req_ptr->status_flag)
  {
    resp_ptr = pm_diag_clocks_log_start();
  }
  else
  {
    resp_ptr = pm_diag_clocks_log_stop();
  }

  /*-----------------------------------------------------------------------*/
  /* Response packet is transmitted by dispatcher upon return.             */
  /*-----------------------------------------------------------------------*/

  return (void *)resp_ptr;

} /* END pm_diag_clocks_log_handler */


/* =========================================================================
**  Function : pm_diag_clocks_init
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
void pm_diag_clocks_init (void)
{
  pm_clocks_diag.log_enabled_flag = FALSE;
  pm_clocks_diag.num_clocks = 0;
  pm_clocks_diag.start_resp_size = 0;
  pm_clocks_diag.timetick_handle = NULL;

  /*-----------------------------------------------------------------------*/
  /* Registers the clocks diag support with the dispatcher.                 */
  /*-----------------------------------------------------------------------*/

  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PM, pm_clocks_diag_table);

} /* END pm_diag_clocks_init */

