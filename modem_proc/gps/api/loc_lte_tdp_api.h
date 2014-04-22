#ifndef LOC_LTE_TDP_API_H
#define LOC_LTE_TDP_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Location LTE TDP API Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE TDP module and
other modules outside GNSS.

Copyright (c) 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/api/loc_lte_tdp_api.h#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "comdef.h"
#include "cgps_api.h"


/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

#define MAX_SEARCHER_CELLS_PER_EARFCN      10

/* Max number of cells in blacklist. Note that if this macro is updated,
TDP_MAX_BLACKLIST_SIZE in wwanme_tdp_db.h should also be updated */
#define TDP_MAX_BLACKLIST_LEN                 32 

#define TDP_VSRC_NUM_CARRIERS                 3

/*--------------------------------------------------------------------------
 * Op CRS Type Declarations
 *-----------------------------------------------------------------------*/

typedef enum
{
  LOC_LTE_OPCRS_MEAS_SUCCESS = 0,
  LOC_LTE_OPCRS_MEAS_ERROR,
  LOC_LTE_OPCRS_MEAS_MAX
}
loc_lte_tdp_OpCrsMeasStatusTypeVal;

typedef uint8 loc_lte_tdp_OpCrsMeasStatusType;

typedef struct
{
  uint16  w_Sfn;
  boolean b_Valid;
  uint64  t_VsrcFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime; 
} 
loc_lte_tdp_OpCrsVsrcInfoType;

typedef struct 
{
  /* Ref Time */
  boolean                  b_RefTimeValid;
  /* Ref Time Validity */
  uint64                   t_RefTime;
} loc_lte_tdp_RefInfoType;

/* LTE Time Transfer sets of O_STMR+GNSS RTCs latched at the same time structure */
typedef struct
{
  /* O_STMR count value (in CDMA Chipx8) */
  uint32  q_OStmr;

  /* O_STMR phase value (in CDMA Chipx2^35) */
  uint32  q_OStmrPhase;

  /* GNSS RTC value (in GPS Chipx80) */
  uint32  q_GnssRtc;

  /* GNSS Phase value (in sub-GPS Chipx80) */
  uint32  q_GnssPhase;

  /* GNSS Sample Count status */
  uint32  q_SampleCount;  
}
loc_lte_tdp_LteTTLatchedDataType;

/*--------------------------------------------------------------------------
 * Op CRS Type Declarations
 *--------------------------------------------------------------------------
*/

typedef struct
{
  boolean b_Sib8Valid;   /* SIB8 "cdma-EUTRA-Synchronization" 
                            info. 
                            TRUE: LTE NW is synchronized.
                            FALSE: LTE NW is NOT synchronized */

  uint64  t_Sib8CdmaSystemTime; /* The CDMA2000 system time corresponding to 
                                   the SFN boundary at or after the ending 
                                   boundary of the SI-Window in which this Time 
                                   Transfer was taken. The size is 52 bits and 
                                   the unit is [CDMA2000 chips] */

  boolean b_Sib16Valid; /* SIB16 System Time info*/

  uint64  t_Sib16GpsMsecs; /* The Full gps time at the SFN boundary in units of 
                              msecs */

  uint16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS 
                            time */


  uint16 w_Sfn;            /* System Frame Number of this OpCRS measurement */

  uint8 u_SubFn;          /* Subframe number of this OpCRS measurement */            

  uint32  q_OStmrSfBoundary;  /* O-STMR value at the boundary of the subframe 
                                 in which CRS measurement was made */

  uint32  q_UstmrLatched; /* Universal STMR value latched at the same time as 
                             O_STMR (in 19.2MHz units) */

  uint32  q_OstmrLatched; /* OSTMR value latched at the same time as USTMR */

  uint32  q_OstmrPhaseLatched; /* OSTMR Phase value latched at the same time as USTMR */

  uint32  q_UstmrFwDLSubFN;     /* Universal STMR value at DL SF according to FW 
                                (in 19.2MHz units) */

  uint32  q_OstmrFwDLSubFN; /* OSTMR value at DL Sub Fn using VSTMR API on USTMR from FW */

  uint32  q_OstmrPhFwDLSubFN; /* OSTMR phase value at DL Sub Fn using VSTMR API on USTMR from FW */

  uint32  q_UstmrFwDLSubFnNoAdj;     /* Universal STMR value at DL SF according to FW 
                                (in 19.2MHz units) */

  uint32  q_OstmrFwDLSubFnNoAdj; /* OSTMR value at DL Sub Fn as per FW */

  boolean b_OstmrVsrcValid; /* Mark if the next field is valid or not */
  loc_lte_tdp_OpCrsVsrcInfoType z_RefVsrcInfo[3];
  loc_lte_tdp_OpCrsVsrcInfoType z_SFNVsrcInfo[3];

//  loc_lte_tdp_RefInfoType  z_RefTimeInfo;
  cgps_RefInfoType  z_RefTimeInfo;

  /* For Unit Testing purpose */
  boolean b_LatchTTr;
  loc_lte_tdp_LteTTLatchedDataType z_LatchedData[8];
  uint32 q_UstmrLatchTTr[8];
} 
loc_lte_tdp_OpcrsTimeInfoType;  


typedef struct
{
  void *p_OpcrsMeasData;  /* Pointer to shared memory block storing OpCRS
                             measurement */

  loc_lte_tdp_OpCrsMeasStatusType  e_MeasStatus; /* Measurement Status. Enum of type 
                                                    'loc_lte_opcrs_MeasStatusType' */

  uint32 q_OccasionSeqNum; /* Unique identifier sent by LTE FW along with 
                              measurement */

  uint16 w_PhyCellId;      /* Physical Cell ID of the measurement */

  uint8  u_CellType;       /* 0=Serving, 1=Nbr */

  uint16 w_Sfn;            /* System Frame Number of this OpCRS measurement */

  uint8 u_SubFn;          /* Subframe number of this OpCRS measurement */

  uint8 u_CarrierIdx;     /* 0=PCC, 1=SCC_0, and so on */

  uint8 e_DlBandwidth; /* LTE DL bandwidth of the measurement. Enum of type 
                                                'loc_lte_tdp_OpCrsBandwidthType' */

  uint32 q_Earfcn;  /* Downlink center frequency - Integer (0..262143)*/

  uint8 u_SignalPower;  /* Range (-140 dBm to -44 dBm) with 1 dBm 
                           resolution. */

  uint8  u_SignalQuality; /* Reference signal received quality.
                           For PRS measurements, the signalQuality will 
                           be RSRQ as defined in LPP. Range (-19.5 dB 
                           to -3dB with 0.5 dB resolution. The same 
                           values and encoding will be used for CRS 
                           measurements. */

  uint16 w_RxTxTimeDiff;   /* Rx-Tx time difference, as defined in LPP. 
                           Range (2 TS to 20472 TS)
                           The same values and encoding will be used 
                           for PRS and CRS measurements. */

  boolean b_CyclicPrefix; /* FALSE: normal
                             TRUE : extended */

  boolean b_AntennaPorts;	  /* FALSE: 1 or 2 transmit antennas
                             TRUE : 4 transmit antennas */

  loc_lte_tdp_OpcrsTimeInfoType z_TimeInfo; /*LTE related timing parameters*/

  uint8 u_lte_state; /*State of LTE at the time of the measurement*/

} loc_lte_tdp_OpcrsMeasInfoType;


/*--------------------------------------------------------------------------
 * WB CRS Type Declarations
 *--------------------------------------------------------------------------
*/

#define MAX_DETECTED_CELLS_PER_EARFCN 10  

/***********************************************
            WB CRS Meas Response
************************************************/

typedef enum
{
  LOC_LTE_WBCRS_MEAS_SUCCESS = 0,
  LOC_LTE_WBCRS_MEAS_ERROR,
  LOC_LTE_WBCRS_MEAS_MAX
}
loc_lte_tdp_WbCrsMeasStatusTypeVal;

typedef uint8 loc_lte_tdp_WbCrsMeasStatusType;

typedef enum
{
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_SM_WRONG_STATE = 0,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_EARFCN_MISMATCH,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_SCELL_NOT_FOUND,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_SM_ABORT,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_KERNEL_FAIL,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_INVALID_PARAMS,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_ABORT,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_UNKNOWN,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_COLLISION_MBSFN,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_CORE_STILL_IN_PROC,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_FD_DMA,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_FW_MISSED_ACTION_TIME,
  LOC_LTE_WBCRS_MEAS_ERROR_CAUSE_MAX
}
loc_lte_tdp_WbCrsMeasErrorCauseVal;

typedef uint8 loc_lte_tdp_WbCrsMeeasErrorCauseType;

typedef enum
{
  /* 1.4MHz bandwidth */
  LOC_LTE_TDP_BW_6 = 0,
  /* 3MHz bandwidth */
  LOC_LTE_TDP_BW_15,
  /* 5MHz bandwidth */
  LOC_LTE_TDP_BW_25,
  /* 10MHz bandwidth */
  LOC_LTE_TDP_BW_50,
  /* 15MHz bandwidth */
  LOC_LTE_TDP_BW_75,
  /* 20MHz bandwidth */
  LOC_LTE_TDP_BW_100,
  /* Unknown Bandwidth */
  LOC_LTE_TDP_BW_UNKNOWN
} loc_lte_tdp_WbCrsBandwidthVal;

typedef uint8 loc_lte_tdp_WbCrsBandwidthType;

typedef struct
{
  uint32 q_Ml1start; // USTMR @ ML1 start of TDP processing request
  uint32 q_Ml1stop;  // USTMR @ ML1 completion of TDP processing request
  uint32 q_Ml1ReadyToSleep; /* USTMR @ General ML1 processing ready to sleep but TDP processing 
                               is keeping it awake. */
} loc_lte_tdp_WbCrsRunTimeType;


typedef struct
{
  uint16  w_Sfn;
  boolean b_Valid;
  uint64  t_VsrcFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime; 
} 
loc_lte_tdp_WbCrsVsrcInfoType;

typedef struct
{
  boolean b_Sib8Valid;   /* SIB8 "cdma-EUTRA-Synchronization" info. 
                              TRUE: LTE NW is synchronized.
                              FALSE: LTE NW is NOT synchronized */

  uint64  t_Sib8CdmaSystemTime; /* The CDMA2000 system time corresponding to 
                           the SFN boundary at or after the ending 
                           boundary of the SI-Window in which this Time 
                           Transfer was taken. The size is 52 bits and 
                           the unit is [CDMA2000 chips] */

  boolean b_Sib16Valid; /* SIB16 System Time info*/

  uint64  t_Sib16GpsMsecs; /* The Full gps time at the SFN boundary in units of msecs */

  uint16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS time */

  uint16 w_Subfn;        /* Sub Frame Number of this WbCrs measurement */

  boolean b_OstmrVsrcValid; /* Mark if the next field is valid or not */
  loc_lte_tdp_WbCrsVsrcInfoType z_RefVsrcInfo[TDP_VSRC_NUM_CARRIERS];
  loc_lte_tdp_WbCrsVsrcInfoType z_SFNVsrcInfo[TDP_VSRC_NUM_CARRIERS];

//  loc_lte_tdp_RefInfoType  z_RefTimeInfo;
  cgps_RefInfoType  z_RefTimeInfo;
  uint32  q_UstmrFwDLSubFN;     /* Universal STMR value at DL SF according to FW 
                                (in 19.2MHz units) */
} loc_lte_tdp_WbCrsTimeInfoType;


typedef struct
{
  void *p_WbCrsMeasData;  /* Pointer to shared memory block storing WbCrs measurement */

  loc_lte_tdp_WbCrsMeasStatusType  e_MeasStatus; /* Measurement Status. Enum of type 'loc_lte_WbCrs_MeasStatusType' */

  loc_lte_tdp_WbCrsMeeasErrorCauseType  e_MeasErrorCause; /* Measurement Error Cause. Enum of type 'loc_lte_tdp_WbCrsMeeasErrorCauseType' */

  uint8 u_CarrierIdx;     /* 0=PCC, 1=SCC_0, and so on */

  loc_lte_tdp_WbCrsBandwidthType e_DlBandwidth; /* LTE DL bandwidth of the measurement. Enum of type 'loc_lte_tdp_WbCrsBandwidthType' */

  uint8 u_SignalPower;  /* Range (-140 dBm to -44 dBm) with 1 dBm 
                                                   resolution. */

  uint8  u_SignalQuality; /* Reference signal received quality.
                                                   For PRS measurements, the signalQuality will 
                                                   be RSRQ as defined in LPP. Range (-19.5 dB 
                                                   to -3dB with 0.5 dB resolution. The same 
                                                   values and encoding will be used for CRS 
                                                   measurements. */

  boolean w_RxTxTimeDiffValid; //--- only valid in connected mode- do we need this or just use u_lte_state below to determine validity?

  uint16 w_RxTxTimeDiff;   /* Rx-Tx time difference, as defined in LPP. 
                                                   Range (2 TS to 20472 TS)
                                                   The same values and encoding will be used 
                                                   for PRS and CRS measurements. */

  loc_lte_tdp_WbCrsTimeInfoType z_TimeInfo; /* LTE related timing parameters */

  uint8 u_lte_state; /*State of LTE at the time of the measurement*/

  loc_lte_tdp_WbCrsRunTimeType z_RunTime; /* ML1 Run-time information (USTMR dumps) */

} loc_lte_tdp_WbCrsMeasInfoType; 


/***********************************************
            SDL AD Response from ML1
************************************************/
typedef struct
{
  uint16 w_StartPhyId;

  uint16 w_EndPhyId;
}loc_lte_tdp_BlackListPhyIdRangeStructType;


typedef struct
{
  /* Num BlackList cells */
  uint8 u_NumRanges;

  /* PhyId of the blacklisted cells */
  loc_lte_tdp_BlackListPhyIdRangeStructType z_PhyIdRange[TDP_MAX_BLACKLIST_LEN];

} loc_lte_tdp_BlackListInfoStructType;


typedef enum
{
  LOC_LTE_TDP_ENB_NUM_TX_ANT_UNKNOWN = 0,
  LOC_LTE_TDP_ENB_NUM_TX_ANT_1 = 1,
  LOC_LTE_TDP_ENB_NUM_TX_ANT_2 = 2,
  LOC_LTE_TDP_ENB_NUM_TX_ANT_4 = 4
} loc_lte_tdp_eNB_NumTxAntVal; 

typedef uint8 loc_lte_tdp_eNB_NumTxAntType;

/*===========================================================================
  Searcher Detected Per Cell Info
===========================================================================*/
typedef struct
{
  /* PhyId of cell */
  uint16 w_PhyCellId;

  /* Cell System bandwidth */
  loc_lte_tdp_WbCrsBandwidthType e_CellBw;

  /* Cyclic prefix 0 - NCP , 1 - ECP */
  boolean v_CyclicPrefix;

  /* Sub frame boundary DL time in Ts */
  uint64 t_SfbDlTimeTs;

  /* Subframe offset for cell */
  uint8 u_SubframeOffset;

  /* Num Tx Antennas detected 0 - unknown */
  loc_lte_tdp_eNB_NumTxAntType u_NumTxAntennas;

  /* Range (-140 dBm to -44 dBm) with 1 dBm resolution. */
  uint8 u_Rsrp;

  /* Reference signal received quality. For PRS measurements, the signalQuality will 
  be RSRQ as defined in LPP. Range (-19.5 dB to -3dB with 0.5 dB resolution. The same 
  values and encoding will be used for CRS measurements. */
  uint8 u_Rsrq;

} loc_lte_tdp_Ml1DetectedCellStructType;

/*===========================================================================
  Searcher Detected Cell List structure per Earfcn
===========================================================================*/
typedef struct
{
   /* Number of cells per EARFCN */
   uint8  u_NumCells;

   /* EARFCN of all cells in below struct */
   uint32 q_Earfcn;

   /* Cells per EARFCN - first element is current serving cell on that EARFCN */
   loc_lte_tdp_Ml1DetectedCellStructType z_CellAd[MAX_SEARCHER_CELLS_PER_EARFCN];

   loc_lte_tdp_BlackListInfoStructType z_BlackListInfo;

} loc_lte_tdp_Ml1DetectedCellsPerEarfcnStructType;


/*--------------------------------------------------------------------------
 * WB CRS Function Definitions
 *--------------------------------------------------------------------------
*/

/*
 ******************************************************************************
 * Function: loc_lte_tdp_WbCrsMeasAvail
 *
 * Description:
 *  This API is called by ML1 to send TDP WB CRS meas to OC task
 *
 * Parameters: p_MeasResultsInfo - Pointer to WB Crs Meas
 *   
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_WbCrsMeasAvail(loc_lte_tdp_WbCrsMeasInfoType *pz_MeasResultsInfo);


/*
 ******************************************************************************
 * Function: loc_lte_tdp_Ml1DetectedCellsRsp
 *
 * Description:
 *  This API is called by ML1 to report detected cells to WWAN ME
 *
 * Parameters: p_Ml1DetectedCellsPerEarfcnStructType
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 ******************************************************************************
 */
void loc_lte_tdp_Ml1DetectedCellsRsp(loc_lte_tdp_Ml1DetectedCellsPerEarfcnStructType 
  *pz_SearcherListInfo);



/*--------------------------------------------------------------------------
 * OpCRS Function Definitions
 *--------------------------------------------------------------------------
*/

/*
 ******************************************************************************
 * Function: loc_lte_tdp_MeasResultsAvail
 *
 * Description:
 *  This API is called by ML1 to send TDP OpCRS meas to OC task
 *
 * Parameters: p_MeasResultsInfo - Pointer to OpCrs Meas
 *   
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_MeasResultsAvail(loc_lte_tdp_OpcrsMeasInfoType *pz_MeasResultsInfo);

#endif /* #ifndef LOC_LTE_TDP_API_H */