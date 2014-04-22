/*! \file rr_select_bcch.c

  This file implements the RR-SELECT-BCCH module.

  This is a general purpose module which allows for a given cell to be selected,
  early camping performed, and system information read. The type of early
  camping, and the specific system information required can be specified.
  Additionally, a reselection power-scan can be performed to verify the cell is
  present before attempting to select it.
  Upon completion of the required actions, a message is output to indicate
  completion.

  This module uses RR IMSGs to trigger the procedures, and indicates its result
  via a RR OMSG. There is no requirement on which parent entity sends and
  receives this messages.

  The IMSGs which can be received by this module are:
  RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ
  RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ
  RR_SELECT_BCCH_IMSG_SELECT_REQ
  RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND
  RR_SELECT_BCCH_IMSG_ABORT_REQ
  \see rr_select_bcch_imsg_e

  The OMSGs which can be output upon completion are:
  RR_SELECT_BCCH_OMSG_SELECT_CNF
  RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND
  RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND
  \see rr_select_bcch_omsg_e

                Copyright (c) 2010-2013 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_select_bcch.c#8 $ */
/* $DateTime: 2019/02/28 09:23:49 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_select_bcch_if.h"
#include "geran_msgs.h"
#include "ms.h"
#include "sys_type.h"
#include "rr_gprs_debug.h"
#include "rr_pscan.h"
#include "rr_decode_bcch.h"
#include "rr_l1_send.h"
#include "rr_general.h"
#include "rr_sys_info_pcell.h"
#include "rr_candidate_cell_db.h"
#include "gprs_mem.h"
#include "rr_l1_idle_mode.h"
#include "rr_multi_sim.h"
#include "rr_sys_info_cache.h"
#include "rr_sys_info_si.h"
#include "rr_sys_info_i.h"
#include "rr_nv.h"
#include "geran_grr_dyn_api.h"
#include "rr_static_general.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*!
  \enum rr_select_bcch_state_e
  \brief The states of the rr_select_bcch_control state machine

  RR_SELECT_BCCH_STATE_NULL
  In this state the module is inactive. It is waiting for a RR IMSG to trigger
  the procedure.

  RR_SELECT_BCCH_STATE_PSCAN
  A RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ was received, which has triggered a
  reselection power-scan. Waiting for results from L1.

  RR_SELECT_BCCH_STATE_LIST
  A power-scan was performed which returned multiple cells. The BCCH for each cell is
  decoded to assess suitability.

  RR_SELECT_BCCH_STATE_SELECT
  Either the reselection power-scan has completed and the requested cell was
  found, or a RR_SELECT_BCCH_IMSG_SELECT_REQ was received. L1 has been requested
  to select the cell.

  RR_SELECT_BCCH_STATE_ACQUIRE_SI
  L1 has confirmed selection of the cell and system information is being
  received.

  RR_SELECT_BCCH_STATE_ABORT
  An abort request has been received and a response from another module or L1 is
  pending before the abort is confirmed
*/
typedef enum
{
  RR_SELECT_BCCH_STATE_NULL,
  RR_SELECT_BCCH_STATE_PSCAN,
  RR_SELECT_BCCH_STATE_LIST,
  RR_SELECT_BCCH_STATE_SELECT,
  RR_SELECT_BCCH_STATE_ACQUIRE_SI,
  RR_SELECT_BCCH_STATE_ABORT,
  RR_SELECT_BCCH_STATE_MAX
} rr_select_bcch_state_e;

typedef enum
{
  RR_L1_EARLY_CAMP_NOT_REQUESTED,
  RR_L1_EARLY_CAMP_REQUESTED,
  RR_L1_EARLY_CAMP_CONFIRMED
} rr_l1_early_camp_status_e;

typedef struct
{
  ARFCN_T                           arfcn;
  BSIC_T                            bsic;
  byte                              rxlev_average;
  rr_candidate_cell_T              *candidate_cell_ptr;
  rr_cell_suitable_cb_fn_t          cell_suitable_cb_fn;
  boolean                           decode_si3_only_for_cell_suitable;
  rr_select_bcch_early_camp_e       early_camp_required;
  uint32                            notify_on_si_mask;
  uint32                            wait_for_si_mask;
  rr_l1_early_camp_status_e         early_camp_status;
  boolean                           si_notify_sent;
  uint32                            si_received_mask;
  boolean                           init_params_valid;
  rr_select_bcch_init_params_t      init_params;
  uint32                            csi_mask;
  uint32                            max_allowed_time_ms;
} rr_select_bcch_data_t;

/*!
  \struct rr_select_bcch_data_t
  \brief Internal module data

  This structure stores intenal runtime data for this module, including the
  options specified in the request message
  \see rr_select_bcch_imsg_pscan_select_req_t
  \see rr_select_bcch_imsg_select_req_t
*/
typedef struct
{
  rr_select_bcch_state_e            state;
  rr_select_bcch_state_e            old_state;
  rr_select_bcch_data_t            *data_ptr;
} rr_select_bcch_control_data_t;


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_select_bcch_control_data_t  rr_select_bcch_data;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 *
 * \return rr_select_bcch_control_data_t*
 */
static rr_select_bcch_control_data_t *rr_select_bcch_get_control_data_ptr( void )
{
  return &rr_select_bcch_data;
}

/*!
 * \brief Return a pointer to the module data.
 *
 * \param control_data_ptr (in) - pointer to the module data
 *
 * \return rr_select_bcch_data_t*
 */
static rr_select_bcch_data_t *rr_select_bcch_get_runtime_data_ptr( void )
{
  rr_select_bcch_control_data_t *control_data_ptr = rr_select_bcch_get_control_data_ptr();

  if (NULL == control_data_ptr->data_ptr)
  {
    ERR_GERAN_FATAL_0_G("control_data_ptr->data_ptr == NULL");
  }

  return control_data_ptr->data_ptr;
}

/*!
 * \brief Allocate memory for the run-time data.
 *
 * \param control_data_ptr (in) - pointer to the module data
 */
static rr_select_bcch_data_t *rr_select_bcch_alloc_data(rr_select_bcch_control_data_t *control_data_ptr)
{
  RR_NULL_CHECK_RETURN_PARAM(control_data_ptr, NULL);

  if (control_data_ptr->data_ptr == NULL)
  {
    control_data_ptr->data_ptr = GPRS_MEM_MALLOC(sizeof(rr_select_bcch_data_t));

    if (control_data_ptr->data_ptr != NULL)
    {
      memset(control_data_ptr->data_ptr, 0, sizeof(rr_select_bcch_data_t));
    }
    else
    {
      ERR_GERAN_FATAL_1("GPRS_MEM_MALLOC failed (%d)",sizeof(rr_select_bcch_data_t));
    }
  }

  return control_data_ptr->data_ptr;

} /* rr_select_bcch_alloc_data */

/*!
 * \brief Free the memory allocated for the run-time data.
 *
 * \param control_data_ptr (in) - pointer to the module data
 */
static void rr_select_bcch_free_data(rr_select_bcch_control_data_t *control_data_ptr)
{
  //RR_NULL_CHECK_RETURN_VOID(control_data_ptr);

  if (control_data_ptr->data_ptr != NULL)
  {
    GPRS_MEM_FREE(control_data_ptr->data_ptr);
    control_data_ptr->data_ptr = NULL;
  }

  return;

} /* rr_select_bcch_free_data */

/*!
 * \brief Returns a text string representing the given RR-SELECT-BCCH state.
 *
 * \param state
 *
 * \return const char*
 */
static const char *rr_select_bcch_state_name(rr_select_bcch_state_e state)
{
  switch (state)
  {
    case RR_SELECT_BCCH_STATE_NULL:       return "NULL";
    case RR_SELECT_BCCH_STATE_PSCAN:      return "PSCAN";
    case RR_SELECT_BCCH_STATE_LIST:       return "LIST";
    case RR_SELECT_BCCH_STATE_SELECT:     return "SELECT";
    case RR_SELECT_BCCH_STATE_ACQUIRE_SI: return "ACQUIRE_SI";
    case RR_SELECT_BCCH_STATE_ABORT:      return "ABORT";

    default:
      MSG_GERAN_LOW_1("Unknown state: %d", state);
  }

  return "?";

} /* rr_select_bcch_state_name */

/*!
 * \brief Setup function to validate and store the notify_on_si_mask and wait_for_si_mask SI bitmask.
 *
 *  This takes into account the early_camp_required mode indicated - if early camping based on SI3 is
 *  required, then SI3 will be added to the wait_for_si_mask if not already present.
 *
 * \param notify_on_si_mask
 * \param wait_for_si_mask
 * \param early_camp_required
 */
static void rr_select_bcch_select_setup_si_masks(
  uint32 notify_on_si_mask,
  uint32 wait_for_si_mask,
  rr_select_bcch_early_camp_e early_camp_required
   
)
{
  rr_select_bcch_data_t *data_ptr;

  // Obtain a pointer to the module data
  data_ptr = rr_select_bcch_get_runtime_data_ptr();

  data_ptr->wait_for_si_mask = wait_for_si_mask;

  // Add the 'notify' mask to the 'wait' mask. The reason for this is that the 'wait' mask should
  // ultimately be a superset of both the original values, because this module will wait for all SIs
  // indicated in the 'wait' mask before signalling completion
  data_ptr->wait_for_si_mask |= notify_on_si_mask;

  // Check that the 'wait' and 'notify' SI masks are equal, as there is no point in generating both
  // notification and completion indications at the same time
  if (wait_for_si_mask == notify_on_si_mask)
  {
    data_ptr->notify_on_si_mask = MASK_SI_NONE;
  }
  else
  {
    data_ptr->notify_on_si_mask = notify_on_si_mask;
  }

  // If early camp is required, ensure that SI3 is included in wait_for_si_mask
  if (early_camp_required != RR_SELECT_BCCH_EARLY_CAMP_NONE)
  {
    if ((data_ptr->wait_for_si_mask & MASK_SI_3) == 0)
    {
      MSG_GERAN_LOW_0_G("Adding SI3 to wait_for_si_mask");
      data_ptr->wait_for_si_mask |= MASK_SI_3;
    }
  }

  data_ptr->si_notify_sent = FALSE;
  data_ptr->si_received_mask = MASK_SI_NONE;

  return;

} /* rr_select_bcch_select_setup_si_masks */

/*!
 * \brief Setup the pending cell database reading for receiving SI messages.
 *
 * \param arfcn
 * \param bsic
 * \param rxlev_average
 */
static void rr_select_bcch_setup_sidb(
  ARFCN_T arfcn,
  BSIC_T bsic,
  byte rxlev_average,
  uint32 si_messages_on_bcch_ext,
  uint32 si_messages_not_broadcast
   
)
{
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

  //RR_NULL_CHECK_FATAL(pcell_info_ptr);

  rr_prepare_sidb_for_camping(
    &pcell_info_ptr->gsm,
    arfcn,
    bsic,
    rxlev_average
  );

  // This module doesn't require the use of these SI event generation flags
  pcell_info_ptr->gsm.generate_si_received_event = FALSE;

  // Preload SI message broadcast information
  pcell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext = si_messages_on_bcch_ext;
  pcell_info_ptr->bcch_sys_info.si_messages_not_broadcast = si_messages_not_broadcast;

  /**
   * In case fast SI acq is used, acqusition of SI2+variants may be skipped.
   * To ensure correct acquisition after camping, the BA-Ind is set to an invalid value, which is detected
   * after camping.
   * If fast SI acq is not used, then the BA-Ind will be set to the correct value when SI2 is received.
   */
   pcell_info_ptr->gsm.complete_BA_list.no_of_entries = 0;
   pcell_info_ptr->gsm.complete_idle_BA_list_received = FALSE;
   pcell_info_ptr->gsm.complete_BA_list.BA_IND        = 0x02;
   pcell_info_ptr->gsm.SI2_BA_list.BA_IND             = 0x02;
   pcell_info_ptr->gsm.SI2bis_BA_list.BA_IND          = 0x02;
   pcell_info_ptr->gsm.SI2ter_BA_list.BA_IND          = 0x02;

  return;

} /* rr_select_bcch_setup_sidb */

/*!
 * \brief Looks at wait_for_si_mask and determines a timeout value based on which SIs are being requested.
 *
 */
static void rr_select_bcch_start_sys_info_timer( )
{
  rr_select_bcch_data_t *data_ptr;

  // Obtain a pointer to the module data
  data_ptr = rr_select_bcch_get_runtime_data_ptr();

  if (data_ptr->wait_for_si_mask != MASK_SI_NONE)
  {
    uint32 timeout_ms = data_ptr->max_allowed_time_ms;

    if (timeout_ms == 0)
    {
      timeout_ms = RR_GSM_SYS_INFO_TIMEOUT;

      if (data_ptr->wait_for_si_mask & MASK_SI_13)
      {
        timeout_ms += RR_GPRS_SYS_INFO_TIMEOUT;
      }

      // Record the calculated value so that max_allowed_time_ms is not zero. This is checked when the
      // procedure completes to know whether to stop the timer or not
      data_ptr->max_allowed_time_ms = timeout_ms;
    }

    (void)rr_timer_start(RR_GSM_SYS_INFO_TIMER, timeout_ms);
  }

  return;

} /* rr_select_bcch_start_sys_info_timer */

/*!
 * \brief Setup function called when RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ is received.
 *
 *  The message contents are stored, and internal data setup appropriately.
 *
 * \param msg_ptr
 */
static void rr_select_bcch_pscan_select_setup(
  const rr_select_bcch_imsg_pscan_select_req_t *msg_ptr
   
)
{
  rr_select_bcch_control_data_t *control_data_ptr;
  rr_select_bcch_data_t *data_ptr;
  uint32 wait_for_si_mask;

  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  // Allocate the run-time data
  // Note: A NULL pointer check is made within this function
  data_ptr = rr_select_bcch_alloc_data(control_data_ptr);

  data_ptr->decode_si3_only_for_cell_suitable = TRUE;
  data_ptr->cell_suitable_cb_fn = msg_ptr->cell_suitable_cb_fn;

  wait_for_si_mask = msg_ptr->wait_for_si_mask;

  // If SI3 is specified in the mask, then early camping will be performed when SI3 is received
  if ((wait_for_si_mask & MASK_SI_3) != 0)
  {
    data_ptr->early_camp_required = RR_SELECT_BCCH_EARLY_CAMP_SI3;
  }
  else
  {
    data_ptr->early_camp_required = RR_SELECT_BCCH_EARLY_CAMP_NONE;
  }

  // If a cell suitable call-back function is supplied, check that a suitable SI was requested
  if (msg_ptr->cell_suitable_cb_fn != NULL)
  {
    // Either SI3 or SI4 can be used for checking - if neither are requested, add SI3 into the mask
    if ((wait_for_si_mask & (MASK_SI_3 | MASK_SI_4)) == 0)
    {
      MSG_GERAN_LOW_0_G("Adding SI3 to wait_for_si_mask to check cell suitability");
      wait_for_si_mask |= MASK_SI_3;
    }
  }

  rr_select_bcch_select_setup_si_masks(
    msg_ptr->notify_on_si_mask,
    wait_for_si_mask,
    data_ptr->early_camp_required
  );

  return;

} /* rr_select_bcch_pscan_select_setup */

/*!
 * \brief Setup function called when RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ is received.
 *
 *  The message contents are stored, and internal data setup appropriately.
 *
 * \param msg_ptr
 */
static void rr_select_bcch_pscan_list_select_setup(
  const rr_select_bcch_imsg_pscan_list_select_req_t *msg_ptr
   
)
{
  rr_select_bcch_control_data_t *control_data_ptr;
  rr_select_bcch_data_t *data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module control data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  // Allocate the run-time data
  // Note: A NULL pointer check is made within this function
  data_ptr = rr_select_bcch_alloc_data(control_data_ptr);

  data_ptr->decode_si3_only_for_cell_suitable = msg_ptr->decode_si3_only;
  data_ptr->cell_suitable_cb_fn = msg_ptr->cell_suitable_cb_fn;

  // If SI3 is specified in the mask, then early camping will be performed when SI3 is received
  if ((msg_ptr->wait_for_si_mask & MASK_SI_3) != 0)
  {
    data_ptr->early_camp_required = RR_SELECT_BCCH_EARLY_CAMP_SI3;
  }
  else
  {
    data_ptr->early_camp_required = RR_SELECT_BCCH_EARLY_CAMP_NONE;
  }

  // Note: SI3/4 for each cell in the list will be read during BCCH list decodes, so there is no need to
  // check that SI3/4 is included in the SI mask

  data_ptr->arfcn.num = INVALID_ARFCN;

  if (msg_ptr->max_allowed_time > 0)
  {
    MSG_GERAN_LOW_1_G("Starting overall guard timer (%d)",msg_ptr->max_allowed_time);
    (void)rr_timer_start(RR_CELL_SELECTION_ABORT_TIMER, msg_ptr->max_allowed_time);
  }

  rr_select_bcch_select_setup_si_masks(
    msg_ptr->notify_on_si_mask,
    msg_ptr->wait_for_si_mask,
    data_ptr->early_camp_required
  );

  return;

} /* rr_select_bcch_pscan_list_select_setup */

/*!
 * \brief Setup function called when RR_SELECT_BCCH_IMSG_SELECT_REQ is received.
 *
 *  The message contents are stored and internal data setup appropriately.
 *
 * \param msg_ptr
 */
static void rr_select_bcch_select_setup(
  const rr_select_bcch_imsg_select_req_t *msg_ptr
   
)
{
  rr_select_bcch_control_data_t *control_data_ptr;
  rr_select_bcch_data_t *data_ptr;
  uint32 wait_for_si_mask;
  BSIC_T dummy_bsic = {0};

  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module control data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  // Allocate the run-time data
  // Note: A NULL pointer check is made within this function
  data_ptr = rr_select_bcch_alloc_data(control_data_ptr);

  data_ptr->arfcn = msg_ptr->arfcn;
  data_ptr->rxlev_average = msg_ptr->rxlev_average;
  data_ptr->cell_suitable_cb_fn = msg_ptr->cell_suitable_cb_fn;
  data_ptr->early_camp_required = msg_ptr->early_camp_required;
  data_ptr->early_camp_status = RR_L1_EARLY_CAMP_NOT_REQUESTED;

  // Note: The BSIC may have been obtained during FCCH/SCH decodes, or may be set to a dummy value
  data_ptr->bsic = msg_ptr->bsic;

  wait_for_si_mask = msg_ptr->wait_for_si_mask;

  // If a cell suitable call-back function is supplied, check that a suitable SI was requested
  if (msg_ptr->cell_suitable_cb_fn != NULL)
  {
    // Either SI3 or SI4 can be used for checking - if neither are requested, add SI3 into the mask
    if ((wait_for_si_mask & (MASK_SI_3 | MASK_SI_4)) == 0)
    {
      MSG_GERAN_LOW_0_G("Adding SI3 to wait_for_si_mask to check cell suitability");
      wait_for_si_mask |= MASK_SI_3;
    }
  }

  rr_select_bcch_select_setup_si_masks(
    msg_ptr->notify_on_si_mask,
    wait_for_si_mask,
    msg_ptr->early_camp_required
  );

  rr_select_bcch_setup_sidb(
    msg_ptr->arfcn,
    dummy_bsic,
    msg_ptr->rxlev_average,
    MASK_SI_NONE,
    MASK_SI_NONE  );

  rr_select_bcch_start_sys_info_timer();

  return;

} /* rr_select_bcch_select_setup */

/*!
 * \brief Setup function called when RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ is received.
 *
 *  The message contents are stored and internal data setup appropriately..
 *
 * \param msg_ptr
 */
static void rr_select_bcch_acquire_si_setup(
  const rr_select_bcch_imsg_acquire_si_req_t *msg_ptr
   
)
{
  rr_select_bcch_control_data_t *control_data_ptr;
  rr_select_bcch_data_t *data_ptr;
  uint32 wait_for_si_mask;

  uint32 si_messages_on_bcch_ext = MASK_SI_NONE;
  uint32 si_messages_not_broadcast = MASK_SI_NONE;

  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module control data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  // Allocate the run-time data
  // Note: A NULL pointer check is made within this function
  data_ptr = rr_select_bcch_alloc_data(control_data_ptr);

  data_ptr->arfcn = msg_ptr->arfcn;
  data_ptr->rxlev_average = msg_ptr->rxlev_average;
  data_ptr->bsic = msg_ptr->bsic;
  data_ptr->cell_suitable_cb_fn = msg_ptr->cell_suitable_cb_fn;
  data_ptr->early_camp_required = msg_ptr->early_camp_required;
  data_ptr->early_camp_status = RR_L1_EARLY_CAMP_NOT_REQUESTED;

  rr_si_print_bitmask("wait_for_si_mask", msg_ptr->wait_for_si_mask);

  if (msg_ptr->init_params_valid)
  {
    MSG_GERAN_HIGH_2_G(
      "Init params supplied: si_on_bcch_ext=0x%x si_not_broadcast=0x%x",
      (int)msg_ptr->init_params.si_messages_on_bcch_ext,
      (int)msg_ptr->init_params.si_messages_not_broadcast
    );

    rr_si_print_bitmask("si_messages_on_bcch_ext", msg_ptr->init_params.si_messages_on_bcch_ext);
    rr_si_print_bitmask("si_messages_not_broadcast", msg_ptr->init_params.si_messages_not_broadcast);

    data_ptr->init_params = msg_ptr->init_params;
    data_ptr->init_params_valid = TRUE;

    si_messages_on_bcch_ext = msg_ptr->init_params.si_messages_on_bcch_ext;
    si_messages_not_broadcast = msg_ptr->init_params.si_messages_not_broadcast;
  }

  wait_for_si_mask = msg_ptr->wait_for_si_mask;

  // If a cell suitable call-back function is supplied, check that a suitable SI was requested
  if (msg_ptr->cell_suitable_cb_fn != NULL)
  {
    // Either SI3 or SI4 can be used for checking - if neither are requested, add SI3 into the mask
    if ((wait_for_si_mask & (MASK_SI_3 | MASK_SI_4)) == 0)
    {
      MSG_GERAN_LOW_0_G("Adding SI3 to wait_for_si_mask to check cell suitability");
      wait_for_si_mask |= MASK_SI_3;
    }
  }

  data_ptr->max_allowed_time_ms = msg_ptr->max_allowed_time_ms;

  rr_select_bcch_select_setup_si_masks(
    msg_ptr->notify_on_si_mask,
    wait_for_si_mask,
    msg_ptr->early_camp_required
  );

  rr_select_bcch_setup_sidb(
    msg_ptr->arfcn,
    msg_ptr->bsic,
    msg_ptr->rxlev_average,
    si_messages_on_bcch_ext,
    si_messages_not_broadcast
  );

  if (data_ptr->max_allowed_time_ms != 0)
  {
    rr_select_bcch_start_sys_info_timer();
  }

  return;

} /* rr_select_bcch_acquire_si_setup */

/*!
 * \brief Starts a reselection power-scan to verify the presence of the cell indicated.
 *
 * \param arfcn_list
 * \param num_of_arfcns
 */
static void rr_select_bcch_start_pscan(ARFCN_T *arfcn_list, uint16 num_of_arfcns )
{
  sys_band_mask_type band_mask = SYS_BAND_MASK_EMPTY;
  boolean mem_free_required = TRUE;

  //RR_NULL_CHECK_RETURN_VOID(arfcn_list);

  // Start reselection power-scan
  // Note: For this type of power-scan, the band_pref is irrelevant for the scan,
  // but it used during post-processing the results

  // If num_of_arfcns is zero, then this indicates that arfcn_list is not a cell list, but a single
  // ARFCN whose memory is not allocated
  if (num_of_arfcns == 0)
  {
    mem_free_required = FALSE;
    num_of_arfcns = 1;
  }

  if (num_of_arfcns == 1)
  {
    ARFCN_T arfcn = arfcn_list[0];

    if (arfcn.band == SYS_BAND_EGSM_900)
    {
      band_mask = SYS_BAND_MASK_GSM_EGSM_900;
    }
    else
    if (arfcn.band == SYS_BAND_PGSM_900)
    {
      band_mask = SYS_BAND_MASK_GSM_PGSM_900;
    }
    else
    if (arfcn.band == SYS_BAND_PCS_1900)
    {
      band_mask = SYS_BAND_MASK_GSM_PCS_1900;
    }
    else
    if (arfcn.band == SYS_BAND_DCS_1800)
    {
      band_mask = SYS_BAND_MASK_GSM_DCS_1800;
    }
    else
    if (arfcn.band == SYS_BAND_CELL_850)
    {
      band_mask = SYS_BAND_MASK_GSM_850;
    }
  }
  else
  {
    band_mask = rr_band_pref();
  }

  rr_candidate_db_clear(rr_get_candidate_cell_db_ptr());
  rr_pscan_setup();
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  rr_log_reselection_power_scan_started();
#endif

  rr_pscan_add_frequencies(
    arfcn_list,
    num_of_arfcns
  );

  // arfcn_list is no longer needed - free the memory
  if (mem_free_required)
  {
    GPRS_MEM_FREE(arfcn_list);
  }

  rr_pscan_send_start_ind(
    RR_SEARCH_RESELECTION,    // search_mode
    band_mask                // band_pref
  );

  return;

} /* rr_select_bcch_start_pscan */

/*!
 * \brief Start BCCH list decodes, specifying SI3-only.
 *
 */
static void rr_select_bcch_start_list_decodes( )
{
  rr_decode_bcch_list_setup();

  (void) rr_candidate_db_send_decode_bcch_start_ind(
    RR_SEARCH_NORMAL,    // bcch_decode_type
    TRUE,                // si3_only
    SYS_BAND_MASK_EMPTY // band filter
  );

  return;

} /* rr_select_bcch_start_list_decodes */

/*!
 * \brief Indicates if this module is using the candidate cell database for its procedures.
 *
 *  Returns TRUE if this module is using the candidate cell database during the ongoing selection/acquisition
 *  procedure. It may be that the caller supplied an ARFCN/BSIC directly to select a cell, and therefore
 *  the candidate cell database is not used.

 *
 * \return boolean - TRUE if the candidate cell database is being used; FALSE otherwise
 */
static boolean rr_select_bcch_is_using_candidate_cell_db( )
{
  // Obtain a pointer to the module data
  rr_select_bcch_data_t *data_ptr = rr_select_bcch_get_runtime_data_ptr();

  return (data_ptr->candidate_cell_ptr != NULL);

} /* rr_select_bcch_is_using_candidate_cell_db */

/*!
 * \brief Updates the currently selected cell's 'type' in the candidate cell database.
 *
 * \param type
 */
static void rr_select_bcch_update_candidate_cell_type(
  rr_candidate_freq_type_enum_T type
   
)
{
  // Obtain a pointer to the module data
  rr_select_bcch_data_t *data_ptr = rr_select_bcch_get_runtime_data_ptr();

  if (data_ptr->candidate_cell_ptr != NULL)
  {
    data_ptr->candidate_cell_ptr->type = type;
  }

  return;

} /* rr_select_bcch_update_candidate_cell_type */

/*!
 * \brief Finds the next best suitable cell from the candidate database.
 *
 *  Finds the next best suitable cell from the candidate database. To verify the cell's suitability the
 *  caller's call-back function is called.
 *
 *
 * \return rr_candidate_cell_T*
 */
static rr_candidate_cell_T *rr_select_bcch_next_candidate_cell( )
{
  uint16 num_of_cells;
  rr_candidate_cell_T *candidate_cell_ptr;
  rr_select_bcch_data_t *data_ptr;
  rr_candidate_cell_db_T *candidate_db_ptr;
  boolean suitable_cell_found = FALSE;

  // Obtain a pointer to the module data
  data_ptr = rr_select_bcch_get_runtime_data_ptr();

  candidate_db_ptr = rr_get_candidate_cell_db_ptr();
  //RR_NULL_CHECK_FATAL(candidate_db_ptr);

  // Return the number of cells to be processed
  num_of_cells = rr_candidate_db_get_num_unprocessed_cell(candidate_db_ptr);

  while (num_of_cells > 0)
  {
    boolean cell_found;

    // Find the first unprocessed cell
    cell_found = rr_candidate_db_find_next(
      candidate_db_ptr,
      &candidate_cell_ptr,
      RR_CANDIDATE_STRONGEST_FIRST
    );

    if (cell_found && candidate_cell_ptr)
    {
      if (data_ptr->cell_suitable_cb_fn)
      {
        suitable_cell_found = (*data_ptr->cell_suitable_cb_fn)(
          candidate_cell_ptr
        );

        MSG_GERAN_MED_2_G("Cell %d is suitable =%d",(int)candidate_cell_ptr->meas.arfcn_num, suitable_cell_found);
      }
      else
      {
        MSG_GERAN_MED_1_G("Cell %d assumed suitable",(int)candidate_cell_ptr->meas.arfcn_num);
        suitable_cell_found = TRUE;
      }

      // Mark this candidate cell as processed so it won't be tried again
      candidate_cell_ptr->processed = TRUE;

      // If a suitable cell has been found then exit the while loop
      if (suitable_cell_found)
      {
        break;
      }
    }
    else
    {
      --num_of_cells;
    }
  }

  if (suitable_cell_found)
  {
    return candidate_cell_ptr;
  }

  return NULL;

} /* rr_select_bcch_next_candidate_cell */

/*!
 * \brief This function is called when L1 indicates the parallel BCCH decodes are completed.
 *
 *  This function is called when L1 indicates the parallel BCCH decodes are completed. Any cells where BCCH
 *  decoding failed are marked as invalid.
 *  It returns the best suitable cell from the candidate database.
 *
 *
 * \return rr_candidate_cell_T* - the best suitable cell from the candidate database.
 */
static rr_candidate_cell_T *rr_select_bcch_process_list_decodes( )
{
  rr_candidate_cell_db_T *candidate_db_ptr;

  candidate_db_ptr = rr_get_candidate_cell_db_ptr();
  //RR_NULL_CHECK_FATAL(candidate_db_ptr);

  // Discard any entries with BCCH decode failures
  rr_candidate_db_mark_invalid_entries_as_processed(candidate_db_ptr);

  return rr_select_bcch_next_candidate_cell();

} /* rr_select_bcch_process_list_decodes */

/*!
 * \brief Send MPH_SELECT_SPECIFIC_BCCH_REQ for the selected cell.
 *
 */
static void rr_select_bcch_start_select( )
{
  // Obtain a pointer to the module data
  rr_select_bcch_data_t *data_ptr = rr_select_bcch_get_runtime_data_ptr();

  rr_send_mph_select_specific_channel(data_ptr->arfcn, data_ptr->bsic);

  return;

} /* rr_select_bcch_start_select */

/*!
 * \brief Send MPH_SELECT_SPECIFIC_BCCH_REQ for the selected cell following the reselection power-scan.
 *
 *  Following the reselection power-scan initiated in response to a RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ,
 *  this function then uses the parameters supplied in that original message to send a
 *  RR_SELECT_BCCH_IMSG_SELECT_REQ to request selecting the BCCH.
 *
 * \param candidate_cell_ptr
 */
static void rr_select_bcch_start_select_after_list_decodes(
  rr_candidate_cell_T *candidate_cell_ptr
   
)
{
  uint32 si_messages_on_bcch_ext = MASK_SI_NONE;
  uint32 si_messages_not_broadcast = MASK_SI_NONE;
  ARFCN_T arfcn;
  rr_select_bcch_data_t *data_ptr;

  arfcn.num = candidate_cell_ptr->meas.arfcn_num;
  arfcn.band = candidate_cell_ptr->meas.arfcn_band;
  // Obtain a pointer to the module data
  data_ptr = rr_select_bcch_get_runtime_data_ptr();

  // Sanity-check the input
  //RR_NULL_CHECK_FATAL(candidate_cell_ptr);

  data_ptr->arfcn = arfcn;
  data_ptr->bsic = candidate_cell_ptr->bsic;
  data_ptr->early_camp_status = RR_L1_EARLY_CAMP_NOT_REQUESTED;

  rr_candidate_get_bsi_scheduling_info(
    candidate_cell_ptr,
    &si_messages_on_bcch_ext,
    &si_messages_not_broadcast
  );

  rr_select_bcch_setup_sidb(
    arfcn,
    candidate_cell_ptr->bsic,
    candidate_cell_ptr->meas.RXLEV_average,
    si_messages_on_bcch_ext,
    si_messages_not_broadcast
  );

  rr_select_bcch_start_sys_info_timer();

  // Store the candidate cell entry for the selected cell for use later
  data_ptr->candidate_cell_ptr = candidate_cell_ptr;

  rr_send_mph_select_specific_channel(data_ptr->arfcn, data_ptr->bsic);

  return;

} /* rr_select_bcch_start_select_after_list_decodes */

/*!
 * \brief Called when a SI message is received. It updates the SI message tracking.
 *
 * Called when a SI message is received. This updates the accumulated SI received mask and checks whether
 * all SIs requested have now been received.
 *
 * \param si_received - (in) SI received
 * \param si_received_mask - (out) Accumulated SI received
 *
 * \return boolean - TRUE if all SIs requested have now been received; FALSE otherwise
 */
static boolean rr_select_bcch_check_si_received(
  rr_si_type_t si_received,
  uint32 *si_received_mask
   
)
{
  uint32 si_received_or_not_broadcast;
  gprs_scell_info_t *pcell_info_ptr;
  rr_select_bcch_data_t *data_ptr;

  // Obtain a pointer to the module data
  data_ptr = rr_select_bcch_get_runtime_data_ptr();

  pcell_info_ptr = rr_gprs_get_pcell_info();
  //RR_NULL_CHECK_FATAL(pcell_info_ptr);

  // Add the received SI to the mask which shows the accumulated SIs received
  data_ptr->si_received_mask |= (SI_TYPE_TO_MASK(si_received));

  // Mask indicating SI not being broadcast (1=not broadcast)
  //uint16 si_not_broadcast = pcell_info_ptr->bcch_sys_info.si_messages_not_broadcast;
  /*
  TODO: To determine whether the set of requested SIs has been received, then as
  well as the SI just received, it is necessary to know which SIs are not being
  broadcast. At present, si_messages_not_broadcast is only partially correct,
  because it includes SI13 but not SI2bis.
  */
  // Find the total of the SIs received and those not being broadcast
  //si_received_or_not_broadcast = (data_ptr->si_received_mask | si_not_broadcast);
  si_received_or_not_broadcast = pcell_info_ptr->gsm.update_message_received;

  // Update accumulated SI parameter, if supplied
  if (si_received_mask != NULL)
  {
    *si_received_mask = si_received_or_not_broadcast;
  }

  if (data_ptr->wait_for_si_mask != MASK_SI_NONE)
  {
    if ((data_ptr->wait_for_si_mask & (~si_received_or_not_broadcast)) == MASK_SI_NONE)
    {
      // All SI requested have been received or are not being broadcast.
      return TRUE;
    }
  }

  return FALSE;

} /* rr_select_bcch_check_si_received */

/*!
 * \brief Called when the procedure completes (success or fail). An OMSG is output to RR.
 *
 * Called when the procedure completes (success or fail).
 * RR_SELECT_BCCH_OMSG_SELECT_CNF is output if the procedure succeeded.
 * RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND is output if the procedure failed.
 *
 * \param reason - indicates if the procedure was successful or failed (and why)
 */
static void rr_select_bcch_finished(rr_select_bcch_fail_reason_e reason )
{
  rr_select_bcch_control_data_t *control_data_ptr;
  rr_select_bcch_data_t *data_ptr;

  // Assume L1 is Early Camped
  boolean early_camped = TRUE;

  // Obtain a pointer to the module data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  // Pointer to the run-time data
  data_ptr = control_data_ptr->data_ptr;
  //RR_NULL_CHECK_FATAL(data_ptr);

  (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER);

  // Check if this module was running the SI acquisition timer, or if this was done externally
  if (data_ptr->max_allowed_time_ms != 0)
  {
    (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER);
  }

  if (data_ptr->early_camp_status == RR_L1_EARLY_CAMP_NOT_REQUESTED)
  {
    early_camped = FALSE;
  }

  if (reason == RR_SELECT_BCCH_FAIL_REASON_NONE)
  {
    rr_select_bcch_send_omsg_select_cnf(
      data_ptr->arfcn,
      data_ptr->bsic,
      early_camped,
      data_ptr->si_received_mask
    );
  }
  else
  {
    rr_select_bcch_send_omsg_select_fail_ind(
      data_ptr->arfcn,
      early_camped,
      reason
    );
  }

  rr_select_bcch_free_data(control_data_ptr);

#ifdef FEATURE_LTE_TO_GSM_CGI
  if(!rr_is_control_state(RR_X2G_CGI))
  {
#endif /* FEATURE_LTE_TO_GSM_CGI */
#ifdef FEATURE_RR_SI_CACHE
    rr_csi_deactivate_pend_store();
#endif
#ifdef FEATURE_LTE_TO_GSM_CGI
  }
#endif /* FEATURE_LTE_TO_GSM_CGI */

  return;
}

#ifdef FEATURE_RR_SI_CACHE
static uint32 rr_select_bcch_csi_retrieve_all( )
{
  // Obtain a pointer to the module data
  rr_select_bcch_data_t *data_ptr = rr_select_bcch_get_runtime_data_ptr();

  // Retrieve all available data from the sys info cache
  data_ptr->csi_mask = rr_csi_retrieve_all_SIs(
    data_ptr->arfcn,
    &data_ptr->init_params.lai,
    &data_ptr->init_params.ci,
    CSI_RETRIEVE_ALL_SIS
  );

  if (data_ptr->csi_mask == MASK_SI_NONE)
  {
    MSG_GERAN_LOW_0_G("No SIs in cache");
  }
  else
  {
    MSG_GERAN_LOW_1_G("SIs in cache: 0x%x",(int)data_ptr->csi_mask);
  }

  return data_ptr->csi_mask;
}
#endif

static void rr_select_bcch_send_l1_read_sys_info_req(uint32 si_received_mask  )
{
  if (rr_nv_iterative_si_acq_is_enabled())
  {
    gprs_scell_info_t *pcell_ptr = rr_gprs_get_pcell_info();
    uint32 si_messages_not_broadcast = rr_si_messages_not_broadcast(pcell_ptr);

    // Start by setting the mask to everything
    uint32 remaining_si_mask = MASK_FULL_BSI_ACQUISITION;

    /* Add SI21/SI9 mask if they are required */
    if(pcell_ptr->bcch_sys_info.si_messages_required & MASK_SI_21)
    {
      remaining_si_mask |= MASK_SI_21;
    }
    else if(pcell_ptr->bcch_sys_info.si_messages_required & MASK_SI_9)
    {
      remaining_si_mask |= MASK_SI_9;
    }
    
    // Now mask off SI already received
    remaining_si_mask &= (~si_received_mask);

    // Now mask off SI not being broadcast
    remaining_si_mask &= (~si_messages_not_broadcast);

    // Now request the remaining SIs from L1
    rr_si_send_l1_read_sys_info_req(remaining_si_mask);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Iteractive SI acq disabled");
  }

  return;
}

static void rr_select_bcch_start_early_camp( )
{
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

  rr_l1_idle_send_imsg_start_early_camp_req(
    pcell_info_ptr->gsm.BCCH_ARFCN,
    pcell_info_ptr->gsm.BSIC,
    pcell_info_ptr->gsm.location_area_identification,
    pcell_info_ptr->gsm.cell_identity,
    pcell_info_ptr->gsm.control_channel_description,
    pcell_info_ptr->gsm.control_channel_description_valid
  );

  rr_select_bcch_get_runtime_data_ptr()->early_camp_status = RR_L1_EARLY_CAMP_REQUESTED;

  return;
}

static void rr_select_bcch_start_early_camp_using_init_params( )
{
  control_channel_description_T ccd;

  // Obtain a pointer to the module data
  rr_select_bcch_data_t *data_ptr = rr_select_bcch_get_runtime_data_ptr();

  memset(&ccd, 0, sizeof(ccd));

  // Note: Only a sub-set of the CCD parameters are required for Early Camping
  ccd.BS_AG_BLKS_RES = data_ptr->init_params.bs_ag_blks_res;
  ccd.BS_CC_CHANS = data_ptr->init_params.bs_cc_chans;
  ccd.BS_CCCH_SDCCH_COMB = data_ptr->init_params.bs_ccch_sdcch_comb;
  ccd.BS_PA_MFRMS = data_ptr->init_params.bs_pa_mfrms;

  rr_l1_idle_send_imsg_start_early_camp_req(
    data_ptr->arfcn,
    data_ptr->bsic,
    data_ptr->init_params.lai,
    data_ptr->init_params.ci,
    ccd,
    TRUE
  );

  data_ptr->early_camp_status = RR_L1_EARLY_CAMP_REQUESTED;

  return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Initialisation function. Resets the state of the module state machine.
 *
 */
void rr_select_bcch_init( )
{
  rr_select_bcch_control_data_t *control_data_ptr;

  // Obtain a pointer to the module data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  control_data_ptr->old_state = RR_SELECT_BCCH_STATE_NULL;
  control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
  control_data_ptr->data_ptr = NULL;

  return;

} /* rr_select_bcch_init() */

/*!
 * \brief The RR-SELECT-BCCH module state machine.
 *
 * \param msg_ptr
 */
void rr_select_bcch_control(rr_cmd_bdy_type *msg_ptr  )
{
  rr_select_bcch_control_data_t *control_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  control_data_ptr = rr_select_bcch_get_control_data_ptr();

  switch (control_data_ptr->state)
  {
    case RR_SELECT_BCCH_STATE_NULL:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_IMSG:
            {
              switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ:
                {
                  rr_select_bcch_imsg_pscan_select_req_t *imsg_ptr;

                  imsg_ptr = &msg_ptr->rr.select_bcch.imsg.rr_select_bcch_imsg_pscan_select_req;

                  rr_select_bcch_pscan_select_setup(imsg_ptr);
                  //RR_NULL_CHECK_RETURN_VOID(control_data_ptr->data_ptr);

                  rr_select_bcch_start_pscan(&imsg_ptr->arfcn, 0);
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_PSCAN;
                  break;
                }

                case RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ:
                {
                  rr_select_bcch_imsg_pscan_list_select_req_t *imsg_ptr;

                  imsg_ptr = &msg_ptr->rr.select_bcch.imsg.rr_select_bcch_imsg_pscan_list_select_req;

                  rr_select_bcch_pscan_list_select_setup(imsg_ptr);
                  //RR_NULL_CHECK_RETURN_VOID(control_data_ptr->data_ptr);

                  rr_select_bcch_start_pscan(imsg_ptr->cell_list, imsg_ptr->num_of_cells);
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_PSCAN;
                  break;
                }

                case RR_SELECT_BCCH_IMSG_SELECT_REQ:
                {
                  rr_select_bcch_imsg_select_req_t *imsg_ptr;

                  imsg_ptr = &msg_ptr->rr.select_bcch.imsg.rr_select_bcch_imsg_select_req;

                  rr_select_bcch_select_setup(imsg_ptr);
                  //RR_NULL_CHECK_RETURN_VOID(control_data_ptr->data_ptr);

                  rr_select_bcch_start_select();
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_SELECT;
                  break;
                }

                case RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ:
                {
                  rr_select_bcch_imsg_acquire_si_req_t *imsg_ptr;

                  imsg_ptr = &msg_ptr->rr.select_bcch.imsg.rr_select_bcch_imsg_acquire_si_req;

                  rr_select_bcch_acquire_si_setup(imsg_ptr);
                  //RR_NULL_CHECK_RETURN_VOID(control_data_ptr->data_ptr);

                  /**
                   * There are two ways to obtain the system information. These are, in order of preference:
                   * 1. Use the sys info cache
                   * 2. Read from BCCH
                   *
                   * There are three ways of obtaining the data to start Early Camping. These are, in order of
                   * preference:
                   * 1. Use the Early Camping data included in the initial parameters
                   * 2. Use SI3 from the sys info cache
                   * 3. Read SI3 from BCCH
                   *
                   * If initial parameters are supplied, then the data included is used to enable the use of
                   * the sys info cache. If the early camp preference is set to
                   * RR_SELECT_BCCH_EARLY_CAMP_USE_PARAMS, then the Early Camping is done using the init
                   * parameters data, otherwise using SI3 from the cache.
                   */

                  // Check if initial paramters were supplied
                  if (imsg_ptr->init_params_valid)
                  {
                    // If Early Camping was requested using the initial parameters, do this now
                    if (RR_SELECT_BCCH_EARLY_CAMP_USE_PARAMS == imsg_ptr->early_camp_required)
                    {
                      rr_select_bcch_start_early_camp_using_init_params();
                    }
                  }
#ifdef FEATURE_RR_SI_CACHE
                  rr_csi_activate_pend_store();
#endif
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_ACQUIRE_SI;
                  break;
                }

                case RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND:
                {
                  break;  // Ignore SI received messages when not active
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_SELECT_BCCH_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_TIMER:
        {
          rr_timer_id_T timer_id = (rr_timer_id_T)msg_ptr->mid_timer_expiry.timer_id;

          switch (timer_id)
          {
            case RR_CELL_SELECTION_ABORT_TIMER:
            {
              MSG_GERAN_HIGH_0_G("Procedure failed - timeout");
              control_data_ptr->state = RR_SELECT_BCCH_STATE_ABORT;
              break;
            }

            default:
            {
              MSG_GERAN_LOW_1_G("Unhandled timer (%d)",(int)timer_id);
            }
          }
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_select_bcch_state_name(control_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_SELECT_BCCH_STATE_NULL */

    case RR_SELECT_BCCH_STATE_PSCAN:
    {
      //RR_NULL_CHECK_FATAL(control_data_ptr->data_ptr);

      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_IMSG:
            {
              switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_IMSG_ABORT_REQ:
                {
                  rr_pscan_send_abort_req();
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_ABORT;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_SELECT_BCCH_IMSG */

            case RR_PSCAN_OMSG:
            {
              switch (msg_ptr->rr.pscan.omsg.header.imh.message_id)
              {
                case RR_PSCAN_OMSG_FINISHED_IND:
                {
                  uint16 num_freqs_found;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
                  rr_log_reselection_power_scan_completed();
#endif
                  num_freqs_found = rr_pscan_process_power_scan_results(RR_NORMAL_SEARCH_ORDER);

                  if (num_freqs_found == 0)
                  {
                    MSG_GERAN_HIGH_0_G("No freqs found during pscan");

                    // Procedure finished but no cells found
                    rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_FOUND);
                    control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                  }
                  else
                  {
                    /**
                     * One or more potential cells are found. Perform BCCH list decodes to check cell 
                     * suitability. 
                     * Note: If only one cell is found, the BCCH list decodes could be skipped and the cell 
                     * selected. But this would mean that SI3 is not available going into the selection phase 
                     * of the procedure, which means Early Camping cannot be done and the sys info cache can't 
                     * be used, which complicates the implementation. 
                     */
                    rr_select_bcch_start_list_decodes();
                    control_data_ptr->state = RR_SELECT_BCCH_STATE_LIST;
                  }

                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_PSCAN_OMSG */

            case RR_PSCAN_IMSG:
            {
              rr_pscan_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            } /* RR_PSCAN_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          rr_pscan_control(EV_INPUT_MESSAGE, msg_ptr);
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_select_bcch_state_name(control_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_SELECT_BCCH_STATE_PSCAN */

    case RR_SELECT_BCCH_STATE_LIST:
    {
      //RR_NULL_CHECK_FATAL(control_data_ptr->data_ptr);

      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_IMSG:
            {
              switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_IMSG_ABORT_REQ:
                {
                  rr_decode_bcch_send_abort_req();
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_ABORT;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_SELECT_BCCH_IMSG */

            case RR_DECODE_BCCH_OMSG:
            {
              switch (msg_ptr->rr.decode_bcch.imsg.header.imh.message_id)
              {
                case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                {
                  rr_candidate_cell_T *candidate_cell_ptr = rr_select_bcch_process_list_decodes();

                  if (candidate_cell_ptr != NULL)
                  {
                    rr_select_bcch_start_select_after_list_decodes(candidate_cell_ptr);
                    control_data_ptr->state = RR_SELECT_BCCH_STATE_SELECT;
                  }
                  else
                  {
                    rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_FOUND);
                    control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_DECODE_BCCH_OMSG */

            case RR_DECODE_BCCH_IMSG:
            {
              rr_decode_bcch_list_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            } /* RR_DECODE_BCCH_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        case MS_TIMER:
        {
          rr_decode_bcch_list_control(EV_INPUT_MESSAGE, msg_ptr);
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_select_bcch_state_name(control_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_SELECT_BCCH_STATE_LIST */

    case RR_SELECT_BCCH_STATE_SELECT:
    {
      //RR_NULL_CHECK_FATAL(control_data_ptr->data_ptr);

      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_IMSG:
            {
              switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_IMSG_ABORT_REQ:
                {
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_ABORT;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_SELECT_BCCH_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MPH_SELECT_SPECIFIC_BCCH_CNF:
            {
              mph_select_specific_bcch_cnf_T *l1_msg_ptr = &msg_ptr->mph_select_specific_bcch_cnf;

              if (l1_msg_ptr->BCCH_found)
              {
                BSIC_T bsic;
                gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

                //RR_NULL_CHECK_FATAL(pcell_info_ptr);

                if (rr_select_bcch_is_using_candidate_cell_db())
                {
                  // Mark this cell as having a valid BCCH in the candidate cell database.
                  // This won't have been done if there is only one candidate cell (whereas if there
                  // are multiple candidate cells, this would have been done already during processing
                  // of BCCH list decodes)
                  rr_select_bcch_update_candidate_cell_type(RR_CANDIDATE_VALID_BCCH);
                }

                rr_compute_BSIC_from_SCH_block_dyn(
                  &bsic,
                  l1_msg_ptr->SCH_block
                );

                control_data_ptr->data_ptr->bsic = bsic;
                pcell_info_ptr->gsm.BSIC = bsic;

                // This module doesn't require the use of these SI event generation flags
                pcell_info_ptr->gsm.generate_si_received_event = FALSE;

                if (control_data_ptr->data_ptr->wait_for_si_mask == MASK_SI_NONE)
                {
                  rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_NONE);
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                }
                else
                {
#ifdef FEATURE_RR_SI_CACHE
                  rr_csi_activate_pend_store();
#endif
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_ACQUIRE_SI;
                }
              }
              else
              {
                if (rr_select_bcch_is_using_candidate_cell_db())
                {
                  // Mark this cell as having an invalid BCCH in the candidate cell database
                  rr_select_bcch_update_candidate_cell_type(RR_CANDIDATE_NOT_BCCH);
                }

                rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_FOUND);
                control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
              }

              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_select_bcch_state_name(control_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_SELECT_BCCH_STATE_SELECT */

    case RR_SELECT_BCCH_STATE_ACQUIRE_SI:
    {
      //RR_NULL_CHECK_FATAL(control_data_ptr->data_ptr);

      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_IMSG:
            {
              switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND:
                {
                  rr_select_bcch_imsg_bsi_received_ind_t *imsg_ptr;
                  rr_select_bcch_data_t *data_ptr = control_data_ptr->data_ptr;

                  /**
                   * Assume the cell is suitable. If a cell suitability call-back function was supplied (which
                   * is optional), this will be called and cell_suitable updated with the result
                   */
                  boolean cell_suitable = TRUE;

                  imsg_ptr = &msg_ptr->rr.select_bcch.imsg.rr_select_bcch_imsg_bsi_received_ind;

                  if (imsg_ptr->si_received != SI_TYPE_NONE)
                  {
                    // A sys info message has been received (from a BCCH read or the sys info cache)

                    if (imsg_ptr->si_received == SI_TYPE_SI_3)
                    {
                      // If a function was supplied to check cell suitability, call it now
                      if (control_data_ptr->data_ptr->cell_suitable_cb_fn)
                      {
                        cell_suitable = data_ptr->cell_suitable_cb_fn(
                          control_data_ptr->data_ptr->candidate_cell_ptr
                        );
                      }

                      if (cell_suitable)
                      {
                        /**
                         * The cell is suitable. 
                         *  
                         * Now that SI3 has been received, if Early Camping is required and has not been done 
                         * yet (i.e. no initial parameters were supplied), then this is done now.
                         */
                        if (data_ptr->early_camp_required &&
                            (data_ptr->early_camp_status == RR_L1_EARLY_CAMP_NOT_REQUESTED))
                        {
                          gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

                          if (pcell_info_ptr->gsm.cell_identity_valid_flag)
                          {                          
                            data_ptr->init_params.lai = pcell_info_ptr->gsm.location_area_identification;
                            data_ptr->init_params.ci = pcell_info_ptr->gsm.cell_identity;
                          } 

                          rr_select_bcch_start_early_camp();
                        }
                      }
                      else
                      {
                        // Procedure finished, but cell not suitable
                        rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_SUITABLE);
                        control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                      }
                    } // end SI3 received

                    if (cell_suitable)
                    {
                      uint32 si_received_mask;
                      boolean finished;

                      /**
                       * Check is all requested SI have now been received. 
                       * This also returns a mask of all the SI received so far (si_received_mask). 
                       * Note: This include SI which are not being broadcast. 
                       */
                      finished = rr_select_bcch_check_si_received(
                        imsg_ptr->si_received,
                        &si_received_mask
                      );

                      if (finished)
                      {
                        /**
                         * If SI3 has just been received and completes the acquisition, then if initial 
                         * parameters were not supplied and Early Camping requested, then L1 will have just 
                         * been put in Early Camping. Wait for the confirmation from L1 before completing. 
                         */
                        if (data_ptr->early_camp_status == RR_L1_EARLY_CAMP_REQUESTED)
                        {
                          MSG_GERAN_HIGH_0_G("Wait for Early Camp confirm before completing");
                        }
                        else
                        {
                          // Procedure finished successfully
                          rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_NONE);
                          control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                        }
                      }
                      else
                      {
                        /**
                         * This SI message reception did not complete the procedure. 
                         * Check if a notification message was requested and if this SI messages means the 
                         * notification mask now matches the SI received. 
                         */
                        if (data_ptr->notify_on_si_mask != MASK_SI_NONE)
                        {
                          if (FALSE == data_ptr->si_notify_sent)
                          {
                            // Check if everything in notify_on_si_mask has been received
                            if ((data_ptr->notify_on_si_mask & (~si_received_mask)) == MASK_SI_NONE)
                            {
                              rr_select_bcch_send_omsg_si_notify_ind(data_ptr->arfcn, data_ptr->bsic);

                              data_ptr->si_notify_sent = TRUE;
                            }
                          }
                        }

                        // Check if any messages were being sourced from the sys info cache
                        if (data_ptr->csi_mask != MASK_SI_NONE)
                        {
                          /**
                           * As SI messages are received, the sys info cache mask is updated to remove those 
                           * messages received. Once the mask is empty, then all the messages from the cache 
                           * have been received. 
                           */
                          data_ptr->csi_mask &= (~si_received_mask);
                        }

                        // Check if all the messages have now been received from the sys info cache
                        if (data_ptr->csi_mask == MASK_SI_NONE)
                        {
                          // If L1 is in early Camping, send iterative updates of remaining SIs required
                          if (RR_L1_EARLY_CAMP_CONFIRMED == data_ptr->early_camp_status)
                          {
                            /**
                             * Now that L1 is in IDLE state, it is now possible to do iterative updating of SI
                             * read requests as SI messages are received. Start the process here by updating L1
                             * with a new mask.
                             * The formula for the new mask is:
                             *  new_mask = (ALL SI) minus (SI already received)
                             */
                            rr_select_bcch_send_l1_read_sys_info_req(
                              si_received_mask   // si_received
                            );
                          }
                        }
                      }
                    }
                  }
                  break;
                }

                case RR_SELECT_BCCH_IMSG_ABORT_REQ:
                {
                  rr_select_bcch_data_t *data_ptr = control_data_ptr->data_ptr;

                  if (RR_L1_EARLY_CAMP_REQUESTED == data_ptr->early_camp_status)
                  {
                    // Wait for L1 to transition to IDLE before aborting
#ifdef FEATURE_RR_SI_CACHE
                    rr_csi_deactivate_pend_store();
#endif
                    control_data_ptr->state = RR_SELECT_BCCH_STATE_ABORT;
                  }
                  else
                  {
                    rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_ABORTED);
                    control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_SELECT_BCCH_IMSG */

            case RR_L1_IDLE_OMSG:
            {
              switch (msg_ptr->rr.header.imh.message_id)
              {
                case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                {
                  uint32 si_received_mask;

                  rr_select_bcch_data_t *data_ptr = control_data_ptr->data_ptr;

                  MSG_GERAN_MED_0_G("L1 confrms Early Camping");

                  if (RR_L1_EARLY_CAMP_REQUESTED == data_ptr->early_camp_status)
                  {
#ifdef FEATURE_RR_SI_CACHE
                    rr_csi_copy_pend_store_to_cache( data_ptr->arfcn,
                                                    &data_ptr->init_params.lai,
                                                    &data_ptr->init_params.ci);

                    rr_csi_deactivate_pend_store();
#endif
                    data_ptr->early_camp_status = RR_L1_EARLY_CAMP_CONFIRMED;

                    if (rr_select_bcch_check_si_received(SI_TYPE_NONE, &si_received_mask))
                    {
                      rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_NONE);
                      control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                    }
                    else
                    {
#ifdef FEATURE_RR_SI_CACHE
                      // Retrieve whatever SIs are stored in the cache
                      control_data_ptr->data_ptr->csi_mask = rr_select_bcch_csi_retrieve_all();
#endif
                      /**
                       * Whatever sys info messages are stored in the cache for this cell (which may be none) 
                       * will now be retrieved and a DL_UNIT_DATA_IND queued for each available message. 
                       *  
                       * No iterative updating to L1 of the SI read mask should be done until all the messages 
                       * from the cache have been processed (because all the messages arrive at the same 
                       * time). 
                       */

                      // Check what SI messges the cache contains
                      if (control_data_ptr->data_ptr->csi_mask == MASK_SI_NONE)
                      {
                        /** 
                         * The sys info cache was empty (for this cell). 
                         *  
                         * Now that L1 is in IDLE state, it is now possible to do iterative updating of SI 
                         * read requests as SI messages are received. Start the process here by updating L1 
                         * with a new mask. The formula for the new mask is: new_mask = (ALL SI) minus (SI 
                         * already received) 
                         */
                        rr_select_bcch_send_l1_read_sys_info_req(
                          si_received_mask   // si_received
                        );
                      }
                    }
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_TIMER:
        {
          rr_timer_id_T timer_id = (rr_timer_id_T)msg_ptr->mid_timer_expiry.timer_id;

          switch (timer_id)
          {
            case RR_CELL_SELECTION_ABORT_TIMER:
            {
              MSG_GERAN_HIGH_0_G("Procedure failed - timeout");

              if (rr_select_bcch_is_using_candidate_cell_db())
              {
                // Invalidate the current cell so that it won't be selected again by this or
                // any external module
                rr_select_bcch_update_candidate_cell_type(RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES);
              }

              rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_TIMEOUT);
              control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
              break;
            }

            case RR_GSM_SYS_INFO_TIMER:
            {
              rr_candidate_cell_T *candidate_cell_ptr;

              MSG_GERAN_HIGH_0_G("SI acquisition failed - timeout");

              /**
               * Find the cell in the candidate cell database (if it's there) and mark the BCCH as invalid, so
               * that it won't be selected again by this module or another module.
               */
              rr_candidate_db_set_type(
                rr_get_candidate_cell_db_ptr(),
                control_data_ptr->data_ptr->arfcn,
                RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES
              );

              /**
               * The current cell may or may not have been selected from the candidate cell database (it may 
               * have been supplied directly by the caller). 
               * If the candidate cell database is being used, then select the next cell to try (if there is 
               * one). 
               */
              if (rr_select_bcch_is_using_candidate_cell_db())
              {
                candidate_cell_ptr = rr_select_bcch_next_candidate_cell();

                if (candidate_cell_ptr == NULL)
                {
                  MSG_GERAN_HIGH_0_G("Procedure failed - no more cells to try");
                }
              }
              else
              {
                candidate_cell_ptr = NULL;
              }

              if (candidate_cell_ptr != NULL)
              {
#ifdef FEATURE_RR_SI_CACHE
                rr_csi_deactivate_pend_store();
#endif
                rr_select_bcch_start_select_after_list_decodes(candidate_cell_ptr);
                control_data_ptr->state = RR_SELECT_BCCH_STATE_SELECT;
              }
              else
              {
                rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_TIMEOUT);
                control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
              }
              break;
            }

            default:
            {
              MSG_GERAN_LOW_1_G("Unhandled timer (%d)",(int)timer_id);
            }
          }
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_select_bcch_state_name(control_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_SELECT_BCCH_STATE_ACQUIRE */

    case RR_SELECT_BCCH_STATE_ABORT:
    {
      //RR_NULL_CHECK_FATAL(control_data_ptr->data_ptr);

      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_PSCAN_OMSG:
            {
              switch (msg_ptr->rr.pscan.omsg.header.imh.message_id)
              {
                case RR_PSCAN_OMSG_FINISHED_IND:
                case RR_PSCAN_OMSG_ABORT_CNF:
                {
                  rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_ABORTED);
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_PSCAN_OMSG */

            case RR_DECODE_BCCH_OMSG:
            {
              switch (msg_ptr->rr.decode_bcch.imsg.header.imh.message_id)
              {
                case RR_DECODE_BCCH_OMSG_ABORT_CNF:
                case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                {
                  rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_ABORTED);
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_DECODE_BCCH_OMSG */

            case RR_L1_IDLE_OMSG:
            {
              switch (msg_ptr->rr.header.imh.message_id)
              {
                case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                {
                  rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_ABORTED);
                  control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_select_bcch_state_name(control_data_ptr->state)
                  );
                }
              }
              break;
            } /* RR_L1_IDLE_OMSG */

            case RR_PSCAN_IMSG:
            {
              rr_pscan_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            } /* RR_PSCAN_IMSG */

            case RR_DECODE_BCCH_IMSG:
            {
              rr_decode_bcch_list_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            } /* RR_DECODE_BCCH_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MPH_SELECT_SPECIFIC_BCCH_CNF:
            {
              rr_select_bcch_finished(RR_SELECT_BCCH_FAIL_REASON_ABORTED);
              control_data_ptr->state = RR_SELECT_BCCH_STATE_NULL;
              break;
            }

            case MPH_POWER_SCAN_CNF:
            {
              rr_pscan_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            }

            case MPH_DECODE_BCCH_LIST_CNF:
            {
              rr_decode_bcch_list_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_select_bcch_state_name(control_data_ptr->state)
              );
            }
          }
          break;
        }

        case MS_TIMER:
        {
          rr_timer_id_T timer_id = (rr_timer_id_T)msg_ptr->mid_timer_expiry.timer_id;

          switch (timer_id)
          {
            case RR_CELL_SELECTION_ABORT_TIMER:
            case RR_GSM_SYS_INFO_TIMER:
            {
              MSG_GERAN_MED_0_G("Waiting to abort");
              break;
            }

            default:
            {
              MSG_GERAN_LOW_1_G("Unhandled timer (%d)",(int)timer_id);
            }
          }
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_select_bcch_state_name(control_data_ptr->state)
          );
        }
      }
      break;
    } /* RR_SELECT_BCCH_STATE_ABORT */

    default:
    {
      MSG_GERAN_LOW_1_G("Unknown state: %d", control_data_ptr->state);
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_SELECT_BCCH_SM,
    EV_INPUT_MESSAGE,
    (byte) control_data_ptr->state,
    msg_ptr
  );
#endif

  if (control_data_ptr->old_state != control_data_ptr->state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_select_bcch_control",
      control_data_ptr->old_state,
      control_data_ptr->state,
      rr_select_bcch_state_name
    );

    control_data_ptr->old_state = control_data_ptr->state;
  }

  return;

} /* rr_select_bcch_control */

/*!
 * \brief Returns a text string representing a RR_SELECT_BCCH IMSG/OMSG
 *
 * \param msg_ptr
 * \param message_set
 * \param message_id
 *
 * \return char*
 */
char *rr_select_bcch_message_name(const rr_cmd_bdy_type *msg_ptr)
{
  char * s = "?";

  if (msg_ptr != NULL)
  {
    switch (msg_ptr->rr.header.rr_message_set)
    {
      case RR_SELECT_BCCH_IMSG:
      {
        switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
        {
          case RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ:
          {
            s = "RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ";
            break;
          }

          case RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ:
          {
            s = "RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ";
            break;
          }

          case RR_SELECT_BCCH_IMSG_SELECT_REQ:
          {
            s = "RR_SELECT_BCCH_IMSG_SELECT_REQ";
            break;
          }

          case RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ:
          {
            s = "RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ";
            break;
          }

          case RR_SELECT_BCCH_IMSG_ABORT_REQ:
          {
            s = "RR_SELECT_BCCH_IMSG_ABORT_REQ";
            break;
          }

          case RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND:
          {
            s = "RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND";
            break;
          }

          default:
          {
            MSG_GERAN_LOW_1("Unknown RR_SELECT_BCCH_IMSG: 0x%02x", msg_ptr->rr.select_bcch.imsg.header.imh.message_id);
            s = "?";
          }
        }
        break;
      } /* RR_SELECT_BCCH_IMSG */

      case RR_SELECT_BCCH_OMSG:
      {
        switch (msg_ptr->rr.select_bcch.omsg.header.imh.message_id)
        {
          case RR_SELECT_BCCH_OMSG_SELECT_CNF:
          {
            s = "RR_SELECT_BCCH_OMSG_SELECT_CNF";
            break;
          }

          case RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND:
          {
            s = "RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND";
            break;
          }

          case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
          {
            s = "RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND";
            break;
          }

          default:
          {
            MSG_GERAN_LOW_1("Unknown RR_SELECT_BCCH_OMSG: 0x%02x", msg_ptr->rr.select_bcch.omsg.header.imh.message_id);
          }
        }
        break;
      } /* RR_SELECT_BCCH_OMSG */

      default:
      {
        MSG_GERAN_LOW_3("Unknown RR message set: %d, message ID: 0x%02x",
          msg_ptr->rr.header.rr_message_set,
          msg_ptr->rr.header.imh.message_id,
          0);
      }
    }
  }
  else
  {
    ERR_GERAN_FATAL_0("msg_ptr == NULL");
  }

  return s;

} /* end rr_select_bcch_message_name */

/*!
 * \brief Indicates if a SI acquisition is currently in progress.
 *
 *
 * \return boolean - TRUE if an SI acquisition is currently in progress, FALSE otherwise
 */
boolean rr_select_bcch_si_acq_in_progress( )
{
  // Obtain a pointer to the module data
  rr_select_bcch_control_data_t *control_data_ptr = rr_select_bcch_get_control_data_ptr();

  return (control_data_ptr->data_ptr != NULL);
}

/* EOF */

