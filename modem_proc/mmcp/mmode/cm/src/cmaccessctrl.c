/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C A L L   M A N A G E R   A C C E S S   C O N T R O L

GENERAL DESCRIPTION
  This module contains declarations used by the call manager in processing
  Access Control command. The module uses the callback function for the caller
  and also MSGR is used for broadcasting the information between CM and all
  other modules who are interested in the information of the access controller

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmac_init() must be called to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmaccessctrl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/13   xs      Added debug code
08/08/13   xs      Remove emg handling from CMAC
07/19/13   jvo     Remove all MDM/MSM Fusion features
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
02/15/13   th      Initial design

===========================================================================*/
#if !defined(FEATURE_MMODE_DISABLE_ACCESS_CONTROL)
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#include <stdlib.h>
#include <string.h>

#include "comdef.h"    /* Definition for basic types and macros */

#include "cm_msgr_msg.h"

#include "cmph.h"      /* Interface to CM phone object */
#include "cmclnup.h"
#include "cmdbg.h"
#include "cmtaski.h"
#include "cmclient.h"
#include "cmaccessctrl.h"
#include "cmcall.h"
#include "cmregprxi.h"
#ifdef CM_GW_SUPPORTED
#include "cmwcall.h"
#endif
#if defined(FEATURE_MMODE_3GPP2)
#include "cai.h"
#endif
#include "cmpmprx.h"
#include "sys.h"       /* System wide definitions */


static cmac_s_type cmac_local;


/*===========================================================================

FUNCTION cmac_get_act_id

DESCRIPTION
  Return act id for access control

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_act_id_type cmac_get_act_id (void)
{
  return (cm_act_id_type)&cmac_local;
} /* cmac_get_act_id() */

/*===========================================================================

FUNCTION cmac_is_in_thermal_emergency_level2

DESCRIPTION
  Returns TRUE if CM AC is in level2 thermal emergency due to UNSAFE temperature

===========================================================================*/
boolean cmac_is_in_thermal_emergency_level2(void)
{
  return (cmac_local.state == CMAC_STATE_THERMAL_EMERG_LEVEL_2);
}

/*===========================================================================

FUNCTION cmac_is_in_thermal_emergency

DESCRIPTION
  Returns TRUE if CM AC is in emergency due to UNSAFE temperature

===========================================================================*/
boolean cmac_is_in_thermal_emergency(void)
{
  if((cmac_local.state == CMAC_STATE_EMERGENCY_ONLY) ||
     (cmac_local.state == CMAC_STATE_EMERGENCY_PENDING)||
     (cmac_local.state == CMAC_STATE_3GPP_DETACH_PENDING))
  {
    return cmac_local.reason_mask & BM(CM_AC_REASON_UNSAFE_TEMP);
  }

  return FALSE;

}

/*===========================================================================

FUNCTION cmac_is_in_sim_lock

DESCRIPTION
  Returns TRUE if CM AC is in emergency due to SIM Lock

===========================================================================*/
boolean cmac_is_in_sim_lock(void)
{
  if((cmac_local.state == CMAC_STATE_EMERGENCY_ONLY) ||
     (cmac_local.state == CMAC_STATE_EMERGENCY_PENDING)||
     (cmac_local.state == CMAC_STATE_3GPP_DETACH_PENDING))
  {
    return cmac_local.reason_mask & BM(CM_AC_REASON_SIM_LOCK);
  }

  return FALSE;

}

/*===========================================================================

FUNCTION cmac_is_in_emergency_only

DESCRIPTION
  Returns TRUE if CM AC is in emergency only state due to any of the reasons

===========================================================================*/
boolean cmac_is_in_emergency_only(void)
{
  return cmac_local.state == CMAC_STATE_EMERGENCY_ONLY ? TRUE: FALSE;
}

/*===========================================================================

FUNCTION cmac_set_current_state

DESCRIPTION
  Set the current state of access control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmac_set_current_state ( cmac_state_e_type ac_state )
{
  if (ac_state >= CMAC_STATE_MAX || ac_state < CMAC_STATE_NORMAL)
  {
    CM_ERR_1("AC state changed failed due to incorrect state: %d" ,ac_state);
    return;
  }

  cmac_local.state = ac_state;
  CM_MSG_HIGH_1("Set AC current state: %d" ,ac_state);

} /* cmac_set_current_state */

/*===========================================================================

FUNCTION cmac_set_current_state_as_emergency_wait

DESCRIPTION
  Set the current state of access control as emergency waiting

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmac_set_current_state_as_emergency_wait ( void )
{
  cmac_set_current_state(CMAC_STATE_EMERGENCY_PENDING);
}

/*===========================================================================

FUNCTION cmac_client_cmd_err

DESCRIPTION
  Notify client access control command error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cmac_client_cmd_err
(
    cm_ac_cmd_s_type        *ac_cmd_ptr,
    cm_ac_cmd_err_e_type    cmd_err
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ac_cmd_ptr != NULL );
  CM_ASSERT( ac_cmd_ptr->cmd_type == CM_CMD_TYPE_AC );
  CM_ASSERT( BETWEEN( cmd_err, CM_AC_CMD_ERR_NONE, CM_AC_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at access control command component
  */

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( ac_cmd_ptr->cmd_cb_func != NULL )
  {
    ac_cmd_ptr->cmd_cb_func( ac_cmd_ptr->data_block_ptr,
                             ac_cmd_ptr->cmd,
                             cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command error is OTHER than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_AC_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH_3( "AC cmd err!, cmd=%d, err=%d, client=%ld",
                         ac_cmd_ptr->cmd, cmd_err, ac_cmd_ptr->client_id );

    cmclient_list_ac_cmd_err_ntfy( cmd_err, ac_cmd_ptr );
  }

} /* cmac_client_cmd_err() */

/*===========================================================================

FUNCTION cmac_client_cmd_check

DESCRIPTION
  Check for access control command parameter errors

DEPENDENCIES
  None.

RETURN VALUE
  CM_AC_CMD_ERR_NOERR if command is allowed in the current state

SIDE EFFECTS
  Might change one or more of the command parameters pointed by cmd_ptr.

===========================================================================*/

static cm_ac_cmd_err_e_type  cmac_client_cmd_check
(
    cm_ac_cmd_s_type *ac_cmd_ptr
)
{
  cm_ac_cmd_err_e_type    ac_cmd_err       = CM_AC_CMD_ERR_NOERR;
  cm_ac_cmd_info_s_type   *cmd_info_ptr    = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ac_cmd_ptr != NULL );
  CM_ASSERT( ac_cmd_ptr->cmd_type == CM_CMD_TYPE_AC );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cmac_client_cmd_check(), cmd=%d",ac_cmd_ptr->cmd );

  cmd_info_ptr = CMD_INFO_PTR(ac_cmd_ptr);
  CM_ASSERT( cmd_info_ptr != NULL );

  /* check cmd is already in error or not */
  if ( ac_cmd_ptr->cmd_err != CM_AC_CMD_ERR_NOERR)
  {
    return ac_cmd_ptr->cmd_err;
  }

  if ( cmd_info_ptr->reason >= CM_AC_REASON_MAX ||
       cmd_info_ptr->reason <= CM_AC_REASON_NONE )
  {
    return CM_AC_CMD_ERR_INVALID_REASON;
  }

  return ac_cmd_err;

} /* cmac_client_cmd_check() */


/*===========================================================================

FUNCTION cmac_send_notify_callback

DESCRIPTION
  Send event to clients through callbacks with reason as payload

===========================================================================*/

static void cmac_send_notify_callback(

  cm_ac_event_e_type evt,
    /* CMAC evt that is sent to CM-clients through callbacks */

  cm_ac_reason_e_type reason
    /* Reason for generating this event */
)
{
  cm_ac_info_s_type evt_info;

  evt_info.asubs_id = cmac_local.asubs_id;
  evt_info.reason = reason;

  cmclient_list_ac_event_ntfy(evt, &evt_info);

  if(reason == CM_AC_REASON_UNSAFE_TEMP)
  {
    /* cmph_event(ph_ptr, CM_PH_EVENT_THERMAL_EMERG_ENTER); */
  }
}

/*===========================================================================

FUNCTION cmac_send_notify_ind

DESCRIPTION
  Send event to client through message router with reason as payload

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmac_send_notify_ind(
  msgr_umid_type ac_evt_umid,
  cm_ac_reason_e_type reason,
  void* user_data,
  sys_modem_as_id_e_type asubs_id
)
{
  errno_enum_type msgr_error;
  cm_ac_info_ind_s_type *ac_evt;
  ac_evt = (cm_ac_info_ind_s_type *)cm_mem_malloc(
    sizeof(cm_ac_info_ind_s_type));

  if(ac_evt == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  memset(ac_evt, 0, sizeof(cm_ac_info_ind_s_type));

  ac_evt->reason = reason;
  ac_evt->user_data = user_data;
  ac_evt->asubs_id  = asubs_id;

  ac_evt->hdr.inst_id= SYS_AS_ID_TO_INST_ID(asubs_id);
        
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("CM->MSGR: NOTIFY_IND, asubs_id=%d, inst_id=%d, reason=%d",
                   ac_evt->asubs_id,
                   ac_evt->hdr.inst_id,
                   reason);

  msgr_error = cm_msgr_send( ac_evt_umid, MSGR_CM_AC,
                           &(ac_evt->hdr), sizeof(cm_ac_info_ind_s_type) );
  cm_mem_free(ac_evt);

} /* cmac_send_notify_ind */

/*===========================================================================

FUNCTION cmac_init

DESCRIPTION
  Access control initialize

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmac_init( void )
{
  cmac_local.reason         = CM_AC_REASON_NONE;
  cmac_local.reason_mask    = 0;
  cmac_local.state          = CMAC_STATE_NORMAL;
  cmac_local.cmd_type       = CM_AC_CMD_NONE;
  cmac_local.data_block_ptr = NULL;
  cmac_local.state_cb_func  = NULL;
  cmac_local.cmd_cb_func    = NULL;
} /* cmac_init() */

/*===========================================================================

FUNCTION cmac_stop_mode_cnf_proc

DESCRIPTION
  As entering emergency happens in two stages, this function implements
  second stage after getting cnf from GWL deactivation.
  If there is no need to deactivate then this function is called directly
  at the end of first stage.
===========================================================================*/

static void cmac_stop_mode_cnf_proc(void)
{
  cmph_s_type         *ph_ptr        = cmph_ptr();
  cm_mode_pref_e_type prep_mode_pref = ph_ptr->main_stack_info.pref_info.mode_pref;
  cm_mode_pref_e_type cur_mode_pref  = ph_ptr->main_stack_info.pref_info.mode_pref;

#ifdef FEATURE_MMODE_EMERG_SUPPORT

  #if defined(FEATURE_MMODE_CDMA) || defined(FEATURE_MMODE_SC_SVLTE)
  /* Disable HDR in  emergency state.
  */
  prep_mode_pref = cm_remove_mode_pref_components(ph_ptr->main_stack_info.pref_info.mode_pref, 1,
                                   SD_SS_MODE_PREF_HDR);

  *cmph_hdr_kick_pending() = FALSE;
  #endif

  #if FEATURE_MMODE_SC_SVLTE

  /* If LTE is active on HYBR_2, then LTE on HYBR_2 is disabled. Because
  ** we allow emergency traffic only on one stack in ETM state.
  */
  if( cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1) 
      #ifdef FEATURE_MMODE_HYBR2_ENABLED
      &&
      cmss_ptr()->info.is_hybr_gw_operational 
      #endif
      &&
     cmregprx_get_stack_info_ptr(SD_SS_HYBR_2)->state ==
                                  CMREGPRX_STATE_ACTIVE )
  {
    prep_mode_pref = cm_remove_mode_pref_components(prep_mode_pref, 1,
                                                    SD_SS_MODE_PREF_LTE);
  }
  #endif

  ph_ptr->main_stack_info.pref_info.mode_pref = prep_mode_pref;

  CM_MSG_HIGH_0("CMAC: Before forcing emergency pref");

  #if defined(CM_GW_SUPPORTED) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  cmwcall_set_lmtd_srvc_req ();
  #endif

  (void) cmph_force_pref_on_the_fly(
                                CM_SS_MAX,
                                ph_ptr,
                                SD_SS_PREF_REAS_USER,
                                CM_ACT_TYPE_AC_EMERG_MODE,
                                ph_ptr->main_stack_info.pref_info.pref_term,
                                prep_mode_pref,
                                CM_BAND_PREF_NO_CHANGE,
                                SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                CM_BAND_PREF_NO_CHANGE,
                                ph_ptr->main_stack_info.pref_info.plmn,
                                CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV,
                                cmac_get_act_id(),
                                CM_ACT_UPDATE_REAS_EMERG_ENTER,
                                TRUE,
                                cmph_get_acq_pri_order_per_sub(CM_SS_MAX),
                                CM_DEFAULT_VALUE,
                                #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                ph_ptr->main_stack_info.pref_info.csg_id,
                                #else
                                SYS_CSG_ID_INVALID,
                                #endif
                                ph_ptr->main_stack_info.pref_info.csg_rat,
                                ph_ptr->main_stack_info.pref_info.ciot_lte_pref
                                );


  ph_ptr->main_stack_info.pref_info.mode_pref = cur_mode_pref;
  cmph_update_ddtm_status(ph_ptr);

  #ifdef FEATURE_MMODE_DUAL_SIM
  if(cmph_is_msim() && !cmph_is_sxlte() && cmac_is_in_thermal_emergency())
  {
    (void) cmph_force_pref_on_the_fly(
                                  CM_SS_HYBR_2,
                                  ph_ptr,
                                  SD_SS_PREF_REAS_USER,
                                  CM_ACT_TYPE_AC_EMERG_MODE,
                                  ph_ptr->hybr_2_stack_info.pref_info.pref_term,
                                  ph_ptr->hybr_2_stack_info.pref_info.mode_pref,
                                  CM_BAND_PREF_NO_CHANGE,
                                  SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                  CM_BAND_PREF_NO_CHANGE,
                                  CM_PRL_PREF_NO_CHANGE,
                                  CM_ROAM_PREF_NO_CHANGE,
                                  CM_HYBR_PREF_OFF,
                                  ph_ptr->hybr_2_stack_info.pref_info.plmn,
                                  CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                  CM_OTASP_ACT_CODE_NONE,
                                  ph_ptr->hybr_2_stack_info.pref_info.acq_order_pref,
                                  CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV,
                                  cmac_get_act_id(),
                                  CM_ACT_UPDATE_REAS_EMERG_ENTER,
                                  TRUE,
                                  cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_2),
                                  CM_DEFAULT_VALUE,
                                  #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                  ph_ptr->hybr_2_stack_info.pref_info.csg_id,
                                  #else
                                  SYS_CSG_ID_INVALID,
                                  #endif
                                  ph_ptr->hybr_2_stack_info.pref_info.csg_rat,

                                  ph_ptr->hybr_2_stack_info.pref_info.ciot_lte_pref
                                  );

  }
  #endif

  
  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  if((cmph_is_tsts() || cmph_is_sxlte()) && cmac_is_in_thermal_emergency())
  {
    (void) cmph_force_pref_on_the_fly(
                                  CM_SS_HYBR_3,
                                  ph_ptr,
                                  SD_SS_PREF_REAS_USER,
                                  CM_ACT_TYPE_AC_EMERG_MODE,
                                  ph_ptr->hybr_3_stack_info.pref_info.pref_term,
                                  ph_ptr->hybr_3_stack_info.pref_info.mode_pref,
                                  CM_BAND_PREF_NO_CHANGE,
                                  SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                  CM_BAND_PREF_NO_CHANGE,
                                  CM_PRL_PREF_NO_CHANGE,
                                  CM_ROAM_PREF_NO_CHANGE,
                                  CM_HYBR_PREF_OFF,
                                  ph_ptr->hybr_3_stack_info.pref_info.plmn,
                                  CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                  CM_OTASP_ACT_CODE_NONE,
                                  ph_ptr->hybr_3_stack_info.pref_info.acq_order_pref,
                                  CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV,
                                  cmac_get_act_id(),
                                  CM_ACT_UPDATE_REAS_EMERG_ENTER,
                                  TRUE,
                                  cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_3),
                                  CM_DEFAULT_VALUE,
                                  #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                  ph_ptr->hybr_3_stack_info.pref_info.csg_id,
                                  #else
                                  SYS_CSG_ID_INVALID,
                                  #endif
                                  ph_ptr->hybr_3_stack_info.pref_info.csg_rat,
                                  ph_ptr->hybr_3_stack_info.pref_info.ciot_lte_pref
                                  );

  }
#endif
#endif/*FEATURE_MMODE_EMERG_SUPPORT*/

  cmac_set_current_state(CMAC_STATE_EMERGENCY_ONLY);
  cmac_send_notify_callback(CM_AC_EVENT_EMERGENCY_ENTER, cmac_local.reason);
}

/*===========================================================================

FUNCTION cmac_deregister_cnf_proc

DESCRIPTION
  As entering emergency happens in three stages, this function implements
  second stage after getting deregister cnf from 1X CP.
  If there is no need to deregister from 1X CP then this function is called directly
  at the end of first stage.
===========================================================================*/

static void cmac_deregister_cnf_proc(void)
{
  cmph_s_type *ph_ptr = cmph_ptr();
  cmss_s_type *ss_ptr = cmss_ptr();
  cm_mode_pref_e_type prep_mode_pref;
  cm_mode_pref_e_type cur_mode_pref = ph_ptr->main_stack_info.pref_info.mode_pref;
  
  cmac_local.stop_mode_cnfs_pending = 0;
  
  /* Remove GWL from mode pref to send STOP_MODE to GWL/TD. This is required
  ** to put GWL/TD on limited service.
  */
  #ifdef FEATURE_MMODE_EMERG_SUPPORT
  prep_mode_pref = cm_remove_mode_pref_components(ph_ptr->main_stack_info.pref_info.mode_pref, 1,
                                   SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE
                                   );

  #else
  prep_mode_pref = CM_MODE_PREF_NONE;
  
  #endif
  cmac_set_current_state(CMAC_STATE_3GPP_DETACH_PENDING);

  if(prep_mode_pref != ph_ptr->main_stack_info.pref_info.mode_pref
    
#ifdef FEATURE_MMODE_EMERG_SUPPORT
    &&
     ss_ptr->main_srv_status != SYS_SRV_STATUS_LIMITED 
#endif
)
  {
    ph_ptr->main_stack_info.pref_info.mode_pref = prep_mode_pref;

    /* If Regprx is active CM needs to wait for STOP_MODE_CNF before
    ** entering Emergency only mode.
    */
    #ifdef FEATURE_MMODE_3GPP
    if( cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state == CMREGPRX_STATE_ACTIVE )
    {
      cmac_local.stop_mode_cnfs_pending++;

      (void) cmph_force_pref_on_the_fly( CM_SS_MAIN,
                                    ph_ptr,
                                    SD_SS_PREF_REAS_USER,
                                    CM_ACT_TYPE_AC_EMERG_MODE,
                                    ph_ptr->main_stack_info.pref_info.pref_term,
                                    prep_mode_pref,
                                    CM_BAND_PREF_NO_CHANGE,
                                    SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                    CM_BAND_PREF_NO_CHANGE,
                                    ph_ptr->main_stack_info.pref_info.plmn,
                                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                    ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                                    cmac_get_act_id(),
                                    CM_ACT_UPDATE_REAS_EMERG_ENTER,
                                    TRUE,
                                    cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                    CM_DEFAULT_VALUE,
                                    #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                    ph_ptr->main_stack_info.pref_info.csg_id,
                                    #else
                                    SYS_CSG_ID_INVALID,
                                    #endif
                                    ph_ptr->main_stack_info.pref_info.csg_rat,
                                    ph_ptr->main_stack_info.pref_info.ciot_lte_pref
                                    );
    }
    else 
    {
      #ifndef FEATURE_MMODE_EMERG_SUPPORT
      (void) cmph_force_pref_on_the_fly( CM_SS_MAIN,
                                       ph_ptr,
                                       SD_SS_PREF_REAS_USER,
                                       CM_ACT_TYPE_AC_EMERG_MODE,
                                       ph_ptr->main_stack_info.pref_info.pref_term,
                                       prep_mode_pref,
                                       CM_BAND_PREF_NO_CHANGE,
                                       SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                       CM_BAND_PREF_NO_CHANGE,
                                       ph_ptr->main_stack_info.pref_info.plmn,
                                       CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                       ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                                       cmac_get_act_id(),
                                       CM_ACT_UPDATE_REAS_EMERG_ENTER,
                                       TRUE,
                                       cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                       CM_DEFAULT_VALUE,
                                   #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                       ph_ptr->main_stack_info.pref_info.csg_id,
                                   #else
                                       SYS_CSG_ID_INVALID,
                                   #endif
                                       ph_ptr->main_stack_info.pref_info.csg_rat,
                                       ph_ptr->main_stack_info.pref_info.ciot_lte_pref
                                       );
      

      #endif
    }
    #endif
  }

  ph_ptr->main_stack_info.pref_info.mode_pref = cur_mode_pref;

  
  if(cmac_local.stop_mode_cnfs_pending == 0)
  {
    cmac_stop_mode_cnf_proc();
  }
}
/*===========================================================================

FUNCTION cmac_restrict_to_emergency

DESCRIPTION
  This function implements access control putting phone in emergency mode
  due to reasons in cm_ac_reason_e_type. This happens in two stages:
  1. If GWL protocols are active, then a deactivate is sent
  2. After recving conf for GWL activate, GWL protocols are put in limited
     service mode

  If at #1, GWL protocols are not active or in limited srv already, then #2
  is performed wihout delay.
===========================================================================*/

void cmac_restrict_to_emergency(

  cm_ac_reason_e_type reason
    /* Reason for putting modem in emergency mode */
)
{
  /* Add reason mask to access control requests */
  cmac_local.reason_mask |= BM(reason);

  CM_MSG_HIGH_2("CMAC is in state %d, emerg reason %d", cmac_local.state, reason);
  /* If modem is already in emergency state, ACK calling CM-client */
  if(cmac_local.state == CMAC_STATE_EMERGENCY_ONLY)
  {
    cmac_send_notify_callback(CM_AC_EVENT_EMERGENCY_ENTER, reason);
    return;
  }

  /* If already in any other AC state, then buffer this req */
  if(cmac_local.state != CMAC_STATE_NORMAL)
  {
    return;
  }

  /* If IMS deregistration/ DS PPP cleanup is pending on the subscription
  ** which is being put in emergency only mode, defer this request till the 
  ** said operations complete. 
  */
  if( cmclnup_is_ongoing_subsc(BM(cmac_local.asubs_id)) )
  {
    cmac_set_current_state( CMAC_STATE_WAIT_CLNUP );
    return;
  }

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if(reason == CM_AC_REASON_UNSAFE_TEMP)
  {
    cmcall_endall_non_emerg_calls(CM_CALL_END_THERMAL_EMERGENCY, FALSE);
  }
  else
  {
    cmcall_endall_non_emerg_calls(CM_CALL_END_EMERGENCY_ONLY, FALSE);
  }
  #endif
  
  cmac_local.reason = reason;

  if(cmcall_emergency_call_active() || cmph_is_in_emergency_cb())
  {
    cmac_set_current_state(CMAC_STATE_EMERGENCY_PENDING);
    return;
  }

  /* If CDMA defined send deregister to 1X, independent of whether actually in service
  ** 1X will respond with failure even when it is not active. Incase 1X is not defined
  ** directly go to handling of deregister cnf
  */
  cmac_deregister_cnf_proc();
  
  return;

}

/*===========================================================================

FUNCTION cmac_enable_emergency_if_pending

DESCRIPTION
  This function enables emergency mode if it was deferred due to emergency
  call or callback mode.
===========================================================================*/
void cmac_enable_emergency_if_pending(void)
{
  if( cmac_local.state == CMAC_STATE_EMERGENCY_PENDING ||
      cmac_local.state == CMAC_STATE_WAIT_CLNUP )
  {
    cmac_local.state = CMAC_STATE_NORMAL;
    cmac_restrict_to_emergency(cmac_local.reason);
  }
}

/*===========================================================================

FUNCTION cmac_restore_to_normal

DESCRIPTION
  Restore access control normal state where normal calls are enabled.
  Before call of this API, phone may be in emergency only mode due to
  reasons in cm_ac_reason_e_type.
===========================================================================*/

void cmac_restore_to_normal(

  cm_ac_reason_e_type reason
    /* Reason due to which normal opr being restored. */
)
{
  cmph_s_type *ph_ptr = cmph_ptr();
  cm_orig_q_s_type  *ph_orig_ptr;

  uint16 updated_mask = cmac_local.reason_mask & ~BM(reason);

  if( updated_mask & ( BM(CM_AC_REASON_UNSAFE_TEMP) |
                       BM(CM_AC_REASON_MAND_PDN_DISABLED) |
                       BM(CM_AC_REASON_SIM_LOCK) )
    )
  {
    /* Update the reason mask */
    cmac_local.reason_mask = updated_mask;

    /* Notify the clients about EMEGENCY_EXIT_PEDNING because not
    ** not call reasons for EMERGENCY_ONLY operation has been 
    ** revoked. 
    ** Send out the reason due to which normal operation was requested. 
    ** This would serve as an intermediate confirmation that the request 
    ** has been processed by CM but device cannot be moved to NORMAL 
    ** operation yet.
    */
    cmac_send_notify_callback( CM_AC_EVENT_EMERGENCY_EXIT_PENDING, reason );

    CM_MSG_HIGH_1("CMAC updated mask %d can't restore normal opr",
                                                          updated_mask);
    return;
  }

  // Detach pending state is required in case if CFCM moves to level 0 
  // before CM actually moves to level 3
  if(cmac_local.state == CMAC_STATE_EMERGENCY_ONLY ||
     cmac_local.state == CMAC_STATE_EMERGENCY_PENDING ||
     cmac_local.state == CMAC_STATE_3GPP_DETACH_PENDING)
  {
    sys_modem_as_id_e_type as_id_emg_cb;
    cm_ss_e_type ss_emg_cb;
    CM_MSG_HIGH_0("Deleting AC object");
    cmac_local.stop_mode_cnfs_pending = 0;

    cmac_set_current_state(CMAC_STATE_NORMAL);


    cmph_delete_orig_param( cmac_get_act_id(),CM_SS_MAIN);
    cmph_remove_orig_mode_with_priority_end(CM_SS_MAIN);
    cmph_delete_orig_param( cmac_get_act_id(),CM_SS_HDR);
    cmph_remove_orig_mode_with_priority_end(CM_SS_HDR);

    if(cmph_is_sxlte_any())
    {
      cmph_delete_orig_param( cmac_get_act_id(),CM_SS_HYBR_2);
      cmph_remove_orig_mode_with_priority_end(CM_SS_HYBR_2);
    }

    ph_orig_ptr = cmtask_orig_para_get_top(  CM_SS_MAIN );
    as_id_emg_cb = cmph_get_sub_in_emg_cb();

    if(as_id_emg_cb != SYS_MODEM_AS_ID_NONE)
    {
      ss_emg_cb = cmph_map_subs_to_ss(as_id_emg_cb);
      ph_orig_ptr = cmtask_orig_para_get_top(ss_emg_cb);

      if(ph_orig_ptr != NULL &&
        ph_orig_ptr->act_type == CM_ACT_TYPE_PH_OBJ)
      {
        CM_MSG_HIGH_0("Forcing preferences after deleting AC object");
        cmph_force_orig_mode_on_the_fly(ss_emg_cb);
      }
    }
    else
    {
      ph_orig_ptr = cmtask_orig_para_get_top(CM_SS_MAIN);

      if(ph_orig_ptr != NULL &&
        ph_orig_ptr->act_type == CM_ACT_TYPE_PH_OBJ)
      {
        CM_MSG_HIGH_0("Forcing preferences after deleting AC object");
        cmph_force_orig_mode_on_the_fly(CM_SS_MAX);
      }
    }

    if(!cmph_is_sxlte_any())
    {
      cmph_delete_update_orig_mode( CM_SS_HYBR_2, cmac_get_act_id(), TRUE);
    }

    cmph_delete_update_orig_mode( CM_SS_HYBR_3, cmac_get_act_id(), TRUE);

    cmph_update_ddtm_status(ph_ptr);
  }

  /* Update CMAC internal state */
  cmac_set_current_state(CMAC_STATE_NORMAL);

  /* Update rason_mask*/
  cmac_local.reason_mask = updated_mask;

  /* Notify clients that UE is resuming normal operation */
  cmac_send_notify_callback(CM_AC_EVENT_EMERGENCY_EXIT, reason);
}

/*===========================================================================

FUNCTION cmac_nas_rpt_proc

DESCRIPTION
  Process reports from NAS. In CMAC, we are looking at STOP_MODE_CNF

===========================================================================*/
void cmac_nas_rpt_proc(

  const cm_hdr_type   *rpt_ptr

)
{
  cm_rpt_type *cm_rpt_ptr = (cm_rpt_type *)rpt_ptr;

  switch(cm_rpt_ptr->hdr.cmd)
  {
    case CM_STOP_MODE_CNF:
      if(cmac_local.state == CMAC_STATE_3GPP_DETACH_PENDING)
      {
        cmac_local.stop_mode_cnfs_pending--;
        if(cmac_local.stop_mode_cnfs_pending == 0)
        cmac_stop_mode_cnf_proc();
      }
      break;

    default:
      /* AC isn't interested in other NAS reports */
      break;
  }
}

/*===========================================================================

FUNCTION cmac_client_cmd_proc

DESCRIPTION
  Process clients' commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmac_client_cmd_proc(

  cm_ac_cmd_s_type  *ac_cmd_ptr

)
{
  cm_ac_cmd_err_e_type    ac_cmd_err       = CM_AC_CMD_ERR_NOERR;

  cm_ac_cmd_info_s_type   *cmd_info_ptr    = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  */
  ac_cmd_err = cmac_client_cmd_check (ac_cmd_ptr);

  cmac_client_cmd_err( ac_cmd_ptr, ac_cmd_err );
  /*
  ** If error found, Notify client and return.
  */
  if( ac_cmd_err != CM_AC_CMD_ERR_NOERR )
  {
    return;
  }

  cmac_local.cmd_cb_func    = ac_cmd_ptr->cmd_cb_func;
  cmac_local.state_cb_func  = ac_cmd_ptr->state_cb_func;
  cmac_local.data_block_ptr = ac_cmd_ptr->data_block_ptr;
  cmac_local.cmd_type       = ac_cmd_ptr->cmd;

  cmac_local.asubs_id = ac_cmd_ptr->info.asubs_id;

  cmd_info_ptr = CMD_INFO_PTR(ac_cmd_ptr);

  switch(ac_cmd_ptr->cmd)
  {
    case CM_AC_CMD_NORMAL_OPERATION:
      cmac_restore_to_normal(ac_cmd_ptr->info.reason);
      break;

    case CM_AC_CMD_EMERGENCY_ONLY:
      cmac_restrict_to_emergency(ac_cmd_ptr->info.reason);
      break;

    default:
      CM_MSG_HIGH_1("CM AC cmd %d not handled", ac_cmd_ptr->cmd);
  }
  return;
} /* cmac_client_cmd_proc() */

/*===========================================================================

FUNCTION cmac_call_cmd_check

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmac_call_cmd_check(
  cm_call_cmd_s_type       *call_cmd_ptr
)
{

  CM_MSG_HIGH_2("cmac_call_cmd_check: AC state: %d, call_type %d", cmac_local.state, call_cmd_ptr->info.call_type);

  /* If we're in normal state then, return CM_CALL_CMD_ERR_NOERR
  ** If we're not in normal state, then check if its emergency call.
  ** If yes, return CM_CALL_CMD_ERR_NOERR
  */
  if (cmac_local.state == CMAC_STATE_NORMAL)
  {
    return CM_CALL_CMD_ERR_NOERR;
  }
  else
  {
    if (call_cmd_ptr->info.call_type == CM_CALL_TYPE_EMERGENCY)
    {
      // For E911 call, abort PPP clean. So we restore mode pref in CM and waiting
      // for 1x to send dereg failed with E911 reason.

      if(cmac_local.state != CMAC_STATE_EMERGENCY_ONLY)
      {
        CM_MSG_HIGH_0("Restor mode pref");
        cmph_delete_update_orig_mode (CM_SS_MAIN, cmac_get_act_id(), TRUE);
      }

      return CM_CALL_CMD_ERR_NOERR;
    }
    else
    {
      return CM_CALL_CMD_ERR_AC_REJECT;
    }
  }
} /* cmac_call_cmd_check */

/*===========================================================================

FUNCTION cmac_ph_cmd_proc

DESCRIPTION
  Process clients' ph commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmac_ph_cmd_proc(

  const cm_ph_cmd_s_type     *ph_cmd_ptr

)
{
  const cm_ph_cmd_info_s_type  *cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if(ph_cmd_ptr->cmd == CM_PH_CMD_OPRT_MODE)
  {
   CM_MSG_HIGH_3("ph_cmd oprt_mode %d, AC state %d, reason %d",cmd_info_ptr->oprt_mode, cmac_local.state, cmac_local.reason);
    switch(cmd_info_ptr->oprt_mode)
    {
      case SYS_OPRT_MODE_ONLINE:
        if(cmac_local.state == CMAC_STATE_EMERGENCY_PENDING)
        {
          cmac_local.state = CMAC_STATE_NORMAL;
          cmac_restrict_to_emergency(cmac_local.reason);
        }

        break;

      case SYS_OPRT_MODE_LPM:
        if(cmac_local.state == CMAC_STATE_3GPP_DETACH_PENDING ||
           cmac_local.state == CMAC_STATE_EMERGENCY_ONLY)
        {
          cmac_local.state = CMAC_STATE_EMERGENCY_PENDING;
        }

        break;

      default:
        break;
    }
  }
  else if(ph_cmd_ptr->cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
          ph_cmd_ptr->cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE ||
          ph_cmd_ptr->cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
  {
    if( !( cmac_is_in_thermal_emergency() ||
           cmac_is_in_sim_lock() )
      )
    {
      CM_MSG_HIGH_2( "AC to normal due ph cmd %d cmac reason %d",
                     ph_cmd_ptr->cmd, cmac_local.reason );
      cmac_restore_to_normal( cmac_local.reason );
    }
  }

} /* cmac_ph_cmd_proc */

/*===========================================================================

FUNCTION cmac_enter_thermal_emergency_level_2

DESCRIPTION
  Updates AC state to thermal emergency level 2
  Also ends any locally held calls
===========================================================================*/

void cmac_enter_thermal_emergency_level_2(void)
{
  cmac_set_current_state(CMAC_STATE_THERMAL_EMERG_LEVEL_2);
  cmac_local.reason = CM_AC_REASON_UNSAFE_TEMP;

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  cmcall_endall_non_emerg_calls(CM_CALL_END_THERMAL_EMERGENCY,TRUE);
  #endif
}

/*===========================================================================

FUNCTION cmac_is_reason_mask_set

DESCRIPTION
  Checks if the reason_mask for AC state machine is set
 
RETURN VALUE 
  FALSE: if reason_mask is NONE
  TRUE: otherwise
===========================================================================*/

boolean cmac_is_reason_mask_set(void)
{
  if ( cmac_local.reason_mask )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION cmaccessctrl_ptr

DESCRIPTION
  Return a pointer to the one and only access control object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmac_s_type  *cmaccessctrl_ptr( void )
{

  return &cmac_local;

} /* cmph_ptr() */
#endif
