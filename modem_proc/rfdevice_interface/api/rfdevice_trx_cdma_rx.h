#ifndef RFDEVICE_TRX_CDMA_RX_H
#define RFDEVICE_TRX_CDMA_RX_H
/*!
   @file
   rfdevice_trx_cdma_rx.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_cdma_rx.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
09/16/15   sd    Interface changes for Rx AGC reload
07/31/15   zhw   Compiler error fix
05/19/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/13/15   ry    Device interface updates for RFC re-design for split Rx/Tx
01/16/15   shb   Added get_discontinuous_rx_script()
11/13/14   zg    Added new API of get_vco_path
08/12/14   shb   Removed default NULL arg for spur mitigation params
08/01/14   lmiao Added new multi_carrier_droop_compensation() API to support cdma mcdo
06/19/14   bm    Updated create_tune_script() interface for SSMA support
03/09/14   pk    Added interface to query DLNA support
12/09/13   rp    Added an interface to retrieve common device pointer associated with 
                 given tech trx class.
11/13/13   shb   Corrected arg in CDMA meas script API
11/11/13   shb   Added new CDMA RX create_meas_script API
11/07/13   rp    Added and API to return common device class pointer.
08/29/13   shb   Added FEATURE_1X for modem_lite build compilation
08/27/13  shb/sr Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfcommon_msg.h"
#ifdef FEATURE_CDMA1X
#include "rfm_cdma_power_mode_types.h"
#include "rfdevice_cdma_type_defs.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_rxtx_common_class.h"

class rfdevice_trx_cdma_rx:public rfdevice_class
{

public:
  rfdevice_trx_cdma_rx();

  virtual ~rfdevice_trx_cdma_rx();


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
    Creates a script that puts RX in RTR to sleep
    
    @details
    This function will generate a RTR Rx Device script for FW. The RF
    device passed into this function will be mapped into a physical path of the
    current RTR chipset by querying RFC.
  
    This function will not perform any SBI transaction or any actions on hardware
    
    @param *buf_obj_ptr
    Data structure in which the all transactions are to be populated

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if Rx chain is used by tech for PRx/DRx operation
   
    @param rx_bw_khz
    Rx Band-width of operation in KHz.
  
    @param tx_bw_khz
    Tx Band-width of operation in KHz.

  */
  virtual boolean create_sleep_script(rf_buffer_intf *buf_obj_ptr,
                                      rfdevice_trx_cdma_rx* paired_device,
                                      rfdevice_rx_mode_type rx_mode,
                                      uint32 rx_bw_khz,  
                                      uint32 tx_bw_khz) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Creates a script that puts RX in RTR to sleep
    
    @details
    This function will generate a RTR Rx Device script for FW. The RF
    device passed into this function will be mapped into a physical path of the
    current RTR chipset by querying RFC.
  
    This function will not perform any SBI transaction or any actions on hardware
   
    @param *rx_device
    Pointer to rx_device of rfdevice_cdma_rx_type on which diversity will be 
    disabled
   
    @param *buf_obj_ptr
    Data structure in which the all transactions are to be populated

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate Rx chain is used by tech for PRx/DRx 
    operation
   
    @param rx_bw_khz
    Rx Band-width of operation in KHz.
  
    @param tx_bw_khz
    Tx Band-width of operation in KHz.
  */
  virtual boolean create_disable_diversity_script(rf_buffer_intf *buf_obj_ptr,
                                                  rfdevice_trx_cdma_rx* paired_device,
                                                  rfdevice_rx_mode_type rx_mode,
                                                  uint32 rx_bw_khz,  
                                                  uint32 tx_bw_khz ) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Program CDMA Rx tune script
    
    @details
    This function will generate the RTR Rx Device tuning script for FW. The RF
    device passed into this function will be mapped into a physical path of the
    current RTR chipset by querying RFC.
  
    This function will not perform any SBI transaction or any actions on hardware
    
    @param mode
    RF Mode for which RF Rx Device Tune script is to be createds
   
    @param band
    Rx Band for which the script is to be generated
   
    @param chan
    Rx Channel for which the script is to be generated
   
    @param rx_bw_khz
    Rx Band-width of operation in KHz.
  
    @param tx_bw_khz
    Tx Band-width of operation in KHz.
   
    @param power_mode
    Power Mode for which the script is to be generated
    
    @param *buf_obj_ptr
    Data structure in which the all transactions are to be populated
   
    @param script_mode
    Input flag indicating whether the script being generated is for a
    temporary tune away for measurement (IRAT) or for a standalone RX tune.
    This is used to determine if device should set its internal state variables
    to a new state based on this function execution

   @rf_cdma_program_wtr_notch_cb
   Callback function pointer from rf cdma tech layer that would program WTR
   notches to the modem
  
   @cb_data
   Pointer to a structure from rf cdma tech layer where all the WTR notch
   data would be populated before invoking the callback function
   
   @param wtr_config_bitmask
   Pointer to a bitmask which will be populated for logging WTR configuration

   @param use_alt_gain
   Parameter specifying whether to use default gain settings or
   the gain settings due to antenna switch
    
  */
  virtual boolean create_tune_script(rfm_mode_enum_type mode,
                                     rfm_cdma_band_class_type band,
                                     rfm_cdma_chan_type chan,
                                     uint32 rx_bw_khz,
                                     uint32 tx_bw_khz,
                                     rfm_cdma_power_mode_type power_mode,
                                     rf_buffer_intf *buf_obj_ptr,  
                                     rfdevice_cdma_script_operation_type script_mode,
                                     rf_cdma_spur_cb_type rf_cdma_program_wtr_notch_cb,
                                     rfdevice_cdma_ssma_callback_data_type* cb_data,
                                     rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                                     boolean use_alt_gain = FALSE) = 0;
                                     
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Create measurement (IRAT) script for CDMA
    
    @details
    Generates preload and trigger scripts for CDMA measurement - tune away or
    tune back.
    
    @param band
    Rx Band for which the script is to be generated
   
    @param chan
    Rx Channel for which the script is to be generated
   
    @param rx_bw_khz
    Rx Band-width of operation in KHz.
  
    @param tx_bw_khz
    Tx Band-width of operation in KHz.
   
    @param power_mode
    Power Mode for which the script is to be generated
   
    @param meas_type
    Indicates whether this is a startup (tune away) script or
    cleanup (tune back) script
  
    @param *script_data_ptr
    Structure with input/output parameters needed for IRAT scripts.
    
    script_data_ptr->preload_action
    is an input param indicating whether preload_script should be executed
    internally by device, returned in buffer provided or skipped
    altogether (preload was already done)
    
    script_data_ptr->preload_buf
    Buffer ptr where preload script should be populated IF
    script_data_ptr->preload_action == RFDEVICE_MEAS_CREATE_SCRIPT.
    Preload script can be executed before the measurement gap without
    disrupting the source RAT.
    
    script_data_ptr->meas_action
    is an input param indicating whether trigger_script should be executed
    internally by device, returned in buffer provided or skipped
    altogether (meas script will be generated later)
    
    script_data_ptr->meas_buf
    Buffer ptr where meas script should be populated if
    script_data_ptr->meas_action == RFDEVICE_MEAS_CREATE_SCRIPT.
    Meas script should be executed within the IRAT measurment gap as its
    execution will immediately disrupt source RAT and tune for away measurement.

    @param use_alt_gain
    Parameter specifying whether to use default gain settings or
    the gain settings due to antenna switch
   
    @return
    Success/Failure flag
  */
  virtual boolean create_meas_script
  (
     rfm_mode_enum_type mode,
     rfm_cdma_band_class_type band,
     rfm_cdma_chan_type chan,
     uint32 rx_bw_khz,
     uint32 tx_bw_khz,
     rfm_cdma_power_mode_type power_mode,
     rfdevice_meas_scenario_type meas_type,
     rfdevice_meas_script_data_type *script_data_ptr,
     boolean use_alt_gain = FALSE
  )
  {
    RF_MSG( RF_ERROR, "create_meas_script: Derived class does not support this "
                      "API!" );
    return FALSE;
  }
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Obtain RF device settings needed to enable the given Rx path
    
    @details
    This function creates a script with RTR setttings needed to enable diversity
    on a given Rx path. The RF device passed into this function will be mapped
    to a physical path of the current RTR chipset by querying RFC.
  
    This function will not perform any SBI transaction or any actions on hardware
    
    @param mode
    RF Mode for which RF Rx Device Tune script is to be createds
   
    @param band
    Rx Band for which the script is to be generated
   
    @param chan
    Rx Channel for which the script is to be generated
   
    @param rx_bw_khz
    Rx Band-width of operation in KHz.
  
    @param tx_bw_khz
    Tx Band-width of operation in KHz.
   
    @param power_mode
    Power Mode for which the script is to be generated
    
    @param *buf_obj_ptr
    Data structure in which the all transactions are to be populated  

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate Rx chain is used by tech for PRx/DRx 
    operation
   
    @param wtr_config_bitmask
    Pointer to a bitmask which will be populated for logging WTR configuration

    @param use_alt_gain
    Parameter specifying whether to use default gain settings or
    the gain settings due to antenna switch
  */
  virtual boolean create_enable_diversity_script(rfm_mode_enum_type mode,
                                                 rfm_cdma_band_class_type band, 
                                                 rfm_cdma_chan_type chan,
                                                 uint32 rx_bw_khz,
                                                 uint32 tx_bw_khz,
                                                 rfm_cdma_power_mode_type power_mode,
                                                 rf_buffer_intf *buf_obj_ptr,
                                                 rfdevice_trx_cdma_rx* paired_device,
                                                 rfdevice_rx_mode_type rx_mode,
                                                 rfdevice_operation_mode operation_mode = RFDEVICE_OPER_MODE_STANDALONE,
                                                 rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                                                 boolean use_alt_gain = FALSE ) = 0;

  
  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Gets script to set LNA to a particular gain state
   
    @details
    This function should be used to get LNA gain word script for one LNA state
    at a time. Also it populates the script in rf_buffer_intf* provided
   
    @param *rx_device
    "this" pointer to receive device whose LNA gain word is queried
   
    @param band
    CDMA band whose LNA gain word is required.
   
    @param power_mode
    Linearity of operation
   
    @param rx_bw_khz
    BW in khz of operation. LNA gain word may be different for single carrier
    vs multi carrier
   
    @param gain_state
    LNA gain state whose script is needed
   
    @param buf_obj_ptr
    Pointer to rf script buffer object where LNA gain script will be populated

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate Rx chain is used by tech for PRx/DRx operation
   
    @retval TRUE
    LNA gain word script was successfully populated for the arguments provided
   
    @retval FALSE
    LNA gain word script population failed! buf ptr was not updated
  */
  virtual boolean create_lna_gain_script(rfm_cdma_band_class_type band,
                                         rfm_cdma_power_mode_type power_mode,
                                         uint32 rx_bw_khz,
                                         rfm_lna_gain_state_type gain_state,
                                         rf_buffer_intf *buf_obj_ptr,
                                         rfdevice_trx_cdma_rx* paired_device,
                                         rfdevice_rx_mode_type rx_mode) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Generic interface for RF RX device functions that do not merit its own
    function pointer in device virtual function table
  
    @details
    It provides IOCTL kind of interface to get and set device specific
    parameters. Must be used for functions that are specific to a RF device to
    avoid forcing other devices that do not support that functionality to
    implement the function. Also may be used for simple device data queries 
    that do not deserve a seperate function in virtual function table.
    
    @param *rx_device
    Pointer to rx_device of rfdevice_cdma_rx_type that should be operated upon
  
    @param cmd
    Specifies the command to performed by the device. Should be from enum
    rfdevice_cmd_enum_type
  
    @param *data
    Void data pointer to handle input and/or output parameter for the command.
   
    @return
    TRUE/FALSE flag that generally indicates success or failure but its
    specific meaning may depend on the cmd being executed. It is the status
    returned by the device command dispatch function pointer
  */
  virtual boolean cmd_dispatch(rfdevice_cmd_enum_type cmd,
                               void *data) = 0;


  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  This API will load cdma mcdo data into the input paramter based on 
  selected bandwidth. This API may be overrided by different sub classes.  

  @param data
  data buffer where cdma mcdo data will be stored, should be allocated 
  prior to this function call

  @param bwHz
  Bandwidth (in Hz) selected for specific mcdo data

  @return
  Success/Failure flag

 */

  virtual boolean multi_carrier_droop_compensation(rfdevice_cdma_multi_carrier_droop_compensations_type_v2 *data,
                                uint32 bwHz)=0;
 /*----------------------------------------------------------------------------*/

  /*!
    @brief
    Do post-PLL lock processing
  
    @param *rx_device
    RX device for which post-PLL lock processing is to be done
  */
  virtual boolean post_pll_lock_processing(void)
  {
    return TRUE;
  }

  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Set linearity of CDMA RF receive device
  
    @details
    RF device will be re-programmed for new linearity but same band/chan
    
    @param *rx_device
    RX device to be retuned for new power_mode
  
    @param power_mode
    rfm_cdma_power_mode_type power mode of operation
  */
  virtual boolean ic_set_linearity(rfm_cdma_power_mode_type power_mode)
  {
    RF_MSG( RF_ERROR, "ic_set_linearity() not supported" );
    return FALSE;
  }

  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Query device driver for intelliceiver support
  
    @details
    This function will check if the device driver supports intelliceiver on a
    given recieve path.
  
    @param *rxdev
    Pointer to rx device for which the intelliceiver check to be performed.
   
    @return
    Flag indicating, if the intelliceiver is supported for current RTR instance
  */
  virtual boolean is_ic_supported(void) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Query device driver for DLNA support
  
    @details
    This function will check if the device driver supports DLNA on a
    given recieve path.
  
    @param *rxdev
    Pointer to rx device for which the DLNA check to be performed.
   
    @return
    Flag indicating, if the DLNA is supported for current WTR instance
  */
  virtual boolean is_dlna_supported(void) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Programs Intelliceiver to trip at the requested switch points
   
    @details
    This function is used to push the intelliceiver calibration data to the
    proper Device driver instance so that the driver can compute the final
    switchpoint to trip the intelliceiver.
   
    @param rx_device
    Pointer to rx device for which the intelliceiver Cal data is to be pushed
   
    @param cal_data
    Pointer to the intelliceiver calibration data to be pushed into the device
  
    @param band
    CDMA band for which calibration data is being set in device
  
    @param power_mode
    CDMA Power mode for which calibration data should be applied to device
   
    @return
    Success/Failure flag
  */
  virtual boolean set_ic_cal_data(rfdevice_cdma_ic_thres_param_type *cal_data,
                                 rfm_cdma_band_class_type band,
                                 rfm_cdma_power_mode_type power_mode)
  {
    RF_MSG( RF_ERROR, "set_ic_cal_data() not supported!");
    return FALSE;
  }

  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get intelliceiver State
  
    @details
    This function will query the current RTR instance to know the current
    intelliceiver jammer state. This does not necessarily inidicate the current
    linearity of RTR. This just indicates the jammer status only.
  
    @param *rxdev
    Pointer to rx device for which the intelliceiver state to queried.
   
    @return
    Current State of intellicever for given RTR instance
  */
  virtual rfm_cdma_power_mode_type get_ic_state(void) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*! 
    @details
    Perform Intelliceiver Calibration
   
    @details
    This function will call the current instance of RTR driver to perform
    intelliceive calibration to estimate the the threshold gain at which the
    intelliceiver will trip.
   
    @param rx_device
    Pointer to the current instance of RTR driver.
   
    @param power
    Power at which intelliceiver is calibrated
   
    @return
    Structure containing the intellicever calibration data which contains the
    threshold gain at which the intelliceiver tripped.
  */
  virtual rfdevice_cdma_ic_cal_data_type ic_calibration(int16 power)
  {
    rfdevice_cdma_ic_cal_data_type cal_data;

    cal_data.cal_status = FALSE;
    cal_data.cal_data = 0;
    
    RF_MSG( RF_ERROR, "CDMA IC cal not supported!!" );
    
    return cal_data;
  }

  
  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Reset Jammer Detector Status
   
    @details
    This function will reset the jammer detector status for the given device.
   
    @param rx_device
    Pointer to current instance of RTR driver for which the jammer detector is
    to be reset.
  */
  virtual boolean ic_reset_jammer_detector(void)
  {
    RF_MSG( RF_ERROR, "ic_reset_jammer_detector: no jd in device!");

    return FALSE;
  }

  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Query for the CDMA Power Mode Configuration supported by RF Device
  
    @details
    This function will check if the device driver supports intelliceiver on a
    given recieve path. If IC is supported, it returns the correct power mode
    configuration based on device, band and bandwidth.
   
    @param *rxdev
    Pointer to rx device for which the power mode config must be returned.
   
    @param band
    Band for which we're requesting power mode configuration
   
    @param rx_bw_khz
    Rx bandwidth for which driver is requesting power mode configuration

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate Rx chain is used by tech for PRx/DRx operation
   
    @return
    Device Power mode configuration
  */
  virtual rfdevice_cdma_ic_power_mode_config_type get_ic_pm_config(rfm_cdma_band_class_type band,
                                                                   uint32 rx_bw_khz,
                                                                   rfdevice_trx_cdma_rx* paired_device,
                                                                   rfdevice_rx_mode_type rx_mode) = 0;

  
  /*============================================================================*/
  /*!
    @name Device Parameter Query
  
    @brief
    Interfaces for RF Upper Layer to query device parameter for upper layer usage
  */
  /*! @{ */
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get the LNA Phase offset to be applied for current device mode
   
    @details
    This function will return the appropriate LNA phase offset for the current
    mode of device. The LNA phase offsets returned by this function depends on
    the current band class, power mode, and SHDR mode.
   
    LNA Offsets returned by this function will be in the units of degrees with
    the range of 0 to 360. Thus the MC layer should convert this phase offsets to
    appropriate modem specific HW units before applying.
   
    @param rx_device
    Pointer to current instance of RTR driver for which the LNA phase offset is
    to be applied
   
    @param band
    RF CDMA band for which the LNA phase offset is queried
   
    @param chan
    Channel for which LNA phase offset is queried.
   
    @param power_mode
    Current power mode for which the LNA phase offsets are required.
   
    @param state
    RF CDMA state of receive device. Settings may be different for Diversity vs
    SHDR mode
   
    @param rx_bw_khz
    RX carrier Bandwidth in kHz. Used to select bandwidth specific phase offset
    for MCDO
   
    @param lna_phase
    Container to populate the LNA phase offsets for given configuration
   
    @return
    Flag indicating the validity of LNA phase offsets populated in lna_phase
  */
  virtual boolean get_lna_phase_offsets(rfm_cdma_band_class_type band,
                                        rfm_cdma_chan_type chan,
                                        rfm_cdma_power_mode_type power_mode,
                                        rf_cdma_state_type state,
                                        uint32 rx_bw_khz,
                                        rfdevice_cdma_lna_phase_offset_type *lna_phase) = 0;

  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Rx band specific data to device
    
    @param *rx_device
    "this" pointer to RX device that needs to be configured
   
    @param band
    CDMA band whose data is being pushed
   
    @param data_array
    Array of int32 containing device Rx data for band
   
    @param array_size
    Size of array of int32 data to be pushed to device
  */
  virtual boolean set_band_data(rfm_cdma_band_class_type band,
                                int32* data_array,
                                uint8 array_size) = 0;

  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get SSBI script for enabling/disabling Rx chain
    
    @param *rx_device
    "this" pointer to RX device for which on/off script is needed
   
    @param rf_on
    TRUE for Rx ON event, FALSE for Rx OFF event
   
    @param script_buffer
    Pointer to the buffer script for storing on/off settings
   
    @param time_delays_us
    Delay in microseconds
  */
  virtual boolean get_rx_on_off_script(boolean rf_on,
                                       rf_buffer_intf* script_buffer,
                                       int16 time_delays_us) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    /*!
    @brief
    Gets device script to enable or disable Rx chain for discontinuous Rx operation
   
    @details
    The wrapper will loop thru all RX devices and append their rx on/off  
    scripts in the same buffer
   
    If any rx_device API fails, the wrapper API will fail.
  
    @param rx_device
    Rx device pointer that needs to be enabled/disabled
   
    @param band
    CDMA Band of operation
    
    @param script_buffer
    RF script buffer where device writes have to be appended
   
    @return
    Success/Failure status of API
  */
  virtual boolean get_discontinuous_rx_script(rfm_cdma_band_class_type band,
                                              boolean rx_on,
                                              rf_buffer_intf* script_buffer)
  {
    RF_MSG(RF_ERROR,"rfdevice_trx_cdma_rx_class:"
      "Derived class has not implemented the get_discontinuous_rx_script API");

    return FALSE;
  }


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get rfdevice_rxtx_common_class pointer associated with this instance of device.

    @return 
    Pointer to rxtx common device associated with current instance of device.
  */
  virtual 
  rfdevice_rxtx_common_class* get_common_device_pointer()
  {
    RF_MSG(RF_ERROR,"rfdevice_trx_cdma_rx_class:"
      "Derived class has not implemented the get common device poiner API");

    return NULL;
  }

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

struct rfdev_cdma_rx;
typedef struct rfdev_cdma_rx rfdevice_trx_cdma_rx;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_rxtx_common_class* 
rfdevice_cdma_rx_get_common_device_pointer(rfdevice_trx_cdma_rx* rx_device);


#endif /* #ifdef __cplusplus */

#endif /* FEATURE_CDMA1X*/

#endif /* RFDEVICE_TRX_CDMA_RX_H */
