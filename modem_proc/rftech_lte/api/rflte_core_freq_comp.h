#ifndef RFLTE_CORE_FREQ_COMP_H
#define RFLTE_CORE_FREQ_COMP_H

/*! 
  @file
  rflte_core.h
 
 @brief
 This file is used to define an interface to all core layer APIs needed by
 the MC layer.
 */

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rftech_lte.mpss/2.10/api/rflte_core_freq_comp.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/04/14    pv     Added support for FBRx RB Based Freq comp.
04/17/14   svi     PA static NV and code redesign
09/05/13   cri     Initial check in

==============================================================================*/

#include "comdef.h"

/* FBRx Related */
#include "rfcommon_fbrx_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  int32 lin_vs_freq_val_slot1_in[RFFW_LTE_MAX_NUM_FREQ_COMP*RF_LTE_MAX_NUM_PA_STATES];
  int32 lin_vs_freq_val_slot1_out[RFFW_LTE_MAX_NUM_FREQ_COMP*RF_LTE_MAX_NUM_PA_STATES];
  int32 lin_vs_freq_val_slot2_in[RFFW_LTE_MAX_NUM_FREQ_COMP*RF_LTE_MAX_NUM_PA_STATES];
  int32 lin_vs_freq_val_slot2_out[RFFW_LTE_MAX_NUM_FREQ_COMP*RF_LTE_MAX_NUM_PA_STATES];
} rflte_core_freq_comp_type;

/*--------------------------------------------------------------------------*/
rflte_core_freq_comp_type* rflte_core_get_freq_comp_offsets(uint32 carrier_idx);
/*--------------------------------------------------------------------------*/
void rflte_core_compute_freq_comp(uint32 carrier_idx,
                                  uint8* rb_start,
                                  uint8* rb_block,
                                  int32* lin_vs_freq_val_p_in,
                                  int32* lin_vs_freq_val_p_out,
                                  int32* lin_vs_freq_val_legacy,
                                  boolean is_lin_vs_temp_vs_freq_in_use);

/*--------------------------------------------------------------------------*/
int16 rflte_core_compute_freq_comp_lim(uint32 carrier_idx,
                                       uint8 phy_start_rb,
                                       uint8 phy_rb_block);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_CORE_H */
