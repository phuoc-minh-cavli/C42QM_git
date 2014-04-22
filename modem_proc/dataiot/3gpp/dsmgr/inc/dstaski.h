#ifndef DSTASKI_H
#define DSTASKI_H
/*===========================================================================
                      DSTASKI.H
DESCRIPTION
This file exposes data structure and API related to internal task operation.

Copyright (c) 2018-2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    pk      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"              
#include "customer.h"

#include "amssassert.h"
#include "rex.h"
#include "task.h"
#include "nv.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif

extern nv_item_type         ds_nv_item;
extern nv_stat_enum_type    ds_nv_status;

/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO         IS_IN_DS_TASK

DESCRIPTION   Wrapper for dsi_is_in_ds_task()

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
#define IS_DS_TASK() (ds_is_in_ds_task())

#define DS_CMD_PAYLOAD_SIZE_ZERO  (size_t)0

#define GET_SIG(sig) (1 << sig)

#if 0
/*===========================================================================
MACRO         DS_FORCE_ASSERT

DESCRIPTION   Called by DS when it wants to force an ASSERT.  Since LINT
              hates an ASSERT(0), do it here witht the LINT comments.

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
#define DS_FORCE_ASSERT()                                                  \
    /*lint -save -e506 -e774 */                                                  \
      ASSERT(0);                                                           \
    /*lint -restore */
#endif

#ifdef FEATURE_REX_SIGS_EXT
#define DS_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
#else
#define DS_SIG_ARR_LEN 1
#endif

/*--------------------------------------------------------------------------
  Minimum signal value which lies in the extension signal range.
---------------------------------------------------------------------------*/
#define DS_MIN_EXT_SIGNAL (sizeof(rex_sigs_type)*DS_SIG_ARR_LEN*8)

#if 0
/*--------------------------------------------------------------------------
    DS_SET_SIG sets the signal sig_num in the mask pointed to by val_ptr
 ---------------------------------------------------------------------------*/
#define P=DS_SET_SIG(val_ptr, sig_num)\
    {\
      *(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) |=  \
        ((rex_sigs_type) 1 << SIG_OFFSET(sig_num)); \
    }
#endif

/*---------------------------------------------------------------------------
  Data Services Task Signal Definitions
---------------------------------------------------------------------------*/

typedef enum {

  DS_TASK_MIN_SIG  = 0,
  DS_CMD_Q_SIG     = 1,
  DS_DOG_HB_REPORT_SIG = 2,
  DS_NV_CMD_SIGNAL               = 3,/* NV cmd event signal           */
  DS_TIMER_SIG =    4,
#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)
  DS_RMNET_FC_SIGNAL             = 5,  /* FC signal to rmnet            */
#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

#ifdef FEATURE_QSAP_SERVICE
  DS_QSAP_SIGNAL                 = 6,  /* Reserved for QSAP             */
#endif /* FEATURE_QSAP_SERVICE */

  DS_DSD_RDR_SIGNAL            = 7,/* Reader sig for QMI DSD    */
  DS_DFS_RDR_SIGNAL            = 8, /* Reader sig for QMI DFS     */
  DS_DMS_RDR_SIGNAL            = 9, /* Reader sig for QMI DMS     */
  DS_NIPD_RDR_SIGNAL           = 10, /* Reader sig for QMI NIPD    */
  DS_WDS_RDR_SIGNAL            = 11, /*Reader sig for QMI WDSLITE */ 
  DS_WDA_RDR_SIGNAL            = 12,  /* Reader sig for QMI WDA */
  /*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  DS_TASK_START_SIG =  13,
  DS_TASK_STOP_SIG  = 14,
  DS_TASK_OFFLINE_SIG = 15,
 
  DS_AT_RDR_SIGNAL            = 18,	 /* Reader sig for QMI AT	   * TODO we do not need ATP service*/

  DS_DATA_PORT_1ST_SIO_RX_SIG  = 19,
  DS_DTR_EVENT_SIG             = 20,
  DS_SIO_RX_PKT_SIG            = 21,
  DS_SIO_ESCAPE_SIG            = 22,
  DS_AT_MT_MSG_SIG             = 23,
  DS_AT_SMS_SIG                = 24,
  DS_AT_CM_MSG_SIG             = 25,
  DS_A2_DEREG_SIG              = 26,
  DS_QTEELS_CLIENT_SIG         = 27,
  DS_DINS_RDR_SIGNAL           = 28,

  DS_MAX_SIGNALS
}ds_sig_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Commands that may be sent to the Data Services Task.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum
{
  DS_MIN_CMD = 0,

    /*-------------------------------------------------------------------------
    PDN_MGR Commands ()
  -------------------------------------------------------------------------*/
  DS_CMD_PDN_INSTANCE_UP,
  DS_CMD_PDN_GET_PDN_CONN_REQ_IND,
  DS_CMD_PDN_ACT_DEFAULT_BEARER_REQ,
  DS_CMD_PDN_ACT_DEDICATED_BEARER_REQ,
  DS_CMD_PDN_DEACT_BEARER_REQ,
  DS_CMD_PDN_CONN_REJ_IND,
  DS_CMD_PDN_CONN_FAIL_IND,
  DS_CMD_PDN_RAB_REEST_IND,
  DS_CMD_PDN_DORM_REORIG,
  DS_CMD_PDN_DORM_REORIG_TMR_EXP,  //10
  DS_CMD_PDN_RAB_RELEASE_IND,
  DS_CMD_PDN_RAB_REJECT_IND,
  DS_CMD_PDN_ATTACH_COMPLETE_IND,
  DS_CMD_PDN_EPS_BEARER_GO_NULL,
  DS_CMD_PDN_MODIFY_BEARER_REQUEST_IND,
  DS_CMD_PDN_FLOW_FLTR_ID_CONFLICT,
  DS_CMD_PDN_FLOW_FLTR_PRECED_CONFLICT,
  DS_CMD_PDN_EMM_DATA_TRANSPORT_IND,
  DS_CMD_PDN_EMM_DATA_TRANSPORT_CNF,
  DS_CMD_PDN_TAU_COMPLETE_IND,   //20
  DS_CMD_PDN_T3402_CHANGED_IND,
  DS_CMD_CONTEXT_TRANSFER_IND,
  DS_CMD_PDN_EMM_DETACH_IND,
  DS_CMD_PDN_REMOVE_UNUSED_PDN,
  DS_CMD_PDN_NW_CIOT_CAP_CHG,
  DS_CMD_PDN_RATE_CTRL_INTERVAL_TMR_EXP,
  DS_CMD_PDN_CP_DATA_RETRY_TIMER_EXP,
  DS_CMD_PDN_CP_DATA_RAI_TIMER_EXP,
  DS_CMD_PDN_CP_DATA_L2_NOTIFY,
  DS_CMD_PDN_PSM_READY,    //30
  DS_CMD_PDN_PSM_ENTER,
  DS_CMD_PDN_PSM_RESTORE_CNTX_REQ,
  DS_CMD_PDN_RESTORE_BEARER_CNTX_RES,
  DS_CMD_PDN_RESTORE_PDP_CNTX_RES,
#ifdef FEATURE_NBIOT_NTN
  DS_CMD_PDN_MM_STORE_NTN_IND,
#endif /* FEATURE_NBIOT_NTN */
  DS_CMD_PDN_INACTIVITY_TMR_EXP,
  DS_CMD_PDN_POLLING_TMR_EXP,
  DS_CMD_PDN_APN_PARAM_CHG_TMR_EXP,
  DS_CMD_PDN_PROFILE_PARAM_CHG_CMD,
  DS_CMD_PDN_WWAN_TECH_SUSPEND_IND,
  DS_CMD_PDN_T3448_STATUS_IND,       //40
  DS_CMD_PDN_BARRING_STATUS_IND,
  DS_CMD_PDN_TEARDOWN_INVALID_PCSCF,

  DS_CMD_PDP_ACTIVATE_CNF,
  DS_CMD_PDP_DEACTIVATE_IND,
  DS_CMD_GSNDCP_REG_CNF,             
  DS_CMD_GSNDCP_RESUME_REG_CNF,
  
  DS_CMD_MSGR_RECV,
  DS_CMD_PLMN_CHANGE_IND,
  DS_CMD_CM_SS_EV_SRV_CHG,
  DS_CMD_CM_PH_EV_INFO,
  DS_CMD_CM_PWROFF_LPM_IND,
  DS_CMD_CM_SUB_NOT_AVAIL_IND,        //50
  DS_CMD_MMGSDI_SIM_INFO,
  DS_CMD_PDNMGR_CM_PH_EV,
  DS_CMD_PDN_PENDING_MO_EX_DATA_IND,
  DS_CMD_PDN_DSM_MEM_CTRL_EV,

  //throtting related commands
  DS_CMD_3GPP_THROT_TMR_EXP,
  DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD,    
  DS_CMD_3GPP_RAU_COMPLTE_CMD,          
  DS_CMD_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP,
  DS_CMD_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR,
  DS_CMD_3GPP_PDN_THROT_SAMPLING_TIMER_EXP,
  DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN,
  DS_CMD_3GPP_PDN_THROT_CLEAR_APN_REJ_T3402_TMR,
  
  DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP,
  DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP,
  DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP,
  DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP,
  DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START,
  DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR,
  DS_CMD_3GPP_SYS_SEL_PREF_ERR,
  DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP,

  DS_CMD_3GPP_RPM_SAMPLING_TIMER_EXP,
  DS_CMD_3GPP_RPM_LR3_TIMER_EXP,
  DS_CMD_3GPP_RPM_HOURLY_TIMER_EXP,
  DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_FAIL,
  DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_CNF,
  DS_CMD_PDN_CLAT_SM_CREATE,
  
  
#ifdef FEATURE_MODE_TLB
  DS_CMD_TLB_MODE_B_HANDLER,
#endif /* FEATURE_MODE_TLB */

  DS_CMD_PDN_CPSR_STATUS_IND,
  
    /*-------------------------------------------------------------------------
    DS_SYS Commands ()
  -------------------------------------------------------------------------*/
  DS_CMD_SYS_EVENT_MIN                        = 1000,
  DS_CMD_SYS_EVENT_HANDLER                    = 1001,
  DS_CMD_SYS_EVENT_MAX                        = 1999,
    /*-------------------------------------------------------------------------
    QMI Commands ()
  -------------------------------------------------------------------------*/
  DS_QMI_MIN_CMD         =  2000, //DCC_MIN_CMD                                  = -1,
 /*-------------------------------------------------------------------------
    Network interface - command to handle WWAN Rm state machine events
  -------------------------------------------------------------------------*/
  DS_QMI_RMNET_SM_EV_CMD  =     2005 ,//DCC_RMNET_SM_EV_CMD = 5,

  /*-------------------------------------------------------------------------
    Network interface - commands for RmNet
  -------------------------------------------------------------------------*/
  DS_QMI_RMNET_SM_CMD          =2006, //DCC_RMNET_SM_CMD = 6,

  /*-------------------------------------------------------------------------
    Network interface - QMI receive command
  -------------------------------------------------------------------------*/
  DS_QMI_QMUX_RX_CMD    =2010,  //DCC_QMUX_RX_CMD = 10,

  /*-------------------------------------------------------------------------
    Network interface - command to QMI module
  -------------------------------------------------------------------------*/
  DS_QMI_CMD          =2011 ,//DCC_QMI_CMD = 11,
  
  /*-------------------------------------------------------------------------
    MIP CCoA cmds
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_PS_UICC
  /*-------------------------------------------------------------------------
    UICC command
  -------------------------------------------------------------------------*/
  //DCC_UICC_CMD                       = 60,   /* UICC N/W interface cmds  */ To check in rmnetifacetls
  //DCC_UICC_SM_CMD                    = 61,   /* UICC State Machine cmds  */   To check in rmifacectls
#endif /* FEATURE_DATA_PS_UICC */

  /*-------------------------------------------------------------------------
    QMI Service init and Request cmd
  -------------------------------------------------------------------------*/
  DS_QMI_INIT_LEGACY_SERVICES_CMD             =2091, //DCC_QMI_INIT_LEGACY_SERVICES_CMD     = 91,
  DS_QMI_RECV_LEGACY_SERVICES_REQ_CMD         =2092, //DCC_QMI_RECV_LEGACY_SERVICES_REQ_CMD = 92,
  DS_QMI_RMNET_XPORT_CMD                      =2093, //DCC_RMNET_XPORT_CMD                  = 93,
  
#ifdef FEATURE_DATA_PS_464XLAT  //Clat will be needed in next release
  DS_CLAT_SM_START_CMD                        =2109, //DCC_CLAT_SM_START_CMD = 112,
  DS_CLAT_SM_STOP_CMD                         =2110, //DCC_CLAT_SM_STOP_CMD = 113,
  DS_CLAT_SM_IPV4_IFACE_UP_CMD                =2111, //DCC_CLAT_SM_IPV4_IFACE_UP_CMD = 114,
  DS_CLAT_SM_IPV4_IFACE_DOWN_CMD              =2112, //DCC_CLAT_SM_IPV4_IFACE_DOWN_CMD = 115,
  DS_CLAT_SM_IPV6_IFACE_UP_CMD                =2113, //DCC_CLAT_SM_IPV6_IFACE_UP_CMD = 116,
  DS_CLAT_SM_IPV6_IFACE_DOWN_CMD              =2114, //DCC_CLAT_SM_IPV6_IFACE_DOWN_CMD = 117,
  DS_CLAT_SM_IPV6_IFACE_PREFIX_UPDATE_CMD     =2115, //DCC_CLAT_SM_IPV6_IFACE_PREFIX_UPDATE_CMD = 118,
  DS_CLAT_SM_PLAT_AVAILABLE_CMD               =2116, //DCC_CLAT_SM_PLAT_AVAILABLE_CMD = 119,
  DS_CLAT_SM_PLAT_UNAVAILABLE_CMD             =2117, //DCC_CLAT_SM_PLAT_UNAVAILABLE_CMD = 120,
  DS_CLAT_SM_DNS_PROXY_ENABLED_CMD            =2118, //DCC_CLAT_SM_DNS_PROXY_ENABLED_CMD = 121,
  DS_CLAT_SM_DNS_PROXY_DISABLED_CMD           =2119, //DCC_CLAT_SM_DNS_PROXY_DISABLED_CMD = 122,
  DS_CLAT_SM_DNS64_CALLBACK_CMD               =2120, //DCC_CLAT_SM_DNS64_CALLBACK_CMD = 123,
  DS_CLAT_SM_DPM_CONTEXT_REGISTERED_CMD       =2121, //DCC_CLAT_SM_DPM_CONTEXT_REGISTERED_CMD = 124,
#endif /* FEATURE_DATA_PS_464XLAT */

  DS_QMI_DSD_SVC_CMD                          =2122,  //DCC_DSD_SVC_CMD = 109,
  DS_QMI_DMS_SVC_CMD                          =2123, //DCC_DMS_SVC_CMD = 125,  
  DS_QMI_DFS_SVC_CMD                          =2124,//DCC_DFS_SVC_CMD = 111,
  DS_QMI_NIPD_SVC_CMD                         =2125, //DCC_NIPD_SVC_CMD = 126,
  DS_QMI_WDS_SVC_CMD                          =2126, //DCC_WDS_SVC_CMD =127,
  DS_QMI_WDA_SVC_CMD                          =2127, //DCC_WDA_SVC_CMD  
  DS_QMI_AT_SVC_CMD	                          =2128,//DCC_ATP_SVC_CMD = 112, //To check if this service is needed
//Temp Manoj  
  DS_QMI_QMUX_CMD                             =2129,

  DS_QMI_QMAP_CMD                             =2130,
  DS_QMI_CTL_SVC_CMD                          =2131,
  DS_QMI_COMMON_IF_CMD                        =2132,  
  DS_QMI_DINS_SVC_CMD                         =2133,
  /* Always must be last */
  DS_QMI_MAX_CMD                              =2999,

  /*-------------------------------------------------------------------------
    DSS Commands ()
  -------------------------------------------------------------------------*/
  DS_CMD_DSS_BRINGUP_CALL_EVT = 3000,
  DS_CMD_DSS_BRINGDOWN_CALL_EVT,
  DS_CMD_DSS_CALL_CALLBACK_EVT,

    /*-------------------------------------------------------------------------
    RMSM Commands ()
  -------------------------------------------------------------------------*/
  /* TIMER, RDM AND DS3GSIOLIB COMMANDS */

  DS_CMD_SIOLIB_TIMER_EXPIRED                         = 4000,
  /* Notification of timer expiry */

  DS_CMD_SIOLIB_RDM_OPEN                              = 4001,
  /* Cmd from RDM to open a serial port */

  DS_CMD_SIOLIB_RDM_CLOSE                             = 4002,
  /* Cmd from RDM to close a serial port */

  DS_CMD_SIOLIB_CLOSE_COMPLETE                        = 4003,
  /* Cmd from SIOLIB when sio_close is done */

  DS_CMD_SIOLIB_DSM_MEM_CTRL                          = 4004,
  /* Cmd from SIOLIB */

  DS_CMD_SIOLIB_DTR_CHANGED                           = 4005,
  /* DTR changed event notification */

  DS_CMD_SIOLIB_CD_CHANGED                            = 4006,
  /* CD changed event notification */

  DS_CMD_SIOLIB_ENTER_ONLINE_CMD_TX_FLUSH             = 4007,
  /* Goto online flush TX */

  DS_CMD_SIOLIB_ENTER_ONLINE_DATA_TX_FLUSH            = 4008,
  /* Goto online-data, flush TX */

  DS_CMD_SIOLIB_ENTER_ONLINE_DATA_NO_TX_FLUSH         = 4009,
  /* Goto online-data, don't flush TX */

  DS_CMD_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH            = 4010,
  /* OK to finish online-cmd switch   */

  DS_CMD_SIOLIB_COMPLETE_ONLINE_DATA_SWITCH           = 4011,
  /* OK to finish online-data switch  */
  DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL = 4012,
  DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED = 4013,
  DS_CMD_3GPP_RMSM_RM_PPP_UP         = 4014,
  DS_CMD_3GPP_RMSM_RM_PPP_DOWN       = 4015,
  DS_CMD_3GPP_RMSM_RM_BRING_UP_PDN   = 4016,
  DS_CMD_3GPP_RMSM_UM_PDN_UP         = 4017,
  DS_CMD_3GPP_RMSM_UM_PDN_DOWN       = 4018,
  DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER    = 4019,
  DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER  = 4020,
  DS_CMD_3GPP_RMSM_RM_IPCP_CREQ_RECD = 4021,
  DS_CMD_3GPP_RMSM_RM_IPV6CP_CREQ_RECD=4022,
  DS_CMD_3GPP_RMSM_PPP_UP_DELAY_TIMER= 4023,
  DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE = 4024,
  DS_CMD_3GPP_RMSM_ADDR_CFG_END      = 4025,
  DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL     = 4026,
  DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED  = 4027,
  DS_CMD_3GPP_RMSM_AUTOCONFIG_SM     = 4028,

  DS_CMD_3GPP_RMSM_AT_PDN_EV         = 4029,
  DS_CMD_3GPP_RMSM_AT_SEND_DATA      = 4030,
  DS_CMD_3GPP_RMSM_CONFIG_WAIT_TIMER = 4031,

    /*-------------------------------------------------------------------------
    ATCOP Commands ()
  -------------------------------------------------------------------------*/

  /*================ AT COMMAND PROCESSOR (values 5000 - 5999) ================*/

  /*-------------------------------------------------------------------------
  ATCoP asynchronous events
  -------------------------------------------------------------------------*/
  DS_CMD_ATCOP_ASYNC_EVENT_START              = 5000, 
  /* Start of ATCoP asynchronous events */

  /*-------------------------------------------------------------------------
  CDMA and ETSI SMS AT asynchronous events
  -------------------------------------------------------------------------*/

  DS_CMD_ATCOP_SMS_ERR_CMD                    = 5001,
  /* Error Event from UASMS */

  DS_CMD_ATCOP_SMS_ABT_CMD                    = 5002,
  /* Event to tell ATCOP to abort cmd */

  DS_CMD_ATCOP_TIMER_EXPIRED_CMD              = 5003,
  /* a ATCOP timer expire	*/

  DS_CMD_ATCOP_CM_CALL_CMD                    = 5004,
  /* Call feedback from CM  */

  DS_CMD_ATCOP_CM_CALL_INFO_CMD               = 5005,
  /* Call information from CM	*/
  
  DS_CMD_ATCOP_CM_SS_CMD                      = 5006,
  /* Serving System feedback from CM  */
  
  DS_CMD_ATCOP_CM_SS_INFO_CMD                 = 5007,
  /* Serving System information from CM  */
  
  DS_CMD_ATCOP_CM_PH_CMD                      = 5008,
  /* Phone command feedback from CM */
  
  DS_CMD_ATCOP_CM_PH_INFO_CMD                 = 5009,
  /* Phone information from CM   */
  
  DS_CMD_ATCOP_CM_INBAND_CMD                  = 5010,
  /* Inband cmd from CM */
  
  DS_CMD_ATCOP_CM_INBAND_INFO_CMD             = 5011,
  /* Inband event from CM	*/
  
  DS_CMD_ATCOP_GSDI_SIM_INFO                  = 5012,
  /* SIM read/write response from GSDI */
  
  /*-------------------------------------------------------------------------
  WCDMA and GSM asynchronous AT events/commands
  -------------------------------------------------------------------------*/
  DS_CMD_ATCOP_STATUS_CMD                     = 5013,
  /* used to report the excution status of an */
  /* async func call.  The callback function of  */
  /* the  async command submit this cmd. */
  
  DS_CMD_ATCOP_CM_SUPS_CMD                    = 5014,
  /* Supplemental Services status from CM   */
  
  DS_CMD_ATCOP_CM_SUPS_INFO_CMD               = 5015,
  /* Supplemental Services response from CM */
  
  DS_CMD_ATCOP_MMGSDI_APDU_RESP               = 5016,
  /* Response APDU sent from GSDI */
  
  /*-------------------------------------------------------------------------
  GSM FAX asynchronous AT events/commands
  -------------------------------------------------------------------------*/
  DS_CMD_ATCOP_FPS_T31_CMD_STATUS             = 5017,
  /* GSM Fax command status  */
  
  DS_CMD_ATCOP_PBM_CB_CMD                     = 5018,
  /* Pbm async function's call back
  function submits this command to send
  the results to ATCOP */
  
  DS_CMD_ATCOP_TO_SIO_LOW_CMD                 = 5019,
  /* ds-to-sio watermark hits low */
  
  DS_CMD_ATCOP_SEND_AT_CMD                    = 5020,
  
  DS_CMD_ATCOP_FWD_CLIENT_REG                 = 5021,
  DS_CMD_ATCOP_FWD_CLIENT_DEREG               = 5022,
  DS_CMD_ATCOP_FWD_AT_CMD_REG                 = 5023,
  DS_CMD_ATCOP_EXT_CMD_RESP                   = 5024,
  DS_CMD_ATCOP_RESET_REQ_CMD                  = 5025,
  DS_CMD_ATCOP_SHUTDOWN_REQ_CMD               = 5026,
  DS_CMD_ATCOP_EXT_CMD_URC                    = 5027,
  
  DS_CMD_ATCOP_MMGSDI_INFO_CMD                = 5028,
  /* Response info sent from MMGSDI */
  
  DS_CMD_ATCOP_MMGSDI_SIM_INFO                = 5029,
  /* SIM read/write response from MMGSDI */
  
  DS_CMD_ATCOP_COPS_AT_CMD                    = 5030,
  DS_CMD_ATCOP_CPOL_AT_CMD                    = 5031,
  
  DS_CMD_ATCOP_MMGSDI_AUTH_CMD                = 5032,
  /* MMGSDI authentication commands result */
  
  DS_CMD_ATCOP_MD5_ABT_CMD                    = 5033,
  /* Event to tell ATCoP to abort MD5 command */
  
  DS_CMD_ATCOP_PDP_CMD                        = 5034,
  /* AT PDP cmd */
  
  DS_CMD_ATCOP_FTM_DIAGPKT_RSP_CMD            = 5035,
  /* Response sent from FTM diag request. */
  
  DS_CMD_ATCOP_ECALL_CMD                      = 5036,
  DS_CMD_ATCOP_PD_INFO_CMD                    = 5037,
  /* GPS information from PDSM */
  
  DS_CMD_ATCOP_MMGSDI_INIT_INFO               = 5038,
  /* Response info sent from MMGSDI */
  
  DS_CMD_ATCOP_MMGSDI_OPER_NAME_INFO          = 5039,
  
  DS_CMD_ATCOP_RMNET_EV_CMD                   = 5040,
  /* Rmnet related Events */
  
  DS_CMD_ATCOP_EAP_SIM_AKA_TASK_SWITCH_CMD    = 5041,
  DS_CMD_ATCOP_EAP_SIM_AKA_RESULT_IND_CMD     = 5042,
  DS_CMD_ATCOP_EAP_SIM_AKA_SUPP_RESULT_IND_CMD  = 5043,
  DS_CMD_ATCOP_EAP_SIM_AKA_TRP_TX_CMD         = 5044,
  DS_CMD_ATCOP_MUX_NOTIFY_CMD                 = 5045,
  DS_CMD_ATCOP_RDM_NOTIFY_CMD                 = 5046,
  DS_CMD_ATCOP_VOIPM_AT_CMD                   = 5047,
  DS_CMD_ATCOP_MMGSDI_CRSM_RESP               = 5048,
  DS_CMD_ATCOP_FWD_AT_CMD_DEREG               = 5049,
  DS_CMD_ATCOP_ASYNC_EVENT_END                = 5050,
  DS_CMD_ATCOP_PSM_GET_CONFIG_CMD             = 5051,
  DS_CMD_ATCOP_PSM_CFG_CHANGE_IND_CMD         = 5052,
  DS_CMD_ATCOP_VOICE_CONF_PARTICIPANTS_IND_CMD  = 5053,
  DS_CMD_ATCOP_EDRX_CONFIG_GET_CNF_CMD          =5054,
  DS_CMD_ATCOP_EDRX_CONFIG_SET_CNF_CMD           =5055,
  DS_CMD_ATCOP_CRTDCP_CMD                       = 5056,
  DS_CMD_ATCOP_SIO_RTS_CHANGED_CMD              = 5057,
#ifdef FEATURE_DSAT_ATCOP_VEND
  DS_CMD_ATCOP_PSM_BOOT_IMAGE_TYPE_IND_CMD      = 5058,  
  DS_CMD_ATCOP_CEINFO_CMD                       = 5059,
  DS_CMD_ATCOP_JD_SERVICE_STATUS_CMD            = 5060,/* JD-Jammer Detection */
  /* Jamming Service status received from RRC*/
  DS_CMD_ATCOP_GET_SFN_CMD                      = 5061,
  DS_CMD_ATCOP_SET_SFN_RESP_CMD                 = 5062,
  DS_CMD_ATCOP_SFN_REACHED_IND_CMD              = 5063,
  /* Jammer detection command call-back received from LTE/GSM*/
  DS_CMD_ATCOP_JAMMER_DETECT_CMD                = 5064,
  DS_CMD_ATCOP_CELL_SELECT_INFO_CMD             = 5065,
  DS_CMD_ATCOP_RESTORE_URC_INFO_IN_PSM_CMD              = 5066,

#ifdef FEATURE_FSK_TX
  DS_CMD_ATCOP_FSK_DEBUG_RSP_CMD                = 5067,
  DS_CMD_ATCOP_FSK_DEBUG_QUERY_RSP_CMD          = 5068,
  DS_CMD_ATCOP_FSK_DATA_RSP_CMD                 = 5069,
  DS_CMD_ATCOP_FSK_HOP_TABLE_RSP_CMD            = 5070,
  DS_CMD_ATCOP_FSK_HOP_TABLE_QUERY_RSP_CMD      = 5071,
  DS_CMD_ATCOP_FSK_START_RSP_CMD                = 5072,
  DS_CMD_ATCOP_FSK_STOP_RSP_CMD                 = 5073,
  DS_CMD_ATCOP_FSK_PCL_RSP_CMD                  = 5074,
  DS_CMD_ATCOP_FSK_PCL_QUERY_RSP_CMD            = 5075,  
  DS_CMD_ATCOP_FSK_DATA_ABT_CMD                 = 5076,
#endif /* FEATURE_FSK_TX */
#endif
  /* RTS is asserted */

  /* End of ATCoP asynchronous events */

  /*-------------------------------------------------------------------------
    DS Application Service commands (6000-6999)
  -------------------------------------------------------s------------------*/
  DS_APPSRV_CMD = 6000,

    /*-------------------------------------------------------------------------
    Qteels Service commands (7000-7999)
  -------------------------------------------------------------------------*/
  DS_QTEELS_SERVICE_AVAILABLE_EVT = 7000,
  DS_QTEELS_CLIENT_LICENSE_CHECK_EVT = 7001,
  DS_QTEELS_CLIENT_SYNC_COMPLETE_BROADCAST_EVT = 7002,
 
  DS_MAX_CMD
} ds_command_enum_type;


/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ds_sig_handler_type)
(
  ds_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/
typedef struct
{
  ds_sig_handler_type sig_handler;
  void *user_data_ptr;
} ds_sig_handler_info_type;

typedef void (*ds_cmd_handler_type)
(
  ds_command_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  MAX command in a module
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_MAX_CMDS_PER_MODULE  1000

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  DS Modules enum type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum 
{
  DS_MODULE_INVALID      = -1,
  DS_MODULE_PDN_MGR      = 0,
  DS_MODULE_DS_SYS       = 1,
  DS_MODULE_QMI          = 2,
  DS_MODULE_DSS          = 3,
  DS_MODULE_RMSM         = 4,
  DS_MODULE_ATCOP        = 5,
  DS_MODULE_APPSRV       = 6,
  DS_MODULE_QTEELS       = 7,
  DS_MODULE_MAX
} ds_module_enum_type_i;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Header type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  q_link_type       link;         /* Queue link type */
  ds_command_enum_type  cmd_id;       /* Identifies the command */
#ifdef FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG
  time_type         timestamp;    /* Timestamp of the command
                                     measured in ms after phone is powered on*/
#endif /*FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG*/
} ds_command_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command type: The command header identifies the command. If the command has
  any parameters, they are specified in the union.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  ds_command_hdr_type   hdr;               /* Command header */
  void             *cmd_payload_ptr;   /* Command Payload */
} ds_command_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command processing callback function type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef void (*ds_cmd_process_func_type)( ds_command_type *cmd_ptr );


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DSI_WAIT

DESCRIPTION   This function suspends execution of the Data Services task
              until one (or more) signals from the requested signal mask is
              set.
              Watchdog kicking is performed in this function.

DEPENDENCIES  None

RETURN VALUE  The signal mask returned by rex_wait().

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type  ds_wait
(
  rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
);

/*===========================================================================
FUNCTION      DSI_IS_IN_DS_TASK

DESCRIPTION   Returns TRUE if executing in DS task.

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_is_in_ds_task( void );

/*===========================================================================
FUNCTION      DS_UT_SYNC_COMMAND_HDLR

DESCRIPTION   UT Sync command

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
void ds_ut_sync_command_hdlr( void );

/*===========================================================================
FUNCTION      ds_register_cmd_hdlr

DESCRIPTION   This function registers command processing function for each 
              module.

DEPENDENCIES  None

RETURN VALUE  TRUE: callback registration successful
              TRUE: callback registration Un-successful

SIDE EFFECTS  None
===========================================================================*/
boolean ds_register_cmd_hdlr
(
  ds_module_enum_type_i       module_id,
  ds_cmd_process_func_type  cb_func_ptr
);

/*===========================================================================
FUNCTION      DS_ALLOCATE_COMMAND_BUF

DESCRIPTION   This function allocates a command buffer from the Data Services 
              Task. The caller of this function must check for a NULL return
              value, and either perform some error recovery or ERR_FATAL,
              as appropriate.

DEPENDENCIES  None

RETURN VALUE  Returns a pointer to a command buffer, if available, or
              NULL if no command buffers are available.

SIDE EFFECTS  None
===========================================================================*/
ds_command_type* ds_allocate_cmd_buf
(
  size_t payload_size  /* number of bytes to allocate for cmd payload */
);

/*===========================================================================
FUNCTION      DS_RELEASE_CMD_BUF

DESCRIPTION   This function returns a command buffer to the Data Services 
              Task's free queue.

DEPENDENCIES  The caller of this function should have previously allocated 
              a command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds_release_cmd_buf
(
  ds_command_type  **cmd_pptr           /* Pointer to command to be released */
);

/*===========================================================================
FUNCTION      DS_GET_COMMAND_QUEUE_CNT

DESCRIPTION   This function returns the count of outstanding commands that 
              are in DS command queue.

DEPENDENCIES  None

RETURN VALUE  Value of type uint8 - Indicates number of outstanding commands 
              in DS command queue.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_command_queue_cnt( void );

/*===========================================================================
FUNCTION      DS_GET_CUR_FREE_COMMAND_CNT

DESCRIPTION   This function returns the current count of free command
              buffers that are available in the DS task.

DEPENDENCIES  None

RETURN VALUE  Returns a uint8 with the number of outstanding commands 
              in the dstask.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_cur_free_command_cnt( void );


/*===========================================================================
FUNCTION      ds_put_cmd

DESCRIPTION   This function puts a command on the Data Services Task's 
              command queue, and sets the DS_CMD_Q_SIG. The command is put
              on the regular external command queue regardless of the calling
              task context, to preserve legacy behavior.

DEPENDENCIES  The caller of this function should have previously allocated a
              command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_put_cmd
(
  ds_command_type    *cmd_ptr                 /* Pointer to command to be sent */
);

/*===========================================================================

FUNCTION DS_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
void reg_set_sig_handler
(
  ds_sig_enum_type sig,              /* Signal assocaited with handler     */
  ds_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);

/*===========================================================================

FUNCTION DS_RESET_SIG_HANDLER()

DESCRIPTION
  This function resets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void reg_dereg_sig_handler
(
  ds_sig_enum_type sig
);

/*===========================================================================
FUNCTION  DS_GET_DS_TASK_PTR

DESCRIPTION   Returns DS TCB pointer

RETURN VALUE  ds_task_ptr

DEPENDENCIES  Should be called after ds_tcb_ptr has been inited .

SIDE EFFECTS  None
===========================================================================*/
rex_tcb_type* ds_get_ds_task_ptr( void );

/*===========================================================================

FUNCTION DS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/

nv_stat_enum_type 
ds_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
);


/*===========================================================================
FUNCTION DS_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

DEPENDENCIES
  This function can only be called from DCC task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/

nv_stat_enum_type 
ds_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
);

/*===========================================================================

FUNCTION DS_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the DS task and This function notifies the scheduler
  that there is processing to be performed in DS task. 
 
PARAMETERS 
  sig   : Signal to be set.
 
DEPENDENCIES
  None
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_set_signal
(
  ds_sig_enum_type sig              /* Signal to be enabled               */
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* DSTASKI_H */
