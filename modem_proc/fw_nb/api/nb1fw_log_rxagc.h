 /*!
  @file
  nb1fw_log_rxagc.h

  @brief
  NB1 FW RxAGC logging interface

  @detail
  NB1 FW RxAGC logging interface

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


when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
9/29/16    bow      init version
===========================================================================*/

#ifndef NB1FW_LOG_RXAGC_H
#define NB1FW_LOG_RXAGC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/*! @brief Number of packets buffered before submit for Rx freq update */
#define NB1FW_LOG_RX_FREQ_UPDATE_NUM_BUFS       10

/*! @brief Number of Samples to captured in every B206 log packet */
#define NB1FW_LOG_SS_NUM_SAMPLES_PER_MS         480

/*! @brief Number of packets buffered for WBEE */
#define NB1FW_LOG_RX_WBEE_ENTRY_NUM 5
/*===========================================================================

      Typedefs

===========================================================================*/
typedef struct{
  uint32 subFrNum         : 4;        /*!< Bits [3:0]  --- Sub frame with in radio frame, range [0..9]  */ 
  uint32 sFrNum           : 10;       /*!< Bits [13:4] --- system frame number in radio frame unit, range [0..1023] */
  uint32                  : 18;

  int32 dc_i;
  int32 dc_q;
  uint32 wbdcPwrScaleSat;
  uint32 bbeeExcNoise;
  uint32 nbeeInst;
}nb1fw_log_rxagc_bbee_entry_s;

/*==========================================================================*/
/* NB1 FW RX Front End AGC Measurements (Log code ID: 0xB201)
 *---------------------------------------------------------------------------*/
 
/**< The RX Front End AGC log packet shows information regarding the state, 
  * parameters and schedule time of the downlink scheduler thread.
  * Scheduler information is gathered for every Subframe 
  * Min Size = 56 bytes
  * Max Size = 56 bytes 
  * Display Format : 
  *  |---------------------------------------------------------------------------------------------------------------------------------------------------------------| 
  *  |  Time  | LNA  | gain  | Number | EST2   | EST2   | Final  | Accum|     BBEE     |     HEE     |     TTEE     |     DVGA     |     RSSI       | Recent |DVGA   | 
  *  |        | Gain | offset| Accum  | CURR_I | CURR_Q | AccEgy | Egy  |     Q24      |     Q24     |     Q24      |     Q24      |-------|--------| Valid  |Update | 
  *  |        | State|       |        |        |        |        |      |              |             |              |              | Total | Inband | Idx    |Idx    | 
  *  |---------------------------------------------------------------------------------------------------------------------------------------------------------------| 
  *  | 22: 1  |   0  |     0 |     5  |     0  |     0  |     0  |   0  | -2147483648  |  -587691452 |  -622977601  |    12904032  |  -288 |  -288  |      8 |     9 |
  *  |---------------------------------------------------------------------------------------------------------------------------------------------------------------| 
  */ 
typedef union{
  struct{
    /* 0..32 */
    uint32 version          : 8;        /*!< Bits [7:0]  --- version of the log packet */
	uint32 numNonZero       : 8;        /*!< Bits [15:8]  --- Non zero BBEE number */
    uint32 numAccumValid    : 8;        /*!< Bits [23:16] --- Number of valid accumulations for BBEE (uint8) */
    uint32 entryCnt         : 8;
	
    /* 32..64 */
    uint32 lnaGainState     : 8;        /*!< Bits [7:0]  --- LNA gain state (uint8) */
    uint32 gainOffset       : 16;       /*!< Bits [23:8] --- Gain offset for current gain state (int16) */
    uint32 numAccum         : 8;        /*!< Bits [31:24] --- Number of accumulations for BBEE (uint8) */

    /* 64..128 */
    uint32 est2CurrI;                   /*!< Bits [31:0] --- HW reading for WBDC_EST2_CURR_I */

    uint32 est2CurrQ;                   /*!< Bits [31:0] --- HW reading for WBDC_EST2_CURR_Q */

    /* 128..192 */
    uint32 finalAccEgy;                 /*!< Bits [31:0] --- HW reading for FINAL_ACC */

    uint32 accumEgy;                    /*!< Bits [31:0] --- accumulated energy over all accumulations */

    /* 192..256 */
    int32 bbeeFinaldBQ24;               /*!< Bits [31:0] --- Final BBEE: WBPWR + (NBPWR_SCALE-WBPWR_SCALE) in Q24 */

    int32 heeAvgdBFSQ24;                /*!< Bits [31:0] --- HEE in dBFS Q24 */

    /* 256..320 */
    int32 totalRssi;                    /*!< Bits [31:0] --- total RSSI in dB10 (int32) */

    int32 tteedBFSQ24;                  /*!< Bits [31:0] --- TTEE in dBFS Q24 */

    /* 320..384 */
    int32 dvgadBQ24;                    /*!< Bits [31:0] --- DVGA in dB Q24 */

    int32 inbandRssi;                   /*!< Bits [31:0] --- inband RSSI in dB10 (int32) */
	
	uint32 earfcn;                      /*!< Bits [31:0] --- EARFCN */
	
	uint32 nbeedBFSQ24;                 /*!< Bits [31:0] --- Average NBEE in dBFS */

    /* 384..448 */
    uint32 subFrNum         : 4;        /*!< Bits [3:0]  --- Sub frame with in radio frame, range [0..9]  */ 
    uint32 sFrNum           : 10;       /*!< Bits [13:4] --- system frame number in radio frame unit, range [0..1023] */ 
    uint32                  : 2;        /*!< Bits [15:14] --- reserved */
    uint32 recentValidIdx   : 8;        /*!< Bits [23:16] --- Energy estimate recent valid index */
    uint32 dvgaUpdatedIdx   : 8;        /*!< Bits [31:24] --- DVGA last update index */
	
    nb1fw_log_rxagc_bbee_entry_s        bbeeEntry[NB1FW_LOG_RX_WBEE_ENTRY_NUM];
  };

  struct {
    uint32 word0;           /*!< Entire log word0 */
    uint32 word1;           /*!< Entire log word1 */
    uint32 word2;           /*!< Entire log word2 */
    uint32 word3;           /*!< Entire log word3 */
    uint32 word4;           /*!< Entire log word4 */
    uint32 word5;           /*!< Entire log word5 */
    uint32 word6;           /*!< Entire log word6 */
    uint32 word7;           /*!< Entire log word7 */
    uint32 word8;           /*!< Entire log word8 */
    uint32 word9;           /*!< Entire log word9 */
    uint32 word10;          /*!< Entire log word10 */
    uint32 word11;          /*!< Entire log word11 */
    uint32 word12;          /*!< Entire log word12 */
    uint32 word13;          /*!< Entire log word13 */
    uint32 word14;          /*!< Entire log word14 */
  };
} nb1fw_log_rxagc_bbee_ind_s;

/*! @brief Rx frequency update logging */
typedef union{
  struct {
    int32 csrCurrFreqOfs;               /*!< Bits [31:0]  --- CSR curr_freq_offset */

    uint32 wbVsrc_t1byt2;               /*!< Bits [31:0]  --- WB VSRC curr_t1byt2 */

    uint32 wbVsrc_t2byt1m1;             /*!< Bits [31:0]  --- WB VSRC curr_t2byt1m1 */

    uint32 stmrFcw;                     /*!< Bits [31:0]  --- STMR FCW */

    uint32 actionTime;                  /*!< Bits [31:0]  --- Action time STMR when update/read value */
  };

  struct {
    uint32 word0;           /*!< Entire log word0 */
    uint32 word1;           /*!< Entire log word1 */
    uint32 word2;           /*!< Entire log word2 */
    uint32 word3;           /*!< Entire log word3 */
    uint32 word4;           /*!< Entire log word4 */
  };
} nb1fw_log_rx_freq_update_s;

/*! @brief Rx frequency update logging structure */
typedef struct
{
  struct {
    uint32 version          : 8;        /*!< Bits [0:7]  --- version of the log packet */
    uint32 bufCnt           : 8;        /*!< Bits [15:8] --- Number of valid items logged */
    uint32                  : 16;
  };

  /* Rx freq update logging buffer */                                    
  nb1fw_log_rx_freq_update_s buf[NB1FW_LOG_RX_FREQ_UPDATE_NUM_BUFS];  
} nb1fw_log_rx_freq_update_ind_s;

/*! @brief Rx timing logging structure */
typedef union{
  struct{
    uint32 version                  : 8;    /*!< Bits [7:0]   --- version of the log packet */
    uint32 mode                     : 8;    /*!< Bits [15:8]  --- chain enable: 0, chain disable: 1 */
    uint32 lastLnaEvalChangedGain   : 8;    /*!< Bits [23:16] --- gain state changed in last eval */ 
    uint32                          : 8;  
	
    uint32 rfSigPathTime;               /*!< Bits [31:0] --- RF signal path action time */

    uint32 rfFullPathTime;              /*!< Bits [31:0] --- RF full path action time */

    uint32 ssBufAddr;                   /*!< Bits [31:0] --- Rx sample server buffer address */
  };

  struct {
    uint32 word0;           /*!< Entire log word0 */
    uint32 word1;           /*!< Entire log word1 */
    uint32 word2;           /*!< Entire log word2 */
    uint32 word3;           /*!< Entire log word3 */
  };
} nb1fw_log_rx_time_ind_s;

/*! @brief Rx timing logging structure */
typedef struct{
  /* 0..32 */
  uint32 version    : 8;    /*!< Bits [7:0]   --- version of the log packet */
  uint32 frame      : 16;   /*!< Bits [23:8]  --- frame Count */
  uint32 subframe   : 8;    /*!< Bits [31:24] --- subframe number for logged SS */

  /* 32..64 */
  uint32 ssIdx;              /*!< Bits [31:0] --- SS Idx where ss buffer starts from */

  /* 64..128 */
  int32 est2CurrI;           /*!< Bits [31:0] --- HW reading for WBDC_EST2_CURR_I */
  int32 est2CurrQ;           /*!< Bits [31:0] --- HW reading for WBDC_EST2_CURR_Q */

  /* 129..160 */
  uint32 pcell_id  : 16;     /*!< Bits [15:0]  --- PCell ID */
  uint32 earfcn    : 16;     /*!< Bits [31:16] --- EARFCN */

  /* 161..192 */
  int32  lifOffset     ;     /*!< Bits [31:0] --- LifOffset in Hz*/
  
  /* 193..224 */
  uint32 rxagcMode     ;     /*!< Bits [31:0] --- rxagc mode, 1: SRCH; 2: CELL */
  
  uint32 dvga   ;            /*!< Bits [31:0] --- dvga linear value for srch mode */
  /* SS buffer 225..15584 */
  uint32 ss[NB1FW_LOG_SS_NUM_SAMPLES_PER_MS];  /*!< Bits [31:0] --- Signed 16bit I, Signed 16bit Q */

} nb1fw_log_ss_ind_s;

/*! @brief Rx chain enable/disable structure */
typedef struct
{
  uint32  version : 8;       
  uint32  logType : 8; /**< Indication if the log data is for chain enable or disable
                                         * 0 = chain enable, 1 = chain disable*/
  uint32  earfcn  : 16;                                       
  uint32  action_time;    /* action time from caller in XO. 0xFFFFFFFF is ASAP mode */                                      
  union
  {
    struct
    {
      /* Word 0*/
      uint32  isRetune    : 1;   /**< Retune type. 0: not retune call; 1: retune call */
      uint32  carrierType : 1;   /**< Carrier type. 0: Anchor carrier; 1: Non-anchor carrier*/
      uint32  startMode   : 2;   /**< start mode
                                   0: NB1FW_RX_START_COLD_MODE,           
                                   1: NB1FW_RX_START_ONE_SHOT_MODE,       
                                   2: NB1FW_RX_START_CARRIER_SWITCH_MODE, 
                                   3: NB1FW_RX_START_DIRECT_MODE,         
                                      nb1fw_rx_start_mode_e; 
                                 */
      uint32  warmup      : 1;   /**< Warmup subframe. 0: start at the next subframe. 1: start at the next next subframe */
      uint32  rxagcMode   : 2;   /**< rxagc mode
                                   0: NB1FW_RXAGC_MODE_DISABLE,
                                   1: NB1FW_RXAGC_MODE_SRCH,
                                   2: NB1FW_RXAGC_MODE_CELL,
                                 */
      uint32  drxResume   : 1;   /**< DRX is resumed from DRX. 0: not from DRX; 1: from DRX */
      uint32  gainState   : 4;   /**< Gain state, valid value: 0 - 9 */
      uint32  reserved    : 20;
    } ;

    struct
    {
      int32 nrsrpdBQ24;   /**< rsrp used for initial rssi calculation */
    } ;
  };
  uint32 subFrCnt     : 11;  /*!< Bits within a radio frame, range [0..VSTMR_NB1_SUBFRAME_RANGE-1] */ 
  uint32 subFrNum     : 4;   /*!< Sub frame with in radio frame, range [0..VSTMR_NB1_SUBFR_CNT_RANGE-1]  */ 
  uint32 sFrNum       : 10;  /*!< System frame number in radio frame unit, range [0..VSTMR_NB1_SFR_CNT_RANGE-1] */
} nb1fw_log_rx_endis_ind_s;
#endif /* NB1FW_LOG_RXAGC_H */

