/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            P S _ Q O S _ N E T _ I N I T I A T E D _ R E Q . C

DESCRIPTION
  File defining functions that can be used to manipulate a network initiated
  QoS request

Copyright (c) 2018 - 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_qos_net_initiated_req.c#1 $
  $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/12/18   sk      Created the file

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/*===========================================================================

                                INCLUDE FILES


===========================================================================*/
#include "ds_qos_net_initiated_req.h"
#include "ds_defs.h"
#include "ps_qos_defs.h" 
#include "ps_mem.h"
#include "data_msg.h"
#include "ps_ip6_addr.h"
#include "ds_system_heap.h"
#include "ds_fwk_rx_qos_fltr.h"
#include "ds_pdn_defs.h"
#include "ds_flow.h"
#include "ds_fwk.h"

#ifdef FEATURE_DATA_PS_QOS

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
typedef struct  
{
  list_link_type link;
  ip_filter_type filter;
} ds_qos_net_initiated_req_filter_type;

/*---------------------------------------------------------------------------
  Tuning the number of PS net initiated qos requests buffers 
  needed by this module
---------------------------------------------------------------------------*/
#define DS_QOS_NET_INITIATED_REQ_BUF_SIZE  ((sizeof(ds_qos_net_initiated_req_type) + 3) & ~3)
#define DS_QOS_NET_INITIATED_REQ_BUF_NUM       DS_QOS_NET_INITIATED_MAX_REQS
#define DS_QOS_NET_INITIATED_REQ_BUF_HIGH_WM   (DS_QOS_NET_INITIATED_MAX_REQS - 5)
#define DS_QOS_NET_INITIATED_REQ_BUF_LOW_WM    (5)

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_qos_net_initiated_req along with ps_mem header
----------------------------------------------------------------------------*/
static int ds_qos_net_initiated_req_buf_mem[
  PS_MEM_GET_TOT_SIZE_OPT(DS_QOS_NET_INITIATED_REQ_BUF_NUM,
                          DS_QOS_NET_INITIATED_REQ_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_qos_net_initiated_req_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type           * ds_qos_net_initiated_req_buf_hdr[DS_QOS_NET_INITIATED_REQ_BUF_NUM];
static ds_qos_net_initiated_req_type * ds_qos_net_initiated_req_buf_ptr[DS_QOS_NET_INITIATED_REQ_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps net initiated qos INFO requests buffers 
  needed by this module
---------------------------------------------------------------------------*/
#define DS_QOS_NET_INITIATED_REQ_INFO_BUF_SIZE  ((sizeof(qos_net_initiated_req_info_type) + 3) & ~3)
#define DS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM       DS_QOS_NET_INITIATED_MAX_REQS
#define DS_QOS_NET_INITIATED_REQ_INFO_BUF_HIGH_WM   (DS_QOS_NET_INITIATED_MAX_REQS - 5)
#define DS_QOS_NET_INITIATED_REQ_INFO_BUF_LOW_WM    (5)

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_qos_net_initiated_req_info 
  along with ps_mem header
----------------------------------------------------------------------------*/
static int ds_qos_net_initiated_req_info_buf_mem[
  PS_MEM_GET_TOT_SIZE_OPT(DS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM,
                          DS_QOS_NET_INITIATED_REQ_INFO_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to 
  ps_qos_net_initiated_req_info_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type                * ds_qos_net_initiated_req_info_buf_hdr[DS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM];
static qos_net_initiated_req_info_type    * ds_qos_net_initiated_req_info_buf_ptr[DS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of PS net initiated qos filter buffers
  needed by this module
---------------------------------------------------------------------------*/
#define DS_QOS_NET_INITIATED_REQ_FILTER_BUF_SIZE  ((sizeof(ds_qos_net_initiated_req_filter_type) + 3) & ~3)
/*---------------------------------------------------------------------------
  There are in average not more than MAX_FLTR_PER_REQ / 4
  filters in every request
---------------------------------------------------------------------------*/
#define DS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM     ((uint32)(DS_QOS_NET_INITIATED_MAX_REQS * (DS_MAX_FLTR_PER_REQ / 4)))
#define DS_QOS_NET_INITIATED_REQ_FILTER_BUF_HIGH_WM (DS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM - 5)  
#define DS_QOS_NET_INITIATED_REQ_FILTER_BUF_LOW_WM  (5)

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_qos_net_initiated_req_filter_buf_mem 
  along with ps_mem header
----------------------------------------------------------------------------*/
static int ds_qos_net_initiated_req_filter_buf_mem[
  PS_MEM_GET_TOT_SIZE_OPT(DS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM,
                          DS_QOS_NET_INITIATED_REQ_FILTER_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
  /*----------------------------------------------------------------------------
    Array of pointers used to facilitate easy debugging.  The first one points
    to the ps_mem header and the latter points to 
    ps_qos_net_initiated_req_info_buf
  ----------------------------------------------------------------------------*/
  static ps_mem_buf_hdr_type                   * ds_qos_net_initiated_req_filter_buf_hdr[DS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM];
  static ds_qos_net_initiated_req_filter_type  * ds_qos_net_initiated_req_filter_buf_ptr[DS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps flow rx filter buffers needed by this module
---------------------------------------------------------------------------*/
#define DS_FWK_RX_FLTR_BUF_SIZE  ((sizeof(ds_fwk_rx_fltr_buf_type) + 3) & ~3)
#define DS_FWK_RX_FLTR_BUF_NUM       200
#define DS_FWK_RX_FLTR_BUF_HIGH_WM   195
#define DS_FWK_RX_FLTR_BUF_LOW_WM      5

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_flow_rx_fltr along with ps_mem header
----------------------------------------------------------------------------*/
static int ds_fwk_rx_fltr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                   (
                                     DS_FWK_RX_FLTR_BUF_NUM,
                                     DS_FWK_RX_FLTR_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_flow_rx_fltr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type       * ps_flow_rx_fltr_buf_hdr[PS_FLOW_RX_FLTR_BUF_NUM];
static ds_fwk_rx_fltr_buf_type   * ps_flow_rx_fltr_buf_ptr[PS_FLOW_RX_FLTR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of fwk instance rx qos filter node buffers needed by this
  module
---------------------------------------------------------------------------*/
#define DS_FWK_RX_QOS_FLTR_NODE_BUF_SIZE  ((sizeof(ds_fwk_rx_qos_fltr_node_type) + 3) & ~3)

#define DS_FWK_RX_QOS_FLTR_NODE_BUF_NUM       200
#define DS_FWK_RX_QOS_FLTR_NODE_BUF_HIGH_WM   195
#define DS_FWK_RX_QOS_FLTR_NODE_BUF_LOW_WM      5

/*----------------------------------------------------------------------------
  Allocate memory to hold ds_fwk_rx_qos_fltr_node along with ps_mem header
----------------------------------------------------------------------------*/
static int ds_fwk_rx_qos_fltr_node_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                             (
                                               DS_FWK_RX_QOS_FLTR_NODE_BUF_NUM,
                                               DS_FWK_RX_QOS_FLTR_NODE_BUF_SIZE
                                             )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ds_fwk_rx_qos_fltr_node_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type            
     * ds_fwk_rx_qos_fltr_node_buf_hdr[DS_FWK_RX_QOS_FLTR_NODE_BUF_NUM];
static ds_fwk_rx_qos_fltr_node_type 
     * ds_fwk_rx_qos_fltr_node_buf_ptr[DS_FWK_RX_QOS_FLTR_NODE_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */



/*===========================================================================

                                INDICATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_FWK_QOS_NET_INITIATED_REQ_INIT()

DESCRIPTION
  Initializes QoS network initiated data structures

PARAMETERS
  None
RETURN VALUE
  None
DEPENDENCIES
  None
SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_qos_net_initiated_req_init  
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
   /* Init the Network Initiated QoS objects buffer */ 
   if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_QOS_NET_INITIATED_REQ_TYPE,
      ds_qos_net_initiated_req_buf_mem,
      DS_QOS_NET_INITIATED_REQ_BUF_SIZE,
      DS_QOS_NET_INITIATED_REQ_BUF_NUM,
      DS_QOS_NET_INITIATED_REQ_BUF_HIGH_WM,
      DS_QOS_NET_INITIATED_REQ_BUF_LOW_WM,
      NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      (int *) ds_qos_net_initiated_req_buf_hdr,
      (int *) ds_qos_net_initiated_req_buf_ptr
#else
      NULL,
      NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      ) == -1)
   {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_qos_net_initiated_req_init(): "
                      "Can't init the module");
   }

   /* Init the Network Initiated QoS info objects buffer */ 
   if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_QOS_NET_INITIATED_REQ_INFO_TYPE,
      ds_qos_net_initiated_req_info_buf_mem,
      DS_QOS_NET_INITIATED_REQ_INFO_BUF_SIZE,
      DS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM,
      DS_QOS_NET_INITIATED_REQ_INFO_BUF_HIGH_WM,
      DS_QOS_NET_INITIATED_REQ_INFO_BUF_LOW_WM,
      NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      (int *) ds_qos_net_initiated_req_info_buf_hdr,
      (int *) ds_qos_net_initiated_req_info_buf_ptr
#else
      NULL,
      NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      ) == -1)
   {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_qos_net_initiated_req_init(): "
                      "Can't init the module");
   }

   /* Init the Network Initiated QoS filters objects buffer */                            
   if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE, 
      ds_qos_net_initiated_req_filter_buf_mem,
      DS_QOS_NET_INITIATED_REQ_FILTER_BUF_SIZE,
      DS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM,
      DS_QOS_NET_INITIATED_REQ_FILTER_BUF_HIGH_WM,
      DS_QOS_NET_INITIATED_REQ_FILTER_BUF_LOW_WM,
      NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      (int *) ds_qos_net_initiated_req_filter_buf_hdr,
      (int *) ds_qos_net_initiated_req_filter_buf_ptr
#else
      NULL,
      NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      ) == -1)
   {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_qos_net_initiated_req_init(): "
                      "Can't init the module");
   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
  
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_RX_QOS_FLTR_BUF_TYPE,
                           ds_fwk_rx_fltr_buf_mem,
                           DS_FWK_RX_FLTR_BUF_SIZE,
                           DS_FWK_RX_FLTR_BUF_NUM,
                           DS_FWK_RX_FLTR_BUF_HIGH_WM,
                           DS_FWK_RX_FLTR_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_flow_rx_fltr_buf_hdr,
                           (int *) ps_flow_rx_fltr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_qos_net_initiated_req_init(): "
                    "Can't init the module");
  }
  
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_RX_QOS_FLTR_NODE_TYPE,
                           ds_fwk_rx_qos_fltr_node_buf_mem,
                           DS_FWK_RX_QOS_FLTR_NODE_BUF_SIZE,
                           DS_FWK_RX_QOS_FLTR_NODE_BUF_NUM,
                           DS_FWK_RX_QOS_FLTR_NODE_BUF_HIGH_WM,
                           DS_FWK_RX_QOS_FLTR_NODE_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ds_fwk_rx_qos_fltr_node_buf_hdr,
                           (int *) ds_fwk_rx_qos_fltr_node_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_qos_net_initiated_req_init(): "
                    "Can't init the module");
  }

} /* ds_fwk_qos_net_initiated_req_init() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_STORE_FLTR_SPEC()

DESCRIPTION
  Stores Rx fltr spec at specified location

PARAMETERS
  qos_info_ptr : Address where Rx fltr spec needs to be stored. Could be
                 qos_info_ptr or modify_qos_info_ptr of ps_flow
  qos_spec_ptr : QOS spec containing Rx fltr spec to be stored

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ds_qos_net_initiated_store_fltr_spec
(
  qos_net_initiated_req_info_type    * qos_info_ptr,
  const net_initiated_qos_spec_type  * qos_spec_ptr,
  int16                              * ps_errno
)
{
  ds_qos_net_initiated_req_filter_type  * new_filter_buf_ptr;
  uint8                                   fltr_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Store Rx filter spec
  -------------------------------------------------------------------------*/
  for (fltr_index = 0;
       fltr_index < qos_spec_ptr->rx_fltr_template.num_filters;
       fltr_index++)
  {
    new_filter_buf_ptr = (ds_qos_net_initiated_req_filter_type *)
                           ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE);
    if (new_filter_buf_ptr == NULL)
    {
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    new_filter_buf_ptr->filter =
      qos_spec_ptr->rx_fltr_template.list_ptr[fltr_index];
    list_push_back(&(qos_info_ptr->rx.fltr_list),
      &(new_filter_buf_ptr->link));
    
    DS_MSG2(MSG_LEGACY_HIGH,"ps_qos_net_initiated_store_fltr_spec()"
                    "App RX filter: %d obj 0x%p",
                    fltr_index, new_filter_buf_ptr);
    
  }

  /*-------------------------------------------------------------------------
    Store Tx filter spec
  -------------------------------------------------------------------------*/
  for (fltr_index = 0;
       fltr_index < qos_spec_ptr->tx_fltr_template.num_filters;
       fltr_index++)
  {
    new_filter_buf_ptr = (ds_qos_net_initiated_req_filter_type *)
                           ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE);
    if (new_filter_buf_ptr == NULL)
    {
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    new_filter_buf_ptr->filter =
      qos_spec_ptr->tx_fltr_template.list_ptr[fltr_index];
    list_push_back(&(qos_info_ptr->tx.fltr_list),
      &(new_filter_buf_ptr->link));

    DS_MSG2(MSG_LEGACY_HIGH,"ps_qos_net_initiated_store_fltr_spec()"
                    "App TX filter: %d obj 0x%p",
                    fltr_index, new_filter_buf_ptr);

  }

  return 0;

} /* ps_qos_net_initiated_store_fltr_spec() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_DELETE_FILTER_SPEC()

DESCRIPTION
Deletes fltr spec from specified location in qos net initiated req spec

PARAMETERS
qos_info_ptr : Address where Rx fltr spec is stored. Could be qos_info_ptr
or modify_qos_info_ptr of ps_flow

RETURN VALUE
None

DEPENDENCIES
Parameters must be valid. Validation is not performed as this is an internal
function

SIDE EFFECTS
None
===========================================================================*/
void ds_qos_net_initiated_delete_filter_spec
(
  list_type filters_list 
)
{
  ds_qos_net_initiated_req_filter_type   * curr_fltr = NULL;
  ds_qos_net_initiated_req_filter_type   * next_fltr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_delete_filter_spec()");

  curr_fltr = list_peek_front(&filters_list);
  while (NULL != curr_fltr)
  {
    next_fltr = list_peek_next(&filters_list,
      &(curr_fltr->link));

    list_pop_item(&filters_list, &(curr_fltr->link));
    PS_MEM_FREE(curr_fltr);

    curr_fltr = next_fltr;
  } /* while there is a fltr in the list */

} /* ds_qos_net_initiated_delete_filter_spec() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_DELETE_QOS_SPEC()

DESCRIPTION
  This function deletes previously stored qos net initiated spec and
  frees up any memory buffers used.

PARAMETERS
  qos_net_initiated_req_ptr : ptr qos net initiated spec

RETURN VALUE
  None

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.

SIDE EFFECTS
  None
===========================================================================*/
void ds_qos_net_initiated_delete_qos_spec
(

ds_qos_net_initiated_req_type    * qos_net_initiated_req_ptr   
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_delete_qos_spec()");

  if ( NULL == qos_net_initiated_req_ptr || 
       NULL == qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr )
  {
    DS_ERR_FATAL("ds_qos_net_initiated_delete_qos_spec(): QOS info pointer of qos net initiated req NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    Free Rx filters and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  ds_qos_net_initiated_delete_filter_spec(
    qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list
    );
  
  /*-------------------------------------------------------------------------
    Free Tx filters and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  ds_qos_net_initiated_delete_filter_spec(
    qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list
    );

  /*-----------------------------------------------------------------------
    Destroy various lists used to store QOS info
  -----------------------------------------------------------------------*/
  list_destroy(&(qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list));
  list_destroy(&(qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list));

  /*-------------------------------------------------------------------------
    Free the qos spec
  -------------------------------------------------------------------------*/  
  PS_MEM_FREE(qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr);

  DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_delete_qos_spec(): "
                  "QoS Net Initiated 0x%p: QOS released",
                  qos_net_initiated_req_ptr);

} /* ds_qos_net_initiated_delete_qos_spec() */


/*===========================================================================
FUNCTION ds_fwk_validate_fltr_spec()

DESCRIPTION
  Validates fltr spec in one direction

PARAMETERS
  fltr_spec_ptr       : Address where fltr spec is stored. Could be rx or tx
                        from QOS_REQUEST or QOS_MODIFY
  flow_type           : Specifies direction of flow spec
  is_fltr_spec_valid  : OUT PARAM specifying if validation is successful
  ps_errno            : error returned back if function fails

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ds_fwk_validate_fltr_spec
(
  const ip_filter_spec_type          * fltr_spec_ptr,
  qos_spec_field_mask_enum_type        flow_type,
  boolean                            * is_fltr_spec_valid,
  int16                              * ps_errno
)
{
  ds_ipfltr_client_id_enum_type  client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Validate that number of filters is in between (0, MAX_FLTR_PER_REQ]
  -----------------------------------------------------------------------*/
  if (fltr_spec_ptr->num_filters == 0)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_validate_fltr_spec(): "
                    "Zero filters specified for mask 0x%x", flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else if (fltr_spec_ptr->num_filters > MAX_FLTR_PER_REQ)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"ds_fwk_validate_fltr_spec(): "
                    "Too many filters specified: %d, max = %d, for mask 0x%x",
                    fltr_spec_ptr->num_filters, MAX_FLTR_PER_REQ, flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  else if (fltr_spec_ptr->list_ptr == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_validate_fltr_spec(): "
                    "NULL list_ptr in fltr spec for mask 0x%x", flow_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Find out IP fltr client based on flow_type
  -------------------------------------------------------------------------*/
  if (flow_type == QOS_MASK_TX_FLOW)
  {
    client_id = FLTR_CLIENT_QOS_OUTPUT;
  }
  else if (flow_type == QOS_MASK_RX_FLOW)
  {
    client_id = FLTR_CLIENT_QOS_INPUT;
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_validate_fltr_spec(): "
                    "Invalid flow_type 0x%x is passed", flow_type);
    DS_ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate individual filters in fltr spec
    As fltr spec is invalid if any of the filters are invalid,
    "is_fltr_spec_valid &=" is used below
  -------------------------------------------------------------------------*/
  *is_fltr_spec_valid &=
    ds_ipfltr_validate_fltr_param(  client_id,
                                    fltr_spec_ptr->list_ptr,
                                    fltr_spec_ptr->num_filters);
  return 0;
} /* ds_fwk_validate_fltr_spec() */

/*===========================================================================
FUNCTION DS_FWK_VALIDATE_NET_INITIATED_QOS_SPEC()

DESCRIPTION
This function validates the requested QOS spec. Both flow spec and filter
spec are validated in each direction. A generic validation is performed on
the parameters and if mode handlers require extra validation they can pass
an optional validation function ptr to perform extended validation. Note
that generic validation will still be performed before calling extended
validaiton functions.

PARAMETERS
qos_req_param     : structure containing qos_spec and information necessary
to validate it.
ps_errno          : specific error code in case operation fails

RETURN VALUE
0 : if QOS spec is valid
-1 : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
int ds_fwk_validate_net_initiated_qos_spec
(
  const net_initiated_qos_spec_type  *   net_initiated_qos_spec,
  int16                              *   ps_errno
)
{
  boolean  is_qos_spec_valid = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




  /*-----------------------------------------------------------------------
    Validate Rx fltr spec
  -----------------------------------------------------------------------*/
  if (net_initiated_qos_spec->rx_fltr_template.num_filters > 0)
  {
    if (ds_fwk_validate_fltr_spec
        (
          &(net_initiated_qos_spec->rx_fltr_template),
          QOS_MASK_RX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_validate_net_initiated_qos_spec(): "
                      "Invalid QOS Spec, Rx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }

  /*-----------------------------------------------------------------------
    Validate Tx fltr spec
  -----------------------------------------------------------------------*/
  if (net_initiated_qos_spec->tx_fltr_template.num_filters > 0)
  {
    if (ds_fwk_validate_fltr_spec
        (
          &(net_initiated_qos_spec->tx_fltr_template),
          QOS_MASK_TX_FLOW,
          &is_qos_spec_valid,
          ps_errno
        ) == -1)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_validate_net_initiated_qos_spec(): "
                      "Invalid QOS Spec, Tx fltr spec is invalid");
      /* ps_errno is populated in ps_flowi_validate_fltr_spec() */
      return -1;
    }
  }

  if (is_qos_spec_valid == FALSE)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_validate_net_initiated_qos_spec(): "
                    "QOS Spec validation failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  return 0;

} /* ds_fwk_validate_net_initiated_qos_spec() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_STORE_QOS_SPEC()

DESCRIPTION
  This function stores the requested network initiated qos spec on the
  qos_net_initiated_req object on the instance.

PARAMETERS
  qos_net_initiated_req : Network Initiated Request object where QOS spec is to be stored on the instance.
  qos_net_initiated_spec: QOS spec to be stored.
  ps_errno              : error code passed back when storage fails.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  TODO: verify the need for locks
  Must be called inside TASKLOCK()

SIDE EFFECTS
  If error occurs during storage, none of QOS spec is stored, i.e semantics
  is all or nothing

===========================================================================*/
int ds_qos_net_initiated_store_qos_spec
(
  ds_qos_net_initiated_req_type        * qos_net_initiated_req,
  const net_initiated_qos_spec_type    * qos_net_initiated_spec,
  int16                                * ps_errno
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_store_qos_spec()");
  DS_ASSERT(NULL != ps_errno);

  if (NULL == qos_net_initiated_spec)
  {
    DS_ERR_FATAL("ds_qos_net_initiated_store_qos_spec(): QOS_SPEC is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (NULL == qos_net_initiated_req || 
      NULL == qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr)
  {
    DS_ERR_FATAL("ds_qos_net_initiated_store_qos_spec(): qos_net_initiated_req or qos_info_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Store filters
  -------------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_store_qos_spec(): "
                  "Storing fltr spec");

  /*-------------------------------------------------------------------------
    Store filter spec on the qos net initiated object
  -------------------------------------------------------------------------*/
  if ( -1 == ds_qos_net_initiated_store_fltr_spec
             (
                qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr,
                qos_net_initiated_spec,
                ps_errno
              ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_qos_net_initiated_store_qos_spec(): "
                    "QoS Net Initiated Flow 0x%p: Couldn't store filter spec",
                    qos_net_initiated_req);
    ds_qos_net_initiated_delete_qos_spec(qos_net_initiated_req);
    return -1;
  }

  return 0;

} /* ds_qos_net_initiated_store_qos_spec() */

/*===========================================================================
FUNCTION DS_FWK_QOS_NET_INITIATED_REQUEST_CREATE()

DESCRIPTION
  This function creates a new qos net initiated request obejct and
  initializes it.

PARAMETERS  
  qos_net_initiated_req_ptr_ptr : OUT PARAM. QoS Net Initiated that is created
  ps_errno        : error returned back in case of an error

RETURN VALUE
   0 : if created successfully
  -1 : otherwise
DEPENDENCIES  
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_fwk_qos_net_initiated_request_create
(  
  ds_qos_net_initiated_req_type  **qos_net_initiated_req_ptr_ptr,
  int16                           *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *qos_net_initiated_req_ptr_ptr = 
    (ds_qos_net_initiated_req_type *) ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_REQ_TYPE);
  if (NULL == *qos_net_initiated_req_ptr_ptr)
  {
    *ps_errno = DS_ENOMEM;
    return -1;
  }

  memset(*qos_net_initiated_req_ptr_ptr, 0, sizeof(ds_qos_net_initiated_req_type));

  /*-------------------------------------------------------------------------
    Allocate qos net initiated info buffer from ps_mem pool
  -------------------------------------------------------------------------*/
  (*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr = 
    (qos_net_initiated_req_info_type *)ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_REQ_INFO_TYPE);

  if (NULL == (*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_qos_net_initiated_request_create(): "
                    "Couldn't create qos_net_initiated_info_ptr for qos_net_initiated_req, 0x%p",
                    *qos_net_initiated_req_ptr_ptr);
    *ps_errno = DS_ENOMEM;
    PS_MEM_FREE(*qos_net_initiated_req_ptr_ptr);
    return -1;
  }

  memset((*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr,
          0,
          sizeof(qos_net_initiated_req_info_type));

  /*-----------------------------------------------------------------------
    Initialize various lists used to store QOS info
  -----------------------------------------------------------------------*/
  list_init(&((*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list));
  list_init(&((*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list));
  
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_qos_net_initiated_request_create(): "
                  "new NIQ req 0x%p new req info 0x%p",
                  *qos_net_initiated_req_ptr_ptr,
                  (*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr);
  
  return 0;

} /* ds_fwk_qos_net_initiated_request_create() */

/*===========================================================================
FUNCTION DS_FWK_QOS_NET_INITIATED_REQUEST_CLEAN()

DESCRIPTION
  This function frees memory allocated for the provided qos net initiated
  request object (including the filters list if any were allocated)

PARAMETERS
  is_qos_supported : is QOS supported on the flow being created?
  flow_ptr         : OUT PARAM. Flow that is created
  ps_errno         : error returned back in case of an error

RETURN VALUE
   0 : if flow can be created
  -1 : otherwise

DEPENDENCIES
  ds_fwk_qos_net_initiated_request_create must be successfully completed
  before calling this function.
  Cleanup within ds_fwk_qos_net_initiated_request_create does not use
  this function.

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_qos_net_initiated_request_clean
(  
  ds_qos_net_initiated_req_type        * qos_net_initiated_req    
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_qos_net_initiated_request_clean()");

  if (NULL == qos_net_initiated_req) 
  {
     return;
  }
  
  ds_qos_net_initiated_delete_qos_spec(qos_net_initiated_req);    
  PS_MEM_FREE(qos_net_initiated_req);
  
  return;

} /* ds_fwk_qos_net_initiated_request_clean() */

/*===========================================================================
FUNCTION  DS_FWK_CREATE_QOS_NET_INITIATED_REQUEST()

DESCRIPTION
  Handles DS_FWK_CREATE_QOS_NET_INITIATED_REQUEST ioctl.

PARAMETERS
  ds_fwk_inst_ptr               : corresponding instance on which QOS is requested.
                                 This MUST be a logical instance, else an error is
                                 returned
  qos_net_initiated_request_ptr: ptr to qos net initiated request
  ps_errno                     : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  none
SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_create_qos_net_initiated_request
(
  ds_fwk_s_type                                 *ds_fwk_inst_ptr,
  ds_fwk_qos_net_initiated_request_type         *qos_net_initiated_request_ptr, 
  int16                                         *ps_errno
)
{
#ifdef FEATURE_DATA_PS_QOS
  ds_qos_net_initiated_req_type        * qos_net_initiated_req = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_create_qos_net_initiated_request()");


  if (  NULL == ds_fwk_inst_ptr ||
        NULL  == qos_net_initiated_request_ptr ||
        NULL == ps_errno)
  {
    DS_ERR_FATAL("ds_fwk_create_qos_net_initiated_request: Invalid args");
    return -1;    
  }
  
  if (ds_fwk_validate_net_initiated_qos_spec(&qos_net_initiated_request_ptr->net_initiated_qos_spec,
                                               ps_errno) != 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_create_qos_net_initiated_request(): "
                    "Invalid QOS spec is sent by application");
    /* ps_errno is populated in ds_fwk_validate_net_initiated_qos_spec() */
    return -1;
  }
                                                 
  if (ds_fwk_qos_net_initiated_request_create(&qos_net_initiated_req, ps_errno) != 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_create_qos_net_initiated_request(): "
                    "Couldn't create a buffer for PS flow");
    return -1;
  }
  
  if (ds_qos_net_initiated_store_qos_spec(
     qos_net_initiated_req,              
     &qos_net_initiated_request_ptr->net_initiated_qos_spec,
     ps_errno) != 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_create_qos_net_initiated_request(): "
                    "Couldn't store qos spec");
    ds_fwk_qos_net_initiated_request_clean(qos_net_initiated_req);      
    /* ps_errno is populated in ps_qos_net_initiated_store_qos_spec() */
    return -1;
  }      

  /* Install the new qos net initiated request object */
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  list_push_front(&(ds_fwk_inst_ptr->net_initiated_qos_req_list),
     &(qos_net_initiated_req->link));

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /* Provide back a pointer to the qos net initiated request object installed on the instance */
  qos_net_initiated_request_ptr->req_handle = (ds_net_initiated_qos_req_handle_type)qos_net_initiated_req;

  return 0;
#else
  return 0;
#endif //FEATURE_DATA_PS_QOS
} /* ds_fwk_create_qos_net_initiated_request() */

/*===========================================================================
FUNCTION  DS_FWK_RELEASE_QOS_NET_INITIATED()

DESCRIPTION
  Handles DS_FWK_RELEASE_QOS_NET_INITIATED_RELEASE ioctl.

PARAMETERS
  fwk_inst                      : corresponding instance on which QOS is requested.
  qos_net_initiated_release_ptr : ptr to qos net initiated release whih contains
                                  the qos net initiated handle.
  ps_errno                      : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_release_qos_net_initiated

(
  ds_fwk_s_type                                 * fwk_inst,
  ds_fwk_qos_net_initiated_release_type         * qos_net_initiated_release_ptr,
  int16                                         * ps_errno
)
{
#ifdef FEATURE_DATA_PS_QOS
  ds_qos_net_initiated_req_type        * qos_net_initiated_req = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_release_qos_net_initiated()");

  if (  NULL == fwk_inst || 
        NULL == qos_net_initiated_release_ptr ||
        0    == qos_net_initiated_release_ptr->req_handle ||
        NULL == ps_errno)
  {
    DS_ERR_FATAL("Invalid args to ds_fwk_release_qos_net_initiated");
    return -1;    
  }

  qos_net_initiated_req = (ds_qos_net_initiated_req_type *)qos_net_initiated_release_ptr->req_handle;
  
  /* Remove the qos net initiated request object */
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /* Remove the QoS Net Initiated object from the instance's list */
  list_pop_item(&(fwk_inst->net_initiated_qos_req_list),
                &(qos_net_initiated_req->link));

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /* Free memory of the QoS Net Initiated request object, QoS spec and filters */
  ds_fwk_qos_net_initiated_request_clean(qos_net_initiated_req);

  return 0;
#else
  return 0;
#endif //FEATURE_DATA_PS_QOS
} /* ds_fwk_release_qos_net_initiated() */

/*===========================================================================
FUNCTION ds_qos_net_initiated_check_port_range

DESCRIPTION
  This function checks if specified port and range of modem filter matches
  a specified port and range of application request.

PARAMETERS
  modem_port  - Modem filter port number
  modem_range - Modem filter port range
  app_port    - Application filter port number
  app_range   - Application filter port range
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_qos_net_initiated_check_port_range
(
  uint16 modem_port,
  uint16 modem_range,
  uint16 app_port,
  uint16 app_range
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_port_range() "
                  "modem_port: %d, modem_range: %d",
                  (int)modem_port, (int)modem_range);
  DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_port_range() "
                  "app_port: %d, app_range: %d",
                  (int)app_port, (int)app_range);

  if (modem_port > app_port)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_port_range() "
                    "modem_port > app_port");
    return FALSE;
  }

  if (modem_port + modem_range < app_port + app_range)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_port_range()" 
                    "modem_port + modem_range < app_port + app_range");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION ds_qos_net_initiated_match_fltr

DESCRIPTION
  This function matches a specified filter with another specified filter.

PARAMETERS
  fltr_ptr1 - Filter 1 which needs to be matched. This filter spec cannot have a range of UDP/TCP ports
  fltr_ptr2 - Filter 2 against the filter1 is matched. This filter spec may have a range of UDP/TCP ports
                
DEPENDENCIES
  - Filters must have the same IP version.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_qos_net_initiated_match_fltr
(
  ip_filter_type *fltr_ptr1,
  ip_filter_type *fltr_ptr2
)
{
  boolean result = FALSE;
  uint32 tmp_mask;
  uint8 next_hdr_prot = 0;
  uint8 tmp_prot;

  DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                  "trying to match filter 0x%x with 0x%x",
                  fltr_ptr1, fltr_ptr2);
  if(fltr_ptr1->ip_vsn != fltr_ptr2->ip_vsn)
  {
      DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                      "fltr_ptr1->ip_vsn != fltr_ptr2->ip_vsn. 0x%x , 0x%x",
                      fltr_ptr1->ip_vsn, fltr_ptr2->ip_vsn);
      return result;
  }

  switch(fltr_ptr1->ip_vsn)
  {
    case IP_V4:
    {
        tmp_mask = fltr_ptr1->ip_hdr.v4.field_mask;

        while(tmp_mask)
        {
          if(tmp_mask & IPFLTR_MASK_IP4_SRC_ADDR)
          {
            if((fltr_ptr1->ip_hdr.v4.src.addr.ps_s_addr &
                fltr_ptr1->ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
            (fltr_ptr2->ip_hdr.v4.src.addr.ps_s_addr &
                fltr_ptr2->ip_hdr.v4.src.subnet_mask.ps_s_addr))
            {
              DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                              "IPv4 :: Fltr1 SRC mask 0x%x and subnetmask 0x%x",
                              fltr_ptr1->ip_hdr.v4.src.addr.ps_s_addr,
                              fltr_ptr1->ip_hdr.v4.src.subnet_mask.ps_s_addr);
              DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                              "IPv4 :: Fltr1 SRC mask 0x%x and subnetmask 0x%x",
                              fltr_ptr2->ip_hdr.v4.src.addr.ps_s_addr,
                              fltr_ptr2->ip_hdr.v4.src.subnet_mask.ps_s_addr);			
              return FALSE;
            }

            tmp_mask &= ~IPFLTR_MASK_IP4_SRC_ADDR;
            continue;
          }

          if(tmp_mask & IPFLTR_MASK_IP4_DST_ADDR)
          {
            if((fltr_ptr2->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
                   && (fltr_ptr1->ip_hdr.v4.dst.addr.ps_s_addr &
                fltr_ptr1->ip_hdr.v4.dst.subnet_mask.ps_s_addr) !=
            (fltr_ptr2->ip_hdr.v4.dst.addr.ps_s_addr &
                fltr_ptr2->ip_hdr.v4.dst.subnet_mask.ps_s_addr))
            {
              DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                              "IPv4 :: DST mask and subnetmask don't match");
              return FALSE;
            }

            tmp_mask &= ~IPFLTR_MASK_IP4_DST_ADDR;
            continue;
          }

          if(tmp_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
          {
            if (PS_IPPROTO_TCP_UDP == fltr_ptr1->ip_hdr.v4.next_hdr_prot)
            {
              if ( PS_IPPROTO_TCP != fltr_ptr2->ip_hdr.v4.next_hdr_prot &&
                   PS_IPPROTO_UDP != fltr_ptr2->ip_hdr.v4.next_hdr_prot )
              {
                DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                                "Application filter is neither TCP or UDP, however NIQ protocol is TCP_UDP"
                                "0x%x, 0x%x",
                                fltr_ptr1->ip_hdr.v4.next_hdr_prot,
                                fltr_ptr2->ip_hdr.v4.next_hdr_prot);
                return FALSE;
              }
            }
            else if (fltr_ptr1->ip_hdr.v4.next_hdr_prot != fltr_ptr2->ip_hdr.v4.next_hdr_prot)
            {
              DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                              "fltr_ptr1->ip_hdr.v4.next_hdr_prot != fltr_ptr2->ip_hdr.v4.next_hdr_prot "
                              "0x%x, 0x%x",
                              fltr_ptr1->ip_hdr.v4.next_hdr_prot,
                              fltr_ptr2->ip_hdr.v4.next_hdr_prot);
              return FALSE;
            }

            next_hdr_prot = fltr_ptr1->ip_hdr.v4.next_hdr_prot;

            tmp_mask &= ~IPFLTR_MASK_IP4_NEXT_HDR_PROT;
            continue;
          }

          if(tmp_mask & IPFLTR_MASK_IP4_TOS)
          {
            if((fltr_ptr1->ip_hdr.v4.tos.val & fltr_ptr1->ip_hdr.v4.tos.mask) !=
               (fltr_ptr2->ip_hdr.v4.tos.val & fltr_ptr2->ip_hdr.v4.tos.mask))
            {
              DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                              "IPv4 :: TOS values don't match 0x%x, 0x%x",
                              fltr_ptr1->ip_hdr.v4.tos.val,
                              fltr_ptr2->ip_hdr.v4.tos.val);
              return FALSE;
            }

            tmp_mask &= ~IPFLTR_MASK_IP4_TOS;
            continue;
          }

          if (0 != tmp_mask)
          {
            DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                            "Unknown v4 filter param in mask 0x%x", tmp_mask);
          }
        }
    }
    break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
    {
      tmp_mask = fltr_ptr1->ip_hdr.v6.field_mask;
      while(tmp_mask)
      {
        if (tmp_mask & IPFLTR_MASK_IP6_SRC_ADDR)
        {
          if (FALSE == IN6_ARE_PREFIX_EQUAL(
                         &fltr_ptr1->ip_hdr.v6.src.addr,
                         &fltr_ptr2->ip_hdr.v6.src.addr,
                         fltr_ptr1->ip_hdr.v6.src.prefix_len  ))
          {
            DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "            
                            "IN6_ARE_PREFIX_EQUAL :: SRC prefix. len 1: 0x%x len 2: 0x%x",
                            fltr_ptr1->ip_hdr.v6.src.prefix_len,
                            fltr_ptr2->ip_hdr.v6.src.prefix_len);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_SRC_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_DST_ADDR)
        {
          if ((fltr_ptr2->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
                 && (FALSE == IN6_ARE_PREFIX_EQUAL(
                         &fltr_ptr1->ip_hdr.v6.dst.addr,
                         &fltr_ptr2->ip_hdr.v6.dst.addr,
                         fltr_ptr1->ip_hdr.v6.dst.prefix_len  )))
          {
            DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "              
                            "IN6_ARE_PREFIX_EQUAL :: DST prefix. len 1: 0x%x len2: 0x%x",
                            fltr_ptr1->ip_hdr.v6.dst.prefix_len,
                            fltr_ptr2->ip_hdr.v6.dst.prefix_len);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_DST_ADDR;
          continue;
        }

        /* Next hdr for IPv6 is transport protocol type */
        if (tmp_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
        {
          if (PS_IPPROTO_TCP_UDP == fltr_ptr1->ip_hdr.v6.next_hdr_prot)
          {
            if ( PS_IPPROTO_TCP != fltr_ptr2->ip_hdr.v6.next_hdr_prot &&
                 PS_IPPROTO_UDP != fltr_ptr2->ip_hdr.v6.next_hdr_prot )
            {
              DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                              "Application filter is neither TCP or UDP on v6, "
                              "however NIQ protocol is TCP_UDP on v6"
                              "0x%x, 0x%x",
                              fltr_ptr1->ip_hdr.v6.next_hdr_prot,
                              fltr_ptr2->ip_hdr.v6.next_hdr_prot);
              return FALSE;
            }
          }
          else if (fltr_ptr1->ip_hdr.v6.next_hdr_prot !=
                   fltr_ptr2->ip_hdr.v6.next_hdr_prot)
          {
            DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                            "IPv6 ext hdr mismatch 0x%x  0x%x",
                            fltr_ptr1->ip_hdr.v6.next_hdr_prot,
                            fltr_ptr2->ip_hdr.v6.next_hdr_prot);
            return FALSE;
          }

          next_hdr_prot = fltr_ptr1->ip_hdr.v6.next_hdr_prot;
          tmp_mask &= ~IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
        {
          if ((fltr_ptr1->ip_hdr.v6.trf_cls.val &
                 fltr_ptr1->ip_hdr.v6.trf_cls.mask) !=
              (fltr_ptr2->ip_hdr.v6.trf_cls.val &
                 fltr_ptr2->ip_hdr.v6.trf_cls.mask))
          {
            DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                            "TRF mismatch :: 0x%x , 0x%x ",
                            fltr_ptr1->ip_hdr.v6.trf_cls.val,
                            fltr_ptr2->ip_hdr.v6.trf_cls.val);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_TRAFFIC_CLASS;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
        {
          if (fltr_ptr1->ip_hdr.v6.flow_label !=
               fltr_ptr2->ip_hdr.v6.flow_label)
          {
            DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                            "FLOW label mismatch :: 0x%x , 0x%x ",
                            fltr_ptr1->ip_hdr.v6.flow_label,
                            fltr_ptr2->ip_hdr.v6.flow_label);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_FLOW_LABEL;
          continue;
        }

        if (0 != tmp_mask)
        {
          DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                          "Unknown v6 filter param in mask 0x%x", tmp_mask);
        }
      }
    }
    break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      /* unsupported IP version */
      DS_MSG1(MSG_LEGACY_ERROR,"Unsupported IP Version %d", fltr_ptr1->ip_vsn);
      ASSERT(0);
      return FALSE;
  }

  if(next_hdr_prot == 0)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                    "next_hdr_prot == 0, Filters Match");
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Optimization done for IP hdrs above may also be used for protocol hdr
    processing when more than 2 parameters can be specified for a protocol.
    For 2 or less parameters the scheme is less optimal and hence avoided.
  -------------------------------------------------------------------------*/
  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.tcp.field_mask))
      {
        break;
      }

      if((tmp_mask & IPFLTR_MASK_TCP_SRC_PORT) &&
         FALSE == ds_qos_net_initiated_check_port_range (
          ps_ntohs(fltr_ptr1->next_prot_hdr.tcp.src.port),
          fltr_ptr1->next_prot_hdr.tcp.src.range,
          ps_ntohs(fltr_ptr2->next_prot_hdr.tcp.src.port),
          fltr_ptr2->next_prot_hdr.tcp.src.range))
      {
        DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                        "TCP src port/range mismatch");
        return FALSE;
      }

      if((tmp_mask & IPFLTR_MASK_TCP_DST_PORT) &&
         (fltr_ptr2->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT) &&
         FALSE == ds_qos_net_initiated_check_port_range (
          ps_ntohs(fltr_ptr1->next_prot_hdr.tcp.dst.port),
          fltr_ptr1->next_prot_hdr.tcp.dst.range,
          ps_ntohs(fltr_ptr2->next_prot_hdr.tcp.dst.port),
          fltr_ptr2->next_prot_hdr.tcp.dst.range))
      {
        DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "        
                        "TCP dst port/range mismatch");
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_UDP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.udp.field_mask))
      {
        break;
      }

      if((tmp_mask & IPFLTR_MASK_UDP_SRC_PORT) &&
         FALSE == ds_qos_net_initiated_check_port_range (
           ps_ntohs(fltr_ptr1->next_prot_hdr.udp.src.port),
           fltr_ptr1->next_prot_hdr.udp.src.range,
           ps_ntohs(fltr_ptr2->next_prot_hdr.udp.src.port),
           fltr_ptr2->next_prot_hdr.udp.src.range))
      {
        DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                        "UDP src port/range mismatch");
        return FALSE;
      }

      if((tmp_mask & IPFLTR_MASK_UDP_DST_PORT) &&
           (fltr_ptr2->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT) &&
           FALSE == ds_qos_net_initiated_check_port_range (
             ps_ntohs(fltr_ptr1->next_prot_hdr.udp.dst.port),
             fltr_ptr1->next_prot_hdr.udp.dst.range,
             ps_ntohs(fltr_ptr2->next_prot_hdr.udp.dst.port),
             fltr_ptr2->next_prot_hdr.udp.dst.range))
      {
        DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                        "UDP dst port/range mismatch");
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.icmp.field_mask))
      {
        break;
      }

      if(tmp_mask & IPFLTR_MASK_ICMP_MSG_TYPE &&
         fltr_ptr1->next_prot_hdr.icmp.type != fltr_ptr2->next_prot_hdr.icmp.type)
      {
        DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                        "ICMP MSG TYPE mismatch 0x%x , 0x%x",
                        fltr_ptr1->next_prot_hdr.icmp.type,
                        fltr_ptr2->next_prot_hdr.icmp.type);
        return FALSE;
      }

      if(tmp_mask & IPFLTR_MASK_ICMP_MSG_CODE &&
         fltr_ptr1->next_prot_hdr.icmp.code != fltr_ptr2->next_prot_hdr.icmp.code)
      {
        DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "        
                        "ICMP MSG CODE mismatch 0x%x , 0x%x",
                        fltr_ptr1->next_prot_hdr.icmp.code,
                        fltr_ptr2->next_prot_hdr.icmp.code);
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_ESP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.esp.field_mask))
      {
        break;
      }
      
      if(tmp_mask & IPFLTR_MASK_ESP_SPI &&
         fltr_ptr1->next_prot_hdr.esp.spi != fltr_ptr2->next_prot_hdr.esp.spi)
      {
        DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                        "ESP SPI mismatch 0x%x  0x%x",
                        fltr_ptr1->next_prot_hdr.esp.spi,
                        fltr_ptr2->next_prot_hdr.esp.spi);
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_AH:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.ah.field_mask))
      {
        break;
      }
      
      if(tmp_mask & IPFLTR_MASK_AH_SPI &&
         fltr_ptr1->next_prot_hdr.ah.spi != fltr_ptr2->next_prot_hdr.ah.spi)
      {
        DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                        "AH SPI mismatch 0x%x  0x%x",
                        fltr_ptr1->next_prot_hdr.ah.spi,
                        fltr_ptr2->next_prot_hdr.ah.spi);
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_TCP_UDP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.tcp_udp_port_range.field_mask))
      {
        break;
      }

      if (fltr_ptr1->ip_vsn == IP_V4)
      {
        tmp_prot = fltr_ptr2->ip_hdr.v4.next_hdr_prot;
      }
      else
      {
        tmp_prot = fltr_ptr2->ip_hdr.v6.next_hdr_prot;
      }

      switch (tmp_prot)
      {
        case PS_IPPROTO_TCP:
        case PS_IPPROTO_UDP:
          if((tmp_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT) &&
               FALSE == ds_qos_net_initiated_check_port_range (
                 ps_ntohs(fltr_ptr1->next_prot_hdr.tcp_udp_port_range.src.port),
                 fltr_ptr1->next_prot_hdr.tcp_udp_port_range.src.range,
                 ps_ntohs(fltr_ptr2->next_prot_hdr.tcp_udp_port_range.src.port),
                 fltr_ptr2->next_prot_hdr.tcp_udp_port_range.src.range))
          {
            DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                            "TCP_UDP src port/range mismatch");
            return FALSE;
          }

          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&
              (fltr_ptr2->next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&
              FALSE == ds_qos_net_initiated_check_port_range (
                ps_ntohs(fltr_ptr1->next_prot_hdr.tcp_udp_port_range.dst.port),
                fltr_ptr1->next_prot_hdr.tcp_udp_port_range.dst.range,
                ps_ntohs(fltr_ptr2->next_prot_hdr.tcp_udp_port_range.dst.port),
                fltr_ptr2->next_prot_hdr.tcp_udp_port_range.dst.range))
          {
            DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                            "TCP_UDP dst port/range mismatch");
            return FALSE;
          }
          break;

        default:
          break;
      }
    }
    break;

    default:
      /* unsupported protocol */
      DS_MSG1(MSG_LEGACY_ERROR,"Unsupported protocol %d", next_hdr_prot);
      ASSERT(0);
      return FALSE;
  }

  /*-------------------------------------------------------------------------
    If we reach here, all the specified parameters matched
  -------------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_match_fltr(): "
                  "Filters Match");
  return TRUE;
} /*ds_qos_net_initiated_match_fltr*/

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_CHECK_ONE_FILTER()

DESCRIPTION
  This function receives one filter specification of Network Initiated QoS
  and checks if it matches a filter from an application request.

PARAMETERS
  app_specified_filter : pointer to a filter specified by application
  filter_spec_of_flow  : pointer to one of the filters of Network Initiated QoS

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_qos_net_initiated_check_one_filter
(  
  ip_filter_type                   * app_specified_filter,
  ip_filter_spec_type              * filter_spec_of_flow
)
{
  int                              index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_one_filter()");

  /* skipping parameters validation as this is already done in calling
     function */

  /*-------------------------------------------------------------------------
    Loop over list of filters. 
    For each one check if it matches the given app specified filter.
  -------------------------------------------------------------------------*/
  
   for (index = 0; index < filter_spec_of_flow->num_filters; index++)
      
   {
      if ( TRUE == ds_qos_net_initiated_match_fltr(
                     filter_spec_of_flow->list_ptr + index,
                     app_specified_filter
                   ) )
      {
         /* Match found */
         DS_MSG2(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_one_filter(): "
                         "Match: App NIQ req filter obj 0x%p matched NIQ filter obj 0x%p",
                         app_specified_filter,
                         filter_spec_of_flow->list_ptr + index);
         return 0;
      }
   }
  /* We went over a list of NIQ filters but
     none of them matched the application NIQ request filter. */
   DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_one_filter(): "
                   "No Match: App NIQ req filter obj 0x%p",
                   app_specified_filter);
   return -1;       
} /* ds_qos_net_initiated_check_one_filter() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_CHECK_FLOW()

DESCRIPTION
  Check if a specific flow can fulfill a request's requirements

PARAMETERS
  flow_ptr                   : pointer to the flow
  net_initiated_qos_spec_ptr : pointer to the request

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_qos_net_initiated_check_flow
(
  ds_flow_type                * flow_ptr,
  net_initiated_qos_spec_type * net_initiated_qos_spec_ptr
)
{
  ip_filter_type       * rx_ip_filter_ptr = NULL, * tx_ip_filter_ptr = NULL;
  int                    num_rx_filters = 0;
  int                    num_tx_filters = -1;
  int                    i;
  int32                  retval = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_flow()");
  
  /* skipping parameters validation as this is already done in calling
     function */

  /*-------------------------------------------------------------------------
    Generate rx_filter_spec.
  -------------------------------------------------------------------------*/
  num_rx_filters = flow_ptr->qos_info_ptr->rx.fltr_template.num_filters;
  if(0 != num_rx_filters)
  {
    /*-------------------------------------------------------------------------
       Loop over all rx filters specified by flow.
  -------------------------------------------------------------------------*/
    for (i = 0; i < net_initiated_qos_spec_ptr->rx_fltr_template.num_filters; i++)
    {
      rx_ip_filter_ptr =
            &(net_initiated_qos_spec_ptr->rx_fltr_template.list_ptr[i]);
      if ( -1 == ds_qos_net_initiated_check_one_filter(rx_ip_filter_ptr,
                                    &(flow_ptr->qos_info_ptr->rx.fltr_template)) )
      {
        /* Did not find any filter in the qos_spec of the flow that matches
                 this filter that the application specified for network initiated QOS
                 notification => The flow does not match application's
                 specification */
        DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_flow : "
           "RX Filter 0x%p in NIQ application request does not match new flow's QoS Spec",
                        rx_ip_filter_ptr);
        retval = -1;
        goto bail;
      }
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_flow() "
            "Zero RX filters, ds_flow 0x%p", flow_ptr);
    if (0 < net_initiated_qos_spec_ptr->rx_fltr_template.num_filters) {
      retval = -1;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Generate tx_filter_spec.
  -------------------------------------------------------------------------*/
  num_tx_filters =  flow_ptr->qos_info_ptr->tx.fltr_template.num_filters;
  if(0 != num_tx_filters)
  {
    /*-------------------------------------------------------------------------
       Loop over all tx filters specified by flow.
  -------------------------------------------------------------------------*/
    for (i = 0; i < net_initiated_qos_spec_ptr->tx_fltr_template.num_filters; i++)
    {
      tx_ip_filter_ptr = &(net_initiated_qos_spec_ptr->tx_fltr_template.list_ptr[i]);
      if ( -1 == ds_qos_net_initiated_check_one_filter(tx_ip_filter_ptr,
                                 &(flow_ptr->qos_info_ptr->tx.fltr_template)) )
      {
         /* Did not find any filter in the qos_spec of the flow that matches
               this filter that the application specified for network initiated QOS
               notification => The flow does not match application's
               specification */
        DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_flow(): "
                 "TX Filter 0x%p in NIQ application request" 
                 " does not match new flow's QoS Spec",
                 tx_ip_filter_ptr);
        retval = -1;
        goto bail;
      }
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_flow(): "
                    "Zero TX filters, ds_flow 0x%p", flow_ptr);
    if (0 < net_initiated_qos_spec_ptr->tx_fltr_template.num_filters) {
      retval = -1;
      goto bail;
    }
  }
  /* We went over all filters specified by the application and for each one we
     found a matching filter in the new flow's QoS Specification */
  retval = 0;

bail:
  DS_MSG4(MSG_LEGACY_HIGH,"ds_qos_net_initiated_check_flow(): "
          "Flow filters rx:%d, tx:%d, App filters rx:%d, tx:%d",
                  num_rx_filters,
                  num_tx_filters,
                  net_initiated_qos_spec_ptr->rx_fltr_template.num_filters,
                  net_initiated_qos_spec_ptr->tx_fltr_template.num_filters);
  return retval;

} /* ds_qos_net_initiated_check_flow() */


/*===========================================================================
FUNCTION DS_FWK_NET_INITIATED_UPDATE_FLOW()

DESCRIPTION
  Goes over all Network Initiated QoS application requests.
  finds the handle match and update flow pointer passed.

PARAMETERS
  ds_fwk_instance_ptr         : corresponding instance on which QOS is requested
  flow_ptr                 : flow to be modified
  req_handle             : NIQ request handle

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_fwk_net_initiated_update_flow
(
  ds_fwk_s_type                         * ds_fwk_inst_ptr,
  ds_flow_type                          * flow_ptr,
  ds_net_initiated_qos_req_handle_type    req_handle
)
{
  ds_qos_net_initiated_req_type    * ds_qos_net_initiated_req;
  qos_spec_type                    * qos_spec_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_net_initiated_update_flow()"
                  "inst_ptr 0x%p, flow_ptr 0x%p, qos_spec_ptr 0x%p",
                  ds_fwk_inst_ptr, flow_ptr, qos_spec_ptr);

  if( flow_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_net_initiated_update_flow():invalid flow");
    return -1;
  }
  
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ds_qos_net_initiated_req = 
    list_peek_front(&(ds_fwk_inst_ptr->net_initiated_qos_req_list));
  while (NULL != ds_qos_net_initiated_req)
  {
    if ( req_handle == 
       (ds_net_initiated_qos_req_handle_type)ds_qos_net_initiated_req)
    {
      if( ds_qos_net_initiated_req->qos_net_initiated_req_private.flow_ptr == NULL )
      {
        ds_qos_net_initiated_req->qos_net_initiated_req_private.flow_ptr = flow_ptr;
        DS_MSG1(MSG_LEGACY_HIGH,"Updating flow pointer in NIQ context %p", 
                ds_qos_net_initiated_req->qos_net_initiated_req_private.flow_ptr);
      }
    }
    ds_qos_net_initiated_req =
      list_peek_next( &(ds_fwk_inst_ptr->net_initiated_qos_req_list),
                      &(ds_qos_net_initiated_req->link) );
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
} /* ps_qos_net_initiated_check_requests() */

/*===========================================================================
FUNCTION  DS_FWK_QOS_NET_INITIATED_CHECK()

DESCRIPTION
  Handles NIQ request comparison with network filters.

PARAMETERS
  clnt_type      : type of client
  clnt_handle    : client handle with which the call was bought up.
  qos_net_initiated_real_time_check_ptr : pointer to structure containing the
                                          qos_spec for the logical flows to be
                                          checked. This MUST not be NULL.
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES

SIDE EFFECTS
  If there is a flow that fits the QoS spec requirements, the request callback
  will be called.
===========================================================================*/
int ds_fwk_qos_net_initiated_check
(
  ds_fwk_client_type                           clnt_type,
  ds_fwk_clnt_handle_type                      clnt_handle,
  ds_fwk_qos_net_initiated_check_type        * qos_net_initiated_check_ptr,
  int16                                      * ps_errno
)
{
#ifdef FEATURE_DATA_PS_QOS
  ds_fwk_clnt_info_s_type        clnt_info_ptr;
  ds_flow_type                 * flow_ptr              = NULL;
  list_type*                     sec_flow_list         =NULL;
  ds_fwk_qos_matching_flow_type* matching_flow_ptr     = NULL;
  int                            retval = -1;
  int16                          temp_ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_qos_net_initiated_check()");
  if (NULL == qos_net_initiated_check_ptr ||
      NULL == ps_errno)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid args to ds_fwk_qos_net_initiated_check");
    return -1;    
  }

  ds_fwk_get_clnt_info_from_handle(clnt_type,clnt_handle,&clnt_info_ptr);
  if( clnt_info_ptr.fw_inst != NULL )
  {
    if( clnt_info_ptr.ip_type == IPV4_ADDR )
    {
      sec_flow_list = &(clnt_info_ptr.fw_inst->v4_pdn_ptr->\
                      inst_ptr[DS_PDN_INSTANCE_IPV4]->flow->sec_flow_list);
    }
    else if( clnt_info_ptr.ip_type == IPV6_ADDR )
    {
      sec_flow_list = &(clnt_info_ptr.fw_inst->v6_pdn_ptr->\
                      inst_ptr[DS_PDN_INSTANCE_IPV6]->flow->sec_flow_list);
    }
    else if( clnt_info_ptr.ip_type == NON_IP_ADDR )
    {
      sec_flow_list = &(clnt_info_ptr.fw_inst->non_ip_pdn_ptr->\
                      inst_ptr[DS_PDN_INSTANCE_NON_IP]->flow->sec_flow_list);
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid IP type %d", clnt_info_ptr.ip_type);
      return -1;
    }
  }
  else
  {
    DS_MSG2(MSG_LEGACY_ERROR,"client handle %d client type %d",
            clnt_handle,clnt_type);
    return -1;
  }
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  flow_ptr = list_peek_front(sec_flow_list);
  temp_ps_errno = DS_EWOULDBLOCK;
  while (flow_ptr != NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_qos_net_initiated_check : "
             "Comparing Flow 0x%p for NIQ request", flow_ptr);
    if ( 0 == ds_qos_net_initiated_check_flow(flow_ptr,
         &qos_net_initiated_check_ptr->net_initiated_qos_spec) )
    {
       DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_qos_net_initiated_check : "
                       "Flow 0x%p has a match for NIQ request. Notifying application",
                       flow_ptr);
       DS_SYSTEM_HEAP_MEM_ALLOC(matching_flow_ptr, sizeof(ds_fwk_qos_matching_flow_type),
                                ds_fwk_qos_matching_flow_type*);
       retval = 0;
       temp_ps_errno = 0;
       if (NULL == matching_flow_ptr)
       {
         /*------------------------------------------------------------------
           Free the memory already allocated 
         ------------------------------------------------------------------*/
         matching_flow_ptr = (ds_fwk_qos_matching_flow_type*)
           list_pop_front(&qos_net_initiated_check_ptr->matching_flow_list);
         while ( NULL != matching_flow_ptr )
         {
            DS_SYSTEM_HEAP_MEM_FREE(matching_flow_ptr);
            matching_flow_ptr = (ds_fwk_qos_matching_flow_type*)
              list_pop_front(&qos_net_initiated_check_ptr->matching_flow_list);
         }
         DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
         *ps_errno = DS_ENOMEM;
         return -1;
       }
       
       matching_flow_ptr->flow_ptr = flow_ptr;
       ds_fwk_net_initiated_update_flow(clnt_info_ptr.fw_inst,
                                        flow_ptr,
                                        qos_net_initiated_check_ptr->req_handle );
       list_push_back(
         &qos_net_initiated_check_ptr->matching_flow_list,
         &matching_flow_ptr->link);
    }
    
    flow_ptr = list_peek_next(sec_flow_list,
                              &flow_ptr->link);
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  if ((DS_EWOULDBLOCK == temp_ps_errno) && (*ps_errno != DS_ENOMEM))
  {
    retval = -1;
    *ps_errno = DS_EWOULDBLOCK;
  }
  return retval;
#else
  return 0;
#endif // FEATURE_DATA_PS_QOS


} /* ds_fwk_qos_net_initiated_check() */

int ds_fwk_qos_get_granted_flow_spec
(
  ds_fwk_qos_get_granted_flow_spec_type*   flow_spec_ptr
)
{
  ds_flow_type* flow_ptr = (ds_flow_type*)flow_spec_ptr->qos_handle;
  if( flow_ptr == NULL || flow_ptr->bearer_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"invalid flow handle or bearer ptr");
    return -1;
  }
  if( flow_ptr->state == DS_FLOW_STATE_DOWN )
  {
    flow_spec_ptr->qos_status = QOS_UNAVAILABLE;
  }
  else if( flow_ptr->state == DS_FLOW_STATE_UP )
  {
    flow_spec_ptr->qos_status = QOS_AVAILABLE;
  }
  else
  {
    flow_spec_ptr->qos_status = QOS_STATE_INVALID;
  }
  memscpy(&flow_spec_ptr->rx_flow,sizeof(ip_flow_type),
        &flow_ptr->qos_info_ptr->rx.flow_template.granted_flow,sizeof(ip_flow_type));
  memscpy(&flow_spec_ptr->tx_flow,sizeof(ip_flow_type),
        &flow_ptr->qos_info_ptr->tx.flow_template.granted_flow,sizeof(ip_flow_type));
  flow_spec_ptr->bearer_id = flow_ptr->bearer_ptr->bearer_id;

  return 0;
}



/*===========================================================================
FUNCTION DS_FWK_NET_INITIATED_CHECK_ONE_REQUEST()

DESCRIPTION
  This function receives filters specification of Network Initiated QoS
  and checks if there is any corresponding application request, i.e.
  if any application registered a request to be notified when QoS with such
  filters specification is established by the network.

PARAMETERS
  ds_qos_net_initiated_req : pointer to an application request object
  qos_spec_ptr             : pointer to filters specifications of the
                             Network Initiated QoS

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_fwk_net_initiated_check_one_request
(
  ds_qos_net_initiated_req_type    * ds_qos_net_initiated_req,
  qos_spec_type                    * qos_spec_ptr
)
{
  ds_qos_net_initiated_req_filter_type * filter_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_net_initiated_check_one_request(): "
                  "App NIQ req obj 0x%p NIQ obj 0x%p",
                  ds_qos_net_initiated_req, qos_spec_ptr);

  /*-------------------------------------------------------------------------
    skipping parameters validation as this is already done in calling
    function
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Loop over all tx filters specified by application.
    For each one we look for a matching filter in the provided qos
    specification of the flow.
    All tx filters specified by application must have matching filters
    in the qos spec of the flow. Otherwise there is no match.
    qos spec of the flow may have additional filters not specified by the app.
    In such case the qos spec of the flow is a subset of the spec specified 
    by the application so the application should be interested to use that 
    flow.
  -------------------------------------------------------------------------*/
  
  filter_buf_ptr = list_peek_front(
    &(ds_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list)
  );
  while (filter_buf_ptr != NULL)
  {
    if ( -1 ==  ds_qos_net_initiated_check_one_filter(&filter_buf_ptr->filter,
                                                      &qos_spec_ptr->tx.fltr_template) )
    {
       /* Did not find any filter in the qos_spec of the new flow that matches
          this filter that the application specified for network initiated QOS
          notification => The new flow does not match application's
          specification */          
       DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_net_initiated_check_one_request(): "
                       "App NIQ req TX Filter 0x%p doesn't match new NIQ Spec 0x%p",
                       filter_buf_ptr, qos_spec_ptr);
       return -1;       
    }

    filter_buf_ptr = list_peek_next( 
      &(ds_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list),
      &(filter_buf_ptr->link) 
    );
  }    

  /*-------------------------------------------------------------------------
    Loop over all rx filters specified by application.
  -------------------------------------------------------------------------*/
  
  filter_buf_ptr = list_peek_front(
    &(ds_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list)
  );
  while (filter_buf_ptr != NULL)
  {
    if ( -1 == ds_qos_net_initiated_check_one_filter(&filter_buf_ptr->filter,
                                                     &qos_spec_ptr->rx.fltr_template) )
    {
       /* Did not find any filter in the qos_spec of the new flow that matches
          this filter that the application specified for network initiated QOS
          notification => The new flow does not match application's
          specification */          
       DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_net_initiated_check_one_request(): "
                       "App NIQ req RX Filter 0x%p doesn't match new NIQ Spec 0x%p",
                       filter_buf_ptr, qos_spec_ptr);
       return -1;       
    }

    filter_buf_ptr = list_peek_next( 
      &(ds_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list),
      &(filter_buf_ptr->link) 
    );
  }    

  /* We went over all filters specified by the application and for each one we
     found a matching filter in the new flow's QoS Specification */
  
  DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_net_initiated_check_one_request(): "
                  "SUCCESS");
  return 0;

} /* ps_qos_net_initiated_check_one_request() */

boolean ds_fwk_get_instance_info_from_flow
(
  ds_flow_type       * flow_ptr,
  ip_addr_enum_type  * ip_type,
  ds_fwk_s_type     ** ds_fwk_inst_ptr
)
{
  ds_fwk_index_type fw_index = 0;

  if( flow_ptr != NULL && flow_ptr->inst_ptr != NULL)
  {
    if( flow_ptr->inst_ptr->type == DS_PDN_INSTANCE_IPV4 )
    {
      *ip_type = IPV4_ADDR;
    }
    else if( flow_ptr->inst_ptr->type == DS_PDN_INSTANCE_IPV6 )
    {
      *ip_type = IPV6_ADDR;
    }
    else if( flow_ptr->inst_ptr->type == DS_PDN_INSTANCE_NON_IP )
    {
      *ip_type = NON_IP_ADDR;
    }
    if( flow_ptr->bearer_ptr->pdn_mgr_ptr != NULL)
    {
      fw_index = flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index;
     *ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(fw_index);
      if( *ds_fwk_inst_ptr == NULL)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"recieved event on invalid flow");
        return FALSE;
      }
    }
    else
    {
      DS_MSG0(MSG_LEGACY_ERROR,"recieved event on invalid flow");
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION DS_FWK_NET_INITIATED_CHECK_REQUESTS()

DESCRIPTION
  Goes over all Network Initiated QoS application requests.
  For each one checks if the filters of the provided QoS Spec matches the filters
  of the application request. If yes notifies the application on the existence
  of matching Network initiated QoS.

PARAMETERS
  ds_fwk_instance_ptr         : corresponding instance on which QOS is requested
  ip_type ...................ip_type of flow
  flow_ptr          : flow to be modified
  event      : event due to which this request is invoked

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_fwk_net_initiated_check_requests
(
  ds_fwk_s_type                    * ds_fwk_inst_ptr,
  ip_addr_enum_type                  ip_type,
  ds_pdn_event_enum_type             event,
  ds_flow_type                     * flow_ptr
)
{
  ds_qos_net_initiated_req_type    * ds_qos_net_initiated_req;
  ds_pdn_event_info_u_type           event_info;
  ds_flow_type                     * req_flow_ptr = NULL;
  qos_spec_type                    * qos_spec_ptr = NULL;
  ds_pdn_event_enum_type             clnt_event; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_net_initiated_check_requests()"
                  "inst_ptr 0x%p, flow_ptr 0x%p, qos_spec_ptr 0x%p",
                  ds_fwk_inst_ptr, flow_ptr, qos_spec_ptr);

  if( flow_ptr != NULL && flow_ptr->qos_info_ptr != NULL)
  {
    qos_spec_ptr = flow_ptr->qos_info_ptr;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_net_initiated_check_requests():invalid flow");
    return -1;
  }
  event_info.net_initiated_qos_info.qos_handle = (ds_qos_handle_type)flow_ptr;
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  
  /*-------------------------------------------------------------------------
     Go over application requests to be notified on matching network initiated
     QoS. For each one check the filters of the new flow.
     below is the algo:
     1) picks each NIQ request from the niq request list of ds_fwk_instance_ptr and 
         performs below actions.
     2) if the event is flow added or modified event.and it is matching with the client NIQ request
         for the first time then the available event would be invoked.
     3) if the event is modified and the flow is matching with earlier NIQ request where available 
         indication already sent then modified event will be sent.
     4) if the event is flow deleted or modified( and still not matched with any of the new filters)
         and if the flow is matching with any of previously matched apps then the unavilable
         event needs to issued
  -------------------------------------------------------------------------*/
  ds_qos_net_initiated_req = 
    list_peek_front(&(ds_fwk_inst_ptr->net_initiated_qos_req_list));
  while (NULL != ds_qos_net_initiated_req)
  {
    req_flow_ptr = ds_qos_net_initiated_req->qos_net_initiated_req_private.flow_ptr;
    clnt_event = PDN_MIN_EV;
    event_info.net_initiated_qos_info.req_handle =
         (int32)ds_qos_net_initiated_req;
    if ( 0 == ds_fwk_net_initiated_check_one_request(ds_qos_net_initiated_req,
                                                     qos_spec_ptr) )
    {
       DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_net_initiated_check_requests(): "
               "NIQ Request 0x%p has a match for flow %p. Notifying application",
               ds_qos_net_initiated_req, req_flow_ptr);
       if( ( event == PDN_FLOW_ADDED_EV || event == PDN_FLOW_MODIFIED_EV )&&
            req_flow_ptr == NULL )
       {
         ds_qos_net_initiated_req->qos_net_initiated_req_private.\
           flow_ptr = flow_ptr;
         clnt_event = PDN_NET_INITIATED_QOS_AVAILABLE_EV;
       }
       else if( event == PDN_FLOW_MODIFIED_EV && req_flow_ptr == flow_ptr )
       {
         clnt_event = PDN_NET_AVAILABLE_QOS_MODIFIED_EV;
       }
       /*Safe check: this could happen when the event is recieved 
               before the filters deletion*/
       else if( event == PDN_FLOW_DELETED_EV && req_flow_ptr == flow_ptr )
       {
         clnt_event = PDN_NET_INITIATED_QOS_UNAVAILABLE_EV;
       }
    }
    else if( ( (event == PDN_FLOW_MODIFIED_EV) || (event == PDN_FLOW_DELETED_EV) ) && 
              req_flow_ptr == flow_ptr )
    {
      clnt_event = PDN_NET_INITIATED_QOS_UNAVAILABLE_EV;
    }

    DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_net_initiated_check_requests(): "
                       "Req_flow_ptr %p event %d",
                       ds_qos_net_initiated_req, clnt_event);
    /*--------------------------------------------------------------------- 
        Notify application on Network Initiated QoS matching application
        specified filters 
        -------------------------------------------------------------------*/	
    if( clnt_event != PDN_MIN_EV )
       {
         ds_fwk_notify_events_ex( ds_fwk_inst_ptr->ds_fwk_index,
                                ip_type,
                                DS_FWK_CLIENT_SOCKETS,
                               clnt_event,
                                event_info);
       }
    ds_qos_net_initiated_req =
      list_peek_next( &(ds_fwk_inst_ptr->net_initiated_qos_req_list),
                      &(ds_qos_net_initiated_req->link) );
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
} /* ps_qos_net_initiated_check_requests() */

void ds_fwk_qos_net_initiated_handle_event
(
  ds_pdn_event_enum_type    event,
  ds_pdn_event_info_u_type  event_info
)
{
  ds_fwk_s_type* ds_fwk_inst_ptr = NULL;
  ds_net_initiated_qos_event_info_type* qos_info = 
     (ds_net_initiated_qos_event_info_type*)&(event_info.net_initiated_qos_info);
  ds_flow_type* flow_ptr = (ds_flow_type*)qos_info->qos_handle;
  ip_addr_enum_type ip_type = IPV4_ADDR;
  DS_MSG2(MSG_LEGACY_HIGH,"Flow event %d recieved on 0x%x flow_ptr",
                           event,flow_ptr);
  if( !ds_fwk_get_instance_info_from_flow(flow_ptr,&ip_type,&ds_fwk_inst_ptr))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"recieved event on invalid flow");
    return;
  }
  switch( event )
  {
    case PDN_FLOW_ADDED_EV:
    case PDN_FLOW_DELETED_EV:
    case PDN_FLOW_MODIFIED_EV:
      ds_fwk_net_initiated_check_requests(  ds_fwk_inst_ptr,
                                            ip_type,
                                            event,
                                            flow_ptr
                                            );
      break;
    default:
      break;
  }
}

#endif /* FEATURE_DATA_PS_QOS */

