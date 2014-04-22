/******************************************************************************
  @file:  loc_pd.c
  @brief: Location Middleware PA module

  DESCRIPTION
  This module configures/retrieves the NV setting of location
  engine on the modem, and processes EFS data.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  Copyright (c) 2013-2014 QUALCOMM Atheros Incorporated.
  All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015-2020 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
08/11/20   sai     Added support for deletion of cell DB postion support from QMI
05/21/20   katta   Handing additional parameters passed from QMI get wwan pos 
10/01/20   ska     Changes done for MGP state to OFF when TBF>=5sec
08/21/19   ska     Added support for NMEA GSV
06/10/19   py      If WWAN is unavailable sent out GENERAL ERROR as status
04/15/19   nath     Added debug NMEA related code.
03/20/19   sum      Corrected the session status while reporting best available position.
10/30/17   nath     Updated data types of loc_unc_horizontal,alongAxisUnc and perpAxisUnc to float
06/12/17   kpn      Correction in F3 for DOP info
05/04/17   yh       Debug NMEA
04/27/17   sum      Populating Horz speed in QMI packet with new velHorU
03/31/17   yh       LocMW clean up round 2
03/08/17   muk      free proper encoded memory block in locPd_EncodeGetAvailableGnssPos
03/08/17   yh       set (mo_)fix_in_progress flag for concurrent E911 feature
02/17/17   skm      BLE and Barometer support for LPPE Phase 2 
12/09/16   yh       Fix for over boundary memory reading in memescpy
11/16/16   yh       Lppe DBH feature implementation
11/15/16   yh       Remove big local varialbe in locPd_SendQmiIndicationProxy
10/27/16   yh       wifiApInfo_len = 0 is a valid input
10/25/16   yh       add sv_mask print out
10/25/16   yh       Do not force session end when there is an active NI session
10/21/16   yh       Add GAL to calculate position
10/06/16   yh       QMI_LOC_SECURE_GET_AVAILABLE_POSITION_REQ_V02 clean up
09/14/16   yh       LocMW only end MO sessions
07/28/16   yh       altitude_wrt_ellipsoid converting fix
07/15/16   yh       heading_unc rad to degree converting fix
06/08/16   yh       Lppe
05/19/16   yh       LocMW refactoring
04/26/16   yh       Single shot fix criteria bug fix
04/13/16   yh       Always update GPS engine state
03/23/16   yh       multiple client position report
10/06/15   yh       QZSS
09/03/15   yh       Use the fatest TBF for pdsm_lite_get_position option
08/26/15   pa       Modifying leap second to 17 and removing redundant constants
06/11/15   sjk      LB 2.0 Integration
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
03/10/14   ah       Added support for Get Avail WWAN Pos
03/14/13   ssu      Support for SUPL MT ES and improved SI rules.
04/11/11   rh       Fixed race condition issue when calling pdsm_lite_client_init()
03/29/10   ns       Changed Loc MW to be an OEM type client

$Id: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sm_interface.c#2 $
$DateTime: 2020/09/14 11:42:17 $
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"
#include "queue.h"
#include "time_svc.h"

#include "math.h"

#include "aries_os_api.h"
#include "pdapi_lite.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_client.h"
#include "loc_sm_interface.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "gnss_calendar_util.h"
#include "loc_conv_locEng_qmiLoc.h"
#include "loc_qmi_shim.h"
#include "gnss_consts.h"
#include "secapi.h"
#include "location_service_iot_v02.h"
#include "loc_utils.h"

/********************************loc_pd macros-start********************/
#define PD_DEFAULT_WAIT_TIMEOUT  120000 // 2 minutes

// Number of seconds between January 1, 1970 and January 6, 1980
#define GPS_TO_UTC_DELTA_SECONDS        (3657 * 24 * 60 * 60)
#define SECONDS_IN_ONE_DAY   (24 * 60 * 60)

#define LAT_LON_SCALE_FACTOR     100000000 // (See 80-VF767-1)

//#define RAD2DEG                  (180.0 / 3.1415926535898 )
#define UNCERTAINTY_TABLE_SIZE   30
#define INV_SQRT_2               0.7071067811865

/* offset (in meters) that should be subtracted from altitude sent
   in the pdsm position report to calculate the altitude w.r.t
   WGS-84 ellipsoid  */
#define LOC_PD_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET (500.0F)

/* This value has been changed to inline with the default ON value*/
#define SESSION_END_THRESHOLD                (5*1000)  /* 5 seconds */

#define LOC_PD_SECURE_DEBUG   0
/********************************loc_pd macros-end********************/

/********************************loc_pa macros-start********************/

//loc_pa macros
#define LOC_PA_OPER_DEFAULT_TIMEOUT  (4000)

/* The Max Valid value of SUPL CertID that can be accepted
 * Valid Values of SUPL CertID is [0..9]
 */
/*
  #9x05: SUPL not supported
*/

#define LOC_PA_ALLOC_PAINFO(pointer) \
   (pointer) = (pdsm_lite_pa_info_type *)loc_calloc(sizeof(*(pointer))); \
   if(NULL == (pointer)) \
   { \
      LOC_MSG_ERROR("pz_PaInfo loc_call returns NULL",0,0,0); \
      return LOC_API_GENERAL_FAILURE; \
   }

#define LOC_PA_FREE_PAINFO(pz_PaInfo) \
   if(NULL != (pz_PaInfo)) \
   { \
      loc_free(pz_PaInfo); \
   }
/********************************loc_pa macros-end********************/

/********************************loc_pd array & structs-start********************/

// Ten by Ten Degree WGS-84 Geoid Separation from -180 to +170 Degrees of Longitude in meters (approximate)
// Source:  Defense Mapping Agency. 12 Jan 1987. GPS UE Relevant WGS-84 Data Base Package.
//                Washington, DC: Defense Mapping Agency
// Note:    Geodal Separation(GS) is the height of MSL above the ellipsoid.
//                Thus, MSL = HAE - GS


// Table used to compute Geoidal separation
int const GeoSep[19][36] =
{
    //    90 Degrees N Latitude
    { 13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13 } ,
    //    80 Degrees N Latitude
    { 3,1,-2,-3,-3,-3,-1,3,1,5,9,11,19,27,31,34,33,34,33,34,28,23,17,13,9,4,4,1,-2,-2,0,2,3,2,1,1 },
    //    70 Degrees N Latitude
    { 2,2,1,-1,-3,-7,-14,-24,-27,-25,-19,3,24,37,47,60,61,58,51,43,29,20,12,5,-2,-10,-14,-12,-10,-14,-12,-6,-2,3,6,4 },
    //    60 Degrees N Latitude
    {2,9,17,10,13,1,-14,-30,-39,-46,-42,-21,6,29,49,65,60,57,47,41,21,18,14,7,-3,-22,-29,-32,-32,-26,-15,-2,13,17,19,6},
    //    50 Degrees N Latitude
    {-8,8,8,1,-11,-19,-16,-18,-22,-35,-40,-26,-12,24,45,63,62,59,47,48,42,28,12,-10,-19,-33,-43,-42,-43,-29,-2,17,23,22,6,2},
    //    40 Degrees N Latitude
    {-12,-10,-13,-20,-31,-34,-21,-16,-26,-34,-33,-35,-26,2,33,59,52,51,52,48,35,40,33,-9,-28,-39,-48,-59,-50,-28,3,23,37,18,-1,-11},
    //    30 Degrees N Latitude
    {-7,-5,-8,-15,-28,-40,-42,-29,-22,-26,-32,-51,-40,-17,17,31,34,44,36,28,29,17,12,-20,-15,-40,-33,-34,-34,-28,7,29,43,20,4,-6},
    //    20 Degrees N Latitude
    {5,10,7,-7,-23,-39,-47,-34,-9,-10,-20,-45,-48,-32,-9,17,25,31,31,26,15,6,1,-29,-44,-61,-67,-59,-36,-11,21,39,49,39,22,10},
    //    10 Degrees N Latitude
    {13,12,11,2,-11,-28,-38,-29,-10,3,1,-11,-41,-42,-16,3,17,33,22,23,2,-3,-7,-36,-59,-90,-95,-63,-24,12,53,60,58,46,36,26},
    //    0 Degrees Latitude
    {22,16,17,13,1,-12,-23,-20,-14,-3,14,10,-15,-27,-18,3,12,20,18,12,-13,-9,-28,-49,-62,-89,-102,-63,-9,33,58,73,74,63,50,32},
    //    10 Degrees S Latitude
    {36,22,11,6,-1,-8,-10,-8,-11,-9,1,32,4,-18,-13,-9,4,14,12,13,-2,-14,-25,-32,-38,-60,-75,-63,-26,0,35,52,68,76,64,52},
    //    20 Degrees S Latitude
    {51,27,10,0,-9,-11,-5,-2,-3,-1,9,35,20,-5,-6,-5,0,13,17,23,21,8,-9,-10,-11,-20,-40,-47,-45,-25,5,23,45,58,57,63},
    //    30 Degrees S Latitude
    {46,22,5,-2,-8,-13,-10,-7,-4,1,9,32,16,4,-8,4,12,15,22,27,34,29,14,15,15,7,-9,-25,-37,-39,-23,-14,15,33,34,45},
    //    40 Degrees S Latitude
    {21,6,1,-7,-12,-12,-12,-10,-7,-1,8,23,15,-2,-6,6,21,24,18,26,31,33,39,41,30,24,13,-2,-20,-32,-33,-27,-14,-2,5,20},
    //    50 Degrees S Latitude
    {-15,-18,-18,-16,-17,-15,-10,-10,-8,-2,6,14,13,3,3,10,20,27,25,26,34,39,45,45,38,39,28,13,-1,-15,-22,-22,-18,-15,-14,-10},
    //    60 Degrees S Latitude
    {-45,-43,-37,-32,-30,-26,-23,-22,-16,-10,-2,10,20,20,21,24,22,17,16,19,25,30,35,35,33,30,27,10,-2,-14,-23,-30,-33,-29,-35,-43},
    //    70 Degrees S Latitude
    {-61,-60,-61,-55,-49,-44,-38,-31,-25,-16,-6,1,4,5,4,2,6,12,16,16,17,21,20,26,26,22,16,10,-1,-16,-29,-36,-46,-55,-54,-59},
    //    80 Degrees S Latitude
    {-53,-54,-55,-52,-48,-42,-38,-38,-29,-26,-26,-24,-23,-21,-19,-16,-12,-8,-4,-1,1,4,4,6,5,4,2,-6,-15,-24,-33,-40,-48,-50,-53,-52},
    //    90 Degrees S Latitude
    {-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30}
};

/* The following structure defines the a "cookie"
   whose address can be passed to any loc_pd commands,
   the cookie will be echoed back when the corresponding command callback
   is called from PDAPI. The information may be used to identify the
   client handle, ioctl and any other command specific information for
   the PDAPI command.
   Note: The cookie may not be echoed back in the event callback, it should
   not be relied upon from the event callback */

typedef struct locPd_CmdCbType
{
  loc_client_handle_type  l_ClientHandle; /* LocMW client handle */
  pdsm_lite_pd_cmd_e_type      e_CmdType;      /* PDSM PD Command Type */
  uint32                  q_QmiLocIndId;  /* QMI_LOC Indication MsgId */
  struct locPd_CmdCbType* p_Self;         /* Self referential pointer to check validity */
} locPd_CmdCbData;



/* Table that maps the PDAPI position source to QMI LOC technology mask */



/* struct that carries the client handle and QMI indication to be sent */
 

/********************************loc_pd array & structs-end*******************************/


/********************************loc_pa array, structs & globals-start********************/

/* All SUPL certificates will be injected in /SUPL/ folder, and will be named
 * Cert0, Cert1, .. Cert9. The common prefix for all Certificates is defined here
 *
 * Note: this prefix must be kept in sync with the definition in tm_common.h
 */




/* -----------------------------------------------------------------------*//**
@brief
   Mapping of masks from QMI-Loc Delete Masks to PDAPI Delete Masks. 
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32        q_QmiMask;
  uint32        q_PdapiMask;
}locPa_MaskMapType;


/* Mapping of masks from QMI_LOC Assistance Data to PDAPI Assistance Data */

static const locPa_MaskMapType z_QmiPdsmClockInfoMaskMap[] =
{
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST_V02,          PDSM_LITE_PA_DELETE_CLOCK_INFO_TIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST_V02,          PDSM_LITE_PA_DELETE_CLOCK_INFO_FREQ_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_WEEK_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME_V02,          PDSM_LITE_PA_DELETE_CLOCK_INFO_RTC_TIME },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER_V02,     PDSM_LITE_PA_DELETE_CLOCK_INFO_TIME_TRANSFER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_GPSTIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_GLOTIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER_V02,     PDSM_LITE_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER_V02,   PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY_V02,  PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT_V02,        PDSM_LITE_PA_DELETE_CLOCK_INFO_DISABLE_TT },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_LEAPSEC_V02,        PDSM_LITE_PA_DELETE_CLOCK_INFO_GG_LEAPSEC },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_GGTB_V02,           PDSM_LITE_PA_DELETE_CLOCK_INFO_GG_GGTB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSTIME_EST_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_BDSTIME_EST },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GB_GBTB_V02,           PDSM_LITE_PA_DELETE_CLOCK_INFO_GB_GBTB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BG_BGTB_V02,           PDSM_LITE_PA_DELETE_CLOCK_INFO_BG_BGTB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSWEEK_NUMBER_V02,    PDSM_LITE_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY_V02,  PDSM_LITE_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY },
/*  { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTIME_EST_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_ }, PDSM mask not defined yet */
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGPS_TB_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_GPS_GAL_TB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOGLO_TB_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO_GAL_TB },
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALTOBDS_TB_V02,       PDSM_LITE_PA_DELETE_CLOCK_INFO_GAL_BDS_TB },
/*  { QMI_LOC_MASK_DELETE_CLOCK_INFO_GALWEEK_NUMBER_V02,    PDSM_LITE_PA_DELETE_CLOCK_INFO_ }, PDSM mask not defined yet 
    { QMI_LOC_MASK_DELETE_CLOCK_INFO_GAL_RF_GRP_DELAY_V02,  PDSM_LITE_PA_DELETE_CLOCK_INFO_ }, PDSM mask not defined yet */
};

static const locPa_MaskMapType z_QmiPdsmCellDBMaskMap[] =
{
  { QMI_LOC_MASK_DELETE_CELLDB_POS_V02,                PDSM_LITE_PA_DELETE_CELLDB_MASK_POS_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02,     PDSM_LITE_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02,            PDSM_LITE_PA_DELETE_CELLDB_MASK_OTA_POS_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02,        PDSM_LITE_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02,            PDSM_LITE_PA_DELETE_CELLDB_MASK_TIMETAG_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02,             PDSM_LITE_PA_DELETE_CELLDB_MASK_CELLID_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02,      PDSM_LITE_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02,      PDSM_LITE_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02,       PDSM_LITE_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO },
  { QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02,      PDSM_LITE_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO },
};


static const locPa_MaskMapType z_QmiPdsmCommonMaskMap[] =
{
    { QMI_LOC_DELETE_COMMON_MASK_POS_V02,             PDSM_LITE_PA_DELETE_POS },
    { QMI_LOC_DELETE_COMMON_MASK_TIME_V02,            PDSM_LITE_PA_DELETE_TIME },
    { QMI_LOC_DELETE_COMMON_MASK_UTC_V02,             PDSM_LITE_PA_DELETE_UTC },
    { QMI_LOC_DELETE_COMMON_MASK_RTI_V02,             PDSM_LITE_PA_DELETE_RTI },
    { QMI_LOC_DELETE_COMMON_MASK_FREQ_BIAS_EST_V02,   PDSM_LITE_PA_DELETE_FREQ_BIAS_EST },
};


/********************************loc_pa array, structs & globals-end*********************/


/****************************loc_pd function declarations-start****************************/

extern void sm_GetGpsUtcOffset(uint8 * p_UtcOffset);

// TBD: Skip the intermediate report between intervals when the last fix is good
/* Event callback called from PDAPI for a PD command.
   The implementation will rely on the UserData pointer to identify the client handle etc,
   and will lock the critical section. */
static void locPd_EventCb
(
  void*                      p_UserData,
  pdsm_lite_pd_event_type         q_PdEvent,
  const pdsm_lite_pd_info_s_type* p_PdInfo
);

/* Command callback called from PDAPI for a PD command.
   The implementation will rely on the DataBlock pointer to identify the client handle etc,
   and will lock the critical section. */
static void locPd_CmdCb
(
  void*                  p_DataBlock,
  pdsm_lite_pd_cmd_e_type     e_CmdType,
  pdsm_lite_pd_cmd_err_e_type e_CmdErrStatus
);

/* Command callback called from PDAPI for a PD command.
   The implementation will rely on the DataBlock pointer to identify the client handle etc,
   and will not lock the critical section. */
static void locPd_CmdCbUnlocked
(
  void*                  p_DataBlock,
  pdsm_lite_pd_cmd_e_type     e_CmdType,
  pdsm_lite_pd_cmd_err_e_type e_CmdErrStatus
);



static boolean locPd_ReportPosition (void);
static boolean locPd_ReportSVInfo(void);
static void locPd_Schedule (void);
extern boolean locClnt_SVInfoCallback(loc_client_handle_type                 l_ClientHandle,
   qmiLocEventRegMaskT_v02                  t_EventType,
   const void*                            p_EventPayload);


/* Function to process external coarse position injection request. */
static int locPd_InjectExtPos
(
  loc_client_handle_type                  l_ClientHandle,
  const qmiLocInjectPositionReqMsgT_v02*  pz_PosReq
);

static qmiLocSessionStatusEnumT_v02 locPd_XlateFixEndReasonCode
(
   pdsm_lite_pd_end_e_type  e_PdSessEndStatus
);

static qmiLocSessionStatusEnumT_v02 locPd_XlateCmdErrCodeToSessStatus
(
   pdsm_lite_pd_cmd_err_e_type  e_PdCmdErr
);

/* Function to translate PD cmd error into session status in the LOC API position report. */
static uint32 locPd_ConvertCmdErr2LocStatus
(
  pdsm_lite_pd_cmd_err_e_type e_PdCmdErr
);

float locPd_GetGeoidalSeparation
(
      double dbLatitude,
      double dbLongitude
);

/* Function to process the best available position get request. */
static int locPd_GetBestAvailPos
(
  loc_client_handle_type                           l_ClientHandle,
  const qmiLocGetBestAvailablePositionReqMsgT_v02* pz_BestAvailPosReq
);

/* Function to process the available WWAN position get request. */
static int locPd_GetAvailWwanPos
(
  loc_client_handle_type                       l_ClientHandle,
  const qmiLocGetAvailWwanPositionReqMsgT_v02* pz_AvailWwanPosReq
);

/* Inject GSM cellular information into the engine */
static int locPd_InjectGsmCellInfo
( 
  loc_client_handle_type                    l_ClientHandle,
  const qmiLocInjectGSMCellInfoReqMsgT_v02* pz_GsmCellInfoReq
);

/* Inject NI Message into the engine */






/****************************loc_pd function declarations-end****************************/


/****************************loc_pa function declarations-start****************************/
static void locPa_EventCb
(
  void*                      p_UserData, /* user data pointer */
  pdsm_lite_pa_event_type         q_PaEvent,  /* Indicate PA event */
  const pdsm_lite_pa_info_s_type* pz_PaInfo    /* PARAM pz_PaInfo POINTER */
);

static void locPa_EventCbExt
(
  void*                      p_UserData,   /* user data pointer */
  pdsm_lite_pa_e_type             e_ParamType,  /* Indicate param type (enum PDSM_LITE_PA_xxx) */
  const pdsm_lite_pa_info_s_type* pz_PaInfo,     /* PARAM pz_PaInfo POINTER */
  pdsm_lite_pa_cmd_err_e_type     e_PaCmdStatus /* Optional field to indicate success/failure status of PA operation */
);

static void locPa_CmdCb
(
  void*                  p_DataBlock,
  pdsm_lite_pa_cmd_e_type     e_PaCmdType,
  pdsm_lite_pa_cmd_err_e_type e_PaCmdStatus
);

void locPa_ResetModule(void);

static uint32 locPa_SetParam(loc_client_handle_type l_ClientHandle, pdsm_lite_pa_e_type e_ParamType, pdsm_lite_pa_info_type* pz_PaInfo);
static uint32 locPa_GetParam(loc_client_handle_type l_ClientHandle, pdsm_lite_pa_e_type e_ParamType);
#if 0 /*Commenting for now as we do not need it now but may require it for QWES*/
static uint32 locPa_SetEngineLock(loc_client_handle_type l_ClientHandle, qmiLocLockEnumT_v02 e_EngineLock);
#endif
/*#9x05: NMEA not supported*/

static uint32 locPa_SetPeConfig(loc_client_handle_type l_ClientHandle, const qmiLocSetPositionEngineConfigParametersReqMsgT_v02* p_SetPeCfg);
static uint32 locPa_SetXtratSessionCtrl(loc_client_handle_type l_ClientHandle, uint8 v_Control);
static uint32 locPa_SetXtraVersionCheck(loc_client_handle_type l_ClientHandle, qmiLocXtraVersionCheckEnumT_v02 e_XtraVersion);
static uint32 locPa_DeleteGnssServiceData(loc_client_handle_type l_ClientHandle, qmiLocDeleteGNSSServiceDataReqMsgT_v02* p_DeleteGnssServiceData);


static qmiLocStatusEnumT_v02 locPa_ConvertCmdError2Qmi(pdsm_lite_pa_cmd_err_e_type pa_cmd_err);

static boolean locPa_SendQmiIndicationProxy
(
  loc_client_handle_type       l_ClientHandle,
  const pdsm_lite_pa_info_s_type*   pz_ParamInfo,
  pdsm_lite_pa_cmd_err_e_type       e_ParamStatus
);


/****************************loc_pd function declarations-end******************************/

/****************************loc_pd function definition-start******************************/

/* Initialize the location position determination module */
boolean locPd_Init()
{
   boolean  ret_val = FALSE;
   pdsm_lite_client_status_e_type status = PDSM_LITE_CLIENT_OK;
   pdsm_lite_pd_event_type pd_event_mask = PDSM_LITE_PD_EVENT_MASK;
   locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);
   uint8 i=0;

   do
   {
      pz_LocPdData->fix_in_progress     = FALSE;
      pz_LocPdData->mo_fix_in_progress  = FALSE;
      pz_LocPdData->z_InjPos.v_InProgress          = FALSE;
      pz_LocPdData->z_GetBestAvailPos.v_InProgress = FALSE; 
      pz_LocPdData->z_GetAvailWwanPos.v_InProgress = FALSE;
      pz_LocPdData->last_get_position_time = 0;
      pz_LocPdData->last_fix_arrival_time  = 0;
      pz_LocPdData->last_fix_ttf           = 0;

      pz_LocPdData->new_pd_info_received            = FALSE;
      pz_LocPdData->new_ext_status_info_received    = FALSE;
      pz_LocPdData->new_interm_pos_info_received    = FALSE;
      pz_LocPdData->new_pd_sess_end_status_received = FALSE;
      pz_LocPdData->new_pd_cmd_err_received         = FALSE;
      pz_LocPdData->session_time_out_ms             = 0;
      pz_LocPdData->v_any_victimized_client         = FALSE;
      // Initialization (cannot be omitted)
      pz_LocPdData->pd_event_cb_hook                = NULL;

      memset (&(pz_LocPdData->pdsm_lite_option), 0, sizeof (pdsm_lite_pd_option_s_type));
      memset (&(pz_LocPdData->pdsm_lite_qos), 0, sizeof (pdsm_lite_pd_qos_type));
      memset (&(pz_LocPdData->pd_info), 0, sizeof (pdsm_lite_pd_info_s_type));
      memset (&(pz_LocPdData->pd_info_copy), 0, sizeof (pdsm_lite_pd_info_s_type));
      pz_LocPdData->l_ActiveClientHandle             = LOC_CLIENT_HANDLE_INVALID;
      pz_LocPdData->z_InjPos.l_ClientHandle          = LOC_CLIENT_HANDLE_INVALID;
      pz_LocPdData->z_GetBestAvailPos.l_ClientHandle = LOC_CLIENT_HANDLE_INVALID;
      pz_LocPdData->z_GetAvailWwanPos.l_ClientHandle = LOC_CLIENT_HANDLE_INVALID;
      pz_LocPdData->engine_state = LOC_ENGINE_STATE_OFF;

      /* the fix timer should be non deferrable, i.e it should always wakeup the
         engine from suspended state */ 

      pz_LocPdData->fix_process_timer = 
         os_TimerCreateInCgpsTimerGroup( (uint32) LOC_MIDDLEWARE_TIMER_ID_FIX,
                                         (uint32) THREAD_ID_LOC_MIDDLEWARE );

      if (loc_middleware_data.pdapi_client_id == -1)
      {
        loc_middleware_data.pdapi_client_id = pdsm_lite_client_init(PDSM_LITE_CLIENT_TYPE_PDA);
      }

      if (loc_middleware_data.pdapi_client_id == -1)
      {
         LOC_MSG_ERROR ( "locPd_Init: pdsm_lite_client_init failed", 0, 0, 0);
         break;
      }

      //pd_event_mask |= PDSM_LITE_PD_EVENT_WPS_NEEDED;

      status = pdsm_lite_client_pd_reg(loc_middleware_data.pdapi_client_id,
                                    NULL,
                                    locPd_EventCb,
                                    PDSM_LITE_CLIENT_EVENT_REG,
                                    pd_event_mask,
                                    NULL);
      if(status != PDSM_LITE_CLIENT_OK)
      {
         LOC_MSG_ERROR("locPd_Init: pdsm_lite_client_pd_reg failed, error code=%d", status, 0, 0);
      }

      

      status = pdsm_lite_client_act(loc_middleware_data.pdapi_client_id);
      if(status != PDSM_LITE_CLIENT_OK)
      {
         LOC_MSG_ERROR ("locPd_Init: pdsm_lite_client_act failed, error code =%d", status, 0, 0);
         break;
      }

      ret_val = TRUE;
   } while (0);

   return ret_val;
}

pdsm_lite_client_status_e_type loc_pd_install_event_cb_hook(pdsm_lite_pd_event_f_type *hook)
{
   pdsm_lite_client_status_e_type e_clientStatus = PDSM_LITE_CLIENT_OK;
   if (loc_middleware_data.z_locPd_Data.pd_event_cb_hook == NULL)
   {
      loc_middleware_data.z_locPd_Data.pd_event_cb_hook = hook;
   }
   else
   {
      e_clientStatus = PDSM_LITE_CLIENT_ERR_CLIENT_ACTIVE;
   }
   return e_clientStatus;
}

/* Initialize the location NMEA sentence module */

/*#9x05: NMEA not supported*/


// This function is used to process cmd request from location task.
int locPd_ProcessCmdRequest (const LocMW_LocCmdStructT* pz_Cmd)
{
   int ret_val = LOC_API_GENERAL_FAILURE;


   if(NULL != pz_Cmd)
   {
       LocMW_LocCmdEnumT cmd_type = pz_Cmd->e_CmdType;

       if (cmd_type == LOC_CMD_TYPE_PD_SCHEDULE)
       {
          loc_pd_process_fix_request (eLOC_PD_SCHEDULE);
          ret_val = LOC_API_SUCCESS;
       }
       else if (cmd_type == LOC_CMD_TYPE_REPORT_POSITION)
       {
          // Notify the requesting client of any new position data
          (void) locPd_ReportPosition();
          ret_val = LOC_API_SUCCESS;
       }
	   else if (cmd_type == LOC_CMD_TYPE_REPORT_SV_INFO)
	   {
	     (void) locPd_ReportSVInfo();
		 ret_val = LOC_API_SUCCESS;
	   }
       else
       {
           // internal error
           ret_val = LOC_API_UNSUPPORTED;
       }
   }
   else
   {
       // internal error
       ret_val = LOC_API_GENERAL_FAILURE;
   }

   if(LOC_API_SUCCESS != ret_val)
   {
       LOC_MSG_ERROR ("locPd_ProcessCmdRequest internal error", 0, 0, 0);
   }

   return ret_val;
}

// Function used to do next fix request. This function is responsible for multiplexing
// between multiple apps, set up fix config and issue fix request based on the requesting client.
// The scheduling algorithm is the min (client last fix requst time + fix internal)
int loc_pd_process_fix_request (loc_pd_fix_request_e_type fix_request_type)
{
   int32                     time_to_wait_ms = INT32_MAX;
   int32                     time_delta_ms;
   boolean                   status;

   // initialize variable to avoid warning on calling time_get_uptime_ms
   time_type                 time_ms = {0};

   uint64                    current_time_ms;
   locQmiShimCltInfoStructT*   client_ptr = NULL;
   boolean                   is_any_client_requesting_fix = FALSE;
   boolean                   is_stop_client_match_active = FALSE;
   boolean                   non_mo_fix_in_progress = FALSE;
   boolean                   call_end_session = FALSE;

   //end_session_type
   pdsm_lite_pd_end_session_opt_s_type end_session_opt ;

   locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);

   LOC_MSG_HIGH ("loc_pd_process_fix_request: request_type = %d, fix_in_progress = %d mo_fix_in_progress = %d\n",
                fix_request_type,
                pz_LocPdData->fix_in_progress,
                pz_LocPdData->mo_fix_in_progress);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   /* Process a stop request from a MO client*/

   if (eLOC_PD_STOP_REQUEST == fix_request_type)
   {
     /* get wait time for next fix to be queued */
     time_to_wait_ms = locClnt_GetMinWaitTime(&client_ptr);

     /* check if any other active state client in queue */
     is_any_client_requesting_fix = locClnt_IsAnyRequestingFix();

     /* check if client calling stop is same as active client that scheduled ongoing fix */
     if (loc_middleware_data.z_locClnt_Data.l_StopClientHandle == locClnt_GetActiveClient())
     {
       is_stop_client_match_active = TRUE;
     }

     /* check if any non-mo fix is in progress */
     non_mo_fix_in_progress = (TRUE == pz_LocPdData->fix_in_progress &&
                               FALSE == pz_LocPdData->mo_fix_in_progress);

     LOC_MSG_MED("loc_pd_process_fix_request(STOP): next fix wait time %d "
                 "is_any_client_requesting_fix %d is_stop_client_match_active %d \n",
                 (int32)time_to_wait_ms, is_any_client_requesting_fix, is_stop_client_match_active);

     /* Try to End the session
        if - there is no non-mo-fix-in-progress (!fix_in_progress || mo_fix_in_progress)
              if - there is no other ready state client
              if - there is no fix scheduled within 10sec threshold time
                   &&
                   client handle calling stop is same as active client
     */
     if (FALSE == non_mo_fix_in_progress)
     {
       if (FALSE == is_any_client_requesting_fix)
       {
         LOC_MSG_MED("loc_pd_process_fix_request: Processing stop when"
                     " is_any_client_requesting_fix: %d", is_any_client_requesting_fix, 0, 0);

         call_end_session = TRUE;
       }
       else if ((time_to_wait_ms > SESSION_END_THRESHOLD) && (TRUE == is_stop_client_match_active))
       {
         LOC_MSG_MED("loc_pd_process_fix_request: Processing stop when wait time > THRESHOLD and"
                     " is_stop_client_match_active = %d \n", is_stop_client_match_active, 0, 0);

         call_end_session = TRUE;
       }
       else
       {
         call_end_session = FALSE;
         LOC_MSG_MED("Not stopping the fix, is_any_client_requesting_fix %d,"
                     " wait time > THRESHOLD = %d, "
                     "is_stop_client_match_active = %d\n",
                     is_any_client_requesting_fix,
                     time_to_wait_ms > SESSION_END_THRESHOLD,
                     is_stop_client_match_active);
       }

       if (TRUE == call_end_session)
       {
         LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

         end_session_opt.receiver_off = TRUE;
		 
         pdsm_lite_end_session(locPd_CmdCb,
                             NULL,
                              &end_session_opt,
                             loc_middleware_data.pdapi_client_id);

         LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
         os_TimerStop( pz_LocPdData->fix_process_timer );

         pz_LocPdData->mo_fix_in_progress = FALSE;
       }
      }
      else
      {
         /* non-mo-session is in progress (NI session, GF, etc) */
         LOC_MSG_MED("loc_process_fix_request: Not stopping the engine as "
                     "an NI session (or non-mo) is active", 0, 0, 0);
      }
   }
   /* check if fix timed out without getting a position fix */
   else if ( eLOC_PD_FIX_TIMEOUT == fix_request_type &&
             TRUE == pz_LocPdData->mo_fix_in_progress)
   {
      os_TimerStop( pz_LocPdData->fix_process_timer );
      // this state is hit because the MO positioning session timed out
      time_get_uptime_ms (time_ms);
      current_time_ms = QW_CVT_Q2N(time_ms);

      time_delta_ms = (int) (current_time_ms - pz_LocPdData->last_get_position_time);

      time_to_wait_ms =  pz_LocPdData->session_time_out_ms - time_delta_ms;

      if (time_to_wait_ms < 0)
      {
         LOC_MSG_ERROR ("Missing pd cmd err or pd done event\n", 0, 0, 0);

         // Inject timeout error to the app
         pz_LocPdData->pd_sess_end_status = PDSM_LITE_PD_END_TIMEOUT;
         pz_LocPdData->new_pd_sess_end_status_received = TRUE;

         // send error report, as an empty location report, to client
         (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                    LOC_CMD_TYPE_REPORT_POSITION,
                                    0,
                                    NULL);

         // Force state reset
         locPd_Schedule();
         LOC_MSG_MED("loc_process_fix_request: Timeout scheduled a position"
                     "report with timeout status, also re-scheduled "
                     "fix processing", 0,0,0);
         time_to_wait_ms = INT32_MAX; // Wait until client state changed
      }
   }
   else if (FALSE == pz_LocPdData->fix_in_progress)
   {
      os_TimerStop( pz_LocPdData->fix_process_timer );
     // In case of back to back periodic fixes.
     // When the second request for periodic fix comes in , make sure we dont have an ongoing fix in progress.
     // If we have an ongoing fix in progress wait till the fix is done , in which case we get a session_done and
     // fix_in_progress is reset to FALSE. And then the scheduler decides which fix request to process next, in
     // which case the start fix request would be honoured.
     if ( (eLOC_PD_START_REQUEST == fix_request_type) ||
          (((eLOC_PD_SCHEDULE == fix_request_type) || (eLOC_PD_OPPORTUNISTIC_SCHEDULE == fix_request_type)) &&
           (TRUE == locClnt_IsAnyRequestingFix())) ||
           (eLOC_PD_FIX_TIMEOUT == fix_request_type)
         )
     {
       /* Make a new position request if:
          a) A new start request was received
          b) A schedule request was received and there is atleast
             one MO client active
          c) The fix timer expired and no fix is in progress */

       // Let the GPS client manager picks the gps configuration to be used.
       // It will also figure out when pdsm_lite_get_postion will be called.
       time_to_wait_ms = locClnt_SetupRequestConfig (eLOC_PD_OPPORTUNISTIC_SCHEDULE == fix_request_type);
       if (time_to_wait_ms <= 0)
       {
         pz_LocPdData->mo_fix_in_progress = TRUE;
         pz_LocPdData->v_any_victimized_client = FALSE;
         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         status = pdsm_lite_get_position(locPd_CmdCb,
                                     NULL, /* not used */
                                     &(pz_LocPdData->pdsm_lite_option),
                                     &(pz_LocPdData->pdsm_lite_qos),
                                     loc_middleware_data.pdapi_client_id);
         LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         if (status == TRUE)
         {
           time_to_wait_ms = PD_DEFAULT_WAIT_TIMEOUT; // wait until position report has received
                                                   // or client state changes
           if (time_to_wait_ms < (pz_LocPdData->pdsm_lite_qos.gps_session_timeout*1000 + 1000))
           {
             time_to_wait_ms = pz_LocPdData->pdsm_lite_qos.gps_session_timeout*1000 + 1000;
           }
           // The loc mw will wait for the engine to time out, add a padding of 1 second
           pz_LocPdData->session_time_out_ms = time_to_wait_ms;
         }
         else
         {
           LOC_MSG_ERROR ("pdsm_lite_get_postion returned %d \n, queuing an empty"
                          "position report and rescheduled fix processing ",
                          status, 0, 0);
           // send error report, as an empty location report, to client
           (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                     LOC_CMD_TYPE_REPORT_POSITION,
                                     0,
                                     NULL);
           pz_LocPdData->mo_fix_in_progress = FALSE;
           locPd_Schedule();
           time_to_wait_ms = INT32_MAX;
            }
         }// end of if (time_to_wait <= 0)
     }// end of !((eLOC_PD_START_REQUEST == fix_request_type)&&(TRUE == pz_LocPdData->fix_in_progress))
     else
     {
       LOC_MSG_MED ("Not making a new position request though no fix in progress",
                     0, 0, 0);
     }
   }// end of (FALSE == pz_LocPdData->fix_in_progress)
   /* This state may be hit if NI fix is in progress and timer expires */
   else
   {
     LOC_MSG_HIGH ("loc_pd_process_fix_request : unknown state "
                   "fix_request_type = %d, fix in progress = %d,"
                   "mo fix in progress = %d", fix_request_type,
                   pz_LocPdData->fix_in_progress,
                   pz_LocPdData->mo_fix_in_progress);

     time_to_wait_ms = INT32_MAX;
   }

   LOC_MSG_HIGH ("loc_module::time_to_wait_ms = %d \n", time_to_wait_ms, 0, 0);

   if ((time_to_wait_ms > 0) && (time_to_wait_ms != INT32_MAX))
   {
      os_TimerStart( pz_LocPdData->fix_process_timer, time_to_wait_ms, 0 );
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   return 1;
}

static boolean locPd_ReportSVInfo (void)
{
   loc_client_handle_type                l_ActiveClientHandle;
   qmiLocEventGnssSvInfoIndMsgT_v02*     pz_SVInfoReport = NULL;
   
   l_ActiveClientHandle = locClnt_GetActiveClient ();
   pz_SVInfoReport = &(loc_middleware_data.z_locSV_Data);
      
   if (l_ActiveClientHandle != LOC_CLIENT_HANDLE_INVALID) 
   {
    (void) locClnt_SVInfoCallback (l_ActiveClientHandle,
                                         QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02,
                                         pz_SVInfoReport);  
   }
   return TRUE;
}



// Notify client regarding position and sv information.
static boolean locPd_ReportPosition (void)
{
   loc_client_handle_type                l_ActiveClientHandle;

   qmiLocEventPositionReportIndMsgT_v02* pz_PositionReportIndMsg = NULL;
   locPd_ModuleDataType*                 pz_LocPdData = NULL;
   
  boolean v_ProcessPdSessEndStatus = FALSE;
  boolean v_ProcessPdCmdErr = FALSE;
   pz_LocPdData = &(loc_middleware_data.z_locPd_Data);

   // the payloads will be memset in locPd_GetReport
    // process the error event
   if (TRUE == pz_LocPdData->new_pd_sess_end_status_received)
   {
      v_ProcessPdSessEndStatus = TRUE;
      pz_LocPdData->pd_sess_end_status_copy = pz_LocPdData->pd_sess_end_status;
      pz_LocPdData->new_pd_sess_end_status_received = FALSE;
   }

   if (TRUE == pz_LocPdData->new_pd_cmd_err_received)
   {
      v_ProcessPdCmdErr                  = TRUE;
      pz_LocPdData->pd_cmd_err_copy = pz_LocPdData->pd_cmd_err;
      pz_LocPdData->new_pd_cmd_err_received = FALSE;
   }
   if (TRUE == v_ProcessPdSessEndStatus)
   {
      // avoid translation to windows mobile generall error code
      pz_LocPdData->pd_info.pd_info.pz_PositionReportIndMsg.sessionStatus = locPd_XlateFixEndReasonCode (pz_LocPdData->pd_sess_end_status_copy);
   }
   else if (TRUE == v_ProcessPdCmdErr)
   {
      // avoid translation to windows mobile generall error code
      pz_LocPdData->pd_info.pd_info.pz_PositionReportIndMsg.sessionStatus = locPd_XlateCmdErrCodeToSessStatus (pz_LocPdData->pd_cmd_err_copy);
   }

   l_ActiveClientHandle = locClnt_GetActiveClient ();
   pz_PositionReportIndMsg = &(pz_LocPdData->pd_info.pd_info.pz_PositionReportIndMsg);
   // Set the gnss report first (the one being populated from final position report), thent the final position report, so the client can

    if ((l_ActiveClientHandle != LOC_CLIENT_HANDLE_INVALID) && (pz_PositionReportIndMsg->sessionStatus >= 0))
    {
       (void) locClnt_PositionCallback (l_ActiveClientHandle,
                                         QMI_LOC_EVENT_MASK_POSITION_REPORT_V02,
                                         &(pz_LocPdData->pd_info.pd_info.pz_PositionReportIndMsg));

       // update the state for single-shot client
       if (pz_PositionReportIndMsg->sessionStatus != eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02)
       {
          (void) locClnt_UpdateStateAfterFix ();
       }
     }
   
   return TRUE;
}



// Fill up leapSeconds and gpsTime as well

#if 0 // Todo: Need to decide how we handle SV reports
static void locPd_dumpSvIn
(
   pdsm_lite_pd_ext_meas_s_type*   pz_PdExtMeas
)
{
   if(NULL != pz_PdExtMeas)
   {
      uint32 q_in;
      LOC_MSG_MED ("locPd_dumpSvIn: --------------------------------", 0, 0, 0);
      LOC_MSG_MED ("locPd_dumpSvIn: eph_svmask: %d, alm_svmask: %d, health_svmask: %d, ", pz_PdExtMeas->eph_svmask ,pz_PdExtMeas->alm_svmask, pz_PdExtMeas->health_svmask );

      for(q_in = 0; 
         (q_in < pz_PdExtMeas->num_svs) && (q_in < PDSM_LITE_EXT_MEAS_NUM_SVS);
         q_in++)
      {
         const pdsm_lite_pd_meas_s_type* pz_PdSvIn = &(pz_PdExtMeas->ext_meas_report_type[q_in]);
         LOC_MSG_MED ("locPd_dumpSvIn: sv_id: %d, elevation: %d, azimuth: %d", pz_PdSvIn->sv_id, pz_PdSvIn->elevation, pz_PdSvIn->azimuth);
         LOC_MSG_MED ("locPd_dumpSvIn: c_no: %d, sv_state:%d ", pz_PdSvIn->c_no, pz_PdSvIn->sv_state, 0);
      }
   }
}
#endif 
// Fill in the SV report from measurement report
#if 0  //Todo:For now we are commenting out this function but we need to check 
       // how we can handle SV report in position report
static void locPd_GetSvReport
(
   qmiLocEventGnssSvInfoIndMsgT_v02*     pz_GnssSvInfoIndMsg
)
{
   locPd_ModuleDataType*      pz_LocPdData = &(loc_middleware_data.z_locPd_Data);
   pdsm_lite_pd_ext_meas_s_type*   pz_PdExtMeas = NULL;
   uint32                     q_out, q_in;

   if(NULL == pz_GnssSvInfoIndMsg)
   {
      LOC_MSG_ERROR("locPd_GetSvReport, NULL pz_GnssSvInfoIndMsg",0,0,0);
      return;
   }

   pz_PdExtMeas = &(pz_LocPdData->ext_status_info_copy.ext_status_info.ext_meas_report_type);

   memset(pz_GnssSvInfoIndMsg, 0, sizeof(*pz_GnssSvInfoIndMsg));
   pz_GnssSvInfoIndMsg->svList_len = 0;

   for(q_out = q_in = 0; 
      (q_in < pz_PdExtMeas->num_svs) && (q_out < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02) && (q_in < PDSM_LITE_EXT_MEAS_NUM_SVS);
      q_in++)
   {
      const pdsm_lite_pd_meas_s_type*       pz_PdSvIn = &(pz_PdExtMeas->ext_meas_report_type[q_in]);
      gnss_sv_id_type                  w_SvId = pz_PdSvIn->sv_id;
      qmiLocSvInfoStructT_v02*         pz_QmiSvOut = &(pz_GnssSvInfoIndMsg->svList[q_out]);

      pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02;
      pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;

      if(w_SvId >= PDSM_LITE_PD_SV_ID_GPS_MIN && w_SvId <= PDSM_LITE_PD_SV_ID_GPS_MAX) // 1~32
      {
         pz_QmiSvOut->system = eQMI_LOC_SV_SYSTEM_GPS_V02;
         pz_QmiSvOut->gnssSvId = w_SvId;

         pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
         pz_QmiSvOut->svInfoMask = 0;
         if ((pz_PdExtMeas->eph_svmask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_GPS_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }
         if ((pz_PdExtMeas->alm_svmask  & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_GPS_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }

         /* a not 0 health_svmask means svmask bit is set */
         if( pz_PdExtMeas->health_svmask )
         {
           pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS_V02;
           pz_QmiSvOut->healthStatus = (uint8)((pz_PdExtMeas->health_svmask >> (w_SvId - PDSM_LITE_PD_SV_ID_GPS_MIN)) & 1);
         }
      }
      else if (w_SvId >= PDSM_LITE_PD_SV_ID_SBAS_MIN && w_SvId <= PDSM_LITE_PD_SV_ID_SBAS_MAX) // 33~64
      {
         // SBAS: GPS RPN: 120-151,
         // In exteneded measurement report, we follow nmea standard, which is from 33-64.
         pz_QmiSvOut->gnssSvId = w_SvId + (120-PDSM_LITE_PD_SV_ID_SBAS_MIN); // convert from 33~64 to 120~151
         pz_QmiSvOut->system = eQMI_LOC_SV_SYSTEM_SBAS_V02;
         pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;

#if (PDSM_LITE_API_MAJ_V > 7) || ((PDSM_LITE_API_MAJ_V == 7) && (PDSM_LITE_API_MIN_V >= 4))
         if ((pz_PdExtMeas->sbas_eph_mask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_SBAS_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }
         else
         {
            pz_QmiSvOut->svInfoMask &= ~QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }

         if ((pz_PdExtMeas->sbas_alm_mask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_SBAS_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }
         else
         {
            pz_QmiSvOut->svInfoMask &= ~QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }
#endif // PDAPI >= 7.4
      }
      else if (w_SvId >= PDSM_LITE_PD_SV_ID_GLO_MIN && w_SvId <= PDSM_LITE_PD_SV_ID_GLO_MAX) // 65~96
      {
         // Gloness: Slot id: 1-32
         // In extended measurement report, we follow nmea standard, which is 65-96
         pz_QmiSvOut->gnssSvId = w_SvId + (1-PDSM_LITE_PD_SV_ID_GLO_MIN);
         pz_QmiSvOut->system = eQMI_LOC_SV_SYSTEM_GLONASS_V02;

#if (PDSM_LITE_API_MAJ_V > 7) || ((PDSM_LITE_API_MAJ_V == 7) && (PDSM_LITE_API_MIN_V >= 4))
         pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
         pz_QmiSvOut->svInfoMask = 0;

         if ((pz_PdExtMeas->glo_eph_svmask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_GLO_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }

         if ((pz_PdExtMeas->glo_alm_svmask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_GLO_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }
#endif // PDAPI >= 7.4
      }
      else if (w_SvId >= PDSM_LITE_PD_SV_ID_BDS_MIN && w_SvId <= PDSM_LITE_PD_SV_ID_BDS_MAX) // 201~237
      {
         pz_QmiSvOut->gnssSvId = w_SvId;
         pz_QmiSvOut->system = eQMI_LOC_SV_SYSTEM_BDS_V02;

#if (PDSM_LITE_API_MAJ_V > 7) || ((PDSM_LITE_API_MAJ_V == 7) && (PDSM_LITE_API_MIN_V >= 30))
         pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
         pz_QmiSvOut->svInfoMask = 0;

         /* 1ULL becuase BDS svmask is uint64 */
         if ((pz_PdExtMeas->bds_eph_svmask & (1ULL << (w_SvId - PDSM_LITE_PD_SV_ID_BDS_MIN)) ) != 0ULL)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }

         if ((pz_PdExtMeas->bds_alm_svmask & (1ULL << (w_SvId - PDSM_LITE_PD_SV_ID_BDS_MIN)) ) != 0ULL)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }
#endif // PDAPI >= 7.30
      }
      else if (w_SvId >= PDSM_LITE_PD_SV_ID_GAL_MIN && w_SvId <= PDSM_LITE_PD_SV_ID_GAL_MAX) // 301~336
      {
         pz_QmiSvOut->gnssSvId = w_SvId;
         pz_QmiSvOut->system = eQMI_LOC_SV_SYSTEM_GALILEO_V02;

#if (PDSM_LITE_API_MAJ_V >= 8) 
         pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
         pz_QmiSvOut->svInfoMask = 0;

         /* 1ULL becuase GAL svmask is uint64 */
         if ((pz_PdExtMeas->gal_eph_svmask & (1ULL << (w_SvId - PDSM_LITE_PD_SV_ID_GAL_MIN)) ) != 0ULL)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }

         if ((pz_PdExtMeas->gal_alm_svmask & (1ULL << (w_SvId - PDSM_LITE_PD_SV_ID_GAL_MIN)) ) != 0ULL)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }
#endif // PDAPI >= 8.x
      }
      else if (w_SvId >= PDSM_LITE_PD_SV_ID_QZSS_MIN && w_SvId <= PDSM_LITE_PD_SV_ID_QZSS_MAX) // 193-197
      {
         pz_QmiSvOut->gnssSvId = w_SvId;
         pz_QmiSvOut->system = eQMI_LOC_SV_SYSTEM_QZSS_V02;

#if (PDSM_LITE_API_MAJ_V > 8) || ((PDSM_LITE_API_MAJ_V == 8) && (PDSM_LITE_API_MIN_V >= 3)) 
         pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
         pz_QmiSvOut->svInfoMask = 0;

         if ((pz_PdExtMeas->qzss_eph_svmask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_QZSS_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
         }

         if ((pz_PdExtMeas->qzss_alm_svmask & (1 << (w_SvId - PDSM_LITE_PD_SV_ID_QZSS_MIN)) ) != 0)
         {
            pz_QmiSvOut->svInfoMask |= QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
         }
#endif // PDAPI >= 8.3
      }
      else
      {
         pz_QmiSvOut->validMask = 0;

         LOC_MSG_MED ("locPd_GetSvReport: skipping SV ID: %d\n", w_SvId, 0, 0);

         // note that q_out won't be increases, so we will overwrite this same out record
         continue;
      }

      // azimuth and elevation in degrees
      if ((pz_PdSvIn->elevation != 0) && 
          (pz_PdSvIn->azimuth != 0)) 
      {
         pz_QmiSvOut->validMask |= (QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02 | QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02);
         pz_QmiSvOut->elevation = (float) (pz_PdSvIn->elevation);
         pz_QmiSvOut->azimuth = (float)  (pz_PdSvIn->azimuth);
      }

      // fill in SV cno, since PDAPI already filters out invalid measurement, cno will always be valid
      pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_SNR_V02;
      pz_QmiSvOut->snr = ((float) pz_PdSvIn->c_no) / 10;

      // fill in SV process status and c/no
      switch (pz_PdSvIn->sv_state)
      {
      case PDSM_LITE_SV_STATE_TRACK:
          pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02;
          pz_QmiSvOut->svStatus = eQMI_LOC_SV_STATUS_TRACK_V02;
          break;
      case PDSM_LITE_SV_STATE_BIT_EDGE:
          pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02;
          pz_QmiSvOut->svStatus = eQMI_LOC_SV_STATUS_SEARCH_V02;
          break;
      case PDSM_LITE_SV_STATE_SEARCH:
      case PDSM_LITE_SV_STATE_SEARCH_VERIFY:
          pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02;
          pz_QmiSvOut->svStatus = eQMI_LOC_SV_STATUS_SEARCH_V02;
          break;
      case PDSM_LITE_SV_STATE_IDLE:
          pz_QmiSvOut->validMask |= QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02;
          pz_QmiSvOut->svStatus = eQMI_LOC_SV_STATUS_IDLE_V02;
          break;
      default:
          break;
      }
      ++q_out;
   }
   pz_GnssSvInfoIndMsg->svList_len = q_out;
   if( pz_GnssSvInfoIndMsg->svList_len > 0 )
   {
      pz_GnssSvInfoIndMsg->svList_valid = TRUE;
   }

   locPd_dumpSvIn(pz_PdExtMeas);
}
#endif
















/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Unswap the Float data to match the system Endianess
  
@param[in]   p_floatData      char data Pointer

@retval    Float   Float data after swapping of bytes
*//* ------------------------------------------------------------------------*/

float FloatUnSwap(const unsigned char* p_floatData)
{
   union
   {
      float swappedFloatData;
      byte b[4];
   } data1, data2;

   if(NULL == p_floatData)
   {
      LOC_MSG_ERROR ("FloatUnSwap NULL input pointer", 0, 0, 0);
      return 0;
   }

   data1.b[0] = p_floatData[0];
   data1.b[1] = p_floatData[1];
   data1.b[2] = p_floatData[2];
   data1.b[3] = p_floatData[3];

   data2.b[0] = data1.b[3];
   data2.b[1] = data1.b[2];
   data2.b[2] = data1.b[1];
   data2.b[3] = data1.b[0];

   return data2.swappedFloatData;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Unswap the Double data to match the system Endianess
  
@param[in]   p_doubleData      char data Pointer

@retval    Double Double data after swapping of bytes
*//* ------------------------------------------------------------------------*/

double DoubleUnSwap(const unsigned char* p_doubleData)
{
   union
   {
      double swappedDoubleData;
      byte b[8];
   } data1, data2;

   if(NULL == p_doubleData)
   {
      LOC_MSG_ERROR ("DoubleUnSwap NULL input pointer", 0, 0, 0);
      return 0;
   }

   data1.b[0] = p_doubleData[0];
   data1.b[1] = p_doubleData[1];
   data1.b[2] = p_doubleData[2];
   data1.b[3] = p_doubleData[3];
   data1.b[4] = p_doubleData[4];
   data1.b[5] = p_doubleData[5];
   data1.b[6] = p_doubleData[6];
   data1.b[7] = p_doubleData[7];

   data2.b[0] = data1.b[7];
   data2.b[1] = data1.b[6];
   data2.b[2] = data1.b[5];
   data2.b[3] = data1.b[4];
   data2.b[4] = data1.b[3];
   data2.b[5] = data1.b[2];
   data2.b[6] = data1.b[1];
   data2.b[7] = data1.b[0];

   return data2.swappedDoubleData;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to return the size of the data type enum used in Opaque data Blob
  
@param[in]   dataType          Enum data Type

@retval    Size                 Size of Data Type
*//* ------------------------------------------------------------------------*/

size_t sizeofDataType(qmiLocSecureMessageDataTypeEnumT_v02 dataType)
{
   size_t size= 0;
   switch (dataType)
   {
      case eQMI_LOC_SECURE_DATA_TYPE_BYTE_8_V02 :
      size = sizeof(byte);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_CHAR_8_V02 :
      size = sizeof(char);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_CHAR_8_V02 :
      size = sizeof(char);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_INT_8_V02 :
      size = sizeof(int8);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02 :
      size = sizeof(uint8);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_INT_16_V02 :
      size = sizeof(int16);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_16_V02 :
      size = sizeof(uint16);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_INT_32_V02 :
      size = sizeof(int32);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02 :
      size = sizeof(uint32);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_INT_64_V02 :
      size = sizeof(int64);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_64_V02 :
      size = sizeof(uint64);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_BOOL_8_V02 :
      size = sizeof(boolean);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_DOUBLE_64_V02 :
      size = sizeof(double);
      break;
      case eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02:
      size = sizeof(float);
      break;
      default :
      size = 0;
      break;
   }
   return size;
}







/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to PD module
  (related to PDAPI Position Determination module)

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locPd_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
)
{
   uint32                  q_RetVal     = LOC_API_GENERAL_FAILURE;

   /* Function arguments sanity check */
   if (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle)
   {
      LOC_MSG_ERROR("locPd_ProcessQmiRequest: invalid clientHandle", 0,0,0);
      return FALSE;
   }

   switch (q_QmiLocMsgId)
   {
      case QMI_LOC_INJECT_POSITION_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPd_InjectExtPos(l_ClientHandle, (qmiLocInjectPositionReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locPd_ProcessQmiRequest QMI_LOC_INJECT_POSITION_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPd_GetBestAvailPos(l_ClientHandle, (qmiLocGetBestAvailablePositionReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locPd_ProcessQmiRequest QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPd_GetAvailWwanPos(l_ClientHandle, (qmiLocGetAvailWwanPositionReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locPd_ProcessQmiRequest QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }



      case QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPd_InjectGsmCellInfo(l_ClientHandle, (qmiLocInjectGSMCellInfoReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locPd_ProcessQmiRequest QMI_LOC_INJECT_GSM_CELL_INFO_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      default:
         q_RetVal = LOC_API_UNSUPPORTED;
         break;
   }

   if( LOC_API_SUCCESS == q_RetVal)
   {
      return TRUE;
   }
   else
   {
      LOC_MSG_ERROR("locPd_ProcessQmiRequest: QMI_LOC 0x%04x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
      return FALSE;
   }
}

/* Utility function to convert from QMI_LOC Reliability to PDSM */
static pdsm_lite_pd_reliability_e_type locPd_ConvertToPdsmReliability
(
  qmiLocReliabilityEnumT_v02 e_Reliability
)
{
  switch (e_Reliability)
  {
    case eQMI_LOC_RELIABILITY_VERY_LOW_V02:
    {
      return PDSM_LITE_POSITION_RELIABILITY_VERY_LOW;
    }
    case eQMI_LOC_RELIABILITY_LOW_V02:
    {
      return PDSM_LITE_POSITION_RELIABILITY_LOW;
    }
    case eQMI_LOC_RELIABILITY_MEDIUM_V02:
    {
      return PDSM_LITE_POSITION_RELIABILITY_MEDIUM;
    }
    case eQMI_LOC_RELIABILITY_HIGH_V02:
    {
      return PDSM_LITE_POSITION_RELIABILITY_HIGH;
    }

    case eQMI_LOC_RELIABILITY_NOT_SET_V02:
    default:
    {
      return PDSM_LITE_POSITION_RELIABILITY_NOT_SET;
    }
  }
}

/* Utility function to convert from QMI_LOC Altitude Source to PDSM */
static pdsm_lite_pd_altitude_source_e_type locPd_ConvertToPdsmAltSrc
(
  qmiLocAltSrcEnumT_v02 e_AltSrc
)
{
  switch(e_AltSrc)
  {
    case eQMI_LOC_ALT_SRC_GPS_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_GPS;
    }
    case eQMI_LOC_ALT_SRC_CELL_ID_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_CID;
    }

    case eQMI_LOC_ALT_SRC_TERRESTRIAL_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_TERRESTRIAL;
    }
    case eQMI_LOC_ALT_SRC_TERRESTRIAL_HYBRID_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_TERRESTRIAL_HYBRID;
    }
    case eQMI_LOC_ALT_SRC_ALTITUDE_DATABASE_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_ALTITUDE_DATABASE;
    }
    case eQMI_LOC_ALT_SRC_BAROMETRIC_ALTIMETER_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_BAROMETRIC_ALTIMETER;
    }
    case eQMI_LOC_ALT_SRC_OTHER_V02:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_OTHER;
    }

    case eQMI_LOC_ALT_SRC_UNKNOWN_V02:
    default:
    {
      return PDSM_LITE_ALTITUDE_SOURCE_UNKNOWN;
    }
  }
}

/* Function to process external coarse position injection request. */
static int locPd_InjectExtPos
(
  loc_client_handle_type                 l_ClientHandle,
  const qmiLocInjectPositionReqMsgT_v02 *pz_PosReq
)
{
  locPd_ModuleDataType*               pz_LocPdData = &(loc_middleware_data.z_locPd_Data);
  pdsm_lite_pd_external_position_info_type z_ExtPos = {0};

  /* Function arguments sanity check */
  if (NULL == pz_PosReq)
  {
    return LOC_API_INVALID_PARAMETER;
  }

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  if (pz_LocPdData->z_InjPos.v_InProgress)
  {
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    LOC_MSG_ERROR("locPd_InjectExtPos: Position Injection already in progress", 0, 0, 0);
    return LOC_API_ENGINE_BUSY;
  }

  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  /* Translate external coarse position from QMI_LOC to PDAPI format */

  /* Latitude, Longitude, Horizontal Uncertainty Circular & Confidence */
  if ((pz_PosReq->latitude_valid)  &&
      (pz_PosReq->longitude_valid) &&
      ((pz_PosReq->horUncCircular_valid) && (pz_PosReq->horUncCircular > 0.0)))
  {
    z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_LAT_LONG_VALID;
    z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID;
    z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID;

    z_ExtPos.latitude    = pz_PosReq->latitude;
    z_ExtPos.longitude   = pz_PosReq->longitude;
    z_ExtPos.loc_unc_hor = pz_PosReq->horUncCircular;

    /* Parse confidence only when circular unc is set */
    if (pz_PosReq->horConfidence_valid)
    {
      z_ExtPos.confidence_hor = pz_PosReq->horConfidence;
    }
    else
    {
      /* If not set, use default value */
      z_ExtPos.confidence_hor = 50;
    }

    if ((pz_PosReq->rawHorUncCircular_valid) && (pz_PosReq->rawHorUncCircular > 0.0))
    {
      z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID;
      z_ExtPos.loc_unc_hor_raw = pz_PosReq->rawHorUncCircular;

      /* Parse raw confidence only when raw circular unc is set */
      if (pz_PosReq->rawHorConfidence_valid)
      {
        z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID;
        z_ExtPos.confidence_hor_raw = pz_PosReq->rawHorConfidence;
      }
      else
      {
        /* If not set, use default value */
        z_ExtPos.confidence_hor_raw = 50;
      }
    }
  }
  else if ((pz_PosReq->latitude_valid)  ||
           (pz_PosReq->longitude_valid) ||
           ((pz_PosReq->horUncCircular_valid) && (pz_PosReq->horUncCircular > 0.0)) ||
           (pz_PosReq->horConfidence_valid))
  {
    LOC_MSG_ERROR("locPd_InjectExtPos: Invalid latitude & longitude parameters", 0, 0, 0);
    LOC_MSG_ERROR("Please provide valid latitude & longitude, with horizontal unc circular & confidence", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

  /* Altitude (wrt Ellipsoid or Mean Sea Level), Vertical Uncertainty & Confidence */
  if (((pz_PosReq->altitudeWrtEllipsoid_valid) || (pz_PosReq->altitudeWrtMeanSeaLevel_valid)) &&
       (pz_PosReq->vertUnc_valid))
  {
    z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID;

    if (pz_PosReq->altitudeWrtEllipsoid_valid)
    {
      z_ExtPos.altitude = pz_PosReq->altitudeWrtEllipsoid;
      z_ExtPos.alt_def  = PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_HAE;
    }
    else //if (pz_PosReq->altitudeWrtMeanSeaLevel_valid)
    {
      z_ExtPos.altitude = pz_PosReq->altitudeWrtMeanSeaLevel;
      z_ExtPos.alt_def  = PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_MSL;
    }

    if (pz_PosReq->vertUnc_valid)
    {
      z_ExtPos.loc_unc_vert = pz_PosReq->vertUnc;
    }

    if (pz_PosReq->vertConfidence_valid)
    {
      z_ExtPos.confidence_vert = pz_PosReq->vertConfidence;
    }
    else
    {
      /* If not set, use default value */
      z_ExtPos.confidence_vert = 50;
    }
  }
  else if ((pz_PosReq->altitudeWrtEllipsoid_valid)    ||
           (pz_PosReq->altitudeWrtMeanSeaLevel_valid) ||
           (pz_PosReq->vertUnc_valid)                 ||
           (pz_PosReq->vertConfidence_valid))
  {
    LOC_MSG_ERROR("locPd_InjectExtPos: Invalid altitude parameters", 0, 0, 0);
    LOC_MSG_ERROR("Please provide valid altitude, with vertical unc & confidence", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

  /* horReliability, vertReliability, altSourceInfo - Loc Api 2.0 Upgrades */
  if (pz_PosReq->horReliability_valid)
  {
    z_ExtPos.e_HoriRelIndicator = locPd_ConvertToPdsmReliability(pz_PosReq->horReliability);
  }
  if (pz_PosReq->vertReliability_valid)
  {
    z_ExtPos.e_VertRelIndicator = locPd_ConvertToPdsmReliability(pz_PosReq->vertReliability);
  }
  if (pz_PosReq->altSourceInfo_valid)
  {
    z_ExtPos.e_AltitudeSource = locPd_ConvertToPdsmAltSrc(pz_PosReq->altSourceInfo.source);

    /* Convert from QMI_LOC Altitude Source Horizontal Linkage to PDSM equivalent */
    switch (pz_PosReq->altSourceInfo.linkage)
    {
      case eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INTERDEPENDENT_V02:
      {
        z_ExtPos.e_AltHorLinkage = PDSM_LITE_ALT_HOR_LINK_FULLY_INTERDEPENDENT;
        break;
      }
      case eQMI_LOC_ALT_SRC_LINKAGE_DEPENDS_ON_LAT_LONG_V02:
      {
        z_ExtPos.e_AltHorLinkage = PDSM_LITE_ALT_HOR_LINK_ALT_DEP_LAT_LONG;
        break;
      }
      case eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INDEPENDENT_V02:
      {
        z_ExtPos.e_AltHorLinkage = PDSM_LITE_ALT_HOR_LINK_FULLY_INDEPENDENT;
        break;
      }

      case eQMI_LOC_ALT_SRC_LINKAGE_NOT_SPECIFIED_V02:
      default:
      {
        z_ExtPos.e_AltHorLinkage = PDSM_LITE_ALT_HOR_LINK_NOT_SPECIFIED;
      }
    }

    /* Convert from QMI_LOC Altitude Source Uncertainty Coverage to PDSM equivalent */
    switch (pz_PosReq->altSourceInfo.coverage)
    {
      case eQMI_LOC_ALT_UNCERTAINTY_POINT_V02:
      {
        z_ExtPos.e_AltUncCoverage = PDSM_LITE_ALT_COVERAGE_POINT_UNC;
        break;
      }
      case eQMI_LOC_ALT_UNCERTAINTY_FULL_V02:
      {
        z_ExtPos.e_AltUncCoverage = PDSM_LITE_ALT_COVERAGE_FULL_UNC;
        break;
      }

      case eQMI_LOC_ALT_UNCERTAINTY_NOT_SPECIFIED_V02:
      default:
      {
        z_ExtPos.e_AltUncCoverage = PDSM_LITE_ALT_COVERAGE_NOT_SPECIFIED;
      }
    }
  }

  /* Timestamps */
  if (pz_PosReq->timestampUtc_valid)
  {
    z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_GPS_TIME_VALID;
    z_ExtPos.time_info_type  = PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_UTC;
    z_ExtPos.timestamp_sec   = (pz_PosReq->timestampUtc + 500) / 1000 - GPS_TO_UTC_DELTA_SECONDS;
  }
  else if (pz_PosReq->timestampAge_valid)
  {
    z_ExtPos.opt_field_mask |= PDSM_LITE_PD_EXTERN_COARSE_POS_GPS_TIME_VALID;
    z_ExtPos.time_info_type  = PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_AGE;
    z_ExtPos.timestamp_sec   = pz_PosReq->timestampAge / 1000; /* convert from ms to sec */
  }

  /* Position Source */
  if (pz_PosReq->positionSrc_valid)
  {
    switch (pz_PosReq->positionSrc)
    {
      case eQMI_LOC_POSITION_SRC_GNSS_V02:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_GPS;
        break;
      }
      case eQMI_LOC_POSITION_SRC_CELLID_V02:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_CELLID;
        break;
      }
      case eQMI_LOC_POSITION_SRC_ENH_CELLID_V02:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_ENH_CELLID;
        break;
      }

      case eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_TERRESTRIAL;
        break;
      }
      case eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID_V02:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID;
        break;
      }
      case eQMI_LOC_POSITION_SRC_OTHER_V02:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_OTHER;
        break;
      }
      default:
      {
        z_ExtPos.source = PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN;
        break;
      }
    }
  }

  if (pz_PosReq->onDemandCpi_valid)
  {
    z_ExtPos.v_is_on_demand_position_inj = (boolean)(pz_PosReq->onDemandCpi);
  }

  if (pz_PosReq->positionSrcProvider_valid)
  {
    z_ExtPos.m.posSrcProviderValid = 0;
    switch(pz_PosReq->positionSrcProvider)
    {
      case eQMI_LOC_POSITION_SRC_PROVIDER_EXTERNAL_V02:
        z_ExtPos.src_provider = PDSM_LITE_PD_EXTERN_CPI_SRC_EXTERNAL;
        z_ExtPos.m.posSrcProviderValid = 1;
        break;

      case eQMI_LOC_POSITION_SRC_PROVIDER_INTERNAL_V02:
        z_ExtPos.src_provider = PDSM_LITE_PD_EXTERN_CPI_SRC_INTERNAL;
        z_ExtPos.m.posSrcProviderValid = 1;
        break;

      default:
        LOC_MSG_ERROR("locPd_InjectExtPos: Invalid positionSrcProvider = %d", pz_PosReq->positionSrcProvider, 0, 0);
    }
  }

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
  pz_LocPdData->z_InjPos.v_InProgress = TRUE;
  pz_LocPdData->z_InjPos.l_ClientHandle = l_ClientHandle; /* for issuing callback later on */
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  if (!pdsm_lite_pd_inject_external_position(locPd_CmdCb,
                                        NULL,       // user data pointer
                                        &z_ExtPos,  // external position
                                        loc_middleware_data.pdapi_client_id)) // Client id of user
  {
    LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    pz_LocPdData->z_InjPos.v_InProgress = FALSE;
    pz_LocPdData->z_InjPos.l_ClientHandle = LOC_CLIENT_HANDLE_INVALID;
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    return LOC_API_GENERAL_FAILURE;
  }

  return LOC_API_SUCCESS;
}

/* Function to translate the client's fix criteria from LOC API to PD API.
   The translated PD API fix criteria will be used in the next position request. */
void locPd_ConvertFixCfg2Pdapi (locQmiShimCltInfoStructT* pz_ClientInfo)
{
   locClnt_FixCriteriaType* p_FixCriteria = NULL;
   pdsm_lite_pd_option_s_type*   p_PdsmOption  = &(loc_middleware_data.z_locPd_Data.pdsm_lite_option);
   pdsm_lite_pd_qos_type*        p_PdsmQos     = &(loc_middleware_data.z_locPd_Data.pdsm_lite_qos);
   uint32                   q_fix_interval= LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT;

   if(NULL == pz_ClientInfo)
   {
      LOC_MSG_ERROR("locPd_ConvertFixCfg2Pdapi, NULL pz_ClientInfo",0,0,0);
      return;
   }
   p_FixCriteria = (locClnt_FixCriteriaType *)(&(pz_ClientInfo->z_FixCriteria));

   /* If it is a willing to share, use the common fix criteria */
   if(p_FixCriteria->v_SharePosition)
   {
      p_FixCriteria = &(loc_middleware_data.z_locPd_Data.loc_fix_criteria_rule);
   }

   p_PdsmOption->session                     = PDSM_LITE_PD_SESS_TYPE_NEW;
   if (LOC_CLIENT_FIX_SINGLE == p_FixCriteria->e_RecurrenceType )
   {
      p_PdsmOption->fix_rate.num_fixes          = 1;
   }
   else
   {
      p_PdsmOption->fix_rate.num_fixes          = UINT32_MAX;
   }

   /* Figure out fix interval */
   if ( LOC_CLIENT_NOTIFY_ON_DISTANCE == p_FixCriteria->e_NotifyType )
   {
      q_fix_interval = p_FixCriteria->q_MinDistSampleInterval;
   }
   else
   {
      q_fix_interval = p_FixCriteria->q_TbfMs;
   }
   p_PdsmOption->fix_rate.time_between_fixes_ms = q_fix_interval;

   if (p_PdsmOption->fix_rate.time_between_fixes_ms < PDSM_LITE_MINIMUM_TBF_MS)
   {
      p_PdsmOption->fix_rate.time_between_fixes_ms = PDSM_LITE_MINIMUM_TBF_MS;
   }

  /* Copy the location server info from client */
  /*9x05: doesnt support A-GNSS*/

  /* Translate the QOS info */
  p_PdsmQos->accuracy_threshold  = p_FixCriteria->q_PreferredAccuracy;
  if (p_FixCriteria->q_PreferredResponseTime > LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT)
  {
      p_PdsmQos->gps_session_timeout = (byte)(LOC_CLIENT_FIX_RESPONSE_TIME_MS_DEFAULT/1000); //255s
  }
  else
  {
    p_PdsmQos->gps_session_timeout = (byte)(p_FixCriteria->q_PreferredResponseTime/1000);
    if ((p_PdsmQos->gps_session_timeout == 0) && (p_FixCriteria->q_PreferredResponseTime != 0) )
    {
         p_PdsmQos->gps_session_timeout = LOC_CLIENT_FIX_INTERVAL_MS_DEFAULT/1000; //1s
    }

    /* For CP MO, floor qos timeout at 30 seconds */
  }

  p_PdsmOption->operation_mode = PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY;

  /* Set QoS timeout to 0 if operation mode is WWAN */

  /* If mode is CELL ID (UMTS) or AFLT (MSA with QoS == 0), and if the engine is ON,
   * we change the mode to MSB ref loc, this is done so that the GPS engine can stay on
   * and in the meantime, we also get a ref loc from the network. Ref Location can be used
   * to give a good quality GPS fix with low power in this case.
   */


  LOC_MSG_MED("locPd_ConvertFixCfg2Pdapi, operation_mode=%d, tbf=%d, accuracy_threshold=%d",
               p_PdsmOption->operation_mode, p_PdsmOption->fix_rate.time_between_fixes_ms, p_PdsmQos->accuracy_threshold);
  
  /* Populate the application ID */
}






/*============================================================================
 *                    U T I L I T Y    F U N C T I O N S FOR SECURE AVAILABLE POSITION
 * =========================================================================*/

static const int16 bsti_endian = 1;  // Byte swap test integer
#define is_bigendian() ( (*(char*)&bsti_endian) == 0 )


/**
In-place swapping of bytes to match endianness of hardware
@param[in/out] *object : memory to swap in-place
@param[in]     _size   : length in bytes
*/
void swapbytes(void *_object, size_t _size)
{
  uint8 *start, *end;
  if(NULL == _object)
  {
    return;
  }
  if (!is_bigendian())
  {
    for (start = (uint8 *)_object, end = start + _size - 1; start < end; ++start, --end)
    {
      unsigned char swap = *start;
      * start = *end;
      * end = swap;
    }
  }
}
/*Handles the nmea report coming from MGP and sends it to the registered clients*/
static void locPd_HandleNmeaPosReport
(
  const pdsm_lite_pd_info_s_type* pd_info_ptr
)
{
   qmiLocEventNmeaIndMsgT_v02   *pz_NmeaIndMsg = NULL;

   if (NULL == pd_info_ptr)
   {
      LOC_MSG_ERROR("locPd_HandleNmeaPosReport NULL status info ptr", 0, 0, 0);
      return;
   }

   pz_NmeaIndMsg = (qmiLocEventNmeaIndMsgT_v02 *)loc_calloc(sizeof(*pz_NmeaIndMsg));
   if (NULL != pz_NmeaIndMsg)
   {
     LOC_MSG_HIGH("locPd_HandleNmeaPosReport %d length nmea, nmea_type = 0x%x", 
            pd_info_ptr->pd_info.nmea_pos_report_type.nmea_length, pd_info_ptr->pd_info.nmea_pos_report_type.nmea_type, 0);

     memscpy(pz_NmeaIndMsg->nmea, sizeof(pz_NmeaIndMsg->nmea),
           pd_info_ptr->pd_info.nmea_pos_report_type.nmea_data, sizeof(pd_info_ptr->pd_info.nmea_pos_report_type.nmea_data));

     (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                               LOC_CMD_TYPE_NOTIFY_CLIENT,
                               QMI_LOC_EVENT_MASK_NMEA_V02,
                              (void *)pz_NmeaIndMsg);
   }
   else
   {
      LOC_MSG_ERROR("locPd_HandleNmeaPosReport, loc_calloc failed ", 0, 0, 0);
   }

}

/* Event callback called from PDAPI for a PD command.
   The implementation will rely on the UserData pointer to identify the client handle etc,
   and will lock the critical section. */
static void locPd_EventCb
(
  void*                      p_UserData,
  pdsm_lite_pd_event_type         q_PdEvent,
  const pdsm_lite_pd_info_s_type* pd_info_ptr
)
{
   qmiLocEventFixSessionStateIndMsgT_v02* pz_FixSessionStateInd = NULL;
   locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);
   qmiLocEventGnssSvInfoIndMsgT_v02* pz_LocSVData = &(loc_middleware_data.z_locSV_Data);
   time_type             time_ms = {0};
   uint64                current_time_ms;
   boolean               result = TRUE;
   qmiLocEventEngineStateIndMsgT_v02*  pz_EngineStateIndMsg = NULL;
   uint32                q_LocApiStatus = LOC_API_GENERAL_FAILURE;   

   LOC_MSG_HIGH("locPd_EventCb: event=0x%x, client_id=%d", q_PdEvent, pd_info_ptr->client_id, 0);

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
   
  if ((PDSM_LITE_PD_STATUS_NMEA_POS_REPORT & q_PdEvent) &&
      (PDAPI_LITE_NMEA_DEBUG == pd_info_ptr->pd_info.nmea_pos_report_type.nmea_type))
  {
     LOC_MSG_HIGH("locPd_ExtEventCb: Receive Debug NMEA",0,0,0);
     locPd_HandleNmeaPosReport(pd_info_ptr);
  }
  else
  {
   /* Location engine is turned on, ignore client id */
   if ((q_PdEvent & PDSM_LITE_PD_EVENT_GPS_BEGIN) ||
      (q_PdEvent & PDSM_LITE_PD_EVENT_GPS_DONE))
   {
      /* Queue the cmd to start another round of fix processing */
      pz_EngineStateIndMsg = (qmiLocEventEngineStateIndMsgT_v02*)loc_calloc(sizeof(*pz_EngineStateIndMsg));

      if (NULL != pz_EngineStateIndMsg)
      {
         if (q_PdEvent & PDSM_LITE_PD_EVENT_GPS_BEGIN)
         {
            LOC_MSG_MED ("locPd_EventCb, PDSM_LITE_PD_EVENT_GPS_BEGIN", 0,0,0);
            pz_LocPdData->engine_state = LOC_ENGINE_STATE_ON;
            pz_EngineStateIndMsg->engineState = eQMI_LOC_ENGINE_STATE_ON_V02;
         }
         else
         {
            LOC_MSG_MED ("locPd_EventCb, PDSM_LITE_PD_EVENT_GPS_DONE", 0,0,0);
            pz_LocPdData->engine_state = LOC_ENGINE_STATE_OFF;
            pz_EngineStateIndMsg->engineState = eQMI_LOC_ENGINE_STATE_OFF_V02;
         }

         if(TRUE != locMW_SendLocCmdProxy(
                       LOC_HANDLE_REGISTERED_CLIENTS,
                       LOC_CMD_TYPE_NOTIFY_CLIENT,
                       QMI_LOC_EVENT_MASK_ENGINE_STATE_V02,
                       (void *)pz_EngineStateIndMsg))
         {
           result            = FALSE;
         }

      }
      else
      {
         /* Memory allocation error */
         result = FALSE;
      }

      if (TRUE != result)
      {
         // TODO: what else can we do?
         LOC_MSG_ERROR("locPd_EventCb: failed processing engine status event", 0, 0, 0);
      }
   }

   /* GPS session begins */
   if (q_PdEvent & PDSM_LITE_PD_EVENT_BEGIN)
   {
      /* Check whether it is a recognized client */
      if (pd_info_ptr->client_id != loc_middleware_data.pdapi_client_id) 
      {
         LOC_MSG_ERROR("locPd_EventCb: dropped session_begin for unrecognized client", 0, 0, 0);
      }
      else
      {
         pz_LocPdData->fix_in_progress = TRUE;
         time_get_uptime_ms (time_ms);
         current_time_ms = QW_CVT_Q2N(time_ms);
         pz_LocPdData->last_get_position_time = current_time_ms;

         /* will be set to true if sent by pdapi_client_id */
         pz_LocPdData->mo_fix_in_progress = FALSE;

         result = TRUE; // will be set to FALSE for failure case explicitly

         /* Notify the active client only in case of MO fixes */
         
            
            pz_LocPdData->mo_fix_in_progress = TRUE;

            /* Send the fix session state event */
            pz_FixSessionStateInd = (qmiLocEventFixSessionStateIndMsgT_v02 *)loc_calloc(sizeof(*pz_FixSessionStateInd));

            if (NULL != pz_FixSessionStateInd)
            {
               pz_FixSessionStateInd->sessionState = eQMI_LOC_FIX_SESSION_STARTED_V02;

               if (TRUE != locMW_SendLocCmdProxy(
                              locClnt_GetActiveClient(),
                              LOC_CMD_TYPE_NOTIFY_CLIENT,
                              QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02,
                              (void *)pz_FixSessionStateInd))
               {
                  result            = FALSE;
               }
            }
            else
            {
               /* Memory allocation error */
               result = FALSE;
            }

            if (TRUE != result)
            {
               // TODO: what else can we do?
               LOC_MSG_ERROR("locPd_EventCb: failed processing session begin event", 0, 0, 0);
            }

      }// end of recognised client 
   }// end of if (pd_event & PDSM_LITE_PD_EVENT_BEGIN)

   // gps session ends
   if (q_PdEvent & PDSM_LITE_PD_EVENT_DONE)
   {
      loc_client_handle_type l_ClientHandle = locClnt_GetActiveClient();
      locQmiShimCltInfoStructT *pz_ClientInfo = locClnt_FindHandle(l_ClientHandle);
      if(NULL != pz_ClientInfo)
      {
         pz_ClientInfo->v_FirstFix = FALSE;
         LOC_MSG_MED("locPd_EventCb: set v_FirstFix to false for loc_client %d",
                      l_ClientHandle, 0, 0);
      }

      result = TRUE; //will be explicity set to false in case of a failure
      pz_LocPdData->fix_in_progress    = FALSE;

      // inform the client only if this was a MO fix
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id )
      {
         qmiLocEventFixSessionStateIndMsgT_v02* pz_FixSessionStateInd = NULL;
         pz_LocPdData->mo_fix_in_progress = FALSE;
         pz_FixSessionStateInd = (qmiLocEventFixSessionStateIndMsgT_v02 *)loc_calloc(sizeof(*pz_FixSessionStateInd));
         if(NULL != pz_FixSessionStateInd)
         {
            pz_FixSessionStateInd->sessionState = eQMI_LOC_FIX_SESSION_FINISHED_V02;

            if( TRUE != locMW_SendLocCmdProxy(
                           locClnt_GetActiveClient(),
                           LOC_CMD_TYPE_NOTIFY_CLIENT,
                           QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02,
                           (void *)pz_FixSessionStateInd))
            {
               result = FALSE;
            }
         }
         else
         {
            // memory allocation failure
            result = FALSE;
         }

         // positioning session is done, reset the state will send a request to update the scheduler
         locPd_Schedule();
      }
      
      else
      {
         //got a session done event for an unknown client
         LOC_MSG_HIGH ("locPd_EventCb: dropping session done event , unknown client %d", 
                        pd_info_ptr->client_id, 0, 0);
         result = FALSE;
      }

      if(TRUE != result)
      {
         // TODO: what else can we do?
          LOC_MSG_ERROR ("locPd_EventCb: failed processing session done event", 0, 0, 0);
      }
   }

   // pd end event, record down the error code
   if (q_PdEvent & PDSM_LITE_PD_EVENT_END)
   {
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
      {
         LOC_MSG_MED ("locPd_EventCb: end status=%d\n", pd_info_ptr->pd_info.end_status, 0, 0);
         // update the position received time
         loc_middleware_data.z_locPd_Data.last_fix_ttf = 0;
         pz_LocPdData->pd_sess_end_status = pd_info_ptr->pd_info.end_status;
         pz_LocPdData->new_pd_sess_end_status_received = TRUE;

         if( (PDSM_LITE_PD_END_GNSS_SRVC_NOT_AVAILABLE == pd_info_ptr->pd_info.end_status) && 
             (TRUE == pz_LocPdData->fix_in_progress) 
           )   
         {
         /* If session is ending without loading MGP, some client was victim. So if MGP is loaded, 
           we know some client's request was not fulfill earlier so start a session  */
           pz_LocPdData->v_any_victimized_client = TRUE;
         }
         
         (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                     LOC_CMD_TYPE_REPORT_POSITION,
                                     0,
                                     NULL);
      }
   }

   // pd position event
   if((q_PdEvent & PDSM_LITE_PD_EVENT_POSITION) ||
      (q_PdEvent & PDSM_LITE_PD_EVENT_VELOCITY) ||
      (q_PdEvent & PDSM_LITE_PD_EVENT_HEIGHT))
   {
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
      {
        // update the position received time
        time_get_uptime_ms (time_ms);
        pz_LocPdData->last_fix_arrival_time = QW_CVT_Q2N(time_ms);
        pz_LocPdData->last_fix_ttf = (dword) (pz_LocPdData->last_fix_arrival_time  - pz_LocPdData->last_get_position_time);
        if(pd_info_ptr->pd_info.pz_PositionReportIndMsg.sessionStatus == eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02)
        {
            pz_LocPdData->new_interm_pos_info_received = TRUE;
        }
        memscpy (&pz_LocPdData->pd_info, sizeof(pz_LocPdData->pd_info), pd_info_ptr, sizeof (*pd_info_ptr));
        pz_LocPdData->new_pd_info_received = TRUE;

        (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                   LOC_CMD_TYPE_REPORT_POSITION,
                                   0,
                                   NULL);
         
      }
      else
      {
        LOC_MSG_ERROR ("locPd_EventCb: client id mismatch between PDAPI %d and LocMW %d", 
                    pd_info_ptr->client_id, loc_middleware_data.pdapi_client_id, 0);
      }
   }
   //pd SV report
   if ( q_PdEvent & PDSM_LITE_PD_STATUS_SV_INFO_REPORT)
   {
      LOC_MSG_HIGH(" handling PDSM_LITE_PD_STATUS_SV_INFO_REPORT",0,0,0);
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
      {
          memscpy (pz_LocSVData, sizeof(*pz_LocSVData),&pd_info_ptr->pz_SVInfoIndMsg,sizeof(pd_info_ptr->pz_SVInfoIndMsg));
	     (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                   LOC_CMD_TYPE_REPORT_SV_INFO,
                                   0,
                                   NULL);
	   	
      }  
      else
      {
        LOC_MSG_ERROR ("locPd_EventCb: client id mismatch between PDAPI %d and LocMW %d", 
                    pd_info_ptr->client_id, loc_middleware_data.pdapi_client_id, 0);
      }
   }
   // wifi ap scan info injection needed. Ignore client id and send to all (NI/MO)
   // wifi ap scan info injection ack.



   // Forwarding the event to callback hook
   if (pz_LocPdData->pd_event_cb_hook != NULL)
   {
      pz_LocPdData->pd_event_cb_hook(p_UserData, q_PdEvent, pd_info_ptr);
   }

   if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
   {
      if ((q_PdEvent & PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE) ||
          (q_PdEvent & PDSM_LITE_PD_EVENT_COARSE_POS_INJ_FAILED)||
          (q_PdEvent & PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR))
      {         

         if (q_PdEvent & PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE)
         {
            q_LocApiStatus = LOC_API_SUCCESS;
         }

         /* Send IPC message to LocMW task to handle QMI_LOC indication */
         locMW_SendQmiIndicationProxy(pz_LocPdData->z_InjPos.l_ClientHandle,
                                       QMI_LOC_INJECT_POSITION_IND_V02, 
                                       NULL, q_LocApiStatus);

         pz_LocPdData->z_InjPos.v_InProgress = FALSE;
      }
   }

    if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
   {
       if ((PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT == q_PdEvent) && pz_LocPdData->z_GetBestAvailPos.v_InProgress)
    {
       qmiLocGetBestAvailablePositionIndMsgT_v02* pz_BestAvailPosInd =
        (qmiLocGetBestAvailablePositionIndMsgT_v02 *)loc_calloc(sizeof(*pz_BestAvailPosInd));
   
       LOC_MSG_MED("locPd_ExtEventCb: Received best available position", 0, 0, 0);
   
       if (NULL != pz_BestAvailPosInd)
       {

             memscpy(pz_BestAvailPosInd, sizeof(*pz_BestAvailPosInd), 
               &(pd_info_ptr->pd_info.pz_BestAvailPosInd),sizeof(*pz_BestAvailPosInd));
        /* copy the transaction id which was saved in the incoming call */
        pz_BestAvailPosInd->transactionId_valid = 1;
        pz_BestAvailPosInd->transactionId     = pz_LocPdData->z_GetBestAvailPos.q_TxnId;

        if(pz_BestAvailPosInd->status == eQMI_LOC_SUCCESS_V02)
        {
          q_LocApiStatus = LOC_API_SUCCESS;
        }
        
        /* Send IPC message to LocMW task to handle QMI_LOC indication */
        locMW_SendQmiIndicationProxy(pz_LocPdData->z_GetBestAvailPos.l_ClientHandle,
                     QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02,
                     pz_BestAvailPosInd,
                     q_LocApiStatus);
       }
       else
       {
        LOC_MSG_ERROR("locPd_EventCb: Failed to allocate memory for QMI_LOC Indication", 0, 0, 0);
       }
   
       pz_LocPdData->z_GetBestAvailPos.v_InProgress = FALSE;
    }
   
    if ((PDSM_LITE_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT == q_PdEvent) && pz_LocPdData->z_GetAvailWwanPos.v_InProgress)
    {
       qmiLocGetAvailWwanPositionIndMsgT_v02* pz_AvailWwanPosInd =
        (qmiLocGetAvailWwanPositionIndMsgT_v02 *)loc_calloc(sizeof(*pz_AvailWwanPosInd));
   
       LOC_MSG_MED("locPd_ExtEventCb: Received available WWAN position", 0, 0, 0);
   
       if (NULL != pz_AvailWwanPosInd)
       {

         memscpy(pz_AvailWwanPosInd, sizeof(*pz_AvailWwanPosInd),
          &(pd_info_ptr->pd_info.pz_BestAvailPosInd),sizeof(*pz_AvailWwanPosInd));
        /* copy the transaction id which was saved in the incoming call */
        pz_AvailWwanPosInd->transactionId_valid = 1;
        pz_AvailWwanPosInd->transactionId     = pz_LocPdData->z_GetAvailWwanPos.q_TxnId;
        if(pz_AvailWwanPosInd->status == eQMI_LOC_SUCCESS_V02)
        {
          q_LocApiStatus = LOC_API_SUCCESS;
        }
        
        /* Send IPC message to LocMW task to handle QMI_LOC indication */
        locMW_SendQmiIndicationProxy(pz_LocPdData->z_GetAvailWwanPos.l_ClientHandle,
                     QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02,
                     pz_AvailWwanPosInd,
                     q_LocApiStatus);
       }
       else
       {
        LOC_MSG_ERROR("locPd_ExtEventCb: Failed to allocate memory for QMI_LOC Indication", 0, 0, 0);
       }
   
       pz_LocPdData->z_GetAvailWwanPos.v_InProgress = FALSE;
    }
   }
   else
   {
      LOC_MSG_HIGH ("locPd_EventCb: client id mismatch for event=0x%x", q_PdEvent, 0, 0);
   }
  }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
}

/* Command callback called from PDAPI for a PD command.
   The implementation will rely on loc_middleware_data to identify the client handle etc,
   and will lock the critical section. */
static void locPd_CmdCb
(
  void*                  p_DataBlock, /* not used */
  pdsm_lite_pd_cmd_e_type     e_CmdType,
  pdsm_lite_pd_cmd_err_e_type e_CmdErrStatus
)
{
  locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);

  LOC_MSG_MED("locPd_CmdCb: cmd=%d, err=%d", e_CmdType, e_CmdErrStatus, 0);

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  /* If error is indicated, no further processing of this PD cmd will be performed. */
  switch (e_CmdType)
  {
    case PDSM_LITE_PD_CMD_GET_POS:
    {
      if (PDSM_LITE_PD_CMD_ERR_NOERR != e_CmdErrStatus)
      {
        pz_LocPdData->new_pd_cmd_err_received = TRUE;
        pz_LocPdData->pd_cmd_err              = e_CmdErrStatus;
        pz_LocPdData->mo_fix_in_progress      = FALSE;

        /* Send error report (empty location report) to client */
        (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                    LOC_CMD_TYPE_REPORT_POSITION,
                                    0,
                                    NULL);

        locPd_Schedule();
      }
      break;
    }

    case PDSM_LITE_PD_CMD_END_SESSION:
    case PDSM_LITE_PD_CMD_END_SESSION_EX:
      break;

    case PDSM_LITE_PD_CMD_INJECT_POS:
    {
      if (PDSM_LITE_PD_CMD_ERR_NOERR != e_CmdErrStatus)
      {
        /* Send IPC message to LocMW task to handle QMI_LOC indication */
        locMW_SendQmiIndicationProxy(pz_LocPdData->z_InjPos.l_ClientHandle,
                                     QMI_LOC_INJECT_POSITION_IND_V02, 
                                     NULL,
                                     locPd_ConvertCmdErr2LocStatus(e_CmdErrStatus));

        pz_LocPdData->z_InjPos.v_InProgress = FALSE;
      }
      break;
    }

    case PDSM_LITE_PD_CMD_GET_BEST_AVAIL_POS:
    {
      if ((PDSM_LITE_PD_CMD_ERR_NOERR != e_CmdErrStatus) &&
          (pz_LocPdData->z_GetBestAvailPos.v_InProgress))
      {
        qmiLocGetBestAvailablePositionIndMsgT_v02* pz_BestAvailPosInd =
           (qmiLocGetBestAvailablePositionIndMsgT_v02 *)loc_calloc(sizeof(*pz_BestAvailPosInd));

        if (NULL == pz_BestAvailPosInd)
        {
          LOC_MSG_ERROR("locPd_CmdCb: Failed to allocate memory for QMI_LOC Indication", 0, 0, 0);
        }
        else
        {
          /* copy the transaction id which was saved in the incoming call */
          pz_BestAvailPosInd->transactionId_valid = 1;
          pz_BestAvailPosInd->transactionId       = pz_LocPdData->z_GetBestAvailPos.q_TxnId;

          /* Send IPC message to LocMW task to handle QMI_LOC indication */
          locMW_SendQmiIndicationProxy(pz_LocPdData->z_GetBestAvailPos.l_ClientHandle,
                                      QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02, 
                                      pz_BestAvailPosInd,
                                      locPd_ConvertCmdErr2LocStatus(e_CmdErrStatus));
        }

          LOC_MSG_ERROR("locPd_CmdCb: Failed to get Best Available position. Error=%d", e_CmdErrStatus, 0, 0);

          /* this code is executed inside critical section so no need enter the critical section here. */
          pz_LocPdData->z_GetBestAvailPos.v_InProgress = FALSE;
      }
      break;
    }

    case PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS:
    {
      if ((PDSM_LITE_PD_CMD_ERR_NOERR != e_CmdErrStatus)
           && (pz_LocPdData->z_GetAvailWwanPos.v_InProgress))
      {
        qmiLocGetAvailWwanPositionIndMsgT_v02* pz_AvailWwanPosInd =
           (qmiLocGetAvailWwanPositionIndMsgT_v02 *)loc_calloc(sizeof(*pz_AvailWwanPosInd));

        if (NULL == pz_AvailWwanPosInd)
        {
          LOC_MSG_ERROR("locPd_CmdCb: Failed to allocate memory for QMI_LOC Indication", 0, 0, 0);
        }
        else
        {
          /* copy the transaction id which was saved in the incoming call */
          pz_AvailWwanPosInd->transactionId_valid = 1;
          pz_AvailWwanPosInd->transactionId       = pz_LocPdData->z_GetAvailWwanPos.q_TxnId;

          /* Send IPC message to LocMW task to handle QMI_LOC indication */
          locMW_SendQmiIndicationProxy(pz_LocPdData->z_GetAvailWwanPos.l_ClientHandle,
                                      QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02, 
                                      pz_AvailWwanPosInd,
                                      locPd_ConvertCmdErr2LocStatus(e_CmdErrStatus));
        }

          LOC_MSG_ERROR("locPd_CmdCb: Failed to get Available WWAN position. Error=%d", e_CmdErrStatus, 0, 0);

          /* this code is executed inside critical section so no need enter the critical section here. */
          pz_LocPdData->z_GetAvailWwanPos.v_InProgress = FALSE;
      }
      break;
    }

   /* wifi on demand - PDSM_LITE_PD_CMD_INJECT_WIFI_ON_DEMAND - will never be called as we are passing null func pointer
      in the pdsm call. Failure QMI RESP is sent when pdsm call return failure. */

   
   

    // No such command or not supported
    default:
    {
      LOC_MSG_ERROR("locPd_CmdCb: cmd not supported", 0, 0, 0);
      break;
    }
  }
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
}

/** locPd_CmdCbUnlocked
 @details: The command callback called in PDSM context, for a PD
           command that was previously sent to PDAPI. The function
           should generate an IPC to notify Loc MW to send an
           QMI_LOC Indication for the corresponding command.
           Extracts the Loc MW client and PD information from the
           DataBlock ptr without any need for locking the Loc MW
           critical section.

 @warning: Should be invoked only for commands that don't have a
           corresponding event_cb. PDAPI gives out the correct
           DataBlock ptr in command callbacks, but not in the
           event callbacks.

 @param[in]: p_DataBlock   : data pointer passed in during command invocation
 @param[in]: e_CmdType     : PDSM command type for which this callback is called
 @param[in]: e_CmdErrStatus: Command processing result
*/ 
static void locPd_CmdCbUnlocked
(
  void*                  p_DataBlock,
  pdsm_lite_pd_cmd_e_type     e_CmdType,
  pdsm_lite_pd_cmd_err_e_type e_CmdErrStatus
)
{
  locPd_CmdCbData* pz_CmdCbData = (locPd_CmdCbData*)p_DataBlock;
  uint32           q_Status    = locPd_ConvertCmdErr2LocStatus(e_CmdErrStatus);
  uint32           q_MsgId     = 0;

  /* Function arguments sanity check */
  if (NULL == pz_CmdCbData || pz_CmdCbData != pz_CmdCbData->p_Self)
  {
    LOC_MSG_ERROR("locPd_CmdCbUnlocked: invalid callback data %p", pz_CmdCbData, 0, 0);
    return;
  }

  LOC_MSG_MED("locPd_CmdCbUnlocked: e_CmdType=%d=%d, error=%u", pz_CmdCbData->e_CmdType, e_CmdType, q_Status);

  q_MsgId = pz_CmdCbData->q_QmiLocIndId;

  switch (pz_CmdCbData->e_CmdType)
  {
    case PDSM_LITE_PD_CMD_INJECT_CELL_INFO:
    {
      if (QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02             == q_MsgId)
      {
        /* Send IPC message to LocMW task to handle QMI_LOC indication */
        locMW_SendQmiIndicationProxy(pz_CmdCbData->l_ClientHandle,
                                     q_MsgId,
                                     NULL,
                                     q_Status);
      }
      else
      {
        LOC_MSG_ERROR("locPd_CmdCbUnlocked: Invalid QMI_LOC Indication MsgId=%d", q_MsgId, 0, 0);
      }

      break;
    }
  
    

    default:
    {
      LOC_MSG_ERROR("locPd_CmdCbUnlocked: Unknown PD CmdType=%d", pz_CmdCbData->e_CmdType, 0, 0);
      break;
    }
  }
  loc_free(pz_CmdCbData);
}

/* C_SV_POLY defined in mgp_pe_common.h */



/*#9x05: NMEA not supported*/




/* This funciton will be called when:
   (1) pdsm_lite_get_position returns false
   (2) pd cmd callback reports an error
   (3) PDSM_LITE_PD_EVENT_DONE is received
   (4) timer expires waiting for PDSM_LITE_PD_EVENT_DONE */
static void locPd_Schedule (void)
{
  LOC_MSG_HIGH("locPd_Schedule", 0, 0, 0);

  /* Not much we can do if it fails. Also no memory is allocated, so no resource to reclaim */
  (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                              LOC_CMD_TYPE_PD_SCHEDULE,0,NULL);

}

// Calculate GEO ID based on latitude and longitude


// Translate fix end reason into session status in the loc api position report
static qmiLocSessionStatusEnumT_v02 locPd_XlateFixEndReasonCode(pdsm_lite_pd_end_e_type e_PdSessEndStatus)
{
   qmiLocSessionStatusEnumT_v02 e_SessionStatus;

   switch(e_PdSessEndStatus)
   {
   case PDSM_LITE_PD_END_SESS_NONE:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_SUCCESS_V02;
      break;

   case PDSM_LITE_PD_END_TIMEOUT:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_TIMEOUT_V02;
      break;

   case PDSM_LITE_PD_END_CLIENT_END:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_USER_END_V02;
      break;
   case PDSM_LITE_PD_END_GNSS_SRVC_NOT_AVAILABLE:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_GNSS_SERVICE_NOT_AVAILABLE_V02;
      break;
   default:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02;
      break;
   }

   LOC_MSG_MED ("locPd_XlateFixEndReasonCode, PdSessEndStatus = %d, qmiLocSessionStatus = %d",
                 e_PdSessEndStatus, e_SessionStatus ,0);

   return e_SessionStatus;
}

// Translate PD cmd error into session status in the loc api position report
static qmiLocSessionStatusEnumT_v02 locPd_XlateCmdErrCodeToSessStatus  (pdsm_lite_pd_cmd_err_e_type e_PdCmdErr)
{
   qmiLocSessionStatusEnumT_v02 e_SessionStatus;

   switch(e_PdCmdErr)
   {
   case PDSM_LITE_PD_CMD_ERR_SESS_TYPE_P:
   case PDSM_LITE_PD_CMD_ERR_OPERATION_P:
   case PDSM_LITE_PD_CMD_ERR_NUM_FIXES_P:
   case PDSM_LITE_PD_CMD_ERR_LSINFO_P:
   case PDSM_LITE_PD_CMD_ERR_TIMEOUT_P:
   case PDSM_LITE_PD_CMD_ERR_QOS_P:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_BAD_PARAMETER_V02;
        break;

   case PDSM_LITE_PD_CMD_ERR_OFFLINE_S:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_PHONE_OFFLINE_V02;
      break;

   case PDSM_LITE_PD_CMD_ERR_GPS_LOCK_S:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_ENGINE_LOCKED_V02;
      break;
   case PDSM_LITE_PD_CMD_ERR_GNSS_SRVC_NOT_AVAILABLE:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_GNSS_SERVICE_NOT_AVAILABLE_V02;
      break;
   default:
      e_SessionStatus = eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02;
      break;
   }

   LOC_MSG_MED ("locPd_XlateCmdErrCodeToSessStatus, cmd err = %d, session status = %d",
                 e_PdCmdErr, e_SessionStatus ,0);
   return e_SessionStatus;
}

/* Function to translate PD cmd error into session status in the LOC API position report. */
static uint32 locPd_ConvertCmdErr2LocStatus (pdsm_lite_pd_cmd_err_e_type e_PdCmdErr)
{
  switch(e_PdCmdErr)
  {
    case PDSM_LITE_PD_CMD_ERR_NOERR:
      return LOC_API_SUCCESS;

    case PDSM_LITE_PD_CMD_ERR_SESS_TYPE_P:
    case PDSM_LITE_PD_CMD_ERR_OPERATION_P:
    case PDSM_LITE_PD_CMD_ERR_NUM_FIXES_P:
    case PDSM_LITE_PD_CMD_ERR_LSINFO_P:
    case PDSM_LITE_PD_CMD_ERR_TIMEOUT_P:
    case PDSM_LITE_PD_CMD_ERR_QOS_P:
      return LOC_API_INVALID_PARAMETER;

    case PDSM_LITE_PD_CMD_ERR_OFFLINE_S:
      return LOC_API_PHONE_OFFLINE;

    case PDSM_LITE_PD_CMD_ERR_GPS_LOCK_S:
      return LOC_API_UNSUPPORTED;

    default:
      return LOC_API_GENERAL_FAILURE;
  }
}

/* Function to process the best available position get request. */
static int locPd_GetBestAvailPos
(
  loc_client_handle_type                           l_ClientHandle,
  const qmiLocGetBestAvailablePositionReqMsgT_v02* pz_BestAvailPosReq
)
{
  locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);

  /* Function arguments sanity check */
  if (NULL == pz_BestAvailPosReq)
  {
    return LOC_API_INVALID_PARAMETER;
  }

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  if (pz_LocPdData->z_GetBestAvailPos.v_InProgress)
  {
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    return LOC_API_ENGINE_BUSY;
  }

  /* Set the parameters since PDAPI can call the callback before this function returns */
  pz_LocPdData->z_GetBestAvailPos.v_InProgress = TRUE;
  pz_LocPdData->z_GetBestAvailPos.l_ClientHandle = l_ClientHandle;
  pz_LocPdData->z_GetBestAvailPos.q_TxnId        = pz_BestAvailPosReq->transactionId;

  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  if (!pdsm_lite_pd_get_best_avail_pos(locPd_CmdCb, loc_middleware_data.pdapi_client_id, NULL))
  {
    /* Clear the in progress flag */
    LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    pz_LocPdData->z_GetBestAvailPos.v_InProgress = FALSE;
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

    return LOC_API_GENERAL_FAILURE;
  }

  return LOC_API_SUCCESS;
}

/* Function to process the available WWAN position get request. */
static int locPd_GetAvailWwanPos
(
  loc_client_handle_type                       l_ClientHandle,
  const qmiLocGetAvailWwanPositionReqMsgT_v02* pz_AvailWwanPosReq
)
{
  locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);
  pdsm_lite_gtp_wwan_info_type z_gtp_info = {0};
  boolean u_retVal;

  /* Function arguments sanity check */
  if (NULL == pz_AvailWwanPosReq)
  {
    return LOC_API_INVALID_PARAMETER;
  }

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  if (pz_LocPdData->z_GetAvailWwanPos.v_InProgress)
  {
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    return LOC_API_ENGINE_BUSY;
  }

  /* Set the in progress flag and client handle since PDAPI can call the callback before this function returns */
  pz_LocPdData->z_GetAvailWwanPos.v_InProgress = TRUE;
  pz_LocPdData->z_GetAvailWwanPos.l_ClientHandle = l_ClientHandle;
  pz_LocPdData->z_GetAvailWwanPos.q_TxnId        = pz_AvailWwanPosReq->transactionId;

  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

  if (pz_AvailWwanPosReq->gtpLocMask_valid && (pz_AvailWwanPosReq->gtpLocMask == QMI_LOC_GTP_ENHANCED_MASK_V02))
  {
    z_gtp_info.u_TimeOutSec = pz_AvailWwanPosReq->timeOut;
  }

  if (!pdsm_lite_pd_get_avail_wwan_pos_ex(locPd_CmdCb, loc_middleware_data.pdapi_client_id, NULL, &z_gtp_info))
  {
    /* Clear the in progress flag */
    LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    pz_LocPdData->z_GetAvailWwanPos.v_InProgress = FALSE;
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
    return LOC_API_GENERAL_FAILURE;
  }
  return LOC_API_SUCCESS;
}




/*===========================================================================
FUNCTION locPd_InjectGsmCellInfo

DESCRIPTION
  Inject cellular information into the engine. 

DEPENDENCIES

INPUT PARAMS
  l_ClientHandle   : Identifies the loc API client injecting
  the GSM Cell information
  pz_GsmCellInfoReq : pointer to the GSM Cell Information 

RETURN VALUE
  LOC_API_SUCCESS           : processed command successfully
  LOC_API_GENERAL_FAILURE   : unspecified error command returned error.
  LOC_API_INVALID_PARAMETER : invalid parameter passed in.

SIDE EFFECTS

===========================================================================*/
static int locPd_InjectGsmCellInfo
( 
  loc_client_handle_type                    l_ClientHandle,
  const qmiLocInjectGSMCellInfoReqMsgT_v02* pz_GsmCellInfoReq
)
{
  int                   q_Status       = LOC_API_SUCCESS;
  locPd_CmdCbData*      pz_CmdCbData    = NULL;
  pdsm_lite_cell_info_s_type z_PdsmCellInfo = {0};

  /* Function arguments sanity check */
  if (NULL == pz_GsmCellInfoReq)
  {
    LOC_MSG_ERROR("locPd_InjectGsmCellInfo: NULL pointer", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

  do
  {
    /* Allocate the callback data on heap to be freed in cmd_cb_unlocked or if there is an error. */
    pz_CmdCbData = (locPd_CmdCbData*)loc_calloc(sizeof(*pz_CmdCbData));
    if (NULL == pz_CmdCbData)
    {
      LOC_MSG_ERROR("locPd_InjectGsmCellInfo: calloc failed", 0, 0, 0);
      q_Status = LOC_API_GENERAL_FAILURE;
      break;
    }

    pz_CmdCbData->l_ClientHandle = l_ClientHandle;
    pz_CmdCbData->e_CmdType      = PDSM_LITE_PD_CMD_INJECT_CELL_INFO;
    pz_CmdCbData->q_QmiLocIndId  = QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02;
    pz_CmdCbData->p_Self         = pz_CmdCbData;

    /* Set the RAT to GSM */
    z_PdsmCellInfo.activeRAT = PDSM_LITE_ACTIVERAT_GSM;

    /* Set the service status */
    z_PdsmCellInfo.serviceStatus = 
       (TRUE == pz_GsmCellInfoReq->roamingStatus) ? STATUS_CAMPED_ROAMING : STATUS_CAMPED_NOT_ROAMING;

    /* Set the GSM cell information */
    z_PdsmCellInfo.cellInfo.gsmCell.CID = pz_GsmCellInfoReq->gsmCellId.CID;
    z_PdsmCellInfo.cellInfo.gsmCell.LAC = pz_GsmCellInfoReq->gsmCellId.LAC;
    z_PdsmCellInfo.cellInfo.gsmCell.MCC = pz_GsmCellInfoReq->gsmCellId.MCC;
    z_PdsmCellInfo.cellInfo.gsmCell.MNC = pz_GsmCellInfoReq->gsmCellId.MNC;

    /* Note: timingAdvance TLV is not parsed */

    if (!pdsm_lite_pd_inject_external_cell_info(locPd_CmdCbUnlocked,
                                           pz_CmdCbData,
                                           loc_middleware_data.pdapi_client_id,
                                           &z_PdsmCellInfo))
    {
      q_Status = LOC_API_GENERAL_FAILURE;
      break;
    }
  } while(0);

  if (LOC_API_SUCCESS != q_Status)
  {
    LOC_MSG_ERROR("locPd_InjectGsmCellInfo: Error=%d", q_Status, 0, 0);

    /* Free the callback data on heap in an error case */
    if (NULL != pz_CmdCbData)
    {
      loc_free(pz_CmdCbData);
    }
  }
  else
  {
    LOC_MSG_MED("locPd_InjectGsmCellInfo: Status=%d", q_Status, 0, 0);
  }

  return q_Status;
}


/****************************loc_pd function definition-end******************************/

/****************************loc_pa function definition-start******************************/
// A general helper function to translate QMI mask to PDSM mask
/* -----------------------------------------------------------------------*//**
@brief
  A helper utility to translate QMI mask to PDSM mask
  
@param[in]   pz_QmiPdsmMaskMap    Predefined QMI and PDSM mask map
             q_Mask               QMI mask
             u_MaskMapSize        Mask map size

@retval      PSDM mask
*//* ------------------------------------------------------------------------*/
static uint32 locPa_XlateQmiPdsmMask(
   const locPa_MaskMapType *pz_QmiPdsmMaskMap,
   uint32                  q_Mask,
   uint8                   u_MaskMapSize)
{
   int    l_Cntr;
   uint32 q_PdapiMask = 0;

   if(NULL == pz_QmiPdsmMaskMap)
   {
      LOC_MSG_ERROR("locPa_XlateQmiPdsmMask, NULL pointer",0,0,0);
      return q_PdapiMask;
   }

   for (  l_Cntr = 0; l_Cntr < u_MaskMapSize; l_Cntr++ )
   {
      if(0 != (pz_QmiPdsmMaskMap[l_Cntr].q_QmiMask & q_Mask))
      {
         q_PdapiMask |= pz_QmiPdsmMaskMap[l_Cntr].q_PdapiMask;
      }
   }

   return q_PdapiMask;
}

/*---------------------------------------------------------------------------
@brief
  Function to initialize location PA module

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locPa_Init()
{
  locPa_ModuleDataType*      pz_LocPaData = &(loc_middleware_data.z_locPa_Data);
  pdsm_lite_client_status_e_type  pdsm_lite_ret_val;
  boolean                    ret_val;

  do
  {
    pz_LocPaData->pz_ProcessTimer = os_TimerCreate(LOC_MIDDLEWARE_TIMER_ID_PA, THREAD_ID_LOC_MIDDLEWARE);

    if (loc_middleware_data.pdapi_client_id == -1)
    {
      loc_middleware_data.pdapi_client_id = pdsm_lite_client_init(PDSM_LITE_CLIENT_TYPE_PDA);
    }

    if (loc_middleware_data.pdapi_client_id == -1)
    {
      LOC_MSG_ERROR ( "pdsm_lite_client_init failed", 0, 0, 0);
      ret_val = FALSE;
      break;
    }

    pdsm_lite_ret_val = pdsm_lite_client_pa_reg(loc_middleware_data.pdapi_client_id,
                                      NULL,
                                      locPa_EventCb,
                                      PDSM_LITE_CLIENT_EVENT_REG,
                                      PDSM_LITE_PA_EVENT_MASK,
                                      NULL);
    if (pdsm_lite_ret_val != PDSM_LITE_CLIENT_OK)
    {
      ret_val = FALSE;
      break;
    }

    /* register with the pdsm pa extension */
 // Todo: Do we need pdsm_lite_client_pa_reg_ex also do we need locPa_EventCbExt
    pdsm_lite_ret_val = pdsm_lite_client_pa_reg_ex(loc_middleware_data.pdapi_client_id,
                                         NULL,
                                         locPa_EventCbExt,
                                         PDSM_LITE_CLIENT_EVENT_REG,
                                         NULL);
    if (pdsm_lite_ret_val != PDSM_LITE_CLIENT_OK)
    {
      ret_val = FALSE;
      break;
    }

    pdsm_lite_ret_val = pdsm_lite_client_act(loc_middleware_data.pdapi_client_id);
    if (pdsm_lite_ret_val != PDSM_LITE_CLIENT_OK)
    {
      ret_val = FALSE;
      break;
    }
    else
    {
      ret_val = TRUE;
    }

    // Issue a command to get mo method
//    locPa_GetParam(LOC_CLIENT_HANDLE_INVALID, PDSM_LITE_PA_MO_METHOD); /* AHSIAU: Is this still required */

   } while (0);

   return ret_val;
}/* locPa_Init */

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to PA module
  (related to PDAPI Get/Set_Parameters)

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length
@param[in/out] pe_QmiErr   : QMI_LOC Response error value

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locPa_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*                  p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen,
   qmi_error_type_v01     *pe_QmiErr
)
{
   locPa_ModuleDataType*  pz_LocPaData = &(loc_middleware_data.z_locPa_Data);
   uint8*                 pu_DestBuffer = NULL;
   uint32                 q_RetVal = LOC_API_GENERAL_FAILURE;
   boolean                v_RetVal = FALSE;

   if (NULL == pe_QmiErr)
   {
      LOC_MSG_ERROR("locPa_ProcessQmiRequest: NUll pe_QmiErr", 0,0,0);
      return v_RetVal;
   }

   /* Function arguments sanity check */
   if (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle)
   {
      LOC_MSG_ERROR("locPa_ProcessQmiRequest: invalid clientHandle", 0,0,0);
      *pe_QmiErr = QMI_ERR_INTERNAL_V01;
      return v_RetVal;
   }

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);

   /* Allow only one PA cmd to be processed at any time */
   if (pz_LocPaData->v_PaCmdInProgress)
   {
      LOC_MSG_ERROR("locPa_ProcessQmiRequest rejected, ParamType=%d still in progress", pz_LocPaData->e_ParamType, 0, 0);

      LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);
      *pe_QmiErr = QMI_ERR_SESSION_OWNERSHIP_V01;
      return v_RetVal;
   }
   LOC_MSG_MED("locPa_ProcessQmiRequest start timer, set InProgress for QMI_LOC 0x%04x", q_QmiLocMsgId,0,0);
   LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

   switch (q_QmiLocMsgId)
   {
#if 0
      case QMI_LOC_SET_ENGINE_LOCK_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPa_SetEngineLock(l_ClientHandle, ((qmiLocSetEngineLockReqMsgT_v02*)p_QmiLocMsgData)->lockType);
         }
         else
         {
            LOC_MSG_ERROR("locPa_ProcessQmiRequest QMI_LOC_SET_ENGINE_LOCK_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_GET_ENGINE_LOCK_REQ_V02:
      {
         q_RetVal = locPa_GetParam(l_ClientHandle, PDSM_LITE_PA_GPS_LOCK);
         break;
      }
      
#endif 


      case QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPa_SetXtratSessionCtrl(l_ClientHandle, ((qmiLocSetXtraTSessionControlReqMsgT_v02*)p_QmiLocMsgData)->xtraTSessionControl);
         }
         else
         {
            LOC_MSG_ERROR("locPa_ProcessQmiRequest QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPa_GetParam(l_ClientHandle, PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL);
         }
         else
         {
            LOC_MSG_ERROR("locPa_ProcessQmiRequest QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }



      case QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPa_SetPeConfig(l_ClientHandle,
                                       (qmiLocSetPositionEngineConfigParametersReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locPa_ProcessQmiRequest QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      case QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
      {
         q_RetVal = locPa_GetParam(l_ClientHandle, PDSM_LITE_PA_PE_CONFIG_PARAM);
         break;
      }

     

      case QMI_LOC_SET_XTRA_VERSION_CHECK_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPa_SetXtraVersionCheck(l_ClientHandle,
                                             ((qmiLocSetXtraVersionCheckReqMsgT_v02*)p_QmiLocMsgData)->xtraVersionCheckMode);
         }
         else
         {
            LOC_MSG_ERROR("locPa_ProcessQmiRequest QMI_LOC_SET_XTRA_VERSION_CHECK_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      /*#9x05: SUPL Not supported*/ 

      case QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02:
      {
         if(NULL != p_QmiLocMsgData)
         {
            q_RetVal = locPa_DeleteGnssServiceData(l_ClientHandle,
                                       (qmiLocDeleteGNSSServiceDataReqMsgT_v02*)p_QmiLocMsgData);
         }
         else
         {
            LOC_MSG_ERROR("locPa_ProcessQmiRequest QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ NULL p_QmiLocMsgData",0,0,0);
         }
         break;
      }

      

      default:
      {
      LOC_MSG_ERROR("locPa_ProcessQmiRequest: unsupported QMI_LOC 0x00%x", q_QmiLocMsgId, 0, 0);
      break;
      }
   }

   if( LOC_API_SUCCESS == q_RetVal)
   {
      
      LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
      pz_LocPaData->v_PaCmdInProgress = TRUE;
      pz_LocPaData->q_PaCmdCounterIn++;
      pz_LocPaData->l_ClientHandle    = l_ClientHandle;
      pz_LocPaData->q_QmiLocMsgId     = q_QmiLocMsgId;
      LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

      os_TimerStart(pz_LocPaData->pz_ProcessTimer, LOC_PA_OPER_DEFAULT_TIMEOUT, 0);
     *pe_QmiErr = QMI_ERR_NONE_V01;
      v_RetVal = TRUE;
   }
   else
   {
      locPa_ResetModule();
      *pe_QmiErr = QMI_ERR_INVALID_ARG_V01;
      LOC_MSG_ERROR("locPa_ProcessQmiRequest: QMI_LOC 0x%04x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
   }

   return v_RetVal;
}/* locPa_ProcessQmiRequest */

static void locPa_EventCb
(
  void*                      p_UserData, /* user data pointer */
  pdsm_lite_pa_event_type         q_PaEvent,  /* Indicate PA event */
  const pdsm_lite_pa_info_s_type* pz_PaInfo    /* PARAM pz_PaInfo POINTER */
)
{
  locPa_ModuleDataType* pz_LocPaData = &(loc_middleware_data.z_locPa_Data);
  boolean               v_PaCmdInProgress;
  pdsm_lite_pa_e_type        e_ParamType;
  pdsm_lite_pa_cmd_e_type    e_PaCmdType;

  if (NULL == pz_PaInfo)
  {
    LOC_MSG_ERROR("locPa_EventCb: pz_PaInfo NULL parameter", 0, 0, 0);
    return;
  }

  /* Extract read copies of LocPa module data */
  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  v_PaCmdInProgress = pz_LocPaData->v_PaCmdInProgress;
  e_ParamType       = pz_LocPaData->e_ParamType;
  e_PaCmdType       = pz_LocPaData->e_PaCmdType;

  if (v_PaCmdInProgress)
  {
    LOC_MSG_HIGH("locPa_EventCb: PaCmdType=%d, ParamType=%d, PaEvent=0x%x", e_PaCmdType, e_ParamType, q_PaEvent);

    /* Send IPC message to LocMW task to handle QMI_LOC indication */
    locPa_SendQmiIndicationProxy((loc_client_handle_type)p_UserData, pz_PaInfo, PDSM_LITE_PA_CMD_ERR_NOERR);
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);
    return;
  }

  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);


  /* Event received when nobody is waiting for the event */
  LOC_MSG_ERROR("locPa_EventCb: ParamType=%d expected ParamType=%d v_PaCmdInProgress=%u",
                 pz_PaInfo->pa_event, e_ParamType, v_PaCmdInProgress);
}/* locPa_EventCb */

static void locPa_EventCbExt
(
  void*                      p_UserData,   /* user data pointer */
  pdsm_lite_pa_e_type             e_ParamType,  /* Indicate PDSM param type, PDSM_LITE_PA_xxx enum */
  const pdsm_lite_pa_info_s_type* pz_PaInfo,     /* A pointer to a PA state information struct */
  pdsm_lite_pa_cmd_err_e_type     e_PaCmdStatus /* Optional field to indicate success/failure status of PA operation */
)
{
  locPa_ModuleDataType* pz_LocPaData = &(loc_middleware_data.z_locPa_Data);
  boolean               v_PaCmdInProgress;
  pdsm_lite_pa_cmd_e_type    e_PaCmdType;

  if (NULL == pz_PaInfo)
  {
    LOC_MSG_ERROR("locPa_EventCbExt: pz_PaInfo NULL parameter", 0, 0, 0);
    return;
  }

  if ((e_ParamType != pz_PaInfo->pa_event) || (e_ParamType != pz_LocPaData->e_ParamType))
  {
    LOC_MSG_ERROR("locPa_EventCbExt: ParamType=%d pa_event=%d e_ParamType=%d mismatch",
                   e_ParamType, pz_PaInfo->pa_event, pz_LocPaData->e_ParamType);
  }

  /* Extract read copies of LocPa module data */
  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  v_PaCmdInProgress = pz_LocPaData->v_PaCmdInProgress;
  e_PaCmdType       = pz_LocPaData->e_PaCmdType;

  if (v_PaCmdInProgress)
  {
    LOC_MSG_HIGH("locPa_EventCbExt: PaCmdType=%d, ParamType=%d, PaCmdStatus=%d",
                  e_PaCmdType, e_ParamType, e_PaCmdStatus);

    /* Send IPC message to LocMW task to handle QMI_LOC indication */
    locPa_SendQmiIndicationProxy((loc_client_handle_type)p_UserData, pz_PaInfo, e_PaCmdStatus);
    LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);
    return;
  }

  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

  /* No PA command in progress, might be already returned by locPa_EventCb*/
  LOC_MSG_HIGH("locPa_EventCbExt: PaCmdType=%d, ParamType=%d, v_PaCmdInProgress=%u",
                 e_PaCmdType, pz_PaInfo->pa_event, v_PaCmdInProgress);
}/* locPa_EventCbExt */

static void locPa_CmdCb
(
  void*                  p_DataBlock,
  pdsm_lite_pa_cmd_e_type     e_PaCmdType,
  pdsm_lite_pa_cmd_err_e_type e_PaCmdStatus
)
{
  locPa_ModuleDataType* pz_LocPaData = &(loc_middleware_data.z_locPa_Data);
  boolean               v_PaCmdInProgress = FALSE;
  pdsm_lite_pa_e_type        e_ParamType = PDSM_LITE_PA_NONE;
  pdsm_lite_pa_info_s_type*  pz_ParamInfo = NULL;
  loc_client_handle_type l_ClientHandle;

  /* Extract read copies of LocPa module data */
  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  v_PaCmdInProgress = pz_LocPaData->v_PaCmdInProgress;
  e_ParamType       = pz_LocPaData->e_ParamType;
  l_ClientHandle = pz_LocPaData->l_ClientHandle;
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

  LOC_MSG_HIGH("locPa_CmdCb: PaCmdType=%d, ParamType=%d, PaCmdStatus=%d", e_PaCmdType, e_ParamType, e_PaCmdStatus);

  if (!v_PaCmdInProgress || (pz_LocPaData->e_PaCmdType != e_PaCmdType))
  {
    LOC_MSG_ERROR("locPa_CmdCb: v_PaCmdInProgress=%u or PaCmdType mismatch %d==%d",
                   v_PaCmdInProgress, e_PaCmdType, pz_LocPaData->e_PaCmdType);
  }
  else if (PDSM_LITE_PA_CMD_ERR_NOERR != e_PaCmdStatus) //|| (e_ParamType == PDSM_LITE_PA_DATA_ENABLE))
  {
    pz_ParamInfo = (pdsm_lite_pa_info_s_type*)loc_calloc(sizeof(*pz_ParamInfo));
    if(NULL == pz_ParamInfo)
    {
       LOC_MSG_ERROR("locPa_CmdCb: loc_calloc returned NULL",0,0,0);
    }
    else
    {
      pz_ParamInfo->client_id = l_ClientHandle;
      pz_ParamInfo->pa_event  = e_ParamType;
      LOC_MSG_ERROR("locPa_CmdCb: error, send indication", 0, 0, 0);
      locPa_SendQmiIndicationProxy(l_ClientHandle, pz_ParamInfo, e_PaCmdStatus);
      loc_free(pz_ParamInfo);
    }
  }

}/* locPa_CmdCb */

/*---------------------------------------------------------------------------
@brief
  Function to process Timer callback.

  When the timer fires, the location engine has not received the PA callback.
  Notify client of the failed status and reset the state.
---------------------------------------------------------------------------*/
void locPa_ProcessTimerCb(void)
{
  locPa_ModuleDataType* pz_LocPaData = &(loc_middleware_data.z_locPa_Data);
  pdsm_lite_pa_info_s_type*  pz_ParamInfo = NULL;

  LOC_MSG_HIGH("locPa_ProcessTimerCb: fired", 0, 0, 0);

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  if (pz_LocPaData->v_PaCmdInProgress)
  {
    pz_ParamInfo = (pdsm_lite_pa_info_s_type*)loc_calloc(sizeof(*pz_ParamInfo));
    if(NULL != pz_ParamInfo)
    {
      /* Notify client about the timeout by sending IPC message to LocMW task to handle QMI_LOC indication */
      pz_ParamInfo->client_id = pz_LocPaData->l_ClientHandle;
      pz_ParamInfo->pa_event  = pz_LocPaData->e_ParamType;
      locPa_SendQmiIndicationProxy(pz_LocPaData->l_ClientHandle, pz_ParamInfo, PDSM_LITE_PA_CMD_ERR_OTHER);
      loc_free(pz_ParamInfo);
    }
    else
    {
       LOC_MSG_ERROR("locPa_ProcessTimerCb: loc_calloc returned NULL",0,0,0);
    }

    /* Reset the PA module */
    pz_LocPaData->v_PaCmdInProgress = FALSE;
    pz_LocPaData->q_PaCmdCounterOut++;
    pz_LocPaData->l_ClientHandle    = LOC_CLIENT_HANDLE_INVALID;
  }
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

}

/*---------------------------------------------------------------------------
@brief
  Function to reset the PA module. This is called when in progress pa cmd is
  done. This is called when PA cmd error fails, or PA event arrives.

  NOTE: This function is always called within LocMW critical section.
---------------------------------------------------------------------------*/
void locPa_ResetModule(void)
{
  locPa_ModuleDataType* pz_LocPaData = &(loc_middleware_data.z_locPa_Data);

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  /* Reset the PA module */
  pz_LocPaData->v_PaCmdInProgress = FALSE;
  pz_LocPaData->q_PaCmdCounterOut++;
  pz_LocPaData->l_ClientHandle    = LOC_CLIENT_HANDLE_INVALID;
  os_TimerStop(pz_LocPaData->pz_ProcessTimer);
  LOC_MSG_MED("locPa_ResetModule PaCmdType=%d, ParamType=%d, QMI_LOC 0x%04x",
     pz_LocPaData->e_PaCmdType, pz_LocPaData->e_ParamType, pz_LocPaData->q_QmiLocMsgId);
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);
}

/*---------------------------------------------------------------------------
@brief
  Function to process the PDSM Set Parameters LocMW request

@param[in] l_ClientHandle : Loc API client handle
@param[in] e_ParamType    : PDSM PA parameter to set

@retval    LOC_API_SUCCESS  Called successfully
@retval    <Anything Else>  Called unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_SetParam
(
  loc_client_handle_type l_ClientHandle,
  pdsm_lite_pa_e_type         e_ParamType,
  pdsm_lite_pa_info_type*     pz_PaInfo
)
{
  uint32 q_RetVal = LOC_API_GENERAL_FAILURE;

  /* Function arguments sanity check */
  if (NULL == pz_PaInfo)
  {
    LOC_MSG_ERROR("locPa_SetParam: NULL parameters", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  loc_middleware_data.z_locPa_Data.e_ParamType = e_ParamType;
  loc_middleware_data.z_locPa_Data.e_PaCmdType = PDSM_LITE_PA_CMD_SET_PARAM;
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

  if (pdsm_lite_set_parameters(locPa_CmdCb,
                          (void*) l_ClientHandle,
                          e_ParamType,
                          pz_PaInfo,
                          loc_middleware_data.pdapi_client_id))
  {
    q_RetVal = LOC_API_SUCCESS;
  }
  else
  {
    q_RetVal = LOC_API_GENERAL_FAILURE;
  }

  LOC_MSG_LOW("locPa_SetParam: RetVal=%d, ParamType=%d", q_RetVal, e_ParamType, 0);
  return q_RetVal;
}


/*---------------------------------------------------------------------------
@brief
  Function to process the PDSM Get Parameters LocMW request

@param[in] l_ClientHandle : Loc API client handle
@param[in] e_ParamType    : PDSM PA parameter to get

@retval    LOC_API_SUCCESS  Called successfully
@retval    <Anything Else>  Called unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_GetParam
(
  loc_client_handle_type l_ClientHandle,
  pdsm_lite_pa_e_type         e_ParamType
)
{
  uint32 q_RetVal = LOC_API_GENERAL_FAILURE;

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.crit_sec);
  loc_middleware_data.z_locPa_Data.e_ParamType = e_ParamType;
  loc_middleware_data.z_locPa_Data.e_PaCmdType = PDSM_LITE_PA_CMD_GET_PARAM;
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.crit_sec);

  if (pdsm_lite_get_parameters(locPa_CmdCb,
                          (void*) l_ClientHandle,
                          e_ParamType,
                          loc_middleware_data.pdapi_client_id))
  {
    q_RetVal = LOC_API_SUCCESS;
  }
  else
  {
    q_RetVal = LOC_API_GENERAL_FAILURE;
  }

  LOC_MSG_LOW("locPa_GetParam: RetVal=%d, ParamType=%d", q_RetVal, e_ParamType, 0);
  return q_RetVal;
}

#if 0
/*---------------------------------------------------------------------------
@brief
  Function to process the Set Engine Lock QMI_LOC request

@param[in] l_ClientHandle : Loc API client handle
@param[in] e_EngineLock   : Engine Lock Type received from client

@retval    LOC_API_SUCCESS  Converted successfully
@retval    <Anything Else>  Converted unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_SetEngineLock
(
  loc_client_handle_type    l_ClientHandle,
   qmiLocLockEnumT_v02       e_EngineLock
)
{
   uint32             q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_lite_pa_info_type* pz_PaInfo = NULL;

   LOC_PA_ALLOC_PAINFO(pz_PaInfo)

  switch (e_EngineLock)
  {
    case eQMI_LOC_LOCK_NONE_V02:
         pz_PaInfo->gps_lock = PDSM_LITE_GPS_LOCK_NONE;
      break;

    case eQMI_LOC_LOCK_MI_V02:
         pz_PaInfo->gps_lock = PDSM_LITE_GPS_LOCK_MI;
      break;

    case eQMI_LOC_LOCK_MT_V02:
         pz_PaInfo->gps_lock = PDSM_LITE_GPS_LOCK_MT;
      break;

    case eQMI_LOC_LOCK_ALL_V02:
         pz_PaInfo->gps_lock = PDSM_LITE_GPS_LOCK_ALL;
      break;

    default:
      LOC_MSG_ERROR("locPa_SetEngineLock: Invalid Lock=%d", e_EngineLock, 0, 0);
         q_RetVal = LOC_API_INVALID_PARAMETER;
  }

  /* Set up global PA module structure with PDAPI specific info */
   if(LOC_API_INVALID_PARAMETER != q_RetVal)
   {
      q_RetVal = locPa_SetParam(l_ClientHandle, PDSM_LITE_PA_GPS_LOCK, pz_PaInfo);
   }
   LOC_PA_FREE_PAINFO(pz_PaInfo)

   LOC_MSG_LOW("locPa_SetEngineLock: RetVal=%d, e_QmiLock=%d, e_PdapiLock=%d", q_RetVal, e_EngineLock, pz_PaInfo->gps_lock);
   return q_RetVal;
}

#endif

/*#9x05: NMEA not supported*/





/*---------------------------------------------------------------------------
@brief
  Function to process the Set Position Engine Config QMI_LOC request

@param[in] l_ClientHandle : Loc API client handle
@param[in] p_SetPeCfg     : Position Engine Config received from client

@retval    LOC_API_SUCCESS  Converted successfully
@retval    <Anything Else>  Converted unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_SetPeConfig
(
  loc_client_handle_type                                    l_ClientHandle, 
  const qmiLocSetPositionEngineConfigParametersReqMsgT_v02* p_SetPeCfg
)
{
   uint32                     q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_lite_pa_info_type         *pz_PaInfo = NULL;
   pdsm_lite_pa_pe_config_s_type*  p_PeCfg  = NULL;

  /* Function arguments sanity check */
  if (NULL == p_SetPeCfg)
  {
    LOC_MSG_ERROR("locPa_SetPeConfig: NULL parameters", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

   LOC_PA_ALLOC_PAINFO(pz_PaInfo)
   p_PeCfg  = &(pz_PaInfo->pe_config_info);
  p_PeCfg->peConfigMask = 0;

  if (p_SetPeCfg->injectedPositionControl_valid &&
      LOC_MW_IS_VALID_BOOLEAN(p_SetPeCfg->injectedPositionControl))
  {
    p_PeCfg->peConfigMask       |= PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION;
    p_PeCfg->useInjectedPosition = p_SetPeCfg->injectedPositionControl;
  }
  else
  {
      q_RetVal = LOC_API_INVALID_PARAMETER;
  }

  if (p_SetPeCfg->filterSvUsage_valid &&
      LOC_MW_IS_VALID_BOOLEAN(p_SetPeCfg->filterSvUsage))
  {
    p_PeCfg->peConfigMask |= PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED;
    p_PeCfg->filterSvUsed  = p_SetPeCfg->filterSvUsage;
  }
  else
  {
      q_RetVal = LOC_API_INVALID_PARAMETER;
  }

 

  /* If at least one of the parameters is invalid, reject the entire message */
   if (LOC_API_INVALID_PARAMETER != q_RetVal)
  {
    /* Set up global PA module structure with PDAPI specific info */
      q_RetVal = locPa_SetParam(l_ClientHandle, PDSM_LITE_PA_PE_CONFIG_PARAM, pz_PaInfo);
  }
   LOC_PA_FREE_PAINFO(pz_PaInfo)

   LOC_MSG_MED("locPa_SetPeConfig: RetVal=%d", q_RetVal, 0, 0);
   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  Function to process the Set XTRA-T Session Control QMI_LOC request

@param[in] l_ClientHandle : Loc API client handle
@param[in] v_Control      : XTRA session control flag

@retval    LOC_API_SUCCESS   Converted successfully
@retval    <Anything Else>   Converted unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_SetXtratSessionCtrl
(
  loc_client_handle_type l_ClientHandle,
  uint8                  v_Control
)
{
   uint32             q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_lite_pa_info_type *pz_PaInfo = NULL;

   LOC_PA_ALLOC_PAINFO(pz_PaInfo)
   pz_PaInfo->session_control = v_Control;

  /* Set up global PA module structure with PDAPI specific info */
   q_RetVal = locPa_SetParam(l_ClientHandle, PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL, pz_PaInfo);

   LOC_MSG_MED("locPa_SetXtratSessionCtrl: RetVal=%d, SessionCtrl=%c",
            q_RetVal, pz_PaInfo->session_control, 0);
   LOC_PA_FREE_PAINFO(pz_PaInfo)

   return q_RetVal;
}








/*---------------------------------------------------------------------------
@brief
  Function to process the Set XTRA Version Check QMI_LOC request

@param[in] l_ClientHandle : Loc API client handle
@param[in] e_XtraVersion  : XTRA Version Check Configuration

@retval    LOC_API_SUCCESS   Converted successfully
@retval    <Anything Else>   Converted unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_SetXtraVersionCheck
(
  loc_client_handle_type                l_ClientHandle,
  qmiLocXtraVersionCheckEnumT_v02       e_XtraVersion
)
{
   uint32             q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_lite_pa_info_type *pz_PaInfo = NULL;

   LOC_PA_ALLOC_PAINFO(pz_PaInfo)
  switch (e_XtraVersion)
  {
    case eQMI_LOC_XTRA_VERSION_CHECK_DISABLE_V02:
         pz_PaInfo->xtra_version_check = PDSM_LITE_PA_XTRA_VERSION_CHECK_DISABLED;
      break;

    case eQMI_LOC_XTRA_VERSION_CHECK_AUTO_V02:
         pz_PaInfo->xtra_version_check = PDSM_LITE_PA_XTRA_VERSION_CHECK_AUTO;
      break;

    case eQMI_LOC_XTRA_VERSION_CHECK_XTRA2_V02:
         pz_PaInfo->xtra_version_check = PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA2;
      break;

    case eQMI_LOC_XTRA_VERSION_CHECK_XTRA3_V02:
         pz_PaInfo->xtra_version_check = PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA3;
      break;

    default:
      LOC_MSG_ERROR("locPa_SetXtraVersionCheck: Invalid XTRA version=%d",
                     e_XtraVersion, 0, 0);
         q_RetVal = LOC_API_INVALID_PARAMETER;
  }

  /* Set up global PA module structure with PDAPI specific info */
   if(LOC_API_INVALID_PARAMETER != q_RetVal)
   {
      q_RetVal = locPa_SetParam(l_ClientHandle, PDSM_LITE_PA_XTRA_VERSION_CHECK, pz_PaInfo);
   }
   LOC_PA_FREE_PAINFO(pz_PaInfo)

  LOC_MSG_MED("locPa_SetXtraVersionCheck: RetVal=%d, XTRA version=%d",
               q_RetVal, e_XtraVersion, 0);
   return q_RetVal;
}


/*---------------------------------------------------------------------------
@brief
  Function to convert Pa command error from PDAPI to QMI LOC format

@param[in] e_CmdStatus  : Command Status sent from PDSM

@retval    corresponding qmiLocStatusEnumT_v02 value
---------------------------------------------------------------------------*/
static qmiLocStatusEnumT_v02 locPa_ConvertCmdError2Qmi(pdsm_lite_pa_cmd_err_e_type e_CmdStatus)
{
  switch (e_CmdStatus)
  {
    case PDSM_LITE_PA_CMD_ERR_NOERR:
      return eQMI_LOC_SUCCESS_V02;

    case PDSM_LITE_PA_CMD_ERR_OFFLINE_S:
      return eQMI_LOC_PHONE_OFFLINE_V02;

    case PDSM_LITE_PA_CMD_ERR_PA_ACTIVE_S:
    case PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED:
      return eQMI_LOC_ENGINE_BUSY_V02;

      case PDSM_LITE_PA_CMD_ERR_OTHER:
         return eQMI_LOC_TIMEOUT_V02;
     case PDSM_LITE_PA_CMD_ERR_GNSS_SRVC_NOT_AVAILABLE:
         return eQMI_LOC_GNSS_SERVICE_NOT_AVAILABLE_V02;
    default:
         LOC_MSG_HIGH("pdsm_lite_pa_cmd_err=%d convert to eQMI_LOC_GENERAL_FAILURE", e_CmdStatus, 0, 0);
      return eQMI_LOC_GENERAL_FAILURE_V02;
  }
}

/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task for servicing Indication to previous
  QMI_LOC request.
  
  NOTE: This function is always called within LocMW critical section.

@param[in] l_ClientHandle : Loc API client handle
@param[in] e_ParamType    : Parameter Type sent from PDSM
@param[in] p_ParamInfo    : Parameter Information Response from PDSM
@param[in] e_ParamStatus  : Parameter Status sent from PDSM

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
static boolean locPa_SendQmiIndicationProxy
(
  loc_client_handle_type l_ClientHandle,
  const pdsm_lite_pa_info_s_type*   pz_ParamInfo,
  pdsm_lite_pa_cmd_err_e_type       e_ParamStatus
)
{
  os_IpcMsgType*       p_IpcMsg = NULL;
  locPa_PdapiIndMsg*   pz_IndMsg = NULL;

  /* Function parameters sanity check */
  if (NULL == pz_ParamInfo)
  {
    LOC_MSG_ERROR("locPa_SendQmiIndicationProxy: NULL parameters", 0, 0, 0);
    return FALSE;
  }

  /* Generate IPC message to LocMW */
  p_IpcMsg = os_IpcCreate(sizeof(*pz_IndMsg), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);
  if(NULL == p_IpcMsg)
  {
    LOC_MSG_ERROR("locPa_SendQmiIndicationProxy: os_IpcCreate failed", 0, 0, 0);
    return FALSE;
  }

  LOC_MSG_MED("locPa_SendQmiIndicationProxy: ClientHandle=%d, ParamType=%d, ParamStatus=%d",
               l_ClientHandle, pz_ParamInfo->pa_event, e_ParamStatus);

  p_IpcMsg->q_MsgId = LM_MIDDLEWARE_MSG_ID_QMI_LOC_PA_IND;
  pz_IndMsg = (locPa_PdapiIndMsg*)(p_IpcMsg->p_Data);
  pz_IndMsg->l_ClientHandle = l_ClientHandle;
  pz_IndMsg->e_PaCmdType    = loc_middleware_data.z_locPa_Data.e_PaCmdType; /* GET or SET Parameter */
  pz_IndMsg->z_ParamInfo    = *pz_ParamInfo;
  pz_IndMsg->e_ParamStatus  = e_ParamStatus;

  /* Generate IPC message to LocMW */
  if(FALSE == os_IpcSend(p_IpcMsg, THREAD_ID_LOC_MIDDLEWARE))
  {
    LOC_MSG_ERROR("locPa_SendQmiIndicationProxy: Sending IPC to LocMW failed", 0, 0, 0);
    os_IpcDelete(p_IpcMsg);
    return FALSE;
  }

  /* Client has been notified of the PA processing status, reset the PA module */
  locPa_ResetModule();

  return TRUE;
}

/*---------------------------------------------------------------------------
@brief
  Function to send QMI_LOC indication to client for previous QMI_LOC request
  related to PDAPI Get/Set_Parameters

@param[in] p_IpcMsg : IPC Message containing the QMI_LOC indication data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locPa_SendQmiIndication
(
  const os_IpcMsgType* p_IpcMsg
)
{
  qmiLocStatusEnumT_v02 e_IndStatus;

  if (NULL == p_IpcMsg)
  {
    LOC_MSG_ERROR("locPa_SendQmiIndication: Invalid IPC message", 0, 0, 0);
    return FALSE;
  }
  else
  {
    locPa_PdapiIndMsg* p_PdapiIndMsg = (locPa_PdapiIndMsg*)(p_IpcMsg->p_Data);

    /* Translate Indication Status from PDAPI to QMI_LOC type */
    e_IndStatus = locPa_ConvertCmdError2Qmi(p_PdapiIndMsg->e_ParamStatus);

    /* Determine if Indication is for GET or SET parameter */
    if (PDSM_LITE_PA_CMD_GET_PARAM == p_PdapiIndMsg->e_PaCmdType)
    {
      return locQmi_ProcessPaGetInd(p_PdapiIndMsg->l_ClientHandle,
                                    p_PdapiIndMsg->z_ParamInfo.pa_event,
                                   &(p_PdapiIndMsg->z_ParamInfo), e_IndStatus);
    }
    else if (PDSM_LITE_PA_CMD_SET_PARAM == p_PdapiIndMsg->e_PaCmdType)
    {
      return locQmi_ProcessPaSetInd(p_PdapiIndMsg->l_ClientHandle,
                                    p_PdapiIndMsg->z_ParamInfo.pa_event,
                                   &(p_PdapiIndMsg->z_ParamInfo), e_IndStatus);
    }
    else
    {
      LOC_MSG_ERROR("locPa_SendQmiIndication: Invalid PaCmdType", 0, 0, 0);
      return FALSE;
    }
  }
}




// Translate GNSS service data system & deleteSatelliteDataMask
static void locPa_XlateDeleteSatelliteData(
  const qmiLocDeleteSatelliteDataStructT_v02 *pz_DeleteSatelliteData,
  pdsm_lite_delete_parms_type* pz_Param)
{
   if(NULL == pz_Param || NULL == pz_DeleteSatelliteData)
   {
      LOC_MSG_ERROR("locPa_XlateDeleteSatelliteData, 0x%x, 0x%x",
                     pz_Param, pz_DeleteSatelliteData, 0);
      return;
   }

   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_GPS_V02))
   {
      LOC_MSG_MED("locPa_XlateDeleteSatelliteData, GPS, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_EPH;
         pz_Param->eph_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM;
         pz_Param->alm_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_HEALTH;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVDIR;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVSTEER;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_CORR;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GPS;
         pz_Param->gps_gnss_sv_blacklist_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SA_DATA_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SADATA;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SV_NO_EXIST_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SV_NO_EXIST;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TIME_GPS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_IONO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TGD_DATA_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TGD;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_GLO_V02))
   {
      LOC_MSG_MED("locPa_XlateDeleteSatelliteData, GLO, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_EPH_GLO;
         pz_Param->glo_eph_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_GLO;
         pz_Param->glo_alm_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_HEALTH_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVDIR_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVSTEER_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_CORR_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GLO;
         pz_Param->glo_gnss_sv_blacklist_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TIME_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_IONO_GLO;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TGD_DATA_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TGD;
      }   
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_BDS_V02))
   {
      LOC_MSG_MED("locPa_XlateDeleteSatelliteData, BDS, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_EPH_BDS;
         pz_Param->bds_eph_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_BDS;
         pz_Param->bds_alm_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_HEALTH_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVDIR_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVSTEER_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_CORR_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_BDS;
         pz_Param->bds_gnss_sv_blacklist_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TIME_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_IONO_BDS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TGD_DATA_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TGD;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_GAL_V02))
   {
      LOC_MSG_MED("locPa_XlateDeleteSatelliteData, GAL, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_EPH_GAL;
         pz_Param->gal_eph_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_GAL;
         pz_Param->gal_alm_sv_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVDIR_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVSTEER_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_CORR_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GAL;
         pz_Param->gal_gnss_sv_blacklist_mask = CGPS_MAX_UINT64;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TIME_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TIME_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_IONO_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_HEALTH_GAL;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TGD_DATA_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TGD;
      }
   }
   if(0 != (pz_DeleteSatelliteData->system & QMI_LOC_SYSTEM_QZSS_V02))
   {
      LOC_MSG_MED("locPa_XlateDeleteSatelliteData, QZSS, deleteSatelliteDataMask=0x%x",
                     pz_DeleteSatelliteData->deleteSatelliteDataMask, 0, 0);
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_EPH_QZSS;
         pz_Param->qzss_eph_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_QZSS;
         pz_Param->qzss_alm_sv_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_HEALTH_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVDIR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVDIR_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_SVSTEER_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_ALM_CORR_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_QZSS;
         pz_Param->qzss_gnss_sv_blacklist_mask = CGPS_MAX_UINT32;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_IONO_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_IONO_QZSS;
      }
      if(0 != (pz_DeleteSatelliteData->deleteSatelliteDataMask & QMI_LOC_DELETE_DATA_MASK_TGD_DATA_V02))
      {
         pz_Param->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_TGD;
      }
   }
}

//GNSS delete service data handler function
static uint32 locPa_XlateDeleteGnssServiceData
(
   const qmiLocDeleteGNSSServiceDataReqMsgT_v02  *pz_DeleteGnssServiceData,
   pdsm_lite_delete_parms_type                        *pz_PdapiDeleteDataMask
)
{
   uint32    q_RetVal = LOC_API_SUCCESS;

   if(NULL == pz_DeleteGnssServiceData || NULL == pz_PdapiDeleteDataMask)
   {
      LOC_MSG_ERROR("locPa_XlateDeleteGnssServiceData, 0x%x, 0x%x",
                    pz_DeleteGnssServiceData,pz_PdapiDeleteDataMask,0);
      return LOC_API_INVALID_PARAMETER;
   }

   memset(pz_PdapiDeleteDataMask, 0, sizeof(*pz_PdapiDeleteDataMask));

   if( FALSE == LOC_MW_IS_VALID_BOOLEAN(pz_DeleteGnssServiceData->deleteAllFlag))
   {
      LOC_MSG_ERROR("locPa_XlateDeleteGnssServiceData, invalid deleteAllFlag %d",
                     pz_DeleteGnssServiceData->deleteAllFlag,0,0);
      return LOC_API_INVALID_PARAMETER;
   }

   if(TRUE == pz_DeleteGnssServiceData->deleteAllFlag )
   {
      qmiLocDeleteSatelliteDataStructT_v02  z_DeleteSatellitData = {0};
      z_DeleteSatellitData.system = CGPS_MAX_UINT32;
      z_DeleteSatellitData.deleteSatelliteDataMask = CGPS_MAX_UINT32;

      // clock Info
      pz_PdapiDeleteDataMask->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_CLOCK_INFO;
      pz_PdapiDeleteDataMask->delete_clock_info_mask = PDSM_LITE_PA_DELETE_ALL_CLOCK_INFO;

      // cell db
#if 0 //Todo: We need to see if we require cell db or not
      pz_PdapiDeleteDataMask->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_CELLDB_INFO;
      pz_PdapiDeleteDataMask->delete_celldb_mask = CGPS_MAX_UINT32;
#endif
      // common data
      pz_PdapiDeleteDataMask->pdsm_lite_delete_parms_flags |=
         locPa_XlateQmiPdsmMask(z_QmiPdsmCommonMaskMap,
                                CGPS_MAX_UINT32,
                                sizeof(z_QmiPdsmCommonMaskMap)/sizeof(z_QmiPdsmCommonMaskMap[0]));

      // satellite data
      locPa_XlateDeleteSatelliteData(&z_DeleteSatellitData,
                                      pz_PdapiDeleteDataMask);

   }
   else
   {
       // TLV=0x10 - clock Info
      if(TRUE == pz_DeleteGnssServiceData->deleteClockInfoMask_valid)
      {
         pz_PdapiDeleteDataMask->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_CLOCK_INFO;
         pz_PdapiDeleteDataMask->delete_clock_info_mask =
         locPa_XlateQmiPdsmMask(z_QmiPdsmClockInfoMaskMap,
                                pz_DeleteGnssServiceData->deleteClockInfoMask,
                                sizeof(z_QmiPdsmClockInfoMaskMap)/sizeof(z_QmiPdsmClockInfoMaskMap[0]));
      }

      // TLV=0x11 - cell db
     if (TRUE == pz_DeleteGnssServiceData->deleteCellDbDataMask_valid)
     {
       pz_PdapiDeleteDataMask->pdsm_lite_delete_parms_flags |= PDSM_LITE_PA_DELETE_CELLDB_INFO;
       pz_PdapiDeleteDataMask->delete_celldb_mask =
       locPa_XlateQmiPdsmMask(z_QmiPdsmCellDBMaskMap,
                                pz_DeleteGnssServiceData->deleteCellDbDataMask,
                                sizeof(z_QmiPdsmCellDBMaskMap) / sizeof(z_QmiPdsmCellDBMaskMap[0]));
     }

      // TLV=0x12 - common data
      if(TRUE == pz_DeleteGnssServiceData->deleteCommonDataMask_valid)
      {
         pz_PdapiDeleteDataMask->pdsm_lite_delete_parms_flags |=
         locPa_XlateQmiPdsmMask(z_QmiPdsmCommonMaskMap,
                                    pz_DeleteGnssServiceData->deleteCommonDataMask,
                                    sizeof(z_QmiPdsmCommonMaskMap)/sizeof(z_QmiPdsmCommonMaskMap[0]));
      }

      // TLV=0x13 - satellite data
      if(TRUE == pz_DeleteGnssServiceData->deleteSatelliteData_valid)
      {
         locPa_XlateDeleteSatelliteData(&pz_DeleteGnssServiceData->deleteSatelliteData,
                                             pz_PdapiDeleteDataMask);
      }
   }

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  Function to process the Delete GNSS Service Data QMI_LOC request

@param[in] l_ClientHandle           : Loc API client handle
@param[in] pz_DeleteGnssServiceData : Delete GNSS Service Data

@retval    LOC_API_SUCCESS   Converted successfully
@retval    <Anything Else>   Converted unsuccessfully
---------------------------------------------------------------------------*/
static uint32 locPa_DeleteGnssServiceData
(
   loc_client_handle_type l_ClientHandle,
   qmiLocDeleteGNSSServiceDataReqMsgT_v02* pz_DeleteGnssServiceData
)
{
   uint32                       q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_lite_pa_info_type           *pz_PaInfo = NULL;

   LOC_PA_ALLOC_PAINFO(pz_PaInfo)
   /* Skp input argument NULL check because it is done in locPa_XlateDeleteGnssServiceData */
   q_RetVal = locPa_XlateDeleteGnssServiceData (pz_DeleteGnssServiceData, &(pz_PaInfo->delete_params));

   if (q_RetVal == LOC_API_SUCCESS)
   {
      /* Set up global PA module structure with PDAPI specific info */
      q_RetVal = locPa_SetParam(l_ClientHandle, PDSM_LITE_PA_DELETE_PARAMS_EX1, pz_PaInfo);
   }
   LOC_PA_FREE_PAINFO(pz_PaInfo)

   LOC_MSG_LOW("locPa_DeleteGnssServiceData: RetVal=%d", q_RetVal, 0,0);
   return q_RetVal;
}

/****************************loc_pd function definition-end******************************/                    

