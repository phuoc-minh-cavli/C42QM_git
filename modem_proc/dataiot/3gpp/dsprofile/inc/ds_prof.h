#ifndef DS_PROF_H
#define DS_PROF_H
/*===========================================================================
                      DS_PROF.H
DESCRIPTION
This file exposes data structure and API related to internal profile operation.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    pk      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "customer.h"
#include "modem_mem.h"
#include "stringl.h"
#include "ds_profile.h"

#define  DS_PROFILE_MAX_PROFILE_NUM   16
#define  DS_PROFILE_MAX_TRANSIENT_PROFILE_NUM 8 
#define INVALID_PROFILE_NUMBER -1


/** Default Values for PDN Connection Limit Handling */  // if no client is using move it to internal

#define DS_PROFILE_DEF_MAX_PDN_CONN_PER_BLK         20
#define DS_PROFILE_DEF_MAX_PDN_CONN_TIME           300 //300 seconds 
#define DS_PROFILE_DEF_PDN_REQ_WAIT_TIME             0

/** Upper Limit Range for PDN Connection Limit Handling */
#define DS_PROFILE_MAX_PDN_CONN_PER_BLK_UL     1023
#define DS_PROFILE_DEF_MAX_PDN_CONN_TIME_UL    3600 //3600 seconds 
#define DS_PROFILE_DEF_PDN_REQ_WAIT_TIME_UL    1023 //1023 seconds


#define  DS_PROFILE_APN_BEARER_FLAG_G   (0x1)  /**< Bearer G. */  
#define  DS_PROFILE_APN_BEARER_FLAG_L   (0x4)  /**< Bearer L. */  
#define  DS_PROFILE_APN_BEARER_FLAG_ALL (0xff) /**< Bearer Any. */  

#define DS_PROFILE_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN 0xFF00
#define DS_PROFILE_OPERATOR_RESERVED_PCO_ID_RANGE_END 0xFFFF

#define DS_PROFILE_MAX_PDN_CONN_PER_BLOCK_VAL  1023
#define DS_PROFILE_MAX_PDN_CONN_REQ_WAIT_TIME 1023
#define DS_PROFILE_MAX_PDN_CONN_TIME 3600

#define DS_PROFILE_MAX_INACTIVITY_TIMEOUT 122820

/*===========================================================================

  INTERNAL DEFINITIONS //can moved to .c file

  ===========================================================================*/
/* ------------------------------------------------------------------
   Boolean variable to know if a command needs to be posted to MH for
   APN param change processing
   ---------------------------------------------------------------------*/
static boolean ds_profile_apn_param_changed = FALSE;

/** TLV identifiers for profile parameters
 */
typedef enum
{
  DS_PROFILE_APN_NAME_TLV                           = 0,
  /** APN name */
  DS_PROFILE_TYPE_TLV                           = 1,
  /** Home PDP type */
  DS_PROFILE_CONTEXT_H_COMP_TLV                 = 2,
  /** PDP header compression support */
  DS_PROFILE_CONTEXT_D_COMP_TLV                 = 3,
  /** PDP data compression support */
  DS_PROFILE_DNS_ADDR_V4_PRIMARY_TLV                = 4,
  /** Primary DNS IPV6 address */
  DS_PROFILE_DNS_ADDR_V4_SECONDARY_TLV              = 5,
  /** Secondary DNS IPV4 address */
  DS_PROFILE_AUTH_USERNAME_TLV                      = 6,
  /** User name */
  DS_PROFILE_AUTH_PASSWORD_TLV                      = 7, 
  /** password */
  DS_PROFILE_AUTH_TYPE_TLV                          = 8,
  /** Aunthentication Preferencre */
  DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV                = 9,  
  /** Priimary DNS IPV6 address */
  DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV              = 10,
  /** Secondary DNS IPV6 address */
  DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG_TLV      = 11,
  /* PCFCH address using dhcp */
  DS_PROFILE_ACCESS_CTRL_FLAG_TLV                   = 12,
  /**Acess control flag */
  DS_PROFILE_IM_CN_FLAG_TLV                         = 13,
  /** im_cn_flag */
  DS_PROFILE_APN_DISABLE_FLAG_TLV                   = 14,
  /** APN disable flag */
  DS_PROFILE_PDN_INACTIVITY_TIMEOUT_TLV             = 15,
  /** PDN inactivity time out */
  DS_PROFILE_APN_CLASS_TLV                          = 16,
  /**APN calss */
  DS_PROFILE_APN_BEARER_TLV                         = 17,
  /** APN bearer */
  DS_PROFILE_OPERATOR_RESERVED_PCO_TLV              = 18,
  /** OPERATOR RESERVED PCO */
  DS_PROFILE_MCC_TLV                                = 19,
  /** MCC */ 
  DS_PROFILE_MNC_TLV                                = 20,
  /** MNC */
  DS_PROFILE_MAX_PDN_CONN_PER_BLOCK_TLV             = 21,
  /** Max PDN Connections per Time Block */
  DS_PROFILE_PDN_DISCON_WAIT_TIME_TLV               = 22,
  /** Max pdn wait time */
  DS_PROFILE_MAX_PDN_CONN_TIMER_TLV                 = 23,
  /** Max PDN Connection Time */
  DS_PROFILE_PDN_REQ_WAIT_TIMER_TLV                 = 24,
  /** PDN Request Wait Time */
  DS_PROFILE_ROAMING_DISALLOWED_TLV                 = 25,
  /** Roaming disallowed*/
  DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE_TLV         = 26,
  /** LTE Roaming PDP type*/ 
  DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE_TLV        = 27,
  /** UMTS Roaming PDP type*/
  DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE_TLV       = 28,
  /** Override IP type of the default home pdp type*/
  DS_PROFILE_PARAM_ATTACH_WO_PDN_TLV                = 29,
  /** Attach without PDN*/
  DS_PROFILE_CLAT_ENABLED_TLV                       = 30,
  /** Clat enabled*/
  DS_PROFILE_PDP_ADDR_TLV                           = 31,
  /** DS profile PDP_ADDR */
  DS_PROFILE_FAILURE_TIMERS_TLV                     = 32,
  DS_PROFILE_MSISDN_FLAG_TLV                        = 33
  /** MSISDN Flag*/
  /*Newly added parameters should be appended to the end of this line*/
}ds_profile_param_enum_type;

/* Profile header */
typedef PACKED struct PACKED_POST{
  uint8 size;
  uint8 num_tlvs;
}ds_profile_header;

/** @brief Stores the parameter value for get/set operations.
  if param is a string (like for example apn name) len should be
  equal to strlen of that string in profile
 */
typedef struct 
{
  ds_profile_param_enum_type tlv; /** TLV identifier */
  uint8   len;   /** Buffer length. */
  void     *buf;  /** Buffer to store the parameter value. */

} ds_profile_info_type;

/** structure to get the profile list */
typedef struct
{
  ds_profile_pdp_type_e			    pdp_type;
  char                              apn[DS_PROFILE_APN_STRING_LEN+1];  
} ds_profile_list_stype;

#define MAX_PROFILE_SIZE (sizeof(ds_profile+header) + sizeof(ds_profile_type))

/*===========================================================================
FUNCTION DS_PROFILE_INIT 

DESCRIPTION
  Initializes the DS profile . On the modem, this function is 
  called only once at initialization. This function reads the profiles from EFS path
  if present if not creates the default profile.

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
  ===========================================================================*/
ds_profile_status_etype ds_profile_init( void );

/*===========================================================================
FUNCTION DS_PROFILE_EFS_READ 

DESCRIPTION
  Initializes the DS profile library. On the modem, this function is 
  called only once at initialization. This function initializes the call back function
  for all profile operation

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
  ===========================================================================*/
ds_profile_status_etype ds_profile_efs_read();

/*===========================================================================
FUNCTION DS_PROFILE_EFS_WRITE

DESCRIPTION
  Initializes the DS profile library. On the modem, this function is
  called only once at initialization. This function initializes the call back function
  for all profile operation

DEPENDENCIES 
  NONE
RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_efs_write(void* ptr, int8 profile_number);

/*===========================================================================
FUNCTION DS_PROFILE_GET_FREE_INDEX

DESCRIPTION
  This function returns free index from the profile pool.

PARAMETERS

DEPENDENCIES 

RETURN VALUE 
  Free index from profile array

SIDE EFFECT
  None
===========================================================================*/
int8 ds_profile_get_free_index();

/*===========================================================================
FUNCTION DS_PROFILE_SEARCH

DESCRIPTION
  This function returns profile which matches with the ident

PARAMETERS
  @param[in] profile_ident Profile identifier
  @param[op] Profile content matches with the ident

DEPENDENCIES 

RETURN VALUE 
  profile index from the array of profiles

SIDE EFFECT
  None
===========================================================================*/
int8 ds_profile_search
(
    ds_profile_ident_info      profile_ident
);

/*===========================================================================
FUNCTION DS_PROFILE_SEARCH_EX

DESCRIPTION
  This function returns profile which matched the profile ident other then the 
  passed index

PARAMETERS
  @param[in] Profile ident to identify the profile
  @param[op] Profile content matches with the ident

DEPENDENCIES 

RETURN VALUE 
  profile index from the array of profiles

SIDE EFFECT
  None
===========================================================================*/
int8 ds_profile_search_ex
(
  ds_profile_ident_info profile_ident,
  uint8                 profile_index
);

/*===========================================================================
FUNCTION DS_PROFILE_RESET_TO_DEFAULT

DESCRIPTION
  This function reset the profile with the matched ident from the array of
  profiles

PARAMETERS
  @param[in] profile_ident Profile identifier

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype ds_profile_reset_to_default
( 
    int8                          profile_index
);


/*===========================================================================
FUNCTION DS_PROFILE_EFS_OPEN

DESCRIPTION
  This function open the file from efs for provided index

PARAMETERS
  @param[in] profile_index profile index is corresponding to profile number
  @param[in] is_default to check whether is a default profile or not

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_efs_open
(
    int8        profile_index, 
    boolean    *is_default
);

/*===========================================================================
FUNCTION DS_PROFILE_SETTING_FROM_PROFILE_ID

DESCRIPTION
  This function deletes profiles with the matched ident from the array of
  profiles

PARAMETERS
  @param[in] profile_pdp    Profile pdp_type

DEPENDENCIES 

RETURN VALUE 
  IP_TYPE
  NON_IP_TYPE

SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype ds_profile_setting_from_profile_id
(
 int8 profile_id,
 ds_profile_type_s *profile
);

/*===========================================================================
FUNCTION DS_PROFILE_NUMBER_IS_VALID

DESCRIPTION
  This function deletes profiles with the matched ident from the array of
  profiles

PARAMETERS
  @param[in] profile_id    Profile id
  @param[out] profile      profile pointer to store the profile content
  coressponding to the profile id

DEPENDENCIES 

RETURN VALUE 
  DS_PROFILE_RESULT_SUCCESS
  DS_PROFILE_RESULT_ERR_INVAL

SIDE EFFECTS 
  none
===========================================================================*/
boolean ds_profile_number_is_valid
(
 int profile_number
);

/*===========================================================================
FUNCTION DS_PROFILE_READ_PROFILE_NUM_FOR_EMBEDDED_AND_TETHERED_CALLS

DESCRIPTION
  This function read tethered and embeded call profile number from EFS

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
 
RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
void  ds_profile_read_profile_num_for_embedded_and_tethered_calls();

/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_NUM_FOR_EMBEDDED_CALLS

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( embedded calls ).  The cached value is return.
  
PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
 
RETURN VALUE 
  The cached embeded profile value 

SIDE EFFECTS 
  None
===========================================================================*/
int8 ds_profile_get_profile_number_for_embeded_calls();

/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_NUM_FOR_TETHERED_CALLS

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( tethered calls ).  The cached value is return.
  
PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
 
RETURN VALUE 
  The cached tethered profile value

SIDE EFFECTS 
  None
===========================================================================*/
int8 ds_profile_get_profile_number_for_tethered_calls();

/*===========================================================================
FUNCTION DS_PROFILE_MODIFY_INTERNAL

DESCRIPTION
  Set the profile data identifier based on the mask provided.
  Sets the profile data element identified by the identifier.The pre-fetched 
  copy is modified. The ds_profile_end_transaction() function modifies the 
  profile on the modem

PARAMETERS
  @param[in]  profile_ident Identifies the profile data elements.
  @param[in]  mask          Identifier of the profile data element which is modified by client
  @param[in] *payload       Provides the modified value for ident mentioned in the mask
  @param[in]  is_create     To diffrentiate the function call for create to avoid invoking the cb functions

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
  
============================================================================*/
ds_profile_status_etype ds_profile_modify_internal
(
  ds_profile_ident_info      profile_ident,
  uint64                     modify_mask,
  ds_profile_type_s          *payload,
  boolean                     is_create
);

/*===========================================================================

FUNCTION DS_PROFILE_GET_PROFILE_ID()

DESCRIPTION
  This function returns the profile number which is internal to DS

DEPENDENCIES
  None.

RETURN VALUE
profile ID

SIDE EFFECTS
  None.

===========================================================================*/
int8 ds_profile_get_profile_id
( 
  ds_profile_ident_info      profile_ident
);


/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_LIST

DESCRIPTION
  This function returns list of profiles APN and IP type

PARAMETERS
  @param[out] profile_list    profile list pointer to store the profiles
  Memory for this pointer needs to be maintained by clients

DEPENDENCIES 

RETURN VALUE 
  Number of profiles
  
SIDE EFFECTS 
  none
===========================================================================*/
uint8 ds_profile_get_profile_list
(
 ds_profile_list_stype *profile_list
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_NUM_LIST

DESCRIPTION
  This function returns list of valid profile numbers in the passed buffer 

PARAMETERS
  @param[out] profile_list    profile list pointer to store the profiles
  Memory for this pointer needs to be maintained by clients

DEPENDENCIES 

RETURN VALUE 
  Number of profiles
  
SIDE EFFECTS 
  none
===========================================================================*/
uint8 ds_profile_cache_get_valid_profile_numbers
(
 uint8 *profile_num_list
);

/*......................................................................................
             PROFILE_CHANGES_FOR_ATCOP
.......................................................................................*/


/*===========================================================================
FUNCTION DSI_PROFILE_DELETE_ex

DESCRIPTION
  This function deletes profiles with the matched profile number

PARAMETERS
  @param[in] profile_number Profile number

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype ds_profile_delete_ex
( 
  uint8                profile_number
);

/*===========================================================================
FUNCTION DSI_PROFILE_CREATE_EX

DESCRIPTION
  This function creates the profiles with provided value

PARAMETERS
  @param[in] profile_ident profile identifier
  @param[in] mask Identifier of the profile data element which is modified by client
  @param[in] payload Provides the modified value for ident mentioned in the mask
  @param[in] num   Profile number

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_create_ex
( 
 ds_profile_ident_info      profile_ident,
 uint64                     mask,
 ds_profile_type_s          *pay_load,
 uint8                      num
);

/*===========================================================================
FUNCTION DS_GET_PROFILE_EX

DESCRIPTION
  Get the profile data by the identifier. This identifiers 
  which used to identify the profile.

PARAMETERS
  @param[in] profile_ident Identifies the profile data elements.
  @param[op] payload       Pointer to return the profile content.
  @param[op] profile_num   profile number for the profile

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  none

============================================================================*/

ds_profile_status_etype ds_profile_get_param_ex
(
 ds_profile_ident_info      profile_ident,
 ds_profile_type_s          *payload,
 uint8                      *profile_num
);


#endif
