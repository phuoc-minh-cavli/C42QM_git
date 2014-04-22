#ifndef RFDEVICE_TRX_GSM_RX_INTF_CLASS_H
#define RFDEVICE_TRX_GSM_RX_INTF_CLASS_H
/*!
   @file
   rfdevice_trx_gsm_rx_intf_class.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_gsm_rx_intf_class.h#1 $

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
03/19/19   bm    Added support for AFC at Rx PLL
09/10/18   tv    Added support to get the num of AUTO_A & AUTO_B transactions in tune script
07/25/18   bm    Added support for WXE double buffer for GSM Rx
07/10/18   bm    Added linearity_mode flag to tune() API
02/06/18   up    Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF) 
                 renamed rfdevice_rxtx_common_class to rfdevice_trx_common_intf_class
01/31/18   smi   Renamed the file: rfdevice_trx_gsm_rx.h/cpp --> rfdevice_trx_gsm_rx_intf_class.h/cpp
01/25/18   smi   Updated below APIs (FR46405)
                      - tune
                      - disable
                      - enter_mode
01/25/18   smi   Removed below api (FR46405)
                      - set_rx_band
                      - tune_rx_direct
                      - build_ip2_cal_setup_script
                      - build_ip2_cal_cleanup_script
                      - update_ip2_cal_results
                      - get_ip2_cal_params
                      - cmd_dispatch
01/25/18   smi   Added below aps  (FR46405)
                      - set_band_data
                      - get_timing_info
                      - get_rsb_data
                      - set_high_lin_mode
                      - set_rx_freq_error 
01/25/18   smi   Added new structure rfdevice_trx_gsm_rx_rsb_info_type
09/16/15   sd    Added support for Rx AGC reload
05/21/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/12/15   sd    Device driver updates for RFC re-design for Split Rx/Tx
12/15/14   sb     Changes to update Jdet timer before jdet on
11/12/14   zg    Added API of get_vco_path
09/19/14   sb    Add compatibility for IP2 cal moving averaging + pass/fail status
08/28/14   shb   Deprecated API to return critical_section_ptr
07/28/14   dps   Added support for LIF feature
07/25/14   hm    Added API to indicate whether Enhanced RX is supported
06/18/14   hm    GSM DRX support to return PRX or PRX+DRX script
06/14/14   sb    Add hooks for sawless support
05/29/14   sb    IP2 cal bug fixes, updates 
04/16/14   sb    IP2 cal Interface and functionality changes
02/24/14   dps   Added rx_init API for rx burst timing
12/20/13   dps   Removed unused APIs
12/10/13   rp    Added an API to get common device pointer associated with 
                 GSM tech class instance.
11/22/13   dps   Replaced build_meas_script and build_meas_script_v2 with an
                 updated version; Updated set_start_script API
11/12/13   dps   Added get_critical_section_ptr; removed device enum params; 
                 Renamed API from build_meas_script_type to build_meas_script
09/19/13   dps   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfcommon_msg.h"
#include "rfdevice_trx_common_intf_class.h"

#ifdef FEATURE_GSM
#include "rfdevice_gsm_type_defs.h"

#ifdef __cplusplus
#include "rfdevice_class.h"

class rfdevice_trx_gsm_rx_intf_class:public rfdevice_class
{

public:
  rfdevice_trx_gsm_rx_intf_class();

  virtual ~rfdevice_trx_gsm_rx_intf_class();

#if 0
  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Enters critical section lock for the device
  */
  virtual boolean enter_critical_section( void ) = 0;

  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Leaves critical section lock for the device
  */
  virtual boolean leave_critical_section( void ) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function will set the GSM band

    @param
    band - Band to be set

    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to set rx band

    @param *script_buffer
    void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally
  */
  virtual boolean set_rx_band(rfcom_gsm_band_type band,
                              rf_device_execution_type execution_type,
                              rf_buffer_intf* script_buffer) = 0;
#endif

virtual boolean set_band_data(rfcom_gsm_band_type band, 
                              uint16 rfc_port,
                              uint32 rx_lna_gain_lut_map) = 0;

  virtual boolean set_rx_band(rfcom_gsm_band_type band,
                              rf_device_execution_type execution_type,
                              rf_buffer_intf* script_buffer) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function tunes the rf device synthesizer to the specified channel for the
    sync Rx burst.

    @details
    Make sure rfgsm_set_band is called first.

    @param
    arfcn - Channel to be tuned
   
    @param
    enh_rx_on_flag - Indicates whether GSM enhanced Rx mode is active

    @param
    enh_rx_freq_offset_hz - Indicates the frequency offset to be applied for enhanced Rx burst
  */
  virtual boolean tune
  (
    uint64 rf_freq_hz,
    int32 freq_error_hz,
    rfcom_gsm_band_type rf_band,
    int32 gain_range, 
    rf_buffer_intf &script_buffer,
    rfdevice_trx_rx_wxebuf_type rx_wxe_buf_type,    
    boolean is_low_lin_mode
  ) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	This function returns the number of auto_a and auto_b  transactions in tune script 
  
	@param num_transactions_auto_b
	Number of transactions within the tune script before the first trigger
  
	@param num_transactions_auto_a
	Number of transactions within the tune script after the first trigger upto second trigger
	
	@retval
	Indicates whether API call is success
  */
  virtual boolean get_num_trans_auto_a_auto_b
  (
	uint16& num_transactions_auto_b,
	uint16& num_transactions_auto_a
  ) = 0;


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function configures the Rx gain for the RF Device.  Note, it also 
    configure the X-tia addresses since the addresses are RF gain dependent.

    @param
    rx_gain_range - RX gain range
  */
  virtual boolean set_rx_gain_range(int32 rx_gain_range)
  {
      return FALSE;
  }

#if 0
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This is called during coarse tune cal or PLL lock test

    @details
    This will write the RF Device Rx script SBI registers directly instead of
    going through mdsp fw.
     
    @param band
    rx band we would like the SBI register to be set.
   
    @param arfcn
    Rx channel for which PLL registers need to be calculated.
   
    @param coarse_tune_cal_flag
    TRUE if the coarse tune cal is active.
  */
  virtual boolean tune_rx_direct(rfcom_gsm_band_type band,
                                 uint16 arfcn,
                                 boolean coarse_tune_cal_flag) = 0;
#endif

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Rx band specific data to device from RFC

    @param rfcom_band
    rfcom_band whose port is being set

    @param data_array
    Array of int32 containing device Rx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return Status flag indicating success or failure
  */
  virtual boolean rx_set_band_data(rfcom_gsm_band_type rfcom_band,
                                   int32* data_array,
                                   uint8 array_size)
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  virtual boolean rx_set_stop_script(rf_buffer_intf *buffer,
                                     rfdevice_trx_gsm_rx_intf_class *paired_device,
                                     rfdevice_rx_mode_type rx_mode)
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function initializes the Rx for GSM operations on target WTR
    If source and target WTRs are different

    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to enter GSM mode

    @param *script_buffer
    Void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally

    @retval TRUE
    device script to enter GSM mode created or executed successfully

    @retval FALSE
    GSM device enter mode failed

  */

  virtual boolean rxonly_init(rf_device_execution_type execution_type,
                              rf_buffer_intf* script_buffer)
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function disables Rx 

    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to enter GSM mode

    @param *script_buffer
    Void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @retval TRUE
    device script to disable rx is created or executed successfully

    @retval FALSE
    GSM device disable RX failed
  */

  virtual boolean disable(rf_device_execution_type execution_type,
                             rfcom_gsm_band_type rf_band,
                             rf_buffer_intf* script_buffer) = 0;


  /*----------------------------------------------------------------------------*/
  /*                          Combined Rx/Tx Functions                          */
  /*----------------------------------------------------------------------------*/


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function initializes the RF Device for GSM operations.
    
    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to enter GSM mode
   
    @param *script_buffer
    Void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally
   
    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @retval TRUE
    device script to enter GSM mode created or executed successfully
   
    @retval FALSE
    GSM device enter mode failed
  */
  virtual boolean enter_mode(rf_device_execution_type execution_type,
                             rf_buffer_intf* script_buffer) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_timing_info(rfdevice_gsm_rx_timing_info_type *timing_info) = 0;

  /*----------------------------------------------------------------------------*/

  virtual boolean get_rsb_data(rfdevice_rsb_coef_type* data) = 0;

  virtual boolean set_high_lin_mode(boolean* data) = 0;

  virtual boolean set_rx_freq_error(int32* data) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function sets the rfc_gsm_core_ptr to point to the right data
    structure, containing, mon/burst timing info, etc.

    @details
    The function sets the rfc_gsm_core_ptr to point to the right data
    structure, containing, mon/burst timing info, etc.

    @param *rfc_gsm_core_ptr
    ptr to the rfc data structure which has the timing info.
  */
  virtual boolean set_rfc_gsm_core_ptr(rfc_gsm_core_config_type *rfc_gsm_core_ptr)
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Configures rf gsm device to operate on the specified band/port 
    (rx_band_port) for the given band (rfcom_band)

    @details
    Ensures rfm_band and tx_band_port are valid and updates instance data to
    store this band/port info for future use

    @param rfcom_band
    rfcom_band whose port is being set

    @param rx_band_port
    rx band/port enum for the rfdevice.

    @param tx_band_port
    tx band/port enum for the rfdevice
  */
  virtual boolean set_band_port(rfcom_gsm_band_type rfcom_band,
                                uint8 rx_band_port,
                                uint8 tx_band_port)
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function generates SBI to switch mode to GSM. It is used for XtoG
    startup and GtoX cleanup

    @details
    The function generates SBI to switch mode to GSM. It is used for XtoG
    startup and GtoX cleanup

    @param gsm_meas_param
    Parameters needed to generate the script
   
    @param *script_data_ptr
    Structure with input/output parameters needed for IRAT scripts.
   
    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @return
    Success/Failure flag
  */
  virtual boolean build_meas_script
  (
    rfdevice_gsm_meas_param_type gsm_meas_param,
    rfdevice_meas_script_data_type* script_data_ptr,
    rfdevice_trx_gsm_rx_intf_class *paired_device,
    rfdevice_rx_mode_type rx_mode
  )
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function generates start/tune script for RX or TX

    @details
    The function generates start/tune script for RX or TX

    @param oper_mode
    Parameter to indicate Stand alone or IRAT mode.
    used to determine WXE cell
   
    @param *script_infor
    Parameter to indicate type of burst and to retrieve timing information

    @param *buffer
    Pointer to buffer_intf object where script needs to be appended.

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @param inc_drx
    Flag to indicate whether drx script should be part of prx script
    If inc_drx is false, script is returned for PRX tune operation
    If inc_drx is true, script is returned for PRX + DRX tune operation.
    This flag is used in scenarios where seperate calls cannot be made,
    for PRX and DRX becasue of timing constraints
   
    @param wtr_config_bitmask
    Pointer to a bitmask which will be populated for logging WTR configuration

    @param use_alt_gain
    Parameter specifying whether to use default gain settings or
    the gain settings due to antenna switch

    @return
    Success/Failure flag
  */
  /*----------------------------------------------------------------------------*/
  virtual boolean set_start_script(rfdevice_operation_mode oper_mode, 
                                   rfgsmlib_sbi_burst_struct *script_infor, 
                                   rf_buffer_intf *buffer,
                                   uint16 *buffer_mask,
                                   rfdevice_trx_gsm_rx_intf_class *paired_device,
                                   rfdevice_rx_mode_type rx_mode,
                                   boolean inc_drx = FALSE,
                                   rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                                   rfdevice_wtr_config_bitmask_type *wtr_config_bitmask_drx = NULL,
                                   boolean use_alt_gain = FALSE)
  {
      return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual boolean rx_init(rfdevice_trx_gsm_rx_intf_class *paired_device,
                          rfdevice_rx_mode_type rx_mode)
  {
      return FALSE;
  }
  /*----------------------------------------------------------------------------*/

#if 0
  virtual boolean build_ip2_cal_setup_script(rfcom_gsm_band_type rfcom_band, 
                                             int32 min_im2_val, 
                                             uint8 min_im2_step_idx,
                                             boolean first_iter, 
                                             rf_buffer_intf *buffer)
  {
    RF_MSG(RF_HIGH,"build_ip2_cal_setup_script not implemented in base class");
    return FALSE;
  }


  virtual boolean build_ip2_cal_cleanup_script(rfcom_gsm_band_type rfcom_band, rf_buffer_intf *buffer)
  {
    RF_MSG(RF_HIGH,"build_ip2_cal_cleanup_script not implemented in base class");
    return FALSE;
  }

  virtual boolean build_ip2_cal_step_script(rfcom_gsm_band_type rfcom_band, 
                                            rf_buffer_intf **buffer, 
                                            rfgsm_ip2_cal_alg_params_struct* ip2_cal_info,
                                            boolean first_iter,
                                            rfgsm_ip2_cal_grid_info_type *grid_info_ptr)
  {
    RF_MSG(RF_HIGH,"build_ip2_cal_step_script not implemented in base class");
    return FALSE;
  }

  virtual boolean get_ip2_cal_params(boolean default_flag,
                                     rfgsm_ip2_cal_params_type* ip2_cal_params_ptr, 
                                     rfcom_gsm_band_type rfcom_band)
  {
      RF_MSG(RF_HIGH,"get_ip2_cal_params not implemented in base class");
      return FALSE;
  }

  virtual boolean update_ip2_cal_results(rfcom_gsm_band_type rfcom_band, 
                                         uint8 min_im2_step_idx,
                                         int32 min_im2_val,
                                         boolean first_iter,
                                         boolean write_to_efs)
  {
      RF_MSG(RF_HIGH,"update_ip2_cal_results not implemented in base class");
      return FALSE;
  }
#endif

  virtual boolean get_sawless_support(rfcom_gsm_band_type rfcom_band, uint8* flag)
  {
      return FALSE;
  }



  virtual boolean update_jdet_timing(void* time_info)
  {
      return TRUE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function indicates whether enhanced rx is supported

    @details
    The function indicates whether enhanced rx is supported

    @param is_enh_rx_supported
    pointer to a boolean flag. TRUE if enhanced rx is supported, FALSE if not.
   
    @return
    Success/Failure flag
  */
  virtual boolean get_enh_rx_support(boolean *is_enh_rx_supported) = 0;


  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
  */
  virtual 
  rfdevice_trx_common_intf_class* get_common_device_pointer() = 0;

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Query Rx VCO path data from TRX reciever

  @param data_array
  Array of int32 containing device Rx data for band

  @param array_size
  Size of array of int32 data to be pushed to device

  @return
  VCO path number 
  */
  virtual rfdevice_rx_vco_path_enum_type get_vco_path
  (
    int32* data_array,
    uint8 array_size
  )
  {
    RF_MSG( RF_ERROR, "rx_get_vco_path: Derived class does not support this API!" );
    return WTR_RX_INVALID_VCO_PATH;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean rx_pwr_sleep( void )
  {
    RF_MSG( RF_ERROR, "rx_pwr_sleep: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean rx_pwr_wakeup( void )
  {
    RF_MSG( RF_ERROR, "rx_pwr_wakeup: Derived class does not support this API!" );
    return FALSE;
  }


};

#else

struct rfdev_gsm_rx;
typedef struct rfdev_gsm_rx rfdevice_trx_gsm_rx_intf_class;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_trx_common_intf_class* 
rfdevice_gsm_rx_get_common_device_pointer(rfdevice_trx_gsm_rx_intf_class* rx_device);


#endif /* #ifdef __cplusplus */

#endif /* FEATURE_GSM */

#endif /* RFDEVICE_TRX_GSM_RX_H */
