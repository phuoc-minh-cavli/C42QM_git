/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR General

GENERAL DESCRIPTION
   This module contains general/auxiliary functions used by the other RR modules

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_general.c_v   1.22   17 Jul 2002 11:50:36   rodgerc  $
$Header: //components/rel/geran.mpss/5.2.0/grr/static/rr_static_general.c#15 $ $DateTime: 2023/03/16 03:37:40 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
===========================================================================*/
#include "rr_static_general.h"
#include "comdef.h"
#include <stdlib.h>
#include <string.h>
#include "gprs_mem.h"
#include "rr_events.h"
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "geran_dmm.h"
#endif /* FEATURE_DYNAMIC_LOADING_GERAN */


/*----------------------------------------------------------------------------
 * Variable Definitions
 * -------------------------------------------------------------------------*/

rr_gprs_debug_data_t  rr_gprs_debug_data;

rr_gprs_debug_data_t *rr_static_gprs_debug_get_data_ptr( )
{
    return(&rr_gprs_debug_data);
}

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/**
 * List of forbidden LAIs provided by the MM. Used to determine cell suitability
 * during cell selection/reselection. (Move to shared cell selection)
 */
#ifdef PERLUTF
#error code not present
#else
static forbidden_lai_list_T forbidden_lai_list;
#endif /*PERLUTF*/

/* Static databases used in GRR */

static rr_static_general_data_t rr_static_general_data;
static rr_public_store_data_t   rr_public_store_data;

static rr_nv_data_t     rr_nv_data;
static rr_efs_nv_data_t rr_efs_nv_data;

/* Reason for which GSM L1 will be stopped */
static rr_l1_gsm_stop_reason_T  rr_l1_gsm_stop_reason;

static rr_sim_data_t  rr_sim_data;

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/

/*!
 * \brief Returns a pointer to the module data.
 *
 *
 * \return rr_general_data_t*
 */
rr_static_general_data_t *rr_static_general_get_data_ptr(void)
{
  return &rr_static_general_data;
}

rr_scell_backup_t* rr_static_get_scell_backup_ptr(void)
{
  return rr_static_general_data.rr_scell_backup;
}

/*!
 * \brief Returns a pointer to the module data.
 *
 *
 * \return rr_public_store_data_t*
 */
rr_public_store_data_t *rr_public_store_get_data_ptr(void)
{
  return &rr_public_store_data;
}

/*!
 * \brief Module initialisation function called at task start.
 */
void rr_public_store_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(&rr_public_store_data, 0, sizeof(rr_public_store_data));

  // Setup any specific non-zero values
  {
    rr_public_store_data_t *data_ptr;

    data_ptr = &rr_public_store_data;

    data_ptr->rrps_store_data.change_mark_si13 = RRPS_CHANGE_MARK_NOT_VALID;
    data_ptr->rrps_store_data.change_mark_psi2 = RRPS_CHANGE_MARK_NOT_VALID;

    data_ptr->rrps_store_data.gmm_data.gmm_state = GMM_IDLE;
    data_ptr->rrps_store_data.gmm_data.ptmsi = TMSI_PTMSI_INVALID;
    data_ptr->rrps_store_data.gmm_data.tmsi = TMSI_PTMSI_INVALID;
    data_ptr->rrps_store_data.gmm_data.tlli = TMSI_PTMSI_INVALID;
    data_ptr->rrps_store_data.gmm_data.tlli_old = TMSI_PTMSI_INVALID;
  }
} /* end rr_public_store_task_start_init() */


/*!
 * \brief Returns a pointer to the module data.
 *
 * \return rr_nv_data_t*
 */
rr_nv_data_t *rr_nv_get_data_ptr(void)
{

  return &rr_nv_data;
}

/*!
 * \brief Returns a pointer to the EFS data.
 *
 * \return rr_efs_nv_data_t*
 */
rr_efs_nv_data_t *rr_nv_get_efs_data_ptr(void)
{
 
  return &rr_efs_nv_data;
}

/*!
 * \brief Returns a pointer to the module data.
 *
 *
 * \return rr_sim_data_t*
 */
rr_sim_data_t *rr_sim_get_data_ptr( void )
{
  return &rr_sim_data;
}

/*!
 * \brief Returns a pointer to the forbidden LAI list 
 *
 * \return forbidden_lai_list_T*
 */
forbidden_lai_list_T *rr_get_forbidden_lai_list_ptr(void)
{
  return &forbidden_lai_list;
}

void rr_clear_forbidden_lai_list_ptr(void)
{
  memset(&forbidden_lai_list, 0, sizeof(forbidden_lai_list));
  return;
}

/**
 * Stores the forbidden LAI list
 *
 * @param forbidden_lai_update_req
 *               Message containing the new forbidden LAI list
 */
void rr_store_forbidden_lai_list(rr_forbidden_lai_update_req_T *forbidden_lai_update_req)
{
  int lai_index;
  int no_of_entries;
  forbidden_lai_list_T *lai_list_ptr;

  //RR_NULL_CHECK_RETURN_VOID(forbidden_lai_update_req);

  // Obtain a pointer to the forbidden LAI list (stored by ASID)
  lai_list_ptr = rr_get_forbidden_lai_list_ptr();
  RR_NULL_CHECK_FATAL(lai_list_ptr);

  no_of_entries = forbidden_lai_update_req->forbidden_lai_list.no_of_entries;
  lai_list_ptr->no_of_entries = (byte) no_of_entries;

  for (lai_index = 0;
       (lai_index < no_of_entries) && (lai_index < MAX_FORBIDDEN_LAIS);
       lai_index++)
  {
    inter_task_lai_T *lai_ptr =
      &(forbidden_lai_update_req->forbidden_lai_list.location_area_identification[lai_index]);

    lai_list_ptr->lai[lai_index].plmn_id.identity[0] = lai_ptr->PLMN_id.identity[0];
    lai_list_ptr->lai[lai_index].plmn_id.identity[1] = lai_ptr->PLMN_id.identity[1];
    lai_list_ptr->lai[lai_index].plmn_id.identity[2] = lai_ptr->PLMN_id.identity[2];

    lai_list_ptr->lai[lai_index].location_area_code =
      (word) ((lai_ptr->location_area_code[0] * 256) + (lai_ptr->location_area_code[1]));
  }

  // reset the rest of the entries in the list to 0 (for unit test consistency)
  for (;lai_index < MAX_FORBIDDEN_LAIS;lai_index++)
  {
    lai_list_ptr->lai[lai_index].location_area_code = 0;
    lai_list_ptr->lai[lai_index].plmn_id.identity[0] = 0;
    lai_list_ptr->lai[lai_index].plmn_id.identity[1] = 0;
    lai_list_ptr->lai[lai_index].plmn_id.identity[2] = 0;
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  if (geran_is_multi_mode_gas_id())
  {
    /* Delete any IRAT cells from the Forbidden LAI database if they are
    in an LAI that is no longer forbidden. */
    rr_g2x_forbidden_lai_list_updated();
  }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  return;
} /* rr_store_forbidden_lai_list */

/**
 * Initialise barred cell list. Cell barring here is due to authentication or
 * authentication and ciphering failure.
 */
void rr_init_barred_cell_list(void)
{
  uint8 index;

  // Obtain a pointer to the module data
  rr_static_general_data_t *rr_static_general_data_ptr = rr_static_general_get_data_ptr();

  /* initialise barred cell list */
  for (index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    rr_static_general_data_ptr->rr_barred_cell_list[index].remaining_time = RR_INVALID_REMAINING_TIME;
    rr_static_general_data_ptr->rr_barred_cell_list[index].arfcn.num      = INVALID_ARFCN;
    rr_static_general_data_ptr->rr_barred_cell_list[index].arfcn.band     = INVALID_BAND;
    rr_static_general_data_ptr->rr_barred_cell_list[index].bsic.BS_colour_code   = INVALID_BSIC_CC;
    rr_static_general_data_ptr->rr_barred_cell_list[index].bsic.PLMN_colour_code = INVALID_PLMN_CC;
  }
} /* rr_init_barred_cell_list() */

/**
 * Find out number of barred cells in the list.
 */
uint8 rr_get_barred_cell_count(void)
{
  uint8 index;
  uint8 barred_cell_count = 0;

  // Obtain a pointer to the module data
  rr_static_general_data_t *rr_static_general_data_ptr = rr_static_general_get_data_ptr();

  for(index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    if(rr_static_general_data_ptr->rr_barred_cell_list[index].remaining_time != RR_INVALID_REMAINING_TIME)
    {
      barred_cell_count++;
    }
  }

  MSG_GERAN_HIGH_1_G("Barred cell count: %d",barred_cell_count);

  return barred_cell_count;
}/* rr_get_barred_cell_count() */

/**
 * Update remaining time of each cell in barred cell list. If remaining time
 * is zero for any cell, then remove the cell from the list
 */
void rr_update_barred_cell_remaining_time(void)
{
  uint8 index;

  // Obtain a pointer to the module data
  rr_static_general_data_t *rr_static_general_data_ptr = rr_static_general_get_data_ptr();

  for(index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    if(rr_static_general_data_ptr->rr_barred_cell_list[index].remaining_time != RR_INVALID_REMAINING_TIME)
    {
      /* Deduct timeout value from remaining time */
      if(rr_static_general_data_ptr->rr_barred_cell_list[index].remaining_time > RR_CELL_BARRED_TIMEOUT)
      {
        rr_static_general_data_ptr->rr_barred_cell_list[index].remaining_time -= RR_CELL_BARRED_TIMEOUT;
      }
      /* Otherwise, remove the cell from list, i.e. invalidate remaining time,
         arfcn, and bsic */
      else
      {
        rr_static_general_data_ptr->rr_barred_cell_list[index].arfcn.num = INVALID_ARFCN;
        rr_static_general_data_ptr->rr_barred_cell_list[index].arfcn.band = INVALID_BAND;
        rr_static_general_data_ptr->rr_barred_cell_list[index].bsic.BS_colour_code = INVALID_BSIC_CC;
        rr_static_general_data_ptr->rr_barred_cell_list[index].bsic.PLMN_colour_code = INVALID_PLMN_CC;
        rr_static_general_data_ptr->rr_barred_cell_list[index].remaining_time = RR_INVALID_REMAINING_TIME;
      }
    }
  }
}/* rr_update_barred_cell_remaining_time() */

/**
 * RR_CELL_BARRED_TIMER expired. Update barred cell remaining time. Remove any
 * barred cells whose remaining time is zero. Start RR_CELL_BARRED_TIMER if
 * there are barred cells in the list.
 */
void rr_process_barred_cell_timer_expiry(void)
{
  /* Firstly, deduct RR_CELL_BARED_TIMEOUT from remaining_time of each
   * cell in barred cell list. Following this deduction, if remaining
   * time is zero for any cell, then remove that cell from the barred
   * cell list */

  rr_update_barred_cell_remaining_time();

  MSG_GERAN_HIGH_0_G("Barred cell timer expired");

  /* Restart barred cell timer if there are barred cells in the list */
  if(rr_get_barred_cell_count() != 0)
  {
    rr_start_cell_barred_timer();
  }
} /* rr_process_barred_cell_timer_expiry() */

/*!
 * \brief Returns the L1 stop reason.
 *
 * This function will always return RR_L1_DEEP_SLEEP if FEATURE_WCDMA is not defined (as a mode change
 * should always result in GSM hardware being deactivated, otherwise this function will return whatever value
 * has previously been set with a call to rr_l1_gsm_stop_reason_set()
 *
 * \return rr_l1_gsm_stop_reason_T
 */
rr_l1_gsm_stop_reason_T rr_l1_gsm_stop_reason_get(void)
{
#ifdef FEATURE_WCDMA
  return (rr_l1_gsm_stop_reason);
#else
  /* If we are not a multi-mode device then we should always
  send DEEP_SLEEP reason to GL1, as we can only be switching
  modem off for deep sleep, or switching to CDMA mode, both
  of which require us to shut down the GSM hardware. */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)  
  if (rr_l1_gsm_stop_reason != RR_L1_DEEP_SLEEP)
  {
    MSG_GERAN_ERROR_2_G("rr_l1_gsm_stop_reason is %d, expected %d",
              rr_l1_gsm_stop_reason,
              RR_L1_DEEP_SLEEP);
  }
#endif /*(FEATURE_WCDMA) || (FEATURE_GSM_TDS) || (FEATURE_LTE) */  
  return RR_L1_DEEP_SLEEP;
#endif /* FEATURE_WCDMA */
}

/*!
 * \brief Sets the L1 stop reason
 *
 * \param gsm_stop_reason
 */
void rr_l1_gsm_stop_reason_set(rr_l1_gsm_stop_reason_T gsm_stop_reason)
{
  rr_l1_gsm_stop_reason = gsm_stop_reason;
}

void set_rr_deact_reason(rr_deact_reason_t reason)
{
  rr_static_general_data.rr_deact_reason = reason;
}

rr_deact_reason_t get_rr_deact_reason(void)
{
  return rr_static_general_data.rr_deact_reason;
}

void set_rr_mm_gsm_stop_reason(rr_mm_gsm_stop_reason_T reason)
{
  rr_static_general_data.rr_mm_gsm_stop_reason = reason;
}

rr_mm_gsm_stop_reason_T get_rr_mm_gsm_stop_reason(void)
{
  return rr_static_general_data.rr_mm_gsm_stop_reason;
}

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 *
 *  Note: The primary purpose of this function is to initialise pointers to control/data structures.
 */
void rr_static_general_init(void)
{
  /* Init SIM data */
  memset(&rr_sim_data, 0, sizeof(rr_sim_data_t));
  /* Initialise debug traces */
  rr_gprs_debug_init();
}

#ifndef PERLUTF /* these two ODM functions are removed for PERLUTF builds */
void gprs_odm_register_cell_type_cb(gprs_odm_cell_type_cb_fn_t cb_fn)
{
  rr_static_general_data.odm_cell_type_cb_fn = cb_fn;

  if (cb_fn)
  {
    MSG_GERAN_HIGH_0_G("ODM cell type callback registered");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("ODM cell type callback deregistered");
  }
} /* end gprs_odm_register_cell_type_cb() */

void gprs_odm_deregister_cell_type_cb(void)
{
  gprs_odm_register_cell_type_cb(NULL);
} /* end gprs_odm_deregister_cell_type_cb() */
#endif /* PERLUTF */

void rr_clear_static_backup()
{
  uint8 i= 0;
  rr_static_general_data_t *rr_static_general_data_ptr = rr_static_general_get_data_ptr();

  if (rr_static_general_data_ptr->rr_scell_backup != NULL)
  {
    csi_entry_t* entry_ptr = rr_static_general_data_ptr->rr_scell_backup->scell_csi;

    for (i = 0; i < rr_static_general_data_ptr->rr_scell_backup->scell_csi_count; i++, entry_ptr++)
    {
      if (entry_ptr->octet_ptr != NULL)
      {                                                                              
        GPRS_MEM_FREE(entry_ptr->octet_ptr);
        entry_ptr->octet_ptr = NULL;
      }

      entry_ptr->msg_type  = 0xFF;
      entry_ptr->si_arfcn.num = 0xFF;
      entry_ptr->cgi.ci = 0xFF;
    }

    GPRS_MEM_FREE(rr_static_general_data_ptr->rr_scell_backup);
    rr_static_general_data_ptr->rr_scell_backup = NULL;
  }

  if (rr_static_general_data_ptr->ps_domain_backup != NULL)
  {
    GPRS_MEM_FREE(rr_static_general_data_ptr->ps_domain_backup);
    rr_static_general_data_ptr->ps_domain_backup = NULL;
  }
}

void rr_set_scell_meas_timestamp(uint32 scell_meas_timestamp)
{
  rr_static_general_data.scell_meas_timestamp = scell_meas_timestamp;
}

uint32 rr_get_scell_meas_timestamp(void)
{
  return rr_static_general_data.scell_meas_timestamp;
}

#ifdef FEATURE_GSM_CELL_SELECT_INFO
void rr_send_cell_select_info_rsp(boolean is_ue_camped, byte rxlev_access_min )
{
  geran_grr_cell_select_info_rsp_t rsp;
  errno_enum_type                  result;

  memset(&rsp, 0, sizeof(geran_grr_cell_select_info_rsp_t));

  msgr_init_hdr(
    &rsp.msg_hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_CELL_SELECT_INFO_RSP
  );

  rsp.is_ue_camped     = is_ue_camped;
  rsp.rxlev_access_min = rxlev_access_min;

  msgr_set_hdr_inst(&rsp.msg_hdr, GERAN_ACCESS_STRATUM_ID_1);

  result = msgr_send(&rsp.msg_hdr, sizeof(rsp)); 
  
  MSG_GERAN_HIGH_2_G("sending GERAN_GRR_CELL_SELECT_INFO_RSP: is_ue-camped= %d, rxlev_access_min= %d",rsp.is_ue_camped,rsp.rxlev_access_min);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}
#endif

/*!
 * \brief Populate the NAS transactions buffer with the given message pair.
 * 
 * \param rr_gprs_debug_data_ptr (in)
 * \param incoming_msg (in)
 * \param outgoing_msg (in)
 */
static void rr_debug_init_nas_transaction(rr_gprs_debug_data_t    *rr_gprs_debug_data_ptr,
                                          mm_rr_message_id_enum_T  incoming_msg,
                                          mm_rr_message_id_enum_T  outgoing_msg)
{
  if (rr_gprs_debug_data_ptr != NULL)
  {
    int trans_index;

    for (trans_index = 0; trans_index < RR_DEBUG_NAS_TRANSACTION_PAIRS; trans_index++)
    {
      /* find the first uninitialised transaction record and store the details there */
      if (FALSE == rr_gprs_debug_data_ptr->nas_transactions[trans_index].valid_record)
      {
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_msg = incoming_msg;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_clk = 0;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_msg = outgoing_msg;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk = 0;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].elapsed_time = 0;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].valid_record = TRUE;
        break;
      }
    }
  }

  return;
}


/*!
 * \brief Module initialisation function.
 * 
 */
void rr_gprs_debug_init( void )
{
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr;

  rr_gprs_debug_data_ptr = rr_static_gprs_debug_get_data_ptr();

  if (rr_gprs_debug_data_ptr != NULL)
  {
    /* reset all the transaction records */
    memset(rr_gprs_debug_data_ptr->nas_transactions, 0, sizeof(rr_gprs_debug_data_ptr->nas_transactions));

    /* and then add transaction tracing for each pair of messages */
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_STOP_GSM_MODE_REQ, RR_STOP_GSM_MODE_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_PLMN_SELECT_REQ,   RR_PLMN_SELECT_CNF);
#ifdef FEATURE_GERAN_CS_SUPPORT
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_ABORT_REQ,         RR_ABORT_CNF);
#endif /* FEATURE_GERAN_CS_SUPPORT */
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_DEACT_REQ,         RR_DEACT_CNF);
#ifdef FEATURE_GERAN_CS_SUPPORT
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_EST_REQ,           RR_EST_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_EST_REQ,           RR_ABORT_IND);
#endif /* FEATURE_GERAN_CS_SUPPORT */
  }

  return;
}

/*!
 * \brief Update the NAS transactions buffer for the given message type.
 * 
 * \param rr_gprs_debug_data_ptr (in)
 * \param msg_id (in)
 */
static void rr_debug_update_nas_transactions(rr_gprs_debug_data_t *rr_gprs_debug_data_ptr, const mm_rr_message_id_enum_T msg_id)
{
  if (rr_gprs_debug_data_ptr != NULL)
  {
    timetick_type msg_time;
    int           trans_index;	  
    /* retrieve the current slow clock timestamp */
    msg_time = timetick_get();	

    for (trans_index = 0; trans_index < RR_DEBUG_NAS_TRANSACTION_PAIRS; trans_index++)
    {
      /* find all transaction records where either the incoming or outgoing primitive matches */
      if (rr_gprs_debug_data_ptr->nas_transactions[trans_index].valid_record)
      {
        if (msg_id == rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_msg)
        {
          /* matched against incoming message ID, record start of new transaction */
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_clk = msg_time;
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk = 0;
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].elapsed_time = 0;
        }
        else if (msg_id == rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_msg)
        {
          /* matched against outgoing message ID, calculate the response time in milliseconds */
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk = msg_time;
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].elapsed_time = timetick_diff(rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_clk,
                                                                                             rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk,
                                                                                             T_MSEC);
        }
      }
    }
  }

  return;
}

/*!
 * \brief Logs L1->RR and RR->L1 messages. Note that some messages are filtered.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 */
void rr_debug_log_rr_l1_message(
  rr_l1_message_id_enum_T message_id,
  uint32                  pended_count
)
{
  switch (message_id)
  {
    /* Various messages are not of interest in the trace buffer, so are
    ignored.  These generally relate to period events that occur in idle mode
    such as serving / neighbour cell measurements. */
    case MPH_BLOCK_QUALITY_IND:
    case MPH_SERVING_DED_MEAS_IND:
    case MPH_SERVING_IDLE_MEAS_IND:
    case MPH_SURROUND_MEAS_IND:
    case MPH_DECODE_BCCH_LIST_FAILURE_IND: /* Up to 140 sent during cell selection, would quickly fill the buffer */
    case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
    case MPH_SERVING_AUX_MEAS_IND:
    case MPH_FCCH_SCH_DECODE_IND: /* Up to 140 sent during cell selection, would quickly fill the buffer */
    case MPH_NC_MEASUREMENT_IND:
    case MPH_SERVING_MEAS_IND:
    case MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND:
    case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
    case MPH_SURROUND_IRAT_MEAS_IND:
    case MPH_SET_PRIORITY_IND:
    case MPH_DSC_THRESHOLD_IND:
    {
      break;
    } /* MPH_BLOCK_QUALITY_IND ... */

    default:
    {
      rr_debug_rr_l1_trace_buffer_entry_t * entry;
	  
      uint32 timestamp;
      if (is_geran_loaded() == TRUE)
  	  {
        timestamp = GSTMR_GET_FN_GERAN();
      }
      else
      {
        timestamp = 0xFFFF;
      }
      // Obtain a pointer to the module data
      rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_static_gprs_debug_get_data_ptr();

      if (rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index >= RR_DEBUG_RR_L1_TRACE_BUFFER_SIZE)
      {
        rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index = 0;
      }

      entry = &rr_gprs_debug_data_ptr->rr_l1_trace_buffer[rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index];

      entry->timestamp = timestamp;
      entry->message_id = message_id;
      entry->pended_count = pended_count;

      ++rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index;
    }
  }
}

/*!
 * \brief Logs MM->RR and RR->MM messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 */
void rr_debug_log_mm_rr_message(
  mm_rr_message_id_enum_T message_id,
  uint32                  pended_count
)
{
  rr_debug_mm_rr_trace_buffer_entry_t * entry;
  uint32 timestamp;
  if (is_geran_loaded() == TRUE)
  {
    timestamp = GSTMR_GET_FN_GERAN();
  }
  else
  {
    timestamp = 0xFFFF;
  }
  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_static_gprs_debug_get_data_ptr();

  /* update the NAS transaction pair trace */
  rr_debug_update_nas_transactions(rr_gprs_debug_data_ptr, message_id);

  if (rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index >= RR_DEBUG_MM_RR_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->mm_rr_trace_buffer[rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index];

  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->pended_count = pended_count;

  ++rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index;
}

#ifdef FEATURE_AWARE_CT21
/*!
 * \brief Logs MM->RR and RR->MM messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
void rr_static_debug_log_dyn_pend_q_message(
  rr_cmd_type             *cmd_ptr,
  rr_cmd_type             *pend_queue_ptr,
  uint8                   message_set,
  mm_rr_message_id_enum_T message_id
)
{
  rr_debug_static_mm_rr_dyn_buffer_entry_t * entry;
  uint32 timestamp;
  if (is_geran_loaded() == TRUE)
  {
    timestamp = GSTMR_GET_FN_GERAN();
  }
  else
  {
    timestamp = 0xFFFF;
  }
  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_static_gprs_debug_data_ptr = rr_static_gprs_debug_get_data_ptr();
  
  if (rr_static_gprs_debug_data_ptr->rr_dyn_static_trace_buffer_index >= RR_DEBUG_STATIC_DYN_TRACE_BUFFER_SIZE)
  {
    rr_static_gprs_debug_data_ptr->rr_dyn_static_trace_buffer_index = 0;
  }
  entry = &rr_static_gprs_debug_data_ptr->rr_static_dyn_mm_trace_buffer[rr_static_gprs_debug_data_ptr->rr_dyn_static_trace_buffer_index];
  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->message_set = message_set;
  entry->cmd_ptr = cmd_ptr;
  entry->pend_queue_ptr = pend_queue_ptr;
  ++rr_static_gprs_debug_data_ptr->rr_dyn_static_trace_buffer_index;
}

/*!
 * \brief Logs MM->RR and RR->MM messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
 
void rr_static_log_freed_saved_q_message(
  rr_cmd_type             *cmd_ptr,
  uint8                   message_set,
  mm_rr_message_id_enum_T message_id
)
{
  rr_debug_static_freed_saved_msg_entry_t * entry;
  uint32 timestamp;
  if (is_geran_loaded() == TRUE)
  {
    timestamp = GSTMR_GET_FN_GERAN();
  }
  else
  {
    timestamp = 0xFFFF;
  }
  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_static_gprs_debug_data_ptr = rr_static_gprs_debug_get_data_ptr();
  
  if (rr_static_gprs_debug_data_ptr->rr_freed_static_trace_buffer_index >= RR_DEBUG_STATIC_SAVED_TRACE_BUFFER_SIZE)
  {
    rr_static_gprs_debug_data_ptr->rr_freed_static_trace_buffer_index = 0;
  }
  entry = &rr_static_gprs_debug_data_ptr->rr_static_freed_saved_trace_buffer[rr_static_gprs_debug_data_ptr->rr_freed_static_trace_buffer_index];
  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->message_set = message_set;
  entry->saved_q_ptr = cmd_ptr;
  ++rr_static_gprs_debug_data_ptr->rr_freed_static_trace_buffer_index;
}

/*!
 * \brief Logs MM->RR and RR->MM messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
void rr_static_log_added_saved_q_message(
  rr_cmd_type             *cmd_ptr,
  uint8                   message_set,
  mm_rr_message_id_enum_T message_id
)
{
  uint32 timestamp;
  rr_debug_static_added_saved_msg_entry_t * entry;

  if (is_geran_loaded() == TRUE)
  {
    timestamp = GSTMR_GET_FN_GERAN();
  }
  else
  {
    timestamp = 0xFFFF;
  }
  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_static_gprs_debug_data_ptr = rr_static_gprs_debug_get_data_ptr();
  
  if (rr_static_gprs_debug_data_ptr->rr_added_static_trace_buffer_index >= RR_DEBUG_STATIC_ADDED_TRACE_BUFFER_SIZE)
  {
    rr_static_gprs_debug_data_ptr->rr_added_static_trace_buffer_index = 0;
  }
  entry = &rr_static_gprs_debug_data_ptr->rr_static_added_saved_trace_buffer[rr_static_gprs_debug_data_ptr->rr_added_static_trace_buffer_index];
  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->message_set = message_set;
  entry->saved_q_ptr = cmd_ptr;
  ++rr_static_gprs_debug_data_ptr->rr_added_static_trace_buffer_index;
}
#endif
