/*!
  @file
  nb1fw_log_codes.h

  @brief
  This file contains NB1 FW log code definitions.

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/nb1fw_log_codes.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $ 

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/

#ifndef NB1FW_LOG_CODES_H
#define NB1FW_LOG_CODES_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <log_codes.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/


/********************* START OF ML1 LOG CODE DEFINES **************************/
/*! @brief NB1 FW log code base 
    LOG_NB1FW_BASE_C = 0xB200
    Log code range for NB1 FW 0xB200-0xB23F
 
  Sys and Rx - 0xB200-0xB207
  Searcher   - 0xB208-0xB20F
  DL         - 0xB210-0xB217
  UL/TX      - 0xB218-0xB21F
  DBACK      - 0xB220-0xB22F

  expend Rx  - 0xB230-0xB237 
*/
#define LOG_NB1FW_BASE_C                      (0x200 + LOG_APPS_BASE_C)
#define LOG_NB1FW_SYS_RX_BASE_C               (0x00 + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_SRCH_BASE_C                 (0x08 + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_DL_BASE_C                   (0x10 + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_UL_BASE_C                   (0x18 + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_DBACK_BASE_C                (0x20 + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_MULTI_GRANT_BASE_C          (0x2A + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_EXP_RX_BASE_C               (0x30 + LOG_NB1FW_BASE_C)
#define LOG_NB1FW_LAST_C                      (0x38 + LOG_NB1FW_BASE_C)


#define LOG_NB1FW_MASK_OFS( code ) \
  ( (uint64) 1 << ( (code) - LOG_NB1FW_BASE_C ) )

// ----------------------------------------------------------------------------
// Sys/Rx logging
// ----------------------------------------------------------------------------
/*! @brief  NB1 FW Command/Responce log code log code */
#define LOG_NB1FW_CMD_RSP_LOG_C               (0x0 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_CMD_RSP_LOG_VERSION         (0x1)

/*! @brief NB1 FW rx-FE AGC log code */
#define LOG_NB1FW_RXFE_AGC_LOG_C              (0x1 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_RXFE_AGC_LOG_VERSION        (0x7)

/*! @brief NB1 FW EFS log code */
#define LOG_NB1FW_EFS_LOG_C                   (0x2 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_EFS_LOG_VERSION             (0x1)

/*! @brief NB1 FW DRX log code */
#define LOG_NB1FW_DRX_LOG_C                   (0x3 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_DRX_LOG_VERSION             (0x3)

/*! @brief NB1 FW RxFE freq update log code */
#define LOG_NB1FW_RXFE_FREQ_UPDATE_LOG_C        (0x4 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_RXFE_FREQ_UPDATE_LOG_VERSION  (0x1)

/*! @brief NB1 FW Rx timing log code */
#define LOG_NB1FW_RXFE_TIME_LOG_C               (0x5 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_RXFE_TIME_LOG_VERSION         (0x1)

/*! @brief NB1 FW Rx timing log code */
#define LOG_NB1FW_SS_LOG_C               (0x6 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_SS_LOG_VERSION         (0x3)

/*! @brief NB1 FW unformated profiling log packet */
#define LOG_NB1FW_PROF_LOG_C                  (0x7 + LOG_NB1FW_SYS_RX_BASE_C)
#define LOG_NB1FW_PROF_LOG_VERSION            (0x1)

// ----------------------------------------------------------------------------
// Expend Rx logging
// ----------------------------------------------------------------------------
/*! @brief  NB1 FW Expend Rx logging log code */
#define LOG_NB1FW_RX_ENDIS_LOG_C               (0x0 + LOG_NB1FW_EXP_RX_BASE_C)
#define LOG_NB1FW_RX_ENDIS_LOG_VERSION         (0x1)

/* log code mask */
#define LOG_NB1FW_SYS_RX_MASK ( \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_CMD_RSP_LOG_C  )            | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_RXFE_AGC_LOG_C )            | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_RX_ENDIS_LOG_C )            | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_EFS_LOG_C )                 | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DRX_LOG_C )                 | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_RXFE_FREQ_UPDATE_LOG_C )    | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_RXFE_TIME_LOG_C )           | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SS_LOG_C )                  | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_PROF_LOG_C     )              \
  )

// ----------------------------------------------------------------------------
// Searcher and Measurements
// ----------------------------------------------------------------------------
/*! @brief NB1 FW search PSS result log code */
#define LOG_NB1FW_SRCH_PSS_AUTOCORR_RESULT_LOG_C          (0x0 + LOG_NB1FW_SRCH_BASE_C)
#define LOG_NB1FW_SRCH_PSS_AUTOCORR_RESULT_LOG_VERSION    (0x3)

/*! @brief NB1 FW search PSS_XCORR_SSS result log code */
#define LOG_NB1FW_SRCH_PSS_XCORR_SSS_RESULT_LOG_C         (0x1 + LOG_NB1FW_SRCH_BASE_C)
#define LOG_NB1FW_SRCH_PSS_XCORR_SSS_RESULT_LOG_VERSION   (0x1)

/*! @brief NB1 FW search SSS result log code */
#define LOG_NB1FW_SRCH_SSS_RESULT_LOG_C                   (0x2 + LOG_NB1FW_SRCH_BASE_C)
#define LOG_NB1FW_SRCH_SSS_RESULT_LOG_VERSION             (0x4)

/*! @brief NB1 FW search PSS_XCORR_FFT result log code */
#define LOG_NB1FW_SRCH_PSS_XCORR_FFT_RESULT_LOG_C         (0x3 + LOG_NB1FW_SRCH_BASE_C)
#define LOG_NB1FW_SRCH_PSS_XCORR_FFT_RESULT_LOG_VERSION   (0x2)

/*! @brief NB1 FW search PSS hcorr result log code */
#define LOG_NB1FW_SRCH_PSS_HCORR_RESULT_LOG_C          (0x4 + LOG_NB1FW_SRCH_BASE_C)
#define LOG_NB1FW_SRCH_PSS_HCORR_RESULT_LOG_VERSION    (0x4)

/*! @brief NB1 FW search PSS refine SSS zift result log code */
#define LOG_NB1FW_SRCH_PSS_REFINE_SSS_RESULT_LOG_C          (0x5 + LOG_NB1FW_SRCH_BASE_C)
#define LOG_NB1FW_SRCH_PSS_REFINE_SSS_RESULT_LOG_VERSION    (0x2)


/* log code mask */
#define LOG_NB1FW_SRCH_MASK ( \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SRCH_PSS_AUTOCORR_RESULT_LOG_C  ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SRCH_PSS_XCORR_SSS_RESULT_LOG_C ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SRCH_SSS_RESULT_LOG_C           ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SRCH_PSS_XCORR_FFT_RESULT_LOG_C ) |\
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SRCH_PSS_HCORR_RESULT_LOG_C ) |\
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_SRCH_PSS_REFINE_SSS_RESULT_LOG_C ) \
  )

// ----------------------------------------------------------------------------
// Downlink
// ----------------------------------------------------------------------------
/*! @brief NB1 FW downlink scheduler log packet */
#define LOG_NB1FW_DL_SCHED_LOG_C               (0x0 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_SCHED_LOG_VERSION         (0x2)

/*! @brief NB1 FW downlink Channel estimation log packet */
#define LOG_NB1FW_DL_CHEST_LOG_C               (0x1 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_CHEST_LOG_VERSION         (0x2)

/*! @brief NB1 FW downlink RXFFT log packet */
#define LOG_NB1FW_DL_RXFFT_LOG_C               (0x2 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_RXFFT_LOG_VERSION         (0x2)

/*! @brief NB1 FW downlink TL log packet */
#define LOG_NB1FW_DL_TL_LOG_C                  (0x3 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_TL_LOG_VERSION            (0x4)

/*! @brief NB1 FW downlink DEMOD log packet */
#define LOG_NB1FW_DL_DEMOD_LOG_C               (0x4 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_DEMOD_LOG_VERSION         (0x2)

/*! @brief NB1 FW downlink meas log packet */
#define LOG_NB1FW_DL_MEAS_LOG_C                (0x5 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_MEAS_LOG_VERSION          (0x3)

/*! @brief NB1 FW downlink meas log packet */
#define LOG_NB1FW_DL_SSFEE_LOG_C               (0x6 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_SSFEE_LOG_VERSION         (0x1)

/*! @brief NB1 FW downlink meas log packet */
#define LOG_NB1FW_DL_CHAN_STATE_LOG_C          (0x7 + LOG_NB1FW_DL_BASE_C)
#define LOG_NB1FW_DL_CHAN_STATE_LOG_VERSION    (0x1)


/*! @brief NB1 FW downlink Multi Grant log packet */
#define LOG_NB1FW_DL_MULTI_GRANT_LOG_C             (0x0 + LOG_NB1FW_MULTI_GRANT_BASE_C)
#define LOG_NB1FW_DL_MULTI_GRANT_LOG_VERSION       (0x1)

/*! @brief NB1 FW downlink Carrier Switch log packet */
#define LOG_NB1FW_DL_CARRIER_SWITCH_LOG_C          (0x1 + LOG_NB1FW_MULTI_GRANT_BASE_C)
#define LOG_NB1FW_DL_CARRIER_SWITCH_LOG_VERSION    (0x1)


/*! @brief NB1 FW downlink Carrier Switch log packet */
#define LOG_NB1FW_DL_NRS_MEASUREMENT_LOG_C          (0x2 + LOG_NB1FW_MULTI_GRANT_BASE_C)
#define LOG_NB1FW_DL_NRS_MEASUREMENT_LOG_VERSION    (0x3)

/*! @brief NB1 FW downlink Carrier Switch log packet */
#define LOG_NB1FW_DL_RAFTT_LOG_C                    (0x3 + LOG_NB1FW_MULTI_GRANT_BASE_C)
#define LOG_NB1FW_DL_RAFTT_LOG_VERSION              (0x1)

/*! @brief NB1 FW downlink pbch meas log packet */
#define LOG_NB1FW_DL_PBCH_MEAS_LOG_C                (0x4 + LOG_NB1FW_MULTI_GRANT_BASE_C)
#define LOG_NB1FW_DL_PBCH_MEAS_LOG_VERSION          (0x2)

/*! @brief NB1 FW downlink Carrier Switch log packet */
#define LOG_NB1FW_DL_DOPPLER_LOG_C                  (0x5 + LOG_NB1FW_MULTI_GRANT_BASE_C)
#define LOG_NB1FW_DL_DOPPLER_LOG_VERSION            (0x1)

/* log code mask */
#define LOG_NB1FW_DL_MASK ( \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_SCHED_LOG_C   ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_CHEST_LOG_C   ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_RXFFT_LOG_C   ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_TL_LOG_C      ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_DEMOD_LOG_C   ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_MEAS_LOG_C    ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_PBCH_MEAS_LOG_C  ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_SSFEE_LOG_C      ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_CHAN_STATE_LOG_C ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_MULTI_GRANT_LOG_C) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_CARRIER_SWITCH_LOG_C) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_NRS_MEASUREMENT_LOG_C) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_RAFTT_LOG_C) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DL_DOPPLER_LOG_C)  \
  )

// ----------------------------------------------------------------------------
// Demback
// ----------------------------------------------------------------------------

/*! @brief NB1 FW Demback log packets */
#define LOG_NB1FW_DBACK_NPBCH_ASG_LOG_C                     (0x0  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPBCH_ASG_LOG_VERSION               (0x1)

#define LOG_NB1FW_DBACK_NPBCH_DECODE_PARAMS_LOG_C           (0x1  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPBCH_DECODE_PARAMS_LOG_VERSION     (0x1)

#define LOG_NB1FW_DBACK_NPBCH_DECODE_RESULT_LOG_C           (0x2  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPBCH_DECODE_RESULT_LOG_VERSION     (0x1)

#define LOG_NB1FW_DBACK_NPDCCH_ASG_LOG_C                    (0x3  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDCCH_ASG_LOG_VERSION              (0x1)

#define LOG_NB1FW_DBACK_NPDCCH_DECODE_PARAMS_LOG_C          (0x4  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDCCH_DECODE_PARAMS_LOG_VERSION    (0x1)

#define LOG_NB1FW_DBACK_NPDCCH_DECODE_RESULT_LOG_C          (0x5  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDCCH_DECODE_RESULT_LOG_VERSION    (0x3)

#define LOG_NB1FW_DBACK_NPDSCH_ASG_LOG_C                    (0x6  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDSCH_ASG_LOG_VERSION              (0x3)

#define LOG_NB1FW_DBACK_NPDSCH_DECODE_PARAMS_LOG_C          (0x7  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDSCH_DECODE_PARAMS_LOG_VERSION    (0x1)

#define LOG_NB1FW_DBACK_NPDSCH_DECODE_RESULT_LOG_C          (0x8  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDSCH_DECODE_RESULT_LOG_VERSION    (0x4)

#define LOG_NB1FW_DBACK_NPDSCH_DECODE_DATA_LOG_C            (0x9  + LOG_NB1FW_DBACK_BASE_C)
#define LOG_NB1FW_DBACK_NPDSCH_DECODE_DATA_LOG_VERSION      (0x1)

#define LOG_NB1FW_DBACK_DECODE_LLR_LOG_C                    (0x1 + LOG_NB1FW_EXP_RX_BASE_C)
#define LOG_NB1FW_DBACK_DECODE_LLR_LOG_VERSION              (0x1)

/* log code mask */
#define LOG_NB1FW_DBACK_MASK ( \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPBCH_ASG_LOG_C            ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPBCH_DECODE_PARAMS_LOG_C  ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPBCH_DECODE_RESULT_LOG_C  ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDCCH_ASG_LOG_C           ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDCCH_DECODE_PARAMS_LOG_C ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDCCH_DECODE_RESULT_LOG_C ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDSCH_ASG_LOG_C           ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDSCH_DECODE_PARAMS_LOG_C ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDSCH_DECODE_RESULT_LOG_C ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_NPDSCH_DECODE_DATA_LOG_C   ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_DBACK_DECODE_LLR_LOG_C           ) \
  )

// ----------------------------------------------------------------------------
// Uplink
// ----------------------------------------------------------------------------

/*! @brief NB1 UL log packets */
#define LOG_NB1FW_UL_TX_SCHED_LOG_C              (0x0  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_SCHED_LOG_VERSION        (0x2)

#define LOG_NB1FW_UL_TX_PRACH_LOG_C              (0x1  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_PRACH_LOG_VERSION        (0x1)

#define LOG_NB1FW_UL_TX_PUSCH_LOG_C              (0x2  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_PUSCH_LOG_VERSION        (0x3)

#define LOG_NB1FW_UL_TX_FREQ_ERR_LOG_C           (0x3  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_FREQ_ERR_LOG_VERSION     (0x1)

#define LOG_NB1FW_UL_TX_TIME_TRACK_LOG_C         (0x4  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_TIME_TRACK_LOG_VERSION   (0x3)

#define LOG_NB1FW_UL_TX_ENC_INPUT_LOG_C          (0x5  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_ENC_INPUT_LOG_VERSION    (0x1)

#define LOG_NB1FW_UL_TX_TXFE_INPUT_LOG_C         (0x6  + LOG_NB1FW_UL_BASE_C)
#define LOG_NB1FW_UL_TX_TXFE_INPUT_LOG_VERSION   (0x1)

/* log code mask */
#define LOG_NB1FW_UL_MASK ( \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_SCHED_LOG_C             ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_PRACH_LOG_C             ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_PUSCH_LOG_C             ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_FREQ_ERR_LOG_C          ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_TIME_TRACK_LOG_C        ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_ENC_INPUT_LOG_C         ) | \
  LOG_NB1FW_MASK_OFS( LOG_NB1FW_UL_TX_TXFE_INPUT_LOG_C        ) \
  )

// Mask of valid log codes
// ML1 needs to forward log masks to NB1 FW, and this mask contains the offsets
// from LOG_NB1FW_BASE_C that have valid codes. It must be kept up to date
// as new codes are added.
#define LOG_NB1FW_VALID_MASK ( \
  ( LOG_NB1FW_SYS_RX_MASK ) | \
  ( LOG_NB1FW_SRCH_MASK   ) | \
  ( LOG_NB1FW_DL_MASK     ) | \
  ( LOG_NB1FW_UL_MASK     ) | \
  ( LOG_NB1FW_DBACK_MASK  ) \
  )

#endif /* NB1FW_LOG_CODES_H */

