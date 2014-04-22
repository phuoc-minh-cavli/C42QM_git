#ifndef RFLTE_CORE_THERM_MITIGATION_H
#define RFLTE_CORE_THERM_MITIGATION_H
/*!
  @file
  rflte_core_therm_mitigation.h

  @brief
  Provides LTE Thermal Mitigation functionality

  @details

*/

/*===========================================================================

Copyright (c) 2019 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:30:23 $ $Author: pwbldsvc $
$Header:

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

02/25/19    dh     Initial version

============================================================================*/
#include "comdef.h"
#include "nv_items.h"
#include "rfcommon_locks.h"

#define RFCOMMON_THERM_MAX_NUM_DEVICES 1

typedef struct
{
  void* therm_obj;
  uint32 therm_handle;
  uint16 therm_read_raw;
  int16  therm_read_in_degC;
  uint32 band_info;
  unsigned long long time_stamp;
}rfcommon_therm_read_data_per_device_t;


/*----------------------------------------------------------------------------*/
/*! @ detail
  structure of therm read data for PA Therm
*/
typedef struct
{
  rfcommon_therm_read_data_per_device_t data[RFCOMMON_THERM_MAX_NUM_DEVICES];
}rfcommon_therm_read_data_t;

typedef struct
{
  rfcommon_therm_read_data_t therm_data;
  /* Save current carrier index in the state machine for
  the timer interrupt to use */
  uint32 curr_carrier_idx;
  /* Enable flag for allowing therm reads. We will 
  enable this in Tx Config and clear in Tx Disable */
  boolean therm_read_enable;
  uint32 timer_interval;
  uint32 start_offset;
  timer_type pa_therm_read_timer;
  rf_lock_data_type crit_section;
}rfcommon_therm_mitigation_sm_type;

/*--------------------------------------------------------------------------*/
void rflte_core_therm_mitigation_enable(uint32 carrier_idx, boolean enable); 
/*--------------------------------------------------------------------------*/
void rflte_core_therm_mitigation_update(uint32 carrier_idx);


#endif /* RFLTE_CORE_THERM_MITIGATION_H */
