#ifndef DS_PROFILE_H
#define DS_PROFILE_H

/*===========================================================================
                      DS_PROFILE.H
DESCRIPTION
This file exposes 3GPP ds profile to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    PK      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"


/**< Maximum length of the APN string. */
#define  DS_PROFILE_APN_STRING_LEN    100

/**< Maximum length of the username/password. */
#define  DS_PROFILE_MAX_AUTH_STRING_LEN  127 

#define DS_PROFILE_ALWAYS_SHARE_APN "always_share"

#define DS_3GPP_MAX_FAILURE_TIMERS              10
#define DS_3GPP_MIN_FAILURE_TIMERS              1 

/** Status return values from functions*/ 
typedef enum   
{
  DS_PROFILE_RESULT_SUCCESS    =       0,
  /**< Successful operation. */
  DS_PROFILE_RESULT_FAIL,                  
  /**< General failure in the library. */
  DS_PROFILE_RESULT_ERR_INVAL_IDENT,
  /**< Invalid identifier. */
  DS_PROFILE_RESULT_ERR_INVAL,
  /**< Other invalid argument. */
  DS_PROFILE_RESULT_ERR_OUT_OF_PROFILES,
  /**< No profile available while create */
  DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY,
  /**< Out of memory */
  DS_PROFILE_RESULT_ERR_FILE_ACCESS,
  /**< Error accessing the embedded file system. */
  DS_PROFILE_RESULT_ERR_EOF,            
  /**< Error end of file. */
  DS_PROFILE_ALREADY_PRESENT,
  /* profile with same apn name and ip type is present*/
  DS_PROFILE_RESULT_ERR_LEN_INVALID,    
  /**< Buffer size length is invalid.\n
       For get_param, the buffer size cannot be less than the maximum.\n
       For set_param, the buffer size cannot be greater than the maximum. */

} ds_profile_status_etype;

/** client enum type to reg profile change cb */
typedef enum
{
  CLIENT_MIN = 0,
  CLIENT_IMS = CLIENT_MIN,
  CLIENT_MAX,
} ds_profile_client_etype;


/** Enumeration type for IP Version type. 
 */
typedef enum
{
  DS_PROFILE_IP_V4   = 1,       /**< IPv4 version. */
  DS_PROFILE_IP_V6   = 2,       /**< IPv6 version. */
  DS_PROFILE_IP_V4V6 = 3,     /**< IPv4v6 version. */
  DS_PROFILE_NONIP   = 4       /**< NON_IP version. */
} ds_profile_ip_version_enum_type;

/** Enumeration type used to discriminate IPv6 addresses.\ This enumeration
  is included only if FEATURE_DATA_PS_IPV6 is TRUE.
 */
typedef enum
{
  DS_PROFILE_IPV6_8 = 0x00, /**< 8-bit address. */
  DS_PROFILE_IPV6_16,       /**< 16-bit address. */
  DS_PROFILE_IPV6_32,       /**< 32-bit address. */
  DS_PROFILE_IPV6_64        /**< 64-bit address. */
} ds_profile_ipv6_etype;

/** identifiers for profile parameters mask
 */

#define DS_PROFILE_APN_NAME                               0x1
/** APN name */
#define DS_PROFILE_TYPE                                   0x2
/** Home PDP type */
#define DS_PROFILE_CONTEXT_H_COMP                         0x4
/** PDP header compression support */
#define DS_PROFILE_CONTEXT_D_COMP                         0x8
/** PDP data compression support */
#define DS_PROFILE_DNS_ADDR_V4_PRIMARY                    0x10
/** Primary DNS IPV6 address */
#define DS_PROFILE_DNS_ADDR_V4_SECONDARY                  0x20
/** Secondary DNS IPV4 address */
#define DS_PROFILE_AUTH_USERNAME                          0x40 
/** User name */
#define DS_PROFILE_AUTH_PASSWORD                          0x80  
/** password */
#define DS_PROFILE_AUTH_TYPE                              0x100
/** Aunthentication Preferencre */
#define DS_PROFILE_DNS_ADDR_V6_PRIMARY                    0x200 
/** Priimary DNS IPV6 address */
#define DS_PROFILE_DNS_ADDR_V6_SECONDARY                  0x400
/** Secondary DNS IPV6 address */
#define DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG          0x800
/* PCFCH address using dhcp */
#define DS_PROFILE_ACCESS_CTRL_FLAG                       0x1000
/**Acess control flag */
#define DS_PROFILE_IM_CN_FLAG                             0x2000
/** im_cn_flag */
#define DS_PROFILE_APN_DISABLE_FLAG                       0x4000
/** APN disable flag */
#define DS_PROFILE_PDN_INACTIVITY_TIMEOUT                 0x8000
/** PDN inactivity time out */
#define DS_PROFILE_APN_CLASS                              0x10000
/**APN calss */
#define DS_PROFILE_APN_BEARER                             0x20000
/** APN bearer */
#define DS_PROFILE_OPERATOR_RESERVED_PCO                  0x40000
/** OPERATOR RESERVED PCO */
#define DS_PROFILE_MCC                                    0x80000
/** MCC */ 
#define DS_PROFILE_MNC                                    0x100000
/** MNC */
#define DS_PROFILE_MAX_PDN_CONN_PER_BLOCK                 0x200000
/** Max PDN Connections per Time Block */
#define DS_PROFILE_PDN_DISCON_WAIT_TIME                   0x400000
/** Max pdn wait time */
#define DS_PROFILE_MAX_PDN_CONN_TIMER                     0x800000
/** Max PDN Connection Time */
#define DS_PROFILE_PDN_REQ_WAIT_TIMER                     0x1000000
/** PDN Request Wait Time */
#define DS_PROFILE_ROAMING_DISALLOWED                     0x2000000
/** Roaming disallowed*/
#define DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE             0x4000000
/** LTE Roaming PDP type*/ 
#define DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE            0x8000000
/** GSM Roaming PDP type*/
#define DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE           0x10000000
/** Override IP type of the default home pdp type*/
#define DS_PROFILE_PARAM_ATTACH_WO_PDN                    0x20000000
/** Attach without PDN*/
#define DS_PROFILE_CLAT_ENABLED                           0x40000000
/** Clat enabled*/
#define DS_PROFILE_PDP_ADDR                               0x80000000
/* Failure timer */
#define DS_PROFILE_FAILURE_TIMERS                         0x100000000
/** MSISDN Flag enabled*/
#define DS_PROFILE_MSISDN_FLAG                            0x200000000


/** Enumeration type for PDP type.\ The maximum value of 0xff is for 
  INTERNAL USE ONLY.
 */
typedef enum
{
  DS_PROFILE_IP=0x0,                    /**< PDP type IP. */
  DS_PROFILE_IPV4=0x0,                  /**< PDP type IPv4. */
  DS_PROFILE_PPP,                       /**< PDP type PPP. */
  DS_PROFILE_IPV6,                      /**< PDP type IPv6. */
  DS_PROFILE_IPV4V6,                    /**< PDP type IPv4v6. */
  DS_PROFILE_NON_IP,                    /**< PDP type NON IP. */
  DS_PROFILE_MAX
  
} ds_profile_type_e;

typedef enum
{
  DS_PROFILE_ROAMING_PDP_IPV4=0x0,                 /**< PDP type IPv4. */
  DS_PROFILE_ROAMING_PDP_IPV6,                     /**< PDP type IPv6. */
  DS_PROFILE_ROAMING_PDP_IPV4V6,                 /**< PDP type IPv4v6. */
  DS_PROFILE_ROAMING_PDP_MAX = 0xFF
} ds_profile_roaming_pdp_type_e;

typedef enum
{
  DS_PROFILE_PDP_IP=0x0,                    /**< PDP type IP. */
  DS_PROFILE_PDP_IPV4=0x0,                  /**< PDP type IPv4. */
  DS_PROFILE_PDP_PPP,                       /**< PDP type PPP. */
  DS_PROFILE_PDP_IPV6,                      /**< PDP type IPv6. */
  DS_PROFILE_PDP_IPV4V6,                    /**< PDP type IPv4v6. */
  DS_PROFILE_PDP_NON_IP,                    /**< PDP type NON IP. */
  DS_PROFILE_PDP_MAX
  
} ds_profile_pdp_type_e;

/** PDP access list control.
 */
typedef enum
{
  DSAT_ACCESS_CONTROL_NONE       = 0x0, /**< No access. */
  DSAT_ACCESS_CONTROL_REJECT     = 0x1, /**< Reject access. */
  DSAT_ACCESS_CONTROL_PERMISSION = 0x2  /**< Allow access. */
} ds_profile_access_control_e_type;

/** PDP header compression types.
 */
typedef PACKED enum 
{
  DS_PROFILE_HEADER_COMP_OFF = 0, /**< PDP header compression is off. */
  DS_PROFILE_HEADER_COMP_ON  = 1, /**< Manufacturer preferred compression. */
  DS_PROFILE_HEADER_COMP_RFC1144, /**< PDP header compression based on 
                                           RFC 1144.*/
  DS_PROFILE_HEADER_COMP_RFC2507, /**< PDP header compression based on 
                                          RFC 2507.*/
  DS_PROFILE_HEADER_COMP_RFC3095, /**< PDP header compression based on 
                                         RFC 3095.*/   
  DS_PROFILE_HEADER_COMP_MAX 
  
} PACKED_POST ds_profile_header_comp_e_type;

/** PDP data compression types defined in 3GPP TS 44.065 (sect 6.6.1.1.4).
 */
typedef PACKED enum 
{

  DS_PROFILE_DATA_COMP_OFF = 0,   /**< PDP data compression is off. */
  DS_PROFILE_DATA_COMP_ON  = 1,   /**< Manufacturer preferred compression. */
  DS_PROFILE_DATA_COMP_V42_BIS,   /**< V.42BIS data compression. */
  DS_PROFILE_DATA_COMP_V44,       /**< V.44 data compression. */  
  DS_PROFILE_DATA_COMP_MAX 
  
} PACKED_POST ds_profile_data_comp_e_type;

/** @brief Stores the PDP context parameters. 

  @sa ds_profile_get_pdp_context_info
 */
typedef PACKED struct PACKED_POST
{
  /**< PDP type (IP/PPP). */
  ds_profile_pdp_type_e              pdp_type;
  /** PDP Type in roaming LTE NW*/
  ds_profile_roaming_pdp_type_e      lte_roaming_pdp_type;
  /** PDP Type in roaming GSM NW*/
  ds_profile_roaming_pdp_type_e      umts_roaming_pdp_type;
  /**< Override Home PDP type. */
  ds_profile_roaming_pdp_type_e      override_home_pdp_type;
  /**< PDP header compression support. */
  ds_profile_header_comp_e_type        h_comp;
  /**< Data compression support -- v.42. */
  ds_profile_data_comp_e_type          d_comp;
  /**< Access control flag. */
  ds_profile_access_control_e_type     access_ctrl_flag;
  /**< APN string. */
  char                                 apn[DS_PROFILE_APN_STRING_LEN+1];

} ds_profile_context_type;

/** Enumeration type for Authentication type for a PDP context.\ The
  maximum value of 0xff is for INTERNAL USE ONLY.

  @note1 The numerical values of these enumerations must match those 
  defined in ps_iface_auth_alg_pref_bitmask_e_type.
 */
typedef enum
{
  DS_PROFILE_AUTH_NONE = 0x0,           /**< No authentication. */
  DS_PROFILE_AUTH_PAP  = 0x1,           /**< PAP authentication. */
  DS_PROFILE_AUTH_CHAP = 0x2,           /**< CHAP authentication. */
  DS_PROFILE_AUTH_CHAP_OR_PAP = 0x3,    /**< CHAP or PAP authentication. */
  DS_PROFILE_AUTH_MAX                   
} ds_profile_auth_enum_type;

/** @brief Stores the PDP authentication parameters.
 */
typedef PACKED struct PACKED_POST
{
  ds_profile_auth_enum_type  auth_type;           /**< Authentication type. */
  char      password[DS_PROFILE_MAX_AUTH_STRING_LEN+1]; /**< Password/secret 
                                                          string. */
  char      username[DS_PROFILE_MAX_AUTH_STRING_LEN+1]; /**< Username string. */
} ds_profile_auth_type;


typedef PACKED struct PACKED_POST
{
  uint16 mnc_digits;
  /**< mnc value */
  boolean mnc_includes_pcs_digit;
  /**< This field is used to interpret the length of the corresponding
    MNC reported Values:
    - TRUE -- MNC is a three-digit value; e.g., a reported value of 
    90 corresponds to an MNC value of 090

    - FALSE -- MNC is a two-digit value; e.g., a reported value of 
    90 corresponds to an MNC value of 90 */
} ds_profile_mnc_type;

/** @brief Structure for IPv6 addresses.

  This structure is included only if FEATURE_DATA_PS_IPV6 is TRUE. The
  structure uses the in6_u union.
 */
typedef PACKED struct PACKED_POST
{
  ds_profile_ipv6_etype type; /**< Type of IPv6 address. */
  PACKED union PACKED_POST
  {
    uint8   u6_addr8[16]; /**< IPv6 8-bit PDP address size. */
    /*~ CASE DS_UMTS_PDP_IPV6_8 ds_umts_pdp_addr_type_ipv6.in6_u.u6_addr8 */
    uint16  u6_addr16[8]; /**< IPv6 16-bit PDP address size. */
    /*~ CASE DS_UMTS_PDP_IPV6_16 ds_umts_pdp_addr_type_ipv6.in6_u.u6_addr16 */
    uint32  u6_addr32[4]; /**< IPv6 32-bit PDP address size. */
    /*~ CASE DS_UMTS_PDP_IPV6_32 ds_umts_pdp_addr_type_ipv6.in6_u.u6_addr32 */
    uint64  u6_addr64[2]; /**< IPv6 64-bit PDP address size. */
    /*~ CASE DS_UMTS_PDP_IPV6_64 ds_umts_pdp_addr_type_ipv6.in6_u.u6_addr64 */
    /*~ DEFAULT ds_umts_pdp_addr_type_ipv6.in6_u.u6_addr8 */
  } in6_u; /**< in6_u union. */ 
  /*~ FIELD ds_umts_pdp_addr_type_ipv6.in6_u 
    DISC ds_umts_pdp_addr_type_ipv6.type */
}ds_profile_addr_type_ipv6;

/** @brief Generic type to hold both IPv4 and IPv6 addresses. 
 */
typedef PACKED struct PACKED_POST
{
  ds_profile_ip_version_enum_type ip_vsn; /**< IP version. */
  PACKED struct PACKED_POST
  {
    uint32 pdp_addr_ipv4;                      /**< IPv4 PDP address type. */ 
    ds_profile_addr_type_ipv6 pdp_addr_ipv6;  /**< IPv6 PDP address type. */
  } pdp_addr; /**< PDP address. */
}ds_profile_addr_type;

/** @brief Stores the Domain Name Service (DNS) parameters for the PDP profile.
 */
typedef PACKED struct PACKED_POST
{  
  ds_profile_ip_version_enum_type ip_vsn;             /**< IP version. */
  ds_profile_addr_type   primary_dns_addr;   /**< Primary DNS address. */
  ds_profile_addr_type   secondary_dns_addr; /**< Secondary DNS address. */
}ds_profile_dns_addresses_type;

/** @brief PDP profile definition.

  @note1 This structure is PACKED and is stored in the EFS as the  
  profile. If there are any changes to the structure or substructures, 
  ensure that all structures are packed as well. 
  Also, the offsets of the substructures and the parameters 
  within are defined below. Check and ensure that they are the 
  right values and account for any additions/deletions. 
 */
typedef PACKED struct PACKED_POST
{

  /**< Context definition. */
  ds_profile_context_type       context;
  /**< Authentication information. */
  ds_profile_auth_type          auth;
  /**< DNS address; user specified. */
  ds_profile_dns_addresses_type dns_addr;
  /**<Flag to indicate if the PCSCF address should be requested in PCO for this profile */
  boolean                           request_pcscf_address_flag; 
  /**< Flag to indicate if im_cn_flag should be requested for this profile. */
  boolean                           im_cn_flag; 
  /**< Used to determine if an APN is disabled */
  boolean                          apn_disable_flag; 
  /**< Value of the inactivity timer in seconds */
  uint32                           inactivity_timer_val;
  /**< Used to determine APN class */
  uint8                            apn_class;
  /**< Flag used to determine APN bearer type */
  uint8                            apn_bearer;
  /**< Indicates Operator Reserved Pco id to be used in the PCO */
  uint16                           operator_reserved_pco;
  /**< Indicates Mcc to be used in the pco */
  boolean                          msisdn_flag;
  /**< Flag used to determine if msisdn info container needs to be sent in pco */
  uint16                           mcc;
  /**<  Indicates Mnc to be used in the pco */
  ds_profile_mnc_type              mnc;
  /**< Indicates max PDN Connections that can be made per Time Blk*/
  uint16                           max_pdn_conn_per_blk;
  /**< Specifies the minimum time interval (sec) between the new PDN connection
    request and the last sucessful UE initiated PDN disconnection */
  uint16                           pdn_req_wait_time;  
  /**< Specifies the time duration in seconds during which the UE
    counts the PDN connections already made */
  uint16                           max_pdn_conn_time;
  /**< This flag specifies whether the UE is allowed to connect to the APN
    specified by the profile while on Roaming */
  boolean                          roaming_disallowed;
  /**< Time in minutes, given to the client application to deregister with the
    network before tearing down the PDN connection */
  uint8                            pdn_discon_wait_time;
  /**< CLAT enabled flag is set.*/
  boolean                          clat_enabled;
  /**< Flag to indicate attach without PDN support */                                       
  boolean                          attach_wo_pdn;
  /**< Mask to identify the user modifed profile fields info */
  uint64                           mask;
  /**< Flag to indicate whethere the profile is persistent or not */
  ds_profile_addr_type             pdp_addr;
  /**< Satic ip address */
  boolean                          is_non_persistent;
  
  uint32                           failure_timers[DS_3GPP_MAX_FAILURE_TIMERS];

}ds_profile_type_s;


/** @brief Stores the information about the profile to modify
 */
typedef struct {
  char                            apn[DS_PROFILE_APN_STRING_LEN+1];
  ds_profile_pdp_type_e           pdp_type;

} ds_profile_ident_info;

typedef void (*ds_profile_change_handler_type)
(
  ds_profile_ident_info      profile_ident,
  uint64                     mask,
  ds_profile_type_s          pay_load
);


/** @} */ /* end_addtogroup datatypes */

/*---------------------------------------------------------------------------
  PUBLIC ROUTINES
  ---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION DS_PROFILE_MODIFY
  ============================================================================*/
/** @ingroup ds_profile_MODIFY
  Set the profile data identifier based on the mask provided.
  Sets the profile data element identified by the identifier.The prefetched 
  copy is modified.

  @datatypes
  #ds_profile_ident_info
  #profile_ident_type
  #ds_profile_struct

  @vertspace
  @param[in]  profile_ident Identifies the profile data elements.
  @param[in]  mask          Identifier of the profile data element which is modified by client
  @param[in] *payload       Provides the modified value for ident mentioned in the mask

  @return
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure.

  @newpage
 */
ds_profile_status_etype ds_profile_modify (
    ds_profile_ident_info      profile_ident,
    uint64                     mask,
    ds_profile_type_s          *pay_load
    );

/*===========================================================================
  FUNCTION DS_GET_PROFILE
  ============================================================================*/
/** @ingroup ds_profile_get_param
  Get the profile data by the identifier. 

  @datatypes
  #ds_profile_ident_onfo\n
  #ds_profile_struct

  @vertspace
  @param[in] profile_ident Identifies the profile data elements.
  @param[op] payload       Pointer to return the profile content.

  @return
  TRUE                -- Operation succeeded.\n
  FALSE               -- General failure. 

  @newpage
 */
ds_profile_status_etype ds_profile_get_param (
    ds_profile_ident_info      profile_ident,
    ds_profile_type_s          *payload
    );

/*===========================================================================
  FUNCTION DSI_PROFILE_CREATE

  DESCRIPTION
  This function creates the profiles with provided value

  PARAMETERS
  @param[in] profile_ident profile identifier
  @param[in] mask Identifier of the profile data element which is modified by client
  @param[in] payload Provides the modified value for ident mentioned in the mask
  client needs to allocate the memory for this panyload.

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
);

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
ds_profile_status_etype ds_profile_delete( 
    ds_profile_ident_info      profile_ident
    );

/*===========================================================================

FUNCTION REG_PROFILE_CHANGE_HANDLER()

DESCRIPTION
  This function is to register the call back function in case of profile parameter change

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
);

/*===========================================================================

FUNCTION DE_REG_PROFILE_CHANGE_HANDLER()

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
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_FOR_DEFAULT_PROFILE_SETTING

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
);

#endif
