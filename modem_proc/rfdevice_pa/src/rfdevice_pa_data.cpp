/*!
   @file
   rfdevice_pa_data.cpp

   @brief
   This file contains implementation the rfdevice_pa_data class,
   which will be used by the PA device driver configure the vendor specific PA device.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:24:51 $ $Author: pwbldsvc $
$Header: //components/rel/rfdevice_pa.mpss/1.30/src/rfdevice_pa_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
09/05/18   dh      Added timing_info_get() API
01/16/13   sr      Added pa command sequence request support.
01/03/13   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_pa_data.h"


// Constructor 
rfdevice_pa_data::rfdevice_pa_data()
{
  // do nothing for now
}

// Destructor
rfdevice_pa_data::~rfdevice_pa_data()
{
  // do nothing for now
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  implement this API to support the PA device command sequence.

*/
boolean rfdevice_pa_data::sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                             rfdevice_pa_cmd_seq_type *cmd_seq)
{
  // to support the PA cmd Sequence, implement this API in derived clases.
  return FALSE;
}


boolean rfdevice_pa_data::get_temperature_lut(int16 **lut_addr, uint8 *lut_size)
{
  return FALSE;
}
boolean rfdevice_pa_data::timing_info_get( rfdevice_pa_timing_info_type *pa_timing_info )
{
  pa_timing_info->asm_settling_delay = 0;
  pa_timing_info->asm_trigger_offset = 0;
  return TRUE;
}