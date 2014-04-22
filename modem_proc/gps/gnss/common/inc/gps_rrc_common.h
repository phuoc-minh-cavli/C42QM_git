 /** @file gps_rrc_common.h
    This file contains the structure definitions and function
    prototypes relevant to the interface between RRC end GPS 
    engine 
 */
#ifndef GPS_RRC_COMMON_H
#define GPS_RRC_COMMON_H

/*=============================================================================

                             GPS RRC interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the GPS network awareness module (essentially the shared structures).
  The functions in this header can not be accessed by C++ callers

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gps_rrc_common.h#1 $ 
  $DateTime: 2020/07/21 21:23:44 $ 
  $Author: pwbldsvc $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/2015  ah      Initial version
=============================================================================*/

#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gps_nw_common.h"
#include "locEng_sm_common_msg_v01.h"

#if defined(FEATURE_CGPS_UMTS_CELLDB) || defined(FEATURE_FSM_V2)
#include "rr_lsm_if.h"
#include "rrclsmif.h"
#endif /* FEATURE_CGPS_UMTS_CELLDB */


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/



/*=============================================================================

       Macros

=============================================================================*/

/* Define the maximum number elements in the Cell DB */
#ifndef FEATURE_CGPS_XTRA_T
#define C_CELLDB_MAX_CELL_INDEX 256
#else
#define C_CELLDB_MAX_CELL_INDEX 20
#endif

/*=============================================================================

       Typedefs

=============================================================================*/


#if defined(FEATURE_CGPS_UMTS_CELLDB) || defined(FEATURE_FSM_V2)
/* GSM Global Cell information struct */
typedef current_bs_info_T cgps_CellInfoGSMGlobalStruct;
/* WCDMA Global Cell information struct */
typedef rrcmeas_plmn_cell_info_struct_type cgps_CellInfoWCDMAGlobalStruct;
/* TDSCDMA Global Cell information struct: use rrcmeas_plmn_cell_info_struct_type because TDS team uses it in API */
typedef rrcmeas_plmn_cell_info_struct_type cgps_CellInfoTDSGlobalStruct;

#endif /* FEATURE_CGPS_UMTS_CELLDB */


/* Global Cell information struct. Union of CDMA/GSM/WCDMA */
typedef union
{
  cgps_CellInfoCDMAGlobalStruct z_CellInfoCDMAGlobal;
  #if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2)
  cgps_CellInfoGSMGlobalStruct z_CellInfoGSMGlobal;
  cgps_CellInfoWCDMAGlobalStruct z_CellInfoWCDMAGlobal;
  cgps_CellInfoTDSGlobalStruct z_CellInfoTDSGlobal;
  #endif /* FEATURE_CGPS_UMTS_CELLDB */

  #ifdef FEATURE_CGPS_LTE_CELLDB
  cgps_CellInfoLTEGlobalStruct z_CellInfoLTEGlobal;
  #endif /* FEATURE_CGPS_LTE_CELLDB */

  cgps_CellInfoTDSCDMAGlobalStruct z_CellInfoTDSCDMAGlobal;

  locEngWifiApInfoStructT_v01 z_wifi_ap_cell_info;

} cgps_CellInfoGlobalStruct;

/* Cached CellInformation Structure */
typedef struct
{
  boolean u_CachedCellIDValid;
  cgps_ActiveRATEnumType e_ActiveRAT;
  uint32 q_NumCellChangesCachedCellID;
  cgps_CellInfoGlobalStruct z_CellInfoGlobalCached;
} cgps_CellInfoCachedStruct;


/* Structure for the Cell DB element */
typedef struct 
{
  /* Validity indicator for the Cell Entry */
  boolean                    u_CellValid;         
  /* Validity indicator for the Global Cell Info */
  boolean                    u_CellGlobalInfoValid;
  /* Validity indicator for the GPS Time reference for Cell Entry */
  boolean                    u_GpsTimeValid;      
  /* Position Conflict indicator. TRUE is position estimate
     does not agree with the Country table
  */
  boolean                    u_PosConflict;
  /* Active Radio Access technology */
  cgps_ActiveRATEnumType     e_ActiveRAT;         
  /* Default Cell Radius in meters */
  FLT                        f_CellRadius;        
  /* Cell Information Local area */
  cgps_CellInfoLocalStruct     z_CellInfoLocal;     
  /* Cell Information Global area (includes globabl unique identifiers) */
  cgps_CellInfoGlobalStruct  z_CellInfoGlobal;    
  /* Position information associated with the cell */
  cgps_CellDBPosInfoStruct    z_CellDBPos;         

  /* Time-Tick count */
  uint64                     t_TimeTickCount;        
  /* GPS Week when Cell info is updated */
  uint16                     w_GpsWeek;           
  /* GPS Msec when Cell info is updated */
  uint32                     q_GpsMsec;           
  /* Bit mask of all neighbors of the cell */
  uint32                     q_NeighborList[(C_CELLDB_MAX_CELL_INDEX+31)>>5]; 

  #ifdef FEATURE_CGPS_XTRA_T
  /* CellDB entry usage status masks */
  uint8                      u_UsageStatus;
  /* Source of this CellDB entry */
  cgps_CellDBUpdateSourceEnum  e_UpdateSource;
  #endif /* FEATURE_CGPS_XTRA_T */

} cgps_CellDBType;

/* Packed version of CDMA Region Identifier */
typedef PACKED struct PACKED_POST
{
  uint16 w_SID;
} cgps_SndbPackedCDMARegionType;

#ifdef FEATURE_CGPS_UMTS_CELLDB

/* Packed version of WCDMA Region Identifier */
typedef PACKED struct PACKED_POST
{
  /* Mobile Country Code - one digit per byte range 0..9)  */
  uint8 u_Mcc[RRC_MAX_MCC_DIGITS];
  /* Number of MNC digits */
  uint8 u_Num_Mnc_Digits;
  /* Mobile Network Code - one digit per byte (range 0..9) */
  uint8 u_Mnc[RRC_MAX_MNC_DIGITS];

  uint8 b_Lac[RRC_MAX_GSMMAP_LAC_LENGTH];
} cgps_SndbPackedWCDMARegionType;

/* Packed version of GSM Region Identifier */
typedef PACKED struct PACKED_POST
{
  uint8 u_PlmnId[3];
  uint16 w_Lac;
} cgps_SndbPackedGSMRegionType;

/* Packed version of UMTS Region Identifier */
typedef PACKED union PACKED_POST
{
  cgps_SndbPackedGSMRegionType z_GsmRegionFormat;
  cgps_SndbPackedWCDMARegionType z_WcdmaRegionFormat;
} cgps_SndbPackedUMTSRegionType;
#endif /* FEATURE_CGPS_UMTS_CELLDB */

/* Packed version of Sparse Network DB Region Identifier */
typedef PACKED struct PACKED_POST
{
  uint8 u_ActiveRAT;                            /* The Cellular technology */

  PACKED union PACKED_POST
  {
    cgps_SndbPackedCDMARegionType z_CdmaID;
#ifdef FEATURE_CGPS_UMTS_CELLDB
    cgps_SndbPackedUMTSRegionType z_UmtsID;
#endif /* FEATURE_CGPS_UMTS_CELLDB */
  } z_Rat;
} cgps_SndbPackedRegionInfoType;


/* If this is updated, mgp_CellDBPosInfoStruct_ForDmLog also needs to be updated */
typedef PACKED struct PACKED_POST
{
  uint8 e_PosInfoSrc; /* Source of the Position Estimate */
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  boolean u_GpsTimeValid; /* GPS Time validity indicator for the Position */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  int64   r_PosLLa[3];
  int32   l_PosUnc;       /* Horizontal Uncertainty in meters (circular) */
  int32   l_AltUnc;       /* Altitude Uncertainty in meters */
} cgps_CellDBPackedPosInfoStruct;

/* Packed version of Time stamp */
typedef PACKED struct PACKED_POST
{
  uint64 t_TimeTick;
  PACKED struct PACKED_POST
  {
    uint16 w_Week;
    uint32 q_MSec;
  } z_Gps;
} cgps_SndbPackedTimeStampType;

/* Packed version of SNDB Record */
typedef PACKED struct PACKED_POST
{
  cgps_SndbPackedRegionInfoType z_Region;
  cgps_CellDBPackedPosInfoStruct z_Pos;
  cgps_SndbPackedTimeStampType z_TimeStamp;
  uint32 q_RegionSize;
} cgps_SndbInjectedRecordType;


/*=============================================================================

       Functions

=============================================================================*/


#endif /* GPS_RRC_COMMON_H */
