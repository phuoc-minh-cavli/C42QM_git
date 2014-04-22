#ifndef PS_QOS_NET_INITIATED_REQ_H
#define PS_QOS_NET_INITIATED_REQ_H
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           P S _ Q O S _ N E T _ I N I T I A T E D _ R E Q . H

DESCRIPTION
  Header file defining the control block to represent an request for monitoring
  Network Initiated QOS and functions to manipulate it.

EXTERNAL FUNCTIONS
  PS_IFACE_CREATE_QOS_NET_INITIATED_REQUEST()
  PS_IFACE_RELEASE_QOS_NET_INITIATED()    

Copyright (c) 2018-19 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/**===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/inc/ds_qos_net_initiated_req.h#1 $
  $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
06/17/11    sz     Created the file.

===========================================================================*/
 

/**===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_ipfltr.h"
#include "ds_ipfltr_defs.h"
#include "ds_defs.h"
#include "ds_fwk.h"

/**===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/**
  @brief Maximum number of Network Initiated QoS requests. Must be at least 5 

  @see PS_QOS_NET_INITIATED_REQ_BUF_HIGH_WM
  TODO : reconsider the value - should depends on MAX_SYSTEM_IFACES?
*/
#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define DS_QOS_NET_INITIATED_MAX_REQS  25

#else

#define DS_QOS_NET_INITIATED_MAX_REQS  5

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/**
  @brief QOS information block stored with the flow providing the QOS.
*/
// TODO: change name to ps_qos_net_initiated_req_info_type
typedef struct
{
  /** Structure holding Rx QOS parameters */
  struct
  {
    list_type        fltr_list;       /**< List of Rx filters   */
  } rx;

  /** Structure holding Tx QOS parameters */
  struct
  {
     list_type        fltr_list;       /**< List of Rx filters  */
  } tx;
} qos_net_initiated_req_info_type;

/**
  @brief Control block for a network initiated request. An instance of this type
  is created when application registers for network initiated monitoring.
*/
typedef struct ps_qos_net_initiated_req_s
{
  list_link_type          link;             /**< Ptr to next node in a list  */
  
  /** The private information that should only ever be changed through ps_flow
      functions. The data in here should NOT be changed directly by a client. */
  struct
  {
    qos_net_initiated_req_info_type * qos_info_ptr;  /**< Ptr to QOS info    */
    ds_flow_type                    * flow_ptr; /*first flow pointer matched */
  } qos_net_initiated_req_private;
} ds_qos_net_initiated_req_type; 

/*---------------------------------------------------------------------------
  Data type used for DS_PDN_IOCTL_QOS_NET_INITIATED_REQUEST
---------------------------------------------------------------------------*/
typedef struct
{
  net_initiated_qos_spec_type               net_initiated_qos_spec;
  /**< QOS specification */
  ds_net_initiated_qos_req_handle_type      req_handle;
} ds_fwk_qos_net_initiated_request_type;


typedef struct
{
  ds_net_initiated_qos_req_handle_type        req_handle;
  net_initiated_qos_spec_type                 net_initiated_qos_spec;
  list_type                                   matching_flow_list;
} ds_fwk_qos_net_initiated_check_type;

typedef struct
{
  list_link_type                               link;
  ds_flow_type *                               flow_ptr;
} ds_fwk_qos_matching_flow_type;

/*---------------------------------------------------------------------------
  Argument to be passed with DSS_PDN_IOCTL_QOS_NET_INITIATED_RELEASE
---------------------------------------------------------------------------*/
typedef struct
{
  ds_net_initiated_qos_req_handle_type      req_handle;
  /**< Handle, which application had obtained obtained from
       DS_PDN_IOCTL_QOS_NET_INITIATED_REQUEST */
} ds_fwk_qos_net_initiated_release_type;

typedef struct
{
  ds_qos_handle_type               qos_handle;       /* Handle to QOS instance */

  /* OUTPUT Parameters */
  ip_flow_type                     rx_flow;      /* Rx Flow spec           */
  ip_flow_type                     tx_flow;      /* Tx Flow spec           */
  ds_pdn_ioctl_qos_status_type     qos_status;   /* status of QOS instance */
  uint8                            bearer_id;    /* EPS bearer id          */
} ds_fwk_qos_get_granted_flow_spec_type;

/**===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


/**===========================================================================

                             UTILITY FUNCTIONS

===========================================================================*/
/**
  @brief FUNCTION  DS_FWK_CREATE_QOS_NET_INITIATED_REQUEST()

  DESCRIPTION
    Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST ioctl.

  @param [in] fwk_inst                       corresponding instance on which QOS Net Initiated
  @param [in] qos_net_initiated_request_ptr  pointer to the argval structure containing the 
                                             request information
  @param [out] ps_errno                      specific error code in case operation fails


  @return
  0  : on success
  -1 : on failure
*/
int ds_fwk_create_qos_net_initiated_request
(
  ds_fwk_s_type								  * fwk_inst,
  ds_fwk_qos_net_initiated_request_type       * qos_net_initiated_request_ptr,
  int16                                       * ps_errno
);


/**
  @brief FUNCTION  DS_FWK_RELEASE_QOS_NET_INITIATED()

  DESCRIPTION
    Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE ioctl.

  @param [in] fwk_inst                      corresponding instance on which QOS Net Initiated
                                             registration cancellation is requested.
  @param [in] qos_net_initiated_request_ptr  pointer to the argval structure containing the 
                                             request cancellation information.
  @param [out] ps_errno                       specific error code in case operation fails


  @return
  0  : on success
  -1 : on failure
*/
int ds_fwk_release_qos_net_initiated
(
  ds_fwk_s_type                          * fwk_inst,
  ds_fwk_qos_net_initiated_release_type  * qos_net_initiated_release_ptr,
  int16                                  * ps_errno
);

/**
  @brief FUNCTION  DS_FWK_QOS_NET_INITIATED_REAL_TIME_CHECK()

  DESCRIPTION
    Handles NIQ req comparison with network filters

  @param [in] clnt_type,clnt_handle          client information with which the call bought up
  @param [in] qos_net_initiated_request_ptr  pointer to the argval structure containing the
                                             real-time check information.
  @param [out] ps_errno                      specific error code in case operation fails


  @return
  0  : on success
  -1 : on failure
*/
int ds_fwk_qos_net_initiated_check
(
  ds_fwk_client_type                          clnt_type,
  ds_fwk_clnt_handle_type                     clnt_handle,
  ds_fwk_qos_net_initiated_check_type       * qos_net_initiated_check_ptr,
  int16                                     * ps_errno
);

int ds_fwk_qos_get_granted_flow_spec
(
  ds_fwk_qos_get_granted_flow_spec_type*   flow_spec_ptr
  
);

void ds_fwk_qos_net_initiated_handle_event
(
  ds_pdn_event_enum_type    event,
  ds_pdn_event_info_u_type  event_info
);

/**
  @brief FUNCTION  DS_FWK_QOS_NET_INITIATED_REQ_INIT()

  DESCRIPTION
    Initializes QoS network initiated data structures

*/
void ds_fwk_qos_net_initiated_req_init
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_QOS_NET_INITIATED_REQ_H */

