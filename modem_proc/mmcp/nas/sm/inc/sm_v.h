#ifndef NAS_SM_H
#define NAS_SM_H

/*====================================================================== 
 
$Header: //components/rel/mmcp.mpss/6.1.10/nas/sm/inc/sm_v.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
09/26/12   am      Back off timer code
09/27/12  am       Handling of unexpected BCM from Network
========================================================================*/

#include "mmcp_variation.h"
#include "assert.h"
#include "rabmtask_v.h"
#include "naslog_v.h"
#include "sys_stru_v.h"
#include "sm.h"
#include "mm.h"
/*=========================================================================
                 DEFINITIONS AND DECLARATIONS FOR THE MODULE
===========================================================================*/


#ifdef FEATURE_MBMS
#define MAX_MBMS_STATUS_CONTEXT_LENGTH 16
#endif

#define MAX_PDP_STATUS_CONTEXT_LENGTH 2

#ifdef FEATURE_EXTENDED_TI

#define MAX_PDP_NUM         255
#define MIN_MO_TI            0
#define MAX_MO_TI            127
#define MIN_MT_TI            128
#define MAX_MT_TI           254

#else

#define MAX_PDP_NUM         16 // Still, TIs are available only through 0~6 and 8~14
#define MIN_MO_TI            0
#define MAX_MO_TI            6
#define MIN_MT_TI            8
#define MAX_MT_TI           14

#endif
/* Maximum number of NSAPIs */ 

#ifdef FEATURE_MBMS
#define MIN_MBMS_NSAPI_NUM 0x80     // 128
#define MAX_NSAPI_NUM  0xFF     // MAX_NSAPI_NUM is 254 to include upto 254
#else
#define MAX_NSAPI_NUM  0x10     
#endif


#define MAX_NUM_PRIMARY_PDP  3 // maximum # of primary PDPs allowed

/* ---------------------------------------------------------------------------
* Timer Signals used in various PDP states 
* --------------------------------------------------------------------------*/
typedef enum
{
  TIMER_ACTIVE_PENDING    = 0x04,
  TIMER_MODIFY_PENDING    = 0x08,
  TIMER_INACTIVE_PENDING  = 0x10,
  TIMER_RABM_RESP_PENDING = 0x20,
  TIMER_EARLY_RAB_FAILURE = 0x30,
#ifndef FEATURE_DUAL_DATA
  TIMER_ESTABLISH_PENDING = 0x100
#else
  TIMER_ESTABLISH_PENDING = 0x40
#endif
} sm_timer_T;

/*=========================================================================
                 DEFINITIONS OF STRUCTURES OF MESSAGES
===========================================================================*/


/* ---------------------------------------------------------------------------
* Timer values for various PDP timers, defined in section 11.2.3 of TS 24.008
* --------------------------------------------------------------------------*/
#define ACTIVE_PENDING_TIME       30000
#define MODIFY_PENDING_TIME       8000
#define INACTIVE_PENDING_TIME     8000

/* ---------------------------------------------------------------------------
* RABM insanity timer, used during a RABM response (indirect RRC response)
* --------------------------------------------------------------------------*/
#define RABM_PENDING_RESP_TIME    20000

/* ---------------------------------------------------------------------------
* This timer is defined to delay implicit RAB Deactivate for UE not sending RAB setup failure in
case of local deactivation in PDP_ACTIVE_PENDING State.
* --------------------------------------------------------------------------*/
#define EARLY_RAB_FAILURE_TIME    4000
/* ---------------------------------------------------------------------------
* GMMSM Establishment Timer
* --------------------------------------------------------------------------*/
#define ESTABLISH_PENDING_TIME    6000
#define EXTENDED_ESTABLISH_PENDING_TIME    8000
#define MAX_TIMER_CNT     0x4


/* Macro for controlling modem statistics operations on first subscription */
#define SM_MODEM_STATISTICS_SUB1\
                               (SYS_MODEM_AS_ID_1 == SM_SUB) || (mm_dual_standby_mode == FALSE)

#define SM_IS_AS_ID_IN_VALID_RANGE(as_id) \
            (((as_id < SYS_MODEM_AS_ID_1) || (as_id >= SYS_MODEM_AS_ID_1 + MAX_AS_IDS))? FALSE : TRUE)

/* -----------------------------------------------------------------------
** Enum values for different Traffic class information from the QOS. 
** Defined in section 10.5.6.5 IE for traffic class field.
** ---------------------------------------------------------------------*/
typedef enum
{
  UNDEFINED_CLASS = -1,
  SUBSCRIBED_CLASS,
  CONVERSATION_CLASS,
  STREAMING_CLASS,
  INTERACTIVE_CLASS,
  BACKGROUND_CLASS,
  TRAFFIC_CLASS_LAST_BIT =7,
  MBMS_RECEPTION_CLASS=20,
  MBMS_PTP_RB_REQUEST_CLASS
}traffic_class_enum_type;

#ifdef FEATURE_LTE
#define ESM_MAX_EPS_BEARERS     11
typedef struct esm_psm_bearer_ctxt_s
{
  eps_bearer_id_T eps_bearer_id;
#ifdef FEATURE_NAS_GW
  rad_priority_T  radio_pri;
  pfi_T           packet_flow_id;
#endif
} esm_psm_bearer_ctxt_s_type;
typedef enum
{
  ESM_BC_ON_ATTACH_DEF_BEARER_PENDING,
  ESM_BC_ON_ATTACH_DEF_BEARER_FAILED,
  ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS,
  ESM_BC_ON_ATTACH_ATTACH_WITHOUT_PDN,
  ESM_BC_ON_ATTACH_DEF_BEARER_NONE
} esm_bc_on_attach_def_bearer_state_T;

typedef struct sm_lte_psm_cache_s
{
  esm_psm_bearer_ctxt_s_type psm_bc[ESM_MAX_EPS_BEARERS];
  esm_bc_on_attach_def_bearer_state_T attach_without_pdn;  //PSM_CHANGE
} sm_lte_psm_cache_s_type;
#endif

typedef struct sm_psm_pdp_ctxt_s
{
  byte            nsapi;
  rad_priority_T  radio_pri;
  pfi_T            packet_flow_id;
  byte             connection_id;
} sm_psm_pdp_ctxt_s_type;


typedef struct sm_gsm_psm_cache_s
{
  sm_psm_pdp_ctxt_s_type psm_pdp[MAX_PDP_NUM];
} sm_gsm_psm_cache_s_type;


typedef struct sm_psm_cache_s
{
#ifdef FEATURE_LTE
  sm_lte_psm_cache_s_type  lte;
#endif
  sm_gsm_psm_cache_s_type  gsm;
} sm_psm_cache_s_type;

#ifdef FEATURE_DUAL_DATA
/**
** Enumeration of Active Subscription ID's 
*/
typedef enum {

  SM_AS_ID_1 = 0,
    /**< Subscription ID 1 */

  SM_AS_ID_2,
    /**< Subscription ID 2 */

#if defined(FEATURE_TRIPLE_SIM)
  SM_AS_ID_3,
#endif
} sm_as_id_type_e;

extern sm_as_id_type_e sm_as_id;

#else
extern sys_modem_as_id_e_type sm_as_id;
extern sys_modem_as_id_e_type ctxt_as_id;
#endif

#ifdef FEATURE_DUAL_DATA
/*To store the Trans ID for NSAPIs*/
extern int nsapi_list_sim[MAX_AS_IDS][MAX_PDP_NUM];
#define nsapi_list nsapi_list_sim[sm_as_id]

extern word timers_pending_sim[MAX_AS_IDS][1];
#define timers_pending timers_pending_sim[sm_as_id]

extern boolean sig_connection_available_sim[MAX_AS_IDS];
#define sig_connection_available sig_connection_available_sim[sm_as_id]

extern boolean ps_signalling_conn_state_sim[MAX_AS_IDS];
#define ps_signalling_conn_state ps_signalling_conn_state_sim[sm_as_id]

extern boolean timer_est_pen_flag_sim[MAX_AS_IDS];
#define timer_est_pen_flag timer_est_pen_flag_sim[sm_as_id]

extern byte qos_ie_length_sim[MAX_AS_IDS];
#define qos_ie_length qos_ie_length_sim[sm_as_id]


extern boolean sm_is_pdp_retranmsit_req_sim[MAX_AS_IDS];
#define sm_is_pdp_retranmsit_req sm_is_pdp_retranmsit_req_sim[sm_as_id]

extern byte ps_call_info_ti_sim[MAX_AS_IDS];
#define ps_call_info_ti ps_call_info_ti_sim[sm_as_id]

extern boolean cm_initiated_ps_signalling_release_sim[MAX_AS_IDS];
#define cm_initiated_ps_signalling_release cm_initiated_ps_signalling_release_sim[sm_as_id]

extern smreg_ps_signalling_rel_req_cause_T cm_initiated_ps_signalling_release_cause_sim[MAX_AS_IDS];
#define cm_initiated_ps_signalling_release_cause cm_initiated_ps_signalling_release_cause_sim[sm_as_id]

#ifdef FEATURE_GSM_GPRS      
extern gmmsm_unitdata_ind_T pending_sm_pdu_sim[MAX_AS_IDS];
#define pending_sm_pdu pending_sm_pdu_sim[sm_as_id]

extern byte pending_sm_pdu_id_sim[MAX_AS_IDS];
#define pending_sm_pdu_id pending_sm_pdu_id_sim[sm_as_id]

#endif
extern boolean sm_abort_cnf_pending_sim[MAX_AS_IDS];
#define sm_abort_cnf_pending sm_abort_cnf_pending_sim[sm_as_id]

rex_timer_type gmmsm_establish_timer_sim[MAX_AS_IDS];/* Timer for GMM-SM Establishment */
#define gmmsm_establish_timer gmmsm_establish_timer_sim[sm_as_id]

#define pdp_list pdp_list_sim[sm_as_id]
#define pdp_list_static pdp_list_static_sim[sm_as_id]

extern word sm_max_timer_count_sim[MAX_AS_IDS][MAX_PDP_NUM];
#define sm_max_timer_count sm_max_timer_count_sim[sm_as_id]

extern word max_est_timer_count_sim[MAX_AS_IDS];
#define max_est_timer_count max_est_timer_count_sim[sm_as_id]

extern sm_psm_cache_s_type *sm_psm_cache_ptr_sim[MAX_AS_IDS];
#define sm_psm_cache_ptr sm_psm_cache_ptr_sim[sm_as_id]

#else


/*To store the Trans ID for NSAPIs*/
extern int nsapi_list[];
extern word timers_pending[];
extern boolean sig_connection_available;
extern boolean ps_signalling_conn_state;
extern boolean timer_est_pen_flag;
extern byte qos_ie_length;
extern boolean sm_is_pdp_retranmsit_req;
extern byte ps_call_info_ti ;

/* Indicates if CM has requested for ps signalling release */
extern boolean cm_initiated_ps_signalling_release;
extern smreg_ps_signalling_rel_req_cause_T cm_initiated_ps_signalling_release_cause;

#ifdef FEATURE_GSM_GPRS      
extern gmmsm_unitdata_ind_T pending_sm_pdu;
extern byte pending_sm_pdu_id;
#endif
extern boolean sm_abort_cnf_pending;

extern rex_timer_type gmmsm_establish_timer;
#ifndef FEATURE_MODEM_HEAP
extern pdp_context_info  pdp_list_static[MAX_PDP_NUM];
#endif

extern word sm_max_timer_count[MAX_PDP_NUM], max_est_timer_count;

extern sm_psm_cache_s_type *sm_psm_cache_ptr;

#endif /* #ifdef FEATURE_DUAL_DATA*/

//extern boolean inter_system_change_pending;
extern rex_crit_sect_type  sm_crit_sect;


extern sys_modem_device_mode_e_type     mmode_nv_setting_for_device_mode;

#if 0
typedef struct sm_blocked_apn_list
{
  uint16                  length;  
  apn_T                  apn_name[MAX_BLOCKED_APN];
} sm_blocked_apn_list_T;

#ifdef FEATURE_DUAL_SIM
extern sm_blocked_apn_list_T sm_blocked_apn_list_sim[MAX_AS_IDS];
#define sm_blocked_apn_list sm_blocked_apn_list_sim[sm_as_id]
#else
extern sm_blocked_apn_list_T sm_blocked_apn_list;
#endif

#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#define SM_DEBUG_MESSAGE_BUFFER_SIZE      40

/* This is log messages for SM/ESM */
typedef enum 
{
  TO_SM,
  FROM_SM,
  TO_ESM,
  FROM_ESM,
  TO_RABM
}sm_esm_message_direction_type_T;

typedef struct sm_esm_debug_buffer_s
{ 
  union sm_debug_buffer
  { 
    struct sm_info
    {
      byte message_set;
      dword message_id;
    } sm_info_debug_buffer;
#ifdef FEATURE_LTE
    struct esm_info
    {
      msgr_umid_type     msg_umid;    
    } esm_info_debug_buffer;
#endif
  } debug_buffer_type;
//  byte present_emm_state;
  sm_esm_message_direction_type_T sm_esm_message_direction;
  sys_modem_as_id_e_type as_id;
}sm_esm_debug_buffer_s_type;


/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_CM_CMD(), defined in smcm.c

** DESCRIPTION  : Processes the Commands received from Call Manager.
** ----------------------------------------------------------------------- */
//extern void sm_process_cm_cmd(sm_cmd_type* cmd_ptr);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_DS_CMD(), defined in smcm.c

** DESCRIPTION  : Processes the Commands received from Data Services.
** ----------------------------------------------------------------------- */
extern void sm_process_ds_cmd(mm_cmd_type* cmd_ptr);


/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_SNDCP_CMD(), defined in smsndcp.c

** DESCRIPTION  : Processes the Commands received from SNDCP entity.
** ----------------------------------------------------------------------- */
extern void sm_process_sndcp_cmd(sm_cmd_type* cmd_ptr);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GMM_CMD(), defined in smgmm.c

** DESCRIPTION  : Processes the Commands received from GMM entity.
** ----------------------------------------------------------------------- */
//extern void sm_process_gmm_cmd(sm_cmd_type* cmd_ptr);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_RABM_CMD(), defined in smrabm.c

** DESCRIPTION  : Processes the Command received from RABM entity.
** ----------------------------------------------------------------------- */
extern void sm_process_rabm_cmd(sm_cmd_type* cmd_ptr);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_notify_rabm(), defined in smrabm.c

** DESCRIPTION  :This function notifies the RABM about a PDP activation,
                 Modification or deactivation procedures.
** ----------------------------------------------------------------------- */
extern void sm_notify_rabm(int rab_cmd, byte nsapi);



/* -----------------------------------------------------------------------
** FUNCTION     : sm_send_cm_confirm(), defined in smcm.c

** DESCRIPTION  : Sends a confirmation to the Call Manager.
** ----------------------------------------------------------------------- */
extern void sm_send_cm_confirm_ti_cause( byte con_id, 
                         sm_cm_status_T cause, cm_sm_cmd_type cm_cmd);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_send_cm_confirm(), defined in smcm.c

** DESCRIPTION  : Sends a confirmation to the Call Manager.
** ----------------------------------------------------------------------- */
extern void sm_send_cm_confirm( pdp_context_info* cur_pdp, 
                         cm_sm_cmd_type cm_cmd, cause_enum_type_T cause_type);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_continue_pdp_deactivate(), defined in smcm.c

** DESCRIPTION  : Continues the PDP deactivation after timer expires.
** ----------------------------------------------------------------------- */
extern void sm_continue_pdp_deactivate(pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_continue_pdp_modify(), defined in smcm.c

** DESCRIPTION  : Continues the PDP modification after timer expires..
** ----------------------------------------------------------------------- */
extern void sm_continue_pdp_modify(pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_process_cm_cmd(), defined in smcm.c

** DESCRIPTION  : Continues the PDP activation after timer expires.
** ----------------------------------------------------------------------- */
extern void sm_continue_pdp_activation(pdp_context_info* pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_SM_PDU(), defined in smgmm.c

** DESCRIPTION  : This function processes a received SM-PDU from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_sm_pdu(gmmsm_unitdata_ind_T* sm_pdu);
/* -----------------------------------------------------------------------*/


#ifdef FEATURE_MBMS

/* -------------------------------------------------------------------------
** FUNCTION     : SM_CONTINUE_MBMS_ACTIVATION()
** DESCRIPTION  : Continues the MBMS activation after timer expires.
** ------------------------------------------------------------------------ */
extern void sm_continue_mbms_activation(pdp_context_info* pdp);

/* -------------------------------------------------------------------------
** FUNCTION     : SM_MBMS_ACTIVATE_REJ_RSP()
** DESCRIPTION  : Sends the Reject message to a Network requested activation.
** ------------------------------------------------------------------------ */
extern void sm_mbms_activate_rej_rsp(byte con_id,
                                    const smreg_mbms_activate_rej_resp_T* rej_rsp);

/* -------------------------------------------------------------------------
** FUNCTION     : SM_CONTINUE_MBMS_ACTIVATE_REJ_RSP()
** DESCRIPTION  : Continues sending the MBMS activation Rejection msg after timer expires.
** ------------------------------------------------------------------------ */

extern void sm_continue_mbms_activate_rej_rsp(pdp_context_info* pdp);

#endif


extern void sm_continue_pdp_activate_rej_rsp(pdp_context_info* pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : send_gmm_smpdu(), defined in smgmm.c

** DESCRIPTION  : Sends the SM-PDU to the GMM task.
** ----------------------------------------------------------------------- */
extern void send_gmm_smpdu(word mesg_len, byte* l3_buf, boolean pdp_inactive_flag, boolean ps_vote_flag);

/* -----------------------------------------------------------------------
** FUNCTION     : send_gmm_establish_req(), defined in smgmm.c

** DESCRIPTION  : Sends the GMM_ESTABLISH_REQ command to the GMM task.
** ----------------------------------------------------------------------- */
extern void send_gmm_establish_req(
#ifdef FEATURE_WCDMA
rrc_establish_cause_e_type est_cause
#endif
);

/* -----------------------------------------------------------------------
** FUNCTION     : send_gmm_close_session_req(), defined in smgmm.c

** DESCRIPTION  : Sends the GMM_CLOSE_SESSION_REQ command to the GMM task.
** ----------------------------------------------------------------------- */
extern void send_gmm_close_session_req(void);


/*===========================================================================
FUNCTION SEND_GMM_PS_CALL_STATUS_IND

DESCRIPTION
  This function sends the status of the PS call to the GMM to be sent to the Network. 
  
** ----------------------------------------------------------------------- */
extern void send_gmm_ps_call_status_ind(boolean pdp_sig_in_progress);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_CHECK_PS_CALL_IN_PROGRESS(), defined in smgmm.c

** DESCRIPTION  : returns TRUE if any PS procedure is ongoing, FALSE otherwise
** ----------------------------------------------------------------------- */
extern boolean sm_check_ps_call_in_progress(void);

/* -----------------------------------------------------------------------
** FUNCTION     : send_gmm_fast_dormancy_req (), defined in smgmm.c

** DESCRIPTION  : Sends the GMMSM_FAST_DORMANCY_REQ command to the GMM task.
** ----------------------------------------------------------------------- */
extern void send_gmm_fast_dormancy_req (void);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_CLEAR_PDP_TIMER(), defined in smtask.c

** DESCRIPTION  : This function clears a PDP timer.
** ----------------------------------------------------------------------- */
extern void sm_clear_pdp_timer (sm_timer_T timer_type);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SET_PDP_TIMER(), defined in smtask.c

** DESCRIPTION  : This function clears a PDP timer.
** ----------------------------------------------------------------------- */
extern void sm_set_pdp_timer (sm_timer_T timer_type);

/* -------------------------------------------------------------------------
** FUNCTION     : SM_PDP_ACTIVATE_REJ_RSP()
** DESCRIPTION  : Sends the Reject message to a Network requested activation.
** ------------------------------------------------------------------------ */
extern void sm_pdp_activate_rej_rsp(byte con_id,
                                    mm_pdp_activate_rej_resp_T* rej_rsp);
/* -----------------------------------------------------------------------
** FUNCTION     : RABM_GPRS_RELEASE_IND 

** DESCRIPTION  : This function sends a GPRS releases Indication to RABM.
** ----------------------------------------------------------------------- */
extern void rabm_gprs_release_ind(void);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_LOG_PDP_INFO 

** DESCRIPTION  : This function logs the PDP Context info to DIAG.
** ----------------------------------------------------------------------- */
extern boolean sm_log_pdp_info(void);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_LOG_QOS_INFO 

** DESCRIPTION  : This function logs the PDP Context info to DIAG.
** ----------------------------------------------------------------------- */
extern boolean sm_log_qos_info(pdp_context_info *pdp);


/* -----------------------------------------------------------------------
** FUNCTION     : SM_LOG_PDP_REQUEST_REJECT_EVENT 

** DESCRIPTION  : This function logs the PDP Activate Request or Reject and
**                PDP Deactivate Request.
** ----------------------------------------------------------------------- */
extern void sm_log_pdp_request_reject_event(byte request, 
                                            byte message_type, byte cause);


/* -----------------------------------------------------------------------
** FUNCTION     : is_WtoGCCO_in_progress

** DESCRIPTION  : This function returns TRUE if W to G CCO is in progress
** ----------------------------------------------------------------------- */

extern boolean is_WtoGCCO_in_progress
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);

/* -----------------------------------------------------------------------
** FUNCTION     : CONVERT_RABM_RRC_EST_CAUSE()
** DESCRIPTION  : This function processes commands sent by CM.
** ----------------------------------------------------------------------- */
#ifdef FEATURE_WCDMA
extern rrc_establish_cause_e_type convert_to_rrc_est_cause(byte traffic_class );
#endif
/*--------------------------------------------------------------------------
** FUNCTION SM_GET_MOST_DEMANDING_TRAFFIC_CLASS
** DESCRIPTION: This function returns most demanding traffic class of the PDP contexts 
                exists
----------------------------------------------------------------------------*/
#if defined(FEATURE_NAS_REL12) && defined(FEATURE_WCDMA)
extern byte sm_get_most_demanding_traffic_class(boolean include_active_pending );
#else
extern byte sm_get_most_demanding_traffic_class(void);
#endif



/* -----------------------------------------------------------------------
** FUNCTION     : SM_PERFORM_LOCAL_PDP_DEACTIVATION
** DESCRIPTION  : This function performs a local deactivation of a particluar 
                  ACTIVE PDP context.
** ----------------------------------------------------------------------- */
extern void sm_perform_local_pdp_deactivation( pdp_context_info *pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : SM_LOCAL_DEACT_ALL_ASSOCIATED_PDP_CTXT
** DESCRIPTION  : This function locally deactivates  all associated ACTIVE PDP contexts.
** ----------------------------------------------------------------------- */
extern void sm_local_deact_all_associated_pdp_ctxt( pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PERFORM_LOCAL_DEACTIVATION
** DESCRIPTION  : This function locally deactivates any ACTIVE PDP contexts.
** ----------------------------------------------------------------------- */
extern void sm_perform_local_deactivation( byte cause );

/* -----------------------------------------------------------------------
** FUNCTION     : SM_VALIDATE_QOS

** DESCRIPTION  : This function validates the network offered QoS against 
**                the minimum QoS requirements of the UE.
** ----------------------------------------------------------------------- */
extern boolean sm_validate_qos ( qos_T offered_qos, qos_T min_qos );

/* -----------------------------------------------------------------------
**  FUNCTION     : SM_PACK_PDP_CONTEXT_STATUS

**  DESCRIPTION  : This function packs the PDP context status for 16 
**                 NSAPIs(0-15) in 16 bits
** ----------------------------------------------------------------------- */
extern void sm_pack_pdp_context_status(byte *ext_buf, sys_modem_as_id_e_type as_id); 

#ifdef FEATURE_MBMS

/* -----------------------------------------------------------------------
**  FUNCTION     : SM_PACK_MBMS_CONTEXT_STATUS

**  DESCRIPTION  : This function packs the MBMS context status for 128 
**                 NSAPIs(128-254) in 16*8 bits
** ----------------------------------------------------------------------- */
extern byte sm_pack_mbms_context_status(byte *ext_buf);

#endif
/* -----------------------------------------------------------------------
** FUNCTION     : SM_HANDLE_QOS_VALIDATION_FAILURE

** DESCRIPTION  : This function handles the QoS validation failure and 
**                initiates the PDP deactivation procedures.
** ----------------------------------------------------------------------- */
extern void sm_handle_qos_validation_failure ( pdp_context_info * );

/* -----------------------------------------------------------------------
FUNCTION    :SM_CHECK_PRI_DUPLICATE_PDP

DESCRIPTION :This function checks whether the requested PDP is already existing or not
-------------------------------------------------------------------------*/
extern byte sm_check_pri_duplicate_pdp(pdp_context_info *);

/* -----------------------------------------------------------------------
**FUNCTION SM_CHECK_PDP_COLLISION

**DESCRIPTION
  This function checks whether there is an MO and MT collision.
------------------------------------------------------------------------*/
extern byte sm_check_pdp_collision( pdp_context_info *pdp);

/* -----------------------------------------------------------------------
FUNCTION    :SM_COMPARE_APN

DESCRIPTION :This function compares the two APNs
-------------------------------------------------------------------------*/
extern boolean sm_compare_apn(apn_T *, apn_T *);

/* -----------------------------------------------------------------------
FUNCTION SM_COMPARE_PDPADDR

DESCRIPTION This function compares the two PDP address
-------------------------------------------------------------------------*/
extern boolean sm_compare_pdpaddr(pdp_address_T *, pdp_address_T *);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
/* -----------------------------------------------------------------------
FUNCTION SM_PROMOTE_SEC_PRI

DESCRIPTION This function picks the best QoS from the secondaries and 
            promote to primary
-------------------------------------------------------------------------*/
extern boolean sm_promote_sec_pri(pdp_context_info *cur_pdp);
#endif /* FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL */

/* -----------------------------------------------------------------------
FUNCTION SM_CHECK_GROUP_PDP_PENDING_DEACTIVATION

DESCRIPTION
  This function checks whether any of the PDP context in the group is in 
    bringing down state
-------------------------------------------------------------------------*/
extern boolean sm_check_group_pdp_pending_deactivation(pdp_context_info *cur_pdp);
/* -----------------------------------------------------------------------
FUNCTION SM_RESELEASE_ALL_NSAPI

DESCRIPTION
  This function releases all the nsapi in nspai list
  This function should only be called from SM 
-------------------------------------------------------------------------*/
extern int sm_release_all_nsapi (void);
/* -----------------------------------------------------------------------
FUNCTION SM_PICK_BEST_QOS

DESCRIPTION This function picks the best QoS from the secondaries 
            and promote to primary
-------------------------------------------------------------------------*/
byte sm_pick_best_qos(pdp_context_info* pri_pdp, pdp_context_info* sec_pdp);

/* -----------------------------------------------------------------------
FUNCTION SM_RELEASE_NSAPI_SM

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
  To be called only from SM
-------------------------------------------------------------------------*/
extern int sm_release_nsapi_sm ( byte connection_id );

/* -----------------------------------------------------------------------
FUNCTION GET_TRANSID

DESCRIPTION
  This function replaces macro GET_TRANSID. Gets transaction id for a NSAPI
-------------------------------------------------------------------------*/
extern int GET_TRANSID (byte nsapi);

#ifdef FEATURE_DUAL_DATA
/* -----------------------------------------------------------------------
FUNCTION GET_TRANSID

DESCRIPTION
  This function replaces macro GET_TRANSID. Gets transaction id for a NSAPI
-------------------------------------------------------------------------*/
extern int GET_TRANSID_PER_SUB (byte nsapi, sys_modem_as_id_e_type as_id);
#endif

/* -----------------------------------------------------------------------
FUNCTION GET_NSAPI

DESCRIPTION
  This function replaces macro GET_NSAPI. Gets nsapi from connectio id.
-------------------------------------------------------------------------*/ 
extern byte GET_NSAPI (byte conn_id);

/* -----------------------------------------------------------------------
FUNCTION SM_INIT_PDP_CONTEXT

DESCRIPTION
  Initialize a PDP context.
-------------------------------------------------------------------------*/
extern void sm_init_pdp_context (pdp_context_info *pdp);
/* --------------------------------------------------------------------------
**FUNCTION    :SM_ABORT_GMM_CONN_PROC
**DESCRIPTION :Aborts GMM connection procedure
** ------------------------------------------------------------------------*/
extern void sm_abort_gmm_conn_proc(cc_cause_T abort_cause);


#ifdef FEATURE_MBMS
/*===========================================================================
FUNCTION SM_DEACT_ASSOCIATED_MBMS_CTXT

DESCRIPTION
  This function checks whether any of the MBMS context is associated with PDP
  and deactivates them locally.
===========================================================================*/
extern void sm_deact_associated_mbms_ctxt(const pdp_context_info *cur_pdp);
/*===========================================================================
FUNCTION SM_ALL_MBMS_LOCAL_DEACTIVATION

DESCRIPTION
  This function deactivates aa of the MBMS context.
===========================================================================*/
extern void sm_all_mbms_local_deactivation (void);
#endif

/* --------------------------------------------------------------------------
**FUNCTION    :SM_SEND_CM_PS_SIG_REL_CONFIRM
**DESCRIPTION :Sends a PS signal release confirmation to the Call Manager.
** ------------------------------------------------------------------------*/
extern void sm_send_cm_ps_sig_rel_confirm(ps_sig_rel_status_T status_t);

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
FUNCTION CHECK_PDP_INACTIVE_STATE

DESCRIPTION
  This function checks if
  a. Only One PDP exists and is in Deactivation process
  b. All the existing PDP are in Deactivation process
  
DEPENDENCIES
  
  
RETURN VALUE

  return TRUE, if the above condition passes.

SIDE EFFECTS

===========================================================================*/

extern boolean check_pdp_inactive_state(void);
#endif


/* --------------------------------------------------------------------------
**FUNCTION    :SM_GET_BCM_FROM_PCO
**DESCRIPTION : This function extracts BCM mode from PCO received from Network
** ------------------------------------------------------------------------*/
extern bearer_ctrl_mode_T sm_get_bcm_from_pco(const protocol_cfg_options_T *pco);

/* --------------------------------------------------------------------------
**FUNCTION    :SM_SET_BCM_IN_PCO
**DESCRIPTION : This function set BCM mode in PCO
** ------------------------------------------------------------------------*/
extern void sm_set_bcm_in_pco(pdp_context_info* cur_pdp, bearer_ctrl_mode_T bcm );

/* -----------------------------------------------------------------------
** FUNCTION     : sm_continue_pdp_nw_modify_rej(), defined in smcm.c

** DESCRIPTION  : Continues the PDP activation after timer expires.
** ----------------------------------------------------------------------- */
extern void sm_continue_pdp_nw_modify_rej(pdp_context_info* pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_continue_pdp_nw_modify_cnf(), defined in smcm.c

** DESCRIPTION  : Continues the PDP activation after timer expires.
** ----------------------------------------------------------------------- */
extern void sm_continue_pdp_nw_modify_cnf(pdp_context_info* pdp);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR, "ds%d" str , (sub+1), (p1), (p2), (p3))
/* Warning: 3rd argument is not logged */
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
     ERR_FATAL ("ds%d" str , (sub+1), (p1), (p2))

#ifdef FEATURE_DUAL_SIM
#define SM_SUB (sm_as_id)
#else
#define SM_SUB 0
#endif

#else

#define SM_SUB 0

/* If DUAL SIM is not defined, no need to print subscription id */
#ifndef MSG_HIGH_DS
#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_HIGH (str , (p1), (p2), (p3))
#endif
#ifndef MSG_MED_DS
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_MED (str , (p1), (p2), (p3))
#endif
#ifndef MSG_LOW_DS
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_LOW (str , (p1), (p2), (p3))
#endif
#ifndef MSG_ERROR_DS
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_ERROR (str , (p1), (p2), (p3))
#endif
#ifndef MSG_FATAL_DS
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
     ERR_FATAL (str , (p1), (p2), (p3))
#endif

#endif

#define RABM_SUB SM_SUB

#ifdef FEATURE_DUAL_DATA
#define RABM_LOCAL_SUB (as_id)
#else
#define RABM_LOCAL_SUB SM_SUB
#endif

/*===========================================================================
FUNCTION SM_GET_PS_CALL_INFO

DESCRIPTION
   This function fills PDP address,QOS and APN values of activa PDP context
   in MM buffer
DEPENDENCIES
  
  
RETURN VALUE

     NONE

SIDE EFFECTS

===========================================================================*/
extern void sm_get_ps_call_info(sys_ps_call_info_s_type *ps_call_info);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_generate_group_id()
** DESCRIPTION  : Generated group id
** ----------------------------------------------------------------------- */
extern byte sm_generate_group_id(void);


#if defined(FEATURE_LTE)
/* -----------------------------------------------------------------------
** FUNCTION     : SM_IS_PDP_CREATED_AFTER_ISR()
** DESCRIPTION  : This function called by GMM to check if PDP activated after exsists.
* ----------------------------------------------------------------------- */
boolean sm_check_isr_deact_status_on_interrat(void);
#endif

/*===========================================================================

FUNCTION GMMSM_PROCESS_RECEIVE_NPDU_NUMBER_LIST

DESCRIPTION
  SM function for use by GMM to process a Receive NPDU Number List IE in a 
  single threaded process
  
  The NPDU list is passed as list pointers to valid flags and Receive NPDU
  numbers. List length must be 11 covering NSAPI5 to NSAPI15.
  
  The data at the location given by the specified pointers is modified to
  return the exchange NPDU Number list for transmission.

  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

extern void gmmsm_process_receive_npdu_number_list
(
  boolean *valid_list_p,
  uint8   *npdu_num_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);
/* --------------------------------------------------------------------------

FUNCTION GMMSM_PROCESS_RECEIVE_NPDU_NUMBER_LIST

DESCRIPTION
  
  Processes a received NPDU Number List and generates an exchange list for
  transmission back to the network. The exchange list is constructed through
  an SMSN_SEQ_IND/SNSM_SEQ_RSP exchange with SNDCP.
** ------------------------------------------------------------------------*/  
extern void sm_process_receive_npdu_number_list
(
  boolean *valid_list_p,
  uint8   *npdu_num_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);

/* --------------------------------------------------------------------------
** Primitive : SM_IS_PDP_ACTIVE, returns TRUE if any PDP is active
** ------------------------------------------------------------------------*/
extern boolean sm_is_pdp_active
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);

/* --------------------------------------------------------------------------
** Primitive : SM_IS_PDP_ACTIVATION_PENDING, returns TRUE if any PDP
**  
** ------------------------------------------------------------------------*/
extern boolean sm_is_pdp_activation_pending
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);

/* --------------------------------------------------------------------------
** Primitive : SM_GET_REVISION_LEVEL_INDICATOR, return ue_rev_level_indicator_type
**  
** ------------------------------------------------------------------------*/
extern ue_rev_level_indicator_type sm_get_revision_level_indicator(void);


/* --------------------------------------------------------------------------
** Primitive : SM_IS_APN_BLOCKED, returns TRUE if APN is blocked
**  
** ------------------------------------------------------------------------*/
//extern boolean sm_is_apn_blocked(apn_T apn);

extern void sm_check_for_null_ptr(void* msg_p);

/*===========================================================================
FUNCTION  sm_psm_read_cache

DESCRIPTION
  This function reads PSM cache from EFS

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void sm_psm_read_cache(sm_psm_cache_s_type *psm_cache_ptr);

/*===========================================================================
FUNCTION  sm_psm_write_cache

DESCRIPTION
  This function converts the PSM cache to a byte array and
  saves it in EFS

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void sm_psm_write_cache(sm_psm_cache_s_type *psm_cache_ptr);

/*==============================================================================
FUNCTION  sm_send_cm_restore_ctxt_psm_req

DESCRIPTION
 This function is used to send a request to DS via CM to restore all bearers
 upon wake up from PSM that existed in the previous LTE session before
 entering PSM

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
/*============================================================================*/
extern void sm_send_cm_restore_ctxt_psm_req (void);
/*===========================================================================
FUNCTION SM_PRO_BLOCKED_APN_LIST 

DESCRIPTION
  This function overwrites the existing SM APN blocking list with one provide in this  funtion call.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void sm_cm_process_restore_ctxt_psm_rsp (mm_restore_psm_ctxt_rsp_T *restore_psm_ctxt_rsp);


/*==============================================================================
FUNCTION  sm_restore_psm_context

DESCRIPTION
 This function restores PSM context upon exiting PSM

DEPENDENCIES
 None

RETURN VALUE
 PSM context restored successfully, else FALSE

SIDE EFFECTS
 None
/*============================================================================*/
extern boolean sm_restore_psm_context(void);


/*==============================================================================
FUNCTION  sm_save_psm_context

DESCRIPTION
 This function saves PSM context upon entering PSM

DEPENDENCIES
 None

RETURN VALUE
 PSM context saved successfully, else FALSE

SIDE EFFECTS
 None
/*============================================================================*/
extern boolean sm_save_psm_context(void);


/*===========================================================================
FUNCTION SMREG_RESTORE_CTXT_PSM_RSP 

DESCRIPTION
    This function processes SMREG_RESTORE_CTXT_PSM_RSP from CM.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void sm_cm_process_restore_ctxt_psm_rsp (mm_restore_psm_ctxt_rsp_T *restore_psm_ctxt_rsp);

/*==============================================================================
FUNCTION  sm_send_gmm_save_ctxt_psm_rsp

DESCRIPTION
 This function is used to send a response to GMM for GMMSM_SAVE_CTXT_PSM_REQ

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
/*============================================================================*/
extern void sm_send_gmm_save_ctxt_psm_cnf
(
  boolean status
);

extern void sm_process_save_ctxt_psm_req (sm_cmd_type *cmd_ptr);
extern void sm_process_restore_ctxt_psm_req (sm_cmd_type *cmd_ptr);
extern void sm_send_gmm_restore_ctxt_psm_cnf(boolean status);

//extern void sm_process_gprs_rel(gmmsm_gprs_release_ind_T* gprs_rel);


/* ========================================================================
** LOCAL FUNCTION PROTOTYPES.
** These functions are available only in the scope of this file.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_CONNECTION_CNF()

** DESCRIPTION  : This function processes GPRS attach response from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_connection_cnf(
        gmmsm_establish_cnf_T* gmmsm_establish_cnf,
        boolean retry
);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_CONNECTION_IND()

** DESCRIPTION  : 
   This function processes the GMM connection ind to indicate GMM is in 
   PMM_connected.state. SM has to trigger the PDP context modification procedure
   for PDPs with trafiic class streaming and conversational to bring upgrade the
   bitrate to original values.
** ----------------------------------------------------------------------- */
extern void sm_process_connection_ind(gmmsm_establish_ind_T* gprs_ind);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GMM_CON_RELEASE_IND()

** DESCRIPTION  : This function processes Error Indications from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_gmm_con_release_ind(gmmsm_con_release_ind_T *release_ind);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_FAST_DORMANCY_CNF ()

** DESCRIPTION  : This function processes dormancy Indications from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_fast_dormancy_cnf (gmmsm_fast_dormancy_cnf_T*dormancy_cnf);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GPRS_REJ()

** DESCRIPTION  : This function processes GPRS attach reject from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_gprs_rej(gmmsm_establish_rej_T* gprs_rej);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GPRS_REL()

** DESCRIPTION  : This function processes GPRS release indication from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_gprs_rel(gmmsm_gprs_release_ind_T* gprs_rel);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SM_STATUS()

** DESCRIPTION  : This function sends the SM-STATUS message to GMM.
** ----------------------------------------------------------------------- */
extern void sm_send_sm_status( pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_SM_PDU()

** DESCRIPTION  : This function processes a received SM-PDU from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_sm_pdu(gmmsm_unitdata_ind_T* sm_pdu);
/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_PDP_STATUS()

** DESCRIPTION  : This function processes a received PDP-CONTEXT-STatus IE  from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_pdp_status(gmmsm_pdp_mbms_context_status_T* gmm_pdp_mbms_context_status);

/** FUNCTION     : SM_PROCESS_PDP_MBMS_STATUS()

** DESCRIPTION  : This function processes a received PDP-CONTEXT-STatus IE  from GMM.
** ----------------------------------------------------------------------- */
extern void sm_process_pdp_mbms_status(const gmmsm_pdp_mbms_context_status_T* gmm_pdp_mbms_context_status);

extern void sm_process_status_ind(byte trans_id);


#ifdef FEATURE_LTE
extern void sm_process_isr_status_ind(gmmsm_isr_status_ind_t  gmm_isr_status_ind);
extern void sm_process_multimode_subs_chgd_req
(
  void
);
#endif
extern void sm_unblock_all_apn(sys_modem_as_id_e_type as_id);

extern void sm_process_con_rel_rej(
#ifdef FEATURE_DUAL_SIM
        sm_cmd_type* cmd_ptr
#else
        void
#endif
);
extern uint32 sm_tlli;

extern void gmm_handle_connection_management( mm_cmd_type *message );
extern void sm_process_timer_expiry_cmd(sm_cmd_type* cmd_ptr);
extern void sm_proc_timer_expired(word con_id, sm_timer_T timer_type);
extern void sm_ulog_register(void);
extern void sm_init( void );

/*===========================================================================
FUNCTION  sm_psm_free_cache

DESCRIPTION
  This function freeds the SM related PSM context in cache.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: EFS/NV written successfully, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern void sm_psm_free_cache(void);


/*===========================================================================
FUNCTION SM_MMGSDI_IN_TRAFFIC_CHANNEL_MODE

DESCRIPTION
  Inform GSDI that data call is connected/disconnected, so GSDI can start/stop
  in call polling.

  in_traffic is a boolean which indicates whether we are in traffic or not.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
extern void sm_mmgsdi_in_traffic_channel_mode(boolean in_traffic);


#endif /* NAS_SM_H */


