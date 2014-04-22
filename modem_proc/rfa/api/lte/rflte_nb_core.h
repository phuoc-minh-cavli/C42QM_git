#ifndef RFLTE_NB_CORE_H
#define RFLTE_NB_CORE_H

/*!
  @file
  rflte_nb_core.h

  @brief
  Provides NBIOT core control.

  @details


*/

/*===========================================================================

Copyright (c) 2010 - 2019 by QUALCOMM TECHNOLOGY, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:27:00 $ $Author: pwbldsvc $
$Header: //components/rel/rfa.mpss/3.10/api/lte/rflte_nb_core.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/15/19   as      Non Anchor Tx retune changes
09/25/18   cv      Added Support of per-band setting of Power Class in Rel.14 Cat.M1 and NB1/NB2
06/16/17   qzh     Add PUSCH format type for TxAGC log packet
03/03/16   qzh     Add API for ML1 to query tx pwr.
09/09/16   qzh     Initial version.

============================================================================*/
#ifdef FEATURE_LTE
#include "rflte_ext_mc.h"
#include "rflm_api_lte.h"

/* Tone bandwidth type of UL transmission */
typedef enum
{
  RFLTE_NB_UL_SINGLE_TONE_3_75_KHZ,
  RFLTE_NB_UL_SINGLE_TONE_15_KHZ,
  RFLTE_NB_UL_MULTI_TONE_3,
  RFLTE_NB_UL_MULTI_TONE_6,
  RFLTE_NB_UL_MULTI_TONE_12
} rflte_nb_ul_tone_bw_e;

/*Structure to carry MTPL parameters*/

typedef struct
{
  rflte_nb_ul_tone_bw_e tone_bw;
  uint8 start_tone_idx;
  rflm_lte_mod_type_s mod;
  uint8 ns_x;
  int16 p_emax;
  boolean phrless_flag;
  int16 cxm_pmax;
  int16 *phr_backoff_db10;
  rflm_lte_nb_chan_format_e chan_format;
  rflte_core_txpl_power_class_type ue_power_class;
  uint8 subframe_num;
}rflte_nb_core_mtpl_param_s;

/*Structure to carry Anchor/Non Anchor Tx tune parameters*/
typedef struct
{
  /* Rx Frequency Error in PPM Q10*/
  int32 rx_freq_error_in_ppm_q10;
  /*Tx channel number*/
  rfcom_lte_earfcn_type retune_tx_chan;
  /*M_UL value, and will be added to tx freq after multiplying 5khz*/
  int8 ul_carrier_freq_offset;
  /* Flag to indicate retune required*/
  boolean tx_retune;
}rflte_nb_core_txpl_tx_retune_param_s;


/* L1 External */
/*--------------------------------------------------------------------------*/
int16 rflte_nb_core_txpl_get_sar(void);
/*--------------------------------------------------------------------------*/
int16 rflte_nb_core_compute_p_cmax_per_sf_with_phr_backoff(
                                       rflte_nb_core_mtpl_param_s* mtpl_param,
                                       rflte_nb_core_txpl_tx_retune_param_s* tx_retune_param);

/*--------------------------------------------------------------------------*/
int16 rflte_nb_core_txpl_get_actual_vs_digital_tx_pwr_delta (void);

#endif /*FEATURE_LTE*/
#endif /* RFLTE_CORE_TXPL_H */
