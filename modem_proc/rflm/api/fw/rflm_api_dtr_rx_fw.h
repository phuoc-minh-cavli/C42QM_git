/*!
  @file
  rflm_api_dtr_rx.h

  @brief
  RFLM API DTR Rx Interface functions to FW
  
  @detail
  This file contains RFLM API DTR Rx Interface prototypes to FW

*/

/*==============================================================================

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

  $Header: //components/rel/rflm.mpss/1.10/api/fw/rflm_api_dtr_rx_fw.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/10/14   pl      Add support for dynamic notch 
10/02/13  hdz      Added dtr rx version number
08/09/13   vrb     Initial version.
==============================================================================*/

#ifndef RFLM_API_DTR_RX_FW_H
#define RFLM_API_DTR_RX_FW_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"
#include "rflm_dtr_rx_fw_intf_ag.h"

/*! @brief Version of the rflm dtr rx module.
*/
#define RFLM_API_DTR_RX_VERSION    \
              ((RFLM_DTR_RX_MAJOR_REV << 16) | RFLM_DTR_RX_FW_INTF_REV )



#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

FUNCTION:  rflm_dtr_rx_adc_settings_low_pwr

==============================================================================*/
extern rflm_err_t rflm_dtr_rx_adc_settings_low_pwr (uint32 adc_idx, rflm_dtr_rx_adc_low_pwr_state_enum state_enum);


/*==============================================================================

FUNCTION:  rflm_dtr_rx_adc_settings_low_pwr

==============================================================================*/
extern rflm_err_t rflm_dtr_rx_adc_settings_restore (rflm_handle_rx_t buf_idx);


/*==============================================================================

FUNCTION:  rflm_dtr_rx_get_group_settings

==============================================================================*/
/*!
  @brief
  Get DTR RX XO related parameters

  @detail
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_rx_get_group_settings(  rflm_handle_rx_t handle,
                                                   rflm_dtr_rx_fw_export_group_id group_id,
                                                   uint32 *addr );


/*==============================================================================

FUNCTION:  rflm_dtr_rx_configure_chain

==============================================================================*/
/*!
  @brief
  Configure Chain

  @detail
  TBD
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_rx_configure_chain(  rflm_handle_rx_t handle,
                                                rflm_dtr_rx_indices_struct *rx_indices,
                                                uint32 config_mask );

/*==============================================================================

FUNCTION:  rflm_dtr_rx_configure_dyn_notch

==============================================================================*/
/*!
  @brief
  Configure Chain

  @detail
  TBD
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_rx_configure_dyn_notch(  rflm_handle_rx_t handle,
                                                    rflm_set_dyn_notch_in_t *notch_in,
                                                    rflm_set_dyn_notch_out_t *result );

extern rflm_err_t rflm_dtr_rx_notch_enable(rflm_handle_rx_t handle,boolean enable);
extern rflm_err_t rflm_dtr_rx_dynamic_notch_cfg(rflm_handle_rx_t handle, rflm_dtr_rx_enum_notch_var depth);
/*==============================================================================

FUNCTION:  rflm_dtr_rx_get_wbdc_params

==============================================================================*/
/*!
  @brief
  Get WBDC parameters

  @detail
  TBD
 
  @return
  rflm error code
*/
/*============================================================================*/

extern rflm_err_t rflm_dtr_rx_get_wbdc_params (rflm_handle_rx_t buf_idx, 
                                        uint8 *est1_left_shift, 
                                        uint8 *est2_left_shift);


/*==============================================================================

FUNCTION:  rflm_dtr_rx_get_rx_tx_delay_vars

==============================================================================*/
/*!
  @brief
  Get rx tx delay  parameters

  @detail
  TBD
 
  @return
  added RX and TX delay.
*/
/*============================================================================*/

extern uint64 rflm_dtr_rx_get_rx_tx_delay_vars(rflm_handle_rx_t rx_handle ,rflm_handle_rx_t tx_handle);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* RFLM_API_DTR_RX_FW_H */


