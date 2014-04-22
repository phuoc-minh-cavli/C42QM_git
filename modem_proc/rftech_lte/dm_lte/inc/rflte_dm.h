#ifndef RFLTE_DM_H
#define RFLTE_DM_H

/*!


*/

/*===========================================================================

  Copyright (c) 2009 - 2020 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rftech_lte.mpss/2.10/dm_lte/inc/rflte_dm.h#1 $

when       who     what, where, why 
-------------------------------------------------------------------------------- 
04/09/20   gk      Fix for default lna state stuck issue
06/07/19   dh      Added support for MAAT LITE feature
04/15/19   as      Non Anchor Tx retune changes
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
02/25/19   dh      Added support for thermal mitigation
02/06/19   as      Non Anchor Rx retune support
02/04/19   as      ADC/DAC control in rfsw
10/25/18   sk      TRACK B Bringup changes
02/15/18   sk      [Phase1] Chiltepin LTE memory optimization
10/16/17   cv      Increasing the channel variable size and addition of static NV
01/11/17   as      Tx PLL fast lock changes for LTE CatM1 
12/21/16   qzh     Add NB1 support
12/04/16   as/qzh  Add AFC and ul hop merging related changes.
11/11/16   qzh     Add CATM1 ul hopping related changes
10/26/16   as      Update in Rx and Tx NPLER settings for CAT-M1
09/15/16   as      storing hd_fdd flag into fed static
07/25/16   as      CATM1 Rx NB Hopping changes
09/30/15   bir     Added support for tx max pwr based EARFCN NV
09/28/15   bir     Added support for PA biass offset w.r.t EARFCN
01/13/15   stw     Intraband CA PCC + SCC2 support
01/12/15   qzh     Add num_pa_range_grfc in fed to indicate the number of PA range GRFC
01/07/15   ska     Added fix to get default offsets for gain states
11/20/14   gvn     Fix alt path usage, pass alt path to apis needing it
11/19/14   svi     Do not populate the intra-band freq comp during IRAT tune-away
11/16/14   gvn     Fix Crash due to ext_rffe_buffer overflow
10/21/14   gvn     Re-tune Tx and Rx in case split band changes on intra scell add or delete
10/08/14   kab     Support to Set DM with given LO Offset from Center Freq
09/29/14   kab     Update DM with Rx LO spacing from Carrier Center Frequency
09/17/14   bsh     support rx enable/disable
09/16/14   npi     Update QFE PA's SM's on wakeup
09/11/14   vb      RSB/IQMC per LNA gain state support
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
08/15/14    st     CA Alt Path Reconfiguration Script Creation and Event Handling
08/14/14    pv     Added support for PA swp adjust mechanism.
08/14/14    pv     Added support for FBRx + GPS concurrency based scripts.
08/07/14    pv     Added support for TDD DRx eLNA ASM programming during Rx burst.
08/06/14   ndb     Added support for extended EARFCN for LTE
08/06/14   svi     Fix not to destroy Tx Lin State machine during SCELL exit mode
08/01/14   sk      Remove Common Txagc Seq creation in non-cal mode for LTE
07/30/14   gvn     Intra-band fixes: get sca lna settings, use correct path idx in intra freq comp
07/18/14   jf      Return TX power in linear region to ML1
07/17/14   pv      Added support for call-back API for spur-mitigation based re-tune in CA mode.
07/15/14   bsh     Remove tech DM dependency for irat and rxagc
07/14/14   bsh     Remove cell deletion from Rx DM during exit
07/14/14   sbm     Added support for therm read-done flag.
07/02/14   jf      AsDiv support
06/26/14   bar     Added support for therm read.
06/25/14   cri     Reduce FTM RF log packet size
06/20/14    pv     Added support for TDD PortA/B swap for ASM.
06/18/14   sbm     Added support for WTR HDET
06/18/14   cri     Reload DPD tables after DLS wakeup
06/04/14    pv     Added support for FBRx RB Based Freq comp.
05/15/14   gvn     Add intra-band support for overlapping split bands
05/13/14   jf      Add CL/AOL support
05/07/14   svi     Tx Config Timeline optimization
05/01/14   bsh     Remove obsolete API
04/24/14    jf     Add script populate function
04/24/14   cri     wmore clipper enablement and nv support
04/23/14   bsh     Rx scripts moved from tech DM to handle pool buffer
04/17/14   svi     PA static NV and code redesign
04/11/14   npi     Clean up common device pointer usage
03/20/14   bsh     Add intra band ca lna offests/scripts and dm_rx cleanup
03/16/14    pv     Added support for new interface for common DM script updates.
03/14/14   bsh     NlIC changes: code clean up 
03/11/14   bsh     Adding NLIC support
03/03/14    pv     Added support for Trigger power level for FBRx.
02/24/14    pv     Added support for FBRx Gain error processing with MTPL.
01/28/14   jf      Append RGI_0 script to TX_ON off script, and trigger HW write 
                   RGI_0 at tx_config 
02/24/14    pv     Added support for FBRx Gain error processing with MTPL.
02/20/14   bsh     Support FTM RF Cal verification on BOLT
02/13/14    pv     Added support for FBRx Gain error processing.
02/12/14   jf      Add xpt logging for ENH TXAGC
02/06/14   bsh     Freq comp support for RxAGC
02/05/14   cri     Freq comp optimization for DM population
01/30/14   kab     AFC RF Event and AFC Gating
01/24/14   cri     DRIF/IREF/LUT enablement
01/08/13   bsh     Fix compiler warning in RFLM
12/17/13   can     Enable LTE TPL.
12/10/13   svi     RFLM FED TxAGC Logging checkin 1
12/02/13   svi     Update FED if Band is FDD or TDD
11/12/13   bsh     Use the new CMD to check cell validity in DM 
11/08/13   svi     FED DM population needs carrier_idx
11/07/13   svi     Added FED Tx DM population support
11/06/13   gvn     Remove update_tx_on_off_script, 
                   use ccs events instead for tx on off 
11/06/13   svi     Added FED Rx DM population support
10/31/13   svi     Fed Shim Layer Checkin - 2
10/26/13   bsh     support for rx fed
08/05/09   can     Initial version. 


===========================================================================*/

/*===========================================================================

                  INCLUDE FILES

===========================================================================*/
#include "rfdevice_lte_type_defs.h"
#include "rflm_lte.h"
#include "rfc_card.h"
#include "comdef.h"
#include "rflte_nv.h"
#include "comdef.h"
#include "rf_lte_mdsp_rf_event.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rfcommon_msm.h"
#include "rflte_mc.h"
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#include "ftm_lte_log.h"
#include "rflte_core_freq_comp.h"
#include "rflte_nb_core.h"
#endif

#define RFLTE_DM_RX_DEFAULT_CELL_ID 0
#define RFLTE_DM_RX_DEFAULT_NUM_CELLS 2
#define RFLTE_DM_RX_INVALID_CELL_ID -1
#define RFLTE_DM_RX_INTRA_BAND_CELL_ID 1
#define RFLTE_DM_RX_ALT_PATH_CELL_ID 2



#ifdef FEATURE_RF_HAS_QTUNER

/*----------------------------------------------------------------------------*/

/*!
  @brief
  This module contains Tuner Related RFLTE Enums and Macros:
  @{
*/

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Enumeration indicating whether Tuner Operation is RX only or RxTx
*/
typedef enum
{

  RF_LTE_TUNER_UPDATE_TUNE_CODE_RXTX    = 0,  /*!< Update Tune Code for RXTX */
  RF_LTE_TUNER_UPDATE_TUNE_CODE_RX_ONLY = 1,  /*!< Update Tune Code for RX Only */
  RF_LTE_TUNER_UPDATE_TUNE_CODE_INVALID,
  RF_LTE_TUNER_UPDATE_TUNE_CODE_MAX     = RF_LTE_TUNER_UPDATE_TUNE_CODE_INVALID,
  
}rf_lte_tuner_update_tune_code_rf_state_t;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Macro Indicating the script timing for Chain 0 RXTX Tune Code
*/
#define RF_LTE_TUNER_UPDATE_TUNE_CODE_RXTX_CHAIN_0_SCRIPT_TIMING  0

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Macro Indicating the script timing for Chain 1 RXTX Tune Code
*/
#define RF_LTE_TUNER_UPDATE_TUNE_CODE_RXTX_CHAIN_1_SCRIPT_TIMING  0

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Macro Indicating the script timing for Chain 0 RX Tune Code
*/
#define RF_LTE_TUNER_UPDATE_TUNE_CODE_RX_CHAIN_0_SCRIPT_TIMING    0

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Macro Indicating the script timing for Chain 1 RX Tune Code
*/
#define RF_LTE_TUNER_UPDATE_TUNE_CODE_RX_CHAIN_1_SCRIPT_TIMING    0

/*----------------------------------------------------------------------------*/

/*! @} */

#endif /* FEATURE_RF_HAS_QTUNER */


typedef struct
{
  /* Script Index */
  uint8 script_index;

  /* FBRx Gain State */
  uint8 fbrx_gain_state;

}rflte_dm_write_event_buffer_intf_type_s;

typedef struct
{
    boolean buffer_allocated;
    uint32*  buff_ptr;
} rflte_dm_rx_buff_alloc_info;

#ifndef FEATURE_RF_LTE_PAGE_ONLY
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure has pointers to store the Tx AGC Lin Data populated to
  the DM
  
*/
typedef struct
{

  /*!> Tx Lin Data for all sweeps for each PA state */
  tx_linearizer_table_data_type* rflte_tx_lin_data[RF_LTE_MAX_NUM_PA_STATES];

  /*!> Tx Lin Data for first sweep for each PA state */  
  tx_linearizer_table_data_type* rflte_tx_lin_data_first_sweep[RF_LTE_MAX_NUM_PA_STATES];

  /*!> Lin Table Per PA state */
  tx_linearizer_table_type *tx_table[RF_LTE_MAX_NUM_PA_STATES];

  /*!> Lin Table for first sweep Per PA state */
  tx_linearizer_table_type *tx_table_fs[RF_LTE_MAX_NUM_PA_STATES];

} rflte_txagc_lin_state_type;

extern rflte_txagc_lin_state_type txagc_lin_state;

int32 rflte_dm_pass_gnss_grfc
(
  uint32 carrier_idx, 
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfdevice_lte_script_data_type* script_data_ptr,
  rfc_logic_type tx_on_off
);
							   
void rf_lte_dm_rf_event_data_fix_up_txplloffset
(
  rf_hal_bus_rffe_type *tx_pll_rffe_script,
  uint8 tx_pll_num_trans,
  uint32 *tx_pll_offset_mtpl_bytes,
  uint32 shared_mem_max_size_bytes
);

void rflte_dm_init_tx 
(
  uint32 carrier_idx
);

#endif							   
boolean rf_lte_dm_rf_event_start_push_commit_to_memory
(
  uint32 event_id, 
  uint8 num_scripts, 
  const rf_hal_bus_resource_script_type **script_ptr_list, 
  void * shared_mem_buf_ptr, 
  uint32 shared_mem_max_size_bytes, 
  boolean is_issue_seq,
  uint8 max_tasks_allocated,
  boolean use_timer_conversion,
  int16 time_offset,
  uint8 event_priority
);		

void rf_lte_dm_rf_event_token_update
(
  uint8 num_scripts,
  const rf_hal_bus_resource_script_type **script_ptr_list, 
  rflm_lte_event_token_info *event_token_info
);	

void rflte_dm_write_event_buffer
(
  uint32 carrier_idx,
  rf_buffer_intf *script,  
  rflm_lte_rf_script_id_type_e script_id, 
  rflte_dm_write_event_buffer_intf_type_s intf_param
);

void rflte_dm_port_switch_transition_detected(void);

//void rflte_dm_clear_tx_pll_offset_hal_buffer(void);

void rflte_dm_reset_event_buffer_index
(
  uint32 carrier_idx
);
#ifndef FEATURE_RF_LTE_PAGE_ONLY
void rflte_dm_tx_init_static_config
(
  uint32 carrier_idx, 
  rflte_nv_tbl_type * rflte_nv_tbl_ptr,  
  rfcom_lte_earfcn_type tx_channel
);

boolean rflte_dm_tx_pa_swp_adjust(uint32 carrier_idx,rflte_nv_tbl_type *rflte_nv_tbl_ptr);

boolean rflte_dm_tx_pa_swp_restore(uint32 carrier_idx,rflte_nv_tbl_type *rflte_nv_tbl_ptr);

void rflte_dm_txagc_populate_txlin_data
(
  uint32 carrier_idx,
  rfcom_device_enum_type device,
  rf_path_enum_type path,
  rfcom_lte_band_type band,
  rfcom_lte_bw_type bw,
  uint32 tx_freq,
  rflte_nv_tbl_type *rflte_nv_tbl_ptr
);

void rflte_dm_adjust_iq_scaling_based_on_bw
(
  uint32 carrier_idx, 
  rfcom_lte_bw_type bw
);

void rflte_dm_update_default_baseband_gains
(
   uint32 carrier_idx, 
   rflte_nv_tbl_type *rflte_nv_tbl_ptr,
   uint32 default_iq_gain
);

void rflte_dm_update_default_clipper_settings
(
   uint32 carrier_idx, 
   rflte_nv_tbl_type *rflte_nv_tbl_ptr
);

void rflte_dm_update_xpt_mode
(
  uint32 carrier_idx,                             
  tx_linearizer_aux_data_type aux_lin_data,
  uint8 pa_state_index
);

void rflte_dm_update_tx_pll_offset_event
(
   uint32 carrier_idx,
   rfm_device_enum_type rfm_device,
   rfcom_lte_band_type band,
   uint32 tx_freq,
   rfcom_lte_bw_type bw,
   rfcom_lte_bw_type tx_sys_bw,
   rfdevice_lte_script_data_type *script_data_ptr
);

void rflte_dm_update_tx_on_wr_on_event
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfdevice_lte_script_data_type* script_data_ptr,
  boolean gnss_blanking_enabled
);

void rflte_dm_update_tx_en_wr_on_event
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfcom_lte_bw_type bw, 
  uint32 freq_khz,
  rfdevice_lte_script_data_type *script_data_ptr
);

void rflte_dm_update_tx_on_wr_off_event
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfdevice_lte_script_data_type* script_data_ptr,
  boolean gnss_blanking_enabled
);

void rflte_dm_update_tx_en_wr_off_event
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfcom_lte_bw_type bw,
  uint32 freq_khz,
  rfdevice_lte_script_data_type *script_data_ptr
);

void rflte_dm_update_tdd_asm_tx_event
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfdevice_lte_script_data_type* script_data_ptr
);

void rflte_dm_update_tdd_asm_tx_script
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfdevice_lte_script_data_type* script_data_ptr,
  uint8 script_index
);
#endif
void rflte_dm_update_tdd_asm_rx_script
(
  rfm_device_enum_type prx_device,
  rfm_device_enum_type drx_device,
  rfcom_lte_band_type prx_band,
  rfcom_lte_band_type drx_band,
  uint32 carrier_idx,
  rfdevice_lte_script_data_type *script_data_ptr,
  uint8 script_index
);

boolean rflte_dm_asm_port_update
(
  uint32 carrier_idx,
  rfa_rf_lte_rx_tx_chain_type rx_tx_chain,  
  uint8 tx_port_state,
  uint8 rx_port_state,  
  boolean trigger_immediate
);

void rflte_dm_repopulate_tdd_asm_rx_script
(
  rfm_device_enum_type device,
   rfcom_lte_band_type band,
  uint32 carrier_idx,
  rfdevice_lte_script_data_type *script_data_ptr
);

#if 0
void rflte_dm_copy_script_to_fed_static
(
  rflm_lte_rf_script_type *rflm_script, 
  rflm_lte_fed_static_s *fed_static, 
  rflm_lte_rf_script_id_type_e script_id, 
  uint8 script_index
);
#endif

#ifndef FEATURE_RF_LTE_PAGE_ONLY
void rflte_dm_update_rf_events_timing
(
  uint32 carrier_idx,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfcom_lte_bw_type bw
);

void rflte_dm_update_pa_script
(
  uint32 carrier_idx,
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_bw_type bw,
  rflte_nv_tbl_type *rflte_nv_tbl_ptr,
  rfdevice_lte_script_data_type *script_data_ptr,
  boolean update_scripts_to_dm,
  uint8 port_index
);

void rflte_dm_update_papm_on_event
(
    uint32 carrier_idx,
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfcom_lte_bw_type bw,
    rfdevice_lte_script_data_type* script_data_ptr
);

void rflte_dm_update_papm_standby_event
(
    uint32 carrier_idx,
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr
);

void rflte_dm_update_qpoet_script

(
   uint32 carrier_idx,
   rfm_device_enum_type rfm_device,
   rfcom_lte_band_type band,
   rfdevice_lte_script_data_type *script_data_ptr
);

void rflte_dm_oride_txagc_output_power
(
   uint32 carrier_idx,
   boolean pwr_oride_enable,
   int32 counts_pwr_oride_val
);


void 
rflte_dm_update_txpll_afc_script
(
uint32                           cell_index,
rf_hal_bus_resource_type         script_type,
rfdevice_lte_script_data_type   *afc_script_buffer,
int32 rx_freq_error_in_hz
);

#endif
void rflte_dm_update_wtr_wxe
(
   uint32 carrier_idx,
   rfm_device_enum_type rfm_device,
   rfcom_lte_band_type band,
   rfcom_lte_bw_type bw,
   uint32 freq_khz,   
   rfdevice_lte_script_data_type *script_data_ptr
);

void rflte_dm_ftm_rf_enabled
(
   boolean enable
);

void rflte_dm_cal_mode_enabled
(
   boolean enable
);

void 
rflte_dm_update_rx_on_off_scripts(uint32                            carrier_idx,
                                  rfm_device_enum_type              rfm_device,
                                  rfcom_lte_band_type               band,
                                  rfdevice_lte_script_data_type*    script_data_ptr,
                                  lm_handle_type                    handle_id,
                                  boolean                           on_off_flag);

void rflte_dm_update_rx_enable_disable_scripts( rflte_mc_carrier_info_type        carrier_info,
                                                rfcom_lte_band_type               band,
                                                rfdevice_lte_script_data_type     *script_data_ptr,
                                                lm_handle_type                    handle_id,
                                                boolean         on_off_flag,
	                                            boolean rxagc_trigger_flag);



void rflte_dm_rxagc_band_config(rflte_mc_carrier_info_type carrier_info,
                                rfcom_lte_band_type rf_band,
                                rflte_nv_tbl_type *lte_nv_tbl_ptr,
                                boolean intra_band_ca_active,
                                lm_handle_type rxlm_buffer_idx,
                                rfcom_lte_earfcn_type curr_rx_chan,
                                rfcom_lte_bw_type rx_bw,
                                boolean is_irat_mode,
                                uint8 alt_path );

void rflte_dm_rxagc_alt_path_config(rflte_mc_carrier_info_type carrier_info,
  rfcom_lte_band_type rf_band,
  rflte_nv_tbl_type *lte_nv_tbl_ptr,
  boolean alt_path_ca_active,
  boolean intra_band_ca_active,
  lm_handle_type rxlm_buffer_idx,
                                    rfcom_lte_earfcn_type curr_rx_chan,
                                    uint8 alt_path,
                                    rfcom_lte_bw_type rx_bw);


boolean rflte_dm_rxagc_band_config_per_cell(uint32 cell_id, rflte_mc_carrier_info_type carrier_info,
                                            rfcom_lte_band_type rf_band, rflte_nv_tbl_type *lte_nv_tbl_ptr,
                                            lm_handle_type rxlm_buffer_idx, rfcom_lte_earfcn_type curr_rx_chan, boolean intra_band_ca_active,
                                            boolean is_irat_mode,
                                            uint8 alt_path,
                                            rfcom_lte_bw_type rfcom_bw);


boolean rflte_dm_rx_add_cells_to_dm( lm_handle_type rxlm_buffer_idx, 
                                     cell_id_t* cell_id ,
                                     uint8 num_cells_to_add);

void rflte_dm_rx_delete_cells_from_dm(rflte_mc_carrier_info_type carrier_info);

boolean rflte_dm_populate_rsb_coeff( rfcom_device_enum_type device,
                                     lm_handle_type rxlm_buffer_idx,
                                     rfcom_lte_band_type rf_band,
                                     rfcom_lte_earfcn_type curr_rx_chan,
                                     rfcom_lte_bw_type rx_bw,
                                     boolean intra_band_ca_active,
                                     uint16 therm_val,
                                     boolean populate_default_rsb );

boolean rflte_dm_repopulate_rx_gain_offsets(uint32 cell_id, 
                                            rflte_mc_carrier_info_type carrier_info,
                                            lm_handle_type rxlm_buffer_idx,
                                            rfdevice_lte_get_rx_gain_settings_type   rx_gain_settings,
                                            rfcom_lte_earfcn_type curr_rx_chan,
                                            boolean intra_band_ca_active,
                                            rflte_nv_tbl_type *lte_nv_tbl_ptr,
                                            rfcom_lte_band_type rf_band);


void 
rflte_dm_update_rxpll_afc_script
(
uint32                           cell_index,
lte_LL1_ul_txpll_script_array_t *rxpll_script_buf_ptr
);

uint32 rflte_dm_get_path_from_device(rfm_device_enum_type device,
                                       uint32 carrier_idx);

void 
rflte_dm_intra_band_ca_active
(
boolean            active_flag
);

boolean
rflte_dm_get_intra_band_ca_active
(
  void
);

int16 
rflte_dm_get_inband_rxagc_val
(
rfm_device_enum_type device, 
uint32 carrier_idx
);

void 
rflte_dm_set_forced_rx_gain_state
(
rfm_device_enum_type                 device, 
uint8                                rx_gain_force_state, 
int16                                expected_agc_cal, 
uint32                               carrier_idx
);

void 
rflte_dm_set_rx_gain_force
(
uint32                       carrier_idx,
rfm_device_enum_type         device
);

uint8 
rflte_dm_get_rx_gain_state
(
rf_path_enum_type           path,
uint32                      carrier_idx
);

void 
rflte_dm_set_delta_rx_gain_offset
(
rfm_device_enum_type device, 
uint8 index, 
int16 delta_rx_gain_offset, 
uint32 carrier_idx
);

void 
rflte_dm_set_rx_gain_rise_fall_thrshlds
(
rfm_device_enum_type             device, 
uint8                            index, 
int16                            rise_thrshlds, 
uint32                           carrier_idx
);

void
rflte_dm_update_rx_gain_scripts
(
rfm_device_enum_type                                    device,
rfdevice_lte_get_rx_gain_state_tbl_settings_type       *lte_rx_gain_table,
uint32                                                  carrier_idx
);

int16 
rflte_dm_get_delta_rx_gain_offset
(
rfm_device_enum_type           device,
uint32                         carrier_idx,
uint8                          index
);
#ifndef FEATURE_RF_LTE_PAGE_ONLY
void 
rflte_dm_update_therm_script
( 
uint32                            carrier_idx,
rfm_device_enum_type              device,
rfcom_lte_band_type               band, 
boolean                           update_read_scripts
);

int32 
rflte_dm_get_oride_txagc
(
uint32            carrier_idx
);

uint8 
rflte_dm_get_current_pa_state
(
void
);

int32 
rflte_dm_get_lte_pwr_db10
(
uint32             carrier_idx
);

uint16 
rflte_dm_get_tx_gain_index
(
uint32             carrier_idx
);

uint16 
rflte_dm_get_baseband_backoff
(
uint32             carrier_idx
);

void 
rflte_dm_set_therm_read_period 
(
uint32             carrier_idx,
int32              therm_read_period_ms
);

uint16 rflte_dm_get_pa_smps_pdm(uint32 carrier_idx);

void rflte_dm_update_tpl(uint32 carrier_idx, 
                         uint8 subframe_num, 
                         int32 tpl_srs);


void rflte_dm_update_tpl_srs(uint32 carrier_idx, 
                             uint8 subframe_num, 
                             int32 tpl_srs);

#endif
void rflte_dm_update_tdd_fdd_band_type
(
boolean                band_is_tdd, 
boolean                band_is_hdfdd,   
boolean                is_nbiot,
uint32                 carrier_idx
);

uint8 
rflte_dm_get_start_rb_index
(
uint32            carrier_idx
);

uint8 
rflte_dm_get_num_rbs
(
uint32             carrier_idx
);

void rflte_dm_rx_chain_mask( uint8 active_mask , uint8 carrier_idx);

void rflte_dm_rx_alt_path_ca_active( boolean active_flag, uint8 carrier_idx, uint8 path);


void rflte_dm_rx_set_rxlm_buffer_idx(uint32 carrier_idx, rfcom_device_enum_type device, lm_handle_type rxlm_handle);


/* API to init RFLM FED Txagc Logging */
void rflte_dm_init_debug ( void );

void rflte_dm_common_rxagc_init_debug(void);

int16 rflte_mc_fed_get_rx_gain_offset_from_dm(rfm_device_enum_type device, 
                                              uint32 carrier_idx, 
                                              uint8 lna_state, 
                                              lm_handle_type rxlm_buf_index);

void rflte_dm_set_rxlm_buffer_idx(uint32 carrier_idx, rfcom_device_enum_type device, lm_handle_type rxlm_handle);

#ifndef FEATURE_RF_LTE_PAGE_ONLY
void
rflte_dm_update_p_cmax
(
   uint8 subframe_num,
   uint32 carrier_idx,
   rflte_nv_tbl_type   *rflte_nv_tbl_ptr,
   /* Freq comp */
   rflte_core_freq_comp_type *freq_comp,
   rflte_core_freq_comp_type *freq_comp_srs,
   boolean srs_enabled,
   boolean lin_vs_freq_vs_temp_enabled,
   boolean dpd_enabled,
   /* PA SWPTS */
   uint8 mpr_value,
   rflte_core_txpl_ns_type ns_x,
   uint8 backoff_value,
   /* TPL */
   int32 tpl,
   int32 dtpl,
   int32 tpl_coex,
   int32 dtpl_coex
);
void rflte_dm_get_xpt_logging_data
(
   uint32 carrier_idx, 
   ftm_log_lte_tx_xpt_data_path_type * xpt_data 
);

void rflte_dm_get_dm_logging_data
( 
   uint32 carrier_idx, 
   ftm_log_lte_tx_agc_enh_data_type *ftm_lte_tx_agc_enh_data 
);

boolean rflte_dm_set_rgi_0(uint32 carrier_idx, 
                           rfm_device_enum_type rfm_device,
                           rfcom_lte_band_type band,
                           rfcom_lte_bw_type bw,
                           uint32 freq_khz,                             
                           rf_device_execution_type execution_type,
                           rf_buffer_intf* rgi_0_script);

boolean rflte_dm_populate_rgi0_dm_script(rflm_lte_tech_data_s *lte_dm ,
                                         rflm_lte_fed_static_s *fed_static,                                        
                                         rf_buffer_intf* rgi_0_script);

void rflte_dm_populate_rgi_0_for_tdd
(
   uint32 carrier_idx,
   rfm_device_enum_type rfm_device,
   rfcom_lte_band_type band,
   rfcom_lte_bw_type bw,
   uint32 freq_khz,   
   rfdevice_lte_script_data_type *script_data_ptr
);

/* To Set/Reset Therm_status flag     */
void rflte_dm_set_therm_status_flag(uint32 carrier_idx, boolean therm_status_flag);

/* To Set/Reset Therm_status flag     */
void rflte_dm_set_therm_mitigation_status_flag(uint32 carrier_idx, boolean therm_status_flag);

/* To Set/Reset PA_Therm_status flag     */
void rflte_dm_set_therm_mitigation_progress_status_flag(uint32 carrier_idx, boolean pa_therm_status_flag);

/* To Set/Reset Therm_status flag     */
void rflte_dm_set_therm_progress_status_flag(uint32 carrier_idx, boolean therm_status_flag);

void rflte_dm_set_pwr_ctrl_mode(uint32 carrier_idx,rflm_lte_tx_pwr_ctrl_type pwr_ctrl_mode);

rflm_lte_tx_pwr_ctrl_type rflte_dm_get_pwr_ctrl_mode(uint32 carrier_idx);

/* RFSW to enable hdet trigger based on timer */
boolean rflte_dm_trigger_hdet_active_flag (uint32 carrier_idx,boolean hdet_enable);

boolean rflte_dm_set_hdet_read_flag (uint32 carrier_idx,
                                     boolean hdet_read_finished);

boolean rflte_dm_get_hdet_read_flag (uint32 carrier_idx);

boolean rflte_dm_get_hdet_trigger_flag (uint32 carrier_idx);

/* RFSW to set the power level for triggering the HDET measurement */
boolean rflte_dm_set_hdet_trigger_pwr_lvl_db10 (uint32 carrier_idx, 
                                                int16 pwr_lvl);


void rflte_dm_update_fbrx_adc_hdet_enable_setup_event
(
   uint32 carrier_idx,
   rfdevice_lte_script_data_type *script_data_ptr,
   rflm_lte_ccs_event_type_e ccs_event_id
);

#ifdef FEATURE_RF_ASDIV
boolean rflte_dm_set_asdiv_lm_index(rflte_mc_carrier_info_type* carrier_info,
                                    lm_handle_type rxlm_buffer_idx);
#endif

void 
rflte_dm_update_tx_num_pa_states
(
  rflte_nv_tbl_type *lte_nv_tbl_ptr,
  rfcom_lte_band_type band,
  uint32 carrier_idx
);

boolean rflte_dm_populate_script(rflm_lte_fed_static_s* fed_static, 
                                 const rf_hal_bus_resource_script_type **resource_script,
                                 int16 resource_script_idx,
                                 rflm_lte_ccs_event_type_e event_id,
                                 rflm_lte_rf_script_type* tx_off_script);


void
rflte_dm_reset_txagc_lin_state
(
  uint32 carrier_idx
);

boolean
rflte_dm_allocate_txagc_lin_state
(
  uint32 carrier_idx,
  uint16 lte_pa_state_max
);

void rflte_dm_tx_get_linear_tx_power(rflte_mc_linear_tx_power_input_s* tx_power_input,
                                     rflte_mc_linear_tx_power_output_s* tx_power_output);

int16 rflte_dm_get_bias_offset(uint32 tx_freq, 
                               rfcom_lte_band_type band,
                               uint32 carrier_idx, 
                               rflte_nv_tbl_type *rflte_nv_tbl_ptr);

#ifdef FEATURE_RF_HAS_QTUNER
boolean rflte_dm_common_update_tuner_config_event(void);
#endif /* FEATURE_RF_HAS_QTUNER */

void rflte_dm_update_tx_pll_offset_script
(
  uint32 carrier_idx,
  uint8 subframe_num,
  uint8 tx_pll_offset_idx,
  rflte_core_txpl_afc_param_s afc_param,
  boolean afc_only
);

void rflte_dm_update_tx_nb_tone_bw
(
  uint32 carrier_idx,
  rflte_nb_ul_tone_bw_e tone_bw
);

rflte_nb_ul_tone_bw_e rflte_dm_get_tx_nb_tone_bw
(
  uint32 carrier_idx
);
#endif

void rflte_dm_overwrite_event_buffer
(
  uint32 carrier_idx,
  rf_buffer_intf *script,  
  rflm_lte_rf_script_id_type_e script_id, 
  rflte_dm_write_event_buffer_intf_type_s intf_param
);

void  rflte_dm_set_notch_enable_disable(uint32 carrier_idx,boolean notch_flag);

void  rflte_dm_set_nb_region_notch_settings(uint32 carrier_idx, uint8 nb_region, rflm_dtr_rx_notch_group_struct* notch_params);

void rflte_dm_update_rx_lo_offset_in_KHz(uint32 carrier_idx);

void rflte_dm_set_rx_lo_offset_in_KHz(uint32 carrier_idx, int32 rx_lo_offset_in_KHz);

boolean rflte_dm_update_rx_nb_region_retune_scripts( rflte_mc_carrier_info_type carrier_info,
                                                rfcom_lte_band_type band,
                                                rfdevice_lte_script_data_type *script_data_ptr,
                                                lm_handle_type handle_id,
                                                rflm_lte_rx_ccs_event_type_e ccs_event_id,
                                                uint32 tune_rx_freq_khz,
                                                uint32 nb_rx_freq_khz );
void rflte_dm_update_rx_events_timing(uint32 carrier_idx,
                                  rfm_device_enum_type rfm_device,
                                  rfcom_lte_band_type band);

boolean rflte_dm_build_fs_retune_scripts
( rflte_mc_carrier_info_type              carrier_info,
  rfcom_lte_band_type                     band,
  uint8                                   num_chan,
  rfcom_lte_earfcn_type                   start_chan,
  int32                                    step_size,
  rfdevice_lte_script_data_type          *dl_script_data_ptr,
  lm_handle_type                          handle_id
);

void rflte_dm_init_fs_rx_pll_offset_hal_buffer(void);

void rflte_dm_clear_fs_rx_pll_offset_hal_buffer(void);

void 
rflte_dm_update_ustmr_sync_scripts
(
  uint32       carrier_idx,
  lm_handle_type           handle_id,
  rfdevice_lte_script_data_type *script_data_ptr,
  rfcom_lte_band_type             band
);

void rflte_dm_nb_get_tx_pll_offset_script
(
   int32 rx_freq_error_in_ppm_q10,
   rfcom_lte_earfcn_type nonanchor_tx_chan,
   int8 ul_carrier_freq_offset,
   uint8 subframe_num
);

#if defined(FEATURE_RF_HAS_QTUNER) && defined (FEATURE_RF_LITE_MAAT)
#ifndef FEATURE_RF_LTE_PAGE_ONLY
extern boolean rflte_dm_get_tx_maat_info(rfa_rf_lte_maat_tx_info_type_s *tx_maat_info_ptr);
#endif
#endif

#endif /*RFLTE_DM_H*/
