 /*!
  @file
  nb1fw_logging_dl.h

  @brief
  NB1 FW DL logging interface

  @detail
  NB1 FW DL logging interface

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/nb1fw_log_dl.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
9/13/16    AT      Created the header file for DL Logging
===========================================================================*/

#ifndef NB1FW_LOG_DL_H
#define NB1FW_LOG_DL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/**< number of packets buffered before submit for DL sched log packet */
#define NB1FW_LOG_DL_SCHED_NUM_BUFS 20

/**< number of packets buffered before submit for DL CHEST log packet */
#define NB1FW_LOG_DP_DL_CHEST_NUM_BUFS 20

/**< number of packets buffered before submit for DL RxFFT log packet */
#define NB1FW_LOG_DP_DL_RXFFT_NUM_BUFS 20

/**< number of packets buffered before submit for DL TLs log packet */
#define NB1FW_LOG_DP_DL_TL_NUM_BUFS 20

/**< number of packets buffered before submit for DL Dopplers log packet */
#define NB1FW_LOG_DP_DL_DOPPLER_NUM_BUFS 20

/**< number of packets buffered before submit for DL Demod log packet */
#define NB1FW_LOG_DP_DL_DEMOD_NUM_BUFS 20

/**< number of packets buffered before submit for DL measurement log packet */
#define NB1FW_LOG_DP_DL_MEAS_NUM_BUFS 5

/**< number of packets buffered before submit for DL measurement log packet */
#define NB1FW_LOG_DP_DL_PBCH_MEAS_NUM_BUFS 5

/**< max number of packets to be buffered before submit for SSFEE log packet */
#define NB1FW_LOG_DP_DL_SSFEE_NUM_BUFS 60

/**< max number of packets to be buffered before submit for channel state log packet */
#define NB1FW_LOG_DL_CHAN_STATE_NUM_BUFS 20


/**< max number of packets to be buffered before submit for multi grant log packet */
#define NB1FW_LOG_DL_MULTI_GRANT_NUM_BUFS 5

/**< max number of packets to be buffered before submit for carrier switch log packet */
#define NB1FW_LOG_DL_CARRIER_SWITCH_NUM_BUFS 1

/**< max number of packets to be buffered before submit for raftt log packet */
#define NB1FW_LOG_DL_RAFTT_NUM_BUFS 1

/**< max number of packets to be buffered before submit for carrier switch log packet */
#define NB1FW_LOG_DL_NRS_MEASUREMENT_NUM_BUFS 1

/*===========================================================================

      Typedefs

===========================================================================*/

/*==========================================================================*/
/* NB1 FW Downlink Scheduler Processing Summary (Log code ID: 0xB210)
 *---------------------------------------------------------------------------*/

/** 
 *  DL scheduler logging structure
 */
typedef struct
{
  /** DL schedule time */
  /* 16..48 */
  union
  {
    struct
    {
      uint16 slotNum  : 1;      /**< slot index  0 or 1 */  
      uint16 subFrNum : 4;      /**< subframe index [1..4] */
      uint16 sFrNum   : 10;     /**< system frame number [6..15] */
      uint16          : 1;      /**< unused */      
    };
    uint16 info;
  } curTime;

  /** DL RxFft time */
  /* 16..48 */
  union
  {
    struct
    {
      uint16 slotNum  : 1;      /**< slot index  0 or 1 */  
      uint16 subFrNum : 4;      /**< subframe index [1..4] */
      uint16 sFrNum   : 10;     /**< system frame number [6..15] */
      uint16 runRxFft : 1;     /**< rxFFT is run this slot, 0 = No, 1 = Yes */      
    };
    uint16 info;
  } RxFft;

  /** DL DemFront time */
  union
  {
    struct
    {
      uint16 slotNum   : 1;      /**< slot index  0 or 1 */  
      uint16 subFrNum  : 4;      /**< subframe index [1..4] */
      uint16 sFrNum    : 10;     /**< system frame number [6..15] */
      uint16 runDfront : 1;      /**< CE/TLs is run this slot, 0 = No, 1 = Yes */      
    };
    uint16 info;
  } DFront;

  /** DL Demod time */
  union
  {
    struct
    {
      uint16 slotNum  : 1;      /**< slot index  0 or 1 */  
      uint16 subFrNum : 4;      /**< subframe index [1..4] */
      uint16 sFrNum   : 10;     /**< system frame number [6..15] */
      uint16 runDemod : 1;      /**< Demod is run this slot, 0 = No, 1 = Yes */      
    };
    uint16 info;
  } Demod;

  /** 
   *   DL sched state
   */
  /* 0..16 */
  struct
  {      
    uint16 activeChan      : 3;   /**< Active Channel type, 0 = PBCH, 1 = SIB, 2 = PDCCH, 3 = PDSCH */       
    uint16 demodChan       : 3;   /**< Demod Channel type, 0 = PBCH, 1 = SIB, 2 = PDCCH, 3 = PDSCH */       
    uint16 suspendedChan   : 3;   /**< Demod Suspended Channel type, 0 = PBCH, 1 = SIB, 2 = PDCCH, 3 = PDSCH */       
    uint16                 : 7;   /**< Unused */
  } state;

} nb1fw_log_dl_sched_s;

/**< The downlink scheduler summary shows information regarding the state, parameters and schedule time 
  * of the downlink scheduler thread. Scheduler information is gathered for every Subframe 
  * Min Size = 2 bytes 
  * Max Size = 122 bytes 
  * Display Format:
  * |---------------------------------------------------------------|
  * | DL Scheduler Summary                                          |
  * |---------------------------------------------------------------|
  * | Schedule Run  |  RxFFT   | DFront   | Demod   | Demod Channel |
  * |               |          |          |         |               |
  * |---------------------------------------------------------------|
  * |    166:6:0    | 166:5:0  | 166:5:0  |   -     | PDCCH         |
  * |---------------------------------------------------------------|
  * |    166:6:1    | 166:5:1  |   -      |   -     | PDCCH         |
  * |---------------------------------------------------------------|
  * |    166:7:0    | 166:6:0  |   -      |   -     |               |
  * |---------------------------------------------------------------|
  */
typedef struct
{
  /* 0..16 */
  uint8  version; /**< version number of log packet */
  uint8  bufCnt;  /**< Number of valid items logged */
    
  /* 16..976 */
  nb1fw_log_dl_sched_s buf[NB1FW_LOG_DL_SCHED_NUM_BUFS]; /**< Logging buffer */

} nb1fw_log_dl_sched_ind_s;


/*==========================================================================*/
/* NB1 FW Downlink Channel Estimation Summary (Log code ID: 0xB211)
 *---------------------------------------------------------------------------*/
 
typedef struct
{
  /* 0..32 */  
  uint16 slotNum     : 1;         /**< slot index  0 or 1 */  
  uint16 subframeNum : 4;         /**< subframe index [1..4] */
  uint16 frameNum    : 10;        /**< system frame number [6..15] */
  uint16 crsCombEn   : 1;         /**< CRS combining Enabled */
  uint16 numSlotsSkipped;         /**< max NB1FW_EIGHT_FRAMELENGTH_IN_SLOTS = 160 */

  /* 32..96 */  
  int32 SNREstimate;              /**< SNR Estimate in 32S22
                                    * Displayed value is calculated as follows
                                    * 10 x log10(<value>/2^22 + epsilon), epsilon is the smallest float value to ensure we don’t run into log10(0)
									*/
									
  int32 NoisePowerEstimate;       /**< Noise/Power Estimate in 32S31
                                    * Displayed value = <value> / 2^31
									*/
  
  /* 96..352 */  
  int32 tdRawTaps[NB1_FW_MAX_TX_PORTS][NB1FW_COH_IIR_FILT_IFFT_SIZE];
                                  /**< TD raw taps after FFT. both real and imaginary in 16S15
								   * Each entry is Displayed as 2 values (lower 16 bits/ 2^15, higher 16 bits/ 2^15)
								   */

  /* 352..432 */  
  int16  doppler;                 /**< doppler value */
  int16  coherentIIRTdTapAlpha;   /**< IIR tD in 16S14 
                                    * Displayed value = <value> / 2^14
									*/
  int16  nonCoherentIIRSNEAlpha;  /**< IIR SNE in 16S14 
                                    * Displayed value = <value> / 2^14
									*/
  int16  nonCoherentIIRR1Alpha;   /**< IIR R1 in 16S14
                                    * Displayed value = <value> / 2^14
									*/
  int16  GDAlpha;                /**< GD in 16S12
                                    * Displayed value = <value> / 2^12
									*/

  /* 432..496 */  
  int16  mmseCleaningFactor[NB1FW_NBRS_TONES_PER_SYMBOL]; /**< 16S15
                                                            * Each displayed value = <value> / 2^15
									                        */

} nb1fw_log_dp_dl_chest_s;

/**< The Downlink channel estimation summary shows information regarding the 
  * channel estimation done during downlink timeline processing. Data items are collected for every subframe 
  * Min Size = 12 bytes
  * Max Size = 1252 bytes
  * Display Format:
  *
  * EARFCN :
  * PCID   :
  * Op Mode:
  * Combining Weight:
  * |--------------------------------------------------------------------------------------------------------------------------------------------------|-------|-------------------------------|---------------------------|
  * |  Proc Time  | CRS | GAP   | SNR       | Noise    |                                      TD RAW TAPS                                              |Doppler|        Alpha          |  GD   |      Cleaning Factor      |
  * |             | Comb|       | Estimate  | Estimate |-----------------------------------------------------------------------------------------------|       |-----------------------|       |---------------------------|
  * |             | En  |       | (db)      |          |                 Tx0                           |              Tx1                              |       |  Coh  |  SNE  |  R1   |       |   0  |   1  |   2  |   3  |
  * |-------------|-----|-------|-----------|----------|-----------------------------------------------|-----------------------------------------------|-------------------------------------------------------------------|
  * |                                            |    0      |    1      |    2      |     3     |    0      |     1     |     2     |     3     |                                                                   |
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |    12: 0:0  | EN  |  864  |    -50    |  0.0006  |0.007,1.956|0.007,0.012|0.001,1.992|0.001,1.980|0.011,1.999|0.013,1.987|1.990,1.988|1.991,1.991|     0 |1.00000|1.00000|0.00000|1.75000|1.0000|1.0000|1.0000|1.0000|
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |    12: 0:1  | DIS |    1  |     11    |  0.0000  |0.017,1.962|1.999,0.001|0.009,1.992|1.999,0.028|0.003,1.990|0.004,0.005|1.996,0.015|0.004,1.999|     0 |0.50000|0.99762|1.00000|1.75000|1.0000|1.0000|1.0000|1.0000|
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |    12: 4:0  | EN  |    7  |     21    |  0.0000  |0.016,1.970|1.986,0.018|0.002,0.007|0.002,0.003|1.982,1.981|1.976,0.000|0.002,1.999|1.995,0.025|     0 |0.50201|0.49884|1.00000|1.75000|0.9980|0.8265|0.9649|0.9933|
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |    12: 4:1  | DIS |    1  |      8    |  0.0001  |1.990,1.957|0.000,0.023|1.986,1.996|1.998,1.987|2.000,1.974|1.969,0.014|1.982,0.023|0.004,0.004|     0 |0.48279|0.33295|0.99762|1.75000|0.9857|0.9590|0.8317|0.6910|
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  *
  */
typedef struct
{
  /* 0..32 */  
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  op_mode      : 8;  /**< operating mode - enum type nb1fw_cell_op_mode_e, range 0-3
                               * Display as String - 0 = INBAND_SAME_PCI, 1 = INBAND, 2 = GUARD_BAND, 3 = STANDALONE */
  uint32  __notused__  : 8;
  
  /* 32..96 */
  uint32 dl_earfcn;     /**< EARFCN as received in cell cfg */
  uint16 pcid;          /**< serving cell Id as received in cell cfg */
  int16  nbrsCrsRatio;  /**< NBRS/CRS power ratio - in linear 16S12
                          * Display as 10 x log10(<value>/2^22 + epsilon) if op_mode == 0
                          * Display -N.A.- if op_mod > 0 */

  /* 96..592 */
  nb1fw_log_dp_dl_chest_s buf[NB1FW_LOG_DP_DL_CHEST_NUM_BUFS];  /**< Logging buffer */
} nb1fw_log_dp_dl_chest_ind_s;


/*==========================================================================*/
/* NB1 FW Downlink RxFFT Summary (Log code ID: 0xB212)
 *---------------------------------------------------------------------------*/
 
typedef struct
{
  /* 0..32 */  
  uint16 slotNum     : 1;      /**< slot index  0 or 1 */  
  uint16 subframeNum : 4;      /**< subframe index [1..4] */
  uint16 frameNum    : 10;     /**< system frame number [6..15] */
  uint16             : 1;      /**< not used */
  uint16 digGain;              /**< Symbol0 DVGA gain in 16S7
                                 * Displayed value = <value> / 2^7
                                 */

  /* 32..128 */
  int32  TTEE;
  int32  xRSEE;
  int32  HEE;
  int32  HEEAvgdBFS;

  /* 128..192 */  
  vect32 symPhase;    /**< Symbol0 Phase
                        * Displayed as 2 values (lower 16 bits, higher 16 bits)
                        */
						
  uint16 ssRefIdx;    /**< Sample server address(1.92Mhz) at radio frame boundary */
  uint16 sampBufIdx;  /**< Sample server address(480Khz) for Symbol0 */
                     
  /* 192..224 */
  uint32 interpPhase : 8;  /**< Symbol0 interpolation phase */
  uint32             : 24;

} nb1fw_log_dp_dl_rxfft_s;

/**< The downlink RxFFT summary shows information regarding the RxFFT 
  * operations done during the downlink channel processing 
  * Min Size = 4 bytes 
  * Max Size = 564 bytes 
  * Display Format:
  * |-------------------------------------------------------------------------------------------------------| 
  * |  Proc Time   |  TTEE    |   xRSEE  |    HEE   |  SymPhase   | DVGA |SSREFIDx| SAMPBUFIDX| INTERP Phase| 
  * |              |          |          |          |             |      |--------|-----------|-------------| 
  * |              |          |          |          |             |      | 1.92Mhz|   480 Khz | 0, 1, 2 or 3| 
  * |-------------------------------------------------------------------------------------------------------| 
  * |  12:0:0      |       0  |       0  |       0  | 49643, 61555|     1|      0 |       11  |        3    |
  * |-------------------------------------------------------------------------------------------------------| 
  * |  12:0:1      |     342  |     324  |     342  | 61555, 15893|     1|      0 |      251  |        3    |
  * |-------------------------------------------------------------------------------------------------------| 
  * |  12:4:0      |       0  |       0  |       0  | 49643, 61555|     1|      0 |     1931  |        3    |
  * |-------------------------------------------------------------------------------------------------------| 
  * |  12:4:1      |     531  |     447  |     531  | 61555, 15893|     1|      0 |     2171  |        3    |
  * |-------------------------------------------------------------------------------------------------------| 
  */
typedef struct
{  
  /* 0..32 */  
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32               : 16;
  
  /* 32..32 */  
  nb1fw_log_dp_dl_rxfft_s buf[NB1FW_LOG_DP_DL_RXFFT_NUM_BUFS];  /**< Logging buffer */

} nb1fw_log_dp_dl_rxfft_ind_s;


/*==========================================================================*/
/* NB1 FW Downlink Tracking Loops Summary (Log code ID: 0xB213)
 *---------------------------------------------------------------------------*/
 
typedef struct
{
  /* 1472..1791 */
  vect64 iirFilteredHcorr[NB1FW_NUM_H_TTL_DISC];  /**< IIR Filtered Correction values
                                              * Each entry is Displayed as 2 values (lower 16 bits/ 2^15, higher 16 bits/ 2^15)
                                              */

  /* 1792..1855 */
  int32  discrTtlErr;      /**< TTL Error
                             * Displayed value = <value> / 2^31
                             */
  uint8  goodStateCnt;     /**< max TTL_MAX_GOOD_STATE_CNT = 11 */
  int8   ttlCorrn;         /**< TTL COrrection -1 , 0, 1 */
  uint16 reserved;         /**< not used */
  
}nb1fw_dp_dl_ttllog_s;

typedef struct
{
  /* 1856..1983 */
  uint32  ftlSnr;                         /**< FTL SNR Value in Q22
                                            * Displayed value is calculated as follows
                                            * 10 x log10(<value>/2^22 + epsilon), epsilon is the smallest float value to ensure we don’t run into log10(0)
                                            */
  int32   rxfeCorrnFreqQ6;                /**< the freq error applied by the rotator for the current suvframe
                                            * Displayed value = <value> / 2^6
                                            */
  int32   rxfeCorrnPhaseQ32;              /**< this is the running Phase of the RxFE */

  /* 1984..2047 */
  int32   excessPhaseQ32;                 /**< how much we have adjusted the correlation Array for the applied rxFE Rotations */

  int32   instFreqErrHzQ6;                /**< Instantaneous FTL Error
                                            * Displayed value = <value> / 2^6
                                            */
  /* 2048.. 2111*/
  int32   ftlCntrlLoopAccQ6;              /**< This is the latest/last output of the FTL loop
                                            * Displayed value = <value> / 2^6
                                            */
  uint32  freqErrUpdt      : 16;           /**< whether total freq error was updated this slot */
  uint32  goodStateCnt     : 8;           /**< max TTL_MAX_GOOD_STATE_CNT = 11 */
  uint32  iirFilterLength  : 8;           /**< updated IIR Filter Length logged as log2(length) - 5
                                            * Displayed value = (32 << iirFilterLength)
                                            */
  uint32  unused;                         /**< Pad bits only >**/
}nb1fw_dp_dl_ftllog_s;

typedef struct
{
  /* 1408..1471 */
  uint16 slotNum     : 1;      /**< slot index  0 or 1 */  
  uint16 subframeNum : 4;      /**< subframe index [1..4] */
  uint16 frameNum    : 10;     /**< system frame number [6..15] */
  uint16             : 1;      /**< not used */
  uint16 iirAlphaNc;           /**< iirAlpha
                                 * Displayed value = <value> / 2^15
                                 */
  int32  ttlAccum;             /**< Accumulated TTL
                                 * Displayed value = <value> / 2^31
                                 */

  /* 1472..1855 */
  nb1fw_dp_dl_ttllog_s ttl;    /**< TTL Data */

  /* 1856..2111 */
  nb1fw_dp_dl_ftllog_s ftl;    /**< FTL Data */

} nb1fw_log_dp_dl_tl_s;

typedef struct
{
  /* 64..127 */
  uint64 slotNum     : 1;      /**< slot index  0 or 1 */
  uint64 subframeNum : 4;      /**< subframe index [1..4] */
  uint64 frameNum    : 10;     /**< system frame number [6..15] */
  uint64 valid       : 1;      /**< whether this is valid or not */
  uint64             : 48;     /**< not used */

  /* 128..1151 */
  vect64  iirFilteredCorr[NB1FW_FTL_MAX_PILOT_DEPTH];  /**< IIR Filtered Correlation H(0)*H"(k)
                                                       * Each entry is Displayed as 2 values (lower 16 bits/ 2^15, higher 16 bits/ 2^15)
                                                       */
  /* 1152..1407 */
  int16 iirAlpha[NB1FW_FTL_MAX_PILOT_DEPTH];    /**< iirAlpha for each correlation
                                                * Each entry is Displayed value = <value> / 2^15
                                                */
}nb1fw_log_dp_dl_ftl_snapshot_s;

/**< 
  * Min Size = 176 bytes
  * Max Size = 3216 bytes
  * Display Format:
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |  Proc Time   |            IIR Filterered H Corrs of TD taps              | goodState | ttlCorrn | iirAlpha | ttlAccum | discrTTL | ftlSNR | psdClean | rxfe Corrn | rxfe Corrn | excess | Inst Freq | total   | FErr | good | filter |         Doppler         |
  * |              |     0     |     1     |     2     |     3     |     4     | Count                                        Err      |        |          | Freq       | Phase      | Phase  | Err       | freq err| Updt | state| Length | Sched | Running | Index |
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |    12: 0: 0  |0.000,0.000|0.000,0.000|0.000,0.000|0.000,0.000|0.000,0.000|        0  |        0 |  0.000   |  0.000   |  0.000   | 0.000  | 0.000    |  0.000     | 0x00000    |0x00000 |  0.000    | 0.000   |  1   |  1   | 0000   |   1   |     1   | 0000  |
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |     0: 0: 0  |0.012,0.499|1.374,2.000|0.106,0.001|0.717,0.003|1.416,0.001|       30  |      179 |  0.000   |  0.000   |  -0.000  | 0.000  | 0.000    |  0.000     | 0x00000    |0x00000 |  0.000    | 0.000   |  0   |  0   | 0000   |   0   |     0   | 0000  |
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
  * 
  * 
  * |---------------------------------------
  * |  Proc Time   | FTL Corrs | Corr Alpha|
  * |---------------------------v cx------------
  * |    12: 0: 0  |0.000,0.000|   0.000   |
  * |              |-----------------------|
  * |              |0.012,0.499|   0.000   |
  * |---------------------------------------
  */
typedef struct
{  
  /* 0..63 */
  uint64  version  : 8;  /**< version number of log packet */
  uint64  bufCnt   : 8;   /**< Number of valid items logged*/
  uint64           : 48;
  
  /* 64..1407 */
  nb1fw_log_dp_dl_ftl_snapshot_s ftlSnapShot;

  /* 1408..15487 */
  nb1fw_log_dp_dl_tl_s buf[NB1FW_LOG_DP_DL_TL_NUM_BUFS];  /**< Logging buffer */
} nb1fw_log_dp_dl_tl_ind_s;


typedef struct
{
  /* 0..15 */
  uint32 slotNum           : 1;       /**< slot index  0 or 1 */
  uint32 subframeNum       : 4;       /**< subframe index [1..4] */
  uint32 frameNum          : 10;      /**< system frame number [6..15] */

  uint32 dopRunning        : 1;       /**< doppler currently running
                                        * Displayed string: 0 = NO, 1 = YES */
  /* 16..31 */
  uint32 dopplerIdx        : 16;      /**< doppler bin Idx
                                        * Displayed Value = 62.5 * dopplerIdx
                                        */
  /* 32..63 */
  uint32  psdCleanThreshold;          /**< computed cleaning Threshold for PSD
                                        * Displayed value = <value> / 2^15
                                      */
  /* 64..575*/
  int16 psd[2*NB1FW_FTL_MAX_PILOT_DEPTH]; /**/
} nb1fw_log_dp_dl_psd_s;


 /*==========================================================================*/
/* NB1 FW Downlink Doppler Estimation Info (Log code ID: 0xB214)
 *---------------------------------------------------------------------------*/
typedef struct
{
  /* 0..63 */
  uint64  version  : 8;  /**< version number of log packet */
  uint64  bufCnt   : 8;  /**< Number of valid items logged*/
  uint64           : 48; /**< used bits for debugging */
  /*64 ...11583 */
  nb1fw_log_dp_dl_psd_s buf[NB1FW_LOG_DP_DL_DOPPLER_NUM_BUFS];  /**< Logging buffer */
}nb1fw_log_dp_dl_doppler_ind_s;

/*==========================================================================*/
/* NB1 FW Downlink Interpollation and Demod Summary (Log code ID: 0xB214)
 *---------------------------------------------------------------------------*/
 
typedef struct
{
  /* 0 .. 32 */
  uint16 slotNum       : 1;      /**< slot index  0 or 1 */  
  uint16 subframeNum   : 4;      /**< subframe index [1..4] */
  uint16 frameNum      : 10;     /**< system frame number [6..15] */
  uint16 IREnableFlag  : 1;      /**< IREanbledFlag, 1 Enabled, 0 not enabled */
  int16 LLR1ValidSize;         /**< Valid size of LLR1 */

  /* 32 .. 64 */
  int16 LLR2ValidSize;         /**< Valid size of LLR2 */
  int16 LLR3ValidSize;         /**< Valid size of LLR3 */

  /* 64 .. 96 */
  int32 invNoisePowEst;        /**< Noise-Power Estimate
                                 * Displayed value = <value> / 2^7
                                 */

  /* 96 .. 128 */
  int16 chInterpCoeff[2];      /**<
                                 * Each Displayed value = <value> / 2^14
                                 */

} nb1fw_log_dp_dl_demod_s;

/**< The downlink interpolation and demod summary shows information regarding 
  * TD Interpolation coefficients, LLR sizes and Invert Noise Power Estimate 
  * Min Size = 4 bytes 
  * Max Size = 324 bytes 
  * Display Format:
  * |--------------------------------------------------------------| 
  * | Proc Time |   LLR Valid Sizes  | INVNoise | TDI INTERP COEFFS| 
  * |           |  0   |  1   |  2   | Estimate |  0     |    1    | 
  * |--------------------------------------------------------------| 
  * |  12: 0: 0 |    72|    72|     0|0x00005244|      1 |      0  | 
  * |--------------------------------------------------------------| 
  * |  12: 0: 1 |   200|   200|     0|0x00005244|      0 |      1  | 
  * |--------------------------------------------------------------| 
  */
typedef struct
{  
  /* 0 .. 32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32               : 16;
  
  /* 32 .. 2592 */
  nb1fw_log_dp_dl_demod_s buf[NB1FW_LOG_DP_DL_DEMOD_NUM_BUFS];  /**< Logging buffer */

} nb1fw_log_dp_dl_demod_ind_s;

/**< Measurement logging struct */
typedef struct
{
  union
  {
    struct
    {
      uint32 slotNum     : 1;  /**< slot index  0 or 1 */
      uint32 subFrNum    : 4;  /**< subframe index [1..4] */
      uint32 fullFrNum   : 14; /**< frame number */
      uint32             : 13; /**< Unused */
    };
    uint32 info;
  } timeInfo;

  /* NRSRP algo related values */

  int32 freqErrIntr[NB1_FW_MAX_TX_PORTS]; /**< Freq error intermediate value
                                           * Displayed value = <value> / 2^6
                                           */

  int32 NRSEE[NB1_FW_MAX_TX_PORTS];   /**< NRSEE intermediate value
                                        * Displayed value = <value> / 2^24
                                        */

  int32 NRSEELog;                    /**< Log scale NRSEE Estimate at end of meas period
                                        * Displayed value = <value> / 2^24
                                        */

  int32 freqErrAvg[NB1_FW_MAX_TX_PORTS]; /**< Freq error for completed measurement period
                                           * Displayed value = <value> / 2^6
                                           */
  int32 NRSRPFinalEst;  /**< NRSRP Final Estimate after adding offsets in dB10
                           * Displayed value = <value> / (2^7 * 10)
                           */
  int32 NRSRQFinalEst;  /**< NRSRQ Final Estimate at end of meas period in dB10
                          * Displayed value = <value> / (2^7 * 10)
                          */

  int32 RSSI;      /**< Inband RSSI value used for NRSRQ estimation in dB10
                      * Displayed value = <value> / (2^7 * 10)
                      */
  /* Control logic related info for NRSRP Meas  */
  int32 rfOffset;   /**< RF offset added to coherent accum period in dB scale
                       * Displayed value = <value> / (2^7 * 10)
                       */
  uint32 snrForMeasParam;  /** < SNR value used to decide meas param
                             * Display value = 10*log(<value> / (2^15))
                             */
  int16  cellId;            /**< Cell Id for which meas is logged */
  uint8  noOfCohAccumSlot;  /**< num of Coherent slots accum done */
  uint8  numOfCohBlk;       /**< Count for number of coherent periods done */
  union
  {
    struct
    {
      uint16 measEnabled      : 1; /**< Meas enabled flag, 0 = NO, 1 = YES*/
      uint16 validNRSRPVal    : 1; /**< NRSRP value valid flag, 0 = Invalid, 1 = Valid */
      uint16 measType         : 1; /**< Cell meas type, 0 = Serv cell, 1 = Neighbor cell */
      uint16 currMeasState    : 2; /**< Meas period state, 0 = Stopped, 1 = Ongoing, 2 = Meas done */
      uint16 gapHandling      : 2; /**< Gap handling, 0 = None, 1 = reset, 2 = resume */
      uint16 gapInfo          : 1; /**< Indicate gap in Coh block, 0 = Not present, 1 = Present */
      uint16 useCESnr         : 1; /**< if CE SNR used, 0 = No, 1 = Yes */
      uint16 measParamIdx     : 3; /**< Meas parameters index */
      uint16 NoOfTx           : 2; /**< No of Tx ports configured */
      uint16 validTx          : 1; /**< Num of Tx used is valid or assumption */
      uint16                  : 1; /**< not used */
    };
    uint8 info;
  } measCtrlInfo;

  int16  totalFerrPpmq10;   /**< Total freq error in PPM Q10 */


} nb1fw_log_dp_dl_meas_s;

  /**< NRSRP measurement summary shows the information regarding intermediate, final values
    * used in measurements along with its control information 
    * Min Size = 4 bytes 
    * Max Size = 244 bytes
    * Display Format:
  * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  * |    Proc Time    |     FreqErrIntr   |      NRSEE        |     NRSEELog     |    FreqErrAvg     | NRSRP Final | NRSRQ Final |    RSSI    | RF    | SNR     | Cell Id | Coh  | Coh Blk |  Meas   |NRSRP |Meas| Meas  | gap      | gapInfo  |CE SNR|SNR bin| Num Tx| valid Tx | totalFerr  |
  * | [SFN:SubFN:Slt] |-------------------|-------------------|                  |-------------------|             |             |            | Offset|         |         | Slots|         | Enabled |valid |Type| State | Handling |          |      | Idx   |       |          | (ppm q10)  |
  * |                 |   Tx0   |   Tx1   |   Tx0   |   Tx1   |                  |   Tx0   |   Tx1   |             |             |            |       |         |         |      |         |         |      |    |       |          |          |      |       |       |          |            |
  * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  * |    12: 0:0      |   4080  |  21666  | 197029  |  884    |   -775033022     |   4569  |  19746  |    -70571   |   -70251    |  -458      | 1254  |   20    |    10   |  68  |   1     |   1     |  1   |  0 | 1     |   resume | present  | 1    |  2    |  2    |   1      |  12345     |
  * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  * |    12: 0:0      |   4080  |  21666  | 197029  |  884    |   -775033022     |   4569  |  19746  |    -70571   |   -70251    |  -458      | 1254  |   20    |    10   |  12  |   4     |   0     |  0   |  1 | 1     |   reset  |Notpresent| 0    |  4    |  1    |   0      |  986763    |
  * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  */
typedef struct
{
  /* 0 .. 32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32               : 16; /**< not used */

  /* 32 .. 2912 */
  nb1fw_log_dp_dl_meas_s buf[NB1FW_LOG_DP_DL_MEAS_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_dp_dl_meas_ind_s;

/**< Measurement logging struct */
typedef struct
{
  union
  {
    struct
    {
      uint32 slotNum     : 1;  /**< slot index  0 or 1 */
      uint32 subFrNum    : 4;  /**< subframe index [1..4] */
      uint32 fullFrNum   : 14; /**< frame number */
      uint32             : 13; /**< Unused */
    };
    uint32 info;
  } timeInfo;

  int16  cellIdx;
  int16  cellId;                /**< Cell Id for which meas is logged */
  uint8  numRsPort;
  uint8  measState;  
  /* mib regeneration */
  uint16 FrameNum;             /*calculated fullFrNum of this cell, include 6 bits SSFN and 10 bits FrameNum  */
  uint32 mib33to6;             /* mib got from sw */
  /* SNR and threshold */
  uint32 num_sf_it   :8;            /* how many interation have done */
  uint32 num_sf_acorr:8;         /* interation number for acorr based SNR estimation */
  uint32             :16;
  uint32 TOT_EGY;             /* total signal energy calculated to compute SNR */
  int32 MIB_SNR_LINEAR;       /* mib snr in linear and without compensation */ 
  int32 MIB_SNR_FINAL_DB;     /* mib snr linear convert to dB and with compensation */
  int32 threshold;            /* threshold from mib snr */
  /* frame metric */
  uint32 num_sf_corr:8; 
  uint32 timeHypoMax:8;          /* max time hypo index */
  uint32 freqHypoMax:8;          /* max freq hypo inde */
  uint32            :8; 
  uint32 mibPower;            /* mibPower calculated in this subfrme*/
 // int32 metric;             /*metric calculated in this frame */
  int32 MIB_FRAME_METRIC;     /* mib metric to decide if mib detected or not */
  
  uint32 mib_detect_flag : 8;   /* if mib is detected or not */
  uint32                 : 24;
  int32 MIB_RSRP;             /* calculated MIB_RSRP in linear */
  int32 RSRP_dB10; 
  int32 RSSI;      /**< Inband RSSI value used for NRSRQ estimation in dB10
                      * Displayed value = <value> / (2^7 * 10)
                      */

  int32 NRSRQFinalEst;  /**< NRSRQ Final Estimate at end of meas period in dB10
                          * Displayed value = <value> / (2^7 * 10)
                          */
  int32 rfOffset;

  int32 totalFerrPpmq10 : 16;   /**< Total freq error in PPM Q10 */
  int32                 : 16;
  int32 pos_offset;        /* new pos offset = old pos_offset + timing error */


} nb1fw_log_dp_dl_pbch_meas_s;

/**< PBCH based measurement summary shows the information regarding intermediate, final values
  * used in measurements along with its control information 
  * Min Size = 4 bytes 
  * Max Size = 404 bytes
  * Display Format:
  * Display Format:
  * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  *|   |    |        |         |       |      |      |         |        |       |       |        |        |        |        |         |         |         |        |         |        |         |        |     |           |      |    |     |
  *|   |    |        |         |       |      |      |         |        |       |       |        |        |        |         |         |         |        |         |        |         |        |     |           |Total |    |     |
  *|   |Slot|SubFrame|FullFrame|       |      |Meas  |Mib33to6 |FrameNum|numRs  |numSfIt|num_sf  |        |        |MIB_SNR  |MIB_SNR  |         |num_sf  |MIB_FRAME|timeHypo|freqHypo |MIB_RSRP|RSRP |Mib Detect |FErr  |RSSI|     |
  *|#  |Num |Num     |Num      |cellIdx|pcid  |State |         |        |Port   |       |accor   |mibPower|TOT_EGY |(linear) |FINAL_DB |threshold|corr    |METRIC   |Max     |Max      |        |dB10 |Flag       |      |    |RSRQ |rfOffset
  *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  *|  0|   1|       4|      130|  0    | 1    |     3|      020|    130 |    1  |   1   |  0     |16282   |      18|    2   |    0    |      1  |    1     |   0   |    1    |   0    | 0       |  1     |    0|  0        |    1 |    |     |
  *|   |    |        |         |       |      |      |         |        |       |       |        |        |        |        |         |         |          |       |         |        |         |        |     |           |      |    |     |
  *|  1|   1|       6|      130|  0    | 2    |    96|    0x516|     130|    1  |    1  |  0     |16266   |      18|    2   |    0    |      1  |    1     |   0   |    1    |    0   | 0       |  1     |    0|  2        |    1 |1801|
  *|   |    |        |         |       |      |      |         |        |       |       |        |        |        |        |         |         |         |        |         |        |       |           |     |   |     |      |
  *|  2|   1|       7|      130|  1    | 1    |     3|      020|    130 |    1  |   1   |  0     |16282   |      18|    2   |    0    |      1  |    1     |   0   |    1    |   0    | 0       |  1     |    0|  0        |    1 |    |     |    
  *|   |    |        |         |       |      |      |         |        |       |       |        |        |        |        |         |         |          |       |         |        |         |        |     |           |      |    |     |    
  *|  3|   1|       8|      130|  1    | 2    |    96|    0x516|     130|    1  |    1  |  0     |16266   |      18|    2   |    0    |      1  |    1     |   0   |    1    |    0   | 0       |  1     |    0|  2        |    1 |1801|    
  */
  
  
typedef struct
{
  /* 0 .. 32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32               : 16; /**< not used */

  /* 32 .. 2912 */
  nb1fw_log_dp_dl_pbch_meas_s buf[NB1FW_LOG_DP_DL_PBCH_MEAS_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_dp_dl_pbch_meas_ind_s;


/*==========================================================================*/
/* NB1 FW Downlink Sync Signal (PSS/SSS) based Frequency estimation
 * Summary (Log code ID: 0xB216)
 *---------------------------------------------------------------------------*/
typedef struct
{
  /* 0..32 */
  int16  frameNum;        /**< system frame number, range 0-1024 */

  int16  ssType     : 1;  /**< sync Signal used for accumulation, valid Range 0 (PSS) 1(SSS)
                            * Display String - PSS, SSS */
  int16  accumCount : 7;  /**< accumulation done so far */
  int16             : 8;  /**< not used */

  /* 32..384 */
  int32 aveCh[11];        /**< average channel for 11 symbols of SSS
                            * Display as set of 2 signed values (lower 16 bits/ 2^15, higher 16 bits/ 2^15)*/
}nb1fw_log_dp_dl_ssfee_s;

/**< The downlink Sync Signal(PSS/SSS) base frequency error estimation routine
  * log packet with information on the frequecy estimate and average channel
  * accumulated every accum duration
  * Min Size = 16 bytes
  * Max Size = 2896 bytes
  * Display Format:
  *
  * Serving Cell Id : <pcid>
  * FEE mode : <mode>
  * Max Accum length : <maxAccum>
  * FEE Ready : <feeReady>
  * Freq Error : <freqErrEstQ6>
  * UL Gap FEE Limit : <ulGapFreqLimitQ6>
  * PSD Accumulation : <sumPSD>
  * Max PSD : <maxPSD>
  * PSD Cleaning Threshold : <cleanPsdThresh>
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------|
  * | Frame | SS   | Accum |                                                    Average Channel                                                                |
  * |       | Type | Count |    0      |    1      |    2      |     3     |    4      |    5      |    6      |     7     |    8      |    9      |    10     |
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |  12   | PSS  |    21 |0.007,1.956|0.007,0.012|0.001,1.992|0.001,1.980|0.007,1.956|0.007,0.012|0.001,1.992|0.001,1.980|0.007,1.956|0.007,0.012|0.001,1.992|
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------|
  * |  12   | SSS  |    22 |0.017,1.962|1.999,0.001|0.009,1.992|1.999,0.028|0.017,1.962|1.999,0.001|0.009,1.992|1.999,0.028|0.017,1.962|1.999,0.001|0.009,1.992|
  * |----------------------------------------------------------------------------------------------------------------------------------------------------------|
  */
typedef struct
{
  /* 0 .. 32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32               : 16; /**< not used */

  /* 32 .. 256 */
  uint32  pcid         : 16; /**< serving cell Id as received in cell cfg */
  uint32  maxAccum     : 7;  /**< maximum number of accum to be done this gap */
  uint32  mode         : 1;  /**< fee mode - 0 -> UL Gap, 1 -> Warmup
                               *  Display as String, range - Ul Gap, Warmup
                               *  */
  uint32  feeReady     : 1;  /**< TRUE or FALSE based on if FEE is available */
  uint32               : 7;  /**< not used */

  int32   freqErrEstQ6;      /**< Frequency estimate this cycle
                               *  Displayed value = <value> / 2^6
                               *  */
  uint32  ulGapFreqLimitQ6;  /**< FEE bounds for UL gap
                               *  Displayed value = <value> / 2^6
                               *  */
  uint64  sumPSD;            /**< summation of PSD, Display unsigned decimal */
  uint32  maxPSD;            /**< maximum of PSD, Display unsigned decimal */
  uint32  cleanPsdThresh;    /**< log as unsigned decimal for now */

  /* 256 .. 23296 */
  nb1fw_log_dp_dl_ssfee_s buf[NB1FW_LOG_DP_DL_SSFEE_NUM_BUFS]; /**< Logging buffer */

}nb1fw_log_dp_dl_ssfee_ind_s;


/*==========================================================================*/
/* NB1 FW Downlink Channel state
 * Summary (Log code ID: 0xB216)
 *---------------------------------------------------------------------------*/

/** 
 *  DL channel state logging structure
 */
typedef struct
{
  /* 0..15 */
  uint16 slotNum  : 1;      /**< slot index  0 or 1 */  
  uint16 subFrNum : 4;      /**< subframe index [1..4] */
  uint16 sFrNum   : 10;     /**< system frame number [6..15] */
  uint16          : 1;      /**< Unused */

  /** 
   *   DL channel state
   */
  /* 16..23 */
  uint16 chan            : 3;   /**< Active Channel type, 0 = PBCH, 1 = SIB, 2 = PDCCH, 3 = PDSCH */       
  uint16 state           : 3;   /**< Channel state, 0 = DISABLED,  1 = CONFIG, 2 = WAIT, 3 = ENABLED, 4 = PRE_SUSPENDED, 5 = SUSPENDED */
  uint16 decodeSuccess   : 1;   /**< Channel decode is successful */
  uint16 dbackInit       : 1;   /**< DBACK is initialized */
  uint16                 : 8;

  /** DL schedule update time */
  /* 16..48 */
  union
  {
    struct
    {
      /* PBCH is configured in ACQ more or not */
      uint32 acqMode  : 1;  /**< PBCH Acq Mode */
      uint32 newCfg   : 1;  /**< Handling new cell config */
      uint32          : 30; /**< not used */
    } pbch;

    struct
    {
      uint8 repIdx;         /**< Repetitions completed */
      uint8 ruIdx;          /**< Resource Units completed */
      uint8 decEnMask;      /**< Indicate SIBs that have decode enabled */
      uint8 sibIdx   : 4;   /**< SIB Index corresponding to the SIB log data */
      uint8          : 4;   /**< not used */
    } sib;

    struct
    {      
      uint16 repIdx         : 12;    /**< current rep idx and num rep (based on max rep or Ri from DCI */
      uint16 reachedDciReps : 1;     /**< Flag indicating if DCI Reps are reached */
      uint16 reachedRmax    : 1;     /**< Flag indicating if R-max is reached */
      uint16 isSuspended    : 1;     /**< Flag indicating if pdcch is suspended */
      uint16                : 1;     /**< notused */
      uint16 ssResumeGSubFn;         /**< search space resume/start subframe number after suspend */
    } pdcch;

    struct
    {
      uint16 sfIdx;           /**< current Subframe Idx (based on numReps and numSfs) */
      uint16 ssStartGSubFn;   /**< Search space start subframe number */
    } pdsch;

    uint32 info;

  } info;

} nb1fw_log_dl_chan_state_s;

/**< The downlink channel state summary shows information regarding the state and parameters 
  * of the downlink channels. Channel information is gathered for every Subframe 
  * Min Size = 2 bytes 
  * Max Size = 122 bytes 
  * Display Format:
  * |-----------------------------------------------------------------------------------------------------------------------|
  * | DL Channel State Summary                                                                                              |
  * |-----------------------------------------------------------------------------------------------------------------------|
  * | Time        | Channel  |    State        | Dec. Pass | DBACK Init | Info                                              |
  * |-----------------------------------------------------------------------------------------------------------------------|
  * |   61:6:0    |  PDCCH   |            WAIT |   YES     |    NO      | 0x      3010                                      |
  * |-----------------------------------------------------------------------------------------------------------------------|
  * |   61:7:0    |   PBCH   |        DISABLED |   YES     |    NO      | 0x         1                                      |
  * |-----------------------------------------------------------------------------------------------------------------------|
  * |   61:7:0    |    SIB   |         ENABLED |   YES     |    NO      | 0x       703                                      |
  * |-----------------------------------------------------------------------------------------------------------------------|
  */
typedef struct
{
  /* 0..16 */
  uint32  version : 8; /**< version number of log packet */
  uint32  bufCnt  : 8;  /**< Number of valid items logged */
  uint32          : 16;

  /* 16..976 */
  nb1fw_log_dl_chan_state_s buf[NB1FW_LOG_DL_CHAN_STATE_NUM_BUFS]; /**< Logging buffer */

} nb1fw_log_dl_chan_state_ind_s;


/*==========================================================================*/
/* NB1 FW MULTI GRANT Logging
 * Summary (Log code ID: 0xB230)
 *---------------------------------------------------------------------------*/

typedef struct
{
  /* 0..15 */
  uint16 slotNum  : 1;      /**< slot index  0 or 1 */  
  uint16 subFrNum : 4;      /**< subframe index [1..4] */
  uint16 sFrNum   : 10;     /**< system frame number [6..15] */
  uint16          : 1;      /**< Unused */

  /* 16..31 */
  uint16 dbIdx      : 1;     /**< db Idx  0 or 1 : Bit (0)*/  
  uint16 harqId     : 1;     /**< harq Id 0 or 1 : Bit (1)*/
  uint16 grantType  : 2;    /**< DCI Rep SFN             */  
  uint16            : 12;    /**< TODO: DCI Rep SFN             */  

  /* 32..47 */
  uint16 ssFrz      : 16;   /**< Search Space Freeze Action Time */  
  /* 48..63 */  
  uint16 ssUnFrz    : 16;   /**< Search Space Un-Freeze Action Time */  
  /* 64..79 */  
  uint16 dlSuspend  : 16;     /**< Dl Suspend Action Time */  
  /* 80..95 */  
  uint16 dlResume   : 16;      /**< Dl Resume Action Time */
  /* 96..111 */  
  uint16 globalSsFrz  : 16;   /**< Global/Consloidated SS Freeze Action Time */
  /* 112..127 */  
  uint16 globalSsUnFrz : 16;   /**< Global/Consloidated SS UnFreeze Action Time */
  /* 128..143 */  
  uint16 globalDlSuspend : 16;  /**< Global/Consloidated DL Suspend Action Time */
  /* 144..159 */  
  uint16 globalDlResume  : 16;  /**< Global/Consloidated DL Resume Action Time */
  /* 160..175 */  
  uint16 sSstartSfn  : 16;  /**< Search space start sub-frame */
  /* 176..191 */  
  uint16 dciRepSfn  : 16;  /**< dci Rep sub-frame */

} nb1fw_log_dl_multi_grant_s;


typedef struct
{
  /* 0..16 */
  uint32  version : 8; /**< version number of log packet */
  uint32  bufCnt  : 8;  /**< Number of valid items logged */
  uint32          : 16;

  /* 16..976 */
  nb1fw_log_dl_multi_grant_s buf[NB1FW_LOG_DL_MULTI_GRANT_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_dl_multi_grant_ind_s;




/*==========================================================================*/
/* NB1 FW CARRIER SWITCH Logging
 * Summary (Log code ID: 0xB22B)
 *---------------------------------------------------------------------------*/

typedef struct
{
  /* 0..15 */
  uint16 slotNum  : 1;      /**< slot index  0 or 1 */  
  uint16 subFrNum : 4;      /**< subframe index [1..4] */
  uint16 sFrNum   : 10;     /**< system frame number [6..15] */
  uint16          : 1;      /**< Unused */

  /* 16..31 */
  uint16 mcCfgCmdEvtT       : 16;   /**< MC CFG CMD Event Time */  
  /* 32..47 */
  uint16 rxChainDisableEvtT : 16;   /**< RX CHAIN DIS Event Time */  
  /* 48..63 */  
  uint16 rxChainEnableEvtT  : 16;   /**< RX CHAIN ENA Event Time  */  
  /* 64..79 */  
  uint16 mcCfgCnfEvtT       : 16;   /**< MC CFG CNF Event Time */  
  /* 80..95 */  
  uint16 retuneActTime      : 16;    /**< RETUNE Action Time */

  /* 96..111 */
  uint16 isAnchor      : 1;     /**< IsAnchor         : Bit (0)*/  
  uint16 isPdcchValid  : 1;     /**< IsPdcchCfg Valid : Bit (1)*/
  uint16 mcCfgMode     : 4;     /**<  mcCfgCmdMode             */  
  uint16 chanBmsk      : 10;    /**< chanBmsk on New Carrier   */  

} nb1fw_log_dl_carrier_switch_s;


typedef struct
{
  /* 0..16 */
  uint32  version : 8; /**< version number of log packet */
  uint32  bufCnt  : 8;  /**< Number of valid items logged */
  uint32          : 16;

  /* 16..976 */
  nb1fw_log_dl_carrier_switch_s buf[NB1FW_LOG_DL_CARRIER_SWITCH_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_dl_carrier_switch_ind_s;


/*==========================================================================*/
/* NB1 FW RAFTT Logging
 * Summary (Log code ID: 0xB22D)
 *---------------------------------------------------------------------------*/
typedef struct
{
  /* 0..15 */
  uint32 startslotNum  : 1;      /**< slot index  0 or 1 */
  uint32 startsubFrNum : 4;      /**< subframe index [1..4] */
  uint32 startsFrNum   : 10;     /**< system frame number [6..15] */
  uint32               : 1;      /**< Unused */
                                 /**< Displayed value = startsFrNum:startsubFrNum:startslotNum */

  /* 0..15 */
  uint32 endslotNum  : 1;      /**< slot index  0 or 1 */
  uint32 endsubFrNum : 4;      /**< subframe index [1..4] */
  uint32 endsFrNum   : 10;     /**< system frame number [6..15] */
  uint32             : 1;      /**< Unused */
                               /**< Displayed value = endsFrNum:endsubFrNum:endslotNum */
  int32  NRSEEQ24;             /**< NRSEE in Q24 linear format */
  int32  NRSRPdBmQ7;           /**< NRSRP in Q7,
                                 *  Display value = <value> / (2^7*10) */
  int32  NRSRQ;                /**< NRSRQ value. dB Q7
                                 *  Display value = <value> / (2^7*10) */
  int32  RSSIdBmQ7;            /**< RSSI in Q7,
                                 *  Display value = <value> / (2^7*10) */
  int32  rfOffsetdBmQ7;        /**< rf offset in Q7,
                                   *  Display value = <value> / (2^7*10) */
  int32 SNRQ22Lin;             /**< SNR value in Q22 linear format */
  uint32 maxPowerQ16;          /**< max power in Q16 linear format */
  uint32 totalPwrQ16;          /**< total power in Q16 linear format */
  int32  NoisePwrQ16;          /**< noise power in Q16 linear format */
  uint8 f_idx;                 /**< freq hypothesis index, range: 0 to 20 */
  uint8 t_idx;                 /**< time hypothesis index, range:  0 to 4 */
  int32 freqErrEstQ6Hz;        /**< freq error in Q6 hz
                                 * Display value = <value> / 2^6
                                 * */
  int32 timeErrEst;            /**< time error estimate, possible values:  { -4, -2, 0, 2, 4} */
  uint8 numSlotsPilots;        /**< pilot slot count, range: 0 to 16 */
  uint8 numValidSlots;         /**< valid slot count,  range: 0 to 16 */
  int32  cellId;               /**< Cell id, range: +ve value */
  int32  numTx;                /**< number of Tx, possible values: 1 or 2 */

} nb1fw_log_dl_raftt_s;


/**< RAFTT log packet shows values related to algo, after RAFTT is done.
  * Min Size =
  * Max Size =
  * Display Format:
  * |---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  * | Start Proc Time |End Proc Time   |     NRSEE(Q24)   | NRSRP(dBm)  | NRSRQ(dB)   |RSSI(dBm)   | RF      | SNR     | maxPower|total Pwr|noise pwr|    Tracking Idx   | Freq error | Time error | No    | No    | Cell Id | Num Tx|
  * | [SFN:SubFN:Slt] |[SFN:SubFN:Slt] |                  |             |             |            | Offset  |(Q22 lin)|  (Q16)  |(Q16)    |(Q16)    |-------------------| ( Hz )     | (1.92 mhz) | pilot | valid |         |       |
  * |                 |                |                  |             |             |            |(dBm)    |         |         |         |         |   f_idx |  t_idx  |            |            | slots | slots |         |       |
  * |---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  * |    12: 0:0      |   12: 0:0      |   -775033022     |    -70571   |   -70251    |  -458      | 1254    |   20    |   20    |   20    |   20    |   10    |  2      |  12345     |  12345     |  2    |  2    |    10   |  2    |
  * |---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  * |    12: 0:0      |   12: 0:0      |   -775033022     |    -70571   |   -70251    |  -458      | 1254    |   20    |   20    |   20    |   20    |   11    |  0      |  986763    |  986763    |  1    |  1    |    10   |  1    |
  * |---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  */

typedef struct
{
  /* 0..16 */
  uint32  version : 8; /**< version number of log packet */
  uint32  bufCnt  : 8;  /**< Number of valid items logged */
  uint32          : 16;

  /* 16..976 */
  nb1fw_log_dl_raftt_s buf[NB1FW_LOG_DL_RAFTT_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_dl_raftt_ind_s;

/*==========================================================================*/
/* NB1 FW NRS MEASUREMENT Logging
 * Summary (Log code ID: 0xB22C)
 *---------------------------------------------------------------------------*/

typedef struct
{
  /* 0..15 */
  uint32 slotNum  : 1;      /**< slot index  0 or 1 */  
  uint32 subFrNum : 4;      /**< subframe index [1..4] */
  uint32 sFrNum   : 10;     /**< system frame number [6..15] */
  uint32          : 1;      /**< Unused */

  /* 16..31 */
  uint32 nrsDetected       : 16;   /**< NRS Detected Indication*/
  /* 32..47 */
  uint32 nrsHDDetected     : 16;   /**< NRS HD Detected Indication */  
  /* 48..63 */  
  uint32 nrsNSF            : 16;   /**< NRS Acc Length  */  
  /* 64..95 */  
  uint32 nrsSNRLinear      ;       /**< NRS SNR linear */  
  /* 96..127 */  
  uint32 nrsSNRdB          ;       /**< NRS SNR dB level */
  /* 32.. 63*/
  int32 signalEst          ;       /**< NRS Signal Power */
  /* 64.. 95*/             
  int32 totalEst           ;       /**< NRS Total Power */
  /* 96 .. 127 */
  int32 signalEstIIR       ;       /** <NRS signal Power IIR 32Q30> **/
   /* 128 .. 159 */
  int32 totalEstIIR        ;       /** <NRS total Power IIR 32Q30> **/

} nb1fw_log_dl_nrs_measurement_s;


typedef struct
{
  /* 0..16 */
  uint32  version : 8; /**< version number of log packet */
  uint32  bufCnt  : 8;  /**< Number of valid items logged */
  uint32          : 16;

  /* 16..976 */
  nb1fw_log_dl_nrs_measurement_s buf[NB1FW_LOG_DL_NRS_MEASUREMENT_NUM_BUFS]; /**< Logging buffer */
} nb1fw_log_dl_nrs_measurement_ind_s;

#endif /* NB1FW_LOG_DL_H */

