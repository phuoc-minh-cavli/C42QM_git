
#ifndef DS_DEFSI_H
#define DS_DEFSI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        DS _ D E F S . H

DESCRIPTION
  Header containing  names that need to be used in other contexts

Copyright (c) 2018 - 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/api/ds_defsi.h#2 $ : //source/qcom/qct/modem/api/datacommon/main/latest/DS_defs.h#41 $ $DateTime: 2020/08/28 01:39:46 $ $Author: pwbldsvc $

when        who        what, where, why
--------    ---    ---------------------------------------------------------- 
01/30/18    svaka    created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue.h"
#include "ds_errno.h"
#include "ps_in.h"
#include "ds_sys.h"
#include "ds_ipfltr_defs.h"
#include "ds_defs.h"
#include "dss.h"
#include "ps_mem_ext.h"


/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
DS_FWK_INVALID_ID - Id which does not identify any valid pdn
---------------------------------------------------------------------------*/
#define DS_FWK_INVALID_ID  -1

/*---------------------------------------------------------------------------
  Maximum number of filters allowed in each QOS request. UMTS can fit atmost
  8 filters in one message. By restricting this to 8, UMTS need not worry
  about whether network rejected previous message.
---------------------------------------------------------------------------*/
#define DS_MAX_FLTR_PER_REQ  16
#define DS_MAX_ADDITIONAL_FLOWS_PER_REQ 6            /* 8 total: min/max/other
                                                     8 for rx and 8 for tx */

/*---------------------------------------------------------------------------
  Define minimum MTU values for V4, V6 and NON IP pdns. An pdn can never have a
  value lower than these values
---------------------------------------------------------------------------*/
#define DS_MIN_V4_MTU   68
#define DS_MIN_V6_MTU  1280
#define DS_MIN_NON_IP_MTU  128

/*---------------------------------------------------------------------------
  MAX_IPV6_PREFIXES - the maximum number of IPv6 prefixes we can support
---------------------------------------------------------------------------*/
#define DS_MAX_IPV6_PREFIXES 1
#define DS_DEFAULT_V6_INDEX  0

/*---------------------------------------------------------------------------
  DS_MAX_IPV6_IIDS - the maximum number of IPv6 iids we support per interface,
                  including internal addresses and those from ext. devices
---------------------------------------------------------------------------*/
#define DS_MAX_IPV6_IIDS 20

/*---------------------------------------------------------------------------
  DS_DEFAULT_UNUSED_IPV6_PRIV_ADDR_TIMEOUT - default (2 min, or 120,000 ms)
    The maximum time to wait for an application to bind to the privacy addr
    before deleteing it.
---------------------------------------------------------------------------*/
#define DS_DEFAULT_UNUSED_IPV6_PRIV_ADDR_TIMEOUT 120000

/*---------------------------------------------------------------------------
  DS_MIN_IPV6_VALID_LIFETIME - The minimum time (7200 sec) that the IPv6 valid
    lifetime can be set to.
---------------------------------------------------------------------------*/
#define DS_MIN_IPV6_VALID_LIFETIME 7200

/*---------------------------------------------------------------------------
  DS_MAX_IPV6_ADDRS - the total number of IPv6 addresses supported per interface
---------------------------------------------------------------------------*/
#define DS_MAX_IPV6_ADDRS (DS_MAX_IPV6_PREFIXES * DS_MAX_IPV6_IIDS)

/*---------------------------------------------------------------------------
TYPEDEF DS_EXTENDED_INFO_CODE_ENUM_TYPE

DESCRIPTION
  Extended reason codes to pass additional information with the
  pdn/flow/phys_link events.  If extended information is not available,
  the default value DS_EIC_NOT_SPECIFIED should be used.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EIC_NOT_SPECIFIED = 0,

  DS_EIC_QOS_INTERNAL_MIN                     = 1,
  DS_EIC_QOS_INVALID_PARAMS                   = DS_EIC_QOS_INTERNAL_MIN + 2,
  DS_EIC_QOS_INTERNAL_INVALID_PARAMS          = DS_EIC_QOS_INVALID_PARAMS,
  DS_EIC_QOS_INTERNAL_CALL_ENDED              = DS_EIC_QOS_INTERNAL_MIN + 3,
  DS_EIC_QOS_INTERNAL_ERROR                   = DS_EIC_QOS_INTERNAL_MIN + 4,
  DS_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES     = DS_EIC_QOS_INTERNAL_MIN + 5,
  DS_EIC_QOS_TIMED_OUT_OPERATION              = DS_EIC_QOS_INTERNAL_MIN + 6,
  DS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE      = DS_EIC_QOS_INTERNAL_MIN + 7,
  DS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS      = DS_EIC_QOS_INTERNAL_MIN + 8,
  DS_EIC_QOS_MODEM_RESTART                    = DS_EIC_QOS_INTERNAL_MIN + 9,
  DS_EIC_QOS_INTERNAL_MAX,

  DS_EIC_QOS_NETWORK_MIN                      = 128,
  DS_EIC_QOS_NOT_SUPPORTED                    = DS_EIC_QOS_NETWORK_MIN + 1,
  DS_EIC_QOS_NOT_AVAILABLE                    = DS_EIC_QOS_NETWORK_MIN + 2,
  DS_EIC_QOS_NOT_GUARANTEED                   = DS_EIC_QOS_NETWORK_MIN + 3,
  DS_EIC_QOS_INSUFFICIENT_NET_RESOURCES       = DS_EIC_QOS_NETWORK_MIN + 4,
  DS_EIC_QOS_AWARE_SYSTEM                     = DS_EIC_QOS_NETWORK_MIN + 5,
  DS_EIC_QOS_UNAWARE_SYSTEM                   = DS_EIC_QOS_NETWORK_MIN + 6,
  DS_EIC_QOS_REJECTED_OPERATION               = DS_EIC_QOS_NETWORK_MIN + 7,
  DS_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED      = DS_EIC_QOS_NETWORK_MIN + 8,
  DS_EIC_QOS_NETWORK_CALL_ENDED               = DS_EIC_QOS_NETWORK_MIN + 9,
  DS_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE        = DS_EIC_QOS_NETWORK_MIN + 10,
  DS_EIC_QOS_NETWORK_L2_LINK_RELEASED         = DS_EIC_QOS_NETWORK_MIN + 11,
  DS_EIC_QOS_NETWORK_L2_LINK_REESTAB_REJ      = DS_EIC_QOS_NETWORK_MIN + 12,
  DS_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND      = DS_EIC_QOS_NETWORK_MIN + 13,
  DS_EIC_QOS_NETWORK_UNKNOWN_CAUSE_CODE       = DS_EIC_QOS_NETWORK_MIN + 14,
  DS_EIC_QOS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED
                                              = DS_EIC_QOS_NETWORK_MIN + 15,
  DS_EIC_QOS_NETWORK_NULL_PROFILE_SUGGESTED   = DS_EIC_QOS_NETWORK_MIN + 16,
  DS_EIC_QOS_NETWORK_UE_NOT_AUTHORIZED        = DS_EIC_QOS_NETWORK_MIN + 17,
  DS_EIC_QOS_NETWORK_MAX,

  DS_EIC_NETWORK_MIN                          = 200,
  DS_EIC_NETWORK_NOT_SPECIFIED                = DS_EIC_NETWORK_MIN + 1,
  DS_EIC_NETWORK_BUSY                         = DS_EIC_NETWORK_MIN + 2,
  DS_EIC_NETWORK_RAB_FAILURE                  = DS_EIC_NETWORK_MIN + 3,
  DS_EIC_CLIENT_END                           = DS_EIC_NETWORK_MIN + 4,
  DS_EIC_NO_SRV                               = DS_EIC_NETWORK_MIN + 5,
  DS_EIC_CDMA_LOCK                            = DS_EIC_NETWORK_MIN + 6,
  DS_EIC_FADE                                 = DS_EIC_NETWORK_MIN + 7,
  DS_EIC_INTERCEPT                            = DS_EIC_NETWORK_MIN + 8,
  DS_EIC_REORDER                              = DS_EIC_NETWORK_MIN + 9,
  DS_EIC_REL_NORMAL                           = DS_EIC_NETWORK_MIN + 10,
  DS_EIC_REL_SO_REJ                           = DS_EIC_NETWORK_MIN + 11,
  DS_EIC_INCOM_CALL                           = DS_EIC_NETWORK_MIN + 12,
  DS_EIC_ALERT_STOP                           = DS_EIC_NETWORK_MIN + 13,
  DS_EIC_ACTIVATION                           = DS_EIC_NETWORK_MIN + 14,
  DS_EIC_MC_ABORT                             = DS_EIC_NETWORK_MIN + 15,
  DS_EIC_MAX_ACCESS_PROBE                     = DS_EIC_NETWORK_MIN + 16,
  DS_EIC_PSIST_NG                             = DS_EIC_NETWORK_MIN + 17,
  DS_EIC_UIM_NOT_PRESENT                      = DS_EIC_NETWORK_MIN + 18,
  DS_EIC_ACC_IN_PROG                          = DS_EIC_NETWORK_MIN + 19,
  DS_EIC_ACC_FAIL                             = DS_EIC_NETWORK_MIN + 20,
  DS_EIC_RETRY_ORDER                          = DS_EIC_NETWORK_MIN + 21,
  DS_EIC_CCS_NOT_SUPPORTED_BY_BS              = DS_EIC_NETWORK_MIN + 22,
  DS_EIC_NO_RESPONSE_FROM_BS                  = DS_EIC_NETWORK_MIN + 23,
  DS_EIC_REJECTED_BY_BS                       = DS_EIC_NETWORK_MIN + 24,
  DS_EIC_INCOMPATIBLE                         = DS_EIC_NETWORK_MIN + 25,
  DS_EIC_ACCESS_BLOCK                         = DS_EIC_NETWORK_MIN + 26,
  DS_EIC_ALREADY_IN_TC                        = DS_EIC_NETWORK_MIN + 27,
  DS_EIC_USER_CALL_ORIG_DURING_GPS            = DS_EIC_NETWORK_MIN + 28,
  DS_EIC_USER_CALL_ORIG_DURING_SMS            = DS_EIC_NETWORK_MIN + 29,
  DS_EIC_USER_CALL_ORIG_DURING_VOICE_CALL     = DS_EIC_NETWORK_MIN + 30,
  DS_EIC_REDIR_OR_HANDOFF                     = DS_EIC_NETWORK_MIN + 31,
  DS_EIC_ACCESS_BLOCK_ALL                     = DS_EIC_NETWORK_MIN + 32,
  DS_EIC_THERMAL_EMERGENCY                    = DS_EIC_NETWORK_MIN + 33,
  DS_EIC_CALL_ORIG_THROTTLED                  = DS_EIC_NETWORK_MIN + 34,
  DS_EIC_OFFLINE                              = DS_EIC_NETWORK_MIN + 35,
  DS_EIC_EMERGENCY_MODE                       = DS_EIC_NETWORK_MIN + 36,
  DS_EIC_PHONE_IN_USE                         = DS_EIC_NETWORK_MIN + 37,
  DS_EIC_INVALID_MODE                         = DS_EIC_NETWORK_MIN + 38,
  DS_EIC_INVALID_SIM_STATE                    = DS_EIC_NETWORK_MIN + 39,
  DS_EIC_NO_COLLOC_HDR                        = DS_EIC_NETWORK_MIN + 40,
  DS_EIC_CALL_CONTROL_REJECTED                = DS_EIC_NETWORK_MIN + 41,
  DS_EIC_NETWORK_MAX

} ds_extended_info_code_enum_type;
/*==========================================================================
DS Framework Policy info: This structure stores the policy related params such apn and ip version
===========================================================================*/
typedef struct
{
  ip_addr_enum_type            family;
  uint8                        length;
  char                         name[DS_SYS_MAX_APN_LEN];
  ds_apn_enum_type             apn_type;
  boolean                      mo_excp_data_cap;
  dss_profile_auth_type_t      authentication_params;
  void*                        user_info_ptr;
} ds_policy_info_type;

  
/*---------------------------------------------------------------------------
TYPEDEF DS_IPFLTR_CLIENT_ID_ENUM_TYPE

DESCRIPTION
  Client Ids for the users of filtering library.
  NOTE : INPUT filter clients must have a client ID between 0 and
         FLTR_CLIENT_INPUT_MAX
---------------------------------------------------------------------------*/
typedef enum
{
  FLTR_CLIENT_SOCKETS  ,  /* Incoming pkt filtering for sockets  */
  FLTR_CLIENT_QOS_INPUT  ,  /* Rx Pkt classification fltrs for QOS */
  FLTR_CLIENT_INPUT_MAX,
  FLTR_CLIENT_QOS_OUTPUT,        /* Tx Pkt classification fltrs for QOS */
  FLTR_CLIENT_IPSEC_OUTPUT,      /* IPSEC filters for output processing */
  FLTR_CLIENT_CLAT,              /* CLAT filters */
  FLTR_CLIENT_MO_EXCP,           /* MO Exceptional filters */  
  FLTR_CLIENT_PDN_SHARING,       /* PDN sharing filters */
  FLTR_CLIENT_MAX
} ds_ipfltr_client_id_enum_type;

/**
  @brief Structure that stores the IP config details of a given pdn
*/
typedef struct
{
  boolean         is_ip_cfg_valid; 
  /**< Flag to indicate validity of this structure contents */
  ps_ip_addr_type ip_addr;
  /**< V4/V6 Ip Address */
  uint32          v4_gateway_addr;
  /**< V4 Gateway Address */
  uint64          v6_gateway_iid;
  /**< V6 gateway IID */
  uint32          prefix_valid_life_time;
  /**< V6 Prefix Valid Life time value */
  uint32          prefix_pref_life_time;
  /**< V6 Prefix Preferred Life time value */
  uint16          router_life_time;
  /**< V6 Router Life time value */
  ip_addr_type    dns_addrs[DS_NUM_DNS_ADDRS];
  /**< V4/V6 DNS Addresses */
  ip_addr_type    sip_serv_addr_array[DS_MAX_SIP_SERVER_ADDRESSES];   
  /**< V4/V6 Sip Server Addresses */
  uint32          valid_sip_serv_addr_count;
  /**< Valid Sip Server addresses in the sip_serv_addr_array */
} ds_ip_config_type;


/*---------------------------------------------------------------------------
TYPEDEF DS_PDN_STATS_TYPE
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    IP packets received/transmitted
  -------------------------------------------------------------------------*/
    uint64  bytes_rx;
    uint64  bytes_tx;
    uint32  pkts_rx;
    uint32  pkts_dropped_rx;
    uint32  pkts_tx;
    uint32  pkts_dropped_tx;
} ds_stats_type;

/*---------------------------------------------------------------------------
  Data type used for DS_PDN_IOCTL_GET_STATS
---------------------------------------------------------------------------*/
typedef ds_stats_type ds_pdn_ioctl_stats_type;

typedef struct ds_fwk_s ds_fwk_s_type;

#endif /* DS_DEFSI_H */
