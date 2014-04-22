 /** @file gps_nw_common.h
    This file contains the structure definitions and function
    prototypes relevant for the GPS network awareness module 
    (essentially the shared structures). 
 */
#ifndef GPS_NW_COMMON_H
#define GPS_NW_COMMON_H

/*=============================================================================

                             GPS network awareness Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the GPS network awareness module (essentially the shared structures).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc. 
All Rights Reserved.

Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gps_nw_common.h#3 $ 
  $DateTime: 2020/09/14 03:12:12 $ 
  $Author: pwbldsvc $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/20   katta    Added new source info in to indicate wwan ranging source
02/26/20   katta    Pass Marb info via IPC 
10/6/2015  ah       Initial version

=============================================================================*/

#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#ifdef __cplusplus

extern "C"
{
#endif

#include "comdef.h"
#include "gps_types.h"
#include "loc_wwan_me_api.h"


/* Added to support changes to gps_NavSolution definition */

#include "cgps_api.h"
#include "sys.h"
#include "lte_as.h"

#ifdef FEATURE_CGPS_LTE_CELLDB
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_CGPS_LTE_CELLDB */

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Typedefs

=============================================================================*/


/* Enumeration of the Active Radio Access Type */
typedef enum
{
  CELLDB_ACTIVERAT_NONE,
  CELLDB_ACTIVERAT_CDMA,
  CELLDB_ACTIVERAT_WCDMA,
  CELLDB_ACTIVERAT_GSM,
  CELLDB_ACTIVERAT_LTE,
  CELLDB_ACTIVERAT_TDSCDMA,
  CELLDB_WIFI, /* WiFi cells for multiple location ID use, not as active RAT */
  CELLDB_ACTIVERAT_MAX
} cgps_ActiveRATEnumTypeVal;
typedef uint8 cgps_ActiveRATEnumType;

/* CDMA Local Cell information */
typedef struct
{
  uint16 w_SID; /* System ID */
  uint16 w_NID; /*Network ID*/
  uint16 w_BSID; /*Base Station ID*/

} cgps_CellInfoCDMAGlobalStruct;

#ifdef FEATURE_CGPS_LTE_CELLDB
typedef struct
{
  lte_rrc_plmn_s selected_plmn; /*selected PLMN to be filled*/
  lte_earfcn_t freq; /*DL Center frequency of cell */
  lte_phy_cell_id_t cell_id; /* Physical cell id of cell */
  lte_rrc_global_cell_id_t cell_identity; /* Global cell id (from SIB1) of cell */
  uint16 tac; /* Tracking area code of cell */

} cgps_CellInfoLTEGlobalStruct;

#endif /* FEATURE_CGPS_LTE_CELLDB */

/* TD-SCDMA Global Cell information struct */


/* Local Copy of RRC structure - rrc_plmn_identity_type */
typedef struct
{
  /* Mobile Country Code - one digit per byte range 0..9)  */
  byte mcc[3];
  /* Number of MNC digits */
  byte num_mnc_digits;
  /* Mobile Network Code - one digit per byte (range 0..9) */
  byte mnc[3];
} cgps_RrcPlmnIdentityType;

/* Local Copy of TDS structure - tdsrrcmeas_plmn_cell_info_struct_type */
typedef struct
{
  uint32 cell_id;
  uint8 cpid;
  uint16 freq;
  cgps_RrcPlmnIdentityType plmn_id;
  uint8 lac[2];
} cgps_CellInfoTDSCDMAGlobalStruct;

/*-*-*-* Cell DB Sources *-*-*-*/
typedef enum {
  CELLDB_UPDATE_SOURCE_UNKNOWN = 0,    /* unknown source */
  CELLDB_UPDATE_SOURCE_XTRA_T,         /* from XTRA-T injection */
  CELLDB_UPDATE_SOURCE_REAL_TIME,      /* from real-time operation */
  CELLDB_UPDATE_SOURCE_MAX
} cgps_CellDBUpdateSourceEnumVal;
typedef uint8 cgps_CellDBUpdateSourceEnum;

/* Typecast the CDMA Local information to be same as the Global. 
   CDMA Cell Entries are based only on SID
*/
typedef cgps_CellInfoCDMAGlobalStruct cgps_CellInfoCDMALocalStruct;

#ifdef FEATURE_CGPS_UMTS_CELLDB

/* GSM Local Cell information */
typedef struct
{
  uint8 bsic;    /* Base Station Identification code */
  ARFCN_T arfcn; /* Frequency Channel Number */

} cgps_CellInfoGSMLocalStruct;

/* WCDMA Local Cell information */
typedef struct
{
  uint16 psc;  /* Primary Scrambling code */
  uint16 freq; /* Frequency */

} cgps_CellInfoWCDMALocalStruct;

#endif /* FEATURE_CGPS_UMTS_CELLDB */

#ifdef FEATURE_CGPS_LTE_CELLDB
/* LTE Local Cell information */
typedef struct
{
  lte_earfcn_t  freq; /* DL frequency. */
  uint16  cell_id; /* Cell ID */

} cgps_CellInfoLTELocalStruct;

#endif /*FEATURE_CGPS_LTE_CELLDB*/

#ifdef FEATURE_CGPS_TDSCDMA_CELLDB
/* TDSCDMA Local Cell information */
typedef struct
{
  uint16  freq; /* DL frequency. */
  uint8  cpid; /* Cell ID */

} cgps_CellInfoTDSCDMALocalStruct;
#endif /*FEATURE_CGPS_TDSCDMA_CELLDB*/

/* Local Cell information struct. Union of CDMA/GSM/WCDMA */
typedef union
{
  cgps_CellInfoCDMALocalStruct z_CellInfoCDMALocal;
#ifdef FEATURE_CGPS_UMTS_CELLDB
  cgps_CellInfoGSMLocalStruct z_CellInfoGSMLocal;
  cgps_CellInfoWCDMALocalStruct z_CellInfoWCDMALocal;
#endif /* FEATURE_CGPS_UMTS_CELLDB */

#ifdef FEATURE_CGPS_LTE_CELLDB
  cgps_CellInfoLTELocalStruct z_CellInfoLTELocal;
#endif /*FEATURE_CGPS_LTE_CELLDB*/

#ifdef FEATURE_CGPS_TDSCDMA_CELLDB
  cgps_CellInfoTDSCDMALocalStruct z_CellInfoTDSCDMALocal;
#endif /*FEATURE_CGPS_TDSCDMA_CELLDB*/
} cgps_CellInfoLocalStruct;


/* Enumeration for the Source of Position Information.
If adding any further TLE pos with source PE, InputToInitEpiData() in ale_proc.c
should be updated to ignore these TLE positions from being used as EPI data in PE*/
typedef enum
{
  C_POS_INFO_SRC_INVALID,
  C_POS_INFO_SRC_PE,
  C_POS_INFO_SRC_EXTERNAL,
  C_POS_INFO_SRC_OTA,
  C_POS_INFO_SRC_PE_PROP_CELLDB,
  C_POS_INFO_SRC_EXTERNAL_PROP_CELLDB,
  C_POS_INFO_SRC_OTA_PROP_CELLDB,
  C_POS_INFO_SRC_PE_PROP_TIME,
  C_POS_INFO_SRC_EXTERNAL_PROP_TIME,
  C_POS_INFO_SRC_OTA_PROP_TIME,
  C_POS_INFO_SRC_COUNTRY_TBL_SID,
  C_POS_INFO_SRC_COUNTRY_TBL_MCC,
  C_POS_INFO_SRC_WHOLE_EARTH,
  C_POS_INFO_SRC_PE_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_EXTERNAL_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_OTA_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_PE_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_EXTERNAL_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_OTA_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_TIME_ZONE_TBL,
  C_POS_INFO_SRC_TIME_ZONE_TBL_PROP_CELLDB,
  C_POS_INFO_SRC_COUNTRY_TABLE_PROP_CELLDB,
  C_POS_INFO_SRC_XTA,
  C_POS_INFO_SRC_XTA_PROP_CELLDB,
  C_POS_INFO_SRC_XTA_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_XTA_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_XTA_ECID,
  C_POS_INFO_SRC_LAST_CONSISTENT_TLE_POS,
  C_POS_INFO_SRC_TLE_WWAN_RANGING,
  C_POS_INFO_SRC_MAX

} cgps_PosInfoSrcTypeVal;
typedef uint8 cgps_PosInfoSrcType;

/* Structure for the Cell DB reference Position */
typedef struct
{
#ifndef WIN32
  cgps_PosInfoSrcType e_PosInfoSrc; /* Source of the Position Estimate */
#else
  uint8 e_PosInfoSrc; /* Source of the Position Estimate */
#endif
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  boolean u_GpsTimeValid; /* GPS Time validity indicator for the Position */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  DBL     d_PosLLa[3];
  FLT     f_PosUnc;       /* Horizontal Uncertainty in meters (circular) */
  FLT     f_AltUnc;       /* Altitude Uncertainty in meters */

} cgps_CellDBPosInfoStruct;


typedef union {

  cgps_GsmCellTimingType z_GsmCell;
  cgps_WcdmaCellTimingType z_WcdmaCell;
  cgps_LteCellTimingType z_LteCell;
  cgps_TDScdmaCellTimingType z_TDScdmaCell;
} gnss_TimeTagCellInfoType;

/* Definitions for Time Tagging */
typedef enum {

  C_GNSS_TIMETAG_NULL,
  C_GNSS_TIMETAG_REQUESTED,
  C_GNSS_TIMETAG_TEST_REQUEST,
  C_GNSS_TIMETAG_CELL_CHANGE,
  C_GNSS_TIMETAG_COMPLETED

} gnss_TimeTagStateTypeVal;
typedef uint8 gnss_TimeTagStateType;

/**
  @brief GPS time data structure.  
*/
typedef struct
{
  uint16 w_GpsWeek;     /*!< GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /*!< GPS msec at reference tick [msecs] */
  FLT    f_ClkTimeBias; /*!< Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
  FLT    f_ClkTimeUncMs; /*!< 1 sided max time bias uncertainty [msecs] */
  uint8                 e_SourceOfTime; /*!< Source of the time */
} gps_TimeStructType;

typedef struct {

  /* TRUE if this data base is valid */
  boolean u_Valid;

  /* See enum above */
  gnss_TimeTagStateType e_TimeTagState;

  /* Time Inject request. If TRUE, inject time on completion of timetag */
  boolean u_TimeInjectReq;

  /* GPS time */
  boolean u_GpsTimeValid; /* Validity flag for GPS time */
  gps_TimeStructType z_GpsTime; /* Week, Msec, Bias and Unc */

  /* TRUE if the 32-bit FCount is valid */
  boolean u_FCount32Valid;

  /* 32-bit extended FCount value */
  uint32 q_FCount32;

  /* GPSRTC milliseconds from the TimeTag message. */
  uint32 q_GpsRtcMs;
  /* GPSRTC SubMs from the TimeTag message */
  uint32 q_SubMsCx80;

  /* USTMR count. Resolution of 19.2 MHz */
  uint32 q_UstmrCount;
  uint32 q_LpmCount;
  /* Active radio access technology. */
  /* 
     SYS_RAT_NONE,
     SYS_RAT_GSM_RADIO_ACCESS,
     SYS_RAT_UMTS_RADIO_ACCESS,
     SYS_RAT_LTE_RADIO_ACCESS,
     SYS_RAT_TDS_RADIO_ACCESS
  */
#ifndef FEATURE_GNSS_SA
  sys_radio_access_tech_e_type ActiveRAT;
#endif
  /* Cell Information */
  boolean u_CellInfoValid; /* Validity flag for Cell Info */
  gnss_TimeTagCellInfoType z_Cell;

  /* Time-taick count at which the timetag database is valid */
  uint64 t_TimeTickCount;

  /* Timetick count at which the last timetag was requested */
  uint64 t_LastReqestTimeTickCount;

} gnss_TimeTagStructType;

typedef struct {
  boolean status;
  sys_cell_search_list_s_type cell_list;
} cgps_CellScanListStructType;

typedef struct
{
  boolean          edrx_enabled;
  uint8            edrx_cycle_length;
}cgps_EdrxcycleInfoType;

typedef struct
{
  uint64          wwan_sleep_start;
  uint64          wwan_sleep_end;
}cgps_WwanStateInfo;

/*=============================================================================

       Functions

=============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* GPS_NW_COMMON_H */
