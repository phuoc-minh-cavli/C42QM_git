/******************************************************************************
  @file    dssysmgr.c
  @brief   This file has implementation for DS interfacing with Call Manager

  DESCRIPTION
  This file contains functions for interfacing to Call Manager, including
  Data Services client initialization and processing of SS & PH events

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  dssysmgr_cm_init() must be called once at startup, to register as a Call
  Manager client.

  ---------------------------------------------------------------------------
  Copyright (C) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/dsmgr/src/dssysmgr.c#10 $
  $DateTime: 2023/06/05 04:10:23 $$Author: pwbldsvc $

  when             who what, where,  why
  ---------- --- ------------------------------------------------------------
  2018-05-06       aks               Created module.

===========================================================================*/

/*--------------------------------------------------------------------------- 
  Include Files
---------------------------------------------------------------------------*/
#include "ds_headers.h"
#include "cm.h"
#include "cm_v.h"
#include "dssysmgr.h"
#include "ds_sys_event.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_apm.h"
#include "ds_pdn_manager.h"
#include "ds_sys_eventi.h"
#include "ds_qmi_if.h"
#include "dsati.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ps_tcp.h"
#include "ds_pdn_cp_data_hdlr.h"

#define DSSYSMGR_MAX_SYS_ALLOWED 1

/*---------------------------------------------------------------------------
  Client id assigned by Call Manager.
---------------------------------------------------------------------------*/
static cm_client_id_type  dssysmgr_cm_client_id;

/*--------------------------------------------------------------------------- 
  Cache Oprt mode of the phone as advertized by CM
---------------------------------------------------------------------------*/
static sys_oprt_mode_e_type ds_3gpp_cm_ph_info_oprt_mode;

/*---------------------------------------------------------------------------
  Variable to store serv_domain_pref e.g CM_SRV_DOMAIN_PREF_CS_PS
---------------------------------------------------------------------------*/
static cm_srv_domain_pref_e_type ds3g_oprt_mode_srv_domain;

/*--------------------------------------------------------------------------
  Variable to store system mode preference e.g CM_MODE_PREF_LTE_ONLY
---------------------------------------------------------------------------*/
static cm_mode_pref_e_type ds3g_sys_mode_pref;

/*--------------------------------------------------------------------------
  SS event info cached for use by dssysmgr
---------------------------------------------------------------------------*/
dssysmgr_ss_event_info_type* dssysmgr_cached_ss_info_ptr;

/*--------------------------------------------------------------------------
  List of clients registered with dssysmgr for SS events
---------------------------------------------------------------------------*/
dssysmgr_event_buf* dssysmgr_reg_client_list[DSSYSMGR_CLIENT_MAX] = {NULL};

/*--------------------------------------------------------------------------
  Cache for radio instance information
---------------------------------------------------------------------------*/
dssysmgr_sm_info *dssysmgr_radio_sm_instance = NULL;

/*--------------------------------------------------------------------------
  Cache params notified to ds_sys in the last event indication
---------------------------------------------------------------------------*/
ds_sys_status_type dssysmgr_notified_param = {0};

bit_mask_256_t dssysmgr_client_mask[DSSYSMGR_EVT_TYPE_MAX] = {{0}};

/*--------------------------------------------------------------------------
  Function ptr to be invoked when LTE enable or disable call to CM fails
--------------------------------------------------------------------------*/
static cm_ph_cmd_cb_f_type *ds3g_cm_ph_cmd_cb_f_ptr = NULL;

static boolean dssysmgr_deep_sleep_status = FALSE;

#ifdef FEATURE_NBIOT_NTN
static boolean dssysmgr_is_NB1_NTN_RAT = FALSE;
#endif /* FEATURE_NBIOT_NTN */
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
/* To identify whether UE is in gps proiority search */
static boolean ue_is_in_gps_high_priority_search = FALSE;
#endif /*FEATURE_GPS_WWAN_PRIORITY_SEARCH*/
/*===========================================================================
FUNCTION      DS3G_SET_CM_CLIENT_ID

DESCRIPTION   Sets client ID assigned by CM in DS3G maintained cache.

PARAMETERS    cm_client_id_type : Client ID set from CM

DEPENDENCIES  None    

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_set_cm_client_id
(
  cm_client_id_type  cm_client_id
)
{
  dssysmgr_cm_client_id = cm_client_id;
  return;
} /* dssysmgr_set_cm_client_id() */

/*===========================================================================
FUNCTION      DS3G_GET_CM_CLIENT_ID

DESCRIPTION   Gets client ID maintained in dssysmgr cache.

PARAMETERS    cm_client_id_type : Client ID set from CM

DEPENDENCIES  None    

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
cm_client_id_type dssysmgr_get_cm_client_id()
{
  return dssysmgr_cm_client_id;
} /* dssysmgr_get_cm_client_id() */

/*===========================================================================
FUNCTION      DS3G_SET_EVENT_MASK

DESCRIPTION   Gets client ID maintained in dssysmgr cache.

PARAMETERS    cm_client_id_type : Client ID set from CM

DEPENDENCIES  None    

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_common_set_cm_event_mask()
{
  dssysmgr_cm_event_type ev_type = DSSYSMGR_EVT_TYPE_MIN;
  for(ev_type = DSSYSMGR_EVT_TYPE_MIN; ev_type < DSSYSMGR_EVT_TYPE_MAX; ev_type++)
  {
    ds_pdn_mgr_set_cm_ev_mask(ev_type,&dssysmgr_client_mask[ev_type]);
    #ifdef FEATURE_DATA_ATCOP
    dsatcmif_set_cm_event_mask(ev_type, &dssysmgr_client_mask[ev_type]);
    #endif /* FEATURE_DATA_ATCOP */
    //dssysmgr_set_cm_ev_mask_qmi(ev_type,&dssysmgr_client_mask[ev_type]);
  }
  return;
}/*dssysmgr_set_event_mask*/

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
void dssysmgr_cm_init(void)
{
  ds_local_err_val_e_type    local_err = DS_LOCAL_ERR_VAL_NONE;
  cm_client_id_type          dssysmgr_cm_cid;
  cm_client_status_e_type    status;  
  /*---------------------------------------------------------------------*/
  
  if(CM_CLIENT_OK != cm_client_init( CM_CLIENT_TYPE_DS, &dssysmgr_cm_cid ))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*Cache the cm_client_id from CM */
  dssysmgr_set_cm_client_id(dssysmgr_cm_cid);
  
  /*create SS event info cache instance*/
  dssysmgr_create_ss_event_info_instance();
  
  /*Allocate radio sm instance*/
  dssysmgr_alloc_radio_sm_inst();
  
  DS_MSG0(MSG_LEGACY_HIGH, "DSSYSMGR Initialization");

  dssysmgr_common_set_cm_event_mask();
  /*Register for CM SS event*/  
  if(CM_CLIENT_OK != cm_mm_client_ss_reg(dssysmgr_cm_cid,
                                         dssysmgr_cm_ss_cb,
                                         CM_CLIENT_EVENT_REG,
                                         dssysmgr_client_mask[DSSYSMGR_EVT_TYPE_SS],
                                         NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
   
  /*Register for CM PH events*/
  if(CM_CLIENT_OK != cm_client_ph_reg(dssysmgr_cm_cid,
                                      dssysmgr_cm_ph_cb,
                                      CM_CLIENT_EVENT_REG,
                                      dssysmgr_client_mask[DSSYSMGR_EVT_TYPE_PH],
                                      NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /*Register for CM Call events*/
  if(CM_CLIENT_OK != cm_mm_client_call_reg(dssysmgr_cm_cid,
                                           dssysmgr_cm_call_cb,
                                           CM_CLIENT_EVENT_REG,
                                           dssysmgr_client_mask[DSSYSMGR_EVT_TYPE_CALL],
                                           NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
  
  /*-----------------------------------------------------------------------
    Activate the registered callback functions.
  -----------------------------------------------------------------------*/

  if(TRUE != cm_data_oprt_mode_reg(dssysmgr_cm_pwroff_lpm_mode_cb))
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }

  if(TRUE != cm_data_subsc_cmd_reg(dssysmgr_cm_sub_not_avail_cb))
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

  status = cm_client_act(dssysmgr_cm_cid);
  if (status != CM_CLIENT_OK)
  {
    local_err = DS_LOCAL_ERR_VAL_6;
  }

  if(cm_client_psm_ready_reg(dssysmgr_cm_psm_ready_req_cb,
                             CM_PSM_MODULE_DS ) != TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_7;
    goto func_exit;

  }
  if(cm_client_psm_enter_reg(dssysmgr_psm_enter_req_cb,
                             CM_PSM_MODULE_DS ) != TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_8;
    goto func_exit;
  }

  /*-----------------------------------------------------------------------
   Initialize the function ptr, This will be passed to CM when
   enabling/disabling LTE.
  -----------------------------------------------------------------------*/
  ds3g_cm_ph_cmd_cb_f_ptr = ds3g_cm_ph_sys_sel_cb;

  /*--------------------------------------------------------------------------
   Register with DS Message Router to receive CM message for service
   domain change indication
  --------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(MM_CM_3GPP_SRV_IND,
                  (dsmsgrcv_msg_hdlr_f)dssysmgr_cm_srv_ind_hdlr);

  dsmsgrrcv_reg_hdlr(MM_CM_PSM_DEEP_SLEEP_ENTER_IND,
                  (dsmsgrcv_msg_hdlr_f)dssysmgr_cm_deep_sleep_ind_hdlr);
  dsmsgrrcv_reg_hdlr(MM_CM_PSM_DEEP_SLEEP_EXIT_IND,
                  (dsmsgrcv_msg_hdlr_f)dssysmgr_cm_deep_sleep_ind_hdlr);
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  /*--------------------------------------------------------------------------
   Register with DS Message Router to receive CM message for GPS scan indication
  --------------------------------------------------------------------------*/

  dsmsgrrcv_reg_hdlr(MM_CM_WWAN_GPS_SCAN_STATUS_IND,
                 (dsmsgrcv_msg_hdlr_f)dssysmgr_cm_gps_scan_ind_hdlr); 
#endif /*FEATURE_GPS_WWAN_PRIORITY_SEARCH*/
func_exit:
  if(DS_LOCAL_ERR_VAL_NONE != local_err)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "dssysmgr_cm_init : err %d",local_err);
  }
  return;
}/* dssysmgr_cm_init() */

/*===========================================================================
FUNCTION      DSSYSMGR_CREATE_SS_EVENT_INFO_INSTANCE

DESCRIPTION   This function allocates SS event info instance for caching
              the parameters.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_create_ss_event_info_instance()
{

  dssysmgr_ss_event_info_type  *ss_event_info_instance = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_SYSTEM_HEAP_MEM_ALLOC(ss_event_info_instance,sizeof(dssysmgr_ss_event_info_type), 
                           dssysmgr_ss_event_info_type *);

  if(ss_event_info_instance != NULL)
  {
    memset(ss_event_info_instance, 0 , sizeof(dssysmgr_ss_event_info_type));
    dssysmgr_cached_ss_info_ptr = ss_event_info_instance;
  }
  else
  {
    DS_ERR_FATAL("Failed to allocate SS event info instance");
  }
  return;
}/* dssysmgr_create_ss_event_info_instance() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/

/*===========================================================================
FUNCTION      DSSYSMGR_ALLOC_RADIO_SM_INST

DESCRIPTION   This function allocates radio sm instance to store the radio
              specific parameters like current_radio type,rat_value & SO mask.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_alloc_radio_sm_inst()
{
  dssysmgr_sm_info *radio_sm = NULL;
  /*-------------------------------------------------------------------------*/
  
  DS_SYSTEM_HEAP_MEM_ALLOC(radio_sm,sizeof(dssysmgr_sm_info),dssysmgr_sm_info *);
  
  if(radio_sm != NULL)
  {
    memset(radio_sm,0,sizeof(dssysmgr_sm_info));
    dssysmgr_radio_sm_instance = radio_sm;
  }
  else
  {
    DS_ERR_FATAL("Failed to allocate radio sm instance");
  }
  return;
} /*dssysmgr_alloc_radio_sm_inst*/

/*===========================================================================
FUNCTION      DSSYSMGR_CM_PWROFF_LPM_MODE_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              PWROFF / LPM mode. It posts cmd to DS.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_cm_pwroff_lpm_mode_cb(void)
{
  ds_command_type  *cmd_ptr = NULL;
  /*-------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    DS_ASSERT(0);
    return FALSE;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"Inside dssysmgr_cm_pwroff_lpm_mode_cb");

  cmd_ptr->hdr.cmd_id = DS_CMD_CM_PWROFF_LPM_IND;
  ds_put_cmd(cmd_ptr);

  return TRUE;
} /* dssysmgr_cm_pwroff_lpm_mode_cb() */

/*===========================================================================
FUNCTION      DSSYSMGR_CM_SUB_NOT_AVAIL_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              subs not available event. It posts cmd to DS.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_cm_sub_not_avail_cb(void)
{
  ds_command_type  *cmd_ptr = NULL;
  /*-------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    DS_ASSERT(0);
    return FALSE;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"Inside dssysmgr_cm_sub_not_avail_cb");

  cmd_ptr->hdr.cmd_id = DS_CMD_CM_SUB_NOT_AVAIL_IND;
  ds_put_cmd(cmd_ptr);

  return TRUE;
} /* dssysmgr_cm_sub_not_avail_cb() */

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

void dssysmgr_cm_call_cb(cm_call_event_e_type               call_event,
                              const cm_mm_call_info_s_type  *call_info_ptr)
{
  dssysmgr_cm_common_event_info common_event_info = {{0}};
  
  if(call_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "SS info params from CM is NULL");
    return;
  }
  
  DS_MSG0(MSG_LEGACY_HIGH, "Processing the incoming CALL event info");
  common_event_info.call_ev_info.call_event =  call_event;
  common_event_info.call_ev_info.call_info_ptr = 
                                 (cm_mm_call_info_s_type*)call_info_ptr;
  dssysmgr_common_notify_client(DSSYSMGR_EVT_TYPE_CALL, common_event_info);
  return;
}/*dssysmgr_cm_call_cb*/

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

void dssysmgr_cm_ss_cb(cm_ss_event_e_type         ss_event,
                              const cm_mm_ss_info_s_type  *ss_info_ptr)
{
  dssysmgr_cm_common_event_info common_event_info = {{0}};
  
  if(ss_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "SS info params from CM is NULL");
    return;
  }
  
  DS_MSG0(MSG_LEGACY_HIGH, "Processing the incoming SS event info");
  dssysmgr_notify_ss_ev_info(ss_event,ss_info_ptr);
  common_event_info.ss_ev_info.ss_event = ss_event;
  common_event_info.ss_ev_info.ss_info_ptr =
                               (cm_mm_ss_info_s_type*)ss_info_ptr;
  dssysmgr_common_notify_client(DSSYSMGR_EVT_TYPE_SS, common_event_info);
  return;
}/*dssysmgr_cm_ss_cb*/

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
void dssysmgr_notify_ss_ev_info(cm_ss_event_e_type ss_event,
                                       const cm_mm_ss_info_s_type *ss_info_ptr)
{
  ds_command_type                 *cmd_ptr = NULL;
  ds_local_err_val_e_type         local_err = DS_LOCAL_ERR_VAL_NONE;
  dssysmgr_ss_event_info_type     *dssys_ss_info_ptr = NULL;
  /*----------------------------------------------------------------------*/
  
  if(dssysmgr_cached_ss_info_ptr == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  cmd_ptr = ds_allocate_cmd_buf(sizeof(dssysmgr_ss_event_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
  DS_MSG0(MSG_LEGACY_HIGH,"Inside dssysmgr_notify_ss_ev_info");
  dssys_ss_info_ptr = (dssysmgr_ss_event_info_type*)cmd_ptr->cmd_payload_ptr;
  memset(dssys_ss_info_ptr, 0, sizeof(dssysmgr_ss_event_info_type));
  dssys_ss_info_ptr->ss_event = ss_event;
  dssys_ss_info_ptr->changed_fields = ss_info_ptr->changed_fields;
  dssys_ss_info_ptr->signal_strength_changed_fields = 
                     ss_info_ptr->signal_strength_changed_fields;
  dssys_ss_info_ptr->ps_data_suspend = ss_info_ptr->ps_data_suspend;
  dssys_ss_info_ptr->div_duplex = ss_info_ptr->div_duplex;
  dssys_ss_info_ptr->sys_mode = ss_info_ptr->sys_mode;
  dssys_ss_info_ptr->srv_status = ss_info_ptr->srv_status;
  dssys_ss_info_ptr->srv_domain = ss_info_ptr->srv_domain;
  
  memscpy(&(dssys_ss_info_ptr->sys_id),
          sizeof(sys_sys_id_s_type),
          &(ss_info_ptr->sys_id),
          sizeof(sys_sys_id_s_type));

  memscpy(&(dssys_ss_info_ptr->cell_srv_ind),
          sizeof(cm_cell_srv_ind_s_type),
          &(ss_info_ptr->cell_srv_ind),
          sizeof(cm_cell_srv_ind_s_type));
  
  cmd_ptr->hdr.cmd_id = DS_CMD_CM_SS_EV_SRV_CHG;
  ds_put_cmd(cmd_ptr);
  
  if(ss_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
  {
    ds_pdn_hdlr_set_cp_ciot_cap(ss_info_ptr->neg_ciot_cap);
#ifdef FEATURE_NBIOT_NTN
   /* send the data upon receiving first full service upon GNSS failure */
    if(ds_pdn_mgr_get_ntn_gnss_status())
    {
      ds_pdn_mgr_set_ntn_gnss_status(FALSE);
      ds_pdn_cp_data_trigger_pending_data();
      ds_bearer_mgr_retry_data_after_gnss_recovery();
    }
#endif
  }
#ifdef FEATURE_NBIOT_NTN
  if( dssysmgr_is_NB1_NTN_RAT == FALSE  && 
      ss_info_ptr->ciot_lte_mode ==  SYS_SYS_MODE_LTE_NB1_NTN )
  {
    dssysmgr_is_NB1_NTN_RAT = TRUE;
    set_ntn_timer();
  }
  if( dssysmgr_is_NB1_NTN_RAT && 
      ss_info_ptr->ciot_lte_mode != SYS_SYS_MODE_LTE_NB1_NTN )
  {
    dssysmgr_is_NB1_NTN_RAT = FALSE;
    reset_ntn_timer();
  }
#endif /* FEATURE_NBIOT_NTN */
func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "dssysmgr_notify_ss_ev_info : err %d",local_err);
  }
  return;
}/* dssysmgr_notify_ss_ev_info */

/*===========================================================================
FUNCTION      DSSYSMGR_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION   DS task processing on receiving SS event change event.
              cmd_id: DS_CMD_CM_SS_EV_SRV_CHG

PARAMETERS    ds_command_type : Command payload to DS task              
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_process_cm_ss_srv_chg_cmd(ds_command_type  *cmd_ptr)
{
  dssysmgr_ss_event_info_type  *ss_event_info_ptr = NULL;
  ds_local_err_val_e_type    local_err = DS_LOCAL_ERR_VAL_NONE;
  dssysmgr_radio_e_type  dssysmgr_radio = DSSYSMGR_RADIO_OOS;
  emm_plmn_change_ind_type     payload;
  /*----------------------------------------------------------------------*/
  
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  ss_event_info_ptr = (dssysmgr_ss_event_info_type*)cmd_ptr->cmd_payload_ptr;
  
  DS_MSG6(MSG_LEGACY_HIGH,
                      "ps_data_suspend : %d  "
                      "sys_mode : %d  "
                      "srv_status : %d  "
                      "srv_domain : %d \n "
                      "effective_srv_domain : %d  "
                      "changed_fields :0x%x  ", 
                      ss_event_info_ptr->ps_data_suspend,
                      ss_event_info_ptr->sys_mode,
                      ss_event_info_ptr->srv_status,ss_event_info_ptr->srv_domain,
                      ss_event_info_ptr->effective_srv_domain,
                      ss_event_info_ptr->changed_fields);
  
  if(NULL == dssysmgr_cached_ss_info_ptr)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  if(dsysmgr_check_ss_event_processing_required(ss_event_info_ptr) == FALSE)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"No change in parameters.Further processing aborted !!");
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  //Add your api here
  qmi_if_process_cm_ss_event(ss_event_info_ptr->ss_event,ss_event_info_ptr);
  switch(ss_event_info_ptr->sys_mode)
  {
    case SYS_SYS_MODE_LTE:
      dssysmgr_process_lte_stack_info(ss_event_info_ptr);
    break;

    case SYS_SYS_MODE_GSM:
      dssysmgr_process_gsm_stack_info(ss_event_info_ptr);
    break;

    default:
      /*Reset radio sm instance*/
      dssysmgr_reset_radio_sm();
    break;
  }
  
  if(ss_event_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
  {
    if(ss_event_info_ptr->sys_mode == SYS_SYS_MODE_GSM)
    {
      memscpy(&(payload.plmn),
  	           sizeof( sys_plmn_id_s_type ),
              &(ss_event_info_ptr->sys_id.id.plmn),
              sizeof(sys_plmn_id_s_type));
      ds_pdn_hdlr_plmn_change_hdlr(&payload);
    }
    
    ds_pdn_hdlr_sys_change_handler(dssysmgr_cached_ss_info_ptr->sys_mode,
                                   ss_event_info_ptr->sys_mode,
                                   dssysmgr_cached_ss_info_ptr->srv_domain,
                                   ss_event_info_ptr->srv_domain);
  }
  if(ss_event_info_ptr->ps_data_suspend != dssysmgr_cached_ss_info_ptr->ps_data_suspend)
  {
    dssysmgr_cached_ss_info_ptr->srv_domain = ss_event_info_ptr->srv_domain;
    ds_pdn_hdlr_ps_data_suspend_resume_hdlr(ss_event_info_ptr->ps_data_suspend);
  }
  
  /* Cache the ss_info_ptr */
  memscpy(dssysmgr_cached_ss_info_ptr, sizeof(dssysmgr_ss_event_info_type),
          ss_event_info_ptr, sizeof(dssysmgr_ss_event_info_type));
  
  /*Notify radio info to external clients*/
  dssysmgr_radio = dssysmgr_ss_to_sysmgr_radiotype(ss_event_info_ptr->sys_mode);
  /* Notify NULL bearer when PS data is suspended in GSM - Aware request */
#ifdef FEATURE_AWARE_CT21  
  if(ss_event_info_ptr->ps_data_suspend && ss_event_info_ptr->sys_mode == SYS_SYS_MODE_GSM)
  {
    dssysmgr_radio = DSSYSMGR_RADIO_OOS;
  }
#endif /* FEATURE_AWARE_CT21 */
  dssysmgr_notify_radio_event(dssysmgr_radio);
  
func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"dssysmgr_process_cm_ss_srv_chg_cmd: err %d",local_err);
  }
  return;
}/* dssysmgr_process_cm_ss_srv_chg_cmd */

/*===========================================================================
FUNCTION      DSSYSMGR_RESET_RADIO_SM

DESCRIPTION   This utility function resets the Radio SM instance.

PARAMETERS    None

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_reset_radio_sm()
{
  dssysmgr_radio_sm_instance->radio = DSSYSMGR_RADIO_OOS;
  dssysmgr_radio_sm_instance->rat_value = DS_SYS_RAT_EX_NULL_BEARER;
  dssysmgr_radio_sm_instance->so_mask = 0;
  return;
}

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
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(srv_domain)
  {
    case SYS_SRV_DOMAIN_PS_ONLY:
    case SYS_SRV_DOMAIN_CS_PS:
      ret_val = TRUE;
      break;

    default:
      break;
  }
  return ret_val;
}/* dssysmgr_is_srv_domain_ps_supported() */

/*===========================================================================
FUNCTION      DSSYSMGR_GET_RAT_VALUE_FROM_CELL_SRV_IND

DESCRIPTION   This utility function returns RAT value from the Cell service ind.

PARAMETERS    sys_mode: system mode
              cell_srv_ind_ptr: pointer to cell service indication

DEPENDENCIES  None.

RETURN VALUE  RAT mask.

SIDE EFFECTS  None.
===========================================================================*/
ds_sys_rat_ex_enum_type dssysmgr_get_rat_value_from_cell_srv_ind
(
  sys_sys_mode_e_type     sys_mode,
  cm_cell_srv_ind_s_type  *cell_srv_ind_ptr
)
{
  ds_sys_rat_ex_enum_type rat_value = DS_SYS_RAT_EX_NULL_BEARER;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cell_srv_ind_ptr == NULL)
  {
    return rat_value;
  }
  
  if(sys_mode == SYS_SYS_MODE_GSM)
  {
    if(cell_srv_ind_ptr->egprs_supp == SYS_EGPRS_SUPPORT_AVAIL)
    {
      rat_value = DS_SYS_RAT_3GPP_EDGE;
    }
    else
    {
      rat_value = DS_SYS_RAT_3GPP_GPRS;
    }
  }
  return rat_value;
}/* dssysmgr_get_rat_value_from_cell_srv_ind() */


/*===========================================================================
FUNCTION      DSSYSMGR_PROCESS_LTE_STACK_INFO

DESCRIPTION   Process LTE stack info rat_value and so_mask.

PARAMETERS    stack_info_ptr
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssysmgr_process_lte_stack_info
(
  dssysmgr_ss_event_info_type* stack_info_ptr
)
{
  
  if((stack_info_ptr == NULL) || (dssysmgr_cached_ss_info_ptr == NULL))
    return;

  ds_pdn_hdlr_set_network_mode(SYS_SYS_MODE_LTE);

  if(dssysmgr_is_srv_domain_ps_supported(stack_info_ptr->srv_domain) == FALSE)
  {
    if(ds_pdn_hdlr_get_attach_initiated_flag() == FALSE)
    {
      dssysmgr_reset_radio_sm();
    }
    else
    {
      DS_MSG0(MSG_LEGACY_HIGH,"LTE attach in progress. Ignore");
    }
    return;
  }
  
  dssysmgr_radio_sm_instance->radio = dssysmgr_ss_to_sysmgr_radiotype(stack_info_ptr->sys_mode);  
  dssysmgr_radio_sm_instance->so_mask = dssysmgr_get_so_mask_from_div_duplex(stack_info_ptr->div_duplex); 
  
  if(stack_info_ptr->srv_status == SYS_SRV_STATUS_SRV &&
     ds_pdn_apm_attach_completed() == TRUE)
  {
    dssysmgr_radio_sm_instance->rat_value = DS_SYS_RAT_EX_3GPP_LTE;
    ds_pdn_apm_unset_ip_rule_required();
  }
  else
  {
    dssysmgr_radio_sm_instance->rat_value = DS_SYS_RAT_EX_NULL_BEARER;
  }

  return;
}/* dssysmgr_process_lte_stack_info */

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
)
{
  uint32  so_mask = DS_SYS_SO_EX_UNSPECIFIED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(div_duplex)
  {
    case SYS_DIV_DUPLEX_NONE:
      break;

    case SYS_DIV_DUPLEX_FREQ:
      so_mask = DS_SYS_SO_EX_3GPP_LTE_FDD;
      break;

    case SYS_DIV_DUPLEX_TIME:
      so_mask = DS_SYS_SO_EX_3GPP_LTE_TDD;
      break;

    default:
      DS_MSG1(MSG_LEGACY_HIGH,
              "div duplex value [%d] unrecognized!",div_duplex);
      break;
  }
  return so_mask;
} /* dssysmgr_get_so_mask_from_div_duplex() */

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
)
{
  ds_sys_rat_ex_enum_type  new_rat_value = DS_SYS_RAT_EX_3GPP_GERAN;
  uint64 new_so_mask = 0;
  /*-----------------------------------------------------------------------------*/
  
  if((stack_info_ptr == NULL) || (dssysmgr_cached_ss_info_ptr == NULL))
    return;

  if(dssysmgr_is_srv_domain_ps_supported(stack_info_ptr->srv_domain)
       == FALSE)
  {
    if((stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_ONLY)||
        (stack_info_ptr->srv_domain == SYS_SRV_DOMAIN_CAMPED))
    {
      new_rat_value = DS_SYS_RAT_EX_NULL_BEARER;
    }
  }
  dssysmgr_radio_sm_instance->radio = dssysmgr_ss_to_sysmgr_radiotype(stack_info_ptr->sys_mode);
  
  if(stack_info_ptr->srv_status != SYS_SRV_STATUS_SRV)
  {
    new_rat_value = DS_SYS_RAT_EX_NULL_BEARER;
  }

  new_so_mask = dssysmgr_get_so_mask_for_egprs_support(stack_info_ptr->cell_srv_ind);

  if((new_rat_value != dssysmgr_radio_sm_instance->rat_value) || (new_so_mask != dssysmgr_radio_sm_instance->so_mask))
  {
    dssysmgr_radio_sm_instance->rat_value = new_rat_value;
    dssysmgr_radio_sm_instance->so_mask = new_so_mask;
  }
  
  return;
}/* dssysmgr_process_gsm_stack_info */

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
)
{
  dssysmgr_radio_e_type translated_radio = DSSYSMGR_RADIO_OOS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  switch(ss_sys)
  {
    case SYS_SYS_MODE_LTE:
      translated_radio =  DSSYSMGR_RADIO_LTE;
    break;

    case SYS_SYS_MODE_GSM:
      translated_radio = DSSYSMGR_RADIO_GSM;
    break;

    default:
    break;
  }
  return translated_radio;
}/* dssysmgr_ss_to_sysmgr_radiotype() */


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
)
{
  if((ss_event_info_ptr == NULL) || (dssysmgr_cached_ss_info_ptr == NULL))
    return FALSE;
  
  if((ss_event_info_ptr->ps_data_suspend != dssysmgr_cached_ss_info_ptr->ps_data_suspend) ||
     (ss_event_info_ptr->sys_mode != dssysmgr_cached_ss_info_ptr->sys_mode) ||
     (ss_event_info_ptr->srv_status != dssysmgr_cached_ss_info_ptr->srv_status) ||
     (ss_event_info_ptr->srv_domain != dssysmgr_cached_ss_info_ptr->srv_domain) ||
     (ss_event_info_ptr->effective_srv_domain != dssysmgr_cached_ss_info_ptr->effective_srv_domain) )
  {
    return TRUE;
  }
  
  return FALSE;
  
}/* dsysmgr_check_ss_event_processing_required() */

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
)
{
  ds_sys_system_status_type dssysmgr_sys_info;
  ds_sys_event_enum_type event = DS_SYS_SYSTEM_STATUS_EV;
  boolean notify = FALSE;

  memset(&dssysmgr_sys_info, 0, sizeof(ds_sys_system_status_type));
  
  dssysmgr_sys_info.num_avail_sys = DSSYSMGR_MAX_SYS_ALLOWED;
  dssysmgr_sys_info.avail_sys[0].active_rat = SYS_RAT_NONE;
  
  switch(radio)
  {
    case DSSYSMGR_RADIO_LTE:
      dssysmgr_sys_info.avail_sys[0].technology = DS_SYS_NETWORK_3GPP;
      dssysmgr_sys_info.avail_sys[0].rat_value =
                                   dssysmgr_radio_sm_instance->rat_value;
      dssysmgr_sys_info.avail_sys[0].so_mask = 
                                   dssysmgr_radio_sm_instance->so_mask;
      dssysmgr_sys_info.avail_sys[0].active_rat = ds_pdn_mgr_get_active_rat();
    break;
    
    case DSSYSMGR_RADIO_GSM:
      dssysmgr_sys_info.avail_sys[0].technology = DS_SYS_NETWORK_3GPP;
      dssysmgr_sys_info.avail_sys[0].rat_value =
                                   dssysmgr_radio_sm_instance->rat_value;
      dssysmgr_sys_info.avail_sys[0].so_mask =
                                   dssysmgr_radio_sm_instance->so_mask;
      ds_pdn_hdlr_set_network_mode(SYS_SYS_MODE_GSM);
    break;
    
    case DSSYSMGR_RADIO_OOS:	
      dssysmgr_sys_info.avail_sys[0].technology = DS_SYS_NETWORK_3GPP;
      dssysmgr_sys_info.avail_sys[0].rat_value =
                                   DS_SYS_RAT_EX_NULL_BEARER;
      dssysmgr_sys_info.avail_sys[0].so_mask = 0;	                                
          ds_pdn_hdlr_set_network_mode(SYS_SYS_MODE_NO_SRV);
    break;
    default:
      DS_MSG0(MSG_LEGACY_HIGH,"Invalid radio system");
  }
  
  if(dssysmgr_sys_info.avail_sys[0].rat_value == DS_SYS_RAT_EX_NULL_BEARER)
  {
    dssysmgr_sys_info.avail_sys[0].so_mask = 0;
  }
    
  if((dssysmgr_notified_param.rat_value != dssysmgr_sys_info.avail_sys[0].rat_value) ||
    (dssysmgr_notified_param.technology != dssysmgr_sys_info.avail_sys[0].technology) ||
    (dssysmgr_notified_param.so_mask != dssysmgr_sys_info.avail_sys[0].so_mask) ||
    (dssysmgr_notified_param.active_rat != dssysmgr_sys_info.avail_sys[0].active_rat))
  {
    notify = TRUE;
    dssysmgr_notified_param.technology = dssysmgr_sys_info.avail_sys[0].technology;
    dssysmgr_notified_param.rat_value = dssysmgr_sys_info.avail_sys[0].rat_value;
    dssysmgr_notified_param.so_mask = dssysmgr_sys_info.avail_sys[0].so_mask;
    dssysmgr_notified_param.active_rat = dssysmgr_sys_info.avail_sys[0].active_rat;
  }
  if(notify)
  {
    DS_MSG4(MSG_LEGACY_HIGH,"Notifying to external clients :: pref nw :%d "
                            "rat_value :%d  so_mask :%lu active_rat %d",
                            dssysmgr_sys_info.avail_sys[0].technology,
                            dssysmgr_sys_info.avail_sys[0].rat_value,
                            dssysmgr_sys_info.avail_sys[0].so_mask,
                            dssysmgr_sys_info.avail_sys[0].active_rat);
    ds_sys_event_ind(DS_SYS_TECH_ALL,
                     event,
                     (void *)&dssysmgr_sys_info
                     );
  }
  return;
}/*dssysmgr_notify_radio_event()*/


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
void dssysmgr_cm_ph_cb( cm_ph_event_e_type         ph_event,
                               const cm_ph_info_s_type   *event_ptr )
{
  boolean   notify = FALSE;
  ds_command_type *cmd_ptr = NULL;
  dssysmgr_ph_event_info_type *ph_ev_info_ptr = NULL;
  dssysmgr_cm_common_event_info common_event_info = {{0}};
  /*-----------------------------------------------------------------------------*/
  
  if(event_ptr == NULL)
    return;

  switch(ph_event)
  {
    case CM_PH_EVENT_OPRT_MODE:
    case CM_PH_EVENT_SYS_SEL_PREF:
    case CM_PH_EVENT_INFO:
    case CM_PH_EVENT_DUAL_STANDBY_PREF:
    case CM_PH_EVENT_DATA_PRIORITY:
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
      notify = TRUE;
      break;

    default:
      notify = FALSE;
      break;
  }

  if(notify)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(dssysmgr_ph_event_info_type));
    if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      DS_ASSERT(0);
      return;
    }
    cmd_ptr->hdr.cmd_id = DS_CMD_CM_PH_EV_INFO;

    ph_ev_info_ptr = (dssysmgr_ph_event_info_type*)cmd_ptr->cmd_payload_ptr;

    ph_ev_info_ptr->ph_event           = ph_event;
    ph_ev_info_ptr->srv_domain_pref    = event_ptr->srv_domain_pref;
    ph_ev_info_ptr->oprt_mode          = event_ptr->oprt_mode;
    ph_ev_info_ptr->mode_pref          = event_ptr->mode_pref;
    ph_ev_info_ptr->ph_state           = event_ptr->ph_state;
    ph_ev_info_ptr->mode_capability    = event_ptr->mode_capability;
    ph_ev_info_ptr->band_capability    = event_ptr->band_capability;
    ph_ev_info_ptr->lte_band_capability= event_ptr->lte_band_capability;
    
    ds_put_cmd( cmd_ptr );
  }
  common_event_info.ph_ev_info.ph_event = ph_event;
  common_event_info.ph_ev_info.ph_info_ptr =(cm_ph_info_s_type*)event_ptr;
  dssysmgr_common_notify_client(DSSYSMGR_EVT_TYPE_PH, common_event_info);
  return;
}/*dssysmgr_cm_ph_cb()*/

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
void dssysmgr_process_cm_ph_event_cmd(ds_command_type  * cmd_ptr)
{

  ds_local_err_val_e_type       local_err = DS_LOCAL_ERR_VAL_NONE;
  dssysmgr_ph_event_info_type*  ph_info_ptr = NULL;
  sys_sys_mode_e_type           curr_mode = SYS_SYS_MODE_NONE;
  /*-----------------------------------------------------------------------------*/
  
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  ph_info_ptr = (dssysmgr_ph_event_info_type*)cmd_ptr->cmd_payload_ptr;
  
  curr_mode = dssysmgr_cached_ss_info_ptr->sys_mode;
  
      /* Store the client requested Service Domain preference */
      ds3g_oprt_mode_srv_domain = ph_info_ptr->srv_domain_pref;
      
      /* Set the system mode preference */
      ds3g_sys_mode_pref = ph_info_ptr->mode_pref;
      
      /*---------------------------------------------------------------------
        Notify if operating mode has changed
      ---------------------------------------------------------------------*/
      if(ph_info_ptr->oprt_mode != ds_3gpp_cm_ph_info_oprt_mode)
      {
        DS_MSG2(MSG_LEGACY_HIGH,
                        "oprt_mode change: Old_mode:%d, new_mode:%d",
                        ds_3gpp_cm_ph_info_oprt_mode,
                        ph_info_ptr->oprt_mode);

        ds_3gpp_cm_ph_info_oprt_mode = ph_info_ptr->oprt_mode;
      }
      qmi_if_process_cm_ph_event(ph_info_ptr->ph_event,ph_info_ptr);
  
func_exit:
  if(DS_LOCAL_ERR_VAL_NONE != local_err)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"dssysmgr_process_cm_ph_event_cmd: err d",local_err);
  }
  return;
}/*dssysmgr_process_cm_ph_event_cmd()*/

/*===========================================================================
FUNCTION      DSSYSMGR_GET_SO_MASK_FOR_EGPRS_SUPPORT

DESCRIPTION   This function returns the SO MASK corresponding to the passed 
               EGPRS support present in cell_srv_ind.

PARAMETERS    cell_srv_ind - Cell service indication info recvd. from CM

DEPENDENCIES  None.

RETURN VALUE  SO mask 

SIDE EFFECTS  None.
===========================================================================*/
uint64 dssysmgr_get_so_mask_for_egprs_support(cm_cell_srv_ind_s_type cell_srv_ind)
{

  uint64 so_mask = DS_SYS_SO_EX_UNSPECIFIED;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (cell_srv_ind.egprs_supp)
  {
    case SYS_EGPRS_SUPPORT_NOT_AVAIL:
      so_mask = DS_SYS_SO_EX_3GPP_GPRS;
      break;

    case SYS_EGPRS_SUPPORT_AVAIL:
      so_mask = DS_SYS_SO_EX_3GPP_EDGE;
      break;

    default:
      DS_MSG1(MSG_LEGACY_HIGH,"Egprs_support value [%d] unrecognized!",
                               cell_srv_ind.egprs_supp);
      break;
  }
  
  return so_mask;
} /* dssysmgr_get_so_mask_for_egprs_support */

/*===========================================================================
FUNCTION      DSSYSMGR_ATTACH_COMPLETE_IND

DESCRIPTION   This function notifies Attach complete to 3gcmif module.

PARAMETERS    None

DEPENDENCIES  None.

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void dssysmgr_attach_complete_ind()
{
  /*Set the RAT mask to LTE once attach is complete*/
  dssysmgr_radio_sm_instance->rat_value = DS_SYS_RAT_EX_3GPP_LTE;
  if(dssysmgr_radio_sm_instance->radio != DSSYSMGR_RADIO_LTE)
    return;

  /* Notify clients of the updated RAT value */
  dssysmgr_notify_radio_event(dssysmgr_radio_sm_instance->radio);
  
  return;
}

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
)
{
  dssysmgr_event_buf* reg_event_buf = NULL;
  /*-----------------------------------------------------------------------------*/

  if(dssysmgr_client < DSSYSMGR_CLIENT_MIN || dssysmgr_client > DSSYSMGR_CLIENT_MAX )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Invalid Client id");
    return FALSE;
  }
  
  if(NULL == ds3g_client_cb_f_ptr)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Invalid Callback function");
    return FALSE;
  }
  
  if((dssysmgr_reg_client_list[dssysmgr_client] != NULL) && 
      (dssysmgr_reg_client_list[dssysmgr_client]->is_regd == TRUE))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Client already registered");
    return TRUE;
  }
  else
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(reg_event_buf, sizeof(dssysmgr_event_buf),
                            dssysmgr_event_buf*);
  
    if(reg_event_buf != NULL)
    {
      reg_event_buf->client_id = dssysmgr_client;
      reg_event_buf->cb_fn_ptr = ds3g_client_cb_f_ptr;
      reg_event_buf->event_type_mask = event_type_mask;
      reg_event_buf->is_regd = TRUE;
      reg_event_buf->cb_data = data_ptr;
      dssysmgr_reg_client_list[dssysmgr_client] = reg_event_buf;
    }
    else
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Failed to allocate client event buffer");
      return FALSE;
    }
  }

  return TRUE;
}/*dssysmgr_client_reg_event()*/

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
)
{
  dssysmgr_event_buf* reg_event_buf = NULL; 
  /*------------------------------------------------------------------*/
  
  if(dssysmgr_client < DSSYSMGR_CLIENT_MIN || dssysmgr_client > DSSYSMGR_CLIENT_MAX )
  {
    DS_MSG0(MSG_LEGACY_LOW,"Invalid Client id");
    return FALSE;
  }

  if(dssysmgr_reg_client_list[dssysmgr_client] == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Client not registered or already deregistered");
    return TRUE;
  }
  else
  {
    reg_event_buf = dssysmgr_reg_client_list[dssysmgr_client];
    DS_SYSTEM_HEAP_MEM_FREE(reg_event_buf);
    dssysmgr_reg_client_list[dssysmgr_client] = NULL;
  }
  return TRUE;
}/* dssysmgr_client_dereg_event */

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
)
{
  dssysmgr_event_buf *reg_event_buf = NULL; 
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
  uint8 index = 0;

  if(event_type < DSSYSMGR_EVT_TYPE_MIN || event_type > DSSYSMGR_EVT_TYPE_MAX)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  for(index = DSSYSMGR_CLIENT_MIN; index < DSSYSMGR_CLIENT_MAX; index++)
  {
    reg_event_buf = dssysmgr_reg_client_list[index];

    if((reg_event_buf) && (reg_event_buf->is_regd == TRUE)
       && (reg_event_buf->event_type_mask & (1 << event_type)))
    {
        reg_event_buf->cb_fn_ptr(event_type,common_info);
    }
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR, "dssysmgr_notify_ss_ev_to_client : err %d",local_err);
  }
  return;
}

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
)
{
  return dssysmgr_cached_ss_info_ptr->srv_status;
} /* dssysmgr_get_srv_status */

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
)
{
  return dssysmgr_cached_ss_info_ptr->srv_domain;
} /* dssysmgr_get_srv_status */

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
)
{
  return ds3g_oprt_mode_srv_domain;
} /* dssysmgr_get_oprt_mode_srv_domain */

/*===========================================================================
FUNCTION      DS3GI_CM_PSM_ENTER_REQ_CN

DESCRIPTION   This callback function is registered with CM to notify DS of
              PSM READY REQUEST. It posts cmd to DS.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_cm_psm_ready_req_cb( void )
{
  ds_command_type           *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);

  if(cmd_ptr == NULL)
  {
    return FALSE;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"PSM Ready Command Posted");

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_PSM_READY;

  ds_put_cmd(cmd_ptr);

  return TRUE;;
}

/*===========================================================================
FUNCTION      dssysmgr_psm_enter_req_cb

DESCRIPTION   This callback function is registered with CM to notify DS of
              PSM ENTER REQUEST. It posts cmd to DS.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssysmgr_psm_enter_req_cb( void )
{
  ds_command_type          *cmd_ptr = NULL;

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);

  if(cmd_ptr == NULL)
  {
     return FALSE;
  }

  DS_MSG0(MSG_LEGACY_HIGH,
                    "PSM Enter Command Posted");

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_PSM_ENTER;

  ds_put_cmd(cmd_ptr);

  return TRUE;
}

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
)
{
  return ds_3gpp_cm_ph_info_oprt_mode;
} /* dssysmgr_get_oprt_mode_srv_domain */

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
)
{
  ds3g_sys_mode_pref = pref_mode;
  return;
}

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
)
{
  return ds3g_sys_mode_pref;
}

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
)
{
  sys_sys_mode_e_type  sys_mode =  SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(mode_pref)
  {
    case CM_MODE_PREF_CDMA_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_ONLY:
    case CM_MODE_PREF_CDMA_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY:
    case CM_MODE_PREF_CDMA_GSM:
    case CM_MODE_PREF_CDMA_GW_TDS:
    case CM_MODE_PREF_CDMA_GSM_AMPS:
    case CM_MODE_PREF_CDMA_GSM_GPS_AMPS:
      sys_mode = SYS_SYS_MODE_CDMA;
      break;

    case CM_MODE_PREF_HDR_ONLY:
    case CM_MODE_PREF_CDMA_HDR_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_HDR_ONLY:
    case CM_MODE_PREF_HDR_WLAN:
    case CM_MODE_PREF_CDMA_HDR_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY:
    case CM_MODE_PREF_CDMA_HDR_GW_TDS:
    case CM_MODE_PREF_CDMA_HDR_GSM_AMPS:
    case CM_MODE_PREF_CDMA_HDR_GSM_GPS_AMPS:
    case CM_MODE_PREF_HDR_GSM:
      sys_mode = SYS_SYS_MODE_HDR;
      break;

    case CM_MODE_PREF_GSM_ONLY:
    case CM_MODE_PREF_GSM_WLAN:
    case CM_MODE_PREF_GSM_GPS:
      sys_mode = SYS_SYS_MODE_GSM;
      break;

    case CM_MODE_PREF_WCDMA_ONLY:
    case CM_MODE_PREF_WCDMA_WLAN:
    case CM_MODE_PREF_GSM_WCDMA_ONLY:
    case CM_MODE_PREF_GW_WLAN:
    case CM_MODE_PREF_WCDMA_GPS:
    case CM_MODE_PREF_GW_GPS:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;
    case CM_MODE_PREF_LTE_ONLY:
    case CM_MODE_PREF_GWL:
    case CM_MODE_PREF_CDMA_LTE_ONLY:
    case CM_MODE_PREF_HDR_LTE_ONLY:
    case CM_MODE_PREF_GSM_LTE:
    case CM_MODE_PREF_WCDMA_LTE:
    case CM_MODE_PREF_CDMA_HDR_LTE_ONLY:
    case CM_MODE_PREF_CDMA_GSM_LTE:
    case CM_MODE_PREF_CDMA_WCDMA_LTE:
    case CM_MODE_PREF_HDR_GSM_LTE:
    case CM_MODE_PREF_HDR_WCDMA_LTE:
    case CM_MODE_PREF_TDS_GSM_LTE:
    case CM_MODE_PREF_TDS_GSM_WCDMA_LTE:
    case CM_MODE_PREF_TDS_LTE:
    case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
    case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
    case CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE:
    case CM_MODE_PREF_TDS_WCDMA_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;

    case CM_MODE_PREF_AUTOMATIC:
    default:
#if defined(FEATURE_DATA_LTE)
      sys_mode = SYS_SYS_MODE_LTE;
#elif defined(FEATURE_GSM)
      sys_mode = SYS_SYS_MODE_GSM;
#endif
      break;
  }

  return sys_mode;
} /* ds3gsubsmgr_mode_pref_to_sys_mode() */

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
)
{
  ds_command_type      *cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
  If ph_cmd is not SYS_SEL_PREF return, Function ptr called incorrectly
  -------------------------------------------------------------------------*/

  if(ph_cmd != CM_PH_CMD_SYS_SEL_PREF)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"CM_PH_CMD_ERR:%d",ph_cmd);
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "SYS_SEL_PREF_ERR:%d", ph_cmd_err);

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    return;
  }

  if(ph_cmd_err == CM_PH_CMD_ERR_NOERR)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR;
    cmd_ptr->cmd_payload_ptr = data_block_ptr;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_SYS_SEL_PREF_ERR;
  }

  ds_put_cmd(cmd_ptr);
  return;
} /* ds3g_cm_ph_sys_sel_cb() */

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
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - -*/
  return (void *)ds3g_cm_ph_cmd_cb_f_ptr;
} /* ds3g_get_sys_sel_cb_fn_ptr() */

/*===========================================================================
FUNCTION   DS3GEX_CM_SRV_IND_HDLR

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
)
{
  cm_3gpp_service_status_s_type        *payload_ptr = NULL;
  static sys_srv_domain_e_type          prev_srv_domain;
  sys_srv_domain_e_type                 curr_srv_domain = SYS_SRV_DOMAIN_NONE;  
  boolean                               ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  do 
  {
    if(msgrtype == MM_CM_3GPP_SRV_IND)
    {
      if (dsmsg_ptr == NULL)
      {            
        break;         
      }

      payload_ptr = (cm_3gpp_service_status_s_type*)dsmsg_ptr;

      /*--------------------------------------------------------------------------
        Ignore if the current serving domain is CAMPED
      --------------------------------------------------------------------------*/
      curr_srv_domain = payload_ptr->srv_domain;
      
      DS_MSG3(MSG_LEGACY_HIGH, "DSMGR CM Serv ind. prev_srv_domain: %d, "
                      "current srv domain: %d, rat: %d",
                      prev_srv_domain,
                      curr_srv_domain,
                      payload_ptr->rat);

      if (curr_srv_domain == SYS_SRV_DOMAIN_CAMPED)
      {
        ret_val = TRUE;
        break;
      }      
       
      /*--------------------------------------------------------------------------
        Clear throttling when UE detaches. This is translated to the following
        serving domain translations:
        1. PS    --> None
        2. PS    --> CS only
        3. CS+PS --> CS only
        4. CS+PS --> None
      --------------------------------------------------------------------------*/
      if ((prev_srv_domain != curr_srv_domain) &&
          ((prev_srv_domain == SYS_SRV_DOMAIN_CS_PS) || 
           (prev_srv_domain == SYS_SRV_DOMAIN_PS_ONLY))&&
          ((curr_srv_domain != SYS_SRV_DOMAIN_CS_PS) && 
           (curr_srv_domain != SYS_SRV_DOMAIN_PS_ONLY))
         )
      {
        ds_3gpp_pdn_throt_detach_info_cb(payload_ptr->rat);
      }

      prev_srv_domain = curr_srv_domain;
      ret_val = TRUE;

    }/* msgrtype == MM_CM_3GPP_SRV_IND */

  }while (0); 

  return ret_val;

} /* ds_3g_cm_srv_ind_hdlr */

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
)
{
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(msgrtype == MM_CM_PSM_DEEP_SLEEP_ENTER_IND) 
  {
    if (dsmsg_ptr == NULL)
    {            
      return FALSE;         
    }
	DS_MSG0(MSG_LEGACY_HIGH," MM_CM_PSM_DEEP_SLEEP_ENTER_IND received");
    dssysmgr_deep_sleep_status = TRUE;
    return TRUE;
  }
  if(msgrtype == MM_CM_PSM_DEEP_SLEEP_EXIT_IND)
  {
    if (dsmsg_ptr == NULL)
    {            
      return FALSE;
    }
	DS_MSG0(MSG_LEGACY_HIGH," MM_CM_PSM_DEEP_SLEEP_EXIT_IND received");
    dssysmgr_deep_sleep_status = FALSE;
    return TRUE;
  }
  DS_MSG0(MSG_LEGACY_HIGH," Invalid cmd received");
  return FALSE;
 }

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
boolean dssysmgr_get_cm_deep_sleep_status()
{
  return dssysmgr_deep_sleep_status;
}
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
)
{
  cm_wwan_gps_scan_status_e_type status = CM_WWAN_GPS_SCAN_STATUS_NONE;

  if (dsmsg == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"dsmsg is NULL");
    return FALSE;
  }
  if (msgrtype != MM_CM_WWAN_GPS_SCAN_STATUS_IND)
  {
    return FALSE;
  }

  status = ((cm_wwan_gps_scan_status_ind_s_type*)dsmsg)->scan_status;
  if (status == CM_WWAN_GPS_SCAN_STATUS_HIGH_PRIORITY_STARTED)
  {
    ue_is_in_gps_high_priority_search = TRUE;
	ds_pdn_evaluate_gps_high_priority_search_flow_for_all_bearers(FALSE);

  }
  if(status == CM_WWAN_GPS_SCAN_STATUS_HIGH_PRIORITY_ENDED)
  {
     ue_is_in_gps_high_priority_search = FALSE;
	 ds_pdn_evaluate_gps_high_priority_search_flow_for_all_bearers(TRUE);
	 
  }

  return TRUE;
}
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

boolean dssysmgr_get_gps_search_status()
{
  return ue_is_in_gps_high_priority_search;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif 
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
boolean dssysmgr_get_nb1_ntn_rat_status()
{
  return dssysmgr_is_NB1_NTN_RAT;
}
#endif