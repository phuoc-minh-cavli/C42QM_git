/*===========================================================================
                      DS_PDN_APM.C
DESCRIPTION
This file defines APM APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_apm.c#3 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_apm.h"
#include "ds_pdn_apm_efs_parser.h"
#include "ds_pdn_nv_manager.h"
#include "queue.h"
#include "dssysmgr.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/

static q_type ds_pdn_apm_q = {{NULL}};
static q_type attach_profile_q = {{NULL}};
static q_type ds_pdn_apm_apn_priority_q = {{NULL}};


/*===========================================================================
Constant that defines the default PLMN blocking Interval in case NAS does 
not notify the changed value to MH 
===========================================================================*/
#define    DS_PDN_APM_PLMN_BLOCKING_INTERVAL               720000

/*===========================================================================
Constant that defines the Invalid PLMN blocking Interval 
===========================================================================*/
#define    DS_PDN_APM_INVALID_T3402_TIMER_VAL              0

/*===========================================================================
Default LTE attach profile 
===========================================================================*/
#define    DEFAULT_LTE_ATTACH_PROFILE                   1

/*---------------------------------------------------------------------------
  Maximum length of any profile parameter value. Refer to
  WDS_MAX_PROFILE_PARAM_VAL_LEN defined in ds_qmi_wds_profile.c
---------------------------------------------------------------------------*/
#define    APM_MAX_PROFILE_PARAM_VAL_LEN               150

/*------------------------------------------------------------------------
This is the singleton instance in UE to remember all information that needs 
to be APM based.
------------------------------------------------------------------------*/
ds_pdn_apm_info_type    apm_info = {{0}};  

/*===========================================================================
FUNCTION      ds_pdn_compare_apn_type

DESCRIPTION   This is a utility to compare APN type of PDNs in APM rule
              queue 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_pdn_compare_apn_type
(
  void *item_ptr, 
  void *target_val
);

/*===========================================================================
FUNCTION       ds_pdn_apm_reset_and_copy_attach_info

DESCRIPTION    This utility function resets the ATTACH PDN info.
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_reset_and_copy_attach_info
(
  void
);

/*===========================================================================
FUNCTION       ds_pdn_apm_set_all_apn_blocked_val

DESCRIPTION    Set the value of if all_apn_blocked
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_all_apn_blocked_val
( 
  boolean                   val 
);

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ALL_ATTACH_APN_BLOCKED

DESCRIPTION    This utility function checks if all APNs configured in EFS file 
               blocked. It is called whenever getting forbidden APN list from
               LTE MH.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_if_all_attach_apn_blocked
(
  q_type           *priority_q
);

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_APN_AVAILABLE

DESCRIPTION    This utility function checks if attach APN is available and why 
               it is not available. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_pdn_apm_if_attach_apn_available
(
  q_type           *priority_q
);

/*===========================================================================
FUNCTION       ds_pdn_apm_is_apn_switch_support(void)

DESCRIPTION    Indicate if APN switch is supported or not
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_is_apn_switch_support
(
  void
);

/*===========================================================================
FUNCTION      ds_pdn_apm_next_attach_pdn_change

DESCRIPTION   This is a utility changes the next attach pdn
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_next_attach_pdn_change
(
  attach_pdn_type           next_attach_pdn
)
{
  boolean  switching_flag = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(apm_info.next_attach_pdn_info.permanent == TRUE) 
  {
    return;
  }

  switch(next_attach_pdn)
  {
    case ATTACH_PDN_PRIMARY:
      if(apm_info.next_attach_pdn_info.next_attach_pdn_type !=
                                                            ATTACH_PDN_PRIMARY)
      {
        switching_flag = TRUE;
        apm_info.next_attach_pdn_info.next_attach_pdn_type = 
                                                          ATTACH_PDN_PRIMARY;
      }
      break;
  
    case ATTACH_PDN_NEXT:
      if(apm_info.next_attach_pdn_info.next_attach_pdn_type != 
                                                                 ATTACH_PDN_NEXT)
      {
        switching_flag = TRUE;
        apm_info.next_attach_pdn_info.next_attach_pdn_type = 
                                                               ATTACH_PDN_NEXT;
      }
      break;
  
    default:
      DS_ERROR_LOG_1("Invalid PDN:=d", next_attach_pdn);
      break;
  }

  if( switching_flag )
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Switching APN Type to: %d", next_attach_pdn);
  }
  return;
}/* ds_dsd_next_attach_pdn_chage()*/


/*===========================================================================
FUNCTION      ds_pdn_apm_next_attach_pdn_unset_permanent

DESCRIPTION   This is a utility unsets permanent flag on next attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_next_attach_pdn_unset_permanent
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(apm_info.next_attach_pdn_info.permanent != FALSE)
  {
    apm_info.next_attach_pdn_info.permanent = FALSE;
  }
  return;
}/*ds_pdn_apm_next_attach_pdn_unset_permanent()*/

/*===========================================================================
FUNCTION      ds_pdn_apm_set_ip_rule_required

DESCRIPTION   This is a utility to set ip rule required for current attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_set_ip_rule_required
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  apm_info.attach_pdn_info.ip_rule_required = TRUE;

  return;
}/* ds_pdn_apm_set_ip_rule_required() */

/*===========================================================================
FUNCTION      ds_pdn_apm_unset_ip_rule_required

DESCRIPTION   This is a utility unset rule required for current attach PDN.
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_unset_ip_rule_required
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG0(MSG_LEGACY_HIGH,
            "ds_pdn_apm_unset_ip_rule_required");

  apm_info.attach_pdn_info.ip_rule_required = FALSE;

  return;
}/* ds_dsd_apm_unset_ip_rule_requiredl() */
/*===========================================================================
FUNCTION      ds_pdn_set_default_apm_rule

DESCRIPTION   This is a utility to set default APm rule if configure error 
              or APN switch feature is turned off. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_set_default_apm_rule
(
  uint16                    attach_profile_id,
  q_type*                   apm_rule_q
)
{
  ds_pdn_apm_data_block*               rule_block = NULL;
  ipRequiredType                       value = 0;
  boolean                              pdn_type_valid = FALSE;
  ds_profile_type_s                    prof_info = {{0}};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG1(MSG_LEGACY_HIGH,
            "Set APM default rule for attach_profile_id:%d",
            attach_profile_id);

  apm_info.dsd_apm_default_apm_rule_enabled = TRUE;

  if (apm_rule_q == NULL)
  {
    return;
  }

  if ( ds_profile_setting_from_profile_id( attach_profile_id,
  	                                        &prof_info) == DS_PROFILE_RESULT_SUCCESS )
  {
    switch(prof_info.context.pdp_type)
    {
      case DS_PROFILE_PDP_IPV4:
        value = ipv4_mandatory_and_ipv6_donotcare;
        pdn_type_valid = TRUE;
        break;
      case DS_PROFILE_PDP_IPV6:
        value = ipv6_mandatory_and_ipv4_donotcare;
        pdn_type_valid = TRUE;
        break;
      case DS_PROFILE_PDP_IPV4V6:
        value = ipv4_mandatory_or_ipv6_mandatory;
        pdn_type_valid = TRUE;
      case DS_PROFILE_PDP_NON_IP:
        value = ipv4_dontcare_and_ipv6_donotcare;
        pdn_type_valid = TRUE;
        break;
      default:
        DS_ERROR_LOG_1("pdn type =d is invalid, no default APM rule", 
        	              prof_info.context.pdp_type);
    }
  }
  
  if( pdn_type_valid == TRUE )
  {

    DS_SYSTEM_HEAP_MEM_ALLOC(rule_block, sizeof(ds_pdn_apm_data_block),
		                     ds_pdn_apm_data_block*);
	

    if( rule_block == NULL )
    {
      DS_ERR_FATAL("Allocate memory for default APM rule fail");
      return;
    }

    memset(rule_block, 0, sizeof(ds_pdn_apm_data_block));

    DS_MSG1(MSG_LEGACY_HIGH,"Put default APM rule to apm queue, IP rule %d",
              value);
   /*-----------------------------------------------------------------------
      Need to initialize the cause code queue in the APM Rule Block
    -----------------------------------------------------------------------*/
    q_init(&(rule_block->apn_info.cause_code_info.cause_code_info_q));
    rule_block->apn_info.apn_type = ATTACH_PDN_DEFAULT;
    rule_block->apn_info.AttachPdn.config_rule.config_valid_flag = 
      CONFIG_IP_ADDRESS;
    rule_block->apn_info.AttachPdn.config_rule.ip_address = value;
    rule_block->apn_info.profile_id = attach_profile_id;
   /*-----------------------------------------------------------------------
    Get the attach APN name by checking the 3GPP profile database
    -----------------------------------------------------------------------*/
    strlcpy(rule_block->apn_info.apn_name,
            prof_info.context.apn,
            DS_PDN_APM_MAX_APN_STRING_LEN);
    rule_block->apn_info.apn_name_len = 
    	                    (uint8)strlen((char*)prof_info.context.apn);

    q_link(rule_block, &(rule_block->link));
    q_put(apm_rule_q, &(rule_block->link));
  }
  return;
} /*ds_pdn_set_default_apm_rule()*/

/*===========================================================================
FUNCTION       DS_DSD_APM_FREE_CAUSE_CODE_Q_FOR_RULE_BLOCK

DESCRIPTION    This utility function frees queue items in the queue for
               the passed subs

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_free_cause_code_q_for_rule_block
(
  q_type                               *cause_code_q_ptr
)
{
  ds_pdn_apm_cause_code_info_type      *cause_code_data_block = NULL;
  ds_pdn_apm_cause_code_info_type      *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != cause_code_q_ptr)
  {
    cause_code_data_block = (ds_pdn_apm_cause_code_info_type  *)
                                                  q_check(cause_code_q_ptr);

    while (cause_code_data_block != NULL)
    {
      prev_block = cause_code_data_block;
      cause_code_data_block = (ds_pdn_apm_cause_code_info_type *)
                                        q_next(cause_code_q_ptr,
                                        &(cause_code_data_block->link));
      q_delete(cause_code_q_ptr, &(prev_block->link));
      DS_SYSTEM_HEAP_MEM_FREE (prev_block);
    }
    q_destroy(cause_code_q_ptr);
  }

  return;
} /* ds_dsd_apm_free_cause_code_q_for_rule_block */

/*===========================================================================
FUNCTION      ds_pdn_apm_delete_default_apm_rule

DESCRIPTION   This is a utility to delete default APM rule
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_delete_default_apm_rule 
(
  q_type                    *rule_q 
)
{
  ds_pdn_apm_data_block   *rule_block = NULL;
  attach_pdn_type         default_type;

  default_type = ATTACH_PDN_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( rule_q == NULL)
  {
    DS_ERROR_LOG_0("APM rule queue ptr is NULL, return");
    return;
  }
  apm_info.dsd_apm_default_apm_rule_enabled = FALSE;

  rule_block = (ds_pdn_apm_data_block*)q_linear_search ( &ds_pdn_apm_q,
                                                ds_pdn_compare_apn_type,
                                                         &default_type);

  if( rule_block != NULL )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Delete default APM rule in apm rule queue");
    ds_pdn_apm_free_cause_code_q_for_rule_block(&(rule_block->apn_info.
                                                  cause_code_info.cause_code_info_q));
    q_delete(rule_q, &(rule_block->link));
    DS_SYSTEM_HEAP_MEM_FREE(rule_block);
  }

  return;
}/*ds_pdn_apm_delete_default_apm_rule*/

/*===========================================================================
FUNCTION      ds_pdn_apm_update_ip_rule

DESCRIPTION   This is a utility to update IP rule

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void ds_pdn_apm_update_ip_rule
(
  ds_profile_pdp_type_e  attach_pdp_type
)
{
  ds_pdn_apm_data_block  *apm_rule_block_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Update IP rule is called only for default APM rule or Single Queue
    entry scenarios and in this cases APM Rules queue will have only one entry
  -------------------------------------------------------------------------*/
  apm_rule_block_p = (ds_pdn_apm_data_block *)q_check(&ds_pdn_apm_q);

  if( apm_rule_block_p!= NULL )
  {
    /* Update rule only if Subs id matches AND IP address configuration is valid */
    if( (apm_rule_block_p->apn_info.AttachPdn.config_rule.config_valid_flag
           & CONFIG_IP_ADDRESS) )
    {
      switch( attach_pdp_type )
      {
        case DS_PROFILE_PDP_IP:
          apm_rule_block_p->apn_info.AttachPdn.config_rule.ip_address
            = ipv4_mandatory_and_ipv6_donotcare;
          break;
        case DS_PROFILE_PDP_IPV6:
          apm_rule_block_p->apn_info.AttachPdn.config_rule.ip_address
            = ipv6_mandatory_and_ipv4_donotcare;
          break;
        case DS_PROFILE_PDP_IPV4V6:
          apm_rule_block_p->apn_info.AttachPdn.config_rule.ip_address
            = ipv4_mandatory_or_ipv6_mandatory;
          break;
        default:
          DS_ERROR_LOG_1("Invalid attach pdp type:=d", attach_pdp_type);
      }
    }
  }

  return;
} /* ds_pdn_apm_update_ip_rule */

/*===========================================================================
FUNCTION      ds_pdn_next_attach_pdn_set_permanent

DESCRIPTION   This is a utility sets permanent flag on next attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_next_attach_pdn_set_permanent
(
  void
)
{
  apm_info.next_attach_pdn_info.permanent = TRUE;
  return;
}/*ds_pdn_next_attach_pdn_set_permanent()*/

/*===========================================================================
FUNCTION      ds_pdn_apm_set_plmn_blocking_interval_timer

DESCRIPTION   This is a utility sets the PLMN blocking Timer Value 
 
PARAMETERS    timer_val:         T3402 Timer value given by MH 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_apm_set_plmn_blocking_interval_timer
(
  uint32                        timer_val
)
{
  apm_info.ds_dsd_apm_plmn_blocking_interval = timer_val;
  return;
}/*ds_pdn_apm_set_plmn_blocking_interval_timer()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_GET_PLMN_BLOCKING_INTERVAL_TIMER

DESCRIPTION   This is a utility gets the PLMN blocking Timer Value 
 
PARAMETERS    NONE
 
DEPENDENCIES  None.

RETURN VALUE  timer_val:         T3402 Timer value given by MH

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_pdn_apm_get_plmn_blocking_interval_timer
(
  void
)
{
  return apm_info.ds_dsd_apm_plmn_blocking_interval;
}/*ds_dsd_apm_get_plmn_blocking_interval_timer*/

/*===========================================================================
FUNCTION      DSD_APM_COMPARE_APN_NAME

DESCRIPTION   Utility function to compare apn name in queue search.
 
DEPENDENCIES           
           
PARAMETERS    item_ptr: block of apn_list (type: ds_pdn_apm_apn_block_info_type)
              target_val: block of priority queue (type: ds_pdn_apm_apn_block_info_type) 

RETURN VALUE  1: the items are equal
              0: the items are not equal

SIDE EFFECTS  
===========================================================================*/
int ds_pdn_compare_apn_name
(
  void *item_ptr, 
  void *target_val
)
{
  int                             ret_val = FALSE;
  apn_priority_data_block         *apn_block;
  ds_pdn_apm_apn_block_info_type  *forbidden_apn_block;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL )
  {
    return ret_val;
  }

  apn_block = (apn_priority_data_block*)item_ptr;
  forbidden_apn_block = (ds_pdn_apm_apn_block_info_type*)target_val;

  if( apn_block->apn_block_info.apn_len == forbidden_apn_block->apn_len)
  {
    if ( apn_block->apn_block_info.apn_len == 0) 
    {
      ret_val = TRUE;
    }
    else
    {
      if( 0 == strncasecmp( apn_block->apn_block_info.apn_name,
                            forbidden_apn_block->apn_name,
                            apn_block->apn_block_info.apn_len) )
      {
        ret_val = TRUE;
      }
    }
  }

  return ret_val;
} /* ds_pdn_compare_apn_name() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_APN_NAME_IP_TYPE_BLOCKED

DESCRIPTION    This utility function to check if a certain type of APN 
               is blocked.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_if_apn_name_ip_type_blocked
(
  char                                       *name,
  uint16                                      name_len,
  uint16                                      ip_type,
  boolean                                    *blocked
)
{
  apn_priority_data_block                     *apn_info_block = NULL;
  ds_pdn_apm_apn_block_info_type              temp_apn_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( name == NULL || blocked == NULL ) 
  {
    DS_ERROR_LOG_0("Passed name or blocked buff is NULL");
    return FALSE;
  }

  /*------------------------------------------------------------------
   This function checks for a given APN whether a partiuclar IP type
   has been blocked or not.
   
   We extract the entry from APN priority_q and perform the required
   checks
  -------------------------------------------------------------------*/ 
  memset(&temp_apn_info, 0, sizeof(ds_pdn_apm_apn_block_info_type));
  temp_apn_info.apn_len = name_len;
  strlcpy(temp_apn_info.apn_name, 
          name, 
          DS_PDN_APM_MAX_APN_STRING_LEN);

  apn_info_block = 
    (apn_priority_data_block*)q_linear_search(&ds_pdn_apm_apn_priority_q,
                                              ds_pdn_compare_apn_name,
                                              &(temp_apn_info));
  if ( apn_info_block == NULL ) 
  {
    *blocked = FALSE;
  }
  else
  {
    if( (ip_type & ~(apn_info_block->apn_block_info.ip_type_blocked)) == 0 ) 
    {
      *blocked = TRUE;
    }
    else
    {
      *blocked = FALSE;
    }
  }

  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"APN: %s, ip_type: %d ",
                       name, ip_type);
  DS_MSG1(MSG_LEGACY_HIGH,"ds_dsd_apm is apn name ip type blocked: %d ",*blocked);

  return TRUE;

}/* ds_dsd_apm_if_apn_name_ip_type_blocked() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_PROFILE_ID_AVAILABLE

DESCRIPTION    This utility function to check if a certain profile is available.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info  ds_pdn_apm_if_profile_id_available
( 
  uint8                               profile_id
)
{
  uint16                               ip_type = 0;
  boolean                              blocked = FALSE;
  attach_apn_availability_info         prof_avail = 0;  
  ds_profile_type_s                    prof_info = {{0}};
  boolean                              status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG1(MSG_LEGACY_HIGH,
            "ds_pdn_apm_if_profile_id_available : profile_id %d",
            profile_id);

  if ( ds_profile_setting_from_profile_id( profile_id,
  	                                        &prof_info) == DS_PROFILE_RESULT_SUCCESS )
  {
    status = TRUE;
    switch(prof_info.context.pdp_type)
    {
      case DS_PROFILE_PDP_IPV4:
        ip_type = V4_PDN_TYPE;
        break;
      case DS_PROFILE_PDP_IPV6:
        ip_type = V6_PDN_TYPE;
        break;
      case DS_PROFILE_PDP_IPV4V6:
        ip_type = V4_V6_PDN_TYPE;
        break;
      case DS_PROFILE_PDP_NON_IP:
      default:
        ip_type = V4_V6_PDN_TYPE;
        break;
    }
  }
  else
  {
    ip_type = V4_V6_PDN_TYPE;
  }

  if( status == TRUE ) 
  {
    if( ds_pdn_apm_if_apn_name_ip_type_blocked(prof_info.context.apn,
                                     strlen((char*)prof_info.context.apn),
                                               ip_type,
                                               &blocked) == TRUE )
    {
      if ( blocked == TRUE ) 
      {
        prof_avail = DS_PDN_APM_ATTACH_APN_BLOCKED;
      }
      else
      {
        prof_avail = DS_PDN_APM_ATTACH_APN_AVAILABLE;
      }
    }
    else
    {
      prof_avail = DS_PDN_APM_PROF_CONFIG_ERROR;
    }
    
    DS_MSG_SPRINTF_4(MSG_LEGACY_HIGH,"ds_dsd_apm profile id: %d, APN name: %s,"
                     " Length: %d, blocked %d",
                     profile_id, prof_info.context.apn, 
                     strlen(prof_info.context.apn), blocked);
  }
  else
  {
    prof_avail = DS_PDN_APM_PROF_CONFIG_ERROR;
  }

  return prof_avail;
}/* ds_dsd_apm_if_profile_id_available() */

/*===========================================================================
FUNCTION      ds_pdn_apm_is_apn_blocked

DESCRIPTION   This is a utility that checks if the APN for a particular prof 
              is blocked or not 
 
PARAMETERS    profile_id:        Profile whose APN blocked status has to be 
                                 checked 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:              If APN is blocked from attach 
              FALSE:             If APN is not blocked

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_apm_is_apn_blocked
(
  uint8                            profile_id
)
{
  boolean                           ret_val = FALSE;
  attach_apn_availability_info      avail_info = 
                                           DS_PDN_APM_ATTACH_APN_AVAILABLE;
/*-------------------------------------------------------------------------*/
  avail_info = ds_pdn_apm_if_profile_id_available (profile_id);

  if(avail_info == DS_PDN_APM_ATTACH_APN_BLOCKED)
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION      ds_pdn_apm_is_apn_disabled

DESCRIPTION   This is a utility that checks if the APN for a particular prof 
              is disabled or not 
 
PARAMETERS    profile_id:        Profile whose APN disable status has to be 
                                 checked 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:              If APN is disabled 
              FALSE:             If APN is not disabled 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_apm_is_apn_disabled
(
   uint8                          profile_id
)
{
  attach_profile_data_block        *profile_block = NULL;
  boolean                           ret_val = FALSE;
/*-------------------------------------------------------------------------*/

  profile_block = (attach_profile_data_block *)q_check(&attach_profile_q);

  while (profile_block != NULL)
  {
    if (profile_block->attach_profile.profile == profile_id &&
        profile_block->attach_profile.apn_disabled == TRUE)
    {
      ret_val = TRUE;
      break;
    }
    profile_block = (attach_profile_data_block *)q_next(&attach_profile_q,
                                                      &(profile_block->link));
  }

  return ret_val;
}


/*===========================================================================
FUNCTION       ds_pdn_apm_set_generic_switching_flag

DESCRIPTION    To Enable or disable Generic APN Switching 
 
PARAMETERS     generic_apn_switch: Flag containing information about enable/
                                   disable status
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_generic_switching_flag
(
  boolean                   generic_apn_switch
)
{
  apm_info.dsd_apm_generic_switching_status = generic_apn_switch;
  DS_MSG1(MSG_LEGACY_HIGH,
            "Generic APN Switch flag : %d", generic_apn_switch);
  return;
}/* ds_pdn_apm_set_generic_switching_flag */

/*===========================================================================
FUNCTION      ds_pdn_apm_action_rule

DESCRIPTION   This is a utility to run action
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_action_rule
(
  uint16                          action
)
{
  boolean                         switch_to_primary = FALSE;
  boolean                         switch_to_next = FALSE;
  attach_pdn_type                 next_pdn_type = ATTACH_PDN_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(action & (uint16)ACTION_CHANGE_TO_PRIMARY)
  {
    switch_to_primary = TRUE;
  }

  if(action & (uint16)ACTION_CHANGE_TO_NEXT)
  {
    switch_to_next = TRUE;
  }
  
  if(switch_to_primary == TRUE)
  {
    next_pdn_type = ATTACH_PDN_PRIMARY;
  }
  else
  {
    if(switch_to_next == TRUE)
    {
      next_pdn_type = ATTACH_PDN_NEXT;
    }
  }

  DS_MSG2(MSG_LEGACY_HIGH,
            "Switching APN,action_rule: 0x%x,Next APN Type: %d",
            action, next_pdn_type);

  ds_pdn_apm_next_attach_pdn_change(next_pdn_type);

  if((action  & (uint16)ACTION_INIT_DETACH) &&
     (action  & (uint16)ACTION_BLOCK_PLMN))
  {
    ds_pdn_apm_dsc_command(DS_DSC_PLMN_BLOCKING_AND_PS_DETACH,
                       ds_pdn_apm_get_plmn_blocking_interval_timer());
  }
  else if((action  & (uint16)ACTION_INIT_DETACH))
  {
    ds_pdn_apm_dsc_command(DS_DSC_PS_DETACH,
                       DS_PDN_APM_INVALID_T3402_TIMER_VAL);
  }
  else if((action  & (uint16)ACTION_BLOCK_PLMN))
  {
    ds_pdn_apm_dsc_command(DS_DSC_PLMN_BLOCKING,
                       ds_pdn_apm_get_plmn_blocking_interval_timer());
  }
  return;
}/* ds_pdn_apm_action_rule()*/

/*===========================================================================
FUNCTION      ds_pdn_compare_apn_type

DESCRIPTION   This is a utility to compare APN type of PDNs in APM rule
              queue 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_pdn_compare_apn_type
(
  void *item_ptr, 
  void *target_val
)
{
  ds_pdn_apm_data_block   *rule_block = NULL;
  attach_pdn_type         *apn_type;
  int                      ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (item_ptr == NULL) || (target_val == NULL) )
  {
    return ret_val;
  }

  rule_block = (ds_pdn_apm_data_block*)item_ptr;
  apn_type = (attach_pdn_type*)target_val;

  if( rule_block->apn_info.apn_type == *apn_type)
  {
    ret_val = TRUE;
  }

  return ret_val;  
}/* ds_pdn_compare_apn_type() */
/*===========================================================================
FUNCTION      ds_pdn_compare_apn_name_rule_q

DESCRIPTION   This is a utility to compare APN name with Rule queue APN name
 
DEPENDENCIES  None.

RETURN VALUE  TRUEL If a match occurs 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
int ds_pdn_compare_apn_name_rule_q
(
  void                   *item_ptr, 
  void                   *target_val
)
{
  ds_pdn_apm_data_block  *rule_block = NULL;
  char                   *apn_name = NULL;
  int                     ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL ) 
  {
    return ret_val;
  }
  rule_block = (ds_pdn_apm_data_block *)item_ptr;
  apn_name = (char *)target_val;

  if( rule_block->apn_info.apn_name_len == strlen(apn_name) )
  {
    if(strncasecmp(apn_name, rule_block->apn_info.apn_name,
                   rule_block->apn_info.apn_name_len) == 0)
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
}/*ds_pdn_compare_apn_name_rule_q*/


/*===========================================================================
FUNCTION      ds_pdn_compare_profile_id

DESCRIPTION   This is a utility to compare APN type of PDNs in APM rule           
              queue 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_pdn_compare_profile_id
(
  void *item_ptr, 
  void *target_val
)
{
  ds_pdn_apm_data_block   *rule_block = NULL;
  uint8                   *profile_id = NULL;
  boolean                  ret_val = FALSE; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (item_ptr == NULL) || (target_val == NULL) )
  {
    return ret_val;
  }

  rule_block = (ds_pdn_apm_data_block*)item_ptr;
  profile_id = (uint8*)target_val;

  if( rule_block->apn_info.profile_id == *profile_id )
  {
    ret_val = TRUE;
  }

  return ret_val;  
}/* ds_pdn_compare_profile_id() */

/*===========================================================================
FUNCTION      ds_pdn_apm_rule

DESCRIPTION   This is a utility to run rules
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_rule
(
  ds_pdn_apm_apn_rule       rule
)
{
  apn_info_type          *apn_info_ptr = NULL; 
  attach_pdn_info_type   *attach_pdn_info_ptr = NULL;
  ds_pdn_apm_data_block  *rule_block = NULL;
  attach_pdn_type         attach_pdn;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  attach_pdn_info_ptr = &apm_info.attach_pdn_info;
  attach_pdn = attach_pdn_info_ptr->pdn_type;

  rule_block = (ds_pdn_apm_data_block*)q_linear_search( &ds_pdn_apm_q,
                                                ds_pdn_compare_apn_type,
                                                           &attach_pdn);
  if( rule_block == NULL )
  {
    DS_ERROR_LOG_0("Current attach PDN does not have rule");
    return;
  }
  apn_info_ptr = &(rule_block->apn_info);

  DS_MSG2(MSG_LEGACY_HIGH,"Current attach PDN type %d, rule %d", 
            attach_pdn_info_ptr->pdn_type,
            rule);

  switch(rule)
  {
    case  DS_PDN_APM_RULE_IP_ADDRESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
          CONFIG_IP_ADDRESS))
      {
        DS_MSG2(MSG_LEGACY_HIGH,
                  "IP RULE CONFIG SUPPORTED, ip address v4 %d, v6 %d",
                  attach_pdn_info_ptr->ipv4_address,
                  attach_pdn_info_ptr->ipv6_address);
        switch(apn_info_ptr->AttachPdn.config_rule.ip_address)
        {
          case ipv4_mandatory_and_ipv6_donotcare:
            if(attach_pdn_info_ptr->ipv4_address == DS_PDN_APM_RCVD_FAIL)
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;
          case ipv6_mandatory_and_ipv4_donotcare:
            if(attach_pdn_info_ptr->ipv6_address == DS_PDN_APM_RCVD_FAIL)
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;
          case ipv4_mandatory_and_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_address == DS_PDN_APM_RCVD_FAIL) || 
               (attach_pdn_info_ptr->ipv6_address == DS_PDN_APM_RCVD_FAIL))
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;
          
          case ipv4_mandatory_or_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_address == DS_PDN_APM_RCVD_FAIL) && 
               (attach_pdn_info_ptr->ipv6_address == DS_PDN_APM_RCVD_FAIL))
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;

          default:
            break;
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH,"CONFIG NOT SUPPORTED -- DEFAULT RULE");
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.ip_address);
      }

      break;
    case DS_PDN_APM_RULE_PCSCF_ADDRESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF))
      {
        DS_MSG2(MSG_LEGACY_HIGH,"PCSCF rule CONFIG SUPPORTED, "
                  "pcscf address v4 %d, v6 %d ",
                  attach_pdn_info_ptr->ipv4_pcscf_address,
                  attach_pdn_info_ptr->ipv6_pcscf_address);
        switch(apn_info_ptr->AttachPdn.config_rule.pcscf_address)
        {
          case ipv4_mandatory_and_ipv6_donotcare:
            if(attach_pdn_info_ptr->ipv4_pcscf_address == DS_PDN_APM_RCVD_FAIL)
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;
          case ipv6_mandatory_and_ipv4_donotcare:
            if(attach_pdn_info_ptr->ipv6_pcscf_address == DS_PDN_APM_RCVD_FAIL)
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;
          case ipv4_mandatory_and_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_pcscf_address == DS_PDN_APM_RCVD_FAIL) || 
               (attach_pdn_info_ptr->ipv6_pcscf_address == DS_PDN_APM_RCVD_FAIL))
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;
          
          case ipv4_mandatory_or_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_pcscf_address == DS_PDN_APM_RCVD_FAIL) && 
               (attach_pdn_info_ptr->ipv6_pcscf_address == DS_PDN_APM_RCVD_FAIL))
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;

          default:
            break;
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH,"CONFIG NOT SUPPORTED -- DEFAULT RULE");
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.pcscf_address);
      }
      break;

    case DS_PDN_APM_RULE_OOS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_OOS)
          == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.out_of_service);
      }
      break;

    case DS_PDN_APM_RULE_PLMN:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PLMN)
         == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.plmn_change);
      }
      break;

    case DS_PDN_APM_RULE_SYS_CHANGE:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_SYS_CHG) == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.sys_change);
      }
      break;

    case DS_PDN_APM_RULE_ATTACH_SUCCESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_ATTACH_SUCCESS) == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.attach_success);
      }
      break;

    case DS_PDN_APM_RULE_ATTACH_FAIL:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_ATTACH_FAIL) == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.attach_fail);
      }
      break;

    case DS_PDN_APM_RULE_DETACH_NORMAL:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_DETACH_NORMAL) == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.detach_normal);
      }
      break;

    case DS_PDN_APM_RULE_RESET_APN:
      if((q_cnt(&(apn_info_ptr->cause_code_info.cause_code_info_q)) > 0) == FALSE)
      {
        ds_pdn_apm_action_rule(apn_info_ptr->AttachPdn.
                             action_rule.reset_attach_apn);
      }
      break;
      
    case  DS_PDN_APM_RULE_DNS_ADDRESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
          CONFIG_DNS_ADDRESS))
      {
        DS_MSG3(MSG_LEGACY_HIGH,
                  "DNS Address CONFIG SUPPORTED, dns addr v4 %d, v6 %d reqd %d",
                  attach_pdn_info_ptr->ipv4_dns_address,
                  attach_pdn_info_ptr->ipv6_dns_address,
                  apn_info_ptr->AttachPdn.config_rule.dns_address);

        switch(apn_info_ptr->AttachPdn.config_rule.dns_address)
        {
          case ipv4_mandatory_and_ipv6_donotcare:
            if(attach_pdn_info_ptr->ipv4_dns_address == DS_PDN_APM_RCVD_FAIL)
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;
          case ipv6_mandatory_and_ipv4_donotcare:
            if(attach_pdn_info_ptr->ipv6_dns_address == DS_PDN_APM_RCVD_FAIL)
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;
          case ipv4_mandatory_and_ipv6_mandatory:
            if(
               (attach_pdn_info_ptr->ipv4_dns_address == DS_PDN_APM_RCVD_FAIL)||
               (attach_pdn_info_ptr->ipv6_dns_address == DS_PDN_APM_RCVD_FAIL)
               )
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;

          case ipv4_mandatory_or_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_address == DS_PDN_APM_RCVD_FAIL) && 
               (attach_pdn_info_ptr->ipv6_address == DS_PDN_APM_RCVD_FAIL))
            {
              ds_pdn_apm_action_rule(apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;

          default:
            break;
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH,"CONFIG NOT SUPPORTED -- DEFAULT RULE");
        ds_pdn_apm_action_rule(apn_info_ptr->
                               AttachPdn.action_rule.dns_address);
      }

    break;
      
    case DS_PDN_APM_RULE_PCSCF_DNS_ADDRESS:
      if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
                                             CONFIG_PCSCF_DNS_ADDRESS)
      {

        switch(apn_info_ptr->AttachPdn.config_rule.pcscf_or_dns_address)
        {
          case ipv4_mandatory_or_ipv6_mandatory:
            ds_pdn_apm_action_rule(apn_info_ptr->
                                   AttachPdn.action_rule.pcscf_or_dns_address);
            break;

          default:
            break;
        }
      }
      break;

    default:
      DS_ERROR_LOG_1("Invalid rule :=d", rule);
      break;
  }
  return;
}/* ds_pdn_apm_rule()*/

/*===========================================================================
FUNCTION      DSD_APM_COMPARE_PROFILE_AND_RULE

DESCRIPTION   Utility function to compare apn name in queue search.
 
DEPENDENCIES           
           
PARAMETERS    item_ptr: block of APM rule (type: ds_pdn_apm_data_block)
              target_val: apn info in profile (type: ds_profile_info_type) 

RETURN VALUE  1: the items are equal
              0: the items are not equal

SIDE EFFECTS  
===========================================================================*/
int ds_pdn_compare_profile_and_rule
(
  void *item_ptr, 
  void *target_val
)
{
  int                             ret_val = FALSE;
  ds_pdn_apm_data_block           *rule_block;
  char                            *apn_block;
  uint8                            apn_len = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL )
  {
    return ret_val;
  }

  rule_block = (ds_pdn_apm_data_block*)item_ptr;
  apn_block = (char*)target_val;
  apn_len = strlen((char*)apn_block);
  
  if( rule_block->apn_info.apn_name_len == apn_len )
  {
    if( 0 == strncasecmp( rule_block->apn_info.apn_name,
                          apn_block,
                          apn_len) )
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
} /* ds_pdn_compare_profile_and_rule() */

/*===========================================================================
FUNCTION      DS_DSD_COMPARE_PROFILE_ID_PROF_Q

DESCRIPTION   This is a utility to compare the attach profile ID in attach 
              profile queue 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:: If a match is found 
              FALSE:: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
int ds_pdn_apm_compare_profile_id_prof_q
(
  void                          *item_ptr, 
  void                          *target_val
)
{
  attach_profile_data_block     *profile_block = NULL;
  uint8                         *profile_id = NULL;
  int                            ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (item_ptr == NULL) || (target_val == NULL) )
  {
    return ret_val;
  }

  profile_block = (attach_profile_data_block  *)item_ptr;
  profile_id = (uint8*)target_val;

  if(profile_block->attach_profile.profile == *profile_id )
  {
    ret_val = TRUE;
  }
  
  return ret_val;  
}/* ds_pdn_apm_compare_profile_id_prof_q() */

/*===========================================================================
FUNCTION      DS_DSD_APM_FILL_APN_DISABLE_INFO

DESCRIPTION   This function populates the values of APN Disable Flag in attach 
              profile queue 
 
PARAMETERS    profile_block: attach profile block whose APN disable flag is to 
                             be filled 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Modification of attach_profile_q contents
===========================================================================*/
void ds_pdn_apm_fill_apn_disable_info
(
  void
)
{
   attach_profile_data_block             *profile_block = NULL;
   ds_profile_type_s                      prof_info = {{0}};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  profile_block = (attach_profile_data_block *)q_check(&attach_profile_q);
 
  while(profile_block != NULL)
  {        
    if ( ds_profile_setting_from_profile_id( profile_block->attach_profile.profile,
                                             &prof_info) == DS_PROFILE_RESULT_SUCCESS )
    {
      profile_block->attach_profile.apn_disabled = prof_info.apn_disable_flag;
    }

    profile_block = (attach_profile_data_block *)q_next(&attach_profile_q,
                                                        &(profile_block->link));
  }
}

/*===========================================================================
FUNCTION      ds_pdn_apm_match_attach_profile_and_rule

DESCRIPTION   Try to match APM rule with attach profile based on APN name 
              and populate APN class to main APM structure queue. 
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_match_attach_profile_and_rule
(
  q_type                    *rule_q,
  q_type                    *profile_q,
  uint16                    *error_code
)
{
  ds_pdn_apm_data_block        *rule_entry = NULL;
  attach_profile_data_block    *prof_entry = NULL;
  uint8                         profile_id;
  ds_profile_type_s            prof_info = {{0}};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (profile_q == NULL) || (rule_q == NULL) )
  {
    *error_code |= EFS_PROFILE_RULE_MISMATCH;
    return;
  }

  prof_entry = (attach_profile_data_block* )q_check(profile_q);
  if( prof_entry == NULL )
  {
    *error_code |= EFS_PROFILE_RULE_MISMATCH;
    return;
  }

  while( prof_entry != NULL)
  {
    profile_id = prof_entry->attach_profile.profile;

    if ( ds_profile_setting_from_profile_id(profile_id,
                                            &prof_info) == DS_PROFILE_RESULT_SUCCESS )
    {
      rule_entry = (ds_pdn_apm_data_block*)q_linear_search(rule_q,
                                       ds_pdn_compare_profile_and_rule,
                                       prof_info.context.apn);
      if( rule_entry != NULL )
      {
        DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                           "Found the matching rule with apn name %s ,profile %d",
                           prof_info.context.apn, profile_id);
      /*------------------------------------------------------------------
           Find matching PDN rules, and save profile id and PDN type
           to APM rule queue
          -------------------------------------------------------------------*/
        rule_entry->apn_info.apn_type = (attach_pdn_type)(prof_info.apn_class);
        rule_entry->apn_info.profile_id = profile_id;
        DS_MSG2(MSG_LEGACY_HIGH,
                  "Get the apn type %d to APM rule with profile id %d ", 
                  rule_entry->apn_info.apn_type,
                  rule_entry->apn_info.profile_id);
      }
      else
      {
          *error_code |= EFS_PROFILE_RULE_MISMATCH;
          break;
      }
    }
    else
    {
      *error_code |= EFS_PROFILE_RULE_MISMATCH;
      break;
    }
    prof_entry = q_next(profile_q, &(prof_entry->link));
  }

  if( *error_code )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Attach profile and APM rule in EFS is not "
              "matching, no APN switching",
              *error_code);
  }
  return;
}/*ds_pdn_apm_match_attach_profile_and_rule*/

/*===========================================================================
FUNCTION      ds_pdn_apm_config_get_attach_prof_num

DESCRIPTION   returns the number of a attach profiles in the queue 
 
DEPENDENCIES  None

RETURN VALUE  uint16 - Number of attach profiles in the queue

SIDE EFFECTS  None
===========================================================================*/
uint16 ds_pdn_apm_config_get_attach_prof_num
(
  q_type                  *attach_profile_q
)
{
  uint16                       num_attach_profs = 0;
  attach_profile_data_block   *attach_prof_block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(attach_profile_q != NULL)
  {
     attach_prof_block = (attach_profile_data_block *)q_check(attach_profile_q);

     while(attach_prof_block != NULL)
     {
       num_attach_profs++;
       attach_prof_block = (attach_profile_data_block *)
                             q_next(attach_profile_q,&(attach_prof_block->link));
     }
  }
  DS_MSG1(MSG_LEGACY_HIGH,
            "number of attach profs %d ", num_attach_profs);
  return num_attach_profs;
}/* ds_pdn_apm_config_get_attach_prof_num() */

/*===========================================================================
FUNCTION      ds_pdn_apm_config_get_apm_rules_num

DESCRIPTION   returns the number of a attach rules in the queue 
 
DEPENDENCIES  None

RETURN VALUE  uint16 - Number of attach rules in the queue

SIDE EFFECTS  None
===========================================================================*/
uint16 ds_pdn_apm_config_get_apm_rules_num
(
  q_type                  *rule_q
)
{
  uint16                 num_rule_q = 0;
  ds_pdn_apm_data_block  *block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( rule_q != NULL)
  {
    block = (ds_pdn_apm_data_block*)q_check(rule_q);
    while (block != NULL)
    {
      num_rule_q++;
      block = (ds_pdn_apm_data_block*)q_next(rule_q, &(block->link));
    }
  }

  DS_MSG1(MSG_LEGACY_HIGH,
            "number of attach rules %d ", num_rule_q);  
  return num_rule_q;
}/* ds_pdn_apm_config_get_apm_rules_num() */

/*===========================================================================
FUNCTION      ds_pdn_apm_config_validate_attach_pdn_class

DESCRIPTION   Check whether the passed PDN class is Class 1 or Class 3 
 
PARAMETERS    pdn_type: PDN Class to be validated 
 
DEPENDENCIES  None

RETURN VALUE  TRUE: If the passed PDN type is Class 1/Class 3 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_apm_config_validate_attach_pdn_class
(
  attach_pdn_type           pdn_type
)
{
  boolean                   ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( (pdn_type !=  ATTACH_PDN_PRIMARY) &&
       (pdn_type !=  ATTACH_PDN_NEXT) )
  {
    DS_MSG1(MSG_LEGACY_HIGH,"APN class %d is not valid",
              pdn_type);
    ret_val = FALSE;
  }
  return ret_val;
} /* ds_pdn_apm_config_validate_attach_pdn_class */

/*===========================================================================
FUNCTION       ds_pdn_apm_validate_queue_cnt

DESCRIPTION    This utility function checks to see if queue elements exist 
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE if there are queue items 
                         FALSE if no queue items exists

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_validate_queue_cnt
(
  q_type                    *rule_q
)
{
  attach_profile_data_block  *profile_block = NULL;
  uint8                       cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  profile_block = (attach_profile_data_block*)q_check(rule_q);

  if (NULL != profile_block) 
  {
    while (profile_block != NULL) 
    {
      cnt++;
      profile_block = (attach_profile_data_block*)q_next(rule_q, 
                                                   &(profile_block->link));
    }

    if (cnt > 0) 
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  return FALSE;
}/*ds_pdn_apm_validate_queue_cnt*/

/*===========================================================================
FUNCTION       ds_pdn_apm_get_last_rule_block

DESCRIPTION    This utility function traverses the rule queue to return a 
               pointer to last block
 
PARAMETERS     NONE
 
DEPENDENCIES   NONE

RETURN VALUE   last_block: Last rule block in the queue

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_data_block* ds_pdn_apm_get_last_rule_block
(
  void
)
{
   ds_pdn_apm_data_block                  *rule_block = NULL;
   ds_pdn_apm_data_block                  *last_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   rule_block = (ds_pdn_apm_data_block *)q_check (&ds_pdn_apm_q);

   while(rule_block != NULL)
   {
     last_block = rule_block;
     rule_block = (ds_pdn_apm_data_block *)q_next(&ds_pdn_apm_q, 
                                                  &(rule_block->link));
   }

   return last_block;
}/*ds_pdn_apm_get_last_rule_block_per_subs*/
/*===========================================================================
FUNCTION       ds_pdn_apm_free_block_queue

DESCRIPTION    This utility function frees queue items in the queue
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_free_block_queue
(
  q_type                     *rule_q
)
{
  apn_priority_data_block     *block = NULL;
  apn_priority_data_block     *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != rule_q) 
  {
    block = (apn_priority_data_block *)q_check(rule_q);

    while (block != NULL)
    {
      prev_block = block;
      block = (apn_priority_data_block *)q_next(rule_q,&(block->link));
      q_delete(rule_q, &(prev_block->link));
      DS_SYSTEM_HEAP_MEM_FREE (prev_block);
    }
  }

  return;
} /* ds_pdn_apm_free_block_queue */

/*===========================================================================
FUNCTION       ds_pdn_apm_free_rule_queue

DESCRIPTION    This utility function frees queue items in the queue
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_free_rule_queue
(
  q_type                    *rule_q
)
{
  ds_pdn_apm_data_block     *block = NULL;
  ds_pdn_apm_data_block     *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != rule_q) 
  {
    block = (ds_pdn_apm_data_block*)q_check(rule_q);

    while (block != NULL)
    {
      prev_block = block;
      block = (ds_pdn_apm_data_block *)q_next(rule_q,&(block->link));
      /*---------------------------------------------------------------------
              Free the cause code queue for a rule block entry
          ---------------------------------------------------------------------*/
      ds_pdn_apm_free_cause_code_q_for_rule_block(&(prev_block->apn_info.
                                          cause_code_info.cause_code_info_q));
      q_delete(rule_q, &(prev_block->link));
      DS_SYSTEM_HEAP_MEM_FREE (prev_block);
    }
  }
  return;
} /* ds_pdn_apm_free_rule_queue */

/*===========================================================================
FUNCTION       ds_pdn_apm_free_prof_queue

DESCRIPTION    This utility function frees queue items in the queue
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_free_prof_queue
(
  q_type                          *rule_q
)
{
  attach_profile_data_block       *block = NULL;
  attach_profile_data_block       *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != rule_q) 
  {
    block = (attach_profile_data_block *)q_check(rule_q);
    while (block != NULL)
    {
      prev_block = block;
      block = (attach_profile_data_block *)q_next(rule_q,&(block->link));
      q_delete(rule_q, &(prev_block->link));
      DS_SYSTEM_HEAP_MEM_FREE (prev_block);
    }
  }
  return ;
} /* ds_pdn_apm_free_prof_queue */

/*===========================================================================
FUNCTION       ds_pdn_apm_is_internal_update_allowed

DESCRIPTION    This internal function checks the APM SM state and decides 
               whether internal structures can be updated or not 
 
PARAMETERS     NONE

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF Update is permitted 
               FALSE: If update is not allowed 

SIDE EFFECTS   None
===========================================================================*/
boolean ds_pdn_apm_is_internal_update_allowed
(
  void
)
{
  boolean                       ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(apm_info.attach_pdn_info.state == ATTACH_PDN_STATE_DISCONNECTED
      || apm_info.attach_pdn_info.state == ATTACH_PDN_STATE_INVALID)
  {
    ret_val = TRUE;
  }

  DS_MSG2(MSG_LEGACY_HIGH,"ds_dsd_apm update internal allowed: %d ,state: %d ",
               ret_val,apm_info.attach_pdn_info.state);
  return ret_val;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_is_apn_in_attach_pdn_list

DESCRIPTION    This function checks whether a given APN is an attach APN
 
PARAMETERS     apn_name: APN which has to be checked

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF APN passed is Attach APN
               FALSE: Otherwise

SIDE EFFECTS   None
===========================================================================*/
boolean ds_pdn_apm_is_apn_in_attach_pdn_list
(
  char                       *apn_name
)
{
  ds_pdn_apm_data_block      *rule_block = NULL;
  boolean                     ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rule_block = (ds_pdn_apm_data_block *)q_linear_search(&ds_pdn_apm_q,
                                               ds_pdn_compare_apn_name_rule_q,
                                               apn_name);

  if(rule_block != NULL)
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION       DSD_APM_IS_GENERIC_APN_SWITCHING_ENABLED

DESCRIPTION    This function checks whether Generic APN Switching has been 
               enabled or not 
 
PARAMETERS     NONE

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF Generic APN Switching is enabled
               FALSE: Otherwise

SIDE EFFECTS   None
===========================================================================*/
boolean ds_pdn_apm_is_generic_apn_switching_enabled
(
  void
)
{

  return apm_info.dsd_apm_generic_switching_status;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_print_apm_q_content

DESCRIPTION    This is a utility function to print the contents of the apm 
               queues. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_print_apm_q_content
(
  void
)
{
  ds_pdn_apm_data_block      *apm_data_block = NULL;

  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_apm_q item cnt:%d",q_cnt(&ds_pdn_apm_q));

  apm_data_block = (ds_pdn_apm_data_block*)q_check(&ds_pdn_apm_q);
  while(apm_data_block != NULL)
  {
    DS_MSG3(MSG_LEGACY_HIGH,
              "apn_name: %s apn_type:%d profile_id:%d",
              apm_data_block->apn_info.apn_name,
              apm_data_block->apn_info.apn_type,
              apm_data_block->apn_info.profile_id);

    apm_data_block = (ds_pdn_apm_data_block*)q_next(&ds_pdn_apm_q,
                                                    &(apm_data_block->link));
  }

  return;
} /* ds_pdn_apm_print_apm_q_content */

/*===========================================================================
FUNCTION      ds_pdn_apm_rule_reset_apn_class

DESCRIPTION   Clean mapped APN class and profile ID in APM rule queue
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_rule_reset_apn_class
(
  q_type                  *q
)
{
  ds_pdn_apm_data_block          *block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( q != NULL )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Reset APN class to be invalid in APM rule queue");
    block = (ds_pdn_apm_data_block*)q_check(q);
    while( block != NULL )
    {
      block->apn_info.apn_type = ATTACH_PDN_INVALID;
      block->apn_info.profile_id = 0;
      block = (ds_pdn_apm_data_block*)q_next(q, &(block->link));
    }
  }
  return;
} /*ds_pdn_apm_rule_reset_apn_class()*/

/*===========================================================================
FUNCTION      ds_pdn_apm_set_class_dependent_action_rules

DESCRIPTION   This function sets the action rules depending on the 
              APN Class
 
PARAMETERS    apn_info : Rule block for whom the action rules have to be set
 
DEPENDENCIES  None

RETURN VALUE  uint16: contain an error code value if any

SIDE EFFECTS  Setting of Action rules in APM Rule queue block
===========================================================================*/
uint16 ds_pdn_apm_set_class_dependent_action_rules
(
   apn_info_type                 *apn_info
)
{
  uint16                          config_error = 0;
/*-------------------------------------------------------------------------*/
  if(apn_info == NULL)
  {
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  switch( apn_info->apn_type )
  {
    case ATTACH_PDN_PRIMARY:
      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
      {
      apn_info->AttachPdn.action_rule.ip_address =
        (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
      {
      apn_info->AttachPdn.action_rule.pcscf_address = 
        (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.dns_address = 
            (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.pcscf_or_dns_address = 
            (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      apn_info->AttachPdn.action_rule.out_of_service = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.plmn_change = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.sys_change = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.attach_fail = 
        (uint16)ACTION_CHANGE_TO_NEXT;

      apn_info->AttachPdn.action_rule.attach_success = (uint16)0;

      apn_info->AttachPdn.action_rule.detach_normal = (uint16)0;

      apn_info->AttachPdn.action_rule.attach_blocked = (uint16)
                                                 ACTION_CHANGE_TO_NEXT;

      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                 ACTION_CHANGE_TO_PRIMARY;
      break;

    case ATTACH_PDN_NEXT:
      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
      {
      apn_info->AttachPdn.action_rule.ip_address = 
        (uint16)ACTION_INIT_DETACH | (uint16)ACTION_CHANGE_TO_PRIMARY |
        (uint16)ACTION_BLOCK_PLMN;
      }

      apn_info->AttachPdn.action_rule.pcscf_address = (uint16)0;

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.dns_address = 
            (uint16)ACTION_CHANGE_TO_PRIMARY | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.pcscf_or_dns_address = 
            (uint16)ACTION_CHANGE_TO_PRIMARY | (uint16)ACTION_INIT_DETACH;
      }

      apn_info->AttachPdn.action_rule.out_of_service = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.plmn_change =
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.sys_change = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.attach_fail = (uint16)0;

      apn_info->AttachPdn.action_rule.attach_success = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.detach_normal = (uint16)0;

      apn_info->AttachPdn.action_rule.attach_blocked = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;
                      
      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                 ACTION_CHANGE_TO_PRIMARY;
                      
      break;

    default:
      config_error = EFS_APM_RULE_APN_CLASS_ERR;
  }
  return config_error;
}

/*===========================================================================
FUNCTION      ds_pdn_apm_set_generic_action_rules

DESCRIPTION   This function sets the generic action rules for Switching on 
              attach failures 
 
PARAMETERS    apn_info : Rule block for whom the action rules have to be set
 
DEPENDENCIES  None

RETURN VALUE  uint16: contain an error code value if any

SIDE EFFECTS  Setting of Action rules in APM Rule queue block
===========================================================================*/
uint16 ds_pdn_apm_set_generic_action_rules 
(
   apn_info_type                 *apn_info
)
{
  ds_pdn_apm_data_block          *apm_rule_block = NULL;
  uint16                          config_error = 0;
/*---------------------------------------------------------------------------*/
  if(apn_info == NULL)
  {
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  apm_rule_block = ds_pdn_apm_get_last_rule_block();

  if(apm_rule_block == NULL)
  {
    config_error = EFS_APM_RULE_APN_CLASS_ERR;
  }
  else
  { 
    /*------------------------------------------------------------------------ 
     If the current rule block is last entry in the rule queue, then we
     set action rules differently
    -------------------------------------------------------------------------*/
    if(memcmp((void *)&(apm_rule_block->apn_info), (void *)apn_info,
              sizeof(apn_info_type)) == 0)
    {
      apn_info->AttachPdn.action_rule.attach_fail = (uint16)0;
      apn_info->AttachPdn.action_rule.attach_blocked = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.attach_success = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.plmn_change = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.sys_change = (uint16) 
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.out_of_service = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
    }
    else
    {
      apn_info->AttachPdn.action_rule.attach_fail = (uint16)
                                                     ACTION_CHANGE_TO_NEXT;
      apn_info->AttachPdn.action_rule.attach_blocked = (uint16)
                                                     ACTION_CHANGE_TO_NEXT;
      apn_info->AttachPdn.action_rule.plmn_change = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.sys_change = (uint16) 
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.out_of_service = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
    }
  }

  return config_error;
}

/*===========================================================================
FUNCTION      ds_pdn_apm_set_cause_code_based_action_rules

DESCRIPTION   This function sets the action rules for Switching on basis
              on Attach PDN & RAT agnostic rules
 
PARAMETERS    apn_info : Rule block for whom the action rules have to be set
 
DEPENDENCIES  None

RETURN VALUE  uint16: contain an error code value if any

SIDE EFFECTS  Setting of Action rules in APM Rule queue block
===========================================================================*/
uint16 ds_pdn_apm_set_cause_code_based_action_rules
(
   apn_info_type                 *apn_info_ptr
)
{
  ds_pdn_apm_data_block          *apm_rule_block_ptr = NULL;
  uint16                          config_error = 0;
  uint16                          ip_addr_pcscf_addr_action_rule = 0;
  uint16                          attach_proc_action_rule = 0;
/*---------------------------------------------------------------------------*/
  if(apn_info_ptr == NULL)
  {
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  apm_rule_block_ptr = ds_pdn_apm_get_last_rule_block();

  if(apm_rule_block_ptr == NULL)
  {
    config_error = EFS_APM_RULE_APN_CLASS_ERR;
  }
  else
  { 
    /*------------------------------------------------------------------------ 
     If the current rule block is last entry in the rule queue, then we
     set action rules differently
    -------------------------------------------------------------------------*/
    if(memcmp((void *)&(apm_rule_block_ptr->apn_info), (void *)apn_info_ptr,
              sizeof(apn_info_type)) == 0)
    {
      ip_addr_pcscf_addr_action_rule = ACTION_CHANGE_TO_PRIMARY |
                                       ACTION_INIT_DETACH;
      attach_proc_action_rule = ACTION_CHANGE_TO_PRIMARY;
    }
    else
    {
      ip_addr_pcscf_addr_action_rule = ACTION_CHANGE_TO_NEXT | 
                                       ACTION_INIT_DETACH;
      attach_proc_action_rule = ACTION_CHANGE_TO_NEXT;
    }
    /*------------------------------------------------------------------------ 
     If the masks were set during EFS read then the action rules are configured
     for the APN
    -------------------------------------------------------------------------*/
    if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
    {
      apn_info_ptr->AttachPdn.action_rule.ip_address =
        (uint16)ip_addr_pcscf_addr_action_rule;
    }
    if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
    {
      apn_info_ptr->AttachPdn.action_rule.pcscf_address = 
        (uint16)ip_addr_pcscf_addr_action_rule;
    }
    if(apn_info_ptr->cause_code_info.config_valid_flag & CONFIG_ATTACH_FAIL)
    {
      apn_info_ptr->AttachPdn.action_rule.attach_fail = (uint16)
                                                     attach_proc_action_rule;
    }
    if(apn_info_ptr->cause_code_info.config_valid_flag & CONFIG_DETACH_NORMAL)
    {
      apn_info_ptr->AttachPdn.action_rule.detach_normal = (uint16)
                                                     attach_proc_action_rule;
    }
    apn_info_ptr->AttachPdn.action_rule.attach_blocked = (uint16)
                                                   attach_proc_action_rule;
  }

  return config_error;
}

/*===========================================================================
FUNCTION ds_pdn_apm_set_single_attach_prof_action_rules

DESCRIPTION
  This function sets the action rules for a single attach profile scenario

PARAMETERS 
  apn_info_ptr: APM rule queue block for which action rules need to be set

DEPENDENCIES
  None

RETURN VALUE
  uint16 - mask containing config error if any
   
SIDE EFFECTS
  None
===========================================================================*/
uint16 ds_pdn_apm_set_single_attach_prof_action_rules
(
   apn_info_type                 *apn_info_ptr
)
{
  uint16                          config_error = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(apn_info_ptr == NULL)
  {
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  apn_info_ptr->AttachPdn.action_rule.attach_blocked = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
  /*------------------------------------------------------------------------ 
   If the masks were set during EFS read then the action rules are configured
   for the APN
  -------------------------------------------------------------------------*/
  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
  {
    /*--------------------------------------------------------------------
      Per PLMN throttling if enabled then this will be vzw imsless config
      hence blocking plmn in case ip address alloc failure is required
    ---------------------------------------------------------------------*/ 
    if(ds_pdn_nv_manager_get_rel_10_throttling_per_plmn_from_nv())
    {
      DS_MSG0(MSG_LEGACY_HIGH,
	            "Per plmn throttling is enabled, set plmn-block ip-addr action rule");
      apn_info_ptr->AttachPdn.action_rule.ip_address = 
              (uint16)ACTION_INIT_DETACH | (uint16)ACTION_CHANGE_TO_PRIMARY |
              (uint16)ACTION_BLOCK_PLMN;
    }
    else
    {
      apn_info_ptr->AttachPdn.action_rule.ip_address =
        (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
    }
  }
  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
  {
    apn_info_ptr->AttachPdn.action_rule.pcscf_address = 
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
  {
    apn_info_ptr->AttachPdn.action_rule.dns_address = 
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
                                                       CONFIG_PCSCF_DNS_ADDRESS)
  {
    apn_info_ptr->AttachPdn.action_rule.pcscf_or_dns_address = 
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }

  return config_error;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_START_BLOCK_RULE_Q

DESCRIPTION    This utility function traverses the rule queue to return a 
               pointer to first block for a particular subscription
 
PARAMETERS      NONE
 
DEPENDENCIES   NONE

RETURN VALUE   first_block: first rule block in the queue for given subs

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_data_block* ds_pdn_apm_get_start_block_rule_q
(
  void
)
{
  ds_pdn_apm_data_block                     *rule_block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rule_block = (ds_pdn_apm_data_block *)q_check(&ds_pdn_apm_q);

  return rule_block;
}

/*===========================================================================
FUNCTION      DSD_APM_SET_ACTION_RULE

DESCRIPTION   This function sets the action rules depending on the 
              configuration
 
PARAMETERS    
              rule_q:       APM Rule queue pointer
              config_error: If Generic APN switching is disabled we dont set
                            action rules and revert to default rule
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Setting of Action rules in APM Rule queue
===========================================================================*/
void ds_pdn_apm_set_action_rules
(
  q_type                         *rule_q,
  uint16                         *config_error
)
{
  ds_pdn_apm_data_block          *rule_block = NULL;
  apn_info_type                  *apn_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( rule_q == NULL )
  {
    return;
  }
  rule_block = (ds_pdn_apm_data_block*)q_check(rule_q);
  
  /*------------------------------------------------------------------------ 
   Here we set the action rules for VZW and Generic APN Switching config.
   1. If the APN Class is either Class 1/Class 3, we set VZW action rules
   2. Else if Generic APN switching is enabled, we set generic switching rules
   3. Else if we have reject/detach cause codes configured for APN switching
   4. Else if we check if we have a single APM rule entry then we need to
      set switching rules as per configuration in EFS file
   5. Else we return configuration error and revert to default APM Rule
   6. Else we return configuration error and revert to default APM Rule
  -------------------------------------------------------------------------*/
  while( rule_block != NULL )
  {
    apn_info = &(rule_block->apn_info);
    DS_MSG1(MSG_LEGACY_HIGH,"Setting action rules for apn class %d", 
              apn_info->apn_type);
    if(ds_pdn_apm_config_validate_attach_pdn_class(apn_info->apn_type))
    {
      *config_error = ds_pdn_apm_set_class_dependent_action_rules (apn_info);
    }
    else if (ds_pdn_apm_is_generic_apn_switching_enabled())
    {
      *config_error = ds_pdn_apm_set_generic_action_rules (apn_info);
    }
   else if(ds_pdn_apm_config_get_apm_rules_num(rule_q) == 
                                                   DS_DSD_APM_SINGLE_QUEUE_ENTRY)
    {
      *config_error = ds_pdn_apm_set_single_attach_prof_action_rules(apn_info);
    } 
    else if(q_cnt(&(apn_info->cause_code_info.cause_code_info_q)) > 0)
    {
      *config_error = ds_pdn_apm_set_cause_code_based_action_rules (apn_info);
    }
    else
    {
      *config_error = EFS_APM_APN_ACTION_RULE_ERR;
    }

    /*--------------------------------------------------------------------- 
     Set the APN Type of first element in APM Rule Queue as PRIMARY
     1. This is done to ensure searching for next attach PDN is faster
    ----------------------------------------------------------------------*/
    if(*config_error == 0)
    {
      if(rule_block == ds_pdn_apm_get_start_block_rule_q())
      {
        rule_block->apn_info.apn_type = ATTACH_PDN_PRIMARY;
      }
      else
      {
        rule_block->apn_info.apn_type = ATTACH_PDN_NEXT;
      }
    }
    else
    {
      break;
    }

    rule_block = (ds_pdn_apm_data_block*)q_next(rule_q, &(rule_block->link));

  }
  return;
}/* dsd_apm_set_action_rule() */

/*===========================================================================
FUNCTION       ds_pdn_next_apn_init

DESCRIPTION    This is the utility function to initialize the next attach PDN 
                
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_next_apn_init
(
  void
)
{
 
  memset(&apm_info.next_attach_pdn_info, 0, 
         sizeof(apm_info.next_attach_pdn_info));

  DS_MSG1(MSG_LEGACY_HIGH,"next attach pdn type is %d", ATTACH_PDN_PRIMARY);
  apm_info.next_attach_pdn_info.next_attach_pdn_type = ATTACH_PDN_PRIMARY;
  return;
}/* ds_pdn_next_apn_init()*/

/*===========================================================================
FUNCTION       DS_DSD_APM_CONFIGURE_IP_RULE_FOR_SINGLE_APM_RULE_ENTRY

DESCRIPTION    This is the utility function that configures the IP address 
               requirement for the single APM rule queue entry 
 
PARAMETERS     NONE
                
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_configure_ip_rule_for_single_apm_rule_entry
(
  void
)
{
  boolean                              pdn_type_valid = TRUE;
  ipRequiredType                       ip_req_enum_value = 
                                           ipv4_dontcare_and_ipv6_donotcare;
  ds_pdn_apm_data_block*               apm_rule_data_block = NULL;
  ds_profile_type_s                    prof_info = {{0}};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  apm_rule_data_block = (ds_pdn_apm_data_block *)q_check(&ds_pdn_apm_q);

  if(NULL != apm_rule_data_block)
  {
    if ( ds_profile_setting_from_profile_id(
                  apm_rule_data_block->apn_info.profile_id,
                  &prof_info) == DS_PROFILE_RESULT_SUCCESS )
    {
      switch(prof_info.context.pdp_type)
      {
        case DS_PROFILE_PDP_IPV4:
          ip_req_enum_value = ipv4_mandatory_and_ipv6_donotcare;
          pdn_type_valid = TRUE;
          break;
        case DS_PROFILE_PDP_IPV6:
          ip_req_enum_value = ipv6_mandatory_and_ipv4_donotcare;
          pdn_type_valid = TRUE;
          break;
        case DS_PROFILE_PDP_IPV4V6:
          ip_req_enum_value = ipv4_mandatory_or_ipv6_mandatory;
          pdn_type_valid = TRUE;
        case DS_PROFILE_PDP_NON_IP:
          ip_req_enum_value = ipv4_dontcare_and_ipv6_donotcare;
          pdn_type_valid = TRUE;
          break;
        default:
          DS_ERROR_LOG_1("pdn type =d is invalid, no setting IP rule", 
                    prof_info.context.pdp_type);
      }
    }

    if(TRUE == pdn_type_valid)
    {
      apm_rule_data_block->apn_info.AttachPdn.config_rule.config_valid_flag |= 
        CONFIG_IP_ADDRESS;
      apm_rule_data_block->apn_info.AttachPdn.config_rule.ip_address = 
        ip_req_enum_value;
    }
  }
}

/*===========================================================================
FUNCTION       ds_pdn_apm_rule_validation

DESCRIPTION    This is the utility function used for rule and profile 
               validation and matching for a VZW Configured UE
 
PARAMETERS     NONE
                
DEPENDENCIES   NONE

RETURN VALUE   config_error: uint16 to indicate config error if any

SIDE EFFECTS   NONE
===========================================================================*/
uint16 ds_pdn_apm_rule_validation
(
  void
)
{
  uint16                                  config_error = 0;
  uint16                                  num_attach_profs = 0, num_rules = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------ 
   We currently support validation only if the 2 conditions are met:
   
   1. The number of attach profiles and rules should be equal
   2. The number should be greater than zero and less than or equal to 2
  -------------------------------------------------------------------------*/
  num_attach_profs = ds_pdn_apm_config_get_attach_prof_num(&attach_profile_q);
 
  num_rules = ds_pdn_apm_config_get_apm_rules_num(&ds_pdn_apm_q);

  if((num_attach_profs == num_rules) &&
     (num_attach_profs <= ATTACH_APN_MAX && num_attach_profs > 0))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Number of attach profiles and rules match");
  }
  else
  {
    DS_ERROR_LOG_0("Mismatch of number of attach profs and rules, default rule");
    config_error = EFS_APM_RULE_ATTACH_PROF_NUM_ERR;
  }

  /*------------------------------------------------------------------
   No configure error found in APM rule and attach profile, matching
   attach profile with APM rule. APM needs to find corresponding rules
   for each attach profile. 
  -------------------------------------------------------------------*/
  if( config_error == 0 )
  {
    /*------------------------------------------------------------------
    Match LTE attach profile to APM rule, and get first attach PDN
    -------------------------------------------------------------------*/
    ds_pdn_apm_match_attach_profile_and_rule(&ds_pdn_apm_q, 
                                          &attach_profile_q,
                                          &config_error);
  }
  /*----------------------------------------------------------------------
   No configure error found in matching APM rule & attach profile, setting
   the IP addr requirement based on profile's PDP type for single
   rule queue entry
  -----------------------------------------------------------------------*/
  if(0 == config_error &&
     DS_DSD_APM_SINGLE_QUEUE_ENTRY == num_rules)
  {
    ds_pdn_apm_configure_ip_rule_for_single_apm_rule_entry();
  }

  return config_error;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_populate_rule_q

DESCRIPTION    This is the utility function used to populate rule queue in 
               case of generic APN switching 
 
PARAMETERS     NONE
                
DEPENDENCIES   NONE

RETURN VALUE   config_error: uint16 to indicate config error if any

SIDE EFFECTS   NONE
===========================================================================*/
uint16 ds_pdn_apm_populate_rule_q
(
  void
)
{
  ds_pdn_apm_data_block        *rule_entry = NULL;
  attach_profile_data_block    *prof_entry = NULL;
  uint8                         profile_id;
  uint16                       config_error = 0;
  ds_profile_type_s            prof_info = {{0}};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG0(MSG_LEGACY_HIGH,
            "Adding APM rules with attach profile list");

  prof_entry = (attach_profile_data_block* )q_check(&attach_profile_q);
  if( prof_entry == NULL )
  {
    DS_ERROR_LOG_0("No valid attach profile id");
    config_error |= EFS_PROFILE_RULE_MISMATCH;
    return config_error;
  }

  while( prof_entry != NULL)
  {
    profile_id = prof_entry->attach_profile.profile;

    if ( ds_profile_setting_from_profile_id(profile_id,
                                            &prof_info) == DS_PROFILE_RESULT_SUCCESS )
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(rule_entry, sizeof(ds_pdn_apm_data_block),
	  	              ds_pdn_apm_data_block*);
      if(rule_entry != NULL)
      {
        memset(rule_entry, 0, sizeof(ds_pdn_apm_data_block));

        rule_entry->apn_info.profile_id = profile_id;
        /*--------------------------------------------------------------
          Need to initialize the cause code queue in the APM Rule Block
        ---------------------------------------------------------------*/
        q_init(&(rule_entry->apn_info.cause_code_info.cause_code_info_q));

        strlcpy(rule_entry->apn_info.apn_name, prof_info.context.apn,
                DS_PDN_APM_MAX_APN_STRING_LEN);
        rule_entry->apn_info.apn_name_len = strlen(prof_info.context.apn);
        q_link(rule_entry, &(rule_entry->link));
        q_put(&ds_pdn_apm_q, &(rule_entry->link));
      }
      else
      {
        config_error |= EFS_ATTACH_PROFILE_READ_ERR;
        break;
      }
    }
    else
    {
      config_error |= EFS_ATTACH_PROFILE_READ_ERR;
      break;
    }
    prof_entry = q_next(&attach_profile_q, &(prof_entry->link));
  }
  return config_error;
}
/*===========================================================================
FUNCTION       ds_pdn_apm_set_default_configuration

DESCRIPTION    This is the utility function to set the default configuration 
               in case of configuration failure or APN Switching/Blocking
               being dsiabled
 
PARAMETERS     NONE
                
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_default_configuration
(
  void
)
{
  attach_profile_data_block             *q_head = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_head = (attach_profile_data_block*)q_check(&attach_profile_q);

  /*------------------------------------------------------------------------ 
   Grab first rule on passed sub
  -------------------------------------------------------------------------*/
  while(q_head != NULL)
  {
    /*------------------------------------------------------------------
       We clear the Rule queue since we wont be using Invalid Rule entries
       and add just the default rule since APN class configuration was not
       done correctly
      -------------------------------------------------------------------*/
    ds_pdn_apm_free_rule_queue(&ds_pdn_apm_q);
  
    /*------------------------------------------------------------------
      Set next attach PDN and set to be permanent
    -------------------------------------------------------------------*/
    memset(&apm_info.next_attach_pdn_info, 0, 
           sizeof(apm_info.next_attach_pdn_info));
  
    apm_info.next_attach_pdn_info.next_attach_pdn_type = 
                                                       ATTACH_PDN_DEFAULT;
    ds_pdn_next_attach_pdn_set_permanent();
    ds_pdn_set_default_apm_rule(q_head->attach_profile.profile,
                                &ds_pdn_apm_q);
    break;
    q_head = q_next(&attach_profile_q, &(q_head->link));
  }
  return;
}

/*===========================================================================
FUNCTION      ds_pdn_apm_set_configuration

DESCRIPTION   Based on APM rule and attach profile, set next attach PDN, 
              APM rule and action based on PDN class 
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_set_configuration
(
  void
)
{
  uint16                                  config_error = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH,"ds_pdn_apm_set_configuration");

  /*------------------------------------------------------------------
    Fill the APN Disable Info
  -------------------------------------------------------------------*/
  ds_pdn_apm_fill_apn_disable_info();

  if(ds_pdn_apm_is_apn_switch_support())
  {
    /*------------------------------------------------------------------
    Unset the permanent flag
    -------------------------------------------------------------------*/
    ds_pdn_apm_next_attach_pdn_unset_permanent();
    /*------------------------------------------------------------------
    clear stale infomation in APM rule queue and delete default rule
    -------------------------------------------------------------------*/
    ds_pdn_apm_delete_default_apm_rule(&ds_pdn_apm_q);
    ds_pdn_apm_rule_reset_apn_class(&ds_pdn_apm_q);

    /*------------------------------------------------------------------
     If generic APN Switching is enabled that suggests no rule file is
     present, we populate the rule queue as per generic APN switching
     requirements
     Else we perform rule validation
    -------------------------------------------------------------------*/
    if(ds_pdn_apm_is_generic_apn_switching_enabled())
    {
      ds_pdn_apm_free_rule_queue(&ds_pdn_apm_q);
      config_error = ds_pdn_apm_populate_rule_q();
    }
    else
    {
      config_error = ds_pdn_apm_rule_validation();
    }

    /*------------------------------------------------------------------
      Set the APM PDN action rules
    -------------------------------------------------------------------*/
    if( config_error == 0)
    {
      ds_pdn_apm_set_action_rules(&ds_pdn_apm_q, &config_error);
    }
    else
    {
      DS_ERROR_LOG_1("APM rule or attach profile configure error =d, do not set"
                     " action rules", config_error);
       /*---------------------------------------------------------------
        Check if profile read fails due to read error and reset the 
        attach profile queue to default socket profile.
       ----------------------------------------------------------------*/
      if( (config_error & EFS_ATTACH_PROFILE_READ_ERR) )
      { 
         DS_ERROR_LOG_0("Resetting to default attach profile");
         ds_pdn_apm_free_prof_queue(&attach_profile_q);
         if(!ds_pdn_apm_read_default_attach_prof(&attach_profile_q))
         {
            DS_ERROR_LOG_0("Default attach profile read failed, block LTE");
         }
      }
    }

    if( config_error == 0 ) /* Configure success*/
    {
      /*------------------------------------------------------------------
        If next attach PDN is specified which means APM already switch
        attach PDN based on certain failure conditon. Otherwise, initiate
        next attach PDN with proper APN type which is power up case or
        configuration error before.
       -------------------------------------------------------------------*/
       if( apm_info.next_attach_pdn_info.next_attach_pdn_type
            == ATTACH_PDN_DEFAULT ||
           apm_info.next_attach_pdn_info.next_attach_pdn_type
            == ATTACH_PDN_INVALID )
       {
         ds_pdn_next_apn_init();
       }
       else
       {
         DS_MSG1(MSG_LEGACY_HIGH,
                   "Not setting the profile. Exisiting profile type = %d",
                   apm_info.next_attach_pdn_info.next_attach_pdn_type);
       }
    }
    else
    {
      /*------------------------------------------------------------------
        We revert to default APM rule in case of rule validation failure
        or action rules setup failure
       -------------------------------------------------------------------*/
      ds_pdn_apm_set_default_configuration();
    }
  } 
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"LTE apn blocking and Switching both disabled");
    /*------------------------------------------------------------------
      We revert to default APM rule
    -------------------------------------------------------------------*/
    ds_pdn_apm_set_default_configuration();
  }
  return;

}/* ds_pdn_apm_set_configuration*/

/*===========================================================================
FUNCTION      ds_pdn_apm_validate_prof_q

DESCRIPTION   This function checks if the attach profiles read in Queue 
              are valid or not 
 
PARAMETERS    uint16 - attach profile counter reference
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
                                    
===========================================================================*/
boolean ds_pdn_apm_validate_prof_q
(
  uint16                                   *attach_prof_count_ptr
)
{
  attach_profile_data_block                *profile_block = NULL;
  uint8                                     profile_id = 0;
  boolean                                   ret_val = FALSE;                         
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if(attach_prof_count_ptr != NULL)
  {
    profile_block = (attach_profile_data_block *)q_check(&attach_profile_q);

    while(profile_block != NULL)
    {
      profile_id = profile_block->attach_profile.profile;

      if(ds_profile_number_is_valid(profile_id))
      {
        (*attach_prof_count_ptr)++;
        profile_block = (attach_profile_data_block *)q_next(&attach_profile_q,
                                                        &(profile_block->link));
        ret_val = TRUE;
      }
      else
      {
        DS_ERROR_LOG_1("Invalid Profile found: =d",profile_id);
        ret_val = FALSE;
        break;
      }
    }
  }
  return ret_val;
}

/*===========================================================================
FUNCTION      DS_DSD_APM_EFS_READ

DESCRIPTION   This function reads EFS file 
 
PARAMETERS    NONE
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Populate contents in attach_profile_q & ds_pdn_apm_q (Rule Q)
                                    
===========================================================================*/
void ds_pdn_apm_info_efs_read
(
  void
)
{
  uint16                                  config_error = 0;
  uint16                                  attach_prof_count = 0;
  uint8                                   new_attach_prof_mask = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH,"ds_dsd_apm_efs_read");

  memset(&(apm_info.next_attach_pdn_info), 0, 
         sizeof(next_attach_pdn_info_type));

  ds_pdn_apm_set_generic_switching_flag(FALSE);


  ds_pdn_apm_free_rule_queue(&ds_pdn_apm_q);
  ds_pdn_apm_free_prof_queue(&attach_profile_q);
  
  new_attach_prof_mask = ds_pdn_apm_read_attach_profile_list_from_efs(
                                                        &attach_profile_q);
  /*---------------------------------------------------------------------
     Read LTE attach profiles from EFS files, if EFS read fails we read
     the default socket profile
  -------------------------------------------------------------------*/
  if (new_attach_prof_mask == FALSE)
  {
    ds_pdn_apm_free_prof_queue(&attach_profile_q);

    if(!ds_pdn_apm_read_default_attach_prof(&attach_profile_q))
    {
      config_error |= EFS_DEF_ATTACH_PROF_READ_ERR;
    }
  } /* read profile list from EFS failed */
  else
  {
    /*---------------------------------------------------------------------
       Validate the contents of attach profile list, if none of the profiles
       are valid then we revert to default socket profile
    -------------------------------------------------------------------*/
    if(ds_pdn_apm_validate_prof_q(&attach_prof_count) == FALSE)
    {
      ds_pdn_apm_free_prof_queue(&attach_profile_q);
      if(!ds_pdn_apm_read_default_attach_prof(&attach_profile_q))
      {
        config_error |= EFS_DEF_ATTACH_PROF_READ_ERR;
      }
    }
    else if(attach_prof_count > ATTACH_APN_MAX)
    {
      DS_MSG1(MSG_LEGACY_HIGH, 
                "More than 2 attach profiles specified, Enabling Generic "
                "Switching", attach_prof_count);
      config_error |= EFS_ATTACH_PROFILE_NUM_ERR;
      if(ds_pdn_apm_is_apn_switch_support() == TRUE)
      {
        ds_pdn_apm_set_generic_switching_flag(TRUE);
      }
    }   
  }
  /*------------------------------------------------------------------
   If read attach profile ID from EFS succeed, read APM rule;
   If read attach profile ID from EFS failed, default behavior and no
   APM switch.
   -------------------------------------------------------------------*/
  if( config_error == 0 )
  {
      if ( ds_pdn_apm_read_pdn_rule_list_from_efs(&attach_profile_q,
                                               &ds_pdn_apm_q) == FALSE )
      {
        config_error |= EFS_APM_RULE_READ_ERR;
        /*------------------------------------------------------------------
         If read APM rule from EFS fail, free APM rule queue and enable
         generic APN Switching
        -------------------------------------------------------------------*/
      ds_pdn_apm_free_rule_queue(&ds_pdn_apm_q);
      if(ds_pdn_apm_is_apn_switch_support() == TRUE)
      {
        ds_pdn_apm_set_generic_switching_flag(TRUE);
      }
    }
  }

  /*---------------------------------------------------------------------
     If the default socket profile read fails, then we block LTE since
     we have no valid attach profile
  -------------------------------------------------------------------*/  
  if(config_error & EFS_DEF_ATTACH_PROF_READ_ERR)
  {
    apm_info.attach_pdn_info.if_lte_attach_allowed = FALSE;
  }
  else
  {
    /*------------------------------------------------------------------
     Based on APM rule and attach profile, set APM module configuration
     including next attach PDN, rule and actions
    -------------------------------------------------------------------*/
    ds_pdn_apm_set_configuration();

    /*--------------------------------------------------------------
     Reset the Attach PDN info and copy next attach PDN info to 
     attach PDN data structure. 
    ----------------------------------------------------------------*/
    ds_pdn_apm_reset_and_copy_attach_info();
  }

  if (config_error != 0) 
  {
    DS_ERROR_LOG_1("config error: =d",config_error);
  }

  return;
}/* ds_dsd_apm_info_efs_read() */

/*===========================================================================

                         UTILITY
===========================================================================*/
 
/*===========================================================================
FUNCTION       ds_pdn_apm_get_next_attach_profile_id

DESCRIPTION    This utility function returns next attach profile id
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint8 ds_pdn_apm_get_next_attach_profile_id
(
  uint8                               profile_id
)
{
  ds_pdn_apm_data_block               *rule_block = NULL;
  attach_profile_data_block           *profile_block = NULL;
  uint16                               ret_val = 0;
  attach_pdn_type                     *next_attach_pdn = NULL;
  uint8                               *apm_profile_id = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG1(MSG_LEGACY_HIGH,"PDN type %d of next attach PDN",
            apm_info.next_attach_pdn_info.next_attach_pdn_type);

  /*---------------------------------------------------------------------- 
    Depending on the queue position we extract the next attach profile
    The if part extracts the head of the queue
    Else part is used to extract any other internal elements
  ----------------------------------------------------------------------*/

  if(apm_info.next_attach_pdn_info.next_attach_pdn_type 
         == ATTACH_PDN_PRIMARY ||
     apm_info.next_attach_pdn_info.next_attach_pdn_type 
         == ATTACH_PDN_DEFAULT
     )
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(next_attach_pdn, sizeof(attach_pdn_type),
		                    attach_pdn_type*);

    if(next_attach_pdn != NULL)
    {
      memset(next_attach_pdn, 0 , sizeof(attach_pdn_type));
      *next_attach_pdn = 
            apm_info.next_attach_pdn_info.next_attach_pdn_type;

      rule_block = (ds_pdn_apm_data_block*)q_linear_search(&ds_pdn_apm_q,
                                                          ds_pdn_compare_apn_type,
                                                          next_attach_pdn);
      if( rule_block != NULL)
      {
        ret_val = rule_block->apn_info.profile_id;
      }
      DS_SYSTEM_HEAP_MEM_FREE (next_attach_pdn);
    }
  }
  else
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(apm_profile_id , sizeof(uint8), uint8 *);
    if(apm_profile_id != NULL)
    {
      memset(apm_profile_id, 0, sizeof(uint8));
      *apm_profile_id = profile_id;

      profile_block = (attach_profile_data_block *)q_linear_search(
                                                    &attach_profile_q,
                                          ds_pdn_apm_compare_profile_id_prof_q,
                                                    apm_profile_id);
      if(profile_block != NULL)
      {
        profile_block = (attach_profile_data_block *)q_next(&attach_profile_q,
                                                        &(profile_block->link));
        if(profile_block != NULL)
        {
          ret_val = profile_block->attach_profile.profile;
        }
        else
        {
          ret_val = profile_id;
        }
      }
      DS_SYSTEM_HEAP_MEM_FREE(apm_profile_id);
    }
  }

  return ret_val;
}/* ds_pdn_apm_get_next_attach_profile_id */


/*===========================================================================
FUNCTION       ds_pdn_apm_is_additonal_apn_switch_required

DESCRIPTION    This utility function checks if we need to switch the APN 
               depending on throttling info provided by MH, APN Disable flag
               status and APN blocking info given by MPPM
 
PARAMETERS      profile_id: Profile on which the above checks need to be run 
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE:      Additional Switch needed
               FALSE:     No additonal switching needed 

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_is_additonal_apn_switch_required
(
   uint8                            profile_id
)
{
   boolean                           is_apn_disabled = FALSE;
   boolean                           is_apn_blocked = FALSE;
   boolean                           is_switch_required = FALSE;
   uint8                             apm_profile_id = 0;
   ds_pdn_apm_data_block            *rule_block = NULL;
   ds_pdn_apm_data_block            *last_block = NULL;  
/*-------------------------------------------------------------------------*/

   /*-----------------------------------------------------------------------
    Here we check for 3 Parameters before deciding we need to perform an
    additional switch or not, if any of the 3 are satisfied we switch
    1. MPPM blocking attach on an APN
    2. APN Disable
    3. APN blocking information given by MH 
   -------------------------------------------------------------------------*/

   is_apn_disabled = ds_pdn_apm_is_apn_disabled(profile_id);

   is_apn_blocked = ds_pdn_apm_is_apn_blocked(profile_id);

   DS_MSG3(MSG_LEGACY_HIGH,"APN Disable Status: %d, APN blocked "
                  "status: %d for Profile: %d",
                  is_apn_disabled, is_apn_blocked, profile_id);

   is_switch_required = (is_apn_blocked || is_apn_disabled);
   if(is_switch_required)
   {
     apm_profile_id = profile_id;
     rule_block = (ds_pdn_apm_data_block *)q_linear_search(&ds_pdn_apm_q,
                                             ds_pdn_compare_profile_id,
                                             &apm_profile_id);

     last_block = ds_pdn_apm_get_last_rule_block();

     if((rule_block == NULL) || (last_block == NULL))
     {
       DS_ERROR_FATAL("Invalid Rule block or Last block");
     }
     else
     {
       if(memcmp((void *)rule_block, (void *)last_block, 
                 sizeof(ds_pdn_apm_data_block)) == 0)
       {
         if(apm_info.dsd_apm_default_apm_rule_enabled == TRUE)
         {
           apm_info.next_attach_pdn_info.next_attach_pdn_type =
                                                               ATTACH_PDN_DEFAULT;
         }
         else
         {
           apm_info.next_attach_pdn_info.next_attach_pdn_type =
                                                               ATTACH_PDN_PRIMARY;
         }
       }
       else
       {
         apm_info.next_attach_pdn_info.next_attach_pdn_type =
                                                              ATTACH_PDN_NEXT;
       }
     }
   }

   return is_switch_required;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_reset_and_copy_attach_info

DESCRIPTION    This utility function resets the ATTACH PDN info.
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_reset_and_copy_attach_info
(
  void
) 
{
  attach_profile_data_block        *profile_block = NULL;
  attach_profile_data_block        *start_block = NULL;
  boolean                           is_switch_required = FALSE;
  uint8                             prev_prof_id;
  uint8                             apm_profile_id = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
   For debugging purpose only.
  -------------------------------------------------------------------------*/
  DS_MSG3(MSG_LEGACY_HIGH,
            "Attach PDN info before re-initialize" 
            "pdn_type: %d pdn_state: %d prof_id: %d",
            apm_info.attach_pdn_info.pdn_type,
            apm_info.attach_pdn_info.state,
            apm_info.attach_pdn_info.attach_profile_id);

  DS_MSG1(MSG_LEGACY_HIGH,"LTE attach allowed old value = %d",
            apm_info.attach_pdn_info.if_lte_attach_allowed);

  /*-------------------------------------------------------------------------
   Update the attach_pdn_info with next attach profile ID if available or
   block LTE attach if all APNs is blocked or disabled
   The logic for selecting attach profile is based on the concept of a
   circular queue
   If First entry fails, we move to the next one and keep on continuing till
   we come back the starting point at which we conclude that no profile is
   available and block LTE
  -------------------------------------------------------------------------*/
  do
  {
    prev_prof_id = apm_info.attach_pdn_info.attach_profile_id;

    memset(&apm_info.attach_pdn_info, 0, 
         sizeof(apm_info.attach_pdn_info));

    apm_info.attach_pdn_info.pdn_type = 
            apm_info.next_attach_pdn_info.next_attach_pdn_type;
    apm_info.attach_pdn_info.attach_profile_id = 
            ds_pdn_apm_get_next_attach_profile_id(prev_prof_id);

    if (apm_info.attach_pdn_info.attach_profile_id != 
                                               DS_PDN_APM_INVALID_PROF_ID_NUM)
    {
      apm_profile_id = apm_info.attach_pdn_info.
                                     attach_profile_id;
      profile_block = (attach_profile_data_block *)q_linear_search(
                                             &attach_profile_q,
                                             ds_pdn_apm_compare_profile_id_prof_q,
                                             &apm_profile_id);
      /*---------------------------------------------------------------------- 
        Storing the starting point to break the circular loop
      ----------------------------------------------------------------------*/
      if(profile_block != NULL)
      {
        /*---------------------------------------------------------------------- 
          Storing the starting point to break the circular loop
        ----------------------------------------------------------------------*/
        if(!is_switch_required)
        {
          start_block = profile_block;
        }
        else
        {
          /*---------------------------------------------------------------------- 
            No profile found for attach so we will block LTE, before that we do
            state reset so as to make sure next attempt is successful
          ----------------------------------------------------------------------*/
          if((memcmp(start_block, profile_block, 
              sizeof(attach_profile_data_block)) == 0) &&
             (start_block == profile_block))
          {
            DS_ERROR_LOG_0("No attach Profile available");
            apm_info.attach_pdn_info.attach_profile_id = prev_prof_id;
            break;
          }
        }
        /*---------------------------------------------------------------------- 
          This function checks if the currently selected profile is still
          available for attach and has not been blocked/disabled
        ----------------------------------------------------------------------*/
        is_switch_required = ds_pdn_apm_is_additonal_apn_switch_required(
                             apm_info.attach_pdn_info.attach_profile_id);
      }
    }
    else
    {
      /*---------------------------------------------------------------------- 
        Done to break out of the while loop in case an Invalid profile is
        returned at first attempt
      ----------------------------------------------------------------------*/
      is_switch_required = FALSE; 
    }
  } while (is_switch_required);

  if(apm_info.attach_pdn_info.attach_profile_id != 
      DS_PDN_APM_INVALID_PROF_ID_NUM && !(is_switch_required))
  {
    apm_info.attach_pdn_info.if_lte_attach_allowed = TRUE;
  }
  else
  {
    apm_info.attach_pdn_info.if_lte_attach_allowed = FALSE;
  }

  apm_info.attach_pdn_info.attach_pdp_type = DS_PROFILE_PDP_MAX;

  DS_MSG3(MSG_LEGACY_HIGH,
            "After Re-initialize PDN TYPE =%d PDN STATE =%d PROF_ID =%d",
            apm_info.attach_pdn_info.pdn_type,
            apm_info.attach_pdn_info.state,
            apm_info.attach_pdn_info.attach_profile_id);

  DS_MSG1(MSG_LEGACY_HIGH,"LTE attach allowed: %d",
                apm_info.attach_pdn_info.if_lte_attach_allowed);
  
  return;
} /* ds_pdn_apm_reset_and_copy_attach_info() */


/*===========================================================================
FUNCTION       ds_pdn_apm_update_apn_disable_flag

DESCRIPTION    Updating the internal structure to keep track of APN's available 
               for switching 
 
PARAMTERS      profile_id : Profile whose APN is disabled 

DEPENDENCIES   NONE

RETURN VALUE   TRUE: If Internal Structures were updated succesfully 
               FALSE: If Updated failed 

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_update_apn_disable_flag
(
   uint8                               profile_id,
   boolean                             apn_disable
)
{
  attach_profile_data_block           *profile_block = NULL;
  boolean                              ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  profile_block = (attach_profile_data_block *)q_check(&attach_profile_q);

  while (profile_block != NULL)
  {
    if (profile_block->attach_profile.profile == profile_id)
    {
      profile_block->attach_profile.apn_disabled = apn_disable;
      ret_val = TRUE;
    }
    profile_block = (attach_profile_data_block *)q_next(&attach_profile_q,
                                                        &profile_block->link);
  }


  return ret_val;
}


/*===========================================================================

                          EXTERNAL INTERFACE IMPLEMENTATION

===========================================================================*/


/*===========================================================================
FUNCTION      DS_PDN_APM_CLEANUP

DESCRIPTION   Called to clean DS_PDN apm rule and profile block queue

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_apm_cleanup(void)
{
  ds_pdn_apm_data_block      *rule_block    = NULL;
  attach_profile_data_block  *profile_block = NULL;
  apn_priority_data_block    *apn_info_block = NULL;
 /*-----------------------------------------------------------*/
  while((rule_block = q_get(&ds_pdn_apm_q)) != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(rule_block);
  }
  while((profile_block = q_get(&attach_profile_q)) != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(profile_block);
  }
  while((apn_info_block = q_get(&ds_pdn_apm_apn_priority_q)) != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(profile_block);
  }

  return;
}

/*===========================================================================
FUNCTION      ds_pdn_apm_init

DESCRIPTION   This function initializes APM module

PARAMETERS
  is_init          : Is it part of power up init

DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  1. APN INFO, ATTACH_PDN_INFO and NEXT_ATTACH_PDN_INFO 
                 data structure intialized. 
              2. Config rule and action rules are populated
                   - either reading from EFS or from static file.
===========================================================================*/
void ds_pdn_apm_init(boolean is_init)
{
   boolean  ret_val=FALSE;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&apm_info.attach_pdn_info, 0, sizeof(apm_info.attach_pdn_info));
  memset(&apm_info.next_attach_pdn_info, 0, sizeof(apm_info.next_attach_pdn_info));
  memset(&apm_info.apm_lte_plmn_id, 0, sizeof(apm_info.apm_lte_plmn_id));
    
  apm_info.dsd_apm_apn_switching_enable = TRUE;
  apm_info.dsd_apm_lte_apn_blocking_support = FALSE;
  apm_info.dsd_apm_profile_id_based_switch = TRUE;
  apm_info.dsd_apm_default_apm_rule_enabled = FALSE;
  apm_info.default_attach_apn_allowed = TRUE;
  apm_info.apm_attach_apn_all_blocked = FALSE;
  apm_info.attach_pdn_info.if_lte_attach_allowed = TRUE;
  apm_info.attach_pdn_info.attach_pdp_type = DS_PROFILE_PDP_MAX; 

  if(is_init)
  {
    /*-------------------------------------------------------------------------
    Initialize the APM rule Queue. It is done before reading the NV item so 
    that the QUEUE library can be used as UTILITY for furture. E.g. QUEUE SIZE 
    -------------------------------------------------------------------------*/
    q_init(&ds_pdn_apm_q);

    /*------------------------------------------------------------------
    Initialize the attach profile Queue.
    -------------------------------------------------------------------*/
    q_init(&attach_profile_q);

    /*------------------------------------------------------------------
    Initialize the attach pdn priority Queue. It is used for LTE APN blocking. 
    By default, UE read attach profile id from EFS, and this queue is used 
    to store all blocked APNs received from 3GPP MH; 
    If attach profile id switching is off, UE read all the APN names from 
    APN name priority EFS files, and store the blocked info for these APN 
    names. 
    -------------------------------------------------------------------*/
    q_init(&ds_pdn_apm_apn_priority_q);
  }
  else
  {
    ds_pdn_apm_cleanup();
  }

  /*-------------------------------------------------------------------------
  Read the NV item. If NV item is true, read EFS, else defaut behavior.
  -------------------------------------------------------------------------*/
  ds_pdn_apm_set_apn_switch_flag(ds_pdn_nv_manager_get_apm_apn_switching());
     
  ds_pdn_apm_set_generic_switching_flag(FALSE);
  /*-------------------------------------------------------------------------
   Set the PLMN blocking Interval to the default value
  --------------------------------------------------------------------------*/
  ds_pdn_apm_set_plmn_blocking_interval_timer(DS_PDN_APM_PLMN_BLOCKING_INTERVAL);

#ifdef FEATURE_DATA_REL10 
  ret_val = ds_pdn_nv_manager_get_nv_lte_rel_10();
  apm_info.dsd_apm_lte_apn_blocking_support = ret_val;

  DS_MSG1(MSG_LEGACY_HIGH,"LTE release 10 support %d",ret_val);
#endif /* FEATURE_DATA_REL10*/

  /*-------------------------------------------------------------------------
   Read APM info from EFS
  -------------------------------------------------------------------------*/
  ds_pdn_apm_info_efs_read();
  /*--------------------------------------------------------------
  At powerup, be default, APM will not run any rule.
  ----------------------------------------------------------------*/
  ds_pdn_apm_unset_ip_rule_required();    

  return;
}/* ds_pdn_apm_init() */

/*===========================================================================
FUNCTION ds_dsd_apm_is_dns_and_pcscf_addr_info_present_and_valid

DESCRIPTION
  This function checks to see if Network has provided DNS & PCSCF address
  information to UE

PARAMETERS 
  None

DEPENDENCIES
  None

RETURN VALUE
  ds_pdn_apm_pcscf_dns_addr_state_type -
       Enum that tells whether PCSCF or DNS was received successfully or not
   
SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_apm_pcscf_dns_addr_state_type 
                     ds_pdn_apm_is_pcscf_and_dns_addr_info_present_and_valid
(
  void
)
{
  ds_pdn_apm_pcscf_dns_addr_state_type  ret_val = DS_PDN_APM_PCSCF_DNS_NOT_RCVD;
  ds_pdn_apm_nw_state_type              dns_addr_valid = DS_PDN_APM_NOT_RCVD;
  ds_pdn_apm_nw_state_type              pcscf_addr_valid = DS_PDN_APM_NOT_RCVD;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------
   Here we check if DNS/PCSCF address is received successfully or not
   
   1. If both v4 and v6 address have failed then we mark it as invalid
   2. If either v4 or v6 is successfully received, we mark it valid
   3. Else we consider it as NOT_RECVD
  ----------------------------------------------------------------*/
  if(DS_PDN_APM_RCVD_FAIL == 
               apm_info.attach_pdn_info.ipv4_dns_address &&
     DS_PDN_APM_RCVD_FAIL == 
               apm_info.attach_pdn_info.ipv6_dns_address)
  {
    dns_addr_valid = DS_PDN_APM_RCVD_FAIL;
  }
  else if (DS_PDN_APM_RCVD_SUCC == 
               apm_info.attach_pdn_info.ipv4_dns_address ||
           DS_PDN_APM_RCVD_SUCC == 
               apm_info.attach_pdn_info.ipv6_dns_address)
  {
    dns_addr_valid = DS_PDN_APM_RCVD_SUCC;
  }

  if(DS_PDN_APM_RCVD_FAIL == 
               apm_info.attach_pdn_info.ipv4_pcscf_address &&
     DS_PDN_APM_RCVD_FAIL == 
               apm_info.attach_pdn_info.ipv6_pcscf_address)
  {
    pcscf_addr_valid = DS_PDN_APM_RCVD_FAIL;
  }
  else if (DS_PDN_APM_RCVD_SUCC == 
           apm_info.attach_pdn_info.ipv4_pcscf_address ||
           DS_PDN_APM_RCVD_SUCC == 
           apm_info.attach_pdn_info.ipv6_pcscf_address)
  {
    pcscf_addr_valid = DS_PDN_APM_RCVD_SUCC;
  }

  if(DS_PDN_APM_RCVD_FAIL == dns_addr_valid &&
     DS_PDN_APM_RCVD_FAIL == pcscf_addr_valid)
  {
    ret_val = DS_PDN_APM_PCSCF_DNS_INVALID;
  }
  else if(DS_PDN_APM_RCVD_SUCC == dns_addr_valid ||
          DS_PDN_APM_RCVD_SUCC == pcscf_addr_valid)
  {
    ret_val = DS_PDN_APM_PCSCF_DNS_VALID;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_attach_completed

DESCRIPTION    This is the utility function to check if ATTACH COMPLETED 
               from APM modules perspective 
  
DEPENDENCIES   NONE

RETURN VALUE   TRUE/FALSE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_attach_completed
(
  void
)
{
  boolean                retVal = TRUE;
  apn_info_type          *apn_info_ptr; 
  attach_pdn_info_type   *attach_pdn_info_ptr; 
  ds_pdn_apm_data_block  *rule_block;
  attach_pdn_type         attach_pdn;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  attach_pdn_info_ptr = &apm_info.attach_pdn_info;
  attach_pdn = attach_pdn_info_ptr->pdn_type;

  if(attach_pdn_info_ptr->state != ATTACH_PDN_STATE_CONNECTED)
  {
    return FALSE;
  }

  /*--------------------------------------------------------------
   Attach PDN does not need to run rules, and LTE attach complete
   received, return TRUE
  ----------------------------------------------------------------*/ 
  rule_block = (ds_pdn_apm_data_block*)q_linear_search( &ds_pdn_apm_q,
                                                ds_pdn_compare_apn_type,
                                                           &attach_pdn);

  if((attach_pdn_info_ptr->is_tlb_mode_active == TRUE) ||
     (attach_pdn_info_ptr->ip_rule_required == FALSE)  ||
     ( rule_block == NULL ))
  {
    return TRUE;
  }

  apn_info_ptr = &(rule_block->apn_info);

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
  {
    DS_MSG4(MSG_LEGACY_HIGH,"attach pdn type: %d,attach PDN IP addr rule %d,"
                           " IPv4 addr: %d ,IPV6 addr: %d",
             attach_pdn_info_ptr->pdn_type,
            apn_info_ptr->AttachPdn.config_rule.ip_address,
            attach_pdn_info_ptr->ipv4_address,
            attach_pdn_info_ptr->ipv6_address);

    switch(apn_info_ptr->AttachPdn.config_rule.ip_address)
    {
      case ipv4_mandatory_and_ipv6_donotcare:
        if(attach_pdn_info_ptr->ipv4_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv6_mandatory_and_ipv4_donotcare:
        if(attach_pdn_info_ptr->ipv6_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_mandatory_and_ipv6_mandatory:
        if(attach_pdn_info_ptr->ipv4_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_dontcare_and_ipv6_donotcare:
        break;

      case ipv4_mandatory_or_ipv6_mandatory:
        /*--------------------------------------------------------------
        Change logic so that APM waits until V4 is successfully received
        or V6 is successfully received.
        ----------------------------------------------------------------*/
        if((attach_pdn_info_ptr->ipv4_address != DS_PDN_APM_RCVD_SUCC) &&
           (attach_pdn_info_ptr->ipv6_address != DS_PDN_APM_RCVD_SUCC))
        {
          return FALSE;
        }
        break;

      default:
        {
          return TRUE;
        }
        break;
    }
  }


  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
  {
    DS_MSG4(MSG_LEGACY_HIGH,"attach PDN type %d, PCSCF address rule %d,"
                             "PCSCF IPV4 addr: %d,PCSCF IPV6 addr: %d",
              attach_pdn_info_ptr->pdn_type,
              apn_info_ptr->AttachPdn.config_rule.pcscf_address,
              attach_pdn_info_ptr->ipv4_pcscf_address,
              attach_pdn_info_ptr->ipv6_pcscf_address);
    switch(apn_info_ptr->AttachPdn.config_rule.pcscf_address)
    {
      case ipv4_mandatory_and_ipv6_donotcare:
        if(attach_pdn_info_ptr->ipv4_pcscf_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv6_mandatory_and_ipv4_donotcare:
        if(attach_pdn_info_ptr->ipv6_pcscf_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_mandatory_and_ipv6_mandatory:
        if(attach_pdn_info_ptr->ipv4_pcscf_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_pcscf_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_dontcare_and_ipv6_donotcare:
        break;

      case ipv4_mandatory_or_ipv6_mandatory:
        if((attach_pdn_info_ptr->ipv4_pcscf_address != DS_PDN_APM_RCVD_SUCC) && 
           (attach_pdn_info_ptr->ipv6_pcscf_address != DS_PDN_APM_RCVD_SUCC))
        {
          return FALSE;
        }
        break;

      default:
        {
          return TRUE;
        }
        break;
    }
  }

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
  {
    DS_MSG4(MSG_LEGACY_HIGH,"attach PDN type %d, DNS address rule %d ,"
                             "DNS IPv4 Addr: %d, DNS IPV6 addr: %d",
              attach_pdn_info_ptr->pdn_type,
              apn_info_ptr->AttachPdn.config_rule.dns_address,
              attach_pdn_info_ptr->ipv4_dns_address,
              attach_pdn_info_ptr->ipv6_dns_address);
    switch(apn_info_ptr->AttachPdn.config_rule.dns_address)
    {
      case ipv4_mandatory_and_ipv6_donotcare:
        if(attach_pdn_info_ptr->ipv4_dns_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv6_mandatory_and_ipv4_donotcare:
        if(attach_pdn_info_ptr->ipv6_dns_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_mandatory_and_ipv6_mandatory:
        if(attach_pdn_info_ptr->ipv4_dns_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_dns_address != DS_PDN_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_dontcare_and_ipv6_donotcare:
        break;

      case ipv4_mandatory_or_ipv6_mandatory:
        if((attach_pdn_info_ptr->ipv4_dns_address != DS_PDN_APM_RCVD_SUCC) && 
           (attach_pdn_info_ptr->ipv6_dns_address != DS_PDN_APM_RCVD_SUCC))
        {
          return FALSE;
        }
        break;

      default:
        {
          return TRUE;
        }
        break;
    }
  }
  /*---------------------------------------------------------------
    We need to make sure if PCSCF_OR_DNS address rule is specified
    then either PCSCF or DNS address is received successfully before
    posting FULL service indication to external clients
  ----------------------------------------------------------------*/
  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
                                                    CONFIG_PCSCF_DNS_ADDRESS)
  {
    DS_MSG2(MSG_LEGACY_HIGH,"attach PDN type %d, PCSCF_DNS address rule %d",
              attach_pdn_info_ptr->pdn_type,
              apn_info_ptr->AttachPdn.config_rule.pcscf_or_dns_address);

    if(ipv4_mandatory_or_ipv6_mandatory == 
                    apn_info_ptr->AttachPdn.config_rule.pcscf_or_dns_address)
    {
      if(DS_PDN_APM_PCSCF_DNS_NOT_RCVD ==
            ds_pdn_apm_is_pcscf_and_dns_addr_info_present_and_valid())
      {
        DS_MSG0(MSG_LEGACY_HIGH,
                  "Waiting for valid DNS or PCSCF address information");
        return FALSE;
      }
    }
   }

  return retVal;
}/* ds_pdn_apm_attach_completed() */

/*===========================================================================
FUNCTION       ds_pdn_apm_payload_processing

DESCRIPTION    This is utility function for payload processing

DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_payload_processing
(
  ds_pdn_apm_ind_pay_load            *pay_load
)
{
  uint8                              attach_profile_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(pay_load == NULL)
  {
    return;
  }

  /*--------------------------------------------------------------
  Process IP field mask
  ----------------------------------------------------------------*/
  if((pay_load->ind_field_mask & DS_APM_IND_IP_ADDRESS_FIELD_MASK))
  {
    /*--------------------------------------------------------------
    Check attach profile PDN type and if attach profile is v4 only, 
    set v6 address failure; if attach profile is v6 only, set v4
    address failure. Since in this case, MH will not send v4/v6 address 
    indication. 
    ----------------------------------------------------------------*/
    attach_profile_id = apm_info.attach_pdn_info.attach_profile_id;

    DS_MSG1(MSG_LEGACY_HIGH,
              "ds_pdn_apm_payload_processing, pdn type: %d",
              apm_info.attach_pdn_info.attach_pdp_type );

    switch( apm_info.attach_pdn_info.attach_pdp_type )
      {
        case DS_PROFILE_PDP_IP:
          apm_info.attach_pdn_info.ipv6_address = DS_PDN_APM_RCVD_FAIL;
          break;

        case DS_PROFILE_PDP_IPV6:
          apm_info.attach_pdn_info.ipv4_address = DS_PDN_APM_RCVD_FAIL;
          break;

        default:
          break;
    }

    /*--------------------------------------------------------------
    Process V4 field mask
    ----------------------------------------------------------------*/
    if((pay_load->ip_address.field_mask & DS_PDN_APM_IP_V4_FIELD_MASK))
    {
      if(pay_load->ip_address.v4_address == TRUE)
      {
        apm_info.attach_pdn_info.ipv4_address = DS_PDN_APM_RCVD_SUCC;
      }
      else
      {
        apm_info.attach_pdn_info.ipv4_address = DS_PDN_APM_RCVD_FAIL;
      }
    }
    /*--------------------------------------------------------------
    Process V6 field mask
    ----------------------------------------------------------------*/
    if((pay_load->ip_address.field_mask & DS_PDN_APM_IP_V6_FIELD_MASK))
    {
      if(pay_load->ip_address.v6_address == TRUE)
      {
        apm_info.attach_pdn_info.ipv6_address = DS_PDN_APM_RCVD_SUCC;
      }
      else
      {
        apm_info.attach_pdn_info.ipv6_address = DS_PDN_APM_RCVD_FAIL;
      }
    }
  }


  /*--------------------------------------------------------------
  Process PCSCF field mask
  ----------------------------------------------------------------*/
  if((pay_load->ind_field_mask & DS_APM_IND_PCSCF_FIELD_MASK))
  {
    /*--------------------------------------------------------------
    Process V4 field mask
    ----------------------------------------------------------------*/
    if((pay_load->pcscf_address.field_mask & DS_PDN_APM_IP_V4_FIELD_MASK))
    {
      if(pay_load->pcscf_address.v4_address == TRUE)
      {
        apm_info.attach_pdn_info.ipv4_pcscf_address = DS_PDN_APM_RCVD_SUCC;
      }
      else
      {
        apm_info.attach_pdn_info.ipv4_pcscf_address  = DS_PDN_APM_RCVD_FAIL;
      }
    }
    /*--------------------------------------------------------------
    Process V6 field mask
    ----------------------------------------------------------------*/
    if((pay_load->pcscf_address.field_mask & DS_PDN_APM_IP_V6_FIELD_MASK))
    {
      if(pay_load->pcscf_address.v6_address == TRUE)
      {
        apm_info.attach_pdn_info.ipv6_pcscf_address = DS_PDN_APM_RCVD_SUCC;
      }
      else
      {
        apm_info.attach_pdn_info.ipv6_pcscf_address = DS_PDN_APM_RCVD_FAIL;
      }
    }
  }

  /*--------------------------------------------------------------
  Process DNS field mask
  ----------------------------------------------------------------*/
  if((pay_load->ind_field_mask & DS_APM_IND_DNS_ADDR_FIELD_MASK))
  {
    /*--------------------------------------------------------------
    Process V4 field mask
    ----------------------------------------------------------------*/
    if((pay_load->dns_address.field_mask & DS_PDN_APM_IP_V4_FIELD_MASK))
    {
      if(pay_load->dns_address.v4_address == TRUE)
      {
        apm_info.attach_pdn_info.ipv4_dns_address = 
                                                   DS_PDN_APM_RCVD_SUCC;
      }
      else
      {
        apm_info.attach_pdn_info.ipv4_dns_address = 
                                                   DS_PDN_APM_RCVD_FAIL;
      }
    }
    /*--------------------------------------------------------------
    Process V6 field mask
    ----------------------------------------------------------------*/
    if((pay_load->dns_address.field_mask & DS_PDN_APM_IP_V6_FIELD_MASK))
    {
      if(pay_load->dns_address.v6_address == TRUE)
      {
        apm_info.attach_pdn_info.ipv6_dns_address = 
                                                   DS_PDN_APM_RCVD_SUCC;
      }
      else
      {
        apm_info.attach_pdn_info.ipv6_dns_address = 
                                                   DS_PDN_APM_RCVD_FAIL;
      }
    }
  }

  return;
}/*ds_pdn_apm_payload_processing()*/


/*===========================================================================
FUNCTION       ds_pdn_apm_update_attach_pdn_info

DESCRIPTION    This internal function updates the attach pdn info global 
               structure to reflect an APN Switch or an update from MPPM.
 
PARAMETERS     NONE

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Resets the global attach_pdn_info structure
===========================================================================*/

void ds_pdn_apm_update_attach_pdn_info
(
  void
)
{
  struct fs_stat                     fs_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&fs_buf, 0, sizeof(struct fs_stat));

  /*---------------------------------------------------------------------- 
    This function is responsible for calling API that re-calculate the
    attach profile if the SM is in a valid state and attach APN has changed
   
    We also need to check whether AP has updated the attach profiles list,
    if so we need to read them from the temp file and use for next attach
    accordingly
  ----------------------------------------------------------------------*/
  if(apm_info.attach_pdn_info.if_lte_attach_allowed == FALSE)
  {
    if(apm_info.dsd_apm_default_apm_rule_enabled)
    {
      apm_info.next_attach_pdn_info.next_attach_pdn_type =
                                                          ATTACH_PDN_DEFAULT;
    }
    else
    {
      apm_info.next_attach_pdn_info.next_attach_pdn_type =
                                                          ATTACH_PDN_PRIMARY;
    }
  }

  if (ds_pdn_apm_is_internal_update_allowed() == TRUE)
  {
    if( mcfg_fs_stat(DS_PDN_APM_TMP_ATTACH_PROFILE_FILE,
                     &fs_buf,
                     MCFG_FS_TYPE_EFS,
                     (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1)
          == MCFG_FS_STATUS_OK )
    {
      DS_MSG0(MSG_LEGACY_HIGH,
                "Temp attach PDN list file present, performing EFS read");
      ds_pdn_apm_info_efs_read();
    }
    else
    {
      ds_pdn_apm_set_configuration();
      ds_pdn_apm_reset_and_copy_attach_info();
    }
  }

}

/*===========================================================================
FUNCTION       ds_pdn_apm_ind

DESCRIPTION    This is the function called by external clients. 
                         The clients have to notify the DSD APM module.

DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_ext_error_code ds_pdn_apm_ind
(
  ds_pdn_apm_ind_type        apm_ind_event, 
  ds_pdn_apm_ind_pay_load    *pay_load
)
{
  ds_pdn_apm_ext_error_code   retVal = DS_PDN_APM_SUCCESS;
  sys_plmn_id_s_type      new_plmn_id;
  uint16                  num_rules = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  DS_MSG1(MSG_LEGACY_HIGH,"ds_dsd_apm_ind_internal, EV: %d",
            apm_ind_event);

  switch(apm_ind_event)
  {
    case DS_PDN_APM_ATTACH_INIT_IND:
      ds_pdn_apm_update_attach_pdn_info();
      
      /*-------------------------------------------------------------- 
      LTE attach procedure starts, APM starts run rules
      ----------------------------------------------------------------*/
      ds_pdn_apm_set_ip_rule_required();

      /*-------------------------------------------------------------- 
      Set current attach PDN state to be LTE attaching.
      ----------------------------------------------------------------*/
      apm_info.attach_pdn_info.state = ATTACH_PDN_STATE_CONNECTING;

      /*-------------------------------------------------------------- 
      Print APM queue content.
      ----------------------------------------------------------------*/
      ds_pdn_apm_print_apm_q_content();

      break;

    case DS_PDN_APM_ATTACH_COMPLETE_IND:
      if( pay_load == NULL )
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD;
      }
      else
      {
        apm_info.attach_pdn_info.state = ATTACH_PDN_STATE_CONNECTED;
        ds_pdn_apm_rule(DS_PDN_APM_RULE_ATTACH_SUCCESS);

        /*--------------------------------------------------------------
               NOTIFY DSD PREF SYS Module
               ----------------------------------------------------------------*/
        if(ds_pdn_apm_attach_completed() == TRUE)
        {
          DS_MSG0(MSG_LEGACY_HIGH,"... APM NOTIFIES PREF SYS");
          dssysmgr_attach_complete_ind();
          ds_pdn_apm_unset_ip_rule_required();
        }
      }

      ds_pdn_apm_print_apm_q_content();
      break;

    case DS_PDN_APM_DETACH_IND:
      if( apm_info.attach_pdn_info.ip_rule_required == TRUE )
      {
        DS_MSG1(MSG_LEGACY_HIGH,"LTE detach ind received during normal "
                  "attach procedure, teardown EPC with profile %d", 
                  apm_info.attach_pdn_info.attach_profile_id);
      }
      if(ds_pdn_apm_is_internal_update_allowed() == FALSE)
      {
        apm_info.attach_pdn_info.state = ATTACH_PDN_STATE_DISCONNECTED;
        ds_pdn_apm_unset_ip_rule_required();
      }
      /*-------------------------------------------------------------- 
        APN Switch desired on detach for certain cause codes, if the
        flag is set we invoke the action rule for Switching APN
      ----------------------------------------------------------------*/
      if(pay_load != NULL)
      {
        if(pay_load->ind_field_mask & DS_APM_IND_APN_SWITCH_MASK)
        {
          if(pay_load->switch_apn == TRUE)
          {
            DS_MSG0(MSG_LEGACY_HIGH,"APN Switch Required on Detach. Run Rule");
            ds_pdn_apm_rule(DS_PDN_APM_RULE_DETACH_NORMAL);
          }
        }
      }

      break;

    case DS_PDN_APM_ATTACH_REJECT_IND:
      apm_info.attach_pdn_info.state = ATTACH_PDN_STATE_DISCONNECTED;
      if(pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD; 
      }
      else
      {
        if((pay_load->ind_field_mask & DS_APM_IND_PROFILE_ID_FIELD_MASK))
        {
          if(pay_load->profile_id == apm_info.attach_pdn_info.attach_profile_id)
          {
            if((pay_load->ind_field_mask & DS_APM_IND_APN_SWITCH_MASK))
            {
              if (pay_load->switch_apn)
              {
                DS_MSG0(MSG_LEGACY_HIGH,"APN Switch Required. Run Rule");
                ds_pdn_apm_rule(DS_PDN_APM_RULE_ATTACH_FAIL);
              }
            }
            else
            {
              retVal = DS_PDN_APM_FIELD_MASK_NOT_SET;
            }
          }
          else
          {
            retVal = DS_PDN_APM_INVALID_PROF_ID;
          }
        }
        else
        {
          retVal = DS_PDN_APM_INVALID_PROF_ID;
        }
        ds_pdn_apm_unset_ip_rule_required();
        }
      break;

    case DS_PDN_APM_IP_ADDRESS_IND:
      if( apm_info.attach_pdn_info.ip_rule_required == FALSE )
      {
        DS_MSG0(MSG_LEGACY_HIGH,
                "APM rule is not required, ignore IP/PCSCF addr ind");
        return DS_PDN_APM_SUCCESS;
      }

      if(pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD; 
      }
      else
      {
        if((pay_load->ind_field_mask & DS_APM_IND_PROFILE_ID_FIELD_MASK))
        {
          if(pay_load->profile_id == apm_info.attach_pdn_info.attach_profile_id)
          {
            ds_pdn_apm_payload_processing(pay_load);

            /*--------------------------------------------------------------
              Here we take the action only if we know both DNS and PCSCF
              address is invalid
            ----------------------------------------------------------------*/
            if(DS_PDN_APM_PCSCF_DNS_INVALID == 
                ds_pdn_apm_is_pcscf_and_dns_addr_info_present_and_valid())
            {
              ds_pdn_apm_rule(DS_PDN_APM_RULE_PCSCF_DNS_ADDRESS);
            }

            /*--------------------------------------------------------------
             RUN IP ADDRESS RULE 
            ----------------------------------------------------------------*/
            if((pay_load->ind_field_mask & DS_APM_IND_IP_ADDRESS_FIELD_MASK))
            {
              ds_pdn_apm_rule(DS_PDN_APM_RULE_IP_ADDRESS);
            }

            /*--------------------------------------------------------------
            RUN PCSCF RULE 
            ----------------------------------------------------------------*/
            if((pay_load->ind_field_mask & DS_APM_IND_PCSCF_FIELD_MASK))
            {
              ds_pdn_apm_rule(DS_PDN_APM_RULE_PCSCF_ADDRESS);
            }

            /*--------------------------------------------------------------
            RUN DNS RULE 
            ----------------------------------------------------------------*/
            if(pay_load->ind_field_mask & DS_APM_IND_DNS_ADDR_FIELD_MASK)
            {
              ds_pdn_apm_rule(DS_PDN_APM_RULE_DNS_ADDRESS);
            }

            if(ds_pdn_apm_attach_completed() == TRUE)
            {
              DS_MSG0(MSG_LEGACY_HIGH,"... APM NOTIFIES PREF SYS");
              dssysmgr_attach_complete_ind();
              ds_pdn_apm_unset_ip_rule_required();
            }
          }
          else
          {
            retVal = DS_PDN_APM_INVALID_PROF_ID;
          }
        }
        else
        {
          retVal = DS_PDN_APM_FIELD_MASK_NOT_SET;
        }
      }
      break;
      
    case DS_PDN_APM_PLMN_CHANGED_IND:
      if(pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD; 
      }
      else
      {
        if( pay_load->ind_field_mask & DS_APM_IND_PLMN_FIELD_MASK )
        {
          memset(&new_plmn_id, 0, sizeof(new_plmn_id));
          memscpy(&new_plmn_id,sizeof(new_plmn_id), 
                  &(pay_load->plmn_id), sizeof(sys_plmn_id_s_type));
        /*--------------------------------------------------------------
         Check if PLMN changed
        ----------------------------------------------------------------*/

          if(apm_info.apm_lte_plmn_id.identity[0] == 0 &&
             apm_info.apm_lte_plmn_id.identity[1] == 0 &&
             apm_info.apm_lte_plmn_id.identity[2] == 0 )
          {
            memscpy(&apm_info.apm_lte_plmn_id,sizeof(sys_plmn_id_s_type), 
                    &new_plmn_id, sizeof(sys_plmn_id_s_type));
            return retVal;
          }

          if(((apm_info.apm_lte_plmn_id.identity[0] == new_plmn_id.identity[0]) &&
              (apm_info.apm_lte_plmn_id.identity[1] == new_plmn_id.identity[1]) &&
              (apm_info.apm_lte_plmn_id.identity[2] == new_plmn_id.identity[2])) == FALSE)
          {
            memscpy((void *)&apm_info.apm_lte_plmn_id, sizeof(apm_info.apm_lte_plmn_id), 
                    (const void *)&new_plmn_id, sizeof(new_plmn_id)); 
            ds_pdn_apm_rule(DS_PDN_APM_RULE_PLMN);
          }
        }
        else
        {
          retVal = DS_PDN_APM_FIELD_MASK_NOT_SET;
        }
      }
      break;

    case DS_PDN_APM_SYS_CHANGED_IND:
      if(pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD; 
      }
      else
      {
        if(pay_load->system == SYS_SYS_MODE_NO_SRV)
        {
          DS_MSG0(MSG_LEGACY_HIGH,"UE moves to no service");
        }
        else
        {
          ds_pdn_apm_rule(DS_PDN_APM_RULE_SYS_CHANGE);
        }
      }
      break;

    case DS_PDN_APM_RESET_ATTACH_APN_IND:

        if (pay_load == NULL)
        {
          retVal = DS_PDN_APM_NO_PAY_LOAD;
        }

        ds_pdn_apm_rule(DS_PDN_APM_RULE_RESET_APN);  
        break;

    case DS_PDN_APM_T3402_CHG_IND:
      if (pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD;
      }
      else
      {
       if (pay_load->t3402_timer_val != DS_PDN_APM_INVALID_T3402_TIMER_VAL)
       {
         ds_pdn_apm_set_plmn_blocking_interval_timer(pay_load->t3402_timer_val);
       }
       else
       {
         DS_ERROR_LOG_0("Invalid T3402 Timer value passed");
       }
      }
      break;

    case DS_PDN_APM_RPM_INIT_IND:
        apm_info.dsd_apm_lte_apn_blocking_support = TRUE;
        ds_pdn_apm_set_apn_switch_flag(FALSE);
        break;

    case DS_PDN_APM_TLB_MODE_ACT_IND:
      if(pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD;
      }
      else
      {
        apm_info.attach_pdn_info.is_tlb_mode_active = 
                                                 pay_load->tlb_mode_active;
        retVal = DS_PDN_APM_SUCCESS;
      }
      break;    

    case DS_PDN_APM_ATTACH_PDP_TYPE_IND:
      if(pay_load == NULL)
      {
        retVal = DS_PDN_APM_NO_PAY_LOAD;
      }
      else
      {
        /* Cache the Attach PDP type */
        apm_info.attach_pdn_info.attach_pdp_type
          = pay_load->attach_pdp_type;

        /* Update IP Rules only if using default rules or single attach
           profile rule */
        num_rules = ds_pdn_apm_config_get_apm_rules_num(&ds_pdn_apm_q);
        if( (apm_info.dsd_apm_default_apm_rule_enabled) ||
            (num_rules == DS_DSD_APM_SINGLE_QUEUE_ENTRY) )
        {
          ds_pdn_apm_update_ip_rule(pay_load->attach_pdp_type);
        }

        retVal = DS_PDN_APM_SUCCESS;
      }
      break;

    default:
      retVal = DS_PDN_APM_INVALID_APM;
      break;
  }

  return retVal;
}/* ds_dsd_apm_ind_internal() */

/*===========================================================================
FUNCTION       ds_pdn_apm_req

DESCRIPTION    This is the function called by external clients to 
               request:
                - ATTACH PROFILE_ID.
                - Mandatory P-CSCF
                - Mandatory IP
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_ext_error_code ds_pdn_apm_req
(
  ds_pdn_apm_req_type        apm_req_event, 
  ds_pdn_apm_req_pay_load   *pay_load
)
{
  ds_pdn_apm_ext_error_code    retVal = DS_PDN_APM_SUCCESS;
  apn_info_type           *apn_info_ptr; 
  ds_pdn_apm_data_block   *rule_block = NULL; 
  uint8                    apm_profile_id = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_apm_req, EV:%d",
            apm_req_event);

  if(pay_load == NULL)
  {
    retVal = DS_PDN_APM_NO_PAY_LOAD;
    return retVal;
  }

  switch(apm_req_event)
  {
    case DS_PDN_APM_ATTCH_REQ:
      if( apm_info.attach_pdn_info.if_lte_attach_allowed == FALSE ) 
      {
        return DS_PDN_APM_ATTACH_NOT_ALLOWED;
      }

      if ( apm_info.apm_attach_apn_all_blocked == TRUE ) 
      {
        DS_ERROR_LOG_0("All request attach APN blocked");
        return DS_PDN_APM_ALL_ATTACH_APN_BLOCKED;
      }

      /*--------------------------------------------------------------
       If APN switching feature is disabled, pdn_type will be
       ATTACH_PDN_DEFAULT; if APN switching feature turned on properly,
       pdn_type will be a valid APN class.
      ----------------------------------------------------------------*/
      if( apm_info.attach_pdn_info.pdn_type == ATTACH_PDN_INVALID )
      {
        DS_ERROR_LOG_0("Attach profile id is not initialized");
        pay_load->attach_apn_avail = ds_pdn_apm_if_attach_profiles_available();
        return DS_PDN_APM_INVALID_APM;
      }
      pay_load->req_field_mask = DS_APM_REQ_PROFILE_ID_FIELD_MASK;

      /*--------------------------------------------------------------
      Get next profile..
      ----------------------------------------------------------------*/
      pay_load->profile_id = apm_info.attach_pdn_info.attach_profile_id;
      DS_MSG1(MSG_LEGACY_HIGH,"Return current attach profile %d", 
                pay_load->profile_id);
      break;

    case DS_PDN_APM_ATTACH_APN_AVAIL_REQ:
      if( apm_info.dsd_apm_profile_id_based_switch == FALSE) 
      {
        pay_load->attach_apn_avail = ds_pdn_apm_if_attach_apn_available
                                                     (&ds_pdn_apm_apn_priority_q);
      }
      else
      {
        pay_load->attach_apn_avail = ds_pdn_apm_if_attach_profiles_available();
      }
      break;

    case DS_PDN_APM_MANDATORY_PCSCF_REQ:
    {
      apm_profile_id = pay_load->profile_id;
        rule_block = (ds_pdn_apm_data_block*)q_linear_search( &ds_pdn_apm_q,
                                                ds_pdn_compare_profile_id,
                                                         &(apm_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr =&(rule_block->apn_info);
        DS_MSG2(MSG_LEGACY_HIGH,"Profile %d, PCSCF attach rule %d",
                    apm_profile_id, pay_load->mandatory_pcscf);

        if( pay_load->rule_type == DS_PDN_APM_ATTACH_RULE )
        {
          pay_load->req_field_mask = DS_APM_REQ_PCSCF_FIELD_MASK;
          pay_load->mandatory_pcscf = apn_info_ptr->AttachPdn.
                                           config_rule.pcscf_address;
        }
        else if( pay_load->rule_type == DS_PDN_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_APM_REQ_PCSCF_FIELD_MASK;
          pay_load->mandatory_pcscf = apn_info_ptr->OnDemandPdn.
                                             config_rule.pcscf_address;
        }
        else
        {
          retVal = DS_PDN_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,"No APM rule configured for profile %d",
                  apm_profile_id);
        retVal = DS_PDN_APM_INVALID_APM;
      }
    }
    break;

    case DS_PDN_APM_MANDATORY_IP_REQ:
    {
      apm_profile_id = pay_load->profile_id;
      rule_block = (ds_pdn_apm_data_block*)q_linear_search( &ds_pdn_apm_q,
                                                ds_pdn_compare_profile_id,
                                                      &(apm_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr = &(rule_block->apn_info);
        DS_MSG2(MSG_LEGACY_HIGH,"Profile %d, IP attach rule %d",
                  apm_profile_id, pay_load->mandatory_ip);
        if( pay_load->rule_type == DS_PDN_APM_ATTACH_RULE )
        {
          pay_load->req_field_mask = DS_APM_REQ_IP_ADDRESS_FIELD_MASK;
          pay_load->mandatory_ip = apn_info_ptr->AttachPdn.
                                          config_rule.ip_address;
        }
        else if( pay_load->rule_type == DS_PDN_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_APM_REQ_IP_ADDRESS_FIELD_MASK;
          pay_load->mandatory_ip = apn_info_ptr->OnDemandPdn.
                                           config_rule.ip_address;
        }
        else
        {
          retVal = DS_PDN_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,"No APM rule configured for profile %d",
                  apm_profile_id);
        retVal = DS_PDN_APM_INVALID_APM;
      }
    }
    break;

    case DS_PDN_APM_MANDATORY_DNS_ADDR_REQ:
    {
      apm_profile_id = pay_load->profile_id;
      rule_block = (ds_pdn_apm_data_block*)q_linear_search( &ds_pdn_apm_q,
                                                ds_pdn_compare_profile_id,
                                                         &(apm_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr = &(rule_block->apn_info);
        if( pay_load->rule_type == DS_PDN_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_APM_REQ_DNS_ADDRESS_FIELD_MASK;
          pay_load->mandatory_dns_addr = apn_info_ptr->OnDemandPdn.
                                             config_rule.dns_address;
          DS_MSG2(MSG_LEGACY_HIGH,
                    "Profile %d, DNS Addr on-demand rule %d",
                    apm_profile_id, pay_load->mandatory_dns_addr);
        }
        else
        {
          retVal = DS_PDN_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,
                  "No APM rule configured for profile %d",
                  apm_profile_id);
        retVal = DS_PDN_APM_INVALID_APM;
      }
    }
    break;

    case DS_PDN_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ:
    {
      apm_profile_id = pay_load->profile_id;
      rule_block = (ds_pdn_apm_data_block*)q_linear_search( &ds_pdn_apm_q,
                                                ds_pdn_compare_profile_id,
                                                         &(apm_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr = &(rule_block->apn_info);
        if( pay_load->rule_type == DS_PDN_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_APM_REQ_DNS_ADDRESS_FIELD_MASK;
          pay_load->mandatory_pcscf_or_dns_addr = apn_info_ptr->OnDemandPdn.
                                             config_rule.pcscf_or_dns_address;
          DS_MSG2(MSG_LEGACY_HIGH,
                    "Profile %d, PCSCF or DNS addr on-demand rule %d",
                    apm_profile_id, 
                    pay_load->mandatory_pcscf_or_dns_addr);
        }
        else
        {
          retVal = DS_PDN_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,
                  "No APM rule configured for profile %d",
                  apm_profile_id);
        retVal = DS_PDN_APM_INVALID_APM;
      }
    }
    break;

    default:
      DS_MSG1(MSG_LEGACY_HIGH,"Not supported request: %d", apm_req_event);
      retVal = DS_PDN_APM_UNSEPCIFIED;
      break;
  }

  if (retVal != DS_PDN_APM_SUCCESS) 
  {
    DS_ERROR_LOG_1(" retval =d",retVal);
  }

  return retVal;
}/* ds_pdn_apm_req()*/

/*===========================================================================

FUNCTION DS_DSD_APM_GET_CURRENT_PLMN_IDL

DESCRIPTION
  This function gets the current_plmn_id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_apm_get_current_plmn_id
(
  sys_plmn_id_s_type        *current_plmn_id
)
{
  boolean                   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (NULL != current_plmn_id)
  {
    memscpy(current_plmn_id,sizeof(sys_plmn_id_s_type), 
            &apm_info.apm_lte_plmn_id, sizeof(sys_plmn_id_s_type));
    ret_val = TRUE;
  }
  return ret_val;
} /* ds3g_get_current_plmn_id_internal */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ALL_ATTACH_APN_BLOCKED

DESCRIPTION    This utility function checks if all APNs configured in EFS file 
               blocked. It is called whenever getting forbidden APN list from
               LTE MH.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_if_all_attach_apn_blocked
(
  q_type                                  *priority_q
)
{
  apn_priority_data_block                 *apn_info_block = NULL;
  boolean                                  all_blocked = TRUE;
  uint16                                   profile_ip_type = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( priority_q == NULL ) 
  {
    DS_ERROR_LOG_0("APN prioriy queue ptr is NULL, return FALSE");
    return FALSE;
  }

  apn_info_block = (apn_priority_data_block*)q_check(priority_q);

  if ( apn_info_block == NULL ) 
  {
    DS_MSG0(MSG_LEGACY_HIGH,"No valid apn in priority_q");
    all_blocked = FALSE;
  }

  while( apn_info_block != NULL ) 
  {
    /*------------------------------------------------------------------
     Check if APN is able to use for LTE attach procedure
    -------------------------------------------------------------------*/
    if ( apn_info_block->apn_block_info.apn_type != ATTACH_PDN_TYPE ) 
    {
      apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                              &(apn_info_block->link));
      continue;
    }
    
    /*------------------------------------------------------------------
     Current implementation:
     If v4/v6 APN blocked, v4v6 dual IP profile is allowed
    -------------------------------------------------------------------*/


    profile_ip_type = (apn_info_block->apn_block_info.ip_type_requested) &
                       ~(apn_info_block->apn_block_info.ip_type_blocked); 

    DS_MSG3(MSG_LEGACY_HIGH,"APN len %d, ip_type_request %d, ip_type_blocked %d,",
              apn_info_block->apn_block_info.apn_len,
              apn_info_block->apn_block_info.ip_type_requested,
              apn_info_block->apn_block_info.ip_type_blocked);

    if ( profile_ip_type != 0) 
    {
      DS_MSG2(MSG_LEGACY_HIGH,"APN is not blocked, efs_config type %d, "
                "blocked type %d, continue searching",
                apn_info_block->apn_block_info.ip_type_requested,
                apn_info_block->apn_block_info.ip_type_blocked);
      all_blocked = FALSE;
      break;
    }

    apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                                  &(apn_info_block->link));
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_apm_if_all_attach_apn_blocked: %d", 
            all_blocked);
  return all_blocked;
}/* ds_dsd_apm_if_all_attach_apn_blocked() */

/*===========================================================================
FUNCTION       ds_pdn_apm_set_all_apn_blocked_val

DESCRIPTION    Set the value of if all_apn_blocked
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_all_apn_blocked_val
(
  boolean                   val
)
{

  DS_MSG2(MSG_LEGACY_HIGH,
            "ds_pdn_apm_set_all_apn_blocked_val, val %d, old_val %d", 
            val, apm_info.apm_attach_apn_all_blocked);
  apm_info.apm_attach_apn_all_blocked = val;
  return;
}/* ds_pdn_apm_set_all_apn_blocked_val() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_PROFILES_AVAILABLE

DESCRIPTION    This utility function check if all the attach profiles are 
               available or not. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_pdn_apm_if_attach_profiles_available
(
  void
)
{
  attach_apn_availability_info       reason = 0;
  attach_apn_availability_info       block_reason = 0;
  attach_profile_data_block          *profile_block = NULL;
  boolean                            all_attach_profile_blocked = TRUE;
  attach_apn_availability_info       prof_avail = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  profile_block = (attach_profile_data_block*)q_check(&attach_profile_q);
  if( profile_block == NULL ) 
  {
    return DS_PDN_APM_EFS_CONFIG_ERROR;
  }

  while( profile_block != NULL ) 
  {
    prof_avail = ds_pdn_apm_if_profile_id_available(profile_block->
                                                    attach_profile.profile);
    if( prof_avail == DS_PDN_APM_ATTACH_APN_AVAILABLE ) 
    {
      reason = DS_PDN_APM_ATTACH_APN_AVAILABLE; 
      break;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Profile %d is blocked, continue checking", 
                profile_block->attach_profile.profile);
      if( prof_avail != DS_PDN_APM_ATTACH_APN_BLOCKED) 
      {
        DS_MSG2(MSG_LEGACY_HIGH,"Profile %d is not available, "
                  "and is not blocked by network: reason: %d",
                  profile_block->attach_profile.profile,
                  prof_avail);
        all_attach_profile_blocked = FALSE;
        block_reason = prof_avail;
      }
    }

    profile_block =(attach_profile_data_block*)q_next(&attach_profile_q,
                                                  &(profile_block->link));
  }


  if( reason != DS_PDN_APM_ATTACH_APN_AVAILABLE ) 
  {
    if( all_attach_profile_blocked == TRUE ) 
    {
      reason = DS_PDN_APM_ATTACH_APN_BLOCKED;
    }
    else
    {
      reason = block_reason;
    }
    DS_MSG1(MSG_LEGACY_HIGH,"All attach profiles is blocked: %d",
              all_attach_profile_blocked);
  }

  return reason;
} /* ds_dsd_apm_if_attach_profile_available() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_APN_AVAILABLE

DESCRIPTION    This utility function checks if attach APN is available and why 
               it is not available. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_pdn_apm_if_attach_apn_available
(
  q_type                                  *priority_q
)
{
  apn_priority_data_block                 *apn_info_block = NULL;
  boolean                                  all_blocked = TRUE;
  uint16                                   profile_ip_type = 0;
  attach_apn_availability_info             ret_val = 0;
  boolean                                  attach_apn_configed = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( priority_q == NULL ) 
  {
    return DS_PDN_APM_INTER_ERR;
  }

  apn_info_block = (apn_priority_data_block*)q_check(priority_q);

  if ( apn_info_block == NULL ) 
  {
    return DS_PDN_APM_EFS_CONFIG_ERROR;
  }

  while( apn_info_block != NULL ) 
  {
    /*------------------------------------------------------------------
     Check if APN is able to use for LTE attach procedure
    -------------------------------------------------------------------*/
    if ( apn_info_block->apn_block_info.apn_type != ATTACH_PDN_TYPE ) 
    {
      apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                              &(apn_info_block->link));
      continue;
    }

    /*------------------------------------------------------------------
     Attach APN is configured in EFS.
    -------------------------------------------------------------------*/    
    attach_apn_configed = TRUE;

    /*------------------------------------------------------------------
     Current implementation:
     If v4/v6 APN blocked, v4v6 dual IP profile is allowed
    -------------------------------------------------------------------*/
    profile_ip_type = (apn_info_block->apn_block_info.ip_type_requested) &
                       ~(apn_info_block->apn_block_info.ip_type_blocked); 

    if ( profile_ip_type != 0) 
    {
      all_blocked = FALSE;
      break;
    }

    DS_MSG3(MSG_LEGACY_HIGH,"is APN blocked %d, blocked_type %d, request_ip %d",
              all_blocked,
              apn_info_block->apn_block_info.ip_type_requested,
              apn_info_block->apn_block_info.ip_type_blocked);

    apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                                  &(apn_info_block->link));
  }

  if( all_blocked ) 
  {
    if( attach_apn_configed ) 
    {
      ret_val = DS_PDN_APM_ATTACH_APN_BLOCKED;
    }
    else
    {
      ret_val = DS_PDN_APM_EFS_CONFIG_ERROR;
    }
  }
  else
  {
    ret_val = DS_PDN_APM_ATTACH_APN_AVAILABLE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"Attach APN configurated: %d ",ret_val);

  return ret_val;
}/* ds_dsd_apm_if_attach_apn_available() */

/*===========================================================================
FUNCTION       ds_pdn_apm_refresh_apn_block_queue

DESCRIPTION    Refresh the APN block queue which only called when profile_id_ 
               switching is FALSE(which is the default option, hardcoded now).
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_refresh_apn_block_queue
( 
  ds_pdn_apm_block_list_type  *forbidden_apn_list
)
{
  apn_priority_data_block                           *apn_info_block = NULL;
  uint8                                             i = 0;
  uint16                                            ip_blocked = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------
   Remove the stale APN block info from 
  -------------------------------------------------------------------*/

  ds_pdn_apm_free_block_queue(&ds_pdn_apm_apn_priority_q);

  if( forbidden_apn_list == NULL ) 
  {
    DS_ERROR_LOG_0(
              "Passed forbidden apn list is NULL, no APN blocked");
    return;
  }

  for( i=0; i<forbidden_apn_list->num_apn; i++ )
  {
    if(forbidden_apn_list->pdn_block_info[i].attach_allowed == TRUE)
    {
      DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                         "No APN Blocking needed to be considered for APN: %s",
                         forbidden_apn_list->pdn_block_info[i].apn_name);
      continue;
    }

    DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"APN block info passed:"
                       "apn_name %s, ip_blocked type %d",
                       (forbidden_apn_list->pdn_block_info)[i].apn_name,
                       (forbidden_apn_list->pdn_block_info)[i].ip_type_blocked);
    ip_blocked = (forbidden_apn_list->pdn_block_info)[i].ip_type_blocked;
    /*------------------------------------------------------------------
     If both v4 and v6 IP blocked, that means v4v6 profile is blocked
    -------------------------------------------------------------------*/
    if ( (ip_blocked & DS_PDN_APM_IP_V4_BLOCKED_MASK) &&
         (ip_blocked & DS_PDN_APM_IP_V6_BLOCKED_MASK) )
    {
      ip_blocked = V4_PDN_TYPE | V6_PDN_TYPE | V4_V6_PDN_TYPE;
    }
    DS_SYSTEM_HEAP_MEM_ALLOC(apn_info_block,sizeof(apn_priority_data_block),
		                    apn_priority_data_block*);
    if ( apn_info_block == NULL ) 
    {
      continue;
    }

    DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                       "Add new PDN %s to block list, ip_blocked %d",
                       (forbidden_apn_list->pdn_block_info)[i].apn_name,
                        ip_blocked);
              
    memset(apn_info_block, 0, sizeof(apn_priority_data_block));
    apn_info_block->apn_block_info.apn_len = (forbidden_apn_list->
                                              pdn_block_info)[i].apn_len;
    memscpy(apn_info_block->apn_block_info.apn_name,DS_PDN_APM_MAX_APN_STRING_LEN,
             (forbidden_apn_list->pdn_block_info)[i].apn_name,
             apn_info_block->apn_block_info.apn_len );
    apn_info_block->apn_block_info.ip_type_blocked = ip_blocked;
    apn_info_block->apn_block_info.apn_type = PDN_TYPE_INVALID;
    apn_info_block->apn_block_info.ip_type_requested = V4_PDN_TYPE | 
                                                       V6_PDN_TYPE | 
                                                       V4_V6_PDN_TYPE;
    q_link(apn_info_block, &(apn_info_block->link));
    q_put(&ds_pdn_apm_apn_priority_q, &(apn_info_block->link));
  }
  return;
} /* ds_pdn_apm_refresh_apn_block_queue() */

/*===========================================================================
FUNCTION       ds_pdn_apm_forbidden_apn_list_ind

DESCRIPTION    Indicate the list of blocked APN name and IP type by 3GPP MH. 
               3GPP MH will call this indication at power up and whenever
               blocked APN list changed.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_forbidden_apn_list_ind
( 
  ds_pdn_apm_block_list_type                       *forbidden_apn_list
)
{
  boolean                                           all_apn_blocked = FALSE;
  attach_apn_availability_info                      attach_prof_avai = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( forbidden_apn_list == NULL ) 
  {
    return;
  }

  ds_pdn_apm_refresh_apn_block_queue(forbidden_apn_list);

  attach_prof_avai =  ds_pdn_apm_if_attach_profiles_available();
  if( attach_prof_avai == DS_PDN_APM_ATTACH_APN_BLOCKED ) 
  {
    all_apn_blocked = TRUE;
  }
  else
  {
    all_apn_blocked = FALSE;
  }
  ds_pdn_apm_set_all_apn_blocked_val(all_apn_blocked);

  return;
} /* ds_pdn_apm_forbidden_apn_list_ind */

/*===========================================================================
FUNCTION       ds_pdn_apm_is_apn_switch_support(void)

DESCRIPTION    Indicate if APN switch is supported or not
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_is_apn_switch_support
(
  void
)
{
  boolean                   is_apn_switch = FALSE;

  is_apn_switch = apm_info.dsd_apm_apn_switching_enable;
  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_apm_is_apn_switch_support :%d", 
            is_apn_switch);

  return is_apn_switch;
}/* ds_pdn_apm_is_apn_switch_support() */
/*===========================================================================
FUNCTION       ds_pdn_apm_set_apn_switch_flag

DESCRIPTION    to set if apn switch is enabled
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_apn_switch_flag
(
  boolean                   is_apn_switch
)
{
  apm_info.dsd_apm_apn_switching_enable = is_apn_switch;
  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_apm_set_apn_switch_flag :%d", 
            is_apn_switch);
  return;
}/* ds_pdn_apm_set_apn_switch_flag */


/*===========================================================================
FUNCTION       ds_pdn_apm_update_apn_name

DESCRIPTION    Check if the passed profile_id exists in apn_rule_q.  
               If exists, update the apn_name from profile db

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_update_apn_name
(
  uint8                     prof_id,
  q_type                    *rule_q
)
{
  ds_pdn_apm_data_block   *rule_entry = NULL;
  boolean                  retval = FALSE;
  uint8                    apm_profile_id;
  ds_profile_type_s        prof_info = {{0}};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rule_q == NULL)
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Get the rule block with matching profile id
  ---------------------------------------------------------------------*/
  apm_profile_id = prof_id;
  rule_entry = (ds_pdn_apm_data_block*)q_linear_search(rule_q,
                                       ds_pdn_compare_profile_id,
                                       &apm_profile_id);

  if( rule_entry != NULL)
  {
    /*---------------------------------------------------------------------
      Get the apn_name from profile database
    ---------------------------------------------------------------------*/
    if ( ds_profile_setting_from_profile_id(prof_id,
                                            &prof_info) == DS_PROFILE_RESULT_SUCCESS )
    {
      /*---------------------------------------------------------------------
        update the apn_name in the rule block
      ---------------------------------------------------------------------*/
      rule_entry->apn_info.apn_name_len = 
                             (uint8)strlen((char*)prof_info.context.apn);
      
      memset(rule_entry->apn_info.apn_name,0,DS_PDN_APM_MAX_APN_STRING_LEN);
      strlcpy(rule_entry->apn_info.apn_name,
      	       prof_info.context.apn,
              DS_PDN_APM_MAX_APN_STRING_LEN);
      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"No APM rule configured for profile %d",
              prof_id);
    retval = FALSE;
  }

  return retval;
}/* ds_pdn_apm_update_apn_name() */

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_MAX_ATTACH_PROFILE_NUM

DESCRIPTION    To get current maximum number of attach profiles configured 
 
DEPENDENCIES   NONE

RETURN VALUE   0: succeeds 
              -1: fails 

SIDE EFFECTS   NONE
===========================================================================*/
int ds_pdn_apm_get_max_attach_profile_num
(
  uint8  *max_num_ptr
)
{
  if(ds_pdn_apm_is_apn_switch_support())
  {
    *max_num_ptr = DS_PROFILE_MAX_PROFILE_NUM;
  }
  else
  {
    *max_num_ptr = 1;
  }
  DS_MSG1(MSG_LEGACY_HIGH,
            "ds_psn_apm_get_attach_pdn_list, max num: %d",
            *max_num_ptr);
  return 0;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_ATTACH_PDN_LIST

DESCRIPTION    To get attach PDN list
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE: succeeds 
                         FALSE: fails

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_get_attach_pdn_list
(
  ds_pdn_apm_lte_attach_pdn_list_type      *attach_list_ptr
)
{
  attach_profile_data_block            *profile_block = NULL;
  uint8                                 i = 0; 
  uint8                                 max_attach_list = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(attach_list_ptr == NULL)
  {
    return FALSE;
  }

  memset(attach_list_ptr, 0, sizeof(ds_pdn_apm_lte_attach_pdn_list_type));

  if(ds_pdn_apm_validate_queue_cnt(&attach_profile_q)==FALSE)
  {
    attach_list_ptr->attach_pdn_profile_list_num = i;
    return TRUE;
  }

  /*------------------------------------------------------------------
   The above check makes sure we have attach profiles in the queue.
   
   Now we loop through the queue and store the contents in the info
   pointer passed.
  -------------------------------------------------------------------*/
  profile_block = (attach_profile_data_block*)q_check(&attach_profile_q);

  ds_pdn_apm_get_max_attach_profile_num(&attach_list_ptr->attach_pdn_profile_list_num);

  max_attach_list = min( DS_PROFILE_MAX_PROFILE_NUM,
			 attach_list_ptr->attach_pdn_profile_list_num);

  while (profile_block != NULL)
  {
    if( i >= max_attach_list)
    {
      break;
    }

    attach_list_ptr->attach_pdn_profile_list[i] = profile_block->
                                                          attach_profile.profile;
    DS_MSG1(MSG_LEGACY_HIGH,"get attach pdn list, i:%d",
              attach_list_ptr->attach_pdn_profile_list[i]);
    i++;

    profile_block = (attach_profile_data_block*)q_next(&attach_profile_q,
                                                       &(profile_block->link));
  }

  attach_list_ptr->attach_pdn_profile_list_num = i;

  return TRUE;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_match_cause_codes

DESCRIPTION    This function compares the cause_type & cause code provided by 
               lowers layers with APN Switching cause codes in rule block 
 
PARAMETERS     
               profile_id: Profile whose rule block entry is needed 
               cause_type: Detach/Reject Network cause
               int_cause: Cause code passed by lower layers
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE: If the Rule block entry for profile contains the cause 
               code passed
               FALSE: If no match for cause code found or Rule block entry
               doesn't exist

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_match_cause_codes
(
  uint8                                profile_id,
  uint16                               int_cause,
  ds_pdn_apm_rule_param_name_enum_type    cause_type
)
{
  ds_pdn_apm_cause_code_info_type  *cause_code_block_ptr = NULL;
  ds_pdn_apm_data_block            *rule_block_ptr = NULL;
  boolean                           ret_val = FALSE;
  uint8                             apm_profile_id = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    We extract the APM rule block based on Profile ID and check if a given
    cause code is configured for NETWORK DETACH or REJECT scenario
  -----------------------------------------------------------------------*/
  apm_profile_id = profile_id;

  rule_block_ptr = (ds_pdn_apm_data_block *)q_linear_search(&ds_pdn_apm_q,
                                           ds_pdn_compare_profile_id,
                                           &apm_profile_id);

  if(rule_block_ptr != NULL)
  {
    cause_code_block_ptr = (ds_pdn_apm_cause_code_info_type *)q_check
           (&(rule_block_ptr->apn_info.cause_code_info.cause_code_info_q));
    while(cause_code_block_ptr != NULL)
    {
      if(cause_code_block_ptr->cause_type == cause_type &&
         cause_code_block_ptr->cause_code == int_cause)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
                "Match found, APN Switching for cause code: %d",int_cause);
        ret_val = TRUE;
        break;
      }
      cause_code_block_ptr = (ds_pdn_apm_cause_code_info_type *)q_next
           (&(rule_block_ptr->apn_info.cause_code_info.cause_code_info_q),
            &(cause_code_block_ptr->link));
    }
  }
  return ret_val;
}


/*===========================================================================
FUNCTION       DS_DSD_APM_GET_LAST_CAUSE_BLOCK_PER_TYPE

DESCRIPTION    This utility function traverses the cause code queue maintained 
               per APN block to return a pointer to last block for a particular
               cause type
 
PARAMETERS     apm_rule_block_ptr: Per APN Rule block 
               cause_type: DETACH/REJECT Cause type 
 
DEPENDENCIES   NONE

RETURN VALUE   last_block: Last rule block in the queue for given APN

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_cause_code_info_type *ds_pdn_apm_get_last_cause_block_per_type
(
  ds_pdn_apm_data_block                    *apm_rule_block_ptr,
  uint16                                    cause_type
)
{
  ds_pdn_apm_cause_code_info_type          *cause_code_block_ptr = NULL;
  ds_pdn_apm_cause_code_info_type          *ret_val_block_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(apm_rule_block_ptr != NULL)
  {
    cause_code_block_ptr = (ds_pdn_apm_cause_code_info_type *)q_check
      (&(apm_rule_block_ptr->apn_info.cause_code_info.cause_code_info_q));

    while(cause_code_block_ptr != NULL)
    {
      if(cause_code_block_ptr->cause_type == cause_type)
      {
        ret_val_block_ptr = cause_code_block_ptr;
      }

      cause_code_block_ptr = (ds_pdn_apm_cause_code_info_type *)q_next
        (&(apm_rule_block_ptr->apn_info.cause_code_info.cause_code_info_q),
         &(cause_code_block_ptr->link));
    }
  }
  else
  {
    DS_ERROR_LOG_1("Invalid Rule block ptr: =x", apm_rule_block_ptr);
  }

  return ret_val_block_ptr;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_ipv4_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_ipv4_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
   ind_payload->ind_field_mask |= DS_APM_IND_IP_ADDRESS_FIELD_MASK;
   ind_payload->ip_address.field_mask |= DS_PDN_APM_IP_V4_FIELD_MASK;
   ind_payload->ip_address.v4_address = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_invalid_ipv4_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_invalid_ipv4_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  ds_net_down_reason_type   cause_type
)
{
  ds_pdn_apm_set_ipv4_addr(ind_payload, FALSE);
  ind_payload->ip_address.v4_reason = cause_type;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_ipv6_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_ipv6_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
   ind_payload->ind_field_mask |= DS_APM_IND_IP_ADDRESS_FIELD_MASK;
   ind_payload->ip_address.field_mask |= DS_PDN_APM_IP_V6_FIELD_MASK;
   ind_payload->ip_address.v6_address = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_invalid_ipv6_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_invalid_ipv6_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  ds_net_down_reason_type   cause_type
)
{
  ds_pdn_apm_set_ipv6_addr(ind_payload, FALSE);
  ind_payload->ip_address.v6_reason = cause_type;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_v4_pcscf_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_v4_pcscf_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
  ind_payload->ind_field_mask |= DS_APM_IND_PCSCF_FIELD_MASK;
  ind_payload->pcscf_address.field_mask |= DS_PDN_APM_IP_V4_FIELD_MASK;
  ind_payload->pcscf_address.v4_address = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_v6_pcscf_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_v6_pcscf_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
  ind_payload->ind_field_mask |= DS_APM_IND_PCSCF_FIELD_MASK;
  ind_payload->pcscf_address.field_mask |= DS_PDN_APM_IP_V6_FIELD_MASK;
  ind_payload->pcscf_address.v6_address = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_v4_dns_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_v4_dns_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
  ind_payload->ind_field_mask |= DS_APM_IND_DNS_ADDR_FIELD_MASK;
  ind_payload->pcscf_address.field_mask |= DS_PDN_APM_IP_V4_FIELD_MASK;
  ind_payload->pcscf_address.v4_address = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_v6_dns_addr

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_v6_dns_addr
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
  ind_payload->ind_field_mask |= DS_APM_IND_DNS_ADDR_FIELD_MASK;
  ind_payload->pcscf_address.field_mask |= DS_PDN_APM_IP_V6_FIELD_MASK;
  ind_payload->pcscf_address.v6_address = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_profile_id

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_profile_id
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  uint8                     value
)
{
   ind_payload->ind_field_mask |= DS_APM_IND_PROFILE_ID_FIELD_MASK;
   ind_payload->profile_id = value;
}

/*===========================================================================
FUNCTION       ds_pdn_apm_set_switch_apn_flag

DESCRIPTION    
 
PARAMETERS     

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_set_switch_apn_flag
(
  ds_pdn_apm_ind_pay_load  *ind_payload,
  boolean                   value
)
{
   ind_payload->ind_field_mask |= DS_APM_IND_APN_SWITCH_MASK;
   ind_payload->switch_apn = value;
}

/*===========================================================================
FUNCTION       DS_PDN_APM_APN_CHANGE_CMD_PROCESS

DESCRIPTION    Processing of DSD APM APN change command

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apm_apn_change_cmd_process
(
  uint8   profile_id
)
{
  boolean                  retval = FALSE;
  char                    *apm_rules_string=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*---------------------------------------------------------------------
     If default APM rules are being used, do not update the APM rules cache
     and the EFS file
    ---------------------------------------------------------------------*/
    if(apm_info.dsd_apm_default_apm_rule_enabled == TRUE)
    {
      break;
    }

    /*---------------------------------------------------------------------
      Find rule block corresponding to profile_id and update its apn name 
    ---------------------------------------------------------------------*/
    DS_MSG1(MSG_LEGACY_HIGH,
            "ds_pdn_apm_apn_change_cmd_process on sub %d. profile_id:%d",
            profile_id);

    retval = ds_pdn_apm_update_apn_name(profile_id, &ds_pdn_apm_q);
    if(retval == FALSE)
    {
      break;
    }

    if(ds_pdn_apm_is_generic_apn_switching_enabled())
    {
      break;
    }

    /*---------------------------------------------------------------------
      Create a new string from APM rules queue
    ---------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(apm_rules_string,
                             DS_PDN_APM_PROFILE_MAX_BUFF_SIZE,
                             char*);
    if(apm_rules_string == NULL)
    {
      /*------------------------------------------------------------------
      This condition can not be recovered
      -------------------------------------------------------------------*/
      break;
    }

    memset(apm_rules_string,0,DS_PDN_APM_PROFILE_MAX_BUFF_SIZE);
    retval = ds_pdn_apm_gen_apm_rule_string(&ds_pdn_apm_q, 
                                            apm_rules_string, 
                                            DS_PDN_APM_PROFILE_MAX_BUFF_SIZE);

    if(retval == FALSE)
    {
      break;
    }

    /*---------------------------------------------------------------------
        Overwrite the existing EFS file with new string
      ---------------------------------------------------------------------*/
    ds_pdn_apm_update_apn_rule_efs_file(&ds_pdn_apm_q, apm_rules_string);

  } while (0);

  if(apm_rules_string != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(apm_rules_string);
  }

  return;
}/* ds_pdn_apm_apn_change_cmd_process() */

