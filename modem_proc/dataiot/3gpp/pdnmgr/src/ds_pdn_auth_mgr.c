/*===========================================================================
                      DS_PDN_APN_TABLE
DESCRIPTION
This file defines PDN auth manager APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_auth_mgr.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_auth_mgr.h"
#include "ds_pdn_nv_manager.h"
#include "list.h"
#include <stringl/stringl.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DS_PDN_AUTH_FB_TBL_SIZE (16) /* Max number of entries in the 
                                         auth_type_fb_tbl */

#define DS_PDN_AUTH_MGR_MAX_RETRIES (3)

typedef struct
{
  list_link_type             link;
  char                       apn[DS_PROFILE_APN_STRING_LEN + 1];
  ds_profile_auth_enum_type  auth_type;
  boolean                    success;  /* if TRUE, call succeeded to apn 
                                         using the auth_type in this entry*/
  int                        count;    /* counts the number of attempts 
                                         before a call is sucessful */
} ds_pdn_auth_mgr_entry_s;

/*===========================================================================
   This Structure is used to maintain all global variables of ds_3gpp_auth_mgr.
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW GLOBAL VARIABLE.
===========================================================================*/
typedef struct
{
  list_type list;
}ds_pdn_auth_mgr_info_type;

static ds_pdn_auth_mgr_info_type ds_pdn_auth_mgr_info = {NULL};



/*===========================================================================

                           FUNCTION DEFINITIONS

===========================================================================*/ 


/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_GET_NEW_ENTRY

DESCRIPTION
  This function allocates a new entry of type
  ds_pdn_auth_mgr_entry_s from heap.
 
PARAMETERS   : 
  None

DEPENDENCIES
  None

RETURN VALUE
 Pointer to the newly allocated chunk of memory.

SIDE EFFECTS
  None
===========================================================================*/
    
LOCAL ds_pdn_auth_mgr_entry_s*
ds_pdn_auth_mgr_get_new_entry( void )
{
  ds_pdn_auth_mgr_entry_s *entry = NULL;
  DS_SYSTEM_HEAP_MEM_ALLOC(entry, sizeof(ds_pdn_auth_mgr_entry_s),
  	                       ds_pdn_auth_mgr_entry_s*);
  if(entry)
  {
    memset(entry, 0, sizeof(ds_pdn_auth_mgr_entry_s));
  }
  return entry;
} /* ds_3gpp_auth_mgr_get_new_entry() */

/*===========================================================================
FUNCTION DS_PDN_AUTH_NEXT

DESCRIPTION
  Given an auth_type, the function returns the next auth type in
  sequence to try during auth fallback. The sequence used in auth
  fallback is {CHAP, PAP, NONE}

PARAMETERS   :
  auth_type  -  Authentication type 

DEPENDENCIES
  None

RETURN VALUE
  The next auth_type to try in sequence.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds_profile_auth_enum_type
ds_pdn_auth_next(ds_profile_auth_enum_type auth_type)
{
  /*------------------------------------------------------------------
     Return the next auth type to try in the cycle
   ------------------------------------------------------------------*/
  switch(auth_type)
  {
  case DS_PROFILE_AUTH_CHAP:
    return DS_PROFILE_AUTH_PAP;
  case DS_PROFILE_AUTH_PAP:
    return DS_PROFILE_AUTH_NONE;
  case DS_PROFILE_AUTH_NONE:
    return DS_PROFILE_AUTH_CHAP;
  default:
    DS_ERROR_LOG_1("ds_pdn_auth_next: Invalid auth_type =d", auth_type);
    return auth_type;
  }
} /* ds_3gpp_auth_next() */

/*===========================================================================
FUNCTION DS_PDN_AUTH_APN_COMPARE

DESCRIPTION
  This function is used as a support function for
  list_linear_search(). Compares the apn string with an auth table
  entry.

PARAMETERS   :
  entry  -  Auth manager list entry
  apn    -  Apn to compare with
DEPENDENCIES
  None

RETURN VALUE
  The next auth_type to try in sequence.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL int
ds_pdn_auth_mgr_apn_compare
(
  void *item_ptr,
  void *compare_val
)
{
  ds_pdn_auth_mgr_entry_s *entry;
  
  entry = (ds_pdn_auth_mgr_entry_s *) item_ptr;
  return (strcasecmp((char *)entry->apn, (char *)compare_val) == 0);
} /* ds_3gpp_auth_mgr_apn_compare()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_LOOKUP_ENTRY

DESCRIPTION
  This function searches for an entry in the auth table, using
  the apn string. It should only be used if auth fallback feature is
  enabled, since it accesses ds_3gpp_auth_mgr_list. 

PARAMETERS   :
  apn      -  Apn name to be used for the table lookup

DEPENDENCIES
  None

RETURN VALUE
  If entry found, a pointer to an entry in the auth table
  (ds_3gpp_auth_fb_tbl)
  Else NULL

SIDE EFFECTS
   
===========================================================================*/ 
LOCAL ds_pdn_auth_mgr_entry_s *
ds_pdn_auth_mgr_lookup_entry
(
  const char              *apn
)
{
  list_type*                list_ptr = NULL;
  ds_pdn_auth_mgr_entry_s* list_entry_ptr = NULL;
  /*------------------------------------------------------------------------*/

  list_ptr = &(ds_pdn_auth_mgr_info.list);

  if (list_ptr != NULL)
  {
    list_entry_ptr =  
      list_linear_search(list_ptr, ds_pdn_auth_mgr_apn_compare,(char*)apn);
  }

  return list_entry_ptr; 
} /* ds_3gpp_auth_mgr_lookup_entry()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_INIT_ENTRY

DESCRIPTION
  This function initializes an auth table entry 

PARAMETERS   :
  entry   -      Pointer to entry to be initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 
===========================================================================*/
LOCAL  void
ds_pdn_auth_mgr_init_entry
(
  ds_pdn_auth_mgr_entry_s *entry,
  const char               *apn
)
{
  memset((void *)entry->apn, 0, DS_PROFILE_APN_STRING_LEN + 1);
  strlcpy((char *)entry->apn, (char *)apn, DS_PROFILE_APN_STRING_LEN + 1);
  entry->count = 0;
  entry->success = FALSE;
  entry->auth_type = DS_PROFILE_AUTH_CHAP;
} /*ds_3gpp_auth_mgr_init_entry()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_UPDATE_ENTRY

DESCRIPTION
  This function updates an entry in ds_3gpp_auth_fb_tbl. List of valid
  updates:
  Auth successful scenario -
  (auth_type, success, count) = {auth_type, true, 0}
  Auth failure scenario -
  (auth_type, success, count) = {next(auth_type), false, count + 1}
 
PARAMETERS   :
  entry  -  Pointer to the entry to be updated.
  success - Specifies the type of update
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void 
ds_pdn_auth_mgr_update_entry
(
  ds_pdn_auth_mgr_entry_s   *entry,
  boolean                    success
)
{  
  /*------------------------------------------------------------------ 
     Entry exists. Update
   ------------------------------------------------------------------*/
  if(success == TRUE)
  {
    /*------------------------------------------------------------------ 
      ds_3gpp_auth_mgr_update_entry called because of auth success
     ------------------------------------------------------------------*/
    entry->success = TRUE;
    entry->count = 0;
  }
  else 
  {
    /*------------------------------------------------------------------ 
      ds_3gpp_auth_mgr_update_entry called because of auth failure
     ------------------------------------------------------------------*/
    entry->success = FALSE;
    entry->count++;
    entry->auth_type = ds_pdn_auth_next(entry->auth_type);
  }
} /*ds_3gpp_auth_mgr_update_entry()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_EVICT_ENTRY

DESCRIPTION
  This function removes and returns the least recently used entry from
  the auth manager list.
 
PARAMETERS   :
  List Ptr

DEPENDENCIES
  None

RETURN VALUE
 Pointer to the evicted entry

SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds_pdn_auth_mgr_entry_s * ds_pdn_auth_mgr_evict_entry
(
  list_type                *list_ptr
)
{
  ds_pdn_auth_mgr_entry_s  *auth_mgr_entry_ptr = NULL;
  /*----------------------------------------------------------------------*/

  if (list_ptr != NULL)
  {
    auth_mgr_entry_ptr = (ds_pdn_auth_mgr_entry_s *)list_pop_back(list_ptr);
  }

  return auth_mgr_entry_ptr;

} /* ds_3gpp_auth_mgr_evict_entry()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_CREATE_ENTRY

DESCRIPTION
  This function creates an auth manager list entry
  The new entry has values (auth_type, success, count) = {CHAP, FALSE,
  0} 
 
PARAMETERS   : 
  apn     -  Apn name for which an entry is being created

DEPENDENCIES
  None

RETURN VALUE
 On success, returns a pointer to the newly created entry
 On failure, NULL
 
SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds_pdn_auth_mgr_entry_s *
ds_pdn_auth_mgr_create_entry
(
  const char             *apn
)
{
  ds_pdn_auth_mgr_entry_s *entry = NULL;
  list_type                *list_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  list_ptr = &(ds_pdn_auth_mgr_info.list);

  if (list_ptr == NULL)
  {
    DS_ASSERT(0);
    return NULL;
  }

  if(list_size(list_ptr) == DS_PDN_AUTH_FB_TBL_SIZE)
  {
    /*------------------------------------------------------------------ 
      List is full, so we try to evict an entry
     ------------------------------------------------------------------*/
    entry = ds_pdn_auth_mgr_evict_entry(list_ptr);
  }
  else
  {
    /*------------------------------------------------------------------     
     List is not full. We allocate a new entry in the list. If alloc
     fails, we try to evict.
    -------------------------------------------------------------------*/
    entry = ds_pdn_auth_mgr_get_new_entry();
  }
  
  if(entry != NULL) 
  {
    list_push_front(list_ptr, &(entry->link));
    ds_pdn_auth_mgr_init_entry(entry, apn);
  }

  return entry;
}/*ds_3gpp_auth_mgr_create_entry()*/


/*===========================================================================
  FUNCTION DS_PDN_AUTH_MGR_CLEAR_INFO

  DESCRIPTION
  This function performs resetting of the PDN auth module info.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_auth_mgr_clear_info
(
  void
)
{
  list_type                *list_ptr = NULL;
  ds_pdn_auth_mgr_entry_s  *auth_ptr = NULL;
  ds_pdn_auth_mgr_entry_s  *next_auth_ptr = NULL;

  DS_MSG0(MSG_LEGACY_HIGH, "Clearing auth info");

  list_ptr = &ds_pdn_auth_mgr_info.list;
  if(list_ptr != NULL)
  { 
    auth_ptr = (ds_pdn_auth_mgr_entry_s*)list_peek_front( list_ptr );

    while (auth_ptr != NULL)
    {
      next_auth_ptr = list_peek_next(list_ptr, &(auth_ptr->link));
      list_pop_item(list_ptr, &(auth_ptr->link) ); 
      DS_SYSTEM_HEAP_MEM_FREE(auth_ptr);
      auth_ptr = next_auth_ptr;
    }
  }

  return;
}/* ds_pdn_auth_mgr_clear_info */

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_INIT

DESCRIPTION
  This function initializes the auth manager. It sets up the auth
  table (ds_3gpp_auth_fb_tbl).
 
PARAMETERS
  is_init          : Is it part of power up init

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void ds_pdn_auth_mgr_init(boolean is_init )
{
  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_init: Auth fallback : %d",
  	       ds_pdn_nv_manager_get_auth_fallback());

  if(is_init)
  {
    list_init(&(ds_pdn_auth_mgr_info.list));
  }
  else
  {
    ds_pdn_auth_mgr_clear_info();
  }
  return;

} /*ds_3gpp_auth_mgr_init()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_NEED_REDIAL

DESCRIPTION
  Queris the auth manager if a redial is required.
 
PARAMETERS   :
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  cause_code           - PS IFACE cause code specifying the reason for
                         prior termination

DEPENDENCIES 
  None

RETURN VALUE
  TRUE - if redialing is required
  FALSE - Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean ds_pdn_auth_mgr_need_redial
(
  char                          *apn,
  ds_profile_auth_enum_type      requested_auth_type,
  ds_net_down_reason_type        cause_code,
  boolean                        call_was_connected
)
{
  ds_pdn_auth_mgr_entry_s *entry;

  /*-------------------------------------------------------------------------*/

  if( (ds_pdn_nv_manager_get_auth_fallback() == FALSE) || 
  	   (requested_auth_type != DS_PROFILE_AUTH_CHAP_OR_PAP) || 
  	   (cause_code != DS_NET_DOWN_REASON_AUTH_FAILED &&
       cause_code != DS_NET_DOWN_REASON_ACTIVATION_REJECT) ||
      (call_was_connected) )
  {
    DS_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_need_redial: No"
                     " redial. APN: %s, requested_auth_type: %d,"
                     " cause_code: %d", (char *) apn,
                     requested_auth_type, cause_code);
    return FALSE;
  }

  entry = ds_pdn_auth_mgr_lookup_entry(apn);
  if(entry == NULL)
  {
    /*------------------------------------------------------------------ 
       Lookup failed. We dont have info to determine if we should
       redial.
     ------------------------------------------------------------------*/
    return FALSE;
  }
  /*------------------------------------------------------------------ 
       Lookup succeeded. We dont have to examine count.
       ds_3gpp_auth_mgr_call_failure() removes the entry if max retries
       has exceeded. Return TRUE.
  ------------------------------------------------------------------*/
  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_need_redial: "
                   "Return True. Apn: %s, count: %d", (char *) apn,
                   entry->count);

  return TRUE;
} /*ds_3gpp_auth_mgr_need_redial()*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_GET_AUTH_TYPE

DESCRIPTION
  Queries the auth manager for the appropriate auth_type to use for an
  outgoing call.
 
PARAMETERS   : 
  IN
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  OUT
  auth_type_to_use_p  - Pointer to the auth_type to be used

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the LRU auth manager list 
===========================================================================*/  
void ds_pdn_auth_mgr_get_auth_type 
(
  const char                  *apn, 
  ds_profile_auth_enum_type    requested_auth_type, 
  ds_profile_auth_enum_type   *auth_type_to_use_p
)
{
  ds_pdn_auth_mgr_entry_s *entry;
  list_type*                list_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  list_ptr = &(ds_pdn_auth_mgr_info.list);

  if(list_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  if(auth_type_to_use_p == NULL)
  {
    return;
  }

  *auth_type_to_use_p = requested_auth_type;

  if( (ds_pdn_nv_manager_get_auth_fallback() == TRUE) &&
      (requested_auth_type == DS_PROFILE_AUTH_CHAP_OR_PAP))
  {
    entry = ds_pdn_auth_mgr_lookup_entry(apn);
    if(entry == NULL)
    {
      /* Entry does not exist, create it*/
      entry = ds_pdn_auth_mgr_create_entry(apn);    
      
      if(entry != NULL)
      {
        /*------------------------------------------------------------------ 
          Create succeeded, we try with auth type CHAP
         ------------------------------------------------------------------*/
        *auth_type_to_use_p = DS_PROFILE_AUTH_CHAP;
      }
    }
    else
    {
      /*------------------------------------------------------------------ 
        Entry exists, use auth type from fallback table
       ------------------------------------------------------------------*/
      *auth_type_to_use_p = entry->auth_type;

      /*------------------------------------------------------------------ 
        Re-order list as LRU
       ------------------------------------------------------------------*/
      list_pop_item(list_ptr, &(entry->link));
      list_push_front(list_ptr, &(entry->link));
    }
  }

  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_get_auth_type:"
                   "returns auth_type: %d apn: %s",*auth_type_to_use_p,
                   (char *)apn);
  return;
} /*ds_3gpp_auth_mgr_get_auth_type() */

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_CALL_SUCCESS

DESCRIPTION
  This function notifies the auth manager of a call success event.
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void ds_pdn_auth_mgr_call_success 
(
  char                       *apn, 
  ds_profile_auth_enum_type   requested_auth_type
)
{
  ds_pdn_auth_mgr_entry_s * entry;
  /*-------------------------------------------------------------------------*/

  if( (ds_pdn_nv_manager_get_auth_fallback() == FALSE) ||
      (requested_auth_type != DS_PROFILE_AUTH_CHAP_OR_PAP) )
  {
    /*------------------------------------------------------------------ 
      Not an auth fallback scenario, nothing to do
     ------------------------------------------------------------------*/
    return;
  }

  entry = ds_pdn_auth_mgr_lookup_entry(apn);
  if(entry == NULL)
  {
    return;
  }

  ds_pdn_auth_mgr_update_entry(entry, TRUE);

  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_call_success:"
                   " APN: %s, auth_type: %d", (char *)apn, entry->auth_type);
  return;
} /* ds_3gpp_auth_mgr_call_success() */

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_CALL_FAILURE

DESCRIPTION
  This function notifies the auth manager of a call failure event..
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void ds_pdn_auth_mgr_call_failure 
(
  char                          *apn, 
  ds_profile_auth_enum_type     requested_auth_type, 
  ds_net_down_reason_type       cause_code,
  boolean                       call_was_connected
)
{
  ds_pdn_auth_mgr_entry_s *entry;
  list_type                *list_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  list_ptr = &(ds_pdn_auth_mgr_info.list);

  if (list_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  if( (ds_pdn_nv_manager_get_auth_fallback() == FALSE)|| 
  	   (requested_auth_type != DS_PROFILE_AUTH_CHAP_OR_PAP) ||
      (cause_code != DS_NET_DOWN_REASON_AUTH_FAILED &&
       cause_code != DS_NET_DOWN_REASON_ACTIVATION_REJECT) ||
      (call_was_connected))
  {
    return;
  }
 
  entry = ds_pdn_auth_mgr_lookup_entry(apn);
  if(entry == NULL)
  {
    return;
  }
  
  if(entry->count == DS_PDN_AUTH_MGR_MAX_RETRIES - 1) 
  {
    /*------------------------------------------------------------------ 
      This was the last unsuccessful try with an auth_type. We remove the
      entry. need_redial() will return false if the entry is not found.
     ------------------------------------------------------------------*/
    DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_call_failure:"
                     " Max retries exceeded.APN: %s", (char *)apn);
    list_pop_item(list_ptr, &(entry->link));
    DS_SYSTEM_HEAP_MEM_FREE(entry);
    return;
  }

  /*------------------------------------------------------------------ 
    Update the entry to reflect the new auth failure
   ------------------------------------------------------------------*/
  ds_pdn_auth_mgr_update_entry(entry, FALSE);
  DS_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"ds_pdn_auth_mgr_call_failure:"
                   " APN: %s, auth_type: %d, count: %d", 
                   (char *)apn, entry->auth_type, entry->count);
  return;
} /* ds_3gpp_auth_mgr_call_failure() */


