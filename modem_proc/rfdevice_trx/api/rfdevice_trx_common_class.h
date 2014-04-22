#ifndef RFDEVICE_TRX_COMMON_CLASS_H
#define RFDEVICE_TRX_COMMON_CLASS_H

/*!
  @file
  rfdevice_trx_common_class.h

  @addtogroup RFDEVICE_TRX_COMMON
*/
/*==============================================================================

  Copyright (c) 2015 - 2021 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_trx.mpss/1.20/api/rfdevice_trx_common_class.h#3 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
05/28/20   up    B66 HDET spur issue fix
05/22/20   up    removed unused API: get_iotfe_therm_read_template
09/09/19   up    IOTFE therm read support in INIT and SLEEP state.
07/26/19   up    Added get_iotfe_therm_read_template function
07/02/19   up    Featurized GSM related functions
05/29/19   up    sdr105 v2.0 therm support
04/01/19   up    Added driver changes to support the v200 table entries and split
                 band support
02/26/19   tv    Updates to get and load JDET templates during enter mode
02/19/19   up    Modified get_therm_template
02/14/19   tv    Updates to use rx2_wxe_mem for GSM Rx MON bursts
02/12/19   up    Updated get_therm_read for iotfe therm read
01/24/19   up    Added DCC cal support
01/15/19   am    Added static variable is_tune for HDET to put rx_en and rx_on low.
12/26/18   up     Addded get_init_npler_settings
11/09/18   up    Added function toggle_hkadc_ref_clk
11/01/18   up    flush_to_script_single_byte updated with input param ext_cmd
10/01/18   am    Added API get_rx2_wxemem_info and get_tx0_2_wxemem_info
                 Commented API get_lte_ref_main_settings and load_and_trigger_path_en_template
09/27/18   up    Added new function load_and_trigger_path_en_template and 
                 get_lte_ref_main_settings
09/27/18   up    Changes to remove FFH banking changes for LTE RX.
09/11/18   up    Added new function read_from_rffe_buf_direct
09/05/18   tv    Added support APIs to handle dummy_script
09/05/18   am    Added load_and_trigger_hdet_template()
08/15/18   up    Added new functions flush_to_hw_single_byte and 
                 send_script_to_hw_single_byte
08/14/18   up    Added new function flush_to_script_single_byte, 
                 populate_buffer_single_byte and write_to_script_single_byte. Added
                 member variable script_entry_holder. This is used for building
                 direct DTOP register script building.
08/13/18   up    Modified function get_rx_pll_script
08/10/18   ssr   Added interface APIs to return LTE/GSM script sizes
08/08/18   up    Modified functions compute_tx_rsb_params and 
                 compute_rsb_alpha_beta
07/25/18   up    Modified execute_tx_super_call_flow and get_tx_pll_script
07/24/18   am    Added new function to load HDET tables during TX Tune
07/20/18   up    Added new functions to load RGI0 table loading and toggle_agc_lp_en
07/09/18   up    Added API: read_from_ccs_rffe_buf
06/28/18   up    Modified the functions get_pa_lut_map and get_rffe_bus_info
06/26/18   bm    Updated interface for in_hw() API
06/22/18   up    Modified APIs related to TX RGI to take BW as input.
06/05/18   up    Added function get_trx_lut_type which converts SDR105 LUT type
                 to TRX lut type
05/25/18   up    Added functions related to bootup init and ustmr sync. Also 
                 added an additional paramer to the function get_rffe_bus_info()
05/10/18   up    Changes to merge timer and data fixup enums and coresponding 
                 driver changes
05/09/18   up    Modified load_and_trigger_rgi_template() to include execution_type
                 as param
04/26/18   up    Changes: 
                   - Modified load_and_trigger_template to include data fixup token
                   - Modified get_txagc_data() to accept execution_type as input.
                     type as input.
04/10/18   up    Modified the function get_txagc_luts()
04/03/18   up    Featurize gsm changes
03/16/18   up    Removed lte_rx_sig_path_masks and lte_rx_sig_path_masks 
                 for SDR105
02/16/18   ssr   Updates to send_script_to_hw() API to flush RFFE writes
02/16/18   up    Compilation error fix in API get_txagc_data()
02/14/18   bm    Added interface API get_txagc_data()
                 Added new APIs load_and_trigger_rgi_template(), update_tx_rgi_data(), 
                 update_tx_bbf_data()
02/09/18   up    Modified the API get_tx_pll_config_in_spll_tdd
02/06/18   up    Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF).
                 Also renamed rfdevice_rxtx_common_class to 
                 rfdevice_trx_common_intf_class
09/04/17   ssr    Updates to pass back PLL freq truncation error to tech during tune script build
08/21/17   ssr    Added compare_spll_config_params API
06/16/17   ssr    Added back init_all_tech_static_data_members API prototype
06/12/17   ssr    Updates to move static data member init to WTR driver
04/12/17   snk    Added changes for P1 port update to match mixer type
03/02/17   ssr    Updates for Static LIF feature implementation
03/01/17   aks    SDR105 SS 105p2 fixes
02/21/17   aks    Read IIP2 data from QFPROM if present
02/16/17 snk/aguo GSM Rx tune script building optimization
01/25/17   ka     FFH Phase 2 implementation
01/06/17   bm/ssr Updates for chip rev specific gain char
12/23/16   ssr    Changes to support DLIF feature implementation
12/13/16   snk    Updated get_rx_pll_script, get_tx_pll_script
11/30/16   ash    Add API to get notch hysteresis from AG Sequence
11/22/16   aguo   Changed rfdevice_trx_settings_per_rgi_type to rflm_rfdevice_trx_settings_per_rgi_type
10/25/16   ash    Added code for Dynamic ports (MIMO only ports and RX0/RX0B)
10/27/16   snk    Added get_sigpath_restriction_tbl
10/17/16   hm/ash Combining WB JDET enable and disable API
10/07/16   ssr    Added SPLL-TDD implementation supporting APIs
                  Added an is_spll_tdd flag to the constructor
09/27/16   hm     Script based therm interface
09/12/16   hm     SDR105 BU related fixes. 
                  Spec Inversion to be flipped wrt 5K
                  Handling resetting qfprom trip at boot up
                  Bypassing timer fixup entries for DLIF trig 1/2/3
                  Update tx and rx template ID's for OFT
                  TX AFC bug fix for SDR105 (Tx needs dflo_sc1 and 2 to be programmed in addition to nf1,2 and nb_na)09/02/16   shb    Moved compute_tx_rsb_params to rfdevice_trx from WTR specific code
09/02/16   shb    Calculate Tx RSB, DCOC, Tau only for unique values (HP_LP,HP,LP)
09/02/16   shb    Implemented Tau (Datapath I/Q delay) calculation from QFPROM
08/29/16   ash    Changes to account for Path Assn dependency on RX IIP2
08/29/16   snk    Added get_chip_revision for OFT
08/05/16   ash    Added entry to store num of bytes of memory reclaimed
08/09/16   aks    Renamed req_cfg_token to req_cfg_scouting_debug_buf_idx
                  Changed data type of cfg_token to int16
08/09/16   aks    Pass flag to indicate if device is in cal_state
08/04/16   aks    Dynamic selection of RX0 and RX0B band modes for LTE B42
08/01/16   ash    Added support for JDET DC CAL WXE Template
07/26/16   aks    Fix crash seen when testing notch CA combo with no DRX path
07/26/16   ssr    Changes to rename lo_freq to nominal_freq when offset freq is passed separately
07/25/16   shb    Implemented control of RxSM during RX enable/disable
07/25/16   hm     Handling SDR105 ilna bbf discontiguity
07/22/16   ssr    Pass offset_freq(afc_hz) as an input argument
07/14/16   ash    Added qfprom rev bmsk dependency for rx iip2
07/19/16   bm     Added support for loading dlif_trig templates
07/14/16   ssr    Added common FBRx mode as an argument to get_fbrx_pll_script
07/07/16   ash    Added support for RX IIP2
06/10/16 aks/snk  Fixed crash due to memory being reclaimed for default DRx port   
05/19/16   aks    Added API: get_sub_band_port
05/16/16   sma    Added support for jdet
05/10/16   aks    Updates to pick WTR and RXLM BBF modes based on the band combination
05/10/16   ssr    Added static data members init API and support for memory reclaim
05/03/16   shb    Added input_port_selection table APIs
05/02/16   ash    Moved applicable structs to rflm_rfdevice_typedefs.h
04/27/16   ash    Added support for TX RSB
04/29/16   snk    Added log on/off flag in reg_dump api
04/29/16   aks    Add support to avoid Rx retune during Tx ON/OFF in standalone mode
04/26/16   aks    Invoke check_split_in_elna as part of get_config_chain_params() API
04/25/16   aks    Added APIs: check_split_in_elna(), get_vco_tbl_for_split_in_elna
04/19/16   aks    Moved code to select LTE Tx DAC rate to Common driver 
04/18/16   aks    Moved code to pick LTE Rx ADC rate to Common TRx driver
04/11/16   snk    Added support for ALL wtr reg dump
04/08/16   aks    Removed function: detect_cw_spur
03/25/16   xf     gnss wxe implementation
03/24/16   shb    Added txrsb_process
03/24/16   hm     Increased MAX_TRX_SCRIPT_BUFFERS to 6, to accomodate all concurrent script building contexts
03/23/16   ash    Added changes to the TRX Notch Interface
03/11/16   hm     MOVE/MOVE16 co-existence (Prep for V2)
02/26/16   hm     qfprom read infrastructure
02/10/16   aks    Updates to pick ADC/DAC rate depending on mode(cal, CA, standalone etc.)
02/09/16   aks    Updates to the scouting API to return notches and ADC/DAC clk rates
02/05/16   snk    Added support FBRx AFC 
02/02/16   hm     wxe protection implementation
01/28/16   shb    Moved get_lo_div_data to base rfdevice_trx class. Implemented
                  get_tx_rgi_lut 
01/28/16   snk    KW errors fix
01/19/16   aks    Update scouting API to return PLL option/LO Div ratio for each chain
01/15/16   hm     Therm implementation
01/14/16   aks    Support to pick the LO div Ratio based on LTE CA/WCDMA DBDC Combination
01/08/16   aks    Updated get_trx_chain_info to populate chain specific info in wtr_specific_info struct
01/06/15   aks    Fixed crash seen in OFT testing of Scouting API for 4Rx + 1Tx CA combinations
12/04/15   aks    Logging framework for TRx Scouting API
12/03/15   aks    Updates to return Tx RSB coefficients per RGI
11/18/15   shb    Return DA Pout in get_txagc_data()
11/17/15   aks    Updated get_timer_fixup_event_id to return event id corresponding 
                  to the token passed
11/12/15   xf     Added load_gnss_ag_tbl API 
11/09/15   aks    Support for multiple timer fixups in a single script
10/14/15   snk    Added support for reg_dump API
10/10/15   aks    Identation update
10/09/15   hm     Moved gains tate info from rx_config_info to new struct for GSM Gx implementaiton
10/05/15   aks    Added API: get_afc_script
10/02/15   aks    Updated TRx Script wrapper to store the offset for timer fixup
                  relative to the start of the current instruction block
10/02/15   hm     Bug fix in copy_rx_gain_fixup_data_to_arr()
10/02/15   aks    Device driver updates to support timer and data fixups
10/01/15   shb    Implmented helper functions to load txagc data
10/01/15   aks    Updated get_gain_fixup_data to directly copy data into array instead 
                  of intermediate script wrapper
09/30/15   hm     send_script_to_hw() implementation
09/29/15   shb    Implemented get_txagc_template()
09/28/15   aks    Added API:copy_fixup_data_to_arr
09/28/15   aks    Updated defintion for load_and_trigger_template API
09/28/15   shb    Implemented rfdevice_trx_cmn_power_up
09/28/15   snk    Updated constructor
09/25/15   aks    Added API: load_trigger_template
09/25/15   aks    Added API to return Combined Rx AGC template
09/24/15   shb    Added load_common_init_tbl()
09/24/15   aks    Updated scouting API interface to accept an array of pointers
09/22/15   aks    Include ptr to other Rx chain in rfdevice_trx_config_rx_chain_params_type struct
                  Added API to get VCO combinations which can share LNA from WTR intf
                  Get Template ID from WTR intf layer
09/10/15   hs     Added support for GNSS.
09/09/14   aks    Scouting API update - Init Tx chain info
09/04/14   aks    Scouting API updates- Only loop through the active VCOs in the system
09/03/15   hs     Removed rxtx param from get_tech_instance() and set_tech_instance() APIs.
09/02/15   hm     Added helper function to load RGI template
09/01/15   aks    Scouting API updates- Store scouting API params in 
                  rfdevice_trx struct instead of wtr5975 struct
08/28/15   aks    Updates for Scouting API
08/26/15   hs     Replaced the param phy_path with trx_path_idx in functions
                  set_tech_instance(0 and get_tech_instance().
08/25/15   aks    Added APIs: buffer_manager, flush_to_script, populate_buffer, send_script_to_hw 
08/11/15   shb    Derive rfdevice_trx_common_class from rfdevice_trx_common_intf_class
07/24/15   hm     MOVE implementation for working around MOVE16 HW bug
07/16/15   dps    Added in_hw
07/16/15   hm     Implemented helpers for FBRX PLL programming
07/16/15   aks    Added APIs: load_fbrx_ag_tbl, execute_fbrx_super_call_flow 
06/26/15   aks    Updated get_tech_instance/set_tech_instance to use the trx 
                  tech enum instead of rfcom_mode 
06/24/15   aks    Added const ZERO_PAD_MIN_ADDR
06/22/15   aks    Updated scope of data members encoded_process and encoded_revision
                  from protected -> public so that it can be accessed in WTR 5975 RFFE driver
06/21/15   shb    Added search_tx_rgi() for better code re-use
06/21/15   shb    Fixed ontarget compilation errors
06/19/15   shb    Added load_tx_rgi()
06/19/15   aks    Added support for WXE EOC in super call flow
06/18/15   hm     Added helper function for wxe trigger
06/18/15   aks    Updated init_dc_cal_tbls to return ptr to all Rx AGC gain words
06/17/15   aks    Added path_assn to Rx config struct. 
                  Added API init_reg_filter
06/17/15   aks    Added APIs:execute_tx_super_call_flow and load_tx_ag_tbl
06/16/15   aks    Updates to support Rx PLL programming
06/16/15   aks    Added APIs: load_opcode and load_move16_dynamic_instr
06/15/15   aks    Updated rfdevice_trx_common_class to derive from rfdevice_class 
                  so that memory from modem heap and not AMSS heap is allocated when 
                  object is created
06/12/15   dps    Added rfdevice_trx_in_hw API for AG sequence compilation
06/12/15   aks    Added API execute_rx_super_call_flow                 
06/11/15   aks    Updates to load_rx_ag_tbl
06/09/15   aks    Added APIs: load_rx_ag_tbl, create_prx_tune_script
                  Moved enum write_wxe_ag_tbl_return_type to within class scope
06/08/15   aks    Moved PLL calc APIs to WTR 5975 driver 
06/04/15   aks    Added APIs: get_tech_instance and set_tech_instance
06/03/15   aks    Updated write_wxe_ag_tbl format API to accept trx_script_wrapper
06/02/15   aks    Added TRx Script and TRx Script Wrapper classes 
05/26/15   aks    Updated APIs to use references instead of pointers
05/21/15   aks    Added apis: out_buf, add_delay, write_wxe_ag_tbl
                  Ported to Common Buffer pool updates
04/20/15   hs     Initial version
==============================================================================*/

#include "rfdevice_class.h"
#include "rfcommon_locks.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_trx_typedef_ag.h"
#include "rfdevice_trx_script_class.h"
//#include "rfdevice_trx_common_intf_class.h"
#include "rfdevice_trx_common_intf_class.h"
#include "rfdevice_trx_tx_port_map_class.h"
#include "rfdevice_trx_gnss_intf_class.h"
//#include "rfdevice_gnss_intf.h"

#include "rfdevice_trx_typedefs.h"
//#include "rflm_qlnk_res_def.h"
#include "rfdevice_trx_utils.h"
//#include "rflm_rfdevice_typedefs.h"
#include "rfdevice_trx_typedef_ag.h"
#include "rfm.h"
#include "rfc_common.h"
#include "rf_hal_rffe.h"


class rfdevice_trx_script_wrapper;

class rfdevice_trx_common_class:public rfdevice_trx_common_intf_class
{
  /*===========================================================================*/
  /*                                Constants                                  */
  /*===========================================================================*/

public:

  /*! @brief Indicate the maximum number of script buffers in the common buffer pool*/
  // 2 for Rx. 1 for each SUB/SIM
  // 1 for TX
  // 1 for IRAT (Only 1 SUB at a time. G2X can have 2 script building contexts)
  // 1 for GNSS
  // 1 for Therm
  static const uint8 MAX_TRX_SCRIPT_BUFFERS = 4;

  /*! @brief Enum to indicates the size of reg_data  */
  static const uint8 REG_DATA_SIZE_BYTES = 2;

  /*! @brief Enum to indicate size of word */
  static const uint8 BYTES_PER_WORD = 4;

  /*! @brief Enum to indicate the size of coded instruction */
  static const uint8 CODED_INSTR_SIZE_BYTES = 4;

  /*! @brief Opcode for WXE EOC */
  static const uint32 WXE_EOC_OPCODE = 0xFFFFFFFF;

  /*! @brief  Opcode of relative timer with time 0. Which is essentially a NOP. 
      And can be fixed up with EOC when needed 
  */
  static const uint32 WXE_RELATIVE_TIMER_ZERO_DELAY = 0x24000000;

  /*! @brief Minimum Address above which all instructions must start at word boundary */
  static const uint16 ZERO_PAD_MIN_ADDR = 0x20;

  /*! @brief Minimum number of Rx VCOs for CA */
  static const uint8 MIN_RX_VCOS_CNT_FOR_CA = 2;

  static boolean is_tune;
  
  /* is this a PC3 rf card */
  static boolean is_pc3_card;

  /*===========================================================================*/
  /*                          Enum declarations                                */
  /*===========================================================================*/
public:
  enum write_wxe_ag_tbl_return_type
  { 
    WRITE_WXE_AG_TBL_FAILURE,
    WRITE_WXE_AG_TBL_SUCCESS,
    WRITE_WXE_AG_TBL_EXT_REF,
    WRITE_WXE_AG_TBL_MAX,
    WRITE_WXE_AG_TBL_INVALID = WRITE_WXE_AG_TBL_MAX
  };


  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

public:
  /* Constructor */
  rfdevice_trx_common_class
  (
    const rfc_logical_device_info_type&                 logical_device_cfg,
    const rfc_phy_device_info_type&                     phy_device_cfg
  );

  /* Destructor */
  ~rfdevice_trx_common_class();

  /*===========================================================================*/
  /*                           Interface APIs                                  */
  /*===========================================================================*/

public:
  rfdevice_trx_gnss_intf_class* get_gnss_instance
  (
     void
  );

  boolean get_txagc_data
  (
    rfm_device_enum_type                                rfm_device, 
    rfm_mode_enum_type                                  tech,  
    rfdevice_trx_lte_tx_path_info_type*                 current_path_info,
    rfdevice_tx_lut_data_type*                          lut_data,
    rfdevice_trx_txagc_template_type                    template_type,
    uint8                                               pa_state,
    uint8                                               rgi,
    boolean                                             get_txrsb_params,
    rf_device_execution_type                            execution_type
  );

  boolean rfdevice_cmn_get_tx_lut_scripts
  (
    rfm_mode_enum_type                                    tech,
    rfcom_band_type_u                                     band_u,
    rfcom_bw_type_u                                       bw_u,
    rf_device_execution_type                              execution_type,
    rfdevice_trx_txlut_per_pa_state_type*                 txlut_per_pa_state_arr,
    uint8                                                 txlut_per_pa_state_arr_size,
    rfdevice_trx_txagc_data_buffer_type*                  txagc_data_buffer_arr,
    uint8                                                 txagc_data_buffer_arr_size
  );

#if 0
  rfdev_cmn_status_type power_up                        
  (                                                     
    rfdev_cmn_vote_status_type                          rfdev_vote,
    rf_device_execution_type                            exec_type,
    rf_buffer_intf*                                     script_buf
  );
#endif
  rfdev_cmn_status_type power_up
  (  rf_device_execution_type                           exec_type,
     rf_buffer_intf*                                    script_buf,
     rfdev_cmn_vote_status_type                         rfdev_vote
  );

  boolean power_reset
  (
    rfdev_cmn_power_down_type                           rfdev_power_down_vote
  );

  boolean get_lte_script_size
  (
    rfdevice_trx_lte_rx_event_size_type&                rx_events,
    rfdevice_trx_lte_tx_event_size_type&                tx_events
  );

  boolean get_gsm_script_size
  (
    rfdevice_trx_gsm_rx_event_size_type&                rx_events,
    rfdevice_trx_gsm_tx_event_size_type&                tx_events
  );

  boolean do_internal_device_cal
  ( rfdevice_cmn_int_dev_cal_data_type*                 cal_data 
  );

  boolean populate_pll_script_fixup_with_zero
  (
    const rfdevice_trx_instr_tbl_type& wxe_template,
    rfdevice_trx_wxe_mem_info_type& wxe_mem_info,
    rfdevice_trx_script_wrapper& trx_script_wrapper
  );

  boolean get_rx_dummy_script_trigger
  (
    rf_buffer_intf&  script_buf
  );

  boolean get_tx_dummy_script_trigger
  (
    rf_buffer_intf&  script_buf
  );

  boolean get_txagc_template
  (
    rf_buffer_intf&                                     script_buf,
    uint32&                                             data_fixup_token_id,
    rfdevice_trx_txagc_template_type                    temp_id
  );

  #if 1
  boolean wb_jdet_combined_enable_disable
  (
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            exec_type,
    rfdevice_trx_jdet_mode                              jdet_mode
  );
#endif

  boolean reg_dump
  (
    rfdevice_reg_dump_cmd_type                          reg_dump_type,
    boolean                                             log_on_off
  );

  boolean get_therm_read
  (
    uint16&                                             therm_read_raw,
    int16&                                              therm_read_in_degC,
    boolean                                             is_iotfe_therm = FALSE
  );


  boolean get_therm_script
  (
    rf_buffer_intf*                                     script_buf
  );

  boolean process_therm_script
  (
    rf_hal_bus_rffe_type*                               raw_rffe_script,
    uint16&                                             therm_read,
    int16&                                              therm_read_in_degC
  );

  void reset_iotfe_template_preload_flag();

  boolean set_dac_iref
  (
    rfdevice_dac_iref_val                               iref_val,
    boolean                                             program_iref,
    uint32*                                             agc_gainword
  );

#ifdef FEATURE_GSM
  boolean load_jdet_templates
  (
    rfdevice_trx_script_wrapper&						trx_script_wrapper
  );

  boolean rxsig_get_jdet_read_script
  (
    rf_device_execution_type                             exec_type,
    rf_buffer_intf*                                      script_buf
  );
  
  boolean rxsig_get_jdet_config_script
  (
    rf_device_execution_type                             exec_type,
    rfcom_gsm_band_type                                  rf_band, 
    rf_buffer_intf*                                      script_buf
  );
  /*! Turns on the JDET path */
  boolean rxsig_jdet_enable
  (
   rf_device_execution_type                             exec_type,
   rf_buffer_intf*                                      script_buf
  );
  
  boolean rxsig_jdet_enable_set_timer
  ( 
   uint16                                               holdoff_time_us,
   rf_device_execution_type                             exec_type,
   rf_buffer_intf*                                      script_buf
  );
  
  /*! Turns off the JDET path */
  boolean rxsig_jdet_disable
  (
   rf_device_execution_type                             exec_type,
   rf_buffer_intf*                                      script_buf
  );
#endif /* #ifdef FEATURE_GSM */

  boolean get_rxagc_template
  (
    rf_buffer_intf&                                     script_buf
  );

  boolean get_rxagc_trigger
  (
    rf_buffer_intf&  script_buf
  );

  boolean calculate_hdet_reading
  (
    int32                                               sum_i,
    int32                                               sum_q,
    uint64                                              sum_i_q_sqr,
    uint32                                              fbrx_accum,
    uint8                                               fbrx_accum_overflow,
    uint16*                                             hdet_reading
  );

  boolean rx_adc_vcm_cal_setup
  (
    rfdevice_rx_vco_path_enum_type                      rx_vco
  );


  boolean get_xogen_dcc_val
  (
    rf_buffer_intf*                                  script_buf,
    rf_device_execution_type                         execution_type,
    rfm_dcc_info_type*                               dcc_info
  );
  
  void set_rfc_power_class
  (
    boolean is_cur_card_pc3
  );


  /*===========================================================================*/
  /*                              Helper APIs                                  */
  /*===========================================================================*/
public:

#if 0
  rfdevice_class* get_tech_instance
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               trx_path_idx
  );


  boolean set_tech_instance
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               trx_path_idx,
    rfdevice_class*                                     device_ptr
  );
#endif
  boolean set_gnss_instance
  (
     rfdevice_trx_gnss_intf_class*                      device_ptr
  );

  boolean load_opcode
  (
    uint32                                              coded_instr,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  );

  boolean load_move_instr
  (
    uint32                                              mov_word_0,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  );

  boolean load_move16_data
  (
    uint32                                              coded_data,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  );
  
  boolean load_move_data
  (
    uint32                                              coded_data,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  );

  boolean load_move16_dynamic_instr
  (
    uint16                                              data_tbl_idx,
    const rfdevice_trx_reg_type&                        reg_filter,
    const rfdevice_trx_instr_type*                      instruction,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl
  );

  boolean load_move_dynamic_instr
  (
    uint16                                              data_tbl_idx,
    const rfdevice_trx_reg_type&                        reg_filter,
    const rfdevice_trx_instr_type*                      instruction,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl
  );

  boolean load_reg_data
  (
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_move_instr_type                        move_instr_type,
    const uint16*                                       data,
    uint16                                              num_data
  );

  write_wxe_ag_tbl_return_type write_wxe_ag_tbl
  (
    uint16&                                             data_tbl_idx,
    uint16&                                             instr_tbl_idx,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl
  );

  boolean load_rx_ag_tbl
  (
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_rx_ag_tbls_type&                     rx_ag_tbls,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl,
    const rfdevice_rx_config_info_type&                 rx_config_info,
    const rfdevice_rx_gain_state_info_type*             gs_info,
    rfdevice_pll_truncation_error_info_type*            pll_trunc_error_info = NULL
  );

  boolean load_gnss_ag_tbl
  (
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl,
    uint16 gain_hwu
  );

  boolean load_rx_agc_gain_word
  (
    int                                                 gain_state,
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_rx_agc_mapping_tbl_type&             rx_agc_mapping_tbl,
    rfdevice_trx_move_instr_type                        move_instr_type
  );

  boolean load_tx_ag_tbl
  (
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_tx_ag_tbls_type&                     tx_ag_tbls,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl,
    const rfdevice_tx_config_info_type&                 tx_config_info
  );
  boolean load_hdet_ag_tbl
  (
	const rfdevice_trx_reg_type&						reg_filter,
	rfdevice_trx_wxe_mem_info_type& 					wxe_mem_info,
	rfdevice_trx_script_wrapper&						trx_script_wrapper,
	const rfdevice_hdet_ag_tbls_type&					hdet_ag_tbls,
	const rfdevice_trx_mapping_tbl_type&				mapping_tbl
  );

  boolean load_hdet_agc_gain_word
  (
	const rfdevice_trx_reg_type&						reg_filter,
	rfdevice_trx_wxe_mem_info_type& 					wxe_mem_info,
	rfdevice_trx_script_wrapper&						trx_script_wrapper,
	const rfdevice_rx_agc_mapping_tbl_type&             hdet_agc_mapping_tbl,
	rfdevice_trx_move_instr_type						move_instr_type
  );

  boolean load_tx_rgi0_ag_tbl
  (
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_tx_ag_tbls_type&                     tx_ag_tbls,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl,
    const rfdevice_tx_config_info_type&                 tx_config_info
  );

  boolean load_tx_rgi_template
  (
    uint8                                               rgi,
    uint8                                               bw,
    rfdevice_trx_txagc_pwr_data_type                    lut_type,
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_tx_rgi_ag_tbls_type&                 tx_rgi_ag_tbls,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl
  );

  boolean load_tx_rgi
  (
    uint8                                               rgi,
    uint8                                               bw,
    rfdevice_trx_txagc_pwr_data_type                    lut_type,
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    const rfdevice_trx_txbbf_lut_type&                  bbf_lut,
    const rfdevice_trx_txagc_lut_type&                  rgi_lut,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_move_instr_type                        move_instr_type,
    boolean                                             is_bbf_data
  );

  boolean load_tx_rgi0
  (
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_tx_ag_tbls_type&                     tx_ag_tbls,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl,
    const rfdevice_tx_config_info_type&                 tx_config_info,
      rfdevice_trx_move_instr_type                      move_instr_type
  );

  boolean search_tx_rgi
  (
    uint8                                               rgi,
    rfdevice_trx_txagc_pwr_data_type                    lut_type,
    const rfdevice_trx_reg_type&                        reg_filter,
    const rfdevice_trx_txagc_lut_type&                  rgi_lut,
    uint16&                                             search_index
  );

  boolean compute_tx_rsb_params
  (
    const rfdevice_trx_txagc_lut_type&                  txagc_lut,
    rfdevice_trx_txagc_pwr_data_type                    trx_lut_type,
    uint8                                               rgi,
    uint8                                               txrsb_tx_index,
    uint16&                                             rsb_coeff_alpha,
    uint16&                                             rsb_coeff_beta
  );

  boolean execute_rx_super_call_flow
  (
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_super_call_flow_type*            sequence,
    const rfdevice_rx_ag_tbls_type&                     rx_ag_tbls,
    const rfdevice_rx_config_info_type&                 rx_config_info,
    rfdevice_pll_truncation_error_info_type*            pll_trunc_error_info = NULL,
    const rfdevice_rx_gain_state_info_type*             gs_info = NULL
  );

  boolean execute_tx_super_call_flow
  (
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_super_call_flow_type*            call_flow_seq,
    const rfdevice_tx_ag_tbls_type&                     tx_ag_tbls,
    const rfdevice_tx_config_info_type&                 tx_config_info
  );
  
  boolean execute_hdet_super_call_flow
  (
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_super_call_flow_type*            call_flow_seq,
    const rfdevice_hdet_ag_tbls_type&                   hdet_ag_tbls
  );
#if 0
  boolean load_fbrx_ag_tbl
  (
    const rfdevice_trx_reg_type&                        reg_filter,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_fbrx_ag_tbls_type&                   fbrx_ag_tbls,
    const rfdevice_trx_mapping_tbl_type&                mapping_tbl,
    const rfdevice_fbrx_config_info_type&               fbrx_config_info,
    uint8*                                              gain_state = NULL
  );

  boolean execute_fbrx_super_call_flow
  (
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_super_call_flow_type*            call_flow_seq,
    const rfdevice_fbrx_ag_tbls_type&                   fbrx_ag_tbls,
    const rfdevice_fbrx_config_info_type&               fbrx_config_info
  );
#endif

  boolean in_hw
  (
    uint16   addr,
    uint32&	 data,
    boolean  is_reg_dump = FALSE
  );

  void init_reg_filter
  (
    rfdevice_trx_reg_type&                              reg_filter,
    uint8                                               revision,
    uint8                                               path_assn,
    uint8                                               process
  )
  {
    reg_filter.process   = process;
    reg_filter.revision  = revision;
    reg_filter.path_assn = path_assn;
  }

  boolean trigger
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    uint16                                              wxe_addr_to_trig = RFDEVICE_TRX_INVALID_16_BIT 
  );

  boolean buffer_manager
  (
    rf_buffer_intf*                                     buf,
    rf_device_execution_type                            exec_type,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    boolean                                             is_wxe_base_blk,
    const rfdevice_trx_rffe_bus_info&                   bus_info
  );

  boolean flush_to_script
  (
    rf_buffer_intf*                                     script_buf,
    uint16                                              addr,
    const uint16*                                       data,
    uint16                                              data_len,
    int16                                               delay,
    rf_buffer_transaction_type                          transaction_type,
    uint8                                               data_fixup_cnt_instr_blk,
    uint8                                               data_fixup_cnt_data_blk,
    uint8                                               timer_fixup_cnt,
    uint16                                              instr_blk_start_offset_in_uint16,
    const rfdevice_trx_data_fixup_info*                 data_fixup_arr_instr_blk,
    const rfdevice_trx_data_fixup_info*                 data_fixup_arr_data_blk,
    const rfdevice_trx_timer_fixup_info                 timer_fixup_arr[rfdevice_trx_addr_block_info_instr::MAX_TIMER_FIXUPS_PER_ADDR_BLK],
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    boolean                                             is_wxe_base_blk,
    rfdevice_trx_rffe_bus_info                          bus_info
  );

  boolean flush_to_script_single_byte
  (
    boolean is_extended = TRUE
  );

  boolean flush_addr_blk_to_ccs_rffe_buff
  (
    uint16                                              start_addr,
    const uint16*                                       data,
    uint16                                              data_len,
    const rfdevice_trx_rffe_bus_info&                   bus_info
  );

  boolean read_from_ccs_rffe_buf
  (
    uint16                                              start_addr,
    uint16*                                             data,
    uint16                                              data_len,
    const rfdevice_trx_rffe_bus_info&                   bus_info
  );

  boolean read_from_rffe_buf_direct
  (
    uint16                                              start_addr,
    uint16*                                             data,
    uint16                                              data_len,
    const rfdevice_trx_rffe_bus_info&                   bus_info
  );

  boolean read_from_rffe_buf_single_byte
  (
    uint16                                              start_addr,
    uint16*                                             data,
    uint16                                              data_len,
    const rfdevice_trx_rffe_bus_info&                   bus_info
  );

  boolean populate_buffer
  (
    rf_buffer_intf*                                     script_buf,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    boolean                                             is_wxe_base_blk,
    rfdevice_trx_rffe_bus_info                          bus_info
  );
  boolean populate_buffer_single_byte
  (
    rf_buffer_intf*                                     script_buf,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_rffe_bus_info                          bus_info,
    boolean                                             extended = TRUE
  );

  boolean write_to_script_single_byte
  (
    rf_buffer_intf* buf,
    uint16 addr,
    uint8 data,
    rf_buffer_transaction_type transaction_type,
    boolean extended_cmd,
    rfdevice_trx_rffe_bus_info& bus_info,
    int16 delay,
    boolean new_time_ref
  );

  boolean flush_to_hw_single_byte
  (
    uint16                                              start_addr,
    const uint16*                                       data,
    uint16                                              data_len,
    const rfdevice_trx_rffe_bus_info&                   bus_info
  );

  boolean send_script_to_hw
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    boolean                                             is_wxe_base_blk,
    const rfdevice_trx_rffe_bus_info&                   bus_info,
    rfdevice_trx_addr_data_info_type*                   rd_data_arr = NULL ,
    const uint16                                        rd_data_arr_size =0 ,
    uint16*                                             num_reads_ptr = NULL
  );

  boolean send_script_to_hw_single_byte
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_rffe_bus_info&                   bus_info,
    rfdevice_trx_addr_data_info_type*                   rd_data_arr =NULL, 
    const uint16                                        rd_data_arr_size =0 ,
    uint16*                                             num_reads_ptr =NULL
  );

  boolean load_common_init_tbl
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    const rfdevice_trx_mapping_tbl_type&                common_init_mapping_tbl
  );

  boolean load_and_trigger_template
  (
    const rfdevice_trx_wxe_template_type&               wxe_template,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    boolean                                             load_trigger = TRUE,
    rfdevice_trx_fixup_token_type                       token_type = RFDEVICE_TRX_FIXUP_TOKEN_MAX
  );
#if 0
  boolean load_and_trigger_path_en_template
  (
    const rfdevice_trx_wxe_template_type&               wxe_template,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    rfcom_lte_band_type                                 rf_band,
    boolean                                             is_rx,
    rfcom_lte_cat_type                                  lte_cat,
    boolean                                             load_trigger = TRUE
  );
#endif
  boolean load_and_trigger_rgi_template
  (
    rfdevice_tx_lut_data_type*                          lut_data,
    const rfdevice_trx_txagc_lut_type&                  txagc_lut,
    const rfdevice_trx_txbbf_lut_type&                  txbbf_lut,
    uint8                                               bw,
    uint16                                              rgi_search_index,
    const rfdevice_trx_wxe_template_type&               wxe_template,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    rf_device_execution_type                            execution_type,
    boolean                                             load_trigger = TRUE
  );

  boolean load_and_trigger_hdet_template
  (
	const rfdevice_trx_hdet_reg_type&		    hdet_divn_lut,
	const rfdevice_trx_wxe_template_type&               wxe_template,
	rfdevice_trx_script_wrapper&                        trx_script_wrapper,
	const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
	rf_device_execution_type                            execution_type,
	boolean                                             load_trigger = TRUE
  );
#if 0
  boolean load_and_trigger_rxsm_template
  (
    const rfdevice_trx_wxe_template_type&               wxe_template,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    const rfdevice_trx_wxe_mem_tbl_type&                wxe_mem_tbl,
    const rfdevice_trx_rxsm_lut_type*                   rxsm_map_tbl,
    uint8                                               rxsm_map_tbl_size,
    uint8                                               prx_path_assn
    //uint8                                               drx_path_assn
  );
#endif

  boolean copy_rx_gain_fixup_data_to_arr
  (
    int                                                 gain_state,
    uint16*                                             fixup_arr,
    uint8&                                              fixup_arr_idx,
    uint8                                               fixup_arr_size_in_uint16,
    const rfdevice_trx_reg_type&                        reg_filter,
    const rfdevice_rx_agc_mapping_tbl_type&             rx_agc_mapping_tbl
  );

  boolean update_tx_rgi_data
  (
    const rfdevice_trx_txagc_lut_type&	                txagc_lut,
    rfdevice_trx_script_wrapper&	                trx_script_wrapper,
    rfdevice_trx_wxe_mem_info_type&	                wxe_mem_info,
    uint16	                                        rgi_search_index,
    uint8&	                                        num_reg_per_rgi
  );

  boolean update_tx_bbf_data
  (
    const rfdevice_trx_txagc_lut_type&                  txagc_lut,
    const rfdevice_trx_txbbf_lut_type&                  txbbf_lut,
    uint8                                               bw,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    uint16                                              rgi_search_index,
    uint8&                                              num_reg_per_rgi,
    const rfdevice_trx_reg_type&                        reg_filter
  );

  rfdevice_trx_fixup_event_type get_fixup_event_id
  (
    rfdevice_trx_fixup_token_type                 fixup_token
  );

  const uint16* get_lo_div_data
  (
    uint8                                               lodiv_ratio,
    const rfdevice_trx_lodiv_lut_type&                  lodiv_data_lut
  );

  const rfdevice_trx_txagc_lut_type* get_tx_rgi_lut
  (
    uint8                                               lodiv_ratio,
    const rfdevice_trx_txagc_lodiv_lut_type&            lodiv_txagc_lut
  );
  
  boolean read_qfprom
  (
    const rfdevice_trx_qfprom_sym_tbl_type& qfprom_sym_tbl
  );

  boolean get_qfprom_symbol_val
  (
    uint16                                              qfprom_symbol,
    uint64&                                             decoded_sym_val
  );

#if 0
  boolean get_bw_in_hz
  (
    rfdevice_trx_tech_type                              tech,
    rfcom_bw_type_u                                     bw_u,
    uint32&                                             bw_hz
  );
#endif

  boolean add_rx_port_to_map_to_psp_map_tbl
  (
    int16*                                              map_to_psp_map_tbl,
    uint16                                              map_to_psp_map_tbl_size,
    uint16                                              wtr_band_port,
    uint16                                              paired_dc_cal_port,
    uint16&                                             num_used_ports
  );

  boolean get_chip_revision
  (
    uint8&                                              chip_revision
  );
  
  boolean reclaim_unused_memory(void);

  boolean get_num_heap_bytes_alloc_map_tbls (uint32 &num_bytes);

#if 0
  boolean get_spll_tdd_and_retune_info
  (
    rfdevice_trx_rf_config_type&                        req_rf_cfg,
    uint8                                               tx_vco_cnt
  );


  boolean compare_spll_config_params
  (
    rfdevice_trx_rf_config_type&                        req_rf_cfg
  );
#endif

  boolean create_script_buffer
  (
    uint32                                            qlink_blk_size_bytes,
    rf_buffer_intf*&                                  script_buff
  );
#if 0
  boolean get_ffh_params
  (
    rfdevice_trx_rf_config_type&                        req_rf_cfg
  );
#endif

#if 0
  //boolean check_prx_drx_mixer
  (
    rfdevice_trx_rf_config_type&                        req_rf_cfg
  );
#endif

private:
  
  boolean decode_qfprom_tbl
  (
    const uint64*                                       qfprom_mem,
    const uint16&                                       num_qfprom_blocks,
    const rfdevice_trx_qfprom_sym_tbl_type&             qfprom_sym_tbl
  );

  boolean bootup_init
  (
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            execution_type
  );

  boolean ustmr_sync
  (
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            execution_type
  );

  boolean extract_bits
  (
    const uint64&                                       base_val,
    uint16                                              bit_start,
    uint16                                              bit_stop,
    uint64&                                             extracted_val
  );

  /*===========================================================================*/
  /*                   Member functions- Pure Virtual                          */
  /*===========================================================================*/
public:

  virtual boolean calc_na_nb_nf_rx
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               mode,
    uint64                                              nominal_freq_hz,
    int32                                               afc_freq_hz,
    const rfdevice_trx_pll_config_type&                 pll_config,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_move_instr_type                        move_instr_type,
    uint8                                               num_afc_regs_exp
  ) = 0;

  virtual boolean calc_na_nb_nf_tx
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               mode,
    uint64                                              nominal_freq_hz,
    int32                                               afc_freq_hz,
    const rfdevice_trx_pll_config_type&                 pll_config,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_move_instr_type                        move_instr_type,
    uint8                                               num_afc_regs_exp
  ) = 0;

#if 0
  virtual boolean calc_na_nb_nf_fbrx
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               mode,
    uint64                                              nominal_freq_hz,
    int32                                               afc_freq_hz,
    const rfdevice_trx_pll_config_type&                 pll_config,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_trx_move_instr_type                        move_instr_type,
    uint8                                               num_afc_regs_exp
  ) = 0;  
#endif

  virtual boolean init_all_tech_static_data_members(void) = 0;
#ifdef FEATURE_GSM
  virtual boolean execute_gsm_rx_port_map_reclaim(void) = 0;

  virtual boolean execute_gsm_tx_port_map_reclaim(void) = 0;

  virtual boolean get_gsm_heap_mem_alloc_map_tbls(uint32 &num_bytes) = 0;
#endif

  virtual boolean execute_lte_rx_port_map_reclaim(void) = 0;

  virtual boolean execute_lte_tx_port_map_reclaim(void) = 0;

  virtual boolean new_port_map_tbl_release(void) = 0;
  
  virtual boolean get_lte_heap_mem_alloc_map_tbls(uint32 &num_bytes) = 0;



  virtual boolean get_rxagc_ilna_data_fixup_size
  (
    uint16&                                             ilna_fixup_size
  ) = 0;

  virtual boolean get_rxagc_bbf_data_fixup_size
  (
    uint16&                                             bbf_fixup_size
  ) = 0;

  virtual boolean get_therm_template
  (
    const rfdevice_trx_wxe_template_type*&              therm_wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl,
    boolean                                             is_script_based = FALSE
  ) = 0;

  virtual boolean get_iotfe_therm_template
  (
    const rfdevice_trx_wxe_template_type*&              therm_wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

#ifdef FEATURE_GSM
  virtual boolean get_jdet_disable_template
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_jdet_enable_template
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_jdet_read_template
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_jdet_config_lb_template
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_jdet_config_mb_template
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;
#endif /* #ifdef FEATURE_GSM */

  virtual boolean toggle_hkadc_ref_clk
  (
    rfdevice_trx_script_wrapper&                        script_wrapper,
    boolean on_off
  ) = 0;

  virtual boolean convert_therm_read
  (
    uint32                                             *therm_data_0 ,
    uint32                                             *therm_data_1 ,
    boolean                                             is_iotfe_therm,
    uint16&                                             therm_read,
    int16&                                              therm_read_in_degC
  ) = 0;

  virtual boolean get_rffe_bus_info
  (
    boolean                                             is_high_speed,
    rfdevice_trx_rffe_bus_info&                         rffe_bus_info
  ) = 0;

  virtual boolean get_lte_script_sizes
  (
    rfdevice_trx_lte_rx_event_size_type&				  rx_events,
    rfdevice_trx_lte_tx_event_size_type&				  tx_events
  ) = 0;

  virtual boolean get_gsm_script_sizes
  (
    rfdevice_trx_gsm_rx_event_size_type&				  rx_events,
    rfdevice_trx_gsm_tx_event_size_type&				  tx_events
  ) = 0;

  virtual boolean get_rx_dummy_script_wxe_templ
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_tx_dummy_script_wxe_templ
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_rx2_wxemem_info
  (
    rfdevice_trx_wxe_mem_info_type&					  wxe_mem_info
  ) = 0;
  
  virtual boolean get_tx0_2_wxemem_info
  (
    rfdevice_trx_wxe_mem_info_type&					  wxe_mem_info
  ) = 0;

protected:

  virtual boolean get_rx_pll_script
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               mode,
    uint16                                              band,
    uint64                                              nominal_freq_hz,
    int32                                               offset_from_nominal_hz,
    const rfdevice_trx_pll_config_type&                 pll_config,
    const rfdevice_trx_wxe_template_type*               pll_wxe_templ,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    rfdevice_pll_truncation_error_info_type*            pll_trunc_error_info = NULL
  ) = 0;


  virtual boolean get_tx_pll_script
  (
    rfdevice_trx_tech_type                              tech,
    uint8                                               mode,
    uint16                                              band,
    uint64                                              nominal_freq_hz,
    int32                                               offset_from_nominal_hz,
    const rfdevice_trx_pll_config_type&                 pll_config,
    const rfdevice_trx_wxe_template_type*               pll_wxe_templ,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper,
    boolean                                             default_pll_option
  ) = 0;

#if 0
  virtual boolean get_fbrx_pll_script
  (
    rfdevice_trx_tech_type                              tech,
    uint16                                              band,
    rfdevice_trx_fbrx_power_mode_enum_type              pwr_mode,
    uint8                                               fbrx_mode,
    uint8                                               pll_option,
    uint64                                              nominal_freq_hz,
    int32                                               offset_from_nominal_hz,
    const rfdevice_trx_pll_config_type&                 pll_config,
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  ) = 0;
#endif


  virtual boolean init_chip_params() = 0;

  virtual boolean get_common_init_tbl
  (
    rfdevice_trx_wxe_mem_info_type&                     wxe_mem_info,
    rfdevice_trx_mapping_tbl_type&                      wan_init_mapping_tbl
  ) = 0;

  virtual boolean get_gain_prx_wxe_templ
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_txagc_wxe_templ
  (
    rfdevice_trx_txagc_template_type                    template_type,
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

#if 1
  virtual boolean get_wb_jdet_enable_plus_disable_wxe_templ
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  )= 0;
#endif

  virtual boolean get_wb_jdet_dc_cal_plus_disable_wxe_templ
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean get_reg_dump_addr
  (
    rfdevice_reg_dump_cmd_type                          reg_dump_type,
    rfdevice_reg_dump_addr_info_type*                   addr_info,
    uint16&                                             total_blocks
  ) =0;

  virtual boolean is_wb_jdet_supported
  (
     rfdevice_trx_jdet_supported_type&                  wb_jdet_supported_flag
  )= 0;


  virtual boolean compute_rsb_alpha_beta
  (
    const rfdevice_rsb_data_type&                       tx_rsb_static_lut,
    uint64                                              qfprom_txrsb_gain,
    uint64                                              qfprom_txrsb_phase,
    uint64                                              qfprom_txrsb_tau,
    uint64                                              qfprom_match_found,
    uint16&                                             rsb_coeff_alpha,
    uint16&                                             rsb_coeff_beta
  ) = 0;

  virtual boolean reset_qfprom_trim
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  ) = 0;

#if 0
  virtual boolean get_tx_pll_config_in_spll
  (
    rfdevice_rx_config_info_type                        rx_config_info,
    rfdevice_trx_pll_config_type&                       spll_tx_config,
    const rfdevice_trx_wxe_template_type*               &pll_wxe_templ
  ) = 0;
#endif

  virtual const rfdevice_trx_vco_pair_type* get_vco_tbl_for_spll_tdd_in_wtr
  (
    uint8&                                              spll_tdd_vco_combo_tbl_size
  ) = 0;
  #if 0
  virtual boolean get_notch_hysteresis
  (
    uint8&                                              notch_hysteresis
  ) = 0;
#endif

  virtual boolean get_txagc_luts
  (
    rfdevice_trx_tech_type                              tech,
    rfdevice_trx_lte_tx_path_info_type*                 current_path_info,
    const rfdevice_trx_txagc_lut_type**                 txagc_lut,
    const rfdevice_trx_txbbf_lut_type**                 txbbf_lut,
    uint8*                                              txrsb_tx_index
  ) = 0;

  virtual boolean get_pa_lut_map
  (
    rfdevice_trx_tech_type                              tech,
    rfdevice_trx_lte_tx_path_info_type*                 current_path_info,
    rfdevice_trx_pa_lut_map_type&                       pa_lut_map
  ) = 0;

  virtual boolean get_bootup_init_seq
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  ) = 0;

  virtual boolean get_ustmr_sync_seq
  (
    rfdevice_trx_script_wrapper&                        script_wrapper,
    uint32                                              ustmr_24bit
  ) = 0;

  virtual boolean get_trx_lut_type
  (
    uint8                                               lut_type,
    rfdevice_trx_txagc_pwr_data_type&                   trx_lut_type
  ) = 0;

  virtual boolean toggle_agc_lp_en
  (
    uint16&                                             tx_agc_reg2_data,
    boolean                                             on_off_flag
  ) = 0;

  virtual boolean get_init_npler_settings
  (
    rfdevice_trx_script_wrapper&                        trx_script_wrapper
  ) = 0;

#if 0
  virtual boolean get_lte_ref_main_settings
  (
    boolean                                             is_rx,
    rfcom_lte_band_type                                 rf_band,
    rfcom_lte_cat_type                                  lte_cat,
    uint16                                              ext_ref,
    uint16*                                             data
  ) = 0;
#endif

  virtual boolean get_xo_gen_dcc_rd_template
  (
    const rfdevice_trx_wxe_template_type*&              wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type*&               wxe_mem_tbl
  ) = 0;

  virtual boolean convert_xo_gen_dcc_read
  (
    uint16                                               read_back_reg_value,
    rfm_dcc_read_back_type&                         dcc_read_back_type
  ) = 0;


  /*===========================================================================*/
  /*                           Friend classes                                  */
  /*===========================================================================*/

private:
  friend class rfdevice_trx_script_wrapper;

  /*===========================================================================*/
  /*                           Data Members                                    */
  /*===========================================================================*/

public:

  /*! @brief Debug buffer to store history of parameters passed to/ 
      returned from scouting API*/
 // rfdevice_trx_scouting_debug_log_buffer_class scouting_debug_buff;

  /*! Chip process */
  rfdevice_trx_chip_process_list encoded_process;

  /*! Chip revision */
  uint8 encoded_revision;

  /*! Chip revison enum mapping */
  uint16 encoded_revision_map_enum;

  /*! Tx RSB process */
  rfdevice_trx_chip_process_type txrsb_process;

  /*! qfprom revision */
  uint8 qfprom_revision;

  /*! Size of qfprom_decoded_read_arr */
  uint16 qfprom_decoded_read_arr_size;

  /*! Array to store values of decoded qfprom symbols */
  uint64* qfprom_decoded_read_arr;

  /*! Pointer to sigpath restriction table which is stored in heap */
 // rfdevice_trx_sig_path_restriction_type* sig_path_restriction_tbl_ptr;

  /*! Stores the reclaimed mem in bytes */
  rfdevice_trx_reclaimed_mem_debug_type reclaimed_mem_debug;

  /* Stores the instr addr of rx2_wxe_wxe_mem after the */
  uint32 wxe_instr_addr_after_cmn_rxtx;
  
  /* Flag to indicate if the PA therm template is preloaded or not! */
  boolean is_iote_therm_template_preloaded;

  /*! Therm median */
  uint32 iotfe_therm_median;


protected:

  /*! Common Script Buffer pool Manager */
  rfdevice_trx_script trx_script_buffers[MAX_TRX_SCRIPT_BUFFERS];

  /*! Mutex lock used for rffe script buffer pool management to 
    allocate and release buffer resources */
  rf_lock_data_type script_mgr_lock;

#if 0
  /*! Stores the device ptrs for each tech/Rxtx/phy_path */
  rfdevice_class* tech_instance[RFDEVICE_TRX_STD_NUM][RFDEVICE_TRX_PATH_IDX_MAX];
#endif
  /*! Stores the device ptrs for GNSS */
  rfdevice_trx_gnss_intf_class* gnss_instance;

  /*! Buffer to holder for RFFE script entry used in write_to_script_single_byte API for
  DTOP ergister scripts
  */
  rfdevice_trx_rffe_script_entry_holder_type script_entry_holder;

};

/*===========================================================================*/
/*                  Typedefs for Scoped  declarations                        */
/*===========================================================================*/

typedef rfdevice_trx_common_class::write_wxe_ag_tbl_return_type write_wxe_ag_tbl_return_type;

#endif // RFDEVICE_TRX_COMMON_AG_CLASS_H
