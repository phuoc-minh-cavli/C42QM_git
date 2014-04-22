/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      C A L L   M A N A G E R   P O L I C Y   M A N A G E R   P R O X Y

GENERAL DESCRIPTION
  This file is the main implementation for the Policy Manager(PM) proxy. The
  purpose of PMProxy is to act as a proxy to interface with the Policy Manager.
  Interpreting the Policy Manager messages into Call Manager actions.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmpmprx_init() must be called to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmpmprx.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/09/14   ss       Do not enforce policy when UE is OFFLINE
09/12/13    jvo     Fix 2 static analysis errors with one change relating to
                    policy comparison
04/29/13    ak      Initial design

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"       /* Customer configuration file */
#include "comdef.h"         /* Definition for basic types and macros */
#include "cmph.h"
#include "cm_msgr_msg.h"
#include "cmtaski.h"
#include "cmidbg.h"
#include "cmpmprx.h"
#include "cmss.h"
#include "cmmsc.h"
#include "cmefs.h"
#include "cmsoa.h"
#include "cmsds.h"
//#include "cmwcall.h"
#ifdef FEATURE_DOMAIN_SELECTION
#include "cmsds.h"
#endif
#include "cmclnup.h"

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmemgext.h"

#include "cmdbg.h"
/*===========================================================================
=============================================================================
=============================================================================
============================  Internal Functions ==================================
=============================================================================
=============================================================================
===========================================================================*/

/* Forward declaration
*/
static policyman_status_t       cmpmprx_retrieve_policy_item_data
(
       cm_policy_config_s_type    *pm_ptr,

       policyman_item_collection_t const *policy_item,

       policy_change_mask         *change_mask

);
static void cmpmprx_init_pm_ue_mode( void );
static void cmpmprx_set_policy_chg_flag( policy_change_mask change_value );
static void cmpmprx_reset_policy_chg_flag( policy_change_mask change_value );
static void cmpmprx_print_current_policy(void);
static boolean cmpmprx_compare_policies(cm_policy_config_s_type    *policy_1,cm_policy_config_s_type    *policy_2);

static cm_policy_config_s_type cmpm_local;
  /* The one and only phone object */

/* Function Definations
*/
/*===========================================================================

FUNCTION cmpm_ptr

DESCRIPTION
  Return a pointer to the one and only phone policy object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_policy_config_s_type  *cmpm_ptr( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmpm_local;

} /* cmph_ptr() */


#ifndef FEATURE_TRIM_CMPMPRX
/*===========================================================================

FUNCTION cmpmprx_compare_policies

DESCRIPTION
  To compare policy for all subs.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: change in policy
  False: no change in policy.

SIDE EFFECTS
  none

===========================================================================*/
static boolean                    cmpmprx_compare_policies(

       cm_policy_config_s_type    *old_policy,

       cm_policy_config_s_type    *new_policy
)
{
  unsigned int i = 0;

  if(memcmp(&(old_policy->current_policy),
              &(new_policy->current_policy),
              sizeof(cm_policy_items_s_type)) != 0)
  {
    return TRUE;
  }

  return FALSE;
  
}
#endif
/*===========================================================================

FUNCTION cmpmprx_init_pm_ue_mode

DESCRIPTION
  Read sglte related configuration from Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_init_pm_ue_mode( void )
{

  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  boolean                 is_policy_valid   = FALSE;
  uint8                   i                 = 0;

  #ifndef CM_DEBUG
  policy_change_mask      local_mask     = 0;
  policyman_item_id_t     pIds[]            =
  {
    POLICYMAN_ITEM_RAT_CAPABILITY,
    POLICYMAN_ITEM_RF_BANDS,
    POLICYMAN_ITEM_SVC_MODE
  };

  size_t                  numIds            = ARR_SIZE(pIds);

  policyman_item_collection_t const  *pCollection = NULL;

#if defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE)
  int32                        read_size = 0;
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pm_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read the configuration from the policy manager. */
  if(POLICYMAN_SUCCEEDED(policyman_get_items_block_msim(pIds,numIds,&pCollection)))
  {
    is_policy_valid = TRUE;

    if( !POLICYMAN_SUCCEEDED(cmpmprx_retrieve_policy_item_data( pm_ptr,pCollection,&local_mask)) )
    {
      is_policy_valid = FALSE;
      CM_MSG_HIGH_0("CMPRX: cmpmprx_retrieve_policy_item_data failed");
    }
    else
    {
      pm_ptr->policy_changes |= local_mask;
      CM_MSG_HIGH_2("Msgr policy mask = %x",i,pm_ptr->policy_changes); 
    }
	
    policyman_item_collection_release(pCollection);
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error in reading the policy item(s). Discard everything and initialize to
  ** default values.
  */
  CM_MSG_HIGH_1("CMPRX: is_policy_valid %d", is_policy_valid);
  pm_ptr->debug_var.is_init_policy_valid = is_policy_valid;

  if (!is_policy_valid)
  {
    pm_ptr->current_policy.policy_band_capability     = SYS_BAND_MASK_ANY;
    pm_ptr->current_policy.policy_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
    pm_ptr->current_policy.service_scan_mode          = POLICYMAN_SVC_MODE_FULL;
    #ifndef CM_DEBUG

    pm_ptr->current_policy.policy_mode_capability     = SD_SS_MODE_PREF_ANY;

    #endif
    cmpmprx_reset_policy_chg_flag(PM_CHG_ALL );
    /* No action need be taken to enforce this policy. So resetting all flags.
    */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmpmprx_update_cm_policy_consumption_eligibility_subsc(CMPMPRX_SUBSC_NOT_AVAIL);
  cmpmprx_print_current_policy();
  return;

}

/*===========================================================================

FUNCTION cmpmprx_init

DESCRIPTION
  Initialize the common functionality of Policy Manager Proxy object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpmprx_init( void )
{

  /* Retrieve the initial policy from the Policy Manager. */
  cmpmprx_init_pm_ue_mode();
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize is_pm_supsend */
  cmpm_ptr()->is_pm_suspend = FALSE;

#ifdef CM_DEBUG
  #error code not present
#endif

  return;
}


/*===========================================================================

FUNCTION cmpmprx_get_device_mode

DESCRIPTION
  Gets device mode which is decided based on no. of SIM and max SIMs that are
  active at a given time.

===========================================================================*/
sys_modem_device_mode_e_type cmpmprx_get_device_mode( void )
{
  return SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
}


/*===========================================================================

FUNCTION cmpmprx_set_policy_chg_flag

DESCRIPTION
  Maintain the bitmask for what all policy configuration items have changed
  since last policy was enforced.

  If PM_CHG_NO_CHANGE passed to the function, clear the bitmask.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_set_policy_chg_flag(

       policy_change_mask change_value
)
{
  cmpm_ptr()->policy_changes |= change_value;
  CM_MSG_HIGH_2("cmpmprx_set_policy_chg_flag() on change_value = %d policy_changes = %d",change_value,cmpm_ptr()->policy_changes);
}

/*===========================================================================

FUNCTION cmpmprx_reset_policy_chg_flag

DESCRIPTION
  Maintain the bitmask for what all policy configuration items have changed
  since last policy was enforced.

  If PM_CHG_NO_CHANGE passed to the function, clear the bitmask.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                       cmpmprx_reset_policy_chg_flag(

       policy_change_mask         change_value
)
{
  cmpm_ptr()->policy_changes &= (~change_value);
}

/*===========================================================================

FUNCTION cmpmprx_is_mask_set

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_is_mask_set(

       policy_change_mask         check_mask,

       cm_policy_config_s_type   *pm_ptr
)
{
  if( pm_ptr == NULL )
  {
    pm_ptr = cmpm_ptr();
  }

  if(pm_ptr->policy_changes & check_mask)
  {
    return TRUE;
  }


  return FALSE;
}

/*===========================================================================

FUNCTION cmpmprx_print_current_policy

DESCRIPTION
  Print the value of the configuration items of the current policy.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_print_current_policy(void)
{
  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();

  CM_MSG_HIGH_2("PMPRX: BANDs(MSB)(LSB) GW = 0x%08x %08x",
                          QWORD_HIGH(pm_ptr->current_policy.policy_band_capability),
                          QWORD_LOW(pm_ptr->current_policy.policy_band_capability));
  
  cm_print_lte_band_mask(pm_ptr->current_policy.policy_lte_band_capability);

  CM_MSG_HIGH_2("PMPRX: Change flags = %d, CM Eligibility to consume %d",
                                                       pm_ptr->policy_changes,
                                  cmpmprx_read_cm_policy_consumption_eligibility());
}

/*===========================================================================

FUNCTION cmpmprx_retrieve_policy_item_data

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static policyman_status_t        cmpmprx_retrieve_policy_item_data
(
       cm_policy_config_s_type    *pm_ptr,

       policyman_item_collection_t const  *policy_item,

       policy_change_mask         *change_mask
)
{
  policyman_status_t  return_val = POLICYMAN_STATUS_ERR;
  uint8 i=0, j=0;
  policyman_item_t const  *pItem;

  CM_ASSERT( pm_ptr != NULL );
  #ifndef CM_DEBUG
  CM_ASSERT( policy_item != NULL );
  #endif

  CM_MSG_HIGH_1("POLICY_ITEM: RETRIEVE, numItems %d",
                 policy_item->numItems);
  
  /* Updating the policy sequence number */

  pm_ptr->seqno = policy_item->seqno;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i=0;i<policy_item->numItems;i++)
  {
    pItem = policy_item->pItems[i];
    switch (policyman_item_get_id(pItem))
    {
     case POLICYMAN_ITEM_RAT_CAPABILITY:
       {
         return_val = policyman_get_rat_capability( pItem,
                        &(pm_ptr->current_policy.policy_mode_capability));

         if (POLICYMAN_SUCCEEDED(return_val))
         {
           *change_mask |= PM_CHG_MODE_CAP;
           CM_MSG_HIGH_1("PM -> PMPRX: mode_cap_chgd: mode_cap=%d",
                            pm_ptr->current_policy.policy_mode_capability);
         }
       }
       
       break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     case POLICYMAN_ITEM_RF_BANDS:
     {
       return_val = policyman_get_rf_bands(pItem,
                                         &(pm_ptr->current_policy.policy_band_capability),
                                         &(pm_ptr->current_policy.policy_lte_band_capability),
                                         NULL);

       if ( POLICYMAN_SUCCEEDED(return_val))
       {
         *change_mask |= PM_CHG_BAND_CAP;

         if ( !POLICYMAN_SUCCEEDED(policyman_get_rf_bands_force( pItem,
                                                               &(pm_ptr->is_force_registration)))
          )
         {
           pm_ptr->is_force_registration = FALSE;
         }

         CM_MSG_HIGH_1("PM -> PMPRX: band_cap_chgd:is_force_registration=%d", 
                           pm_ptr->is_force_registration);
       }
     }
       break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     case POLICYMAN_ITEM_SVC_MODE:
       {
         return_val = policyman_get_svc_mode( pItem,
                                              &(pm_ptr->current_policy.service_scan_mode));

         if ( POLICYMAN_SUCCEEDED(return_val))
         {
           *change_mask |= PM_CHG_SVC_SCAN;
           CM_MSG_HIGH_1("PM -> PMPRX: srv_mode_chgd: srv_mode=%d",
                            pm_ptr->current_policy.service_scan_mode);
         }
       }
       break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     /*- - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_ENABLE_PM_RPL
	  case POLICYMAN_ITEM_RAT_ACQ_ORDER:          
       {
        CM_MSG_HIGH_1("PM -> PMPRX: policy changes for sub :%x.",
                      pm_ptr->policy_changes);

         return_val = policyman_get_rat_order(pItem,
                                           &(pm_ptr->current_policy.acq_order_pref));
                                           
         if ( POLICYMAN_SUCCEEDED(return_val))
         {
           *change_mask |= PM_CHG_RAT_ACQ_ORDER;

           CM_MSG_HIGH_3("PM -> PMPRX: rat_acq_chgd : list elements=%d %d %d %d %d %d",
                          pm_ptr->current_policy.acq_order_pref.acq_order[0],
                          pm_ptr->current_policy.acq_order_pref.acq_order[1],
                          pm_ptr->current_policy.acq_order_pref.acq_order[2]);
         }
      }
     break;
#endif
     default:
       return_val = POLICYMAN_STATUS_SUCCESS;
    }
    CM_MSG_HIGH_2("Retrieve policy data pitem %d change mask %x",pItem,  change_mask); 
  }

  return return_val;
}

/*===========================================================================

FUNCTION cmpmprx_fetch_mode_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_fetch_mode_capability(

       sys_sys_mode_mask_e_type   *mode_capability_buffer
)
{
  if ( !cmpmprx_is_mask_set( PM_CHG_MODE_CAP, cmpm_ptr() )||
       mode_capability_buffer == NULL       ||
       !cmpmprx_read_cm_policy_consumption_eligibility()
     )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("PMPRX -> CM: Mode capability 0x%x, change mask: %d",
                 cmpm_ptr()->current_policy.policy_mode_capability, cmpm_ptr()->policy_changes);

  *mode_capability_buffer = cmpm_ptr()->current_policy.policy_mode_capability;
  cmpmprx_reset_policy_chg_flag( PM_CHG_MODE_CAP );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_get_mode_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_get_mode_capability(

       sys_sys_mode_mask_e_type   *mode_capability_buffer
)
{

   *mode_capability_buffer = cmpm_ptr()->current_policy.policy_mode_capability;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_get_band_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_get_band_capability(

       sys_band_mask_e_type       *band_capability_buffer,

       sys_lte_band_mask_e_type   *lte_band_capability_buffer,

       sys_band_mask_e_type       *tds_band_capability_buffer,

       boolean                    *force_registration,

       sys_lte_band_mask_e_type       *lte_m1_band_capability_buffer,

       sys_lte_band_mask_e_type       *lte_nb1_band_capability_buffer

)
{
  cm_policy_items_s_type* curr_policy_ptr = NULL;

  if ( !cmpmprx_is_mask_set( PM_CHG_BAND_CAP, cmpm_ptr() )   ||
       band_capability_buffer     == NULL     ||
       lte_band_capability_buffer == NULL     ||
       tds_band_capability_buffer == NULL     ||
       force_registration         == NULL     ||
       !cmpmprx_read_cm_policy_consumption_eligibility()
     )
  {
    return FALSE;
  }

  curr_policy_ptr = &(cmpm_ptr()->current_policy);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("PMPRX -> CM(MSB)(LSB): BANDs GW = 0x%08x 0x%08x",
                      QWORD_HIGH(cmpm_ptr()->current_policy.policy_band_capability),
                      QWORD_LOW(cmpm_ptr()->current_policy.policy_band_capability));
  cm_print_lte_band_mask(curr_policy_ptr->policy_lte_band_capability);

  CM_MSG_HIGH_1("PMPRX -> CM: force_registration = %d",
                      cmpm_ptr()->is_force_registration);

  *band_capability_buffer     = curr_policy_ptr->policy_band_capability;
  *lte_band_capability_buffer = curr_policy_ptr->policy_lte_band_capability;
  *force_registration         = cmpm_ptr()->is_force_registration;
  *lte_m1_band_capability_buffer = curr_policy_ptr->policy_lte_m1_band_capability;
  *lte_nb1_band_capability_buffer = curr_policy_ptr->policy_lte_nb1_band_capability;

  cmpmprx_reset_policy_chg_flag( PM_CHG_BAND_CAP );
  cmpm_ptr()->is_force_registration = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;
}

#ifndef FEATURE_TRIM_CMPMPRX
/*===========================================================================

FUNCTION cmpmprx_is_special_volte_e911_calls

DESCRIPTION
  Returns specialization value for SYS_SPECIAL_VOLTE_E911_CALLS.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean cmpmprx_is_special_volte_e911_calls(void)
{
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("PMPRX: special_volte_e911_calls %d",cmpm_ptr()->special_volte_e911_calls);

  return (cmpm_ptr()->special_volte_e911_calls == SYS_SPECIAL_VOLTE_E911_CALLS);
}


/*===========================================================================

FUNCTION cmpmprx_is_ok_to_enforce_policy

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpmprx_is_ok_to_enforce_policy(

       cm_policy_config_s_type   *policy_ptr

)
{
  uint8 i=0;

  if(policy_ptr == NULL)
  {
    policy_ptr = cmpm_ptr();
  }

  CM_ASSERT(policy_ptr != NULL);

  /* If Subscription not available
  ** or operating mode not online
  ** or no policy change (not expected)
  */
  if ( (!cmpmprx_read_cm_policy_consumption_eligibility()||
       !cmpmprx_is_mask_set( PM_CHG_ALL, policy_ptr ))
       ||
       cmclnup_is_ongoing_subsc(BM(SYS_MODEM_AS_ID_1)))
  {
    CM_MSG_HIGH_0( "Policy Not forced, due to Subsc not avail or No policy changed");
    return FALSE;
  }

  /* Check the UE Operation Mode. If the UE is OFFLINE,
  ** return FALSE. Mode OFFLINE_CDMA and OFFLINE_AMPS need
  ** to be checked here for Legecy compatibility because
  ** QXDM sends down OFFLINE as OFFLINE_CDMA */
  if( cmph_ptr()->oprt_mode == SYS_OPRT_MODE_OFFLINE ||
      cmph_ptr()->oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA )
  {
    CM_MSG_HIGH_0( "UE is OFFLINE. Cannot Enforce Policy");
    return FALSE;
  }

  /* Service scan mode changed, UE mode not changed. */
  if ( cmpmprx_is_mask_set( PM_CHG_SVC_SCAN, policy_ptr ) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( PM_CHG_MODE_CAP, policy_ptr ) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( PM_CHG_BAND_CAP, policy_ptr ) )
  {
    return TRUE;
  }

#ifdef FEATURE_ENABLE_PM_RPL

  if ( cmpmprx_is_mask_set( PM_CHG_RAT_ACQ_ORDER, policy_ptr) )
  {
    return TRUE;
  }
#endif
  CM_MSG_HIGH_0("Policy Not forced, No conditions meets..");
  return FALSE; /* Code execution should not come here. */
}

/*===========================================================================

FUNCTION cmpmprx_enforce_current_policy

DESCRIPTION
  Process the policy currently in cmpm_local.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void         cmpmprx_enforce_current_policy(
   )
{
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();
  cmph_s_type *ph_ptr = cmph_ptr();
  boolean rat_acq_order_chgd = FALSE;
  cm_act_update_reas_e_type  act_update_reas = CM_ACT_UPDATE_REAS_USER;
  boolean force_top_pref = FALSE;


  cmpmprx_print_current_policy();

  if ( !cmpmprx_is_ok_to_enforce_policy( pm_ptr) )
  {
    return;
  }

#ifdef FEATURE_ENABLE_PM_RPL

  if ( cmpmprx_is_mask_set( PM_CHG_RAT_ACQ_ORDER, pm_ptr )
	 )
  {
    CM_MSG_HIGH_1("PM->PMPRX :updating phone object: policy_chgd %x ", 
                   pm_ptr->policy_changes);
    cmph_get_sub_config_ptr(asubs_id)->rat_acq_order_pref
      = pm_ptr->current_policy[asubs_id].acq_order_pref;
    //cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_RAT_ACQ_ORDER);
    rat_acq_order_chgd = TRUE;
  }
#endif

  if ( cmpmprx_is_mask_set(  PM_CHG_SVC_SCAN, pm_ptr ) )
  {
    cmpmprx_enforce_service_scan_mode();
  }
  else if(cmpmprx_is_mask_set( PM_CHG_ALL, cmpm_ptr()) ||
           rat_acq_order_chgd )
  {

    /* If forcing is to be done on MAIN/HYBR_1/HYBR_2 combined */

    cm_orig_q_s_type  *main_stack_top = cmtask_orig_para_get_top(CM_SS_MAIN);
    if((main_stack_top != NULL) && 
       (main_stack_top->act_type == CM_ACT_TYPE_PH_OBJ ||
        main_stack_top->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT) )
    {
      force_top_pref = TRUE;
  	  act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
    }
      (void) cmph_force_pref_on_the_fly( CM_SS_MAX,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->main_stack_info.pref_info.pref_term,
                                         ph_ptr->main_stack_info.pref_info.mode_pref,
                                         ph_ptr->main_stack_info.pref_info.band_pref,
                                         ph_ptr->main_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->main_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->main_stack_info.pref_info.plmn,
                                         ph_ptr->main_stack_info.pref_info.srv_domain_pref,
                                         ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                                         (cm_act_id_type)ph_ptr,
                                         act_update_reas,
                                         force_top_pref,
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

   
  }
  
 
#ifdef FEATURE_ENABLE_PM_RPL
  /*Reset for rat _acq_order here itself*/
  cmpmprx_reset_policy_chg_flag(PM_CHG_RAT_ACQ_ORDER);
#endif
  
}

/*===========================================================================

FUNCTION cmpmprx_update_curr_policy

DESCRIPTION
  Update cmpm_local with new policy retrieved from Policy Manager..

DEPENDENCIES
  cmpm_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                              cmpmprx_update_curr_policy(

       cm_policy_config_s_type    *local_policy

)
{
  if ( local_policy != NULL )
  {
    cm_policy_config_s_type *curr_policy = cmpm_ptr();

    *curr_policy = *local_policy;
  }
  return;
}


/*===========================================================================

FUNCTION cmpmprx_process_retrieved_policy

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_process_retrieved_policy(

       cm_policy_config_s_type    *local_policy,

       policy_change_mask         local_mask
)

{
  if ( local_policy != NULL )
  {
    CM_MSG_HIGH_2("cmpmprx_process_retrieved_policy() on local_mask = %d policy_changes = %d",
                  local_mask,cmpm_ptr()->policy_changes);
    local_policy->policy_changes |= local_mask;
    cmpmprx_update_curr_policy(local_policy);
    cmpmprx_enforce_current_policy();
  }
}

/*===========================================================================

FUNCTION cmpmprx_process_msgr_msg

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean                    cmpmprx_process_msgr_msg(

       cm_policyman_cfg_s         *rpt_ptr,

       cm_policy_config_s_type    *local_policy
)
{
  int                     i                   = 0;
  msgr_attach_s           *pAttach            = NULL;
  boolean                 is_new_policy_valid = TRUE;
  policy_change_mask      local_mask          = 0;

  cm_policy_config_s_type *curr_policy        = cmpm_ptr();
  boolean                 is_any_policy_chg   = FALSE;
  boolean                 is_policy_seq_invalid = FALSE;
  size_t                  local_seqno = cmpm_ptr()->seqno;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_1("PROC MSGR: num_attach=%d",
                 msgr_get_num_attach(&(rpt_ptr->msg_hdr)));
  

  for (i = 0 ; i < msgr_get_num_attach(&(rpt_ptr->msg_hdr)) ; ++i)
  {
    
    policyman_item_collection_t const *policy_item = NULL;

    pAttach = msgr_get_attach(&(rpt_ptr->msg_hdr), i);
    if (pAttach == NULL || pAttach->type != MSGR_ATTACH_REFCNT)
    {
      CM_MSG_HIGH_1("PROC MSGR: PMPRX: Invalid attachment %d, Abort new policy",i);
      is_new_policy_valid = FALSE;
      break;
    }

    policy_item = pAttach->data.refcnt.refcnt_obj_ptr;

    if (policy_item->seqno <= curr_policy->seqno)
	{
	   CM_MSG_HIGH_2("PROC MSGR: current policy is already handled: policy_item->seqno= %d, curr_policy->seqno = %d",policy_item->seqno, curr_policy->seqno);
	   return FALSE;
	}

	if(policy_item->seqno != curr_policy->seqno + 1)
	{
	   is_policy_seq_invalid = TRUE;
      CM_MSG_HIGH_2("PROC MSGR: CM-PM policy seq nums are out of Sync policy_item->seqno= %d, curr_policy->seqno = %d",
                    policy_item->seqno, curr_policy->seqno);
	   break;
	}
    if(!POLICYMAN_SUCCEEDED(cmpmprx_retrieve_policy_item_data( local_policy, policy_item, &local_mask)))
    {
      CM_MSG_HIGH_1("PROC MSGR: PMPRX: Unable to retrieve data for attachment %d",i);
	   /*As the sequence number would be also be valid in case retrieval policy fails*/
	  local_seqno = policy_item->seqno;
      is_new_policy_valid = FALSE;
      break;
    }
	local_seqno = policy_item->seqno;
  } /* for() */

  if(is_policy_seq_invalid)
  {
    policyman_item_collection_t const *new_policy_item = policyman_get_items_since_sequence_number(curr_policy->seqno);

    if (new_policy_item == NULL)
    {
      CM_MSG_HIGH_0("PROC MSGR: PMPRX: Invalid attachment %d, Abort new policy");
      is_new_policy_valid = FALSE;
    }
	 else if(!POLICYMAN_SUCCEEDED(cmpmprx_retrieve_policy_item_data( local_policy, new_policy_item, &local_mask)))
    {
      CM_MSG_HIGH_1("PROC MSGR: PMPRX: Unable to retrieve data for attachment %d",i);
      is_new_policy_valid = FALSE;
    }

    if(new_policy_item != NULL)
	{
      /* update the sequence number both in case retrieval is success or failure
	  retrieve fail shall also update policy sequence number */
      local_seqno = new_policy_item->seqno;
	  policyman_item_collection_release(new_policy_item);
	}
  }

  is_any_policy_chg = (is_any_policy_chg || (local_mask != 0));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !is_new_policy_valid                   ||
       !is_any_policy_chg                      ||
       (cmpmprx_compare_policies(local_policy,curr_policy) == 0))
  {
    CM_MSG_HIGH_2("PROC MSGR: PMPRX: Discard new policy valid = %d, is_any_policy_chg = %d",
                                            is_new_policy_valid,is_any_policy_chg);
	
	cmpm_ptr()->seqno = local_seqno;
    return FALSE;
  }


  local_policy->policy_changes |= local_mask;
  CM_MSG_HIGH_2("PROC MSGR: Msgr policy[%d] mask = %x",i,local_policy->policy_changes); 


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_policy_chg_ind_proc

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpmprx_policy_chg_ind_proc(
    cm_policyman_cfg_s          *rpt_ptr
     /* Pointer to the report sent by Message Router */
   )
{
  cm_policy_config_s_type *local_policy = NULL;
  uint8 i=0;
  policy_change_mask device_local_policy = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_0("MSGR RXD: policy_chg_ind");

  if ( rpt_ptr == NULL )
  {
    CM_MSG_HIGH_0("ERROR!!! PM -> PMPRX: Incoming policy message NULL");
    return;
  }

  /* Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("ERROR!!! PM -> PMPRX: unable to process policy as cannot allocate memory");
    return;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retrieve the policy data into a local buffer. If all the policy items in
  ** the new policy are successfully retrieved, only then update the current
  ** policy and attempt to enforce it.
  */
  if (cmpmprx_process_msgr_msg(rpt_ptr,local_policy))
  {
    #ifdef FEATURE_CM_DEBUG_BUFFER
    cmdbg_add_to_dbg_buffer(CM_BUFF_MSG_RTR_TYPE, CM_DBG_BUFF_MSGR_RTR_PM_CFG_UPDATE_IND, (void*)local_policy);
    #endif
    cmpmprx_update_curr_policy(local_policy);
  
  
    cmpmprx_enforce_current_policy();

  }
  else
  {
    CM_MSG_HIGH_0("PMPRX<< PM: MSGR return false");
  }
  cm_mem_free(local_policy);
  
  policyman_msgr_attachments_release(&(rpt_ptr->msg_hdr));

  return;
}
#endif
/*===========================================================================

FUNCTION cmpmprx_update_cm_policy_consumption_eligibility

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void   cmpmprx_update_cm_policy_consumption_eligibility_subsc(
       cm_policy_subsc_change_e_type   gwl_subsc
)
{
#ifndef FEATURE_TRIM_CMPMPRX
  boolean prev_eligibility = cmpmprx_read_cm_policy_consumption_eligibility();
  cm_policy_config_s_type *curr_policy = cmpm_ptr();

  if(gwl_subsc == CMPMPRX_SUBSC_AVAIL)
  {
    curr_policy->is_cm_eligible.gwl_subsc = TRUE;
  }
  else if(gwl_subsc == CMPMPRX_SUBSC_NOT_AVAIL)
  {
    curr_policy->is_cm_eligible.gwl_subsc = FALSE;
  }

  CM_MSG_HIGH_1("PMPRX: Updating CM eligibility to consume policy:gwl subsc:%d",
         curr_policy->is_cm_eligible.gwl_subsc);

#ifdef FEATURE_ENABLE_PM_RPL
  if ( cmpmprx_read_cm_policy_consumption_eligibility() &&
       cmpmprx_is_mask_set( PM_CHG_RAT_ACQ_ORDER, cmpm_ptr() )
	 )
  {
    CM_MSG_HIGH_0("CM_DEBUG PMPRX: rat acq changed");
    cmph_get_sub_config_ptr(SYS_MODEM_AS_ID_1)->rat_acq_order_pref
      = curr_policy->current_policy.acq_order_pref;
	cmpmprx_reset_policy_chg_flag(PM_CHG_RAT_ACQ_ORDER);
  }
#endif
  /* There are pending policy changes and CM just became eligible to consume
  ** Enforce service scan. All others will be read by subscription change.
  */
  if ( !prev_eligibility &&
       cmpmprx_read_cm_policy_consumption_eligibility() &&
       cmpmprx_is_mask_set( PM_CHG_SVC_SCAN, cmpm_ptr() ) )
  {
    cmpmprx_enforce_service_scan_mode();
  }
#endif
}

/*===========================================================================

FUNCTION cmpmprx_update_cm_policy_consumption_eligibility

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_enforce_policy_oprt_mode(void)
{
#ifndef FEATURE_TRIM_CMPMPRX
  cm_policy_config_s_type *curr_policy = cmpm_ptr();
  uint8 i=0;
  boolean is_no_op = TRUE;


  if ( curr_policy->current_policy.service_scan_mode != POLICYMAN_SVC_MODE_FULL )
  {
    cmpmprx_set_policy_chg_flag(PM_CHG_SVC_SCAN);
    is_no_op = FALSE;

/* There are pending policy changes and CM just became eligible to consume*/
    if ( cmpmprx_read_cm_policy_consumption_eligibility() &&
         cmpmprx_is_mask_set(  PM_CHG_ALL, cmpm_ptr() ) )
    {
      cmpmprx_enforce_current_policy();
    }
  }

  return (!is_no_op);
  #else
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION cmpmprx_notify_call_end - remove_feature

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_notify_call_end(

       boolean is_emerg
)
{
#ifndef FEATURE_TRIM_CMPMPRX
  
  CM_MSG_HIGH_1("PMPRX: resume PM is_em %d", is_emerg);
  if ( is_emerg == TRUE && cmpm_ptr()->is_pm_suspend)
  {
    policyman_resume();
    cmpm_ptr()->is_pm_suspend = FALSE;
  }

  if( cmpmprx_is_mask_set(  PM_CHG_ALL, cmpm_ptr() ) )
  {
    cmpmprx_enforce_current_policy();
  }

  #endif
}

/*===========================================================================

FUNCTION cmpmprx_notify_emg_call_start

DESCRIPTION
  The function is called to notify cmpmprx that emergency call has started.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmpmprx_notify_emg_call_start()
{
#ifndef FEATURE_TRIM_CMPMPRX
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();
  
  /* Suspend Policy Manager so it will not process any rules for the duration
  ** of the emergency call. 
  */
  if (!pm_ptr->is_pm_suspend)
  {
    CM_MSG_HIGH_0("PMPRX: suspend PM");
    policyman_suspend();
    pm_ptr->is_pm_suspend = TRUE;
  }
  else
  {
    CM_MSG_HIGH_0("PMPRX: PM already in suspend");
  }
#endif
}

/*===========================================================================

FUNCTION cmpmprx_read_cm_policy_consumption_eligibility

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpmprx_read_cm_policy_consumption_eligibility(void)

{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();

  //Size of array is 3 , For as_id 1, 2, 3
  return (curr_policy->is_cm_eligible.gwl_subsc);
}
#ifndef FEATURE_TRIM_CMPMPRX

/*===========================================================================

FUNCTION cmpmprx_compute_restricted_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cm_network_sel_mode_pref_e_type cmpmprx_compute_restricted_mode(

   cm_network_sel_mode_pref_e_type  net_sel_mode_pref
)
{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();
  cm_network_sel_mode_pref_e_type  pm_net_sel_mode_pref = net_sel_mode_pref;
  
  switch (curr_policy->current_policy.service_scan_mode)
  {
     case POLICYMAN_SVC_MODE_LIMITED:
       pm_net_sel_mode_pref =  CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV ;
     break;

     case POLICYMAN_SVC_MODE_CAMP_ONLY:
     case POLICYMAN_SVC_MODE_ALL:
       {
         cmph_s_type *ph_ptr = cmph_ptr();
         cmph_stack_info_s_type *stack_info;
         {
           stack_info = &(ph_ptr->main_stack_info);
         }

         if (stack_info->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL_CAMP_ONLY ||
           stack_info->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL_LIMITED_SRV ||
           stack_info->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
         {
           pm_net_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_MANUAL_CAMP_ONLY;
         }
         else
         {
           pm_net_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTO_CAMP_ONLY;
         }

       }
       break;
     case POLICYMAN_SVC_MODE_FULL:
     default:
       break;
  } /* switch */

  if(net_sel_mode_pref != pm_net_sel_mode_pref)
  {
    CM_MSG_HIGH_2("cmpmprx_compute_restricted_mode: net_sel_mode_pref %d, srv_scan:%d",
                          net_sel_mode_pref, curr_policy->current_policy.service_scan_mode);
  }
  return pm_net_sel_mode_pref;
}

/*===========================================================================

FUNCTION cmpmprx_enforce_service_scan_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_enforce_service_scan_mode(void)
{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();
  cmph_s_type             *ph_ptr = cmph_ptr();
  cm_network_sel_mode_pref_e_type rest_net_sel_mode_pref = ph_ptr->main_stack_info.pref_info.network_sel_mode_pref;

  if ( !cmpmprx_is_mask_set( PM_CHG_SVC_SCAN, cmpm_ptr() ) )
  {
    return;
  }

  cmpmprx_reset_policy_chg_flag(PM_CHG_SVC_SCAN);

  if ( curr_policy->current_policy.service_scan_mode == POLICYMAN_SVC_MODE_FULL)
  {
    cm_orig_q_s_type *pm_rest_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                          CM_ACT_TYPE_POLICYMAN_RESTRICT);
    if( pm_rest_obj_ptr != NULL )
    {
       CM_MSG_HIGH_0("<- full_rat_search mode");
       cmph_delete_update_orig_mode( CM_SS_MAIN,
                                    (cm_act_id_type) curr_policy,
                                    TRUE );
    }
  }
  else
  {
    rest_net_sel_mode_pref = cmpmprx_compute_restricted_mode( ph_ptr->main_stack_info.pref_info.network_sel_mode_pref );

    if ( cmpmprx_read_cm_policy_consumption_eligibility() )
    {
      CM_MSG_HIGH_0("-> full_rat_search mode");
      
      /* Since full rat mode does only limited acq, forcing CS only as domain pref*/   
      
      (void) cmph_force_pref_on_the_fly(CM_SS_MAIN,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_POLICYMAN_RESTRICT,
                                         ph_ptr->main_stack_info.pref_info.pref_term,
                                         ph_ptr->main_stack_info.pref_info.mode_pref,
                                         ph_ptr->main_stack_info.pref_info.band_pref,
                                         ph_ptr->main_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->main_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->main_stack_info.pref_info.plmn,
                                         CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         rest_net_sel_mode_pref,
                                         (cm_act_id_type) curr_policy,
                                         CM_ACT_UPDATE_REAS_ACT_START,
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
  }
}

/*===========================================================================

FUNCTION cmpmprx_unforce_policy

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_unforce_policy(void)
{
  cm_orig_q_s_type        *pm_rest_obj_ptr  =
                          cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                            CM_ACT_TYPE_POLICYMAN_RESTRICT);

  cmpmprx_set_policy_chg_flag( PM_CHG_ALL );

  
  if ( pm_rest_obj_ptr != NULL )
  {
    (void)cmtask_orig_para_delete(CM_SS_MAIN,pm_rest_obj_ptr,TRUE);
  }
}
#endif

/*===========================================================================

FUNCTION cmpmprx_sys_sel_pref_proc

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                         cmpmprx_sys_sel_pref_proc(

       cm_ph_cmd_info_s_type        *cmd_info_ptr,

       cm_mode_pref_e_type          prefered_mode,    

       cm_acq_pri_order_pref_s_type *rat_acq_pri_order
)
{
  cm_orig_q_s_type        *pm_rest_obj_ptr  =
                          cmtask_orig_para_search_act_type(CM_SS_MAIN,
                                                            CM_ACT_TYPE_POLICYMAN_RESTRICT);

  #if defined(FEATURE_CIOT)
  cm_network_sel_mode_pref_e_type net_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV;
  #else
  cm_network_sel_mode_pref_e_type net_sel_mode_pref = 
                cmpmprx_compute_restricted_mode( cmd_info_ptr->cmd_subs,cmd_info_ptr->network_sel_mode_pref);
  #endif
  
  if ( pm_rest_obj_ptr == NULL )
  {
    return;
  }

  CM_MSG_HIGH_0("PMPRX: Force the user preferences on PolicyMan obj too");

  (void)cmph_force_pref_on_the_fly2(
                                     CM_SS_MAX,
                                     cmph_ptr(),
                                     SD_SS_PREF_REAS_USER,
                                     CM_ACT_TYPE_POLICYMAN_RESTRICT,
                                     cmph_ptr()->main_stack_info.pref_info.pref_term,
                                     prefered_mode,
                                     cmd_info_ptr->band_pref,
                                     cmd_info_ptr->lte_band_pref,
                                     cmd_info_ptr->tds_band_pref,
                                     cmd_info_ptr->plmn,
                                     CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                     net_sel_mode_pref,
                                     (cm_act_id_type) cmpm_ptr(),
                                     CM_ACT_UPDATE_REAS_ACT_START,
                                     FALSE,
                                     rat_acq_pri_order,
                                     cmd_info_ptr->sys_sel_pref_req_id,
                                     cmd_info_ptr->csg_id,
                                     cmd_info_ptr->csg_rat,
                                     cmd_info_ptr->ciot_lte_pref);
}


/*===========================================================================

FUNCTION cmpmprx_get_num_of_sims

DESCRIPTION
  Get num of sims supported base on Policy manager configuration item.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
size_t  cmpmprx_get_num_of_sims( void )
{
  return 1;
}

/*===========================================================================

FUNCTION cmpmprx_get_num_of_active_data

DESCRIPTION
  Get num of active data supported base on Policy manager configuration item.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
size_t  cmpmprx_get_num_of_active_data( void )
{
  return 1;
}
#ifndef FEATURE_TRIM_CMPMPRX

/*===========================================================================

FUNCTION cmpmprx_check_sys_sel_pref_policy_conflict

DESCRIPTION
  If the new system selection preferences cause a policy change, check if it 
  in conflict with current device state.
  If policy change results in UE mode change and voice call ongoing, then
  mark conflict as TRUE

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
cm_ph_cmd_err_e_type                     cmpmprx_check_sys_sel_pref_policy_conflict(

  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  msgr_attach_s           *pAttach            = NULL;
  int                      i = 0;
  uint8                    j = 0;
  policy_change_mask       change_mask = 0;
  cm_sys_sel_pref_params_s_type pref_info;
  cm_ph_cmd_info_s_type*    cmd_info_ptr;
  sys_modem_as_id_e_type    asubs_id;
  cmph_s_type*              ph_ptr = cmph_ptr();
  cm_ph_cmd_err_e_type    return_val = CM_PH_CMD_ERR_NOERR;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );
  asubs_id = cmd_info_ptr->cmd_subs;

  /* Call into Policy manager and retrieve new policy if needed.
  ** Use the same preferences as used in the force_pref_on_fly() below.
  */
  pref_info.mode_pref        = cmd_info_ptr->mode_pref;
  pref_info.term_pref        = ph_ptr->main_stack_info.pref_info.pref_term;
  pref_info.pref_duration    = cmd_info_ptr->pref_duration;
  pref_info.band_pref        = cmd_info_ptr->band_pref;
  #if !defined(FEATURE_CIOT)
  pref_info.lte_band_pref    = cmd_info_ptr->lte_band_pref;
  #else
  pref_info.lte_band_pref    = cmd_info_ptr->ciot_lte_pref.lte_band_pref;
  #endif
  pref_info.tds_band_pref    = cmd_info_ptr->tds_band_pref;
  pref_info.prl_pref         = cmd_info_ptr->prl_pref;
  pref_info.roam_pref        = cmd_info_ptr->roam_pref;
  pref_info.hybr_pref        = cmd_info_ptr->hybr_pref;
  pref_info.srv_domain_pref  = cmd_info_ptr->srv_domain_pref;
  pref_info.network_sel_mode_pref = cmd_info_ptr->network_sel_mode_pref;
   /* this is to avoid sending null PLMN values to PM when network sel mode is set to no_change from manual */
  if((cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL) && ((sys_plmn_id_is_null(cmd_info_ptr->plmn))|| (sys_plmn_id_is_undefined(cmd_info_ptr->plmn))))
  {
    pref_info.plmn_ptr	   = &(ph_ptr->main_stack_info.pref_info.plmn);
  }
  else
  {
    pref_info.plmn_ptr         = &(cmd_info_ptr->plmn);
  }

  //pref_ptr->rat_acq_order_pref_ptr = &rat_acq_pri_order;
  #if !defined(FEATURE_MMODE_DISABLE_CSG)
  pref_info.csg_id           = cmd_info_ptr->csg_id;
  #endif
  pref_info.csg_rat          = cmd_info_ptr->csg_rat;

  pref_info.voice_domain_pref = cmd_info_ptr->voice_domain_pref;
  pref_info.lte_disable_cause = cmd_info_ptr->lte_disable_cause;
  pref_info.volte_enable_state = CM_VOLTE_NO_CHANGE;

  #ifdef FEATURE_CIOT
  pref_info.ciot_lte_pref.lte_op_mode_pref = cmd_info_ptr->ciot_lte_pref.lte_op_mode_pref;
  pref_info.ciot_lte_pref.lte_m1_band_pref = cmd_info_ptr->ciot_lte_pref.lte_m1_band_pref;
  pref_info.ciot_lte_pref.lte_nb1_band_pref = cmd_info_ptr->ciot_lte_pref.lte_nb1_band_pref;
  #endif
  pref_info.ue_usage_setting = CMPH_SS_UE_USAGE_SETTING(asubs_id, ph_ptr);

  
  if (cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_NO_CHANGE)
  {
    pref_info.srv_domain_pref = CMPH_SS_SRV_DOMAIN_PREF( asubs_id, ph_ptr );
  }

  if(pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ATTACH)
  {
    if( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_ONLY ) ||
          ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS ) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_PS;
    }
    else if ( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_NONE) ||
       ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_PS_ONLY) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_PS_ONLY;
    }
  }

  if(pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_DETACH)
  {
    if( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_ONLY ) ||
          ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS ) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_ONLY;
    }
    else if ( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_NONE) ||
       ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_PS_ONLY) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_NONE;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_compute_policy_with_newpref( asubs_id, &pref_info);


  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_MED_0("PMPRX: New preferences have conflict in Policy Manager, Rsp NULL");

    /* For UT verification*/
    #ifdef CM_DEBUG
    #error code not present
#endif

    return return_val;
  }
  
  for (i = 0 ; i < msgr_get_num_attach(&(pm_msgr_rsp->msg_hdr)) ; ++i)
  {
    policyman_item_collection_t const *policy_item = NULL;
    pAttach = msgr_get_attach(&(pm_msgr_rsp->msg_hdr), i);
    if (pAttach == NULL || pAttach->type != MSGR_ATTACH_REFCNT)
    {
      CM_MSG_HIGH_1("PMPRX: Invalid attachment %d.Abort new policy",i);
      change_mask = PM_CHG_CHANGE_NONE;
      break;
    }

    policy_item = pAttach->data.refcnt.refcnt_obj_ptr;

  }

  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );

  CM_MSG_HIGH_1("PMPRX: New Change mask:%d",change_mask);

  return return_val;
}
#endif
/*===========================================================================

FUNCTION cmpmprx_sys_sel_pref_policy_update

DESCRIPTION
  If the new system selection preferences cause a policy change, retrieve it
  from Policy Manager and update the cmpm_local.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
boolean                           cmpmprx_sys_sel_pref_policy_update(

       cm_sys_sel_pref_params_s_type* pref_ptr,

	    cm_ph_cmd_s_type          *ph_cmd_ptr
)
{
#ifndef FEATURE_TRIM_CMPMPRX
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  boolean                 return_val    = FALSE;
  policy_change_mask device_local_policy = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pref_ptr == NULL)
  {
    CM_MSG_HIGH_0("PMPRX: Incoming pref_ptr NULL");
    return return_val;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_report_preference_change( SYS_MODEM_AS_ID_1, pref_ptr);


  if ( pm_msgr_rsp == NULL )
  {
    return return_val;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Policy changed so we need to retrieve the new policy data items.
  ** Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to process policy as cannot allocate memory");
    return return_val;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retrieve the policy data into a local buffer. If all the policy items in
  ** the new policy are successfully retrieved, only then update the current
  ** policy and attempt to enforce it.
  */
  return_val = cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy);

  CM_MSG_HIGH_1("device local policy %d",device_local_policy);

  /* If there is a pref change while ue is in voice all and the retrieved
  ** policy has ue_mode change, dont enforce the policy. Wait for it to arrive
  ** on message router message so it wont interfere with call.
  */
  if ( return_val == TRUE &&
       cmpmprx_is_ok_to_enforce_policy( local_policy) )
  {
    CM_MSG_HIGH_0("policy_chgd: SYS_SEL_PREF");
    cmpmprx_update_curr_policy(local_policy);
  }
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );

  cm_mem_free(local_policy);
  return return_val;
  #else
  return FALSE;
  #endif
}


#ifdef FEATURE_MMODE_EMG_SUPPORT
/*===========================================================================

FUNCTION cmpmprx_emergency_call_policy_update

DESCRIPTION
  Enforce Full RAT mode on the UE.
  *** THIS IS TO BE USED ONLY FOR EMERGENCY CALL WHEN HYBRID2 NOT IN SRV ***

  Restore the original policy to cmpm_local so that when the call is ended, it
  will be enforced.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void                       cmpmprx_emergency_call_policy_update(void)
{

  cm_policy_config_s_type *emerg_policy = NULL; // Store the full rat policy.
  cm_policy_config_s_type *curr_policy = NULL; // Store the full rat policy.

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate memory for FULL RAT mode policy. */
  emerg_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  curr_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));

  if ( emerg_policy == NULL || curr_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to enforce Full RAT as cannot allocate memory");
    return;
  }

  *emerg_policy = *(cmpm_ptr());
  *curr_policy = *(cmpm_ptr());

  /* Populate FULL RAT mode for the emergency call. */
  emerg_policy->current_policy.service_scan_mode          = POLICYMAN_SVC_MODE_LIMITED;
  emerg_policy->current_policy.policy_mode_capability     = SD_SS_MODE_PREF_ALL_MODES;
  emerg_policy->current_policy.policy_band_capability     = SYS_BAND_MASK_ANY;
  emerg_policy->current_policy.policy_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;

  cmpmprx_update_curr_policy(emerg_policy);

  cm_mem_free(emerg_policy);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Enforce FULL RAT mode for the emergency call. */
  cmpmprx_enforce_current_policy();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_policy->policy_changes = PM_CHG_ALL;

  cmpmprx_update_curr_policy(curr_policy);

  cm_mem_free(curr_policy);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}
#endif

/*===========================================================================

FUNCTION cmpmprx_get_overall_featuremode

DESCRIPTION
  Called from other modules to read the current value of the overall
  featuremode in CMPM
===========================================================================*/
sys_overall_feature_t cmpmprx_get_overall_featuremode(void)
{
  return SYS_OVERALL_FEATURE_MODE_NORMAL;
}
#ifndef FEATURE_TRIM_CMPMPRX

/*===========================================================================

FUNCTION cmpmprx_clnup_complete_ind

DESCRIPTION
  CM will notify PMPRX that the cleanup for Refresh type 0 OR operating mode
  is complete and PMPRX can enforce any pending policies.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_clnup_complete_ind()
{
  if ( cmpmprx_read_cm_policy_consumption_eligibility() &&
       cmpmprx_is_mask_set( PM_CHG_ALL, cmpm_ptr() )
     )
  {
    cmpmprx_enforce_current_policy();
  }

}

/*===========================================================================

FUNCTION cmpmprx_process_thermal_policy_change

DESCRIPTION
  Handles policy change due to thermal state change and takes necessary action

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_process_thermal_policy_change(
    const cfcm_cmd_msg_type_s *cfcm_ind,
    cm_policy_config_s_type  *new_policy
    )
{
  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  unsigned int                  i           = 0;
  uint32                   removed_mode_mask;
  cmph_s_type             *ph_ptr           = cmph_ptr();
  if(cfcm_ind->fc_cmd.cmd != CFCM_CMD_DOWN ||
      !(cfcm_ind->fc_cmd.monitors_active & CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX)
     )
  {
    CM_MSG_HIGH_0("No need to end the calls in current state");
    return;
  }

  if(!cmpmprx_read_cm_policy_consumption_eligibility())
    return;

  if(cmpmprx_is_mask_set(PM_CHG_MODE_CAP,new_policy))
  {
    CM_MSG_HIGH_0("PM_CHG_MODE_CAP"); 

    removed_mode_mask = (pm_ptr->current_policy.policy_mode_capability & 
                    ~new_policy->current_policy.policy_mode_capability);

    #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)

    if(removed_mode_mask &
      cmph_map_cm_mode_pref_to_sd_mode_pref(CMPH_SS_MODE_PREF(SYS_MODEM_AS_ID_1, ph_ptr)))
    {
      cmcall_endall_non_emerg_cs_calls(CM_CALL_END_THERMAL_EMERGENCY, BM(i));
    }
    #endif
  }


}

/*===========================================================================

FUNCTION cmpmprx_thermal_state_change_policy_update

DESCRIPTION
  If the new system selection preferences cause a policy change, retrieve it
  from Policy Manager and update the cmpm_local.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
void                           cmpmprx_thermal_state_change_policy_update(

       const cfcm_cmd_msg_type_s *cfcm_ind
)
{
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  unsigned int             i            = 0;
  policy_change_mask device_local_policy = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cfcm_ind == NULL)
  {
    CM_MSG_HIGH_0("PMPRX: Incoming cfcm_ind NULL");
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_report_thermal_state( &cfcm_ind->fc_cmd);


  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_MED_0("PMPRX: New preferences have no effect on policy");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Policy changed so we need to retrieve the new policy data items.
  ** Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to process policy as cannot allocate memory");
    return ;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy) )
  {
    cmpmprx_process_thermal_policy_change(cfcm_ind,local_policy);
    cmpmprx_update_curr_policy(local_policy);
  }

  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );

  cm_mem_free(local_policy);

  if(cfcm_ind->fc_cmd.cmd == CFCM_CMD_SHUT_DOWN)
  {
    CM_MSG_HIGH_0("In Lv3: policy change will be handled after moving to emergency mode");
    return;
  }
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( cmpmprx_is_mask_set( PM_CHG_ALL, cmpm_ptr() ) )
  {
    cmpmprx_enforce_current_policy();
  }

}

/*===========================================================================

FUNCTION cmpmprx_map_volte_mode_enable_to_volte_state

DESCRIPTION
  Handles E911 Volte On/Off command from QMI-V
  and forwards it to PM 


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cm_volte_enable_e_type cmpmprx_map_volte_mode_enable_to_volte_state(boolean volte_mode_from_ap)
{ 

  if(volte_mode_from_ap == TRUE)
  {
    return CM_VOLTE_ENABLED;
  }  
  else
  {
    return CM_VOLTE_DISABLED;
  }
  
}


/*===========================================================================

FUNCTION cmpmprx_handle_volte_state_notification_cmd

DESCRIPTION
  

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                    cmpmprx_handle_volte_state_notification_cmd(
       cm_volte_state_notification_cmd_s_type *volte_state_cmd
)
{
  cm_sys_sel_pref_params_s_type *pmprx_pref_ptr = NULL;
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  sys_modem_as_id_e_type  asubs_id = volte_state_cmd->as_id;
  cmph_s_type *ph_ptr = cmph_ptr();
  boolean return_val;
  policy_change_mask device_local_policy = 0;

  if(!(volte_state_cmd->as_id > SYS_MODEM_AS_ID_NONE &&
         volte_state_cmd->as_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    CM_MSG_HIGH_0("Invalid AS_ID in volte state notification cmd");
	return;
  }

  pmprx_pref_ptr = (cm_sys_sel_pref_params_s_type*)
                         cm_mem_malloc(sizeof(cm_sys_sel_pref_params_s_type));

  if (pmprx_pref_ptr == NULL)
  {
    CM_MSG_HIGH_0("PMPRX: pmprx_pref_ptr NULL");
    return;
  }

  

  /* Fill all the fields with No_change, for fields like term_pref use from cmph_ptr */

  pmprx_pref_ptr->mode_pref        = CM_MODE_PREF_NO_CHANGE;  
  pmprx_pref_ptr->pref_duration    = 0;
  pmprx_pref_ptr->band_pref        = CM_BAND_PREF_NO_CHANGE;
  pmprx_pref_ptr->lte_band_pref    = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  pmprx_pref_ptr->tds_band_pref    = CM_BAND_PREF_NO_CHANGE;
  pmprx_pref_ptr->prl_pref         = CM_PRL_PREF_NO_CHANGE;
  pmprx_pref_ptr->roam_pref        = CM_ROAM_PREF_NO_CHANGE;
  pmprx_pref_ptr->hybr_pref        = CM_HYBR_PREF_NO_CHANGE;
  pmprx_pref_ptr->srv_domain_pref  = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  pmprx_pref_ptr->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;  
  #if !defined(FEATURE_MMODE_DISABLE_CSG)
  pmprx_pref_ptr->csg_id           = cmph_ptr()->csg_id;
  #endif
  pmprx_pref_ptr->csg_rat          = cmph_ptr()->csg_rat;

  pmprx_pref_ptr->ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
  pmprx_pref_ptr->voice_domain_pref = CMPH_SS_VOICE_DOMAIN_PREF(asubs_id,ph_ptr);
  pmprx_pref_ptr->lte_disable_cause = ph_ptr->lte_disable_cause;

  #ifdef FEATURE_CIOT
  pmprx_pref_ptr->ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NO_CHANGE;
  pmprx_pref_ptr->ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  pmprx_pref_ptr->ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  #endif


  pmprx_pref_ptr->term_pref        = cmph_ptr()->main_stack_info.pref_info.pref_term;
  pmprx_pref_ptr->plmn_ptr         = &(cmph_ptr()->main_stack_info.pref_info.plmn);

  /* fill new value of volte mode coming from AP */
  pmprx_pref_ptr->volte_enable_state = cmpmprx_map_volte_mode_enable_to_volte_state(volte_state_cmd->volte_mode_enable);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifndef CM_DEBUG
  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_report_preference_change(SYS_MODEM_AS_ID_1, pmprx_pref_ptr);
 
  #endif
  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: New Volte Mode Enable have no effect on policy");
    cm_mem_free(pmprx_pref_ptr);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Policy changed so we need to retrieve the new policy data items.
  ** Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to process policy as cannot allocate memory");
    cm_mem_free(pmprx_pref_ptr);
    return;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retrieve the policy data into a local buffer. If all the policy items in
  ** the new policy are successfully retrieved, only then update the current
  ** policy and attempt to enforce it.
  */
  return_val = cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy);

  /* If there is a pref change while ue is in voice all and the retrieved
  ** policy has ue_mode change, dont enforce the policy. Wait for it to arrive
  ** on message router message so it wont interfere with call.
  */
  if ( return_val == TRUE &&
       cmpmprx_is_ok_to_enforce_policy( local_policy))
  {
    CM_MSG_HIGH_0("policy_chgd: volte_state_noti");
    
    cmpmprx_update_curr_policy(local_policy);
    cmpmprx_check_and_enforce_policy();
  }

  #ifndef CM_DEBUG
  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );
  #endif

  cm_mem_free(local_policy);
  cm_mem_free(pmprx_pref_ptr);

  return;
}
#endif
/*===========================================================================

FUNCTION cmpmprx_get_pm_sfmode

DESCRIPTION
  API for CM to get the PM sub-feature mode configured in XML.  
  Note: For 7+5, PM sub-feature mode is different from the CM internal sub-feature mode.

DEPENDENCIES
  none

RETURN VALUE
  sys_subs_feature_t

SIDE EFFECTS
  none

===========================================================================*/
sys_subs_feature_t    cmpmprx_get_pm_sfmode(void)
{
  cm_policy_config_s_type  *pm_info = cmpm_ptr();

  return SYS_SUBS_FEATURE_MODE_NORMAL;
}
#ifndef FEATURE_TRIM_CMPMPRX
/*===========================================================================

FUNCTION cmpmprx_report_full_srv_lost

DESCRIPTION
  Reports to PM whenever Full Service Lost indication is received from SD.
  
===========================================================================*/

boolean cmpmprx_report_full_srv_lost(void)
{
  CM_MSG_HIGH_0("Reporting full_srv_lost to PM");
  policyman_report_acq_fail(SYS_MODEM_AS_ID_1, SYS_MODEM_STACK_ID_1);
  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_check_and_enforce_policy

DESCRIPTION
  Force policy if any policy element is set


DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void    cmpmprx_check_and_enforce_policy(void)
{
  cm_policy_config_s_type  *pm_info = cmpm_ptr();

  if ( cmpmprx_is_mask_set( PM_CHG_ALL, pm_info ) )
  {
    cmpmprx_enforce_current_policy();
  }

  return;
}
#endif

