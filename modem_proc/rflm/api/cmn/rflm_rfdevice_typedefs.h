/*!
  @file
  rflm_rfdevice_typedefs.h

  @brief
   interface for RFLM and RFDEVICE
   This is the header for any definitions that are used directly by device driver. 
   The header should compile standalone and should not include any other rflm, rfmodem or fw headers
   
  @addtogroup RFLM_API_COMMON
  @{
*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/api/cmn/rflm_rfdevice_typedefs.h#1 $

when          who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/16   ash     Initial Revision

===========================================================================*/
#ifndef RFLM_RFDEVICE_TYPEDEFS_H
#define RFLM_RFDEVICE_TYPEDEFS_H

#include "comdef.h"
/*----------------------------------------------------------------------------*/
/*! @brief Indicates Max size of Rx AGC LUT data per gain state */
#define RFLM_RFDEVICE_MAX_RXDATA_PER_GAIN_STATE_IN_UINT16 16

/*----------------------------------------------------------------------------*/
/*! @brief Structure containing miscellaneous RGI specific information like 
  RSB coefficients, DA expected power
  copied definition from RF device, cross inclusion is under discussion
*/
typedef struct
{
  /*! @brief Expected DA output power for the RGI in dB x 100 */
  int16 da_pout_db100;

  /*! @brief RSB "A" coefficient to be programmed to MSM registers */
  uint16 rsb_coeff_alpha;

  /*! @brief RSB "B" coefficient to be programmed to MSM registers */
  uint16 rsb_coeff_beta;

  /*! @brief DCOC I component to be programmed to MSM registers */
  uint16 dcoc_i_component;
  
  /*! @brief DCOC Q component to be programmed to MSM registers */
  uint16 dcoc_q_component;

}rflm_rfdevice_txagc_misc_data_per_rgi_type;


/*
@brief 8PSK delay match parameters

@detail
*/
typedef struct
{
  /*! @brief TXFE DP DM2 Tau I Component Fine value */
  uint16 dtr_txfe_dp_dm2_i_fine_tau;
  /*! @brief TXFE DP DM2 Tau Q Component Fine value */
  uint16 dtr_txfe_dp_dm2_q_fine_tau;
  /*! @brief TXFE DP DM3 Tau I Component Fractional value */
  uint16 dtr_txfe_dp_dm3_i_frac_tau;
  /*! @brief TXFE DP DM3 Tau Q Component Fractional value */
  uint16 dtr_txfe_dp_dm3_q_frac_tau;
  /*! @brief TXFE ET DM3 Tau I Component Fractional value */
  uint8  dtr_txfe_et_dm3_frac_tau;
  /*! @brief TXFE ET DM2 Tau I Component Fine value */
  uint8  dtr_txfe_et_dm2_fine_tau;
  /*! @brief TXFE RFI ET Coarse Tau value */
  uint8  dp_rfi_et_coarse_tau;
} rflm_rfdevice_txagc_env_delay_type;

/*! @brief Structure containing Rx AGC LUT per gain state */
typedef struct
{
  uint16 data[RFLM_RFDEVICE_MAX_RXDATA_PER_GAIN_STATE_IN_UINT16]; 
}rflm_rfdevice_trx_rx_agc_data_per_gs_type; 


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif // RFLM_RFDEVICE_TYPEDEFS_H


