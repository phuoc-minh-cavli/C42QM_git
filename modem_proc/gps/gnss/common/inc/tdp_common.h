/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *  Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *----------------------------------------------------------------------------
 */
#ifndef TDP_COMMON_H
#define TDP_COMMON_H
/*=============================================================================

                             TDP Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the entire GPS module (essentially the shared structures).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/tdp_common.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-2-14   hs  First version. Added support for TDP feature.




=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "gnss_common.h"
#include "loc_wwan_me_api.h"

#ifdef __cplusplus
extern "C"
{
#endif 

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/
#define NUM_MAX_TDP_MEAS     (24)  /* Max # of TDP meas in a TDP Upload meas 
                                   block */
#define MAX_EARFCNS   (1)  /*  Max number of earfcns for TDP 2.0 */
#define MAX_WBCRS_WIN_UNC99_METERS     (2048*9.7588690f)    /* Max WB CRS WINDOW UNC
												                                      995 confidence */
#define WBCRS_MEAS_AMBIGUITY_METERS    (LIGHT_SEC*2048 / 3 / (2048*15000))   /* WB CRS measurement ambiguity */


#define TDP_NUM_AMBIG_PEAKS 7 /* Number of ambiguities reported for each cell by WWAN ME */

/*=============================================================================

       Typedefs

=============================================================================*/

/* Typedef struct for all the response/request from Searcher to PDSM.
*/


/*=============================================================================
   Structs used for TDP upload
=============================================================================*/
/* ELC Detection flag */
typedef enum
{
  /* 0=unknown, 1=no ELC, 2=ELC Type 0 ( 6RB only ), 3= ELC Type 1 ( Full BW Symbol 0 ), 4= Spare 1, 5=Spare 2, 6=Spare 3, , 7=spare 4 */
  TDP_ELC_UNKNOWN,
  TDP_ELC_NOT_DETECTED,
  TDP_ELC_ALL_6RB,
  TDP_ELC_FULLBW_SYMB0,
  TDP_SPARE1,
  TDP_SPARE2,
  TDP_SPARE3,
  TDP_SPARE4
} tdp_ElcDetectionEnumTypeVal;
typedef uint8 tdp_ElcDetectionEnumType;

typedef struct 
{
  struct 
  {
     uint8 b_IsMccValid : 1;
     uint8 b_IsMncValid : 1;
     uint8 b_IsTacValid : 1;    
     uint8 b_IsCellIdValid : 1;
  } z_CellIdMask;
	
  uint16 w_PhyId;  /* Physical Id 0-503 */
  uint32 q_Earfcn; /* Downlink center frequency - Integer (0..262143) */
  uint16 w_Mcc; /* Layer 1 region ID */
  uint16 w_Mnc; /* Layer 2 region ID */
  uint16 w_Tac; /* Layer 3 region ID */
  uint32 q_CellId; /* Layer 4 region ID */
  tdp_ElcDetectionEnumType e_ELCtype; /* TODO - WWAN ME needs to fill this correctly */
} tdp_LteCellIdStructType;

/* LTE supported bandwidths */
typedef enum
{
  /* 1.4MHz bandwidth */
  TDP_CRS_BW_6 = 0,
  /* 3MHz bandwidth */
  TDP_CRS_BW_15,
  /* 5MHz bandwidth */
  TDP_CRS_BW_25,
  /* 10MHz bandwidth */
  TDP_CRS_BW_50,
  /* 15MHz bandwidth */
  TDP_CRS_BW_75,
  /* 20MHz bandwidth */
  TDP_CRS_BW_100,
  /* Unknown Bandwidth */
  TDP_CRS_BW_UNKNOWN
} tdp_LteSystemBandwidthEnumTypeVal;
typedef uint8 tdp_LteSystemBandwidthEnumType;

/* LTE Antenna Ports */
typedef enum
{
  /* 0=unknown, 1=1Tx, 2=2Tx, 3=4Tx, 4=8Tx, 5=spare1, 6=spare2, 7=spare3 */
  TDP_LTE_ANT_CNT_UNKNOWN,
  TDP_LTE_ANT_CNT_1,
  TDP_LTE_ANT_CNT_2,
  TDP_LTE_ANT_CNT_4,
  TDP_LTE_ANT_CNT_8,
  TDP_LTE_ANT_CNT_SPARE1,
  TDP_LTE_ANT_CNT_SPARE2,
  TDP_LTE_ANT_CNT_SPARE3
} tdp_LteAntennaPortCntEnumTypeVal;
typedef uint8 tdp_LteAntennaPortCntEnumType;

typedef struct
{
  struct 
  {
    uint8 b_IsSib8Valid : 1;
    uint8 b_IsSib16Valid : 1;
  }z_ConfigMask;

  tdp_LteSystemBandwidthEnumType e_CellSystemBw; /* LTE DL bandwidth of 
                                                      the measurement */

  boolean b_CyclicPrefix; /* FALSE: normal
                          TRUE : extended */

  tdp_LteAntennaPortCntEnumType e_AntennaPorts; /* 0=unknown, 1=1Tx, 2=2Tx, 3=4Tx, 4=8Tx, 5=spare1, 6=spare2, 7=spare3 */

  uint64  t_Sib8CdmaSystemTime; /* The CDMA2000 system time corresponding to 
                                   the SFN boundary at or after the ending 
                                   boundary of the SI-Window in which this Time 
                                   Transfer was taken. The size is 52 bits and 
                                   the unit is [CDMA2000 chips] */

  uint64  t_Sib16FullGpsMsecs; /* The Full gps time at the SFN boundary in
                                   units of msecs */

  uint16 w_LeapSeconds;         /* UTC leap second */
  

} tdp_ServingCellConfigType;


typedef enum
{
  TDM_MEASTYPE_PRS = 0,
  TDP_MEASTYPE_OPCRS,
  TDP_MEASTYPE_UKCRS 
} tdp_RangingMeasEnumTypeVal;
typedef uint8 tdp_RangingMeasEnumType;


typedef struct 
{
   struct 
   {
     uint8 b_IsSignalPowerValid : 1;
     uint8 b_IsSignalQualityValid : 1;
     uint8 b_IsRxTxTimeDiffValid : 1;
     uint8 b_IsTotMsValid : 1;
     uint8 b_IsServing : 1;
   } z_MeasMask;

   uint8 u_SignalPower; /* Range (-140 dBm to -44 dBm) with 1 dBm 
                           resolution. Refer to GTP the ICD. */

   uint8 u_SignalQuality; /* Reference signal received quality.
                           For PRS measurements, the signalQuality will 
                           be RSRQ as defined in LPP. Range (-19.5 dB 
                           to -3dB with 0.5 dB resolution. The same 
                           values and encoding will be used for CRS 
                           measurements. Refer to GTP the ICD.*/

   uint16 w_RxTxTimeDiff; /* Rx-Tx time difference, as defined in 
                            LPP. Range (2 TS to 20472 TS). The same 
                            values and encoding will be used for PRS 
                            and CRS measurements. Refer to GTP the ICD.*/

   tdp_RangingMeasEnumType e_RangingMeasType; /* PRS, CRS, UKCRS */

   tdp_LteSystemBandwidthEnumType e_RangingMeasBW; /* LTE DL BW */

   boolean b_FineClockSyncFlag; /* TRUE: LTE-GNSS Time sync applied */
 
   uint16 w_TotMs; /* Offset of the observed signal’s time of 
                   transmission from start of LTE frame structure in 
                   units of ms. Value is calculated as: (System Frame 
                   Number) * 10 +  sub_frame_offset_within_frame.
                   Required to be present for the serving cell.                
                   Optionally present for neighboring cells. When
                   present for neighboring cells, the value shall apply 
                   to the neighbor’s */

   int16 x_rToaMs; /* TOA measurements are relative to the GPS time in
                    the position estimate for this lteTdpUploadData
                    instance. Ms component of rToa Range (~ -32.7 sec
                    to 32.7 sec ) */   

   int32 l_rToaNs; /* TOA measurements are relative to the GPS time in 
                    the position estimate for this lteTdpUploadData                
                    instance. Ns component of rToa Range (-0.5 ms to 
                    0.5 ms) */

   uint8 u_rToaMeasUncUnPropK; /* K-value encoded measurement uncertainty, 
                              computed based on signal strength, multipath, 
                              etc.*/

   uint8 u_rToaClockUncPropK; /* K-value encoded clock Uncertainty. If the 
                             fineClockSyncFlag is set to 1, then this  
                             field represents the clock uncertainty wrt 
                             the GPS time of fix. Else, this represents 
                             the relative clock uncertainty growth from 
                             the first measurement of the block. */

} tdp_UploadMeasStructType;

typedef struct
{
  tdp_LteCellIdStructType z_CellId;

  tdp_UploadMeasStructType z_Meas;

} tdp_MeasAndCellDataStructType;

typedef struct 
{
  tdp_ServingCellConfigType z_ServingCellConfig; /* Serving cell cfg */

  uint8 u_NumTdpMeas;/* Number of measurements in the scanlist below */

  tdp_MeasAndCellDataStructType z_MeasAndCellData[NUM_MAX_TDP_MEAS]; 
                                                      /* Max 24 meas */

} tdp_MeasAndCellDataBlkStructType;

typedef enum
{
  TDP_SCAN_METHOD_ONE = 0,
  TDP_SCAN_METHOD_MAX = 0xFFFF
}tdp_ScanSelectionMethodEnumType;

typedef struct
{
  boolean b_Valid;         /* TRUE, if the configuration is valid */
  boolean b_UploadControl; /* 0: Disable TDP upload for subsequent transactions 
                              1: Enable TDP upload for subsequent transactions 
                                (default)*/

  uint16 w_MaxNumScanListPerCell; /* Maximum number scanlists 
                                    (instances of lteTdpUploadData) that may 
                                    be uploaded per serving cell record*/

  uint16 w_GridSizeDeg;  /* Recommended spacing between scanlist collection 
                            points in the unit of 0.00001 degree of latitude
                            and longitude.*/

  uint32 q_MinScanIntervalUploadSec; /* Recommended minimum time between scanlists
                                        for upload in seconds*/

  uint16 w_MaxPosUncUpload; /* Maximum position uncertainty for scanlists for 
                               uploads in meters */

  uint8 u_ScanSelectionParameter; /* A parameter for a cost function calculation,
                                     used for selection among collected scans 
                                     within a grid segment.*/

  tdp_ScanSelectionMethodEnumType e_ScanSelectionMethod; 

  boolean b_UploadGNSSOnly; /* TRUE, only accept GNSS-only position for CS */
} tdp_UploadControlConfigType;


/*=============================================================================
                      TDP DOWNLOAD STRUCTURES
==============================================================================*/

/*=============================================================================
   TDP Clock Structure
=============================================================================*/
typedef  struct
{
   /* Boolean to indicate System Rtc is valid */
   boolean     b_SystemRtcValid;

   /* System Rtc */
   uint64      t_SystemRtcMs;  

   /* GPS Time  */
   gps_TimeStructType z_GpsTime; 

} tdp_ClockStructType;


/*=============================================================================
   TDP Base Station Location
=============================================================================*/
typedef struct
{
  /* ECEF Co-ordinates of the Base Station */
  DBL d_BsEcef[3];

  /* Base Station Hepe */
  FLT f_BsHorUnc63Meters;

  /* Base Station 1 sigma Alt Unc */
  FLT f_BsAltUnc68Meters;

  /* Forward Link Correction of the Base station in meters */
  FLT f_FlcMeters; 

  /* Forward Link Correction Uncertainty in meters */
  FLT f_FlcUnc68Meters; 
  
  /* Base station location horizontal reliability */
  pdsm_pd_reliability_e_type e_HzReliability;

  /* Base station location vertical reliability */
  pdsm_pd_reliability_e_type e_VertReliability;

}tdp_BsLocStruct;


/*=============================================================================
   TDP Base station Structure that includes the BS location struct and 
   the Lte Cell Info
=============================================================================*/
typedef struct
{
   /* LTE cell id */
   tdp_LteCellIdStructType z_LteCellId;

   /* BS Location data */
   tdp_BsLocStruct z_BsLocStruct;

}tdp_BsStructType;


/*=============================================================================
   TDP TOA Struct
=============================================================================*/
typedef struct
{
  /* Time Difference Of Arrival between this cell and the serving cell at this
     Epoch. In nanosecond units */
  int32    l_TdoaNsec;

  /* Odds for the ambiguous TOA Peak in dB scale */
  uint8    u_Odds;

} tdp_TdoaStructType;


/*=============================================================================
   TDP LTE Measurement Structure
=============================================================================*/
typedef struct
{
  /* TOA of the Ambiguous peaks in decreasing order of odds */
  tdp_TdoaStructType z_AmbigPeakTdoa[TDP_NUM_AMBIG_PEAKS];

  /* Cell Measurement Latency from t_TimeTickMsec */
  uint16    w_TdoaLatencyMsec;

  /* Meas 1 sigma uncertainity in meters */
  uint16 w_MeasUnc1SigmaMeters;

  /* SNR of max peak in db */
  uint8  u_MaxPeakSnrdB;

  /* Physical cell ID */
  uint16 w_PhyCellId;
  
} tdp_LteMeasType;


/*=============================================================================
   TDP Measurement structure includes the LTE meas and BS data
=============================================================================*/
typedef struct
{
  tdp_LteMeasType z_LteMeas;
  tdp_BsStructType z_BsStruct;
} tdp_MeasStructType;


/*=============================================================================
   TDP Measurement Block sent to NF (for positioning)
=============================================================================*/
typedef struct
{
  /*! TimeTickMsec that comes from ME 
       in tdp_WwanMeMeasBlkStructType */
  uint64 t_TimeTickMsec; 

  /*! Number of Cell measurements being reported */
  uint8 u_NumOfCellMeas; 

  /* Terrain Altitude */
  FLT f_TerrainAltMeters;

  /* Terrain Alt Uncertainty */
  FLT f_TerrainAltUnc68Meters;


  /* TDP Meas Array */
  tdp_MeasStructType z_TdpMeas[N_ACTIVE_TDP_CHAN];

} tdp_MeasBlkStructType;


/*===========================================================================
   TDP LTE Positioning Measurements Block Structure sent by WWAN ME
===========================================================================*/
typedef struct
{
  /*! USTMR Time tick msec of the serving cell sub-frame boundary in 
       the latest measurement occasion. This is needed to
       associate gps time to the TDP fix for comparison 
       with truth in post processing for error computation */  
  uint64 t_TimeTickMsec;

  /*! Number of Cell measurements being reported */
  uint8 u_NumOfCellMeas;

  /* TDP Meas Structure */
  tdp_LteMeasType       z_Meas[NUM_MAX_TDP_MEAS];
} tdp_WwanMePosMeasBlkStructType;

#ifdef __cplusplus
}
#endif

#endif /* TDP_COMMON_H */