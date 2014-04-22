 /*!
  @file
  nb1fw_logging_ul.h

  @brief
  NB1 FW UL logging interface

  @detail
  NB1 FW UL logging interface

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/nb1fw_log_ul.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
22/04/17   NM      Created UL time tracking log packet.
12/08/16   HS      Update PUSCH log packet 
11/07/16   HS      Created header file for UL Logging
===========================================================================*/

#ifndef NB1FW_LOG_UL_H
#define NB1FW_LOG_UL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/**< number of packets buffered before submitting for UL sched */
#define NB1FW_LOG_UL_SCHED_NUM_BUFS 20

#define NB1FW_LOG_UL_PRACH_NUM_BUFS 20

#define NB1FW_LOG_UL_PUSCH_NUM_BUFS 10

#define NB1FW_LOG_UL_FREQ_ERR_NUM_BUFS 5

#define NB1FW_LOG_UL_TIME_TRACK_NUM_BUFS 5

/*===========================================================================

      Typedefs

===========================================================================*/

/*==========================================================================*/
/* NB1 FW Uplink Scheduler Summary (Log code ID: 0xB218)
 *---------------------------------------------------------------------------*/
 
/**< UL scheduler logging structure */
typedef struct
{
  /**< ~ Word0 ~ */
  /**< Tx state */
  /* 0..15 */
  union
  {
    struct
    {
      uint16 prachState         : 3;  /**< PRACH channel state, 0 = DISABLED, 1 = CONFIG, 2 = IN_GAP, 3 = ENABLED */
      uint16 puschState         : 3;  /**< PUSCH channel state, 0 = DISABLED, 1 = DISABLING_IN_PROGRESS, 2 = WAIT, 
                                        *  3 = IN_GAP, 4 = DATA_ENABLED, 5 = ACK_NACK_ENABLED */
      uint16 inGap              : 1;  /**< Flag indicating 40 ms transmission gap */   
      uint16 lastSentRfStatus   : 2;  /**< last sent RF On/Off cmd, 0 = DTX, 1 = ACTIVE, 2 = SHORT_DTX */
      uint16 lastSentTxlmStatus : 2;  /**< last sent TxLM On/Off cmd, 0 = DTX, 1 = ACTIVE, 2 = SHORT_DTX */
      uint16                    : 5;  /**< UNUSED */
    };
    uint16 info;
  } state;

  /**< sched time */
  /* 16..31 */
  union
  {
    struct
    {
      uint16 subFrNum : 4;  /**< subframe index [0..3] */
      uint16 sFrNum   : 10; /**< system frame number [4..13] */
      uint16          : 2;  /**< UNUSED */
    };
    uint16 info;
  } time;
} nb1fw_log_ul_sched_s;

/**< The uplink scheduler summary shows information regarding the status of uplink scheduler 
  * along with the RF status for each subframe that the transmission thread is handling PRACH/PUSCH channels. 
  * Min Size = 2 bytes 
  * Max Size = 82 bytes
  * Display Format:
  * |----------------------------------------------------------------------|
  * | UL Scheduler Summary                                                 |
  * |----------------------------------------------------------------------|
  * | Schedule  |  PRACH  | PUSCH | In Gap? | Status of   | Status of      |
  * | Time      |  State  | State |         | Last sent RF| Last sent Txlm |
  * |----------------------------------------------------------------------|
  * |  266:1    |     0   |     2 |     0   |       0     |       0        |
  * |----------------------------------------------------------------------|
  * |  266:1    |     0   |     2 |     0   |       0     |       0        |
  * |----------------------------------------------------------------------|
  */
typedef struct
{
  /* 0..15 */
  uint8 version; /**< version of the log packet */    
  uint8 bufCnt;  /**< Number of valid items logged*/
  
  /* 16..655 */
  nb1fw_log_ul_sched_s buf[NB1FW_LOG_UL_SCHED_NUM_BUFS];  /**< Logging buffer */

} nb1fw_log_ul_sched_ind_s;

/*==========================================================================*/
/* NB1 FW NPRACH Transmission Summary (Log code ID: 0xB219)
 *---------------------------------------------------------------------------*/
 
/**< UL PRACH (per period) logging structure */
typedef struct
{
  /**< ~ Word0 ~ */
  /**< PRACH parameter update per period */
  /* 0..15 */
  union
  {
    struct
    {
      uint16 groupIdx     : 2;  /**< symbol group idx */
      uint16 repIdx       : 7;  /**< Occurrence/repetition idx */
      uint16 toneOffset   : 4;  /**< tone offset within 12-tone group */
      uint16              : 3;  /**< UNUSED */
    };
    uint16 info;
  } state;
} nb1fw_log_ul_prach_s;

/**< The PRACH transmission summary shows information regarding parameters and info 
  * used for each of the transmission sent during the PRACH period 
  * Min Size = 2 bytes 
  * Max Size = 42 bytes 
  * Display Format:
  * |-------------------------------------------------------|
  * | PRACH Transmission Summary                            |
  * |-------------------------------------------------------|
  * |  Group Index   |   Repetition Index   |  Tone Offset  |
  * |-------------------------------------------------------|
  * |      0         |      0               |      3        |
  * |-------------------------------------------------------|
  * |      1         |      0               |      2        |
  * |-------------------------------------------------------|
  */
typedef struct
{
  /* 0..15 */
  uint8 version; /**< version of the log packet */    
  uint8 bufCnt;  /**< Number of valid items logged */
  
  /* 16..335 */
  nb1fw_log_ul_prach_s buf[NB1FW_LOG_UL_PRACH_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_ul_prach_ind_s;

/*==========================================================================*/
/* NB1 FW NPUSCH Transmission Summary (Log code ID: 0xB21A)
 *---------------------------------------------------------------------------*/
 
/**< UL PUSCH (per period) logging structure */
typedef struct
{
  /**< ~ Word0 ~ */
  /**< PUSCH parameter update per period */
  /* 0..31 */
  union
  {
    struct
    {
      uint32 cycleIdx       : 7;  /**< repetition cycle idx */
      uint32 ruIdx          : 4;  /**< RU idx in cycle */      
      uint32 periodIdx      : 4;  /**< period idx in RU */
      uint32 repIdx         : 2;  /**< repetition idx for given period in RU */
      uint32 repInCycle     : 3;  /**< max. repetitions in current cycle */ // AI:  can be replaced with other parameters
      uint32 repCnt         : 8;  /**< cumulative repetition count for given transmission */ // AI:  can be replaced with other parameters
      uint32 rv             : 1;  /**< redundancy version */
      uint32 srsPunc        : 1;  /**< SRS puncturing (0 - no puncturing; 1 - puncturing) */
      uint32 nPrachAvoidGap : 1;  /**< if entering NPRACH avoidance gap from next block period */
      uint32 harqId         : 1;  /**< harq id */
    };
    uint32 info;
  } state;

  /**< ~ Word1 ~ */
  /**< DMRS info */
  /* 32..47 */
  union
  {
    struct
    {
      uint16 baseSeqIdxSlot0   : 5;   /**< u (base seq idx) for first slot */
      uint16 baseSeqIdxSlot1   : 5;   /**< u (base seq idx) for second slot (Fmt1, 15 Khz spacing only) */      
      uint16 occIdxSlot0       : 2;   /**< orthogonal cover code idx for slot 0 (Fmt 2 only) */
      uint16 occIdxSlot1       : 2;   /**< orthogonal cover code idx for slot 1 (Fmt 2, 15 Khz spacing only) */
      uint16 pn35Bit0          : 1;   /**< PN sequence c bit (c_init = 35) for slot 0 (single tone only) */
      uint16 pn35Bit1          : 1;   /**< PN sequence c bit (c_init = 35) for slot 1 (single tone, 15 Khz spacing only) */
    };
    uint16 info; 
  } dmrs;

  /**< time */
  /* 48..63 */
  union
  {
    struct
    {
      uint16 subFrNum : 4;  /**< subframe index [0..3] */
      uint16 sFrNum   : 10; /**< system frame number [4..13] */
      uint16          : 2;  /**< UNUSED */
    };
    uint16 info;
  } time;

} nb1fw_log_ul_pusch_s;

/**< The PUSCH transmission summary shows information regarding the parameters and info 
  * used for the transmission during PUSCH period. One entry is available each PUSCH period 
  * Min Size = 2 bytes 
  * Max Size = 84 bytes 
  * Display Format:
  * |---------------------------------------------------------------------------------------------------------------------------|
  * | PUSCH Decode Results                                                                                                      |
  * |---------------------------------------------------------------------------------------------------------------------------|
  * | Schedule  | Cycle | RU    | Period | Rep   | Rep In  | Rep   | RV | SRS   | nPrach    |              DMRS                 |
  * |           |       |       |        |       |         |       |    |       |           |-----------------------------------|
  * | Time      | Index | Index | Index  | Index | Cycle   | Count |    | Punc. | Avoid Gap |  seq idx  |  occ idx  |   pn35    |
  * |           |       |       |        |       |         |       |    |       |           |-----------------------------------|
  * |           |       |       |        |       |         |       |    |       |           |  0  |  1  |  0  |  1  |  0  |  1  |
  * |---------------------------------------------------------------------------------------------------------------------------|
  * |   121:1   |     0 |     1 |      2 |     0 |      0  |     0 |  0 |     0 |      0    |  10 |  10 |   0 |   0 |   0 |   0 |
  * |---------------------------------------------------------------------------------------------------------------------------|
  * |   121:2   |     0 |     1 |      3 |     0 |      0  |     0 |  0 |     0 |      0    |  10 |  10 |   0 |   0 |   0 |   0 |
  * |---------------------------------------------------------------------------------------------------------------------------|
  */
typedef struct
{
  /* 0..31 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  __notused__  : 16;
    
  /* 32..671 */
  nb1fw_log_ul_pusch_s buf[NB1FW_LOG_UL_PUSCH_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_ul_pusch_ind_s;

/*==========================================================================*/
/* NB1 FW UL Frequency Error Summary (Log code ID: 0xB21B)
 *---------------------------------------------------------------------------*/
 
/**< UL frequency error update (every new/resume transmission) logging structure */
typedef struct
{
  /**< Frequency error info */
  /* 0..31 */
  int32 freqErrHzQ6;      /**< DL freq error in Q6 Hz units */

  /* 32..63 */
  uint32 invDlFreqQ52;    /**< 2^33 / DL freq in Q19 units. Overall Q52 unit */

  /* 64..95 */
  uint32 resamplingPda;   /**< FCW for TXR_PDA_CTRL_An HWIO */

} nb1fw_log_ul_freq_err_s;

/**< The Frequency Error summary shows information regarding parameters and info 
  * used for Tx resampling based on DL frequency error 
  * Min Size = 2 bytes 
  * Max Size = 122 bytes 
  * Display Format:
  * |-----------------------------------------------------------|
  * | Frequency Error Update Summary                            |
  * |-----------------------------------------------------------|
  * | DL Freq Err (Q6 Hz) |   invDlFreqQ52   |  Resampling PDA  |
  * |-----------------------------------------------------------|
  * |      22             |      0           |      3           |
  * |-----------------------------------------------------------|
  * |      80             |      0           |      2           |
  * |-----------------------------------------------------------|
  */
typedef struct
{
  /* 0..31 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  __notused__  : 16;
  
  /* 32..991 */
  nb1fw_log_ul_freq_err_s buf[NB1FW_LOG_UL_FREQ_ERR_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_ul_freq_err_ind_s;

/*==========================================================================*/
/* NB1 FW UL time tracking Summary (Log code ID: 0xB21C)
 *---------------------------------------------------------------------------*/
 
/**< UL time tracking logging structure */
typedef struct
{
  union
  {
    struct
    {
      uint32 entryType      : 2 ;  /**< TA entry type, 0 = NB1FW_UL_PRACH_SCHED, 1 = NB1FW_UL_TA_CMD , 2 = NB1FW_UL_TIME_TRACK  */
      uint32 taCmdType      : 2 ;  /**< TA cmd type, 0 = NB1FW_UL_TA_SLAM, 1 = NB1FW_UL_TA_DELTA_UPDATE, 2 = NB1FW_UL_TA_INVALID */
      uint32 taVal          : 28;  /**< Timing Advance value */      
      //uint32                : 12;  /**< UNUSED */
    };
    uint32 info;
  } taInfo;

  uint32 ulTimeOffset; /**< UL time offset */
  uint32 dlTimeOffset; /**< DL time offset */

  union
  {
    struct
    {
      uint32 subFrNum :  4; /**< subframe index [0..3] */
      uint32 sFrNum   : 10; /**< system frame number [4..13] */
      uint32          : 18; /**< UNUSED */
    };
    uint32 info;
  } lastTxStartTimeTracked;

} nb1fw_log_ul_time_track_s;

/**< The Ul timimg tracking summary shows information regarding TA entry point,TA command type, 
  *  TA value , UL & DL time offsets and last Tx start time tracked
  * Min Size = 2 bytes 
  * Max Size = 82 bytes 
  * Display Format:
  *|--------------------------------------------------------------------------------------------|
  *|                          UL time tracking Summary                                          |
  *|--------------------------------------------------------------------------------------------|
  *|       taInfo                       |                              | lastTxStartTimeTracked |
  *|--------------------------------------------------------------------------------------------|
  *| entry type |Cmd type  |   ta       | ulTimeOffset |  dlTimeOffset |  SFN      |  Sub Fn    |
  *|--------------------------------------------------------------------------------------------|
  *|    0       | 2        |     0      |   314572725  |             0 |    0      |    0       |
  *|--------------------------------------------------------------------------------------------|
  */
typedef struct
{
  /* 0..31 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  __notused__  : 16;
  
  /* 32..2255 */
  nb1fw_log_ul_time_track_s buf[NB1FW_LOG_UL_TIME_TRACK_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_ul_time_track_ind_s;

/*! @brief tx encoder input logging structure */
typedef struct{
  uint32 tbSize;     /*!< UL encoder tb size */
  uint32 encIput[100]; /* max 100*4*8 = 3200bits */
} nb1fw_log_ul_enc_input_s;

typedef struct
{
  /* 0..31 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  __notused__  : 16;
  
  /* 32..3263 */
  nb1fw_log_ul_enc_input_s buf[1]; /**< Logging buffer */
} nb1fw_log_ul_enc_input_ind_s;

/*! @brief Tx IQ logging structure */
typedef struct{
  uint32 frame ;       /*!< Bits [31:17]  --- frame Count */
  uint32 subframe ;    /*!< Bits [0:16] --- subframe number for logged SS */
  uint32 slot;
  uint32 earfcn;       /*!< UL EARFCN */
  
  /* SS buffer 225..15584 */
  uint32 ss[960];  /*!< Bits [31:0] --- Signed 16bit I, Signed 16bit Q */
} nb1fw_log_ul_txfe_input_s;

typedef struct
{
  /* 0..31 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  __notused__  : 16;
  
  /* 32..30879 */
  nb1fw_log_ul_txfe_input_s buf[1]; /**< Logging buffer */
} nb1fw_log_ul_txfe_input_ind_s;



#endif /**< NB1FW_LOG_UL_H */

