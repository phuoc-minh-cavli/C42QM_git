#ifndef DSSYSMGR_H
#define DSSYSMGR_H
/*===========================================================================

                         D S S Y S M G R . H
                         H E A D E R    F I L E

DESCRIPTION

  This file contains functions for interfacing to Call Manager, including
  Data Services client initialization and processing of SS/PH events.This
  header file is intended to be used internal to the ds3g module only.


Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
  when          who     what, where, why
---------------------------------------------------------------------------
  2018-05-06    aks     First version of dssysmgr
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_pdn_defs.h"
#include "cm.h"
#include "dstaski.h"


/*-----------------------------------------------------------------------
  Enum for radio types supported
-------------------------------------------------------------------------*/
typedef enum
{
  DSSYSMGR_RADIO_MIN = 0,
  DSSYSMGR_RADIO_OOS = DSSYSMGR_RADIO_MIN,
  DSSYSMGR_RADIO_LTE,
  DSSYSMGR_RADIO_GSM,
  DSSYSMGR_RADIO_MAX=DSSYSMGR_RADIO_GSM
}dssysmgr_radio_e_type;

/*-----------------------------------------------------------------------
  Module IDs for internal clients registered with dssysmgr for SS events
-------------------------------------------------------------------------*/
typedef enum
{
  DSSYSMGR_CLIENT_MIN = 0,
  DSSYSMGR_CLIENT_3GPP = DSSYSMGR_CLIENT_MIN,
  DSSYSMGR_CLIENT_ATCOP,
  DSSYSMGR_CLIENT_QMI,
  DSSYSMGR_CLIENT_MAX
}dssysmgr_client_e_type;

typedef enum
{
  DSSYSMGR_EVT_TYPE_MIN = 0,
  DSSYSMGR_EVT_TYPE_SS = DSSYSMGR_EVT_TYPE_MIN,
  DSSYSMGR_EVT_TYPE_PH,
  DSSYSMGR_EVT_TYPE_CALL,
  DSSYSMGR_EVT_TYPE_SUPL,
  DSSYSMGR_EVT_TYPE_MAX
}dssysmgr_cm_event_type;
  
typedef struct
{
  cm_ss_event_e_type    ss_event;
  cm_mm_ss_info_s_type  *ss_info_ptr;
}cm_ss_event_info;

typedef struct
{
  cm_ph_event_e_type ph_event;
  cm_ph_info_s_type  *ph_info_ptr;
}cm_ph_event_info;

typedef struct
{
  cm_call_event_e_type   call_event;
  cm_mm_call_info_s_type *call_info_ptr;
}cm_call_event_info;

typedef struct
{
  cm_sups_event_e_type  sups_event;
  cm_sups_info_s_type   *sups_info_ptr;
}cm_supl_event_info;

typedef union
{
  cm_ss_event_info   ss_ev_info;
  cm_ph_event_info   ph_ev_info;
  cm_call_event_info call_ev_info;
  cm_supl_event_info cm_supl_ev_info;
}dssysmgr_cm_common_event_info;

/*-----------------------------------------------------------------------
  Event notification callback of registered internal clients for SS event
-------------------------------------------------------------------------*/
typedef void (*ds3g_notify_func_type)(dssysmgr_cm_event_type event_type,
                                dssysmgr_cm_common_event_info cm_common_ev_info);

/*-------------------------------------------------------------------------
  Structure to cache SS event info
-------------------------------------------------------------------------*/
struct dssysmgr_ss_event_info_s
{
  cm_ss_event_e_type          ss_event;
  uint64                      changed_fields;
  uint64                      signal_strength_changed_fields;
  boolean                     ps_data_suspend;
  sys_div_duplex_e_type       div_duplex;
  sys_sys_mode_e_type         sys_mode;
  sys_srv_status_e_type       srv_status;
  sys_srv_domain_e_type       srv_domain;
  sys_srv_domain_e_type       effective_srv_domain;
  sys_sys_id_s_type           sys_id;
  cm_cell_srv_ind_s_type      cell_srv_ind;
};

/*-------------------------------------------------------------------------
  Structure to cache PH event info
-------------------------------------------------------------------------*/
struct dssysmgr_ph_event_info_s
{
  cm_ph_event_e_type                  ph_event;
  cm_srv_domain_pref_e_type           srv_domain_pref;
  sys_oprt_mode_e_type                oprt_mode;
  cm_mode_pref_e_type                 mode_pref;
  cm_ph_state_e_type                  ph_state;
  sys_modem_dual_standby_pref_e_type  standby_pref;
  cm_hybr_pref_e_type                 hybr_pref;
  sys_data_priority_e_type            priority_type;
  sys_sys_mode_mask_e_type            mode_capability;
  sys_band_mask_e_type                band_capability;  
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type            lte_band_capability;
#else
  sys_band_mask_e_type                lte_band_capability;
#endif /* LTE_BAND_NUM */
};

/*-------------------------------------------------------------------------
  Structure to store calculated radio info parameters
-------------------------------------------------------------------------*/
typedef struct
{
  dssysmgr_radio_e_type     radio;
  ds_sys_rat_ex_enum_type   rat_value;
  uint64                    so_mask;
} dssysmgr_sm_info;


/*-------------------------------------------------------------------------
  Structure holding info for the internal clients registered with dssysmgr 
---------------------------------------------------------------------------*/
typedef struct{
  dssysmgr_client_e_type   client_id;
  boolean                  is_regd;
  ds3g_notify_func_type    cb_fn_ptr;
  uint8                    event_type_mask;
  void*                    cb_data;
}dssysmgr_event_buf;

/*===========================================================================
FUNCTION      DSSYSMGR_SET_CM_CLIENT_ID

DESCRIPTION   Sets client ID assigned by CM in DSSYSMGR maintained cache.

PARAMETERS    cm_client_id_type : Client ID set from CM

DEPENDENCIES  None    

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_set_cm_client_id
(
  cm_client_id_type  cm_client_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CM_CLIENT_ID

DESCRIPTION   Gets client ID maintained in dssysmgr cache.

PARAMETERS    cm_client_id_type : Client ID set from CM

DEPENDENCIES  None    

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
cm_client_id_type dssysmgr_get_cm_client_id();

/*===========================================================================
FUNCTION      DSSYSMGR_CM_INIT

DESCRIPTION   Function to register with Call Manager for SS & PH events.
              Masks for both event types that DS internal modules are
              interested in would be set here.
              
PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_cm_init
(
  void
);


/*===========================================================================
FUNCTION      DSSYSMGR_CM_PWROFF_LPM_MODE_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              PWROFF / LPM mode. It posts cmd to DS.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  Boolean value

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_cm_pwroff_lpm_mode_cb
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_CM_SUB_NOT_AVAIL_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              subs not available event. It posts cmd to DS.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_cm_sub_not_avail_cb
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_CM_SS_CB

DESCRIPTION   Callback function invoked by Call Manager to notify DSSYSMGR
              whenever a serving system related event occurs that DSSYSMGR
              is registered for. It then dispatches internal module specific 
              callbacks based on the client mask.

PARAMETERS    cm_ss_event_e_type: SS event type
              cm_mm_ss_info_s_type : SS event info params

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_cm_ss_cb
(
  cm_ss_event_e_type ss_event,
  const cm_mm_ss_info_s_type  *ss_info_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_CM_CALL_CB

DESCRIPTION   Callback function invoked by Call Manager to notify DSSYSMGR
              whenever a serving system related event occurs that DSSYSMGR
              is registered for. It then dispatches internal module specific 
              callbacks based on the client mask.

PARAMETERS    cm_ss_event_e_type: SS event type
              cm_mm_ss_info_s_type : SS event info params

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_cm_call_cb
(
  cm_call_event_e_type          call_event,
  const cm_mm_call_info_s_type  *call_info_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_NOTIFY_SS_EV_INFO

DESCRIPTION   Notify the MH module regarding the change in SS info.
              Push the task to DS queue and yield to CM task

PARAMETERS    cm_ss_event_e_type: SS event type
              cm_mm_ss_info_s_type : SS event info params

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_notify_ss_ev_info
(
  cm_ss_event_e_type ss_event,
  const cm_mm_ss_info_s_type *ss_info_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION   DS task processing on receiving SS event change event.
              cmd_id: DS_CMD_CM_SS_EV_SRV_CHG

PARAMETERS    ds_command_type : Command payload to DS task              
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_process_cm_ss_srv_chg_cmd
(
  ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_PROCESS_LTE_STACK_INFO

DESCRIPTION   Process LTE stack info rat_value and so_mask

PARAMETERS    stack_info_ptr

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_process_lte_stack_info
(
  dssysmgr_ss_event_info_type* stack_info_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_PROCESS_GSM_STACK_INFO

DESCRIPTION   Process stack info for GSM to calculate the rat_value and so_mask.

PARAMETERS    stack_info_ptr

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_process_gsm_stack_info
(
  dssysmgr_ss_event_info_type* stack_info_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_NOTIFY_RADIO_EVENT

DESCRIPTION   Notifies system event info to DSSYS about the current radio 
              parameters.

PARAMETERS    Radio type

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_notify_radio_event
(
  dssysmgr_radio_e_type radio
);

/*===========================================================================
FUNCTION      DSSYSMGR_CM_PH_CB

DESCRIPTION   Callback function invoked by Call Manager to notify DSSYSMGR
              whenever Phone event occurs that DSSYSMGR registered for.
              This function sends a command to the DS task, depending on the
              type of serving system event that occurred.

PARAMETERS    cm_ph_event_e_type: Phone event received from CM
              cm_ph_info_s_type : Event information
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_cm_ph_cb
(
  cm_ph_event_e_type         ph_event,
  const cm_ph_info_s_type   *event_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_PROCESS_CM_PH_EVENT_CMD

DESCRIPTION   Callback function invoked by Call Manager to notify DSSYSMGR
              whenever Phone event occurs that DSSYSMGR
              registered for.This function sends a command to the DS task, 
              depending on the type of serving system event that occurred.

DEPENDENCIES  cmd_ptr: Command payload for DS task processing

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_process_cm_ph_event_cmd
(
  ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_RESET_RADIO_SM

DESCRIPTION   This utility function resets the Radio SM instance.

PARAMETERS    None

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_reset_radio_sm
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_CHECK_PROCESSING_REQUIRED

DESCRIPTION   Checks if there are any changes in cm ss event parameters from 
              what is stored in cache to decide if processing is needed.

PARAMETERS    dssysmgr_ss_event_info_type : SS event info params

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/
boolean dsysmgr_check_ss_event_processing_required
(
  dssysmgr_ss_event_info_type *ss_event_info_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_IS_SRV_DOMAIN_PS_SUPPORTED

DESCRIPTION   This utility function checks if srv_domain is PS supported.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If the srv domain is PS supported
              FALSE: If the srv domain is not PS supported

SIDE EFFECTS  None.
===========================================================================*/
boolean dssysmgr_is_srv_domain_ps_supported
(
  sys_srv_domain_e_type srv_domain
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_RAT_VALUE_FROM_CELL_SRV_IND

DESCRIPTION   This utility function returns RAT mask from the Cell service ind.

PARAMETERS    sys_mode: system mode
              cell_srv_ind_ptr: pointer to cell service indication

DEPENDENCIES  None.

RETURN VALUE  RAT mask.

SIDE EFFECTS  None.
===========================================================================*/
ds_sys_rat_ex_enum_type dssysmgr_get_rat_value_from_cell_srv_ind
(
  sys_sys_mode_e_type        sys_mode,
  cm_cell_srv_ind_s_type     *cell_srv_ind_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_SO_MASK_FROM_DIV_DUPLEX

DESCRIPTION   Utility function to get so mask from div duplex.

PARAMETERS    sys_div_duplex_e_type

DEPENDENCIES  None

RETURN VALUE  The so mask for LTE

SIDE EFFECTS  None
===========================================================================*/
uint32 dssysmgr_get_so_mask_from_div_duplex
(
  sys_div_duplex_e_type  div_duplex
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_SO_MASK_FOR_EGPRS_SUPPORT

DESCRIPTION   This function returns the SO MASK corresponding to the passed 
               EGPRS support present in cell_srv_ind.

PARAMETERS    cell_srv_ind - Cell service indication info recvd. from CM

DEPENDENCIES  None.

RETURN VALUE  SO mask 

SIDE EFFECTS  None.
===========================================================================*/
uint64 dssysmgr_get_so_mask_for_egprs_support
(
  cm_cell_srv_ind_s_type cell_srv_ind
);

/*===========================================================================
FUNCTION      DSSYSMGR_CREATE_SS_EVENT_INFO_INSTANCE

DESCRIPTION   This function allocates SS event info instance for caching
              the parameters.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_create_ss_event_info_instance
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_FREE_SS_EVENT_INFO_INSTANCE

DESCRIPTION   This function cleans up the cached SS event info instance.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_free_ss_event_info_instance
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_ALLOC_RADIO_SM_INST

DESCRIPTION   This function allocates radio sm instance to store the radio
              specific parameters like current_radio type,rat_value & SO mask.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_alloc_radio_sm_inst
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_SS_TO_SYSMGR_RADIOTYPE

DESCRIPTION   This is a utility to translate ss radio type to dssysmgr radio.

PARAMETERS    sys_sys_mode_e_type : Radio type maintained at CM

DEPENDENCIES  None.

RETURN VALUE  DSSYSMGR radio.

SIDE EFFECTS  None.
===========================================================================*/
dssysmgr_radio_e_type  dssysmgr_ss_to_sysmgr_radiotype
(
  sys_sys_mode_e_type  ss_sys
);

/*===========================================================================
FUNCTION      DSSYSMGR_CLIENT_REG_EVENT

DESCRIPTION   This function registers clients for ss events with 
              DSSYMGR module. 

PARAMETERS    dssysmgr_client: Registered client id
              ds3g_client_cb_f_ptr: Notification callback for client
              event_mask: Mask for events client registered for.
              data_ptr: Client specific user data
  
DEPENDENCIES  None.

RETURN VALUE  TRUE: Success, FALSE: Failure

SIDE EFFECTS  None.
===========================================================================*/
boolean dssysmgr_client_reg_event
(
  dssysmgr_client_e_type  dssysmgr_client,
  uint8                   event_type_mask,  
  ds3g_notify_func_type   ds3g_client_cb_f_ptr,
  void                    *data_ptr
);

/*===========================================================================
FUNCTION      DSSYSMGR_CLIENT_DEREG_EVENT

DESCRIPTION   Function to deregister clients from ss event.

PARAMETERS    dssysmgr_client_e_type : client id

DEPENDENCIES  None.

RETURN VALUE  TRUE: Success, FALSE: Failure

SIDE EFFECTS  None.
===========================================================================*/
boolean dssysmgr_client_dereg_event 
(
  dssysmgr_client_e_type  dssysmgr_client
);

/*===========================================================================
FUNCTION      DSSYSMGR_COMMON_NOTIFY_CLIENT

DESCRIPTION   This function dispatches client notification for registered 
              clients.

PARAMETERS    CM event Type.
              CM event common info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_common_notify_client
(
  dssysmgr_cm_event_type event_type,
  dssysmgr_cm_common_event_info common_info
);

/*===========================================================================
FUNCTION      DSSYSMGR_ATTACH_COMPLETE_IND

DESCRIPTION   This function notifies Attach complete to 3gcmif module.

PARAMETERS    None

DEPENDENCIES  None.

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_attach_complete_ind
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_SRV_STATUS

DESCRIPTION   This function returns the current PS data service status.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  sys_srv_status_e_type - Service status

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_status_e_type dssysmgr_get_srv_status
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_SRV_DOMAIN

DESCRIPTION   This function returns the current service domain.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  sys_srv_domain_e_type - Service domain

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_domain_e_type dssysmgr_get_srv_domain
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_OPRT_MODE_SRV_DOMAIN

DESCRIPTION   This function returns the operating mode service domain.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  cm_srv_domain_pref_e_type - Operating mode service domain

SIDE EFFECTS  None.
===========================================================================*/
cm_srv_domain_pref_e_type dssysmgr_get_oprt_mode_srv_domain
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_OPRT_MODE

DESCRIPTION   This function returns the current oprt mode.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  sys_oprt_mode_e_type - Operating mode

SIDE EFFECTS  None.
===========================================================================*/
sys_oprt_mode_e_type dssysmgr_get_oprt_mode
(
  void
);
/*===========================================================================
FUNCTION      DSSYSMGR_CM_PSM_ENTER_REQ_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              PSM READY REQUEST. It posts cmd to DS.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_cm_psm_ready_req_cb( void );

/*===========================================================================
FUNCTION      DSSYSMGR_CM_PSM_ENTER_REQ_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              PSM ENTER REQUEST. It posts cmd to DS.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_psm_enter_req_cb( void );

/*===========================================================================
FUNCTION      DSSYSMGR_SET_MODE_PREF

DESCRIPTION   This function sets the preferred mode.

PARAMETERS    cm_mode_pref_e_type.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_set_mode_pref
(
  cm_mode_pref_e_type pref_mode
);

/*===========================================================================
FUNCTION      DSSYSMGR_GET_MODE_PREF

DESCRIPTION   This function returns the preferred mode.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  cm_mode_pref_e_type

SIDE EFFECTS  None.
===========================================================================*/
cm_mode_pref_e_type dssysmgr_get_mode_pref
(
  void
);

/*===========================================================================
FUNCTION      DSSYSMGR_MODE_PREF_TO_SYS_MODE

DESCRIPTION   This function returns the preferred sys mode from cm mode pref

DEPENDENCIES  None.

RETURN VALUE  sys mode.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type dssysmgr_mode_pref_to_sys_mode
(
  cm_mode_pref_e_type mode_pref
);

/*===========================================================================
FUNCTION      ds3g_cm_ph_sys_sel_cb

DESCRIPTION   Callback function to be called by CM when system selection 
              enable/disable is performed by DS.
              Post a cmd only if an error value is received.

DEPENDENCIES  None

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cm_ph_sys_sel_cb
(
  void                   *data_block_ptr,
  cm_ph_cmd_e_type        ph_cmd,
  cm_ph_cmd_err_e_type    ph_cmd_err
);

/*===========================================================================
FUNCTION      ds3g_get_sys_sel_cb_fn_ptr

DESCRIPTION   Returns the sys_sel_cb_fn_ptr

DEPENDENCIES  None

RETURN VALUE  void *

SIDE EFFECTS  None
===========================================================================*/
void *ds3g_get_sys_sel_cb_fn_ptr
(
  void
);

/*===========================================================================
FUNCTION   DSSYSMGR_CM_SRV_IND_HDLR

DESCRIPTION
  This is the handler function called as part of CM service indicaton from
  CM. This function determines whether a detach was triggered and uses
  this information to clear network override throttling.
 
PARAMETERS
  msgrtype  - UMID of the message sent
  subs_id   - Subscription ID
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if the message header is correct and we can handle the message
  FALSE - otherwise

  Note: We will not return FALSE for errors in the message payload

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssysmgr_cm_srv_ind_hdlr
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*===========================================================================
FUNCTION   DSSYSMGR_CM_DEEP_SLEEP_IND_HDLR

DESCRIPTION
  This is the handler function called as part of CM deep sleep indicaton from
  CM. 
 
PARAMETERS
  msgrtype  - UMID of the message sent
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if the message header is correct and we can handle the message
  FALSE - otherwise

  Note: We will not return FALSE for errors in the message payload

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssysmgr_cm_deep_sleep_ind_hdlr
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*===========================================================================
FUNCTION   DSSYSMGR_GET_CM_DEEP_SLEEP_STATUS

DESCRIPTION
  This is function to return the cm deep sleep status
 
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if the UE is in deep sleep
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssysmgr_get_cm_deep_sleep_status();


#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
/*===========================================================================
FUNCTION  DSSYSMGR_CM_GPS_SCAN_IND_HDLR

DESCRIPTION
  This function handles messages coming from cm for gps scan inidcation
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssysmgr_cm_gps_scan_ind_hdlr 
(
  msgr_umid_type        msgrtype,
  const msgr_hdr_struct_type *dsmsg
);

/*===========================================================================
FUNCTION  DSSYSMGR_GET_GPS_SEARCH_STATUS

DESCRIPTION
  This function returns  gps scan inidcation
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/

boolean dssysmgr_get_gps_search_status();

#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */

#ifdef FEATURE_NBIOT_NTN
/*===========================================================================
FUNCTION  DSSYSMGR_GET_NB1_NTN_RAT_STATUS

DESCRIPTION
  This function returns NTN rat status
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssysmgr_get_nb1_ntn_rat_status();
#endif /* FEATURE_NBIOT_NTN */
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif /*DSSYSMGR_H*/
