#ifndef DS_IPFLTR_H
#define DS_IPFLTR_H

/*===========================================================================

                          DS_IPFLTR . H

DESCRIPTION
  Header file for declaration of functions to use IP filters for inbound
  pdn specific processing of IP packets received on that PDN.

EXTERNALIZED FUNCTIONS

  DS_IPFLTR_ADD()
    adds filters to the framework instance

  DS_IPFLTR_DELETE()
    deletes filters from the framework instance

  DS_IPFLTR_CONTROL()
    Performs control operation on filters (enable/disable)

  DS_IPFLTR_EXECUTE()
    executes filter for the processing inbound IP pkts

  DS_IPFLTR_PROCESS_PKT
    Process a raw IP pkt and then performs filtering on the packet

  DS_IPFLTR_GET_QUEUED_FRAGMENTS
    Passed a fraghdl returned by ds_ipfltr_process_pkt(), returns
    Fragments queued in the passed fraghdl datastructure

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/api/public/ds_ipfltr.h#1 $
  $DateTime: 2006/10/02 16:26:28
 
  when        who    what, where, why
  --------    ---    -------------------------------------------------------- 
05/08/18   svaka   created ipfltr header file.

===========================================================================*/

#include "ds_defsi.h"
#include "ds_ipfltr_defs.h"
#include "ps_pkt_info.h"
#include "list.h"
#include "ps_in.h"
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
TYPEDEF DS_IPFLTR_HANDLE_TYPE
  Client specified 4 byte IP filter handle.
---------------------------------------------------------------------------*/
typedef uint32 ds_ipfltr_handle_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_IPFLTR_RESULT_TYPE
  Client specified 4 byte IP filter result. Returned when a fltr matches
  given pkt.
---------------------------------------------------------------------------*/
typedef uint32 ds_ipfltr_result_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_IPFLTR_SUBSET_ID_TYPE
  Client specified 4 byte IP filter subset id.
---------------------------------------------------------------------------*/
typedef uint32 ds_ipfltr_subset_id_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_RX_QOS_FLTR_HANDLE_TYPE
---------------------------------------------------------------------------*/
typedef uint32 ds_rx_qos_fltr_handle_type;

/*---------------------------------------------------------------------------
DS_IPFLTR_INVALID_HANDLE - handle which does not identify any filter
---------------------------------------------------------------------------*/
#define DS_IPFLTR_INVALID_HANDLE  0x0

/*---------------------------------------------------------------------------
DS_IPFLTR_DELETE_ALL_HANDLE - handle which results in delete operation  
to be performed on all filters on given client id.
---------------------------------------------------------------------------*/
#define DS_IPFLTR_DELETE_ALL_HANDLE  0xFFFFFFFF

/**
  @brief Maximum number of MO exceptional ipfilters allowed to install. This can be
  changed uptil the max number of filters allowed globally
*/
#define DS_MO_EXCP_IPFLTR_MAX      10

/**
  @brief Maximum number of PDN sharing ipfilters allowed to install. This can be
  changed uptil the max number of filters allowed globally
*/
#define DS_PDN_SHARING_IPFLTR_MAX      20


/*---------------------------------------------------------------------------
DS_IPFLTR_NOMATCH - Result returned when pkt doesn't match any filter
---------------------------------------------------------------------------*/
#define DS_IPFLTR_NOMATCH  0x0

/*---------------------------------------------------------------------------
DS_IPFLTR_SUBSET_ID_DEFAULT
  Subset ID indicating that all filters in a queue must be executed to match
  a packet
---------------------------------------------------------------------------*/
#define DS_IPFLTR_SUBSET_ID_DEFAULT  0x0

/*---------------------------------------------------------------------------
TYPEDEF FLTR_PRIORITY_ENUM_TYPE

DESCRIPTION
  Defines various priorities in which a filter can be added to existing
  filters
---------------------------------------------------------------------------*/
typedef enum
{
  DS_IPFLTR_PRIORITY_MIN     = 0,
  DS_IPFLTR_PRIORITY_FCFS    = 0,
  DS_IPFLTR_PRIORITY_CUSTOM  = 1,
  DS_IPFLTR_PRIORITY_DEFAULT = DS_IPFLTR_PRIORITY_FCFS,
  DS_IPFLTR_PRIORITY_MAX
} ds_fltr_priority_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_NAME_ENUM_TYPE

DESCRIPTION
  List of groups.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_DEFAULT_GROUP      = 0x0001,
  DS_UM_GROUP           = 0x0002,
  DS_RM_GROUP           = 0x0004,
} ds_filter_group_enum_type;

/*===========================================================================
  A global optimized filter specification for the DL optimized data path.
==========================================================================*/
#define DS_MAX_OPT_FILTER_ENTRIES (50)

#define MAX_GLOBAL_OPT_FILTER_ENTRIES (7)

/*---------------------------------------------------------------------------
DS_FWK_IPFLTR_SUBSET_ID_DEFAULT
  Subset ID indicating that all filters in a queue must be executed to match
  a packet
---------------------------------------------------------------------------*/
#define DS_FWK_IPFLTR_SUBSET_ID_DEFAULT  0x0

typedef struct ds_dl_opt_filter_spec_s
{
  uint32   fi_handle;
  uint16   dst_port;
  uint16   dst_port_range;
  uint16   src_port;
  uint16   src_port_range;
  uint8    protocol;
} ds_dl_opt_filter_spec_type;

/*===========================================================================
  An optimized filter specification for the UL optimized data path.
===========================================================================*/
typedef struct ds_ul_opt_filter_spec_s
{
  uint32   fi_handle;
  uint16   dst_port;
  uint16   dst_port_range;
  uint16   src_port;
  uint16   src_port_range;
  uint8    protocol;
} ds_ul_opt_filter_spec_type;

/*===========================================================================
MACRO DS_IPFLTR_GET_OPT_FILTER_BASE()

DESCRIPTION
  Returns the base addr for optimized filter array. 

  NOTE : Particular to optimized data path only.

PARAMETERS
  iface_ptr   : pointer to iface

RETURN VALUE
  Optimized filters list head ptr
===========================================================================*/
#define DS_IPFLTR_GET_OPT_FILTER_BASE(fwk_inst)                            \
  (fwk_inst)->dl_opt_fltr_arr

/*---------------------------------------------------------------------------
TYPEDEF FLTR_COMPARISON_F_PTR_TYPE

DESCRIPTION
  Function prototype to compare two filter sets to compute precedence.

PARAMETERS
  fi_one_ptr: Ptr to first filter set to compare
  fi_two_ptr: Ptr to second filter set to compare

RETURN VALUE
  TRUE  - Filter1 has higher precedence than filter2
  FALSE - Filter2 has higher precedence than filter1
---------------------------------------------------------------------------*/
typedef boolean (*ipfltr_comparison_f_ptr_type)
(
  ip_filter_type                 *fi_one_ptr,
  ip_filter_type                 *fi_two_ptr
);

/*---------------------------------------------------------------------------
TYPEDEF DS_IPFLTR_ADD_PARAM_TYPE

DESCRIPTION
  Structure containing all the necessary information to add filters to an
  framework instance
---------------------------------------------------------------------------*/
typedef struct
{
  ds_fwk_client_type                client_type;
  uint8                             client_handle;
  ds_fwk_s_type*                    fw_inst_ptr;//Applicable for only TX QOS filters
  ds_ipfltr_client_id_enum_type     client_id;
  void                            * fi_ptr_arr;
  ds_ipfltr_result_type             fi_result;
  ds_ipfltr_subset_id_type          subset_id;
  ds_fltr_priority_enum_type        fltr_priority;
  uint8                             num_filters;
  boolean                           is_validated;
} ds_ipfltr_add_param_type;

typedef struct
{
  ds_fwk_client_type                client_type;
  uint8                             client_handle;
  ds_fwk_s_type*                    fw_inst_ptr;//Applicable for only TX QOS filters
  ds_ipfltr_client_id_enum_type     client_id;
  ds_ipfltr_handle_type             fi_handle;
} ds_ipfltr_delete_param_type;

typedef struct
{
  ds_fwk_s_type                       * ds_fwk_inst_ptr;
  ds_ipfltr_client_id_enum_type         client_id;
  ds_ipfltr_handle_type                 fi_handle;
  boolean                               enable;
} ds_ipfltr_ctrl_param_type;

/*---------------------------------------------------------------------------
  Definition of data buffer used to store filters in framework instance
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                     link;         /* Link to enqueue filter  */
  ds_filter_group_enum_type       filter_group;  /* Type of this filter     */
  ip_filter_type                  fi_default;   /* Default filter          */
  ds_ipfltr_handle_type           fi_handle;  /* Client specific filter id */
  ds_ipfltr_result_type           fi_result;  /* Result sent when fltr match*/
  ds_ipfltr_subset_id_type        subset_id;  /* Subset ID of this filter  */
  boolean                         disabled;   /* Is disabled for execution */
} ds_ipfilteri_type;

/*---------------------------------------------------------------------------
  Definition of various Filtering result.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_IPFLTR_RESULT_NONE              = 0,
  DS_IPFLTR_RESULT_UL_STATIC_LOCAL   = 1,
  DS_IPFLTR_RESULT_UL_STATIC_WWAN    = 2,
  DS_IPFLTR_RESULT_UL_IPV6           = 3,
  DS_IPFLTR_RESULT_UL_CLAT           = 4,
  DS_IPFLTR_RESULT_MAX
} ds_ipfltr_result_enum_type;

extern q_type global_ipfltr_info[FLTR_CLIENT_MAX];

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DS_IPFLTR_INIT()

DESCRIPTION
  Initialize the global IP filter queue.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_ipfltr_init
(
 void
);

/*===========================================================================
FUNCTION DS_IPFLTR_ADD()

DESCRIPTION
  This function adds a set of IP filters to the framework instance matching to client handle.
  The filter set is tied to a filter handle, which uniquely idenifies a set of
  filters added by the given client for the given client_handle. The filter handle
  is used to manipulate the filter set. A client needs to provide a filter
  result which is returned when a filter successfully match during the filter
  execution.
  if   client_type is DS_FWK_CLIENT_INVALID or if client_handle is invalid 
  then filters are installed globally.

PARAMETERS
  fltr_add_param_ptr : ptr to structure containing all the necessary info to
                               add filters to an framework instance
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  DS_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_handle_type ds_ipfltr_add
(
  ds_ipfltr_add_param_type  * fltr_add_param_ptr,
  int16                     * ps_errno
);

/*===========================================================================
FUNCTION DS_IPFLTR_ADD_BY_GROUP()

DESCRIPTION
  This function adds a set of IP filters to the global ip filter array
  based on group. The filter set is tied to a filter handle, which uniquely
  idenifies a set of filters added by the client for the given iface
  group. The filter handle is used to manipulate the filter set. A client
  needs to provide a filter result which is returned when a filter
  successfully match during the filter execution.

PARAMETERS
  filter_group        : iface group for which filters need to be executed
  client_id             : Filtering client id
  fltr_param_ptr     : ptr to structure containing all the necessary info
                             to add filters to an framework instance
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  DS_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_handle_type ds_ipfltr_add_by_group
(
  ds_filter_group_enum_type               filter_group,
  ds_ipfltr_client_id_enum_type           client_id,
  const ds_ipfltr_add_param_type        * fltr_param_ptr,
  int16                                 * ps_errno
);


/*===========================================================================
FUNCTION DS_IPFLTR_DELETE()

DESCRIPTION
  This function deletes all the existing IP filter rules for the specified
  filter handle for a given client from the specified framework instance.
  if client_type is DS_FWK_CLIENT_INVALID or if client_handle is invalid 
then filters are deleted globally.

DEPENDENCIES
  None

PARAMETERS
  ds_ipfltr_delete_param_type: all the necessary parms to delete the filter.
  ps_errno  : error returned to the caller if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

SIDE EFFECTS
  Some packets may not get filtered once these filters are deleted
===========================================================================*/
int ds_ipfltr_delete
(
  ds_ipfltr_delete_param_type    * fltr_delete_param_ptr,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION DS_IPFLTR_CONTROL()

DESCRIPTION
  This function allows a control operation on all IP filters currently
  associated with the specified filter handle of the client for the Fwk instance.
  Only operation supported for now is disabling or enabling the filter
  execution state. Disabled filters are not used during filter execution.

DEPENDENCIES
  None

PARAMETERS
  fwk_ptr     : fwk instance on which filters are added
  client_id    : Filtering client id
  fi_handle   : filter handle which identifies a specific filter set added on
                    this instance by this client.
  enable       : Whether to enable or disable already installed filters
  ps_errno    : error returned to the caller if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

SIDE EFFECTS
  Some packets may not get filtered once a filter set is disabled and they
  may get filtered if a filter set is enabled
===========================================================================*/
int ds_ipfltr_control
(
  ds_ipfltr_ctrl_param_type           * fltr_ctrl_param_ptr,
  int16                               * ps_errno
);

/*===========================================================================
FUNCTION DS_IPFLTR_ADD_QOS_RULES()

DESCRIPTION
  This function adds a set of IP filters to the specified fwk instance. The
  filter set is tied to a filter handle, which uniquely idenifies a set of
  filters added by the given client for the given iface. The filter handle
  is used to manipulate the filter set. A client needs to provide a filter
  result which is returned when a filter successfully match during the filter
  execution.
  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  fltr_add_param_ptr : ptr to structure containing all the necessary info to
                       add filters to an iface
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  DS_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_handle_type ds_ipfltr_add_qos_rules
(
  int                               fwk_index,
  ip_addr_enum_type                 ip_type,
  ds_ipfltr_add_param_type        * fltr_param_ptr,
  int16                           * ps_errno
);


/**
  @brief  This function returns stats related to filters installed on a given
          iface
  client_type                            client type 
  client_handle                         client handle
  @param[in]  filter_client          Filters client type
  @param[out] max_allowed     Maximum number of filters allowed to be installed
                         on the given iface
  @param[out] used            Current number of filters installed on the given 
                          iface
  @param[out] ps_errno        Failure reason.
                              DS_EFAULT : Invalid arguments
                              DS_EINVAL : Invalid Operation

  @return     -1              On Failure.
               0              On Success.

  @code
  @endcode
*/

int ds_ipfltr_stats
(
  ds_fwk_client_type                      client_type,
  uint8                                   client_handle,
  ds_ipfltr_client_id_enum_type           filter_client,
  uint32                                * max_allowed,
  uint32                                * used,
  int16                                 * ps_errno
);

/*===========================================================================
FUNCTION DS_IPFLTR_EXECUTE()

DESCRIPTION
  This function executes all the filters in an framework instance to see if any of
  those passes criteria specified in the information passed in. Processing
  is done until the first match is found and filter id associated
  with that filter is returned. 

DEPENDENCIES
  None

PARAMETERS
  bearer_ptr          - bearer ptr on which the packet is recieved.
  client_id             - Filtering client id
  subset_id          - ID which identified a subset of all filters installed
                       on framework instance. Only these filters are considered for
                       matching a packet
  ip_filter_info_ptr - Ptr to IP pkt information block to apply filter on

RETURN VALUE
  filter id associated with the filter : on a successful filter match
  DS_IPFLTR_NOMATCH              : for no match

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_result_type ds_ipfltr_execute
(
  int                            fwk_inst,
  ds_ipfltr_client_id_enum_type  client_id,
  ds_ipfltr_subset_id_type       subset_id,
  ip_pkt_info_type              *ip_filter_info_ptr
);

/*===========================================================================
FUNCTION DS_IPFLTR_VALIDATE_FLTR_PARAM()

DESCRIPTION
  Validates parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_arr  : Array of ptr to filters
  num_fltr  : Number of filters in above array

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  the appropriate error masks will be set with the appropriate error codes
  for values and enums which failed validation. Because variables are only
  validated if the enums are set in the field masks the only error masks
  that will be updated will be those corresponding to the values set within
  the field masks.
===========================================================================*/
boolean ds_ipfltr_validate_fltr_param
(
  ds_ipfltr_client_id_enum_type        client_id,
  ip_filter_type                       fltr_arr[],
  uint8                                num_fltr
);

/*===========================================================================
FUNCTION DS_FWK_EXECUTE_PDN_SHARING_FILTERS()

DESCRIPTION
  Executes the PDN sharing filters
  
DEPENDENCIES
  fwk_inst  : Framework Instance
  pi_ptr    : Pkt info pointer which contains packet meta info

PARAMETERS
  None.

RETURN VALUE
  Uint32  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_fwk_execute_pdn_sharing_filters
(
 ds_fwk_s_type          *fwk_inst,
 ip_pkt_info_type		*pi_ptr
);

#endif /* DS_IPFLTR_H */

