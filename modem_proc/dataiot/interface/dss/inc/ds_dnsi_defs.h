#ifndef PS_DNSI_DEFS_H
#define PS_DNSI_DEFS_H
/*===========================================================================

                       P S  _  D N S I  _  D E F S  .  H

DESCRIPTION

  The Data Services DNS module internal Header File. Contains declarations
  of enums, variables and control blocks used by the DNS subsystem.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATIONS AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_DATA_PS_DNS
#define FEATURE_DATA_PS_DNS
#endif

#ifdef FEATURE_DATA_PS
#ifdef FEATURE_DATA_PS_DNS


#include "dssdns.h"
#include "queue.h"
#include "ps_svc.h"
#include "err.h"
#include "msg.h"
#include "assert.h"
#include "ds_defsi.h"


/*===========================================================================

                               MACRO DECLARATIONS

===========================================================================*/
#ifndef isalpha
#define isalpha(a) ( (a >= 'A' && a <= 'Z') || (a >= 'a' && a <= 'z') )
#endif

#ifndef isalnum
#define isalnum(a) ( isalpha(a) || (a >= '0' && a <= '9') )
#endif

#ifdef FEATURE_Q_NO_SELF_QPTR
#define Q_DELETE( qptr, linkptr )        q_delete( (qptr), (linkptr) );
#else
#define Q_DELETE( qptr, linkptr )        q_delete( (linkptr) );
#endif /* FEATURE_Q_NO_SELF_QPTR */


/*===========================================================================

                            INTERNAL DATA DECLARATIONS

===========================================================================*/
#define PS_DNSI_PS_TIMER                2

typedef int32 ps_dnsi_resolver_handle_type;
#define PS_DNSI_RESOLVER_INVALID_HANDLE  (-1)

/*---------------------------------------------------------------------------
  Typedef for Query Class: Currently supported is only IN (internet) class.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_QUERY_CLASS_MIN    = 0,
  PS_DNSI_QUERY_CLASS_IN     = PS_DNSI_QUERY_CLASS_MIN,
  PS_DNSI_QUERY_CLASS_MAX    = 1
} ps_dnsi_query_class_enum_type;

/*---------------------------------------------------------------------------
  Enum for query type: currently supported query types are only A/AAAA/PTR
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_QUERY_TYPE_MIN      = 0,
  PS_DNSI_QUERY_TYPE_A        = PS_DNSI_QUERY_TYPE_MIN,
  PS_DNSI_QUERY_TYPE_AAAA     = 1,
  PS_DNSI_QUERY_TYPE_PTR      = 2,
  PS_DNSI_QUERY_TYPE_CNAME    = 3,
  PS_DNSI_QUERY_TYPE_MAX      = 4
} ps_dnsi_query_type_enum_type;

/*---------------------------------------------------------------------------
  Data structure for a Generic Resource Record
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_generic_rr_s_type
{
  q_link_type                      link;
  uint32                           ttl;
  char                             domain_name[DSS_DNS_MAX_DOMAIN_NAME_LEN]; //Should be char * we are an embedded system, memory is precious :-)
  unsigned char                  * rdata;
  int16                            rdata_len;
  ps_dnsi_query_type_enum_type     rr_type;
  ps_dnsi_query_class_enum_type    rr_class;
} ps_dnsi_generic_rr_type;

/*-------------------------------------------------------------------------
  State of the API control block
-------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_API_CB_STATE_INVALID = 0,
  PS_DNSI_API_CB_STATE_OPEN    = 1,
  PS_DNSI_API_CB_STATE_FREE    = 2
} ps_dnsi_api_state_enum_type;


/*-------------------------------------------------------------------------
  Suffixes for IPv4 and IPv6 PTR queries.
-------------------------------------------------------------------------*/
#define DNS_INADDR_ARPA_STR     "in-addr.arpa."
#define DNS_IN6ADDR_ARPA_STR    "ip6.arpa."

/*---------------------------------------------------------------------------
  DNS Critical section
---------------------------------------------------------------------------*/
/*
#ifdef FEATURE_DATA_PS_L4
static rex_crit_sect_type dns_crit_section;
#endif
*/

/*-------------------------------------------------------------------------
  Hostname query struct.
  Contains an array of query type, query_class and one query name.
  Used by getaddrinfo as well as getipnodebyname.
-------------------------------------------------------------------------*/
#define PS_DNSI_MAX_SPLIT_QUERIES   (2)

typedef struct ps_dnsi_query_struct_type
{
  int16                          num_queries;
  ps_dnsi_query_type_enum_type   query_type[PS_DNSI_MAX_SPLIT_QUERIES];
  ps_dnsi_query_class_enum_type  query_class[PS_DNSI_MAX_SPLIT_QUERIES];
  char                           query_data[DSS_DNS_MAX_DOMAIN_NAME_LEN];
} ps_dnsi_query_struct_type;

/*---------------------------------------------------------------------------
  Data structure for the API layer control block.

  handle            - Query handle
  session_handle    - Session manager handle of the session in which this
                      API query instance was created.
  res_handle        - Resolver handle for this API instance.
  api_type          - Type of API (ADDRINFO, NAMINFO)
  state             - State of API control block
  query_index       - Query index for multiple iteration case. Currently
                      used by ADDRINFO
  queries           - A query struct, which is populated with the query
                      type, query class for each iteration.
  port_num          - Port number (Valid only for ADDRINFO API)
  sock_type         - Socket type (Valid only for ADDRINFO API)
  transport_proto   - Transport protocol (Valid only for ADDRINFO API)
  addr_family       - Address family (Valid only for ADDRINFO API)
  flags             - Options associated with the API
                      for ADDRINFO this is cast to DSS_DNS_AI_FLAGS_ENUM_TYPE
                      for NAMEINFO this is cast to DSS_DNS_NI_FLAGS_ENUM_TYPE
  answer_q          - A queue of generic Resource records for this query.
  response_received - A flag that indicates if any successful response was
                      received, and success should be forwarded to the
                      application.
----------------------------------------------------------------------------*/
typedef struct ps_dnsi_api_cb_s_type
{
  dss_dns_query_handle_type          handle;
  dss_dns_session_mgr_handle_type    session_handle;
  dss_dns_api_type_enum_type         api_type;
  ps_dnsi_resolver_handle_type       res_handle;
  ps_dnsi_api_state_enum_type        state;
  uint16                             query_index;
  ps_dnsi_query_struct_type          queries;
  uint16                             port_num;
  uint16                             sock_type;
  uint16                             transport_proto;
  uint16                             addr_family;
  uint32                             flags;
  q_type                             answer_q;
  boolean                            response_received;
} ps_dnsi_api_cb_type;

/*---------------------------------------------------------------------------
  Session control block
  session_handle       - Session handle identifying a session control block.
  cback_f_ptr          - Application callback function pointer.
  user_data_ptr        - Application callback function user data.
  net_policy           - Network policy to be used to send the DNS query.
  is_net_policy_set    - If the application sets the iface_id, routeable
                         iface mode or network policy config param, this 
                         value would be set to true.
  use_cache_results    - Specifies if the resolver can use results from the
                         the DNS cache or not.
  add_to_cache         - Specifies if the results of the name resolution
                         should be added to the cache or not.
  session_deleted      - Identifies if the delete session API is already 
                         processed.
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_session_mgr_cb_s_type
{
  dss_dns_session_mgr_handle_type    session_handle;
  dss_dns_cback_f_type               cback_f_ptr;
  void                             * user_data_ptr;
  ds_policy_info_type                net_policy;
  boolean                            is_net_policy_set;
  boolean                            use_cache_results;
  boolean                            add_to_cache;
  boolean                            session_deleted;
  ps_dnsi_api_cb_type              * api_cb_ptr;
} ps_dnsi_session_mgr_cb_type;


/*---------------------------------------------------------------------------
  Hostent API control block
  session_handle       - Session handle identifying a session control block.
  cback_f_ptr          - Application callback function pointer.
  user_data_ptr        - Application callback function user data.
  iface_id             - Iface id configuration parameter.
  routeable_iface_mode - Identifies if the lookup needs to be performed when
                         mobile is connected to an external device
  use_cache_results    - Specifies if the resolver can use results from the
                         the DNS cache or not.
  add_to_cache         - Specifies if the results of the name resolution
                         should be added to the cache or not.
---------------------------------------------------------------------------*/
typedef struct dss_dnsi_hostent_cb_s_type
{
  dss_dns_session_mgr_handle_type    session_handle;
  dss_dns_query_handle_type          query_handle;
  dss_dns_api_type_enum_type         api_type;
  int16                              addr_family;
  char                               query_data_ptr[DSS_DNS_MAX_DOMAIN_NAME_LEN];
  struct dss_hostent               * ret_hostent_ptr;
  dss_dns_hostent_cback_f_type       cback_f_ptr;
  void                             * user_data_ptr;
} dss_dnsi_hostent_cb_type;



#endif /* FEATURE_DATA_PS_DNS */
#endif /* FEATURE_DATA_PS */
#endif  /* PS_DNSI_DEFS_H */

