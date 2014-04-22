/*!
  @file geran_grr_static_api.c

  @brief
  External static API implementation for RRC task.

  @detail

*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/static/geran_grr_static_api.c#13 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
16/05/16   sc      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "geran_grr_static_api.h"

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "geran_dmm.h"
#endif

#include "geran_grr_api.h"
#include "rr_static_main.h"
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_GSM_TDS)
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/
sys_sband_e_type geran_grr_get_arfcn_band(
  uint16                 arfcn,
  geran_grr_band_ind_e   band_ind,
  boolean               *is_band_valid
)
{
  sys_sband_e_type      band = SYS_SBAND_MAX;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, band, geran_grr_get_arfcn_band, arfcn, band_ind, is_band_valid);
#else
  band = geran_grr_get_arfcn_band_dyn(arfcn, band_ind, is_band_valid);
#endif

  return band;
}

ARFCN_T geran_grr_convert_to_arfcn_type(uint16 arfcn, geran_grr_band_ind_e band_ind)
{
  ARFCN_T output_arfcn = {INVALID_ARFCN, SYS_BAND_NONE};
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, output_arfcn, geran_grr_convert_to_arfcn_type, arfcn, band_ind);
#else
  output_arfcn = geran_grr_convert_to_arfcn_type_dyn(arfcn, band_ind);
#endif

  return output_arfcn;
}

/* TODO: Remove calls to this from gsclassmark.c */
void mb_initialise(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, mb_initialise);
#else
  mb_initialise_dyn();
#endif

  return;
} /* end mb_initialise() */

/* This function replaces the macro ARFCNS_EQUAL for GERAN clients as
   the macro includes conditional compilation */
boolean arfcns_equal(ARFCN_T arfcn1, ARFCN_T arfcn2)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, arfcns_equal, arfcn1, arfcn2);
#else
  result = arfcns_equal_dyn(arfcn1, arfcn2);
#endif

  return result;
}

/*!
 * \brief Starts capture of power-scan data into the provided buffer. Note: Called by CM.
 *
 * \param memptr - Pointer to a buffer to place the results
 * \param memsize - Size (bytes) of the buffer
 *
 * \return boolean - TRUE if successful; FALSE otherwise
 */
boolean geran_start_capture_scan_data(uint32 *memptr, uint16 memsize)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_start_capture_scan_data,  memptr, memsize);
#else
  result = geran_start_capture_scan_data_dyn(memptr, memsize);
#endif

  return result;
}

/*!
 * \brief Stops capture of power-scan data. Note: Called by CM.
 *
 * \return boolean - TRUE if successful; FALSE otherwise
 */
boolean geran_stop_capture_scan_data(void)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_stop_capture_scan_data);
#else
  result = geran_stop_capture_scan_data_dyn();
#endif

  return result;
}

boolean geran_get_nv_recovery_restart_enabled(void)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_get_nv_recovery_restart_enabled);
#else
  result = geran_get_nv_recovery_restart_enabled_dyn();
#endif

  return result;
}

void geran_initiate_recovery_restart(const geran_client_enum_t client_id)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_initiate_recovery_restart, client_id);
#else
  geran_initiate_recovery_restart_dyn(client_id);
#endif

  return;
}

/*!
 * \brief Returns the last timing advance paramters.
 *  
 * Note: This is a public API and so is guarded by a mutex lock.
 * 
 * \param p_TimingAdvance (out)
 */
void geran_grr_get_current_timing_advance_info_per_subs(geran_grr_current_timing_advance_type *p_TimingAdvance)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_get_current_timing_advance_info_per_subs, p_TimingAdvance);
#else
  geran_grr_get_current_timing_advance_info_per_subs_dyn(p_TimingAdvance);
#endif

  return;
}

/*!
 * \brief Returns the last timing advance paramters.
 *  
 * Note: This is a public API and so is guarded by a mutex lock.
 * 
 * \param p_TimingAdvance (out)
 */
void geran_grr_get_current_timing_advance_info(geran_grr_current_timing_advance_type *p_TimingAdvance)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_get_current_timing_advance_info, p_TimingAdvance);
#else
  geran_grr_get_current_timing_advance_info_dyn(p_TimingAdvance);
#endif

  return;
}

/**
  @brief Turn DTM override on.

  This public function is called from NAS at certain times when a call
  is established to avoid a problem where noise was heard on the audio
  which was found to be related to DTX operation.

  This function allows NAS to override the DTX setting and force it
  OFF during certain signalling procedures.

  rr_dtx_override_off() then allows DTX to be turned on again.
**/
void geran_grr_dtx_override_on_per_subs(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_dtx_override_on_per_subs);
#else
  geran_grr_dtx_override_on_per_subs_dyn();
#endif

  return;
}

void geran_grr_dtx_override_on(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_dtx_override_on);
#else
  geran_grr_dtx_override_on_dyn();
#endif

}

void rr_dtx_override_on(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_dtx_override_on);
#else
  rr_dtx_override_on_dyn();
#endif

  return;
}

void geran_grr_dtx_override_off_per_subs(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_dtx_override_off_per_subs);
#else
  geran_grr_dtx_override_off_per_subs_dyn();
#endif

  return;
}

void geran_grr_dtx_override_off(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_dtx_override_off);
#else
  geran_grr_dtx_override_off_dyn();
#endif

  return;
}

void rr_dtx_override_off(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_dtx_override_off);
#else
  rr_dtx_override_off_dyn();
#endif

  return;
}

/*!
 * \brief Gets the current camped on status, cell identity, LAI, BSIC and ARFCN. 
 *  
 * Note: This function is called from outside RR's context by the LSM task.
 * 
 * \param p_BsInfo (out)
 */
void geran_grr_get_current_bs_info_per_subs(sys_modem_as_id_e_type as_id,geran_grr_current_bs_info_type *info_ptr)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_get_current_bs_info_per_subs, info_ptr);
#else
  geran_grr_get_current_bs_info_per_subs_dyn( info_ptr);
#endif

  return;
}

/*!
 * \brief Gets the current camped on status, cell identity, LAI, BSIC and ARFCN. 
 *  
 * Note: This function is called from outside RR's context by the LSM task.
 * 
 * \param p_BsInfo (out)
 */
void geran_grr_get_current_bs_info(geran_grr_current_bs_info_type *p_BsInfo)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_get_current_bs_info, p_BsInfo);
#else
  geran_grr_get_current_bs_info_dyn(p_BsInfo);
#endif

  return;
}

/**
   @brief Access fn for ghdi_mvs_cb_func() / ghdi_reset_sr_and_inform_AS()

   @param speech_enc_samp_freq  8000: narrow-band or 16000: wide-band, 0 - unknown: ignore
   @param call_est;             new call established, ignore speech_enc_samp_freq
   @return TRUE if the sample rate was written

*/
boolean geran_grr_set_codec_ho_sample_rate_per_subs(uint32 speech_enc_samp_freq, boolean call_est)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_grr_set_codec_ho_sample_rate_per_subs, speech_enc_samp_freq, call_est);
#else
  result = geran_grr_set_codec_ho_sample_rate_per_subs_dyn(speech_enc_samp_freq, call_est);
#endif

  return result;
}

boolean geran_grr_set_codec_ho_sample_rate(uint32 speech_enc_samp_freq, boolean call_est)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_grr_set_codec_ho_sample_rate, speech_enc_samp_freq, call_est);
#else
  result = geran_grr_set_codec_ho_sample_rate_dyn(speech_enc_samp_freq, call_est);
#endif

  return result;
}

/**
   @brief Access fn for cfa_send_event_if_neccessary()

   @param message_id  RR/L2 message ID
   @param msg_ptr pointer to RR/L2 message

   @return TRUE if message_ID is an OTA message
   @return fpos contains FPOS of layer3_message[0] in the message
   @return isTx is TRUE if message_id is a req, FALSE if it's an ind
*/
boolean geran_grr_get_l3_msg_FPOS(const byte message_id, const byte *msg_ptr, uint32 *fpos, boolean *isTx)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_grr_get_l3_msg_FPOS, message_id, msg_ptr, fpos, isTx);
#else
  result = geran_grr_get_l3_msg_FPOS_dyn(message_id, msg_ptr, fpos, isTx);
#endif


  return result;
}

/*!
 * \brief This function is called by CGPS to register a callback which will be used by RR to provide events 
 *        and/or data back to CGPS 
 * 
 * \param cb_fp (in)
 */
void geran_grr_register_cgps_event_cb(rr_cgps_event_cb_type cb_fp)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_grr_register_cgps_event_cb, cb_fp);
#else
  geran_grr_register_cgps_event_cb_dyn(cb_fp);
#endif

  return;
}

void rr_compute_BSIC_from_SCH_block(BSIC_T *BSIC_result_ptr, byte *SCH_block)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_compute_BSIC_from_SCH_block, BSIC_result_ptr, SCH_block);
#else
  rr_compute_BSIC_from_SCH_block_dyn(BSIC_result_ptr, SCH_block);
#endif

}

/*!
 * \brief This function is used to access parameters in the Public Store.
 *
 * \param param_id (in) - parameter identifier
 * \param param_copy (out) - updated with the value of the parameter
 */
void rr_read_public_store(rr_public_store_id_t param_id, void *param_copy)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_read_public_store, param_id, param_copy);
#else
  rr_read_public_store_dyn(param_id, param_copy);
#endif

} /* end rr_read_public_store */

/*!
 * \brief This function is used to access the Mobile Allocation frequency lists.
 *
 * \param ma_number (in) - MA_NUMBER_14 or MA_NUMBER_15
 *
 * \return rrps_frequency_list_t*
 */
rrps_frequency_list_t *rr_read_public_store_freq_list(uint8 ma_number)
{
  rrps_frequency_list_t  *ret_ptr = NULL;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, ret_ptr, rr_read_public_store_freq_list, ma_number);
#else
  ret_ptr = rr_read_public_store_freq_list_dyn(ma_number);
#endif

  return(ret_ptr);
} /* end rr_read_public_store_freq_list */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief Called from GCOMMON to post GERAN_NV_REFRESH_IND to GRR queue.
 *  
 */
void rr_nv_send_nv_refresh_ind(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_nv_send_nv_refresh_ind);
#else
  rr_nv_send_nv_refresh_ind_dyn();
#endif

  return;
}
#endif

/*!
 * \brief Indicates if eng mode debug is enabled.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_eng_mode_debug_is_enabled(void)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_nv_eng_mode_debug_is_enabled);
#else
  result = rr_nv_eng_mode_debug_is_enabled_dyn();
#endif

  return result;
}

rrps_frequency_list_t *rr_packet_assignment_mobile_alloc(
  encoding_type_e type,
  void *data,
  boolean *compare_ptr
)
{
  rrps_frequency_list_t  *ret_ptr = NULL;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, ret_ptr, rr_packet_assignment_mobile_alloc, type, data, compare_ptr);
#else
  ret_ptr = rr_packet_assignment_mobile_alloc_dyn(type, data, compare_ptr);
#endif

  return(ret_ptr);
}

/**
 * determines if the given band is supported or not. May be called from
 * multiple tasks
 *
 * @param band   band
 *
 * @return TRUE if the band is allowed, FALSE otherwise
 */
boolean rr_gapi_is_supported_band(sys_band_T band)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_gapi_is_supported_band, band);
#else
  result = rr_gapi_is_supported_band_dyn(band);
#endif

  return result;
}

boolean rr_is_supported_band(rr_internal_band_T band)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s,result, rr_is_supported_band, band);
#else
  result = rr_is_supported_band_dyn(band);
#endif

  return result;
}

boolean rr_get_nmr_bcch(uint8 *nmr, rr_bcch_list_type *bcch_list)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_nmr_bcch, nmr, bcch_list);
#else
  result = rr_get_nmr_bcch_dyn(nmr, bcch_list);
#endif

  return result;
}

boolean rr_api_get_nmr_bcch(uint8 *nmr, uint8 len_of_nmr,  rr_bcch_list_type *bcch_list)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_api_get_nmr_bcch, nmr, len_of_nmr, bcch_list);
#else
  result = rr_api_get_nmr_bcch_dyn(nmr, len_of_nmr, bcch_list);
#endif

  return result;
}

/**
 * Externalized function used by usat to get cell identity
 *
 * @param cell_id - pointer where cell id is returned
 */
boolean rr_get_cell_id(uint8 *cell_id)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_cell_id, cell_id);
#else
  result = rr_get_cell_id_dyn(cell_id);
#endif

  return result;
}

/**
 * Externalized function used by usat to get timing advance
 * for GSM cells
 *
 * @param me_status - pointer where mobile status is returned
 * @param timing_advance - pointer where timing advance returned
 */
boolean rr_get_timing_advance(uint8 *me_status, uint8* timing_advance)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_timing_advance, me_status, timing_advance);
#else
  result = rr_get_timing_advance_dyn(me_status, timing_advance);
#endif

  return result;
}

/**
 * Function: rr_get_band
 *
 * @param local_arfcn  channel number (0..1023)
 *
 * @return band associated with the given channel or
 *         BAND_NONE if the ARFCN is unknown or not supported
 */
rr_internal_band_T rr_get_band(word local_arfcn)
{
  rr_internal_band_T result = BAND_NONE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_band, local_arfcn);
#else
  result = rr_get_band_dyn(local_arfcn);
#endif

  return result;
}
rr_internal_band_T rr_get_current_band(void)
{
  rr_internal_band_T result = BAND_NONE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_current_band);
#else
  result = rr_get_current_band_dyn();
#endif

  return result;
}

byte rr_get_current_rf_power_class(void)
{
  byte result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_current_rf_power_class);
#else
  result = rr_get_current_rf_power_class_dyn();
#endif

  return result;
}

byte rr_gapi_get_band_rf_power_class(rr_internal_band_T band)
{
  byte result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_gapi_get_band_rf_power_class, band);
#else
  result = rr_gapi_get_band_rf_power_class_dyn(band);
#endif

  return result;
}

/**
 * May be called by multiple tasks
 *
 * @param band   band for which the current RF power class is desired
 *
 * @return current power class associated with the given band
 */
byte rr_get_band_rf_power_class(rr_internal_band_T band)
{
  byte result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_band_rf_power_class, band);
#else
  result = rr_get_band_rf_power_class_dyn(band);
#endif

  return result;
}

/**
 * May be called from multiple tasks.
 *
 * @return number of bands currently supported
 */
byte rr_get_supported_band_count(void)
{
  byte result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_supported_band_count);
#else
  result = rr_get_supported_band_count_dyn();
#endif

  return result;
}

sys_band_class_e_type rr_get_band_indicator_for_nas(boolean in_service)
{
  sys_band_class_e_type result = SYS_BAND_CLASS_NONE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_band_indicator_for_nas, in_service);
#else
  result = rr_get_band_indicator_for_nas_dyn(in_service);
#endif

  return result;
}

/**
 * Gets the maximum power output of the mobile for a given frequency
 * Note: Called from WCDMA (WL1)
 *
 * @frequency - frequency required
 *
 * @return Max output power in dBm
 */
byte rr_get_maximum_power_output_for_frequency(ARFCN_T frequency)
{
  byte result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_maximum_power_output_for_frequency, frequency);
#else
  result = rr_get_maximum_power_output_for_frequency_dyn(frequency);
#endif

  return result;
}

uint16 rr_dtm_get_max_LAPDm_octets_per_subs( void )
{
  uint16 result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_dtm_get_max_LAPDm_octets_per_subs);
#else
  result = rr_dtm_get_max_LAPDm_octets_per_subs_dyn();
#endif

  return result;
} /* end rr_dtm_get_max_LAPDm_octets_per_subs */

/*!
 * \brief Returns the number of octets that can be transferred via GTTP. 
 *  
 * If not currently in a CS connection on a DTM-capable cell, then zero will be returned. 
 * Note: This function is called from outside GERAN* 
 * 
 * \return uint16 - number of octets
 */
uint16 rr_dtm_get_max_LAPDm_octets(void)
{
  uint16 result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_dtm_get_max_LAPDm_octets);
#else
  result = rr_dtm_get_max_LAPDm_octets_dyn();
#endif

  return result;
}

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
/*===========================================================================

FUNCTION RR_REGISTER_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layers to register a callback function if notification
of the serving cell id and/or neighbour cell is required.  Once the function is registered,
it will be called whenever there is a serving/neighbour cell change.


PARAMETERS

  cb_ptr - pointer to function that requires registration

RETURN VALUE

  BOOLEAN:  TRUE signifies that registration has succeeded.  FALSE signifies
            that registration failed.

SIDE EFFECTS

  Read/write rr_cell_change_ind_cb_func_ptr_ptr

===========================================================================*/

boolean rr_register_cell_change_ind_cb(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_register_cell_change_ind_cb, cb_ptr);
#else
  result = rr_register_cell_change_ind_cb_dyn(cb_ptr);
#endif

  return result;
}

/*===========================================================================

FUNCTION RR_DEREGISTER_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layer to deregister the callback function that was
registered for notification of serving/neighbour cell id.


PARAMETERS

  cb_ptr - pointer to function that requires deregistration

RETURN VALUE

  BOOLEAN:  TRUE signifies that deregistration has succeeded.  FALSE signifies
            that deregistration failed.

SIDE EFFECTS

  read/write rr_cell_change_ind_cb_func_ptr_ptr

===========================================================================*/

boolean rr_deregister_cell_change_ind_cb(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result,rr_deregister_cell_change_ind_cb,  cb_ptr);
#else
  result = rr_deregister_cell_change_ind_cb_dyn(cb_ptr);
#endif

  return result;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
boolean rr_ds_register_cell_change_ind_cb(const sys_modem_as_id_e_type as_id, RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  boolean result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_register_cell_change_ind_cb,  as_id, cb_ptr);
#else
  result = rr_ds_register_cell_change_ind_cb_dyn(as_id, cb_ptr);
#endif

  return result;
}

boolean rr_ds_deregister_cell_change_ind_cb(const sys_modem_as_id_e_type as_id, RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr)
{
  boolean result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_deregister_cell_change_ind_cb,  as_id, cb_ptr);
#else
  result = rr_ds_deregister_cell_change_ind_cb_dyn(as_id, cb_ptr);
#endif

  return result;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */


/**
 * May be called from multiple tasks.
 *
 * @return number of bands currently supported
 */
byte rr_ds_get_supported_band_count( void )
{
  byte   result = 0;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_get_supported_band_count);
#else
  result = rr_ds_get_supported_band_count_dyn();
#endif

  return result;
}

sys_band_class_e_type rr_ds_get_band_indicator_for_nas(boolean in_service)
{
  sys_band_class_e_type  result = SYS_BAND_CLASS_NONE;
 
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_get_band_indicator_for_nas,  in_service);
#else
  result = rr_ds_get_band_indicator_for_nas_dyn(in_service );
#endif

  return result;
}

/**
 * Externalized function used by usat to get measurement results and ba list
 * for GSM cells. When called in dedicated, the measurement results are from
 * the last meas results IE. If not in dedicated a meas results is generated
 * based on the last idle mode measurement.
 *
 * @param nmr - ptr where last measurement results should be copied to.
 *   Uses the same format as the measurement results IE sent in dedicated
 *   meas reports.
 * @param bcch_list - ba list
 */
boolean rr_ds_get_nmr_bcch(uint8 *nmr, rr_bcch_list_type *bcch_list)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_get_nmr_bcch,  nmr, bcch_list);
#else
  result = rr_ds_get_nmr_bcch_dyn(nmr, bcch_list);
#endif

  return result;
}

boolean rr_ds_api_get_nmr_bcch(uint8 *nmr, uint8 len_of_nmr, rr_bcch_list_type *bcch_list, sys_modem_as_id_e_type as_id)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_api_get_nmr_bcch,  nmr, len_of_nmr, bcch_list);
#else
  result = rr_ds_api_get_nmr_bcch_dyn(nmr, len_of_nmr, bcch_list);
#endif

  return result;
}

/**
 * Externalized function used by usat to get cell identity,
 *
 * @param cell_id - pointer where cell id is returned.
 */
boolean rr_ds_get_cell_id(uint8 *cell_id, sys_modem_as_id_e_type as_id)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_get_cell_id,  cell_id);
#else
  result = rr_ds_get_cell_id_dyn(cell_id);
#endif

  return result;
}

/**
 * Externalized function used by usat to get timing advance
 * for GSM cells, referenced by access stratum id.
 *
 * @param me_status - pointer where mobile status is returned.
 * @param timing_advance - pointer where timing advance
 *                         returned.
 */
boolean rr_ds_get_timing_advance(uint8 *me_status, uint8 *timing_advance, sys_modem_as_id_e_type as_id)
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_ds_get_timing_advance, me_status, timing_advance);
#else
  result = rr_ds_get_timing_advance_dyn(me_status, timing_advance);
#endif

  return result;
}

#ifdef FEATURE_WCDMA
void rr_send_rrc_message_to_rr(void *message_ptr, rrc_rr_cmd_e_type rrc_cmd_id, word length)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_send_rrc_message_to_rr, message_ptr, rrc_cmd_id, length);
#else
  rr_send_rrc_message_to_rr_dyn(message_ptr, rrc_cmd_id, length);
#endif

  return;
}

PLMN_id_T rrc_get_rr_curr_camped_plmn_sub_id(void)
{
  PLMN_id_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rrc_get_rr_curr_camped_plmn_sub_id);
#else
  result = rrc_get_rr_curr_camped_plmn_sub_id_dyn();
#endif

  return result;
}

/* wrapper function for legacy codebases */
PLMN_id_T rrc_get_rr_curr_camped_plmn(void)
{
  PLMN_id_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rrc_get_rr_curr_camped_plmn);
#else
  result = rrc_get_rr_curr_camped_plmn_dyn();
#endif

  return result;
}

void rr_rrc_acq_db_save_sub_id(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_rrc_acq_db_save_sub_id);
#else
  rr_rrc_acq_db_save_sub_id_dyn();
#endif

  return;
}

/* wrapper function for legacy codebases */
void rr_rrc_acq_db_save(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_rrc_acq_db_save);
#else
  rr_rrc_acq_db_save_dyn();
#endif

  return;
}

/* wrapper function for legacy codebases */
sys_band_T rr_rrc_get_arfcn_band(uint16 arfcn, rrc_rr_frequence_band_e_type band_ind)
{
  sys_band_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_rrc_get_arfcn_band, arfcn, band_ind);
#else
  result = rr_rrc_get_arfcn_band_dyn(arfcn, band_ind);
#endif

  return result;
}

sys_band_T rr_rrc_get_arfcn_band_sub_id(uint16 arfcn, rrc_rr_frequence_band_e_type band_ind)
{
  sys_band_T           result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_rrc_get_arfcn_band_sub_id,  arfcn, band_ind);
#else
  result = rr_rrc_get_arfcn_band_sub_id_dyn(arfcn, band_ind);
#endif

  return result;
}

/* wrapper function for legacy codebases */
void rr_rrc_set_band_pref(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_rrc_set_band_pref, band_pref);
#else
  rr_rrc_set_band_pref_dyn(band_pref);
#endif

  return;
}

void rr_rrc_set_band_pref_sub_id(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_rrc_set_band_pref_sub_id, band_pref);
#else
  rr_rrc_set_band_pref_sub_id_dyn(band_pref);
#endif

  return;
}

void rr_rrc_set_bst_band_pref(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_rrc_set_bst_band_pref, band_pref);
#else
  rr_rrc_set_bst_band_pref_dyn(band_pref);
#endif

  return;
}

void rr_rrc_set_bst_band_pref_sub_id(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_rrc_set_bst_band_pref_sub_id, band_pref);
#else
  rr_rrc_set_bst_band_pref_sub_id_dyn(band_pref);
#endif

  return;
}
#endif /*FEATURE_WCDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
/*!
 * \brief Multi-SIM API to obtain partial PLMN search results.
 *
 * \param available_plmn_list_ptr (out)
 *
 * \return boolean - TRUE if results valid, FALSE otherwise
 */
boolean rr_get_inter_rat_bplmn_prtl_result_sub_id(sys_detailed_plmn_list_s_type *available_plmn_list_ptr)
{
  boolean result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_inter_rat_bplmn_prtl_result_sub_id, available_plmn_list_ptr);
#else
  result = rr_get_inter_rat_bplmn_prtl_result_sub_id_dyn(available_plmn_list_ptr);
#endif

  return result;
}

/*!
 * \brief API to obtain partial PLMN search results.
 *
 * \param available_plmn_list_ptr (out)
 *
 * \return boolean - TRUE if results valid, FALSE otherwise
 */
boolean rr_get_inter_rat_bplmn_prtl_result(sys_detailed_plmn_list_s_type *available_plmn_list_ptr)
{
  boolean result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_get_inter_rat_bplmn_prtl_result, available_plmn_list_ptr);
#else
  result = rr_get_inter_rat_bplmn_prtl_result_dyn(available_plmn_list_ptr);
#endif

  return result;
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

void rr_init_hz_info(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_init_hz_info);
#else
  rr_init_hz_info_dyn();
#endif

  return;
}

/**
 * Registers the callback function for Home Zone info
 */
#ifndef PERLUTF
boolean rr_hz_register_state_change_cb(rr_hz_state_cb_fn_T new_fn_ptr)
#else
#error code not present
#endif
{
  boolean result = FALSE;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, rr_hz_register_state_change_cb, new_fn_ptr);
#else
  result = rr_hz_register_state_change_cb_dyn(new_fn_ptr);
#endif

  return result;
}

/**
 * De-registers the callback function for Home Zone info
 */
void rr_hz_deregister_state_change_cb(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, rr_hz_deregister_state_change_cb);
#else
  rr_hz_deregister_state_change_cb_dyn();
#endif

  return;
}

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_GSM_TDS)
void tdsrr_send_rrc_message_to_rr(void *message_ptr, tdsrr_rrc_cmd_e_type rrc_cmd_id, word length)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_send_rrc_message_to_rr, message_ptr, rrc_cmd_id, length);
#else
  tdsrr_send_rrc_message_to_rr_dyn(message_ptr, rrc_cmd_id, length);
#endif

  return;
}

sys_band_T tdsrr_get_arfcn_band_sub_id(uint16 arfcn, tdsrr_frequence_band_e_type band_ind)
{
  sys_band_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, tdsrr_get_arfcn_band_sub_id, arfcn, band_ind);
#else
  result = tdsrr_get_arfcn_band_sub_id_dyn(arfcn, band_ind);
#endif

  return result;
}

sys_band_T tdsrr_get_arfcn_band(uint16 arfcn, tdsrr_frequence_band_e_type band_ind)
{
  sys_band_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, tdsrr_get_arfcn_band, arfcn, band_ind);
#else
  result = tdsrr_get_arfcn_band_dyn(arfcn, band_ind);
#endif

  return result;
}

void tdsrr_set_band_pref_sub_id(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_set_band_pref_sub_id, band_pref);
#else
  tdsrr_set_band_pref_sub_id_dyn(band_pref);
#endif

  return;
}

void tdsrr_set_band_pref(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_set_band_pref, band_pref);
#else
  tdsrr_set_band_pref_dyn(band_pref);
#endif

  return;
}

void tdsrr_set_bst_band_pref_sub_id(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_set_bst_band_pref_sub_id, band_pref);
#else
  tdsrr_set_bst_band_pref_sub_id_dyn(band_pref);
#endif

  return;
}

void tdsrr_set_bst_band_pref(sys_band_mask_type band_pref)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_set_bst_band_pref, band_pref);
#else
  tdsrr_set_bst_band_pref_dyn(band_pref);
#endif

  return;
}

PLMN_id_T tdsrr_get_rr_curr_camped_plmn_sub_id(void)
{
  PLMN_id_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, tdsrr_get_rr_curr_camped_plmn_sub_id);
#else
  result = tdsrr_get_rr_curr_camped_plmn_sub_id_dyn();
#endif

  return result;
}

PLMN_id_T tdsrr_get_rr_curr_camped_plmn(void)
{
  PLMN_id_T result;
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, tdsrr_get_rr_curr_camped_plmn);
#else
  result = tdsrr_get_rr_curr_camped_plmn_dyn();
#endif

  return result;
}

void tdsrr_acq_db_save_sub_id(void)
{
  
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_acq_db_save_sub_id);
#else
  tdsrr_acq_db_save_sub_id_dyn();
#endif

  return;
}

void tdsrr_acq_db_save(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, tdsrr_acq_db_save);
#else
  tdsrr_acq_db_save_dyn();
#endif

  return;
}
#endif /*FEATURE_TDSCDMA || FEATURE_GSM_TDS*/

void geran_send_cm_test_mode_on(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_send_cm_test_mode_on);
#else
  geran_send_cm_test_mode_on_dyn();
#endif

  return;
}

void geran_send_cm_test_mode_off(void)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, geran_send_cm_test_mode_off);
#else
  geran_send_cm_test_mode_off_dyn();
#endif

  return;
}

#if !defined(PERLUTF) && defined(FEATURE_RR_THERMAL_MITIGATION)
void change_event_cb_fn(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, change_event_cb_fn, context, event_type, data, data_size);
#else
  change_event_cb_fn_dyn(context, event_type, data, data_size);
#endif

  return;
}

void initialise_event_cb_fn(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL_PRIV( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, initialise_event_cb_fn, context, event_type, data, data_size);
#else
  initialise_event_cb_fn_dyn(context, event_type, data, data_size);
#endif

  return;
}
#endif

void grr_message_dispatcher( rr_cmd_type *cmd_ptr)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
    DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, grr_message_dispatcher, cmd_ptr);
#else
    rr_message_dispatcher( cmd_ptr);
#endif
}

/*!
 * \brief This API Maps Engineering Mode Band Information to correct System Band Class.
 * 
 * \param gsm_band:  Engineering Mode Band Information (in)
 * 
 * \return sys_sband_e_type: System Band Class
 */
sys_sband_e_type geran_map_gsm_band_to_sys_band_class(eng_mode_band_t gsm_band)
{
  sys_sband_e_type result = SYS_SBAND_BC0_A;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_map_gsm_band_to_sys_band_class, gsm_band);
#else
  result = geran_map_gsm_band_to_sys_band_class_dyn(gsm_band);
#endif

  return result;
}

uint8 gprs_odm_info_get_rach_counter(void)
{
  uint8 result = 0;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, gprs_odm_info_get_rach_counter);
#else
  result = gprs_odm_info_get_rach_counter_dyn();
#endif

  return result;
}

uint8 gprs_odm_info_get_num_of_neighbour_cells(void)
{
  uint8 result = 0;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, gprs_odm_info_get_num_of_neighbour_cells);
#else
  result = gprs_odm_info_get_num_of_neighbour_cells_dyn();
#endif

  return result;
}

uint8 gprs_odm_info_get_paging_multiframe(void)
{
  uint8 result = 0;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, gprs_odm_info_get_paging_multiframe);
#else
  result = gprs_odm_info_get_paging_multiframe_dyn();
#endif

  return result;
}

int8 gprs_odm_info_get_rx_lev_dBm(void)
{
  int8 result = -111;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, gprs_odm_info_get_rx_lev_dBm);
#else
  result = gprs_odm_info_get_rx_lev_dBm_dyn();
#endif

  return result;
}

uint8 gprs_odm_info_get_rx_lev_status(void)
{
  uint8 result = 0;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, gprs_odm_info_get_rx_lev_status);
#else
  result = gprs_odm_info_get_rx_lev_status_dyn();
#endif

  return result;
}

/* Returns: 0 - PBCCH not present; 1 - PBCCH present, MS camped on BCCH; 2 - MS camped on PBCCH */
uint8 gprs_odm_info_get_pbcch_status(void)
{
  return 0;
}

gprs_odm_cell_type_t gprs_odm_info_get_cell_type(void)
{
  gprs_odm_cell_type_t result = ODM_CELL_TYPE_UNKNOWN;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, gprs_odm_info_get_cell_type);
#else
  result = gprs_odm_info_get_cell_type_dyn();
#endif

  return result;
}

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
void gprs_odm_get_plmn_lac_list(gprs_odm_plmn_lacs_t *plmn_lac_list)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, gprs_odm_get_plmn_lac_list, plmn_lac_list);
#else
  gprs_odm_get_plmn_lac_list_dyn(plmn_lac_list);
#endif

  return;
}

void gprs_odm_set_plmn_lac_list(
  const gprs_odm_plmn_lac_ranges_t *odm_lac_range_list,
  gprs_odm_lac_cat_t               range_cat
)
{
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_VOIDFUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, gprs_odm_set_plmn_lac_list, odm_lac_range_list, range_cat);
#else
  gprs_odm_set_plmn_lac_list_dyn(odm_lac_range_list, range_cat);
#endif

  return;
}
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
boolean geran_get_current_serving_cell_info(geran_current_scell_info_t *current_scell_info)
{
  boolean result = FALSE;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, result, geran_get_current_serving_cell_info, current_scell_info);
#else
  result = geran_get_current_serving_cell_info_dyn(current_scell_info);
#endif

  return result;
}
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */