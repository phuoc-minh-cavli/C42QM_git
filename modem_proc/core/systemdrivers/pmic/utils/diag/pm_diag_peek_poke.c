/*! \file pm_diag_peek_poke.c
*
*  \brief Diag handlers to get PMIC peek_poke status.
*  \n
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag_peek_poke.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------
10/28/14   kt      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_comm.h"
#include "pm_diag.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/* Max number of register that can be burst read at once */
#define PM_DIAG_MAX_PEEK_SIZE 8

/*
 * Peek-Poke command type used in QXDM/Diag communications.
 */
typedef enum
{
  PM_DIAG_CMD_NONE,
  PM_DIAG_CMD_PEEK,
  PM_DIAG_CMD_POKE,
  PM_DIAG_CMD_INVALID,
} pm_diag_peek_poke_cmd_type;

/*
 * Diag command packet to start/stop peek_poke status logging.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      num_of_addr; /* Num of reg addresses (0 implies stop request) */
} pm_diag_peek_poke_log_req_type;

/*
 * This structure is used to describe the PEEK-POKE diag 
 * log packet sent back to the test tool. This structure 
 * is appended to Diag response packet structure in 
 * memory prior to submitting to Diag.
 */
typedef PACK(struct)
{
  uint8    peek_poke_type;        /**< Peek-Poke command type */
  uint8    pmic_index;            /**< PMIC Index */
  uint8    sid_index;             /**< Slave id Index */
  uint16   reg_addr;              /**< Peek-Poke start reg addr */
  uint8    reg_data;              /**< Poke reg data */
  uint8    num_regs;              /**< # of regs being logged per address */
} pm_peek_poke_diag_log_req_item_type;


/*
 * Diag response packet sent back to the test tool.
 *
 * The status of the peek_poke log is appended to
 * this structure in memory.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      command_type;
  uint8                      result_type;
  uint8                      num_regs;
  uint8                      pmic_name_len;
} pm_diag_peek_poke_log_resp_type;

/*
 * This is a callback function which gets executed upon the reception and
 * routing of a Diag subsystem command/request packet to our driver.
 */
static void * pm_diag_peek_poke_log_handler(void *req_ptr, uint16 req_len);

/*=========================================================================
      Data
==========================================================================*/

static const diagpkt_user_table_entry_type pm_peek_poke_diag_table[] =
{
  {PM_DIAG_PEEK_POKE_LOG_CMD_ID, PM_DIAG_PEEK_POKE_LOG_CMD_ID, pm_diag_peek_poke_log_handler}
};

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  boolean                             log_enabled_flag;      /**< state variable */
  uint32                              start_resp_size;       /**< size of packet in response to start cmd */
  DalDeviceHandle                     *timetick_handle;      /**< handle for getting system time stamp */
  pm_diag_peek_poke_log_req_type      *req_start_ptr;
  pm_diag_peek_poke_cmd_type          peek_poke_type;        /**< Peek-Poke command type */
  uint8                               pmic_index;            /**< PMIC Index */
  uint8                               sid_index;             /**< Slave id Index */
  uint16                              reg_addr;              /**< Peek-Poke start reg addr */
  uint8                               reg_data;              /**< Poke reg data */
  uint8                               num_regs;              /**< # of regs being logged per address */
} pm_peek_poke_diag;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : pm_diag_peek_poke_status
** =========================================================================*/
/**
  Gets the status for the GPIOs and appends the log to the diag response
  packet in memory.

  This function gets the Enable Status, Mode Type, Volage Level and Range
  for all the SMPS and fills in the diag packet memory.

  @param **peek_poke_log_item_ptr [out]  - Storage for Clock logs.
  @param   pmic_index       [in]   - PMIC index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_peek_poke_status
(
  pm_peek_poke_diag_log_item_type **peek_poke_log_item_ptr,
  uint8 pmic_index
)
{
  pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
  DalTimetickTime64Type            current_tick = 0;
  pm_peek_poke_diag_log_item_type  peek_poke_log; // local variable to store the status.
  pm_model_type                    pmic_type = PMIC_IS_INVALID;
  const char                       *pmic_model_name;
  uint32                           slave_id = 0;
  uint8                            peek_data_arr[PM_DIAG_MAX_PEEK_SIZE] = {0};
  uint8                            index = 0;

  /*
   * Get logging time stamp in ticks.
   */
  DalTimetick_GetTimetick64(pm_peek_poke_diag.timetick_handle, &current_tick);

  err_flag = pm_get_slave_id(pmic_index, pm_peek_poke_diag.sid_index, &slave_id);

  /*
   * Get PMIC model name.
   */
  pmic_type = pm_get_pmic_model(pmic_index);
  pmic_model_name = pm_diag_model_lookup(pmic_type);

  /*
   * Peek or Poke the register based on the command type.
   */

  if (pm_peek_poke_diag.peek_poke_type == PM_DIAG_CMD_POKE)
  {
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      err_flag = pm_comm_write_byte(slave_id, pm_peek_poke_diag.reg_addr, pm_peek_poke_diag.reg_data, 0);
    }

    /*
     * Save POKE status in the local variable.
     */
    peek_poke_log.timestamp = (uint64)current_tick;
    strlcpy(peek_poke_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    peek_poke_log.reg_addr = pm_peek_poke_diag.reg_addr;
    peek_poke_log.pmic_index = pmic_index;
    peek_poke_log.sid_index = pm_peek_poke_diag.sid_index;
    peek_poke_log.reg_data = pm_peek_poke_diag.reg_data;
    peek_poke_log.peek_poke_type = (uint8)pm_peek_poke_diag.peek_poke_type;
    peek_poke_log.status = (uint8)err_flag;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*peek_poke_log_item_ptr,&peek_poke_log,sizeof(pm_peek_poke_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*peek_poke_log_item_ptr)++;
  }
  else
  {
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      err_flag = pm_comm_read_byte_array(slave_id, pm_peek_poke_diag.reg_addr, pm_peek_poke_diag.num_regs, peek_data_arr, 0);
    }

    for(index = 0;index < pm_peek_poke_diag.num_regs;index++)
    {
      /*
       * Save PEEK status in the local variable.
       */
      peek_poke_log.timestamp = (uint64)current_tick;
      strlcpy(peek_poke_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
      peek_poke_log.reg_addr = pm_peek_poke_diag.reg_addr;
      peek_poke_log.pmic_index = pmic_index;
      peek_poke_log.sid_index = pm_peek_poke_diag.sid_index;
      peek_poke_log.reg_data = peek_data_arr[index];
      peek_poke_log.peek_poke_type = (uint8)pm_peek_poke_diag.peek_poke_type;
      peek_poke_log.status = (uint8)err_flag;

      /*
       * Copy the log items to allocated diag packet memory.
       */
      DALSYS_memcpy(*peek_poke_log_item_ptr,&peek_poke_log,sizeof(pm_peek_poke_diag_log_item_type));

      /*
       * Adjust pointer to the next item in the array.
       */
      (*peek_poke_log_item_ptr)++;

      pm_peek_poke_diag.reg_addr++;
    }
  }

} /* END pm_diag_peek_poke_status */

/* =========================================================================
**  Function : pm_diag_peek_poke_error_response
** =========================================================================*/
/**
  Packages up an error response for the QXDM tool with the requested command.

  @param command [in] -- This specifies the command which resulted in error.

  @return
  pm_diag_peek_poke_log_resp_type* -- Response packet to Diag.

  @dependencies
  None.
*/
static pm_diag_peek_poke_log_resp_type* pm_diag_peek_poke_error_response
(
  pm_diag_command_type command
)
{
  pm_diag_peek_poke_log_resp_type *error_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Try to allocate memory for error response                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr =
    (pm_diag_peek_poke_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_PEEK_POKE_LOG_CMD_ID,
        sizeof(pm_diag_peek_poke_log_resp_type));

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
} /* END pm_diag_peek_poke_error_response */

/* =========================================================================
**  Function : pm_diag_peek_poke_deinit_logging
** =========================================================================*/
/**
  DeInit routine for all logging related activities.

  This function cleans up the preparation done during logging initialization.

  @param
  None.

  @return
  DALResult -- Result of logging deinit.

  @dependencies
  None.
*/
static DALResult pm_diag_peek_poke_deinit_logging(void)
{
  pm_peek_poke_diag.log_enabled_flag = FALSE;
  pm_peek_poke_diag.start_resp_size = 0;
  pm_peek_poke_diag.peek_poke_type = PM_DIAG_CMD_NONE;
  pm_peek_poke_diag.pmic_index = 0;
  pm_peek_poke_diag.sid_index = 0;
  pm_peek_poke_diag.reg_addr = 0;
  pm_peek_poke_diag.reg_data = 0;
  pm_peek_poke_diag.num_regs = 0;
  pm_peek_poke_diag.req_start_ptr = NULL;

  /*
   * Detach timetick handler.
   */
  if (DAL_SUCCESS != DAL_DeviceDetach(pm_peek_poke_diag.timetick_handle))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
} /* END pm_diag_peek_poke_deinit_logging */

/* =========================================================================
**  Function : pm_diag_peek_poke_init_logging
** =========================================================================*/
/**
  Init routine for all logging related activities.

  This function is invoked by the Diag log command handler to prepare the
  driver for capturing and reporting peek_poke status via diag resp packet.
 
  @param
  None.

  @return
  DALResult -- Result of logging setup.

  @dependencies
  None.
*/
static DALResult pm_diag_peek_poke_init_logging()
{
  static boolean log_supported_flag = TRUE;
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

  if (pm_peek_poke_diag.log_enabled_flag == TRUE)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to DAL Timetick to get the System time stamp later.            */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &pm_peek_poke_diag.timetick_handle))
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : pm_diag_peek_poke_log_start
** =========================================================================*/
/**
  Enables peek_poke status logging functionality.

  This function starts the logging of peek_poke status and sends back
  a Diag packet containing the status of the peek_poke and their
  respective string names including the corresponding PMIC name.
 
  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_peek_poke_log_start()
{
  pm_diag_peek_poke_log_resp_type *resp_start_ptr = NULL;
  pm_peek_poke_diag_log_item_type *peek_poke_log_resp_ptr = NULL;
  pm_peek_poke_diag_log_req_item_type *peek_poke_log_req_ptr = NULL;
  char                            *diag_buf_ptr = NULL;
  uint32                          total_num_regs = 0;
  uint8                           index = 0;

  /*-----------------------------------------------------------------------*/
  /* Prepare the logging system.                                           */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != pm_diag_peek_poke_init_logging())
  {
    return pm_diag_peek_poke_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Compute Diag start log response packet size.                          */
  /*-----------------------------------------------------------------------*/

  /* Get the req log items start ptr */
  peek_poke_log_req_ptr = (pm_peek_poke_diag_log_req_item_type*)(pm_peek_poke_diag.req_start_ptr + 1);

  /* Calculate the total num of regs to be peek-poked by looping through all the reg address */
  for (index = 0; index < pm_peek_poke_diag.req_start_ptr->num_of_addr; index++)
  {
    if (peek_poke_log_req_ptr == NULL)
    {
      return pm_diag_peek_poke_error_response(PM_DIAG_LOGGING_START);
    }

    if (((pm_diag_peek_poke_cmd_type)peek_poke_log_req_ptr->peek_poke_type) == PM_DIAG_CMD_POKE)
    {
      /* Only single byte write supported for POKE commands */
      total_num_regs = total_num_regs + 1;
    }
    else
    {
      /* Consider multi-byte array read for PEEK commands */
      if (peek_poke_log_req_ptr->num_regs > PM_DIAG_MAX_PEEK_SIZE)
      {
        total_num_regs = total_num_regs + PM_DIAG_MAX_PEEK_SIZE;
      }
      else
      {
        total_num_regs = total_num_regs + peek_poke_log_req_ptr->num_regs;
      }
    }

    /* Increment the pointer to next request log item */
    peek_poke_log_req_ptr++;
  }

  pm_peek_poke_diag.start_resp_size = 
    sizeof(pm_diag_peek_poke_log_resp_type) + (sizeof(pm_peek_poke_diag_log_item_type) * total_num_regs);

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  diag_buf_ptr = diagpkt_subsys_alloc(DIAG_SUBSYS_PM, PM_DIAG_PEEK_POKE_LOG_CMD_ID, 
                                      pm_peek_poke_diag.start_resp_size);
  if (diag_buf_ptr == NULL)
  {
    return pm_diag_peek_poke_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Locate fields in newly allocated Diag buffer.                         */
  /*-----------------------------------------------------------------------*/

  resp_start_ptr = (pm_diag_peek_poke_log_resp_type *)diag_buf_ptr;
  peek_poke_log_resp_ptr = 
    (pm_peek_poke_diag_log_item_type *)( diag_buf_ptr + sizeof(pm_diag_peek_poke_log_resp_type) );

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  resp_start_ptr->command_type = PM_DIAG_LOGGING_START;

  /*-----------------------------------------------------------------------*/
  /* Populate the diag response packet with the peek_poke array size.      */
  /* This is needed so that QXDM knows how many peek_poke names to pick    */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  resp_start_ptr->num_regs = total_num_regs;

  /*-----------------------------------------------------------------------*/
  /* Send the buffer length used for the pmic names.                       */
  /* This is needed so that QXDM knows how many bytes/bits are in each of  */
  /* the pmic name array fields.                                           */
  /*-----------------------------------------------------------------------*/

  resp_start_ptr->pmic_name_len = PM_DIAG_PMIC_NAME_LEN;

  /*-----------------------------------------------------------------------*/
  /* Fill in start response with peek_poke status data.                    */
  /*-----------------------------------------------------------------------*/

  peek_poke_log_req_ptr = (pm_peek_poke_diag_log_req_item_type*)(pm_peek_poke_diag.req_start_ptr + 1);

  for (index = 0; index < pm_peek_poke_diag.req_start_ptr->num_of_addr; index++)
  {
    if (peek_poke_log_req_ptr == NULL)
    {
      return pm_diag_peek_poke_error_response(PM_DIAG_LOGGING_START);
    }

    /* Update the static global struct used by PEEK-POKE status function */
    pm_peek_poke_diag.peek_poke_type = (pm_diag_peek_poke_cmd_type)peek_poke_log_req_ptr->peek_poke_type;
    pm_peek_poke_diag.num_regs = peek_poke_log_req_ptr->num_regs;

    if (pm_peek_poke_diag.peek_poke_type == PM_DIAG_CMD_POKE)
    {    
      /* Only single byte write supported for POKE commands */
      pm_peek_poke_diag.num_regs = 1;
    }
    else
    {
      /* Consider multi-byte array read for PEEK commands */
      pm_peek_poke_diag.peek_poke_type = PM_DIAG_CMD_PEEK;

      if (pm_peek_poke_diag.num_regs > PM_DIAG_MAX_PEEK_SIZE)
      {    
        pm_peek_poke_diag.num_regs = PM_DIAG_MAX_PEEK_SIZE;
      }
    }
      
    /* Update the static global struct used by PEEK-POKE status function */
    pm_peek_poke_diag.pmic_index = peek_poke_log_req_ptr->pmic_index;
    pm_peek_poke_diag.sid_index = peek_poke_log_req_ptr->sid_index;
    pm_peek_poke_diag.reg_addr = peek_poke_log_req_ptr->reg_addr;
    pm_peek_poke_diag.reg_data = peek_poke_log_req_ptr->reg_data;

    /* Get the peek-poke status for the selected address */
    pm_diag_peek_poke_status(&peek_poke_log_resp_ptr, pm_peek_poke_diag.pmic_index);

    /* Increment the pointer to next request log item */
    peek_poke_log_req_ptr++;
  }

  /*-----------------------------------------------------------------------*/
  /* The callback setup was successful.                                    */
  /*-----------------------------------------------------------------------*/

  resp_start_ptr->result_type = PM_DIAG_SUCCESS;
  pm_peek_poke_diag.log_enabled_flag = TRUE;

  return resp_start_ptr;

} /* END pm_diag_peek_poke_log_start */


/* =========================================================================
**  Function : pm_diag_peek_poke_log_stop
** =========================================================================*/
/**
  Disables peek_poke status logging functionality.

  This function stops the logging of peek_poke status and sends back a Diag packet
  with the result.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_peek_poke_log_stop(void)
{
  pm_diag_peek_poke_log_resp_type *stop_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr =
    (pm_diag_peek_poke_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_PEEK_POKE_LOG_CMD_ID,
        sizeof(pm_diag_peek_poke_log_resp_type));

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
  /* Return success is peek_poke logging is already stopped.             */
  /*-----------------------------------------------------------------------*/

  if (pm_peek_poke_diag.log_enabled_flag == FALSE)
  {
    stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
  }
  else
  {
    /*
     * Deinitialize logging.
     */
    if (DAL_SUCCESS != pm_diag_peek_poke_deinit_logging())
    {
      stop_resp_ptr->result_type = PM_DIAG_ERROR;
    }
    else
    {
      stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
    }
  }

  return (void *)stop_resp_ptr;

} /* END pm_diag_peek_poke_log_stop */

/* =========================================================================
**  Function : pm_diag_peek_poke_log_handler
** =========================================================================*/
/**
  Handler to start/stop peek_poke status logging.

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
void * pm_diag_peek_poke_log_handler
(
  void  *req_ptr,   /* pointer to request packet */
  uint16 req_len /* length of request packet  */
)
{
  void *resp_ptr;
  pm_diag_peek_poke_log_req_type *peek_poke_req_ptr = (pm_diag_peek_poke_log_req_type*)req_ptr;

  /*-----------------------------------------------------------------------*/
  /* Validate the incoming packet is as expected.                          */
  /*-----------------------------------------------------------------------*/

  if (peek_poke_req_ptr == NULL)
  {
    return NULL;
  }

  /*if (req_len != sizeof(pm_diag_peek_poke_log_req_type))
  {
    resp_ptr = diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, sizeof(pm_diag_peek_poke_log_req_type));

    return (void *)resp_ptr;
  }*/

  /*-----------------------------------------------------------------------*/
  /* Start or stop the peek-poke logging.                                  */
  /*-----------------------------------------------------------------------*/

  if (peek_poke_req_ptr->num_of_addr != 0)
  {
    pm_peek_poke_diag.req_start_ptr = peek_poke_req_ptr;

    resp_ptr = pm_diag_peek_poke_log_start();
  }
  else
  {
    resp_ptr = pm_diag_peek_poke_log_stop();
  }

  /*-----------------------------------------------------------------------*/
  /* Response packet is transmitted by dispatcher upon return.             */
  /*-----------------------------------------------------------------------*/

  return (void *)resp_ptr;

} /* END pm_diag_peek_poke_log_handler */


/* =========================================================================
**  Function : pm_diag_peek_poke_init
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
void pm_diag_peek_poke_init (void)
{
  pm_peek_poke_diag.log_enabled_flag = FALSE;
  pm_peek_poke_diag.start_resp_size = 0;
  pm_peek_poke_diag.timetick_handle = NULL;
  pm_peek_poke_diag.peek_poke_type = PM_DIAG_CMD_NONE;
  pm_peek_poke_diag.pmic_index = 0;
  pm_peek_poke_diag.sid_index = 0;
  pm_peek_poke_diag.reg_addr = 0;
  pm_peek_poke_diag.reg_data = 0;
  pm_peek_poke_diag.num_regs = 0;
  pm_peek_poke_diag.req_start_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Registers the peek_poke diag support with the dispatcher.                 */
  /*-----------------------------------------------------------------------*/

  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PM, pm_peek_poke_diag_table);

} /* END pm_diag_peek_poke_init */

