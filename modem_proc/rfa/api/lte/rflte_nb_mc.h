#ifndef RFLTE_NB_MC_H
#define RFLTE_NB_MC_H

/*
   @file
   rflte_nb_mc.h

   @brief
   RF LTE NBIOT Driver's interface file.

   @details

*/

/*===========================================================================
Copyright (c) 2016 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/api/lte/rflte_nb_mc.h#1 $
$DateTime: 2020/04/16 12:27:00 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/28/17   as      Support for high speed ADC rate for certain EARFCNs to avoid desense due to ADC spurs 
09/09/16   qzh     Initial Check-in

============================================================================*/

#include "rfcom.h"
#include "rfm_device_types.h"
#include "rflte_ext_mc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================
 
                     EXTERNAL STRUCTURE  DEFINITION
 
 ===========================================================================*/

 typedef enum
 {
   RFLTE_NB_ADC_MODE_NB_IOT, 
   RFLTE_NB_ADC_MODE_NB_IOT_HIGHSPEED, 
   RFLTE_NB_ADC_MODE_NUM,
   RFLTE_NB_ADC_MODE_INVALID
 }rflte_nb_adc_mode_type;



 /*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

boolean rflte_nb_mc_get_tuner_scenario_list(rfm_device_enum_type device,
                                         uint32 channel,
                                         rfm_antenna_tuner_scenario_data *scenario_data,
                                         uint8 *free_space_index,
                                         rfm_tuner_handle_type *tuner_handle);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RFLTE_MC_EXT_H */

