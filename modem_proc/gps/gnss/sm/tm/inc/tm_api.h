#ifndef TM_API_H
#define TM_API_H


/*===========================================================================
  
                            TM_API
                   
DESCRIPTION:
  This header file contains TM API interface related constants, variables
  and function protocols.

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2021 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/inc/tm_api.h#4 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  01/08/20    sai     Added API to query SM if GTP 2.0 fix is possible or not
  29/04/20    ska     Added changes for asking less proc duration when EPH is valid
  07/04/19    py      Retry again If single shot session fails due to MGP load fail
  06/28/19   katta    added API to provide timeout value to TLE
  04/01/19    nath    Added debug NMEA related code.
  10/25/17    rn      Add IPC to indicate XTRA EFS in GPS FS task has completed.
  02/17/17    skm     BLE and Barometer support for LPPE Phase 2
  08/10/16    mj     Add new IPC for leap second updates from ME
  06/06/16    mj     Add GERA support for AP-MP TTR
  02/17/16    mj      Added NV support to enable/disable CPI injections and early exit logic
  10/26/15    ss      Supporting Geofence Client Config indication
  08/13/15    hs      Unified fix reporting API implementation
  05/01/15    rh      Added get gps lock TM API
  04/28/14    ah      Added handling of Set Premium Services indication status
  01/28/13    mj      Modified tm_api_reg_task_init_notifier to add new param Client thread id
  01/10/13    gk      LPP ECID changes
  11/06/12    mj      Updated NV_CGPS_UTC_GPS_TIME_OFFSET_DEFAULT to 16
  08/01/12    mjohn   Added TM_CORE_MSG_ID_LSMP_GET_PARAM and TM_CORE_MSG_ID_LSMP_SET_PARAM msg id
  04/18/12    ssu     Adding sm_PositionReportStructType for GPS state information.
  03/01/12    rh      Added TM API tm_api_send_engine_error_recovery_report()
  10/10/11    rh      Changed XTRA SNTP servers to time.gpsonextra.net
  06/10/09    gk      ODP 2.0 support
  06/10/09    gk      Added Get UTC offset from NV
  01/12/09    atien   XTRA-T support    
  09/15/08    gk      Rename the on-demand struct to sm_ from cgps_
  04/25/08    gk      on-demand changes
  07/10/07    LT      Initial check-in.
===========================================================================*/
#include "pdapi_lite.h" //adding to make sm code work
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "gps_common.h"
#include "gnss_common.h"
#include "gnss_consts.h"
#include "gnss_nv_loader.h"
#include "sm_api.h"
#include "sm_loc_lte_otdoa.h"
#include "wl_common.h"
#include "location_service_iot_v02.h"
#include "pdapi_lite.h"	 //To do: Remove this once pdapi_lite.h is completely removed

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------

    Timer ID range allocation for each modules

---------------------------------------------------*/
/* Each TM software module is assigned a timer ID range */
#define TM_CORE_TIMER_ID_RANGE                0x00000000
#define TM_STANDALONE_TIMER_ID_RANGE          0x01000000
#define TM_GM_TIMER_ID_RANGE                  0x02000000
#define TM_LM_TIMER_ID_RANGE                  0x03000000
#define TM_LOCMW_TIMER_ID_RANGE               0x04000000
#define TM_PDAPI_TIMER_ID_RANGE               0x06000000
#define TM_DIAG_TIMER_ID_RANGE                0x07000000
#define TM_XTRA_TIMER_ID_RANGE                0x0A000000

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               DEFINES                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define SM_GEN8_IPC_ID_START_RANGE 0x10000 
#define TM_CORE_MSG_ID_FIRST 0x4000 
#define LM_MIDDLEWARE_MSG_ID_FIRST 0x2000
#define GM_CORE_MSG_ID_FIRST 0x6000

typedef enum //sm_apis started
{
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_0 = SM_GEN8_IPC_ID_START_RANGE,

  /****************************************************/
  /********** GEN8 MessageId Range Start **************/
  /****************************************************/
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_3=65572,   /* Faulty CPI Reporting API */
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_1, /* Deprecated */
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1=65575,
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_1,
  M_ARIES_MSG_ID_MGP_LM_XO_OFFSET_INFO,
  M_SM_MSG_ID_LM_MGP_REPORT_XTRA_INTEGRITY_0,
  M_SM_MSG_ID_MGP_LM_REPORT_XTRA_INTEGRITY_INJ_DONE_0,
  M_C_SM_LM_IPC_MSG_END,
}m_lm_ipc_msg_id_type;

  typedef enum //sm_apis started
{
  /*****************
  LM
  ***************/
  LM_MSG_ID_FIRST = C_USR_MSG_ID_FIRST,
  ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_0=11, // 11 used to parse tools 
  ARIES_MSG_ID_MGP_LM_REPORT_FIX_0=12,//12 used to parse tools 
  ARIES_MSG_ID_MGP_LM_REPORT_UTC_INFO,
  ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS,
  ARIES_MSG_ID_MGP_LM_REPORT_EVENT=18,//18 used to parse tools 
  ARIES_MSG_ID_MGP_LM_REPORT_FIX_1 =25,//25 used to parse tools 
  ARIES_MSG_ID_MGP_LM_REPORT_FIX_2 =27,//27 used to parse tools 
  ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS,
  ARIES_MSG_ID_MGP_LM_NAV_CONFIG_CHANGE_RESP,
  ARIES_MSG_ID_MGP_LM_END = 32,

  /*RF test related commands, Msg id calcullated in tm_diag_sarf_cmd_handler()*/
  ARIES_MSG_ID_TM_LM_FIRST = ARIES_MSG_ID_MGP_LM_END, // 1000=guard band between sm_api and lm_api
  ARIES_MSG_ID_TM_LM_SARF_MODE_SWITCH,
  ARIES_MSG_ID_TM_LM_PRESC_DWELL,
  ARIES_MSG_ID_TM_LM_IQ_TEST,
  ARIES_MSG_ID_TM_LM_1SV_STANDALONE, /* Also serves single sv */
  ARIES_MSG_ID_TM_LM_MEM_BW_TEST,
  ARIES_MSG_ID_TM_LM_TSG_CONFIG,
  ARIES_MSG_ID_TM_LM_RF_CONFIG,
  ARIES_MSG_ID_TM_LM_RFDEV_TEST, /* Callisto RF Dev Multichan Track test command message */
  ARIES_MSG_ID_TM_LM_PRX_RF_CONFIG,
  ARIES_MSG_ID_TM_LM_RF_LINEARITY_CONFIG,
  ARIES_MSG_ID_TM_LM_RF_DEV_FEATURE_CAPABILITIES, /* Query for build's RF Dev feature capabilities*/  
  ARIES_MSG_ID_TM_LM_GET_LATEST_WB_FFT_RESULTS,
  /* Following enums are reserved for OEMs usage */
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_1,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_2,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_3,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_4,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_5,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_6,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_7,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_8,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_9,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_10,
  ARIES_MSG_ID_TM_LM_SPECTRUM_ANALYZER_CMD,
  ARIES_MSG_ID_TM_LM_PFA_TEST, /*Dont delet it, Needed to maintain script compatibilty*/
  ARIES_MSG_ID_TM_LM_NAV_SET_TEST_MODE_CMD,
  ARIES_MSG_ID_TM_LM_NAV_IOCTL_CMD,                                
  ARIES_MSG_ID_TM_LM_NAV_CONFIG_CMD,
#ifdef FEATURE_GNSS_LTE_CATM_COEX
  ARIES_MSG_ID_TM_LM_LTE_CATM_TX_START,
  ARIES_MSG_ID_TM_LM_LTE_CATM_TX_END,  
#endif
  ARIES_MSG_ID_TM_LM_BP_JAMMER_CTRL_CMD,
  ARIES_MSG_ID_TM_LM_END,
  
  C_SM_LM_MSG_FIRST = ARIES_MSG_ID_TM_LM_END, //1000=guard band between lm_iface.h and lm_api 
  C_SM_LM_MSG_SESSION_REQUEST,    /* LM-TM session related request from TM to LM */
  C_SM_LM_MSG_REQUEST,            /* LM-TM session independent request from TM to LM */
  C_SM_LM_MSG_INFO,               /* LM-TM session independent info from TM to LM */
  C_SM_LM_DIAG_GSC_MSG,           /* GPS Session Control (GSC) DIAG packet from GPSDIAG to LM */ 
  C_SM_LM_SV_NO_EXIST,            /* Sv No Exist List updated from TM to LM */
  C_SM_LM_FORCE_DPO_FOR_POWER_MEAS,/* Force DPO for Power Meas Cmd from TM to LM */
  C_SM_LM_MSG_ARB_LOAD_ACCEPT,
  C_SM_LM_MSG_MGP_INITED_IND,
  C_SM_LM_MSG_MGP_DEINITED_IND,
  C_SM_LM_MSG_ARB_NOTIFY_IND,
  C_SM_LM_MSG_ARB_UNLOAD_IND,
  C_SM_LM_MSG_SESSION_CLEANUP_NEEDED,
  C_SM_LM_MSG_TEST_trigger_mode_switch_enter_sarf,
  C_SM_LM_MSG_END,
  
  ARIES_MSG_ID_MGP_LM_REPORT_FIX_3,
  SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE,
  SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE, //  TODO : [an][modify] f]hard coded to make sm compile, need to align later
  SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS,
  ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE ,  /* Deprecated */
  ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME ,
  ARIES_MSG_ID_MGP_LM_XO_OFFSET_INFO ,
  SM_MSG_ID_LM_MGP_REPORT_XTRA_INTEGRITY        = M_SM_MSG_ID_LM_MGP_REPORT_XTRA_INTEGRITY_0,
  SM_MSG_ID_MGP_LM_REPORT_XTRA_INTEGRITY_INJ_DONE = M_SM_MSG_ID_MGP_LM_REPORT_XTRA_INTEGRITY_INJ_DONE_0,
  LM_MSG_ID_END ,

  
/*************************************
LOC_MW
****************************************/
  LM_MIDDLEWARE_MSG_ID_PDAPI = LM_MIDDLEWARE_MSG_ID_FIRST,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_GEN_ALERT_IND ,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_BREACH_IND,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_ADD_IND,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_DELETE_IND ,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_SET_ENGINE_CONFIG_IND ,
  LM_MIDDLEWARE_MSG_ID_SDP_EVENT_VEHICLE_SENSOR_INJECTION_READINESS_IND,
  LM_MIDDLEWARE_MSG_ID_SDP_INJECT_VEHICLE_SENSOR_DATA_IND ,
  LM_MIDDLEWARE_MSG_ID_GTP_CLIENT_DLD_DATA_IND,
  LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_IND , /* MP sending indication to start a session */
  LM_MIDDLEWARE_MSG_ID_GDT_END_IND , /* MP sending indication to end a session */
  LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_REQ_IND , /* Ind resp. to the request received from AP */
  LM_MIDDLEWARE_MSG_ID_GDT_END_REQ_IND , /* Ind resp. to therequest received from AP */
  LM_MIDDLEWARE_MSG_ID_WLE_APCACHE_REQ_IND , /* Ind resp. to APCACHE request received from AP */
  LM_MIDDLEWARE_MSG_ID_WLE_NOAPCACHE_REQ_IND , /* Ind resp. to NOAPCACHE request received from AP */
  LM_MIDDLEWARE_MSG_ID_SEND_FDCL_SERVICE_REQ_IND,
  LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL_BS_LIST_IND,
  LM_MIDDLEWARE_MSG_ID_SEND_INJECT_FDCL_DATA_IND,
  LM_MIDDLEWARE_MSG_ID_SEND_FDCL2_SERVICE_REQ_IND, /* Service Request indicating MP has data to upload */
  LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND,
  LM_MIDDLEWARE_MSG_ID_QMI_LOC_MW_IND ,
  LM_MIDDLEWARE_MSG_ID_UNUSED_IND ,
  LM_MIDDLEWARE_MSG_ID_QMI_LOC_PA_IND ,
  LM_MIDDLEWARE_MSG_ID_AON_MIN,
  LM_MIDDLEWARE_MSG_ID_AON_MAX = LM_MIDDLEWARE_MSG_ID_AON_MIN +16,
  LM_MIDDLEWARE_MSG_ID_AON_STATUS_CB, 
  LM_MIDDLEWARE_MSG_ID_AON_LOCATION_CB,
  LM_MIDDLEWARE_MSG_ID_GDT2_MIN ,
  LM_MIDDLEWARE_MSG_ID_GTP_AP_STATUS_REQ_IND ,
  LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_READY_STATUS_IND,
  LM_MIDDLEWARE_MSG_ID_GDT_RECEIVE_DONE_STATUS_IND,
  LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_END_STATUS_IND,
  LM_MIDDLEWARE_MSG_ID_GDT_RECEIVE_DONE_REQ_IND,
  LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_END_REQ_IND ,
  LM_MIDDLEWARE_MSG_ID_GDT2_MAX = LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_END_REQ_IND,
  LM_MIDDLEWARE_MSG_ID_CROWDSOURCE_MANAGER_MIN ,
  LM_MIDDLEWARE_MSG_ID_CROWDSOURCE_MANAGER_CONTROL_IND,
  LM_MIDDLEWARE_MSG_ID_QUERY_GNSS_ENERGY_CONSUMED,
  LM_MIDDLEWARE_MSG_ID_PSM_ENTRY_IND,
  LM_MIDDLEWARE_MSG_ID_OPPORTUNISTIC_SCHEDULE_IND,
  LM_MIDDLEWARE_MSG_ID_CROWDSOURCE_MANAGER_MAX = LM_MIDDLEWARE_MSG_ID_OPPORTUNISTIC_SCHEDULE_IND,

  /*************************************
  SLIM ID's to be added between LM_MIDDLEWARE_MSG_ID_SLIM_MIN & LM_MIDDLEWARE_MSG_ID_SLIM_MAX
  ****************************************/

  LM_MIDDLEWARE_MSG_ID_SLIM_MIN,
  LM_MIDDLEWARE_MSG_ID_SLIM_MAX = LM_MIDDLEWARE_MSG_ID_SLIM_MIN +999,
  LM_MIDDLEWARE_MSG_ID_SEND_GET_WWAN_DATA_IND,

  LM_MIDDLEWARE_MSG_ID_END , // newly added as a part of 9x05

/*************************************
TM
****************************************/
TM_CORE_MSG_ID_DIAG=TM_CORE_MSG_ID_FIRST ,  /* 0x1027 Forward to Diag Module */
/* TM-Core TM-LM API MSG */
TM_CORE_MSG_ID_LFM_REG_CB, 

TM_CORE_MSG_ID_SESS_REQ_API,              /* 0x1028 IPC MSG for TM-LM Session Request API */
TM_CORE_MSG_ID_PDAPI ,                     /* 0x1031 IPC MSG for PD API */
TM_CORE_CM_MSG_ID_CMD_NTFY,            //0x1051
TM_CORE_CM_MSG_ID_EVENT_NTFY,          //0x1052
TM_CORE_RR_MSG_ID_EVENT_NTFY,          //0x1060
TM_CORE_MSG_ID_XTRA_GTS_TIME_REQUEST,   
TM_CORE_MSG_ID_XTRA_INJECT_TO_MGP_REQUEST,

/* IPC MSG for TM-LM Session Update API */
  TM_CORE_MSG_ID_SESS_UPDATE_API,
/* IPC MSG for TM-LM Session Info API */
  TM_CORE_MSG_ID_SESS_INFO_API,
/* IPC MSG for XTM (xtrat) - TM to report success of NV write*/
  TM_CORE_MSG_ID_XTM_CLIENT_TOKEN,
  TM_CORE_MSG_ID_XTM_SESSION_CONTROL,
/* TM-Core Geofence MSG */
  TM_CORE_GM_MSG_ID_BASE, 
  TM_CORE_GM_MSG_ID_ADD_RESPONSE=  TM_CORE_GM_MSG_ID_BASE ,
  TM_CORE_GM_MSG_ID_EDIT_NOTIFY,
  TM_CORE_GM_MSG_ID_DELETE_RESPONSE,
  TM_CORE_GM_MSG_ID_PURGE_RESPONSE, 
  TM_CORE_GM_MSG_ID_BREACH_NOTIFY, 
  TM_CORE_GM_MSG_ID_POS_REQ, 
  TM_CORE_GM_MSG_ID_CLIENT_BLOB_NOTIFY,
  TM_CORE_GM_MSG_ID_CLIENT_GEOFENCE_HANDELS_NOTIFY,
  TM_CORE_GM_MSG_ID_GEOFENCE_HANDELS_NOTIFY,
  TM_CORE_GM_MSG_ID_QUERY_NOTIFY,
  TM_CORE_GM_MSG_ID_UNSOL_EVENT_NOTIFY, 
  TM_CORE_GM_MSG_ID_PRIO_POS_REQ_INDICATION_NOTIFY,
  TM_CORE_GM_MSG_ID_SET_ENGINE_CONFIG_NOTIFY, 
  TM_CORE_GM_MSG_ID_ABORT_POS_REQ, 
  TM_CORE_GM_MSG_ID_BATCH_BREACH_NOTIFY, 
  TM_CORE_GM_MSG_ID_PROXIMITY_NOTIFY, 
  TM_CORE_GM_MSG_ID_REPLACE_NOTIFY,
  TM_CORE_GM_MSG_ID_BATCH_DWELL_NOTIFY,
  TM_CORE_GM_MSG_ID_TZ_REQ_NOTIFY, 
  TM_CORE_GM_MSG_ID_SET_CLIENT_CONFIG_NOTIFY, 
  TM_CORE_GM_MSG_ID_CTXT_CHANGE_NOTIFY,
  TM_CORE_GM_MSG_ID_LAST,
  TM_CORE_MGP_WBIQ_INFO, 
/* Best available position Report */
  TM_CORE_MSG_ID_BEST_AVAIL_POS_REPORT,
/* Engine DPO status Report */
  TM_CORE_MSG_ID_ENGINE_DPO_STATUS_REPORT,
/* LPP Control Plane Protocol Related */
  TM_CORE_MSG_ID_NAS_EVENT_BASE,
/* Request to retrieve IMSI */
  TM_CORE_MSG_ID_RETRIEVE_IMSI,
/* TM IPC to re-read carrier-specific NV  */
  TM_CORE_MSG_ID_GNSS_QUAL_IND,
/* TM API to set Leap Second Info */
  TM_CORE_MSG_ID_LEAP_SEC_INFO,
/* TM API to receive the result of NAV CONFIG Change command */
  TM_CORE_MGP_NAV_CONFIG_CHANGE_RESP,
/* TM API to receive NMEA string, which will be sent to HLOS for debugging purpose */
  TM_CORE_MSG_ID_NMEA_DEBUG_INFO,
/* TM API to handle XO offset info from MC */
  TM_CORE_MSG_ID_XO_OFFSET_INFO,
/* TM API to receive AS bit from MC */
 TM_CORE_MSG_ID_GPS_AS_BIT,
 /* TM API to request deletion of XTRA data  */
 TM_CORE_MSG_ID_DELETE_XTRA_DATA,
 /* TM API to request deletion of XTRA integrity data  */
 TM_CORE_MSG_ID_DELETE_XTRA_INTEGRITY_DATA,
 TM_CORE_MSG_ID_LFM_CONF_INFO,
 TM_CORE_MSG_ID_UPDATE_FEATURE_STATUS,
 /*TM api to update if receiver is in US or not*/
 TM_CORE_MSG_ID_RCVR_IN_US_STATUS,
/*TM API to receive cell scan from cm */
 TM_CORE_MSG_ID_TLE_PLMN_CELL_SCAN_REQ,
 /* TM API to recieve EPH expiry time from PE */
 TM_CORE_MSG_ID_EPH_EXPIRY_TIME,
 TM_CORE_MSG_ID_END,
  /*************************************
  GM
  ****************************************/
  GM_CORE_ADD_GEOFENCE = GM_CORE_MSG_ID_FIRST,
  GM_CORE_EDIT_GEOFENCE,
  GM_CORE_DELTE_GEOFENCE,
  GM_CORE_PURGE_GEOFENCE,  
  GM_CORE_REPLACE_GEOFENCE,
  GM_CORE_UPDATE_CELL_DB,
  GM_CORE_UPDATE_POSITION,
  GM_CORE_CLIENT_REGISTER_NOTIFY,
  GM_CORE_BREACH_ACK_NOTIFY,
  GM_CORE_GET_CLIENT_GEOFENCE_HANDLES,
  GM_CORE_GET_GEOFENCE_CLIENT_BLOB,  
  GM_CORE_GET_GEOFENCE_HANDLES,
  GM_CORE_QUERY_GEOFENCE_STATUS,
  GM_CORE_RRC_INFO_CELL_DCH_REPORT,
  GM_CORE_GPS_LOCK_STATE_UPDATE,
  GM_CORE_DPO_STATE_UPDATE,
  GM_CORE_API_CALLBACK,
  GM_CORE_INJECT_AMD_IND,
  GM_CORE_INJECT_RMD_IND,
  GM_CORE_INJECT_PED_IND,
  GM_CORE_INJECT_CMC_DB_IND,
  GM_CORE_INJECT_WIFI_SCAN_IND,
  GM_CORE_UPDATE_FIX_FAIL,
  GM_CORE_SET_GEOFENCE_ENGINE_CONFIG,
  GM_CORE_UPDATE_FIX_REQUEST_FAIL,
  GM_CORE_UPDATE_PD_EVENT,
  GM_CORE_INJECT_SMD_IND,
  GM_CORE_UPDATE_SRV_SYS,
  GM_CORE_WCDMA_CELL_MEAS_UPDATE,
  GM_CORE_LTE_CELL_MEAS_UPDATE,
  GM_CORE_PERIODIC_BEST_AVAIL_POS_REQ_TIMER,
  GM_CORE_INJECT_WIFI_ATTACH_STATUS_IND,
  GM_CORE_INJECT_WIFI_ENABLED_STATUS_IND,
  GM_CORE_INJECT_LOWI_SUBSCRIPTION_EVENT,
  GM_CORE_INJECT_ALS_IOD_IND,
  GM_CORE_DWELL_REPORT_TIMER,
  GM_CORE_COMP_LOGGING_TIMER,
  GM_CORE_IN_MEM_LOGGING_START,
  GM_CORE_IN_MEM_LOGGING_STOP,
  GM_CORE_IN_MEM_LOGGING_PERIOD_TIMER,
  GM_CORE_INJECT_TIME_ZONE_INFO,  
  GM_CORE_SET_GEOFENCE_CLIENT_CONFIG,
  GM_CORE_GNSS_ENGINE_STATE_UPDATE,
  GM_CORE_REGISTER_WIFI_SIGNATURE_CHANGE,
  GM_CORE_DEREGISTER_WIFI_SIGNATURE_CHANGE,
  GM_CORE_DRIVE_CTXT_CHANGE_WAIT_TIMER,
  GM_CORE_REGISTER_CTXT_CHANGE,
  GM_CORE_DEREGISTER_CTXT_CHANGE,
  GM_CORE_CTXT_CHANGE_GENERIC_TIMER,
  GM_CORE_DRIVE_PREDICTOR_DECREMENT_SET_CHANGE_TIMER,
  GM_CORE_TIME_ID_GTP_POS_WAIT,
  GM_CORE_UPDATE_MGP_LOAD_AVAIL,
  GM_CORE_MSG_ID_END,

  
 /***Block for tools*/
  ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1 = SM_GEN8_IPC_ID_START_RANGE,
  ARIES_MSG_ID_MGP_LM_REPORT_FIX = M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_3,
  ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS = M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1,
  TM_CORE_MSG_ID_ENGINE_ERROR_RECOVERY_REPORT = (SM_GEN8_IPC_ID_START_RANGE + 58)
} tm_ipc_msg_id_type;


typedef enum
{
  GM_CORE_TIMER_ID_FIRST = TM_GM_TIMER_ID_RANGE,
  GM_CORE_TIMER_ID_BACKOFF=GM_CORE_TIMER_ID_FIRST,
  GM_CORE_TIMER_ID_ACTIVITY,
  GM_CORE_TIMER_ID_GNSS_POS_WAIT,
  GM_CORE_TIME_ID_WIFI_POS_WAIT,
  GM_CORE_TIMER_ID_RRC_CELL_ID_REQUEST_RETRY,
  GM_CORE_TIMER_ID_PRIOR_POS_REQ_INDICATION,
  GM_CORE_TIMER_ID_EBEE_TIMER_ID_FIRST,
  GM_CORE_TIMER_ID_EBEE_TIMER_ID_LAST = GM_CORE_TIMER_ID_EBEE_TIMER_ID_FIRST + 1000,
  GM_CORE_TIMER_ID_API_CALLBACK,
  GM_CORE_TIMER_ID_GNSS_UNAVAIL_INDICATION,
  GM_CORE_TIMER_ID_WIFI_POS_REQ_BACKOFF_TIMER,
  GM_CORE_TIMER_ID_BATCH_BREACH_COLLECT_TIMER,
  GM_CORE_TIMER_ID_BATCH_BREACH_IPC_INACTIVE_TIMER,
  GM_CORE_TIMER_ID_LOWI_REG_TIMER,
  GM_CORE_TIMER_ID_LOWI_WLAN_STATUS_QUERY_TIMER,
  GM_CORE_TIMER_ID_LOWI_REG_LONG_TERM_TIMER,
  GM_CORE_TIMER_ID_NONQMI_GNSS_UNAVAIL_INDICATION,
  GM_CORE_TIMER_ID_END,

  LM_TIMER_ID_FIRST =TM_LM_TIMER_ID_RANGE,
  LM_FIX_TIMER_ID=LM_TIMER_ID_FIRST,
  LM_SESSION_TIMER_ID,
  LM_GOING_ON_TIMER_ID,
  LM_GOING_OFF_TIMER_ID,
  LM_GOING_IDLE_TIMER_ID,
  LM_RC_ON_TIMER_ID,
  LM_MGP_SHUT_OFF_TIMER_ID,
  LM_IQ_TESTING_TIMER_ID,
  LM_AFLT_DEFERRED_OFF_TIMER_ID,
  LM_POWER_TEST_GPS_START_TIMER_ID,
  LM_GOING_LPPM_TIMER_ID,
  LM_MGP_LOAD_ATTEMPT_BACK_OFF_TIMER_ID,
  LM_DMM_LOAD_ARB_REQ_WAIT_TIMER_ID,
  LM_MGP_LOAD_TIMER_ID,
  LM_MGP_UNLOAD_TIMER_ID,
  LM_TIMER_ID_END,

  LOC_MIDDLEWARE_TIMER_ID_FIRST =TM_LOCMW_TIMER_ID_RANGE,
  LOC_MIDDLEWARE_TIMER_ID_FIX =LOC_MIDDLEWARE_TIMER_ID_FIRST, 
  LOC_MIDDLEWARE_TIMER_ID_XTRA ,
  LOC_MIDDLEWARE_TIMER_ID_PA  ,
  LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN ,
  LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS = LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN+999,
  LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MAX =LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS,


  LOC_MIDDLEWARE_TIMER_ID_SLIM_MIN ,
  LOC_MIDDLEWARE_TIMER_ID_SLIM_MAX =LOC_MIDDLEWARE_TIMER_ID_SLIM_MIN+999,


  TM_CORE_TIMER_ID_FIRST = TM_CORE_TIMER_ID_RANGE,
  TM_CORE_SESS_TIMER_ID_DIAG=TM_CORE_TIMER_ID_FIRST,
  TM_CORE_TIMER_ID_DELETE_THROTTLE_ID,
  TM_CORE_GTP_WWAN_FIX_TIMER_ID,
  TM_CORE_GTS_SYS_UNC_WAIT_TIMER_ID,
  TM_CORE_TIMER_ID_END,


}common_clockids;

/*--------------------------------------------------

           IPC MSG ID definition

---------------------------------------------------*/

/* TM API to indicate XTRA EFS GPS FS task is done. */
#define TM_CORE_MSG_ID_XTRA_EFS_GPS_FS_TASK_DONE          (SM_GEN8_IPC_ID_START_RANGE + 109)
/*
 * Default NV Item Value for XTRA
 */
#define NV_AAGPS_DEFAULT_XTRA_DOWNLOAD_INTERVAL         (48)     // hour
#define NV_AAGPS_DEFAULT_XTRA_NUM_DOWNLOAD_ATTEMPTS     (3)
#define NV_AAGPS_DEFAULT_XTRA_TIME_BETWEEN_ATTEMPTS     (10)     // minute
 
#define NV_AAGPS_DEFAULT_XTRA_SERVER_URL_SIZE           (128)

/* Default NV Item Value for XTRA Time Information */
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_ENABLED                (1)
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_DISABLED               (0)
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_UNC_THRESH             (12500)   // ms
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_DELAY_THRESH           (1000)    // ms

/* Default NV Item Value for XTRA-T */
#define NV_AAGPS_DEFAULT_XTRA_T_DISABLED                  (0) 

/* Maximum number of callbacks that can be registered. The callbacks are
   for potential clients to be notified of the fact that engine is ready to
   accept incoming clients

   Current clients (Please increase limit and add clients here):
   [0]                     - QMI LOC
   [1]                     - SDP
   [2]                     - LBS
*/

#define TM_MAX_NOTIFIERS                      3

/* Future new clients that register callbacks with TM will have to add their indexes, and update the TM_MAX_NOTIFIERS */
#define TM_LOC_MW_CLIENT_INDEX	(0)

#define TM_UNSUPPORTED_CLIENT_INDEX (TM_MAX_NOTIFIERS)


/* Enum for XTM set nv items */
typedef enum
{
  TM_XTM_SET_CLIENT_TOKEN = 0x0001,     /* sets the client token NV*/
  TM_XTM_SESSION_CONTROL  = 0x0002,     /* sets the session control NV*/
} tm_xtm_nv_e_options;

typedef union
{
  uint32 client_token;
  uint8 session_control;
} tm_xtm_nv_u_data;

typedef struct
{
  tm_xtm_nv_e_options xtm_nv_item_set;     /*Type of NV item set by XTRAT */
  tm_xtm_nv_u_data xtm_nv_data_set;        /*Contains the data that is being set*/
  int client_id;                           /*Type of client */
} tm_xtm_struct_nv_write_status;

/* -----------------------------------------------------------------------*//**
  This callback function is called by GPS tasks once they are up and initialized. 
*//* ------------------------------------------------------------------------*/
typedef void (*tm_TaskInitNotifierCb)
(
  uint32 taskId
);


/* -----------------------------------------------------------------------*//**
  This callback function is called to deliver IMSI information when it has been retrieved. 
*//* ------------------------------------------------------------------------*/
typedef void (*tm_RetrieveIMSICb)
(
  const cgps_IMSIType * const pz_IMSIInfo
);

typedef struct
{
  uint8 sub;
  tm_RetrieveIMSICb pz_RetrieveIMSICb;
} tm_RetrieveIMSIType;

typedef enum
{
  TM_GERA_TEST_CMD_FIRST = 0,
  /* TM gera command to register a client  */
  TM_GERA_TEST_CMD_CLIENT_REG =  TM_GERA_TEST_CMD_FIRST,
  /* TM gera command to deregister a client  */
  TM_GERA_TEST_CMD_CLIENT_DEREG = 1,
  /* TM gera command to enable a client  */
  TM_GERA_TEST_CMD_CLIENT_ENABLE_REQ = 2,
  /* TM gera command for extended enable of a client  */
  TM_GERA_TEST_CMD_CLIENT_ENABLE_REQ_EX = 3,
  /* TM gera command for disable of a client  */
  TM_GERA_TEST_CMD_CLIENT_DISABLE_REQ = 4,
  /* TM gera command for disabled info from the client  */
  TM_GERA_TEST_CMD_CLIENT_DISABLED = 5,
  /* TM gera command to reset GERA  */
  TM_GERA_TEST_CMD_RESET = 6,
  /* TM gera command to query client state  */
  TM_GERA_TEST_CMD_QUERY_CLIENT_STATE = 7,
  TM_GERA_TEST_CMD_LAST = TM_GERA_TEST_CMD_QUERY_CLIENT_STATE,
  TM_GERA_TEST_CMD_MAX = 0xFFFFFFFF
} tm_gera_test_cmd_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              DATA TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FUNCTION DECLARATIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
 * @brief Converts CDMA timestamp into milliseconds
 *
 * This function takes a timestamp in the format generated by time_get()
 * and converts it to the implied total number of gps milliseconds.
 *
 * @param[in]  pt_CmdaTimestamp   Quadword with CDMA timestamp.
 * @param[out] pt_MillisecondsOut Quadword with timestamp in milliseconds.
 *
 * @return None.
 */
void gps_total_msec
(
  const qword pt_CdmaTimestamp,
  qword       pt_MillisecondsOut
);

/**
 * @brief Convert quadword timestamp into GPS week number and milliseconds.
 *
 * This function takes a total number of gps milliseconds and returns the
 * week and the number of milliseconds in the week.
 *
 * @param[in]  pt_Milliseconds Quadword with milliseconds.
 * @param[out] pw_GpsWeekOut   GPS week number.
 * @param[out] pq_GpsMsOut     GPS millisecond since week start.
 *
 * @return None
 */
void gps_week_ms
(
  const qword  pt_Milliseconds,
  uint16      *pw_GpsWeekOut,
  uint32      *pq_GpsMsOut
);

/**
 * @brief Convert GPS time into CDMA system time
 *
 * This function takes time stored in GPS Time and converts it into
 * CDMA system time.
 *
 * @param[in]  pz_Clock        GPS time structure.
 * @param[out] pt_TimestampOut Quadword pointer for CDMA time.
 *
 * @return None
 */
void gps_to_cdma_time
(
  const gps_TimeStructType *pz_Clock,
  qword                     pt_TimestampOut
);

/**
 * @brief Convert GPS time into CDMA system time
 *
 * This function takes time stored in GPS Time and converts it into
 * CDMA system time.
 *
 * @param[in]  pz_Clock        GPS time structure.
 * @param[out] pt_TimestampOut Quadword pointer for CDMA time.
 *
 * @return None
 */
void gps_to_cdma_time
(
  const gps_TimeStructType *pz_Clock,
  qword                     pt_TimestampOut
);


/*******************************************************************************
 * gps_MsecWeekLimit
 *
 * Function description:
 *
 *  me_GpsMsecWeekLimit is a helper function used to perform the 
 *  fairly common check to see if a msec is indeed with the allowable
 *  range of 0 thru WEEK_MSECS-1. Only values within a single week
 *  are entertained. (ie- Multiple week adjustments are not performed
 *
 *  Assigning a NULL pointer to p_GpsWeek will disable the week 
 *  adjustment logic ... ie - The p_GpsMsec will only be affected.
 *
 * Parameters: 
 *
 *  p_GpsMsecs - Pointer to the msec value under test
 *  p_GpsWeek - Pointer to week number which may be adjusted
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void gps_MsecWeekLimit(int32 *p_GpsMsecs, uint16 *p_GpsWeek);

/*******************************************************************************
 * tm_api_store_ftcal_tt_data
 *
 * Function description:
 *
 * Parameters: 
 *
 *  
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_store_ftcal_tt_data (cgps_FtcalTTStruct* p_TTData);

/*******************************************************************************
 * tm_api_update_pos_punc_tunc
 *
 * Function description:
 *
 * Parameters: 
 *
 *  p_PosInfo - Pointer to the best available position fix info.
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_update_gps_state_info ( const sm_FixReportStructType* const p_PosInfo );

/*******************************************************************************
 * tm_api_report_utc_offset
 *
 * Function description:
 *  Get UTC offset from TM
 *
 * Parameters:
 *
 *  p_UtcOffset - Pointer to UTC offset information
 *
 * Return value:
 *
 *  void
 *
 *******************************************************************************/
void tm_api_report_utc_offset(uint8 *p_UtcOffset);


/*===========================================================================
FUNCTION tm_xtra_copy_data

DESCRIPTION
This function is called by PD API at client's context. Be sure only to perform
simple operations in this function.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_lite_xtra_cmd_err_e_type tm_xtra_copy_data(  uint8 part_num,byte *xtra_part_ptr, uint32 xtra_part_length );
/*******************************************************************************
 * tm_api_XTM_NV_status_report
 *
 * Function description:
 *  Called by XTRAT module to report the success of NV write to TM thread.
 *
 * Parameters:
 * which nv item has been set 
 * 
 * Return value: 
 * none
 *
 *******************************************************************************/

extern boolean tm_api_xtm_nv_status_report(tm_xtm_struct_nv_write_status* xtm_nv_set );

/*===========================================================================

FUNCTION tm_cm_map_sys_mode

DESCRIPTION
  This function maps a sys_sys_mode_e_type variable to a cgps_SrvSystemType
  variable
 
PARAMETERS
  sys_mode - The sys_sys_mode_e_type variable to be mapped
 
DEPENDENCIES
  None.

RETURN VALUE
  cgps_SrvSystemType

SIDE EFFECTS
  None

===========================================================================*/
cgps_SrvSystemType tm_cm_map_sys_mode(sys_sys_mode_e_type sys_mode);
/*=============================================================================

FUNCTION
  tm_api_reg_task_init_notifier

DESCRIPTION
  Called by potential clients to register their callbacks. The callbacks are
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called
  inline.  The caller should be ready to handle this and queue an IPC to itself.

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered
  clientThreadId - Client Thread ID to identify the index within the array

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_reg_task_init_notifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId);

/**
@brief Calls the tm_xtra function for indicating xtra injection 
       has completed
@param None
@return None
*/
void tm_api_xtra_inj_done(uint32  q_AcceptedMask);

/*******************************************************************************
 * tm_api_report_wbiq_info
 *
 * Function description:
 *  Called by LM to update TM with the latest WBIQ FFT info
 *
 * Parameters: 
 *
 *  p_ReportWBIQInfo - WBIQ Info
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_report_wbiq_info (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo);

/*******************************************************************************
 * tm_api_nav_config_change_resp
 *
 * Function description:
 *  Called by LM to update TM with the result of the NAV config change cmd.
 *
 * Parameters: 
 *
 *  u_GnssConfig - Current GNSS configuration
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_nav_config_change_resp (const uint8 u_GnssConfig);

/*******************************************************************************
 * tm_api_ftcal_request
 *
 * Function description:
 *  Called by LM to update TM that FTCal is enabled/disabled 
 *
 * Parameters: 
 *
 *  b_Enable - TRUE to enable FTCal 
 *             FALSE to disable FTCal
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_ftcal_request (boolean b_Enable);
/*=============================================================================

FUNCTION
  tm_api_send_engine_error_recovery_report

DESCRIPTION
  TM API to send engine error recovery report to QMI

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_send_engine_error_recovery_report( gnss_engine_err_recovery_rpt *pErrReport );

/*
******************************************************************************
* tm_read_qmi_services_configuration
*
* Function description:
*
*  This function is called by QMI services to read which QMI services are 
*  enabled. This function can be called only after SM has been initialized. 
*
* Parameters:
*  output_cfg : QMI Configuration read from NV
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_read_qmi_services_configuration( pdsm_lite_QMIServiceConfig* output_cfg);

/*
******************************************************************************
* tm_retrieve_IMSI
*
* Function description:
*
*  This function is called by XTM to request TM core to retrieve the IMSI
*
* Parameters:
*  NONE
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_retrieve_IMSI( uint8 sub, tm_RetrieveIMSICb p_RetrieveImsiInfoCb );


/*=============================================================================

FUNCTION
  tm_api_send_dpo_status_report

DESCRIPTION
  TM API to send DPO status report to TM which inturn sends it to QMI.

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_send_dpo_status_report( sm_DpoStatusReportStructType *p_DpoStatusReport );

/*===========================================================================
FUNCTION tm_core_get_param_msg_handler

DESCRIPTION
  This function handles the get param ipc msg, and sends an ipc msg as a response
  with the param value retrieved
  
DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS


===========================================================================*/
extern boolean tm_core_get_param_msg_handler(void *p_Data);

/*===========================================================================
FUNCTION tm_core_handle_prem_svc_status

DESCRIPTION
  This function handles the send premium service indication status ipc msg, 
  and calls tm_pdapi_pa_event_callback to send event to registered clients.

DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_prem_svc_status(void *p_Data);

/*===========================================================================

FUNCTION tm_api_get_gps_lock_status()

DESCRIPTION
  TM module can call this function to get current GPS lock status

  This is a synchronous API, will return immediately

DEPENDENCIES

RETURN VALUE
  Current GPS lock status

SIDE EFFECTS

===========================================================================*/
extern pdsm_lite_gps_lock_e_type tm_api_get_gps_lock_status(void);

/*===========================================================================
  tm_handle_lppm_status

  Description:

     This function handles LPPM status report

 Parameters:

   pz_LppmStatus:  pointer to the LPPM status struct

  Return value:
    void

=============================================================================*/
#if 0
void tm_handle_lppm_status(const sm_LppmStatusStructType *pz_LppmStatus);
#endif
/*===========================================================================
  tm_handle_ped_status

  Description:

     This function is used to handle MGP Ped Status

 Parameters:

   e_PedStatus:  Pedestrian status enum

  Return value:
    void

=============================================================================*/
void tm_handle_ped_status(gnss_MgpPedStatusEnumType e_PedStatus);

/*===========================================================================
  tm_handle_gnss_qual_ind

  Description:

     This function is used to handle GNSS Qual Indication

 Parameters:

   e_GnssQualityInd:  GNSS Quality indication enum

  Return value:
    void

=============================================================================*/
void tm_handle_gnss_qual_ind(const gnss_QualIndEnumType e_GnssQualityInd);


/*===========================================================================
  tm_handle_ped_dev_ctx

  Description:

     This function is used  to handle pedestrian device context

 Parameters:

   e_PedDevCtx:   Pedestrian Device Context enum

  Return value:
    void

=============================================================================*/
void tm_handle_ped_dev_ctx(const gnss_PedDevCxtEnumType e_PedDevCtx);

/*===========================================================================
FUNCTION tm_config_external_time_transfer

DESCRIPTION
  This function sends an IPC to enable/disable external time transfer request to tm_core which will eventually get GERA's
  approval before enabling/disabling at MGP

DEPENDENCIES

RETURN VALUE
  boolean: TRUE: Successfully sent the request to TM
               FALSE: Failed to send the request to TM

SIDE EFFECTS

===========================================================================*/
boolean tm_config_external_time_transfer(uint8* u_EnableFlag);

/*=============================================================================

FUNCTION
  tm_api_leap_second_report

DESCRIPTION
  Leap Second information from ME to SM

DEPENDENCIES
  None.

PARAMETERS
  mgp_LeapSecondInfoStructType - Pointer to leap second info

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_leap_second_report(const mgp_LeapSecondInfoStructType *pz_LeapSecInfo);

/*******************************************************************************
 * tm_api_report_xooffset_info
 *
 * Function description:
 *  Called by LM to update TM with the latest XO offset info
 *
 * Parameters: 
 *
 *  pz_ReportXOOffsetInfo - XO offset Info
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_report_xooffset_info 
(
  const sm_ReportXOOffsetInfoStructType *pz_ReportXOOffsetInfo
);

/*===========================================================================
FUNCTION tm_api_notify_prtl_nv_op

DESCRIPTION
  Queue an IPC message to handle the next NV read.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_api_notify_prtl_nv_op( uint32 u_nvGroup );

/*===========================================================================

FUNCTION tm_core_mem_alloc_for_cache_aiding_data

DESCRIPTION
  This function either allocates memory for frees memory to cache aiding data during UMTS_UP_NI sessions 
  in concurrency mode. During concurrency, aiding data needs to be sent to UMTS_UP_NI sessions specially
  as its not the primary client running in LM.

PARAMS
  v_mem_alloc: TRUE means mem is allocated, FALSE means free the allocated mem
DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS

===========================================================================*/
void tm_core_mem_alloc_for_cache_aiding_data(boolean v_mem_alloc);


/*=============================================================================

FUNCTION
  tm_api_xtra_gts_time_request

DESCRIPTION
  This function is used to process GTS time request from XTRA module 

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE: If request was sent succesfully.
  FALSE: Otherwise

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_xtra_gts_time_request(void);


/*
 ******************************************************************************
 * tm_api_request_xtra_inject_to_mgp
 *
 * Function description:
 *
 * This function is used to request XTRA injection from TM to MGP
 *
 * Parameters:
 *   None
 *
 * Return value:  
 * TRUE if request was succesful
 * FALSE otherwise
 ******************************************************************************
*/
boolean tm_api_request_xtra_inject_to_mgp(void);
/*=============================================================================

FUNCTION
  tm_api_RequestPlmnCellScan

DESCRIPTION
  This function is used to request cell scan from tle

DEPENDENCIES
  None.

PARAMETERS
  plmnSearch

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_RequestPlmnCellScan(tle_tm_plmn_search* plmnSearch);
/*===========================================================================

FUNCTION tm_core_is_rcvr_config_update_needed

DESCRIPTION
  This function returns if current config update is required

  RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_rcvr_config_update_needed(boolean v_RcvrInUS);

/*===========================================================================
  
FUNCTION tm_api_get_gtp_wwan_fix_timer_val
 
DESCRIPTION
  This function returns the timer count base on cell RAT we are camped
 
  RETURN VALUE
  None
 
DEPENDENCIES
  None
  
SIDE EFFECTS
  
===========================================================================*/
void tm_api_get_gtp_wwan_fix_timer_val(uint32 *q_TimerCount);

/*===========================================================================
	
FUNCTION tm_api_is_gtp_2_0_fix_possible
	
DESCRIPTION
  This function returns TRUE if GTP 2.0 fix is possible else returns FALSE
	
  RETURN VALUE
  None
	
DEPENDENCIES
  None
	
SIDE EFFECTS
	
===========================================================================*/
bool tm_api_is_gtp_2_0_fix_possible(void);

#ifdef __cplusplus
}
#endif

#endif /* TM_API_H */
