/*!
  @file rflte_state.h

  @brief
  This file contains all declarations necessary to use the LTE State Machine.

  @details

*/

/*===========================================================================

  Copyright (c) 2013-19 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rftech_lte.mpss/2.10/api/rflte_state.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/21/21   gk      B8_B addition(Network assigned duplex spacing)
06/12/19   ak      PA/ASM internal timings in RFC
03/26/19   sk      FR53265 : VBATT backoff support
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
11/01/18   as      Enable LIF in NB
10/25/18   sk      TRACK B Bringup changes
09/25/18   cv      Added Support of per-band setting of Power Class in Rel.14 Cat.M1 and NB1/NB2
09/08/17   qzh     Update RxAGC algo to use RSRP calculated RSSI for CATM1 FTM NS mode
01/17/17   as      Remove srs_rb_start_nb_region to fix SRS issues
12/21/16   qzh     Add NB1 support
12/08/16   dh      Remove 'FEATURE_RF_HAS_QTUNER' for CAT-M
12/04/16   as/qzh  Add AFC and ul hop merging related changes.
07/30/16   qzh     Add CATM1 ul hopping related changes
06/03/16   qzh     Add CATM1 HD FDD related changes
09/28/15   bir     Added support for PA biass offset w.r.t EARFCN
09/11/15   as      64QAM initial support
12/15/14   ars     Added fix to handle intra CA with split bands where PRx and DRx are on different splits.
11/14/14    pv     HORxD Feature Bring up changes.
11/12/14   bsh     Cleanup TQ assignment for all cells
11/04/14   gvn     Re-tune Tx and Rx in case split band changes on intra scell add or delete
09/12/14    pv     Path to Carrier type conversion. 
09/11/14   kab     Separate placeholders for PRx,Drx Bands in LTE State Machine
08/27/14   svi     Move Alternate Path State Variable to Path Specific
08/21/14   svi     Added Support for Rx Config Script Creation
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
08/15/14   st      CA Alt Path Reconfiguration Support
08/04/14    pv     Added support for PA swp adjust mechanism.
08/01/14   gvn     Update state machine with correct pcell band in case of split intraband case
07/31/14   ndb     Added support for extended EARFCN for LTE
07/30/14   svi     Notch Application based on Tx AFC - Phase 2
07/08/14   gvn     Modify algorithm for split-intraband support 
06/20/14    pv     Added support for TDD PortA/B swap for ASM.
06/18/14   sbm     Added state machine support to determine tx pwr ctrl mode.
06/13/14   svi     Notch Application based on Tx AFC
06/04/14    pv     Added support for FBRx RB Based Freq comp.
05/07/14   svi     Tx Config Timeline optimization
04/20/14   gvn     S-LTE Support
04/17/14    pv     Added support for passing threshold for power tracking method.
04/17/14   svi     PA static NV and code redesign
04/11/14   bsh     Split Band Support
03/27/14   svi     [ASDIV] Added API to get l2l irat state for last irat channel
03/11/14   bsh     Adding NLIC support - add rx freq error in sm
01/28/14   jf      Append RGI_0 script to TX_ON off script, and trigger HW write 
                   RGI_0 at tx_config
02/21/14   cri     Fix null access crash during freq comp
02/20/14   cri     Update QRCT log packet for Bolt to work with FED
02/18/14    pv     Added support for FBRx Gain error processing with Common FBRx Trigger.
02/18/14   cri     Only allocate freq comp during tx tune
02/04/14    pv     Added support for FBRx Gain error processing.
01/08/13   npi     Track current state of Intra-band CA
11/26/13   bsh     Change the get rxlm API name to match other APIs + New APIs for txlm 
11/12/13   bsh     Delete the states related to Rx Dm 
10/26/13   bsh     support for rx fed
09/05/13   bsh     Initial check in

===========================================================================*/

#ifndef RFLTE_STATE_H
#define RFLTE_STATE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "msgr_types.h"
#include "lm_types.h"
#include "rfc_common.h"
#include "rflte_msg.h"
#include "rf_hal_common.h"
#ifdef FEATURE_RF_HAS_QTUNER
#include "rfcommon_atuner_intf.h"
#endif
#include "rflte_mc.h"
#include "rflte_nv.h"
#include "rfdevice_lte_type_defs.h"
#include "rfc_msm_signal_info_ag.h"
#include "rflte_msg.h"
#include "rflm_lte.h"
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#include "rflm_txagc_api.h"
#include "rflte_core_freq_comp.h"
#endif

/* This is needed to implement timer for our HDET/FBRx Read */
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Structure Definitions */
/*--------------------------------------------------------------------------*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY
typedef struct
{
  /* State of TXPL module */
  rflte_core_txpl_mode_type mode;
  uint32                    counter;
  volatile boolean          write_in_progress;
  rflte_core_txpl_pwr_trking_type pwr_corr_mode;

  /* ACQ mode filtering configuration */
  uint16   init_mode_alpha;
  uint16   init_mode_period_ms;
  uint16   init_mode_count;
  uint16   init_mode_exit_wait;

  /* ACQ mode filtering configuration */
  uint16   acq_mode_alpha;
  uint16   acq_mode_period_ms;
  uint16   acq_mode_count;
  uint16   acq_mode_exit_wait;
  int16    acq_mode_resolution;

  /* TRK mode filtering configuration */
  uint16   trk_mode_alpha;
  uint16   trk_mode_period_ms;
  uint16   trk_mode_count;
  uint16   trk_mode_exit_wait;
  int16    trk_mode_resolution;

  /* TX power limits */
  uint16 txpl_sar;
  uint16 txpl_nv;
  uint16 txpl_overheat;
  uint16 txpl_pcmax;
  uint16 txpl_pemax;
  int16 txpl_dtpl;
  int16 txpl_sgsar;
  int16 txpl_coex;
  int16 txpl_coex_wlan_limit;
  int16 txpl_subframe_coex;
  int16 vbatt_backoff;

  /*TDD Config backoff for SAR limit*/
  uint16 tdd_cfg_backoff;

  /* SRS TX power limits */
  uint16 txpl_srs_pcmax;
  uint16 txpl_srs_pemax;
  int16 txpl_srs_dtpl;
  int16 txpl_srs_coex_wlan_limit;

  /* HDET read error */
  uint16 hdet_raw_read;
  int16  hdet_agc;
  int16 hdet_txagc_inst_err;
  float hdet_txagc_filtered_err;

  /* FBRx Gain error */
  int16 fbrx_gain_current_err;
  int16 fbrx_gain_err_inst_vs_txagc_val;
  float fbrx_gain_filtered_err;

  /* HDET read configurations */
  boolean lpm_enabled;
  uint16 pdet_switch_point;
  uint16 hdet_threshhold;

  /* output of MTPL/HDET loop */
  int16 mtpl;
  int16 srs_mtpl;

  /* logging */
  uint8 mpr;
  uint8 ampr;
  uint8 ns_flag;
  uint8 rb_block_start_1;
  uint8 rb_block_start_2;
  uint8 rb_block_num;
  uint8 rb_start_nb_region;
  rflm_lte_mod_type_s mod;

  uint8 srs_mpr;
  uint8 srs_ampr;
  uint8 srs_ns_flag;
  uint8 srs_rb_block_start;
  uint8 srs_rb_block_num;
   /* DAC setpoint override */
  boolean dac_setpoint_override;
  uint16  dac_setpoint;

  /* timer params  */
  /* This is needed for maintaining time intervals
     based on our power tracking timer.
     Power tracking timer handle. Can be used to trigger
     FBRx/HDET/TDET measurement */
  boolean timer_created;
  timer_type pwr_trking_read_timer;

  /* The loop timer for repeating the pwr trking algorithm */
  uint32 loop_period;

  /* Carrier Index, this Structure belongs to */
  uint32 curr_carrier_idx;

  /* Freq comp values for both carrier indices */
  rflte_core_freq_comp_type freq_comp_offsets;

  boolean tx_tune_update; 

} rflte_core_txpl_sm_type;

typedef struct {

  /* Mapping of first sweep RGIs to qsorted linearizer */
  uint8  first_sweep_rgis[RF_LTE_MAX_NUM_PA_STATES][RFCOM_TXLIN_NUM_SEGMENTS];
  uint8  cal_rgis[RF_LTE_MAX_NUM_PA_STATES][RFCOM_TXLIN_NUM_SEGMENTS];
  uint16 pa_bias[RF_LTE_MAX_NUM_PA_STATES][RFCOM_TXLIN_NUM_SEGMENTS];
} rflte_state_lin_mapping_type;

/* Timer call-back Funtion */
void rflte_core_pwr_trking_timer_read_cb(timer_cb_data_type data);
#endif

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_validate_state_input(uint32 carrier_idx, uint32 umid, void* data);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_nb_mc_validate_state_input(uint32 umid, void* data);

/*----------------------------------------------------------------------------*/
void rflte_mc_update_state(uint32 carrier_idx, uint32 umid, void *payload_ptr);

/*----------------------------------------------------------------------------*/
void rflte_nb_mc_update_state(uint32 umid, void *payload_ptr);

/*----------------------------------------------------------------------------*/
rf_lte_state_enum_type rflte_mc_get_state(uint32 carrier_idx);


/*----------------------------------------------------------------------------*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY
rfcom_lte_band_type rflte_state_get_curr_band_tx( uint8 carr_index );
void rflte_state_set_curr_band_tx( uint8 carr_index, rfcom_lte_band_type band_tx);

/*----------------------------------------------------------------------------*/

rf_lte_power_control_mode_type rflte_state_get_power_ctrl_mode(uint32 carrier_idx);
void rflte_state_set_power_ctrl_mode(
uint32 carrier_idx,
rf_lte_power_control_mode_type pwr_ctrl_mode
);

/*----------------------------------------------------------------------------*/
boolean rflte_mc_get_tuner_present(
void
);
/*----------------------------------------------------------------------------*/
void rflte_mc_set_tuner_present(
boolean status
);
/*----------------------------------------------------------------------------*/
#endif
rflte_nv_tbl_type* rflte_mc_get_nv_tbl_ptr(uint32 carrier_idx);
void rflte_mc_set_nv_tbl_ptr(uint32 carrier_idx, rflte_nv_tbl_type* nv_tbl_ptr);
#ifndef FEATURE_RF_LTE_PAGE_ONLY
rflte_core_txpl_ns_type rflte_mc_get_active_ns_flag_based_hw_ctl(uint32 carrier_idx);
void rflte_mc_set_active_ns_flag_based_hw_ctl(uint32 carrier_idx, rflte_core_txpl_ns_type ns_x);
#endif
/*----------------------------------------------------------------------------*/

rfdevice_lte_script_data_type* rflte_state_get_script_data_ptr(uint32 carrier_idx, rflte_mc_carrier_type carrier_type);

/*----------------------------------------------------------------------------*/

boolean rflte_mc_get_init_status(void);

/*----------------------------------------------------------------------------*/

void rflte_mc_set_init_status(boolean flag);

/*----------------------------------------------------------------------------*/

void rflte_state_set_default_asm_port(void);

/*----------------------------------------------------------------------------*/

void rflte_state_set_tx_rx_asm_port(uint8 band, uint8 port_state);

/*----------------------------------------------------------------------------*/

uint8 rflte_state_get_tx_rx_asm_port(uint32 band_idx);

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

rfcom_lte_band_type rflte_state_get_curr_band_rx(uint8 cell_idx, rflte_mc_carrier_type carrier_type);

/*----------------------------------------------------------------------------*/

void rflte_state_set_curr_band_rx(uint8 cell_idx, rfcom_lte_band_type rx_band, rflte_mc_carrier_type carrier_type);

/*----------------------------------------------------------------------------*/

void rflte_state_init(boolean nv_init_done);

/*----------------------------------------------------------------------------*/

void rflte_state_reset_path_state(uint32 cell_index);

rfcom_device_enum_type rflte_state_get_prx_rfm_device(uint32 carrier_index);
void rflte_state_set_prx_rfm_device(uint32 carrier_index, rfcom_device_enum_type rfm_device);

rfcom_lte_earfcn_type rflte_state_get_curr_chan_rx(uint32 cell_idx);

void rflte_state_set_curr_chan_rx(uint32 cell_index, rfcom_lte_earfcn_type rx_chan);

#ifndef FEATURE_RF_LTE_PAGE_ONLY

/*----------------------------------------------------------------------------*/

void rflte_state_reset_l2l_irat_state(void);

/*----------------------------------------------------------------------------*/
rfcom_device_enum_type rflte_state_get_tx_rfm_device(uint32 carrier_index);
void rflte_state_set_tx_rfm_device(uint32 carrier_index, rfcom_device_enum_type rfm_device);
/*----------------------------------------------------------------------------*/

rfcom_lte_earfcn_type rflte_state_get_curr_chan_tx(uint32 cell_idx);

void rflte_state_set_curr_chan_tx(uint32 cell_index, rfcom_lte_earfcn_type tx_chan);

int8 rflte_state_get_ul_carrier_freq_offset(uint32 cell_index);

void rflte_state_set_ul_carrier_freq_offset(uint32 cell_index, int8 ul_carrier_freq_offset);
#endif

/*----------------------------------------------------------------------------*/
void rflte_state_set_nv_status(boolean status);

/*----------------------------------------------------------------------------*/
boolean rflte_state_get_nv_status(void);

#ifndef FEATURE_RF_LTE_PAGE_ONLY
/*----------------------------------------------------------------------------*/

void rflte_state_set_ftm_ns_mode(boolean flag);
boolean rflte_state_get_ftm_ns_mode(void);
/*----------------------------------------------------------------------------*/

void rflte_state_set_ftm_mode(boolean flag);
boolean rflte_state_get_ftm_mode(void);
/*----------------------------------------------------------------------------*/
void rflte_state_set_ftm_ns_mode(boolean flag);
boolean rflte_state_get_ftm_ns_mode(void);
/*----------------------------------------------------------------------------*/
#endif
int32 rflte_state_get_rfc_msm_timing(rfc_timing_param_type timing_index, boolean is_internal);

/*----------------------------------------------------------------------------*/
void rflte_state_set_tdd_mode(boolean tdd_mode);
boolean rflte_state_get_tdd_mode(void);

/*----------------------------------------------------------------------------*/
boolean rflte_state_get_hd_fdd_mode(void);
void rflte_state_set_hd_fdd_mode(boolean hd_fdd_mode);

/*----------------------------------------------------------------------------*/
boolean rflte_state_get_lte_nb_mode(void);
void rflte_state_set_lte_nb_mode(boolean lte_nb_mode);

/*----------------------------------------------------------------------------*/
rfcom_lte_bw_type rflte_state_get_bw_rx(uint32 carrier_index);
void rflte_state_set_bw_rx(uint32 carrier_index, rfcom_lte_bw_type bw_rx);
rfcom_lte_bw_type rflte_state_get_sys_bw_rx(uint32 carrier_index);
void rflte_state_set_sys_bw_rx(uint32 carrier_index, rfcom_lte_bw_type sys_bw_rx);

/*----------------------------------------------------------------------------*/
boolean rflte_state_get_wake_up_tq_enable_flag(void);

void rflte_state_set_wake_up_tq_enable_flag(boolean wake_up_tq_flag);
/*----------------------------------------------------------------------------*/
void rflte_state_set_rfc_msm_timing_info(rfc_timing_info_type* timing_info_ptr, boolean is_internal);
lm_handle_type rflte_state_get_rxlm_buffer_idx(rfcom_device_enum_type device, uint32 carrier_idx);
/*----------------------------------------------------------------------------*/

void rflte_state_set_rxlm_buffer_idx(uint32 carrier_idx, rfcom_device_enum_type device, lm_handle_type rxlm_buffer_idx);

/*----------------------------------------------------------------------------*/
void rflte_state_set_rx_freq_error(uint32 carrier_idx, int32 rx_freq_error_in_hz);
/*----------------------------------------------------------------------------*/
int32 rflte_state_get_rx_freq_error(uint32 carrier_idx);

/*----------------------------------------------------------------------------*/
/* This API is to set the path override index for a given carrier */
void rflte_state_set_path_override_index(uint32 carrier_idx, uint8 path_override_index, rflte_mc_carrier_type curr_carr_type);

/*----------------------------------------------------------------------------*/

/* This API  returns the current path override index used in the system */
uint8 rflte_state_get_path_override_index(uint32 carrier_idx, rflte_mc_carrier_type curr_carr_type);

/*----------------------------------------------------------------------------*/
rf_lte_state_enum_type rflte_state_get_rx_ta_flag_status(uint32 carrier_index);

#ifndef FEATURE_RF_LTE_PAGE_ONLY
boolean rflte_state_get_xo_cal_mode(void);
void rflte_state_set_xo_cal_mode(boolean xo_cal_mode);
/*----------------------------------------------------------------------------*/
void rflte_state_set_gnss_blanking_status(boolean gnss_status);
boolean rflte_state_get_gnss_blanking_status(void);
/*----------------------------------------------------------------------------*/
boolean rflte_state_get_lin_vs_temp_vs_freq_status(uint32 carrier_idx);
void rflte_state_set_lin_vs_temp_vs_freq_status(uint32 carrier_idx, boolean status);

/*----------------------------------------------------------------------------*/
rfcom_lte_bw_type rflte_state_get_bw_tx(uint32 carrier_index);
rfcom_lte_bw_type rflte_state_get_sys_bw_tx(uint32 carrier_index);
void rflte_state_set_bw_tx(uint32 carrier_index, rfcom_lte_bw_type bw_tx);
void rflte_state_set_sys_bw_tx(uint32 carrier_index, rfcom_lte_bw_type sys_bw_tx);
/*----------------------------------------------------------------------------*/
void rflte_mc_update_state(uint32 carrier_idx, uint32 umid, void *payload_ptr);
#endif
/*----------------------------------------------------------------------------*/
rf_lte_state_enum_type rflte_state_get_rf_state(uint32 carrier_index);
void rflte_state_set_rf_state(uint32 carrier_index, rf_lte_state_enum_type rf_state);
#ifndef FEATURE_RF_LTE_PAGE_ONLY
/*----------------------------------------------------------------------------*/

uint32 rflte_state_get_l2l_irat_state_last_irat_band(void);

void rflte_state_set_l2l_irat_state_last_irat_band(uint32 irat_band);

/*----------------------------------------------------------------------------*/
boolean rflte_state_get_et_is_in_use_status(uint32 carrier_index);

void rflte_state_set_et_is_in_use_status(uint32 carrier_index, boolean et_flag);

/*----------------------------------------------------------------------------*/
boolean rflte_state_get_l2l_irat_state_lte_mc_in_suspended_rx_state(void);

void rflte_state_set_l2l_irat_state_lte_mc_in_suspended_rx_state(boolean irat_rx_state);

/*----------------------------------------------------------------------------*/
const int8* rflte_state_get_bb_device_filter_coeff(void);

void rflte_state_set_bb_device_filter_coeff(const int8* filter_coeff);

/*----------------------------------------------------------------------------*/
void rflte_state_set_l2l_irat_state_last_irat_channel(uint32 channel);

/*----------------------------------------------------------------------------*/
uint32 rflte_state_get_l2l_irat_state_last_irat_channel( void );
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void rflte_state_set_txlm_buffer_idx(uint32 carrier_idx, lm_handle_type txlm_buffer_idx);

boolean rflte_state_get_txlm_buffer_idx(uint32 carrier_idx, lm_handle_type *txlm_buffer_idx);
/*----------------------------------------------------------------------------*/

rfc_coex_filter_status_type rflte_state_get_gsm_concurrency_path_info( uint32 carrier_idx );
/*----------------------------------------------------------------------------*/
void rflte_state_reset_lin_maps(void);
/*----------------------------------------------------------------------------*/
void rflte_state_update_first_sweep_lin_maps(uint8 pa_state, tx_linearizer_table_type *lin_data);
/*----------------------------------------------------------------------------*/
void rflte_state_update_cal_lin_maps(uint8 pa_state, tx_linearizer_table_type *lin_data, int16 bias_off);
/*----------------------------------------------------------------------------*/
uint8 rflte_state_get_first_sweep_lin_mapping(uint8 pa_state, uint8 lin_index);
/*----------------------------------------------------------------------------*/
uint8 rflte_state_get_cal_lin_mapping(uint8 pa_state, uint8 lin_index);
/*----------------------------------------------------------------------------*/
uint16 rflte_state_get_cal_bias_mapping(uint8 pa_state, uint8 lin_index);
/*----------------------------------------------------------------------------*/
void rflte_state_set_active_pa_state(uint32 carrier_idx, uint8 pa_state_index, boolean active_status);
/*----------------------------------------------------------------------------*/
boolean rflte_state_get_pa_state_active_status(uint32 carrier_idx, uint8 pa_state_index);

/*----------------------------------------------------------------------------*/

rf_lte_update_tx_fed_static_dm_type 
rflte_state_get_tx_fed_static_dm_update
(
  uint32                                carrier_idx
);

/*----------------------------------------------------------------------------*/
void 
rflte_state_config_tx_fed_static_dm_update
(
  uint32                                carrier_idx,
  rf_lte_update_tx_fed_static_dm_type   config_state
);
#endif
/*----------------------------------------------------------------------------*/
boolean rflte_state_get_olta_enable_status(uint32 carrier_index);
/*----------------------------------------------------------------------------*/
void rflte_state_set_gsm_concurrency_path_info(uint32 carrier_idx, rfc_coex_filter_status_type gsm_concurrency_path);
/*----------------------------------------------------------------------------*/
rflte_mc_notch_programmed_config_type rflte_state_get_prx_notch_config (uint32 carrier_idx );
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_notch_config (uint32 carrier_idx, rflte_mc_notch_programmed_config_type notch_config );
/*----------------------------------------------------------------------------*/

uint64 rflte_state_get_prx_dl_lo_rx_hz (uint32 carrier_idx );
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_dl_lo_rx_hz (uint32 carrier_idx, uint64 dl_lo_rx_hz );
/*----------------------------------------------------------------------------*/

uint32 rflte_state_get_prx_num_notches (uint32 carrier_idx );
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_num_notches (uint32 carrier_idx, uint32 prx_num_notches );
/*----------------------------------------------------------------------------*/

void rflte_state_increase_prx_num_notches (uint32 carrier_idx );
/*----------------------------------------------------------------------------*/

uint32 rflte_state_get_prx_notch_application_freq (uint32 carrier_idx, uint32 notch_index );
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_notch_application_freq (uint32 carrier_idx, uint32 notch_application_freq, uint32 notch_index );
/*----------------------------------------------------------------------------*/

uint32 rflte_state_get_prx_notch_depth (uint32 carrier_idx, uint32 notch_index );
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_notch_depth (uint32 carrier_idx, uint32 notch_depth, uint32 notch_index );
/*----------------------------------------------------------------------------*/

uint32 rflte_state_get_prx_spur_threshold (uint32 carrier_idx, uint32 notch_index );
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_spur_threshold (uint32 carrier_idx, uint32 spur_thresh, uint32 notch_index );
/*----------------------------------------------------------------------------*/

int32 rflte_state_get_afc_rx_freq_error_in_hz (uint32 carrier_idx );
/*----------------------------------------------------------------------------*/

void rflte_state_set_afc_rx_freq_error_in_hz (uint32 carrier_idx, int32 error_in_hz );
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void rflte_state_set_calibration_alt_path(rfcom_device_enum_type rfm_device, uint8 alt_path);
/*----------------------------------------------------------------------------*/

uint8 rflte_state_get_calibration_alt_path(rfcom_device_enum_type rfm_device);
/*----------------------------------------------------------------------------*/

void rflte_state_set_prx_spur_type (uint32 carrier_idx, rflte_mc_spur_mitigated_type spur_type, uint32 notch_index );
/*----------------------------------------------------------------------------*/

rflte_mc_spur_mitigated_type rflte_state_get_prx_spur_type (uint32 carrier_idx, uint32 notch_index );
/*----------------------------------------------------------------------------*/

void rflte_state_set_alternate_path ( uint32 carrier_idx, rflte_mc_carrier_type carrier_type, uint8 alternate_path );
/*----------------------------------------------------------------------------*/

uint8 rflte_state_get_alternate_path ( uint32 carrier_idx, rflte_mc_carrier_type carrier_type );
/*----------------------------------------------------------------------------*/

/*!
  @brief
  Get TQ flag from state structure 

  @details

  @param
  carrier_index
*/
boolean rflte_state_get_tq_allocated_flag(uint32 carrier_idx);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set TQ flag in state structure based on whether it was allocated/dealloacted 

  @details

  @param
  carrier_index, flag
*/
void rflte_state_set_tq_allocated_flag(uint32 carrier_idx, boolean flag);

/*----------------------------------------------------------------------------*/

void rflte_state_set_rx_nb_region_center_frequency(uint32 carrier_idx, rflm_lte_nb_region_type nb_region, uint32 frequency);

/*----------------------------------------------------------------------------*/

uint32 rflte_state_get_rx_nb_region_center_frequency(uint32 carrier_idx, rflm_lte_nb_region_type nb_region);

/*----------------------------------------------------------------------------*/

void rflte_state_set_nb_rx_lif_offset(uint32 carrier_idx, int32 lif_offset_hz);

/*---------------------------------------------------------------------------*/

int32 rflte_state_get_nb_rx_lif_offset(uint32 carrier_idx);

/*----------------------------------------------------------------------------*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY


void rflte_state_set_ue_power_class(rflte_core_txpl_power_class_type uepower_class);

rflte_core_txpl_power_class_type rflte_state_get_ue_power_class(void);
/*----------------------------------------------------------------------------*/

void rflte_state_set_nb_anchor_nonanchor_chan_tx(uint32 cell_index, rfcom_lte_earfcn_type anchor_tx,rfcom_lte_earfcn_type nonanchor_tx);

#endif

void rflte_state_set_nb_anchor_nonanchor_chan_rx(uint32 cell_index, rfcom_lte_earfcn_type anchor_rx,rfcom_lte_earfcn_type nonanchor_rx);
/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_state_get_nb_anchor_nonanchor_chan_rx(uint32 cell_index,rflte_mc_nb_carrier_type_e nb_carrier);
/*----------------------------------------------------------------------------*/

void rflte_state_set_nb_rx_raster_offset(uint32 carrier_idx,rflte_mc_nb_carrier_type_e nb_carrier,int32 raster_offset);
/*----------------------------------------------------------------------------*/

int32 rflte_state_get_nb_rx_raster_offset(uint32 carrier_idx,rflte_mc_nb_carrier_type_e nb_carrier);

void rflte_state_set_band_duplex_spacing_flag(boolean flag);

boolean rflte_state_is_non_3gpp_spacing();

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_STATE_H */

