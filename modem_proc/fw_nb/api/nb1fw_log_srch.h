 /*!
  @file
  nb1fw_log_srch.h

  @brief
  NB1 FW SRCH logging interface

  @detail
  NB1 FW SRCH logging interface

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/nb1fw_log_srch.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
9/27/16    gurdeeps Updated bufCnt to 32 to avoid padding issue
9/27/16    ahmedm   Created the header file for SRCH Logging
===========================================================================*/

#ifndef NB1FW_LOG_SRCH_H
#define NB1FW_LOG_SRCH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/**<  number of packets buffered before submit for DL sched  */
#define NB1FW_LOG_SRCH_PSS_HCORR_NUM_BUFS         (1)
#define NB1FW_LOG_SRCH_PSS_REFINE_SSS_NUM_BUFS   (1)
#define NB1FW_LOG_SRCH_SSS_NUM_BUFS               (1)
#define NB1FW_LOG_SRCH_PSS_HCORR_ENTRY_NUM (15)
/*===========================================================================

      Typedefs

===========================================================================*/




/*==========================================================================*/
/* NB1 FW Search PSS Hcorrelation Results Summary (Log code ID: 0xB20C)
 *---------------------------------------------------------------------------*/

typedef struct
{

    uint32                       dvgaGain;

    uint32                       numFramesDone       : 8;                           /**< number of radio frame processed */
    uint32                       pssPeakPass         : 1;                           /**< indication of peak peak pass or not */
    uint32                       reserved0           : 23;
   
    /* 32..160 */
    int32                        pssPeakA[5];                            /**< pss hcorrelation peak value for each freq hypo*/
    int32                        pssPeakPos[5];                          /**< pss hcorrelation peak position for each freq hypo */
    int32                        pssPeakPower[5];                        /**< pss hcorrelation peak power for each freq hypo */
    uint32                       pssAccumPower[5];                       /**< associated pss energy level, used to calculate the threshold */
    uint32                       threshold;                              /**< threshold used to compare with pssPeakPower */ 
    uint32                       pssPosition;
    int32                        frequencyOffset;                        /**< frequency offset in Hz */
    
}nb1fw_log_srch_pss_hcorr_per_frame_s;

 
typedef struct
{
  /* 0..31 */
  uint32                       associatedCmdType    : 8;                           /**< associated L1 command type 
                                                                                    * Displayed String based on value
                                                                                    * 0 = "NONE",
                                                                                    * 1 = "FULL_FSCAN",
                                                                                    * 2 = "LIST_FSCAN",
                                                                                    * 3 = "INIT_ACQ",
                                                                                    * 4 = "CELL_SRCH",
                                                                                    * 5 = "ABORT",
                                                                                    * 6 = "BLOCKED_CELL_LIST" */
  uint32                       associatedEarfcn      :16;                                  /**< associated earfcn */
  uint32                       scanLevel             :3 ;                      /*  0 deep scan; 1, shallow scan */
  uint32                       pssPeakPass           :1 ;

  uint32                       runTwiceFlag          : 1;               /* runTwiceFlag is 1 for large frquency uncertainty */
  uint32                       numLogFrame           :3 ;     /*how many frames will display in the log */                   

  /* 32 ... 63  */                                                                                
  uint32                       frequency;                                 /* earfcn frequency in Q8 */
  /*63...95 */
  uint32                       ferrUncertaintyPpmQ10  :16;    
  uint32                       fMax                   : 6;         /* from 0  to 50 */
  uint32                       coherentLen            : 7;         /* can be 32 or 64 */
  uint32                       paramSetIdx            : 3;         /* from 0 to 5 */

  /*96 ...  191*/
  int16                        ferrHypoes[5];                 /* frequency hypoes */   
  uint16                       reserved2;

  uint32                       centerFreqOffset     : 16;  /* will be valid only when runTwiceFlag is true */
  uint32                       StartStmrSubFrCnt    : 11;  /*!< Bits within a radio frame, range [0..VSTMR_NB1_SUBFRAME_RANGE-1] */ 
  uint32                       StartStmrsubFrNum    : 4;   /*!< Sub frame with in radio frame, range [0..VSTMR_NB1_SUBFR_CNT_RANGE-1]  */ 
  uint32                       reserved3            : 1;   /*!< Unused */

  nb1fw_log_srch_pss_hcorr_per_frame_s  frame[NB1FW_LOG_SRCH_PSS_HCORR_ENTRY_NUM]; 

} nb1fw_log_srch_pss_hcorr_s;

/**< log packet definition for pss hcorrelation task (init-acq) 
  * Min Size = 4 bytes 
  * Max Size = 432 bytes   (25*4+7)*4+4 =  432
  * Display Format:
  * 
  *  associatedCmdType =    
  *  associatedEarfcn  =     
  *  scanLevel  =          
  *  frequency   =
  *  ferrUncertaintyPpmQ10 =  
  *  runTwiceFlag    =       
  *  fMax  =      
  *  coherentLen      =       
  *  ferrHypoes[5]     =  
  *  logFrameNum = 
  * |-----------------------------------------------------------------------------------------------------|
  * |  |NumFrame  | dvga |Peak  |Peak      |peak       |peak       |Position|threshold |pssPos |freqOffset|
  * |# |Done      | Gain |Pass  |A(Hex)    |Power      |accumPow   |        |          |       |          |
  * |-------------------------------------------------------------------- --|----------|-------|----------|
  * | 0|  1       | 122  |   1  | 32465fef |           |           |        |          |13456  |  -7500   |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |----------------------------------------------------------------------------------|-------|----------|
  * | 1|  2       | 122  |   1  | 32465fef |           |           |        |          |       |          |        
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |----------------------------------------------------------------------------------|-------|----------|
  * | 2|  3       | 122  |   1  | 32465fef |           |           |        |          |       |          | 
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |  |          |      |      | 32465fef |           |           |        |          |       |          |
  * |-----------------------------------------------------------------------------------------------------|

  */
typedef struct
{
  /* 0..32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged */
  uint32               : 16;

  /* 32..1248 */
  nb1fw_log_srch_pss_hcorr_s   buf[NB1FW_LOG_SRCH_PSS_HCORR_NUM_BUFS];  /**<  Logging buffer  */
} nb1fw_log_srch_pss_hcorr_ind_s;



/*==========================================================================*/
/* NB1 FW Search SSS Results Summary (Log code ID: 0xB20A)
 *---------------------------------------------------------------------------*/

typedef struct
{
  uint16 pos;          /* Sub radio frame position (0..19200-1) */
  uint16 sfn_offset;   /* SFN (systems frame number) offset from ref time (0..7) */
  uint16 pcell_id;     /* Pcell Id from SSS */
  int16  ferr_ppm_q10; /* frequency error measured by FW */
  int32  pssPsr;
  int32  sssPsr;
} nb1fw_srch_cell_srch_peak_log_s;

typedef struct
{
  /* 0..32 */
  uint32                            associatedCmdType   : 8;    /**< associated L1 command type
                                                                 * Displayed String based on value
                                                                 * 0 = "NONE",
                                                                 * 1 = "FULL_FSCAN",
                                                                 * 2 = "LIST_FSCAN",
                                                                 * 3 = "INIT_ACQ",
                                                                 * 4 = "CELL_SRCH",
                                                                 * 5 = "ABORT",
                                                                 * 6 = "BLOCKED_CELL_LIST" */
  uint32                            numFramesDone       : 8;    /**< number of radio frame processed */
  uint32                            sssPeakPass         : 1;    /**< indication of peak passing threshold */
  uint32                            reserved0           : 15;

  /* 32..64 */
  uint32                            associatedEarfcn;           /**< associated earfcn */
  /* 64..192 */
  nb1fw_srch_cell_srch_peak_log_s       peak;


} nb1fw_log_srch_sss_s;

/**< log packet definition for sss cross-correlation task (cell search) 
  * Min Size = 4 bytes 
  * Max Size = 32 bytes 
  * Display Format:
  *
  * initial_sfn_offset  = 4 
  * initial_pos_offset  = 1001        
  * initial_ferr_ppm_q10 = -200
  * |------------------------------------------------------------------------------------|
  * |  |Cmd       |Fr# |EARFCN |Peak |Cell |SFN    |Ferr   |Cell  |PssPsr      |Sss      | 
  * |# |          |    |       |Pass |Id   |Offset |ppmQ10 |Pos   |            |Psr      | 
  * |------------------------------------------------------------------------------------|
  * | 0|  INIT_ACQ|   6|   1234|    0|  124|      0|      0| 10128|  31182268  |  354048 |  
  * |------------------------------------------------------------------------------------|
  *
  */
typedef struct
{
  /* 0..32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32  bufCnt       : 8;  /**< Number of valid items logged*/
  uint32  initial_sfn_offset    : 16;  /* intial sfn offset  */
  
  uint16  initial_pos_offset;         /* intial pos offset */
  int16  initial_ferr_ppm_q10;        /* intial frequency error */
  /* 64..256 */
  nb1fw_log_srch_sss_s              buf[NB1FW_LOG_SRCH_SSS_NUM_BUFS];           /**<  Logging buffer  */
} nb1fw_log_srch_sss_ind_s;


/*==========================================================================*/
/* NB1 FW Search PSS refine SSS Results Summary (Log code ID: 0xB20D)
 *---------------------------------------------------------------------------*/

typedef struct
{  
  uint32  sfnOffset : 4;
  uint32  cellId    : 12;
  uint32  cellPos   : 16;
  uint32  sssPsr    : 32;
}nb1fw_log_cell_info_s;  

typedef struct
{
  /* 0..32 */
  uint32                       associatedCmdType   : 8;    /**< associated L1 command type
                                                            * Displayed String based on value
                                                            * 0 = "NONE",
                                                            * 1 = "FULL_FSCAN",
                                                            * 2 = "LIST_FSCAN",
                                                            * 3 = "INIT_ACQ",
                                                            * 4 = "CELL_SRCH",
                                                            * 5 = "ABORT",
                                                            * 6 = "BLOCKED_CELL_LIST" */
															
  uint32                       numFramesDone       : 4;    /**< number of radio frame processed */
  uint32                       threadIdx           : 4;    /**< thread index */
  uint32                       pssRefinePass       : 1;    /**< indication of pss refine pass or not */
  uint32                       reserved            : 7;
  uint32                       detectedCellNum     : 8;    /* how many cells detected in this pssPeak */

  /* 32..64 */
  uint32                       associatedEarfcn    ;           /**< associated earfcn */

  /* 64..96 */
  int32                        ferrPpmQ10          : 16;   /**< frequency error in ppm Q10 format */
  int32                        pssPeakPos          : 16;

  /*96..128 */  
  int32                        inputPssPeakPos     : 16;            /**< input pss peak position */
  int32                        inputPssPeakCFO     : 16;        /**< input pss peak frequency offset */

  /* 128..384 */
  nb1fw_log_cell_info_s        cellInfo[4];  

  /* 384..416 */
  uint32                       pssPsr;                     /**< pss energy level */
} nb1fw_log_srch_pss_refine_sss_s;

/**< log packet definition for pss refine and sss zift task (common for init-acq and neighbor search) 
  * Min Size = 4 bytes 
  * Max Size = 57 bytes 
  * Display Format:
  * 
  * initial_sfn_offset  = 4 
  * initial_pos_offset  = 1001        
  * initial_ferr_ppm_q10 = -200
  *----------------------------------------------------------------------------------------------------
  *|   |          |Num   |      |      |pss   |       |      |     |      |        |    |      |      |      |
  *|   |Associated|Frames|Thread|      |refine|pssPeak|Ferr  |Input|Input |detected|Cell|Cell  |Sfn   |      |      |
  *|#  |Cmd Type  |Done  |Id    |EARFCN|Pass  |Pos    |ppmQ10|Pos  |CFO   |cellNum |Id  |Pos   |Offset|sssPsr|pssPsr|
  *----------------------------------------------------------------------------------------------------
  *|  0|FULL_FSCAN|    12|     4|    64|   1  |      0| -6725| 6380|-12346|  1     |   0|   0  |     0|     0| 27276|
  *|   |          |      |      |      |      |       |      |     |      |        |   0|   0  |     0|     0|      |
  *|   |          |      |      |      |      |       |      |     |      |        |   0|   0  |     0|     0|      |
  *|   |          |      |      |      |      |       |      |     |      |        |   0|   0  |     0|     0|      |
  

  */
typedef struct
{
  /* 0..32 */
  uint32  version      : 8;  /**<*< version number of log packet */
  uint32  bufCnt       : 8;  /**<*< Number of valid items logged*/
  uint32  initial_sfn_offset    : 16;  /* intial sfn offset  */
  
  uint16  initial_pos_offset;         /* intial pos offset */
  int16  initial_ferr_ppm_q10;        /* intial frequency error */
  /* 64..448*/
  nb1fw_log_srch_pss_refine_sss_s                      buf[NB1FW_LOG_SRCH_PSS_REFINE_SSS_NUM_BUFS]; /**<  Logging buffer  */
} nb1fw_log_srch_pss_refine_sss_ind_s;





#endif /* NB1FW_LOG_SRCH_H */

