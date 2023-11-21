#ifndef RFDEVICE_TRX_GSM_TX_INTF_CLASS_H
#define RFDEVICE_TRX_GSM_TX_INTF_CLASS_H
/*!
   @file
   rfdevice_trx_gsm_tx_intf_class.h

   @brief

*/

/*===========================================================================

Copyright (c) 2018 - 2020 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_gsm_tx_intf_class.h#2 $

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
04/13/20   up    Added API get_fsk_afc_script
05/29/19   up    Removed env_delay param in tune API 
09/24/18   bm    Updated the data type of env_delay argument in tune API
27/07/18   tv    RGI and tune API modified to provide timer fixup token ID &
                 RGI API updated to support double buffer
05/21/18   hkm   RGI API update 
04/17/18   am    Added API get_tx_delay_backoff
04/05/18   hkm   tx disable update
03/13/18   bm    Updated interface for tune API
02/06/18   smi   compilation error fixes
01/31/18   smi   Renamed the file name rfdevice_trx_gsm_rx.h/cpp --> rfdevice_trx_gsm_rx_intf_class.h/cpp
01/25/18   smi   updated interface of below api
                      - enter_mode
                      - set_band
                      - tune
                      - enable_kv_cal
                      - execute_kv_cal
                      - set_kv_cal_code
                      - compute_final_kv_cal
01/25/18   smi   Removed below API
                      - set_rgi
                      - cmd_dispatch
01/25/18   smi   added below api
                      - set_band_data
                      - get_rgi_script
                      - get_timing_info
                      - set_tx_alpha_time
                      - get_updated_txagc_info
                      - set_timed_writes
                      - clear_timed_writes
                      - set_tx_freq_error
01/25/18   smi   Added new micro RFDEVICE_TRX_GSM_MAX_TX_SLOTS
05/21/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
11/19/14   dps   Added APIs for FBRx Self-test support
08/28/14   hm    Added/Modified APi's for TX DSDA support
04/10/14   dps   Changed return type for get_num_therm_regs_to_read API to allow for a
                 negative return value for failure cases
03/11/14   dps   Added API to get number of therm registers to read
                 removed prog_cal_tx_burst_sbi API
12/20/13   dps   Added set_rfc_gsm_core_ptr and enter_mode APIs
12/10/13   rp    Added an API to get common device pointer associated with 
                 GSM tech class instance.
10/22/13   dps   Updated set_start_script API
11/19/13   dps   Added get_trans_script API
11/12/13   dps   Added get_critical_section_ptr and removed device enum params
09/19/13   dps   Fixed return value for set_trans_script
09/19/13   dps   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_trx_common_intf_class.h"
//#include "rflm_cmn_intf.h"
#include "rflm_rfdevice_typedefs.h"

#include "rfgsm_mdsp.h"


#define RFDEVICE_TRX_GSM_MAX_TX_SLOTS 5

#ifdef FEATURE_GSM
#include "rfdevice_gsm_type_defs.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
/*! @brief Structure containing timing info for GSM Tx events */

class rfdevice_trx_gsm_tx_intf_class:public rfdevice_class
{

public:
  rfdevice_trx_gsm_tx_intf_class();

  virtual ~rfdevice_trx_gsm_tx_intf_class();

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
#endif 

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function initializes the RF Device for GSM operations.
    
    @param * script_data_ptr
    Pointer to rfdevice_meas_script_data_type
   
    @retval TRUE
    device script to enter GSM mode created or executed successfully
   
    @retval FALSE
    GSM device enter mode failed
  */
  virtual boolean enter_mode(  rf_device_execution_type execution_type,
                                   rf_buffer_intf* script_data_ptr) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function reads the thermistor value from the RF Device HKADC.

    @details
    This functions read thermistor value inside of HKADC. 

    @return
    temperature uint16 12 bits resolution therm_value.
    0xFFFF indicates invalid result
    low result value indicates high temperature
  */
  virtual uint16 therm_read( void )
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function setup the RF device to handle band dependent Tx setting

    @details
    It should be called way before the actual RX/TX burst.

    @param band
    Band to be set

    @param * script_data_ptr
    Pointer to rfdevice_meas_script_data_type
  */
  virtual boolean set_band
  (
    rfcom_gsm_band_type rf_band,
    rf_buffer_intf* script_buffer
  ) = 0;


  virtual boolean set_band_data
  (
    rfcom_gsm_band_type band,
    uint16 rfc_port,
    uint8 etdac_chain
  )=0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function tunes the rf device synthesizer to the specified channel for the
    sync Tx burst.

    @param arfcn
    channel to be tuned
  */
  virtual boolean tune(  uint64 rf_freq_hz,
                        int32 freq_error_hz,
                        rfcom_gsm_band_type rf_band,
                        rf_buffer_intf *script_buffer,
                        rfdevice_trx_timer_fixup_type (&timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
                        uint8& num_timer_fixups
    )=0;

   /*----------------------------------------------------------------------------*/
   /*!
     @brief
     This function does the rf disable for tx
   
     @param band, rfm_dev
   */

  virtual boolean  disable(rfcom_gsm_band_type rf_band,
                              rf_buffer_intf *script_buffer
    )=0;

#if 0
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function sets rgi. Note that rgi and power are related.
    i.e. As rgi increases power increases.

    @param rfgsm_band
    Current GSM band

    @param rgi
    RF Gain Index

    @param mod_type
    Modulation type

    @param slot_num
    Number of slots

    @param set_script
    Flag to add settings to the script, or write direct SSBI

  */
  virtual boolean set_rgi(uint16 rgi,
                          rfgsm_modulation_type mod_type,
                          uint8 slot_num,
                          boolean set_script) = 0;
#endif

  /*----------------------------------------------------------------------------*/
  virtual boolean get_rgi_script
  (
    rfcom_gsm_band_type rf_band,
    uint32 rf_freq_khz,  
    uint8 num_slots,
    uint16 rgi[RFDEVICE_TRX_GSM_MAX_TX_SLOTS],
    rfgsm_modulation_type mod_type[RFDEVICE_TRX_GSM_MAX_TX_SLOTS],
    rf_buffer_intf *script_buffer[RFDEVICE_TRX_GSM_MAX_TX_SLOTS],
    rfdevice_trx_timer_fixup_type (&timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
    uint8& num_timer_fixups,
    rfdevice_trx_rx_wxebuf_type tx_wxe_buf_type
  )=0;



  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function will enable/disable Phase modulation.

    @param flag
    Flag to enable/disable phase modulation
  */

  virtual boolean toggle_phase_mod
  (
    rfcom_gsm_band_type rf_band,
    boolean phase_on_off,
    rf_buffer_intf *script_buffer
  )=0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function will enable/disable kv adaptation.

    @param flag
    Flag to enable/disable kv adaptation
  */
  virtual boolean enable_kv_cal(boolean flag)
  {
    RF_MSG(RF_HIGH,"enable_kv_cal not implemented in base class");
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function executes the KV calibration routine.

    @param kv_value

    @param num_iter
  */
  virtual boolean execute_kv_cal(rfgsmlib_kv_code_type *kv_value,
                                 uint8 num_iter)
  {
    RF_MSG(RF_HIGH,"execute_kv_cal not implemented in base class");
    return FALSE;
  }


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function set the kv calibration code based on the value passed in.

    @param code
  */
  virtual boolean set_kv_cal_code(uint16 code)
  {
    RF_MSG(RF_HIGH,"set_kv_cal_code not implemented in base class");
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function computes final KV code

    @details
    This function computes the final KV code based on the average KV and 
    acquisition code.  The desired code is 0x88 (value for Reg4D).  The KV 
    and acquisition codes are inversely proportion.  The equation to compute
    the final KV code is: final_kv = (acq_code - 0x88) + kv_code

    @param kv
    Averaged KV code

    @param r1
    Averaged Fine Tune Code
  */
  virtual int16 compute_final_kv_cal(int16 kv,
                                     uint16 r1)
  {
    RF_MSG(RF_HIGH,"compute_final_kv_cal not implemented in base class");
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function returns the start time for first SBI transaction sent during a TX burst.

    @details
    SBI clock regime must be enabled.

    @retval
    Start time for first SBI transcation sent during a TX burst
  */
  virtual int16 get_tx_sbi_start_delta( void )
  {
    return FALSE;
  }
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function turns off Phase modulation in DPLL. It is used
    only for Linear AMAM cal. 

    @details
    This function will will turn off phase modulation in DPLL. This is required
    Linear AMAM cal.

    @param on_off
    FALSE will turn phase modulation OFF, TRUE will turn phase modulation ON

    @param rf_buffer_intf*
    pointer to script buffer
  */
  virtual boolean set_tx_phase_const(boolean on_off,
                                     rf_buffer_intf *buffer)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Tx band specific data to device from RFC

    @param rfcom_band
    rfcom_band whose port is being set

    @param data_array
    Array of int32 containing device Tx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return Status flag indicating success or failure
  */
  virtual boolean tx_set_band_data(rfcom_gsm_band_type rfcom_band,
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
  virtual boolean tx_set_trans_script(rf_buffer_intf *buffer)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  virtual boolean tx_set_stop_script(rf_buffer_intf *buffer)
  {
    return FALSE;
  }

  virtual boolean get_timing_info(rfdevice_gsm_tx_timing_info_type *timing_info) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param arfcn
   
    @param extcal
   
    @param buffer
  */
  virtual boolean prog_cal_tx_start_script(uint16 arfcn,
                                           int8 extcal, 
                                           rf_buffer_intf *buffer)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  virtual uint16 get_therm_read(rf_buffer_intf *buffer)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/

  virtual boolean set_tx_alpha_time(uint16* data)=0;

  virtual boolean get_updated_txagc_info(rfdevice_tx_agc_info_type* data)=0;

 virtual boolean set_timed_writes(int16* data) = 0;

  virtual boolean clear_timed_writes(int16* data)=0;

  virtual boolean set_tx_freq_error(int32* data)=0;

  /*----------------------------------------------------------------------------*/
  /*                          Combined Rx/Tx Functions                          */
  /*----------------------------------------------------------------------------*/
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
    The function generates start/tune script for TX

    @details
    The function generates start/tune script for TX

    @param oper_mode
    Parameter to indicate Stand alone or IRAT mode.
    used to determine WXE cell
   
    @param *script_infor
    Parameter to indicate type of burst and to retrieve timing information

    @param * buffer
    Pointer to script buffer
   
    @param wtr_config_bitmask
    Pointer to a bitmask which will be populated for logging WTR configuration

    @return
    Success/Failure flag
  */
  virtual boolean set_start_script(rfdevice_operation_mode oper_mode, 
                                   rfgsmlib_sbi_burst_struct *script_infor, 
                                   rf_buffer_intf *buffer,
                                   uint16 *buffer_mask,
                                   rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function generates start/tune script for TX

    @details
    The function generates start/tune script for TX

    @param oper_mode
    Parameter to indicate Stand alone or IRAT mode.
    used to determine WXE cell
   
    @param *script_infor
    Parameter to indicate type of burst and to retrieve timing information

    @param * script_data_ptr
    Pointer to rfdevice_meas_script_data_type

    @return
    Success/Failure flag
  */
  virtual boolean set_start_script(rfdevice_operation_mode oper_mode, 
                                   rfgsmlib_sbi_burst_struct *script_infor, 
                                   rfdevice_meas_script_data_type* script_data_ptr)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function gets the Tx transition script.
   
    @details
    The transition script is not reset between bursts; it persists while the
    device is in GSM mode.
    
    @param script_buffers_agc_gmsk
   
    @param script_buffers_agc_8PSK
  */
  virtual boolean get_trans_script
  (
    rf_buffer_intf* script_buffers_agc_gmsk[RFDEVICE_GSM_MAX_TX_SLOTS],
    rf_buffer_intf* script_buffers_agc_8PSK[RFDEVICE_GSM_MAX_TX_SLOTS]
  );

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
    Get rfdevice_rxtx_common_class pointer associated with this instance of device.
    
    @return 
    Pointer to rxtx common device associated with current instance of device.
  */
  virtual 
  rfdevice_trx_common_intf_class* get_common_device_pointer() = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get the number of therm registers to read on this device.
  */
  virtual
  int16 get_num_therm_regs_to_read()
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Enable FBRx self-test feature.
  */
  virtual
  boolean fbrx_self_test_on
  (
    rfcom_gsm_band_type band,
    uint8 gain_state,
    rf_device_execution_type execution_type = RFDEVICE_EXECUTE_IMMEDIATE,
    rf_buffer_intf* fbrx_enable_script = NULL
  );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Enable FBRx self-test feature.
  */
  virtual
  boolean fbrx_self_test_off
  (
    rf_device_execution_type execution_type = RFDEVICE_EXECUTE_IMMEDIATE,
    rf_buffer_intf *fbrx_disable_script = NULL
  );

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean tx_pwr_sleep( void )
  {
    RF_MSG( RF_ERROR, "tx_pwr_sleep: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean tx_pwr_wakeup (void ) 
  {
    RF_MSG( RF_ERROR, "tx_pwr_wakeup: Derived class does not support this API!" );
    return FALSE;
  }

  virtual 
  boolean get_4x_phase_clk_script
  (
    boolean is_set,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type exec_type
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual 
  int16 get_tx_delay_backoff (void) = 0;

#ifdef RFGSM_FSK_ENABLE
  virtual boolean get_fsk_afc_script
  (
    rfcom_gsm_band_type                                   rf_band,
    uint32                                                nominal_freq_khz,
    int32                                                 freq_offset_hz,
    rf_buffer_intf*                                       script_buf,
    rf_device_execution_type                              exec_type
  ) = 0;
#endif
};

#else

struct rfdev_gsm_tx;
typedef struct rfdev_gsm_tx rfdevice_trx_gsm_tx_intf_class;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_trx_common_intf_class* 
rfdevice_gsm_tx_get_common_device_pointer(rfdevice_trx_gsm_tx_intf_class* tx_device);


#endif /* #ifdef __cplusplus */

#endif /* FEATURE_GSM */

#endif /* RFDEVICE_TRX_GSM_TX_H */
