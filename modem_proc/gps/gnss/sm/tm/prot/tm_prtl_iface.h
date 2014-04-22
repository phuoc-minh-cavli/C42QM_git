#ifndef TM_PRTL_IFACE_H
#define TM_PRTL_IFACE_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

    TM_PRTL_IFACE Header File

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Position Determination module.

Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2012 - 2021 Qualcomm Technologies, Inc. 
Confidential and Proprietary - Qualcomm Technologies, Inc.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/prot/tm_prtl_iface.h#2 $  
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/20   sai     Workaround to handle Random MGP RCVR ACK OFF sent by MGP in ON state
08/21/19   ska     Added support for NMEA GSV
01/25/17   kpn     L+L changes for OTDOA session
04/02/16   mj      GERA support
10/27/15   mj      Handle OTA Reset for UMTS
10/01/15   rk      Define new internal API for protocol to know if A_GNSS position 
                   methods are supported.
09/03/15   rk      Add new enum for otdoa stop engine. 
06/09/15   mj     Add support for W+W feature
04/30/15   mj      Add new TM stop reason "TM_STOP_REASON_LTE_TO_UMTS_CSFB"
10/27/14   rk      Added LPP RSTD >24 measurement processing support
09/11/14   mj      Add asid as part of prtl_rr_event_s_type
04/17/14   skm    NV Async Read changes 
1/30/14    ss      Allowing Set Param when Geofencing Session is running
09/07/12   mj      Added new cell_db_event for SGLTE external cell id update
09/03/12   rh      Added SMS event type for SUPL NI msg
12/14/11   gk      LPP related changes
06/02/11   rh      Added PRTL_LCS_EVENT_TYPE_SET_CONFIG
01/03/11   gk      Eph throttling rework
08/23/10   gk      Added new handling for optimal modes
08/23/09   gk      Added new event for guard timer expiry and MM ind
07/29/09   gk      Add SS_MOLR error code in tm_sess_stop_reason_e_type 
07/22/09   atien   Add tm_post_ext_status_ref_loc_rep_event
07/23/09   gk     Support ODP 2.0
03/27/09   jd      Added stop code for aborting Wiper fixes after no request for some time.
04/02/09   ns      Support for sending diag info to sysd
02/04/09   atien   MOLR vs AD call flow select
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
08/25/08    gk     On-Demand GPS lock changes.
04/25/08    gk     On-Demand changes.
08/02/07    LT     Added variables for SVs non-existence message.
07/30/07    ank    Changed type for accuracy_threshold from uint8 to uint32.
07/17/07    lt     L1 interface.
06/12/07    cl     Only allow PD API access when TM task is active
05/27/07    rw     Add support for JGPS (KDDI) module
02/15/07    cl     Added support to inject pre-fix ppm
01/31/07    cl     Modified event callback function for PD API events
11/29/06    cl     Change event_payload to pointer in prtl_event_cb_f_type
06/13/06    cl     Initial Release
===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "aries_os_globals.h"
#include "aries_os_api.h"
#include "sm_api.h"
#include "tm_lm_iface.h"
#include "tm_common.h"
#include "pdapi_lite.h"
#include "tm_api.h"
#include "sys.h"

/***************************************************************************
                            Data Section
****************************************************************************/

/* Return value of operation request API */
typedef enum
{
  PRTL_START_SESS_REQ_ALLOWED = 0,      
  PRTL_START_SESS_REQ_NOT_ALLOWED,
  PRTL_START_SESS_REQ_WAIT,
  PRTL_START_SESS_REQ_PROCEED  
} prtl_start_sess_req_result_e_type;

/* Param union for a protocol auth req */ 
typedef struct
{
  pdsm_lite_client_type_e_type       client_type;
  pdsm_lite_client_id_type           client_id;
  pdsm_lite_pd_option_s_type         pd_option;        /* PD option type */
  pdsm_lite_pd_qos_type              pd_qos;           /* Ptr to user QOS for this request */  
  boolean                       passive_start;    /* for periodic PPM only */
} prtl_start_sess_req_param_s_type;

/* data request types from protocol sub-module to TM-Core */
typedef uint32                         tm_op_req_type;
#define TM_OP_REQ_NONE                 0x0000
#define TM_OP_REQ_LR                   0x0004
#define TM_OP_REQ_AIDING_DATA_STATUS   0x0008
#define TM_OP_REQ_REF_LOC              0x0010
#define TM_OP_REQ_RF_INFO              0x0020
#define TM_OP_REQ_DEL_ASSIST           0x0040

/* data request types from protocol sub-module to TM-Core */
typedef enum
{
  TM_SESS_OPERATION_MODE_NONE = -1,
  TM_SESS_OPERATION_MODE_STANDALONE,
  TM_SESS_OPERATION_MODE_FTM,
  TM_SESS_OPERATION_MODE_ODP,
} tm_sess_operation_mode_e_type;


/* Stop types from protocol sub-modules to TM-Core */ 
typedef enum
{
  TM_STOP_TYPE_NONE = -1,
  TM_STOP_TYPE_SESSION,
  TM_STOP_TYPE_OPERATION
} tm_sess_stop_e_type;
  
/* Stop reasons from protocol sub-modules to TM-Core */ 
typedef enum
{
  TM_STOP_REASON_NONE = -1,
  TM_STOP_REASON_COMPLETED,
  TM_STOP_REASON_USER_TERMINATED,
  TM_STOP_REASON_FIX_TIMEOUT,
  TM_STOP_REASON_BAD_PARAM,
  TM_STOP_REASON_MPC_REJECTED,
  TM_STOP_REASON_GENERAL_ERROR,   
  TM_STOP_REASON_TASK_OFFLINE,
  TM_STOP_REASON_ABORT_INTERNAL_SESSION,
  TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM,
  TM_STOP_REASON_TASK_STOP,
  TM_STOP_REASON_SERVING_SYSTEM_CHANGE,
  TM_STOP_REASON_ON_DEMAND_OVERRIDE,
  TM_STOP_REASON_ON_DEMAND_DISABLED,
  TM_STOP_REASON_ON_DEMAND_READY_ENABLED,
  TM_STOP_REASON_ON_DEMAND_SESSION_IN_PROGRESS,
  TM_STOP_REASON_ON_DEMAND_CHARGER_DISCONNECTED,
  TM_STOP_REASON_ON_DEMAND_DELETE_PARAMS,
  TM_STOP_REASON_ON_DEMAND_GPS_LOCKED,
  TM_STOP_REASON_SS_MOLR_ERRORS_SYSTEM_FAILURE,
  TM_STOP_REASON_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE,
  TM_STOP_REASON_SS_MOLR_ERRORS_DATA_MISSING,
  TM_STOP_REASON_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED,
  TM_STOP_REASON_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION,
  TM_STOP_REASON_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE,
  TM_STOP_REASON_SS_MOLR_ERRORS_UNDEFINED,
  TM_STOP_REASON_SYS_MODE_CHANGE,
  TM_STOP_REASON_PDE_NO_CON,         /* No connection with PDE */
  TM_STOP_REASON_PDE_CON_FAIL,       /* Connection failure with PDE */
  TM_STOP_REASON_PDE_REJECT,         /* Reject from PDE */
  TM_STOP_REASON_GNSS_SERVICE_NOT_AVAILABLE,/*MGP loading error*/
  TM_STOP_REASON_GNSS_RANDOM_RCVR_OFF_ERROR,/*Random Receiver Off error*/
} tm_sess_stop_reason_e_type;

/* Protocol to TM-Core Request types */
typedef struct
{
  tm_sess_operation_mode_e_type  op_mode;
  tm_op_req_type                 op_req;  
  boolean                        abort_curr_sess;
  uint32                         num_fixes;
  // uint32                         tbf;
  uint32                         tbf_ms;
  uint32                         accuracy_threshold;      
  uint32                         ppm_qos;
  uint32                         prm_qos;
  uint32                         lr_qos;
  uint8                          prm_prq;
  boolean                        dynamic_qos_enabled;
  boolean                        multi_report_msa; /* indicate multi MSA rept */

  /*  
  1. Need to decision qos crietira, can be combinaton of performance/timout/threshold....... 
  tm_sess_qos_s_type           qos;  
  2. What if there are multiple tm_op_req_type (PPM & PRM for example)
  */  

} tm_sess_req_start_param_s_type;


typedef struct
{
  tm_sess_stop_e_type         stop_type;
  tm_sess_stop_reason_e_type  stop_reason;
  tm_op_req_type              op_to_stop;
} tm_sess_req_stop_param_s_type;

typedef struct
{
  tm_sess_operation_mode_e_type  op_mode;
  tm_op_req_type                 op_req;  
  uint32                         num_fixes;
  // uint32                         tbf;
  uint32                         tbf_ms;
  uint32                         accuracy_threshold;        
  uint32                         ppm_qos;
  uint32                         prm_qos;
  uint32                         lr_qos;
  uint8                          prm_prq;
  boolean                        dynamic_qos_enabled;
  boolean                        multi_report_msa; /* indicate multi MSA rept */
  /*  
  1. Need to decision qos crietira, can be combinaton of performance/timout/threshold....... 
  tm_sess_qos_s_type           qos;  
  2. What if there are multiple tm_op_req_type (PPM & PRM for example)
  */  
} tm_sess_req_continue_param_s_type;

/* Stop types from TM-Core to protocol sub-modules */ 
typedef enum
{
  PRTL_SESS_STOP_NONE = -1,  
  PRTL_SESS_STOP_SINGLE_FIX,  
  PRTL_SESS_STOP_TRACKING_SESSION
} prtl_sess_stop_type;

/* Aiding data request mask */
typedef uint32     prtl_data_req_type;
#define PRTL_AIDING_DATA_REQ_NONE   0x0000
#define PRTL_AIDING_DATA_REQ_SA     0x0002
#define PRTL_AIDING_DATA_REQ_EPH    0x0004
#define PRTL_AIDING_DATA_REQ_ALM    0x0008
#define PRTL_AIDING_DATA_REQ_SEED   0x0010


/* session info types*/
typedef enum
{
  PRTL_SESS_INFO_NONE = -1,
  PRTL_SESS_INFO_LR,
  PRTL_SESS_INFO_INTERM_LR,
  PRTL_SESS_INFO_NOFIX,
  PRTL_SESS_INFO_REF_LR,  
  PRTL_SESS_INFO_STATUS,
  PRTL_SESS_INFO_RF_INFO,
  PRTL_SESS_INFO_DIAG_INFO,
  PRTL_SESS_INFO_RPT_REF_POS,   /* instruct protocol module to report ref pos */
  PRTL_SESS_INFO_SV_INFO,
} prtl_sess_info_e_type;

/* Union of all the session info struct */
typedef union
{
  sm_FixInfoStructType            lr_data;
  sm_FixInfoStructType            ref_lr_data;  
  sm_ReportRfInfoStructType       rf_info_data;
} prtl_sess_info_param_u_type;

typedef enum
{
  PRTL_EVENT_TYPE_NONE = -1,
  PRTL_EVENT_TYPE_PHONE,
  PRTL_EVENT_TYPE_PD,
  PRTL_EVENT_TYPE_PA,  
  PRTL_EVENT_TYPE_CELL_DB,
  PRTL_EVENT_TYPE_ON_DEMAND_POS, 
  PRTL_EVENT_TYPE_SMS,      /* Receive external SMS */
  PRTL_EVENT_TYPE_CARRIER_NV_REFRESH,  /*PRTL event for SIM hot swap carrier-specific NV REFRESH */
  PTRL_EVENT_TYPE_DBH_POS_REPORT /* DBH position report */
} prtl_event_e_type;


/****************** Cell DB *************************************************/

typedef enum
{
  PRTL_MAIN_MODEM_CELL_DB_UPD = 0,
  PRTL_EXTERNAL_CELL_DB_UPD 
} prtl_cell_db_event_e_type;

typedef struct
{
  cgps_ActiveRATEnumType       e_TriggerRAT;
  cgps_CellInfoCachedStruct*   cell_db_event_data_ptr;
} pdsm_main_modem_cell_db_s_type;

typedef union
{
  pdsm_main_modem_cell_db_s_type    main_modem_cell_db_payload;
  pdsm_lite_cell_info_s_type       external_modem_cell_db_payload;
} prtl_cell_db_payload_u_type;


typedef struct
{
  prtl_cell_db_event_e_type    cell_db_event;
  prtl_cell_db_payload_u_type  cell_db_event_payload;
} prtl_cell_db_event_s_type;





typedef struct
{
  tm_phone_event_e_type          event;
  tm_phone_event_payload_u_type  payload;
} prtl_phone_event_s_type;

typedef struct
{
  pdsm_pd_event_type  pd_event;
} prtl_pd_event_s_type;

typedef struct
{
  pdsm_lite_pa_e_type      pa_event;
  pdsm_lite_pa_info_type   *pa_info_ptr;
} prtl_pa_event_s_type;
/****************** NI MSG event ******************************/
typedef struct
{
  uint32  length;
  uint8  *data;
} prtl_sms_event_s_type;

typedef struct
{
  boolean read_from_hw;  /* TRUE if SM should read from HW (instead of cache) */
}prtl_carrier_nv_refressh_s_type;

/*
 *  PRTL Event Union
 */
typedef union
{
  prtl_phone_event_s_type  phone_event_payload;
  prtl_pd_event_s_type     pd_event_payload;
  prtl_pa_event_s_type     pa_event_payload;
  prtl_cell_db_event_s_type cell_db_event_payload;
  prtl_sms_event_s_type     sms_event_payload;
  prtl_carrier_nv_refressh_s_type  nv_refresh_payload;  /*PRTL event payload for NV refresh */
  sm_FixReportStructType    z_dbh_pos_report; /* DGH position report */
} prtl_event_u_type;


/* Protocol to TM-Core Request types */
typedef uint32                             tm_sess_req_type;
#define TM_SESS_REQ_NONE                   0x0000
#define TM_SESS_REQ_START                  0x0001
#define TM_SESS_REQ_STOP                   0x0002
#define TM_SESS_REQ_CONTINUE               0x0004

/* union for session request paramters from protocol sub-modules to TM-Core */
typedef union
{
  tm_sess_req_start_param_s_type         start_param;
  tm_sess_req_stop_param_s_type          stop_param;  
  tm_sess_req_continue_param_s_type      continue_param;
} tm_sess_req_param_u_type;

/* Data type returning from protocol sub-module to TM-Core */
typedef enum
{
  TM_POST_DATA_TYPE_NONE = -1,
  TM_POST_DATA_TYPE_SV_DIR,
  TM_POST_DATA_TYPE_SV_DIR_GLO,
  TM_POST_DATA_TYPE_SA,
  TM_POST_DATA_TYPE_EPH,
  TM_POST_DATA_TYPE_ALM,
  TM_POST_DATA_TYPE_SV_NO_EXIST,            /// GPS Non-existence SVs
  TM_POST_DATA_TYPE_SEED,                   /// GPS Reference Location
  TM_POST_DATA_TYPE_REFTIME,
  TM_POST_DATA_TYPE_LR,
  TM_POST_DATA_TYPE_UTC_MODEL,              /// UTC params relate to GPS system time
  TM_POST_DATA_TYPE_REFPOS_RPT,
  TM_POST_DATA_TYPE_SV_UNHEALTHY,           /// GPS SV health information (a.k.a RTI)
  TM_POST_DATA_TYPE_GNSS_SV_HEALTH_INFO,    /// GNSS SV health information (a.k.a RTI)
  TM_POST_DATA_TYPE_REFTIME_GLO,            /// GLONASS Reference Time
  TM_POST_DATA_TYPE_EPH_GLO,                /// GLONASS NavigationModel a.k.a Ephermesis
  TM_POST_DATA_TYPE_ALM_GLO,                /// GLONASS Almanac
  TM_POST_DATA_TYPE_UTC_MODEL_GLO,          /// UTC params relate to GLONASS system time
  TM_POST_DATA_TYPE_TIME_MODEL_GLO,         /// GLONASS Time model
  TM_POST_DATA_TYPE_GNSS_SV_NO_EXIST,       /// GLONASS Non-existence SVs
  TM_POST_DATA_TYPE_FTA_DATA,

  TM_POST_DATA_TYPE_REFTIME_BDS,            /// BDS Reference Time
  TM_POST_DATA_TYPE_EPH_BDS,                /// BDS NavigationModel a.k.a Ephermesis
  TM_POST_DATA_TYPE_ALM_BDS,                /// BDS Almanac
  TM_POST_DATA_TYPE_TIME_MODEL_BDS,         /// BDS Time model
} tm_post_data_e_type;

/* union all the aiding data struct */
/* data structures have been defined in LM */
typedef union
{
  /* Common */
  gnss_TimeModelType               gnss_time_model_data;
  gnss_AdvHealthStructType         gnss_sv_health_info;   /// a.k.a RealTimeIntegrity (RTI)
  gnss_SvDirStructType             gnss_sv_dir;
  gnss_RefTimeStructType           gnss_reftime_data;
  /* GPS */
  gps_SADataStructType             sa_data;
  gps_EphStructType                eph_data;
  gps_AlmStructType                alm_data;
  gps_SvNoExistStructType          sv_no_exist_data;
  gps_RefLocStructType             seed_data;
  gps_RefTimeStructType            reftime_data;
  gps_UtcInfoRptStructType         utc_model_data;
  gps_SvUnhealthyStructType        sv_unhealthy_data;
  gnss_SvNoExistStructType         sv_no_exist_info;

  /* GLONASS */
  glo_EphStructType                eph_glo_data;
  glo_AlmStructType                alm_glo_data;
  gnss_SvNoExistStructType         sv_no_exist_glo_data;
  glo_RefTimeStructType            reftime_glo_data;
  glo_UnscaledUtcStructType        utc_model_glo_data;

  /* BDS */
  bds_EphStructType                eph_bds_data;
  bds_AlmStructType                alm_bds_data;

  gnss_FTA_Information             fta_assist_data;
} tm_post_data_payload_type;

/*===========================================================================

FUNCTION: prtl_start_sess_req_f_type

DESCRIPTION:
  This function is called by TM-Core to protocol sub-module to see if a request 
  to start a session should be allowed or not

  If the incoming session req is granted, a protocol sub-module should return 
  PRTL_START_SESS_REQ_ALLOWED and populate action pointer to tell TM-Core what
  action to take.

  If a protocol sub-module needs to check with MPC, it should return 
  PRTL_START_SESS_REQ_WAIT, to put TM-Core in wait_auth state.
  Once it gets result back from MPC, it should send a asynchronous response
  back to TM-Core by (calling tm_prtl_post_event) to complete the session request.
  
  If for any reason, a protocol sub-module does not allow the session request, 
  it should return PRTL_START_SESS_REQ_NOT_ALLOWED. 

PARAMETERS:
  
===========================================================================*/
typedef prtl_start_sess_req_result_e_type (prtl_start_sess_req_f_type)
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
);

/*===========================================================================

FUNCTION: prtl_stop_sess_req_f_type

DESCRIPTION:
  This function is called by TM-Core to protocol sub-module to stop
  1. single fix
  2. tracking session
  
  Normally, a fix or a tracking session is terminated by protocol sub-modules. 
  This function provides the mechanism for TM to request to terminate a session 
  due to abnormal scenarios.
  If stop request is accepted, protocol sub-moudles should return true,
  otherwise, it should return false.

PARAMETERS:
  
===========================================================================*/
typedef boolean (prtl_stop_sess_req_f_type)
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
);

/*===========================================================================

FUNCTION: prtl_session_req_f_type

DESCRIPTION:
  This function is called by TM-Core to protocol sub-module to request aiding 
  data.

  If the incoming request req is valid, a protocol sub-module should return 
  true. Otherwise, it should return false.
 
  
PARAMETERS:
  
===========================================================================*/
typedef boolean (prtl_session_req_f_type)
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
);

/*===========================================================================

FUNCTION: prtl_session_info_f_type

DESCRIPTION:
  This function is called by TM-Core to protocol sub-module to provide session
  info data (PPM/PRM for example).

   
PARAMETERS:
  
===========================================================================*/
typedef boolean (prtl_session_info_f_type)
(
  tm_sess_handle_type                session_handle,
  prtl_sess_info_e_type              sess_info_type,
  prtl_sess_info_param_u_type        *sess_info_param
);

/*===========================================================================

FUNCTION: gm_event_handle_f_type

DESCRIPTION:
  This function is called by TM-Core to relay Geofence Manager events to protocol
  submodules.
  
  Use msg_type to decode msg_data_ptr (can be one of the following)  
   
PARAMETERS:
  
===========================================================================*/
typedef boolean (prtl_gm_event_handle_f_type)
(
  uint32   msg_type,
  void     *msg_data_ptr
);

/*===========================================================================

FUNCTION: prtl_timer_cb_f_type

DESCRIPTION:
  This function is called by TM-Core to notify protocol sub-module that a 
  paticular timer has expired.
   
PARAMETERS:

NOTE:
  Each protocol sub-module should be assigned a range of usable timer ids by
  TM-Core.
  
  This way, TM-Core does NOT have to remember which protocol sub-module to 
  forward timer events to for every single timer.
  
  All TM-Core needs is to check the timer id range and call this callback.
  
  Ex: 1x V1/V2 uses timer id range     0x1xxx
      1x CP uses timer id range        0x2xxx
      UMTS SUPL uses timer id range    0x4xxx
      UMTS CP uses timer id range      0x8xxx
  
  When TM-Core gets timer events, it can easily do an AND operation to figure
  out the proper protocol sub-modules to relay the timer event to.
    
===========================================================================*/
typedef void (prtl_timer_cb_f_type)
(
  void *timerData
);

/*===========================================================================

FUNCTION: prtl_event_cb_f_type

DESCRIPTION:
  This function is called by TM-Core to notify protocol sub-modules that an
  particular event (phone event, user response) has happened. 
   
PARAMETERS:
    
===========================================================================*/
typedef boolean (prtl_event_cb_f_type)
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload
);

typedef struct 
{
  prtl_start_sess_req_f_type        *start_sess_req_fp;
  prtl_stop_sess_req_f_type         *stop_sess_req_fp;
  prtl_session_req_f_type           *sess_req_data_fp;
  prtl_session_info_f_type          *sess_info_fp;
  prtl_timer_cb_f_type              *timer_cb_fp;
  prtl_event_cb_f_type              *event_cb_fp;
  prtl_gm_event_handle_f_type       *gm_event_handle_fp;
} tm_prtl_cb_s_type;


/***************************************************************************
                            API Section
****************************************************************************/

/*===========================================================================

FUNCTION tm_PrtlReg

DESCRIPTION
  This function is called by protocol sub-moudles to inform TM-Core sub-module 
  that it is ready to serve.

  It should provide a cb table with 5 function callbacks to TM-Core, 
  so TM-Core knows how to communicate with it.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module allows registration
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tm_prtl_reg
(
  tm_prtl_type             prtl_type,
  const tm_prtl_cb_s_type  *cb_tbl
);

/*===========================================================================

FUNCTION tm_sess_req

DESCRIPTION
  This function is called by protocol sub-moudles to start, stop a session.
  It is also called by protocol sub-modules to request aiding data and get 
  the status (validity) of aiding data.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts the request
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tm_sess_req
(
  tm_prtl_type                    prtl_type,
  tm_sess_handle_type             sess_handle,
  tm_sess_req_type                req_type, 
  const tm_sess_req_param_u_type  *req_param
); 

/*===========================================================================

FUNCTION tm_post_data
used only by protocols, not stanalone

FUNCTION tm_post_pd_event

DESCRIPTION
  This function is called by protocol sub-modules to send PD API events back 
  to applications.
  
DEPENDENCIES

RETURN VALUE
  TRUE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tm_post_pd_event
(
  tm_prtl_type                prtl_type,
  tm_sess_handle_type         sess_handle,
  pdsm_lite_pd_event_type          pd_event,
  pdsm_lite_pd_info_s_type*        pd_info_ptr,
  gnss_NavSlnFlagsStructType* p_PosSrcFlags
);


#ifdef __cplusplus
}
#endif



#endif /* TM_PRTL_IFACE_H */


