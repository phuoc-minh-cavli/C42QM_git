#ifndef TXLM_HAL_H
#define TXLM_HAL_H

/*!
   @file
   txlm_hal.h

   @brief
   This file implements the TXLM MDM9K HAL functionality. These are platform
   dependent.

*/

/*===========================================================================

Copyright (c) 2010 - 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:31:05 $ $Author: pwbldsvc $
$Header: //components/rel/rflm.mpss/1.10/dtr/inc/txlm_hal.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
08/04/14   dr      Adding support for foundry dependent IREF gain programming. 
04/24/14   ljl     cleanup txlm_hal_bringup_dac api
02/13/14   st      DAC Temperature Compensation Support
11/06/13   st      LMEM Info Storage for DRIF Group A/B
10/16/13   st      Remove RFA-VARIATIONs dependency
10/14/13   st      Reloc IREF LUT Type Definition
10/04/13   st      Bolt DTR-TX New APIs
04/01/13   dej    Add dynamic settings tables api to TxLM
01/14/12   aca     DIME Dac cal- migrated to AG functions
12/03/12   aca     DIME Dac cal
08/04/11   cd     Support for SW Dynamic Updates to LM buffer
                  Support for reading portions of LM buffer 
03/24/11   vrb    LM Intf changes for CMI Compliance
12/22/10   vrb    Added extern functions for the mock fw
12/14/10   sr     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#ifdef _cplusplus
extern "C" {
#endif

#include "comdef.h"

#ifndef FEATURE_RF_LTE_PAGE_ONLY
#include "err.h"

#include "txlm_intf.h"

txlm_config_type *txlm_hal_config_get(void);

lm_status_type txlm_init_settings_get ( 
            txlm_chain_type chain ,
            lm_tech_type tech ,
            void *cfg ,
            uint32 *addr ,
            uint32 *size 
            );

lm_status_type txlm_dynamic_group_offset_addr_get ( 
            uint32 dyn_group ,
            uint32 *offset_addr ,
            uint32 *size 
            );

lm_status_type txlm_update_dynamic_group_struct_ag (
            uint32 dyn_functionality ,
            uint32 dyn_group ,
            uint8 * target_struct ,
            uint32 table_index 
            );

lm_status_type txlm_hal_execute_sequence
    (
      txlm_chain_type chain,
      uint32 seq_id,
      uint32 *data
    );  

lm_status_type txlm_hal_retrieve_dac_iref_lut
    (
      const uint8* iref_lut,
      uint8 iref_lut_length);

lm_status_type txlm_hal_get_chain(lm_handle_type txlm_handle, txlm_chain_type* chain);

void txlm_hal_config_dac_iref(txlm_chain_type chain,
                                        boolean update_en,
                                        uint32  iref_lut_idx,
                                        uint32 iref_update_start_time,
                                        uint32 iref_update_stop_time);
#endif /*FEATURE_RF_LTE_PAGE_ONLY*/

#ifdef _cplusplus
}
#endif
#endif /* TXLM_HAL_H */

