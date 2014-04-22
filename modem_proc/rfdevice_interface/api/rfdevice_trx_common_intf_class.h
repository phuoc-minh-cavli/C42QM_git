#ifndef RFDEVICE_TRX_COMMON_INTF_CLASS
#define RFDEVICE_TRX_COMMON_INTF_CLASS
/*!
  @file rfdevice_rxtx_common_class.h 

  @brief
  This is the rf device factory which creates all devices.



*/

/*===========================================================================

  Copyright (c) 2012 - 2021 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_common_intf_class.h#2 $

when       who     what, where, why
--------------------------------------------------------------------------------
05/28/21   up      B66 HDET spur issue Fix
09/09/19   up      IOTFE therm read support in INIT and SLEEP state.
07/02/19   up      Featurized GSM related functions
02/26/19   tv      Updated the JDET interface APIs
02/12/19   up      Updated API get_therm_read for iotfe therm read
01/24/19   up      Added DCC cal support
09/04/18   tv      Added support APIs to handle dummy_script
08/14/18   up      Modified API ustmr_sync
08/10/18   ssr     Added interface APIs to return LTE/GSM script sizes
05/25/18   up      Added new APIs for booutup init and ustmr sync
04/19/18   up      Added the interface get_vco_info
02/14/18   bm      Updated interface for virtual API get_txagc_data()
02/06/18   up      Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF) 
                   device driver addition/Modification 
10/20/15   sd      Added API to get WTR capability for Autopin PAR captures
10/06/15   sd      Added support for AutoPin PAR
08/03/15   sma     Added ULCA 20+20 Tx LUT power offset 
02/24/15   aks     Device driver updates to enable ADC Vcm Cal 
02/19/15   hs      Added support for retention.
02/10/15   dps     Added support for Tx RSB calibration on WTR devices
11/24/14   aks     Updated get_tx_lut_scripts to accept bw union as an input
11/13/14   hs      Added API to set the JDET reading timer.
10/08/14   sd      Added support for Therm enable, therm read and hdet calculation
09/18/14   rsr     Added support to initialize manufacture/product and revision ID of transceiver.
09/09/14   lm      Added reg_dump() to print register debug info
08/17/14   tks     Added support for overriding fbrx gain state
08/15/14   sd      Added tech argument to FBRX Enable API
08/14/14   shb     Updated fbrx_setup/disable APIs to support slow_ramp ON and 
                   OFF sequence for LDO 26
08/05/14   sd      Added suppport for ILPC
07/25/14   sd      Added frequency arg to FBRX Enable API to handle band
                   dependent sequences
06/24/14   sma     Added LNA gain state for FBRX RSB  
06/16/14   sbm     HDET online brinup
06/12/14   hs      Added suppport for JDET.
05/29/14   shb     Added frequency arg to FBRX APIs to handle band dependent 
                   sequences
05/23/14   hm      Added API support to indicate PMIC 1V rail shutoff 
05/22/14   bm      Added API for light sleep
04/14/14   bm      Changes to decouple GPS and WWAN device ON/OFF state changes 
03/28/14   npi     Update communication protocol for device
03/10/14   hm      FBRX enable API enhancement to add delay after LDO enable, if configured
03/03/14   hs      Added two parameters to power_on()to allow the power on 
                   sequence to be logged in the script.
02/14/14   shb     Added API to retrieve FBRX RSB coefficients
02/05/14   hm      Added FBRX DC Cal enable and disable API. Renamed some API's
02/05/14   shb     Added read_therm API in WTR common class interface
01/16/14   hm      STG API implementation to enable and disable tone generation
12/09/13   rp      Added variable and methods to keep track of instance number of 
                   given rxtx device. Part of "==" operator overload change.
11/01/13   shb     Corrected variable name
10/11/13   shb     Added FBRX setup API. Updated FBRx enable/disable for 
                   immediate execution support
10/04/13   shb     Added APIs to return dummy tx trigger script, fbrx_enable and
                   fbrx_disable scripts
10/04/13   shb     Implemented get_tx_lut_scripts() for TXAGC
09/16/13   shb     Implemented virtual functions for common interface in 
                   rfdevice_rxtx_common_class()
08/22/13 shb/sr    Added get/set_tech_instance to support C++ tech interfaces
08/15/12   shb     Fixed compiler errors 
05/24/12   vss     Initial revision.


===========================================================================*/

#include "rfdevice_intf_cmd.h"
#include "rfdevice_class.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_trx_gnss_intf_class.h" 
#include "rfcommon_msg.h"
#include "rf_hal_rffe.h"

#ifdef __cplusplus

class rfdevice_trx_common_intf_class:public rfdevice_class
{
  protected:
  rfdevice_cmn_type* common_device;
  uint32 device_asic_id;
  
  //rfdevice_class *tech_instance[RFCOM_NUM_MODES][RFDEVICE_RXTX_MAX][RFDEVICE_TRX_MAX_PATHS];
    /*! Stores the device ptrs for each tech/Rxtx/phy_path */
  rfdevice_class* tech_instance[RFDEVICE_TRX_STD_NUM][RFDEVICE_TRX_PATH_IDX_MAX];
  
  rfdevice_comm_proto_enum_type rf_device_comm_protocol; /*! Communication protocol in use */

  public:
  rfdevice_trx_common_intf_class();

  rfdevice_trx_common_intf_class(uint16 m_id, uint16 p_id, uint16 rev_id); 

  virtual rfdevice_cmn_type* get_instance();

  virtual uint32 get_device_asic_id();

  virtual rfdevice_comm_proto_enum_type get_comm_protocol();
#if 0
  virtual rfdevice_class* get_tech_instance
  (
    rfcom_mode_enum_type                                mode, 
    rfdevice_rxtx_enum_type                             rxtx, 
    rfdevice_trx_phy_path_enum_type                     phy_path
  );

  virtual boolean set_tech_instance
  (
    rfcom_mode_enum_type                                mode, 
    rfdevice_rxtx_enum_type                             rxtx, 
    rfdevice_trx_phy_path_enum_type                     phy_path,
    rfdevice_class*                                     rfdev
  );
#endif
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

  virtual ~rfdevice_trx_common_intf_class();

  /* Below this line are APIs that need to be implemented by new devices implementing
  C++ interface directly. Adapter must implement the APIs for legacy devices using
  C style function pointer table */
  /*! Interface to set Iref of interest */
  virtual boolean set_dac_iref
  (
    rfdevice_dac_iref_val                               iref_val,
    boolean                                             program_iref,
    uint32*                                             agc_gainword
  )
  {
    RF_MSG( RF_ERROR, "set_dac_iref not implemented in derived common class!");
    return FALSE;
  }

  /*! Interface to configure XOGEN  */
  virtual boolean bootup_init
  (
    rf_buffer_intf*                                       script_buf,
    rf_device_execution_type                              execution_type
  )
  {
    RF_MSG( RF_ERROR, "bootup_init not implemented in derived common class!");
    return FALSE;
  }
  virtual rfdevice_trx_gnss_intf_class* get_gnss_instance
  (
    void
  )
  {
    RF_MSG( RF_ERROR, "get_gnss_instance is not implemented in derived common class!");
    return FALSE;
  }

  /*! Interface to sysc RFdevice ustimer with the modem ustimer */
  virtual boolean ustmr_sync
  (
    rf_buffer_intf*                                       script_buf,
    rf_device_execution_type                              execution_type
  )
  {
    RF_MSG( RF_ERROR, "ustmr_sync not implemented in derived common class!");
    return FALSE;
  }

  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type power_up
  (
    rf_device_execution_type                            execution_type,
    rf_buffer_intf*                                     pwrup_setup_script,
    rfdev_cmn_vote_status_type                          rfdev_vote
  ) 
  {
    RF_MSG( RF_ERROR, "power_up not implemented in derived common class!");
    return RFDEV_OFF;
  }

#if 0
  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type power_up_gps( rf_device_execution_type execution_type,
                                              rf_buffer_intf* pwrup_setup_script,
                                              rfdev_cmn_vote_status_type rfdev_vote,
                                              rfdev_cmn_voting_type rfdev_voting_type )
  {
    RF_MSG( RF_ERROR, "power_up_gps not implemented in derived common class!");
    return RFDEV_OFF;
  }

  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type enable_wwan( rf_device_execution_type execution_type,
                                             rf_buffer_intf* pwrup_setup_script,
                                             rfdev_cmn_vote_status_type rfdev_vote,
                                             rfdev_cmn_voting_type rfdev_voting_type)
  {
    RF_MSG( RF_ERROR, "enable_wwan not implemented in derived common class!");
    return RFDEV_OFF;
  }
  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type enable_gps( rf_device_execution_type execution_type,
                                            rf_buffer_intf* pwrup_setup_script,
                                            rfdev_cmn_vote_status_type rfdev_vote,
                                            rfdev_cmn_voting_type rfdev_voting_type )
  {
    RF_MSG( RF_ERROR, "enable_gps not implemented in derived common class!");
    return RFDEV_OFF;
  }

  /*! Interface to perform light sleep or partial shutdown */
  virtual boolean light_sleep(rf_device_execution_type execution_type,
                              rf_buffer_intf* light_sleep_setup_script)
  {
    RF_MSG( RF_ERROR, "light_sleep not implemented in derived common class!");
    return FALSE;
  }

  /*! Backup any state/shadow inside device */
  virtual boolean backup_state( uint8* backup_token )
  {
    RF_MSG( RF_ERROR, "backup_state not implemented in derived common class!");
    return FALSE;
  }

  /*! Restores device state to a previously backed up version represented by the 
    backup_token */
  virtual boolean restore_state( uint8 backup_token )
  {
    RF_MSG( RF_ERROR, "restore_state not implemented in derived common class!");
    return FALSE;
  }
#endif

  /*! Interface to perform WAN or GPS reset/shut off */
  virtual boolean power_reset(rfdev_cmn_power_down_type rfdev_power_down_vote)
  {
    RF_MSG( RF_ERROR, "power_reset not implemented in derived common class!");
    return FALSE;
  }

  /*! Triggers device to execute internal device calibration */
  virtual boolean do_internal_device_cal
  (
    rfdevice_cmn_int_dev_cal_data_type*                 cal_data 
  ) 
  {
    RF_MSG( RF_ERROR, "do_internal_device_cal not implemented in derived common "
                      "class!");
    return FALSE;
  }

#if 0
  /*! Returns legacy C style device pointer */
  virtual void* get_legacy_tech_device( rfm_mode_enum_type rfm_mode,
                                        rfdevice_chain_enum_type chain )
  {
    RF_MSG( RF_ERROR, "get_legacy_tech_device not implemented in derived "
                      "common class!");
    return NULL;
  }

  /*! Enter critical section lock for a device */
  virtual boolean enter_critical_section( void )
  {
    RF_MSG( RF_ERROR, "enter_critical_section not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Leave critical section lock for a device */
  virtual boolean leave_critical_section( void )
  {
    RF_MSG( RF_ERROR, "leave_critical_section not implemented in derived "
                      "common class!");
    return FALSE;
  }
#endif

//UMESP::in 2K it looks like the tx lut script is directly obtained (full script) but in case of 6K, it gets the template (only once) 
//and gets the RGI specific LUT later on and update the LUT data only

  /*! Returns TX scripts and IREF per RGI for a given tech, band, pa_state */
#if 0
  virtual boolean get_tx_lut_scripts( rfm_mode_enum_type tech,
                                      rfcom_band_type_u band_u,
                                      rfcom_bw_type_u bw_u,
                                      uint8 pa_state,
                                      uint8 rgi,
                                      rf_device_execution_type execution_type,
                                      rfdevice_tx_lut_data_type* lut_data,
                                      rfdevice_tx_power_offset_bw_type* pwr_offset_bw) 
#endif
#if 0
  virtual boolean get_tx_lut_scripts
  (
    rfm_mode_enum_type                                    tech,
    rfcom_band_type_u                                     band_u,
    rfcom_bw_type_u                                       bw_u,
    rf_device_execution_type                              execution_type,
    rfdevice_trx_txlut_per_pa_state_type*                 txlut_per_pa_state_arr,
    uint8                                                 txlut_per_pa_state_arr_size,
    rfdevice_trx_txagc_data_buffer_type*                  txagc_data_buffer_arr,
    uint8                                                 txagc_data_buffer_arr_size
  )
#if 1
    {
      RF_MSG( RF_ERROR, "get_tx_lut_scripts not implemented in derived "
                        "common class!");
      return FALSE;
    }
#endif
#endif

  virtual boolean get_rx_dummy_script_trigger
  (
    rf_buffer_intf&                                     script_buf
  )
  {
    RF_MSG( RF_ERROR, "get_rx_dummy_script_trigger not implemented in derived "
                      "common class!");
    return FALSE;
  }

  virtual boolean get_tx_dummy_script_trigger
  (
    rf_buffer_intf&                                     script_buf
  )
  {
    RF_MSG( RF_ERROR, "get_tx_dummy_script_trigger not implemented in derived "
                      "common class!");
    return FALSE;
  }

  virtual boolean get_txagc_template
  (
    rf_buffer_intf&                                     script_buf,
    uint32&                                             data_fixup_token_id,
    rfdevice_trx_txagc_template_type                    template_type
  ) 
  {
      RF_MSG( RF_ERROR, "get_txagc_template not implemented in derived "
                        "common class!");
      return FALSE;
  }

  virtual boolean get_txagc_data
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
  )
  {
    RF_MSG( RF_ERROR, "get_txagc_data not implemented in derived "
            "common class!");
    return FALSE;
  }

  virtual boolean get_rxagc_template
  (
    rf_buffer_intf&                                     script_buf
  )
  {
    RF_MSG( RF_ERROR, "get_combined_rxagc_template not implemented in derived "
                      "common class!");
    return FALSE;
  }

  virtual boolean get_rxagc_trigger
  (
    rf_buffer_intf&  script_buf
  )
  {
    RF_MSG( RF_ERROR, "get_rxagc_trigger not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! RF device register dump function*/
  virtual boolean reg_dump
  (
    rfdevice_reg_dump_cmd_type                          reg_dump_type,
    boolean                                             log_on_off
  )
  {
    RF_MSG( RF_ERROR, "reg_dump not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /* overload the operator '==' to use it to compare two rfdevice rxtx devices pointers*/
  virtual boolean operator==
  (
    rfdevice_trx_common_intf_class                          cmn_rxtx_device
  )
  {
    return ((device_asic_id == cmn_rxtx_device.get_device_asic_id()) &&
            (rf_device_id == cmn_rxtx_device.rfdevice_id_get())&&
            (rf_device_type == cmn_rxtx_device.rfdevice_type_get()))?TRUE:FALSE;
  }

  virtual boolean get_therm_read
  (
    uint16&                                             therm_read_raw,
    int16&                                              therm_read_in_degC,
    boolean                                             is_iotfe_therm = FALSE
  ) 
  {
    RF_MSG( RF_ERROR, "read_therm not implemented in derived "
                      "common class!");
    return FALSE;
    
  }

  virtual boolean get_therm_script
  (
    rf_buffer_intf*                                     script_buf
  ) 
  {
    RF_MSG( RF_ERROR, "read_therm not implemented in derived "
                      "common class!");
    return FALSE;
  }
  

  virtual boolean process_therm_script
  (
    rf_hal_bus_rffe_type*                               raw_rffe_script,
    uint16&                                             therm_read,
    int16&                                              therm_read_in_degC
  )
  {
    RF_MSG( RF_ERROR, "read_therm not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Setup the Rx chains for Rx ADC Vcm Cal  */
  virtual boolean rx_adc_vcm_cal_setup
  (
    rfdevice_rx_vco_path_enum_type                      rx_vco
  )
  {
    RF_MSG(RF_ERROR, "rx_adc_vcm_cal_setup not implemented in derived class " )  ;
    return FALSE;
  }

#ifdef FEATURE_GSM
  /*! Returns the JDET read script for a praticular sample */
  virtual boolean rxsig_get_jdet_read_script
  (
    rf_device_execution_type                            exec_type,
    rf_buffer_intf*                                     script_buf)
  {
    RF_MSG( RF_ERROR, "rxsig_get_jdet_read_script not implemented in derived "
                      "common class!");
  	return FALSE;
  }

  /*! Returns the JDET config script */
  virtual boolean rxsig_get_jdet_config_script
  (
    rf_device_execution_type                            exec_type,
    rfcom_gsm_band_type                                 rf_band, 
    rf_buffer_intf*                                     script_buf)
  {
    RF_MSG( RF_ERROR, "rxsig_get_jdet_config_script not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Turns on the JDET path */
  virtual boolean rxsig_jdet_enable
  (
    rf_device_execution_type                            exec_type,
    rf_buffer_intf*                                     script_buf
  )
  {
    RF_MSG( RF_ERROR, "rxsig_jdet_enable not implemented in derived "
                      "common class!");
    return FALSE;
  }

  virtual boolean rxsig_jdet_enable_set_timer
  ( 
    uint16                                              holdoff_time_us,
    rf_device_execution_type                            exec_type,
    rf_buffer_intf*                                     script_buf
  )
  {
    RF_MSG( RF_ERROR, "rxsig_jdet_enable_set_timer not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Turns off the JDET path */
  virtual boolean rxsig_jdet_disable
  (
    rf_device_execution_type                            exec_type,
    rf_buffer_intf*                                     script_buf
  )
  {
    RF_MSG( RF_ERROR, "rxsig_jdet_disable not implemented in derived "
                      "common class!");
    return FALSE;
  }
#endif /* #ifdef FEATURE_GSM */

  /*! Setup and enable therm */
  virtual boolean calculate_hdet_reading
  (
    int32                                               sum_i,
    int32                                               sum_q,
    uint64                                              sum_i_q_sqr,
    uint32                                              fbrx_accum,
    uint8                                               fbrx_accum_overflow,
    uint16*                                             hdet_reading
  )
  {
    RF_MSG( RF_HIGH, "calculate_hdet_reading not implemented in derived "
                     "common class!");
    return FALSE;
  }

  virtual boolean get_pll_lock_status
  (
    uint8                                               vco_id,
    boolean*                                            is_pll_locked  
  )
  {
    RF_MSG( RF_HIGH, "get_pll_lock_status not implemented in derived "
                     "common class!");
    return FALSE;
  }

  virtual const rfdevice_trx_vco_info_type* get_vco_info
  (
    uint8&                                              vco_info_tbl_size
  )
  {
    RF_MSG( RF_HIGH, "get_vco_info not implemented in derived "
                     "common class!");
    return FALSE;
  }

  virtual boolean get_lte_script_size
  (
    rfdevice_trx_lte_rx_event_size_type&                  rx_events,
    rfdevice_trx_lte_tx_event_size_type&                  tx_events
  )
  {
    RF_MSG( RF_HIGH, "get_lte_script_size not implemented in derived "
                     "common class!");
    return FALSE;
  }

  virtual boolean get_gsm_script_size
  (
    rfdevice_trx_gsm_rx_event_size_type&                  rx_events,
    rfdevice_trx_gsm_tx_event_size_type&                  tx_events
  )
  {
    RF_MSG( RF_HIGH, "get_gsm_script_size not implemented in derived "
	                 "common class!");
    return FALSE;
  }

  virtual boolean get_xogen_dcc_val
  (
    rf_buffer_intf*                                  script_buf,
    rf_device_execution_type                         execution_type,
    rfm_dcc_info_type*                               dcc_info
  )
  {
    RF_MSG( RF_HIGH, "get_pmic_dcc_script not implemented in derived "
                     "common class!");
    return FALSE;
  }

  virtual void reset_iotfe_template_preload_flag()
  {
    RF_MSG( RF_HIGH, "reset_iotfe_template_preload_flag not implemented in derived "
                   "common class!");
  }

  
  virtual void set_rfc_power_class(boolean is_cur_card_pc3)
  {
    RF_MSG( RF_HIGH, "set_rfc_power_class not implemented in derived "
                   "common class!");
  }

};


#else /* #ifdef __cplusplus */

struct rfdev_trx_cmn;
typedef struct rfdev_trx_cmn rfdevice_trx_common_intf_class;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper API to compare two rfdevice_rxtx_common_class objects.
  Can be used in .c files to compare two common rxtx class objects.

  @param cmn_rxtx_device1, cmn_rxtx_device2
  Pointer to common class devices to be compared
  
  @return 
  TRUE if both the devices are same else FALSE.
*/
boolean
rfdevice_compare_common_rxtx_devices
(
  rfdevice_trx_common_intf_class* cmn_rxtx_device_1,
  rfdevice_trx_common_intf_class* cmn_rxtx_device_2
);
#endif
#endif

