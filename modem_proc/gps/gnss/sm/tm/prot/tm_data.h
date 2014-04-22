#ifndef TM_DATA_H
#define TM_DATA_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM_DATA Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for both
  TM & TM protocol tasks.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2021 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/prot/tm_data.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/21   sai     Expanding TUNC/FUNC NV 74194 to bypass FUNC explicitly
01/27/21   sai     Added NV for Controlling Fix related operations, currently controls cache fix propogation and its reporting to client
29/04/20   ska     Added changes for asking less proc duration when EPH is valid
09/18/19   py      CHeck if any active GNSS session on XTRA avail req done
08/21/19   ska     Added support for NMEA GSV
06/28/19   katta    added API to provide timeout value to TLE
10/26/17   rn      Notification from GPS FS task regarding XTRA EFS write operation.
12/14/17   mj      Block sessions during SARF 
06/15/17   kpn     Add AS ID to tm_handle_ota_reset()
05/08/17   rn      Modified NV for LPP12 2 timer feature config to support LPP CP.
05/05/17   kpn     Clear cache of constellations whose meas blk is not received
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
12/07/16   ss     Using OEM NV Bit 15 to configure Cell CPI Fix for Geofencing
11/16/16   mj      E911 and apptracking concurrency support
09/14/16   rn      Add ULog support for SM.
08/30/16   skm     Added NV for default LPP12 2 timer feature config 
08/30/16   kpn     Add slot id into mmsgsdi global session id table to support pbm_session_record_read()
07/21/16   mj      Add num_fixes tracking in TM session info.
06/23/16   mj      Add changes to get leap second from MGP
06/08/16   mj      Add changes to identify if a session was started for E911
06/06/16   mj      Add GERA support for AP-MP
04/15/16   ld      Added NHz configuration parameter and type 
04/02/16   mj      GERA support
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic 
02/11/16   gk      Added LPPe support 
10/27/15   mj      Handle OTA Reset for UMTS 
08/05/15   rh      Added carrier-specific NV 1920 (pos mode support) to TM core 
07/08/15   rh      Changed NV refresh to be done on one specifc sub
06/09/15   ssu     Adding NV for GM filtering of WLAN freq. 
06/01/15   jv      Added QZSS Support
04/06/15   mj      Add new function tm_core_is_active_sess_mo 
03/14/15   rh      Added support for MultiSIM NV 
11/16/14   rh      Added API to get subs ID from MMSGDI table for SIM swap 
10/29/14   ssu     GM LOWI Integration
3/12/14    gk     Handle OTA reset when session is active. consider as end of the session
11/25/14   rk      NV item for LPP Rel10 grt 24 AD 
10/27/14   gk      NV item for terminating SUPL after final fix 
10/24/14   jv      Added Galileo Support
09/10/14   gk      Added delete throttle timer to prevent B-b delete commands
04/28/14   ah      Added handling of Set Premium Services indication status
04/17/14   skm     NV Async Read changes 
02/21/14   ssu   Adding NV for configuring motion sensing distance in case of challenging GPS env. 
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.  
03/10/14   ah      Added support for Get Avail WWAN Pos
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/31/14    ss     Supporting Max acceptable GF PUNC confguration from QMI
01/10/14   ss      Supporting High Responsiveness Geofencing Config
12/23/13   mj        Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
12/02/13   ah      Support GPS State Info requests for multiple clients simultaneously
10/16/13   rk      Added generic Glonass AcqAsst processing functions.
09/27/13   mj      TSTS Support: Update DSDS_MAX_SESS_TYPES for TSTS
09/12/13   mj      Add new parameter to function tm_mmgsdi_get_sim_info_from_asid
06/07/13   rh      Removed diag timer and NO_FIX report handling 
02/04/13   kj      Move to the DOG Heartbeat mechanism on DIME and beyond
01/31/13   mj       Add new timer api_callback_timer
11/30/12   mj      Added tm_core_lpp_cp_evt_handler and tm_core_external_cell_db_proc
07/17/12   mjohn   Added premium_services_config into  tm_core_config_s_type
03/01/12   rh      Added tm_core_handle_engine_error_recovery_report()
02/06/12   gk      LPP configuration support
06/03/11   gk      Clear cached position  when delete param or gps lock is sent
01/03/11   gk      Eph throttling rework
10/05/10   ns      Port CL1333689: Save status of gps state info req by SYSD.
06/21/10   gk      Save the cached gps state
03/08/10   gk      Save the eph need list
03/03/10   ns      DSDS Support
09/30/09   gk      Support ODp 2.0
07/23/09   gk      Support ODp 2.0
07/10/09   atien   Spirent requested feature:  allow XTRA dload after MSB dload comm failure
6/10/09    gk      Added support for gps utc offset NV item
06/10/09   gk      ODP 1.1 support
6/04/09    gk      Added support for gps utc offset NV item, other inter-position changes
04/02/09   ns      support for qwip
03/11/09   ns      Add variable to store whether sysd was triggered for
                   this session
03/04/09   ns      Created new Data str. for get gps state
02/04/09   atien   MOLR vs AD Call Flow Select support
01/13/09   atien   XTRA-T support
01/27/09   ns      qWip Integration support changes.
08/23/08   gk      GPS lock to be honored for on-demand
08/12/08   gk      Added force reciever off
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
06/12/08   lt      Support for external coarse position injection.
04/25/08   gk      on-demand changes
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
09/14/07    gk     NMEA CONFIG set parameters support
07/30/07   gk      Add handling for last position
07/17/07   lt      L1 interface.
06/12/07   cl      Only allow PD API access when TM task is active
05/30/07   lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR.
04/17/07   lt      Added mo_method into tm_core_config_s_type.
04/04/07   gk/mr   Added function for Time injection handling
01/31/07   cl      Added function for LCS handling
12/04/06    cl     Added NV functionality
10/13/06    cl     Change to use SM_TM for dog report
07/12/06    cl     Initial Release
===========================================================================*/

#include "pdapi_lite.h" //adding to make sm code work
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#if !defined (WIN32) && !defined (FEATURE_CGPS_PLAYBACK)
#include "task.h"
#endif
#include "aries_os_globals.h"
#include "aries_os_api.h"
#include "aries_ipc_diag_log.h"   /* For ULog */
#include "tm_common.h"
#include "tm_prtl_iface.h"
#include "tm_cm_iface.h"
#include "tm_pdapi_iface.h"
#include "tm_api.h"
#include "sm_nv_efs.h"

/* TM-Core Timer Definition Section */
#define TM_CORE_TIMER_DISPATCH_MASK               0xFF000000

/* TM-Core Transaction ID Definition Section */
#define TM_CORE_TRANSACTION_ID_DISPATCH_MASK      0xFF000000


//#define TM_CORE_SESS_TIMER_ID_DIAG              (0x00000002)
//#define TM_CORE_TIMER_ID_DELETE_THROTTLE_ID  (0x00000004)
#define TM_CORE_MIN_VELOCTY            0.15 /* 0.15 m/s */

/* constant for the MSB of msec elapsed from 6 Jan 1980 00:00:00 to 6th Jan 2000*/
#define MSB_MSEC_ELAPSED_SINCE_DEFAULT 75

/* wait for 3 second for Sys UNC */
#define TM_CORE_GTS_SYS_UNC_WAIT_TIMER_VAL     3*1000

#define TM_CORE_GTS_SYS_TUNC_AND_FUNC_BYPASS   0x01
#define TM_CORE_GTS_SYS_FUNC_BYPASS            0x02
#define TM_CORE_GTS_SYS_TUNC_DL_FAILURE_ALLOW_SESS TM_CORE_GTS_SYS_FUNC_BYPASS

/* TM task info structure */
typedef struct
{
  boolean          task_active;
  os_TimerMsgType  *api_callback_timer;
} tm_task_info_s_type;

/* macro for easier access to Bits corresponding to MEAS and POLY from NV73542 */
/* Check if n-th bit in x is set */
#define BIT_POLY 1
#define BIT_MEAS 0
#define B_IS_SET(x, n)   (((x) & (1<<(n)))?1:0)
/* TM-Core default allowed Session handle mask */
#define TM_CORE_ALLOWED_MT_SESSION_HANDLE_MASK  ((TM_SESS_HANDLE_FTEST))

/*TM_Core masks to enable AGPS on RAT's*/
#define TM_CORE_NV_RAT_ENABLE_GSM_MASK   0x8
#define TM_CORE_NV_RAT_ENABLE_ALL_MASK   0xFF

/* channelNumber INTEGER (-7..13) as per RRLP 8.6.0, LPP 9.6.0, RRC R8 */
#define C_PROT_GLO_FREQ_MIN GLO_LOWEST_FREQ_NUMBER
#define C_PROT_GLO_FREQ_MAX 13

/* GPS, GLONASS, BDS, GAL SV used as position source to compute the fix (by PE) */
#define C_POS_SOURCE_GPS (0x1<<0)
#define C_POS_SOURCE_GLO (0x1<<1)
#define C_POS_SOURCE_BDS (0x1<<2)
#define C_POS_SOURCE_GAL (0x1<<3)

#define TM_GNSS_GPS_ENABLE_MASK        0x00000001              //BIT-0 to enable GPS
#define TM_GNSS_GLO_ENABLE_MASK        0x00000002              //BIT-1 to enable GLO
#define TM_GNSS_GAL_ENABLE_MASK        0x00000028              //BIT-3 or 5 to enable GAL 
#define TM_GNSS_BDS_ENABLE_MASK        0x00000014              //BIT-2 or 4 to enable BDS
#define TM_GNSS_QZSS_ENABLE_MASK       0x00000340              //BIT-6,8 or 9 to enable QZSS 

#define TM_GNSS_GPS_CONSTELL  SM_GNSS_GPS_CONSTELL
#define TM_GNSS_GLO_CONSTELL  SM_GNSS_GLO_CONSTELL
#define TM_GNSS_BDS_CONSTELL  SM_GNSS_BDS_CONSTELL
#define TM_GNSS_GAL_CONSTELL  SM_GNSS_GAL_CONSTELL
#define TM_GNSS_QZSS_CONSTELL SM_GNSS_QZSS_CONSTELL
#define TM_GNSS_MAX_CONSTELL  SM_GNSS_MAX_CONSTELL

/* Glonass Sv/Channel# sub-buffer structure. Source of data Glo AuxiInfo and/or Glo Almanac */
typedef struct
{
  struct
  {
    unsigned channelNumValid : 1; /* Set to 1 when channel_num is valid */
  } m;
  int8 channel_num;                 /* Insert valid frequency number -7..13 */
} prot_glo_cn_buffer_s_type;

/* Glonass Sv/Channel# buffer structure. Source of data Glo AuxiInfo and/or Glo Almanac */
typedef struct
{
  uint8 u_n;                                            /* Number of valid Sv/ChannelNum */
  prot_glo_cn_buffer_s_type z_glo_sat_elem[N_GLO_SV];   /* Array index is Glonass Sv Id of 0..23 */
} prot_glo_svcn_buffer_s_type;

/* Glonass Navigation Model structure */
typedef struct
{
  boolean             u_freq_num_valid;
  glo_EphStructType   z_eph_glo_data;
} prot_glo_eph_buffer_s_type;

/*===========================================================================
           Definition of TM-Core Data Structure
===========================================================================*/
typedef void (prtl_init_f_type)(void);

typedef enum
{
  TM_CORE_SESS_STATE_INIT = 0,
  TM_CORE_SESS_STATE_WAIT_AUTH,
  TM_CORE_SESS_STATE_ACCESS_GRANTED,
  TM_CORE_SESS_STATE_RESOURCE_CLEANUP,
  TM_CORE_SESS_STATE_COMPLETED
} tm_core_session_state_e_type;

typedef enum
{
  TM_CORE_SYSTEM_UNC_VALID,
  TM_CORE_SYSTEM_UNC_WAIT,
  TM_CORE_SYSTEM_UNC_INVALID
}tm_core_system_unc_state;

/* Struct for get pos request*/
typedef struct
{
  pdsm_lite_client_type_e_type       client_type;
  pdsm_lite_client_id_type           client_id;
  pdsm_lite_mt_initiator_e_type      mt_initiator;  /* 0 (not applicable), (1) CP, or (2) UP */
  pdsm_lite_pd_option_s_type         option;        /* PD option type */
  pdsm_lite_pd_qos_type              qos;           /* Ptr to user QOS for this request */
  boolean                           internal_session;
} tm_queued_get_pos_param_s_type;


/* union for tm core get gps state payload*/
typedef  union
{
  pdsm_lite_cmd_s_type *p_cmd_type;
}tm_get_gps_state_payload_u_type;

/* Struct for tm core get gps state payload*/
typedef struct
{
  sm_ReqEnumType source;
  tm_get_gps_state_payload_u_type cmd;
} tm_core_get_gps_state_param_s_type;

typedef struct {
    uint32  q_FeatureID;
    boolean v_Status;
} LocFM_FeatureStatusStructT;

typedef tm_core_get_gps_state_param_s_type tm_core_get_best_avail_pos_param_s_type;
typedef tm_core_get_gps_state_param_s_type tm_core_get_avail_wwan_pos_param_s_type;
typedef tm_core_get_gps_state_param_s_type tm_core_get_available_pos_param_s_type;


typedef struct
{
  uint32 q_ClientId;
  uint16 w_TmRcvdDataMask;     /* Bitmask to indicate which data type has been received so far */
} tm_GpsStateInfoClientType;

#define TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID 0x0001


typedef struct
{
  /* TM-Core Active Session Handle */
  tm_sess_handle_type         active_sess_handle;

  /* Current Operation Mode */
  tm_sess_operation_mode_e_type  op_mode;

  /* Operation already Requested */
  tm_op_req_type                 op_requested;

  /* Protocol sub-module the active session handle belongs to */
  tm_prtl_type                from_protocol;

  /* TM-Core State */
  tm_core_session_state_e_type session_state;

  /* MO session handle seed */
  unsigned short              mo_sess_handle_seed;

  /* Client information */
  prtl_start_sess_req_param_s_type pd_param;
  /* Session start/done Event firing */
  boolean                    session_event_firing;

  /* Cached eph and alm have-list */
  uint32                     eph_svmask;
  uint32                     alm_svmask;
  uint32                     utc;
  uint32                     health_svmask;
  uint32                     valid_svmask;

  uint32                     glo_eph_svmask;
  uint32                     glo_alm_svmask;
  uint32                     glo_utc;
  uint32                     glo_health_svmask;

  uint64                     bds_eph_svmask;
  uint64                     bds_alm_svmask;
  uint32                     bds_utc;
  uint64                     bds_health_svmask;

  uint32                     qzss_eph_svmask;
  uint32                     qzss_alm_svmask;
  uint32                     qzss_utc;
  uint32                     qzss_health_svmask;
  uint64                     gal_eph_svmask;
  uint64                     gal_alm_svmask;
  uint32                     gal_utc;
  uint64                     gal_health_svmask;

  boolean                         queued_request;
  tm_queued_get_pos_param_s_type  queued_get_pos_param;
  boolean                         force_eph_download;
  boolean                         glo_force_eph_download;
  boolean                         bds_force_eph_download;
  boolean                         gal_force_eph_download;
  uint32 q_FixTimeRemainingMSec;

  boolean                    end_odp_session;
  /* save the GPS eph need request coming from PE */
  uint32                     eph_need_sv_mask;
  /* save the GLONASS eph need request coming from PE */
  uint32                     glo_eph_need_sv_mask;
  /* save the BDS eph need request coming from PE */
  uint64                     bds_eph_need_sv_mask;
  /* save the GAL eph need request coming from PE */
  uint64                     gal_eph_need_sv_mask;
  /* state info flags. use this instead of adding new booleans */
  uint32                     sess_info_valid_flags; /* see the flag definitions above */
  
  /* Request for MGP receiver off */
  boolean                    receiver_off_needed;

  /* Bitmask to indicate if request for client (1<<sm_ReqEnumType) is on-going */
  uint32                     q_ReqClientMask;
#ifdef FEATURE_CGPS_XTRA_T
    /* TRUE if Request XTRA download triggered by AGPS session(SUPL/V2 etc) */
  boolean                    req_xtra_trig_by_agps;
#endif /*FEATURE_CGPS_XTRA_T*/
  /* Structure that holds requesting client specific data (multiple client support) */
  tm_GpsStateInfoClientType  z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_MAX+1];

  uint32                     q_num_fixes;

  /* If current Tm session requests for XTRA AVAIL INFO */
  boolean                    v_xtra_info_requested;
} tm_core_sess_info_s_type;

typedef struct
{
  /* Save client_id for returning back to PD API module */
  pdsm_lite_client_id_type   client_id;
} tm_core_pa_client_info_s_type;

typedef struct
{
  prtl_init_f_type                  *init_fp;
  prtl_start_sess_req_f_type        *start_sess_req_fp;
  prtl_stop_sess_req_f_type         *stop_sess_req_fp;
  prtl_session_req_f_type           *sess_req_data_fp;
  prtl_session_info_f_type          *sess_info_fp;
  prtl_timer_cb_f_type              *timer_cb_fp;
  prtl_event_cb_f_type              *event_cb_fp;
  prtl_gm_event_handle_f_type       *gm_event_handle_fp;
} tm_core_prtl_cb_s_type;

typedef struct
{
  /* Timeout in seconds after which GNSS unavail indication is sent out from GM in a bad GNSS environment */
  uint32 sm_gm_gnss_unavail_ind_timeout;

  /*Geofence Motion Detection Sources*/
  uint32 sm_gm_motion_detection_sources;
  /*Geofence GNSS session timeout*/
  uint8 sm_gm_gnss_pos_qos_sess_timeout;
  /*Geofence Position Sources*/
  uint32 sm_gm_position_sources;
  /*Geofence Max pos unc accepted */
  uint32 sm_gm_gnss_max_pos_unc_accepted;

  /*High Responsiveness Config*/
  uint32 sm_gm_high_responsiveness_config;

  /*GM OEM Control Mask*/
  boolean sm_gm_oem_control_mask;
  /* GM CPI request rate */
  uint32 sm_gm_cpi_request_rate;

  /*GM Medium resp. backoff in seconds */
  uint32 sm_gm_med_resp_backoff;

  /* Backoff config when in Challenging GPS Env in seconds */
  gm_chal_gps_env_backoff_config_s_type sm_gm_chal_gps_env_bo_config;

  /* The motion sensing distance config in case of challenging GPS env */
  gm_chal_gps_env_ms_dist_config_s_type sm_gm_chal_gps_env_ms_dist_config;

  /*The motion state speed config used by Geofencing*/
  gm_ms_speed_config_s_type sm_gm_motion_state_speed_config;
  
  /*Comprehensive Logging Rate*/
  uint32 q_gm_comp_logging_rate;  
  
//#9x05 remove all

  /* Geofence use network assisted fixes */
  boolean sm_gm_use_nw_asst_fixes;
  /* Enable GM LOWI registration */
  boolean sm_gm_lowi_reg_enable;
  /* Mask to subscribe to different services from LOWI */
  uint32 sm_gm_lowi_sub_mask;
  /* Configure how GM registers with LOWI */
  gm_lowi_reg_config_s_type sm_gm_lowi_reg_config;
    /*GM filtering of Wlan freq mask*/
  uint32 q_sm_gm_filter_wlan_freq_mask;
  
//#9x05

  /*The ALS IOD sensor config mask used by Geofencing*/
  uint32 q_sm_gm_iod_config_switch_mask;
  
  /*Accept Cell Fixes from CPI*/
  boolean v_sm_gm_accept_cell_cpi_fixes;

  /*Enable predictor in GM drive to ped context detector */
  boolean sm_gm_drive_to_ped_predictor_enable;
} geofence_engine_config_s_type;


/* Below are carrier-specific EFS NVs */
typedef struct
{
/* Below from legacy/non-EFS NV */
  /* GPS Lock */
  pdsm_lite_gps_lock_e_type       gps_lock;

/* NMEA String Output Format */
  uint32         nmea_output_format;

/* Below from EFS NV */

  /* Positioning Modes Supported  (NV 1920) */
  uint32                      positioning_modes_supported;

} tm_core_config_carrier_specific_s_type;

/* macro for easier access to carrier-specific configuration */
#define TM_CORE_CONFIG_DD(x)  (tm_core_info.config_info.subs_cfgs.x)

#define TM_CORE_CONFIG_IS_GTP_WWAN_AVAIL() (tm_core_info.config_info.b_isGtpWwanServiceFeatureEnabled)

#define TM_CONFIG_POS_MODE_BIT_IS_SET(x) \
    ( ((tm_core_info.config_info.subs_cfgs.positioning_modes_supported) & (x)) != 0)

typedef struct
{
  /* UMTS' MO type: CP-LocEstimate, CP-AssistData, and UP:
     initialized to NV_Item cgps_mo_method and can be updated by pdsm_lite_setparam() */
  uint8                      u_mo_method;

  /* preferred xtra data validity age */
  uint16                     preferred_xtra_age;

  /* if set to 1, TM will send PE's NO_FIX report as IntermediateReport */
  uint8                      send_no_fix_report;

  /*QMI Services config*/
  pdsm_lite_QMIServiceConfig         qmi_cfg;

  /*Geofence Engine Config*/
  geofence_engine_config_s_type gm_config;
  
  /* array to store configurations for all Subs */
  tm_core_config_carrier_specific_s_type  subs_cfgs; //TODO  [an][modify] converted from an array


  /* Leap Second Information */ 
  mgp_LeapSecondInfoStructType z_LeapSecondInfo;

  /* GNSS NHz Config */
  /* For normal operation, this would have value defined in gnss_common.h
     #define GNSS_NHZ_ENABLED 0x0001
     #define GNSS_NHZ_MODE_AUTOMOTIVE 0x0002
     #define GNSS_NHZ_MODE_DRONE 0x0004
  */
  uint32 gnss_nhz_config; 


  /* SARF status (1: RF test in progress; 0: No RF test in progress */
  boolean v_sarf_status;
  boolean                       b_isGtpWwanServiceFeatureEnabled;

  /* if value greater than 0, by-pass gnss aiding check*/
  uint8 bypass_gnss_aiding_check;

  /* Duration of timer waiting for GTP WWAN fix */
  uint32 gtp_wwan_fix_timer_val;  

  /*Flag to control default fix operation. Default value is 0.
	Bit 0 corresponds to cache fix control, set it to 1 to disable cache fix propogation and reporting to client*/
  uint8 gnss_control_fix_operation;

} tm_core_config_s_type;

#define  TM_RCVD_POS_FROM_PE         0x01
#define  TM_RCVD_TUNC_FROM_ME        0x02
#define  TM_RCVD_ALL (TM_RCVD_POS_FROM_PE | TM_RCVD_TUNC_FROM_ME )

typedef struct
{
  uint32 flags;
  int32  lat;                  /* Latitude:  LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
  int32  lon;                  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
  int16  altitude;             /* Altitude in meters */
  uint32  pos_unc;              /* Position Unc in meters */
  uint16 loc_unc_vertical;     /* vertical unc in meters */
  dword  timestamp_tow_ms;     /* Timestamp in GPS TOW milliseconds */
  uint16 timestamp_gps_week;   /* GPS week number */
  uint32 time_unc_us;          /* Time uncertainty in micro seconds */
  uint32 frequency_unc;        /* 1 sided max freq bias uncertainty [m/s] */
  uint32 clk_drift;            /* Clock Drift in meters/sec */
  uint32 clk_bias;             /* Clock frequency bias in meters/sec */
  uint8  engine_state;         /* GPS engine state on/off */
  uint32 eph_svmask;           /* SV Mask for the ephemeris */
  uint32 alm_svmask;           /* SV Mask for the Almanac */
  uint32 health_svmask;        /* SV Mask for Health */
  uint32 health_bad_svmask;    /* SV Mask for Health */
  uint32 health_noex_svmask;   /* SV Mask for Health */
  uint32 health_unk_svmask;    /* SV Mask for Health */
  uint32 visible_svmask;       /* SV mask for visible SVs */
  uint16 start_gps_week;       /* Current XTRA info is good starting this GPS week */
  uint16 start_gps_minutes;    /* Current XTRA information is good starting this many minutes into the week */
  uint16 valid_duration_hours; /* XTRA info is valid for this many hours */

} tm_core_gps_state_info_s_type;


typedef struct
{
  /* TM task Info */
  tm_task_info_s_type           tm_task_info;

  /* TM Configuration Info (NV settings) */
  tm_core_config_s_type         config_info;

  /* Session Relelated Info */
  tm_core_sess_info_s_type      session_info;

  /* Session Relelated Info */
  tm_core_pa_client_info_s_type pa_client_info;

  /* TM-Core Function Table */
  tm_core_prtl_cb_s_type        prtl_func_cb_table[TM_PRTL_NUM];

  /* Extended data */
/*TODO:[DEL] #9x05 pdsm_lite_pd_data_ext_s_type pd_ext_data is removed as the position report is directly
             translated to QMI structure all the intermediate structures are being removed
 */
  tm_core_gps_state_info_s_type gps_state_info;
  /*Cached MGP receiver state*/
  gps_RcvrStateEnumType         e_cached_rcvr_state;
  boolean                       internal_session;

  /*If this session is internal*/
  os_TimerMsgType*              tm_delete_param_throttle_timer;     /*  throttle timer to throttle back to back delete requests from PDAPI */
  os_TimerMsgType*              tm_core_gtp_wwan_fix_timer;
  os_TimerMsgType*              tm_core_gts_sys_unc_wait_timer;
  boolean                       cs_on_demand_active;
  boolean                       ext_inj_allowed;
  uint8                         u_sm_curr_serving_sec_gnss_constellation;    /*Id of the constellation that can be used in addition to GPS for gnss fixes and measurement*/
  uint32                        q_mgp_curr_gnss_constellation_config;
  uint64                        tm_core_last_fail_gtp_wwan_fix_time;
  boolean                       v_is_update_config_needed;
  uint16                        min_wait_interval_btwn_req_to_gtp_server;
  uint32                        gtp_wwan_fix_timer_val;
  uint32                        efs_nv_gnss_config;

  uint16                        w_RecvdMeasBlkMask;
  gnss_MeasBlkStructType        gps_meas_blk;
  gnss_MeasBlkStructType        glo_meas_blk;
  gnss_MeasBlkStructType        bds_meas_blk;
  gnss_MeasBlkStructType        qzss_meas_blk;
  gnss_MeasBlkStructType        gal_meas_blk;
  gnss_EphExpTimeStructType     z_eph_exp_time;
} tm_core_param_s_type;

typedef struct
{
  boolean                  cached_meas_valid;
  sm_GpsMeasRptStructType  z_meas;
} tm_core_cached_meas_s_type;

/*Todo:[APP][DEL] This is being removed for now because the cached position report is not being used. 
                  We will create a new structure for caching position report DM. It will be used to
                  store position report in raw form as received from MGP
*/

#if 0
typedef struct
{
  boolean           cached_ref_pos_valid;
  pdsm_lite_ext_status_info_s_type   pd_ext_status_info;
  boolean           ref_pos_reported;
} tm_core_cached_ref_pos_s_type;
#endif

typedef struct
{
  uint8                      no_of_sims;
  uint8                      no_of_active_subs;
  sys_overall_feature_t      tm_cm_overall_device_feature;
  sys_subs_feature_t         tm_cm_subs_feature[1];//Changes just for compiling smart need to check it had index 0
  sys_ue_mode_e_type         tm_cm_subs_ue_mode[1];
} tm_core_device_config_s_type;

/* Gobal variable which store device configuration info such as SS/DSDS/DSDA/TSTS */
static tm_core_device_config_s_type tm_core_device_config;


#define SM_TOT_GNSS_MEAS_SYSTEM         5     /// Total GNSS system measurement types supported (GPS + GLONASS + BDS +GAL +QZSS)
#define SM_GNSS_MEAS_SYSTEM_GPS         0
#define SM_GNSS_MEAS_SYSTEM_GLONASS     1
#define SM_GNSS_MEAS_SYSTEM_BDS         2
#define SM_GNSS_MEAS_SYSTEM_GAL         3
#define SM_GNSS_MEAS_SYSTEM_QZSS        4
#define SM_GNSS_MEAS_SYSTEM_INVALID     -1

/* Bitmap for measurement report */
#define SM_GNSS_MEAS_REPORTS_BMAP_GPS     (1<<0)
#define SM_GNSS_MEAS_REPORTS_BMAP_GLONASS (1<<1)
#define SM_GNSS_MEAS_REPORTS_BMAP_BDS     (1<<2)
#define SM_GNSS_MEAS_REPORTS_BMAP_GAL     (1<<3)
#define SM_GNSS_MEAS_REPORTS_BMAP_QZSS    (1<<4)


typedef struct
{
  struct {
    uint8 s_StatusCheckNeeded : 1; /* When build the measurement whether status bit field should be checked or not */
  }m;
  uint8                     u_meas_reports_bmap; /* Use SM_GNSS_MEAS_REPORTS_BMAP_XXX to check which measurement
                                                    report is filled */
  sm_GpsMeasRptStructType   z_meas_reports[SM_TOT_GNSS_MEAS_SYSTEM]; /* Use u_meas_reports_bmap to check which measurement
                                                                        report element is populated. */ 
} SmGnssMeasReportStructType;

typedef struct
{
  boolean                  cached_gnss_pos_valid;
  sm_FixInfoStructType     z_cached_gnss_pos_rpt;
}tm_core_gnss_pos_cache_s_type;

/* TM ULog Circular Buffer Size */
#define TM_ULOG_SIZE  (1024) 

/*--------------------------------------

      Extern Valuables

---------------------------------------*/
extern tm_core_param_s_type     tm_core_info;

#ifdef TM_DATA_DECLARE
  #define extern
  #define _INITIALIZE(x)  = x
#else
  #define _INITIALIZE(x)
#endif

/* TM ULog Handle */
extern OS_ULOG_HANDLE tm_ULogHandle _INITIALIZE(0);

#ifdef TM_DATA_DECLARE
  #undef extern
#endif

/*-----------------------------------------------------------------------------

                          Extern Functions

-----------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION tm_core_init

DESCRIPTION
  This function is to initialize TM-Core data structures.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_init(void);

/*===========================================================================

FUNCTION tm_core_session_active

DESCRIPTION
  This function tells if tmCore is in any active session (MI or MT)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_session_active(void);

/*===========================================================================

FUNCTION tm_core_abort_recover

DESCRIPTION
  This function performs TM-Core software recovery.

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS

===========================================================================*/
extern void tm_core_abort_recover
(
   boolean                       ask_prtl,
   tm_sess_stop_reason_e_type    abort_reason
   );

/*===========================================================================

FUNCTION tm_core_delete_inject_aiding_data

DESCRIPTION
  This function is called to delete or inject aiding data.

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
extern void tm_core_delete_inject_aiding_data
(
   pdsm_lite_delete_parms_type *delete_param_ptr,
   boolean                 force_rcvr_off,
   boolean                 delete_all,
   pdsm_lite_pa_e_type delete_type
   );

/*===========================================================================

FUNCTION tm_core_inject_ext_time

DESCRIPTION
  This function is called to inject external time into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
extern void tm_core_inject_ext_time
(
   pdsm_lite_pd_external_time_info_type *ext_time_ptr
   );

/*===========================================================================

FUNCTION tm_core_inject_ext_pos

DESCRIPTION
  This function is called to inject external position into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_inject_ext_pos
(
   pdsm_lite_cmd_s_type *p_cmd_type
   );


/*===========================================================================

FUNCTION tm_core_get_gps_status

DESCRIPTION
  This function is called to get the GPS status information

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_status
(
   pdsm_lite_cmd_s_type *p_cmd_type
   );


/*===========================================================================

FUNCTION tm_core_update_session_state

DESCRIPTION
  Determine if the state transition is valid and update the tm session
  state.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
extern void tm_core_update_session_state
(
   tm_core_session_state_e_type new_state
   );

/*===========================================================================

FUNCTION tm_core_new_mo_sess_handle

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern tm_sess_handle_type tm_core_new_mo_sess_handle(void);

/*===========================================================================

FUNCTION tm_core_timer_dispatcher

DESCRIPTION
  This function is to figure out which TM software sub-module the timer expiry event
  belongs to and call the proper timer handler (provided to TM-Core by each sub-module)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_timer_dispatcher(void *timer_data);

/*===========================================================================

FUNCTION tm_core_lm_sess_req_handler

DESCRIPTION
  This function is to handle sess request from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lm_sess_req_handler(void *lm_sess_req_data);

/*===========================================================================

FUNCTION tm_core_lm_sess_update_handler

DESCRIPTION
  This function is to handle sess update from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lm_sess_update_handler(void *lm_sess_update_data);

/*===========================================================================

FUNCTION tm_core_lm_sess_info_handler

DESCRIPTION
  This function is to handle sess info from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lm_sess_info_handler(void *lm_sess_info_data);

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_type

DESCRIPTION
  This function is to get protocol array index by protcol type

DEPENDENCIES

RETURN VALUE
  -1, if protocol is not supported.
  Otherwise, array index for the protocol type

SIDE EFFECTS

===========================================================================*/
extern int  tm_core_get_prtl_idx_by_type(tm_prtl_type prtl_type);



/*===========================================================================

FUNCTION tm_core_sess_req_start

DESCRIPTION
  This function is to figure out if TM-Core accepts session start request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_start(tm_prtl_type                    prtl_type,
                                      tm_sess_handle_type             sess_handle,
                                      tm_sess_req_start_param_s_type  sess_start_param);
/*===========================================================================

FUNCTION tm_core_sess_req_continue

DESCRIPTION
  This function is to figure out if TM-Core accepts session continue request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_continue(tm_prtl_type                       prtl_type,
                                         tm_sess_handle_type                sess_handle,
                                         tm_sess_req_continue_param_s_type  sess_continue_param);

/*===========================================================================

FUNCTION tm_core_sess_req_stop

DESCRIPTION
  This function is to figure out if TM-Core accepts stop request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_stop(tm_prtl_type                       prtl_type,
                                     tm_sess_handle_type                sess_handle,
                                     tm_sess_req_stop_param_s_type      sess_stop_param);


/*===========================================================================

FUNCTION tm_core_recv_data
used only by protocol modules, no standalone calls
FUNCTION tm_core_recv_event

DESCRIPTION
  This function is to handle PD API Events sent from protocol sub-modules.

DEPENDENCIES

RETURN VALUE
  TRUE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_recv_event(
                                  pdsm_lite_pd_event_type   pd_event,
                                  pdsm_lite_pd_info_s_type *pd_info_ptr,
                                  sm_FixInfoStructType *lr_data
                                  );

/*===========================================================================

FUNCTION tm_core_cm_notify_event

DESCRIPTION
  This function is called by TM-CM module to inform TM-Core phone events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_cm_notify_event
(
   tm_phone_event_e_type          cm_event_type,
   tm_phone_event_payload_u_type  *evt_payload_ptr
   );


#ifdef FEATURE_CGPS_XTRA
/*===========================================================================
FUNCTION tm_core_is_xtra_operation_allowed

DESCRIPTION
Check if assisted mode GPS session are in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_xtra_operation_allowed
(
   void
   );

#endif /* FEATURE_CGPS_XTRA */

/*===========================================================================

FUNCTION tm_core_get_gps_lock_state

DESCRIPTION
  This function determines if gps is locked

RETURN VALUE
  TRUE if gps is locked, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_gps_lock_state(void);

/*===========================================================================

FUNCTION tm_core_get_on_demand_state

DESCRIPTION
  This function determines if on-dmeand positioning is enabled

RETURN VALUE
  TRUE if on-demand is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_on_demand_state(void);

/*===========================================================================

FUNCTION tm_core_is_internal_session_on

DESCRIPTION
  This function determines if internal session is ongoing

RETURN VALUE
  TRUE if internal session is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_internal_session_on(void);

/*===========================================================================
FUNCTION tm_core_generate_ext_pd_data

DESCRIPTION
  This function generates extended pd data

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_generate_ext_pd_data( const sm_FixInfoStructType *fix_ptr);

/*===========================================================================

FUNCTION tm_core_handle_gtp_wwan_unavailability

DESCRIPTION
  This function is called to handle the scenario, When GTP WWAn is disabled through Caas.

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_gtp_wwan_unavailability(sm_ReqEnumType source);

/*===========================================================================

FUNCTION tm_core_get_gps_state

DESCRIPTION
  This function is called to get the GPS Receiver state information

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_state(
   tm_core_get_gps_state_param_s_type *p_get_gps_state_param
   );

/*===========================================================================

FUNCTION tm_core_get_gps_state_para_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_cmd_err_e_type tm_core_get_gps_state_err_check(void);


/*===========================================================================

FUNCTION tm_core_get_best_avail_pos_err_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_cmd_err_e_type tm_core_get_best_avail_pos_err_check(void);


/*===========================================================================

FUNCTION tm_core_report_gps_state_info

DESCRIPTION
  This function is called by MGP to return the gps state info
  for on-demand positioning and user requested state info

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_report_gps_state_info(void *pos_data);

/*===========================================================================

FUNCTION tm_core_get_utc_offset

DESCRIPTION
  This function returns the UTC offset obtained from MGP. If UTC offset is invalid, then the UTC offset will contain
  default value from MGP.

RETURN VALUE
  Boolean: TRUE: If UTC offset is valid
                FALSE: If UTC offset is invalid

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tm_core_get_utc_offset(uint8 *p_utc_offset);
#if 0
/*===========================================================================

FUNCTION tm_core_save_ref_position

DESCRIPTION
  This function saves the reference position received from the network so
   it can be reported at the next time tick

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS

==========================================================================*/
void tm_core_save_ref_position(pdsm_lite_ext_status_info_s_type   *pd_ext_status_info);
#endif

/*===========================================================================

FUNCTION tm_on_demand_get_punc_tunc

DESCRIPTION
  This function returns the PUNC tunc values

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_get_punc_tunc(FLT *p_punc, FLT *p_tunc);


/*===========================================================================

FUNCTION tm_core_get_xtra_validity_age

DESCRIPTION
  This function returns the NV item for XTRA data validity age

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_xtra_validity_age(uint16 *p_age);


/*===========================================================================

FUNCTION tm_core_get_xtra_validity_age

DESCRIPTION
  This function returns the NV item for XTRA data validity age

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_xtra_validity_age(uint16 *p_age);

/*====================================================================
FUNCTION tm_core_time_validity

DESCRIPTION
   Reports whether the time from time_get_ms is valid or not

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_core_time_validity(void);

/*===========================================================================
FUNCTION tm_core_clear_cached_pos

DESCRIPTION Clears the RAM copy of the position

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_clear_cached_pos(void);

/*===========================================================================
FUNCTION tm_core_is_standalone_active

DESCRIPTION
Check if Standalone session is in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_standalone_active(void);

/*===========================================================================

FUNCTION tm_core_translate_end_status

DESCRIPTION
  This function translate internal abort reason to external end status.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_end_e_type tm_core_translate_end_status
(
   tm_sess_stop_reason_e_type  abort_reason
   );

/*===========================================================================

FUNCTION tm_core_gm_evt_handler

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_gm_evt_handler(uint32 msg_id, void *p_Data);

/*===========================================================================
FUNCTION tm_core_handle_engine_error_recovery_report

DESCRIPTION
  This function handles engine error recovery report (usually it'll be sent to
  QMI)

DEPENDENCIES

RETURN VALUE
  TRUE if success

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_engine_error_recovery_report(gnss_engine_err_recovery_rpt *pErrReport);

/*===========================================================================
FUNCTION tm_core_handle_engine_dpo_status_report

DESCRIPTION
  This function handles engine DPO status report (usually it'll be sent to
  QMI)

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
extern void tm_core_handle_engine_dpo_status_report(sm_DpoStatusReportStructType *p_DpoStatusReport);

/*===========================================================================
FUNCTION tm_core_handle_best_avail_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_best_avail_pos
(
   tm_core_get_best_avail_pos_param_s_type *p_get_best_avail_pos_param
   );

/*===========================================================================
FUNCTION tm_core_handle_avail_wwan_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_avail_wwan_pos
(
   tm_core_get_avail_wwan_pos_param_s_type *p_GetAvailWwanPosParam
   );

/*===========================================================================
FUNCTION tm_core_enable_DpoStatusReport

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE is DPO Status Report was enabled

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_enable_DpoStatusReport(boolean enable);


/*===========================================================================

FUNCTION tm_core_external_cell_db_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming external cell database update message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_external_cell_db_proc(pdsm_lite_cell_info_s_type *external_cell_db_info_ptr);

/*===========================================================================

FUNCTION tm_core_get_gm_engine_config

DESCRIPTION
  This function returns the GM Engine configuration

RETURN VALUE
  The timeout value in secs.

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gm_engine_config(geofence_engine_config_s_type **gm_config);

/*===========================================================================

FUNCTION tm_util_translate_glo_intcodephase

DESCRIPTION
 Compute the Glonass milli-second of the week from interger code phase &
 Glonass Time Of Day in milli-seconds.

DEPENDENCIES

RETURN VALUE
  GNSS milli-second of the week

SIDE EFFECTS
  None

===========================================================================*/
uint32 tm_util_translate_glo_intcodephase(uint32 const q_integer_code_phase, uint32 const q_GloMsecInDay);

/********************************************************************
*
* tm_get_imei
*
* Function description:
*  Gets the IMEI value from the GHSDI
*
* Parameters: 
*  p_imei_ptr - The retrieved IMEI to return to the caller (output)
*
* Return:
*  TRUE/FALSE
*  
*********************************************************************
*/
boolean tm_get_imei(byte *p_imei_ptr);

/*===========================================================================

FUNCTION tm_core_refresh_carrier_specific_nv

DESCRIPTION
  This function refresh carrier-specific NVs for a specific subscription


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_refresh_carrier_specific_nv(void* p_data);

/*===========================================================================
  tm_core_handle_start_cpi

  Description:

     This function handles start CPI request.
     The only called of this function shall be the Tech Selector.

 Parameters:
    v_isEmergency: Is Emergency CPI

  Return value: 
    boolean: Pass\Fail

=============================================================================*/
boolean tm_core_handle_start_cpi(boolean v_isEmergency);


/*===========================================================================
  tm_core_handle_stop_cpi

  Description:
  
     This function handles stop CPI request.
     The only called of this function shall be the Tech Selector.

 Parameters:
    void

  Return value: 
    boolean: Pass\Fail

=============================================================================*/
boolean tm_core_handle_stop_cpi(boolean v_isEmergency);

/*===========================================================================

FUNCTION tm_core_cache_leap_second_info

DESCRIPTION
  This function caches the Leap Second Info into TM, sent from ME

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_cache_leap_second_info(mgp_LeapSecondInfoStructType* pz_LeapSecInfo);

/*===========================================================================
FUNCTION tm_nmea_debug

DESCRIPTION
  This function sends the debug NMEA sentences to LocMW directly via TM and also
  on the NMEA COM port if it is open. This function is not to be confused with
  tm_nmea_debug.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_debug_report(sm_DebugNMEA* pz_DebugNMEA);

/*===========================================================================

FUNCTION tm_core_nmea_debug_info

DESCRIPTION
  This function processes NMEA debug string in TM And forwards
  it to HLOS for debugging purpose. Should not be mixed with NMEA strings
  generated with Measurement reports.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_nmea_debug_info(sm_DebugNMEA* pz_DebugNMEACustom);

/*===========================================================================

FUNCTION tm_core_xlate_sv_state

DESCRIPTION
Translates the SV state from the measurement report to PDAPI state 

DEPENDENCIES 
  None

RETURN VALUE 
PDSM state 

SIDE EFFECTS

===========================================================================*/
pdsm_lite_sv_state tm_core_xlate_sv_state(gnss_SvStateEnumType in_sv_state);

/*===========================================================================

FUNCTION tm_core_convert_op_mode

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
lm_gps_mode_e_type tm_core_convert_op_mode(tm_sess_operation_mode_e_type op_mode,
                                           tm_prtl_type                  prtl_type);

/*===========================================================================
FUNCTION  tm_core_is_active_sess_mo 

DESCRIPTION
  Checks if the current on-going session is MO AppTrack/Standalone

DEPENDENCIES

RETURN VALUE
  TRUE: If the on-going session is MO AppTrack/Standalone
  FALSE: If the on-going session is not MO AppTrack/Standalone

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_active_sess_mo(void);

/*===========================================================================

FUNCTION tm_core_handover_to_xtra()

DESCRIPTION
      This function is called by TM to request XTRA module about
      the availability of XTRA data. It marks this request made in global first,
      to keep the context . XTRA module can decide whether to start 
      GNSS session or not based on this global.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_core_handover_to_xtra(boolean v_is_tunc_valid);

/*===========================================================================

FUNCTION tm_core_active_session_mode

DESCRIPTION
  Function to get the Session mode for the session in TM Core.

RETURN VALUE
  Return : TM OP_Mode if a session is active.

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
tm_sess_operation_mode_e_type tm_core_active_session_mode();

/*===========================================================================

FUNCTION tm_core_xtra_efs_gps_fs_task_done

DESCRIPTION
  This function processes the notification from the GPS FS task and is used 
  to indicate that the EFS file operation has completed. 

INPUT PARAMS
  Pointer to the sm_GpsFsTaskNotificationDataType structure which contains
  EFS status, file operation, and filename.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_core_xtra_efs_gps_fs_task_done(sm_GpsFsTaskNotificationDataType *pz_NotificationData);


/*===========================================================================
FUNCTION tm_core_get_sarf_status

DESCRIPTION
  This function returns the current SARF status from TM config 

DEPENDENCIES

RETURN VALUE boolean: TRUE: RF test in progress; FALSE: No RF test in progress

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_sarf_status(void);

void tm_core_handle_feature_status_update(LocFM_FeatureStatusStructT *pz_featureStatus);

/*===========================================================================
FUNCTION tm_util_set_nmea_type

DESCRIPTION
  This function sets the debug nmea type

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS

===========================================================================*/

void tm_util_set_nmea_type(void);

/*===========================================================================

FUNCTION tm_util_update_constellation_config_if_in_US

DESCRIPTION
  This function updates the flag indicating if constellation config
  and ID update is needed.
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

void tm_util_update_constellation_config_if_in_US(boolean * v_is_config_update_needed);

  /*===========================================================================
  
  FUNCTION tm_core_update_constellation_info_if_in_US
  
  DESCRIPTION
    This function update the constellation config and constellation ID
    when receiver is in US.
  RETURN VALUE
    None
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
  
  ===========================================================================*/
  void tm_core_update_constellation_info_if_in_US(void);

  /*===========================================================================
  FUNCTION tm_core_get_gtp_wwan_fix_timer_val
  
  DESCRIPTION
    This function returns the timer count base on cell RAT we are camped
  
  DEPENDENCIES
  
  RETURN VALUE boolean: TRUE: RF test in progress; FALSE: No RF test in progress
  
  SIDE EFFECTS
  ===========================================================================*/

  void tm_core_get_gtp_wwan_fix_timer_val(uint32 *q_TimerCount);

  /*=============================================================================
  
  FUNCTION 
    tm_core_get_sys_status_info
  
  DESCRIPTION
    This function provides the expected session duration in enum format based on system uncertainties
  
  DEPENDENCIES
    None.
  
  PARAMETERS
    None
  
  RETURN VALUE
    uint32 : System status validity
  
  SIDE EFFECTS
    None.
  
  =============================================================================*/
  uint32 tm_core_get_sys_status_info(void);
/*===========================================================================
	
FUNCTION tm_core_handle_eph_exp_time
	
DESCRIPTION
  This function stores the eph expiry time info from PE in tm core.
	
RETURN VALUE
  None
	
DEPENDENCIES
  None
	
SIDE EFFECTS
	
===========================================================================*/
void tm_core_handle_eph_exp_time(gnss_EphExpTimeStructType *pz_eph_exp_time);

/*===========================================================================

FUNCTION tm_core_is_gnss_control_fix_operation_allowed

DESCRIPTION
  This function checks if specified GNSS fix control operation is allowed or not  .

RETURN VALUE
  TRUE : If specified bit is set in the NV
  FALSE: If specified bit is not set in the NV

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
bool tm_core_is_gnss_control_fix_operation_allowed(uint8 u_GnssControlFixOperationMask);


#ifdef __cplusplus
}
#endif

#endif /* TM_DATA_H */

