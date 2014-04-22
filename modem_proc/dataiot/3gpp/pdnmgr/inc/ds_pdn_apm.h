#ifndef DS_PDN_APM_H
#define DS_PDN_APM_H

/*===========================================================================
                      DS_PDN_APM.H
DESCRIPTION
This file exposes APM through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_apm.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_apm_dsc.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

#define ATTACH_APN_MAX 2
#define DS_PDN_APM_MAX_APN_STRING_LEN   101
#define DS_PDN_APM_INVALID_PROF_ID_NUM  0

/*---------------------------------------------------------------------------
   Field MASK to indicate the available field  in payload
---------------------------------------------------------------------------*/
#define DS_PDN_APM_IP_V4_FIELD_MASK            0x01 
#define DS_PDN_APM_IP_V6_FIELD_MASK            0x02


/*===========================================================================
                          INDICATION 
===========================================================================*/

/*-------------------------------------------------------------------------
Pay load field mask to indicate the available field in payload
--------------------------------------------------------------------------*/
#define DS_APM_IND_PROFILE_ID_FIELD_MASK            0x0001
#define DS_APM_IND_PCSCF_FIELD_MASK                 0x0002
#define DS_APM_IND_CAUSE_CODE_FIELD_MASK            0x0004
#define DS_APM_IND_IP_ADDRESS_FIELD_MASK            0x0008
#define DS_APM_IND_PLMN_FIELD_MASK                  0x0010
#define DS_APM_IND_SYS_FIELD_MASK                   0x0020
#define DS_APM_IND_APN_SWITCH_MASK                  0x0080
#define DS_APM_IND_DNS_ADDR_FIELD_MASK              0x0100




/*===========================================================================
                          REQUEST 
===========================================================================*/
/*---------------------------------------------------------------------------
  Pay load field mask to indicate the available field in payload
---------------------------------------------------------------------------*/
#define DS_APM_REQ_PROFILE_ID_FIELD_MASK        0x0001
#define DS_APM_REQ_PCSCF_FIELD_MASK             0x0002
#define DS_APM_REQ_IP_ADDRESS_FIELD_MASK        0x0004
#define DS_APM_REQ_DNS_ADDRESS_FIELD_MASK       0x0008
#define DS_APM_REQ_PCSCF_DNS_ADDR_FIELD_MASK    0x0010

/*---------------------------------------------------------------------------
  A client's REQUEST to get information 
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_APM_ATTCH_REQ,           /* Client requesting attach profile id  */
  DS_PDN_APM_MANDATORY_PCSCF_REQ, /* Client requesting mandatory P-CSCFs  */
  DS_PDN_APM_MANDATORY_IP_REQ,     /* Client requesting mandatory IPs      */
  DS_PDN_APM_ATTACH_APN_AVAIL_REQ,  /* Client requesting if attach APN is 
                                      available */
  DS_PDN_APM_MANDATORY_DNS_ADDR_REQ, /* Client requesting mandatory DNS */
  DS_PDN_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ 
                                /* Client requesting mandatory PCSCF or DNS */
}ds_pdn_apm_req_type;

typedef enum
{
  ipv4_mandatory_and_ipv6_donotcare = 1,
  ipv6_mandatory_and_ipv4_donotcare = 2,
  ipv4_mandatory_and_ipv6_mandatory = 3,
  ipv4_dontcare_and_ipv6_donotcare  = 4,
  ipv4_mandatory_or_ipv6_mandatory  = 5
}ipRequiredType;

typedef enum
{
  DS_PDN_APM_ATTACH_RULE,
  DS_PDN_APM_ON_DEMAND_RULE
}rule_category_type;

typedef enum
{
  DS_PDN_APM_INTER_ERR                  = 0,
  DS_PDN_APM_ATTACH_APN_AVAILABLE       = 1,
  DS_PDN_APM_ATTACH_APN_BLOCKED         = 2,
  DS_PDN_APM_EFS_CONFIG_ERROR           = 3,
  DS_PDN_APM_PROF_CONFIG_ERROR          = 4
}attach_apn_availability_info;

/*---------------------------------------------------------------------------
  The INDICATIONS that the client is passing.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_APM_ATTACH_INIT_IND,      /* UE start LTE attach procedure */
  DS_PDN_APM_ATTACH_COMPLETE_IND, /* NW attach completes and L2 ACK received */
  DS_PDN_APM_ATTACH_REJECT_IND,   /* NW reject attach request*/
  DS_PDN_APM_DETACH_IND,          /* N/W detaches                            */
  DS_PDN_APM_IP_ADDRESS_IND,      /* IP address field changes                */
  DS_PDN_APM_PLMN_CHANGED_IND,    /* how to know if there is true PLMN change*/
  DS_PDN_APM_SYS_CHANGED_IND,     /* If the UE moves to system other than LTE*/
  DS_PDN_APM_RPM_INIT_IND,        /* RPM supported SIM is inserted */
  DS_PDN_APM_T3402_CHG_IND,        /* T3402 Timer val has changed */
  DS_PDN_APM_RESET_ATTACH_APN_IND, /* Reset Attach APN to Class 1 */
  DS_PDN_APM_TLB_MODE_ACT_IND,      /* TLB Mode A Activate Ind */
  DS_PDN_APM_ATTACH_PDP_TYPE_IND,  /* PDP type used for attach Ind */
  DS_PDN_APM_MAX_IND                /*Max value of the ENUM*/
}ds_pdn_apm_ind_type;

/*---------------------------------------------------------------------------
  Enum for parameter names specified in the Rules file in EFS.
  Any update to this enum requires a corresponding update to
    ds_pdn_apm_rule_param_name_id.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_APM_RULE_PARAM_ID_MIN = 0,
  DS_PDN_APM_APN_STRING = DS_PDN_APM_RULE_PARAM_ID_MIN,
  DS_PDN_APM_ATTACH_TYPE_RULES,
  DS_PDN_APM_ON_DEMAND_TYPE_RULES,
  DS_PDN_APM_PCSCF_REQUIRED,
  DS_PDN_APM_IPADDRESS_REQUIRED,
  DS_PDN_APM_DNSADDRESS_REQUIRED,
  DS_PDN_APM_PCSCF_OR_DNSADDRESS_REQUIRED,
  DS_PDN_APM_CAUSE_CODE_RULES,
  DS_PDN_APM_DETACH_NETWORK_CAUSE,
  DS_PDN_APM_REJECT_NETWORK_CAUSE,
  DS_PDN_APM_RULE_PARAM_ID_MAX = DS_PDN_APM_REJECT_NETWORK_CAUSE
}ds_pdn_apm_rule_param_name_enum_type;

/*===========================================================================
                          COMMON TO REQUEST AND INDICATION
===========================================================================*/
typedef enum
{
  DS_PDN_APM_SUCCESS,                 /* Success operation                */
  DS_PDN_APM_INVALID_APM,             /* APM is not valid                 */
  DS_PDN_APM_NO_PAY_LOAD,            /* No payload defined for the event */
  DS_PDN_APM_NO_MEMORY_SUPPLIED,     /* no memory allocated to writeback */
  DS_PDN_APM_INVALID_PROF_ID,        /* Client provided profile not valid*/
  DS_PDN_APM_INVALID_RULE_TYPE,      /* Client provided rule type is not valid*/
  DS_PDN_APM_INVALID_ATTACH_TYPE,    /* Client provided attach type not valid*/
  DS_PDN_APM_FIELD_MASK_NOT_SET,    /* Client does not set field mask*/
  DS_PDN_APM_ALL_ATTACH_APN_BLOCKED,  /* All attach APN requested is blocked*/
  DS_PDN_APM_ATTACH_NOT_ALLOWED,      /* Current LTE attach is not allowed */
  DS_PDN_APM_UNSEPCIFIED            /* Unspecified                      */
}ds_pdn_apm_ext_error_code;


typedef struct
{
  /*-------------------------------------------------------------------------
  Field mask indicates the validity of the following fields.
  -------------------------------------------------------------------------*/
  uint8     field_mask;

  /*-------------------------------------------------------------------------
  TRUE - if IP address available 
  FALSE - if address not available 
  -------------------------------------------------------------------------*/
  boolean   v4_address; 
  boolean   v6_address;

  /*-------------------------------------------------------------------------
  Indicate reason of address failure
  -------------------------------------------------------------------------*/
  ds_net_down_reason_type v4_reason;
  ds_net_down_reason_type v6_reason;
}ds_pdn_apm_ip_type; 


/*---------------------------------------------------------------------------
   Indication Pay load data structure
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
  Field mask indicates the validity of the fields.
  -------------------------------------------------------------------------*/
  uint16                          ind_field_mask;

  /*-------------------------------------------------------------------------
  This API is called using theattach profile ID as parameter. So that,                           
  if same API is called (e.g. IPv4 address complete for other profiles),                         
  it can be seperated out.
  -------------------------------------------------------------------------*/
  uint8                           profile_id;

  /*-------------------------------------------------------------------------
  Availability of IP address is notified.
  -------------------------------------------------------------------------*/
  ds_pdn_apm_ip_type             ip_address;

  /*-------------------------------------------------------------------------
  Availability of P-CSCF is notified.
  -------------------------------------------------------------------------*/
  ds_pdn_apm_ip_type              pcscf_address;

  /*-------------------------------------------------------------------------
  Availability of DNS Address is notified.
  -------------------------------------------------------------------------*/
  ds_pdn_apm_ip_type              dns_address;


  /*-------------------------------------------------------------------------
  The actual PLMN ID value is passed by client. 
  Is it relevant to pass 1xRTT, eHRPD and compare with LTE?? TBD 
  -------------------------------------------------------------------------*/
  sys_plmn_id_s_type              plmn_id;

  /*-------------------------------------------------------------------------
  The system. This serves two purpose: 
    - If the system goes OOS
    - If the system acquires a system (e.g. LTE, eHRPD)
  -------------------------------------------------------------------------*/
  sys_sys_mode_e_type             system;

  /*-------------------------------------------------------------------------
  EMM DETACH and REJECT are notified through this cause code. The semantics 
  of reason code is as follows: 
  if (EMM !=19) 
  { 
    internal error; //no switching
  }
  else 
  { 
    if (ESM cause code = 54)
    {
       internal error; //no action on switching
    }
    else
    {
      external error; // switching 
    }
  }
   
  Note: ESM  cause code is valid only if EMM cause code = 19. This logic is 
  embedded in NAS layer. 
  -------------------------------------------------------------------------*/
  ds_net_down_reason_type         cause_code; 

  /*-------------------------------------------------------------------------
  PLMN Blocking Interval Timer value supplied by MH to APM which it gets 
  from NAS 
  -------------------------------------------------------------------------*/
  uint32                          t3402_timer_val; 

  /*-------------------------------------------------------------------------
  Flag that informs APM whether APN switching needs to be performed on 
  receiving an ATTACH REJECT IND 
  -------------------------------------------------------------------------*/
  boolean                         switch_apn;

  /*-------------------------------------------------------------------------
  Flag that informs APM whether APN switching needs to be performed on 
  receiving an ATTACH REJECT IND 
  -------------------------------------------------------------------------*/
  boolean                         tlb_mode_active;

  /*-------------------------------------------------------------------------
    MH notifies APM regarding Attach PDP type 
  -------------------------------------------------------------------------*/
  ds_profile_pdp_type_e             attach_pdp_type;

}ds_pdn_apm_ind_pay_load;

typedef struct
{ 
  /*-------------------------------------------------------------------------
  Field mask indicates the validity of the fields.
  -------------------------------------------------------------------------*/
  uint16                        req_field_mask;

  /*-------------------------------------------------------------------------
  The attach profile ID  
  -------------------------------------------------------------------------*/
  uint8                         profile_id; 

  rule_category_type            rule_type;

  ipRequiredType                mandatory_ip;

  ipRequiredType                mandatory_pcscf;

  ipRequiredType                mandatory_dns_addr;

  ipRequiredType                mandatory_pcscf_or_dns_addr;

  /*-------------------------------------------------------------------------
  Attach APN availability
  -------------------------------------------------------------------------*/
  attach_apn_availability_info  attach_apn_avail;

}ds_pdn_apm_req_pay_load;

/*-------------------------------------------------------------------------
  Indicate the blocked IP type  
-------------------------------------------------------------------------*/
#define DS_PDN_APM_IP_V4_BLOCKED_MASK            0x01 
#define DS_PDN_APM_IP_V6_BLOCKED_MASK            0x02 

typedef struct
{
  uint8                               apn_string_len;
  char                                apn_string[DS_PDN_APM_MAX_APN_STRING_LEN];
}ds_pdn_apm_apn_name_type;

typedef struct
{
  boolean                           attach_allowed;
  uint16                            ip_type_blocked;
  uint16                            apn_len;
  char                              apn_name[DS_PDN_APM_MAX_APN_STRING_LEN]; 
}ds_pdn_apm_apn_block_info_type;

/*-------------------------------------------------------------------------
  The caller needs to allocate and free memory
-------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_apm_apn_block_info_type     *pdn_block_info;
  uint16                             num_apn;
}ds_pdn_apm_block_list_type;

typedef enum
{
  DS_PDN_APM_RULE_INVALID           = 0,
  DS_PDN_APM_RULE_IP_ADDRESS       = 1,
  DS_PDN_APM_RULE_PCSCF_ADDRESS     = 2,
  DS_PDN_APM_RULE_OOS               = 3,
  DS_PDN_APM_RULE_PLMN              = 4,
  DS_PDN_APM_RULE_SYS_CHANGE        = 5,
  DS_PDN_APM_RULE_ATTACH_SUCCESS    = 6,
  DS_PDN_APM_RULE_ATTACH_FAIL       = 7,
  DS_PDN_APM_RULE_ATTACH_BLOCKED    = 8,
  DS_PDN_APM_RULE_DETACH_NORMAL     = 9,
  DS_PDN_APM_RULE_RESET_APN         = 10,
  DS_PDN_APM_RULE_DNS_ADDRESS       = 11,
  DS_PDN_APM_RULE_PCSCF_DNS_ADDRESS = 12,
  DS_PDN_APM_RULE_MAX = DS_PDN_APM_RULE_PCSCF_DNS_ADDRESS
}ds_pdn_apm_apn_rule;

typedef enum
{
  ATTACH_PDN_INVALID     = 0,
  ATTACH_PDN_MIN         = 1,
  ATTACH_PDN_PRIMARY     = ATTACH_PDN_MIN,
  ATTACH_PDN_NEXT        = ATTACH_PDN_MIN+2,
  ATTACH_PDN_MAX         = 0xFF,
  ATTACH_PDN_DEFAULT     = ATTACH_PDN_MAX
}attach_pdn_type;

/*------------------------------------------------------------------------
Data structure to maintain APN information for config rule and acFtion rule
------------------------------------------------------------------------*/
/*----------------------------------------
      Config Flag
-----------------------------------------*/
#define CONFIG_IP_ADDRESS         0x0001
#define CONFIG_PCSCF              0x0002
#define CONFIG_OOS                0x0004
#define CONFIG_PLMN               0x0008
#define CONFIG_SYS_CHG            0x0010
#define CONFIG_ATTACH_SUCCESS     0x0020
#define CONFIG_ATTACH_FAIL        0x0040
#define CONFIG_DETACH_NORMAL      0x0080
#define CONFIG_DNS_ADDRESS        0x0100
#define CONFIG_PCSCF_DNS_ADDRESS  0x0200

/*----------------------------------------
      Action Flag
-----------------------------------------*/
#define ACTION_BLOCK_PLMN         0x0001
#define ACTION_INIT_DETACH        0x0002
#define ACTION_CHANGE_TO_PRIMARY  0x0004
#define ACTION_CHANGE_TO_NEXT     0x0008

/*------------------------------------------------------------
      Enum used to check whether single APM Rule is presemt
-------------------------------------------------------------*/
#define DS_DSD_APM_SINGLE_QUEUE_ENTRY    1

typedef struct
{
  attach_pdn_type            apn_type;
  uint8                      apn_name_len;
  char                       apn_name[DS_PDN_APM_MAX_APN_STRING_LEN];    /*  char e.g. qcomIMS.com, power up*/
  uint8                      profile_id;       /*  This value is read  power up   */

  /*----------------------------------------
  ATTACH PDN CONFIG and ACTION RULE
  -----------------------------------------*/
  struct
  {
    boolean                  attach_rule_valid;
    //Configuration Rule
    struct
    {
      uint16                 config_valid_flag;
      ipRequiredType         ip_address;
      ipRequiredType         pcscf_address;
      ipRequiredType         dns_address;
      ipRequiredType         pcscf_or_dns_address;
    }config_rule;

    //Action Rule
    struct
    {
      uint16                 ip_address;
      uint16                 pcscf_address;
      uint16                 out_of_service;
      uint16                 plmn_change;
      uint16                 sys_change;
      uint16                 attach_success;
      uint16                 attach_fail;
      uint16                 detach_normal;
      uint16                 attach_blocked;
      uint16                 reset_attach_apn;
      uint16                 dns_address;
      uint16                 pcscf_or_dns_address;
    }action_rule;
  }AttachPdn;

  /*----------------------------------------
  ONDEMAND PDN CONFIG and ACTION RULE
  -----------------------------------------*/
  struct
  {
    boolean                  on_demand_rule_valid;
    struct
    {
      uint16                 config_valid_flag;
      ipRequiredType         ip_address;
      ipRequiredType         pcscf_address;
      ipRequiredType         dns_address;
      ipRequiredType         pcscf_or_dns_address;
    }config_rule;
  }OnDemandPdn;

  /*-------------------------------------------
   Cause Code struct to store Cause code info
  --------------------------------------------*/
  struct
  {
    boolean                  cause_code_rule_valid;
    uint16                   config_valid_flag;
    q_type                   cause_code_info_q;
  }cause_code_info;

}apn_info_type;

/*------------------------------------------------------------------------
Data structure to maintain currently attaching pdn information. The 
data structures are used to run the config and action rule. 
------------------------------------------------------------------------*/

/*----------------------------------------
ATTACH PDN STATE (in DSDs view)
-----------------------------------------*/
typedef enum
{
  ATTACH_PDN_STATE_INVALID        = 0,
  ATTACH_PDN_STATE_DISCONNECTED   = 1,
  ATTACH_PDN_STATE_CONNECTING     = 2,
  ATTACH_PDN_STATE_CONNECTED      = 3,
  ATTACH_PDN_STATE_DISCONNECTING  = 4
}attach_pdn_state;

/*----------------------------------------
Network state of a message.
-----------------------------------------*/
typedef enum
{
  DS_PDN_APM_NOT_RCVD       = 0,
  DS_PDN_APM_RCVD_SUCC      = 1,
  DS_PDN_APM_RCVD_FAIL      = 2
}ds_pdn_apm_nw_state_type;

/*-----------------------------------------------
PCSCF or DNS address state. 
 
 0 - Either PCSCF or DNS address is not received
 1 - Both PCSCF & DNS address are invalid
 2 - Either PCSCF or DNS address is valid
------------------------------------------------*/
typedef enum
{
  DS_PDN_APM_PCSCF_DNS_NOT_RCVD = 0,
  DS_PDN_APM_PCSCF_DNS_INVALID  = 1,
  DS_PDN_APM_PCSCF_DNS_VALID    = 2
}ds_pdn_apm_pcscf_dns_addr_state_type;

/*------------------------------------------------------
ATTACH PDN related data structes. This object is 
re-initialized every-time the the ATTACH state goes 
to ATTACH_PDN_CONNECTING.  
------------------------------------------------------*/
typedef struct
{
  attach_pdn_type                   pdn_type; 
  attach_pdn_state                  state; 
  boolean                           ip_rule_required;
  ds_pdn_apm_nw_state_type          ipv4_address;
  ds_pdn_apm_nw_state_type          ipv6_address;
  ds_pdn_apm_nw_state_type          ipv4_pcscf_address;
  ds_pdn_apm_nw_state_type          ipv6_pcscf_address;
  ds_pdn_apm_nw_state_type          ipv4_dns_address;
  ds_pdn_apm_nw_state_type          ipv6_dns_address;
  uint8                             attach_profile_id;
  boolean                           if_lte_attach_allowed;
  boolean                           is_tlb_mode_active;
  ds_profile_pdp_type_e             attach_pdp_type;
}attach_pdn_info_type;

/*------------------------------------------------------------------------
Data structure to maintain next attach pdn information
------------------------------------------------------------------------*/
typedef struct
{
  attach_pdn_type               next_attach_pdn_type;
  boolean                       permanent; 
}next_attach_pdn_info_type;

/*------------------------------------------------------------------------
Data structure to maintain attach pdn information provided through EFS. 
 
TODO: this will be obsolete once EFS reading becomes robust. That will 
provide the number of items and profiles in order or user preference. 
------------------------------------------------------------------------*/
typedef struct
{
  boolean   valid;
  uint8     profile_id;
}efs_profile_input;

typedef struct  
{
  q_link_type                   link; 
  apn_info_type                 apn_info;
}ds_pdn_apm_data_block;

/*------------------------------------------------------------------------
Define a struct for attach_profile_q entry
------------------------------------------------------------------------*/
typedef struct
{
  uint8                          profile;
  boolean                        apn_disabled;
}apm_attach_profile;

typedef struct
{
  q_link_type                     link;
  apm_attach_profile              attach_profile;
}attach_profile_data_block;

/*------------------------------------------------------------------------
Define a struct for cause code Queue block entry
------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                         link;
  uint16                              cause_code;
  ds_pdn_apm_rule_param_name_enum_type   cause_type;
}ds_pdn_apm_cause_code_info_type;

/*------------------------------------------------------------------------
Define a struct for ds_dsd_apn_priority_q entry
------------------------------------------------------------------------*/
typedef enum
{
  PDN_TYPE_INVALID          = 0,
  ATTACH_PDN_TYPE           = 1,
  ON_DEMAND_PDN_TYPE        = 2
}apm_pdn_type;

#define V4_PDN_TYPE        0x01 
#define V6_PDN_TYPE        0x02
#define V4_V6_PDN_TYPE     0x04

#define V4_PROF_PREFER     0x01
#define V6_PROF_PREFER     0x02
#define V4V6_PROF_PREFER   0x04

/*------------------------------------------------------------------------
  Data structure to store APN block/unblock info. 
  ip_type_requested:
            0x01 - V4 only (V4_PDN_TYPE)
            0x02 - V6 only (V6_PDN_TYPE)
            0x04 - V4V6 dual IP only (V4_V6_PDN_TYPE)
            0x05 - V4 or V4V6, V4 prefer (V4_V6_PDN_TYPE & V4_PDN_TYPE)
            0x06 - V6 or V4V6, V6 prefer (V4_V6_PDN_TYPE & V6_PDN_TYPE)
            0x07 - V4 or V6
 
  ip_type_blocked:
            0x01 - V4 block
            0x02 - V6 block
            0x04 - V4V6 block
 
  ip_type_preferred:
            0x01 - v4 prefer
            0x02 - v6 prefer
            0x04 - v4v6 prefer
------------------------------------------------------------------------*/
typedef struct
{
  uint16                   ip_type_blocked; 
  uint16                   ip_type_requested; 
  apm_pdn_type             apn_type;
  uint16                   apn_len;
  char                     apn_name[DS_PDN_APM_MAX_APN_STRING_LEN];
}apm_apn_block_info_type;

typedef struct
{
  q_link_type                    link;
  apm_apn_block_info_type        apn_block_info;
}apn_priority_data_block;

/*----------------------------------------
      Configure Error Flag
-----------------------------------------*/
#define EFS_APM_RULE_READ_ERR                    0x0001
#define EFS_ATTACH_PROFILE_READ_ERR              0x0002
#define EFS_PROFILE_RULE_MISMATCH                0x0004
#define EFS_ATTACH_PROFILE_NUM_ERR               0x0008
#define EFS_APM_RULE_APN_CLASS_ERR               0x0010
#define EFS_DEF_ATTACH_PROF_READ_ERR             0x0020
#define EFS_APM_APN_ACTION_RULE_ERR              0x0040
#define EFS_APM_RULE_ATTACH_PROF_NUM_ERR         0x0080


/*------------------------------------------------------------------------
Define a struct for info required per sub
------------------------------------------------------------------------*/
typedef struct
{
  attach_pdn_info_type          attach_pdn_info;
  next_attach_pdn_info_type     next_attach_pdn_info;
  sys_plmn_id_s_type            apm_lte_plmn_id;
  boolean                       dsd_apm_apn_switching_enable;
  boolean                       dsd_apm_generic_switching_status;
  boolean                       dsd_apm_lte_apn_blocking_support;
  boolean                       dsd_apm_profile_id_based_switch;
  boolean                       dsd_apm_default_apm_rule_enabled;
  boolean                       default_attach_apn_allowed;
  boolean                       apm_attach_apn_all_blocked;
  uint32                        ds_dsd_apm_plmn_blocking_interval;
}ds_pdn_apm_info_type;

typedef struct
{
  uint8  attach_pdn_profile_list_num;   /**< Num of attach PDN in the array */
  uint8  attach_pdn_profile_list[DS_PROFILE_MAX_PROFILE_NUM]; 
    /**< Array of attach PDN profile IDs*/
} ds_pdn_apm_lte_attach_pdn_list_type;


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
);

/*===========================================================================

FUNCTION ds_pdn_apm_get_current_plmn_id

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
);

/*===========================================================================
 
FUNCTION ds_pdn_apm_unset_ip_rule_required

DESCRIPTION
  This is a utility unset rule required for current attach PDN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_apm_unset_ip_rule_required
(
  void
);

/*===========================================================================
FUNCTION       ds_pdn_apm_req

DESCRIPTION    This is the internal function called by external clients to 
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
  ds_pdn_apm_req_pay_load    *pay_load
);

/*===========================================================================
FUNCTION       ds_pdn_apm_ind

DESCRIPTION    This is the external function called by external clients. 
               The clients have to notify the DSD APM module.

DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_ext_error_code ds_pdn_apm_ind
(
  ds_pdn_apm_ind_type        apm_ind_event, 
  ds_pdn_apm_ind_pay_load  * pay_load
);

/*===========================================================================
FUNCTION      ds_pdn_apm_init

DESCRIPTION   This function initializes APM module
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  1. APN INFO, ATTACH_PDN_INFO and NEXT_ATTACH_PDN_INFO 
                 data structure intialized. 
              2. Config rule and action rules are populated
                   - either reading from EFS or from static file.
===========================================================================*/
void ds_pdn_apm_init(boolean is_init);

/*===========================================================================
FUNCTION      DS_PDN_APM_CLEANUP

DESCRIPTION   Called to clean DS_PDN apm rule and profile block queue

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_apm_cleanup(void);

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
  ds_pdn_apm_block_list_type  *forbidden_apn_list
);

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
);

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
);


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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
); 

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
);

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
); 

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
);

#endif /* DS_PDN_APM_H */
