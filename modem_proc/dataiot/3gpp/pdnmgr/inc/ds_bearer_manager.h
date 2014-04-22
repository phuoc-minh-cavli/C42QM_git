/*===========================================================================
                      DS_BEARER_MANAGER.H
DESCRIPTION
This file exposes Bearer manager through API'S and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
  

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_bearer_manager.h#10 $
  $DateTime: 2023/03/23 03:53:38 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#ifndef DS_BEARER_MANAGER_H
#define DS_BEARER_MANAGER_H

#include "ds_pdn_defs.h"
#include "ds_eps_bearer_manager.h"
#include "pstimer.h"
#include "ps_svc.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define BEARER_MGR_VALID_COOKIE  0xbeabeabe

/*---------------------------------------------------------------------------
  Default Watermark Levels for LTE  
---------------------------------------------------------------------------*/
#define DSLTEPS_UL_WM_DEFAULT_LO        30000 
#define DSLTEPS_UL_WM_DEFAULT_LO_QCNT   40
#define DSLTEPS_UL_WM_DEFAULT_HI        45000 
#define DSLTEPS_UL_WM_DEFAULT_HI_QCNT   100 
#define DSLTEPS_UL_WM_DEFAULT_DNE       60000
#define DSLTEPS_UL_WM_DEFAULT_DNE_QCNT  120 //QCNT = DNE/500

#define DSLTEPS_DL_WM_DEFAULT_LO        15000
#define DSLTEPS_DL_WM_DEFAULT_HI        30000
#define DSLTEPS_DL_WM_DEFAULT_DNE       45000
#define DSLTEPS_DL_WM_DEFAULT_DNE_QCNT  90 //QCNT = DNE/500

/*---------------------------------------------------------------------------
  Default Watermark Levels for GSM
---------------------------------------------------------------------------*/
#define DSGPSD_UL_WM_DEFAULT_LO        12000
#define DSGPSD_UL_WM_DEFAULT_HI        30000
#define DSGPSD_UL_WM_DEFAULT_DNE       37500
#define DSGPSD_UL_WM_DEFAULT_DNE_QCNT  75 //QCNT = DNE/500
	
#define DSGPSD_DL_WM_DEFAULT_LO        18000
#define DSGPSD_DL_WM_DEFAULT_HI        45000
#define DSGPSD_DL_WM_DEFAULT_DNE       56250
#define DSGPSD_DL_WM_DEFAULT_DNE_QCNT  112 //QCNT = DNE/500 


#define DS_BEARER_THROUGHPUT_TMR_DEFAULT_VAL 2 /* in seconds */
  
  /*-------------------------------------------------------------------------
   TCP ACK threshold
  ---------------------------------------------------------------------------*/
#define TCP_THRESHOLD_ACK_CNT_DEF        70 /* Default number of ACKs on uplink which 
                                               would trigger BP removal */

  /*---------------------------------------------------------------------------
    Macro for RAB Reestablish retry time. This variable is used as the
    minimum interval to resend the RAB Reestablish Request when in dormant
    mode. When RAB Reestablish reject is received a timer for a random time
    between DSUMTS_RAB_REESTABLISH_MIN_RETRY_TIME and 
    DSUMTS_RAB_REESTABLISH_MIN_RETRY_TIME * 4 is started.
  ---------------------------------------------------------------------------*/
#define DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME 100
  
  /*----------------------------------------------------------------------------
  This is the defualt number of attempts when a rab reject is received with 
  ACCESS_BARRING_CAUSE_CODE
  -----------------------------------------------------------------------------*/
#define DS_BEARER_MGR_ACCESS_BARR_RAB_RETRY_COUNTER 128

/*---------------------------------------------------------------------------
Access barring time remaining
-----------------------------------------------------------------------------*/
#define ACCESS_BARRING_INFINITE_RETRY_TIME 0

#define  RAB_REESTAB_RETRY_CNT_INVALID  0x7fffffff
#define  RAB_REESTAB_RETRY_CNT_DEFAULT  3

/*---------------------------------------------------------------------------
  Flow masks
---------------------------------------------------------------------------*/
#define DS_BEARER_ALL_FLOWS_ENABLED   0x00000000  /* All flows enabled flag  */
#define DS_BEARER_DSM_LARGE_Q_MASK  0x00000001  /**< Out of large items mask. */
#define DS_BEARER_DSM_SMALL_Q_MASK  0x00000002  /**< Out of small items mask. */
#define DS_BEARER_PKT_WM_MASK       0x00000004  /**< Packet mode watermark 
                                                   mask. */
#define DS_BEARER_CONTROL_MASK          0x00000008  /**< PDN control path mask */
#define DS_BEARER_LL_MASK               0x00000010  /**< Mask for all LL events. */

#define DS_BEARER_SPLMN_RATE_CTRL_MASK  0x00000020  /**< Serving PLMN Rate Control mask. */
#define DS_BEARER_APN_RATE_CTRL_MASK    0x00000040  /**< APN Rate Control mask. */
#define DS_BEARER_CP_DATA_BACKOFF_MASK  0x00000080   /** disable flow except for CP data */
#define DS_BEARER_TECH_SUSPENSION_MASK  0x00000100  /**< Tech suspension mask */
#define DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK      0x00000200  /**< APN Rate Control mask. but allow MO EX */
#define DS_BEARER_MODIFICATION_IN_PROGRESS_MASK    0x00000400  /**< APN Rate Control mask. but allow MO EX */

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
#define DS_BEARER_GPS_SEARCH_IN_PROGRESS_MASK      0x00000800  /**< Disable flow except but allow MO EX */
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */

#define DS_BEARER_MEM_FLOW_CTL_MASK        0x00001000  /**< Disable flow for dsm flow control */
#define DS_BEARER_MO_DATA_BARRING_MASK     0x00002000  /**< MO data barring mask */
#define DS_BEARER_MO_EX_DATA_BARRING_MASK  0x00004000 /**< MO_EX data barring mask */

/*-----------------------------------------------------------------------
 This is the eunm that defines the various states of the bearer.

 DOWN                   - Bearer not in use
 COMING_UP           - Call has been initiated on this bearer
 PEND_REDIAL        - Call was internally rejected due to RAT change
                                DS will redial on the new RAT.
 UP                         - Call is connected on this bearer
                                 Traffic can be assumed to be active now.
 UP_DORMANT         - RAB has been released due to inactivity
 UP_DORMANT_REESTAB - RAB reestablish request has been sent out
                                        Bearer will remain in this state until the network returns a result for
                                        the bearer establishment request.
 GOING_DOWN        - Call hangup has been initiated on this bearer
-----------------------------------------------------------------------*/
typedef enum
{
  DS_BEARER_MGR_STATE_DOWN                  = 0,
  DS_BEARER_MGR_STATE_COMING_UP             = 1,
  DS_BEARER_MGR_STATE_PEND_REDIAL           = 2,  
  DS_BEARER_MGR_STATE_PEND_RAB_REG          = 3,
  DS_BEARER_MGR_STATE_UP                    = 4,
  DS_BEARER_MGR_STATE_UP_DORMANT            = 5,
  DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB    = 6,
  DS_BEARER_MGR_STATE_GOING_DOWN            = 7,
  DS_BEARER_MGR_STATE_MAX
} ds_bearer_mgr_state_e;

/*-------------------------------------------------------------------- 
   Structure to hold WM queue ptr and the WM
--------------------------------------------------------------------*/
typedef struct
{
  q_type             wm_queue;       /*!< Transmit Queue          */
  dsm_watermark_type wm_item;        /*!< Transmit watermark      */
} ds_bearer_mgr_wm_s;

/*---------------------------------------------------------------------------
TYPEDEF DS_BEARER_TX_CMD_F_PTR_TYPE

DESCRIPTION
  TX cmd function pointer declaration.

PARAMETERS
  bearer_mgr_p:  Ptr to bearer on which to operate on.
  pkt_ref_ptr:      Pkt to be sent.
  meta_info_ptr:  Metainfo associated with the packet

RETURN VALUE
  None
---------------------------------------------------------------------------*/

typedef void (*ds_bearer_tx_cmd_f_ptr_type)
(
  ds_bearer_mgr_s       *bearer_mgr_p,
  dsm_item_type        **item_ptr,
  ps_meta_info_type     *meta_info_ptr
);

/*-------------------------------------------------------------------- 
  Bearer manager structure
--------------------------------------------------------------------*/
struct ds_bearer_mgr
{
  uint32                         cookie;     /* Cookie to ensure dyn mem is valid */
  ds_bearer_mgr_state_e          state;      /* Bearer state */
  uint8                          index;      /* Index in Bearer manager table */
  boolean                        is_default; /* Default or dedicated bearer */
  boolean                        call_was_connected;

  uint8                          bearer_id;  /* Bearer id - NSAPI/EPS BID */  
  uint16                         sdf_id;     /* SDF ID to keep track of the PDN 
                                                                                   connectivity request */
  ds_pdn_mgr_s                  *pdn_mgr_ptr;     /* PDN reference */

  ds_bearer_mgr_wm_s             tx_wm;           /* PS to L2 watermark, UL data */                                                                                    
  ds_bearer_mgr_wm_s             rx_wm;           /* L2 to PS watermark, DL data */ 
  ds_bearer_mgr_wm_s             a2_to_ps_wm;     /* A2 to PS watermark, DL data */ 
  
  ds_bearer_tx_cmd_f_ptr_type    tx_ptr;          /* Func Ptr for UL data */
#ifdef FEATURE_MODE_TLB
  ps_sig_enum_type               rx_sig;          /* RX Signal for Bearer context */  
#endif /* FEATURE_MODE_TLB */  
  boolean                        drb_is_up;       /* used to check wm count by nb1*/
  boolean                        tx_hi;  /* whether hi wm cb was called last on tx wm*/

  ds_eps_bearer_manager_s       *eps_bearer;    /* LTE specific bearer parameters */
  ds_gprs_bearer_manager_s      *gprs_bearer;   /* GPRS specific bearer parameters */



#ifdef FEATURE_DATA_IMS
  /* Flag to indicate if this bearer serves the purpose being the signaling link for IMS */ 
  boolean                        im_cn_subsystem_flag; 
#endif

  /* Flag to indicate that a command to reorig has been enqueued.
        This prevents multiple commands from being posted to DS when a bunch of
        packets are enqueued into the WM */
  boolean                        reorig_cmd_enqueued;

  /* Flag to keep note of the bearer on which the reorig was sent out */
  boolean                        srv_rsp_exp;

  /* rab_reestab_retry counter      */
  uint32                         rab_reestab_retry_cnt;

  /* Holds the number of bytes transmitted upto last polling interval   */
  uint64                         tx_bytes;
  /* Holds the number of bytes received upto last polling interval      */
  uint64                         rx_bytes;

  /* PS timer handle for dynamic back pressure disable */
  ps_timer_handle_type           tmr_hndl_dyn_back_pressure;
  
  /* Current TCP DL Ack counter */
  uint64                         tcp_dl_ack_counter;     
  /* Last TCP DL Ack count */
  uint64                         last_tcp_dl_ack_count;  

  /*!< ps_eic. */
//  ps_extended_info_code_enum_type     down_reason_ps_eic;     <-- bearer down reason, earlier was maintained in phys link, now need to manage in bearer

  /* flag will be set if this bearer needs to deactivate on modify IND
        in UMTS NW init QoS case */
  boolean                        teardown_on_modify_ind;

  hc_config_T                    hc_config;    /* Header compression config */
  int                            apn_index;    /* Entry in APN table */
  uint32                         tx_flow_mask;  /* if !=0, bearer is flow_ctl */
#ifdef FEATURE_MODE_TLB
  boolean                        is_in_tlb;  /* Indicates if this bearer is in TLB mode */ 
#endif /* FEATURE_MODE_TLB */
} ;


/*===========================================================================
   This Structure is used to maintain all subscription specific global 
   variables of bearer manager.
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW SYSTEM
   SPECIFIC GLOBAL VARIABLE FOR BEARER
===========================================================================*/
typedef struct
{
  /*-----------------------------------------------------------------------------
    Timer for sending out RAB request again. This timer is effective only from 
    the point when we get a RAB REESTAB reject.
  -----------------------------------------------------------------------------*/
  ps_timer_handle_type   tmr_rab_reestab;
  
  /*-----------------------------------------------------------------------------
    Boolean flag that indicates if the timer is running. Although REX provides
    API which can be used to check if the timer is running, it is more expensive
    in terms of CPU cycles. Hence this boolean flag is maintained. It becomes
    extremely critical to set or reset this flag appropriately as and when the 
    timer is set or reset. Also this boolean should never be set/reset in any
    other task except the DS task.
    Also on timer expiry, this flag has to be explicitly reset in the function
    that handles the timer_expired command
  -----------------------------------------------------------------------------*/
  boolean                timer_running;
  
  /*-----------------------------------------------------------------------------
    This flag is used to keep track of any pending service requests
  -----------------------------------------------------------------------------*/
  boolean                service_request_pending_flag;
  uint8                  uplink_data_bitmask;
  
  boolean                ds_3gpp_reg_in_progress;
  
  boolean                access_barring_rab_retry;  
  uint16                 access_barring_rab_retry_timer;  
  uint16                 access_barring_rab_retry_counter;
    /*-----------------------------------------------------------------------------
    This flag is used to keep track of mem_ctrl_ev posted to DS task. This is to 
    avoid posting multiple commands to DS task when a mem_ctrl_ev is already 
    pending.
   -----------------------------------------------------------------------------*/
  boolean                       mem_ctrl_ev_posted;
  
  rex_crit_sect_type     ul_data_crit_sect; /* Critical section for UL data */

  uint8                         pool_id_mask;
  
} ds_bearer_mgr_info_type;

typedef struct
{
  dsm_mempool_id_enum_type    pool_id;
  dsm_mem_op_enum_type        op;
 
} ds_bearer_mgr_event_type;

/*===========================================================================
FUNCTION DS_BEARER_MGR_INIT

DESCRIPTION
  This function finitialises Bearer Manager module.
 
PARAMETERS
  is_init          : Is it part of power up init

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_init
(
  boolean is_init
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_INSTANCE_PTR

DESCRIPTION
  This function fetches the Bearer Manager Pointer corresponding to the
  given Bearer Instance.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_instance_ptr
(
  uint8 instance
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_CAHCED_INSTANCE_PTR

DESCRIPTION
  This function fetches the Bearer Manager Pointer corresponding to the
  given Bearer Instance.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_cached_instance_ptr
(
  uint8 instance
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_VALIDATE_BEARER_PTR

DESCRIPTION
  This function checks if the bearer ptr has been allocated dynamic memory

PARAMETERS
  bearer_mgr_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_bearer_mgr_validate_bearer_ptr
(
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_bearer_mgr_free_dyn_mem

DESCRIPTION
  This function frees dynamic bearer context

PARAMETERS
  bearer_p       - bearer reference to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None
===========================================================================*/
void ds_bearer_mgr_free_dyn_mem
(
  ds_bearer_mgr_s *bearer_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_ALLOCATE_NEW_BEARER

DESCRIPTION
  This function is used to return a bearer from the pool of available bearer
  contexts.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_mgr_s* - Bearer ptr

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_allocate_new_bearer
(
  void
);


/*===========================================================================
FUNCTION  DS_BEARER_MGR_BRING_UP_BEARER

DESCRIPTION
  This function handles both the reorigination from dormancy as well as new
  call bring up. 

PARAMETERS
  Bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_bring_up_bearer
(
  ds_bearer_mgr_s                 *bearer_mgr_p
);


#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_IMCN_CALL_UP

DESCRIPTION
  This fn is used during call orig to find out if there is already a call
  with the IM_CN_subsystem flag set to TRUE

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the IMCN call is up, False otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_IMCN_call_up
(
  void
);
#endif /* FEATURE_DATA_IMS */


/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FROM_SDF_ID

DESCRIPTION
  This fn is used to find the bearer for a given sdf_id

PARAMETERS
  sdf_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_from_sdf_id
(
  uint16   sdf_id
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FROM_EPS_ID

DESCRIPTION
  This fn is used to find the bearer for a given eps_id

PARAMETERS
  eps_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_from_eps_id
(
  uint8   eps_id
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_CACHED_BEARER_FROM_EPS_ID

DESCRIPTION
  This fn is used to find the cached bearer for a given eps_id

PARAMETERS
  eps_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_cached_bearer_from_eps_id
(
  uint8   eps_id
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FROM_NSAPI

DESCRIPTION
  This fn is used to find the bearer for a given eps_id

PARAMETERS
  eps_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_from_nsapi
(
  uint8   naspi
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_UNSET_UL_DATA_BIT_MASK

DESCRIPTION
  This fn is used to clear the bearer id bit mask for UL data in bearer info.

PARAMETERS
  bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_unset_ul_data_bit_mask
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_UL_DATA_CRIT_SECT

DESCRIPTION
  This fn is used to get the critical section lock for UL data path.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  Reference for rex type.

SIDE EFFECTS
  None.
===========================================================================*/
rex_crit_sect_type* ds_bearer_mgr_get_ul_data_crit_sect
(
  void
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_INFO_CLEANUP

DESCRIPTION
  This fn is used to clear the UE based params in bearer info.

PARAMETERS
  bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_info_cleanup
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_bearer_mgr_reorig_dorm_call_cb

DESCRIPTION
  This function sends a command to DS task to trigger dormant re-orig. Called
  when data is enqueued into the Um Tx watermark.
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/
void ds_bearer_mgr_reorig_dorm_call_cb
(
  dsm_watermark_type *wm_p,
  void               *call_back_data
);

/*===========================================================================
FUNCTION  DS_BEARER_MGR_PROC_REORIG_DORM_CMD

DESCRIPTION
  This function is called when we process the command to reestablish RAB
  due to enqueuing data into the watermarks.

PARAMETERS   
 *bearer_mgr_p - Pointer to the bearer context

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_proc_reorig_dorm_cmd
(
  ds_bearer_mgr_s   *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_bearer_mgr_reorig_dorm_timer_cb

DESCRIPTION
  This function is registered as the timer call back. When the timer to do
  Rab_reestab retry expires, this function is called. The processing is 
  serialized by posting a command to DS.
  
PARAMETERS  
   None
   
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_reorig_dorm_timer_cb
(
  void  *instance
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_PROC_REORIG_DORM_TMR_EXP_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of dormancy timer expiry. This simply calls the function to reestab RAB.
      
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.  
===========================================================================*/
void ds_bearer_mgr_proc_reorig_dorm_tmr_exp_cmd
(
  void
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_RAB_RE_ESTAB_IND_HDLR

DESCRIPTION
  This function handles rab reject indication for 3GPP modes. This will in
  turn dispatch any mode specific functions
  
PARAMETERS  
  esm_drb_setup_ind
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_rab_re_estab_ind_hdlr
(
  emm_drb_setup_ind_s_type  *info_ptr
);

/*===========================================================================
FUNCTION ds_bearer_mgr_rab_release_ind_hdlr

DESCRIPTION
  This function handles rab release procedure for 3GPP modes.
  
PARAMETERS  
  call_id    - CM call id
  end_param  - call end parameters
  subs_id    - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_mgr_rab_release_ind_hdlr
(
  uint8  bearer_id
);

/*===========================================================================
FUNCTION ds_bearer_mgr_rab_re_estab_rej_hdlr

DESCRIPTION
  This function handles rab reject indication from NAS.
  
PARAMETERS  
  emm_drb_reestablish_reject_ind_s_type - RAB reject info ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_rab_re_estab_rej_hdlr
(
  emm_drb_reestablish_reject_ind_s_type  *rab_rej_info
);

/*===========================================================================
FUNCTION ds_bearer_mgr_suspend_ind

DESCRIPTION
  This function is to perform any mode specific operations on data suspend
  indication.
  
PARAMETERS  
  send_ps_data_avail - we are not allowed to send data_available_ind
  if we are OOS or in limited service
  subs_id            - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_suspend_ind
(
  void
);

/*===========================================================================
FUNCTION ds_bearer_mgr_resume_ind

DESCRIPTION
  This function is to perform any mode specific operations on data resume
  indication.
  
PARAMETERS  
  None
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_resume_ind
(
  void
);

/*===========================================================================
FUNCTION  DS_BEARER_MGR_TEAR_DOWN_BEARER

DESCRIPTION
  This function handles bearer tear down as part of instance down handling. 

PARAMETERS
  Bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_tear_down_bearer
(
  ds_pdn_mgr_s       *pdn_mgr_p,
  ds_bearer_mgr_s    *bearer_mgr_p  
);

/*===========================================================================

FUNCTION DS_BEARER_MGR_LOCAL_CLEANUP

DESCRIPTION
  This function peforms local call cleanup

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_bearer_mgr_local_cleanup 
(
  ds_bearer_mgr_s   *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_ENABLE_TX_FLOW
 
DESCRIPTION
  This function processes the TX flow enable event posted to bearer manager.

PARAMETERS 
  Bearer pointer
  mask - Flow mask
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_bearer_mgr_enable_tx_flow
(
  ds_bearer_mgr_s     *bearer_p,
  uint32               mask
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_DISABLE_TX_FLOW
 
DESCRIPTION
  This function processes the TX flow disable event posted to bearer manager.

PARAMETERS 
  Bearer ptr
  mask - Flow mask
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_bearer_mgr_disable_tx_flow
(
  ds_bearer_mgr_s   *bearer_p,
  uint32             mask
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_TX_FLOW_ENABLED
 
DESCRIPTION
  This function returns the TX flow status of the bearer.

PARAMETERS 
  Bearer pointer
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_bearer_mgr_is_tx_flow_enabled
(
  ds_bearer_mgr_s     *bearer_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_TX_FLOW_ENABLED_EX
 
DESCRIPTION
  This function returns the TX flow status of the bearer.
  with aditional check for mo_ex_data.
  will return TRUE when MO_EX data is pending and flow is disabled with
  only DS_BEARER_CP_DATA_BACKOFF_MASK.

PARAMETERS 
  Bearer pointer
  is_cp_data_enabled
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_bearer_mgr_is_tx_flow_enabled_ex
(
  ds_bearer_mgr_s     *bearer_p,
  boolean              is_mo_ex_data
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_WM_FLOW_DISABLE_CB

DESCRIPTION
  This function sends a flow disable message to ds task
  
PARAMETERS  
  Watermark pointer,
  Bearer instance
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_wm_flow_disable_cb
(
 dsm_watermark_type *wm,
 void*               callback_data
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_WM_FLOW_ENABLE_CB

DESCRIPTION
  This function sends a flow enable message to ds task
  
PARAMETERS  
  Watermark ptr
  bearer instance
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_wm_flow_enable_cb
(
 dsm_watermark_type *wm,
 void*               callback_data
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_TMR_BACK_PRESSURE_EXP_HDLR

DESCRIPTION
  This is the timer exp hdlr for dynamic back pressure.

PARAMETERS
  user_data_ptr - user data.

DEPENDENCIES
  None.

RETURN VALUE
  None 

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_tmr_dyn_back_pressure_exp_hdlr
(
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_DISABLE_BACK_PRESSURE

DESCRIPTION
  This function disables back pressure.
  
PARAMETERS  
  *bearer_mgr_p  - bearer_mgr pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_disable_back_pressure
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION  DS_BEARER_MGR_RESET_SRV_REQ_THROT

DESCRIPTION
  This function resets service request throttling SM. This is called during
  RAT change. 

PARAMETERS
  sys_sys_mode_e_type     - new mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_reset_srv_req_throt
(
  sys_sys_mode_e_type    target_rat
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_START_BP_TIMER

DESCRIPTION
  This function starts the BP removal timer
  
PARAMETERS  
  *bearer_mgr_p  - bearer_mgr pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_start_bp_timer
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_SEND_PS_DATA_AVAIL

DESCRIPTION
  This function sends PS data available indication to NAS
  
PARAMETERS  
  None
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_send_ps_data_avail
(
  void
);

/*===========================================================================
FUNCTION ds_bearer_mgr_resume_ind

DESCRIPTION
  This function is to perform any mode specific operations on data resume
  indication.
  
PARAMETERS  
  None
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_resume_ind
(
  void
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_ACTIVE_BEARER_CNTXT

DESCRIPTION
  This function returns TRUE if there is an active bearer context in the
  system. Else, it returns FALSE.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_active_bearer_cntxt
(
  void
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_DEF_BEARER_COMING_UP

DESCRIPTION
  This function checks if the default bearer corresponding to the
  given PDN is in COMING UP state.

PARAMETERS
  pdn_cntx_p - PDN Context Ptr

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer in COMING state
  FALSE, otherwise

SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_bearer_mgr_is_def_bearer_coming_up
(
  ds_pdn_mgr_s  *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GENERATE_16BIT_RAND_NUM_BY_RANGE

DESCRIPTION
  This fn is used to generate 16bit random number within a specified range.

PARAMETERS
  High and low limits of the range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
word ds_bearer_mgr_generate_16bit_rand_num_by_range
(
  word  lo_val,
  word  hi_val
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_POST_BEARER_UP_EVENT

DESCRIPTION
  This function post bearer up event to FWK which will be given to clients.

PARAMETERS
  Bearer pointer

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_post_bearer_up_event
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_DORMANT_REORIG_REQUIRED

DESCRIPTION
  This function checks if the bearer is dormant and re-origination is required.

PARAMETERS
  Bearer pointer

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer is dormant and needs re-origination.
  FALSE, otherwise

SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_bearer_mgr_is_dormant_reorig_required
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_BEARER_WRITABLE

DESCRIPTION
  This function checks if the bearer is writable for UL data.

PARAMETERS
  Bearer pointer

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer is writable.
  FALSE, otherwise

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_bearer_writable
(
  ds_bearer_mgr_s  *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_CANCEL_RAB_REEST_TIMER

DESCRIPTION
  This function gets the bearer info ptr

PARAMETERS
   NONE

DEPENDENCIES
  None.

RETURN VALUE
  bearer info ptr

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_cancel_rab_reest_timer();

/*===========================================================================
FUNCTION DS_BEARER_MGR_START_RAB_REEST_TIMER

DESCRIPTION
  This function gets the bearer info ptr

PARAMETERS
  timer_val   timer value

DEPENDENCIES
  None.

RETURN VALUE
  bearer info ptr

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_start_rab_reest_timer(uint32 timer_val);

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_RAB_REEST_TIMER

DESCRIPTION
  This function gets the bearer info ptr

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  bearer info ptr

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_bearer_mgr_get_rab_reest_timer();

/*===========================================================================

FUNCTION DS_3GPP_BEARER_MGR_MEM_CTRL_EV_CB

DESCRIPTION
  This function posts cmd to DS task to notify DSM Memory control event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_mgr_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type pool_id,            /* memory pool ID */
  dsm_mem_level_enum_type  event,              /* type of memory event */
  dsm_mem_op_enum_type     op
);

/*===========================================================================
FUNCTION ds_bearer_mgr_set_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function sets the Mem Ctrl Event posted Flag
 
PARAMETERS
  flag: Mem Cntrl Event posted Flag
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void ds_bearer_mgr_set_mem_ctrl_ev_posted_flag(boolean flag);

/*===========================================================================

FUNCTION DS_BEARER_MGR_PROCESS_MEM_FC_EVENT

DESCRIPTION
  This function flushes PS-L2 watermarks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_mgr_process_mem_fc_event
(
  ds_bearer_mgr_event_type         *ds_bearer_mgr_event_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_DATA_PENDING

DESCRIPTION
  This function returns TRUE if there is pending data in any of the bearers. 
  Else, it returns FALSE.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_data_pending
(
  void
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#if defined FEATURE_APPSRV_TEST_TOOL || defined TEST_FRAMEWORK
/*===========================================================================
FUNCTION DS_BEARER_MGR_DIAG_TOOL_FLOW_CONTROL

DESCRIPTION
  This function does flow control based on commands from DIAG tool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_mgr_diag_tool_flow_control
(
  boolean is_bearer_flow_enabled,
  uint32  flow_val
);

#endif

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FLOW_MASK
 
DESCRIPTION
  This function returns the flow mask for the bearer.

PARAMETERS 
  Bearer ptr
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  MASK
 
SIDE EFFECTS 
  None
===========================================================================*/

uint32 ds_bearer_mgr_get_bearer_flow_mask
(
  ds_bearer_mgr_s   *bearer_p
);

/*===========================================================================
FUNCTION  DS_BEARER_MGR_RESET_CACHED_BEARER

DESCRIPTION
  This function resets cached bearer pending while call ended during tech suspension. 

PARAMETERS
  ds_bearer_mgr_s     - bearer_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_reset_cached_bearer
(
  ds_bearer_mgr_s   *bearer_mgr_p
);
#ifdef FEATURE_NBIOT_NTN
/*===========================================================================
FUNCTION      DS_PDN_RETRY_DATA_AFTER_GNSS_RECOVERY

DESCRIPTION   This function will retyr the data after gnss recovery on NTN RAT

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_bearer_mgr_retry_data_after_gnss_recovery();
#endif /* FEATURE_NBIOT_NTN */

#endif /* DS_BEARER_MANAGER_H */
