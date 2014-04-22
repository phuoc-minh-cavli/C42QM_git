/*===========================================================================
                      DS_PDN_ROAMING_HDLR.C
DESCRIPTION
This file defines PDN roaming handler APIs.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_roaming_hdlr.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_roaming_hdlr.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_sys_eventi.h"
#include "ds3gmmgsdiif.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------- 
  Maximum value of PDN Disconnect Wait Time. If PDN Disconnection Wait Time is
  configured to be 0xFF, it is the client app's responsibility to deregister with
  the network (which would inturn trigger PDN Disconnect Request).
 -----------------------------------------------------------------------------*/
#define DS_PDN_ROAMING_MAX_PDN_DISCON_WAIT_TIME 0xFF

/*---------------------------------------------------------------------------- 
  This structure defines an item in the PDN Disconnect Wait Timer List.
-----------------------------------------------------------------------------*/
typedef struct
{
  list_link_type     link;         /* Pointer to the next item in list */
  uint8              profile_num;  /* Profile number for which the wait timer
                                      was running */
  ps_timer_handle_type   timer_ptr;    /* PDN Disconnect Wait Timer */
}ds_pdn_roaming_wait_timer_item_type;

/*---------------------------------------------------------------------------- 
  This structure defines an item in the Home PLMN List
-----------------------------------------------------------------------------*/
typedef struct
{
  list_link_type     link;     /* Pointer to the next item in list */
  sys_plmn_id_s_type plmn_id;  /* Home PLMN Id*/
} ds_pdn_roaming_home_plmn_list_item_type;

/*---------------------------------------------------------------------------- 
  This structure defines all the information related to the Roaming Handler
  Module.
-----------------------------------------------------------------------------*/
typedef struct
{
  boolean    roaming_status;  /* Whether UE is in Roaming or not */
  list_type  home_plmn_list;  /* List of Home PLMNs */
  list_type  wait_timer_list; /* PDN Disconnect Wait Timer List */
} ds_pdn_roaming_info_type;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------- 
  This variable declares all the information related to the Roaming Handler
  Module.
-----------------------------------------------------------------------------*/
static ds_pdn_roaming_info_type ds_pdn_roaming_info = {NULL};

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
static void ds_pdn_roaming_advertise_throt_info
(
  boolean                disable_lte_if_needed
);
  
/*===========================================================================


                               FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CONVERT_TO_PLMN_FORMAT

  DESCRIPTION
  This function converts MCC MNC to PLMN Format
  
  PARAMETERS  
  MCC MNC Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PLMN Id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_pdn_roaming_convert_to_plmn_format
(
  ds_pdn_roaming_mcc_mnc_info_s info
)
{
  sys_plmn_id_s_type plmn_id;
  /*---------------------------------------------------------------------*/

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  plmn_id.identity[0] = (byte) ((info.mcc[1] << 4) + info.mcc[0]);

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
   plmn_id.identity[1] = (byte) ((info.mnc[2] << 4) + info.mcc[2]);

  /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   plmn_id.identity[2] = (byte) ((info.mnc[1] << 4) + info.mnc[0]);

   return plmn_id;
} /* ds_3gpp_roaming_convert_to_plmn_format */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_FLUSH_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the entire wait timer list.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_flush_wait_timer_list
(
  void
)
{
  list_type                             *list_ptr = NULL;
  ds_pdn_roaming_wait_timer_item_type  *current_list_item_ptr = NULL;
  ds_pdn_roaming_wait_timer_item_type  *next_list_item_ptr = NULL;
  /*---------------------------------------------------------------------*/

  list_ptr = &ds_pdn_roaming_info.wait_timer_list;
  
  current_list_item_ptr = 
    (ds_pdn_roaming_wait_timer_item_type*)list_peek_front( list_ptr );

  while (current_list_item_ptr != NULL)
  {
    next_list_item_ptr = 
      list_peek_next(list_ptr, &(current_list_item_ptr->link));


    DS_MSG1(MSG_LEGACY_HIGH,"Clearing the wait timer memory for profile %d", 
                      current_list_item_ptr->profile_num);

    ps_timer_cancel(current_list_item_ptr->timer_ptr);

    PS_TIMER_FREE_HANDLE(current_list_item_ptr->timer_ptr);

    list_pop_item(list_ptr, &(current_list_item_ptr->link) );

    DS_SYSTEM_HEAP_MEM_FREE(current_list_item_ptr);

    current_list_item_ptr = next_list_item_ptr;
    
  }
  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CLEAR_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the wait timer corresponding to the given
  profile.
  
  PARAMETERS  
  Profile Num
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
void ds_pdn_roaming_clear_wait_timer_entry
(
  uint8 profile_num  
)
{
  list_type                             *list_ptr = NULL;
  ds_pdn_roaming_wait_timer_item_type  *current_list_item_ptr = NULL;
  /*---------------------------------------------------------------------*/

  list_ptr = &ds_pdn_roaming_info.wait_timer_list;
  
  current_list_item_ptr = 
    (ds_pdn_roaming_wait_timer_item_type*)list_peek_front( list_ptr );

  while (current_list_item_ptr != NULL)
  {
    if ( current_list_item_ptr->profile_num == profile_num )
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Clearing the wait timer memory for profile %d",
                        profile_num);

      ps_timer_cancel(current_list_item_ptr->timer_ptr);

      PS_TIMER_FREE_HANDLE(current_list_item_ptr->timer_ptr);

      list_pop_item(list_ptr, &(current_list_item_ptr->link) );

      DS_SYSTEM_HEAP_MEM_FREE(current_list_item_ptr);

      break;
    }

    current_list_item_ptr = 
      list_peek_next(list_ptr, &(current_list_item_ptr->link));
  }

  return;
}/* ds_3gpp_roaming_clear_wait_timer_entry */

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_WAIT_TIMER_EXP_CB

  DESCRIPTION
  This function callback is invoked when the PDN Disconnect wait timer
  expires.
  
  PARAMETERS  
  Callback Data
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_pdn_roaming_wait_timer_exp_cb
(
  void* cb_data
)
{
  uint8                                  profile_num = (uint8)cb_data;
  list_type                             *list_ptr = NULL;
  ds_pdn_roaming_wait_timer_item_type  *current_list_item_ptr = NULL;
  /*---------------------------------------------------------------------*/

  DS_MSG1(MSG_LEGACY_HIGH,
          "Wait timer expired, Profile number: %d. "
          "Try to tear down the PDN", profile_num);

  ds_pdn_mgr_teardown_pdn_per_profile
    (profile_num, DS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING);    

  list_ptr = &ds_pdn_roaming_info.wait_timer_list;  
  current_list_item_ptr = 
    (ds_pdn_roaming_wait_timer_item_type*)list_peek_front( list_ptr );

  while (current_list_item_ptr != NULL)
  {
    if ( current_list_item_ptr->profile_num == profile_num )
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Clearing the wait timer memory");

      PS_TIMER_FREE_HANDLE(current_list_item_ptr->timer_ptr);
      list_pop_item(list_ptr, &(current_list_item_ptr->link) );

      DS_SYSTEM_HEAP_MEM_FREE(current_list_item_ptr);
      break;
    }

    current_list_item_ptr = 
      list_peek_next(list_ptr, &(current_list_item_ptr->link));
  }  
} /* ds_pdn_roaming_wait_timer_exp_cb */

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_ALLOCATE_AND_START_WAIT_TIMER

  DESCRIPTION
  This function tries to allocate and start the wait timer.
  
  PARAMETERS  
  Profile for which the wait timer is configured.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if Wait Timer started or no need to Start Wait Timer
  FALSE, if Wait Timer Did not Start
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 
boolean ds_pdn_roaming_allocate_and_start_wait_timer
(
  ds_profile_type_s   *profile_info_ptr,
  uint8                profile_num
)
{
  ds_pdn_roaming_wait_timer_item_type *wait_timer_list_item_ptr = NULL;
  uint8                                 pdn_discon_wait_time = 0;
  boolean                               ret_val = FALSE;
  uint32                                temp_prof_num = (uint32)profile_num;
  /*----------------------------------------------------------------*/

  pdn_discon_wait_time = profile_info_ptr->pdn_discon_wait_time;

  DS_MSG2(MSG_LEGACY_HIGH, 
  	       "Profile number: %d, PDN Discon Wait Time: %d minutes", 
          profile_num, pdn_discon_wait_time );

  if (pdn_discon_wait_time != 0)
  {
    if (pdn_discon_wait_time == DS_PDN_ROAMING_MAX_PDN_DISCON_WAIT_TIME)
    {
      /* Responsibility of Client APP to perform dereg when it seems necessary */
      ret_val = TRUE;
    }
    else
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(wait_timer_list_item_ptr,
	  	                       sizeof(ds_pdn_roaming_wait_timer_item_type),
	  	                       ds_pdn_roaming_wait_timer_item_type*);

      if (wait_timer_list_item_ptr != NULL)
      {
        wait_timer_list_item_ptr->timer_ptr =  
          ps_timer_alloc (ds_pdn_roaming_wait_timer_exp_cb, 
                          (void*)temp_prof_num);

        if (wait_timer_list_item_ptr->timer_ptr != PS_TIMER_INVALID_HANDLE)
        {
          wait_timer_list_item_ptr->profile_num = profile_num;

          list_push_front(&ds_pdn_roaming_info.wait_timer_list, 
                              &(wait_timer_list_item_ptr->link));

          ps_timer_start(wait_timer_list_item_ptr->timer_ptr, 
                        pdn_discon_wait_time * 60 * 1000);
          ret_val = TRUE;
        }
      }
    }
  }

  return ret_val;
} /* ds_pdn_roaming_allocate_and_start_wait_timer */

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_BLOCK_APN_PER_PROFILE

  DESCRIPTION
  This function blocks given APN corresponding to given profile
  because of Roaming.
  
  PARAMETERS  
  Subscription id
  Profile Information Ptr
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_pdn_roaming_block_apn_per_profile
(
  ds_profile_type_s   *profile_info_ptr,
  uint8                profile_id
)
{
  ds_pdn_mgr_state_e            state = DS_PDN_MGR_STATE_DOWN;
  boolean                       tear_down_pdn = TRUE;
  ds_pdn_mgr_s*                 pdn_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s       *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s       *v6_inst = NULL;
  /*---------------------------------------------------------------------------*/

  pdn_mgr_p = ds_pdn_mgr_get_pdn_from_profile_id(profile_id);

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    state = pdn_mgr_p->state;
  }

  switch (state)
  {
    case DS_PDN_MGR_STATE_UP:
    {
      v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

      if ( (v4_inst != NULL && v4_inst->in_use) ||
           (v6_inst != NULL && v6_inst->in_use) )
      {
        if (ds_pdn_roaming_allocate_and_start_wait_timer (profile_info_ptr,
        	                                                 profile_id))
        {
          tear_down_pdn = FALSE;
        }
      }
      break;
    }

    case DS_PDN_MGR_STATE_DOWN:
    case DS_PDN_MGR_STATE_GOING_DOWN:
    {
      tear_down_pdn = FALSE;
      break;
    }

    default:
      break;
  }

  if (tear_down_pdn)
  {
    ds_pdn_mgr_teardown_pdn_per_profile(profile_id, 
                     DS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING);
  }

  ds_3gpp_pdn_throt_set_global_throt_mask
    ((byte*)profile_info_ptr->context.apn, 
       DS_3GPP_PDN_THROT_ROAMING_DISABLED_MASK);

} /* ds_pdn_roaming_block_apn_per_profile */

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_BLOCK_APNS

  DESCRIPTION
  This function blocks all APNs which are disallowed during Roaming.
  
  PARAMETERS  
  None
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_pdn_roaming_block_apns
(
  void
)
{
  ds_profile_type_s               profile_info;
  ds_profile_status_etype         result = DS_PROFILE_RESULT_FAIL;
  uint8                           i; 
  /*----------------------------------------------------------------------*/

  for (i=1; i< DS_PROFILE_MAX_PROFILE_NUM; i++)
  {
    memset(&profile_info, 0, sizeof(ds_profile_type_s));
    
    result = ds_profile_setting_from_profile_id(i, &profile_info);
    if (result != DS_PROFILE_RESULT_SUCCESS)
    {
      continue;
    }

    if(profile_info.roaming_disallowed)
    {
      DS_MSG2(MSG_LEGACY_HIGH,"Roaming Disallowed Flag: %d, Profile Id: %d ",
              profile_info.roaming_disallowed, i);
      ds_pdn_roaming_block_apn_per_profile(&profile_info, i);
    }
  }

  return;
} /* ds_pdn_roaming_block_apns */


/*===========================================================================
  FUNCTION DS_PDN_ROAMING_HDLR_CLEAR_INFO

  DESCRIPTION
  This function performs resetting of the roaming module info.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_hdlr_clear_info
(
  void
)
{
  list_type                             *list_ptr = NULL;
  ds_pdn_roaming_home_plmn_list_item_type  *current_home_plmn_list_ptr = NULL;
  ds_pdn_roaming_home_plmn_list_item_type  *next_home_plmn_list_item_ptr = NULL;  

  DS_MSG0(MSG_LEGACY_HIGH, "Clearing roaming info");

  list_ptr = &ds_pdn_roaming_info.home_plmn_list;
  if(list_ptr != NULL)
  { 
    current_home_plmn_list_ptr = 
      (ds_pdn_roaming_home_plmn_list_item_type*)list_peek_front( list_ptr );

    while (current_home_plmn_list_ptr != NULL)
    {
      next_home_plmn_list_item_ptr = 
           list_peek_next(list_ptr, &(current_home_plmn_list_ptr->link));
      list_pop_item(list_ptr, &(current_home_plmn_list_ptr->link) ); 
      DS_SYSTEM_HEAP_MEM_FREE(current_home_plmn_list_ptr);
      current_home_plmn_list_ptr = next_home_plmn_list_item_ptr;

    }
    
  }
  
  ds_pdn_roaming_flush_wait_timer_list();  

  return;
}/* ds_pdn_roaming_hdlr_clear_info */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS
  is_init          : Is it part of power up init 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_hdlr_powerup_init
(
  void
)
{
  ds_pdn_roaming_efs_info_s                 efs_info={0};
  int                                        j = 0;
  ds_pdn_roaming_home_plmn_list_item_type  *home_plmn_list_item_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  if (ds_pdn_roaming_info.home_plmn_list.list_initialized)
  {
    ds_pdn_roaming_hdlr_clear_info();
    return;
  }
  
  list_init(&(ds_pdn_roaming_info.home_plmn_list));
  if (ds_pdn_nv_manager_read_efs_nv(
                      "/nv/item_files/modem/data/3gpp/ps/home_plmn_list",
                      (unsigned char *)(&efs_info),
                      sizeof(efs_info)) != NV_DONE_S)
  {
    return;
  }
  
  for (j=0; j < efs_info.num_valid_entries; j++)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(home_plmn_list_item_ptr,
		                     sizeof(ds_pdn_roaming_home_plmn_list_item_type),
		                     ds_pdn_roaming_home_plmn_list_item_type*);

    if (home_plmn_list_item_ptr != NULL)
    {
      home_plmn_list_item_ptr->plmn_id =  
        ds_pdn_roaming_convert_to_plmn_format(efs_info.mcc_mnc_info[j]);

      list_push_front(&(ds_pdn_roaming_info.home_plmn_list), 
                      &(home_plmn_list_item_ptr->link));
    }
  }

  list_init(&ds_pdn_roaming_info.wait_timer_list);
  return;
}/* ds_3gpp_roaming_hdlr_powerup_init */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_STATUS

  DESCRIPTION
  This function tells whether the UE is Roaming or not.
  
  PARAMETERS  
  None
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE,  if UE is roaming
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_pdn_roaming_get_status
(
  void
)
{
  return ds_pdn_roaming_info.roaming_status;
} /* ds_3gpp_roaming_get_status */


/*===========================================================================
FUNCTION DS_PDN_THROT_UNBLOCK_ROAMING_THROTTLING

DESCRIPTION
  This function unblocks roaming throttling for given APN or for all
  APNs blocked due to roaming.
  
PARAMETERS
  1. APN name to be throttled
  2. Subscription Id
  3. Flag to indicate whether only the given APN should be unblocked or
     all APNs blocked due to roaming should be unblocked.
  4. Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
static void ds_pdn_roaming_unblock_throttling
(
  byte*                                apn,
  boolean                              unblock_all_apns
)
{
  uint8                                    apn_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
  /*-------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl();

  if (apn_based_tbl_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"apn_based_tbl_ptr is NULL");
    return;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if (unblock_all_apns == FALSE)
    {
      if (apn != NULL)
      {
        if (strcasecmp((char*)apn_index_ptr->apn, (char*)apn) != 0)
        {
          continue;
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"APN ptr is NULL !");
        return;
      }
    }

    if( (NULL != apn_index_ptr) && 
        (TRUE == apn_index_ptr->valid) && 
        (apn_index_ptr->global_throttle_mask &
           DS_3GPP_PDN_THROT_ROAMING_DISABLED_MASK ) )
    {
      DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing Roaming Throttling"
                              " for APN: %s", (char*)apn_index_ptr->apn);

      ds_3gpp_pdn_throt_reset_global_throt_mask
        (apn_index_ptr->apn, DS_3GPP_PDN_THROT_ROAMING_DISABLED_MASK);

      ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed
        (apn_index_ptr,FALSE);
    }
  }
  return;
} /* ds_pdn_roaming_unblock_throttling */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_SET_STATUS

  DESCRIPTION
  This function sets the roaming status.
  
  PARAMETERS  
  Roaming Status
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_set_status
(
  boolean                status
)
{
  boolean                               prev_status = FALSE;
  /*--------------------------------------------------------------------*/

  prev_status = ds_pdn_roaming_info.roaming_status;
  ds_pdn_roaming_info.roaming_status = status;

  DS_MSG2(MSG_LEGACY_HIGH, "Roaming status. Old: %d, New: %d ", 
                           prev_status, status);

  if (status != prev_status)
  {
    if (status == TRUE)
    {
      ds_pdn_roaming_block_apns();
    }
    else
    {      
      ds_pdn_roaming_flush_wait_timer_list();
      ds_pdn_roaming_unblock_throttling(NULL, TRUE);      
    }

    ds_pdn_roaming_advertise_throt_info(status);    
  }
} /* ds_3gpp_roaming_set_status */

/*===========================================================================
FUNCTION      DS_PDN_ROAMING_IS_PLMN_IN_EHPLMN_LIST

DESCRIPTION   This function checks whether the given PLMN is in EHPLMN List

DEPENDENCIES  None

RETURN VALUE  TRUE, if PLMN is in EHPLMN List
              FALSE, otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_roaming_is_plmn_in_ehplmn_list
(
  sys_plmn_id_s_type      plmn_id
)
{
  reg_sim_plmn_list_s_type   ehplmn_list = {0};
  uint32                     index = 0;
  boolean                    ret_val = FALSE;
  /*------------------------------------------------------------------------*/

  reg_sim_per_subs_ehplmn_list_get_ptr(&ehplmn_list,SYS_MODEM_AS_ID_1);

  for (index = 0; index < ehplmn_list.length; index++)
  {
    if (sys_plmn_match(plmn_id, ehplmn_list.plmn[index]) == TRUE)
    {
      ret_val = TRUE;
      break;
    }
  }

  return ret_val;
} /* ds_pdn_roaming_is_plmn_in_ehplmn_list */

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_GET_CURRENT_PLMN_LIST

  DESCRIPTION
  This function gets the current PLMN List. If current PLMN is same as RPLMN
  or part of EPLMN list, copy RPLMN and EPLMN list to current PLMN list.
  Otherwise, copy current PLMN to current PLMN list.
  
  PARAMETERS
  New PLMN Id
  Current PLMN List
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_get_current_plmn_list
(
  sys_plmn_id_s_type         new_plmn_id,
  sys_plmn_list_s_type      *curr_plmn_list_p
)
{
  sys_plmn_id_s_type         rplmn = {0};
  sys_plmn_list_s_type       eplmn_list = {0};
  boolean                    match_found = FALSE;
  uint32                     i;
  /*-----------------------------------------------------------------------*/

  if (curr_plmn_list_p == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Get EPLMN list and RPLMN
  -------------------------------------------------------------------------*/
  //sys_eplmn_per_subs_list_get(&eplmn_list, &rplmn);

  /* Compare current PLMN with RPLMN and EPLMN list */
  if (sys_plmn_match(rplmn, new_plmn_id))
  {
    match_found = TRUE;
  }
  else
  {
    for (i = 0; i < eplmn_list.length; i++)
    {
      if (sys_plmn_match(eplmn_list.plmn[i], new_plmn_id))
      {
        match_found = TRUE;
        break;
      }
    }
  }

  /* If current PLMN is same as RPLMN or part of EPLMN list, copy RPLMN and
     EPLMN list to current PLMN list, provided the eplmn-list length is less than SYS_PLMN_LIST_MAX_LENGTH.
	 Otherwise, copy current PLMN to current PLMN list
 */
  if ( match_found &&
      (eplmn_list.length < SYS_PLMN_LIST_MAX_LENGTH) )
  {
    memscpy(curr_plmn_list_p,
            sizeof(sys_plmn_list_s_type),
            &eplmn_list,
            sizeof(sys_plmn_list_s_type));
    memscpy(&curr_plmn_list_p->plmn[eplmn_list.length],
            sizeof(sys_plmn_id_s_type),
            &rplmn,
            sizeof(sys_plmn_id_s_type));
    curr_plmn_list_p->length++;
  }
  else
  {
    memscpy(&curr_plmn_list_p->plmn[0],
            sizeof(sys_plmn_id_s_type),
            &new_plmn_id,
            sizeof(sys_plmn_id_s_type));
    curr_plmn_list_p->length = 1;
  }
}/* ds_pdn_roaming_get_current_plmn_list */

/*===========================================================================
  FUNCTION ds_pdn_roaming_inform_new_pmn_info

  DESCRIPTION
  This function is used to notify the Roaming Module of PLMN change.
  
  PARAMETERS  
  Subscription Id
  New PLMN Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_inform_new_plmn_info
(
  sys_plmn_id_s_type     new_plmn_id  
)
{
  list_type                                  *list_ptr = NULL;
  ds_pdn_roaming_home_plmn_list_item_type   *item_ptr = NULL;
  boolean                                     match_found = FALSE;
  boolean                                     roaming_status = TRUE;
  boolean                                     check_ehplmn_list = FALSE;
  sys_plmn_list_s_type                        curr_plmn_list = {0};
  int                                         i;  
  ds_sys_3gpp_roaming_status_type             sys_roam_status = {0};
  /*--------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_HIGH, "Processing new PLMN Info");

  /*---------------------------------------------------------------------- 
    If Check EHPLMN List NV or Home PLMN List NV is configured and 
    the current plmn is not present in either of those lists, 
    then consider UE to be roaming.
    If current PLMN is in one of those lists, consider UE to be Home.
  --------------------------------------------------------------------*/
  check_ehplmn_list = ds_pdn_nv_manager_get_check_ehplmn_list();

  /*-------------------------------------------------------------------- 
    If EHPLMN list NV is enabled and SIM is not ready return from this
    point.
    Roaming/Home PLMN decision will be made when SIM is ready
   --------------------------------------------------------------------*/
  if((check_ehplmn_list) &&
     (FALSE == ds3g_mmgsdi_is_subs_ready()))
  {
    DS_MSG0(MSG_LEGACY_HIGH,
    	       "Subscription is not ready and check_ehplmn is TRUE, return."); 
    return;
  }

  if ((list_size(&ds_pdn_roaming_info.home_plmn_list) == 0) &&
      (check_ehplmn_list == FALSE))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Home PLMN List was not configured, ignore.");
    return;
  }

  ds_pdn_roaming_get_current_plmn_list(new_plmn_id, &curr_plmn_list);  
  
  for (i = 0; i < curr_plmn_list.length; i++)
  {
    list_ptr = &ds_pdn_roaming_info.home_plmn_list;
  
    /*-------------------------------------------------------------------- 
      Get the pointer to the head node in the list
    --------------------------------------------------------------------*/
    item_ptr = (ds_pdn_roaming_home_plmn_list_item_type*) 
                    list_peek_front( list_ptr );
  
    while (item_ptr != NULL)
    {
      if ( memcmp(&curr_plmn_list.plmn[i], &(item_ptr->plmn_id), 
                  sizeof(sys_plmn_id_s_type)) == 0 )
      {
        match_found = TRUE;      
        break;
      }
  
      item_ptr = list_peek_next(list_ptr, &(item_ptr->link));
    }
  
    if (!match_found)
    {
      if (check_ehplmn_list)
      {
        if (ds_pdn_roaming_is_plmn_in_ehplmn_list(curr_plmn_list.plmn[i]))
        {
          roaming_status = FALSE;
          break;
        }
      }
    }
    else
    {
      roaming_status = FALSE;
      break;
    }
  }

  ds_pdn_roaming_set_status(roaming_status);

  sys_roam_status.roaming_status = roaming_status;
  ds_sys_event_ind(DS_SYS_TECH_3GPP,
                   DS_SYS_3GPP_ROAMING_STATUS_EV,
                   (void *)&sys_roam_status);
  return;
} /* ds_pdn_roaming_inform_new_plmn_info */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UPDATE_PLMN_INFO_ON_SUBS_READY

  DESCRIPTION
  This function is invoked when subscription ready ev is received.
  This function calls roaming_inform_new_plnn_onfo
  which will then take action to perform blocking/unblocking.
  
  PARAMETERS  
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_roaming_update_plmn_info_on_subs_ready
(
  void
)
{
  sys_plmn_id_s_type zero_plmn_id;
  sys_plmn_id_s_type current_plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - -*/

  /*--------------------------------------------------------------------------
    Memset the Zero PLMN Id to 0
   --------------------------------------------------------------------------*/
  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  ds_pdn_hdlr_get_current_plmn(&current_plmn_id);

  if(memcmp((void*)&(current_plmn_id),
             (void*)&(zero_plmn_id),
             sizeof(sys_plmn_id_s_type)) != 0)
  {
    ds_pdn_roaming_inform_new_plmn_info(current_plmn_id);
  }

  return;
}/*ds_3gpp_roaming_update_plmn_info_on_subs_ready*/

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_UNBLOCK_APN_PER_PROFILE

  DESCRIPTION
  This function unblocks APN corresponding to the given profile.
  
  PARAMETERS  
  Subscription id
  Profile Information Ptr
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_pdn_roaming_unblock_apn_per_profile
(
  ds_profile_type_s  *profile_info_ptr,
  uint8               profile_num
)
{
  /*-------------------------------------------------------------------------*/

  if (profile_info_ptr != NULL)
  {
    ds_pdn_roaming_clear_wait_timer_entry(profile_num);

    ds_pdn_roaming_unblock_throttling((byte *)profile_info_ptr->context.apn, FALSE);
  }
} /* ds_pdn_roaming_unblock_apn_per_profile */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ADVERTISE_APNS_BLOCKED_DUE_TO_ROAMING

  DESCRIPTION
  This function advertises all Roaming Blocked APNs to its clients
  
  PARAMETERS  
  Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
void ds_pdn_roaming_advertise_apns_blocked_due_to_roaming
(
  boolean                advertise_empty_list
)
{
  ds_profile_type_s                       profile_info;
  ds_profile_status_etype                 result = DS_PROFILE_RESULT_FAIL;
  uint8                                   i = 0; 
  ds_sys_roaming_disallowed_info_type     roaming_disallowed_info = {0};
  uint8                                   num_valid_apns = 0;
  /*----------------------------------------------------------------------*/

  if (ds_pdn_roaming_get_status() && advertise_empty_list == FALSE )
  {
    for (i=1; i < DS_PROFILE_MAX_PROFILE_NUM; i++)
    {
      memset(&profile_info, 0, sizeof(ds_profile_type_s));

      result = ds_profile_setting_from_profile_id(i,&profile_info);
      if (result != DS_PROFILE_RESULT_SUCCESS)
      {
        continue;
      }

      if(profile_info.roaming_disallowed)
      {
        DS_MSG2(MSG_LEGACY_HIGH,
        	       "Roaming Disallowed Flag: %d, Profile Id: %d ",
                          profile_info.roaming_disallowed, i);
      }

      if (profile_info.roaming_disallowed && num_valid_apns < DS_SYS_MAX_APNS)
      {
        memscpy(roaming_disallowed_info.apn_list[num_valid_apns].apn_name , 
                DS_PROFILE_APN_STRING_LEN+1,
                profile_info.context.apn, 
                strlen((const char*)profile_info.context.apn) + 1);
        num_valid_apns++;
      }
    }

    if (num_valid_apns > 0)
    {
      roaming_disallowed_info.is_roaming_disallowed = TRUE;
      roaming_disallowed_info.num_apn_names = num_valid_apns;
    }
  } /* if (ds_3gpp_roaming_get_status(subs_id)) */

  DS_MSG1(MSG_LEGACY_HIGH,
  	       "Roaming Disallowed Ind. Number of Blocked APNs: %d ",
          roaming_disallowed_info.num_apn_names);

  ds_sys_event_ind(DS_SYS_TECH_3GPP,
                   DS_SYS_3GPP_ROAMING_DISALLOWED_INFO_EV,
                   (void *)&roaming_disallowed_info);

} /* ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming */

/*===========================================================================
  FUNCTION DS_PDN_ROAMING_ADVERTISE_THROT_INFO

  DESCRIPTION
  This function advertises Throttling Information to all clients and
  disables lte if needed
  
  PARAMETERS
  1. disable_lte_if_needed: Flag indicating whether LTE needs to be disabled
  2. Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 
static void ds_pdn_roaming_advertise_throt_info
(
  boolean                disable_lte_if_needed
)
{
  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();

#ifdef FEATURE_DATA_LTE
  if (disable_lte_if_needed)
  {
    ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();
  }
  else
  {
    ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available();
  }
#endif /* FEATURE_DATA_LTE */

  ds_pdn_roaming_advertise_apns_blocked_due_to_roaming(FALSE);
} /* ds_pdn_roaming_advertise_throt_info */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_PROCESS_DISALLOWED_FLAG_CHG_CMD

  DESCRIPTION
  This function is used to process the change in Roaming Disallowed Flag
  
  PARAMETERS  
  Profile Number
  Roaming Disallowed Flag
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_roaming_process_disallowed_flag_chg_cmd
(
  uint8                  profile_num,
  boolean                roaming_disallowed_flag
)
{
  ds_profile_type_s           profile_info;
  ds_profile_status_etype     result = DS_PROFILE_RESULT_FAIL;
  /*------------------------------------------------------------------------*/

  if (ds_pdn_roaming_get_status() == FALSE)
  {
    return;
  }
  
  result = ds_profile_setting_from_profile_id(profile_num, &profile_info);  
  if (result != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_ERROR_LOG_1("Cannot read profile num =d ", profile_num);
    return;
  }
  
  if (roaming_disallowed_flag == TRUE)
  {
    ds_pdn_roaming_block_apn_per_profile(&profile_info, profile_num);   
  }
  else
  {
    ds_pdn_roaming_unblock_apn_per_profile(&profile_info, profile_num);
  }

  ds_pdn_roaming_advertise_throt_info(roaming_disallowed_flag);
  
  return;
} /* ds_3gpp_roaming_process_disallowed_flag_chg_cmd */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_APN_DISALLOWED

  DESCRIPTION
  This function checks whether PDN Connection to given APN is allowed or not.
  
  PARAMETERS
  APN Name
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if PDN Connection is allowed
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_pdn_roaming_is_apn_disallowed
(
  char    *apn
)
{
  ds_profile_type_s               profile_info;
  ds_profile_status_etype         result = DS_PROFILE_RESULT_FAIL;
  uint8                           i; 
  /*----------------------------------------------------------------------*/

  if (ds_pdn_roaming_get_status() == FALSE)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"UE is not roaming. "
            "No need to process change to disallowed flag");
    return FALSE;
  }

  for (i=1; i< DS_PROFILE_MAX_PROFILE_NUM; i++)
  {
    memset(&profile_info, 0, sizeof(ds_profile_type_s));
    
    result = ds_profile_setting_from_profile_id(i, &profile_info);
    if (result != DS_PROFILE_RESULT_SUCCESS)
    {
      continue;
    }

    if( (profile_info.roaming_disallowed) &&
        (memcmp(profile_info.context.apn, apn, strlen(apn) + 1) == 0) )
    {
      DS_MSG2(MSG_LEGACY_HIGH,"Roaming Disallowed Flag: %d, Profile Id: %d ",
              profile_info.roaming_disallowed, i);
      return TRUE;
    }
  }
  
  return FALSE;
} /* ds_3gpp_roaming_is_apn_disallowed */

