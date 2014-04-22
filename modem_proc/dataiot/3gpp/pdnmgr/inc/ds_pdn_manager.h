#ifndef DS_PDN_MANAGER_H
#define DS_PDN_MANAGER_H

/*===========================================================================
                      DS_PDN_MANAGER.H
DESCRIPTION
This file exposes PDN manager through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_manager.h#15 $
  $DateTime: 2023/05/09 03:51:31 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_bearer_manager.h"
#include "ds_pdn_apm.h"
#include "ps_ip6_sm.h"
#include "ds_pdn_pco_epco_hdlr.h"
#include "ds_pdn_apn_table.h"
#include "ds_pdn_v6_hdlr.h"
#include "ds_fwk.h"
#include "ds_sys_ioctl.h"
#include "dssysmgr.h"
#include "ds_pdn_clat_hdlr.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat_sm.h"
#endif /* #ifdef FEATURE_DATA_PS_464XLAT */
/*============================================================

                   EXTERNAL DEFINITIONS AND TYPES

============================================================*/


/*-------------------------------------------------------------------------
Cookie to ensure that dynamic memory allocated is valid
---------------------------------------------------------------------------*/
#define PDN_MGR_VALID_COOKIE  0x9d9d9d9d

#define PDN_MGR_INVALID_FW_INDEX -1

/* Max lenght of App specific information in Operator reserved PCO */
#define MAX_OP_RES_PCO_APP_SPECIFIC_LEN 255

/*---------------------------------------------------------------------------
  Config protocol field of Protocol options field (PDP ctx req msg)
  should be 0x80 : ref: TS24.008 sec 10.5.6.3
  MSB should be 1 (ext field) protocol field b0-b2 should be 0x0
  define this set but stream value
---------------------------------------------------------------------------*/
#define PROT_CFG_IE_OCT3_VAL  0x80

/*---------------------------------------------------------------------------
  IPV4 DNS Container Identifier field of Protocol options field (PDP ctx req
  msg)
  should be 0x000D : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV4_DNS_CONTAIN_ID 0x000D

/*---------------------------------------------------------------------------
  Maximum Number of IPV6 filters that can be filled in PDP context activate
  message
---------------------------------------------------------------------------*/
#define DS3GPP_IPV6_IID_OFFSET 8

/*---------------------------------------------------------------------------
  Config protocol field of Protocol options field (PDP ctx req msg)
  should be 0x80 : ref: TS24.008 sec 10.5.6.3
  MSB should be 1 (ext field) protocol field b0-b2 should be 0x0
  define this set but stream value
---------------------------------------------------------------------------*/
#define PROT_CFG_IE_OCT3_VAL  0x80

/*---------------------------------------------------------------------------
  IPV6 DNS Container Identifier field of Protocol options field (PDP ctx req
  msg)
  should be 0x0003 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV6_DNS_CONTAIN_ID 0x0003

/*---------------------------------------------------------------------------
  Bearer Control Mode Container Identifier field of Protocol options
  MS Support of Network Requested Bearer Control indicator for NW to NW and
  Selected Bearer Control Mode for NW to MS
  should be 0x0005 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define BEARER_CONTROL_MODE_CONTAINER_ID 0x0005

/*---------------------------------------------------------------------------
  IPv4 Link MTU Request Container Identifier field of Protocol options
  should be 0x0010 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV4_LINK_MTU_CONTAINER_ID 0x0010

/*---------------------------------------------------------------------------
  Non-IP Link MTU Request Container Identifier field of Protocol options
  should be 0x0015 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define NON_IP_LINK_MTU_CONTAINER_ID 0x0015

/*---------------------------------------------------------------------------
  Non-IP Link MTU Request Container Identifier field of Protocol options
  should be 0x0015 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define APN_RATE_CONTROL_CONTAINER_ID 0x0016
#define ADDITIONAL_APN_RATE_CONTROL_CONTAINER_ID 0x0019


#define MSIDSN_CONTAINER_ID        0x000E


/*---------------------------------------------------------------------------
  Rel11: Spec CR_2088 Local address Support in TFT
  should be 0x0011: ref CR_2088 Doc: C1_121662
---------------------------------------------------------------------------*/
#define LOCAL_ADDRESS_SUPPORT_IN_TFT_CONTAINER_ID 0x0011

/*---------------------------------------------------------------------------
  IP address allocation mechanism container IDs
  ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IP_ADDR_ALLOC_NAS_CONTAINER_ID 0x0A

/*---------------------------------------------------------------------------
  IPv4 Link MTU container length (NW to MS)
  should be 2 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV4_LINK_MTU_CONTAINER_LEN  2

/*---------------------------------------------------------------------------
  NON-IP Link MTU container length (NW to MS)
  should be 2 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define NON_IP_LINK_MTU_CONTAINER_LEN  2


/* --------------------------------------------------------------------------- 
  Bit mask used while decoding Protocol Config Options
---------------------------------------------------------------------------*/
#define DS_3GPP_PCO_DECODE_FAILED_MASK            (0x1)
#define DS_3GPP_IPCP_PKT_DECODED_MASK             (0x2)
#define DS_3GPP_IPV4_PKT_DECODED_MASK             (0x4)
#define DS_3GPP_IPV6_PKT_DECODED_MASK             (0x8)
#define DS_3GPP_PCSCF_V4_PKT_DECODED_MASK         (0x10)
#define DS_3GPP_PCSCF_V6_PKT_DECODED_MASK         (0x20)
#define DS_3GPP_BEARER_CONTROL_MODE_DECODED_MASK  (0x40)
#define DS_3GPP_LINK_MTU_DECODED_MASK             (0x80)
#define DS_3GPP_OPER_DECODED_MASK                 (0x100)
#define DS_3GPP_LINK_MTU_INVALID                  (0x200)
#define DS_3GPP_MSISDN_INFO_DECODE_MASK           (0x400)

#define MAX_SYSTEM_MTU 9216      /* MAX system MTU supported by Modem      */

/*---------------------------------------------------------------------------
  Define minimum MTU values for V4, V6 and NON IP instances. 
  An instance can never have a value lower than these values
---------------------------------------------------------------------------*/
#define MIN_V4_MTU   68
#define MIN_V6_MTU  1280
#define MIN_NON_IP_MTU  128

#define MAX_IP_INSTANCES_PER_PDN 2   /* v4 and v6 instances */

/*---------------------------------------------------------------------------
  This is the interval between successive polls to the lower layers to find
  out if there has been any data transfer in either direction

  Recommendation: The polling interval should be set such that the PDN 
  inactivity timer value is an integral multiple of the polling interval. 
  This is just to avoid the polling timer from expiring after the PDN 
  inactivity timer expires.
---------------------------------------------------------------------------*/
#define DS_3GPP_POLLING_INTERVAL 30000

#define DS_3GPP_MAX_PDN_CONTEXT               4 /* Maximum PDN supported */
#define DS_3GPP_MAX_INSTANCES_PER_PDN_CONTEXT 2 /* Dual IP - v4v6 */
#define DS_3GPP_MAX_INSTANCE                  8

#define DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN    5
#define DS_3GPP_MAX_BEARER_CONTEXT            5
#define DS_3GPP_MIN_BEARER_CONTEXT            0   
#define DS_3GPP_DEFAULT_BEARER_INDEX          0

#define DS_3GPP_MAX_FLOW_CONTEXT              20
#define DS_3GPP_MAX_FLOW_CONTEXT_PER_BEARER   4

#define DS_3GPP_IPV6_IID_OFFSET 1  /* Offset is 1 coz we treat 64 bits at once */
#define DS_3GPP_IPV4V6_V4_OFFSET 8 /* Ref 9.9.4.9 */

#define DS_PDN_CFG_PROFILES_MAX  (5)

/*------------------------------------------------------------------------------
  Maximum number of PLMNs we would support simultaneously
------------------------------------------------------------------------------*/
#define DS_3GPP_MAX_PLMN_SUPPORTED 8

#define DS_MAX_THROT_RAT 2

#define DS_MAX_APN_PLMN_SUPPORTED DS_MAX_APN_NUM*DS_3GPP_MAX_PLMN_SUPPORTED* \
                                  DS_MAX_THROT_RAT

/*---------------------------------------------------------------------------
  3GPP release version for filling Container ID:s
    1.DNS server IPv4 address request. 
    2.P-CSCF IPv4 address request 
    3.IP address allocation via NAS signaling
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_REL99 = 0,
  DS_3GPP_REL5  = 1,
  DS_3GPP_REL6  = 2,
  DS_3GPP_REL7  = 3,
  DS_3GPP_REL8  = 4,
  DS_3GPP_REL9  = 5,
  DS_3GPP_REL10 = 6, 
  DS_3GPP_REL11 = 7,
  DS_3GPP_REL_MAX = 0xFF
}ds_3gpp_release_e;


/*---------------------------------------------------------------------------
TYPEDEF DS_PDN_IP_VER_FAILOVER_E_TYPE

DESCRIPTION
  Mapping of the NV item for failover behavior.
---------------------------------------------------------------------------*/
typedef enum
{
  IPV4_ONLY                             = 0,
  IPV6_ONLY                             = 1,
  IPV6_PREFERRED                        = 2,
  IPV4_PREFERRED                        = 3,
  IPV6_DESIRED                          = 4,
  IPV4_DESIRED                          = 5
} ds_pdn_ip_ver_failover_e_type;

/** Network configuration parameter mask. */
typedef enum
{
  NET_CFG_PRM_PRI_DNS_MASK         =   0x00000001, /**< Primary DNS mask. */
  NET_CFG_PRM_SEC_DNS_MASK         =   0x00000002, /**< Secondary DNS mask. */
  NET_CFG_PRM_PRI_NBNS_MASK        =   0x00000004, /**< Primary NBNS mask. */
  NET_CFG_PRM_SEC_NBNS_MASK        =   0x00000008, /**< Secondary NBNS mask. */ 
  NET_CFG_PRM_IP_ADDR_MASK         =   0x00000010, /**< IP address mask. */
  NET_CFG_PRM_AUTH_PREF_MASK       =   0x00000020, /**< Preference mask. */
  NET_CFG_PRM_AUTH_USERNAME_MASK   =   0x00000040, /**< Username mask. */
  NET_CFG_PRM_AUTH_PASSWORD_MASK   =   0x00000080, /**< Password mask. */
  NET_CFG_PRM_CHAP_CHAL_INFO_MASK  =   0x00000100, /**< Mask that includes chal_info 
                                                        chal_name and auth_id. */
  NET_CFG_PRM_DIAL_STR_MASK        =   0x00000200, /**< Dial string mask. */
  NET_CFG_PRM_MAX_MASK                             /**< Maximum mask. */
} net_cfg_params_mask_e_type;


/*------------------------------------------------------------------- 
  PDN states
--------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_MGR_STATE_DOWN        = 0,
  DS_PDN_MGR_STATE_COMING_UP   = 1,
  DS_PDN_MGR_STATE_UP          = 2, 
  DS_PDN_MGR_STATE_GOING_DOWN  = 3,
  DS_PDN_MGR_STATE_PEND_REDIAL = 4,
  DS_PDN_MGR_STATE_MAX   
}ds_pdn_mgr_state_e;

/*------------------------------------------------------------------- 
  Instance types for PDN
--------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_INSTANCE_IPV4 = 0,
  DS_PDN_INSTANCE_IPV6,
  DS_PDN_INSTANCE_NON_IP,
  DS_PDN_INSTANCE_MAX
}ds_pdn_mgr_instance_type_e;

/*------------------------------------------------------------------- 
  Instance states for v4/v6/nonIP
--------------------------------------------------------------------*/
typedef enum
{
  DS_INSTANCE_STATE_DOWN = 0,
  DS_INSTANCE_STATE_COMING_UP,
  DS_INSTANCE_STATE_UP,
  DS_INSTANCE_STATE_GOING_DOWN
}ds_pdn_mgr_instance_state_e;

/*--------------------------------------------------------------------------
  Enum to indicate whether Attach Profile is Available or Not Available.
-------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL = 0,
  DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL,
  DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN
} ds_pdn_cntxt_attach_profile_status_type;

/** 
  @brief Structure definitions used to pass UM configuration 
  information (e.g., ipcp, auth) in IFACE bring up command function.
*/
typedef struct
{
  uint32  ip_address;          /**< IP address requested. */
  uint32  primary_dns;         /**< Primary DNS address. */
  uint32  secondary_dns;       /**< Secondary DNS address. */
} ds_pdn_mgr_ipcp_info_type;


// DS FLOW params

/*---------------------------------------------------------------------------
  Maximum no of instances for UE (v4/v6/nonIP)
---------------------------------------------------------------------------*/
#define MAX_SYSTEM_INSTANCES  8

/*---------------------------------------------------------------------------
  Maximum number of secondary ds_flows that are supported by the mobile
---------------------------------------------------------------------------*/
#define DS_FLOW_MAX_SEC_DS_FLOWS  40    // need to check this value

/*---------------------------------------------------------------------------
  Maximum number of ds_flows that are supported by the UE
---------------------------------------------------------------------------*/
#define DS_FLOW_MAX_DS_FLOWS  (MAX_SYSTEM_INSTANCES + DS_FLOW_MAX_SEC_DS_FLOWS)


/*----------------------------------------------------------------------- 
  This enum type is used to distinguish between various redial scenarios.
--------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_MGR_REDIAL_DUAL_IP_FALLBACK,
  DS_PDN_MGR_REDIAL_AUTH_FALLBACK,
  DS_PDN_MGR_REDIAL_OP_PCO_CHG,
  DS_PDN_MGR_NO_REDIAL
}ds_pdn_mgr_redial_e_type;

typedef struct
{
  uint32 valid_fields;          /**< Mask to validate the authentication
                                     parameters. */
  /** Structure to hold authentication information. */
  struct
  {
    uint8 auth_type;            /**< Authorization negotiated. Valid values: CHAP, PAP, or 
                                     None. */
    ppp_auth_info_type params;  /**< Pointer to authorization information 
                                     parameters. */
    uint8 auth_id;              /**< ID used for authorization packets. */
  }auth_info;
  byte dial_string[NV_PKT_DIAL_STRING_SIZE]; /**< Dial string passed by ATCOP. */
  ds_pdn_mgr_ipcp_info_type ipcp_info;     /**< IPCP information. */
} ds_pdn_network_params_info_type;

/*---------------------------------------------------------------------------
  This structure ontains info that is important in configuring the response to orig or end a call.
---------------------------------------------------------------------------*/
typedef struct
{ 
  uint8           call_instance;     /* call instance to work on    */
  void           *user_data;         /* This is used to hold any other
                                                                     info that needs to be passed.*/
  boolean         low_priority_signaling; /* Set to indicate 
                                                                              low priority signaling */
  boolean         attach_wo_pdn;     /* Flag to indicate if attach is with
                                                                     or without PDN */
} ds_pdn_mgr_call_info_type;

/*-------------------------------------------------------------------- 
  LTE specific PDN parameters
--------------------------------------------------------------------*/
typedef struct 
{
  ps_timer_handle_type      apn_param_chg_timer;  
                                    /* Timer that would be started when clients 
                                       are indicated and given a chance to
                                       teardown  -- Will run for 5 secs */
  boolean                   apn_param_chg_teardown;
                                   /* Flag indicating that the pdn will be 
                                      put to pending redial state and a reconnection
                                      request will be sent out*/
  ds_pdn_ioctl_3gpp_lte_reestab_calltype_enum_type
                            rab_reestab_call_type[MAX_IP_INSTANCES_PER_PDN];

  boolean                   mo_exception_data;
  /* Flag to indicate support for MO Exception data */

  boolean                   allow_force_attach;
  /* Flag to indicate whether force attach is allowed */

  int                       num_mo_ex_filters;
  /* Counter that tracks the number of mo exception filters installed */
}ds_pdn_mgr_lte_pdn_params;

/*-------------------------------------------------------------------- 
  GSM specific PDN parameters
--------------------------------------------------------------------*/
typedef struct 
{
  byte                     pdp_type_org; /* PDP type organization. */  
  byte                     pdp_type_num; /* PDP type number. */
  byte                     pdp_addr_len; /* PDP address length. */
}ds_pdn_mgr_gsm_pdn_params;

/*---------------------------------------------------------------------------
   Instance type structure
---------------------------------------------------------------------------*/
struct ds_pdn_mgr_inst_type
{
  ds_pdn_mgr_instance_type_e      type;      /* v4/v6/nonIP */
  ds_pdn_mgr_instance_state_e     state;     /* Instance state */

  ds_pdn_v6_sm_cb_type           *ds_pdn_v6_sm_cb;    /* DSSNET6 state machine */
  ip6_sm_type                    *ip6_sm;           /* V6 state machine */

  ds_flow_list_s                 *flow;             /* Flow list for this instance */

  boolean                         in_use;           /* any associate client */
  ds_net_down_reason_type         net_down_reason;  /* Down reason */
};


/*-------------------------------------------------------------------- 
  PDN manager structure
--------------------------------------------------------------------*/
struct ds_pdn_mgr_type
{ 
  uint32                    cookie;      /* Cookie to validate the dyn mem*/     
  ds_pdn_mgr_state_e        state;       /* PDN context state */
  boolean                   is_default;  /* Set if this is the default PDN */
  uint8                     index;       /* Index in PDN manager table */
  int8                      fw_index;    /* Index of framework structure - Back ptr */

  ds_pdn_mgr_inst_type_s   *inst_ptr[DS_PDN_INSTANCE_MAX]; /* Instance pointers */

  uint8                     num_active_bearer_context; /* Number of active bearers */
  ds_bearer_mgr_s          *bearer_ptr_tbl[DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN];
                                                   /* Array: ptrs to BearerContext */
  ds_profile_type_s        *pdp_profile;           /* Pointer to profile context */
  uint8                     pdp_profile_num;       /* Profile number */
  byte                      pdp_group_id;   

  sys_sys_mode_e_type       nw_mode;      /* NW mode on which PDN was brought up */

  boolean                   allow_inst_bring_up;   
  /* Flag to indicate second instance bring up for a dual IP PDN during attach */
  
  ps_timer_handle_type      pdn_inactivity_timer;
                            /* To tear down PDN connection on inactivity */
  ps_timer_handle_type      polling_timer;
                            /* Use to poll lower layers for data activity*/

  boolean                   ue_initiated_tear_down;
  /* Flag to indicate whether the Network or UE initiated the Tear Down */

  ds_pdn_network_params_info_type info; /* Network IPCP/auth params */

  ds_pdn_mgr_redial_e_type  redial_reason;
                            /* State to know why a redial on the PDN occured*/
  ds_net_down_reason_type   net_down_reason;
                            /* Used to cache the down reason received from the network*/

  ds_pdn_mgr_lte_pdn_params *lte_params;  /* LTE specific params */
  ds_pdn_mgr_gsm_pdn_params *gsm_params;  /* GSM specific params */

  boolean                   force_ota_teardown;
  /* This can be set if OTA teardown needs to be forced */

  boolean                   ims_sip_reg_failure_flag; 
  /* Flag is set by IMS Client using IOCTL when SIP signaling fails */

  boolean                  is_always_shared;
  /* Flag indicating if this PDN is being used as always share PDN */

  boolean                  low_priority_signaling;
   /* Flag to indicate low priority signaling */

  boolean                  is_restore_mode; 
  /* Flag to indicate PSM mode */

  ds_apn_enum_type                    apn_type; /* APN type */
  ds_pdn_mgr_op_reserved_pco_type     op_reserved_pco;     
  ds_pdn_mgr_modem_reserved_pco_type  modem_reserved_pco;  /* PCO info */
  boolean                        cache_v4_inst_down;
  
#ifdef FEATURE_DATA_PS_464XLAT
  ps_clat_sm_type                *clat_sm_inst;
#else
  void  *clat_sm_inst;
#endif
  ds_pdn_clat_sm_state           clat_state; 

};

typedef struct
{
  ds_pdn_mgr_s   *pdn_mgr_p;
  ds_pdn_mgr_instance_type_e inst_type;
  ds_pdn_network_params_info_type *info_ptr;
}ds_pdn_mgr_instance_cmd_type;

typedef struct
{
  uint8    profile_num;
  uint64   mask;
  boolean  apn_param_changed;
}ds_pdn_mgr_profile_change_s;

typedef struct
{
  boolean                        in_progress;
  sys_radio_access_tech_e_type   srat;
  sys_radio_access_tech_e_type   trat;
}ds_pdn_mgr_irat_info_s;

/*===========================================================================

                   FUNCTION DECLARATONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_MGR_INIT

DESCRIPTION
  This function initialises the PDN manager module.

PARAMETERS
  None
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_init
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_VALIDATE_PDN_PTR

DESCRIPTION
  This function checks if the bearer ptr has been allocated dynamic memory

PARAMETERS
  pdn_mgr_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_validate_pdn_ptr
(
  ds_pdn_mgr_s *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_PTR_FROM_INDEX

DESCRIPTION
  This function fetches the PDN Manager Pointer corresponding to the
  given PDN index.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_ptr_from_index
(
  uint8 instance
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_DEFAULT_BEARER

DESCRIPTION
  This function fetches the default bearer corresponding to the
  given PDN.
 
PARAMETERS
  PDN pointer

DEPENDENCIES
  None.

RETURN VALUE
  Bearer pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_pdn_mgr_get_default_bearer
(
  ds_pdn_mgr_s   *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_DEFAULT_BEARER_STATE

DESCRIPTION
  This function fetches the default bearer state corresponding to the
  given PDN.
 
PARAMETERS
  PDN pointer

DEPENDENCIES
  None.

RETURN VALUE
  Bearer state.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_state_e ds_pdn_mgr_get_default_bearer_state
(
  ds_pdn_mgr_s   *pdn_mgr_p
);


/*===========================================================================
FUNCTION  DS_PDN_MGR_SET_ACTIVE_RAT

DESCRIPTION
  This function sets the current active rat received from NAS

PARAMETERS
  sys_radio_access_tech_e_type

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_set_active_rat
(
  sys_radio_access_tech_e_type  active_rat
);

/*===========================================================================
FUNCTION  DS_PDN_MGR_GET_ACTIVE_RAT

DESCRIPTION
  This function returns the active rat

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  sys_radio_access_tech_e_type

SIDE EFFECTS
  None.

===========================================================================*/
sys_radio_access_tech_e_type ds_pdn_mgr_get_active_rat
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_DEF_PDN

DESCRIPTION
  This function is used to get to the default PDN context.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the default PDN context irrespective of the state of the PDN. 
   
SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_def_pdn
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_PDP_PROFILE_INFO

DESCRIPTION
  This function Sets the PDP profile to be used for the call instance
  specified. It also gets the profile parameters for the call and stores
  it in the umtsps iface table.

  This function is called by the routing layer before to set this information.
  Subsequent to this, the call origination happens.

PARAMETERS
  *umtspsps_iface_ptr - ptr to umtsps iface instance
  pdp_profile_number - contains the PDP profile information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_mgr_process_pdp_profile_info
(
  ds_pdn_mgr_s              *pdn_mgr_p,
  ds_apn_ip_support_type_e  *ip_support_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_FROM_PROFILE_ID

DESCRIPTION
  This function returns PDN corresponding to a profile ID

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  PDN pointer

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_from_profile_id
( 
  uint8   profile_id 
);

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_ANY_PDN_GOING_DOWN

DESCRIPTION
  This function returns whether there is a pdn in going down

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean - whether there is a pdn context in going_down state

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_is_any_pdn_going_down
( 
   void 
);
/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_pdn_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN  in UP state. 
 
PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_pdn_up();
/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_other_pdn_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) in UP state. Whether the PDN passed in is 
  UP or not, does not matter. 
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state, other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_other_pdn_up
(
  ds_pdn_mgr_s  *pdn_mgr_p
);

/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_other_normal_pdn_diff_profile_num_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) and not the same profile number in UP state.
  Whether the PDN passed in is UP or not, does not matter. 
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean ds_pdn_mgr_is_any_other_normal_pdn_diff_profile_num_active
(
  ds_pdn_mgr_s *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_PDN_INACTIVITY_CMD

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the PDN inactivity timer expires. This command cleans up the PDN context
  irrespective of whether it is the last one standing or otherwise.

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_process_pdn_inactivity_cmd
(
  ds_pdn_mgr_s* pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_POLLING_TMR_EXP_CMD

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the polling timer expires. This function queries the lower layers to figure
  out if there has been any data activity since the last polling timer expiry

  If there has been data activity, we restart the PDN inactivity timer
  Otherwise, we let it run.

PARAMETERS
  pdn_mgr_p - Pointer to the PDN 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_process_polling_tmr_exp_cmd
(
  ds_pdn_mgr_s* pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_ALLOCATE_NEW_INSTANCE

DESCRIPTION
  This function allocates instance type. 

PARAMETERS
  PDN ptr
  Instance type

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated instance or NULL if allocation fails.

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_inst_type_s* ds_pdn_mgr_allocate_new_instance
(
 ds_pdn_mgr_s               *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e  inst_type
);

/*===========================================================================
FUNCTION DS_PDN_MGR_ALLOCATE_NEW_PDN

DESCRIPTION
  This function allocates unused pdn context. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context or NULL if there is none left.

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_allocate_new_pdn_context
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_REJECT_NULL_APN

DESCRIPTION
  This function checks to see if null apn is supported
  Null apn is supported only if the PDN is attach PDN
 
PARAMETERS

DEPENDENCIES

RETURN VALUE
  True -  If the apn in the pdn context is NULL
              and the PDN is not attach PDN
  False - Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_reject_null_apn
(
  ds_pdn_mgr_s         *pdn_mgr_p,
  char                 *apn_name  
);

/*===========================================================================
FUNCTION      ds_pdn_mgr_cleanup_pdn

DESCRIPTION
  Moves the current Iface and other iface to DOWN state. Cleans the APN table 
  entry. Also frees the PDN context.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought down.  
  down_reason   - Reason to cleanup 

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_cleanup_pdn
(
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
);

/*===========================================================================
FUNCTION ds_pdn_mgr_handle_lpm_mode_change

DESCRIPTION
  This function is called to clean up calls when UE goes to LPM

PARAMETERS
  void

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_pdn_mgr_handle_lpm_mode_change
(
  sys_oprt_mode_e_type oprt_mode
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_INSTANCE_UP_HDLR

DESCRIPTION
  Brings up the IP instance requested after allocating a PDN instance.

PARAMETERS
  PDN context
  Instance type
  Call Info
  is_comp_bring_up - Is this call for companion instance bring up.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  True- If successfull instance bring up.
  False - Otherwise

SIDE EFFECTS
  If MT table has matching call info as current call,
  this will clear the table entry.
===========================================================================*/
boolean ds_pdn_mgr_instance_up_hdlr
(
  ds_pdn_mgr_s                    *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e       inst_type,
  ds_pdn_network_params_info_type *info_ptr,
  boolean                          is_comp_bring_up
);

/*===========================================================================
FUNCTION ds_pdn_mgr_update_pco

DESCRIPTION
  This function is used to update PCO. This inturn invokes
  ds_pdn_cntxt_decodeProtocolParams to perform the actual update.
  
PARAMETERS
    Bearer ptr
    PCO
    ePCO    
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_update_pco
(
  ds_bearer_mgr_s                  *bearer_mgr_p,
  protocol_cfg_options_T           *config_options_p,
  ext_protocol_cfg_options_T       *ext_protocol_conf_opt_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_PCO_CONTAINER_INFO

DESCRIPTION
  This function fills the requested container info in PCO. 
  
PARAMETERS   
  protocol_cfg_options_T       *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - If packet container appended succesfully
  FALSE- Error

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_fill_pco_container_info
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  uint16                          container_id,
  ds_pdn_protocol_id_s          **proto_info
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPCP_ORIG_PARAMS

DESCRIPTION
  This function fills the IPCP (Primary and Secondary DNS only) specific 
  protocol information in the PDP context request message.
  
PARAMETERS 
    *orig_params_ptr     - Ptr to PDN origination parameters 
    req_spec_dns_flg     - IS dns address requested
    primary_dns          - primary dns address requested
    secondary_dns        - secondary dns address requested
    req_spec_nbns_flg    - IS nbns address requested
    primary_nbns         - primary WINS address
    secondary_nbns       - secondary WINS address
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPCP packet generated succesfully and included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of PPP packets included by this function
     
===========================================================================*/
boolean ds_pdn_mgr_fill_ipcp_orig_params
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  boolean                       req_spec_dns_flg,/* Should spec DNS add req*/
  uint32                        primary_dns,    /* primary DNS             */
  uint32                        secondary_dns,  /* secondary DNS           */
  net_cfg_params_mask_e_type    valid_fields    /* IPCP fileds to request */
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV4_DNS_CONTAINER

DESCRIPTION
  This function appends DNS server Ipv4 address request container to the 
  PCO buffer. 
  
PARAMETERS   
  protocol_cfg_options_T       *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV4 DNS packet container appended succesfully
  FALSE- Error

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_fill_ipv4_dns_container
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV6_DNS_PARAMS

DESCRIPTION
  This function fills the IPV6 (DNS only) specific protocol information 
  in the PDP context request message.
  
PARAMETERS 
    protocol_cfg_options_T       *protocol_config_options_p
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV6 DNS packet generated succesfully and included in PDP context 
  request message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
boolean ds_pdn_mgr_fill_ipv6_dns_params
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_AUTH_PARAMS_FROM_INFO

DESCRIPTION
  This function fills the authentication information obtained from the info_ptr
  in the instance up cmd hdlr.
  
PARAMETERS 
  *protocol_config_options_p    - config option that has to be filled
  *profile_p,                   - PDP profile
  *info_p                   - info pointer
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Auth info not included in PDP context request.
  
SIDE EFFECTS 
  None     
===========================================================================*/
boolean ds_pdn_mgr_fill_auth_params_from_info
(
  ds_pdn_protocol_cfg_options_s        *protocol_config_options_p,
  ds_profile_type_s                     *profile_p, 
  const ds_pdn_network_params_info_type *ppp_info_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_AUTH_PARAMS_FROM_PROFILE

DESCRIPTION
  This function fills the authentication information obtained from profile
  in the PDP context request message.
  
PARAMETERS 
  *protocol_config_options_p    - Config option that has to be filled
  *profile_p,                   - PDP profile
   subs_id                      - Subscription id
 
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
    
===========================================================================*/
boolean ds_pdn_mgr_fill_auth_params_from_profile
(
  ds_pdn_protocol_cfg_options_s   *protocol_config_options_p,
  const ds_profile_type_s          *profile_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_OPER_PCO_INFO_FROM_PROFILE

DESCRIPTION
  This function fills the operator specific protocol information in 
  the PDP context req. message. 
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    profile_p - Prt to the profile info    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_mgr_fill_oper_pco_info_from_profile
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  ds_profile_type_s               *profile_p
);

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS_PDN_MGR_GEN_PCSCF_ADDRESS_REQUEST_PACKET

DESCRIPTION
  This function generates a PCSCF request packet to request the 
  pcscf address.

PARAMETERS   
  orig_params_ptr  - Pointer to the buffer to store the pcscf request packet
    
DEPENDENCIES 
  None.

RETURN VALUE  
  DS3G_SUCCESS - packet generated succesfully
  DS3G_FAILURE - pkt not generated
  
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_gen_pcscf_address_request_packet
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  const ds_profile_type_s         *profile_params /* profile parameters */
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GEN_IMCN_SUBSYSTEM_REQUEST_PACKET

DESCRIPTION
  This function generates a IMCN request packet to request the 
  signaling only context.

PARAMETERS   
  orig_params_ptr  - Pointer to the buffer to store the IMCN request packet
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE  - packet generated succesfully
  FALSE - pkt not generated
  
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_gen_imcn_subsystem_request_packet
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p 
);
#endif /* FEATURE_DATA_IMS */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_ADDRESS_ALLOC_PARAM

DESCRIPTION
  This function fills the IPV4 address allocation container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
    *profile_p           - Ptr to ds profile
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Successfully filled IPv4 Address allocation in PCO
  FALSE- Error
  
SIDE EFFECTS 
  None.     
===========================================================================*/
void ds_pdn_mgr_fill_address_alloc_param
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  ds_profile_type_s               *profile_p
);
 
/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_BCM_REQUEST

DESCRIPTION
  This function fills the BCM container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  none
  
SIDE EFFECTS 
  None     
===========================================================================*/
void ds_pdn_mgr_fill_bcm_indication
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV4V6_LINK_MTU_CONTAINER

DESCRIPTION
  This function fills the IPV4V6 mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  None.    
===========================================================================*/
void ds_pdn_mgr_fill_ipv4v6_link_mtu_container
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV4V6_LOCAL_ADDRESS_SUPPORT_CONTAINER

DESCRIPTION
  This function fills the IPV4V6 mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  None.    
===========================================================================*/
void ds_pdn_mgr_fill_ipv4v6_local_address_support_container
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
);

/*===========================================================================
FUNCTION ds_pdn_mgr_store_encoded_apn

DESCRIPTION
  This function encodes the apn name contained in apn_src
  and stores it in apn_dest.


PARAMETERS
  apn_dest:   Destination to store the encoded apn.
  apn_src:    Uncoded APN name.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the encoded APN

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_mgr_store_encoded_apn
(
  byte *apn_dest,
  byte *apn_src
);

/*===========================================================================
FUNCTION ds_pdn_mgr_tear_down_instance

DESCRIPTION
  This function tears down the instance.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_mgr_tear_down_instance
( 
  ds_pdn_mgr_s                 *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e    inst_type,
  ds_net_down_reason_type       down_reason
);

/*===========================================================================
FUNCTION ds_pdn_mgr_bearer_down_ind

DESCRIPTION
  This function handles bearer teardown from PDN perspective.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_mgr_bearer_down_ind
( 
  ds_pdn_mgr_s                  *pdn_mgr_p,
  ds_bearer_mgr_s               *bearer_mgr_p,
  ds_net_down_reason_type        net_down_reason
);

/*===========================================================================
FUNCTION DS_PDN_MGR_RESEND_PDN_CONN_REQ

DESCRIPTION
  This function is used to resend the PDN connectivity request. This function
  is applicable only to the LTE mode of operation.

PARAMETERS
  pdn_mgr_p - Pointer to the PDN context on which the request is to be sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_resend_pdn_conn_req
(
  ds_pdn_mgr_s*              pdn_mgr_p,
  ds_pdn_mgr_instance_type_e inst_type
);

/*===========================================================================
FUNCTION DS_PDN_MGR_SETUP_PDN_WITH_DS_INSTANCE

DESCRIPTION
  This function associates a valid DS instance to a newly allocated pdn context
  Updated the pdp type in the profile and the APN table as required.
 
PARAMETERS 
  new_pdn_cnt_p    - Pointer to the newly allocated PDN context
  old_pdn_cntx_p    - Pointer to the old PDN context
  inst_p                 - Pointer to a previously valid DS instance that is being
                              associated with the newly allocated PDN

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_setup_pdn_with_ds_instance
(
  ds_pdn_mgr_s           *new_pdn_cntx_p, 
  ds_pdn_mgr_s           *old_pdn_cntx_p, 
  ds_pdn_mgr_inst_type_s *inst_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_DISSOCIATE_INSTANCE

DESCRIPTION
  This function dissociates the PDN context from the instance that was not
  granted in Activate Bearer Indication

PARAMETERS   
  pdn_mgr_p             - Pointer to the PDN context
  ip_support_granted - The IP support that was returned by the Network
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_dissociate_instance
(
  ds_pdn_mgr_s             *pdn_mgr_p, 
  ds_apn_ip_support_type_e  ip_support_granted
);

/*===========================================================================
FUNCTION ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t

DESCRIPTION
  This function converts Data Compression parameter from PDP Profile's 
  native type to sys_pdp_data_comp_e_type.

PARAMETERS
    ds_umts__pdp_data_comp_e_type - PDP Profile Header Compression type

DEPENDENCIES
  None.

RETURN VALUE
  sys_pdp_data_comp_e_type - Header Compression type.

SIDE EFFECTS
  None.
===========================================================================*/
sys_pdp_data_comp_e_type ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t
(
  const ds_profile_data_comp_e_type *pdp_d_comp
);

/*===========================================================================
FUNCTION ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t

DESCRIPTION
  This function converts Header Compression parameter from PDP Profile's 
  native type to sys_pdp_header_comp_e_type.

PARAMETERS
    ds_umts_pdp_header_comp_e_type - PDP Profile Header Compression type

DEPENDENCIES
  None.

RETURN VALUE
  sys_pdp_header_comp_e_type - Header Compression type.

SIDE EFFECTS
  None.

===========================================================================*/
sys_pdp_header_comp_e_type ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t
(
  const ds_profile_header_comp_e_type *pdp_h_comp
);

/*===========================================================================
FUNCTION ds_pdn_mgr_set_mtu

DESCRIPTION
  This function sets MTU for the provided instance.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_set_mtu
( 
 ds_pdn_mgr_s               *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e  inst_type,
  uint16                  mtu
);

/*===========================================================================
FUNCTION ds_pdn_mgr_set_msisdn_info

DESCRIPTION
  This function sets MSISDN INFO for the provided instance.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_set_msisdn_info
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_sys_msisdn_info_type    *msisdn_info
);

/*===========================================================================
FUNCTION	  ds_pdn_mgr_get_msisdn_info

DESCRIPTION
  This function fetches the msisdn info from the fwk instance.

PARAMETERS
  arg_val_ptr[in] -  Pointer to operation specific argument
  ps_errno [out]    -    Specifies type of error in case of error.
                             DS_EFAULT if parameter is invalid
                             DS_EINVAL if operation is invalid
                             DS_NOMEMORY if memory allocation fails


DEPENDENCIES  None.

RETURN VALUE  TRUE  if fetch successful.
              FALSE if something goes wrong.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_mgr_get_msisdn_info
(
  void            *arg_val_ptr,
  int16           *ps_errno
);


/*===========================================================================
FUNCTION	  ds_pdn_mgr_decode_msisdn

DESCRIPTION
  This function decodes the msisdn from the PCO.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_mgr_p 	 -  Pointer to current PDN context
  decode_mask   	 -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_decode_msisdn
(
  const byte		   *protocol_pkt_ptr, /* Pointer to the buffer forMTU info */
  byte  				protocol_pkt_len,  /* Length of the MTU container */
  ds_pdn_mgr_s         *pdn_mgr_p,
  uint32			   *decode_mask_ptr
);


/*===========================================================================
FUNCTION ds_pdn_mgr_decode_proto_params

DESCRIPTION
  This function decodes the protocol parameters received for a PDP call.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  Integer bitmask representing the decoded packets 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_pdn_mgr_decode_proto_params
(
  ds_pdn_mgr_s                     *pdn_mgr_p,
  ds_pdn_protocol_cfg_options_s   *protocol_config_options_p
);

ds_pdn_mgr_instance_type_e ds_pdn_mgr_convert_fw_ip_type_to_pdn_inst
(
  ip_addr_enum_type ip_type  
);

/*===========================================================================
FUNCTION ds_pdn_mgr_setup_pdp_ip_proto_stack

DESCRIPTION
  This function configures IP/nonIP instances with NAS provided call info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_setup_pdp_proto_stack
( 
  ds_pdn_mgr_s                   *pdn_mgr_p,
  ds_profile_addr_type           *ds_pdn_addr,  
  ds_pdn_protocol_cfg_options_s *protocol_config_options_p
);

ip_addr_enum_type ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type
(
  ds_pdn_mgr_instance_type_e  pdn_inst_type
);

/*===========================================================================
FUNCTION ds_pdn_mgr_instance_up_ind

DESCRIPTION
  This function posts instance up indications.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void ds_pdn_mgr_instance_up_ind
(
  ds_pdn_mgr_s            *pdn_mgr_p,
  ds_pdn_mgr_inst_type_s  *inst_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_BRING_DOWN_BEARER

DESCRIPTION
  This function brings down default bearer and tears down the PDN instance.
  
PARAMETERS 
  pdn_mgr_p - PDN control block
  instance type - v4/v6/nonIP
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_bring_down_bearer
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
);

/*===========================================================================
FUNCTION DS_PDN_MGR_INST_DOWN_HDLR

DESCRIPTION
  This function tears down the PDN instance.
  
PARAMETERS 
  pdn_mgr_p - PDN control block
  instance type - v4/v6/nonIP
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_inst_down_hdlr
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
);

/*===========================================================================
FUNCTION DS_PDN_MGR_TEARDOWN_INSTANCES_FOR_PDN

DESCRIPTION
  This function tears down the PDN connection.
  
PARAMETERS 
  pdn_mgr_p - PDN control block
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_teardown_instances_for_pdn
( 
  ds_pdn_mgr_s            *pdn_mgr_p,
  ds_net_down_reason_type  tear_down_reason
);

/*===========================================================================
FUNCTION DS_PDN_MGR_BEARER_UP_IND

DESCRIPTION
  This function completes PDN and instance configuration in response to 
  bearer up ind.

PARAMETERS
  PDN pointer
  bearer pointer

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void ds_pdn_mgr_bearer_up_ind
( 
  ds_pdn_mgr_s           *pdn_mgr_p,
  ds_bearer_mgr_s        *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_pdn_mgr_is_bearer_deactivated_by_network

DESCRIPTION
  This function is used to check if the bearer is deactivated by the network
  Whenever the bearer is deactivated by the network PDN contexts
  1. Force OTA teardown must not be set
  2. UE initiated teardown must not be set

PARAMETERS
  pdn context
  bearer context

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_mgr_is_bearer_deactivated_by_network
(
   ds_pdn_mgr_s          *pdn_mgr_p,
   ds_bearer_mgr_s       *bearer_mgr_p
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_WWAN_TECH_SUSPENDED

DESCRIPTION
  This function return the status whether WWAN tech is suspended.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - IF WWAN tech is suspended.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_is_wwan_tech_suspended
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_CLEAR_IRAT_INFO

DESCRIPTION
  This function clears the IRAT information.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_clear_irat_info
( 
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_IRAT_IN_PRORESS

DESCRIPTION
  This function retrun the status if IRAT procedure is ongoing.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - IF IRAT is in progress.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_is_irat_in_progress
( 
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_COMPLETE_IRAT

DESCRIPTION
  This function completes the IRAT procedure.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_complete_irat
( 
  void
);

/*===========================================================================
FUNCTION DS_PDN_MGR_TEARDOWN_PDN_PER_PROFILE

DESCRIPTION
  This function tears down the PDN Connections
  using the passed profile number.

PARAMETERS
 Profile Number
 DOWN Reason to be given to Client Applications

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_teardown_pdn_per_profile
( 
  uint8                         profile_num,
  ds_net_down_reason_type       down_reason
);

/*===========================================================================
FUNCTION DS_PDN_MGR_DS_TO_NAS_SEND_MSG
 
DESCRIPTION
  This function sends messages from PDN manager to NAS.

PARAMETERS 
  Message - void*
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  
===========================================================================*/
void ds_pdn_mgr_ds_to_nas_send_msg
(
  void*         msg_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_CMDS
 
DESCRIPTION
  This function processes the commands posted to PDN manager.

PARAMETERS 
  Message - void*
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_mgr_process_cmds
(
  ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION ds_pdn_mgr_handle_v6_inst_down

DESCRIPTION
  This function does additional handling when v6 instance goes down  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_mgr_handle_v6_inst_down
(
  ds_pdn_mgr_s             *pdn_mgr_p,
  ds_net_down_reason_type   down_reason
);

/*===========================================================================
FUNCTION DS_PDN_MGR_HANDLE_V6_EV_CBACK()

DESCRIPTION
  Called when the IP6 comes up/down.  This posts an appropriate event to the
  dssnet6 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_handle_v6_ev_cback
(
  ds_pdn_v6_sm_cb_type    *instance,
  ds_pdn_v6_sm_event_type  cur_event
);

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_DEFAULT_TX_FLOW_ENABLED
 
DESCRIPTION
  This function returns the TX flow status of the default bearer for this PDN

PARAMETERS 
  PDN pointer
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_pdn_mgr_is_default_tx_flow_enabled
(
  ds_pdn_mgr_s     *pdn_mgr_p
);

/*===========================================================================
  FUNCTION DS_PDN_MGR_LOOKUP_PDN()

  DESCRIPTION
    This function leads to PDN lookup by framework.

  PARAMETERS
    apn, ip_type

  RETURN VALUE
     valid framework index -- if matches with any of the existing pdn
     -1 -- if not matching

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ds_fwk_index_type ds_pdn_mgr_lookup_pdn
(
  ds_policy_info_type  policy_info
); 


/*===========================================================================
  FUNCTION DS_PDN_MGR_BRINGUP_PDN()

  DESCRIPTION
    This function leads to PDN bringup.
    this functions returns immediately with the info such as pdn_ptr and cause codes(incase of failure).
    pdn_ptr will be updated in ds_fwk strcutures for further operations.
    however clients needs to wait for PDN events for PDN UP,DOWN status.

  PARAMETERS
    ds_fwk_idx,apn, ip_type,info

  RETURN VALUE
      0  -- if bringup is successful.(pdn_ptr will be updated in info out param)
     -1 -- if bringup is failed.(failure reason will be updated in info out param)
      102 -Operation would block.(pdn_ptr will be updated in info out param)
      
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ds_pdn_mgr_bringup_pdn
(
  ds_fwk_index_type               ds_fwk_idx,
  ds_policy_info_type             policy_info,
  ds_fwk_response_info_s_type*    response_info
);

/*===========================================================================
  FUNCTION DS_PDN_MGR_TEARDOWN_PDN()

  DESCRIPTION
    This function leads to PDN teardown.
    teardown function returns immediately from MH and clients needs to wait for pdn_down 
    events for cleanup.  as part of PDN down events ds framework posts events to its clients 
    and deletes the ds_framework instance incase of v4,v6,non-ip pdn's are invalid.
    
  PARAMETERS
    apn, ip_type

  RETURN VALUE
      0  -- if teardown is successful.
     -1 -- if teardown is failed.
      102 -Operation would block.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ds_pdn_mgr_teardown_pdn
(
  ds_pdn_mgr_s                  *pdn_ptr,
  ip_addr_enum_type             ip_type,
  ds_fwk_response_info_s_type*  response_info
);

#if 0
/* Look up function */
uint8 lookup
(
  apn
}
{
/*
  - Loop through all valid PDNs, and see if the passed APN matches with APN/resolved APN in PDN.
  - If any match, return the fw_index stored in PDN.
  - If not, return max value i.e. FF
*/
}

/* Bring up function */
void bring_up
(
  policy_info,     
  fw_index,
  func_ptr_info_s 
)
{

/* Scenarios :
1. For call on attach PDN,
   - As instance is already up, return success with PDN ptr.
   - Send PDN UP event with client handle.

2. For new call on additional PDN which is not yet allocated, or if corresponding instance is coming up,
   - Send E_WOULD_BLOCK.
   - Send PDN UP/DOWN event based on response from NW.

3. For any failure,
   - Send failure with down reason.  */


  // Get the APN and IP type from policy_info
  /* Run policy checks
     - Set roaming status during bring up
     - Deny call if NV refesh in progess
     - Deny call if thermal mitigation in progress
     - TLB mode validation, if UE in TLB mode, check if policy also specifies the same.
     - Get profile based on APN and IP type
     - Check if current NW mode is L/G
     - Check for roaming disallowed flag if in roaming
     - Determine PDP type based on home/roaming PDP types
     - Determine call type to be originated based on above info and IP type in policy info.
     - Check for non IP validation
     - APN validation
**   - UI data/roaming status
     - Check for MO exception support if requested
     - If APN assocated with any PDN, and if APN table has entry
        - If multi PDN same APN not supported
             - reframe ds_3gpp_call_hdlr_ip_support_for_apn, check for all possibilities
        - If multi PDN same APN supported
             - check for possibilities
    - Compare for PDP profile info. refer ds3gpp_compare_pdp_profile_info
    - Post proc functinality
       - If PDN exists
           - If policy requests for MO ex, and UE supports but PDN is not marked, set the flag, and do force attach
           - For coming up scenario also, trigger force attach
           - Store FW index
           - Return success with PDN ptr
       - If PDN doesn't exist
           - Update v6 SM
           - Allocate new PDN
           - Store FW index
           - Allocate concerned instance, and companion instance(if required).   
           - Update APN table
           - Return success with PDN ptr. */
}



/* Tear down function */
void tear_down
(
  pdn_ptr,
  ip_type
  func_ptr_info_s 
)
{

/* Scenarios :
1. For call on attach PDN,
   - As no OTA, decrement the ref_cnt and return success. Post command to udpdate APN table, and if any other cleanup.
   - Send PDN DOWN event with client handle.

2. For any other call, if no OTA required, behavior is same as 1.

3. For any call requiring OTA,
   - Send E_WOULD_BLOCK 
   - Once PDN is cleaned up, send PDN DOWN event  */

}
#endif
/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_PTR

DESCRIPTION
  This function fetches the PDN Manager Pointer corresponding to the
  given index.
 
PARAMETERS
  PDN Index

DEPENDENCIES
  None.

RETURN VALUE
  PDN Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_ptr
(
  uint8 index
);

/*===========================================================================
FUNCTION  ds_pdn_mgr_get_low_priority_signaling_bit

DESCRIPTION
  The function decides the low priority signaling bit's value.
  ie
  Low priority signaling bit will be set if UE is configured for
  Low priority signaling with the below exceptions
 
  Low priority signaling will not be set for access Class from the SIM is 11-15
   
PARAMETERS
  ds_pdn_mgr_s - PDN context pointer
 
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_mgr_get_low_priority_signaling_bit
(
  ds_pdn_mgr_s       *pdn_mgr_p
);

/*===========================================================================
FUNCTION  DS_PDN_MGR_SYS_IOCTL_CBACK

DESCRIPTION
  The function will be registered with ds_sys_ioctl_registeri_cback() during init
  and gets called when client query for this
   
PARAMETERS
  ioctl_name ioctl name 
  arg_val_ptr used to return thr ewquested value memory for this pointer will be 
              maintained by clients
  ps_error    used to return if there is any error.
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.

===========================================================================*/

int16 ds_pdn_mgr_sys_ioctl_cback
(
  ds_sys_ioctl_enum_type   ioctl_name,
  void                     *arg_val_ptr,
  int16                    *ps_errno
);

/*===========================================================================
FUNCTION  DS_PDN_MGR_PDN_IOCTL_CBACK

DESCRIPTION
  The function will be registered with FWK during init
  and gets called when client query for this
   
PARAMETERS
  PDN pointer
  IP type
  ioctl_name ioctl name 
  arg_val_ptr used to return thr requested value memory for this pointer will be 
              maintained by clients
  ps_error    used to return if there is any error.
  
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.
===========================================================================*/

int ds_pdn_mgr_pdn_ioctl_cback
(
 ds_pdn_mgr_s            *pdn_mgr_p,
 ip_addr_enum_type        ip_type,
 ds_pdn_ioctl_type        ioctl_name,
 void                    *argval_ptr,
 sint15                  *ps_errno
);

/*===========================================================================
  FUNCTION DS_PDN_MGR_CHNG_BYTE_ORDER
  
  DESCRIPTION
    Changes the byte order
    
  PARAMETERS  
    uint32 containing the original byte order
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    unit32 containing the changed (reverse) byte order
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/

uint32 ds_pdn_mgr_change_byte_order 
(
  uint32 orig_byte_order
);

/*===========================================================================

FUNCTION DS_PDN_SET_SIP_SERV_ADDR

DESCRIPTION
  This function returns the list of sip server addresses stored in the iface.

PARAMETERS
  fwk_inst:        ptr to fwk control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.   
  pdn_instance		Instance of the PDN V4 or V6

RETURN VALUE
  0 Success
 -1 Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_set_sip_serv_addr
(
  ds_fwk_s_type                 *fwk_inst,
  ip_addr_type                  *sip_serv_addr_ptr_array,
  uint32                         count,
  ds_pdn_mgr_instance_type_e     pdn_instance
);


/*===========================================================================
FUNCTION DS_PDN_MGR_GET_SIP_SERV_ADDR

DESCRIPTION
  This function returns the list of sip server addresses stored in the instance.

PARAMETERS
  tinst_p: ptr to interface control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_get_sip_serv_addr
(
  ds_pdn_mgr_s                *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e   inst_type,
  ps_ip_addr_type             *sip_serv_addr_ptr_array,
  uint8                       *count
);


/*===========================================================================
FUNCTION      DS_PDN_PROCESS_RESTORE_BEARER_CTXT_RSP_CMD

DESCRIPTION   This function processes the DS_CM_CALL_RESTORE_BEARER_CTXT_RSP_CMD
              from Call Manager. This command is notification of bearer setup
              by network.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_process_restore_bearer_ctxt_rsp_cmd
(
  ds_command_type  *cmd_ptr
);
#ifdef FEATURE_NBIOT_NTN

/*===========================================================================
FUNCTION      DS_PDN_MGR_SET_NTN_RESTORATION_IN_PROGRESS

DESCRIPTION   This function will set if NTN is in progress.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mgr_set_ntn_restoration_in_progress
(
  boolean  flag
);
/*===========================================================================
FUNCTION      DS_PDN_MGR_IS_NTN_RESTORATION_IN_PROGRESS

DESCRIPTION   This function will return if NTN is in progress or not

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean  ds_pdn_mgr_is_ntn_restoration_in_progress( void );
#endif /* FEATURE_NBIOT_NTN */

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_RESOLVEDAPN_FOR_DEFAULTPDN

DESCRIPTION
  This function is used to return the resolved apn name for default 
  pdn context

PARAMETERS   :
  char** - Resolved APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_pdn_mgr_get_resolvedapn_for_defaultpdn
(
  char** resolved_apn
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_SET_CM_EV_MASK

DESCRIPTION   This function sets event mask for various event types like SS,
              PH & CALL events.

PARAMETERS    CM event Type.
              bit256 mask pointer to be set.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_set_cm_ev_mask
(
  dssysmgr_cm_event_type event,
  bit_mask_256_t         *mask_ptr
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_NOTIFY_CB

DESCRIPTION   Call back function from Sysmgr for CM events

PARAMETERS    CM event Type.
              CM event common info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds_pdn_mgr_notify_cb
(
  dssysmgr_cm_event_type ev_type,
  dssysmgr_cm_common_event_info cm_info
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_HANDLE_PH_EV_CB

DESCRIPTION   Call back function from Sysmgr for CM PH events


PARAMETERS    PH event Type.
              PH info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_handle_ph_ev_cb
(
  cm_ph_event_e_type         event,
  const cm_ph_info_s_type   *event_ptr
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_PH_EV_CMD_HDLR

DESCRIPTION   

PARAMETERS    CM event Type.
              CM event common info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_ph_ev_cmd_hdlr
(
  ds_command_type  * cmd_ptr
);

/*===========================================================================
FUNCTION DS_PDN_MGR_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function detects if any of following profile params has changed or not
  1.  APN disabled flag
  2.  APN Name
  3.  Maximum PDN connections per block
  4.  Maximum PDN connection wait timer
  5.  PDN request wait timer
  6.  APN bearer
  7.  APN class
  8.  PDP Type
 
  If any of those above params have changed then it calls another which handles
  PDN disconnect processing if any PDN is up with the changed profile
 
PARAMETERS 
  prof_num:                 Profile whose contents have changed
  prf_data:                 Profile specific data (mask, new values ....)
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_apn_param_change_cmd_hdlr
(
  uint16                           prof_num,
  ds_profile_type_s                prof
);

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_APN_PARAM_CHG_TMR_EXP_CMD

DESCRIPTION
The function is called when the apn param chg timer expires, This would imply that 
the client will not tear down the call. IT will be MH's responsibility 
to clean up the call. 

PARAMETERS
  pdn_index - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_mgr_process_apn_param_chg_tmr_exp_cmd
(
  uint8           pdn_index
);

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_ALL_PDN_CONTEXT_PROFILES

DESCRIPTION
  This function returns the list of all primary Profile number that are 
  currently active.

PARAMETERS   :
  profile_list_ptr  - list of profile numbers which are active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_pdn_mgr_get_all_pdn_context_profiles
(
  ds_prim_profile_list_type * profile_list_ptr
);

/*===========================================================================
FUNCTION ds_pdn_mgr_profile_change_cb

DESCRIPTION
  The function is called when the profile param changes. CB is recieved
  from profile module, so post a command to DS task.

PARAMETERS
  Profile number
  Mak for changed parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_profile_change_cb
(
  uint8                         profile_num,
  uint64                        mask,
  boolean                       apn_param_changed
);

/*===========================================================================
FUNCTION ds_pdn_mgr_profile_change_cmd_hdlr

DESCRIPTION
  The function is called when the profile param changes as 
  cmd processing in DS task.

PARAMETERS
  Changed profile param staructure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_profile_change_cmd_hdlr
(
  ds_pdn_mgr_profile_change_s  *change_ptr
);

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTX_GET_THR0TTLE_INFO

DESCRIPTION
  Handler for PS_SYS_IOCTL_PDN_THROTTLE_INFO. Retrieves throttle info
  for all apns for the current plmn, saved in the Per PLMN throttle
  table.

PARAMETERS 
  arg_val_ptr: Argument to be populated
  subs_id:     Subscription id
  ps_errno:    Err number if IOCTL cant be handled
   
  
 
DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
 
SIDE EFFECTS
  None.
===========================================================================*/ 
int16 ds_3gpp_pdn_cntx_get_throttle_info
(
  void                    *arg_val_ptr,
  int16                   *ps_errno
);


/*==============================================================================
FUNCTION DS_PDN_MGR_SET_t3448_STAUS_IND

DESCRIPTION
  This function is used to update the status of t3448 from NAS

PARAMETERS   :
     t3448_timer status frrom NAS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void ds_pdn_mgr_set_T3448_staus_ind
(
  emm_timer_t3448_status_ind_type* t3448_status
);

/*==============================================================================
FUNCTION DS_PDN_MGR_SET_BARRING_STAUS_IND

DESCRIPTION
  This function is used to update the status of t302 from NAS

PARAMETERS   :
     t302_timer status frrom NAS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void ds_pdn_mgr_set_barring_staus_ind
(
  emm_timer_barring_status_ind_type* barring_status
);

/*==============================================================================
FUNCTION DS_PDN_MGR_IS_T3448_RUNNING

DESCRIPTION
  This function is used to check whether t3448 timer is running or not

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
  TRUE if its running 
  FASE if its not running

SIDE EFFECTS
  None
==============================================================================*/
boolean ds_pdn_mgr_is_T3448_running();

/*==============================================================================
FUNCTION DS_PDN_MGR_IS_BARRING_TIMER_RUNNING

DESCRIPTION
  This function is used to check whether t3448 timer is running or not

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
  TRUE if its running 
  FASE if its not running

SIDE EFFECTS
  None
==============================================================================*/
boolean ds_pdn_mgr_is_barring_timer_running();

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_ACTIVE_BARRING_BMASK

DESCRIPTION
  This function is used to get the actvie barring bmask

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
bmask

SIDE EFFECTS
  None
==============================================================================*/
uint32 ds_pdn_mgr_get_active_barring_bmask();

/*===========================================================================
FUNCTION      DS_PDN_MGR_STORE_BEARER_CONTEXT

DESCRIPTION
  This function is used to store the bearer context information in the PDN
  context.

PARAMETERS
    pdn_mgr_p     - This is the PDN context inside which we want to store
                            the bearer
    bearer_mgr_p - This is the pointer to the bearer context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on successfully storing the bearer info
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_store_bearer_context
(
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_pdn_mgr_update_pco

DESCRIPTION
  This function is used to update PCO. This inturn invokes
  ds_pdn_cntxt_decodeProtocolParams to perform the actual update.
  
PARAMETERS
    Bearer ptr
    PCO
    ePCO    
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_update_pco
(
  ds_bearer_mgr_s                  *bearer_mgr_p,
  protocol_cfg_options_T           *config_options_p,
  ext_protocol_cfg_options_T       *ext_protocol_conf_opt_p
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_REMOVE_BEARER_CONTEXT

DESCRIPTION
  This function is used to remove the bearer context information in the PDN
  context.

PARAMETERS
    pdn_mgr_p     - This is the PDN context from which we want to remove
                            the bearer
    bearer_mgr_p - This is the pointer to the bearer context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on successfully removing the bearer info
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_remove_bearer_context
(
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION      DS_PDN_MGR_GET_COMPANION_INST

DESCRIPTION
  Fetches the other instance from the current instance pointer.

PARAMETERS
  inst_p       - Pointer to the current instance.  

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  companion_inst_p - Pointer to the other instance.  

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_inst_type_s* ds_pdn_mgr_get_companion_inst
(
  ds_pdn_mgr_s              *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e inst_type
);
/*===========================================================================
FUNCTION DS_PDN_MGR_READ_NAS_CONFIG_FILE
 
DESCRIPTION
  This function reads NAS config file
 

PARAMETERS 
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  Boolean 
 
SIDE EFFECTS 
  None   
===========================================================================*/
boolean ds_pdn_mgr_read_nas_config_file
(
  ds3g_mmgsdi_tag_e_type    tag_id,
  void                      *tag_value_p
);

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_CURRENT_ACTIVE_CONTEXT

DESCRIPTION
  This function returns the number of active bearer contexts.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of active bearer contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_mgr_get_current_active_context();

/*===========================================================================
FUNCTION ds_pdn_mgr_local_teardown

DESCRIPTION
  This function is used to teardown calls when LPM transition occurs
 
PARAMETERS
  void
 
DEPENDENCIES
  void

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_pdn_mgr_local_teardown
(
  void
);

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_t3448_TIMER_VALUE

DESCRIPTION
  This function is used to get the t3448 timer value

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
 Timer value

SIDE EFFECTS
  None
==============================================================================*/
dword ds_pdn_mgr_get_t3448_timer_value();

/*===========================================================================
FUNCTION ds_pdn_mgr_execute_cached_deactivation()

DESCRIPTION

PARAMETERS
  pdn context
  bearer context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_execute_cached_deactivation
(
  ds_pdn_mgr_s          *pdn_mgr_p,
  ds_bearer_mgr_s       *bearer_mgr_p
);


/*===========================================================================
FUNCTION      DS_PDN_CCMI_REGISTER_WITH_CFCM_EVENTS

DESCRIPTION   Registers CCM with CFCM to get CFCM events for Thermal monitors.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_ccmi_register_with_cfcm_events (void);


/*===========================================================================
FUNCTION      DS_PDN_MGR_REGISTER_MEM_EVENT_CALLBACKS

DESCRIPTION   This function will register call functions with DSM memory events

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mgr_register_mem_event_callbacks( void );


#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
/*===========================================================================
FUNCTION ds_pdn_evaluate_gps_high_priority_search_flow_for_all_bearers

DESCRIPTION
  This function cycles through the bearer context table, finds all
  bearers evaluates the flow as applicable when GPS high prioirt seach
  started/stoped.
  
PARAMETERS
  enable_flow 
  True enable the flow
  False Disable the flow 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_evaluate_gps_high_priority_search_flow_for_all_bearers
(
  boolean  enable_flow
);
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */

#ifdef FEATURE_NBIOT_NTN
/*===========================================================================
FUNCTION      ds_pdn_mgr_set_ntn_gnss_status

DESCRIPTION   This function will used to set the ntn gnss status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mgr_set_ntn_gnss_status(boolean status);


/*===========================================================================
FUNCTION      ds_pdn_mgr_get_ntn_gnss_status

DESCRIPTION   This function will used to get the ntn gnss status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_mgr_get_ntn_gnss_status();

#endif /* FEATURE_NBIOT_NTN */
#endif /* DS_PDN_MANAGER_H */
