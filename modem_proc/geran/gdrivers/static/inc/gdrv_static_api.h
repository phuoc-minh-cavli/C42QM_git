 #ifndef GDRV_STATIC_API_H
#define GDRV_STATIC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     G D R V   S T A T I C   A P I

GENERAL DESCRIPTION
     This file contains the function pointer types and vTable type definition for gdrivers

EXTERNALIZED FUNCTIONS
     gdrv_vtbl - the vTable for gdrivers

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gdrivers/static/inc/gdrv_static_api.h#8 $
$DateTime: 2020/03/29 06:09:26 $ $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
27/03/20   ktg      CR2575302 FR 58047: FSK based alarm reporting on jamming detection for IIOT
01/10/18   ng      CR2325381 Move all GPS code under feature flag 'FEATURE_CGPS_TIME_TAGGING'
16/08/18   akm     CR2297635 Compilation error EXTPL flavor
07/08/18   shm     CR 2274892 | Instantaneous WWAN timing and frequency information (rude wakeup)
05/03/18   sk       CR2230579 (FR 50568): Porting the Single Voice Session FR changes onto CT.1.0
09/01/18   hd       CR2167422 : FR 46686: Support GERAN as dynamic module using DMM/External paging framework 
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
#include "gmacdef.h"
#include "gmacutil.h"
#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "geran_test_mode_api.h"
#include "g2wsrchwfwintf.h"
#include "g2wsrchdrv.h"
#include "vstmr_wcdma.h"
#include "gl1_hwi.h"
#include "mdsp_cmd.h"
#include "gl1_hw_g.h"

#include "gl1_hw_dyn_api.h"




/****************************************************/

typedef void (*geran_test_mode_api_set_vctcxo_pdm_val_t)(int32 pdm_val,sys_modem_as_id_e_type as_id);

typedef void (*geran_test_mode_api_gprs_l1_mac_acc_func_t)(geran_test_mode_api_mac_l1_sig_t *common_msg_ptr
#ifdef FEATURE_DUAL_SIM
,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */
);

typedef uint32 (*geran_test_mode_api_get_best_six_info_no_of_entries_t)(void *msg);

typedef void (*geran_test_mode_api_start_geran_dynamic_t)(
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
typedef void (*geran_test_mode_api_start_fsk_geran_dynamic_t)(
                                      void (*tdma_isr_ptr)(sys_modem_as_id_e_type),
                                      rex_tcb_type  *task_ptr,
                                      void          (*task_wait_handler)(rex_sigs_type, sys_modem_as_id_e_type),
                                      rex_sigs_type task_wait_sig_1,
                                      rex_sigs_type task_wait_sig_2,
                                      gl1_init_rflm_type gsm_init_rflm_state
                                     );
typedef void (*geran_test_mode_api_stop_fsk_geran_dynamic_t)(void);
#endif

typedef void (*geran_test_mode_api_stop_geran_dynamic_t)(boolean deep_sleep, sys_modem_as_id_e_type as_id);

typedef void (*geran_test_mode_api_restart_srb_loop_t)
(
#ifdef FEATURE_DUAL_SIM
sys_modem_as_id_e_type as_id
#else
void
#endif /*FEATURE_DUAL_SIM */
);


typedef boolean (*gl1_hw_ftm_get_rx_power_results_t)(uint32 *rssi, dBx16_T *pwr_dbm,
                                                  uint32 *rssi_divrx, dBx16_T *pwr_dbm_divrx,
                                                  sys_modem_as_id_e_type as_id);
typedef sys_modem_as_id_e_type (*gl1_hw_rf_map_device2_as_id_t)(rfm_device_enum_type device,ftm_mode_type ftm_mode);
 
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
typedef channel_mode_T (*gl1_get_current_ded_chan_type_t)( sys_modem_as_id_e_type as_id );
#else
typedef channel_mode_T (*gl1_get_current_ded_chan_type_t)(void);
#endif
 

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
typedef void (*gl1_TimeTagRequest_t)(boolean allow_sleep_timetag, sys_modem_as_id_e_type as_id);
#else
typedef void (*gl1_TimeTagRequest_t)(boolean allow_sleep_timetag);
#endif

typedef void (*gpl1_config_modem_power_stats_tx_t)(boolean en_pwr_stats, sys_modem_as_id_e_type as_id);
typedef void (*gpl1_get_modem_power_stats_tx_t)(uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX], sys_modem_as_id_e_type as_id);


typedef  void (*geran_test_mode_api_turn_tx_on_t) (sys_modem_as_id_e_type as_id );

typedef  void (*geran_test_mode_api_set_dedicated_t) ( boolean dedicated,sys_modem_as_id_e_type as_id );

typedef  void (*geran_test_mode_api_rf_set_tx_band_t) (rfgsm_band_type band,sys_modem_as_id_e_type as_id);

typedef int32 (*geran_test_mode_api_rf_get_freq_error_t) (sys_modem_as_id_e_type as_id);

typedef  void (*geran_test_mode_api_set_idle_mode_t) (boolean idle,sys_modem_as_id_e_type as_id);

typedef uint32 (*geran_test_mode_api_get_FN_t) (sys_modem_as_id_e_type as_id);

typedef uint32 (*geran_test_mode_api_get_qsym_count_t) (sys_modem_as_id_e_type as_id);

typedef  void (*geran_test_mode_api_schedule_frame_tick_t) (uint32 timestamp,sys_modem_as_id_e_type as_id);

typedef  void (*geran_test_mode_api_sched_rx_tx_t) (sys_modem_as_id_e_type as_id);

typedef  void (*geran_test_mode_api_rx_dtm_gprs_t) 
(
   const gl1_hw_rx_signal_type      signal_info[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS],
   uint8                            num_tn,
   const gl1_hw_rx_pkt_burst_type   *burst_info,
   gl1_hw_rx_tags_type              tags[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS],
   sys_modem_as_id_e_type           as_id
);

typedef  void (*geran_test_mode_api_rx_dtm_gsm_t) 
(
   const gl1_hw_rx_signal_type   *signal_info,
   const gl1_hw_rx_burst_type    *burst_info,
   gl1_hw_rx_tags_type           *tags,
   sys_modem_as_id_e_type        as_id

);

typedef  void (*geran_test_mode_api_get_rx_metrics_t) (
   gl1_defs_rx_pkt_metrics_type **metrics_info,
   uint8                        num_msgs,
   boolean                      get_usf_info,
   boolean                      get_hard_decisions,
   gl1_hw_rx_tags_type          *tags,
   sys_modem_as_id_e_type       as_id
);

typedef boolean (*geran_test_mode_api_get_rx_data_t) 
(
   boolean                   ptcch_type,
   gl1_defs_tn_type          tn,
   gl1_hw_rx_tags_type       *tag,
   boolean                   last_attempt,
   gl1_defs_rx_pkt_data_type *msg_info,
   sys_modem_as_id_e_type    as_id

);

typedef boolean (*geran_test_mode_api_get_sync_rx_metrics_t) 
(
   gl1_defs_metrics_rpt     *rpt,
   gl1_hw_rx_tags_type      *tags,
   sys_modem_as_id_e_type   as_id
);

typedef  void (*geran_test_mode_api_rf_tx_burst_t) (uint8                    num_assigned,
                                     uint8                    num_slots,
                                     rfgsm_band_type          band,
                                     uint16                   arfcn,
                                     int32                    freq_error,
                                     uint16                   pa_index[GL1_DEFS_MAX_ASSIGNED_UL_TS],
                                     gl1_defs_modulation_type mod[GL1_DEFS_MAX_ASSIGNED_UL_TS],
                                     sys_modem_as_id_e_type as_id);

typedef  void (*geran_test_mode_api_log_burst_metrics_t) 
(
  gl1_msg_chan_type            channel,
  gl1_msg_metrics_rpt          *rpt_ptr[],
  sys_modem_as_id_e_type       as_id
);

typedef boolean (*geran_test_mode_api_is_gfw_error_detected_t) ( sys_modem_as_id_e_type as_id );

typedef  void (*geran_test_mode_api_start_acquisition_t) 
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

typedef  void (*geran_test_mode_api_stop_acquisition_t) (uint16 offset,sys_modem_as_id_e_type as_id);

typedef gs_status_T (*geran_test_mode_api_set_sacch_tx_pwr_control_t) (boolean tx_pwr_control,sys_modem_as_id_e_type as_id);

typedef  void (*geran_test_mode_api_do_tch_loopback_t) (gl1_defs_loopback_type loopback, sys_modem_as_id_e_type as_id);

typedef boolean (*geran_test_mode_api_write_tx_pkt_data_t) (
   gl1_defs_tx_pkt_data_type *msg_ptr,
   uint8                     len,
   gl1_defs_tn_type          tn,
   gl1_hw_pkt_msg_type       msg_type,
   gl1_hw_channel_type       chan_type,
   gl1_hw_tx_tags_type       *tags,
   sys_modem_as_id_e_type    as_id
);

typedef boolean (*geran_test_mode_api_get_tx_scheduled_result_t)
(
   gl1_hw_tx_tags_type      *tags,
   boolean                  discard_untxed,
   sys_modem_as_id_e_type   as_id

);


typedef  void (*geran_test_mode_api_tx_gprs_t) (
   const gl1_hw_tx_signal_type       *signal_info,
   uint8                             num_tn,
   const gl1_hw_tx_pkt_burst_type    *burst_info,
   gl1_hw_tx_tags_type               *tags,
   uint8                             num_msgs,
   uint8                             num_ul_timeslots,
   sys_modem_as_id_e_type            as_id
);


typedef boolean (*geran_test_mode_api_cm_get_power_measure_results_t) (
   gl1_hw_measurement_T         samples[],
   uint8                        num,      /* Number of results to process  */
   gl1_hw_rx_tags_type          tags[],
   sys_modem_as_id_e_type       as_id
);

typedef  void (*geran_test_mode_api_async_intf_app_mode_config_t) (
   GfwAppModeType               app_mode,
   uint32                       rxlm_buf_idx,
   sys_modem_as_id_e_type       as_id
);

typedef  void (*geran_test_mode_api_do_ip2cal_t) ( uint8 NoOfIP2CalSteps, uint32 ip2CalStepSizeQS,sys_modem_as_id_e_type as_id );

typedef  void (*gl1_hw_gsac_clock_required_t) (uint32  );

typedef rfgsm_band_type (*gl1_hw_arfcn_to_rfgsm_band_t)( ARFCN_T );

typedef void (*gstmr_align_timebase_t)(uint16);


typedef void (*gl1_hw_timer_sleep_commit_t)( void );
typedef void (*gl1_hw_tcxo_rpush_flag_cb_t)(boolean);
typedef void (*gl1_hw_tcxo_rpush_mode_cb_t)(tcxomgr_rpush_mode_cb_data_type );


/****************************************************/



typedef struct
{

  geran_test_mode_api_set_vctcxo_pdm_val_t                                  geran_test_mode_api_set_vctcxo_pdm_val;
                                                                            
  geran_test_mode_api_gprs_l1_mac_acc_func_t                                geran_test_mode_api_gprs_l1_mac_acc_func;
                                                                            
  geran_test_mode_api_get_best_six_info_no_of_entries_t                     geran_test_mode_api_get_best_six_info_no_of_entries;
  
  geran_test_mode_api_start_geran_dynamic_t                                 geran_test_mode_api_start_geran_dynamic_fn_ptr;
                                                                            
  geran_test_mode_api_stop_geran_dynamic_t                                  geran_test_mode_api_stop_geran_dynamic_fn_ptr;
                                                                            
  geran_test_mode_api_restart_srb_loop_t                                    geran_test_mode_api_restart_srb_loop;
                                                                            
  gl1_hw_ftm_get_rx_power_results_t                                         gl1_hw_ftm_get_rx_power_results;
                                                                            
  gl1_hw_rf_map_device2_as_id_t                                             gl1_hw_rf_map_device2_as_id;

  gl1_get_current_ded_chan_type_t                                           gl1_get_current_ded_chan_type;
                                                                            
  gl1_TimeTagRequest_t                                                      gl1_TimeTagRequest;

  gpl1_config_modem_power_stats_tx_t                                        gpl1_config_modem_power_stats_tx;
  
  gpl1_get_modem_power_stats_tx_t                                           gpl1_get_modem_power_stats_tx;

  geran_test_mode_api_turn_tx_on_t                                          geran_test_mode_api_turn_tx_on;

  geran_test_mode_api_set_dedicated_t                                       geran_test_mode_api_set_dedicated;
                                                                     
  geran_test_mode_api_rf_set_tx_band_t                                      geran_test_mode_api_rf_set_tx_band;
                                                                     
  geran_test_mode_api_rf_get_freq_error_t                                   geran_test_mode_api_rf_get_freq_error;
                                                                     
  geran_test_mode_api_set_idle_mode_t                                       geran_test_mode_api_set_idle_mode;
                                                                     
  geran_test_mode_api_get_FN_t                                              geran_test_mode_api_get_FN;
                                                                     
  geran_test_mode_api_get_qsym_count_t                                      geran_test_mode_api_get_qsym_count;
                                                                     
  geran_test_mode_api_schedule_frame_tick_t                                 geran_test_mode_api_schedule_frame_tick;
                                                                     
  geran_test_mode_api_sched_rx_tx_t                                         geran_test_mode_api_sched_rx_tx;
                                                                     
  geran_test_mode_api_rx_dtm_gprs_t                                         geran_test_mode_api_rx_dtm_gprs;
                                                                     
  geran_test_mode_api_rx_dtm_gsm_t                                          geran_test_mode_api_rx_dtm_gsm; 
                                                                     
  geran_test_mode_api_get_rx_metrics_t                                      geran_test_mode_api_get_rx_metrics;
  
  geran_test_mode_api_get_rx_data_t                                         geran_test_mode_api_get_rx_data; 
                                                                     
  geran_test_mode_api_get_sync_rx_metrics_t                                 geran_test_mode_api_get_sync_rx_metrics;
                                                                     
  geran_test_mode_api_rf_tx_burst_t                                         geran_test_mode_api_rf_tx_burst;               
                                                                     
  geran_test_mode_api_log_burst_metrics_t                                   geran_test_mode_api_log_burst_metrics;
                                                                     
  geran_test_mode_api_is_gfw_error_detected_t                               geran_test_mode_api_is_gfw_error_detected;
                                                                     
  geran_test_mode_api_start_acquisition_t                                   geran_test_mode_api_start_acquisition;
                                                                     
  geran_test_mode_api_stop_acquisition_t                                    geran_test_mode_api_stop_acquisition;
                                                                     
  geran_test_mode_api_set_sacch_tx_pwr_control_t                            geran_test_mode_api_set_sacch_tx_pwr_control;
                                                                     
  geran_test_mode_api_do_tch_loopback_t                                     geran_test_mode_api_do_tch_loopback;
                                                                     
  geran_test_mode_api_write_tx_pkt_data_t                                   geran_test_mode_api_write_tx_pkt_data;
  geran_test_mode_api_get_tx_scheduled_result_t                             geran_test_mode_api_get_tx_scheduled_result;
  geran_test_mode_api_tx_gprs_t                                             geran_test_mode_api_tx_gprs;
  geran_test_mode_api_cm_get_power_measure_results_t                        geran_test_mode_api_cm_get_power_measure_results;
  geran_test_mode_api_async_intf_app_mode_config_t                          geran_test_mode_api_async_intf_app_mode_config;
  geran_test_mode_api_do_ip2cal_t                                           geran_test_mode_api_do_ip2cal;
  gl1_hw_gsac_clock_required_t                                              gsac_clock_not_required;
  gl1_hw_gsac_clock_required_t                                              gsac_clock_is_required;
  gl1_hw_arfcn_to_rfgsm_band_t                                              arfcn_to_rfgsm_band;
  gstmr_align_timebase_t                                                    gstmr_align_timebase;
  gl1_hw_timer_sleep_commit_t                                               timer_sleep_commit;
  gl1_hw_tcxo_rpush_flag_cb_t                                               rpush_flag_cb;
  gl1_hw_tcxo_rpush_mode_cb_t                                               rpush_mode_cb;
#ifdef FEATURE_FSK_SUPPORT
  geran_test_mode_api_start_fsk_geran_dynamic_t                             geran_test_mode_api_start_fsk_geran_dynamic_fn_ptr;
  geran_test_mode_api_stop_fsk_geran_dynamic_t                              geran_test_mode_api_stop_fsk_geran_dynamic_fn_ptr;
#endif
  }geran_static_gdrv_ext_api_s;

extern geran_static_gdrv_ext_api_s gdrv_vtbl;

#endif /*GDRV_STATIC_API_H*/
