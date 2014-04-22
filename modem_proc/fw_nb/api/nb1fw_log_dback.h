/*!
  @file
  nb1fw_log_dback.h

  @brief
  NB1 FW Demback logging interface

  @detail
  NB1 FW Demback logging interface

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/nb1fw_log_dback.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
9/29/16    amitm      Created the header file for DL Logging
===========================================================================*/

/*
 * nb1fw_log_dback.h
 *
 *  Created on: Sep 29, 2016
 *      Author: amitm
 */

#ifndef NB1FW_LOG_DBACK_H

#define NB1FW_LOG_DBACK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/* number of packets buffered before submit for Demback */
#define NB1FW_LOG_DBACK_NPBCH_ASG_NUM_BUFS                1
#define NB1FW_LOG_DBACK_NPBCH_DECODE_PARAMS_NUM_BUFS      8
#define NB1FW_LOG_DBACK_NPBCH_DECODE_RESULT_NUM_BUFS      1
#define NB1FW_LOG_DBACK_NPDCCH_ASG_NUM_BUFS               1
#define NB1FW_LOG_DBACK_NPDCCH_DECODE_PARAMS_NUM_BUFS     8
#define NB1FW_LOG_DBACK_NPDCCH_DECODE_RESULT_NUM_BUFS     1
#define NB1FW_LOG_DBACK_NPDSCH_ASG_NUM_BUFS               1
#define NB1FW_LOG_DBACK_NPDSCH_DECODE_PARAMS_NUM_BUFS     8
#define NB1FW_LOG_DBACK_NPDSCH_DECODE_RESULT_NUM_BUFS     1

/*! @brief Number of LLR values to capture per ms per LLR buffer. 336 = 14 sym x 12 subcarrier x 2 bits_per_QPSK */
#define NB1FW_LOG_DBACK_NUM_LLR1_PER_MS                  336

typedef enum
{
  FAIL = 0,
  PASS
} nb1fw_crc_status_e; 
 
/*==========================================================================*/
/* NB1 FW NPBCH Demback ASG Summary (Log code ID: 0xB220)
 *---------------------------------------------------------------------------*/

typedef struct
{
  uint16    startSfn;             /**< Start System Frame Number */
  uint16    cellId;               /**< Cell Id */
} nb1fw_log_dback_npbch_asg_s;


/*! This log packet gives NPBCH initialization parameterssent to demback.
 *  The log record is generated once every NPBCH period and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 2 bytes
 *  Max Size = 6 bytes
 *
 *  Display Format:
 * |---------------------|
 * | NPBCH ASG Summary   |
 * |---------------------|
 * | Cell Id|  Start SFN |
 * |---------------------|
 * |     0  |    12      |
 * |---------------------|

*/

typedef struct
{
  /* 0..16 */
  uint8     version;   /**< log packet version */
  uint8     bufCnt;    /**< 0 - 255 max number of entries supported */

  /* 16..48 */
  nb1fw_log_dback_npbch_asg_s buf[NB1FW_LOG_DBACK_NPBCH_ASG_NUM_BUFS];
} nb1fw_log_dback_npbch_asg_ind_s;

/*==========================================================================*/
/* NB1 FW NPBCH Demback Decode Parameters Summary (Log code ID: 0xB221)
 *---------------------------------------------------------------------------*/

typedef struct
{
  uint8 crcPass      : 1;  /**< b0  CRC pass, 0 = N, 1 = Y */
  uint8 emSeT_pass   : 1;  /**< b1  emSeT_pass, 0 = N, 1 = Y */
  uint8 tm_pass      : 1;  /**< b2  tail match pass, 0 = N, 1 = Y */
  uint8 SNR_pass     : 1;  /**< b3  SNR_pass, 0 = N, 1 = Y */
  uint8 boundaryHit  : 1;  /**< b4  boundary hit indicator, 0 = N, 1 = Y */
  uint8 reserved     : 3;  /**< b5-b7 */
} pruningPassFlgs_s;

typedef struct
{
  /* 0..32 */
  uint16            energyVal;        /**< Energy Value */
  uint16            sumNzLlrs;        /**< Total LLRs */

  /* 32..64 */
  uint8             snr_estimate;     /**< SNR Estimate*/
  uint8             symMismatchCnt;   /**< Total Symbol Mismatch */
  uint8             numNzLlrs;        /**< Number of LLRs */
  pruningPassFlgs_s pruningPassFlgs;  /**< Pruning Flags */
} pruningHypo_s;

typedef struct
{
  /* 0..32 */
  uint16        sfn;                  /**< System Frame Number */
  uint16        pbchSfn;              /**< PBCH Start System Frame Number */

  /* 32..1056 */
  pruningHypo_s pruningData[16];      /**< PBCH Pruning data */
} nb1fw_log_dback_npbch_decode_params_s;

/*! This log packet gives NPBCH parameters sent to demback module.
 *  The log record is generated once every NPBCH period and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 2 bytes
 *  Max Size = 1058 bytes
 *  Display Format:
 * |-------------------------------------------------------------------------------------------------------------|
 * |   DL time     |  hypothesis  |    Pruning chks pass   |               PRUNING METRICS                       |
 * |               |--------------|------------------------|-----------------------------------------------------|
 * |[sysFr:pbchSFN]| nAnt | phase | crc | Snr | Tail | Em  | energy | L1_LLR | estSNR | # MismtchSym | numNzLlrs |
 * |---------------|------|-------|-----|-----|------|-----|--------|--------|--------|--------------|-----------|
 * |    12 :    12 |   0  |   0   |  N  |  N  |   N  |  N  |     13 |     13 |   14   |       0      |     14    |
 * |               |   1  |   0   |  N  |  N  |   N  |  N  |     15 |     15 |    0   |       0      |      0    |
 * |               |   0  |   1   |  Y  |  Y  |   N  |  N  |      0 |      0 |    0   |       0      |      0    |
 * |---------------|------|-------|-----|-----|------|-----|--------|--------|--------|--------------|-----------|
*/
typedef struct
{
  /* 0..16 */
  uint16     version       : 8;   /**< log packet version */
  uint16     bufCnt        : 8;   /**< 0 - 255 max number of entries supported */

  /* 16..8464 */
  nb1fw_log_dback_npbch_decode_params_s buf[NB1FW_LOG_DBACK_NPBCH_DECODE_PARAMS_NUM_BUFS];
} nb1fw_log_dback_npbch_decode_params_ind_s;

/*==========================================================================*/
/* NB1 FW NPBCH Demback Decode Result (Log code ID: 0xB222)
 *---------------------------------------------------------------------------*/

 /*! This log packet gives NPBCH Decode result from demback module.
 *  The log record is generated for every NPBCH decode and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 18 bytes
 *  Max Size = 18 bytes
 *  Display Format:
 * |--------------------------------------------------------------------------------------------|
 * | NPBCH Decode Results                                                                       |
 * |--------------------------------------------------------------------------------------------|
 * | CRC  |  Hypothesis | nAntenna | 640ms Start | Decoded SFN | Computed CRC | Decoded Data    |
 * |------|-------------|----------|-------------|-------------|--------------|-----------------|
 * | PASS |       1     |      1   |       0     |      0      |    0x0f00    |  0x13a3003e     |
 * |                                                                          |  0x04e8c000     |
 * |--------------------------------------------------------------------------------------------|
*/

typedef struct
{
  /* 0..32 */
  uint8     version;              /**< log packet version */
  uint8     hyp;                  /**< Passing Hypothesis */
  uint8     nAnt;                 /**< Number of Antennas */
  uint8     __notused__;

  /* 32..64 */
  uint16    startSfn;             /**< 640 ms boundary  */
  uint16    decodeSfn;            /**< SFN when decode passed */

  /* 64..144 */
  uint16                 computedCRC;          /**< CRC-16 from decoded bits */
  nb1fw_crc_status_e     crcPass;              /**< CRC Pass indicator 
                                                * 0 = fail, 1 = pass
                                                */
  uint8     decodedBits[7];       /**< Decoded Bits including CRC */
} nb1fw_log_dback_npbch_decode_result_ind_s;

/*==========================================================================*/
/* NB1 FW NPDCCH Demod ASG Results Summary (Log code ID: 0xB223)
 *---------------------------------------------------------------------------*/

 typedef struct
{
  /* 0..48 */
  uint16    rmax;                  /**< Max number of repetitions (1 to 2048) */
  uint16    numBitsPerSfFull;      /**< Number of Bits per SF (200 to 320) */
  uint16    numBitsPerSfCCE;       /**< Number of Bits per SF (100 to 160) */

  /* 48..96 */
  uint16    rntiBits[2];           /**< 16 RNTI bits */
  uint16    startingSF;            /**< NPDSCH TB starting subframe number */

  /* 96..128 */
  uint8     numInfoBits;           /**< Number of Info bits in NPDCCH (up to 23) */
  uint8     ssType;                /**< Search space type (paging CSS, other CSS, USS) */
  uint8     numHypo;               /**< Number of Hypothesis(1-4) */
  uint8     numRnti;               /**< Number Rnti (1 or 2) */
} nb1fw_log_dback_npdcch_asg_s;

/*! This log packet gives NPDCCH initialization parameterssent to demback.
 *  The log record is generated once every NPBCH period and k records are collected  
 *  before submitting the log packet. where 'k' is configurable. 
 *  Min Size = 2 bytes
 *  Max Size = 18 bytes
 *  Display Format:
 * |-------------------------------------------------------------------------------------------|
 * | NPDCCH ASG Summary                                                                        |
 * |-------------------------------------------------------------------------------------------|
 * | Rmax  | Num. of Bits/SF|     RNTI Bits     | Starting | Number of | SS Type | Num  | Num  |
 * |       |  Full | CCE    |    0    |    1    | Subframe | Info Bits |         | Hypo | RNTI |
 * |-------------------------------------------------------------------------------------------|
 * |    16 |   320 |   160  | 0x001f  | 0x6003  |      0   |      23   |    USS  |    4 |    0 |
 * |-------------------------------------------------------------------------------------------|
*/
typedef struct
{
  /* 0..16 */
  uint8     version;   /**< log packet version */
  uint8     bufCnt;   /**< 0 - 255 max number of entries supported */

  /* 16..144 */
  nb1fw_log_dback_npdcch_asg_s buf[NB1FW_LOG_DBACK_NPDCCH_ASG_NUM_BUFS];
} nb1fw_log_dback_npdcch_asg_ind_s;

/*==========================================================================*/
/* NB1 FW NPDCCH Demod Decode Parameters Summary (Log code ID: 0xB224)
 *---------------------------------------------------------------------------*/
typedef struct
{
  uint8 dci_pass    : 1;   /**< b0, 0 = N, 1 = Y */
  uint8 emSeT_pass  : 1;   /**< b1  emSeT_pass, 0 = N, 1 = Y */
  uint8 SNR_pass    : 1;   /**< b2  SNR_pass, 0 = N, 1 = Y */
  uint8 tm_pass     : 1;   /**< b3  tail match indicator, 0 = N, 1 = Y */
  uint8 crc_pass    : 1;   /**< b4, 0 = N, 1 = Y */
  uint8 boundaryHit : 1;   /**< b5  decode boundary hit indicator, 0 = N, 1 = Y */
  uint8 hypo        : 2;   /**< Hypothesis # (b7-b6) */
} pruningHypoDecFlgs_s;


typedef struct
{
  /* 0..32 */
  uint16                energyVal;          /**< Energy Value */
  uint16                sumNzLlrs;          /**< Total LLRs */

  /* 32..64 */
  int8                  snr_estimate;       /**< SNR Estimate */
  uint8                 symMismatchCnt;     /**< Total Symbol Mismatch */
  uint8                 numNzLlrs;          /**< Number of LLRs */
  pruningHypoDecFlgs_s  pruningHypoDecFlgs; /**< Pruning Hypothesis Flags */
} pruningData_s;

typedef struct
{
  /* 0..256 */
  pruningData_s     pruningData[4];        /**<  for hypothesis 0-3 */

  /* 256..320 */
  uint32            cinit;                 /**< Current value of cinit */
  uint16            sysFNum;               /**< System Frame number */
  uint16            subFrameNum;           /**< Sub-Frame number */

  /* 320..352 */
  uint32            sfCntr         : 16;   /**< subframe counter */
  uint32                           : 16;
} nb1fw_log_dback_npdcch_decode_params_s;

/*! This log packet gives NPDCCH parameters sent to demback module.
 *  The log record is generated once every NPDCCH period and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 4 bytes
 *  Max Size = 356 bytes
 *  Display Format:
 * |------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |   DL time     |   Cinit    | sfCntr | hypothesis  |    Pruning checks pass         |                                             PRUNING METRICS                                                     |
 * |               |            |        |-------------|--------------------------------|-----------------------------------------------------------------------------------------------------------------|
 * | [sysFN:subFN] |            |        | eval | dec  | crc | Snr | Tail | Em   | Dci  |           energy            |          L1_LLR             |      estSNR     |  # MismtchSym   |    numNzLlrs    |
 * |---------------|------------|--------|------|------|-----|-----|------|------|------|-----------------------------|-----------------------------|-----------------|-----------------|-----------------|
 * |   2048:     0 | 0x00040056 |      0 | ---- | ---- |NNNN |NNNN | NNNN | NNNN | NNNN |      0      0      0      0 |      0      0      0      0 |   0   0   0   0 |   0   0   0   0 |   0   0   0   0 |
 * |      0:     0 | 0x00000800 |      0 | ---- | ---- |NNNN |NNNN | NNNN | NNNN | NNNN |  57568      0      0      0 |      0      0      0      0 |   0   0   0   0 |   0   0   0   0 |   0   0   0   0 |
 * |      0:     0 | 0x00000000 |      0 | ---- | ---- |NNNN |NNNN | NNNN | NNNN | NNNN |      2      0      0      0 |  57364      0      0      0 | -32   0   0   0 | -32   0   0   0 |   0   0   0   0 |
 * |      0:     0 | 0x00000000 |      0 | 4--- | 1--- |NNNN |NNNN | NNNN | NNNN | NNNN |     86  57568      0      0 |      7      0      0      0 |   3   0   0   0 |   0   0   0   0 | 224   0   0   0 |
 * |      0:     0 | 0x00000000 |      0 | ---- | ---- |NNNN |NNNN | NNNN | NNNN | NNNN |   2048      4      0      0 |      0  13332      0      0 | -42 -32   0   0 |   0 -32   0   0 |   8   0   0   0 |
 * |      0:     0 | 0x00000000 |      0 | -4-- | -1-- |NNNN |NNNN | NNNN | NNNN | NNNN |      0     87  57568      0 |      0      1      0      0 |   0   5   0   0 |   2   0   0   0 |   0 224   0   0 |
 * |      0:     0 | 0x00000000 |      0 | ---- | ---- |NNNN |NNNN | NNNN | NNNN | NNNN |      0    512      6      0 |      0      0  57364      0 |   0 -41 -32   0 |   0   0 -32   0 |   0   2   0   0 |
 * |      0:     0 | 0x00000000 |      0 | --4- | --1- |NNNN |NNNN | NNNN | NNNN | NNNN |      0      0     87  57568 |      0      0      3      0 |   0   0   7   0 |   0   2   0   0 |   0   0 224   0 |
 * |     64: 45569 | 0x00000000 |      0 | ---- | ---- |NNNN |NNNN | NNNN | NNNN | NNNN |      0      0    512      8 |      0      0      0  12308 |   0   0 -41 -48 |   0   0   0 -32 |   0   0   4   0 |
 * |-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *
*/
typedef struct
{
  /* 0..32 */
  uint16     version   : 8;   /**< log packet version */
  uint16               : 8;
  uint16     bufCnt;    /**< max number of entries supported */

  /* 32..2848 */
  nb1fw_log_dback_npdcch_decode_params_s buf[NB1FW_LOG_DBACK_NPDCCH_DECODE_PARAMS_NUM_BUFS];
} nb1fw_log_dback_npdcch_decode_params_ind_s;

/*==========================================================================*/
/* NB1 FW NPDCCH Demod Decode Result (Log code ID: 0xB225)
 *---------------------------------------------------------------------------*/

 /*! This log packet gives NPDCCH Decode result from demback module.
 *  The log record is generated for every NPDCCH decode and k records are collected  
 *  before submitting the log packet. where 'k' is configurable. 
 *  Min Size = 20 bytes
 *  Max Size = 20 bytes
 *  Display Format:
 *  |-----------------------------------------------------------------------------------------------------------------------------------|
 *  | NPDCCH Decode Results                                                                                                             |
 *  |-----------------------------------------------------------------------------------------------------------------------------------|
 *  | CRC  |  Decode   | Computed  | Success | Decoded Bits  |  Start SF of  |  RNTI  |   RNTI  |  twoHarq  | sysFrm |subFrm |
 *  |      |  Attempts | CRC       | Hypo    |               |  success hypo |  Index |  type   |           |        |       |
 *  |-----------------------------------------------------------------------------------------------------------------------------------|
 *  | FAIL |     15    | 0x7e8e   |      0  |  0x2c7e57184a |  0xffff       |    0   |  C-RNTI |    enable  |  266   |  8    |
 *  |-----------------------------------------------------------------------------------------------------------------------------------|

*/
typedef struct
{
  /* 0..48 */
  uint8                version            ;/**< log packet version */
  uint8                decodedBits[5]     ;/**< Decoded bits (including CRC-16) (up to 5 bytes) */

  /* 48..96 */
  uint16               startSfOfSuccessHyp;/**< starting SF index of successful hypothesis */
  uint16               numDecodeAttempts  ;/**< Actual number of decode attempts (1 to 2048) */
  uint16               computedCRC        ;/**< Computed CRC-16 bits */

  /* 96..128 */
  nb1fw_crc_status_e   crcPass            ;/**< CRC pass indicator */
  uint8                numDecodedBytes    ;/**< Number of decoded bytes (including CRC-16) */
  uint8                successHypo        ;/**< Winning hypothesis (1 to 4) */
  uint8                twoHarqEnable :2   ;/**< Two HARQ Enable > **/
  uint8                rntiIndex     :2   ;/**< index of RNTI (0 or 1) that passes CRC check. Ignore if crcPass is FALSE */

  /*! NB1_SI_RNTI_TYPE   = 0  RNTI to identify System Information */
  /*! NB1_C_RNTI_TYPE    = 1  RNTI to decode dedicated UE data in connected mode */
  /*! NB1_P_RNTI_TYPE    = 2  RNTI used to identify paging information */
  /*! NB1_RA_RNTI_TYPE   = 3  RNTI associated with PDCCH carrying RACH response */ 
  /*! NB1_TEMP_RNTI_TYPE = 4  Temporary RNTI to decode MSG 4 of the RACH procedure */
  uint8                rntiType      :4   ;/**< RNTI Type Information */  
  
  /* 128..160 */
  uint16               sysFrameNum        ;/**< System Frame number */
  uint16               subFrameNum        ;/**< Sub-Frame number */

} nb1fw_log_dback_npdcch_decode_result_ind_s;

/*==========================================================================*/
/* NB1 FW NPDSCH Demod ASG Results Summary (Log code ID: 0xB226)
 *---------------------------------------------------------------------------*/
typedef struct
{
  /* 0..64 */
  uint16    rmax;                  /**< Max number of repetitions (1 to 2048). This is derived param: zPeriod*numRepCycles */
  uint16    numInfoBits;           /**< Number of Info bits in TB (16 to 680) */
  uint16    numBitsPerSf;          /**< Number of Bits per SF (200 to 320) */
  uint16    numRepCycles;          /**< Number of repetition cycles (1 to 512) */

  /* 64..128 */
  uint8     nprb;                  /**< Number of PRBs for 1 TB (1 to 10) */
  uint8     zPeriod;               /**< Z period (1, 2, 4) */
  uint16    startingSF;            /**< NPDSCH TB starting subframe number */
  boolean   sibIndicator;          /**< “1” if SIB, “0” otherwise */
  boolean   ndi;                   /**< New Data Indicator Bit */
  boolean   firstDecode;           /**< Indicates if it is the first decode for SIB or a recombination */
  uint8     __notused__;           /**< Not used */
} nb1fw_log_dback_npdsch_asg_s;

/*! This log packet gives NPDSCH initialization parameters sent to demback.
 *  The log record is generated once every NPDSCH period and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 2 bytes
 *  Max Size = 18 bytes
 *  Display Format:
 * |----------------------------------------------------------------------------------------------------|
 * | NPDSCH ASG Summary                                                                                 |
 * |----------------------------------------------------------------------------------------------------|
 * | Rmax | Number of | Number of  | Number of  | nprb | zPeriod |  Starting | SIB       | ndi  | First |
 * |      | Info Bits | Bits per SF| Rep Cycles |      |         |  SF       | Indicator |      |decode |
 * |--------------------------------------------------------------------------------------------|-------|
 * |   2  |    680    |     320    |      2     |   8  |     1   |    841    |       0   | TRUE | TRUE  |
 * |--------------------------------------------------------------------------------------------|-------|

*/
typedef struct
{
  /* 0..16 */
  uint8     version;   /**< log packet version */
  uint8     bufCnt;    /**< 0 - 255 max number of entries supported */

  /* 16..144 */
  nb1fw_log_dback_npdsch_asg_s buf[NB1FW_LOG_DBACK_NPDSCH_ASG_NUM_BUFS];
} nb1fw_log_dback_npdsch_asg_ind_s;

/*==========================================================================*/
/* NB1 FW NPDSCH Demod Decode Parameters Summary (Log code ID: 0xB227)
 *---------------------------------------------------------------------------*/
typedef struct
{
  /* 0..64 */
  uint32   cinit;                  /**< Current value of cinit */
  uint16   sfCntr;                 /**< subframe counter */
  uint16   subFrameNum;            /**< Sub-Frame number */

  /* 64..128 */
  uint16   sysFrameNum;            /**< System Frame number */
  int16    nRepCycleCntr;          /**< Current NrepCycle counter (1 to 512) */
  int16    zCntr;                  /**< Current Zperiod counter (1 to 4) */
  int16    nPrbCntr;               /**< Current Nprb counter (1 to 10) */
  
} nb1fw_log_dback_npdsch_decode_params_s;

/*! This log packet gives NPDSCH parameters sent to demback module.
 *  The log record is generated once every NPDSCH period and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 4 bytes
 *  Max Size = 132 bytes
 *  Display Format:
 *  |-----------------------------------------------------------------------------|
 *  | NPDSCH Decode Parameters                                                    |
 *  |-----------------------------------------------------------------------------|
 *  | Global   | Subframe | System Frame |   cinit    | zCntr | nPrb |  Number of |
 *  | Subframe | Counter  | Number       |            |       |      |  Rep Cycle |
 *  |-----------------------------------------------------------------------------|
 *  |    644   |      1   |      64      | 0x3FFFC800 |     0 |    1 |      2     |
 *  |-----------------------------------------------------------------------------|
 *  |    664   |      2   |      66      | 0x3FFFC800 |     0 |    1 |      2     |
 *  |-----------------------------------------------------------------------------|
*/
typedef struct
{
  /* 0..32 */
  uint32     version       : 8;   /**< log packet version */
  uint32     bufCnt        : 8;   /**< 0 - 255 max number of entries supported */
  uint32                   : 16;

  /* 32..1056 */
  nb1fw_log_dback_npdsch_decode_params_s buf[NB1FW_LOG_DBACK_NPDSCH_DECODE_PARAMS_NUM_BUFS];
} nb1fw_log_dback_npdsch_decode_params_ind_s;

/*==========================================================================*/
/* NB1 FW NPDSCH Demod Decode Result (Log code ID: 0xB228)
 *---------------------------------------------------------------------------*/
 
/*! This log packet gives NPDSCH Decode result from demback module.
 *  The log record is generated for every NPDSCH decode and k records are collected  
 *  before submitting the log packet. where 'k' is configurable.
 *  Min Size = 32 bytes
 *  Max Size = 32 bytes
 *  Display Format:
 *  |----------------------------------------------------------------------------------------------------------------------------------|
 *  | NPDSCH Decode Results                                                                                                            |
 *  |----------------------------------------------------------------------------------------------------------------------------------|
 *  | Global   | System Frame | Number of     | Number of       | Computed   | CRC  | Energy | Sum of  | Symb Mismatch | Num Nz | SNR  |
 *  | Subframe | Number       | Decoded Bytes | Decode Attempts | CRC        |      |        | Nz LLRs | Count         | LLRs   |      |
 *  |-------------------------|--------------------------------------------------------------------------------------------------------|
 *  |    644   |      64      |      29       |      1          |  0xd01e62  | PASS | 22994  | 23562   |    9          | 744    | 10   |
 *  |----------------------------------------------------------------------------------------------------------------------------------|
*/  

typedef struct
{
   /* 0..64 */
   uint32                energyVal;          /**< Energy Value */
   uint32                sumNzLlrs;          /**< Total LLRs */
  
   /* 64..96 */
   uint16                symMismatchCnt;     /**< Total Symbol Mismatch */
   uint16                numNzLlrs;          /**< Number of LLRs */
  
   /* 96..128 */
   int32                 snr_estimate;        
} llr_metrics_s;
    
typedef struct
{
  /* 0..31 */
  uint32               version       : 8;     /**< log packet version */
  uint32               harqId        : 8;     /**< npdsch harq iq*/
  uint32                             :16;     /**< */
  /* 32..63 */
  uint16               sysFrameNum;           /**< System Frame number */
  uint16               subFrameNum;           /**< Sub-Frame number */
  
  /* 64..95 */
  uint16               numDecodedBytes;       /**< Number of decoded bytes (including CRC-24) */
  uint16               numDecodeAttempts;     /**< Actual number of decode attempts (1 to 2048) */

  /* 96..127 */
  uint8                computedCRC[3];        /**< Computed CRC-24 bits */
  nb1fw_crc_status_e   crcPass;               /**< CRC pass indicator */

  /* 128..255*/
  llr_metrics_s llrMetrics;                   /**< Structure for LLR metrics */
} nb1fw_log_dback_npdsch_decode_result_ind_s;

/*==========================================================================*/
/* NB1 FW NPDSCH Demod Decoded Data (Log code ID: 0xB229)
 *---------------------------------------------------------------------------*/
typedef struct
{
  uint8     version;               /**< log packet version */
  uint8     decodePktNum;          /**< Serial Number of Decoded Data Packets */
  uint8     decodedBits[11];       /**< Decoded bits (including CRC-24) (up to 88 bytes) */
} nb1fw_log_dback_npdsch_decoded_data_s;


/*==========================================================================*/
/* LTE NB1 FW DBack Decode LLR (Log code ID: 0xB231)
 *---------------------------------------------------------------------------*/
/*! This log packet gives LLR input values for sent to demback.
 *  The log record is generated once every NPDSCH period and k records are collected  
 *  Min Size = 16 bytes  (when llrValidSize = 0, do not print array llr[])
 *  Max Size = 688 bytes
 *  Display Format:
 * Version             = 3
 * SFN                 = 143
 * Sub FN              = 2
 * Channel Type        = PDCCH
 * Channel Info        = 1
 * LLR Buffer Index    = 0
 * LLR Valid Size      = 260 
 * LLR Values = {
 *    100, -200, 300, -200,  
 * ...
 * }
 */
typedef struct
{
  /* 0..31 */
  uint32     version   : 8;        /**< log packet version */
  uint32               : 24;

  /* 32..63*/
  uint16     sysFrameNum;          /**< System Frame number */
  uint16     subFrameNum;          /**< Sub-Frame number */

  /* 64..95*/
  uint16     chanType;             /**< channel type, nb1fw_dl_chan_e in \fw\nb\nb1\dl\inc\nb1fw_dl.h */
                                   /* chanType display: INVALID = -1, PBCH = 0, SIB = 1, PDCCH = 2, PDSCH = 3, PSS_SSS = 4, NRS = 5, MEAS_PBCH = 6 */
  uint16     chanInfo;             /**< channel info of nb1fw_dback_chan_decode_info_s in fw\nb\nb1\dback\inc\nb1fw_dback.h  */
                                   /* when chanType = PDSCH, chanInfo is nb1fw_dl_decode_type_e in fw\nb\nb1\api\intf_nb1fw_dl.h*/

  /* 96..127*/
  uint16     llrBufIdx;            /**< llr buf idx, 0, 1, or 2 */
  uint16     llrValidSize;         /**< valid number of elements in array llr[] */

  /* 128..5503*/
  int16      llr[NB1FW_LOG_DBACK_NUM_LLR1_PER_MS]; /* LLR values in 16S10 format; only display llrValidSize elements */
} nb1fw_log_dback_decode_llr_ind_s;


#endif /**< NB1FW_LOG_DBACK_H */
