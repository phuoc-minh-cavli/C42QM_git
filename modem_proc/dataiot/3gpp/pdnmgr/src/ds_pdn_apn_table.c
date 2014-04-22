/*===========================================================================
                      DS_PDN_APN_TABLE
DESCRIPTION
This file defines APN table APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_apn_table.c#4 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ran.h"
#include <stringl/stringl.h>
#include "ds_pdn_apn_table.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_nv_manager.h"


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------
  APN table which stores the known PDN IP support returned by NW during
  bearer activation.  
  ref TS24.301 v810 6.2.2 
---------------------------------------------------------------------------*/
ds_pdn_mgr_apn_table_s *ds_pdn_apn_table[DS_MAX_APN_NUM] = {NULL};

/*===========================================================================
FUNCTION ds_apn_table_validate_index

DESCRIPTION
  This function checks if an index in the APN table has been allocated memory

PARAMETERS
  index        - index to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_validate_index
(
  int                    index
)
{
  boolean                 ret_val = FALSE;
  ds_pdn_mgr_apn_table_s *apn_table_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((index < 0) || (index >= DS_MAX_APN_NUM))
  {
    DS_ERROR_LOG_1("Index:=d is out of bounds",index);
    return ret_val;
  }

  apn_table_p = ds_pdn_apn_table[index];
  if((apn_table_p != NULL) && (apn_table_p->cookie == APN_VALID_COOKIE))
  {
    ret_val = TRUE;
  }
    
  return ret_val;
}

/*===========================================================================
FUNCTION ds_apn_table_alloc_dyn_mem

DESCRIPTION
  This function allocates dynamic memory and also determine an index.

PARAMETERS
  *index       - returns the allocated index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If allocation was successful
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_alloc_dyn_mem
(
  uint8        *alloc_index
)
{
  boolean ret_val = FALSE;
  int     index = 0;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_MAX_APN_NUM; index++)
  {
    if(!ds_apn_table_validate_index(index))
    {  
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_pdn_apn_table[index], sizeof(ds_pdn_mgr_apn_table_s),
	  	                       ds_pdn_mgr_apn_table_s *);

      if (ds_pdn_apn_table[index] != NULL)
      {
        DS_MSG1(MSG_LEGACY_HIGH, "APN table dyn memory allocated at index:%d",
                                 index);
        /*--------------------------------------------------------------------
              Memset the dynamic memory to 0 before using it
              --------------------------------------------------------------------*/
        memset(ds_pdn_apn_table[index], 0, sizeof(ds_pdn_mgr_apn_table_s));
        ds_pdn_apn_table[index]->cookie = APN_VALID_COOKIE;
        *alloc_index = index;
        ret_val = TRUE;
      }
      else
      {
        ret_val = FALSE;
      }
      
      break;
    }
  }

  if (index == DS_MAX_APN_NUM)
  {
    DS_ERROR_LOG_0("Max APN contexts have been allocated");
  }

  return ret_val;
}

/*===========================================================================
FUNCTION ds_apn_table_free_dyn_mem

DESCRIPTION
  This function frees dynamic memory based on the index passed. 

PARAMETERS
   index       - Index to be freed
    
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_apn_table_free_dyn_mem
(
  uint8           free_index
)
{

  if( free_index >= DS_MAX_APN_NUM )
  {
    DS_ERROR_LOG_1("Index passed is invalid:=d",free_index);
  }
  else
  {
   /*------------------------------------------------------------------- 
       Free the dynamic memory
    -------------------------------------------------------------------*/
    if (ds_pdn_apn_table[free_index] != NULL)
    {      
      // Reset the cookie
      ds_pdn_apn_table[free_index]->cookie = 0;

      DS_SYSTEM_HEAP_MEM_FREE(ds_pdn_apn_table[free_index]);
      ds_pdn_apn_table[free_index] = NULL;
      DS_MSG1(MSG_LEGACY_HIGH, 
      	       "APN Table dyn memory index:%d freed",free_index);
    }
  }

  return;
}/* ds_apn_table_free_dyn_mem */

/*============================================================
FUNCTION DS_APN_TABLE_GET_APN_TABLE_PTR

DESCRIPTION
  This function searchs the APN table and returns the APN table ptr of a 
  particular APN

PARAMETERS
  APN name
  APN table pointer - to be filled
   
DEPENDENCIES
  None.

RETURN VALUE
  loop index - Index in the APN table, if found
  -1            - If not entry found 

SIDE EFFECTS
  None
=============================================================*/
int ds_apn_table_get_apn_table_ptr 
(
  char                      *apn,
  ds_pdn_mgr_apn_table_s   **apn_ptr
)
{
  uint8 loop = 0;
  
  for( loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if ( (ds_apn_table_validate_index(loop)) &&
         ( (strncasecmp((char*)ds_pdn_apn_table[loop]->apn, 
                        (char *)apn,DS_PROFILE_APN_STRING_LEN+1) == 0)||
           ( ((char)ds_pdn_apn_table[loop]->resolved_apn[0] != '\0') && 
             (strncasecmp((char*)ds_pdn_apn_table[loop]->resolved_apn, 
                          (char*)apn, DS_PROFILE_APN_STRING_LEN+1) == 0) ) )
       )  
    {
      *apn_ptr = ds_pdn_apn_table[loop];
      return loop;
    }
  }
  
  return -1;
}/* ds_apn_table_get_apn_table_ptr */

/*============================================================
FUNCTION DS_APN_TABLE_GET_APN_IP_SUPPORT_INFO

DESCRIPTION
  This function searchs the APN table and returns the ip support type of a 
  particular APN

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
=============================================================*/
boolean ds_apn_table_get_apn_ip_support_info 
(
  char                      *apn,
  ds_apn_ip_support_type_e  *ip_support
)
{
  ds_pdn_mgr_apn_table_s  *apn_ptr = NULL;

  if(ds_apn_table_get_apn_table_ptr(apn, &apn_ptr) != -1)
  {
    *ip_support = apn_ptr->ip_support;
    return TRUE;
  }
  
  return FALSE;
}/* ds_apn_table_get_apn_ip_support_info */

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_V4_AND_V6_PDN

DESCRIPTION
  This function returns the v4 and v6 PDN pointers corresponding to 
  any given PDN entry in APN table. One(or both) of the PDN returned could be 
  same as the input PDN. This API is used to clearly diff bet v4 and v6 PDN.

PARAMETERS   :
  input_pdn_cntx_p*                 - input PDN pointer 
  v4_pdn_cntx_p** (OUT)-               Pointer to the v4 context 
  v6_pdn_cntx_p** (OUT)-               Pointer to the v6 context
      
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_get_v4_and_v6_pdn
(
  ds_pdn_mgr_s          *input_pdn_cntx_p,
  ds_pdn_mgr_s         **v4_pdn_cntx_p,
  ds_pdn_mgr_s         **v6_pdn_cntx_p
)
{
  int                    loop = 0;
  boolean                retval = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  for( loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if ( (ds_apn_table_validate_index(loop)) &&
         ( (ds_pdn_apn_table[loop]->v4_pdn_ptr == input_pdn_cntx_p) ||
           (ds_pdn_apn_table[loop]->v6_pdn_ptr == input_pdn_cntx_p) ) )
    {
      *v4_pdn_cntx_p = ds_pdn_apn_table[loop]->v4_pdn_ptr;
      *v6_pdn_cntx_p = ds_pdn_apn_table[loop]->v6_pdn_ptr;
      retval = TRUE;
      break;
    }
  }

  return retval;
}


/*===========================================================================
FUNCTION DS_APN_TABLE_GET_V4_PDN

DESCRIPTION
  This function returns the pointer to the V4 PDN

PARAMETERS   :
  byte*                  - APN name
  ds_pdn_mgr_s**  - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_v4_pdn
(
  char                      *apn,
  ds_pdn_mgr_s             **pdn_mgr_p
)
{
  ds_pdn_mgr_apn_table_s  *apn_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_apn_table_get_apn_table_ptr(apn, &apn_ptr) != -1)
  {
    *pdn_mgr_p = apn_ptr->v4_pdn_ptr;
    return TRUE;
  }
  
  return FALSE;
} /* ds_apn_table_get_v4_pdn */

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_V6_PDN

DESCRIPTION
  This function returns the pointer to the V6 PDN

PARAMETERS   :
  byte*                  - APN name
  ds_pdn_mgr_s** - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_v6_pdn
(
  char                    *apn,
  ds_pdn_mgr_s           **pdn_mgr_p
)
{  
  ds_pdn_mgr_apn_table_s  *apn_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_apn_table_get_apn_table_ptr(apn, &apn_ptr) != -1)
  {
    *pdn_mgr_p = apn_ptr->v6_pdn_ptr;
    return TRUE;
  }
   
  return FALSE;
} /* ds_apn_table_get_v6_pdn */

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_NON_IP_PDN

DESCRIPTION
  This function returns the pointer to the nonIP PDN

PARAMETERS   :
  byte*                  - APN name
  ds_pdn_mgr_s** - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_non_ip_pdn
(
  char                    *apn,
  ds_pdn_mgr_s           **pdn_mgr_p
)
{  
  ds_pdn_mgr_apn_table_s  *apn_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_apn_table_get_apn_table_ptr(apn, &apn_ptr) != -1)
  {
    *pdn_mgr_p = apn_ptr->non_ip_pdn_ptr;
    return TRUE;
  }
   
  return FALSE;
} /* ds_apn_table_get_non_ip_pdn */

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_PDN_MATCH_APN_INDEX

DESCRIPTION
  This function returns status if any of the PDN in passed APN index matches with the passed
  PDN pointer.

PARAMETERS   :
  uint8                 - Index for APN table
  ds_pdn_mgr_s* - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If the passed index entry has any PDN matching with the passed PDN
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_pdn_match_apn_index
(
  uint8                   index,
  ds_pdn_mgr_s           *pdn_mgr_p
)
{  
  if ((ds_apn_table_validate_index(index)) &&
      ((ds_pdn_apn_table[index]->v4_pdn_ptr == pdn_mgr_p) ||
       (ds_pdn_apn_table[index]->v6_pdn_ptr == pdn_mgr_p) ||
       (ds_pdn_apn_table[index]->non_ip_pdn_ptr == pdn_mgr_p)))
  {
    return TRUE;
  }
   
  return FALSE;
} /* ds_apn_table_get_non_ip_pdn */

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_PDN_POINTER

DESCRIPTION
  This function updates the PDN context pointer field of the APN table based
  on IP support type

PARAMETERS   :
    
DEPENDENCIES
  Remember to call this function before you update the IP support of the 
  entry in the APN table. Should the order be reversed, APN table behaviour
  will be unpredictable.

RETURN VALUE
  TRUE - Successfully updated APn table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_apn_table_update_pdn_pointer
(
  int                       index,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_mgr_s             *pdn_ptr
)
{
  ds_pdn_mgr_apn_table_s   *apn_table_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_ptr) ||
      ds_apn_table_validate_index(index) == FALSE)
  {
    DS_ERROR_LOG_0("Invalid parameter for APN table update.");
    return;
  }
  DS_ASSERT(ip_support != DS_IP_TYPE_INVALID);
  apn_table_p = ds_pdn_apn_table[index];

  switch (ip_support)
  {
    case DS_IPV4_ONLY:
    case DS_IPV4_SINGLE_BEARER:
      if(apn_table_p->ip_support == DS_IPV4V6 ||
         apn_table_p->ip_support == DS_IPV4V6_NON_IP)
      {
        /*----------------------------------------------------------------------
              This is a transition from Dual IP to single IP or V4 only
              Clean up the other PDN context pointer
             ----------------------------------------------------------------------*/
        apn_table_p->v6_pdn_ptr = NULL;
      }
      else if((apn_table_p->ip_support == DS_SINGLE_BEARER) ||
              (apn_table_p->ip_support == DS_IP_TYPE_INVALID)||
              (apn_table_p->ip_support == DS_IPV4_ONLY) ||
              (apn_table_p->ip_support == DS_NON_IP))
      {
        /*----------------------------------------------------------------------
              Here we just want to add another PDN pointer to the APN table entry
              ----------------------------------------------------------------------*/
        apn_table_p->v4_pdn_ptr = pdn_ptr;
      }
      else if((apn_table_p->ip_support == DS_IPV6_ONLY) &&
              (ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == TRUE))
      {
        /*----------------------------------------------------------------------
              For multi PDN same APN case, we need to add PDN pointer to the same APN table entry
              ----------------------------------------------------------------------*/
        apn_table_p->v4_pdn_ptr = pdn_ptr;
      }
      break;
      
    case DS_IPV6_ONLY:
    case DS_IPV6_SINGLE_BEARER:
      if(apn_table_p->ip_support == DS_IPV4V6 ||
         apn_table_p->ip_support == DS_IPV4V6_NON_IP)
      {
        /*----------------------------------------------------------------------
              This is a transition from Dual IP to single IP or V6 only
              Clean up the other PDN context pointer
              ----------------------------------------------------------------------*/
        apn_table_p->v4_pdn_ptr = NULL;
      }
      else if((apn_table_p->ip_support == DS_SINGLE_BEARER) ||
              (apn_table_p->ip_support == DS_IP_TYPE_INVALID)||
              (apn_table_p->ip_support == DS_IPV6_ONLY) || 
              (apn_table_p->ip_support == DS_NON_IP))
      {
        /*----------------------------------------------------------------------
              Here we just want to add another PDN pointer to the APN table entry
              ----------------------------------------------------------------------*/
        apn_table_p->v6_pdn_ptr = pdn_ptr;
      }
      else if((apn_table_p->ip_support == DS_IPV4_ONLY) &&
              (ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == TRUE))
      {
        /*----------------------------------------------------------------------
              For multi PDN same APN case, we need to add PDN pointer to the same APN table entry
              ----------------------------------------------------------------------*/
        apn_table_p->v6_pdn_ptr = pdn_ptr;
      }
      break;
      
    case DS_IPV4V6:
      apn_table_p->v4_pdn_ptr = pdn_ptr;
      apn_table_p->v6_pdn_ptr = pdn_ptr;
      break;

    case DS_NON_IP:
    case DS_NON_IP_SINGLE_BEARER:
      apn_table_p->non_ip_pdn_ptr = pdn_ptr;
      break;
      
    default:
      DS_ERROR_FATAL("Unknown PDP type.");
  }
} /* ds_apn_table_update_pdn_pointer */

/*===========================================================================
FUNCTION DS_APN_TABLE_FILL_IP_SUPPORT

DESCRIPTION
  This function fills the ip support info at the given apn table pointer

PARAMETERS   :
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ds_apn_table_fill_ip_support
(
  ds_pdn_mgr_apn_table_s      *apn_table_p,
  ds_apn_ip_support_type_e     ip_support
)
{  
  DS_ASSERT(ip_support != DS_IP_TYPE_INVALID);
  switch(ip_support)
  {
    case DS_IPV4_SINGLE_BEARER:
      if(ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == TRUE &&
         DS_IPV6_ONLY == apn_table_p->ip_support)
      {
        apn_table_p->ip_support = DS_IPV4V6;
      }
      else
      {
        apn_table_p->ip_support = DS_SINGLE_BEARER;  
      }
      break;
      
    case DS_IPV6_SINGLE_BEARER:
      if(ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == TRUE &&
         DS_IPV4_ONLY == apn_table_p->ip_support)
      {
        apn_table_p->ip_support = DS_IPV4V6;
      }
      else
      {
        apn_table_p->ip_support = DS_SINGLE_BEARER;  
      }
      break;
      
    case DS_NON_IP_SINGLE_BEARER:
    case DS_SINGLE_BEARER:
      apn_table_p->ip_support = DS_SINGLE_BEARER;
      break;
      
    case DS_IPV4V6:
      // Non-ip DPN already up, Dual IP PDN being brought up 
      if (DS_NON_IP == apn_table_p->ip_support)
      {
        apn_table_p->ip_support = DS_IPV4V6_NON_IP;
      }
      else if (DS_IPV4V6_NON_IP != apn_table_p->ip_support)
      {
        apn_table_p->ip_support = ip_support;
      }
      break;
  
    case DS_NON_IP:
      // Dual -IP PDN already up, non-ip is being brought up
      if (DS_IPV4V6 == apn_table_p->ip_support)
      {
        apn_table_p->ip_support = DS_IPV4V6_NON_IP;
      }    
      // Single IP PDN already up, non-ip is being brought up
      else if ( DS_SINGLE_BEARER == apn_table_p->ip_support )
      {
        apn_table_p->ip_support = DS_SINGLE_BEARER;
      }
      // V4only PDN already up, non_ip is being brought up
      else if (DS_IPV4_ONLY == apn_table_p->ip_support ||
               DS_IPV4_NON_IP == apn_table_p->ip_support)
      {
        apn_table_p->ip_support = DS_IPV4_NON_IP;
      }
      // V6only PDN already up, non_ip is being brought up
      else if (DS_IPV6_ONLY == apn_table_p->ip_support ||
               DS_IPV6_NON_IP == apn_table_p->ip_support)
      {
        apn_table_p->ip_support = DS_IPV6_NON_IP;
      }
      else if (DS_IPV4V6_NON_IP != apn_table_p->ip_support)
      {
        apn_table_p->ip_support = ip_support;
      }
      break;
  
    case DS_IPV4_ONLY:
      /*-----------------------------------------------------------------------
             Example Scenario 
                     1. Non-IP PDN already up.
                     2. V4V6 PDN bring up started ( IP suuport changed to IPV4V6_NON_IP )
                     3. Network came back with V4 only or V6 only 
                     4. So the IP type should be V4_NON_IP or V6_NON_IP
          -----------------------------------------------------------------------*/
      if ( DS_IPV4V6_NON_IP == apn_table_p->ip_support )
      {
        apn_table_p->ip_support = DS_IPV4_NON_IP;
      }
      else
      {
        apn_table_p->ip_support = ip_support;
      }
      break;
    
    case DS_IPV6_ONLY:
      if ( DS_IPV4V6_NON_IP == apn_table_p->ip_support )
      {
        apn_table_p->ip_support = DS_IPV6_NON_IP;
      }
      else
      {
        apn_table_p->ip_support = ip_support;
      }
      break;

    case DS_IPV4V6_NON_IP:
      break;
      
    default:
      apn_table_p->ip_support = ip_support;
      break;
  }
   DS_MSG1(MSG_LEGACY_HIGH,
         "IP support filled in APN table: %d ",apn_table_p->ip_support);
  
}/* ds_apn_table_fill_ip_support */

/*===========================================================================
FUNCTION ds_apn_table_fill_info

DESCRIPTION
  This function fills the apn table with the provided info at the provided 
  index

PARAMETERS   :
    
DEPENDENCIES

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_fill_info
(
  int                       index,
  char                     *apn,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_mgr_s             *pdn_ptr
)
{
  ds_pdn_mgr_apn_table_s   *apn_table_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_apn_table_validate_index(index) == FALSE)
  {
    DS_ERROR_LOG_0("Invalid index");
    return FALSE;
  }
  DS_ASSERT(ip_support != DS_IP_TYPE_INVALID);
  apn_table_p = ds_pdn_apn_table[index];

  /*---------------------------------------------------------------------- 
    For fresh entry check if both apn and resolved apn are NULL strings
    If they are null strings the assumption is it is a fresh addition.
    Even if there exists a call with both apn and resolved as NULL, we
    overwrite NULL to apn.
   ---------------------------------------------------------------------*/
  if ((apn_table_p->apn[0] == '\0') && 
      (apn_table_p->resolved_apn[0] == '\0'))
  {
    memscpy(apn_table_p->apn, (DS_PROFILE_APN_STRING_LEN+1),
            apn,(DS_PROFILE_APN_STRING_LEN+1));
  }

  ds_apn_table_update_pdn_pointer(index, ip_support, pdn_ptr);
  ds_apn_table_fill_ip_support(apn_table_p,ip_support);
  return TRUE;
}

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_APN_TABLE

DESCRIPTION
  This function updates the APN table with the supplied info

PARAMETERS   :
    
DEPENDENCIES
  Remember to call update_pdn_ptr fn before you update the IP support of the 
  entry in the APN table. Should the order be reversed, APN table behaviour
  will be unpredictable.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_update_apn_table
(
  char                     *apn,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_mgr_s             *pdn_ptr
)
{
  int       update_index = -1;
  uint8     loop = 0;
  boolean   ret_val = FALSE;

  DS_ASSERT(ip_support != DS_IP_TYPE_INVALID);
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_ptr))
  {
    return FALSE;
  }

  for(loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if(ds_apn_table_get_pdn_match_apn_index(loop, pdn_ptr))
    {
      update_index = loop;
    }
  }

  if (update_index >= 0 ) 
  {
    ret_val = ds_apn_table_fill_info(update_index, apn, ip_support, pdn_ptr);
  }
  else
  {
    DS_ERROR_LOG_0("Update Fail. No matching PDN in APN table");
  }

  return ret_val;
} /* ds_apn_table_update_apn_table */

/*===========================================================================
FUNCTION DS_APN_TABLE_REMOVE_ENTRY

DESCRIPTION
  This function checks if the corresponding entry in the APN table for a
  specific APN can be removed or not

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If entry is to be removed
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_remove_entry
(
  ds_pdn_mgr_apn_table_s  *apn_table_p,
  ds_pdn_mgr_s            *incoming_pdn_mgr_p
)
{
  boolean                clear_table_entry = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( NULL == apn_table_p )
  {
    return FALSE;
  }

  switch(apn_table_p->ip_support)
  {
    case DS_IPV4V6:
      if((apn_table_p->v4_pdn_ptr == incoming_pdn_mgr_p) &&
         (apn_table_p->v6_pdn_ptr == incoming_pdn_mgr_p))
      {
        clear_table_entry = TRUE;
      }
      else if(ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == TRUE)
      {
        if((apn_table_p->v4_pdn_ptr == incoming_pdn_mgr_p) &&
           (apn_table_p->v6_pdn_ptr != incoming_pdn_mgr_p))
        {
          apn_table_p->v4_pdn_ptr = NULL;
          apn_table_p->ip_support = DS_IPV6_ONLY;
        }
        else if((apn_table_p->v6_pdn_ptr == incoming_pdn_mgr_p) &&
                (apn_table_p->v4_pdn_ptr != incoming_pdn_mgr_p))
        {
          apn_table_p->v6_pdn_ptr = NULL;
          apn_table_p->ip_support = DS_IPV4_ONLY;
        }
      }
      break;

    case DS_IPV4_ONLY:
    case DS_IPV6_ONLY:
    case DS_SINGLE_BEARER:
      /*--------------------------------------------------------------------
        Determine if the APN table has three PDN context pointers associated
        with it. If so, just clean up the incoming PDN context pointer

        If there is only one PDN context pointer present in the APN table
        entry, mark this entry for clearing.
   
        In some cases like (IPV6 RS/RA timeout),  the IP support is moved from 
        DS_SINGLE_BEARER to DS_IPV4_ONLY (or) DS_IPV6_ONLY. Hence, it may
        be possible that V4 and V6 PDN contexts may exist and are different
      --------------------------------------------------------------------*/
      if ((apn_table_p->v4_pdn_ptr == incoming_pdn_mgr_p) &&
          ((apn_table_p->v6_pdn_ptr != NULL) ||
           (apn_table_p->non_ip_pdn_ptr != NULL)))
      {
        apn_table_p->v4_pdn_ptr = NULL;

        /*--------------------------------------------------------------------
          This is the case where v4 and non-ip pdn where present 
          ( ip_support = DS_SINGLE_BEARER). Now once the v4 PDN is cleared,
          the ip_support should change to DS_NON_IP
        --------------------------------------------------------------------*/
        if(apn_table_p->non_ip_pdn_ptr != NULL &&
           apn_table_p->v6_pdn_ptr == NULL )
        {
          // change the ip support from DS_SINGLE_BEARER -> DS_NON_IP
          apn_table_p->ip_support = DS_NON_IP;
        }
      }
      else if ((apn_table_p->v6_pdn_ptr == incoming_pdn_mgr_p) &&
               ((apn_table_p->v4_pdn_ptr != NULL) ||
                (apn_table_p->non_ip_pdn_ptr != NULL)))
      {
        apn_table_p->v6_pdn_ptr = NULL;
        /*--------------------------------------------------------------------
          This is the case where v6 and non-ip pdn where present 
          ( ip_support = DS_SINGLE_BEARER). Now once the v6 PDN is cleared,
          the ip_support should change to DS_NON_IP
        --------------------------------------------------------------------*/
        if(apn_table_p->non_ip_pdn_ptr != NULL &&
           apn_table_p->v4_pdn_ptr == NULL )
        {
          // change the ip support from DS_SINGLE_BEARER -> DS_NON_IP
          apn_table_p->ip_support = DS_NON_IP;
        }
      }
      else if ((apn_table_p->non_ip_pdn_ptr == incoming_pdn_mgr_p) &&
               ((apn_table_p->v4_pdn_ptr != NULL) ||
                (apn_table_p->v6_pdn_ptr != NULL)))
      {
        apn_table_p->non_ip_pdn_ptr = NULL;
      }
      else
      {
        clear_table_entry = TRUE;
      }
      break;

    case DS_NON_IP:
      if (apn_table_p->non_ip_pdn_ptr == incoming_pdn_mgr_p)
      {
        clear_table_entry = TRUE;
      }
      break;

    case DS_IPV4V6_NON_IP:
      if((apn_table_p->v4_pdn_ptr == incoming_pdn_mgr_p) &&
         (apn_table_p->v6_pdn_ptr == incoming_pdn_mgr_p))
      {
        if (apn_table_p->non_ip_pdn_ptr != NULL)
        {
          apn_table_p->v4_pdn_ptr = NULL;
          apn_table_p->v6_pdn_ptr = NULL;
          
          // change the ip support from DS_IPV4V6_NON_IP -> DS_NON_IP
          apn_table_p->ip_support = DS_NON_IP;
        }
        else
        {
          // something is wrong.
          clear_table_entry = TRUE;
        }
      }
      else if (apn_table_p->non_ip_pdn_ptr == incoming_pdn_mgr_p)
      {
        if ((apn_table_p->v4_pdn_ptr != NULL) &&
            (apn_table_p->v6_pdn_ptr != NULL))
          {
            apn_table_p->non_ip_pdn_ptr = NULL;
            // change the ip support from DS_IPV4V6_NON_IP -> DS_IPV4V6
            apn_table_p->ip_support = DS_IPV4V6;
          }
          else
          {
            // something is wrong.
            clear_table_entry = TRUE;
          }
      }
      break;

    case DS_IPV4_NON_IP:
      if((apn_table_p->v4_pdn_ptr == incoming_pdn_mgr_p) &&
         (apn_table_p->non_ip_pdn_ptr != NULL))
      {
        apn_table_p->v4_pdn_ptr = NULL;
        // change the ip support from DS_IPV4_NON_IP -> DS_NON_IP
        apn_table_p->ip_support = DS_NON_IP;
      }
      else if ((apn_table_p->non_ip_pdn_ptr == incoming_pdn_mgr_p) &&
               (apn_table_p->v4_pdn_ptr != NULL))
      {
        apn_table_p->non_ip_pdn_ptr = NULL;
        // change the ip support from DS_IPV4_NON_IP -> DS_IPV4_ONLY
        apn_table_p->ip_support = DS_IPV4_ONLY;
      }
      else
      {
        clear_table_entry = TRUE;
      }
      break;

    case DS_IPV6_NON_IP:
      if((apn_table_p->v6_pdn_ptr == incoming_pdn_mgr_p) &&
         (apn_table_p->non_ip_pdn_ptr != NULL))
      {
        apn_table_p->v6_pdn_ptr = NULL;
        // change the ip support from DS_IPV4_NON_IP -> DS_NON_IP
        apn_table_p->ip_support = DS_NON_IP;
      }
      else if ((apn_table_p->non_ip_pdn_ptr == incoming_pdn_mgr_p) &&
               (apn_table_p->v6_pdn_ptr != NULL))
      {
        apn_table_p->non_ip_pdn_ptr = NULL;
        // change the ip support from DS_IPV4_NON_IP -> DS_IPV6_ONLY
        apn_table_p->ip_support = DS_IPV6_ONLY;
      }
      else
      {
        clear_table_entry = TRUE;
      }
      break;

    default:
      DS_ERROR_LOG_1("APN table cannot be in this state: =d", 
                     apn_table_p->ip_support);
      break;
  } // switch based on the IP support

  return clear_table_entry;
} /* ds_apn_table_remove_entry */

/*===========================================================================
FUNCTION DS_APN_TABLE_REMOVE_ENTRY_IN_APN_TABLE

DESCRIPTION
  This function removes the corresponding entry in the APN table for a
  specific APN

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully removed this entry
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_remove_entry_in_apn_table
(
  ds_pdn_mgr_s   *incoming_pdn_mgr_p
)
{
  uint8                   loop = 0;
  boolean                 ret_val = FALSE;
  boolean                 clear_table_entry = FALSE;
  ds_pdn_mgr_apn_table_s *apn_table_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(incoming_pdn_mgr_p))
  {
    return FALSE;
  }
  
  for( loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if ( (ds_apn_table_validate_index(loop)) &&
         ( (ds_pdn_apn_table[loop]->v4_pdn_ptr == incoming_pdn_mgr_p) ||
           (ds_pdn_apn_table[loop]->v6_pdn_ptr == incoming_pdn_mgr_p) ||
           (ds_pdn_apn_table[loop]->non_ip_pdn_ptr == incoming_pdn_mgr_p) ) )
    {
      apn_table_p = ds_pdn_apn_table[loop];
      clear_table_entry = ds_apn_table_remove_entry
                                      (apn_table_p,incoming_pdn_mgr_p);
      
      if(clear_table_entry == TRUE)
      {
        ds_pdn_apn_table_free_dyn_mem(loop);
      }

      ret_val = TRUE;
      break;
    }
  }

  if(ret_val == FALSE)
  {
    DS_ERROR_LOG_0("APN entry not found in apn table!");
  }

  return ret_val;
} /* ds_apn_table_remove_entry_in_apn_table */

/*===========================================================================
FUNCTION DS_APN_TABLE_IS_PDN_ASSOCIATED_FOR_APN

DESCRIPTION
  This function searchs the APN table and returns whether pdn context exists
  for a given apn

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If there is a pdn context associated with this apn
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_is_pdn_associated_for_apn
(
  char                      *apn
)
{
  ds_pdn_mgr_apn_table_s *apn_table_p = NULL;

  if(ds_apn_table_get_apn_table_ptr(apn, &apn_table_p) != -1)
  {
    if (apn_table_p->v4_pdn_ptr != NULL ||
        apn_table_p->v6_pdn_ptr != NULL ||
        apn_table_p->non_ip_pdn_ptr != NULL)
    {
      return TRUE;
    }    
  }

  return FALSE;
}

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_IP_SUPPORT_INFO_FOR_PDN

DESCRIPTION
  This function searchs the APN table and returns the ip support type of a 
  particular PDN. Note that DS_IPV4V6_SINGLE_BEARER is not a valid support
  type for a particular PDN since there would be 2 PDN connectivity to the 
  same APN and this particular PDN can only be one of them.

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this pdn
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_get_ip_support_info_for_pdn
(
  ds_pdn_mgr_s              *pdn_mgr_ptr,
  ds_apn_ip_support_type_e  *ip_support
)
{
  uint8                    loop = 0;
  ds_pdn_mgr_apn_table_s  *apn_table_p = NULL;

  *ip_support = DS_IP_TYPE_INVALID;
  
  for( loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if(ds_apn_table_get_pdn_match_apn_index(loop, pdn_mgr_ptr))
    {
      apn_table_p = ds_pdn_apn_table[loop];

      switch(apn_table_p->ip_support)
      {
        /* in the single bearer v4v6 case, we need to find out this pdn is v4 ,v6, or non-ip */      
        case DS_SINGLE_BEARER:
          if (apn_table_p->v4_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_IPV4_SINGLE_BEARER;
          }
          else if (apn_table_p->v6_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_IPV6_SINGLE_BEARER;
          }
          else if (apn_table_p->non_ip_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_NON_IP_SINGLE_BEARER;
          } 
          break;

        /* In case of v4_non_ip , v6_non_ip we need to find out if pdn is non-ip or v4 /v6 */
        case DS_IPV4_NON_IP:           
          if (apn_table_p->v4_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_IPV4_ONLY;
          }
          else if (apn_table_p->non_ip_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_NON_IP;
          }
          break;

        case DS_IPV6_NON_IP:           
          if (apn_table_p->v6_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_IPV6_ONLY;
          }
          else if (apn_table_p->non_ip_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_NON_IP;
          }
          break;

        /* In case of v4v6_non_ip , we need to find out if this pdn is non-ip or v4v6 */
        case DS_IPV4V6_NON_IP:
          if (apn_table_p->v4_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_IPV4V6;
          }
          else if (apn_table_p->non_ip_pdn_ptr == pdn_mgr_ptr)
          {
            *ip_support = DS_NON_IP;
          }
          break;

        default:
          *ip_support = apn_table_p->ip_support;
          DS_ASSERT(apn_table_p->ip_support != DS_IP_TYPE_INVALID);
          break;
       }

      if(DS_IP_TYPE_INVALID != *ip_support)
      {
        return TRUE;
      }

      break;
    }
  }
  DS_ASSERT(0);  
  return FALSE;
}

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_PDN_PDP_TYPE

DESCRIPTION
  This function gets the PDP type of the given pdn context pointer

PARAMETERS   
    
DEPENDENCIES
  None.

RETURN VALUE
  IP support type of the pdn context

SIDE EFFECTS
  None

===========================================================================*/
ds_profile_pdp_type_e ds_apn_table_get_pdn_pdp_type
(
  ds_pdn_mgr_s*       pdn_mgr_p
)
{
  ds_apn_ip_support_type_e   ip_support;
  ds_profile_pdp_type_e      pdp_support_type = DS_PROFILE_PDP_MAX;

  if (ds_apn_table_get_ip_support_info_for_pdn(pdn_mgr_p, &ip_support) != TRUE)
  {
    /*-------------------------------------------------------------------
      If there is no entry for this apn in the apn table, there is an
      error in apn table
    -------------------------------------------------------------------*/
    DS_ERROR_LOG_1("APN in pdn context: =x is not present in APN table.",
                   pdn_mgr_p);
    DS_ASSERT(0);
  }

  switch(ip_support)
  {
    case DS_IPV4_ONLY: //lint fallthrough
    case DS_IPV4_SINGLE_BEARER:
      pdp_support_type =  DS_PROFILE_PDP_IPV4;
      break;
      
    case DS_IPV6_ONLY: //lint fallthrough
    case DS_IPV6_SINGLE_BEARER:
      pdp_support_type = DS_PROFILE_PDP_IPV6;
      break;
      
    case DS_IPV4V6:
      pdp_support_type =  DS_PROFILE_PDP_IPV4V6;
      break;
      
    case DS_NON_IP:
    case DS_NON_IP_SINGLE_BEARER:
      pdp_support_type = DS_PROFILE_PDP_NON_IP;
      break;
      
    default:
      DS_ERROR_FATAL("Unknown PDP type.");
  }
  
  return pdp_support_type;
}

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_ALLOWED_IP_TYPE

DESCRIPTION
  This function blocks a specific IP type.
  1. If the IP_type becomes unavailable due to address configuration failure.
     In this case IP support will change from V4V6 to V4_ONLY or V6_ONLY.
  2. If the profile is configured single IP but OTA request is V4V6
     In this case IP support will change from SINGLE_BEARER to V4_ONLY or
     V6_ONLY.
  This function should not be called to change the the ip_support from one
  IP type to another V6 ex of unsupported transition (V4_Single_bearer to
  V6_Single_bearer or V6_ONLY)
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_apn_table_update_allowed_ip_type
(
  ds_pdn_mgr_s             *pdn_mgr_ptr,
  ds_apn_ip_support_type_e  allowed_ip_type
)
{
  char                     *apn = NULL;
  ds_pdn_mgr_s             *v4_pdn_mgr_p = NULL;
  ds_pdn_mgr_s             *v6_pdn_mgr_p = NULL;
  ds_pdn_mgr_s             *non_ip_pdn_mgr_p = NULL;
  ds_apn_ip_support_type_e  ip_support;
  boolean                   update_allowed = FALSE;
  ds_pdn_mgr_s             *update_pdn_mgr_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(allowed_ip_type != DS_IPV4_ONLY && allowed_ip_type != DS_IPV6_ONLY)
  {
    DS_ERROR_LOG_1("No ip support update for =d which is not V4/V6 only",
                       allowed_ip_type);
    return;
  }

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_ptr))
  {
    /*------------------------------------------------------------------------
      Get the APN name from the PDN context whose connection was rejected
    ------------------------------------------------------------------------*/
    apn = pdn_mgr_ptr->pdp_profile->context.apn;

    if(ds_apn_table_get_ip_support_info_for_pdn(pdn_mgr_ptr, &ip_support)
                                                == TRUE)
    {
      /*----------------------------------------------------------------------
        Get the V6 and the V4 PDN context pointers.
      ----------------------------------------------------------------------*/
      if(ds_apn_table_get_v4_and_v6_pdn
                   (pdn_mgr_ptr,&v4_pdn_mgr_p,&v6_pdn_mgr_p) != TRUE)
      {
        DS_ERROR_LOG_0("APN tbl entry not found. Trying to get V4/V6 PDN ptr");
      }

      /*----------------------------------------------------------------------
        Get the Non-IP PDN context pointer.
      ----------------------------------------------------------------------*/
      if(ds_apn_table_get_non_ip_pdn(apn, &non_ip_pdn_mgr_p) != TRUE)
      {
        DS_ERROR_LOG_0("APN tbl entry not found. Trying to get nonIP PDN ptr");
      }

      // Update APN IP type with the IP type being passed 
      switch (ip_support)
      {
        case DS_IPV4V6:
          /*-------------- ---------------------------------------------------
            Update allowed ip type - v4 and v6 pdn ptrs should be the same
          -------------------------------------------------------------------*/
          update_allowed = ((v4_pdn_mgr_p != NULL) &&
                            (v4_pdn_mgr_p == v6_pdn_mgr_p));
          update_pdn_mgr_p = v4_pdn_mgr_p;
          break;

        case DS_IPV4_SINGLE_BEARER:
          /*------------------------------------------------------------------
            IPV4_ONLY should be updated only when both V6 PDN and Non-IP PDN 
            does not exist, if V6 PDN ptr or non-ip PDN ptr exists then there should 
            be no operation and IP type should remain as IP_V4_SINGLE_BEARER
          -------------------------------------------------------------------*/
          update_allowed = ((v4_pdn_mgr_p != NULL) &&
                            (allowed_ip_type == DS_IPV4_ONLY) &&
                            (v6_pdn_mgr_p == NULL) &&
                            (non_ip_pdn_mgr_p == NULL));
          update_pdn_mgr_p = v4_pdn_mgr_p;
          break;

        case DS_IPV6_SINGLE_BEARER:
           /*------------------------------------------------------------------
             update per allowed ip type, v6 pdn ptr should not be NULL
           -------------------------------------------------------------------*/
          update_allowed = ((v6_pdn_mgr_p != NULL) &&
                            (allowed_ip_type == DS_IPV6_ONLY)&&
                            (v4_pdn_mgr_p == NULL) &&
                            (non_ip_pdn_mgr_p == NULL));
          update_pdn_mgr_p = v6_pdn_mgr_p;
          break;

        case DS_NON_IP_SINGLE_BEARER:
        case DS_IPV4_ONLY:
        case DS_IPV6_ONLY:
        default:
         //ignore
          break;
      }

      if(update_allowed == TRUE)
      {
        (void)ds_apn_table_update_apn_table(apn,
                                            allowed_ip_type,
                                            update_pdn_mgr_p);

        DS_MSG1(MSG_LEGACY_HIGH,"IP_Support update to %d",allowed_ip_type);
        return;
      }
      else
      {
        DS_MSG2(MSG_LEGACY_HIGH,"No change: IP_Support from %d to %d",
                            ip_support,
                            allowed_ip_type);
        DS_MSG2(MSG_LEGACY_HIGH,"With PDN ptrs 0x%x 0x%x",
                            v4_pdn_mgr_p,
                            v6_pdn_mgr_p);
      }      
    } // Found the APN entry in the table
  } // incoming param non NULL
}

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_RESOLVED_APN_NAME

DESCRIPTION
  This function updates an entry in the APN table with the resolved APN name.
  This is used only if the APN name in the profile was NULL

PARAMETERS   :
  pdn_mgr_ptr* - Pointer to PDN context
  byte*              - APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the entry corresponding to the profile APN name was found
  FALSE - if otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_update_resolved_apn_name 
(
  ds_pdn_mgr_s *pdn_mgr_p,
  char         *resolved_apn_name
)
{
  uint8         loop = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return FALSE;
  }

  for(loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if(ds_apn_table_get_pdn_match_apn_index(loop,pdn_mgr_p))
                                                                          
    {
      DS_MSG0(MSG_LEGACY_HIGH,"APN tbl entry found. Updating resolved name");
      (void)strlcpy((char*)ds_pdn_apn_table[loop]->resolved_apn,
                    (char*)resolved_apn_name,
                    DS_PROFILE_APN_STRING_LEN+1);
      return TRUE;
    }
  }

  return FALSE;
} /* ds_pdn_cntxt_update_resolved_apn_name */

/*==============================================================================
FUNCTION DS_APN_TABLE_GET_RESOLVED_APN_NAME

DESCRIPTION
  This function is used to return the resolved name given 
  the pdn context

PARAMETERS   :

  IN
  pdn_mgr_ptr* - Pointer to PDN context

  OUT
  byte** - Resolved APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the entry corresponding to the profile APN name was found
  FALSE - if otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds_apn_table_get_resolved_apn_name
(
  ds_pdn_mgr_s  *pdn_mgr_p, 
  char         **res_apn_name
)
{ 
  uint8                  loop = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return FALSE;
  }

  for(loop = 0; loop < DS_MAX_APN_NUM; loop++)
  {
    if(ds_apn_table_get_pdn_match_apn_index(loop,pdn_mgr_p))                                                                          
    {
      if((char)ds_pdn_apn_table[loop]->resolved_apn[0] != '\0')
      {
        *res_apn_name = ds_pdn_apn_table[loop]->resolved_apn;
        return TRUE;
      }
      
      break;
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION ds_apn_table_add_pdn

DESCRIPTION
  This function adds an entry to the APN table with the supplied info

PARAMETERS
  apn        - name of the APN
  ip_support - IP support for the APN 
  pdn_ptr    - Pointer to the PDN context


    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_add_pdn
(
  char                      *apn,
  ds_apn_ip_support_type_e   incoming_ip_support,
  ds_pdn_mgr_s              *pdn_ptr
)
{
  boolean                 ret_val = FALSE;
  boolean                 apn_already_present = FALSE;
  uint8                   add_index=DS_MAX_APN_NUM;
  int                     update_index = -1;
  boolean                 allow_update = FALSE;
  ds_pdn_mgr_apn_table_s *apn_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT(incoming_ip_support != DS_IP_TYPE_INVALID);
  update_index = ds_apn_table_get_apn_table_ptr(apn,&apn_ptr);
  if(-1 != update_index)
  {
    apn_already_present = TRUE;
    
    switch(incoming_ip_support)
    {
      case DS_IPV4_ONLY:
      case DS_IPV4_SINGLE_BEARER:
        if(apn_ptr->v4_pdn_ptr == NULL)
        {
          allow_update = TRUE;
        }
        break;
    
      case DS_IPV6_ONLY:
      case DS_IPV6_SINGLE_BEARER:
        if(apn_ptr->v6_pdn_ptr == NULL)
        {
          allow_update = TRUE;
        }
        break;
    
      case DS_IPV4V6:
        if(apn_ptr->v4_pdn_ptr == NULL && apn_ptr->v6_pdn_ptr == NULL)
        {
          allow_update = TRUE;
        }
        break;
    
      case DS_NON_IP:
        if(apn_ptr->non_ip_pdn_ptr == NULL)
        {
          allow_update = TRUE;
        }
        break;
    
      default:
        break; 
    }/* switch */
  }

  /*-------------------------------------------------------------------
    Decide whether we are going to update or add a new entry
   -------------------------------------------------------------------*/
  if (allow_update == TRUE) 
  {
    //call fill API with update_index
    ret_val = ds_apn_table_fill_info(update_index, apn, 
                                     incoming_ip_support, pdn_ptr);
    return ret_val;
  }

  if( (apn_already_present == TRUE) &&
      (ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == FALSE))
  {
    DS_ERROR_LOG_0("Skipping dup entry addition in APN table");   
    return FALSE;
  }

  //Add fresh in first available index if it is not already present
  ret_val = ds_apn_table_alloc_dyn_mem(&add_index);

  /*----------------------------------------------------------------------- 
      APN not present in the APN table. Allocate memory to record the
      entry in the APN table
    -----------------------------------------------------------------------*/
  if( ret_val == TRUE )    
  {
    //call fill with add_index
    ret_val = ds_apn_table_fill_info(add_index, apn, incoming_ip_support, pdn_ptr);
  }

  return ret_val;
} /* ds_apn_table_add_entry */

/*===========================================================================
FUNCTION DS_APN_TABLE_RECOMPUTE_IP_SUPPORT_FOR_APN

DESCRIPTION
  This function recomputes the IP support for a given APN. Recomputation is
  done based on two aspects:
  1. a prior knowledge about the IP support of an APN. This is usually 
     assumed based on the PDP type
  2. New data from the network - The new data that this function uses is
     from the PDN reject indication. If the call is accepted, there is no
     need to recompute as the assumed IP support is correct.

  This function gets the recomputed IP support and removes the PDN context
  pointer (that was rejected) from the APN table. It also updates the APN
  table so that the correct info is present in the table.

PARAMETERS
  IN
    rejected_pdn_cntx_p - PDN context that has to be removed from the table
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  When bearer_down_indication is processed, we will call remove_entry_from_apn
  with the same PDN context once again. However, that function call will result
  in no operation since the PDN context pointer was already removed.

===========================================================================*/
void ds_apn_table_recompute_ip_support_for_apn
(
  ds_pdn_mgr_s  *rejected_pdn_cntx_p
)
{
  byte                     *apn = NULL;
  ds_pdn_mgr_s             *v4_pdn_cntx_p = NULL;
  ds_pdn_mgr_s             *v6_pdn_cntx_p = NULL;
  ds_apn_ip_support_type_e  ip_support;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(rejected_pdn_cntx_p))
  {
    return;
  }
  
  /*----------------------------------------------------------------------------
    This function is used to recompute the IP support for a given APN

    This function is of use only is the present IP support is Single Bearer
    and there are two PDN context pointers in the APN table entry.

    If the support is Single Bearer and there is only one PDN context, this
    function does nothing. The APN table entry in such a case will be cleaned
    up during processing of Bearer Down indication
  ----------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Get the APN name from the PDN context whose connection was rejected
  ------------------------------------------------------------------------*/
  apn = (byte *)rejected_pdn_cntx_p->pdp_profile->context.apn;

  if(ds_apn_table_get_ip_support_info_for_pdn(rejected_pdn_cntx_p, 
                                              &ip_support) == TRUE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "PDN reject recvd. Updating APN table curr support: %d",
             ip_support);

    /*----------------------------------------------------------------------
      Get the V6 and the V4 PDN context pointers.
    ----------------------------------------------------------------------*/
    if(ds_apn_table_get_v4_and_v6_pdn(rejected_pdn_cntx_p, 
                                      &v4_pdn_cntx_p,
                                      &v6_pdn_cntx_p) != TRUE)
    {
      DS_ERROR_LOG_0("APN tbl entry not found.");
    }

    if( (v4_pdn_cntx_p != NULL) &&
        (v6_pdn_cntx_p != NULL) &&
        (v4_pdn_cntx_p != v6_pdn_cntx_p) )
    {
      /*---------------------------------------------------------------------
        We have established that we have 2 different PDN contexts to the same
        APN. We will not remove the PDN context pointer from the APN table
        now since we do it while processing bearer_down_ind.

        All we do here is to set the APN IP support in the APN table.
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Determine the IP support and update the APN table
      ---------------------------------------------------------------------*/
      if(rejected_pdn_cntx_p == v4_pdn_cntx_p)
      {
        (void)ds_apn_table_update_apn_table((char *)apn, DS_IPV6_ONLY, v6_pdn_cntx_p);
      }
      else if (rejected_pdn_cntx_p == v6_pdn_cntx_p)
      {
        (void)ds_apn_table_update_apn_table((char *)apn, DS_IPV4_ONLY, v4_pdn_cntx_p);
      }
      else
      {
        DS_ERROR_LOG_0("Rejected PDN cntxt does not match any in APN tbl");
      }
    } //Switch based on existing IP Support
  } // Found the APN entry in the table
}/* ds_pdn_cntxt_recompute_ip_support_for_apn */

