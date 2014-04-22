/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as SVLTE, DualSim, SGLTE, etc.

EXTERNALIZED FUNCTIONS
  Functional Interface:



Copyright (c) 2013 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmmsc.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/18/14   ss      Added function to check is SS valid for PS detach 
01/30/14   ss      Conditional Reset of ue_mode_forced flag 
01/02/14   ss      Check for on-going voice or emergency calls before forcing 
                   user prefs down to MMoC in SGLTE 
11/14/13   ss      Add check to not send UE MODE seicth addl action for pref
                   reas restore
10/07/13   jvo     Replace FEATURE_UICC_RAT_INDICATOR_SUPPORT with
                   is_rat_bal flag
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
07/22/13   fj      Add support for DSDX L+G
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/03/13   dk      KW error fixed in cmmsc_get_policy_mode_band_cap,
                   cmmsc_action_local_deact, cmmsc_send_sys_sel_pref_chgd_cmd
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
02/26/13   cl      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */

#include "comdef.h"   /* Definition for basic types and macros */
#include "cmph.h"     /* Interface to CM ph functions */
#include "cmmsc.h"
#include "cmsoa.h"
#include "cmtaski.h"
#include "cmsds.h"
#include "mmoc_v.h"
#include "modem_mem.h"
#include "sys.h"      /* Common types. */
#include "rfm_device_config_type.h"
#include "sd_v.h"

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
#include "cmregprxi.h"
#include "cmss.h"
#endif

#include "cmpmprx.h"
#include "cmemgext.h"
#include "cmaccessctrl.h"
#include "cmdbg.h"

#ifdef CM_DEBUG
#error code not present
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


//static cmmsc_state_machine_s_type cmmsc_state_machine;
#if defined(FEATURE_MMODE_SC_SGLTE)
static cmmsc_routing_table_s_type sglte_route_table;
#endif

//static cmmsc_routing_table_s_type auto_route_table;

/*===========================================================================

            FUNCTION DECLARATIONS AND IMPLEMENTATION

This section contains function declaration and implementation for this
feature.

===========================================================================*/

/* Static functions
*/
#if 0
static void cmmsc_action_power_down(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine);

static void cmmsc_action_sub(
       cmmsc_param_subs_chgd_s_type *evt_pkg
);
#endif
static void cmmsc_init_mode_and_capability (
       cmmsc_state_machine_s_type  *state_machine
);


static void cmmsc_get_stack_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       cm_preference_info_s_type     *pref_info,
       cmmsc_stack_capability_s_type *common_cap_ptr
);
static void cmmsc_get_user_mode_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       sd_ss_mode_pref_e_type        *user_mode_pref
);
static boolean cmmsc_force_all_stack_required (
  cmmsc_state_machine_op_mode_e_type prev_op_mode,
  cmmsc_state_machine_op_mode_e_type new_op_mode
);
static cm_mode_band_pref_s_type  cmmsc_current_stack_preference(
  cmmsc_state_machine_s_type  *state_machine
);
#if 0
static boolean cmmsc_is_op_mode_multi_stacks (
  cmmsc_state_machine_op_mode_e_type op_mode
);

static cmmsc_event_e_type cmmsc_get_route_tbl_event(
       cmmsc_routing_table_s_type *route_table,
       int route_idx
);
static cmmsc_state_machine_op_mode_e_type cmmsc_get_route_tbl_mode(
       cmmsc_routing_table_s_type *route_table,
       int route_idx
);
static cmmsc_stack_state_e_type cmmsc_get_route_tbl_stack_state(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
);
static cmmsc_stack_state_e_type cmmsc_get_route_tbl_stack_new_state(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
);
static cmmsc_action_s_type *cmmsc_get_route_tbl_action(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
);
static boolean cmmsc_is_volte_active_on_hybr2 (
  cmmsc_state_machine_s_type  *state_machine  
);

static sys_lte_band_mask_e_type cmmsc_read_svlte_capable_lte_bands(void);

static void cmmsc_action_suspend_stack(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine);

static void cmmsc_action_resume_stack(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine);
#endif

/****************************************************************************
**    Function definitions
****************************************************************************/
  cmmsc_state_machine_s_type     *cmmsc_state_machine_ptr(
    void
    )
  {
    static cmmsc_state_machine_s_type  msc_machine;
    return &msc_machine;
  }
cmmsc_state_machine_s_type     *cmmsimc_state_machine_ptr(
  sys_modem_as_id_e_type        as_id
)
{
  return cmmsc_state_machine_ptr();
}

/*===========================================================================

FUNCTION cmmsc_init

DESCRIPTION
  This function initialized the available routing tables with proper data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init( void )
{

  cmmsc_state_machine_init(cmmsc_state_machine_ptr());
}

/*===========================================================================

FUNCTION cmmsc_state_machine_init

DESCRIPTION
  This function should be called to initialize the whole state machine

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_state_machine_init(cmmsc_state_machine_s_type *state_machine)
{
  if(state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  /* Initialize the stack common info */
  cmmsc_init_stack_common(state_machine);

  /* Initialize the phone capability to default values */
  cmmsc_init_mode_and_capability(state_machine);
}
/*===========================================================================

FUNCTION cmmsc_init_mode_and_capability

DESCRIPTION
  Initialize the phone capability to default values.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmmsc_init_mode_and_capability (cmmsc_state_machine_s_type  *state_machine)
{
  state_machine->stack_common_info.ue_band_capability     = SD_SS_BAND_PREF_ANY;
  state_machine->stack_common_info.ue_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
  state_machine->stack_common_info.ue_tds_band_capability = SD_SS_BAND_PREF_ANY;
  state_machine->stack_common_info.ue_mode_capability     = cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ALL_MODES);
  state_machine->stack_common_info.ue_lte_m1_band_capability = SYS_LTE_M1_BAND_MASK_CONST_ANY;
  state_machine->stack_common_info.ue_lte_nb1_band_capability = SYS_LTE_NB1_BAND_MASK_CONST_ANY;
  state_machine->stack_common_info.ue_max_mode_capability = cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ALL_MODES);

}

/*===========================================================================

FUNCTION cmmsc_get_policy_mode_band_cap

DESCRIPTION
  Retrieve the Mode and Band capability from PMPRX module.
  fetch the call mode capability per subs and store that in stack_common_info as well.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_get_policy_mode_band_cap (
     cmmsc_state_machine_s_type  *state_machine,
     sys_sys_mode_mask_e_type    *mode,
     sys_band_mask_e_type        *band,
     sys_band_mask_e_type        *tds_band,
     sys_lte_band_mask_e_type        *lte_band,
     sys_lte_band_mask_e_type        *lte_m1_band,
     sys_lte_band_mask_e_type        *lte_nb1_band
)
{
  boolean is_force_reg = FALSE;
  #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  //sys_call_mode_t          call_mode_buffer;
  #endif

  CM_ASSERT(mode != NULL);
  CM_ASSERT(band != NULL);
  CM_ASSERT(tds_band != NULL);
  CM_ASSERT(lte_band != NULL);

  if (mode == NULL || band == NULL || tds_band == NULL || lte_band == NULL ||
      state_machine == NULL)
  {
    CM_ERR_3("MSC: band null ptr, band %d, tds_band %d, lte_band %d",
            band,tds_band,lte_band);
    return;
  }

  if(!cmpmprx_read_cm_policy_consumption_eligibility())
  {
    CM_MSG_HIGH_0("MSC: Not eligible to read the new policy so send default value ");
    state_machine->stack_common_info.ue_mode_capability 
      = *mode = cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ALL_MODES);
    state_machine->stack_common_info.ue_band_capability = *band = SYS_BAND_MASK_ANY;
    state_machine->stack_common_info.ue_lte_band_capability = *lte_band = SYS_LTE_BAND_MASK_CONST_ANY;
    state_machine->stack_common_info.ue_lte_m1_band_capability = *lte_m1_band = SYS_LTE_M1_BAND_MASK_CONST_ANY;
    state_machine->stack_common_info.ue_lte_nb1_band_capability = *lte_nb1_band = SYS_LTE_NB1_BAND_MASK_CONST_ANY;
    return;
  }

  if( cmpmprx_fetch_mode_capability(mode) == TRUE )
  {
    state_machine->stack_common_info.ue_mode_capability = *mode;
  }
  else
  {
    *mode = state_machine->stack_common_info.ue_mode_capability;
  }

  if( cmpmprx_get_band_capability( band,
                                   lte_band,
                                   tds_band,
                                   &is_force_reg,
                                   lte_m1_band,
                                   lte_nb1_band) == TRUE )
  {
    state_machine->stack_common_info.ue_band_capability = *band;
    state_machine->stack_common_info.ue_lte_band_capability = *lte_band;
    state_machine->stack_common_info.ue_tds_band_capability = *tds_band;
    state_machine->stack_common_info.ue_lte_m1_band_capability = *lte_m1_band = SYS_LTE_M1_BAND_MASK_CONST_ANY;
    state_machine->stack_common_info.ue_lte_nb1_band_capability = *lte_nb1_band = SYS_LTE_NB1_BAND_MASK_CONST_ANY;
  }
  else
  {
    *band     = state_machine->stack_common_info.ue_band_capability;
    *lte_band = state_machine->stack_common_info.ue_lte_band_capability;
    *lte_m1_band =  state_machine->stack_common_info.ue_lte_m1_band_capability;
    *lte_nb1_band =  state_machine->stack_common_info.ue_lte_nb1_band_capability;
    *tds_band = state_machine->stack_common_info.ue_tds_band_capability;
  }

}

/*===========================================================================

FUNCTION cmmsc_set_stack_capability_auto

DESCRIPTION
  This function set stack capability for auto.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_auto(
    cmmsc_state_machine_s_type  *state_machine
)
{
  sys_sys_mode_mask_e_type  pm_mode    = SYS_SYS_MODE_MASK_NONE;
  sd_ss_mode_pref_e_type    mode       = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type    band       = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type    tds_band   = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type  lte_band   = SYS_LTE_BAND_MASK_CONST_NONE;
  sys_lte_band_mask_e_type    lte_m1_band   = SYS_LTE_BAND_MASK_CONST_NONE;
  sys_lte_band_mask_e_type    lte_nb1_band   = SYS_LTE_BAND_MASK_CONST_NONE;

  /* Get PM capability */
  cmmsc_get_policy_mode_band_cap(state_machine,
                                 &pm_mode,
                                 &band,
                                 &tds_band,
                                 &lte_band,
                                 &lte_m1_band,
                                 &lte_nb1_band);

  mode = cmph_map_sys_mode_mask_to_sd_mode_pref(pm_mode);

  state_machine->stack[0].prev_registration_capability = state_machine->stack[0].capablity;
  
  /* Set other capabilities
  */
  state_machine->stack[0].capablity.mode     = mode;
  state_machine->stack[0].capablity.domain   = SD_SS_SRV_DOMAIN_PREF_CS_PS;
  state_machine->stack[0].capablity.band     = band;
  state_machine->stack[0].capablity.tds_band = tds_band;
  state_machine->stack[0].capablity.lte_band = lte_band;
  state_machine->stack[0].capablity.lte_m1_band = lte_m1_band;
  state_machine->stack[0].capablity.lte_nb1_band = lte_nb1_band;
}
#ifdef FEATURE_MMODE_EMERG_SUPPORT
/*===========================================================================

FUNCTION cmmsc_set_stack_capability_auto_emergency

DESCRIPTION
  This function set stack capability for auto in emergency call scenarios.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_auto_emergency(
    cmmsc_state_machine_s_type  *state_machine
)
{
  size_t                    index = 0;

  /* For emergency call, enable all capability per PM max mode cap for stack 0 */
  state_machine->stack[0].capablity.mode = SD_SS_MODE_PREF_ANY;

  state_machine->stack[0].capablity.domain    = SD_SS_SRV_DOMAIN_PREF_CS_PS;
  state_machine->stack[0].capablity.band      = SD_SS_BAND_PREF_ANY;
  state_machine->stack[0].capablity.tds_band  = SD_SS_BAND_PREF_TDS_ANY;
  state_machine->stack[0].capablity.lte_band  = SYS_LTE_BAND_MASK_CONST_ANY;
  state_machine->stack[0].capablity.lte_m1_band  = SYS_LTE_M1_BAND_MASK_CONST_ANY;
  state_machine->stack[0].capablity.lte_nb1_band  = SYS_LTE_NB1_BAND_MASK_CONST_ANY;
}
#endif

/*===========================================================================

FUNCTION cmmsc_set_stack_capability

DESCRIPTION
  This function set stack capability. This function is called when we
  power-up or send subscriptions. Depending on which harware , we set
  different capability.
  For SGLTE, the total capability needs to come from PM, however, this
  function will split the capabilities to individual stacks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability( cmmsc_state_machine_s_type   *state_machine )
{
  sys_subs_feature_t  feature_mode = SYS_SUBS_FEATURE_MODE_NORMAL ;

  #ifdef FEATURE_MMODE_EMERG_SUPPORT
  if( state_machine->stack_common_info.is_in_emergency_call )
  {
    cmmsc_set_stack_capability_auto_emergency(state_machine);
  }
  else
  #endif
  {
    cmmsc_set_stack_capability_auto(state_machine);
  }
}

/*===========================================================================

FUNCTION  cmmsc_pack_cmd_for_pref_sys_chgd_per_top_queue

DESCRIPTION
  Pack the param for cmd_pref_sys_chgd base on the preference of the top priority
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_pack_cmd_for_pref_sys_chgd_per_top_queue(

        cm_ss_e_type                          ss,
            /* Indicates the ss this pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type                pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type                orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        boolean                               is_new_policy_tobe_read,
            /* Indicate if to update the policy */

        const cm_orig_q_s_type               *ph_orig_top_ptr,
           /* preference of the top priority queue.
           */

        cm_act_update_reas_e_type             act_update_reas,
           /* Reason for the update
           */

        cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cm_preference_info_s_type  *pref_info = NULL;
  sys_modem_as_id_e_type      asubs_id = cmph_map_cm_ss_to_subs(ss);

  if (cmd == NULL)
  {
    CM_ERR_0("cmd ptr null");
    return;
  }

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("queue_top_ptr null");
    return;
  }

  cmd->cm_ss = ss;
  cmd->is_new_policy_tobe_read = is_new_policy_tobe_read;

  /* Allocate memory for preference info */
  pref_info = (cm_preference_info_s_type*) cm_mem_malloc(
                          sizeof(cm_preference_info_s_type));
  cmd->pref_info = pref_info;

  /* Set preference base on the input params */
  pref_info->pref_reas = pref_reas;
  pref_info->orig_mode = orig_mode;
  pref_info->act_update_reas = act_update_reas;

  /* Set preference base on default values */
  pref_info->avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info->avoid_time = 0;

  /* Set preference base on top priority queue */
  pref_info->mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  pref_info->band_pref = ph_orig_top_ptr->orig->orig_band_pref;
  pref_info->lte_band_pref = ph_orig_top_ptr->orig->orig_lte_band_pref;
  pref_info->tds_band_pref = ph_orig_top_ptr->orig->orig_tds_band_pref;
  pref_info->hybr_pref = ph_orig_top_ptr->orig->orig_hybr_pref;
  pref_info->prl_pref = ph_orig_top_ptr->orig->orig_prl_pref;
  pref_info->roam_pref = ph_orig_top_ptr->orig->orig_roam_pref;
  pref_info->srv_domain_pref = ph_orig_top_ptr->orig->orig_srv_domain_pref;
  #ifndef FEATURE_MMODE_DISABLE_CSG
  pref_info->csg_id = ph_orig_top_ptr->orig->csg_id;
  #endif
  pref_info->csg_rat = ph_orig_top_ptr->orig->csg_rat;
  /* Set preference base on ph preference - those preference are not part of priority queue */
  pref_info->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, cmph_ptr());
  pref_info->camp_mode_pref
    = cmph_map_cm_net_sel_mode_pref_to_camp_pref(
       pref_info->network_sel_mode_pref);
  pref_info->rat_acq_order_ptr
    = cmph_get_acq_pri_order_per_sub(cmph_map_subs_to_ss(asubs_id));
  pref_info->acq_order_pref = CMPH_SS_ACQ_ORDER_PREF(asubs_id, cmph_ptr());
  pref_info->sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info->sys_id.sys_id.id.plmn  = CMPH_SS_PLMN_PREF(asubs_id, cmph_ptr());
  pref_info->sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  pref_info->hybr1_pref.hybr_mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  pref_info->hybr1_pref.hybr_orig_mode = orig_mode;

  /* retrieve if there is any CMSOA pref_update_reason  */
  pref_info->pref_update_reason = ph_orig_top_ptr->orig->pref_update_reason;
  pref_info->ciot_lte_pref      = ph_orig_top_ptr->orig->ciot_lte_pref;
  CM_MSG_HIGH_1("phobj pref_update_reason %d",ph_orig_top_ptr->orig->pref_update_reason);  
}

/*===========================================================================

FUNCTION  cmmsc_pack_cmd_for_pref_sys_chgd_per_ph_pref

DESCRIPTION
  Pack the param for cmd_pref_sys_chgd base on the phone preference.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_pack_cmd_for_pref_sys_chgd_per_ph_pref(

        cm_ss_e_type                          ss,
            /* Indicates the ss this pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type                pref_reas,
            /* Reason for changing the SS-Preference.
            */

        boolean                               is_new_policy_tobe_read,
            /* Indicate if to update the policy */

        cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cm_preference_info_s_type  *pref_info = NULL;
  sys_modem_as_id_e_type      asubs_id = cmph_map_cm_ss_to_subs(ss);
  cmph_s_type                 *ph_ptr = cmph_ptr();

  if (cmd == NULL)
  {
    CM_ERR_0("cmd ptr null");
    return;
  }

  cmd->cm_ss = ss;
  cmd->is_new_policy_tobe_read = is_new_policy_tobe_read;

  /* Allocate memory for preference info */
  pref_info = (cm_preference_info_s_type*) cm_mem_malloc(
                          sizeof(cm_preference_info_s_type));
  cmd->pref_info = pref_info;

  /* Set preference base on the input params */
  pref_info->pref_reas = pref_reas;

  /* Set preference base on phone pref */
  pref_info->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, ph_ptr);
  pref_info->orig_mode = cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                      pref_info->network_sel_mode_pref);
  pref_info->mode_pref = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);

  pref_info->hybr1_pref.hybr_mode_pref = pref_info->mode_pref;
  pref_info->hybr1_pref.hybr_orig_mode = pref_info->orig_mode;

  pref_info->hybr_pref = CMPH_SS_HYBR_PREF(asubs_id, ph_ptr);
  pref_info->band_pref = CMPH_SS_BAND_PREF(asubs_id, ph_ptr);
  pref_info->lte_band_pref = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
  pref_info->tds_band_pref = CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr);
  pref_info->prl_pref = CMPH_SS_PRL_PREF(asubs_id, ph_ptr);
  pref_info->roam_pref = CMPH_SS_ROAM_PREF(asubs_id, ph_ptr);
  pref_info->srv_domain_pref = CMPH_SS_SRV_DOMAIN_PREF(asubs_id, ph_ptr);
  pref_info->acq_order_pref = CMPH_SS_ACQ_ORDER_PREF(asubs_id, ph_ptr);
  pref_info->rat_acq_order_ptr
    = cmph_get_acq_pri_order_per_sub(cmph_map_subs_to_ss(asubs_id));
  pref_info->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, ph_ptr);
  pref_info->camp_mode_pref = cmph_map_cm_net_sel_mode_pref_to_camp_pref(
       pref_info->network_sel_mode_pref);
  pref_info->sys_id.sys_id.id.plmn = CMPH_SS_PLMN_PREF(asubs_id, ph_ptr);
  pref_info->sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info->sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  /* Set preference base on default values */
  pref_info->avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info->avoid_time = 0;
  pref_info->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  pref_info->ciot_lte_pref.lte_op_mode_pref = CMPH_SS_LTE_IOT_OP_MODE(asubs_id, ph_ptr);
  pref_info->ciot_lte_pref.lte_m1_band_pref = CMPH_SS_LTE_M1_BAND_PREF(asubs_id, ph_ptr);
  pref_info->ciot_lte_pref.lte_nb1_band_pref = CMPH_SS_LTE_M1_BAND_PREF(asubs_id, ph_ptr);
  pref_info->ciot_lte_pref.lte_band_pref = CMPH_SS_LTE_WB_BAND_PREF(asubs_id, ph_ptr);
}


/*===========================================================================

FUNCTION  cmmsc_pack_cmd_for_pref_sys_chgd_per_pref

DESCRIPTION
  Pack the param for cmd_pref_sys_chgd base on the given preference .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_pack_cmd_for_pref_sys_chgd_per_pref(

    cm_ss_e_type                          ss,
        /* Indicates the ss this pref_sys_chgd command is intended for.
        */

    sd_ss_pref_reas_e_type                pref_reas,
        /* Reason for changing the SS-Preference.
        */

    sd_ss_orig_mode_e_type                orig_mode,
        /* The origination mode (normal, OTASP, emergency, other, etc.)
        ** that is associated with the selected NAM.
        */

    const cm_pref_s_type                 *ss_pref,
       /* Stack preference that is associated with the selected NAM.
       */

    sd_band_e_type                        otasp_band,
       /* The band-class that is associated with OTASP origination mode.
       ** Note that this parameter is ignored if orig_mode != OTASP.
       */

    sd_blksys_e_type                      otasp_blksys,
      /* The PCS frequency block/Cellular system that is associated
      ** with OTASP origination mode. Note that this parameter is ignored
      ** if orig_mode != OTASP.
      */

    sd_ss_avoid_sys_e_type                avoid_type,
      /* Type of the system to be avoided, valid only
      ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
      */

    dword                                 avoid_time,
      /* Time in seconds for which the system is to be avoided, valid
      ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
      */
    const cm_acq_pri_order_pref_s_type   *rat_acq_order_ptr,
       /* Rat acquisition priority order
       */

    sd_ss_pref_camp_mode_e_type           camp_mode_pref,
       /* camp only pref
       */

    boolean                               is_new_policy_tobe_read,

    cm_act_update_reas_e_type             act_update_reas,
    /* Reason for the update
    */

    cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cm_preference_info_s_type  *pref_info = NULL;

  if (cmd == NULL)
  {
    CM_ERR_0("cmd ptr null");
    return;
  }

  cmd->cm_ss = ss;
  cmd->is_new_policy_tobe_read = is_new_policy_tobe_read;

  /* Allocate memory for preference info */
  pref_info = (cm_preference_info_s_type*) cm_mem_malloc(
                          sizeof(cm_preference_info_s_type));
  cmd->pref_info = pref_info;

  pref_info->pref_reas  = pref_reas;
  pref_info->orig_mode  = orig_mode;
  pref_info->avoid_type   = avoid_type;
  pref_info->avoid_time   = avoid_time;
  pref_info->rat_acq_order_ptr = rat_acq_order_ptr;
  pref_info->camp_mode_pref    = camp_mode_pref;
  pref_info->act_update_reas   = act_update_reas;

  if (ss_pref)
  {
    pref_info->pref_update_reason = ss_pref->pref_update_reason;
    pref_info->mode_pref = ss_pref->mode_pref;
    pref_info->band_pref = ss_pref->band_pref;
    pref_info->lte_band_pref = ss_pref->lte_band_pref;
    pref_info->tds_band_pref = ss_pref->tds_band_pref;
    pref_info->hybr_pref = ss_pref->hybr_pref;
    pref_info->prl_pref = ss_pref->prl_pref;
    pref_info->roam_pref = ss_pref->roam_pref;
    pref_info->acq_order_pref = ss_pref->acq_order_pref;
    pref_info->srv_domain_pref = ss_pref->srv_domain_pref;
    pref_info->network_sel_mode_pref = ss_pref->network_sel_mode_pref;
    pref_info->sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
    pref_info->sys_id.sys_id.id.plmn  = ss_pref->plmn;
    pref_info->sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
    pref_info->hybr1_pref.hybr_mode_pref = ss_pref->mode_pref;
    pref_info->ciot_lte_pref             = ss_pref->ciot_lte_pref;
  }

  pref_info->hybr1_pref.hybr_orig_mode = orig_mode;

}


/*===========================================================================

FUNCTION cmmsc_proc_cmd_oprt_mode_chgd

DESCRIPTION

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_cmd_oprt_mode_chgd
(
    sys_oprt_mode_e_type              oprt_mode
   /* New operating mode of the phone. */
)
{

  cmmsc_state_machine_s_type   *state_machine;
  cmph_s_type                   *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Terminate any pending get network request. */
  cmph_term_pending_get_networks_req_client_only(CM_SS_MAX);

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Setting stacks capability */
  
  cmmsc_set_stack_capability(state_machine);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}

cm_mode_band_pref_s_type  cmmsc_current_stack_preference(
  cmmsc_state_machine_s_type  *state_machine
)
{
  
  cm_mode_band_pref_s_type               stack_pref;
  
  if(state_machine == NULL)
  {
    CM_ERR_FATAL_0("state machine NULL");
  }
  else
  {
  
  /* Update preference for that stack */
    stack_pref.mode_pref      =  state_machine->stack_common_info.preference.mode_pref;
    stack_pref.band_pref      =  state_machine->stack_common_info.preference.band_pref;
    stack_pref.lte_band_pref  =  state_machine->stack_common_info.preference.lte_band_pref;
    stack_pref.tds_band_pref  =  state_machine->stack_common_info.preference.tds_band_pref;
    stack_pref.lte_m1_band_pref = state_machine->stack_common_info.preference.lte_m1_band_pref;
    stack_pref.lte_nb1_band_pref = state_machine->stack_common_info.preference.lte_nb1_band_pref;

  }
 
  return stack_pref;
  
}

/*===========================================================================

FUNCTION cmmsc_proc_cmd_pref_sys_chgd

DESCRIPTION
  This function should be called when the phone preferences have changed.
  This module will be responsible for queing pref_sys_chgd command to MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_proc_cmd_pref_sys_chgd
(
  cmmsc_proc_cmd_pref_sys_chgd_s_type * cmd
)
{
  cmmsc_state_machine_s_type           *state_machine = NULL;

  boolean                             is_new_policy_tobe_read = 0;
  boolean                             is_emerg_call_actv = FALSE;
  sd_ss_pref_camp_mode_e_type         camp_mode_pref = SD_SS_PREF_CAMP_MODE_NONE;
  cm_preference_info_s_type           *pref_info = NULL;
  cm_ss_e_type                        cm_ss;
  int                                 cm_stack_id = 0;
  sd_ss_orig_mode_e_type              orig_mode = SD_SS_ORIG_MODE_NONE;
  #if !defined(FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_CIOT)
  cm_mode_band_pref_s_type            old_stack_pref;
  #endif
  cm_orig_q_s_type                    *ph_orig_top_ptr     = NULL;
  /* Stack preference that is associated with the selected NAM.
  */
  cmph_s_type                        *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate parameters */
  if(cmd == NULL)
    sys_err_fatal_null_ptr_exception();

  state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd == NULL || state_machine == NULL)
    sys_err_fatal_null_ptr_exception();

  /* Get the params from the cmd */
  is_new_policy_tobe_read = cmd->is_new_policy_tobe_read;
  cm_ss = cmd->cm_ss;

  pref_info = cmd->pref_info;
  if (pref_info == NULL)
  {
    CM_ERR_0("pref info null ptr");
    return;
  }

  CM_MSG_HIGH_3("pref_chgd: MSC->MMOC cm_ss=%d, pref mode_pref=%d, is_new_policy_tobe_read=%d", 
                   cm_ss, pref_info->mode_pref, is_new_policy_tobe_read);
  #if defined(FEATURE_MMODE_SC_SGLTE)
  if ( !cmmsc_is_ok_to_force_prefs( pref_info->pref_reas,
                                    pref_info->act_update_reas,
                                    state_machine ) )
  {
    CM_MSG_HIGH_0( "MSC: Voice Call in progress. Don't force user prefs");
    return;
  }
  #endif

  /* Check if ss is valid per op mode and hybr pref */
  #ifdef FEATURE_MMODE_3GPP2
  hybr_pref = pref_info->hybr_pref;
  if (!cmmsc_is_valid_ss_per_op_mode(state_machine, cm_stack_id,hybr_pref))
    return;
  #endif
  
  camp_mode_pref = pref_info->camp_mode_pref;
  orig_mode = pref_info->orig_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For SGLTE, Policy Manager will set the UE-Mode
  ** For SVLTE, we might have to check and determine the UE by CM.
  */
  /* Save the previous stack prefernce. This will be used later to determine PS_DETACH */
  #if !defined(FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_CIOT)
  old_stack_pref = cmmsc_current_stack_preference(state_machine);
  #endif

  /****************************************************
  ** SGLTE: Read UE mode if PM indicates UE changed
  ** SVLTE: Determine UE mode
  *****************************************************/
  
  {
    if( is_new_policy_tobe_read || cm_ss == CM_SS_MAX)
    {
      cmmsc_set_stack_capability(state_machine);
    }

    #ifdef FEATURE_MMODE_EMERG_SUPPORT
    /* Set capability during entire duration of of emerg call */
    is_emerg_call_actv = cmcall_emergency_call_active();
    
    if(is_emerg_call_actv || 
        (state_machine->stack_common_info.is_in_emergency_call != is_emerg_call_actv))
    {
      state_machine->stack_common_info.is_in_emergency_call = is_emerg_call_actv;

      /* If emerg orig req is on a specific SS, then don't do special mode setting */
      if(! (is_emerg_call_actv && cmmsc_is_msc_multi_stacks(state_machine->asubs_id)))
      {
        cmmsc_set_stack_capability(state_machine);
      }
    }
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For operation mode LPM/FTM, we also allow sys_sel_pref to go through
     Override pref_reas to update SD pref but not invoke SD's engine */
  if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE ||
      ph_ptr->oprt_mode_send_time != CMPH_MAX_UPTIME)
  {
    pref_info->pref_reas = SD_SS_PREF_REAS_RESTORE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If ss is not active, force pref_reason to restore */
  #if defined(FEATURE_MMODE_DUAL_SIM)
  if (cmph_is_msim()
    && !(ph_ptr->active_subs & BM(state_machine->asubs_id))
    && pref_info->pref_reas != SD_SS_PREF_REAS_ORIG_END)
  {
    pref_info->pref_reas = SD_SS_PREF_REAS_RESTORE;
  }
  #endif
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* If preference changed on HYBR-2 because of emergency call and CMMSC was
  ** not in emergency call before then
  ** - Update the state to be in emergency
  ** If we're in Dual-stack then:
  ** - Force preference on both stacks to shutdown main.
  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* If UE moving from CSFB --> SGLTE, and UE was doing Manual Selection on
  ** MAIN, then we have to make sure UE doing Manual Selection on Hybr-2 also.
  */
  /* Get the top of the priority queue */
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("MSC: ph_orig_top_ptr null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For SGLTE, if UE_MODE has been changed which triggers capability changed,
  ** then CM has to perform ps_detach.
  ** For SVLTE, preference is constant and capability remains the same unless
  ** preference changed, hence CM don't need to do explicit detach.
  */
  /* PS_DETACH request is sent to MMOC if top of queue is either CM_ACT_TYPE_PH_OBJ 
     or CM_ACT_TYPE_DATA_CALL. So evaluation also restricted to that 
   */
  #if !defined(FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_CIOT)
  if( is_new_policy_tobe_read && ph_ptr->is_ps_detach_required == FALSE &&
    (ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ || 
     ph_orig_top_ptr->act_type == CM_ACT_TYPE_DATA_CALL))
  {
    cmmsc_evaluate_ps_detach_required_sglte(state_machine,cmmsc_mapping_ss_to_stack_idx(cm_ss),old_stack_pref);
  }
  #endif
  
  cmmsc_update_stack_preference(
    state_machine,
    pref_info->mode_pref,
    pref_info->band_pref,
    pref_info->lte_band_pref,
    pref_info->tds_band_pref,
    pref_info->ciot_lte_pref.lte_m1_band_pref,
    pref_info->ciot_lte_pref.lte_nb1_band_pref);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if all the conditions meet, restore 3GPP back to phone object 
  */
  #if defined(FEATURE_MMODE_3GPP2)
  if( is_new_policy_tobe_read )
  {
    cmmsc_auto_restore_3gpp_policy_chgd(state_machine, cm_ss,
                                          &(pref_info->mode_pref));
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_HDR)
  cmmsc_auto_set_hybr1_pref(state_machine, old_op_mode, pref_info);
  #endif
  
  cmmsc_send_sys_sel_pref_chgd_cmd(state_machine,
                                   cm_stack_id, pref_info, NULL);

  cm_mem_free(pref_info);
  cmd->pref_info = NULL;
}

/*===========================================================================

FUNCTION cmmsc_mapping_ss_to_stack_idx

DESCRIPTION
  The function maps cm ss to stack index.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
int cmmsc_mapping_ss_to_stack_idx(cm_ss_e_type ss)
{
  return 0;
}
/*===========================================================================

  This function returns a boolean value:
  TRUE : If stack pointed by ss is active and being attempted
  FALSE: If stack is not active and no service acquisition is taking place
  
===========================================================================*/
static boolean cmmsc_cmss_is_operational(

  cm_ss_e_type ss

)
{
  return TRUE;
}

/*===========================================================================

FUNCTION cmmsc_init_stack_common

DESCRIPTION
  This function should be called to initialize stack common info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_stack_common(cmmsc_state_machine_s_type  *state_machine)
{
  if(state_machine == NULL)
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state_machine->stack_common_info.is_in_emergency_call = FALSE;
  #ifdef FEATURE_MMODE_3GPP2
  state_machine->stack_common_info.call_mode = SYS_CALL_MODE_NORMAL;
  sd_ss_update_call_mode(cmph_map_subs_to_sd_ss(state_machine->asubs_id),SYS_CALL_MODE_NORMAL);
  #endif


  state_machine->stack_common_info.preference.mode_pref = CMPH_PRST_SS_MODE_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.band_pref = CMPH_PRST_SS_BAND_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.tds_band_pref = CMPH_PRST_SS_TDS_BAND_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.lte_band_pref = CMPH_PRST_SS_LTE_BAND_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.lte_m1_band_pref = CMPH_PRST_SS_LTE_M1_BAND_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.lte_nb1_band_pref = CMPH_PRST_SS_LTE_NB1_BAND_PREF(state_machine->asubs_id, cmph_ptr());
}

void    cmmsc_send_sys_sel_pref_chgd_cmd(
  cmmsc_state_machine_s_type                 *state_machine,
  int                                         stack_idx,
  cm_preference_info_s_type                  *pref_info,
  const cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type   *addl_ptr
)
{
  cmmsc_stack_s_type              *stack_pointer;
  sd_ss_pref_update_reason_e_type  pref_update_reason  =
                                  SD_SS_PREF_UPDATE_REASON_NONE;
  sys_addtl_action_s_type         addl_action;
  uint16                          sys_sel_pref_req_id = 0;
  sys_csg_id_type                 csg_id              = SYS_CSG_ID_INVALID;
  sys_radio_access_tech_e_type    csg_rat             = SYS_RAT_NONE;
  cm_ss_e_type                    ss                  = CM_SS_MAIN;
  cm_orig_q_s_type               *ph_orig_top_ptr     = NULL;
  cmph_s_type                    *ph_ptr              = cmph_ptr();
  cmmsc_stack_capability_s_type   common_pref;
  sd_ss_mode_pref_e_type          user_mode_pref      = SD_SS_MODE_PREF_ANY;
  sd_rat_acq_order_s_type         sd_rat_acq_order;
  sd_ss_hybr_pref_e_type          internal_hybr_pref  = SD_SS_HYBR_PREF_NONE;

  sd_ss_pref_reas_e_type          pref_reason;
  sd_ss_pref_camp_mode_e_type     camp_mode;
  sd_ss_orig_mode_e_type          orig_mode = SD_SS_ORIG_MODE_NONE;
  sd_ss_ciot_lte_pref_s_type      ciot_lte_pref;

  addl_action.action = (uint8)MMOC_PREF_SYS_ADDL_ACTION_NONE;
  addl_action.ue_mode = SYS_UE_MODE_NONE;
  addl_action.is_ue_mode_substate_srlte = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pref_info == NULL)
  {
    CM_ERR_0("MSC: pref info null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the correct preference pointer to the correct stack */

  stack_pointer = &state_machine->stack[stack_idx];
  /* Get the top of the priority queue */
  ph_orig_top_ptr = cmtask_orig_para_get_top(ss);

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("MSC: ph_orig_top_ptr null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get pref info */

  pref_reason = pref_info->pref_reas;
  camp_mode = pref_info->camp_mode_pref;
  sd_rat_acq_order =
     cmph_map_cm_rat_acq_order_pref_to_sd (pref_info->rat_acq_order_ptr);

  orig_mode = pref_info->orig_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Terminate the pending get networks request. */

  if( pref_reason == SD_SS_PREF_REAS_ORIG_START_CS ||
      pref_reason == SD_SS_PREF_REAS_ORIG_START_PS ||
      pref_reason == SD_SS_PREF_REAS_ORIG_RESUME   ||
      pref_reason == SD_SS_PREF_REAS_REDIAL_NORM   ||
      pref_reason == SD_SS_PREF_REAS_REDIAL_OPTI   ||
      pref_reason == SD_SS_PREF_REAS_USER          ||
      pref_reason == SD_SS_PREF_REAS_USER_RESEL
    )
  {
    cmph_term_pending_get_networks_req(ss);
	#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
	if(ph_ptr->is_gps_search_high_priority == TRUE)
	{
	  ph_ptr->is_gps_search_mplmn_abort_event_sent = TRUE;
	}
	#endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update csg_id & csg_rat */

  if( ph_orig_top_ptr->orig != NULL )
  {
    sys_sel_pref_req_id = ph_orig_top_ptr->orig->sys_sel_pref_req_id;
    #ifndef FEATURE_MMODE_DISABLE_CSG
    csg_id = ph_orig_top_ptr->orig->csg_id;
    #endif
    csg_rat = ph_orig_top_ptr->orig->csg_rat;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update additional action */

  if( (cmph_ptr()->is_ps_detach_required == TRUE)
       &&
      (ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ ||
       ph_orig_top_ptr->act_type == CM_ACT_TYPE_DATA_CALL || 
       ph_orig_top_ptr->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT)
    )
  {
    addl_action.action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_PS_DETACH;
    ph_ptr->is_ps_detach_required = FALSE;
    CM_MSG_HIGH_1 ("MSC: addl_action %d", addl_action.action);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set pref_update reason */
  
  #if defined(CM_GW_SUPPORTED) && defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_GPRS_LTE)
  /* Set EF_RAT variables */
  if (ph_ptr->is_rat_bal == TRUE &&
            ph_ptr->is_mode_change_due_to_ef_rat == TRUE && 
            ss != CM_SS_HYBR_1 )
  {
    pref_update_reason =  SD_SS_PREF_UPDATE_REASON_EF_RAT_MODE_CHG;
    /* Reset the flag */
    ph_ptr->is_mode_change_due_to_ef_rat = FALSE;
    CM_MSG_MED_0 ("MSC: hybr2/3:EF_RAT_MODE_CHG set FALSE");
  }
  #endif /* CM_GW_SUPPORTED */
  

  /* Update pref reason as AC_EMERG when device in AC EMERG state
  ** to overide checks in SD for emergency preferences*/
  #if !defined(FEATURE_MMODE_DISABLE_ACCESS_CONTROL)
  if(pref_update_reason == SD_SS_PREF_UPDATE_REASON_NONE &&
    cmac_is_in_emergency_only())
  {
    pref_update_reason = SD_SS_PREF_UPDATE_REASON_AC_EMERG;
  }
  #endif

  pref_info->srv_domain_pref = cmtask_compute_srv_domain_pref(ss);
  #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  
  /* For emergency call make user mode preference as EMERGENCY so that SD
  ** can look for service on all possible RATs
  */
  if ( (pref_info->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG &&
       ph_orig_top_ptr->act_type != CM_ACT_TYPE_POLICYMAN_RESTRICT)||
       pref_info->orig_mode == SD_SS_ORIG_MODE_EMERG_CB    
      )
  { 
    /* For emergency call if domain pref is CS only, CM need not to add LTE in mode pref to SD */
    if( pref_info->srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_ONLY )
    {
       CM_MSG_HIGH_0("MSC: Remove LTE as pref_info->srv_domain_pref is CS only ");
       user_mode_pref = SD_SS_MODE_PREF_ANY_BUT_LTE;
    }
    /* If LTE emergency call is not possible and user mode pref did not already include LTE,
       don't add LTE to the user mode pref */ 
    #if (( defined(FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined (FEATURE_IP_CALL) )\
        && defined FEATURE_LTE && !defined (FEATURE_MMODE_DISABLE_LTE_VOICE))
    else if( !cmsds_is_LTE_emerg_call_possible(SYS_MODEM_AS_ID_1) && 
             !cmcall_misc_is_mode_pref( cmph_get_ss_mode_pref(ss), CM_MODE_PREF_LTE_ONLY ) 
        )
    {      
      CM_MSG_HIGH_0("MSC: Remove LTE as pref_info->srv_domain_pref is CS only ");
      user_mode_pref = SD_SS_MODE_PREF_ANY_BUT_LTE;
    }
    #endif
    else 
    {
      user_mode_pref = SD_SS_MODE_PREF_ANY;
    }
  }
  else
  #endif
  {
    if (ph_orig_top_ptr->orig !=NULL)

    {
      user_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                       ph_orig_top_ptr->orig->user_mode_pref);
    }
    else
    {
      CM_ERR_1("top element in the priority queue of stack %d is NULL",ss);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Filter mode, band, and domain */
  cmmsc_get_stack_pref_per_cap(state_machine, stack_idx, pref_info, &common_pref);
  cmmsc_get_user_mode_pref_per_cap(state_machine, stack_idx, &user_mode_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the addl info from addl_ptr */

  if (addl_ptr && addl_ptr->pref_update_reas)
  {
    pref_update_reason = *addl_ptr->pref_update_reas;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_CIOT)
  ciot_lte_pref.lte_op_mode_pref = cmph_map_cm_lte_op_mode_to_sd_lte_op_mode(pref_info->ciot_lte_pref.lte_op_mode_pref);
  ciot_lte_pref.lte_m1_band_pref = common_pref.lte_m1_band;
  ciot_lte_pref.lte_nb1_band_pref = common_pref.lte_nb1_band;
  ciot_lte_pref.lte_band_pref = common_pref.lte_band;
  SYS_LTE_BAND_MASK_ADD_MASK(&common_pref.lte_band,&common_pref.lte_m1_band);
  SYS_LTE_BAND_MASK_ADD_MASK(&common_pref.lte_band,&common_pref.lte_nb1_band);  
  #else
  memset((void *)&ciot_lte_pref, 0, sizeof(sd_ss_ciot_lte_pref_s_type));
  #endif
  
  /* Send command to MMOC */
  cmmsc_send_mmoc_pref_sys_chgd_cmd(cmph_map_cm_ss_type_to_sd_ss_type(ss),
                                      pref_reason,
                                      orig_mode,
                                      common_pref.mode,
                                      common_pref.band,
                                      common_pref.lte_band,
                                      common_pref.tds_band,
                                      SD_SS_PRL_PREF_ANY,
                                      SD_SS_ROAM_PREF_ANY,
                                      internal_hybr_pref,
                                      pref_info->otasp_band,
                                      pref_info->otasp_blksys,
                                      pref_info->avoid_type,
                                      pref_info->avoid_time,
                                      (const sys_manual_sys_info_s_type *)&(pref_info->sys_id),
                                      common_pref.domain,
                                      cmph_map_cm_acq_order_pref_to_sd(pref_info->acq_order_pref),
                                      pref_update_reason,
                                      &addl_action,
                                      user_mode_pref,
                                      (const sd_rat_acq_order_s_type *)&sd_rat_acq_order,
                                      sys_sel_pref_req_id,
                                      camp_mode,
                                      csg_id,
                                      csg_rat,
                                      CMPH_SS_VOICE_DOMAIN_PREF(state_machine->asubs_id, ph_ptr),
                                      &ciot_lte_pref
                                    );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update last preferences which were sent */
  if (ss != CM_SS_HYBR_1)
  {
    cm_act_orig_s_type *pref_sent_ptr = cmph_get_last_pref_sent(ss);
  
    if (pref_sent_ptr)
    {
      pref_sent_ptr->act_priority = CM_ACT_PRIORITY_PH;
      pref_sent_ptr->orig_band_pref = cmph_map_sd_band_pref_to_cm_band_pref(common_pref.band);
      pref_sent_ptr->orig_lte_band_pref = common_pref.lte_band;
      pref_sent_ptr->orig_tds_band_pref = cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(common_pref.tds_band);
      pref_sent_ptr->orig_mode = orig_mode;
      pref_sent_ptr->orig_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(common_pref.mode);
      pref_sent_ptr->orig_pref_term = CM_PREF_TERM_NONE;
      pref_sent_ptr->orig_prl_pref = pref_info->prl_pref;
      pref_sent_ptr->orig_roam_pref = pref_info->roam_pref;

      #if defined(FEATURE_CIOT)
      pref_sent_ptr->ciot_lte_pref.lte_op_mode_pref = pref_info->ciot_lte_pref.lte_op_mode_pref;
      pref_sent_ptr->ciot_lte_pref.lte_m1_band_pref = common_pref.lte_m1_band;

      pref_sent_ptr->ciot_lte_pref.lte_nb1_band_pref = common_pref.lte_nb1_band;
      
      pref_sent_ptr->ciot_lte_pref.lte_band_pref = common_pref.lte_band;

      #endif
      
      pref_sent_ptr->orig_srv_domain_pref =
        cmph_map_sd_srv_domain_pref_to_cm_domain_pref(common_pref.domain);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For L+G DSDS */
  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim()&&
     (ph_ptr->dds_status == CMPH_DDS_STATUS_PS_DETACH_PENDING))
  {
    /* If there's LTE call(s), change the dds_status to REMOVE_DATA_OBJ
    ** Else, reset the dds_status.
    */
    if (cmcall_is_no_call_in_lte_ps() == FALSE)
    {
      ph_ptr->dds_status = CMPH_DDS_STATUS_REMOVE_DATA_OBJ;
    }
    else
    {
      ph_ptr->dds_status = CMPH_DDS_STATUS_NONE;
    }
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For svlte, if is_stack_sync_bsr_pending and call end, force preference */
  #if defined(FEATURE_MMODE_SC_SVLTE)
  if(cmmsc_auto_is_stack_sync_bsr_pending(state_machine) &&
    ss == CM_SS_HYBR_2 &&
    pref_reason == SD_SS_PREF_REAS_ORIG_END)
  {
    cmmsc_send_mmoc_pref_sys_chgd_cmd(cmph_map_cm_ss_type_to_sd_ss_type(ss),
                                      SD_SS_PREF_REAS_STACK_SYNC_UP_BSR,
                                      orig_mode,
                                      common_pref.mode,
                                      common_pref.band,
                                      common_pref.lte_band,
                                      common_pref.tds_band,
                                      cmph_map_cm_prl_pref_to_sd_prl_pref(pref_info->prl_pref),
                                      cmph_map_cm_roam_pref_to_sd_roam_pref(pref_info->roam_pref),
                                      internal_hybr_pref,
                                      pref_info->otasp_band,
                                      pref_info->otasp_blksys,
                                      pref_info->avoid_type,
                                      pref_info->avoid_time,
                                      (const sys_manual_sys_info_s_type *)&(pref_info->sys_id),
                                      common_pref.domain,
                                      cmph_map_cm_acq_order_pref_to_sd(pref_info->acq_order_pref),
                                      pref_update_reason,
                                      &addl_action,
                                      user_mode_pref,
                                      NULL,
                                      sys_sel_pref_req_id,
                                      camp_mode,
                                      csg_id,
                                      csg_rat,
                                      CMPH_SS_VOICE_DOMAIN_PREF(state_machine->asubs_id, ph_ptr),
                                      &ciot_lte_pref
                                    );
    cmmsc_auto_update_is_stack_sync_bsr_pending(state_machine, FALSE);
  }  
  #endif
  
  if(ph_orig_top_ptr->orig != NULL)
  {
    ph_orig_top_ptr->orig->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_send_mmoc_pref_sys_chgd_cmd

DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void    cmmsc_send_mmoc_pref_sys_chgd_cmd
(

        sd_ss_e_type                   ss,
            /**< Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */


        sd_ss_pref_reas_e_type         pref_reas,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sys_lte_band_mask_e_type       lte_band_pref,
           /* LTE band preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         tds_band_pref,
           /* TDS band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /**< The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /**< The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /**< Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /**< Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
          */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type  pref_update_reas,
           /**< Preference change reason
           */

        sys_addtl_action_s_type        *addl_action,
        /*
        To indicate sub action to be performed by MMOC during pref sys chgd cmd
        */

        sd_ss_mode_pref_e_type         user_mode_pref,
        /**< The user/phone mode preference.
        */

        const sd_rat_acq_order_s_type  *rat_acq_order_ptr,
        /**< Rat acquisition order including LTE.
        */

        uint16                         req_id,
        /**< System Selection Preferences request id.
        */

        sd_ss_pref_camp_mode_e_type    camp_only_pref,

        sys_csg_id_type                csg_id,
         /**< CSG identifier */

        sys_radio_access_tech_e_type   csg_rat,
        /**< RAT specified for CSG */

        sys_voice_domain_pref_e_type   voice_domain_pref,
        /**< voice domain pref */

        sd_ss_ciot_lte_pref_s_type     *ciot_lte_pref
)
{
  #ifdef FEATURE_CM_DEBUG_BUFFER
  {
    CMDBG_MEM_ALLOC(cm_mmoc_cmd_buff, cm_mmoc_prot_gen_cmd_entry_s_type)

    cm_mmoc_cmd_buff->ss                = ss;
    cm_mmoc_cmd_buff->pref_reas         = pref_reas;
    cm_mmoc_cmd_buff->orig_mode         = orig_mode;
    cm_mmoc_cmd_buff->mode_pref         = mode_pref;
    cm_mmoc_cmd_buff->band_pref         = band_pref;
    cm_mmoc_cmd_buff->lte_band_pref     = lte_band_pref;
    cm_mmoc_cmd_buff->tds_band_pref     = tds_band_pref;
    cm_mmoc_cmd_buff->roam_pref         = roam_pref;
    cm_mmoc_cmd_buff->hybr_pref         = hybr_pref;
    cm_mmoc_cmd_buff->avoid_type        = avoid_type;
    cm_mmoc_cmd_buff->avoid_time        = avoid_time;
    cm_mmoc_cmd_buff->domain_pref       = srv_domain_pref;
    cm_mmoc_cmd_buff->pref_update_reas  = pref_update_reas;
    cm_mmoc_cmd_buff->addl_action       = *addl_action;
    cm_mmoc_cmd_buff->user_mode_pref    = user_mode_pref;
    cm_mmoc_cmd_buff->camp_mode_pref    = camp_only_pref;
    cm_mmoc_cmd_buff->voice_domain_pref = voice_domain_pref;
    cmdbg_add_to_dbg_buffer(CM_BUFF_MMOC_CMD_TYPE, MMOC_CMD_PROT_GEN_CMD, ((void*)cm_mmoc_cmd_buff));
  }
  #endif
  mmoc_cmd_pref_sys_chgd7( pref_reas,
                           orig_mode,
                           mode_pref,
                           band_pref,
                           lte_band_pref,
                           tds_band_pref,
                           prl_pref,
                           roam_pref,
                           hybr_pref,
                           otasp_band,
                           otasp_blksys,
                           avoid_type,
                           avoid_time,
                           manual_sys_info_ptr,
                           srv_domain_pref,
                           acq_order_pref,
                           pref_update_reas,
                           addl_action,
                           user_mode_pref,
                           rat_acq_order_ptr,
                           req_id,
                           camp_only_pref,
                           csg_id,
                           csg_rat,
                           voice_domain_pref,
                           ciot_lte_pref
                        );

}

/*===========================================================================

FUNCTION cmmsc_form_sub_data_sim0

DESCRIPTION
  This function forms subscription info for sim0 to send to mmoc.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_form_sub_data_sim0(

  boolean                 sub_avail,
  /* An array containing the info of subs_avail for each sub.
   ** 0 for gw primary, 1 for cdma primary ...
   */

  const cm_pref_s_type         *pref_info,
  /* pref_info pointer for sim0
   */

  mmoc_subs_data_s_type  *sub_data
  /* An array of pointers to point to sub_data for sim0
   */
)
{
  sys_manual_sys_info_s_type       manual_sys_info;
  mmgsdi_session_type_enum_type    session_type;
  cmmsc_state_machine_s_type       *state_machine;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  
  /* Validate params
   */
  CM_ASSERT(pref_info != NULL);
  CM_ASSERT(sub_data != NULL);

  if (pref_info == NULL || sub_data == NULL
    || state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sub_data->mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->mode_pref);
  sub_data->band_pref= cmph_map_cm_band_pref_to_sd_band_pref(pref_info->band_pref);
  sub_data->lte_band_pref = pref_info->lte_band_pref;

  #ifdef FEATURE_CIOT
  sub_data->ciot_lte_pref.lte_op_mode_pref = cmph_map_cm_lte_op_mode_to_sd_lte_op_mode(pref_info->ciot_lte_pref.lte_op_mode_pref);
  sub_data->ciot_lte_pref.lte_m1_band_pref= pref_info->ciot_lte_pref.lte_m1_band_pref;
  sub_data->ciot_lte_pref.lte_nb1_band_pref = pref_info->ciot_lte_pref.lte_nb1_band_pref;
  #endif

  sub_data->hybr_pref = SD_SS_HYBR_PREF_NONE;

  sub_data->orig_mode= cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                                pref_info->network_sel_mode_pref);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Fill Subscription Information for SIM1 - gw pri + cdma pri */

  /* GW pri specific info */
  sub_data->is_subs_avail          = sub_avail;
  sub_data->is_perso_locked        = FALSE;
  if(cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(SYS_MODEM_AS_ID_1,&session_type) != CM_MMGSDI_SUCCESS)
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }
  sub_data->gw.acq_order_pref  =
              cmph_map_cm_acq_order_pref_to_sd(pref_info->acq_order_pref);
  sub_data->gw.srv_domain_pref = cmph_map_cm_srv_domain_pref_to_sd(pref_info->srv_domain_pref);;

  manual_sys_info.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS;
  manual_sys_info.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
  manual_sys_info.sys_id.id.plmn    = pref_info->plmn;
  sub_data->gw.manual_sys_info = manual_sys_info;

  #ifdef FEATURE_MMODE_3GPP2
  sub_data->curr_ue_mode =   state_machine->stack_common_info.ue_mode;
  if (cmph_is_subs_feature_mode_srlte(SYS_MODEM_AS_ID_1))
  {
    sub_data->is_ue_mode_substate_srlte = 
                (state_machine->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2);
  }
  else
  #endif

  sub_data->mode_pref = SD_GET_COMMON_MODE(sub_data->mode_pref,
                       state_machine->stack[0].capablity.mode);

  sub_data->band_pref = SD_GET_COMMON_BAND(sub_data->band_pref,
                         state_machine->stack[0].capablity.band);
                       

  sub_data->lte_band_pref = SYS_LTE_BAND_MASK_COMMON(&(sub_data->lte_band_pref),
                         &(state_machine->stack[0].capablity.lte_band));

  sub_data->ciot_lte_pref.lte_band_pref = SYS_LTE_BAND_MASK_COMMON(&(sub_data->ciot_lte_pref.lte_band_pref),
                          &(state_machine->stack[0].capablity.lte_band));

  sub_data->ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_COMMON(&(sub_data->ciot_lte_pref.lte_m1_band_pref),
                         &(state_machine->stack[0].capablity.lte_m1_band));

  sub_data->ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_COMMON(&(sub_data->ciot_lte_pref.lte_nb1_band_pref),
                         &(state_machine->stack[0].capablity.lte_nb1_band));

  CM_MSG_HIGH_3("CM_SUB_DATA: sim_idx=0, mode_pref=0x%x, orig_mode=%d, srv_domain_pref=%d",
                sub_data->mode_pref,
                sub_data->orig_mode, 
                sub_data->gw.srv_domain_pref);
                
}


/*===========================================================================

FUNCTION cmmsc_send_mmoc_subsc_chgd

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static  void    cmmsc_send_mmoc_subsc_chgd
(
        mmoc_subsc_chg_e_type                chg_type,
             /* Subscription change type.
             */
        prot_subsc_chg_e_type                prot_subsc_chg,

        boolean                              hybr_gw_subs_chg,

        boolean                                   hybr3_gw_subs_chg,


        byte                                 nam,

        const mmoc_subs_data_s_type          *cdma_subs_ptr,
        /* CDMA subscription info */

        const mmoc_subs_data_s_type          *gw_subs_ptr,
           /* GW subscription info */

        const mmoc_subs_data_s_type          *gw_hybr_subs_ptr ,

           /* GW HYBR subscription info */
        const mmoc_subs_data_s_type          *gw3_hybr_subs_ptr,
        

        uint8                                 active_subs


)
{
  uint8                 active_ss = ACTIVE_SS_NONE;


  if(active_subs & SYS_MODEM_AS_ID_1_MASK)
  {
     active_ss |= ACTIVE_SS_MAIN;
  }

  if(active_subs & SYS_MODEM_AS_ID_2_MASK)
  {
    if(cmph_is_sxlte())
    {
      active_ss |= ACTIVE_SS_HYBR_3;
    }
    else
    {
      active_ss |= ACTIVE_SS_HYBR_2;
    }
  }

  if(active_subs & SYS_MODEM_AS_ID_3_MASK)
  {
     active_ss |= ACTIVE_SS_HYBR_3;
  }


  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_subs_cmd_to_buffer( chg_type,
                                prot_subsc_chg,
                                gw_subs_ptr);
  
  #endif
  mmoc_cmd_subscription_chgd_new( chg_type,
                                  prot_subsc_chg,
                                  gw_subs_ptr
                                 );

}
/*===========================================================================

FUNCTION cmmsc_proc_cmd_subsc_chgd

DESCRIPTION
  This function should be called when the subscrption availability has
  changed.This function will pack the info and in turn call the function
  cmmsc_proc_cmd_subsc_chgd during processing based on which subscription.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_proc_cmd_subsc_chgd
(

        mmoc_subsc_chg_e_type           chg_type,
             /* Subscription change type.
             */

        cm_pref_s_type                 *pref_info_ptr,
           /* hybr2 preference info pointer.
           */

        boolean                         is_gwl_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        prot_subsc_chg_e_type           prot_subsc_chg
           /* Enumeration of protocols that have a change in
           ** subscription available status */
)
{
  cmmsc_state_machine_s_type*    state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  mmoc_subs_data_s_type            *sub_data;

  sub_data = (mmoc_subs_data_s_type *)cm_mem_malloc(sizeof(mmoc_subs_data_s_type));
  
  state_machine->stack_common_info.is_in_emergency_call = cmcall_emergency_call_active();
  cmmsc_set_stack_capability(state_machine);

  cmmsc_form_sub_data_sim0(is_gwl_subsc_avail,pref_info_ptr,sub_data);

  cmmsc_send_mmoc_subsc_chgd( chg_type,
                              prot_subsc_chg,
                              0,
                              0,
                              CM_NAM_1,
                              NULL,
                              sub_data,
                              NULL,
                              NULL,
                              #ifndef FEATURE_MMODE_DUAL_SIM
                                ACTIVE_SS_MAIN
                              #else
                              subs_change->active_subs
                              #endif
                                );
  cm_mem_free(sub_data);
}

static void cmmsc_get_stack_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       cm_preference_info_s_type     *pref_info,
       cmmsc_stack_capability_s_type *common_cap_ptr
)
{

  cmmsc_stack_s_type            *stack_pointer;
  #if defined(FEATURE_CIOT)
  sys_lte_band_mask_e_type        common_lte_band;
  sys_lte_band_mask_e_type        common_lte_m1_band;
  sys_lte_band_mask_e_type        common_lte_nb1_band;
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pref_info == NULL)
  {
    CM_ERR_0("MSC: pref info null ptr");
    return;
  }

  if(common_cap_ptr == NULL)
  {
    CM_ERR_0("MSC: stack_cap_ptr null");
    return;
  }

  if (state_machine == NULL)
    return;

  stack_pointer = &state_machine->stack[stack_idx];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef FEATURE_HDR_HYBRID
  /* Get mode pref from hybr1 specific pref */
  if (cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine) == CM_SS_HYBR_1)
  {
    common_cap_ptr->mode =
        SD_GET_COMMON_MODE(
           cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->hybr1_pref.hybr_mode_pref),
           stack_pointer->capablity.mode);

  }
  else
  #endif
  {                  
    common_cap_ptr->mode =
        SD_GET_COMMON_MODE(
           cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->mode_pref),
           stack_pointer->capablity.mode);

  }

  /* For sglte dual stack mode, if main stack has GSM only, then set mode to none.
   ** This is done due to NAS limiation - NAS is unable to handle G + G
   */
  #if defined(FEATURE_MMODE_SC_SGLTE)
  cmmsc_special_mode_update_for_main_stack(state_machine,
                                           stack_idx, &(common_cap_ptr->mode));
  #endif
  
  common_cap_ptr->band =
      SD_GET_COMMON_BAND(
         cmph_map_cm_band_pref_to_sd_band_pref(pref_info->band_pref),
         stack_pointer->capablity.band);

  common_cap_ptr->tds_band =
      SD_GET_COMMON_BAND(
         cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(pref_info->tds_band_pref),
         stack_pointer->capablity.tds_band);

  #ifndef FEATURE_CIOT
  common_cap_ptr->lte_band =
      SYS_LTE_BAND_MASK_COMMON(
         &(pref_info->lte_band_pref),
         &(stack_pointer->capablity.lte_band));
  #else
  /* Incase of CIOT, pref_info_lte_band_pref is unioin of lte, m1, nb1 band pref's */
  common_lte_band =  SYS_LTE_BAND_MASK_COMMON(
         &(pref_info->ciot_lte_pref.lte_band_pref),
         &(stack_pointer->capablity.lte_band));

  common_lte_m1_band =  SYS_LTE_BAND_MASK_COMMON(
         &(pref_info->ciot_lte_pref.lte_m1_band_pref),
         &(stack_pointer->capablity.lte_m1_band));

  common_lte_nb1_band =  SYS_LTE_BAND_MASK_COMMON(
         &(pref_info->ciot_lte_pref.lte_nb1_band_pref),
         &(stack_pointer->capablity.lte_nb1_band));

  common_cap_ptr->lte_band = common_lte_band;

  common_cap_ptr->lte_m1_band = common_lte_m1_band;

  common_cap_ptr->lte_nb1_band = common_lte_nb1_band;

  #endif

  common_cap_ptr->domain
    = cmmsc_get_common_domain(
          cmph_map_cm_srv_domain_pref_to_sd(pref_info->srv_domain_pref),
          stack_pointer->capablity.domain );

  if(common_cap_ptr->band != cmph_map_cm_band_pref_to_sd_band_pref(pref_info->band_pref)
    || ((common_cap_ptr->mode & SD_SS_MODE_PREF_TDS) 
         && common_cap_ptr->tds_band != cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(pref_info->tds_band_pref))
     )
  {
    CM_MSG_HIGH_4("MSC->MMOC band_pref = 0x%08x %08x. tds_band_pref = 0x%08x %08x ",
                                                         QWORD_HIGH(pref_info->band_pref),
                                                         QWORD_LOW(pref_info->band_pref),
                                                         QWORD_HIGH(pref_info->tds_band_pref),
                                                         QWORD_LOW(pref_info->tds_band_pref));
  }

  if(!SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(pref_info->lte_band_pref),
         &(common_cap_ptr->lte_band)))
  {
    cm_print_lte_band_mask(pref_info->lte_band_pref);
  }

}

static void cmmsc_get_user_mode_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       sd_ss_mode_pref_e_type         *user_mode_pref
)
{
  cmmsc_stack_s_type            *stack_pointer = &state_machine->stack[stack_idx];

  if(user_mode_pref == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  *user_mode_pref =
      SD_GET_COMMON_MODE( *user_mode_pref, stack_pointer->capablity.mode);

  /* For sglte dual stack mode, if main stack has GSM only, then set mode to none.
   ** This is done due to NAS limiation - NAS is unable to handle G + G
   */
  #if defined(FEATURE_MMODE_SC_SGLTE)
  cmmsc_special_mode_update_for_main_stack(state_machine,
                                           stack_idx, user_mode_pref);
  #endif

}

sd_ss_srv_domain_pref_e_type cmmsc_get_common_domain (
  sd_ss_srv_domain_pref_e_type  domain1,
  sd_ss_srv_domain_pref_e_type  domain2
)
{
  /* Get the common domain pref of 2 domain_pref */
  if (domain1 == domain2)
  {
    return domain1;
  }
  else if ((domain1 == SD_SS_SRV_DOMAIN_PREF_CS_ONLY
            && domain2 == SD_SS_SRV_DOMAIN_PREF_PS_ONLY) ||
           (domain1 == SD_SS_SRV_DOMAIN_PREF_PS_ONLY
            && domain2 == SD_SS_SRV_DOMAIN_PREF_CS_ONLY))
  {
    CM_MSG_HIGH_2("MSC: Common domain none - domain1 %d domain2 %d",domain1,domain2);
    return SD_SS_SRV_DOMAIN_PREF_NONE;
  }
  else if(domain1 == SD_SS_SRV_DOMAIN_PREF_CS_PS)
  {
    return domain2;
  }
  else if (domain2 == SD_SS_SRV_DOMAIN_PREF_CS_PS)
  {
    return domain1;
  }
  else
  {
    CM_MSG_HIGH_2("MSC: Common domain none - domain1 %d domain2 %d",domain1,domain2);
    return SD_SS_SRV_DOMAIN_PREF_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_get_cm_ss_mode_cap

DESCRIPTION
  This function returns the mode pref of the top priority queue base on the capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsc_get_cm_ss_mode_cap (
  cm_ss_e_type  cm_ss
)
{
  cmmsc_stack_s_type          *stack_pointer = NULL;
  int                         stack_idx = cmmsc_mapping_ss_to_stack_idx(cm_ss);
  cmmsc_state_machine_s_type  *state_machine
    = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  if (state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  stack_pointer = &state_machine->stack[stack_idx];
  return stack_pointer->capablity.mode;
}


#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)

/*===========================================================================

FUNCTION cmmsc_get_cm_ss_top_mode_pref_and_cap

DESCRIPTION
  This function returns the mode pref of the top priority queue base on the capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsc_get_cm_ss_top_mode_pref_and_cap (
  cm_ss_e_type  cm_ss
)
{
  cmmsc_stack_s_type          *stack_pointer = NULL;
  int                         stack_idx = cmmsc_mapping_ss_to_stack_idx(cm_ss);
  cm_orig_q_s_type            *ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);
  sd_ss_mode_pref_e_type      mode_pref = SD_SS_MODE_PREF_NONE;
  cmmsc_state_machine_s_type  *state_machine
    = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  if (state_machine == NULL)
    return mode_pref;

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_1("MSC: Top element on priority queue of stack_idx=%d is NULL",
            stack_idx);
    return mode_pref;
  }

  stack_pointer = &state_machine->stack[stack_idx];
  mode_pref = SD_GET_COMMON_MODE(
                          cmph_map_cm_mode_pref_to_sd_mode_pref(
                          ph_orig_top_ptr->orig->orig_mode_pref),
                          stack_pointer->capablity.mode);
  return mode_pref;
}

/*===========================================================================

FUNCTION cmmsc_get_sd_ss_top_mode_pref_and_cap

DESCRIPTION
  This function returns the mode pref of the top priority queue base on the capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsc_get_sd_ss_top_mode_pref_and_cap (
  sd_ss_e_type  sd_ss
)
{
  cm_ss_e_type cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(sd_ss);
  return cmmsc_get_cm_ss_top_mode_pref_and_cap(cm_ss);
}
#endif
void cmmsc_update_stack_preference(

        cmmsc_state_machine_s_type       *state_machine,
            /* Subscription for which preference need to be updated */
        
        cm_mode_pref_e_type              mode_pref,
            /* Mode preference which to update */
        
        cm_band_pref_e_type              band_pref,
            /* Band preference which to update */
        
        sys_lte_band_mask_e_type         lte_band_pref,
            /* LTE band preference which to update */
        
        cm_band_pref_e_type              tds_band_pref,
            /* TDS band preference which to update */

        sys_lte_band_mask_e_type         lte_m1_band_pref,
            /* LTE band preference which to update */
        
        sys_lte_band_mask_e_type         lte_nb1_band_pref
            /* LTE band preference which to update */

)
{
  if(  state_machine->stack_common_info.preference.mode_pref != mode_pref ||
  state_machine->stack_common_info.preference.band_pref != band_pref ||
  !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&state_machine->stack_common_info.preference.lte_band_pref, &lte_band_pref) ||
  state_machine->stack_common_info.preference.tds_band_pref != tds_band_pref ||
  !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&state_machine->stack_common_info.preference.lte_m1_band_pref, &lte_m1_band_pref) ||
  !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&state_machine->stack_common_info.preference.lte_nb1_band_pref, &lte_nb1_band_pref)
)
  {
    CM_MSG_HIGH_6("update_stack_preference(): mode_pref Old %d, new %d,band_pref Old 0x%x, new 0x%x, tds_band_pref Old 0x%x, new 0x%x", 
                  state_machine->stack_common_info.preference.mode_pref, mode_pref, state_machine->stack_common_info.preference.band_pref, 
                  band_pref, state_machine->stack_common_info.preference.tds_band_pref, tds_band_pref);
    cm_print_lte_band_mask(lte_band_pref);
    cm_print_lte_band_mask(lte_m1_band_pref);
    cm_print_lte_band_mask(lte_nb1_band_pref);
  }
  state_machine->stack_common_info.preference.mode_pref = mode_pref;
  state_machine->stack_common_info.preference.band_pref = band_pref;
  state_machine->stack_common_info.preference.lte_band_pref = lte_band_pref;
  state_machine->stack_common_info.preference.tds_band_pref = tds_band_pref;
  state_machine->stack_common_info.preference.lte_m1_band_pref = lte_m1_band_pref;
  state_machine->stack_common_info.preference.lte_nb1_band_pref = lte_nb1_band_pref;

}


/*===========================================================================

===========================================================================*/
#if defined(FEATURE_MMODE_HYBR2_ENABLED)
boolean cmmsc_is_hybr2_kick_allowed(

  cmmsc_state_machine_s_type *msc

)
{
  boolean allowed = TRUE;

  /* if in emerg mode, do not kick LTE stack with orig mode NONE or pref reas != EMERG;
  ** or if cmregprx is active for main, do not kick LTE.
  */
  #ifdef FEATURE_MMODE_3GPP
  allowed =( (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state != CMREGPRX_STATE_ACTIVE &&
               cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1)) ||
             cmph_is_subs_feature_mode_sglte(SYS_MODEM_AS_ID_1)  
             #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
             ||  cmph_ptr()->ph_state == CM_PH_STATE_EMERG_CB
             #endif
             || cmcall_emergency_call_active()
          );
  CM_MSG_HIGH_3("asubs_id=%d, is_hybr2_kick_allowed = %d, main cmregprx state=%d", 
                 msc->asubs_id, allowed, 
                 cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state);
  #endif


  return allowed;
}
#endif

