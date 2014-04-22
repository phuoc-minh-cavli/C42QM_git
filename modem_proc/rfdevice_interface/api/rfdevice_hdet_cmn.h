#ifndef RFDEVICE_HDET_CMN_H
#define RFDEVICE_HDET_CMN_H
/*!
  @file
  rfdevice_hdet_cmn.h

  @brief
  Technology independent HDET interfaces.
*/

/*===========================================================================

Copyright (c) 2012 - 2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_hdet_cmn.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/23/18   up      Added common HDET APIs: update_temp_comp_param_cb and 
                   set_gain_state.
02/16/18   up      Modified for SDR105, FR 46604: NBPDET support on SDR105
                   Removed unsupported APIs
03/29/13   adk     Read and store temp (K-sensor) values for QFE TDET
03/13/13   aca     Tuner power characterization
01/23/13   ndb     API updates for WTR1605 compatibility 
10/26/12   kai     Added program_k0_val, removed do_k_sensor_cal
10/16/12   kai     Added API getting tech specific HDET object pointer
10/16/12   kai     Added trigger_flag to get_ref_val
10/15/12   kai     Added common APIs and FTM APIs
09/26/12   vb      Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfdevice_hdet_cmn_intf.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

class rfdevice_hdet_cmn : public rfdevice_class
{
public:

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function get the pointer to tech specific hdet instance

  @details
  This function get the pointer to tech specific hdet instance.
  The tech specific hdet instance will be created only once.
 
  @return
  void pointer to tech specific hdet instance
*/
  virtual void* get_hdet_tech_object( rfm_mode_enum_type tech ) = 0;

    
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns temperature compensation data to QFE
  
  @details
  This function updates the temp comp parameters in the TDET object.

  If TDET is not being used then this API returns FALSE.

  @param temp_comp_data_ptr
  ptr to the temp comp data. This data will be stored in the TDET object.

  @return
  boolean indicates pass (TRUE) or fail (FALSE)
*/
  virtual boolean update_temp_comp_param_cb( qfe_script_type* temp_comp_data_ptr ) = 0;


/*******************************************************************************
                                    FTM APIs
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the HDET gain state.
  
  @details
  This function sets the HDET gain state. HDET inititalization must have been
  completed before this API can be called.
  
  External script can be used along with this API
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @param hdet_gain_state
  HDET gain state to be set

  @hdet_pid
  PID of interest. Defalted to 0.

  @return
  boolean indicating pass (TRUE) or fail (FALSE)   
*/
virtual boolean set_gain_state
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    rfdevice_hdet_gain_state_type hdet_gain_state,
    uint8 hdet_pid = 0
  ) = 0;

};


#endif /*RFDEVICE_HDET_CMN_H*/

