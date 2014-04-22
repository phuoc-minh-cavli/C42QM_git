#ifndef DS_FWK_RX_QOS_FLTR_H
#define DS_FWK_RX_QOS_FLTR_H

/*===========================================================================

                  DS_FWK _ R X _ Q O S _ F L T R . H

DESCRIPTION
  Header file for declaration of functions to manipulate Rx QOS filters

EXTERNALIZED FUNCTIONS
  DS_FWK_RX_QOS_FLTR_ADD()
    Adds pointers to Rx filters to the instance. Rx filters are actually
    stored in ps_flow

  DS_FWK_RX_QOS_FLTR_MODIFY()
    Adds pointers to Rx filters to the instance. Rx filters are actually
    stored in ps_flow. This function is called when existing Rx fltrs are
    modified in QOS_MODIFY

  DS_FWK_RX_QOS_FLTR_DELETE()
    Deletes pointers to Rx filters from the instance. Rx filters are actually
    stored in ps_flow

Copyright (c) 2005-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds_fwk_ipfltr.h_v   1.0   07 Feb 2003 20:14:34   ubabbar  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/inc/ds_fwk_rx_qos_fltr.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/15/18    sk      created file
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_QOS
#include "list.h"
#include "ds_defs.h"

/*===========================================================================

                          REGIONAL DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
TYPEDEF DS_FWK_RX_QOS_FLTR_BUF_TYPE

DESCRIPTION
  This is the type that is used to enqueue Rx QOS filters in a ps_flow
===========================================================================*/
typedef struct
{
  ip_filter_type*  filter;
  uint8            precedence;
} ds_fwk_rx_fltr_buf_type;

/*===========================================================================
TYPEDEF DS_FWK_RX_QOS_FLTR_NODE_TYPE

DESCRIPTION
  This is the type that is used to enqueue pointers to Rx QOS filters in a
  ds_fwk.
===========================================================================*/
typedef struct
{
  list_link_type                      link;
  ds_fwk_rx_fltr_buf_type           * fltr_buf_ptr;
  ds_rx_qos_fltr_handle_type          fltr_handle;
} ds_fwk_rx_qos_fltr_node_type;



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_FWK_RX_QOS_FLTR_ADD()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified instance. The
  filter set is tied to a filter id, which uniquely idenifies a filter
  for the given instance for the given QOS client.

PARAMETERS
  ds_fwk_inst_ptr          : instance to add filters to
  qos_spec_ptr       : address where Rx fltrs are stored in a ps_flow. Could
                       be qos_info_ptr or qos_modify_info_ptr
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  DS_IPFLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_rx_qos_fltr_handle_type ds_fwk_rx_qos_fltr_add
(
  ds_fwk_s_type                 * ds_fwk_inst_ptr,
  qos_spec_type                 * qos_spec_ptr,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
);

/*===========================================================================
FUNCTION DS_FWK_RX_QOS_FLTR_MODIFY()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified instance. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given instance for the given ps_flow.

PARAMETERS
  ds_fwk_inst_ptr          : instance to add filters to
  qos_spec_ptr       : address where Rx fltrs are stored in a ps_flow. Has
                       to be qos_modify_info_ptr
  rx_fltr_handle     : handle to existing Rx fltrs specified in current QOS
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  DS_IPFLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  Must be called only if existing Rx fltrs are modified in QOS_MODIFY

SIDE EFFECTS
  None
===========================================================================*/
ds_rx_qos_fltr_handle_type ds_fwk_rx_qos_fltr_modify
(
  ds_fwk_s_type                     * ds_fwk_inst_ptr,
  qos_spec_type                     * qos_spec_ptr,
  ds_rx_qos_fltr_handle_type          rx_fltr_handle,
  ipfltr_comparison_f_ptr_type        fltr_compare_f_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION DS_FWK_RX_QOS_FLTR_DELETE()

DESCRIPTION
  This function deletes all the existing Rx QOS filters ientified by the
  specified filter handle from the specified instance.

PARAMETERS
  ds_fwk_inst_ptr      : instance to delete filters from
  rx_fltr_handle : handle identifying filters to be deleted

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_rx_qos_fltr_delete
(
  ds_fwk_s_type                     * ds_fwk_inst_ptr,
  ds_rx_qos_fltr_handle_type          rx_fltr_handle
);

#endif /* FEATURE_DATA_PS_QOS */
#endif /* DS_FWK_RX_QOS_FLTR_H */
