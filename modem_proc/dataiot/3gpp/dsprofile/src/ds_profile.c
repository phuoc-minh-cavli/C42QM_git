/*===========================================================================
                      DS_PROFILE.C
DESCRIPTION
This file defines ds profile APIs.

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

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "ds_prof.h"
#include "data_msg.h"
#include "mcfg_fs.h"
#include "ds_system_heap.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_apm_efs_parser.h"



static const char PROFILE_DIR[] = "/Data_Profiles/";
static const char PROFILE_FILENAME_BASE[] = "Profile";
static const char ds_profile_embedded_call_file_name[] = 
                                     "/pdp_profiles/embedded_call_prof_num";
static const char ds_profile_tethered_call_file_name[] = 
                                    "/pdp_profile/tethered_call_prof_num";
byte cached_embedded_call_profile_number = 1;
byte cached_tethered_call_profile_number = 1;

mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;
ds_profile_type_s *ds_profile_list [DS_PROFILE_MAX_PROFILE_NUM] = {NULL};

ds_profile_change_handler_type *ds_profile_change_handler[CLIENT_MAX] = {NULL};

#define PROFILE_INDEX_FROM_NUMBER(num) (num-1) 
#define PROFILE_INDEX_TO_NUMBER(num) (num+1)
/*---------------------------------------------------------------------------
  UTILITY MACROS
---------------------------------------------------------------------------*/
#define MAX_PATH_LEN  128
#define TLV_TO_MASK(tlv) ((uint64)1 << tlv)
#define TLV_SIZE 1
#define TLV_LENGTH 1
#define PROFILE_HDR_SIZE(size) (TLV_SIZE + TLV_LENGTH + size)
#define INVALID_PROFILE_NUMBER -1
#define INVALID_PROFILE_INDEX  -1
#define DEFAULT_PROFILE_INDEX   0
#define IGNORE_PROFILE          0xff
#define DEFAULT_PROFILE_NUMBER  1
#define MAX_TLV_SIZE (33*(TLV_SIZE + TLV_LENGTH))+ sizeof(ds_profile_type_s) +\
	                  sizeof(ds_profile_header)


/*===========================================================================
FUNCTION ds_profile_build_efs_profile

DESCRIPTION
  This function creates the profile in efs file structure format

PARAMETERS
  @param[in] profile_index profile index is corresponding to profile number
  @param[in] is_default to check whether is a default profile or not

DEPENDENCIES 

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/

void ds_profile_build_efs_profile
(
  ds_profile_param_enum_type tlv,
  uint8                      size,
  void                       *payload,
  ds_profile_type_s          *temp_profile,
  ds_profile_header          *profile_header,
  char                       **offset_ptr
 )
{
  uint64 mask = TLV_TO_MASK(tlv);
/*...........................................................................*/

  memscpy(*offset_ptr, TLV_SIZE, &tlv, TLV_SIZE);
  *offset_ptr += TLV_SIZE;
  memscpy(*offset_ptr, TLV_LENGTH, &size, TLV_LENGTH);
  *offset_ptr += TLV_LENGTH;
  memscpy(*offset_ptr, size, payload, size);
  *offset_ptr += size;
  temp_profile->mask |= mask;
  profile_header->size += size;
  profile_header->size += (TLV_SIZE + TLV_LENGTH);
  profile_header->num_tlvs += 1;
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_MODIFY

DESCRIPTION
  Internally calls modify_internal function for modify operation
  
PARAMETERS
  @param[in]  profile_ident Identifies the profile data elements.
  @param[in]  mask          Identifier of the profile data element which is modified by client
  @param[in] *payload       Provides the modified value for ident mentioned in the mask

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  None
  
============================================================================*/
ds_profile_status_etype ds_profile_modify
(
  ds_profile_ident_info      profile_ident,
  uint64                     modify_mask,
  ds_profile_type_s          *payload
 )
{
  ds_profile_status_etype ret_val = DS_PROFILE_RESULT_SUCCESS;
  
  ret_val = ds_profile_modify_internal
               (profile_ident, modify_mask, payload, FALSE);

  DS_MSG1(MSG_LEGACY_HIGH, "ds_profile_modify ret_val%d", ret_val);
  return ret_val;

}
 
 
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
  boolean                    is_create
)
{
  int8 profile_index = 0;
  uint64 temp_mask = 0;
  uint64 trv_mask = 0;                // traversal_bit_mask
  uint64 bitmask_ident = 1;          //bitmask to get the bit value from the mask
  uint64 ident;
  void* efs_profile = NULL;
  ds_profile_header *profile_header = NULL;
  uint64 temp_x = 1;
  char* offset_ptr = NULL;
  ds_profile_status_etype ret_val = DS_PROFILE_RESULT_SUCCESS;
  ds_profile_client_etype  client_id = CLIENT_MIN;
  ds_profile_ident_info    changed_profile_ident;
  ds_profile_type_s*      temp_profile = NULL;
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SYSTEM_HEAP_MEM_ALLOC(efs_profile, MAX_TLV_SIZE, void*);
  
  if(efs_profile == NULL)
  {
    ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
    goto EXIT;
  }
  memset(efs_profile, 0, MAX_TLV_SIZE);

  DS_SYSTEM_HEAP_MEM_ALLOC(profile_header, sizeof(ds_profile_header),
  	                       ds_profile_header*);
  if(profile_header == NULL)
  {
    ret_val =  DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
    goto EXIT;
  }
  memset(profile_header, 0, sizeof(ds_profile_header));
  memset(&changed_profile_ident, 0, sizeof(ds_profile_ident_info));

  if ((profile_index = ds_profile_search(profile_ident))
      != INVALID_PROFILE_NUMBER)
  {
    if(modify_mask & DS_PROFILE_TYPE || modify_mask & DS_PROFILE_APN_NAME)
    {       
      if( strlen(payload->context.apn) > DS_PROFILE_APN_STRING_LEN)
      {
        DS_MSG1(MSG_LEGACY_HIGH, "payload pdp type %d",payload->context.pdp_type);
        ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
        local_err = DS_LOCAL_ERR_VAL_0;
        goto EXIT;
      }

      if(modify_mask & DS_PROFILE_TYPE)
      {
        if(payload->context.pdp_type >= DS_PROFILE_PDP_MAX)
        {
          ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
          local_err = DS_LOCAL_ERR_VAL_1;
          goto EXIT;
        }
        changed_profile_ident.pdp_type = payload->context.pdp_type;
      }
      else
      {
         changed_profile_ident.pdp_type = profile_ident.pdp_type;
      }

   
      if(modify_mask & DS_PROFILE_APN_NAME)
      {
        memscpy(changed_profile_ident.apn,
                strlen(payload->context.apn)+1, payload->context.apn,
                strlen(payload->context.apn)+1);
      }
      else
      {
        memscpy(changed_profile_ident.apn,
                DS_PROFILE_APN_STRING_LEN, profile_ident.apn,
                strlen(profile_ident.apn));
      }
      if (!is_create &&
           INVALID_PROFILE_NUMBER != ds_profile_search_ex(
                                        changed_profile_ident,profile_index))
      {        
          ret_val = DS_PROFILE_ALREADY_PRESENT;
          local_err = DS_LOCAL_ERR_VAL_2;
          goto EXIT;
      }
    }
	
    if(ds_profile_list[profile_index] == NULL)
    {
      ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
      goto EXIT;
    }
    else
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(temp_profile, sizeof(ds_profile_type_s),
                               ds_profile_type_s*);

      if(temp_profile == NULL)
      {
        ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
        local_err = DS_LOCAL_ERR_VAL_3;
        goto EXIT;
      }
      memscpy(temp_profile, sizeof(ds_profile_type_s), 
                    ds_profile_list[profile_index],sizeof(ds_profile_type_s));
    }
    offset_ptr = (char *)efs_profile + sizeof(ds_profile_header);
    temp_mask = temp_profile->mask | modify_mask;
    trv_mask = temp_mask;

    do
    {
      if (trv_mask & temp_x)
      {
        ident = temp_mask & bitmask_ident;
        switch(ident)
        {
          case DS_PROFILE_APN_NAME:
            if(modify_mask & ident)
            {
              if((strncasecmp((char*)temp_profile->context.apn, 
                  payload->context.apn, DS_PROFILE_APN_STRING_LEN+1) !=0))
              {
                memset(temp_profile->context.apn, 0,
                                DS_PROFILE_APN_STRING_LEN);
                memscpy(temp_profile->context.apn,
                  DS_PROFILE_APN_STRING_LEN, payload->context.apn,
                  strlen(payload->context.apn));
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_APN_NAME;
              }
               
            }
            ds_profile_build_efs_profile ( DS_PROFILE_APN_NAME_TLV,
               strlen(temp_profile->context.apn),
               temp_profile->context.apn,temp_profile,
               profile_header,&offset_ptr );
            break;

          case DS_PROFILE_TYPE:
            if(modify_mask & ident)
            {
              if(temp_profile->context.pdp_type != payload->context.pdp_type)
              {
                temp_profile->context.pdp_type = payload->context.pdp_type;
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_TYPE;
              }
            }
            ds_profile_build_efs_profile (DS_PROFILE_TYPE_TLV,
               sizeof(ds_profile_pdp_type_e),
                &temp_profile->context.pdp_type,
                temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_CONTEXT_H_COMP:
            if(modify_mask & ident)
            {
              if(payload->context.h_comp >= DS_PROFILE_HEADER_COMP_MAX)
              {
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                local_err = DS_LOCAL_ERR_VAL_4;
                goto EXIT;
              }
              temp_profile->context.h_comp = payload->context.h_comp;
            }
            ds_profile_build_efs_profile (DS_PROFILE_CONTEXT_H_COMP_TLV,
               sizeof(ds_profile_header_comp_e_type),
               &temp_profile->context.h_comp,
               temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_CONTEXT_D_COMP:
            if(modify_mask & ident)
            {
              if(payload->context.d_comp >= DS_PROFILE_DATA_COMP_MAX)
              {
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                local_err = DS_LOCAL_ERR_VAL_5;
                goto EXIT;
              }
              temp_profile->context.d_comp = payload->context.d_comp;
            }
            ds_profile_build_efs_profile (DS_PROFILE_CONTEXT_D_COMP_TLV,
               sizeof(ds_profile_header_comp_e_type),
               &temp_profile->context.d_comp,
               temp_profile,profile_header, &offset_ptr);
            break;

          case DS_PROFILE_DNS_ADDR_V4_PRIMARY:
            if(modify_mask & ident)
            {
              temp_profile->dns_addr.\
                  primary_dns_addr.pdp_addr.pdp_addr_ipv4
                =payload->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4;
            }
            ds_profile_build_efs_profile (DS_PROFILE_CONTEXT_D_COMP_TLV,
                sizeof(uint32), &temp_profile->dns_addr
               .primary_dns_addr.pdp_addr.pdp_addr_ipv4,temp_profile,
                profile_header,&offset_ptr);
            break;

          case DS_PROFILE_DNS_ADDR_V4_SECONDARY:
            if(modify_mask & ident)
            {
              temp_profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4
                =payload->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4;
            }
            ds_profile_build_efs_profile (DS_PROFILE_CONTEXT_D_COMP_TLV,
                sizeof(uint32), &temp_profile->dns_addr.\
                secondary_dns_addr.pdp_addr.pdp_addr_ipv4,temp_profile,
                profile_header, &offset_ptr);
            break;

          case DS_PROFILE_AUTH_USERNAME:
            if(modify_mask & ident)
            {
              if(strlen(payload->auth.username) > DS_PROFILE_MAX_AUTH_STRING_LEN)
              {
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
                goto EXIT;                 
              }
              memset(temp_profile->auth.username, 0,
                        DS_PROFILE_MAX_AUTH_STRING_LEN);
              memscpy(temp_profile->auth.username,
                  DS_PROFILE_MAX_AUTH_STRING_LEN, payload->auth.username,
                  strlen(payload->auth.username));
            }
            ds_profile_build_efs_profile (DS_PROFILE_AUTH_USERNAME_TLV,
               strlen(temp_profile->auth.username),
               &temp_profile->auth.username, temp_profile,
               profile_header,&offset_ptr);
            break;

          case DS_PROFILE_AUTH_PASSWORD:
            if(modify_mask & ident)
            {
              if(strlen(payload->auth.password) > DS_PROFILE_MAX_AUTH_STRING_LEN)
              {
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
                goto EXIT;                 
              }
              memset(temp_profile->auth.password, 0,
                        DS_PROFILE_MAX_AUTH_STRING_LEN);
              memscpy(temp_profile->auth.password,
                  DS_PROFILE_MAX_AUTH_STRING_LEN, payload->auth.password,
                  strlen(payload->auth.password));
            }
            ds_profile_build_efs_profile (DS_PROFILE_AUTH_PASSWORD_TLV,
               strlen(temp_profile->auth.password),
               &temp_profile->auth.password,
               temp_profile,profile_header,&offset_ptr);
            break;

          case DS_PROFILE_AUTH_TYPE:
            if(modify_mask & ident)
            {
              if(payload->auth.auth_type >= DS_PROFILE_AUTH_MAX)
              {
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                goto EXIT;  
              }
              temp_profile->auth.auth_type = payload->auth.auth_type;
            }
            ds_profile_build_efs_profile (DS_PROFILE_AUTH_TYPE_TLV,
               sizeof(ds_profile_auth_enum_type),&temp_profile->auth.auth_type,
               temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_DNS_ADDR_V6_PRIMARY:
            switch (payload->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.type)
            {
              case DS_PROFILE_IPV6_8:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr8,
                      ((sizeof(uint8))*16),payload->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr8,
                      ((sizeof(uint8))*16));
                }
                ds_profile_build_efs_profile(DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV,
                   ((sizeof(uint8))*16), &temp_profile->\
                   dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr8,
                   temp_profile, profile_header, &offset_ptr);
                break;

              case DS_PROFILE_IPV6_16:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr16,
                      ((sizeof(uint16))*8),payload->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr16,
                      ((sizeof(uint16))*8));
                }
                ds_profile_build_efs_profile (DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV,
                   ((sizeof(uint16))*8),
                    &temp_profile->dns_addr.primary_dns_addr.\
                    pdp_addr.pdp_addr_ipv6.in6_u.u6_addr16,temp_profile,
                    profile_header, &offset_ptr);
                break;

              case DS_PROFILE_IPV6_32:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32,
                      ((sizeof(uint32))*4),payload->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32,
                      ((sizeof(uint32))*4));
                }
                ds_profile_build_efs_profile (DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV,
                   ((sizeof(uint32))*4),
                   &temp_profile->dns_addr.\
                    primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32,
                   temp_profile, profile_header,&offset_ptr);
                break;

              case DS_PROFILE_IPV6_64:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64,
                      ((sizeof(uint64))*2),payload->dns_addr.primary_dns_addr.\
                      pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64,
                      ((sizeof(uint64))*2));
                }
                ds_profile_build_efs_profile (DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV,
                   ((sizeof(uint64))*2),
                    &temp_profile->dns_addr.primary_dns_addr.\
                    pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64, temp_profile,
                    profile_header,&offset_ptr);
                break;
            }
            break;

          case DS_PROFILE_DNS_ADDR_V6_SECONDARY:
            switch (payload->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.type)
            {
              case DS_PROFILE_IPV6_8:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      secondary_dns_addr.pdp_addr.\
                      pdp_addr_ipv6.in6_u.u6_addr8,((sizeof(uint8))*16),
                      payload->dns_addr.secondary_dns_addr.pdp_addr.\
                      pdp_addr_ipv6.in6_u.u6_addr8, ((sizeof(uint8))*16));
                }
                ds_profile_build_efs_profile
                   (DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV, ((sizeof(uint8))*16),
                    &temp_profile->dns_addr.secondary_dns_addr.pdp_addr.\
                    pdp_addr_ipv6.in6_u.u6_addr8,temp_profile, profile_header,
                    &offset_ptr
                    );
                break;

              case DS_PROFILE_IPV6_16:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      secondary_dns_addr.pdp_addr.\
                      pdp_addr_ipv6.in6_u.u6_addr16,((sizeof(uint16))*8),
                      payload->dns_addr.secondary_dns_addr.\
                      pdp_addr.pdp_addr_ipv6.in6_u.u6_addr8,
                      ((sizeof(uint16))*8));
                }
                ds_profile_build_efs_profile
                   (DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV, ((sizeof(uint16))*8),
                    &temp_profile->dns_addr.secondary_dns_addr.\
                    pdp_addr.pdp_addr_ipv6.in6_u.u6_addr16, temp_profile,
                    profile_header, &offset_ptr
                    );
                break;

              case DS_PROFILE_IPV6_32:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32,
                      ((sizeof(uint32))*4),payload->dns_addr.\
                      secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32,
                      ((sizeof(uint32))*4));
                }
                ds_profile_build_efs_profile
                   (DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV, ((sizeof(uint32))*4),
                    &temp_profile->\
                    dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                    u6_addr32, temp_profile, profile_header, &offset_ptr
                    );
                break;

              case DS_PROFILE_IPV6_64:
                if(modify_mask & ident)
                {
                  memscpy(&temp_profile->dns_addr.\
                      secondary_dns_addr.pdp_addr.\
                      pdp_addr_ipv6.in6_u.u6_addr64,((sizeof(uint64))*2),
                      payload->dns_addr.secondary_dns_addr.pdp_addr.\
                      pdp_addr_ipv6.in6_u.u6_addr64,
                      ((sizeof(uint64))*2));
                }
                ds_profile_build_efs_profile
                   (DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV, ((sizeof(uint64))*2),
                    &temp_profile->dns_addr.secondary_dns_addr.\
                    pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64, 
                    temp_profile, profile_header, &offset_ptr
                    );
                break;
            }
            break;

          case DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG:
            if(modify_mask & ident)
            {
              temp_profile->request_pcscf_address_flag =
                payload->request_pcscf_address_flag;
            }
            ds_profile_build_efs_profile
               (DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG_TLV, sizeof(boolean),
                &temp_profile->request_pcscf_address_flag,
                temp_profile,profile_header, &offset_ptr
                );
            break;

          case DS_PROFILE_ACCESS_CTRL_FLAG:

            if(modify_mask & ident)
            {
              if(payload->context.access_ctrl_flag >
                                  DSAT_ACCESS_CONTROL_PERMISSION)
              {
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                local_err = DS_LOCAL_ERR_VAL_6;
                goto EXIT;  
              }
              temp_profile->context.access_ctrl_flag =
                payload->context.access_ctrl_flag;
            }
            ds_profile_build_efs_profile
               (DS_PROFILE_ACCESS_CTRL_FLAG_TLV,
                sizeof(ds_profile_access_control_e_type),
                &temp_profile->context.access_ctrl_flag,
                temp_profile, profile_header,&offset_ptr
               );
            break;

          case DS_PROFILE_IM_CN_FLAG:
            if(modify_mask & ident)
            {
              temp_profile->im_cn_flag = payload->im_cn_flag;
            }
            ds_profile_build_efs_profile (DS_PROFILE_IM_CN_FLAG_TLV,
                sizeof(boolean), &temp_profile->im_cn_flag,
                temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_APN_DISABLE_FLAG:
            if(modify_mask & ident)
            {
              if(temp_profile->apn_disable_flag !=
			     payload->apn_disable_flag)
              {
                temp_profile->apn_disable_flag = payload->apn_disable_flag;
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_APN_DISABLE_FLAG;
              }
            }
            ds_profile_build_efs_profile (DS_PROFILE_APN_DISABLE_FLAG_TLV,
                sizeof(boolean), &temp_profile->apn_disable_flag,
                temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_PDN_INACTIVITY_TIMEOUT:
            if(modify_mask & ident)
            {
              if(payload->inactivity_timer_val > DS_PROFILE_MAX_INACTIVITY_TIMEOUT)
              {
                local_err = DS_LOCAL_ERR_VAL_7;
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
                goto EXIT;
              }
              temp_profile->inactivity_timer_val = payload->inactivity_timer_val;
            }
            ds_profile_build_efs_profile (DS_PROFILE_PDN_INACTIVITY_TIMEOUT_TLV,
                sizeof(uint32), &temp_profile->inactivity_timer_val,
                temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_APN_CLASS:
            if(modify_mask & ident)
            {
              if(temp_profile->apn_class != payload->apn_class)
              {
                temp_profile->apn_class = payload->apn_class;
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_APN_CLASS;
              }
            }
            ds_profile_build_efs_profile (DS_PROFILE_APN_CLASS_TLV,
                sizeof(uint8), &temp_profile->apn_class,
                temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_APN_BEARER:
            if(modify_mask & ident)
            {
              if(payload->apn_bearer != DS_PROFILE_APN_BEARER_FLAG_ALL &&
			  	 (payload->apn_bearer & ~(DS_PROFILE_APN_BEARER_FLAG_G |
			  	 DS_PROFILE_APN_BEARER_FLAG_L)) != 0 )
              {
                local_err = DS_LOCAL_ERR_VAL_8;
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
                goto EXIT;
              }
              temp_profile->apn_bearer = payload->apn_bearer;
            }
            ds_profile_build_efs_profile (DS_PROFILE_APN_BEARER_TLV,
                sizeof(uint8), &temp_profile->apn_bearer,
                temp_profile, profile_header,&offset_ptr);
            break;

            case DS_PROFILE_OPERATOR_RESERVED_PCO:
              if(modify_mask & ident)
              {
                if(payload->operator_reserved_pco < 
					         DS_PROFILE_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN ||
                   payload->operator_reserved_pco > 
                            DS_PROFILE_OPERATOR_RESERVED_PCO_ID_RANGE_END)
                {
                  local_err = DS_LOCAL_ERR_VAL_9;
                  ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
                  goto EXIT;
                }
                temp_profile->operator_reserved_pco = payload->operator_reserved_pco;
              }
            ds_profile_build_efs_profile
               (DS_PROFILE_OPERATOR_RESERVED_PCO_TLV, sizeof(uint16),
                &temp_profile->operator_reserved_pco,
                temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_MCC:
            if(modify_mask & ident)
            {
              temp_profile->mcc = payload->mcc;
            }
            ds_profile_build_efs_profile (DS_PROFILE_MCC_TLV,
               sizeof(uint16), &temp_profile->mcc,
               temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_MNC:
            if(modify_mask & ident)
            {
              temp_profile->mnc = payload->mnc;
            }
            ds_profile_build_efs_profile (DS_PROFILE_MNC_TLV,
               sizeof(ds_profile_mnc_type), &temp_profile->mnc,
               temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_MAX_PDN_CONN_PER_BLOCK:
            if(modify_mask & ident)
            {
              if(payload->max_pdn_conn_per_blk > DS_PROFILE_MAX_PDN_CONN_PER_BLOCK_VAL)
              {
                local_err = DS_LOCAL_ERR_VAL_10;
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
                goto EXIT;
              }
              if( temp_profile->max_pdn_conn_per_blk !=
			     payload->max_pdn_conn_per_blk )
              {
                temp_profile->max_pdn_conn_per_blk = payload->max_pdn_conn_per_blk;
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_MAX_PDN_CONN_PER_BLOCK;
              }
            }
            ds_profile_build_efs_profile (DS_PROFILE_MAX_PDN_CONN_PER_BLOCK_TLV,
               sizeof(uint16),&temp_profile->\
               max_pdn_conn_per_blk, temp_profile, profile_header,
               &offset_ptr);
            break;

          case DS_PROFILE_MAX_PDN_CONN_TIMER:
            if(modify_mask & ident)
            {
              if(payload->max_pdn_conn_time > DS_PROFILE_MAX_PDN_CONN_TIME)
              {
                local_err = DS_LOCAL_ERR_VAL_11;
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
				goto EXIT;                
              }
              if(temp_profile->max_pdn_conn_time  != payload->max_pdn_conn_time)
              {
                temp_profile->max_pdn_conn_time = payload->max_pdn_conn_time;
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_MAX_PDN_CONN_TIMER;
              }
            }
            ds_profile_build_efs_profile (DS_PROFILE_MAX_PDN_CONN_TIMER_TLV,
                sizeof(uint16), &temp_profile->max_pdn_conn_time,
                temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_PDN_REQ_WAIT_TIMER:
            if(modify_mask & ident)
            {
              if(payload->pdn_req_wait_time > DS_PROFILE_MAX_PDN_CONN_REQ_WAIT_TIME)
              {
                local_err = DS_LOCAL_ERR_VAL_12;
                ret_val = DS_PROFILE_RESULT_ERR_LEN_INVALID;
				goto EXIT;
              }
              if(temp_profile->pdn_req_wait_time != 
			       payload->pdn_req_wait_time)
              {
                temp_profile->pdn_req_wait_time = payload->pdn_req_wait_time;
              ds_profile_apn_param_changed = TRUE;
            }
              else
              {
                modify_mask &= ~DS_PROFILE_PDN_REQ_WAIT_TIMER;
              }
            }
            ds_profile_build_efs_profile (DS_PROFILE_PDN_REQ_WAIT_TIMER_TLV,
               sizeof(uint16), &temp_profile->pdn_req_wait_time,
               temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_ROAMING_DISALLOWED:
            if(modify_mask & ident)
            {
              temp_profile->roaming_disallowed = payload->roaming_disallowed;
            }
            ds_profile_build_efs_profile (DS_PROFILE_ROAMING_DISALLOWED_TLV,
               sizeof(boolean), &temp_profile->roaming_disallowed,
               temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE:
            if(modify_mask & ident)
            {
              if(payload->context.lte_roaming_pdp_type >= DS_PROFILE_PDP_MAX)
              {
                local_err = DS_LOCAL_ERR_VAL_13;
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                goto EXIT;
              }
              temp_profile->context.lte_roaming_pdp_type = 
                payload->context.lte_roaming_pdp_type;
            }
            ds_profile_build_efs_profile
               (DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE_TLV,
                sizeof(ds_profile_pdp_type_e),
                &temp_profile->context.lte_roaming_pdp_type, 
                temp_profile, profile_header, &offset_ptr
                );
            break;

          case DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE:
            if(modify_mask & ident)
            {
              if(payload->context.umts_roaming_pdp_type >= DS_PROFILE_PDP_MAX)
              {
                local_err = DS_LOCAL_ERR_VAL_14;
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                goto EXIT;
              }
             temp_profile->context.umts_roaming_pdp_type =
                payload->context.umts_roaming_pdp_type;
            }
            ds_profile_build_efs_profile
               (DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE_TLV,
                sizeof(ds_profile_pdp_type_e),
                &temp_profile->context.umts_roaming_pdp_type, 
                temp_profile, profile_header,&offset_ptr
                );
            break;

          case DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE:
            if(modify_mask & ident)
            {
              if(payload->context.umts_roaming_pdp_type >= DS_PROFILE_PDP_MAX)
              {
                local_err = DS_LOCAL_ERR_VAL_15;
                ret_val = DS_PROFILE_RESULT_ERR_INVAL;
                goto EXIT;
              }
              temp_profile->context.override_home_pdp_type =
                payload->context.override_home_pdp_type;
            }
            ds_profile_build_efs_profile
               (DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE_TLV,
                sizeof(ds_profile_pdp_type_e), 
                &temp_profile->context.override_home_pdp_type, 
                temp_profile, profile_header,&offset_ptr
                );
            break;

          case DS_PROFILE_PARAM_ATTACH_WO_PDN:
            if(modify_mask & ident)
            {
              temp_profile->attach_wo_pdn = payload->attach_wo_pdn;
            }
            ds_profile_build_efs_profile (DS_PROFILE_PARAM_ATTACH_WO_PDN_TLV,
               sizeof(boolean),
               &temp_profile->attach_wo_pdn,
               temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_CLAT_ENABLED:
            if(modify_mask & ident)
            {
              temp_profile->clat_enabled = payload->clat_enabled;
            }
            ds_profile_build_efs_profile (DS_PROFILE_CLAT_ENABLED_TLV,
               sizeof(boolean), &temp_profile->clat_enabled,
               temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_PDP_ADDR:
            if(modify_mask & ident)
            {
              temp_profile->pdp_addr = payload->pdp_addr;
            }
            ds_profile_build_efs_profile (DS_PROFILE_PDP_ADDR_TLV,
               sizeof(ds_profile_addr_type),
               &temp_profile->pdp_addr,
               temp_profile, profile_header,&offset_ptr);
            break;

          case DS_PROFILE_PDN_DISCON_WAIT_TIME:
            if(modify_mask & ident)
            {
              temp_profile->pdn_discon_wait_time = payload->pdn_discon_wait_time;
            }
            ds_profile_build_efs_profile (DS_PROFILE_PDN_DISCON_WAIT_TIME_TLV,
               sizeof(uint8), 
               &temp_profile->pdn_discon_wait_time,
               temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_FAILURE_TIMERS:
            if(modify_mask & ident)
            {
              memscpy(temp_profile->failure_timers,
			          sizeof(temp_profile->failure_timers),
					  payload->failure_timers,
					  sizeof(payload->failure_timers));
              
            }
            ds_profile_build_efs_profile (DS_PROFILE_FAILURE_TIMERS_TLV,
               sizeof(temp_profile->failure_timers), 
               &temp_profile->failure_timers,
               temp_profile, profile_header, &offset_ptr);
            break;

          case DS_PROFILE_MSISDN_FLAG:
            if(modify_mask & ident)
            {
              temp_profile->msisdn_flag = payload->msisdn_flag;
            }
            ds_profile_build_efs_profile (DS_PROFILE_MSISDN_FLAG_TLV,
               sizeof(boolean), &temp_profile->msisdn_flag,
               temp_profile, profile_header,&offset_ptr);
            break;

          default:
            break;
        }
      }
      bitmask_ident <<= 1;
      trv_mask >>= 1;
    } while (trv_mask);
  }
  else
  {
    ret_val = DS_PROFILE_RESULT_ERR_INVAL_IDENT;
    goto EXIT;
  }

  if (!is_create)
  {    
    ds_pdn_mgr_profile_change_cb(PROFILE_INDEX_TO_NUMBER(profile_index),
                                 modify_mask,
                                 ds_profile_apn_param_changed);
    ds_profile_apn_param_changed = FALSE;

    while (client_id < CLIENT_MAX)
    {
      if (ds_profile_change_handler[client_id] != NULL)
      {
        (*ds_profile_change_handler[client_id])
                                         (profile_ident,modify_mask, *payload);
      }
      client_id++; 	  
    }
  }
  
  if(!(payload->is_non_persistent))
  {
    memscpy(efs_profile, sizeof(ds_profile_header), profile_header,
            sizeof(ds_profile_header));
    if ((ret_val = ds_profile_efs_write
                      (efs_profile, PROFILE_INDEX_TO_NUMBER(profile_index))) !=
                       DS_PROFILE_RESULT_SUCCESS)
    {
      ret_val = DS_PROFILE_RESULT_FAIL;
      goto EXIT;
    }
    memscpy(ds_profile_list[profile_index],sizeof(ds_profile_type_s),
                  temp_profile,sizeof(ds_profile_type_s));
  }
  else
  {
    memscpy(ds_profile_list[profile_index],sizeof(ds_profile_type_s),
	         temp_profile,sizeof(ds_profile_type_s));
  }

EXIT:
    if (efs_profile != NULL)
    {
      DS_SYSTEM_HEAP_MEM_FREE(efs_profile);
      efs_profile = NULL;
    }
    if (profile_header != NULL)
    {
      DS_SYSTEM_HEAP_MEM_FREE(profile_header);
      profile_header = NULL;
    }
    if(temp_profile != NULL)
    {
      DS_SYSTEM_HEAP_MEM_FREE(temp_profile);
      temp_profile = NULL;
    }

  if (ret_val != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_MSG4(MSG_LEGACY_HIGH,"Modify profile failed with error %d for APN %s \
        PDP type %d local error %d", ret_val,profile_ident.apn, profile_ident.pdp_type,local_err);
    return ret_val;
  }
  return DS_PROFILE_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION DS_GET_PROFILE

DESCRIPTION
  Get the profile data by the identifier. This identifiers 
  which used to identify the profile.

PARAMETERS
  @param[in] profile_ident Identifies the profile data elements.
  @param[op] payload       Pointer to return the profile content.

DEPENDENCIES 

RETURN VALUE 
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

SIDE EFFECTS 
  none

============================================================================*/

ds_profile_status_etype ds_profile_get_param
(
 ds_profile_ident_info      profile_ident,
 ds_profile_type_s           *payload
)
{
  int8 profile_index = ds_profile_search(profile_ident);
  
  if (profile_index >= 0)
  {
    memscpy(payload, sizeof(ds_profile_type_s),ds_profile_list[profile_index],
        sizeof(ds_profile_type_s));
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"No profile found in cache ");
    return DS_PROFILE_RESULT_ERR_INVAL_IDENT ;
  }
  return DS_PROFILE_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION DSI_PROFILE_DELETE

DESCRIPTION
  This function deletes profiles with the matched ident from the array of
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
ds_profile_status_etype ds_profile_delete
( 
 ds_profile_ident_info      profile_ident
)
{
  mcfg_fs_status_e_type  delete_status = 0;
  int8 profile_index = ds_profile_search(profile_ident);
  char file_name[MAX_PATH_LEN] = {'\0'};
  int ret = 0;
/*...........................................................................*/

  if(PROFILE_INDEX_TO_NUMBER(profile_index) != 
             ds_profile_get_profile_number_for_tethered_calls() &&
	     PROFILE_INDEX_TO_NUMBER(profile_index) != 
	     ds_profile_get_profile_number_for_embeded_calls() &&
     profile_index > DEFAULT_PROFILE_INDEX)
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_profile_list[profile_index]);
    ds_profile_list[profile_index] = NULL;

    ret = snprintf(file_name, MAX_PATH_LEN, "%s%s%u", PROFILE_DIR,
                PROFILE_FILENAME_BASE, PROFILE_INDEX_TO_NUMBER(profile_index));
    if (!ret)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"BuildFileName(): error in snprintf");
      return -1;
    }
    delete_status = mcfg_fs_delete(file_name, MCFG_FS_TYPE_EFS, 0);
    if (delete_status != MCFG_FS_STATUS_OK)
    {
      DS_MSG1(MSG_LEGACY_ERROR,
          "Unable to delete EFS item, error:%d ", delete_status);
      return DS_PROFILE_RESULT_ERR_FILE_ACCESS;
    }
    return DS_PROFILE_RESULT_SUCCESS;
  }
  DS_ERROR_LOG_0("Default profile can't be deleted");
  return DS_PROFILE_RESULT_FAIL;
}

/*===========================================================================
FUNCTION DS_PROFILE_GET_FREE_INDEX

DESCRIPTION
  This function returns free index from the profile pool.

PARAMETERS

DEPENDENCIES 

RETURN VALUE 
  Free index from profile array
  -1 on faliure

SIDE EFFECT
  None
===========================================================================*/
int8 ds_profile_get_free_index()
{

  int8 indexes = DEFAULT_PROFILE_INDEX;
/*................................................................................*/
  
  while(indexes < DS_PROFILE_MAX_PROFILE_NUM &&
        ds_profile_list[indexes] != NULL)
  {
    indexes++;
  }
  if (indexes < DS_PROFILE_MAX_PROFILE_NUM)
  {
    return indexes;
  }
  return INVALID_PROFILE_INDEX;
}


/*===========================================================================
FUNCTION DSI_PROFILE_RESET_TO_DEFAULT

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
 int8                       profile_index
)
{

  if((profile_index > -1) && ds_profile_list[profile_index] != NULL)
  {
    memset(ds_profile_list[profile_index],0,sizeof(ds_profile_type_s));
    ds_profile_list[profile_index]->context.pdp_type = DS_PROFILE_PDP_IPV4V6;
    ds_profile_list[profile_index]->context.h_comp = DS_PROFILE_HEADER_COMP_OFF;
    ds_profile_list[profile_index]->context.d_comp = DS_PROFILE_DATA_COMP_OFF;
    ds_profile_list[profile_index]->apn_bearer = DS_PROFILE_APN_BEARER_FLAG_ALL;
    ds_profile_list[profile_index]->max_pdn_conn_per_blk = 
                                                DS_PROFILE_DEF_MAX_PDN_CONN_PER_BLK;
    ds_profile_list[profile_index]->max_pdn_conn_time = 
                                                   DS_PROFILE_DEF_MAX_PDN_CONN_TIME;
    ds_profile_list[profile_index]->pdn_discon_wait_time =
                                                   DS_PROFILE_DEF_PDN_REQ_WAIT_TIME;
    ds_profile_list[profile_index]->context.lte_roaming_pdp_type = DS_PROFILE_ROAMING_PDP_MAX;
    ds_profile_list[profile_index]->context.umts_roaming_pdp_type = DS_PROFILE_ROAMING_PDP_MAX;
    ds_profile_list[profile_index]->context.override_home_pdp_type = DS_PROFILE_ROAMING_PDP_MAX;
    ds_profile_list[profile_index]->attach_wo_pdn = TRUE;
	}
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH, "No profile found ");
    return DS_PROFILE_RESULT_ERR_INVAL_IDENT;
  }
  return DS_PROFILE_RESULT_SUCCESS;
}

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
ds_profile_status_etype ds_profile_efs_read()
{
  
  ds_profile_status_etype ret_val      = DS_PROFILE_RESULT_SUCCESS;
  void *heapMemForFile                 = NULL;
  ds_profile_header efs_header;
  ds_profile_type_s *temp_profile      = NULL;
  char* offset_ptr                     = NULL;
  char* buf                            = NULL;
  uint64 mask                          = 0;
  int8 profile_number                  = DEFAULT_PROFILE_INDEX;
  uint8 read_tlvs                      = 0;
  uint8 read_size                      = 0;
  ds_profile_ident_info profile_ident;
  char file_name[MAX_PATH_LEN]         = {'\0'};
  mcfg_fs_status_e_type read_status    = 0;
  uint8 size                           = 0;
  uint8 tlv                            = 0;
  struct fs_stat  temp_buf;
  int ret                              = 0;
/*................................................................................*/

  memset(&temp_buf, 0, sizeof(struct fs_stat));
  profile_ident.pdp_type = DS_PROFILE_PDP_IPV4V6;
  memset(profile_ident.apn, 0, DS_PROFILE_APN_STRING_LEN+1);
  DS_SYSTEM_HEAP_MEM_ALLOC(temp_profile, sizeof(ds_profile_type_s),
  	                        ds_profile_type_s *);
  if(temp_profile == NULL)
  {
    ret_val =  DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
    goto EXIT;;
  }

  while(profile_number < DS_PROFILE_MAX_PROFILE_NUM)
  {
    read_tlvs = 0;
    read_size = 0;
    memset(temp_profile, 0, sizeof(ds_profile_type_s));

    ret = snprintf(file_name, MAX_PATH_LEN, "%s%s%u", PROFILE_DIR,
        PROFILE_FILENAME_BASE, (profile_number+1) );
    if (!ret)
    {
      ret_val =  DS_PROFILE_RESULT_FAIL;
      goto EXIT;
    }
    if( (read_status = mcfg_fs_stat((const char*)file_name,
            &temp_buf,
            MCFG_FS_TYPE_EFS,
            MCFG_FS_SUBID_NONE)) !=  MCFG_FS_STATUS_OK)
    {
      DS_MSG_SPRINTF_1( MSG_LEGACY_ERROR,"Error opening file %s", file_name);
    }

    if (read_status != MCFG_FS_STATUS_OK && !profile_number)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_profile_list[0], sizeof(ds_profile_type_s),
	  	                       ds_profile_type_s*);
      if(ds_profile_list[0] == NULL)
      {
        ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
        goto EXIT;
      }
      if(ds_profile_reset_to_default(DEFAULT_PROFILE_INDEX) != 
	  	                             DS_PROFILE_RESULT_SUCCESS)
      {
        ret_val =  DS_PROFILE_RESULT_FAIL;
        goto EXIT;
      }
      mask = DS_PROFILE_APN_NAME | DS_PROFILE_TYPE;
      if (ds_profile_modify_internal(profile_ident, mask,
                     ds_profile_list[0], TRUE) != DS_PROFILE_RESULT_SUCCESS)
      {
        ret_val = DS_PROFILE_RESULT_FAIL;
        goto EXIT;
      }
      break;
    }
    else if(read_status == MCFG_FS_STATUS_OK)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(heapMemForFile, temp_buf.st_size, 
	  	                       void*);
      if(heapMemForFile == NULL)
      {
        ret_val =  DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
        goto EXIT;;
      }
      memset(heapMemForFile, 0, temp_buf.st_size);
      read_status =  mcfg_fs_read (file_name, heapMemForFile,
	  	                           temp_buf.st_size ,file_type, 0);
      if(read_status == MCFG_FS_STATUS_OK)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(ds_profile_list[profile_number],
			                     sizeof(ds_profile_type_s),ds_profile_type_s*);
        if(ds_profile_list[profile_number] == NULL)
        {
          ret_val =  DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
          goto EXIT;;
        }
        
        if(ds_profile_reset_to_default(profile_number) != DS_PROFILE_RESULT_SUCCESS)
        {
          ret_val =  DS_PROFILE_RESULT_FAIL;
          goto EXIT;
        }
       /* This is done to make sure that temp profile has default value set for default 
           parmeters as we copy its complete payload to profile entry at the end of the
           below loop */
        memscpy(temp_profile, sizeof(ds_profile_type_s), 
                ds_profile_list[profile_number], sizeof(ds_profile_type_s));

        memscpy(&efs_header, 2, heapMemForFile, 2);
        offset_ptr = (char *)heapMemForFile + sizeof(ds_profile_header);

        while(read_tlvs < efs_header.num_tlvs && read_size < efs_header.size)
        {
          memscpy(&tlv, TLV_SIZE, offset_ptr, TLV_SIZE);
          offset_ptr += TLV_SIZE;
          memscpy(&size, TLV_LENGTH, offset_ptr, TLV_LENGTH);
          offset_ptr += TLV_LENGTH;
          
          if(size)
          {
            DS_SYSTEM_HEAP_MEM_ALLOC(buf, size,char*);
          }
          if(buf != NULL)
          {
            memset(buf, 0, size);
            memscpy(buf, size, offset_ptr, size);            
          }
          else if((tlv != DS_PROFILE_APN_NAME_TLV) && (tlv != DS_PROFILE_AUTH_USERNAME_TLV) &&
                  (tlv != DS_PROFILE_AUTH_PASSWORD_TLV))
          {
            ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
            goto EXIT;
          }

          switch(tlv)
          {
            case DS_PROFILE_APN_NAME_TLV:
              memset(temp_profile->context.apn, 0,
                                   DS_PROFILE_APN_STRING_LEN+1);
              if(buf != NULL)
              {
                memscpy(temp_profile->context.apn, DS_PROFILE_APN_STRING_LEN,
			                        buf, size);
              }
              mask |= TLV_TO_MASK(DS_PROFILE_APN_NAME_TLV);
              break;

            case DS_PROFILE_TYPE_TLV:
              memscpy(&(temp_profile->context.pdp_type), sizeof(ds_profile_pdp_type_e),
                  buf,sizeof(ds_profile_pdp_type_e));
              mask |= TLV_TO_MASK( DS_PROFILE_TYPE_TLV);
              break;

            case DS_PROFILE_CONTEXT_H_COMP_TLV:
              memscpy(&(temp_profile->context.h_comp),sizeof(ds_profile_data_comp_e_type),
                  buf, sizeof(ds_profile_data_comp_e_type));
              mask |= TLV_TO_MASK( DS_PROFILE_CONTEXT_H_COMP_TLV);
              break;

            case DS_PROFILE_CONTEXT_D_COMP_TLV:
              memscpy(&(temp_profile->context.d_comp), sizeof(ds_profile_data_comp_e_type),
                  buf, sizeof(ds_profile_data_comp_e_type));
              mask |= TLV_TO_MASK(DS_PROFILE_CONTEXT_D_COMP_TLV);
              break;

            case DS_PROFILE_DNS_ADDR_V4_PRIMARY_TLV:
              memscpy(&(temp_profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4),
                  sizeof(uint32), buf, sizeof(uint32));
              mask |= TLV_TO_MASK( DS_PROFILE_DNS_ADDR_V4_PRIMARY_TLV);
              break;

            case DS_PROFILE_DNS_ADDR_V4_SECONDARY_TLV:
              memscpy(&(temp_profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4),
                  sizeof(uint32), buf, sizeof(uint32));
              mask |= TLV_TO_MASK(DS_PROFILE_DNS_ADDR_V4_SECONDARY_TLV);
              break;

            case DS_PROFILE_AUTH_USERNAME_TLV:
              memset(temp_profile->auth.username, 0,
                                   DS_PROFILE_MAX_AUTH_STRING_LEN+1);
              if(buf != NULL)
              {
                memscpy(temp_profile->auth.username, DS_PROFILE_MAX_AUTH_STRING_LEN,
                                                     buf, size);
                mask |= TLV_TO_MASK( DS_PROFILE_AUTH_USERNAME_TLV);
              }
              break;

            case DS_PROFILE_AUTH_PASSWORD_TLV:
              memset(temp_profile->auth.password, 0,
                                   DS_PROFILE_MAX_AUTH_STRING_LEN+1);
              if(buf != NULL)
              {
                memscpy(temp_profile->auth.password, DS_PROFILE_MAX_AUTH_STRING_LEN,
                                                     buf, size);
                mask |= TLV_TO_MASK(DS_PROFILE_AUTH_PASSWORD_TLV);
              }
              break;

            case DS_PROFILE_AUTH_TYPE_TLV:
              memscpy(&(temp_profile->auth.auth_type), sizeof(ds_profile_auth_enum_type),
                  buf, sizeof(ds_profile_auth_enum_type));
              mask |= TLV_TO_MASK(DS_PROFILE_AUTH_TYPE_TLV);
              break;

            case DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV:
              mask |= TLV_TO_MASK(DS_PROFILE_DNS_ADDR_V6_PRIMARY_TLV);
              switch (temp_profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.type)
              {
                case DS_PROFILE_IPV6_8:
                  memscpy(&(temp_profile->dns_addr.primary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr8), (sizeof(uint8)*16),buf,
                      (sizeof(uint8)*16));
                  break;

                case DS_PROFILE_IPV6_16:
                  memscpy(&(temp_profile->dns_addr.primary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr16), (sizeof(uint16)*8),buf,
                      (sizeof(uint16)*8));
                  break;

                case DS_PROFILE_IPV6_32:
                  memscpy(&(temp_profile->dns_addr.primary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr32), (sizeof(uint32)*4),buf,
                      (sizeof(uint32)*4));
                  break;

                case DS_PROFILE_IPV6_64:
                  memscpy(&(temp_profile->dns_addr.primary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr64), (sizeof(uint64)*2),buf,
                      (sizeof(uint64)*2));
                  break;

                default:
                  break;
              }
              break;

            case DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV:
              mask |= (1 << DS_PROFILE_DNS_ADDR_V6_SECONDARY_TLV);
              switch (temp_profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.type)
              {
                case DS_PROFILE_IPV6_8:
                  memscpy(&(temp_profile->dns_addr.secondary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr8),(sizeof(uint8)*16),buf,
                      (sizeof(uint8)*16));
                  break;

                case DS_PROFILE_IPV6_16:
                  memscpy(&(temp_profile->dns_addr.secondary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr16),(sizeof(uint16)*8),buf,
                      (sizeof(uint16)*8));
                  break;

                case DS_PROFILE_IPV6_32:
                  memscpy(&(temp_profile->dns_addr.secondary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr32),(sizeof(uint32)*4),buf,
                      (sizeof(uint32)*4));
                  break;

                case DS_PROFILE_IPV6_64:
                  memscpy(&(temp_profile->dns_addr.secondary_dns_addr.pdp_addr.\
                        pdp_addr_ipv6.in6_u.u6_addr64),(sizeof(uint64)*2),buf,
                      (sizeof(uint64)*2));
                  break;

                default:
                  break;
              }
              break;

            case DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG_TLV:
              memscpy(&(temp_profile->request_pcscf_address_flag),
                  sizeof(boolean), buf, sizeof(
                    boolean));
              mask |= TLV_TO_MASK(DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG_TLV);
              break;

            case DS_PROFILE_ACCESS_CTRL_FLAG_TLV:
              memscpy(&(temp_profile->context.access_ctrl_flag),
                  sizeof(ds_profile_access_control_e_type), buf,
                  sizeof(ds_profile_access_control_e_type));
              mask |= TLV_TO_MASK( DS_PROFILE_ACCESS_CTRL_FLAG_TLV);
              break;

            case DS_PROFILE_IM_CN_FLAG_TLV:
              memscpy(&(temp_profile->im_cn_flag),sizeof(boolean), buf,
                  sizeof(boolean));
              mask |= TLV_TO_MASK(DS_PROFILE_IM_CN_FLAG_TLV);
              break;

            case DS_PROFILE_APN_DISABLE_FLAG_TLV:
              memscpy(&(temp_profile->apn_disable_flag),
                  sizeof(boolean),buf,sizeof(boolean));
              mask |= TLV_TO_MASK(DS_PROFILE_APN_DISABLE_FLAG_TLV);
              break;

            case DS_PROFILE_PDN_INACTIVITY_TIMEOUT_TLV:
              memscpy(&(temp_profile->inactivity_timer_val),
                  sizeof(uint32),buf, sizeof(uint32));
              mask |= TLV_TO_MASK( DS_PROFILE_PDN_INACTIVITY_TIMEOUT_TLV);
              break;

            case DS_PROFILE_APN_CLASS_TLV:
              memscpy(&(temp_profile->apn_class), sizeof(uint8), buf,
                  sizeof(uint8));
              mask |= TLV_TO_MASK( DS_PROFILE_APN_CLASS_TLV);
              break;

            case DS_PROFILE_APN_BEARER_TLV:
              memscpy(&(temp_profile->apn_bearer), sizeof(uint8),buf,
                  sizeof(uint8));
              mask |= TLV_TO_MASK(DS_PROFILE_APN_BEARER_TLV);
              break;

            case DS_PROFILE_OPERATOR_RESERVED_PCO_TLV:
              memscpy(&(temp_profile->operator_reserved_pco),
                  sizeof(uint16),buf, sizeof(uint16));
              mask |= TLV_TO_MASK(DS_PROFILE_OPERATOR_RESERVED_PCO_TLV);
              break;

            case DS_PROFILE_MCC_TLV:
              memscpy(&(temp_profile->mcc), sizeof(uint16),
                  buf, sizeof(uint16));
              mask |= TLV_TO_MASK(DS_PROFILE_MCC_TLV);
              break;

            case DS_PROFILE_MNC_TLV:
              memscpy(&(temp_profile->mnc), sizeof(ds_profile_mnc_type),
                  buf, sizeof(ds_profile_mnc_type));
              mask |= TLV_TO_MASK(DS_PROFILE_MNC_TLV);
              break;

            case DS_PROFILE_MAX_PDN_CONN_PER_BLOCK_TLV:
              memscpy(&(temp_profile->max_pdn_conn_per_blk), sizeof(uint16),
                  buf, sizeof(uint16));
              mask |= TLV_TO_MASK( DS_PROFILE_MAX_PDN_CONN_PER_BLOCK_TLV);
              break;

            case DS_PROFILE_MAX_PDN_CONN_TIMER_TLV:
              memscpy(&(temp_profile->max_pdn_conn_time),sizeof(uint16),
                  buf, sizeof(uint16));
              mask |= TLV_TO_MASK(DS_PROFILE_MAX_PDN_CONN_TIMER_TLV);
              break;

            case DS_PROFILE_PDN_REQ_WAIT_TIMER_TLV:
              memscpy(&(temp_profile->pdn_req_wait_time),sizeof(uint16),
                  buf, sizeof(uint16));
              mask |= TLV_TO_MASK(DS_PROFILE_PDN_REQ_WAIT_TIMER_TLV);
              break;

            case DS_PROFILE_ROAMING_DISALLOWED_TLV:
              memscpy(&(temp_profile->roaming_disallowed),sizeof(boolean),
                  buf, sizeof(boolean));
              mask |= TLV_TO_MASK( DS_PROFILE_ROAMING_DISALLOWED_TLV);
              break;

            case DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE_TLV:
              memscpy(&(temp_profile->context.lte_roaming_pdp_type),
                  sizeof(ds_profile_pdp_type_e),buf, sizeof(ds_profile_pdp_type_e));
              mask |= TLV_TO_MASK( DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE_TLV);
              break;

            case DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE_TLV:
              memscpy(&(temp_profile->context.umts_roaming_pdp_type),
                  sizeof(ds_profile_pdp_type_e),buf, sizeof(ds_profile_pdp_type_e));
              mask |= TLV_TO_MASK( DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE_TLV);
              break;

            case DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE_TLV:
              memscpy(&(temp_profile->context.override_home_pdp_type),
                  sizeof(ds_profile_pdp_type_e),buf, sizeof(ds_profile_pdp_type_e));
              mask |= TLV_TO_MASK( DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE_TLV);
              break;

            case DS_PROFILE_PARAM_ATTACH_WO_PDN_TLV:
              memscpy(&(temp_profile->attach_wo_pdn),sizeof(boolean),
                  buf, sizeof(boolean));
              mask |= TLV_TO_MASK(DS_PROFILE_PARAM_ATTACH_WO_PDN_TLV);
              break;

            case DS_PROFILE_CLAT_ENABLED_TLV:
              memscpy(&(temp_profile->clat_enabled),sizeof(boolean),
                  buf, sizeof(boolean));
              mask |= TLV_TO_MASK(DS_PROFILE_CLAT_ENABLED_TLV);
              break;

            case DS_PROFILE_PDP_ADDR_TLV:
              memscpy(&(temp_profile->pdp_addr),sizeof(ds_profile_addr_type),
                  buf, sizeof(ds_profile_addr_type));
              mask |= TLV_TO_MASK( DS_PROFILE_PDP_ADDR_TLV);
              break;

            case DS_PROFILE_PDN_DISCON_WAIT_TIME_TLV:
              memscpy(&(temp_profile->pdn_discon_wait_time),sizeof(uint8),
                  buf, sizeof(uint8));
              mask |= TLV_TO_MASK( DS_PROFILE_PDN_DISCON_WAIT_TIME_TLV);
              break;

            case DS_PROFILE_FAILURE_TIMERS_TLV:
              memscpy(&(temp_profile->failure_timers),
                  sizeof(temp_profile->failure_timers),buf, 
				  sizeof(temp_profile->failure_timers));
              mask |= TLV_TO_MASK(DS_PROFILE_FAILURE_TIMERS_TLV);
              break;

            case DS_PROFILE_MSISDN_FLAG_TLV:
              memscpy(&(temp_profile->msisdn_flag),sizeof(boolean),
                  buf, sizeof(boolean));
              mask |= TLV_TO_MASK(DS_PROFILE_MSISDN_FLAG_TLV);
              break;

            default:
              break;
          }
          read_tlvs++;
          read_size += (size + (TLV_SIZE + TLV_LENGTH));
          offset_ptr += size;
          if(buf != NULL)
          {
            DS_SYSTEM_HEAP_MEM_FREE(buf);
            buf = NULL;
          }
        }

      }
      temp_profile->mask = mask;
      memscpy(ds_profile_list[profile_number], sizeof(ds_profile_type_s), temp_profile,
                                 sizeof(ds_profile_type_s));
      mask = 0;
    }
    profile_number++;
  }

EXIT:
  if( ret_val != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"read fails with error %d", ret_val);
    if(ds_profile_list[0] != NULL)
    {
      DS_SYSTEM_HEAP_MEM_FREE(ds_profile_list[0]);
      ds_profile_list[0] = NULL;
    }
    return ret_val;
  }
  if(temp_profile != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(temp_profile);
    temp_profile = NULL;
  }
  
  if(heapMemForFile != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(heapMemForFile);
    heapMemForFile = NULL;
  }
  return DS_PROFILE_RESULT_SUCCESS;  
}

/*===========================================================================
FUNCTION DSI_PROFILE_CREATE

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
ds_profile_status_etype ds_profile_create
( 
 ds_profile_ident_info      profile_ident,
 uint64                     mask,
 ds_profile_type_s          *pay_load
)
{
  return ds_profile_create_ex(profile_ident, mask, pay_load, IGNORE_PROFILE);
}


/*===========================================================================
FUNCTION DS_PROFILE_EFS_WRITE

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
ds_profile_status_etype ds_profile_efs_write
(
 void           *payload,
 int8            profile_number
)
{
  int efs_flag     =
                     MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL;
  uint32 size_to_write                = 0;
  mcfg_fs_status_e_type  write_status = 0;
  char file_name[MAX_PATH_LEN]        = {'\0'};
  int ret                             = 0;
/*............................................................................*/

  if (-1 == efs_flag)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Open(): invalid efs_flag %d", efs_flag);
    return -1;
  }

  ret = snprintf(file_name, MAX_PATH_LEN, "%s%s%u", PROFILE_DIR,
      PROFILE_FILENAME_BASE, profile_number);
  if (!ret)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"BuildFileName(): error in snprintf");
    return -1;
  }
  size_to_write = ((ds_profile_header*)payload)->size + sizeof(ds_profile_header);
  write_status = mcfg_fs_write(file_name, payload,size_to_write,
                                 MCFG_FS_O_CREAT|MCFG_FS_O_AUTODIR,
                                 MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, 0);
  if (write_status != MCFG_FS_STATUS_OK)
  {
    DS_MSG1(MSG_LEGACY_ERROR,
                    "Unable to write EFS item, error:%d ", write_status);
    return DS_PROFILE_RESULT_ERR_FILE_ACCESS;
  }
  return DS_PROFILE_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION DS_PROFILE_SETTING_FROM_PROFILE_ID

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
ds_profile_status_etype ds_profile_setting_from_profile_id
(
 int8               profile_id,
 ds_profile_type_s *profile
)
{
  if(!ds_profile_number_is_valid(profile_id))
  {
    return DS_PROFILE_RESULT_ERR_INVAL;
  }
  
  if(profile != NULL && (ds_profile_list[(profile_id -1)] != NULL))
  {
    memscpy(profile, sizeof(ds_profile_type_s), ds_profile_list[(profile_id -1)],
        sizeof(ds_profile_type_s));
    return DS_PROFILE_RESULT_SUCCESS;
  }
  
  return DS_PROFILE_RESULT_ERR_INVAL;
}

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
)
{
  if(profile_number > 0 && profile_number <= DS_PROFILE_MAX_PROFILE_NUM)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION DS_PROFILE_READ_PROFILE_NUM_FOR_EMBEDDED_AND_TETHERED_CALLS

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( embedded calls ).  The cached value is return if available;
  otherwise, the read is done from the EFS file.

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
 
RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  None
===========================================================================*/
void ds_profile_read_profile_num_for_embedded_and_tethered_calls()
{
  byte profile_number = 0;
  mcfg_fs_status_e_type efs_result = 0;
/*............................................................................*/  
       
  efs_result = mcfg_fs_read( ds_profile_embedded_call_file_name,
                            (void *)&profile_number,
                             sizeof(byte),
                             MCFG_FS_TYPE_EFS,0);
  

  if (( efs_result == MCFG_FS_STATUS_OK) && (profile_number < DS_PROFILE_MAX_PROFILE_NUM))
  {
    cached_embedded_call_profile_number = profile_number;
  }
  
  efs_result = mcfg_fs_read( ds_profile_tethered_call_file_name,
                            (void *)&profile_number,
                             sizeof(byte),
                             MCFG_FS_TYPE_EFS, 0 );
  
  if (( efs_result == MCFG_FS_STATUS_OK) && (profile_number < DS_PROFILE_MAX_PROFILE_NUM))
  {
    cached_tethered_call_profile_number = profile_number;
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_NUM_FOR_EMBEDDED_CALLS

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( embedded calls ).  The cached value is return

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
 
RETURN VALUE 
   The cached embeded profile value 
 
SIDE EFFECTS 
  None
===========================================================================*/
int8 ds_profile_get_profile_number_for_embeded_calls()
{
  return cached_embedded_call_profile_number;
}

/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_NUM_FOR_TETHERED_CALLS

DESCRIPTION
  This function retrieves the profile number specififed for embedded
  data calls ( tethered calls ).  The cached value is return

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
 
RETURN VALUE 
   The cached embeded profile value 
 
SIDE EFFECTS 
  None
===========================================================================*/
int8 ds_profile_get_profile_number_for_tethered_calls()
{
  return cached_tethered_call_profile_number;
}

/*===========================================================================
FUNCTION DS_PROFILE_INIT 

DESCRIPTION
 Initializes the DS profile . On the modem, this function is 
 called only once at initialization. This function reads the profiles from EFS path
 if present if not creates the default profile.

DEPENDENCIES 

RETURN VALUE 
  DS_PROFILE_RESULT_SUCCESS            -- Operation succeeded.\n
  DS_PROFILE_RESULT_ERR_INVAL           -- General failure.

SIDE EFFECTS 
None
===========================================================================*/
ds_profile_status_etype ds_profile_init( void )
{

  ds_profile_status_etype status = DS_PROFILE_RESULT_ERR_INVAL;
/*............................................................................*/

  if((status = ds_profile_efs_read()) != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"read fails with error %d in init", status);
    return DS_PROFILE_RESULT_ERR_INVAL;
  }

  ds_profile_read_profile_num_for_embedded_and_tethered_calls();
  return DS_PROFILE_RESULT_SUCCESS;
  
}

/*===========================================================================

FUNCTION DS_PROFILE_REG_PARAM_CHANGE_HANDLER

DESCRIPTION
  This function is to register the call back function in case of profile
  parameter change

DEPENDENCIES
  None.

RETURN VALUE
 TRUE - SUCCESS
 FALSE - FAILURE

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_profile_reg_param_change_handler
(
  ds_profile_client_etype client,             /* client id to reg*/
  ds_profile_change_handler_type cb_fun   /* callback function pointer */
)
{
  ds_profile_client_etype client_id = CLIENT_MIN;
  if (client >= CLIENT_MIN  && client < CLIENT_MAX)
  {
    DS_ASSERT( cb_fun != NULL );
    if( ds_profile_change_handler[client_id] == NULL)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_profile_change_handler[client_id], 
                            sizeof(ds_profile_change_handler_type),
                            ds_profile_change_handler_type*);

      if(ds_profile_change_handler[client_id] != NULL)
      {
        *ds_profile_change_handler[client_id] = cb_fun;
        return TRUE;
      }
    }
    else
    {
       *ds_profile_change_handler[client_id] = cb_fun;
       return TRUE;
    }
  }
  return FALSE;
}


/*===========================================================================

FUNCTION DS_PROFILE_DEREG_PARAM_CHANGE_HANDLER()

DESCRIPTION
  This function is to deregister the call back function

DEPENDENCIES
  None.

RETURN VALUE
None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_profile_dereg_param_change_handler
(
ds_profile_client_etype client
)
{
   if( ds_profile_change_handler[client] != NULL)
   {
     DS_SYSTEM_HEAP_MEM_FREE(ds_profile_change_handler[client]);
     ds_profile_change_handler[client] = NULL;
   }
}

/*===========================================================================

FUNCTION DS_PROFILE_GET_PROFILE_ID()

DESCRIPTION
  This function returns the profile number which is internal to DS

DEPENDENCIES
  None.

RETURN VALUE
  profile index

SIDE EFFECTS
  None.

===========================================================================*/
int8 ds_profile_get_profile_id
( 
  ds_profile_ident_info      profile_ident
 )
{
  int8 profile_index = -1;
/*............................................................................*/

  if ((profile_index = ds_profile_search(profile_ident))
                        != INVALID_PROFILE_NUMBER)
  {
    return PROFILE_INDEX_TO_NUMBER(profile_index);
  }
  return INVALID_PROFILE_NUMBER;
} 
/*===========================================================================
FUNCTION DS_PROFILE_GET_DEFAULT_PROFILE_SETTING

DESCRIPTION
  This function returns the default profile parameters

PARAMETERS
  @param[out] profile      profile pointer to store the profile content
  Memory for this pointer needs to be maintained by clients

DEPENDENCIES 

RETURN VALUE 
  DS_PROFILE_RESULT_SUCCESS
  DS_PROFILE_RESULT_ERR_INVAL

SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype ds_profile_get_default_profile_setting
(
 ds_profile_type_s *profile
)
{
  int8 profile_number = INVALID_PROFILE_NUMBER;
/*............................................................................*/
  profile_number = ds_profile_get_profile_number_for_embeded_calls();
  if(profile_number >= DEFAULT_PROFILE_NUMBER && 
      profile_number < DS_PROFILE_MAX_PROFILE_NUM)
  {
    memscpy(profile, sizeof(ds_profile_type_s),
            ds_profile_list[PROFILE_INDEX_FROM_NUMBER(profile_number)],
              sizeof(ds_profile_type_s));
    return DS_PROFILE_RESULT_SUCCESS;
  }
  return DS_PROFILE_RESULT_ERR_INVAL;
}

/*===========================================================================
FUNCTION DS_PROFILE_SEARCH

DESCRIPTION
  This function returns profile which matched the profile ident

PARAMETERS
  @param[in] Profile ident to identify the profile
  @param[op] Profile content matches with the ident

DEPENDENCIES 

RETURN VALUE 
  profile index from the array of profiles

SIDE EFFECT
  None
===========================================================================*/
int8 ds_profile_search
(
  ds_profile_ident_info profile_ident
)
{
  return ds_profile_search_ex(profile_ident, IGNORE_PROFILE);
}

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
)
{
  int8    indexes = 0;
  boolean is_profile_present = FALSE;
/*................................................................................*/
  if(profile_ident.pdp_type >= DS_PROFILE_PDP_MAX)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "profile_iden_pdp_type is invalid");
    return INVALID_PROFILE_NUMBER;
  }

  while(indexes < DS_PROFILE_MAX_PROFILE_NUM)
  {
    if(ds_profile_list[indexes] != NULL )
    {
      if ((strlen(ds_profile_list[indexes]->context.apn) == 
           strlen(profile_ident.apn)) && 
           (strncasecmp((ds_profile_list[indexes]->context.apn) ,
           (profile_ident.apn), strlen(profile_ident.apn)) == 0)
            && indexes != profile_index)
      {
        switch(profile_ident.pdp_type)
        {
          case DS_PROFILE_PDP_IPV4:
           if((ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV4)||
               ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV4V6 ||
              (ds_profile_list[indexes]->clat_enabled && 
               ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV6))
          {
             if(ds_profile_list[indexes]->clat_enabled && 
               ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV6){
               DS_MSG0(MSG_LEGACY_HIGH, "V4 type call allowed on V6 type profile as CLAT enabled");
             }
               
             is_profile_present = TRUE;
          }
           break;

          case DS_PROFILE_PDP_IPV6:
            if((ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV6)||
                ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV4V6)
            {
              is_profile_present = TRUE;
            }
            break;

          case DS_PROFILE_PDP_IPV4V6:
            if(ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV4V6||
               ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV4||
               ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_IPV6)
            {
              is_profile_present = TRUE;
            }
            break;

          case DS_PROFILE_PDP_NON_IP:
            if(ds_profile_list[indexes]->context.pdp_type == DS_PROFILE_PDP_NON_IP)
            {
              is_profile_present = TRUE;
            }
            break;

          default:
            break;
        }
        if(is_profile_present && indexes != profile_index)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "Profile found at index %d", indexes);
          return indexes;
        }
      }
    }
    indexes++;
  }

  DS_MSG0(MSG_LEGACY_HIGH, "No profile found ");
  return INVALID_PROFILE_NUMBER;
}

/*===========================================================================
FUNCTION DS_PROFILE_GET_PROFILE_SETTING

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
)
{
  uint8                  profiles = 0;
  int                    loop = 0;
/*................................................................................*/
  for (loop =0; loop < DS_PROFILE_MAX_PROFILE_NUM; loop++)
  {
    if(ds_profile_list[loop] != NULL)
    {
      profile_list[profiles].pdp_type = ds_profile_list[loop]->context.pdp_type;
      if(ds_profile_list[loop]->context.apn[0] != '\0')
      {
        memscpy(profile_list[profiles].apn, DS_PROFILE_APN_STRING_LEN+1, 
               ds_profile_list[loop]->context.apn, DS_PROFILE_APN_STRING_LEN+1);
      }
      profiles++;
    }
  }

  return profiles;
}

/*===========================================================================
FUNCTION DS_PROFILE_CACHE_GET_VALID_PROFILE_NUMBERS

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
)
{
  uint8                  profiles = 0;
  int                    loop = 0;
/*................................................................................*/
  if(!profile_num_list)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid Buffer passed");
    return profiles;
  }
  
  for (loop = 0; loop < DS_PROFILE_MAX_PROFILE_NUM; loop++)
  {
    if(ds_profile_list[loop] != NULL)
    {
      profile_num_list[profiles] = PROFILE_INDEX_TO_NUMBER(loop);
      profiles++;
    }
  }
  DS_MSG1(MSG_LEGACY_HIGH,"Number of profiles %d", profiles);
  return profiles;
}

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
)
{
  ds_profile_ident_info       profile_ident;
  uint8                       profile_index = INVALID_PROFILE_INDEX;
  ds_profile_status_etype     ret_val = DS_PROFILE_RESULT_SUCCESS;
/*..........................................................................*/
  if(!ds_profile_number_is_valid(profile_number))
  {
    return DS_PROFILE_RESULT_ERR_INVAL_IDENT;
  }

  profile_index = PROFILE_INDEX_FROM_NUMBER(profile_number);
  if(profile_number == ds_profile_get_profile_number_for_tethered_calls() ||
	 profile_number == ds_profile_get_profile_number_for_embeded_calls() ||
	 profile_number == DEFAULT_PROFILE_NUMBER)
  {	  
    DS_ERROR_LOG_0("Default profile can't be deleted");
    return DS_PROFILE_RESULT_FAIL;
  }

  if(ds_profile_list[profile_index] == NULL)
  {
    DS_ERROR_LOG_0("NO profile found");
    return DS_PROFILE_RESULT_ERR_INVAL_IDENT;
  }
  
  memset(profile_ident.apn, 0, DS_PROFILE_APN_STRING_LEN+1);
  memscpy(profile_ident.apn, DS_PROFILE_APN_STRING_LEN,
          ds_profile_list[profile_index]->context.apn,
          strlen(ds_profile_list[profile_index]->context.apn));
  profile_ident.pdp_type = ds_profile_list[profile_index]->context.pdp_type;

  ret_val = ds_profile_delete(profile_ident);

  DS_MSG1(MSG_LEGACY_HIGH, "delete_ex %d", ret_val);
  return ret_val;
}


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
)
{
  ds_profile_status_etype     ret_val = DS_PROFILE_RESULT_SUCCESS;
  int8                        profile_index  = num;
  boolean                     is_create = FALSE;
/*...........................................................................*/

  if(profile_ident.pdp_type >= DS_PROFILE_PDP_MAX)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "profile_iden_pdp_type is invalid");
    return DS_PROFILE_RESULT_ERR_LEN_INVALID;
  }
  
  if( strlen(profile_ident.apn) > DS_PROFILE_APN_STRING_LEN)
  {
    return DS_PROFILE_RESULT_ERR_LEN_INVALID;
  }
  
  if (num == IGNORE_PROFILE)
  {
    if (ds_profile_search(profile_ident) != INVALID_PROFILE_NUMBER)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Profile already present with same apn name and"
                             "ip type");
      ret_val = DS_PROFILE_ALREADY_PRESENT;
      goto EXIT;
    }

    if((profile_index = ds_profile_get_free_index()) != INVALID_PROFILE_INDEX)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_profile_list[profile_index], 
                               sizeof(ds_profile_type_s),ds_profile_type_s*);
      is_create = TRUE;
      if (ds_profile_list[profile_index] == NULL)
      {
        ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
        goto EXIT;
      }
    }
  }
  else if( ds_profile_number_is_valid(num) )
  {
    profile_index = PROFILE_INDEX_FROM_NUMBER(num);

    if (ds_profile_list[profile_index] == NULL)
    {
      memscpy(profile_ident.apn,
                      DS_PROFILE_APN_STRING_LEN,
                      pay_load->context.apn,
                      strlen(pay_load->context.apn)+1);
      profile_ident.pdp_type = pay_load->context.pdp_type;
      if (ds_profile_search(profile_ident) != INVALID_PROFILE_NUMBER)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Profile already present with same apn name and"
                                 "ip type");
        ret_val = DS_PROFILE_ALREADY_PRESENT;
        goto EXIT;
      } 
      DS_SYSTEM_HEAP_MEM_ALLOC( ds_profile_list[profile_index],
                  sizeof(ds_profile_type_s),ds_profile_type_s*);
      is_create = TRUE;
      if (ds_profile_list[profile_index] == NULL)
      {
        ret_val = DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY;
        goto EXIT;
      }
    }
    else
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Profile already present modifying");

      memscpy(profile_ident.apn,
                 DS_PROFILE_APN_STRING_LEN,
                 ds_profile_list[profile_index]->context.apn,
                 strlen(ds_profile_list[profile_index]->context.apn)+1);
      profile_ident.pdp_type = ds_profile_list[profile_index]->context.pdp_type;
      DS_MSG1(MSG_LEGACY_ERROR,"Profile already present modifying pdp %d",profile_ident.pdp_type );
      ret_val = ds_profile_modify_internal(profile_ident, mask, pay_load, FALSE);
      goto EXIT;
    }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid profile number ");
    ret_val = DS_PROFILE_RESULT_ERR_INVAL;
    goto EXIT;
  }

  if(ds_profile_reset_to_default(profile_index) != DS_PROFILE_RESULT_SUCCESS)
  {
    ret_val = DS_PROFILE_RESULT_FAIL;
    goto EXIT;
  }
  if(strlen(pay_load->context.apn) > DS_PROFILE_APN_STRING_LEN
      || pay_load->context.pdp_type >= DS_PROFILE_PDP_MAX)
  {
    ret_val = DS_PROFILE_RESULT_ERR_INVAL;
    goto EXIT;
  }
  memscpy(ds_profile_list[profile_index]->context.apn,
          DS_PROFILE_APN_STRING_LEN+1, pay_load->context.apn,
          DS_PROFILE_APN_STRING_LEN+1);

  if(mask & DS_PROFILE_TYPE)
  {
    ds_profile_list[profile_index]->context.pdp_type = pay_load->context.pdp_type;
  }
  if(ds_profile_modify_internal(profile_ident, mask, pay_load, TRUE) != 
      DS_PROFILE_RESULT_SUCCESS)
  {
    ret_val = DS_PROFILE_RESULT_FAIL;
    goto EXIT;
  }

EXIT:
  if (ret_val != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_MSG1(MSG_LEGACY_ERROR,
               "ds_profile_create_failed with %d", ret_val);
    if ( profile_index > DEFAULT_PROFILE_INDEX &&
         profile_index < DS_PROFILE_MAX_PROFILE_NUM &&
         ds_profile_list[profile_index] != NULL && is_create)
    {
      DS_SYSTEM_HEAP_MEM_FREE(ds_profile_list[profile_index]);
    }
  }
  return ret_val;
}

/*===========================================================================
FUNCTION DS_PROFILE_GET_PARAM_EX

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
)
{
  int8 profile_index = ds_profile_search(profile_ident);
/*............................................................................*/
  if(profile_index != INVALID_PROFILE_NUMBER)
  {
    *profile_num = PROFILE_INDEX_TO_NUMBER(profile_index);
    return ds_profile_get_param(profile_ident, payload);
  }
  
  DS_MSG0(MSG_LEGACY_HIGH, "No profile found ");
  return INVALID_PROFILE_NUMBER;
}
