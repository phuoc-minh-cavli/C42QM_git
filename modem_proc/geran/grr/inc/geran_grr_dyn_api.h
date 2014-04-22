#ifndef GERAN_GRR_DYN_API_H
#define GERAN_GRR_DYN_API_H

/*!
  @file geran_grr_dyn_api.h

  @brief
  External dynamic API implementation for RRC task.

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

$Header: //components/rel/geran.mpss/5.2.0/grr/inc/geran_grr_dyn_api.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
17/05/16   sc      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "sys.h"
#include "geran_grr_types.h"
#include "geran_dual_sim.h"
#include "grr_mutex.h"
#include "rr_gprs_defs.h"
#include "rr_cb_if.h"
#include "rr_lsm_if.h"
#include "rr_init.h"
#include "rr_public_store.h"
#include "rr_general.h"
#include "rr_multiband_g.h"
#include "ghdi_nvmem.h"
#include "rr_static_main.h"
#include "gprs_odm_info.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern sys_sband_e_type geran_grr_get_arfcn_band_dyn(uint16 arfcn, geran_grr_band_ind_e band_ind, boolean *is_band_valid);
extern ARFCN_T geran_grr_convert_to_arfcn_type_dyn(uint16 arfcn, geran_grr_band_ind_e band_ind);
extern void mb_initialise_dyn(void);
extern boolean arfcns_equal_dyn(ARFCN_T arfcn1, ARFCN_T arfcn2);
extern boolean geran_start_capture_scan_data_dyn(uint32 *memptr, uint16 memsize);
extern boolean geran_stop_capture_scan_data_dyn(void);
extern boolean geran_get_nv_recovery_restart_enabled_dyn(void);
extern void geran_initiate_recovery_restart_dyn(const geran_client_enum_t);
extern void geran_grr_get_current_timing_advance_info_per_subs_dyn(geran_grr_current_timing_advance_type *p_TimingAdvance);
extern void geran_grr_get_current_timing_advance_info_dyn(geran_grr_current_timing_advance_type *p_TimingAdvance);
extern void geran_grr_dtx_override_on_per_subs_dyn(void);
extern void geran_grr_dtx_override_on_dyn(void);
extern void rr_dtx_override_on_dyn(void);
extern void geran_grr_dtx_override_off_per_subs_dyn(void);
extern void geran_grr_dtx_override_off_dyn(void);
extern void rr_dtx_override_off_dyn(void);
extern void geran_grr_get_current_bs_info_per_subs_dyn(geran_grr_current_bs_info_type *info_ptr);
extern void geran_grr_get_current_bs_info_dyn(geran_grr_current_bs_info_type *p_BsInfo);
extern boolean geran_grr_set_codec_ho_sample_rate_per_subs_dyn(uint32 speech_enc_samp_freq, boolean call_est);
extern boolean geran_grr_set_codec_ho_sample_rate_dyn(uint32 speech_enc_samp_freq, boolean call_est);
extern boolean geran_grr_get_l3_msg_FPOS_dyn(const byte message_id, const byte *msg_ptr, uint32 *fpos, boolean *isTx);
extern void geran_grr_register_cgps_event_cb_dyn(rr_cgps_event_cb_type cb_fp);
extern void rr_compute_BSIC_from_SCH_block_dyn(BSIC_T *BSIC_result_ptr, byte *SCH_block);
extern void rr_read_public_store_dyn(rr_public_store_id_t param_id, void *param_copy);
extern rrps_frequency_list_t *rr_read_public_store_freq_list_dyn(uint8 ma_number);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
extern void rr_nv_send_nv_refresh_ind_dyn(void);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

extern boolean rr_nv_eng_mode_debug_is_enabled_dyn(void);
extern rrps_frequency_list_t *rr_packet_assignment_mobile_alloc_dyn(encoding_type_e type, void *data, boolean *compare_ptr);
extern boolean rr_gapi_is_supported_band_dyn(sys_band_T band);
extern boolean rr_is_supported_band_dyn(rr_internal_band_T band);
extern boolean rr_get_nmr_bcch_dyn(uint8 *nmr, rr_bcch_list_type *bcch_list);
extern boolean rr_api_get_nmr_bcch_dyn(uint8 *nmr, uint8 len_of_nmr,  rr_bcch_list_type *bcch_list);
extern boolean rr_get_cell_id_dyn(uint8 *cell_id);
extern boolean rr_get_timing_advance_dyn(uint8 *me_status, uint8 *timing_advance);
extern rr_internal_band_T rr_get_band_dyn(word local_arfcn);
extern rr_internal_band_T rr_get_current_band_dyn(void);
extern byte rr_get_current_rf_power_class_dyn(void);
extern byte rr_gapi_get_band_rf_power_class_dyn(rr_internal_band_T band);
extern byte rr_get_band_rf_power_class_dyn(rr_internal_band_T band);
extern byte rr_get_supported_band_count_dyn(void);
extern sys_band_class_e_type rr_get_band_indicator_for_nas_dyn(boolean in_service);
extern byte rr_get_maximum_power_output_for_frequency_dyn(ARFCN_T frequency);
extern uint16 rr_dtm_get_max_LAPDm_octets_per_subs_dyn(void);
extern uint16 rr_dtm_get_max_LAPDm_octets_dyn(void);

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
extern boolean rr_register_cell_change_ind_cb_dyn(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr);
extern boolean rr_deregister_cell_change_ind_cb_dyn(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
extern boolean rr_ds_register_cell_change_ind_cb_dyn(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr);
extern boolean rr_ds_deregister_cell_change_ind_cb_dyn(RR_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr);
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

extern byte rr_ds_get_supported_band_count_dyn(void);
extern sys_band_class_e_type rr_ds_get_band_indicator_for_nas_dyn(boolean in_service);
extern boolean rr_ds_get_nmr_bcch_dyn(uint8 *nmr, rr_bcch_list_type *bcch_list);
extern boolean rr_ds_api_get_nmr_bcch_dyn(uint8 *nmr, uint8 len_of_nmr, rr_bcch_list_type *bcch_list);
extern boolean rr_ds_get_cell_id_dyn(uint8 *cell_id);
extern boolean rr_ds_get_timing_advance_dyn(uint8 *me_status, uint8 *timing_advance);

#ifdef FEATURE_WCDMA
extern void rr_send_rrc_message_to_rr_dyn(void *message_ptr, rrc_rr_cmd_e_type rrc_cmd_id, word length);
extern PLMN_id_T rrc_get_rr_curr_camped_plmn_sub_id_dyn(void);
extern PLMN_id_T rrc_get_rr_curr_camped_plmn_dyn(void);
extern void rr_rrc_acq_db_save_dyn(void);
extern void rr_rrc_acq_db_save_sub_id_dyn(void);
extern sys_band_T rr_rrc_get_arfcn_band_dyn(uint16 arfcn, rrc_rr_frequence_band_e_type band_ind);
extern sys_band_T rr_rrc_get_arfcn_band_sub_id_dyn(uint16 arfcn, rrc_rr_frequence_band_e_type band_ind);
extern void rr_rrc_set_band_pref_dyn(sys_band_mask_type band_pref);
extern void rr_rrc_set_band_pref_sub_id_dyn(sys_band_mask_type band_pref);
extern void rr_rrc_set_bst_band_pref_dyn(sys_band_mask_type band_pref);
extern void rr_rrc_set_bst_band_pref_sub_id_dyn(sys_band_mask_type band_pref);
#endif /* FEATURE_WCDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
extern boolean rr_get_inter_rat_bplmn_prtl_result_sub_id_dyn(sys_detailed_plmn_list_s_type *available_plmn_list_ptr, sys_modem_as_id_e_type as_id);
extern boolean rr_get_inter_rat_bplmn_prtl_result_dyn(sys_detailed_plmn_list_s_type *available_plmn_list_ptr);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

extern void rr_init_hz_info_dyn(void);
#ifndef PERLUTF
extern boolean rr_hz_register_state_change_cb_dyn(rr_hz_state_cb_fn_T new_fn_ptr);
#else
#error code not present
#endif

extern void rr_hz_deregister_state_change_cb_dyn(void);

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_GSM_TDS)
extern void tdsrr_send_rrc_message_to_rr_dyn(void*, tdsrr_rrc_cmd_e_type, word);
extern sys_band_T tdsrr_get_arfcn_band_sub_id_dyn(uint16, tdsrr_frequence_band_e_type, sys_modem_as_id_e_type);
extern sys_band_T tdsrr_get_arfcn_band_dyn(uint16, tdsrr_frequence_band_e_type);
extern void tdsrr_set_band_pref_sub_id_dyn(sys_band_mask_type band_pref);
extern void tdsrr_set_band_pref_dyn(sys_band_mask_type);
extern void tdsrr_set_bst_band_pref_sub_id_dyn(sys_band_mask_type, sys_modem_as_id_e_type);
extern void tdsrr_set_bst_band_pref_dyn(sys_band_mask_type);
extern PLMN_id_T tdsrr_get_rr_curr_camped_plmn_sub_id_dyn(sys_modem_as_id_e_type);
extern PLMN_id_T tdsrr_get_rr_curr_camped_plmn_dyn(void);
extern void tdsrr_acq_db_save_sub_id_dyn(sys_modem_as_id_e_type);
extern void tdsrr_acq_db_save_dyn(void);
#endif /* FEATURE_TDSCDMA || FEATURE_GSM_TDS */

extern void geran_send_cm_test_mode_on_dyn(void);
extern void geran_send_cm_test_mode_off_dyn(void);

#if !defined(PERLUTF) && defined(FEATURE_RR_THERMAL_MITIGATION)
extern void change_event_cb_fn_dyn(void*, unsigned int, void*, unsigned int);
extern void initialise_event_cb_fn_dyn(void*, unsigned int, void*, unsigned int);
#endif
extern sys_sband_e_type geran_map_gsm_band_to_sys_band_class_dyn(eng_mode_band_t gsm_band);
extern uint8 gprs_odm_info_get_rach_counter_dyn(void);
extern uint8 gprs_odm_info_get_num_of_neighbour_cells_dyn(void);
extern uint8 gprs_odm_info_get_paging_multiframe_dyn(void);
extern int8  gprs_odm_info_get_rx_lev_dBm_dyn(void);
extern uint8 gprs_odm_info_get_rx_lev_status_dyn(void);
extern gprs_odm_cell_type_t gprs_odm_info_get_cell_type_dyn(void);
#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
extern void gprs_odm_get_plmn_lac_list_dyn(gprs_odm_plmn_lacs_t*);
extern void gprs_odm_set_plmn_lac_list_dyn(const gprs_odm_plmn_lac_ranges_t*, gprs_odm_lac_cat_t);
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
extern boolean geran_get_current_serving_cell_info_dyn(geran_current_scell_info_t *current_scell_info);
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
#endif
