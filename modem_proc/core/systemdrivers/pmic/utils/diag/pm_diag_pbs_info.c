/*! \file pm_diag_pbs_info.c
*
*  \brief Diag handlers to get PMIC Info status.
*  \n
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag_pbs_info.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------
10/28/14   kt      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_diag.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/*
 * Diag command packet to start/stop PBS Info status logging.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      status_flag;
} pm_diag_pbs_info_log_req_type;


/*
 * Diag response packet sent back to the test tool.
 *
 * The status of the pmic info log is appended to
 * this structure in memory.
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header_type;
  uint8                      command_type;
  uint8                      result_type;
  uint8                      num_pmics;
  uint8                      pmic_name_len;
  uint8                      num_lot_ids;
} pm_diag_pbs_info_log_resp_type;

/*
 * This is a callback function which gets executed upon the reception and
 * routing of a Diag subsystem command/request packet to our driver.
 */
static void *pm_diag_pbs_info_log_handler(void *req_ptr, uint16 req_len);

/*=========================================================================
      Data
==========================================================================*/

static const diagpkt_user_table_entry_type pm_pbs_info_diag_table[] =
{
  {PM_DIAG_PBS_INFO_LOG_CMD_ID, PM_DIAG_PBS_INFO_LOG_CMD_ID, pm_diag_pbs_info_log_handler}
};

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  boolean                    log_enabled_flag;      /**< state variable */
  uint32                     start_resp_size;       /**< size of packet in response to start cmd */
  uint8                      num_pmics;             /**< # of pmics info being logged */
} pm_pbs_info_diag;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : pm_diag_pbs_info_status
** =========================================================================*/
/**
  Gets the PMIC Info and appends the log to the diag response
  packet in memory.

  @param **pbs_info_log_item_ptr [out]  - Storage for PBS Info log.
  @param   pmic_index            [in]   - PMIC index.

  @return
  None.

  @dependencies
  None.
*/
static void pm_diag_pbs_info_status
(
  pm_pbs_info_diag_log_item_type **pbs_info_log_item_ptr,
  uint8 pmic_index
)
{
  pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
  pm_pbs_info_diag_log_item_type   pbs_info_log; // local variable to store the status.
  const char                       *pmic_model_name;
  pm_pbs_info_type                 pbs_info; 
  pm_model_type                    pmic_model = PMIC_IS_UNKNOWN;

  /* Initialize PBS Info */
  DALSYS_memset(&pbs_info, 0, sizeof(pm_pbs_info_type));

  pmic_model = pm_get_pmic_model(pmic_index);

  if ((pmic_model != PMIC_IS_UNKNOWN) && (pmic_model != PMIC_IS_INVALID))
  {
    pmic_model_name = pm_diag_model_lookup(pmic_model);

    /*
     * Get PBS Info.
     */
    err_flag = pm_get_pbs_info(pmic_index, &pbs_info);

    /*
     * Save PBS Info in the local variable.
     */
    strlcpy(pbs_info_log.pmic_name, pmic_model_name, PM_DIAG_PMIC_NAME_LEN);
    DALSYS_memcpy(pbs_info_log.lot_id, pbs_info.nLotId,sizeof(pbs_info_log.lot_id));
    pbs_info_log.otp_branch_id = (uint8)(pbs_info.nPBSROMVersion & 0xFF);
    pbs_info_log.otp_rev_id = (uint8)((pbs_info.nPBSROMVersion>>8) & 0xFF);
    pbs_info_log.ram_branch_id = (uint8)(pbs_info.nPBSRAMVersion & 0xFF);
    pbs_info_log.ram_rev_id = (uint8)((pbs_info.nPBSRAMVersion>>8) & 0xFF);
    pbs_info_log.fab_id = (uint8)pbs_info.nFabId;
    pbs_info_log.wafer_id = (uint8)pbs_info.nWaferId;
    pbs_info_log.x_coord = (uint8)pbs_info.nXcoord;
    pbs_info_log.y_coord = (uint8)pbs_info.nYcoord;
    pbs_info_log.test_pgm_rev = (uint8)pbs_info.nTestPgmRev;
    pbs_info_log.mfg_id_major = (uint8)pbs_info.eMfgId.major;
    pbs_info_log.mfg_id_minor = (uint8)pbs_info.eMfgId.minor;
    pbs_info_log.mfg_id_shrink = (uint8)pbs_info.eMfgId.shrink;
    pbs_info_log.pmic_index = pmic_index;
    pbs_info_log.pmic_model = (uint8)pmic_model;
    pbs_info_log.status = (uint8)err_flag;

    /*
     * Copy the log items to allocated diag packet memory.
     */
    DALSYS_memcpy(*pbs_info_log_item_ptr,&pbs_info_log,sizeof(pm_pbs_info_diag_log_item_type));

    /*
     * Adjust pointer to the next item in the array.
     */
    (*pbs_info_log_item_ptr)++;
  }
} /* END pm_diag_pbs_info_status */

/* =========================================================================
**  Function : pm_diag_pbs_info_error_response
** =========================================================================*/
/**
  Packages up an error response for the QXDM tool with the requested command.

  @param command [in] -- This specifies the command which resulted in error.

  @return
  pm_diag_pbs_info_log_resp_type* -- Response packet to Diag.

  @dependencies
  None.
*/
static pm_diag_pbs_info_log_resp_type* pm_diag_pbs_info_error_response
(
  pm_diag_command_type command
)
{
  pm_diag_pbs_info_log_resp_type *error_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Try to allocate memory for error response                             */
  /*-----------------------------------------------------------------------*/

  error_resp_ptr =
    (pm_diag_pbs_info_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_PBS_INFO_LOG_CMD_ID,
        sizeof(pm_diag_pbs_info_log_resp_type));

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
} /* END pm_diag_pbs_info_error_response */

/* =========================================================================
**  Function : pm_diag_pbs_info_deinit_logging
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
static DALResult pm_diag_pbs_info_deinit_logging(void)
{
  pm_pbs_info_diag.log_enabled_flag = FALSE;
  pm_pbs_info_diag.num_pmics = 0;
  pm_pbs_info_diag.start_resp_size = 0;

  return DAL_SUCCESS;
} /* END pm_diag_pbs_info_deinit_logging */

/* =========================================================================
**  Function : pm_diag_pbs_info_init_logging
** =========================================================================*/
/**
  Init routine for all logging related activities.

  This function is invoked by the Diag log command handler to prepare the
  driver for capturing and reporting PMIC Info via diag resp packet.

  @param None.

  @return
  DALResult -- Result of logging setup.

  @dependencies
  None.
*/
static DALResult pm_diag_pbs_info_init_logging(void)
{
  static boolean log_supported_flag = TRUE;
  uint8 pmic_index = 0;
  pm_model_type pmic_model = PMIC_IS_UNKNOWN;

  
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

  if (pm_pbs_info_diag.log_enabled_flag == TRUE)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the number of PBS Info to calculate the log resp packet size.     */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    pmic_model = pm_get_pmic_model(pmic_index);

    if ((pmic_model != PMIC_IS_UNKNOWN) && (pmic_model != PMIC_IS_INVALID))
    {
      pm_pbs_info_diag.num_pmics++;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* If there are no pbs_info we will not support logging.              */
  /*-----------------------------------------------------------------------*/

  if (pm_pbs_info_diag.num_pmics == 0)
  {
    /*
     * Update state to indicate we cannot support logging.
     */
    log_supported_flag = FALSE;
    if (DAL_SUCCESS != pm_diag_pbs_info_deinit_logging())
    {
      return DAL_ERROR;
    }

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Compute Diag start log response packet size.                          */
  /*-----------------------------------------------------------------------*/
  pm_pbs_info_diag.start_resp_size = 
    sizeof(pm_diag_pbs_info_log_resp_type) + (sizeof(pm_pbs_info_diag_log_item_type) * pm_pbs_info_diag.num_pmics);

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : pm_diag_pbs_info_log_start
** =========================================================================*/
/**
  Enables pbs_info status logging functionality.

  This function the logs the pmic info and sends back
  a Diag packet to the tool with the pmic info.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_pbs_info_log_start(void)
{
  pm_diag_pbs_info_log_resp_type *start_resp_ptr = NULL;
  pm_pbs_info_diag_log_item_type *pbs_info_log_resp_ptr = NULL;
  char                            *diag_buf_ptr = NULL;
  uint8                           pmic_index = 0;

  /*-----------------------------------------------------------------------*/
  /* Prepare the logging system.                                           */
  /*-----------------------------------------------------------------------*/

  if (DAL_SUCCESS != pm_diag_pbs_info_init_logging())
  {
    return pm_diag_pbs_info_error_response(PM_DIAG_LOGGING_START);
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  diag_buf_ptr = diagpkt_subsys_alloc(DIAG_SUBSYS_PM, PM_DIAG_PBS_INFO_LOG_CMD_ID, 
                                      pm_pbs_info_diag.start_resp_size);
  if (diag_buf_ptr == NULL)
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Locate fields in newly allocated Diag buffer.                         */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr = (pm_diag_pbs_info_log_resp_type *)diag_buf_ptr;
  pbs_info_log_resp_ptr = 
    (pm_pbs_info_diag_log_item_type *)( diag_buf_ptr + sizeof(pm_diag_pbs_info_log_resp_type) );

  /*-----------------------------------------------------------------------*/
  /* Indicate the command being responded to so that QXDM knows how to     */
  /* parse the response accordingly.                                       */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->command_type = PM_DIAG_LOGGING_START;

  /*-----------------------------------------------------------------------*/
  /* Populate the diag response packet with the PBS Info array size.       */
  /* This is needed so that QXDM knows how many PMICs PBS info to pick     */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->num_pmics = pm_pbs_info_diag.num_pmics;

  /*-----------------------------------------------------------------------*/
  /* Send the buffer length used for the PMIC names.                       */
  /* This is needed so that QXDM knows how many bytes/bits are in each of  */
  /* the PMIC name array fields.                                           */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->pmic_name_len = PM_DIAG_PMIC_NAME_LEN;

  /*-----------------------------------------------------------------------*/
  /* This is needed so that QXDM knows how many PBS Lot ids to pick        */
  /* up in the response packet.                                            */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->num_lot_ids = PM_PBS_INFO_NUM_LOT_IDS;

  /*-----------------------------------------------------------------------*/
  /* Fill in start response with PBS Info status data.                     */
  /*-----------------------------------------------------------------------*/

  for (pmic_index = 0;pmic_index < PM_MAX_NUM_PMICS;pmic_index++)
  {
    pm_diag_pbs_info_status(&pbs_info_log_resp_ptr,pmic_index);
  }

  /*-----------------------------------------------------------------------*/
  /* The callback setup was successful.                                    */
  /*-----------------------------------------------------------------------*/

  start_resp_ptr->result_type = PM_DIAG_SUCCESS;
  pm_pbs_info_diag.log_enabled_flag = TRUE;

  return start_resp_ptr;

} /* END pm_diag_pbs_info_log_start */


/* =========================================================================
**  Function : pm_diag_pbs_info_log_stop
** =========================================================================*/
/**
  Disables pbs_info status logging functionality.

  This function stops the logging of pbs_info status and sends back a Diag packet
  with the result.

  @param
  None.

  @return
  void* -- Response packet to Diag.

  @dependencies
  None.
*/
static void* pm_diag_pbs_info_log_stop(void)
{
  pm_diag_pbs_info_log_resp_type *stop_resp_ptr = NULL;

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for logging start response packet.                   */
  /*-----------------------------------------------------------------------*/

  stop_resp_ptr =
    (pm_diag_pbs_info_log_resp_type *)
      diagpkt_subsys_alloc(
        DIAG_SUBSYS_PM,
        PM_DIAG_PBS_INFO_LOG_CMD_ID,
        sizeof(pm_diag_pbs_info_log_resp_type));

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
  /* Return success is pbs_info logging is already stopped.             */
  /*-----------------------------------------------------------------------*/

  if (pm_pbs_info_diag.log_enabled_flag == FALSE)
  {
    stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
  }
  else
  {
    /*
     * Deinitialize logging.
     */
    if (DAL_SUCCESS != pm_diag_pbs_info_deinit_logging())
    {
      stop_resp_ptr->result_type = PM_DIAG_ERROR;
    }
    else
    {
      stop_resp_ptr->result_type = PM_DIAG_SUCCESS;
    }
  }

  return (void *)stop_resp_ptr;

} /* END pm_diag_pbs_info_log_stop */

/* =========================================================================
**  Function : pm_diag_pbs_info_log_handler
** =========================================================================*/
/**
  Handler to start/stop pbs_info status logging.

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
void * pm_diag_pbs_info_log_handler
(
  void  *req_ptr,   /* pointer to request packet */
  uint16 req_len /* length of request packet  */
)
{
  void                          *resp_ptr;
  pm_diag_pbs_info_log_req_type *pbs_info_req_ptr = (pm_diag_pbs_info_log_req_type*)req_ptr;

  /*-----------------------------------------------------------------------*/
  /* Validate the incoming packet is as expected.                          */
  /*-----------------------------------------------------------------------*/

  if (pbs_info_req_ptr == NULL)
  {
    return NULL;
  }

  if (req_len != sizeof(pm_diag_pbs_info_log_req_type))
  {
    resp_ptr = diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, sizeof(pm_diag_pbs_info_log_req_type));

    return (void *)resp_ptr;
  }

  /*-----------------------------------------------------------------------*/
  /* Start or stop the pbs_info status logging.                         */
  /*-----------------------------------------------------------------------*/

  if (pbs_info_req_ptr->status_flag)
  {
    resp_ptr = pm_diag_pbs_info_log_start();
  }
  else
  {
    resp_ptr = pm_diag_pbs_info_log_stop();
  }

  /*-----------------------------------------------------------------------*/
  /* Response packet is transmitted by dispatcher upon return.             */
  /*-----------------------------------------------------------------------*/

  return (void *)resp_ptr;

} /* END pm_diag_pbs_info_log_handler */


/* =========================================================================
**  Function : pm_diag_pbs_info_init
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
void pm_diag_pbs_info_init (void)
{
  pm_pbs_info_diag.log_enabled_flag = FALSE;
  pm_pbs_info_diag.num_pmics = 0;
  pm_pbs_info_diag.start_resp_size = 0;

  /*-----------------------------------------------------------------------*/
  /* Registers the pbs_info diag support with the dispatcher.                 */
  /*-----------------------------------------------------------------------*/

  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PM, pm_pbs_info_diag_table);

} /* END pm_diag_pbs_info_init */

