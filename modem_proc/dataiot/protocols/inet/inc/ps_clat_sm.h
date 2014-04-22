#ifndef __PS_CLAT_SM_H_
#define __PS_CLAT_SM_H_
/*===========================================================================

                           P S _ C L A T _ S M . H
                   
DESCRIPTION
  The header file for CLAT State Machine.

EXTERNALIZED FUNCTIONS

Copyright (c) 2014-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_clat_sm.h#1 $
  $Author: pwbldsvc $ $DateTime: 2021/01/08 07:47:06 $

--------    ---    ----------------------------------------------------------
12/16/15    kc     Add changes to event handling in XLAT.
09/29/15    ds     Added net down reason when clat is down
02/04/15    ssh    Introduced failover v6 iface in passthrough mode bringup
02/03/15    skc    Added changes for CLAT QTF test 
10/10/14    ssh    IP Translation Engine 464XLAT   
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
//#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_in.h"
#include "dssdns.h"
#include "ds_fwk.h"

/*===========================================================================

                                CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum length for the Provider Translator (CLAT peer) FQDN 
---------------------------------------------------------------------------*/
#define FQDN_MAX_LEN 128

/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_STATE_TYPE

DESCRIPTION
  Enum of CLAT SM states
---------------------------------------------------------------------------*/
#define CLAT_SM_MAX_STATE 3 
typedef enum
{
  CLAT_SM_DOWN        = 0,
  CLAT_SM_CONFIGURING = 1,
  CLAT_SM_TRANSLATE   = 2
} ps_clat_sm_state_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_EVENT_TYPE

DESCRIPTION
  Enum of SM events
---------------------------------------------------------------------------*/
#define CLAT_SM_MAX_EV 5
typedef enum
{
  CLAT_SM_START_EV                     = 0,
  CLAT_SM_STOP_EV                      = 1,
  CLAT_SM_IPV6_IFACE_PREFIX_UPDATE_EV  = 2,
  CLAT_SM_PLAT_AVAILABLE_EV            = 3,
  CLAT_SM_PLAT_UNAVAILABLE_EV          = 4,
} ps_clat_sm_event_type;

/*---------------------------------------------------------------------------
TYPEDEF CLAT_SM_INDICATION_TYPE

DESCRIPTION
  This is the type of indications that clients can register for from the SM.
---------------------------------------------------------------------------*/
typedef enum
{ 
  CLAT_SM_MIN_IND     = 0,
  CLAT_SM_DOWN_IND    = 0,
  CLAT_SM_UP_IND      = 1,
  CLAT_SM_MAX_IND
} clat_sm_indication_type;



/*---------------------------------------------------------------------------
  build IPv4 Address - in NETWORK order
---------------------------------------------------------------------------*/
#define IPV4( arg1, arg2, arg3, arg4 )                                      \
  ((uint32)( (((uint32)(arg4 & 0xff)) << 24)                                \
           | (((uint32)(arg3 & 0xff)) << 16)                                \
           | (((uint32)(arg2 & 0xff)) << 8)                                 \
           | (((uint32)(arg1 & 0xff)))))


/*---------------------------------------------------------------------------
  Maximum number of CLAT instances (PDNs) supported
---------------------------------------------------------------------------*/
#define CLAT_MAX_INSTANCES PS_SYS_MAX_AVAIL_PDNS 

/*---------------------------------------------------------------------------
  IPv4 Well-Known addresses (WKA) for use in PLAT Discovery. RFC 7050.
---------------------------------------------------------------------------*/
#define CLAT_IPV4_WKA_ONE  IPV4(192, 0, 0, 170)
#define CLAT_IPV4_WKA_TWO  IPV4(192, 0, 0, 171)

/*---------------------------------------------------------------------------
  IPv4 Service Continuity Prefix. 
  RFC 7335 recommends this to be 192.0.0.0/29 (mask 255.255.255.248), but 
  that only allows 6 usable addresses total (192.0.0.1 - 192.0.0.6). 
  We need a total of 16 (2 per PDN), so we use 192.0.0.0/27 
  (mask 255.255.255.224), which allows 30 addresses, 192.0.0.1 - 192.0.0.30.
  11111111.11111111.11111111.11100000b
---------------------------------------------------------------------------*/
#define CLAT_ADDR_SUBNET_MASK         IPV4(255, 255, 255, 224) 
#define CLAT_SUBNET_ADDR              IPV4(192, 0, 0, 0)
#define CLAT_ADDR_SUBNET_COMMON_BITS  27   

/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/
typedef struct clat_addr_s
{
  ps_ip_addr_type own_ip_addr;
  ps_ip_addr_type te_ip_addr;
  boolean         is_used;
} ps_clat_addr_type;

/*===========================================================================


/*---------------------------------------------------------------------------
TYPEDEF PLAT_PROVISIONING_MODE_ENUM_TYPE

DESCRIPTION
  Enum of PLAT provisioning mode
---------------------------------------------------------------------------*/
typedef enum
{
  LOCALLY_PROVISIONED = 0,
  DISCOVER_VIA_DNS64  = 1
} plat_provisioning_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF ICMP_TX_ERR_MODE_ENUM_TYPE

DESCRIPTION
  Enum of CLAT ICMP error mode
---------------------------------------------------------------------------*/
typedef enum
{
  DISABLED      = 0,
  RATE_LIMITED  = 1,
  ENABLED       = 2
} icmp_tx_err_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_DNS_CB_INFO_TYPE

DESCRIPTION
  Structure to hold DNS callback information
---------------------------------------------------------------------------*/
typedef struct ps_clat_sm_dns_cb_info_type_s
{
  dss_dns_session_mgr_handle_type session_handle;
  dss_dns_query_handle_type       query_handle;
  dss_dns_api_type_enum_type      api_type;
  uint16                          num_records;
} ps_clat_sm_dns_cb_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_DNS_INFO_TYPE

DESCRIPTION
  Structure to hold DNS information
---------------------------------------------------------------------------*/
typedef struct ps_clat_sm_dns_info_type_s
{
  dss_dns_session_mgr_handle_type dss_dns_session_handle;
  dss_dns_query_handle_type       dss_dns_query_handle;
  ps_clat_sm_dns_cb_info_type     dns_cb_info;
} ps_clat_sm_dns_info_type;

/*---------------------------------------------------------------------------
TYPEDEF CLAT_SM_CBACK_TYPE

DESCRIPTION
  This is the type for the callbacks which can be registered for indications

PARAMETERS
  instance: the instance that caused the indication.
  ind: the indication that occurred (will only be posted on an edge - e.g. UP
       will only happen when not already up).
  user_data: data that was given at registration time.

RETURN VALUE
  None
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
TYPEDEF PS_CLAT_SM_CB_TYPE

DESCRIPTION
  Control block for CLAT state machine
---------------------------------------------------------------------------*/
struct ps_clat_sm_cb_s;
typedef struct ps_clat_sm_cb_s ps_clat_sm_type;

typedef void (*clat_sm_cback_type)
(
  struct ps_clat_sm_cb_s *instance,
  clat_sm_indication_type ind,
  void *user_data
);

struct ps_clat_sm_cb_s 
{
  ps_clat_sm_state_type     state;               /* Current SM state       */
  ds_fwk_s_type             *fwk_inst;           /* DS Fwk Instance        */
  ps_clat_addr_type   v4_addrs;            /* V4 addrs for this PDN  */
  plat_provisioning_mode_enum_type plat_mode;    /* PLAT provisioning mode */
  struct ps_in6_addr        plat_addr;           /* PLAT address           */
  ipv6_prefix_len_enum_type plat_addr_prefix_len;/* PLAT addr prefix len   */
  char                      plat_fqdn[FQDN_MAX_LEN]; /* PLAT FQDN          */
  struct ps_in6_addr        own_ip6_addr;        /* Own IPv6 address       */
  ps_clat_sm_dns_info_type dns_info;             /* PLAT DNS info          */
  ps_clat_sm_type        * this_sm_ptr;          /* self pointer           */
  boolean                  is_dns_query;    /* Boolean for DNS type check */
  struct
  {
    clat_sm_cback_type cback;
    void             *data;
  } ind;
};

/*---------------------------------------------------------------------------
TYPEDEF PLAT_CONFIG_INFO_TYPE

DESCRIPTION
  This structure stores the PLAT config information.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  plat_provisioning_mode_enum_type mode;
  char                             prefix[PS_IN6_ADDRSTRLEN];
  ipv6_prefix_len_enum_type        prefix_len;
  char                             fqdn[FQDN_MAX_LEN];
} plat_config_info_type;

/*---------------------------------------------------------------------------
TYPEDEF CLAT_CONFIG_INFO_TYPE

DESCRIPTION
  This structure stores the CLAT config information.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8                       version;
  boolean                     enabled;                     /* CLAT ON/OFF? */
  plat_config_info_type       plat_config_info;            /* PLAT info    */
  boolean                     ul_ignore_v4_tos;            /* UL v4 TOS    */ 
  uint8                       dl_v4_tos_val;               /* DL v4 TOS    */
  boolean                     ul_no_df_insert_v6_frag_hdr; /* frag hdr ?   */
  boolean                     ul_udp_process_zero_cksum;   /* udp 0 cksum? */
  icmp_tx_err_mode_enum_type  icmpv4_err_mode;             /* v4 err tx?   */
  icmp_tx_err_mode_enum_type  icmpv6_err_mode;             /* v6 err tx?   */          
} clat_config_info_type;

/*===========================================================================

                            MACROS

===========================================================================*/
#define CLAT_INST_INITIALIZED(inst_ptr)                                    \
  ((inst_ptr != NULL) && (inst_ptr->this_sm_ptr == inst_ptr))

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION  PS_CLAT_UL_IGNORE_V4_TOS()

DESCRIPTION
  Indicates if IPv4 TOS bit should be ignored on UL and IPv6 traffic class
  should be set to 0 in the IPv6 translated packet.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_ul_ignore_v4_tos
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_DL_V4_TOS_VAL()

DESCRIPTION
  Indicates what value to use for the IPv4 TOS bit on DL IPv4 translated 
  packets. 0 indicates that traffic class value from the incoming IPv6 
  packet should be used as-is.

DEPENDENCIES
  None

RETURN VALUE
  0: Set IPv4 TOS to the traffic class value from IPv6 packet
  other values: Set IPv4 TOS to return value

SIDE EFFECTS
  None
===========================================================================*/
uint8 ps_clat_dl_v4_tos_val
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_UL_NO_DF_INSERT_V6_FRAG_HDR()

DESCRIPTION
  Indicates if an IPv6 fragment header should be inserted in the IPv6 
  translated packet if the incoming IPv4 packet has DF bit unset.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_ul_no_df_insert_v6_frag_hdr
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_UL_UDP_PROCESS_ZERO_CKSUM()

DESCRIPTION
  Indicates if UL IPv4 UDP packets with zero checksum should be processed
  (translated to IPv6 and checksum updated) or dropped aftr logging an 
  error.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_ul_udp_process_zero_cksum
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_ICMPV4_ERR_MODE()

DESCRIPTION
  Returns the error mode type configured for ICMPv4 error packet generation

DEPENDENCIES
  None

RETURN VALUE
  icmp_tx_err_mode_enum_type

SIDE EFFECTS
  None
===========================================================================*/
icmp_tx_err_mode_enum_type ps_clat_icmpv4_err_mode
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_ICMPV6_ERR_MODE()

DESCRIPTION
  Returns the error mode type configured for ICMPv6 error packet generation

DEPENDENCIES
  None

RETURN VALUE
  icmp_tx_err_mode_enum_type

SIDE EFFECTS
  None
===========================================================================*/
icmp_tx_err_mode_enum_type ps_clat_icmpv6_err_mode
(
  void
);

/*===========================================================================
FUNCTION  PS_CLAT_POWERUP()

DESCRIPTION
  Performs CLAT powerup initialization

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_powerup
(
  void
);


/*===========================================================================
FUNCTION  PS_CLAT_SM_POST_EVENT

DESCRIPTION
  This functions posts events to the CLAT state machine (which runs in DCC
  task)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_sm_post_event
(
  ps_clat_sm_type       *instance,
  ps_clat_sm_event_type event,
  void                  *event_data
);

int clat_sm_create
(
  ps_clat_sm_type *instance,
  ds_fwk_s_type   *fwk_inst_ptr,
  ds_pdn_mgr_s    *pdn_ptr
);

int ps_clat_get_addrs
(
  ps_clat_addr_type * addr_ptr
);
 
int ps_clat_release_addrs
(
  ps_clat_addr_type * addr_ptr
); 

//#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* __PS_CLAT_SM_H_ */
