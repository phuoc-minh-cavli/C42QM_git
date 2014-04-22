#ifndef GERAN_GRR_STATIC_API_H
#define GERAN_GRR_STATIC_API_H

/*!
  @file geran_grr_static_api.h

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

$Header: //components/rel/geran.mpss/5.2.0/grr/static/geran_grr_static_api.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
16/05/17   sc      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"

#include "rr_task.h"
#include "sys.h"
#include "geran_grr_types.h"
#include "rr_lsm_if.h"
#include "rr_cb_if.h"
#include "rr_multiband.h"
#include "rr_public_store.h"
#include "rr_general.h"
#include "gprs_odm_info.h"
#include "geran_grr_dyn_api.h"

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
typedef void (*message_dispatcher_t) (rr_cmd_type*);
typedef void (*grr_process_reset_signal_t)(void);
typedef sys_sband_e_type (*geran_grr_get_arfcn_band_t)(uint16, geran_grr_band_ind_e, boolean*);
typedef ARFCN_T (*geran_grr_convert_to_arfcn_type_t)(uint16, geran_grr_band_ind_e);
typedef void (*mb_initialise_t)(void);
typedef boolean (*arfcns_equal_t)(ARFCN_T, ARFCN_T);
typedef boolean (*geran_start_capture_scan_data_t)(uint32*, uint16);
typedef boolean (*geran_stop_capture_scan_data_t)(void);
typedef boolean (*geran_get_nv_recovery_restart_enabled_t)(void);
typedef void (*geran_initiate_recovery_restart_t)(const geran_client_enum_t);
typedef void (*geran_grr_get_current_timing_advance_info_per_subs_t)(geran_grr_current_timing_advance_type*);
typedef void (*geran_grr_get_current_timing_advance_info_t)(geran_grr_current_timing_advance_type*);
typedef void (*geran_grr_dtx_override_on_per_subs_t)(void);
typedef void (*geran_grr_dtx_override_on_t)(void);
typedef void (*rr_dtx_override_on_t)(void);
typedef void (*geran_grr_dtx_override_off_per_subs_t)(void);
typedef void (*geran_grr_dtx_override_off_t)(void);
typedef void (*rr_dtx_override_off_t)(void);
typedef void (*geran_grr_get_current_bs_info_per_subs_t)(geran_grr_current_bs_info_type*);
typedef void (*geran_grr_get_current_bs_info_t)(geran_grr_current_bs_info_type*);
typedef boolean (*geran_grr_set_codec_ho_sample_rate_per_subs_t)(uint32, boolean);
typedef boolean (*geran_grr_set_codec_ho_sample_rate_t)(uint32, boolean);
typedef boolean (*geran_grr_get_l3_msg_FPOS_t)(const byte, const byte*, uint32*, boolean*);
typedef void (*geran_grr_register_cgps_event_cb_t)(rr_cgps_event_cb_type);
typedef uint32 (*geran_api_grr_arfcn_frequency_t)(const ARFCN_T);
typedef void (*rr_compute_BSIC_from_SCH_block_t)(BSIC_T*, byte*);
typedef void (*rr_read_public_store_t)(rr_public_store_id_t, void*);
typedef rrps_frequency_list_t* (*rr_read_public_store_freq_list_t)(uint8);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
typedef void (*rr_nv_send_nv_refresh_ind_t)(void);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

typedef boolean (*rr_nv_eng_mode_debug_is_enabled_t)(void);
typedef rrps_frequency_list_t* (*rr_packet_assignment_mobile_alloc_t)(encoding_type_e, void*, boolean*);
typedef boolean (*rr_gapi_is_supported_band_t)(sys_band_T);
typedef boolean (*rr_is_supported_band_t)(rr_internal_band_T);
typedef boolean (*rr_get_nmr_bcch_t)(uint8*, rr_bcch_list_type*);
typedef boolean (*rr_api_get_nmr_bcch_t)(uint8*, uint8, rr_bcch_list_type*);
typedef boolean (*rr_get_cell_id_t)(uint8*);
typedef boolean (*rr_get_timing_advance_t)(uint8*, uint8*);
typedef rr_internal_band_T (*rr_get_band_t)(word);
typedef rr_internal_band_T (*rr_get_current_band_t)(void);
typedef byte (*rr_get_current_rf_power_class_t)(void);
typedef byte (*rr_gapi_get_band_rf_power_class_t)(rr_internal_band_T);
typedef byte (*rr_get_band_rf_power_class_t)(rr_internal_band_T);
typedef byte (*rr_get_supported_band_count_t)(void);
typedef sys_band_class_e_type (*rr_get_band_indicator_for_nas_t)(boolean);
typedef byte (*rr_get_maximum_power_output_for_frequency_t)(ARFCN_T);
typedef uint16 (*rr_dtm_get_max_LAPDm_octets_per_subs_t)(void);
typedef uint16 (*rr_dtm_get_max_LAPDm_octets_t)(void);

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
typedef boolean (*rr_register_cell_change_ind_cb_t)(RR_CELL_CHANGE_IND_CB_FUNC_TYPE*);
typedef boolean (*rr_deregister_cell_change_ind_cb_t)(RR_CELL_CHANGE_IND_CB_FUNC_TYPE*);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
typedef boolean (*rr_ds_register_cell_change_ind_cb_t)(RR_CELL_CHANGE_IND_CB_FUNC_TYPE*);
typedef boolean (*rr_ds_deregister_cell_change_ind_cb_t)(RR_CELL_CHANGE_IND_CB_FUNC_TYPE*);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

typedef byte (*rr_ds_get_supported_band_count_t)();
typedef sys_band_class_e_type (*rr_ds_get_band_indicator_for_nas_t)(boolean);
typedef boolean (*rr_ds_get_nmr_bcch_t)(uint8*, rr_bcch_list_type*);
typedef boolean (*rr_ds_api_get_nmr_bcch_t)(uint8*, uint8, rr_bcch_list_type*);
typedef boolean (*rr_ds_get_cell_id_t)(uint8*);
typedef boolean (*rr_ds_get_timing_advance_t)(uint8*, uint8*);

#ifdef FEATURE_WCDMA
typedef void (*rr_send_rrc_message_to_rr_t)(void*, rrc_rr_cmd_e_type, word);
typedef PLMN_id_T (*rrc_get_rr_curr_camped_plmn_sub_id_t)(sys_modem_as_id_e_type);
typedef PLMN_id_T (*rrc_get_rr_curr_camped_plmn_t)(void);
typedef void (*rr_rrc_acq_db_save_t)(void);
typedef void (*rr_rrc_acq_db_save_sub_id_t)(sys_modem_as_id_e_type);
typedef sys_band_T (*rr_rrc_get_arfcn_band_t)(uint16, rrc_rr_frequence_band_e_type);
typedef sys_band_T (*rr_rrc_get_arfcn_band_sub_id_t)(uint16, rrc_rr_frequence_band_e_type, sys_modem_as_id_e_type);
typedef void (*rr_rrc_set_band_pref_t)(sys_band_mask_type);
typedef void (*rr_rrc_set_band_pref_sub_id_t)(sys_band_mask_type, sys_modem_as_id_e_type);
typedef void (*rr_rrc_set_bst_band_pref_t)(sys_band_mask_type);
typedef void (*rr_rrc_set_bst_band_pref_sub_id_t)(sys_band_mask_type, sys_modem_as_id_e_type);
#endif /* FEATURE_WCDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
typedef boolean (*rr_get_inter_rat_bplmn_prtl_result_sub_id_t)(sys_detailed_plmn_list_s_type*, sys_modem_as_id_e_type);
typedef boolean (*rr_get_inter_rat_bplmn_prtl_result_t)(sys_detailed_plmn_list_s_type*);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef void (*rr_init_hz_info_t)(void);

#ifndef PERLUTF
typedef boolean (*rr_hz_register_state_change_cb_t)(rr_hz_state_cb_fn_T);
#else
#error code not present
#endif

typedef void (*rr_hz_deregister_state_change_cb_t)(void);

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_GSM_TDS)
typedef void (*tdsrr_send_rrc_message_to_rr_t)(void*, tdsrr_rrc_cmd_e_type, word);
typedef sys_band_T (*tdsrr_get_arfcn_band_sub_id_t)(uint16, tdsrr_frequence_band_e_type, sys_modem_as_id_e_type);
typedef sys_band_T (*tdsrr_get_arfcn_band_t)(uint16, tdsrr_frequence_band_e_type);
typedef void (*tdsrr_set_band_pref_sub_id_t)(sys_band_mask_type, sys_modem_as_id_e_type);
typedef void (*tdsrr_set_band_pref_t)(sys_band_mask_type);
typedef void (*tdsrr_set_bst_band_pref_sub_id_t)(sys_band_mask_type, sys_modem_as_id_e_type);
typedef void (*tdsrr_set_bst_band_pref_t)(sys_band_mask_type);
typedef PLMN_id_T (*tdsrr_get_rr_curr_camped_plmn_sub_id_t)(sys_modem_as_id_e_type);
typedef PLMN_id_T (*tdsrr_get_rr_curr_camped_plmn_t)(void);
typedef void (*tdsrr_acq_db_save_sub_id_t)(sys_modem_as_id_e_type);
typedef void (*tdsrr_acq_db_save_t)(void);
#endif /* FEATURE_TDSCDMA || FEATURE_GSM_TDS */

typedef void (*geran_send_cm_test_mode_on_t)(void);
typedef void (*geran_send_cm_test_mode_off_t)(void);
#if !defined(PERLUTF) && defined(FEATURE_RR_THERMAL_MITIGATION)
typedef void (*change_event_cb_fn_t)(void*, unsigned int, void*, unsigned int);
typedef void (*initialise_event_cb_fn_t)(void*, unsigned int, void*, unsigned int);
#endif

typedef sys_sband_e_type (*geran_map_gsm_band_to_sys_band_class_t)(eng_mode_band_t);

typedef uint8 (*gprs_odm_info_get_rach_counter_t)(void);
typedef uint8 (*gprs_odm_info_get_num_of_neighbour_cells_t)(void);
typedef uint8 (*gprs_odm_info_get_paging_multiframe_t)(void);
typedef int8  (*gprs_odm_info_get_rx_lev_dBm_t)(void);
typedef uint8 (*gprs_odm_info_get_rx_lev_status_t)(void);
typedef gprs_odm_cell_type_t (*gprs_odm_info_get_cell_type_t)(void);
#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
typedef void (*gprs_odm_get_plmn_lac_list_t)(gprs_odm_plmn_lacs_t*);
typedef void (*gprs_odm_set_plmn_lac_list_t)(const gprs_odm_plmn_lac_ranges_t*, gprs_odm_lac_cat_t);
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
typedef boolean (*geran_get_current_serving_cell_info_t)(geran_current_scell_info_t *);
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

/* Static API fptr struct*/
typedef struct geran_static_grr_ext_api_t
{
  message_dispatcher_t                                   grr_message_dispatcher;
  grr_process_reset_signal_t                             grr_process_reset_signal;
  geran_grr_get_arfcn_band_t                             geran_grr_get_arfcn_band;
  geran_grr_convert_to_arfcn_type_t                      geran_grr_convert_to_arfcn_type;
  mb_initialise_t                                        mb_initialise;
  arfcns_equal_t                                         arfcns_equal;
  geran_start_capture_scan_data_t                        geran_start_capture_scan_data;
  geran_stop_capture_scan_data_t                         geran_stop_capture_scan_data;
  geran_get_nv_recovery_restart_enabled_t                geran_get_nv_recovery_restart_enabled;
  geran_initiate_recovery_restart_t                      geran_initiate_recovery_restart;
  geran_grr_get_current_timing_advance_info_per_subs_t   geran_grr_get_current_timing_advance_info_per_subs;
  geran_grr_get_current_timing_advance_info_t            geran_grr_get_current_timing_advance_info;
  geran_grr_dtx_override_on_per_subs_t                   geran_grr_dtx_override_on_per_subs;
  geran_grr_dtx_override_on_t                            geran_grr_dtx_override_on;
  rr_dtx_override_on_t                                   rr_dtx_override_on;
  geran_grr_dtx_override_off_per_subs_t                  geran_grr_dtx_override_off_per_subs;
  geran_grr_dtx_override_off_t                           geran_grr_dtx_override_off;
  rr_dtx_override_off_t                                  rr_dtx_override_off;
  geran_grr_get_current_bs_info_per_subs_t               geran_grr_get_current_bs_info_per_subs;
  geran_grr_get_current_bs_info_t                        geran_grr_get_current_bs_info;
  geran_grr_set_codec_ho_sample_rate_per_subs_t          geran_grr_set_codec_ho_sample_rate_per_subs;
  geran_grr_set_codec_ho_sample_rate_t                   geran_grr_set_codec_ho_sample_rate;
  geran_grr_get_l3_msg_FPOS_t                            geran_grr_get_l3_msg_FPOS;
  geran_grr_register_cgps_event_cb_t                     geran_grr_register_cgps_event_cb;
  rr_compute_BSIC_from_SCH_block_t                       rr_compute_BSIC_from_SCH_block;
  rr_read_public_store_t                                 rr_read_public_store;
  rr_read_public_store_freq_list_t                       rr_read_public_store_freq_list;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  rr_nv_send_nv_refresh_ind_t                            rr_nv_send_nv_refresh_ind;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  rr_nv_eng_mode_debug_is_enabled_t                      rr_nv_eng_mode_debug_is_enabled;
  rr_packet_assignment_mobile_alloc_t                    rr_packet_assignment_mobile_alloc;
  rr_gapi_is_supported_band_t                            rr_gapi_is_supported_band;
  rr_is_supported_band_t                                 rr_is_supported_band;
  rr_get_nmr_bcch_t                                      rr_get_nmr_bcch;
  rr_api_get_nmr_bcch_t                                  rr_api_get_nmr_bcch;
  rr_get_cell_id_t                                       rr_get_cell_id;
  rr_get_timing_advance_t                                rr_get_timing_advance;
  rr_get_band_t                                          rr_get_band;
  rr_get_current_band_t                                  rr_get_current_band;
  rr_get_current_rf_power_class_t                        rr_get_current_rf_power_class;
  rr_gapi_get_band_rf_power_class_t                      rr_gapi_get_band_rf_power_class;
  rr_get_band_rf_power_class_t                           rr_get_band_rf_power_class;
  rr_get_supported_band_count_t                          rr_get_supported_band_count;
  rr_get_band_indicator_for_nas_t                        rr_get_band_indicator_for_nas;
  rr_get_maximum_power_output_for_frequency_t            rr_get_maximum_power_output_for_frequency;
  rr_dtm_get_max_LAPDm_octets_per_subs_t                 rr_dtm_get_max_LAPDm_octets_per_subs;
  rr_dtm_get_max_LAPDm_octets_t                          rr_dtm_get_max_LAPDm_octets;

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  rr_register_cell_change_ind_cb_t                       rr_register_cell_change_ind_cb;
  rr_deregister_cell_change_ind_cb_t                     rr_deregister_cell_change_ind_cb;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ds_register_cell_change_ind_cb_t                    rr_ds_register_cell_change_ind_cb;
  rr_ds_deregister_cell_change_ind_cb_t                  rr_ds_deregister_cell_change_ind_cb;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

  rr_ds_get_supported_band_count_t                       rr_ds_get_supported_band_count;
  rr_ds_get_band_indicator_for_nas_t                     rr_ds_get_band_indicator_for_nas;
  rr_ds_get_nmr_bcch_t                                   rr_ds_get_nmr_bcch;
  rr_ds_api_get_nmr_bcch_t                               rr_ds_api_get_nmr_bcch;
  rr_ds_get_cell_id_t                                    rr_ds_get_cell_id;
  rr_ds_get_timing_advance_t                             rr_ds_get_timing_advance;

#ifdef FEATURE_WCDMA
  rr_send_rrc_message_to_rr_t                            rr_send_rrc_message_to_rr;
  rrc_get_rr_curr_camped_plmn_sub_id_t                   rrc_get_rr_curr_camped_plmn_sub_id;
  rrc_get_rr_curr_camped_plmn_t                          rrc_get_rr_curr_camped_plmn;
  rr_rrc_acq_db_save_t                                   rr_rrc_acq_db_save;
  rr_rrc_acq_db_save_sub_id_t                            rr_rrc_acq_db_save_sub_id;
  rr_rrc_get_arfcn_band_t                                rr_rrc_get_arfcn_band;
  rr_rrc_get_arfcn_band_sub_id_t                         rr_rrc_get_arfcn_band_sub_id;
  rr_rrc_set_band_pref_t                                 rr_rrc_set_band_pref;
  rr_rrc_set_band_pref_sub_id_t                          rr_rrc_set_band_pref_sub_id;
  rr_rrc_set_bst_band_pref_t                             rr_rrc_set_bst_band_pref;
  rr_rrc_set_bst_band_pref_sub_id_t                      rr_rrc_set_bst_band_pref_sub_id;
#endif /* FEATURE_WCDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_get_inter_rat_bplmn_prtl_result_sub_id_t        rr_get_inter_rat_bplmn_prtl_result_sub_id;
  rr_get_inter_rat_bplmn_prtl_result_t               rr_get_inter_rat_bplmn_prtl_result;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  rr_init_hz_info_t                                      rr_init_hz_info;
  rr_hz_register_state_change_cb_t                       rr_hz_register_state_change_cb;
  rr_hz_deregister_state_change_cb_t                     rr_hz_deregister_state_change_cb;

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_GSM_TDS)
  tdsrr_send_rrc_message_to_rr_t                         tdsrr_send_rrc_message_to_rr;
  tdsrr_get_arfcn_band_sub_id_t                          tdsrr_get_arfcn_band_sub_id;
  tdsrr_get_arfcn_band_t                                 tdsrr_get_arfcn_band;
  tdsrr_set_band_pref_sub_id_t                           tdsrr_set_band_pref_sub_id;
  tdsrr_set_band_pref_t                                  tdsrr_set_band_pref;
  tdsrr_set_bst_band_pref_sub_id_t                       tdsrr_set_bst_band_pref_sub_id;
  tdsrr_set_bst_band_pref_t                              tdsrr_set_bst_band_pref;
  tdsrr_get_rr_curr_camped_plmn_sub_id_t                 tdsrr_get_rr_curr_camped_plmn_sub_id;
  tdsrr_get_rr_curr_camped_plmn_t                        tdsrr_get_rr_curr_camped_plmn;
  tdsrr_acq_db_save_sub_id_t                             tdsrr_acq_db_save_sub_id;
  tdsrr_acq_db_save_t                                    tdsrr_acq_db_save;
#endif /* FEATURE_TDSCDMA || FEATURE_GSM_TDS */

  geran_send_cm_test_mode_on_t                           geran_send_cm_test_mode_on;
  geran_send_cm_test_mode_off_t                          geran_send_cm_test_mode_off;
#if !defined(PERLUTF) && defined(FEATURE_RR_THERMAL_MITIGATION)
  change_event_cb_fn_t                                   change_event_cb_fn;
  initialise_event_cb_fn_t                               initialise_event_cb_fn;
#endif
  geran_map_gsm_band_to_sys_band_class_t                 geran_map_gsm_band_to_sys_band_class;
  gprs_odm_info_get_rach_counter_t                       gprs_odm_info_get_rach_counter;
  gprs_odm_info_get_num_of_neighbour_cells_t             gprs_odm_info_get_num_of_neighbour_cells;
  gprs_odm_info_get_paging_multiframe_t                  gprs_odm_info_get_paging_multiframe;
  gprs_odm_info_get_rx_lev_dBm_t                         gprs_odm_info_get_rx_lev_dBm;
  gprs_odm_info_get_rx_lev_status_t                      gprs_odm_info_get_rx_lev_status;
  gprs_odm_info_get_cell_type_t                          gprs_odm_info_get_cell_type;
#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  gprs_odm_get_plmn_lac_list_t                           gprs_odm_get_plmn_lac_list;
  gprs_odm_set_plmn_lac_list_t                           gprs_odm_set_plmn_lac_list;
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
  geran_get_current_serving_cell_info_t                  geran_get_current_serving_cell_info;
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
} geran_static_grr_ext_api_s;

extern void rr_message_dispatcher(rr_cmd_type *cmd_ptr);

#endif /* GERAN_GRR_STATIC_API_H */
