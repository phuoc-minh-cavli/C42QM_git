#ifndef RFDEVICE_HDET_CMN_INTF_H
#define RFDEVICE_HDET_CMN_INTF_H
/*!
   @file
   rfdevice_hdet_cmn_intf.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2012 - 2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/device/rfdev_intf/inc/rfdevice_hdet_cmn_intf.h#1 $

when       who   what, where, why
--------   ---   -------------------------------------------------------------------
03/23/18   up     Added cmn HDET APIs: rfdevice_hdet_cmn_measure_temp_comp_cb and 
                  rfdevice_hdet_cmn_set_gain_state
02/16/18   up     Modified for SDR105, FR 46604: NBPDET support on SDR105
                  Removed unsupported APIs
03/29/13   adk    Update temp comp (K-sensor) values for QFE TDET
03/16/13   lp     Added qfe1520_hdet_pwr_index_type
03/13/13   aca    Tuner characterization
01/29/13   adk    Added support for rfdevice_hdet_cmn_get_gated_incident_val()
10/26/12   kai    Added rfdevice_hdet_cmn_program_k0_val, removed rfdevice_hdet_cmn_do_k_sensor_cal
10/16/12   kai    Added API getting tech specific HDET object pointer
10/15/12   kai    Added APIs and implementations
09/26/12   vb     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rfm_mode_types.h"

/*===========================================================================
                         Data Definitions
===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum indicating the device ID/number
*/
typedef enum
{
  RFDEVICE_HDET_DEVICE_0,
  RFDEVICE_HDET_DEVICE_1,
  RFDEVICE_HDET_DEVICE_2,
  RFDEVICE_HDET_DEVICE_3,
  RFDEVICE_HDET_DEVICE_4,
  RFDEVICE_HDET_DEVICE_5,
  RFDEVICE_HDET_DEVICE_6,
  RFDEVICE_HDET_DEVICE_7,
  RFDEVICE_HDET_DEVICE_8,
  RFDEVICE_HDET_DEVICE_9,
  RFDEVICE_HDET_DEVICE_MAX
} rfdevice_hdet_device_num_type;

/* HDET Gain State enums */
typedef enum
{
  RFDEVICE_HDET_GAIN_STATE_0,    /* Gain state 0 */
  RFDEVICE_HDET_GAIN_STATE_1,    /* Gain state 1 */
  RFDEVICE_HDET_GAIN_STATE_2,    /* Gain state 2 */
  RFDEVICE_HDET_GAIN_STATE_3,    /* Gain state 3 */
  RFDEVICE_HDET_GAIN_STATE_4,    /* Gain state 4 */
  RFDEVICE_HDET_GAIN_STATE_5,    /* Gain state 5 */
  RFDEVICE_HDET_GAIN_STATE_6,    /* Gain state 6 */
  RFDEVICE_HDET_GAIN_STATE_7,    /* Gain state 7 */
  RFDEVICE_HDET_GAIN_STATE_8,    /* Gain state 8 */
  RFDEVICE_HDET_GAIN_STATE_NUM,  /* Total number of gain states */
} rfdevice_hdet_gain_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing QFE1510 HDET paths

  @details
*/
typedef enum
{
  QFE1510_HDET_INC_PWR_INDEX = 0,
  QFE1510_HDET_REF_PWR_INDEX = 1, 
  QFE1510_HDET_PWR_MAX_INDEX
} qfe1510_hdet_pwr_index_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing QFE1520 HDET paths

  @details
*/
typedef enum
{
  QFE1520_HDET_INC_PWR_INDEX = 0,
  QFE1520_HDET_REF_PWR_INDEX = 1, 
  QFE1520_HDET_PWR_MAX_INDEX
} qfe1520_hdet_pwr_index_type;

/*===========================================================================
                         Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */
void* rfdevice_hdet_cmn_get_tech_object
( 
  void *device_ptr,
  rfm_mode_enum_type tech
);

boolean rfdevice_hdet_cmn_measure_temp_comp_cb
(
  void *device_ptr,
  qfe_script_type* temp_comp_data_ptr
);

boolean rfdevice_hdet_cmn_set_gain_state
(
  void *device_ptr, void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_hdet_gain_state_type hdet_gain_state,
  uint8 hdet_pid
);

#ifdef __cplusplus
}
#endif


#endif /* RFDEVICE_HDET_CMN_INTF_H */


