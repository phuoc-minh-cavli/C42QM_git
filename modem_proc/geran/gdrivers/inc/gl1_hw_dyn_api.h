#ifndef GL1_HW_DYN_API_H
#define GL1_HW_DYN_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          G L 1 H W    D Y N    A P I

GENERAL DESCRIPTION
   This header file contains prototypes of dynamic APIs 
   
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gdrivers/inc/gl1_hw_dyn_api.h#6 $
$DateTime: 2020/03/29 06:09:26 $ $Author: pwbldsvc $


when       who       what, where, why
--------   ---       ---------------------------------------------------------
27/03/20    ktg     CR2575302 FR 58047: FSK based alarm reporting on jamming detection for IIOT
01/10/18    ng      CR2325381 Move all GPS code under feature flag 'FEATURE_CGPS_TIME_TAGGING'
05/03/18    sk      CR2230579 (FR 50568): Porting the Single Voice Session FR changes onto CT.1.0
09/01/18    hd      CR2167422 : FR 46686: Support GERAN as dynamic module using DMM/External pagin framework 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "gl1_hw_clk_ctl.h"
#include "gl1_msg_pdch.h"
#include "gpl1_gprs_test.h"
#include "l1i.h"
#include "l1_isr.h"
#include "l1_utils.h"
#include "l1_task.h"

#include "geran_dual_sim.h"
#include "gl1_defs.h"

#include "gl1_hw_g.h"

#include "geran_test_mode_api.h"


void gl1_hw_gsac_clock_not_required_dyn (uint32 mask);

void gl1_hw_gsac_clock_is_required_dyn (uint32 mask);

rfgsm_band_type gl1_hw_arfcn_to_rfgsm_band_dyn( ARFCN_T arfcn );

void gstmr_align_timebase_dyn(uint16 delay_qs);

void gl1_hw_timer_sleep_commit_dyn( void );

/* Dynamic function prototype  */
void geran_test_mode_api_restart_srb_loop_dyn(
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type as_id
#else
void
#endif /*FEATURE_DUAL_SIM */
);  

void geran_test_mode_api_set_vctcxo_pdm_val_dyn(int32 pdm_val,sys_modem_as_id_e_type as_id);

void geran_test_mode_api_gprs_l1_mac_acc_func_dyn(geran_test_mode_api_mac_l1_sig_t *common_msg_ptr
#ifdef FEATURE_DUAL_SIM
,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */
);

uint32 geran_test_mode_api_get_best_six_info_no_of_entries_dyn(void *msg);

void geran_test_mode_api_start_geran_dynamic(
                                    void (*tdma_isr_ptr)(sys_modem_as_id_e_type),
                                    rex_tcb_type  *task_ptr,
                                    void          (*task_wait_handler)(rex_sigs_type, sys_modem_as_id_e_type),
                                    rex_sigs_type task_wait_sig_1,
                                    rex_sigs_type task_wait_sig_2,
                                    gl1_init_rflm_type gsm_init_rflm_state,
                                    sys_modem_as_id_e_type as_id
                                  ,boolean enableRxd,
                                  boolean enableDrx,
                                  boolean enablePrx
                                   );

#ifdef FEATURE_FSK_SUPPORT
void geran_test_mode_api_start_fsk_geran_dynamic(
                                      void (*tdma_isr_ptr)(sys_modem_as_id_e_type),
                                      rex_tcb_type  *task_ptr,
                                      void          (*task_wait_handler)(rex_sigs_type, sys_modem_as_id_e_type),
                                      rex_sigs_type task_wait_sig_1,
                                      rex_sigs_type task_wait_sig_2,
                                      gl1_init_rflm_type gsm_init_rflm_state
                                     );
void geran_test_mode_api_stop_fsk_geran_dynamic(void);
#endif

void geran_test_mode_api_stop_geran_dynamic(boolean deep_sleep, sys_modem_as_id_e_type as_id);



boolean gl1_hw_ftm_get_rx_power_results_dyn(uint32 *rssi, dBx16_T *pwr_dbm,
                                             uint32 *rssi_divrx, dBx16_T *pwr_dbm_divrx,
                                             sys_modem_as_id_e_type as_id);


sys_modem_as_id_e_type gl1_hw_rf_map_device2_as_id_dyn(rfm_device_enum_type device,ftm_mode_type ftm_mode);

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
channel_mode_T gl1_get_current_ded_chan_type_dyn( sys_modem_as_id_e_type as_id );
#else
channel_mode_T gl1_get_current_ded_chan_type_dyn(void);
#endif


#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void gl1_TimeTagRequest_dyn(boolean allow_sleep_timetag, sys_modem_as_id_e_type as_id);
#else
void gl1_TimeTagRequest_dyn(boolean allow_sleep_timetag);
#endif

void gpl1_config_modem_power_stats_tx_dyn(boolean en_pwr_stats, sys_modem_as_id_e_type as_id);
void gpl1_get_modem_power_stats_tx_dyn(uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX], sys_modem_as_id_e_type as_id);


void geran_test_mode_api_turn_tx_on_dyn (sys_modem_as_id_e_type as_id );

void geran_test_mode_api_set_dedicated_dyn ( boolean dedicated,sys_modem_as_id_e_type as_id );

void geran_test_mode_api_rf_set_tx_band_dyn (rfgsm_band_type band,sys_modem_as_id_e_type as_id);

int32 geran_test_mode_api_rf_get_freq_error_dyn (sys_modem_as_id_e_type as_id);

void geran_test_mode_api_set_idle_mode_dyn (boolean idle,sys_modem_as_id_e_type as_id);

uint32 geran_test_mode_api_get_FN_dyn (sys_modem_as_id_e_type as_id);

uint32 geran_test_mode_api_get_qsym_count_dyn(sys_modem_as_id_e_type as_id);

void geran_test_mode_api_schedule_frame_tick_dyn(uint32 timestamp,sys_modem_as_id_e_type as_id);

void geran_test_mode_api_sched_rx_tx_dyn (sys_modem_as_id_e_type as_id);

void geran_test_mode_api_rx_dtm_gprs_dyn 
(
 const gl1_hw_rx_signal_type      signal_info[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS],
 uint8                            num_tn,
 const gl1_hw_rx_pkt_burst_type   *burst_info,
 gl1_hw_rx_tags_type              tags[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS],
 sys_modem_as_id_e_type           as_id
);

void geran_test_mode_api_rx_dtm_gsm_dyn 
(
 const gl1_hw_rx_signal_type   *signal_info,
 const gl1_hw_rx_burst_type    *burst_info,
 gl1_hw_rx_tags_type           *tags,
 sys_modem_as_id_e_type        as_id

);

void geran_test_mode_api_get_rx_metrics_dyn (
 gl1_defs_rx_pkt_metrics_type **metrics_info,
 uint8                        num_msgs,
 boolean                      get_usf_info,
 boolean                      get_hard_decisions,
 gl1_hw_rx_tags_type          *tags,
 sys_modem_as_id_e_type       as_id
);

boolean geran_test_mode_api_get_rx_data_dyn 
(
 boolean                   ptcch_type,
 gl1_defs_tn_type          tn,
 gl1_hw_rx_tags_type       *tag,
 boolean                   last_attempt,
 gl1_defs_rx_pkt_data_type *msg_info,
 sys_modem_as_id_e_type    as_id

);

boolean geran_test_mode_api_get_sync_rx_metrics_dyn(
 gl1_defs_metrics_rpt     *rpt,
 gl1_hw_rx_tags_type      *tags,
 sys_modem_as_id_e_type   as_id
);

void geran_test_mode_api_rf_tx_burst_dyn (uint8                    num_assigned,
                                   uint8                    num_slots,
                                   rfgsm_band_type          band,
                                   uint16                   arfcn,
                                   int32                    freq_error,
                                   uint16                   pa_index[GL1_DEFS_MAX_ASSIGNED_UL_TS],
                                   gl1_defs_modulation_type mod[GL1_DEFS_MAX_ASSIGNED_UL_TS],
                                   sys_modem_as_id_e_type as_id);

void geran_test_mode_api_log_burst_metrics_dyn 
(
gl1_msg_chan_type            channel,
gl1_msg_metrics_rpt          *rpt_ptr[],
sys_modem_as_id_e_type       as_id
);

boolean geran_test_mode_api_is_gfw_error_detected_dyn ( sys_modem_as_id_e_type as_id );

void geran_test_mode_api_start_acquisition_dyn 
(
 ARFCN_T ARFCN,
 dBx16_T rx_signal_strength,
 uint16  timing_offset,
 int16   frequency_offset,
 boolean sch_decode,
 gl1_hw_acq_afc_type AFC,
 gl1_hw_rx_tags_type *tags,
 sys_modem_as_id_e_type as_id
);

void geran_test_mode_api_stop_acquisition_dyn (uint16 offset,sys_modem_as_id_e_type as_id);

gs_status_T geran_test_mode_api_set_sacch_tx_pwr_control_dyn (boolean tx_pwr_control,sys_modem_as_id_e_type as_id);

void geran_test_mode_api_do_tch_loopback_dyn (gl1_defs_loopback_type loopback, sys_modem_as_id_e_type as_id);

boolean geran_test_mode_api_write_tx_pkt_data_dyn(
 gl1_defs_tx_pkt_data_type *msg_ptr,
 uint8                     len,
 gl1_defs_tn_type          tn,
 gl1_hw_pkt_msg_type       msg_type,
 gl1_hw_channel_type       chan_type,
 gl1_hw_tx_tags_type       *tags,
 sys_modem_as_id_e_type    as_id
);

boolean geran_test_mode_api_get_tx_scheduled_result_dyn
(
 gl1_hw_tx_tags_type      *tags,
 boolean                  discard_untxed,
 sys_modem_as_id_e_type   as_id

);


void geran_test_mode_api_tx_gprs_dyn (
 const gl1_hw_tx_signal_type       *signal_info,
 uint8                             num_tn,
 const gl1_hw_tx_pkt_burst_type    *burst_info,
 gl1_hw_tx_tags_type               *tags,
 uint8                             num_msgs,
 uint8                             num_ul_timeslots,
 sys_modem_as_id_e_type            as_id
);


boolean geran_test_mode_api_cm_get_power_measure_results_dyn (
 gl1_hw_measurement_T         samples[],
 uint8                        num,      /* Number of results to process  */
 gl1_hw_rx_tags_type          tags[],
 sys_modem_as_id_e_type       as_id
);

void geran_test_mode_api_async_intf_app_mode_config_dyn (
 GfwAppModeType               app_mode,
 uint32                       rxlm_buf_idx,
 sys_modem_as_id_e_type       as_id
);

void geran_test_mode_api_do_ip2cal_dyn ( uint8 NoOfIP2CalSteps, uint32 ip2CalStepSizeQS,sys_modem_as_id_e_type as_id );

void gl1_hw_tcxo_rpush_flag_cb_dyn( boolean gl1_tcxo_enable_push );
void gl1_hw_tcxo_rpush_mode_cb_dyn( tcxomgr_rpush_mode_cb_data_type gl1_tcxo_push_mode );


#endif /* GL1_HW_DYN_API_H */
