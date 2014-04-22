/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Main Module

GENERAL DESCRIPTION
   This module contains functions that send messages from rr to mm.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_mm_send.c_v   1.16   17 Jul 2002 11:49:28   rodgerc  $
$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_mm_send.c#14 $ $DateTime: 2021/07/12 05:28:37 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
31/08/11    sjw     Finer control of parameters within PLMN_SELECT_CNF
11/08/11    sjw     Allow DSDS module to complete the PLMN_SELECT_CNF
07/13/11    sjw     Support for high-priority Cell Broadcast in DSDS mode
04/28/11    sjw     Send default response to deactivation signals if NAS uses wrong ASID
07/22/09    sjw     Added support for FEATURE_UMTS_VOICE_CIPHERING_IND
05/01/09    sjw     Change band indicator function prototype for FTM compatibility
11/28/08    sjw     Add function to derive current band indicator for NAS
05/17/04    DMB     New function rr_send_plmn_select_cnf_indicating_no_service
May/11/04   VT      Introduced new function rr_send_abort_ind_if_lai_change_during_selection()
                      to Send an ABORT_IND to MM if there is an LAI change during
                      cell selection and if there is a saved est_req and if the
                      search_reason is not equal to RR_PERFORM_REEST.
11/08/02    he      Used rr_gprs_get_sys_info_for_mm() in rr_service_ind()
                    and rr_send_plmn_select_cnf().
08/01/02    rmc     Changes to interface for cell selection/reselection optimizations
07/30/02    rmc     Logging RR service change events
07/30/02    Rao     Added InterRAT ( WCDMA To GSM )  Handover support
07/15/02    rmc     Add new parameter to rr_send_paging_ind for GPRS support
07/15/02    rmc     Change include of "cmll.h" to "cmwll.h" for new CM VU
07/01/02    rmc     Featurize rr_send_abort_cnf inside function to not have to featurize
                    everywhere else
06/18/02    rmc     Added available PLMN list parameter to rr_send_plmn_list_cnf
06/08/02    rmc     Changes to MM-RR ICD, inter_task_LAI_T and temporary rr_data_ind change
05/30/02    rmc     Fixed bug in rr_send_plmn_select_cnf to include the cell access status
05/22/02    he      Changed FEATURE_CM_INCLUDED to FEATURE_CM
05/22/02    rmc     Changes to MM-RR ICD
05/16/02    Rao     Added RR_PAGING_IND support for Dual Mode.
05/02/02    rmc     Changes for dual mode interface
04/04/02    rmc     Fixed accidental deletion of QXDM message for MM messages sent by RR
02/27/02    rao     if def'ed rr_act_cnf ( for DUALMODE )
02/20/02    ATM     Merge GS_GSM and GS functions together
02/12/02    rmc     QXDM Cleanup. CFA logging for messages sent to MM
11/14/01    RMC     Modifying rr_send_mmi_status_ind to rr_send_cm_status_ind and
                    using appropriate message passing mechanism
                    Commented out send to CB task
12/11/00    TS      Initial Revision.

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include <string.h>
#include "ms.h"
#include "rr_seg_load.h"
#include "rr_mm_send.h"
#include "mm_rr.h"
#include "cb_rr.h"
#include "gs.h"
#include "msg.h"
#include "err.h"
#include "rr_gprs_debug.h"
#include "rr_general.h"
#include "cfa.h"
#include "rr_defs.h"
#include "rr_log.h"
#include "ms.h"
#include "rr_plmn_list.h"
#include "rr_cipher.h"

#include "cmwll.h"

#include "rr_hz.h"

#include "rr_gprs_defs.h"
#include "rr_conn_establish.h"
#include "rr_cell_selection.h"
#include "rr_conn_establish.h"
#include "sys_cnst.h"

#ifdef FEATURE_GPRS_PS_HANDOVER
#include "rr_psho.h"
#endif /* FEATURE_GPRS_PS_HANODVER */


#include "geran_dual_sim.h"
#include "amssassert.h"
#include "rr_ps_domain.h"
#include "rr_thermal.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "rr_msc.h"
#include "rr_nv.h"
#include "rr_control.h"
#include "geran_grr_dyn_api.h"

typedef struct
{
  /* variables used to determine if a cell has changed when sending rr_service_ind */
  ARFCN_T rr_previous_BCCH_ARFCN;
  BSIC_T  rr_previous_BSIC;
  LAI_T   rr_previous_LAI;
  mm_as_service_state_T     service_state;
  sys_srv_capability_e_type service_capability;
  struct
  {
    boolean gmm_updated;
    uint8   gprs_multislot_class;
    uint8   egprs_multislot_class;
#ifdef FEATURE_GSM_DTM
    uint8   dtm_multislot_class;
#endif
  } multislot_class;
  boolean            gprs_sys_info_present;
  mm_gprs_sys_info_T gprs_sys_info;
#ifdef FEATURE_GERAN_BSR_PS_STATUS
  boolean ps_is_active;
#endif

} rr_mm_send_data_t;

static rr_mm_send_data_t  rr_mm_send_data;

/*!
 * \brief Return a pointer to the module data.
 *
 *
 * \return rr_mm_send_data_t*
 */
rr_mm_send_data_t *rr_mm_send_get_data_ptr( void )
{
  return &rr_mm_send_data;
}

/**
  @brief Updates the local copy of the service status & capability
*/
static void rr_mm_service_available(
  sys_srv_capability_e_type service_capability
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  if (service_capability == SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
  {
    MSG_GERAN_HIGH_0_G("service capability is CS-only");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("service capability is CS+PS");
  }

  rr_mm_send_data_ptr->service_state = MM_AS_SERVICE_AVAILABLE;
  rr_mm_send_data_ptr->service_capability = service_capability;

  return;
}


/**
  @brief Updates the local copy of the service status & capability
*/
static void rr_mm_service_not_available(
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  rr_mm_send_data_ptr->service_state = MM_AS_NO_SERVICE;
  rr_mm_send_data_ptr->service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

  return;
}


/**
 * Resets any cell information used to determine if the cell has changed
 * when sending a rr_service_ind message to MM
 */
void rr_mm_send_reset_last_cell_info(
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  rr_mm_send_data_ptr->rr_previous_LAI.location_area_code  = INVALID_LAC_WORD;
  rr_mm_send_data_ptr->rr_previous_LAI.plmn_id.identity[0] = INVALID_PLMN_ID;
  rr_mm_send_data_ptr->rr_previous_LAI.plmn_id.identity[1] = INVALID_PLMN_ID;
  rr_mm_send_data_ptr->rr_previous_LAI.plmn_id.identity[2] = INVALID_PLMN_ID;

  rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN.num  = INVALID_ARFCN;
  rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN.band = (sys_band_T)BAND_NONE;

  rr_mm_send_data_ptr->rr_previous_BSIC.BS_colour_code   = INVALID_BSIC_CC;
  rr_mm_send_data_ptr->rr_previous_BSIC.PLMN_colour_code = INVALID_PLMN_CC;

  rr_mm_send_data_ptr->gprs_sys_info_present = FALSE;
} /* rr_mm_send_reset_last_cell_info */

/**
 * Access function to allow other modules
 * to set the previous LAI
 */
void rr_set_previous_LAI(
  PLMN_id_T plmn_id,
  word lac
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  rr_mm_send_reset_last_cell_info();
  rr_mm_send_data_ptr->rr_previous_LAI.location_area_code = lac;
  rr_mm_send_data_ptr->rr_previous_LAI.plmn_id = plmn_id;
}


/**
  @brief Returns the service state last updated to MM

  @return MM_AS_SERVICE_AVAILABLE or MM_AS_NO_SERVICE
*/
mm_as_service_state_T rr_mm_get_service_state(
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  return rr_mm_send_data_ptr->service_state;
}


/**
  @brief Returns the service capability last updated to MM

  @return SYS_SRV_CAPABILITY_CS_PS_SERVICE or SYS_SRV_CAPABILITY_CS_SERVICE_ONLY
 */
sys_srv_capability_e_type rr_mm_get_service_capability(
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  return rr_mm_send_data_ptr->service_capability;
}


/**
  @brief Returns the Multislot Class value(s) last updated to GMM

  @return TRUE if GMM has been updated with these values (i.e. they are valid); FALSE otherwise
 */
boolean rr_mm_get_multislot_class(
  uint8 *egprs_multislot_class_ptr,
#ifdef FEATURE_GSM_DTM
  uint8 *dtm_multislot_class_ptr,
#endif
  uint8 *gprs_multislot_class_ptr
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();

  // Exit early indicating no update if the local data is not valid
  RR_NULL_CHECK_RETURN_PARAM(rr_mm_send_data_ptr, FALSE);

  if (egprs_multislot_class_ptr != NULL)
  {
    (*egprs_multislot_class_ptr) = rr_mm_send_data_ptr->multislot_class.egprs_multislot_class;
  }
#ifdef FEATURE_GSM_DTM
  if (dtm_multislot_class_ptr != NULL)
  {
    (*dtm_multislot_class_ptr) = rr_mm_send_data_ptr->multislot_class.dtm_multislot_class;
  }
#endif
  if (gprs_multislot_class_ptr != NULL)
  {
    (*gprs_multislot_class_ptr) = rr_mm_send_data_ptr->multislot_class.gprs_multislot_class;
  }

  return rr_mm_send_data_ptr->multislot_class.gmm_updated;
}

/*!
 * \brief Compares the supplied GPRS sys info with that last updated to MM
 * 
 * \param gprs_sys_info_ptr (in)
 * 
 * \return boolean - TRUE if they are different, FALSE otherwise
 */
boolean rr_mm_diff_gprs_sys_info(
  mm_gprs_sys_info_T *gprs_sys_info_ptr
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();

  // If there is no MM send history, indicate the data has changed
  RR_NULL_CHECK_RETURN_PARAM(rr_mm_send_data_ptr, TRUE);

  if (rr_mm_send_data_ptr->gprs_sys_info_present)
  {
    if (memcmp(
          gprs_sys_info_ptr,                    // Buf1
          &rr_mm_send_data_ptr->gprs_sys_info,  // Buf2
          sizeof(mm_gprs_sys_info_T)            // Size
        ) == 0)
    {
      // indicates they are equal
      return FALSE;
    }
  }

  // indicates they are different
  return TRUE;
}

/**
 * Send the message to a NAS entity (MM or CB)
 *
 * @param message_ptr Pointer to the message to send
 *                    Header indicates which NAS queue to send to
 */
static void rr_send_message_to_nas(IMH_T *imh_ptr)
{
  gs_queue_id_T queue_id;
  gs_status_T   gs_status;

  if (imh_ptr->message_set == MS_MM_RR)
  {
    queue_id = GS_QUEUE_MM;

    /* log the MM message send to QXDM */
    rr_show_output_message("MM",imh_ptr,imh_ptr->message_set,imh_ptr->message_id);

    rr_debug_log_omsg(imh_ptr);
  }
  else if (imh_ptr->message_set == MS_CB_RR)
  {
    queue_id = GS_QUEUE_CB;
  }
  else
  {
    queue_id = GS_MAX_QUEUE;
  }

  if (queue_id < GS_MAX_QUEUE)
  {
    gs_status = gs_send_message(queue_id, (void *)imh_ptr, TRUE);
  }
  else
  {
    gs_status = GS_INVALID_ID;
  }

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_2_G("Unable to send msg to %d, status=%d",(int)queue_id,(int)gs_status);
  }

  return;
} /* rr_send_message_to_nas */

sys_band_class_e_type rr_gapi_get_band_indicator_for_nas(
  boolean in_service
   
)
{
  sys_band_class_e_type nas_band_ind;

  if (in_service == TRUE)
  {
    if (rr_get_current_band_indicator(
          TRUE     // is_camped
        ) == SI_PSI_BAND_IND_DCS1800)
    {
      nas_band_ind = SYS_BAND_GSM_DCS_1800;
      MSG_GERAN_HIGH_0_G("Band indicator for NAS is DCS1800");
    }
    else
    {
      nas_band_ind = SYS_BAND_GSM_PCS_1900;
      MSG_GERAN_HIGH_0_G("Band indicator for NAS is PCS1900");
    }
  }
  else
  {
    /* NOTE: the value below is non-zero */
    nas_band_ind = SYS_BAND_CLASS_NONE;
  }

  return(nas_band_ind);
}

sys_band_class_e_type rr_get_band_indicator_for_nas_dyn(
  boolean                in_service
)
{
  return
    rr_gapi_get_band_indicator_for_nas(
      in_service                 // in_service
    );
}

sys_band_class_e_type rr_ds_get_band_indicator_for_nas_dyn(
  boolean                in_service
)
{


  return
    rr_gapi_get_band_indicator_for_nas(
      in_service   // in_service
    );
}

#ifdef FEATURE_GERAN_CS_SUPPORT
/**
 * Send the RR_ABORT_IND primitive to MM
 *
 * @param RR_abort_cause Abort cause sent to MM
 * @param MM_fail_cause  Failure cause sent to MM
 * @param GAS ID
 */
void rr_send_abort_ind(
  byte RR_abort_cause,
  mm_rr_failure_cause_e_type MM_fail_cause
   
)
{
  rr_abort_ind_T rr_abort_ind;

  RR_PREPARE_MM_MESSAGE(rr_abort_ind, RR_ABORT_IND);

  rr_abort_ind.RR_abort_cause = RR_abort_cause;
  rr_abort_ind.failure_cause  = MM_fail_cause;
 
  rr_send_message_to_nas((IMH_T *)&rr_abort_ind);

  return;
}

/**
 * Send the RR_ABORT_CNF primitive to MM
 */
void rr_send_abort_cnf(
  mm_as_status_T status
)
{
  rr_abort_cnf_T rr_abort_cnf;

  RR_PREPARE_MM_MESSAGE(rr_abort_cnf, RR_ABORT_CNF);

  rr_abort_cnf.status = status;


  rr_send_message_to_nas((IMH_T *)&rr_abort_cnf);
} /* rr_send_abort_cnf */
#endif /* FEATURE_GERAN_CS_SUPPORT */

/**
 * Send the RR_DEACT_CNF primitive to MM
 */
void rr_send_deact_cnf(void)
{
  rr_deact_cnf_T rr_deact_cnf;

  RR_PREPARE_MM_MESSAGE(rr_deact_cnf, RR_DEACT_CNF);

#ifdef FEATURE_DUAL_SIM
  rr_ms_access_stratum_deactivated(FALSE);
#endif /* FEATURE_DUAL_SIM */

  rr_send_message_to_nas((IMH_T *)&rr_deact_cnf);

  rr_mm_service_not_available();
} /* rr_send_deact_cnf */

#ifdef FEATURE_DUAL_SIM
/**
 * Send the RR_DS_DATA_IND primitive to MM
 *
 * @param msg_ptr Byte array containing the layer 3 message to send to MM
 *
 * @param msg_len Length of the layer 3 message in bytes
 */
static void rr_send_ds_data_ind(
  byte *msg_ptr,
  byte msg_len
)
{
  rr_ds_data_ind_T rr_ds_data_ind;
  size_t           payload_size;

  memset(&rr_ds_data_ind, 0, sizeof(rr_ds_data_ind_T));

  rr_ds_data_ind.message_header.message_set = MS_MM_RR;
  rr_ds_data_ind.message_header.message_id  = (byte)RR_DS_DATA_IND;

  /* only the number of octets received OTA need to be transported to NAS */
  payload_size  = sizeof(rr_ds_data_ind_T);
  payload_size -= (sizeof(IMH_T) + MAX_OCTETS_L3_MESSAGE);
  payload_size += (size_t)msg_len;
  PUT_IMH_LEN(payload_size, &(rr_ds_data_ind.message_header));

  if (msg_len <= MAX_OCTETS_L3_MESSAGE)
  {
    rr_ds_data_ind.l3_message_length = msg_len;

    memscpy(rr_ds_data_ind.layer3_message,sizeof(rr_ds_data_ind.layer3_message), msg_ptr, msg_len); 

    rr_send_message_to_nas((IMH_T *)&rr_ds_data_ind);
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Unable to copy %d octets into RR_DS_DATA_IND", (int)msg_len);
  }

  return;
} /* rr_send_ds_data_ind */
#endif /* FEATURE_DUAL_SIM */

/**
 * Send the RR_DATA_IND primitive to MM
 *
 * @param layer3_message Byte array containing the layer 3 message to send to MM
 *
 * @param message_length Length of the layer 3 message in bytes
 */
void rr_send_data_ind(
  byte *layer3_message,
  byte message_length
   
)
{
#ifdef FEATURE_DUAL_SIM
  /* for Dual SIM, the RR_DS_DATA_IND primitive is used instead */
  /* to avoid NAS having to make assumptions about payload size */
  rr_send_ds_data_ind(layer3_message, message_length);
#else
  rr_data_ind_T rr_data_ind;

  RR_ZERO_STRUCTURE(rr_data_ind);

  rr_data_ind.message_header.message_set = MS_MM_RR;
  rr_data_ind.message_header.message_id = (byte) RR_DATA_IND;
  PUT_IMH_LEN(message_length,&rr_data_ind.message_header);

  if (message_length <= MAX_OCTETS_L3_MESSAGE)
  {
    memscpy(rr_data_ind.layer3_message,sizeof(rr_data_ind.layer3_message), layer3_message, message_length);

    rr_send_message_to_nas((IMH_T *)&rr_data_ind);
  }
  else
  {
    MSG_GERAN_LOW_1_G("Unable to copy %d octets into RR_DATA_IND", (int)message_length);
  }
#endif /* FEATURE_DUAL_SIM */

  return;
} /* rr_send_data_ind */

/**
 * Send the RR_PAGING_IND primitive to MM
 */
void rr_send_paging_ind(
  rr_page_domain_e domain,
  rr_identity_type_e identity_type
   
)
{
  rr_paging_type_T paging_type = CS_PAGING;
  boolean valid_domain_and_identity_type = TRUE;

  switch (domain)
  {
#ifdef FEATURE_GERAN_CS_SUPPORT
    case RR_PAGE_DOMAIN_CS:
    {
      switch (identity_type)
      {
        case RR_IDENTITY_TYPE_TMSI:
        {
          break;
        }

        case RR_IDENTITY_TYPE_IMSI:
        {
          paging_type = CS_PAGING_WITH_IMSI;
          break;
        }

        default:
        {
          valid_domain_and_identity_type = FALSE;
        }
      }

      break;
    }
#endif /* FEATURE_GERAN_CS_SUPPORT */

    case RR_PAGE_DOMAIN_PS:
    {
      switch (identity_type)
      {
        case RR_IDENTITY_TYPE_PTMSI:
        {
          paging_type = PS_PAGING;
          break;
        }

        case RR_IDENTITY_TYPE_IMSI:
        {
          paging_type = PS_PAGING_ABNORMAL;
          break;
        }

        default:
        {
          valid_domain_and_identity_type = FALSE;
        }
      }

      break;
    }

    default:
    {
      valid_domain_and_identity_type = FALSE;
    }
  }

  if (valid_domain_and_identity_type)
  {
    rr_paging_ind_T rr_paging_ind;

    MSG_GERAN_HIGH_3_G("Sending page type=%d (domain=%d identity=%d)",
             (int)paging_type,
             (int)domain,
             (int)identity_type);

    RR_PREPARE_MM_MESSAGE(rr_paging_ind, RR_PAGING_IND);

    rr_paging_ind.paging_type = paging_type;

    rr_send_message_to_nas((IMH_T *)&rr_paging_ind);
  }
  else
  {
    MSG_GERAN_ERROR_2_G("Invalid page domain (%d) & identity (%d) combination",
              (int)domain,
              (int)identity_type);
  }
}

#ifdef FEATURE_GERAN_CS_SUPPORT
/**
 * Send the RR_EST_CNF primitive to MM
 */
void rr_send_est_cnf(void)
{
  rr_est_cnf_T rr_est_cnf;

  RR_PREPARE_MM_MESSAGE(rr_est_cnf, RR_EST_CNF);

  rr_send_message_to_nas((IMH_T *)&rr_est_cnf);
}
#endif /* FEATURE_GERAN_CS_SUPPORT */

/**
 * Send a RR_PLMN_LIST_CNF to MM. For now, the message always contains an
 * empty list.
 */
void rr_send_plmn_list_cnf(
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr,
  mm_as_status_T status,
  rr_PLMN_list_req_info_T *plmn_list_req
   
)
{
  rr_plmn_list_cnf_T rr_plmn_list_cnf;
  uint8 i = 0;

  RR_PREPARE_MM_MESSAGE(rr_plmn_list_cnf, RR_PLMN_LIST_CNF);

  rr_plmn_list_cnf.status = status;

  MSG_GERAN_HIGH_0_G("[GPS search] : prepare rr_plmn_list_cnf ");

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
  if (plmn_list_req && plmn_list_req->service_search == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
  {
    MSG_GERAN_HIGH_0_G("[GPS search] : populating result for GPS search ");
    rr_candidate_cell_db_populate_gps_plmn_search_list(&(rr_plmn_list_cnf.available_plmn_list.gps_wwan_scan_result));
  }
  else
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
  if (plmn_list_req && plmn_list_req->service_search != SYS_SERVICE_SEARCH_CELL_SEARCH)
  {
    if (available_plmn_list_ptr != NULL)
    {
      uint32 index;
      uint32 length = available_plmn_list_ptr->length;

      rr_plmn_list_cnf.available_plmn_list.plmn_list.length = (uint32) length;
      for (index=0; index < length && index < SYS_PLMN_LIST_MAX_LENGTH; ++index)
      {
       rr_plmn_list_cnf.available_plmn_list.plmn_list.info[index] = available_plmn_list_ptr->info[index];
      }
    }
    else
    {
      rr_plmn_list_cnf.available_plmn_list.plmn_list.length = 0;
    }
  }
  else
  {
    rr_candidate_cell_db_populate_cell_info_list(
      &(rr_plmn_list_cnf.available_plmn_list.cell_info_list.geran_cell_list));
  }

  if (plmn_list_req != NULL)
  {
    rr_plmn_list_cnf.trans_id = plmn_list_req->trans_id;

    /* added to avoid MM handling CNF of automatic as CNF of manual */
    rr_plmn_list_cnf.net_selection_mode = plmn_list_req->network_selection_mode;

      rr_plmn_list_cnf.service_search = plmn_list_req->service_search;
  }
  else
  {
    MSG_GERAN_LOW_0_G("plmn_list_req is NULL");
  }

  {
    rr_plmn_list_cnf.failure_cause = MM_RR_OTHER_FAILURE;
  }

#ifdef FEATURE_FEMTO_CSG
  if ( plmn_list_req != NULL &&
       available_plmn_list_ptr != NULL )
  {
    if (plmn_list_req->service_search == SYS_SERVICE_SEARCH_CSG )
    {
      for (i=0;i < rr_plmn_list_cnf.available_plmn_list.plmn_list.length && i < SYS_PLMN_LIST_MAX_LENGTH;i++)
      {
        /*LTE, WCDMA CSG_ID are valid if service search is CSGs*/
        if (rr_plmn_list_cnf.available_plmn_list.plmn_list.info[i].rat == SYS_RAT_TDS_RADIO_ACCESS ||
            rr_plmn_list_cnf.available_plmn_list.plmn_list.info[i].rat == SYS_RAT_GSM_RADIO_ACCESS)
        {
          rr_plmn_list_cnf.available_plmn_list.plmn_list.info[i].csg_info.csg_id = SYS_CSG_ID_INVALID;
        }
      }
    }
    else if (plmn_list_req->service_search != SYS_SERVICE_SEARCH_CELL_SEARCH)
    {
      /*CSG currently unsupported by GERAN*/
      rr_invalidate_csg_info(&rr_plmn_list_cnf.available_plmn_list.plmn_list);
    }
  }
#endif /* FEATURE_FEMTO_CSG */

  rr_send_message_to_nas((IMH_T *)&rr_plmn_list_cnf);
} /* rr_send_plmn_list_cnf */


/**
 * Send the RR_SELECT_PLMN_CNF primitive to MM
 *
 * @param error_code          indicates if the message was processed or not
 *
 * @param service_state       indicates if service was found or not for the requested PLMN
 *
 * @param location_area_identification
 *                            If successful, the LAI of the selected cell
 *
 * @param available_plmn_list_ptr
 *                            Pointer to a list of available PLMNs. Only used if not NULL.
 *
 * @param bcch_allocation_ptr Pointer to a bcch allocation. Only used if non-NULL.
 *
 * @param sys_info_db_ptr     Pointer to system information database for selected
 *                            cell (used to fill SI related information). Only used
 *                            if non-NULL (should be provided if service state is
 *                            MM_AS_SERVICE_AVAILABLE)
 */
void rr_send_plmn_select_cnf(
  mm_as_status_T                error_code,
  mm_as_service_state_T         service_state,
  LAI_T                         *lai_ptr,
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr,
  byte                          *bcch_allocation_ptr,
  system_information_database_T *sys_info_db_ptr
)
{
  rr_plmn_select_cnf_T  rr_plmn_select_cnf;

  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  RR_PREPARE_MM_MESSAGE(rr_plmn_select_cnf, RR_PLMN_SELECT_CNF);

  /* set the error code & service state and band indicator */
  rr_plmn_select_cnf.error_code     = error_code;
  rr_plmn_select_cnf.service_state  = service_state;
  rr_plmn_select_cnf.band_indicator = rr_gapi_get_band_indicator_for_nas(
    (boolean)(service_state == MM_AS_SERVICE_AVAILABLE)    // in_service
  );
  rr_plmn_select_cnf.ue_oos         = rr_cell_selection_get_ue_oos_status();

  /* only set LAI if service is available */
  if ( (service_state == MM_AS_SERVICE_AVAILABLE) &&
       (lai_ptr != NULL) &&
       (sys_info_db_ptr != NULL) )
  {
    /* set the LAI (as an inter_task_lai_T) */
    rr_plmn_select_cnf.location_area_identification.location_area_code[0] =
      (byte)(lai_ptr->location_area_code / 256);
    rr_plmn_select_cnf.location_area_identification.location_area_code[1] =
      (byte)(lai_ptr->location_area_code % 256);

    rr_plmn_select_cnf.location_area_identification.PLMN_id = lai_ptr->plmn_id;
    rr_plmn_select_cnf.arfcn                                = sys_info_db_ptr->BCCH_ARFCN;
    rr_plmn_select_cnf.bsic                                 = CONVERT_BSIC_TO_BYTE(sys_info_db_ptr->BSIC);

    /* store the new values for the cell information */
    rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN = sys_info_db_ptr->BCCH_ARFCN;
    rr_mm_send_data_ptr->rr_previous_BSIC       = sys_info_db_ptr->BSIC;
    rr_mm_send_data_ptr->rr_previous_LAI        = *lai_ptr;
    rr_plmn_select_cnf.cell_identity_valid      = sys_info_db_ptr->cell_identity_valid_flag;
    rr_plmn_select_cnf.cell_identity            = sys_info_db_ptr->cell_identity; 
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
    rr_plmn_select_cnf.cell_supports_peo = rr_cell_supports_peo();
#else  
    rr_plmn_select_cnf.cell_supports_peo = FALSE;
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/


#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
    rr_update_c1_delta(RR_C1_DELTA_AFTER_PLMN_SELECT);
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

    rr_hz_update_zone_info(RR_HZ_NOTIFY_ON_CHANGE);
  }
  else
  {
    /* set the LAI to INVALID values, which should be ignored by MM */
    rr_plmn_select_cnf.location_area_identification.location_area_code[0] = INVALID_LA_BYTE;
    rr_plmn_select_cnf.location_area_identification.location_area_code[1] = INVALID_LA_BYTE;
    rr_plmn_select_cnf.location_area_identification.PLMN_id.identity[0]   = INVALID_PLMN_ID;
    rr_plmn_select_cnf.location_area_identification.PLMN_id.identity[1]   = INVALID_PLMN_ID;
    rr_plmn_select_cnf.location_area_identification.PLMN_id.identity[2]   = INVALID_PLMN_ID;
    rr_plmn_select_cnf.arfcn.num                                          = INVALID_ARFCN;
    rr_plmn_select_cnf.arfcn.band                                         = SYS_BAND_NONE;
    rr_plmn_select_cnf.bsic                                               = 0xFF;
    rr_plmn_select_cnf.cell_supports_peo                                  = FALSE;

    rr_hz_notify_state_change(RR_HZ_STATE_NO_SERVICE);
  }

  /* set the available plmn list if one was provided */
  if (available_plmn_list_ptr != NULL)
  {
    uint32 index;
    rr_plmn_select_cnf.available_plmn_list.length = available_plmn_list_ptr->length;
    for (index=0;index < available_plmn_list_ptr->length;++index)
    {
      rr_plmn_select_cnf.available_plmn_list.info[index] = available_plmn_list_ptr->info[index];
    }
  }
  else
  {
    rr_plmn_select_cnf.available_plmn_list.length = 0;
    memset(rr_plmn_select_cnf.available_plmn_list.info,0x0,sizeof(rr_plmn_select_cnf.available_plmn_list.info));
  }

#ifdef FEATURE_FEMTO_CSG
  rr_invalidate_csg_info(&rr_plmn_select_cnf.available_plmn_list);
#endif /* FEATURE_FEMTO_CSG */

  /* set the BCCH allocation if one was provided */
  if (bcch_allocation_ptr != NULL)
  {
    memscpy(
      rr_plmn_select_cnf.bcch_allocation,
      sizeof(rr_plmn_select_cnf.bcch_allocation),
      (void *)bcch_allocation_ptr,
      sizeof(rr_plmn_select_cnf.bcch_allocation)
    );
  }

  /* set the system information values */
  if (sys_info_db_ptr != NULL)
  {
    rr_plmn_select_cnf.ATT                      = sys_info_db_ptr->control_channel_description.ATT;
    rr_plmn_select_cnf.cell_access              = rr_get_cell_access_status(sys_info_db_ptr);
    rr_plmn_select_cnf.MSCR                     = sys_info_db_ptr->control_channel_description.MSCR;
    rr_plmn_select_cnf.reestablishment_allowed  = TRUE;
    rr_plmn_select_cnf.T3212_timeout_value      = sys_info_db_ptr->control_channel_description.T3212_timeout;

#ifdef FEATURE_RR_THERMAL_MITIGATION
    if (rr_thermal_ps_allowed())
#endif
    {
      rr_plmn_select_cnf.service_capability = rr_get_cell_service_capability(
        sys_info_db_ptr
      );

      rr_plmn_select_cnf.gprs_sys_info_present = rr_gprs_get_sys_info_for_mm(
        &rr_plmn_select_cnf.gprs_sys_info
      );
    }
#ifdef FEATURE_RR_THERMAL_MITIGATION
    else
    {
      rr_plmn_select_cnf.service_capability     = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
    }
#endif
  }
  else
  {
    /* use default values */
    rr_plmn_select_cnf.ATT                      = 0;
    rr_plmn_select_cnf.cell_access              = MM_AS_ACCESS_ALL_CALLS;
    rr_plmn_select_cnf.MSCR                     = 0;
    rr_plmn_select_cnf.reestablishment_allowed  = FALSE;
    rr_plmn_select_cnf.service_capability       = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
    rr_plmn_select_cnf.T3212_timeout_value      = 0;
  }

  if (service_state != MM_AS_NO_SERVICE && sys_info_db_ptr != NULL)
  {
    rr_plmn_select_cnf.band_pref = rr_convert_band_info(sys_info_db_ptr->BCCH_ARFCN.band);
  }
  else
  {
    rr_plmn_select_cnf.band_pref = SYS_BAND_MASK_EMPTY;
  }


  rr_plmn_select_cnf.gmm_suspended = grr_is_gmm_suspended();
  rr_plmn_select_cnf.t3122_running = rr_get_t3122_running_for_nas();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_complete_nas_service_cnf(&rr_plmn_select_cnf);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_send_message_to_nas((IMH_T *)&rr_plmn_select_cnf);

  if (service_state == MM_AS_SERVICE_AVAILABLE)
  {
    rr_mm_service_available(
      rr_plmn_select_cnf.service_capability    // service_capability
    );
  }
  else
  {
    rr_mm_service_not_available();
  }

  if (rr_plmn_select_cnf.gprs_sys_info_present)
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = TRUE;
    rr_mm_send_data_ptr->gprs_sys_info = rr_plmn_select_cnf.gprs_sys_info;
  }
  else
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = FALSE;
  }

} /* rr_send_plmn_select_cnf */


/*
 * Send a RR_PLMN_SELECT_CNF to MM indicating no service
 */
void rr_send_plmn_select_cnf_no_service(
  mm_as_status_T error_code
   
)
{
  rr_send_plmn_select_cnf(
    error_code,         // error_code
    MM_AS_NO_SERVICE,   // service_state
    NULL,               // lai_ptr
    NULL,               // available_plmn_list_ptr
    NULL,               // bcch_allocation_ptr
    NULL               // sys_info_db_ptr
  );

} /* end rr_send_plmn_select_cnf_no_service() */


/*
 * Send a RR_PLMN_SELECT_CNF to MM indicating success
 */
void rr_send_plmn_select_cnf_success(
   
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

  rr_send_plmn_select_cnf(
    MM_AS_SUCCESS,                                          // error_code
    MM_AS_SERVICE_AVAILABLE,                                // service_state
    &(scell_info_ptr->gsm.location_area_identification),    // lai_ptr
    NULL,                                                   // available_plmn_list_ptr
    scell_info_ptr->gsm.BCCH_allocation,                    // bcch_allocation_ptr
    &scell_info_ptr->gsm                                   // sys_info_db_ptr
  );

} /* end rr_send_plmn_select_cnf_success() */


#ifdef FEATURE_GERAN_CS_SUPPORT
/**
 * Send the RR_REL_IND primitive to MM
 *
 * @param RR_cause Cause the RR connection was released.
 */
void rr_send_rel_ind(
  byte RR_cause
   
)
{
  rr_rel_ind_T rr_rel_ind;

  RR_PREPARE_MM_MESSAGE(rr_rel_ind, RR_REL_IND);

  rr_rel_ind.RR_cause = RR_cause;

  rr_send_message_to_nas((IMH_T *)&rr_rel_ind);
} /* rr_send_rel_ind */
#endif /* FEATURE_GERAN_CS_SUPPORT */


/**
 * Send the RR_SERVICE_IND message to MM
 *
 * @param service_state The service state from the RR perspective
 *
 * @param lai           The LAI of the current cell
 *
 * @param si_db_ptr     Pointer to the system information database for the cell. May be NULL
 *                      if no service is being provided
 */
void rr_send_service_ind(
  mm_as_service_state_T         service_state,
  LAI_T                         *lai_ptr,
  system_information_database_T *si_db_ptr
)
{
  rr_service_ind_T   rr_service_ind;
  rr_mm_send_data_t *rr_mm_send_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(si_db_ptr);

  // Obtain a pointer to the module data
  rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  RR_PREPARE_MM_MESSAGE(rr_service_ind, RR_SERVICE_IND);

  /* set the service state and band indicator */
  rr_service_ind.service_state  = service_state;
  rr_service_ind.band_indicator = rr_gapi_get_band_indicator_for_nas(
    (boolean)(service_state == MM_AS_SERVICE_AVAILABLE)    // in_service
  );

  if (service_state == MM_AS_SERVICE_AVAILABLE)
  {
    //RR_NULL_CHECK_FATAL(lai_ptr);

    rr_service_ind.location_area_identification.location_area_code[0] =
      (byte)(lai_ptr->location_area_code / 256);
    rr_service_ind.location_area_identification.location_area_code[1] =
      (byte)(lai_ptr->location_area_code % 256);
    rr_service_ind.location_area_identification.PLMN_id = lai_ptr->plmn_id;

    /* set the system information values */
    rr_service_ind.ATT = si_db_ptr->control_channel_description.ATT;
    rr_service_ind.reestablishment_allowed = TRUE;
    rr_service_ind.T3212_timeout_value = si_db_ptr->control_channel_description.T3212_timeout;
    rr_service_ind.MSCR = si_db_ptr->control_channel_description.MSCR;
    rr_service_ind.cell_identity_valid = si_db_ptr->cell_identity_valid_flag;
    rr_service_ind.cell_identity = si_db_ptr->cell_identity;
    /* @@TODO: Add BCCH allocation value here ? */
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
    if (!ARFCNS_EQUAL(si_db_ptr->BCCH_ARFCN, rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN))
    {
      rr_update_c1_delta(RR_C1_DELTA_AFTER_CELL_CHANGE);
    }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

    /* cell changed is determined by using LAI, BCCH_ARFCN,
     * and BSIC to determine if the cell is different
     */
    if ((!ARFCNS_EQUAL(si_db_ptr->BCCH_ARFCN, rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN)) ||
        (!rr_same_lai(lai_ptr, &rr_mm_send_data_ptr->rr_previous_LAI))                      ||
        (!rr_same_BSIC(&(si_db_ptr->BSIC), &rr_mm_send_data_ptr->rr_previous_BSIC))
       )
    {
      /* cell has changed from previous time this message was sent */
      rr_service_ind.cell_changed = TRUE;
    }

    /* store the new values for the cell information */
    rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN = si_db_ptr->BCCH_ARFCN;
    rr_mm_send_data_ptr->rr_previous_BSIC       = si_db_ptr->BSIC;
    rr_mm_send_data_ptr->rr_previous_LAI        = *lai_ptr;

    /* cell access status of the serving cell */
    rr_service_ind.cell_access = rr_get_cell_access_status(si_db_ptr);

    /* the service capability of the serving cell */
#ifdef FEATURE_RR_THERMAL_MITIGATION
    if (rr_thermal_ps_allowed())
#endif
    {
      rr_service_ind.service_capability = rr_get_cell_service_capability(
        si_db_ptr   // si_db_ptr
      );
      rr_service_ind.gprs_sys_info_present = rr_gprs_get_sys_info_for_mm(
        &rr_service_ind.gprs_sys_info    // mm_gprs_sys_info_ptr
      );
    }
#ifdef FEATURE_RR_THERMAL_MITIGATION
    else
    {
      rr_service_ind.service_capability = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
    }
#endif

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
    rr_service_ind.cell_supports_peo = rr_cell_supports_peo();
#else  
    rr_service_ind.cell_supports_peo = FALSE;
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

    rr_service_ind.arfcn = si_db_ptr->BCCH_ARFCN;
    rr_service_ind.bsic  = CONVERT_BSIC_TO_BYTE(si_db_ptr->BSIC);
    rr_service_ind.band_pref = rr_convert_band_info(si_db_ptr->BCCH_ARFCN.band);

    /* log network selection mode */
    rr_log_cell_camp_status();

    /* log that RR is in service */
    rr_log_service_change(
      TRUE     // in_service
    );

    rr_gprs_odm_update_cell_type(
      TRUE     // in_service
    );

    rr_hz_update_zone_info(
      RR_HZ_NOTIFY_ON_CHANGE   // bNotify
    );
  }
  else /* no service */
  {
    /* set the cell change to false and the cell access to all calls */
    rr_service_ind.cell_access = MM_AS_ACCESS_ALL_CALLS;
    rr_service_ind.cell_changed = FALSE;

    /* set service capability to some reasonable value */
    rr_service_ind.service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

    /* set the LAI to INVALID values, which should be ignored by MM */
    rr_service_ind.location_area_identification.location_area_code[0] = INVALID_LA_BYTE;
    rr_service_ind.location_area_identification.location_area_code[1] = INVALID_LA_BYTE;
    rr_service_ind.location_area_identification.PLMN_id.identity[0]   = INVALID_PLMN_ID;
    rr_service_ind.location_area_identification.PLMN_id.identity[1]   = INVALID_PLMN_ID;
    rr_service_ind.location_area_identification.PLMN_id.identity[2]   = INVALID_PLMN_ID;

    rr_service_ind.arfcn.num                                          = INVALID_ARFCN;
    rr_service_ind.arfcn.band                                         = SYS_BAND_NONE;
    rr_service_ind.bsic                                               = 0xFF;
    rr_service_ind.band_pref                                          = SYS_BAND_MASK_EMPTY;
    rr_service_ind.cell_supports_peo                                  = FALSE;

    /* log the loss of service in RR */
    rr_log_service_change(FALSE);
    rr_gprs_odm_update_cell_type(FALSE);

    rr_hz_notify_state_change(RR_HZ_STATE_NO_SERVICE);
  }

  rr_service_ind.gmm_suspended = grr_is_gmm_suspended();

#ifdef FEATURE_GPRS_PS_HANDOVER
  if (rr_psho_in_progress())
  {
    rr_service_ind.ps_handover_indicator = TRUE;
  }
  else
  {
    rr_service_ind.ps_handover_indicator = FALSE;
  }
#endif /* FEATURE_GPRS_PS_HANDOVER */

  rr_service_ind.t3122_running = rr_get_t3122_running_for_nas();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_complete_nas_service_ind(&rr_service_ind);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  rr_send_message_to_nas((IMH_T *)&rr_service_ind);

  rr_gmm_gprs_service_req_expected();

  if (service_state == MM_AS_SERVICE_AVAILABLE)
  {
    rr_mm_service_available(
      rr_service_ind.service_capability    // service_capability
    );
  }
  else
  {
    rr_mm_service_not_available();
  }

  if (rr_service_ind.gprs_sys_info_present)
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = TRUE;
    rr_mm_send_data_ptr->gprs_sys_info = rr_service_ind.gprs_sys_info;
  }
  else
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = FALSE;
  }

} /* rr_send_service_ind */

#ifdef FEATURE_SGLTE

void rr_sglte_srvcc_send_abort_ind( )
{
  rr_abort_ind_T rr_abort_ind;

  RR_PREPARE_MM_MESSAGE(rr_abort_ind, RR_ABORT_IND);

  rr_abort_ind.RR_abort_cause = OTHER_REASON;

  rr_abort_ind.failure_cause = MM_RR_SGLTE_SRVCC_CS_HANDOVER;

  rr_send_message_to_nas((IMH_T *)&rr_abort_ind);
}

void rr_sglte_srvcc_send_no_service_ind(
  mm_as_service_state_T         service_state
)
{
  rr_service_ind_T   rr_service_ind;
  RR_PREPARE_MM_MESSAGE(rr_service_ind, RR_SERVICE_IND);

  /* set the service state and band indicator */
  rr_service_ind.service_state  = service_state;
  rr_service_ind.band_indicator = rr_get_band_indicator_for_nas_dyn((boolean)(service_state == MM_AS_SERVICE_AVAILABLE));


    /* set the cell change to false and the cell access to all calls */
  rr_service_ind.cell_access = MM_AS_ACCESS_ALL_CALLS;
  rr_service_ind.cell_changed = FALSE;

    /* set service capability to some reasonable value */
  rr_service_ind.service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

    /* set the LAI to INVALID values, which should be ignored by MM */
  rr_service_ind.location_area_identification.location_area_code[0] = INVALID_LA_BYTE;
  rr_service_ind.location_area_identification.location_area_code[1] = INVALID_LA_BYTE;
  rr_service_ind.location_area_identification.PLMN_id.identity[0]   = INVALID_PLMN_ID;
  rr_service_ind.location_area_identification.PLMN_id.identity[1]   = INVALID_PLMN_ID;
  rr_service_ind.location_area_identification.PLMN_id.identity[2]   = INVALID_PLMN_ID;

    /* log the loss of service in RR */
  rr_log_service_change(FALSE);
  rr_gprs_odm_update_cell_type(FALSE);

  rr_service_ind.band_pref = SYS_BAND_MASK_EMPTY;

  rr_hz_notify_state_change(RR_HZ_STATE_NO_SERVICE);
  rr_service_ind.arfcn.num = INVALID_ARFCN;
  rr_service_ind.arfcn.band= SYS_BAND_NONE;
  rr_service_ind.bsic = 0xff;

  rr_service_ind.gmm_suspended = grr_is_gmm_suspended();

#ifdef FEATURE_GPRS_PS_HANDOVER
  if (rr_psho_in_progress())
  {
    rr_service_ind.ps_handover_indicator = TRUE;
  }
  else
  {
    rr_service_ind.ps_handover_indicator = FALSE;
  }
#endif /* FEATURE_GPRS_PS_HANDOVER */

  rr_service_ind.t3122_running = rr_get_t3122_running_for_nas();


#ifdef FEATURE_DUAL_SIM
  rr_ms_complete_nas_service_ind(&rr_service_ind);
#endif /* FEATURE_DUAL_SIM */
  rr_send_message_to_nas((IMH_T *)&rr_service_ind);

  rr_gmm_gprs_service_req_expected();

  if (service_state == MM_AS_SERVICE_AVAILABLE)
  {
    rr_mm_service_available(rr_service_ind.service_capability);
  }
  else
  {
    rr_mm_service_not_available();
  }

} /* rr_send_service_ind */

#endif /*FEATURE_SGLTE*/


/**
 * Send the RR_SERVICE_IND_FROM_OTA_MSG message to MM
 *
 * @param service_state The service state from the RR perspective
 *
 * @param lai           The LAI of the current cell
 *
 * @param si_db_ptr     Pointer to the system information database for the cell. May be NULL
 *                      if no service is being provided
 */

#ifdef FEATURE_GSM_DTM

void rr_send_service_ind_from_ota_msg(
  mm_as_service_state_T         service_state,
  LAI_T                         *lai_ptr,
  system_information_database_T *si_db_ptr
)
{
  rr_mm_send_data_t *rr_mm_send_data_ptr;
  rr_service_ind_T   rr_service_ind_from_ota_msg;

  //RR_NULL_CHECK_RETURN_VOID(si_db_ptr);

  // Obtain a pointer to the module data
  rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  RR_PREPARE_MM_MESSAGE(rr_service_ind_from_ota_msg, RR_SERVICE_IND_FROM_OTA_MSG);

  /* set the service state and band indicator */
  rr_service_ind_from_ota_msg.service_state  = service_state;
  rr_service_ind_from_ota_msg.band_indicator = rr_gapi_get_band_indicator_for_nas(
    (boolean)(service_state == MM_AS_SERVICE_AVAILABLE)    // in_service
  );

  if (service_state == MM_AS_SERVICE_AVAILABLE)
  {
    //RR_NULL_CHECK_FATAL(lai_ptr);

    rr_service_ind_from_ota_msg.location_area_identification.location_area_code[0] =
      (byte)(lai_ptr->location_area_code / 256);
    rr_service_ind_from_ota_msg.location_area_identification.location_area_code[1] =
      (byte)(lai_ptr->location_area_code % 256);
    rr_service_ind_from_ota_msg.location_area_identification.PLMN_id = lai_ptr->plmn_id;

    /* set the system information values */
    rr_service_ind_from_ota_msg.ATT = si_db_ptr->control_channel_description.ATT;
    rr_service_ind_from_ota_msg.reestablishment_allowed = TRUE;
    rr_service_ind_from_ota_msg.T3212_timeout_value = si_db_ptr->control_channel_description.T3212_timeout;
    rr_service_ind_from_ota_msg.MSCR = si_db_ptr->control_channel_description.MSCR;
    rr_service_ind_from_ota_msg.cell_identity_valid = si_db_ptr->cell_identity_valid_flag;
    rr_service_ind_from_ota_msg.cell_identity            = si_db_ptr->cell_identity;
    /* @@TODO: Add BCCH allocation value here ? */

    /* cell changed is determined by using LAI, BCCH_ARFCN,
     * and BSIC to determine if the cell is different
     */
    if ((!ARFCNS_EQUAL(si_db_ptr->BCCH_ARFCN, rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN)) ||
        (!rr_same_lai(lai_ptr, &rr_mm_send_data_ptr->rr_previous_LAI))                      ||
        (!rr_same_BSIC(&(si_db_ptr->BSIC), &rr_mm_send_data_ptr->rr_previous_BSIC))
       )
    {
      /* cell has changed from previous time this message was sent */
      rr_service_ind_from_ota_msg.cell_changed = TRUE;
    }

    /* store the new values for the cell information */
    rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN = si_db_ptr->BCCH_ARFCN;
    rr_mm_send_data_ptr->rr_previous_BSIC       = si_db_ptr->BSIC;
    rr_mm_send_data_ptr->rr_previous_LAI        = *lai_ptr;

    /* cell access status of the serving cell */
    rr_service_ind_from_ota_msg.cell_access = rr_get_cell_access_status(si_db_ptr);

    rr_service_ind_from_ota_msg.band_pref = rr_convert_band_info(si_db_ptr->BCCH_ARFCN.band);

    /* the service capability of the serving cell */
#ifdef FEATURE_RR_THERMAL_MITIGATION
    if (rr_thermal_ps_allowed())
#endif
    {
      rr_service_ind_from_ota_msg.service_capability = rr_get_cell_service_capability(
        si_db_ptr    // si_db_ptr
      );

      rr_service_ind_from_ota_msg.gprs_sys_info_present = rr_gprs_get_sys_info_for_mm(
        &rr_service_ind_from_ota_msg.gprs_sys_info   // mm_gprs_sys_info_ptr
      );
    }
#ifdef FEATURE_RR_THERMAL_MITIGATION
    else
    {
      rr_service_ind_from_ota_msg.service_capability = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
    }
#endif

    rr_service_ind_from_ota_msg.arfcn = si_db_ptr->BCCH_ARFCN;
    rr_service_ind_from_ota_msg.bsic  = CONVERT_BSIC_TO_BYTE(si_db_ptr->BSIC);

    /* log that RR is in service */
    rr_log_service_change(
      TRUE     // in_service
    );

    rr_gprs_odm_update_cell_type(
      TRUE     // in_service
    );
  }
  else /* no service */
  {
    /* set the cell change to false and the cell access to all calls */
    rr_service_ind_from_ota_msg.cell_access = MM_AS_ACCESS_ALL_CALLS;
    rr_service_ind_from_ota_msg.cell_changed = FALSE;

    /* set service capability to some reasonable value */
    rr_service_ind_from_ota_msg.service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

    /* set the LAI to INVALID values, which should be ignored by MM */
    rr_service_ind_from_ota_msg.location_area_identification.location_area_code[0] = INVALID_LA_BYTE;
    rr_service_ind_from_ota_msg.location_area_identification.location_area_code[1] = INVALID_LA_BYTE;
    rr_service_ind_from_ota_msg.location_area_identification.PLMN_id.identity[0]   = INVALID_PLMN_ID;
    rr_service_ind_from_ota_msg.location_area_identification.PLMN_id.identity[1]   = INVALID_PLMN_ID;
    rr_service_ind_from_ota_msg.location_area_identification.PLMN_id.identity[2]   = INVALID_PLMN_ID;

    rr_service_ind_from_ota_msg.arfcn.num  = INVALID_ARFCN;
    rr_service_ind_from_ota_msg.arfcn.band = SYS_BAND_NONE;
    rr_service_ind_from_ota_msg.bsic       = 0xFF;
    rr_service_ind_from_ota_msg.band_pref  = SYS_BAND_MASK_EMPTY;

    /* log the loss of service in RR */
    rr_log_service_change(
      FALSE    // in_service
    );

    rr_gprs_odm_update_cell_type(
      FALSE    // in_service
    );
  }

  rr_service_ind_from_ota_msg.gmm_suspended = grr_is_gmm_suspended();
  rr_service_ind_from_ota_msg.t3122_running = rr_get_t3122_running_for_nas();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_complete_nas_service_ind(&rr_service_ind_from_ota_msg);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  rr_send_message_to_nas((IMH_T *)&rr_service_ind_from_ota_msg);

  rr_mm_service_available(
    rr_service_ind_from_ota_msg.service_capability   // service_capability
  );

  if (rr_service_ind_from_ota_msg.gprs_sys_info_present)
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = TRUE;
    rr_mm_send_data_ptr->gprs_sys_info = rr_service_ind_from_ota_msg.gprs_sys_info;
  }
  else
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = FALSE;
  }

} /* rr_send_service_ind_from_ota_msg */

#ifdef FEATURE_GSM_EDTM
void rr_send_est_ind(
   
)
{
  rr_est_ind_T rr_est_ind;

  RR_PREPARE_MM_MESSAGE(rr_est_ind, RR_EST_IND);

  rr_send_message_to_nas((IMH_T *)&rr_est_ind);
} /* rr_send_est_ind */
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */


#ifdef FEATURE_GERAN_CS_SUPPORT
boolean rr_abort_pending_rr_est_req(
  byte RR_abort_cause
   
)
{
  rr_cmd_bdy_type *saved_msg_ptr;

  boolean bDiscarded = FALSE;
  rr_event_T saved_event = rr_examine_saved_message(&saved_msg_ptr);

  if ((saved_event == EV_INPUT_MESSAGE) && (saved_msg_ptr != NULL))
  {
    byte saved_message_set = saved_msg_ptr->message_header.message_set;
    byte saved_message_id = saved_msg_ptr->message_header.message_id;

    if ((saved_message_set == MS_MM_RR) && (saved_message_id == (byte)RR_EST_REQ))
    {
      MSG_GERAN_HIGH_1_G("Abort pending RR_EST_REQ with RR_ABORT_IND(0x%02x)", RR_abort_cause);
      rr_send_abort_ind(RR_abort_cause, MM_RR_OTHER_FAILURE);
      grr_throwaway_saved_message();
      bDiscarded = TRUE;
    }
  }

  return bDiscarded;

} /* end rr_abort_pending_rr_est_req() */


/**
 * Send an ABORT_IND to MM if there is a LAI change and a RR_EST_REQ is pending
 * if RR_EST_REQ is pending then the saved message was discarded
 */
void rr_abort_pending_rr_est_req_if_lai_changed(
  LAI_T *current_lai_ptr
   
)
{
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  MSG_GERAN_HIGH_0_G("Checking for LAI change and pending RR_EST_REQ");

  /* rr_previous_LAI is a static variable available only in this file */
  if (!rr_same_lai(current_lai_ptr, &rr_mm_send_data_ptr->rr_previous_LAI))
  {
    rr_cmd_bdy_type *saved_msg_ptr;

    rr_event_T saved_event = rr_examine_saved_message(&saved_msg_ptr);

    if ((saved_event == EV_INPUT_MESSAGE) && (saved_msg_ptr != NULL))
    {
      byte saved_message_set = saved_msg_ptr->message_header.message_set;
      byte saved_message_id = saved_msg_ptr->message_header.message_id;

      if ((saved_message_set == MS_MM_RR) && (saved_message_id == (byte)RR_EST_REQ))
      {
        if ((saved_msg_ptr->rr_est_req.establishment_cause == (byte)MM_RR_CALL_REEST) ||
            (saved_msg_ptr->rr_est_req.establishment_cause == (byte)MM_RR_EMERGENCY_CALL))
        {
          MSG_GERAN_HIGH_1_G("RR_EST_REQ for cause %d takes priority over LAU.", 
            saved_msg_ptr->rr_est_req.establishment_cause);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("Call aborted because LAU expected");
          rr_send_abort_ind(RR_ABORT_LAI_CHANGED, MM_RR_OTHER_FAILURE);
          grr_throwaway_saved_message();
        }
      }
    }
  }
}  /* rr_abort_pending_rr_est_req_if_lai_changed */
#endif /* FEATURE_GERAN_CS_SUPPORT */

/**
 * Send a RR_SERVICE_IND to MM indicating no service is available
 */
void rr_send_no_service_ind(
   
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

  rr_send_service_ind(
    MM_AS_NO_SERVICE,       // service_state
    NULL,                   // lai_ptr - not required for NO SERVICE
    &scell_info_ptr->gsm   // si_db_ptr
  );

} /* rr_send_no_service_ind */


/**
 * Send the RR_SERVICE_IND message to MM when a PLMN search is prioritised to the foreground
 *
 *
 */
void rr_send_no_service_for_plmn_list( )
{
  rr_service_ind_T   rr_service_ind;
  rr_mm_send_data_t *rr_mm_send_data_ptr;

  RR_PREPARE_MM_MESSAGE(rr_service_ind, RR_SERVICE_IND);

  /* set the service state, failure cause and band indicator */
  /* N.B. this code intentionally avoids a NO_RESOURCE cause */
  rr_service_ind.service_state  = MM_AS_NO_SERVICE;
  rr_service_ind.failure_cause  = MM_RR_OTHER_FAILURE;
  rr_service_ind.band_indicator = rr_gapi_get_band_indicator_for_nas(FALSE);

  /* set the cell change to false and the cell access to all calls */
  rr_service_ind.cell_access  = MM_AS_ACCESS_ALL_CALLS;
  rr_service_ind.cell_changed = FALSE;

  /* set service capability to some reasonable value */
  rr_service_ind.service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

  /* set the LAI to INVALID values, which should be ignored by MM */
  rr_service_ind.location_area_identification.location_area_code[0] = INVALID_LA_BYTE;
  rr_service_ind.location_area_identification.location_area_code[1] = INVALID_LA_BYTE;
  rr_service_ind.location_area_identification.PLMN_id.identity[0]   = INVALID_PLMN_ID;
  rr_service_ind.location_area_identification.PLMN_id.identity[1]   = INVALID_PLMN_ID;
  rr_service_ind.location_area_identification.PLMN_id.identity[2]   = INVALID_PLMN_ID;

  /* set the serving ARFCN, BSIC and band to INVALID values for the same reason */
  rr_service_ind.arfcn.num                                          = INVALID_ARFCN;
  rr_service_ind.arfcn.band                                         = SYS_BAND_NONE;
  rr_service_ind.bsic                                               = 0xFF;
  rr_service_ind.band_pref                                          = SYS_BAND_MASK_EMPTY;

  /* set the remaining information used by NAS */
  rr_service_ind.gmm_suspended         = grr_is_gmm_suspended();
  rr_service_ind.ps_handover_indicator = FALSE;
  rr_service_ind.t3122_running         = rr_get_t3122_running_for_nas();
  rr_service_ind.gprs_sys_info_present = FALSE;

  /* log the loss of service in RR */
  rr_log_service_change(FALSE);
  rr_gprs_odm_update_cell_type(FALSE);

  /* send the NO_SERVICE indication to NAS */
  rr_send_message_to_nas((IMH_T *)&rr_service_ind);

  /* and then update appropriate local GRR data */
  rr_gmm_gprs_service_req_expected();
  rr_mm_service_not_available();

  rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  if (rr_mm_send_data_ptr != NULL)
  {
    rr_mm_send_data_ptr->gprs_sys_info_present = FALSE;
  }

  return;
} /* rr_send_no_service_for_plmn_list */

#ifdef FEATURE_GERAN_CS_SUPPORT
/**
 * Send the RR_SYNC_IND primitive to MM
 *
 * @param sync_reason  Reason the sync message is being sent
 *
 * @param channel_type Type of channel involved
 *
 * @param channel_mode Mode of the channel
 *
 * @param subchannel   Subchannel number
 */
void rr_send_sync_ind(
  sync_reason_T sync_reason,
  channel_type_T channel_type,
  channel_mode_T channel_mode,
  byte subchannel
   
)
{
  rr_sync_ind_T rr_sync_ind;
  boolean       ciphering_started = rr_has_ciphering_started();

#ifdef FEATURE_VAMOS_II
  channel_mode = rr_remove_vamos2_indication_from_channel_mode(channel_mode);
#endif /*FEATURE_VAMOS_II*/

  RR_PREPARE_MM_MESSAGE(rr_sync_ind, RR_SYNC_IND);

  rr_sync_ind.sync_reason = sync_reason;
  rr_sync_ind.channel_mode = channel_mode;
  rr_sync_ind.subchannel = subchannel;
  rr_sync_ind.channel_type = channel_type;
#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
  /* pass the current ciphering status to NAS */
  rr_sync_ind.ciphering_enabled = ciphering_started;
#else
  MSG_GERAN_HIGH_2_G("RR_SYNC_IND(reason=%d,ciphering=%1d)", (int)sync_reason, (int)ciphering_started);
#endif /* FEATURE_UMTS_VOICE_CIPHERING_IND */


  rr_send_message_to_nas((IMH_T *)&rr_sync_ind);
} /* rr_send_sync_ind */
#endif /* FEATURE_GERAN_CS_SUPPORT */


/**
 * Send the RR_STOP_GSM_MODE_CNF primitive to MM
 *
 * @param status The status to send in the response
 */
void rr_send_stop_gsm_mode_cnf(
  mm_as_status_T status
)
{
  rr_stop_gsm_mode_cnf_T rr_stop_gsm_mode_cnf;

  RR_PREPARE_MM_MESSAGE(rr_stop_gsm_mode_cnf, RR_STOP_GSM_MODE_CNF);

  rr_stop_gsm_mode_cnf.status = status;

  rr_send_message_to_nas((IMH_T *)&rr_stop_gsm_mode_cnf);

  /* Fix for CR 2876405*/
  rr_static_set_stop_gsm_mode_cnf_pending(FALSE);

  rr_mm_service_not_available();
} /* rr_send_stop_gsm_mode_cnf */

/**
 * Send the RR_ACTIVATION_IND primitive to MM
 *
 */
void rr_send_activation_ind(
  mm_as_status_T status
   
)
{
  rr_activation_ind_T rr_activation_ind;

  RR_PREPARE_MM_MESSAGE(rr_activation_ind, RR_ACTIVATION_IND);

  rr_activation_ind.status = status;


  rr_send_message_to_nas((IMH_T *)&rr_activation_ind);
} /* rr_send_activation_ind */

/**
 * Send the RR_CELL_CHANGE_IND to CB
 *
 * @param location_area_identification
 *                      LAI associated with the cell
 *
 * @param cell_identity Identity of the cell
 */
void rr_send_cell_change_ind(
  LAI_T location_area_identification,
  word cell_identity,boolean cbch_present
   
)
{
  rr_cell_change_ind_T rr_cell_change_ind;
  byte index;

  RR_PREPARE_CB_MESSAGE(rr_cell_change_ind, RR_CELL_CHANGE_IND);

  for (index = 0; index < 3; index++)
  {
    rr_cell_change_ind.plmn_id[index] = location_area_identification.plmn_id.identity[index];
  }

  rr_cell_change_ind.lac[0] = (byte)(location_area_identification.location_area_code / 256);
  rr_cell_change_ind.lac[1] = (byte)(location_area_identification.location_area_code % 256);

  rr_cell_change_ind.cell_id[0] = (byte)(cell_identity / 256);
  rr_cell_change_ind.cell_id[1] = (byte)(cell_identity % 256);
  rr_cell_change_ind.CBCH_present = cbch_present;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_complete_nas_cb_ind(&rr_cell_change_ind);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_send_message_to_nas((IMH_T *)&rr_cell_change_ind);
}

/*===========================================================================*/
/**
 * Send the CM_STATUS_IND message to CM indicating the
 * current signal strength of the serving cell in units
 * of absolute dBm
 *
 * Uses the current_signal_strength global variable.
 */
/*===========================================================================*/
void rr_send_cm_status_ind(
   
)
{
  cm_rpt_type *cmd_ptr;

  /* allocate a message buffer using the Call Manager interface */
  cmd_ptr = cm_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    rr_measured_signal_data_t *rr_measured_signal_data_ptr;
    byte                       signal_strength;

    /* the signal strength needs to be mapped between the RXLEV_average units used in GSM
     * and the absolute dBm expected by CM/UI.
     */
    rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr();
    signal_strength = rr_convert_rxlex_to_absolute_dBm(rr_measured_signal_data_ptr->current_signal_strength);

    /* format the primitive and send to the destination task */
    cmd_ptr->hdr.cmd = CM_RR_STATUS_IND;
    cmd_ptr->cmd.signal_status.signal_strength = (byte)signal_strength;
    cmd_ptr->cmd.signal_status.bit_error_rate  = rr_measured_signal_data_ptr->current_signal_quality;

    cm_put_cmd(cmd_ptr);

    MSG_GERAN_MED_0_G("RR sent CM_RR_STATUS_IND to CM");
  }

  return;
} /* rr_send_cm_status_ind */

/*===========================================================================*/
/**
 * Send the CM_RR_CELL_BROADCAST_INFO_IND message to CM giving an indication
 * of cell broadcast support.
 */
/*===========================================================================*/
void rr_send_cb_info(
  boolean cbch_present
   
)
{
  cm_rpt_type *cmd_ptr;

  /* allocate a message buffer using the Call Manager interface */
  cmd_ptr = cm_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* format the primitive and send to the destination task */
    cmd_ptr->hdr.cmd = CM_RR_CELL_BROADCAST_INFO_IND;

    if (cbch_present)
    {
      cmd_ptr->cmd.cell_bc_info.cell_bc_capability = SYS_CELL_BROADCAST_CAPABILITY_ON;
    }
    else
    {
      cmd_ptr->cmd.cell_bc_info.cell_bc_capability = SYS_CELL_BROADCAST_CAPABILITY_OFF;
    }

    cm_put_cmd(cmd_ptr);

    MSG_GERAN_MED_1_G("RR sent CM_RR_CELL_BROADCAST_INFO_IND to CM, CB cap:%d",cbch_present);
  }

  return;
} /* rr_send_cb_info */

/**
 * Send the RR_GMM_GPRS_SUSPENSION primitive to GMM
 *
 */
void rr_send_grr_gmm_gprs_suspension_ind(
   
)
{
  rr_gmm_gprs_suspension_T rr_gmm_gprs_suspension;

  RR_PREPARE_MM_MESSAGE(rr_gmm_gprs_suspension, RR_GMM_GPRS_SUSPENSION);

  rr_send_message_to_nas((IMH_T *)&rr_gmm_gprs_suspension);
} /* end rr_send_grr_gmm_gprs_suspension_ind() */


/**
 * Send the RR_GMM_GPRS_RESUMPTION_IND primitive to GMM
 *
 */
#ifdef FEATURE_GSM_DTM
void rr_send_grr_gmm_gprs_resumption_ind(
  boolean gprs_resumption_ind,
  boolean cell_update_required
   
)
#else
void rr_send_grr_gmm_gprs_resumption_ind(
  boolean gprs_resumption_ind
   
)
#endif /* FEATURE_GSM_DTM */
{
  rr_gmm_gprs_resumption_ind_T rr_gmm_gprs_resumption;

  RR_PREPARE_MM_MESSAGE(rr_gmm_gprs_resumption, RR_GMM_GPRS_RESUMPTION_IND);

  rr_gmm_gprs_resumption.gprs_resumption_ind = gprs_resumption_ind;
#ifdef FEATURE_GSM_DTM
  rr_gmm_gprs_resumption.cell_update_required = cell_update_required;
#endif /* FEATURE_GSM_DTM */

  rr_send_message_to_nas((IMH_T *)&rr_gmm_gprs_resumption);

  rr_gmm_gprs_service_req_expected();

} /* end rr_send_grr_gmm_gprs_resumption_ind() */


void rr_LAI_with_PLMN_only(LAI_T *lai_ptr, PLMN_id_T *plmn_id_ptr)
{
  /* fill the LAI structure with invalid values */
  lai_ptr->location_area_code = INVALID_LAC_WORD;

  /* except for the PLMN identity, which is copied over */
  lai_ptr->plmn_id.identity[0] = plmn_id_ptr->identity[0];
  lai_ptr->plmn_id.identity[1] = plmn_id_ptr->identity[1];
  lai_ptr->plmn_id.identity[2] = plmn_id_ptr->identity[2];
}

void rr_send_rr_reselection_ind(
   
)
{
  rr_reselection_ind_T rr_reselection_ind;

  RR_PREPARE_MM_MESSAGE(rr_reselection_ind, RR_RESELECTION_IND);

  rr_send_message_to_nas((IMH_T *)&rr_reselection_ind);
} /* end rr_send_rr_reselection_ind() */


/**
 * Send the RR_GMM_GPRS_SERVICE_CNF primitive to GMM
 *
 */
void rr_send_rr_gmm_gprs_service_cnf(
   
)
{
  rr_gmm_gprs_service_cnf_T rr_gmm_gprs_service_cnf;

  RR_PREPARE_MM_MESSAGE(rr_gmm_gprs_service_cnf, RR_GMM_GPRS_SERVICE_CNF);

  rr_send_message_to_nas((IMH_T *)&rr_gmm_gprs_service_cnf);
} /* end rr_send_rr_gmm_gprs_service_cnf() */


#ifdef FEATURE_INTERRAT_PCCO_GTOW

void rr_send_gtow_cco_complete_ind(
   
)
{
  rr_gtow_cco_complete_ind_T gtow_cco_complete_ind;

  RR_PREPARE_MM_MESSAGE(gtow_cco_complete_ind, RR_GTOW_CCO_COMPLETE_IND);


  rr_send_message_to_nas((IMH_T *)&gtow_cco_complete_ind);
} /* end rr_send_rr_gprs_pccch_camp_cnf() */

#endif /* FEATURE_INTERRAT_PCCO_GTOW */

#ifdef FEATURE_GPRS_PS_HANDOVER

void rr_send_ps_handover_ind(
   
)
{
  rr_psho_ind_T rr_psho_ind;
  psho_details_t *psho_details = rr_psho_get_details();

  RR_NULL_CHECK_FATAL(psho_details);

  RR_PREPARE_MM_MESSAGE(rr_psho_ind, RR_PSHO_IND);

  if (psho_details->target_type == CSN_PSHO_GSM_TARGET_CELL)
  {
    rr_psho_ind.nas_container_present = psho_details->payload.a_gb.nas_container_present;

    memscpy(
      &rr_psho_ind.nas_container, 
      sizeof(rr_psho_ind.nas_container),
      psho_details->payload.a_gb.nas_container.nas_container_data, sizeof(nas_container_T)
    );

    rr_send_message_to_nas((IMH_T *)&rr_psho_ind);
  }
  else
  {
    MSG_GERAN_ERROR_1_G("PSHO target is not GSM cell, %d", psho_details->target_type);
  }
} /* end rr_send_ps_handover_ind() */

#endif /* FEATURE_GPRS_PS_HANDOVER */

/**
  @brief Sends CM_CHANGE_TEST_MODE_IND to CM.
*/
static void rr_send_cm_change_test_mode(
  cmtc_ue_mode_type_T mode_type
   
)
{
  cm_rpt_type *cm_cmd_ptr;

  if (!rr_multi_sim_api_valid())
  {
    return;
  }

  if ((cm_cmd_ptr = cm_get_cmd_buf()) != NULL) /* Get CM command */
  {
     // Format the header part of the primitive
     cm_cmd_ptr->hdr.cmd = CM_CHANGE_TEST_MODE_IND;

     // Format the command part of the primitive
     cm_cmd_ptr->cmd.tc_change_test_mode_ind.mode_type = mode_type;

     // Send the message to CM task
     cm_put_cmd(cm_cmd_ptr);

     MSG_GERAN_LOW_1_G("OMsg: CM_CHANGE_TEST_MODE_IND(%d)", mode_type);
  }
  else /* Allocation failure */
  {
     MSG_GERAN_ERROR_0_G("Failed to allocate buffer for CM_CHANGE_TEST_MODE_IND");
  }
}

void geran_send_cm_test_mode_on_dyn( )
{
  rr_send_cm_change_test_mode(CMTC_UE_TEST_SETUP_IN_PROGRESS);
}

void geran_send_cm_test_mode_off_dyn( )
{
  rr_send_cm_change_test_mode(CMTC_UE_NORMAL_MODE);
}

void rr_send_gmm_gprs_multislot_class_ind(
  uint8 egprs_mulislot_class,
#ifdef FEATURE_GSM_DTM
  uint8 dtm_multislot_class,
#endif
  uint8 gprs_multislot_class
   
)
{
  rr_gmm_gprs_multislot_class_ind_T multislot_class_ind;

  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  RR_PREPARE_MM_MESSAGE(multislot_class_ind, RR_GMM_GPRS_MULTISLOT_CLASS_IND);

  multislot_class_ind.edge_multi_slot_class = egprs_mulislot_class;
#ifdef FEATURE_GSM_DTM
  multislot_class_ind.dtm_multi_slot_subclass =
    rr_msc_conv_dtm_multislot_class_to_subclass(dtm_multislot_class);
#endif
  multislot_class_ind.multi_slot_class = gprs_multislot_class;

  rr_send_message_to_nas((IMH_T *)&multislot_class_ind);

  // Take a copy of the values just sent for comparison later
  rr_mm_send_data_ptr->multislot_class.egprs_multislot_class = egprs_mulislot_class;
#ifdef FEATURE_GSM_DTM
  rr_mm_send_data_ptr->multislot_class.dtm_multislot_class = dtm_multislot_class;
#endif
  rr_mm_send_data_ptr->multislot_class.gprs_multislot_class = gprs_multislot_class;
  rr_mm_send_data_ptr->multislot_class.gmm_updated = TRUE;

  rr_gmm_gprs_service_req_expected();
}

/**
  @brief Sends RR_GMM_GPRS_MS_RA_CAP_REQ to GMM.
*/
void rr_send_gmm_gprs_ms_ra_cap_req( )
{
  rr_gmm_gprs_ms_ra_cap_req_T ra_cap_req;

  RR_PREPARE_MM_MESSAGE(ra_cap_req, RR_GMM_GPRS_MS_RA_CAP_REQ);


  /* notify the PS domain module that a response is pending */
  rr_gmm_set_gprs_ra_cap_expected(TRUE);

  rr_send_message_to_nas((IMH_T *)&ra_cap_req);

  return;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

void rr_ms_send_rr_ps_abort_cnf( )
{
  rr_ps_abort_cnf_T rr_ps_abort_cnf;

  memset(&rr_ps_abort_cnf, 0, sizeof(rr_ps_abort_cnf_T));

  rr_ps_abort_cnf.message_header.message_set = MS_MM_RR;
  rr_ps_abort_cnf.message_header.message_id  = (byte)RR_PS_ABORT_CNF;

  RR_SET_MESSAGE_LENGTH(rr_ps_abort_cnf);

 

  rr_send_message_to_nas((IMH_T *)&rr_ps_abort_cnf);

  return;
}

/**
 * Send the Multi SIM version of the RR_SELECT_PLMN_CNF primitive to MM
 *
 * @param error_code     indicates if the message was processed or not
 */
void rr_ms_send_rr_plmn_select_cnf(
  const mm_as_status_T error_code,
  const mm_rr_failure_cause_e_type dsds_cause,
  const boolean as_oos
)
{
  rr_plmn_select_cnf_T rr_plmn_select_cnf;

  memset(&rr_plmn_select_cnf, 0, sizeof(rr_plmn_select_cnf_T));

  rr_plmn_select_cnf.message_header.message_set = MS_MM_RR;
  rr_plmn_select_cnf.message_header.message_id  = (byte)RR_PLMN_SELECT_CNF;
  RR_SET_MESSAGE_LENGTH(rr_plmn_select_cnf);

  /* set the error code & service state and band indicator */
  rr_plmn_select_cnf.error_code     = error_code;
  rr_plmn_select_cnf.service_state  = MM_AS_NO_SERVICE;
  rr_plmn_select_cnf.band_indicator = SYS_BAND_CLASS_NONE;
  rr_plmn_select_cnf.ue_oos         = as_oos;

  /* set the LAI to INVALID values, which should be ignored by MM */
  rr_plmn_select_cnf.location_area_identification.location_area_code[0] = INVALID_LA_BYTE;
  rr_plmn_select_cnf.location_area_identification.location_area_code[1] = INVALID_LA_BYTE;
  rr_plmn_select_cnf.location_area_identification.PLMN_id.identity[0]   = INVALID_PLMN_ID;
  rr_plmn_select_cnf.location_area_identification.PLMN_id.identity[1]   = INVALID_PLMN_ID;
  rr_plmn_select_cnf.location_area_identification.PLMN_id.identity[2]   = INVALID_PLMN_ID;

  /* use default values */
  rr_plmn_select_cnf.gprs_sys_info_present    = FALSE;
  rr_plmn_select_cnf.ATT                      = 0;
  rr_plmn_select_cnf.cell_access              = MM_AS_ACCESS_NO_CALLS;
  rr_plmn_select_cnf.MSCR                     = 0;
  rr_plmn_select_cnf.reestablishment_allowed  = FALSE;
  rr_plmn_select_cnf.service_capability       = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
  rr_plmn_select_cnf.T3212_timeout_value      = 0;
  rr_plmn_select_cnf.band_pref                = SYS_BAND_MASK_EMPTY;
  rr_plmn_select_cnf.gmm_suspended            = FALSE;
  rr_plmn_select_cnf.failure_cause            = dsds_cause;
  rr_plmn_select_cnf.arfcn.num                = INVALID_ARFCN;
  rr_plmn_select_cnf.arfcn.band               = SYS_BAND_NONE;
  rr_plmn_select_cnf.bsic                     = 0xFF;

  rr_send_message_to_nas((IMH_T *)&rr_plmn_select_cnf);

  return;
} /* rr_ms_send_rr_plmn_select_cnf */

/**
 * Send the Multi-SIM version of the RR_PLMN_LIST_CNF to MM.
 * For now, the message always contains an empty list.
 */
void rr_ms_send_rr_plmn_list_cnf(
  const rr_plmn_list_req_T *req_ptr,
  const mm_as_status_T status
)
{
  rr_plmn_list_cnf_T rr_plmn_list_cnf;

  memset(&rr_plmn_list_cnf, 0, sizeof(rr_plmn_list_cnf_T));

  rr_plmn_list_cnf.message_header.message_set = MS_MM_RR;
  rr_plmn_list_cnf.message_header.message_id  = (byte)RR_PLMN_LIST_CNF;

  RR_SET_MESSAGE_LENGTH(rr_plmn_list_cnf);

  if (req_ptr != NULL)
  {
    rr_plmn_list_cnf.status                               = status;
    rr_plmn_list_cnf.available_plmn_list.plmn_list.length = 0;
    rr_plmn_list_cnf.failure_cause                        = MM_RR_NO_RESOURCE_AVAILABLE;
    rr_plmn_list_cnf.trans_id                             = req_ptr->trans_id;

#ifdef FEATURE_WCDMA
    rr_plmn_list_cnf.net_selection_mode         = req_ptr->network_selection_mode;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
    rr_plmn_list_cnf.service_search             = req_ptr->service_search;
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

    rr_send_message_to_nas((IMH_T *)&rr_plmn_list_cnf);
  }
  else
  {
    MSG_GERAN_ERROR_0("Unable to respond to RR_PLMN_LIST_REQ");
  }

  return;
} /* rr_ms_send_rr_plmn_list_cnf */

/*!
 * \brief Sends RR_MULTIMODE_SUBS_CHGD_CNF to MM. The message has no content.
 */
void rr_mm_send_multimode_subs_chgd_cnf(
  void
)
{
  rr_multimode_subs_chgd_cnf_T rr_multimode_subs_chgd_cnf;

  RR_ZERO_STRUCTURE(rr_multimode_subs_chgd_cnf);

  rr_multimode_subs_chgd_cnf.message_header.message_set = MS_MM_RR;
  rr_multimode_subs_chgd_cnf.message_header.message_id  = (byte)RR_MULTIMODE_SUBS_CHGD_CNF;

  RR_SET_MESSAGE_LENGTH(rr_multimode_subs_chgd_cnf);

  rr_send_message_to_nas((IMH_T *)&rr_multimode_subs_chgd_cnf);
}

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */


void rr_send_srvcc_handover_failure_ind( )
{
  rr_srvcc_handover_failure_ind_T rr_srvcc_handover_failure_ind;

  RR_PREPARE_MM_MESSAGE(rr_srvcc_handover_failure_ind, RR_SRVCC_HANDOVER_FAILURE_IND);

  rr_send_message_to_nas(&rr_srvcc_handover_failure_ind.message_header);
}

#ifdef FEATURE_GERAN_BSR_PS_STATUS
/*!
 * \brief Sends RR_PS_STATUS_CHANGE_IND to NAS-MM.
 * 
 * \param ps_is_active (in)
 */
void rr_mm_send_rr_ps_status_change_ind(
  boolean ps_is_active
   
)
{
  rr_ps_status_change_ind_T rr_ps_status_change_ind;

  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);

  RR_PREPARE_MM_MESSAGE(rr_ps_status_change_ind, RR_PS_STATUS_CHANGE_IND); 

  // Check if the value of 'ps_is_active' is different to the last indication sent
  if (rr_mm_send_data_ptr->ps_is_active != ps_is_active) 
  {
    MSG_GERAN_HIGH_1_G("Updating PS status to NAS (%d)",ps_is_active);

    rr_ps_status_change_ind.ps_is_active = ps_is_active;

    rr_send_message_to_nas((IMH_T *)&rr_ps_status_change_ind);

    // Update the local copy
    rr_mm_send_data_ptr->ps_is_active = ps_is_active;
  }
} /* end rr_mm_send_rr_ps_status_change_ind() */ 
#endif // FEATURE_GERAN_BSR_PS_STATUS

#if defined(FEATURE_GERAN_IMPLICIT_PS_REJECT) || defined(FEATURE_GERAN_EAB)

/**
 * Send the RR_PS_IMPLICIT_REJECT_IND primitive to MM
 *
 * @param uint32 wait_time

 * @param GAS ID
 */
void rr_send_ps_implicit_reject_ind(
  uint32  wait_time
   
)
{
  rr_ps_implicit_reject_ind_T rr_ps_implicit_reject_ind;

  RR_PREPARE_MM_MESSAGE(rr_ps_implicit_reject_ind, RR_PS_IMPLICIT_REJECT_IND);

  rr_ps_implicit_reject_ind.wait_time = wait_time;

#ifndef PERLUTF
  rr_send_message_to_nas((IMH_T *)&rr_ps_implicit_reject_ind);
#endif
  return;
}
#endif /* FEATURE_GERAN_IMPLICIT_PS_REJECT || FEATURE_GERAN_EAB */
/*!
 * \brief Gets the last camped cell ARFCN.
 * 
 * \param 
 */

ARFCN_T rr_get_previous_camped_BCCH_ARFCN ( void )
{  
  // Obtain a pointer to the module data
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_mm_send_data_ptr);
  return rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN;
}
/**
  @brief Initialisation function. Should be called once at RR task start

  Note: The primary purpose of this function is to initialise pointers to control/data structures.
*/
void rr_mm_send_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(&rr_mm_send_data, 0, sizeof(rr_mm_send_data));

  // Initialise any special values
  rr_mm_send_data.service_state = MM_AS_NO_SERVICE;
  rr_mm_send_data.service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
}

void rr_mm_send_update_old_scell(
  ARFCN_T BCCH_ARFCN,
  BSIC_T  BSIC,
  LAI_T   LAI)
{
  rr_mm_send_data_t *rr_mm_send_data_ptr = rr_mm_send_get_data_ptr();
  rr_mm_send_data_ptr->rr_previous_BCCH_ARFCN = BCCH_ARFCN;
  rr_mm_send_data_ptr->rr_previous_BSIC       = BSIC;
  rr_mm_send_data_ptr->rr_previous_LAI        = LAI;
}

/* EOF */
