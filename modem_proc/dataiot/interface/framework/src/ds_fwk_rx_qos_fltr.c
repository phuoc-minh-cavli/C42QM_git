/*===========================================================================

                 DS_FWK _ R X _ Q O S _ F L T R . C

DESCRIPTION
  This file provides functions to manipulate QOS filters which are used by
  network to filter traffic in forward direction.

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

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_fwk_rx_qos_fltr.c#1 $
  $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/25/06    msr    L4/Tasklock code review changes
02/06/06    msr    Updated for L4 tasklock/crit sections.
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    mct    Fixed naming causing some issues w/C++ compilers.
08/03/05    msr    Added ds_fwk_rx_qos_fltr_modify().
05/12/05    ks     fixed lint errors
05/03/05    msr    Using int16 instead of errno_enum_type.
04/17/05    msr    File created.
===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_QOS

#include "ds_ipfltr.h"
#include "ps_mem.h"
#include "ds_defs.h"
#include "ds_fwk.h"
#include "ps_utils.h"
#include "ds_crit_sect.h"
#include "ds_fwk_rx_qos_fltr.h"
#include "ds_flow.h"

/*===========================================================================

                             LOCAL DEFINITIONS

===========================================================================*/
#define MAX_FLTR_PRCD  255



/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_FWK_RX_QOS_FLTR_STORE_PTRS()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified instance. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given instance for the given ps_flow.

PARAMETERS
  ds_fwk_inst_ptr          : Iface to add filters to
  qos_info_ptr       : address where Rx fltrs are stored in a ps_flow. Could
                       be qos_info_ptr or qos_modify_info_ptr
  fltr_prcd          : precedence to be assigned to fltrs
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of stored filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                    : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static ds_rx_qos_fltr_handle_type ds_fwk_rx_qos_fltr_store_ptrs
(
  ds_fwk_s_type                 * ds_fwk_inst_ptr,
  qos_spec_type                 * qos_spec_ptr,
  uint8                           fltr_prcd,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
)
{
  ds_fwk_rx_qos_fltr_node_type     * new_filter_node_ptr;
  ds_fwk_rx_qos_fltr_node_type     * tmp_rx_fltr_buf;
  ds_fwk_rx_fltr_buf_type          * fltr_buf_ptr;
  ds_rx_qos_fltr_handle_type         fltr_handle;
  uint8                              fltr_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( qos_spec_ptr == NULL || qos_spec_ptr->rx.fltr_template.num_filters == 0 )
  {
    *ps_errno = DS_EINVAL;
     DS_MSG0(MSG_LEGACY_ERROR,"Invalid args");
     return DS_IPFLTR_INVALID_HANDLE;
  }

  fltr_handle = DS_IPFLTR_INVALID_HANDLE;
  for (fltr_index = 0 ; 
       fltr_index < qos_spec_ptr->rx.fltr_template.num_filters ;fltr_index++ )
  {
   
    fltr_buf_ptr = (ds_fwk_rx_fltr_buf_type *)
                    ps_mem_get_buf(PS_MEM_RX_QOS_FLTR_BUF_TYPE);
    if (fltr_buf_ptr == NULL)
    {
      *ps_errno = DS_ENOMEM;
       DS_MSG0(MSG_LEGACY_ERROR,"No Memory");
       return DS_IPFLTR_INVALID_HANDLE;
    }
    fltr_buf_ptr->filter = 
           &(qos_spec_ptr->rx.fltr_template.list_ptr[fltr_index]);
    /*-----------------------------------------------------------------------
      Get a buffer for rx filter from ps_mem. Store filters in the
      decreasing order of precedence.
    -----------------------------------------------------------------------*/
    new_filter_node_ptr =
      (ds_fwk_rx_qos_fltr_node_type *)
        ps_mem_get_buf(PS_MEM_RX_QOS_FLTR_NODE_TYPE);
    if (new_filter_node_ptr == NULL)
    {
      DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_rx_qos_fltr_store_ptrs:"
                        " fw instance index %d",ds_fwk_inst_ptr->ds_fwk_index);
      ds_fwk_rx_qos_fltr_delete(ds_fwk_inst_ptr, fltr_handle);
      //PS_MEM_FREE( fltr_buf_ptr );not needed as we are deleting in above function.
      *ps_errno = DS_ENOMEM;
      return DS_IPFLTR_INVALID_HANDLE;
    }

    if (fltr_handle == DS_IPFLTR_INVALID_HANDLE)
    {
      fltr_handle = (uint32) new_filter_node_ptr;
    }

    new_filter_node_ptr->fltr_buf_ptr             = fltr_buf_ptr;
    new_filter_node_ptr->fltr_handle              = fltr_handle;
    new_filter_node_ptr->fltr_buf_ptr->precedence = fltr_prcd++;

    /*-------------------------------------------------------------------------
       Add the new filters to the flow sorted by precedence
    -------------------------------------------------------------------------*/
    tmp_rx_fltr_buf =
      list_peek_front(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list));

    while (tmp_rx_fltr_buf != NULL &&
           new_filter_node_ptr->fltr_buf_ptr->precedence >=
             tmp_rx_fltr_buf->fltr_buf_ptr->precedence)
    {
      tmp_rx_fltr_buf =
        list_peek_next(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                       &(tmp_rx_fltr_buf->link));
    }

    if (NULL == tmp_rx_fltr_buf)
    {
      list_push_back(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                     &(new_filter_node_ptr->link));
    }
    else
    {
      list_push_before(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                       &(new_filter_node_ptr->link), &(tmp_rx_fltr_buf->link));
    }
  } /* while there is a fltr in the list */

  return fltr_handle;
} /* ds_fwk_rx_qos_fltr_store_ptrs() */



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_FWK_RX_QOS_FLTR_ADD()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified instance. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given instance for the given ps_flow.

PARAMETERS
  ds_fwk_inst_ptr          : Iface to add filters to
  qos_spec_ptr       : address where Rx fltrs are stored in a ps_flow. Could
                       be qos_info_ptr or qos_modify_info_ptr
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                   : on failure

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
)
{
  ds_fwk_rx_qos_fltr_node_type     * filter_node_ptr;
  ds_rx_qos_fltr_handle_type     fltr_handle;
  uint8                                fltr_prcd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL || qos_spec_ptr == NULL || ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_rx_qos_fltr_add: invalid args %d passed");
    DS_ASSERT(0);
    return DS_IPFLTR_INVALID_HANDLE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_rx_qos_fltr_add: instance %d",
                            ds_fwk_inst_ptr->ds_fwk_index);

  /*-------------------------------------------------------------------------
    Find a block in precedence space which can be used to assign precedence
    for newly added filters
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  filter_node_ptr =
    list_peek_back(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list));
  if (filter_node_ptr == NULL)
  {
    fltr_prcd = 0;
  }
  else
  {
    fltr_prcd = filter_node_ptr->fltr_buf_ptr->precedence;

    if ((fltr_prcd + MAX_FLTR_PER_REQ) < MAX_FLTR_PRCD) /* Blk found at end */
    {
      /*---------------------------------------------------------------------
        Get next multiple of MAX_FLTR_PER_REQ
      ---------------------------------------------------------------------*/
      fltr_prcd += MAX_FLTR_PER_REQ - (fltr_prcd % MAX_FLTR_PER_REQ);
    }
    else /* Wrap around happened */
    {
      /*---------------------------------------------------------------------
        Get first block in precedence space which is available. Since FCFS
        with blocks are used to assign precedence, a partially full block is
        not considered as a hole
      ---------------------------------------------------------------------*/
      fltr_prcd = 0;
      filter_node_ptr =
        list_peek_front(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list));
      while (filter_node_ptr != NULL)
      {
        if (filter_node_ptr->fltr_buf_ptr->precedence > fltr_prcd)
        {
          break;
        }
        else
        {
          /*-----------------------------------------------------------------
            Skip this block since precedence is already used
          -----------------------------------------------------------------*/
          if (filter_node_ptr->fltr_buf_ptr->precedence == fltr_prcd)
          {
            fltr_prcd += MAX_FLTR_PER_REQ;
          }

          filter_node_ptr =
            list_peek_next(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                           &(filter_node_ptr->link));
        }
      }

      if (filter_node_ptr == NULL)
      {
        DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_rx_qos_fltr_add: instance %d"
                                "Could not find a hole for precedence",
                                 ds_fwk_inst_ptr->ds_fwk_index);
        DS_ASSERT(0);
        *ps_errno = DS_ENOMEM;
        return DS_IPFLTR_INVALID_HANDLE;
      }
    }
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_rx_qos_fltr_add(): "
                  "Assigning fltr precedence from %d", fltr_prcd);

  fltr_handle = ds_fwk_rx_qos_fltr_store_ptrs(ds_fwk_inst_ptr,
                                                 qos_spec_ptr,
                                                 fltr_prcd,
                                                 fltr_compare_f_ptr,
                                                 ps_errno);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return fltr_handle;

} /* ds_fwk_rx_qos_fltr_add() */



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
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  Must be called only if existing Rx fltrs are modified in QOS_MODIFY
  instance critical section must have been entered

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
)
{
  ds_fwk_rx_qos_fltr_node_type     * filter_node_ptr;
  ds_rx_qos_fltr_handle_type         fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,
           "ds_fwk_rx_qos_fltr_modify(): NULL parameter is passed");
    return DS_IPFLTR_INVALID_HANDLE;
  }

  if ( qos_spec_ptr == NULL || ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_rx_qos_fltr_modify(): invalid args");
    *ps_errno = DS_EFAULT;
    return DS_IPFLTR_INVALID_HANDLE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_rx_qos_fltr_modify(): "
          "Modifying pointers to Rx fltrs on instance %d",
          ds_fwk_inst_ptr->ds_fwk_index);

  /*-------------------------------------------------------------------------
    Find a block in precedence space which can be used to assign precedence
    for newly added filters
  -------------------------------------------------------------------------*/
  filter_node_ptr =
    list_peek_front(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list));
  while (filter_node_ptr != NULL &&
         filter_node_ptr->fltr_handle != rx_fltr_handle)
  {
    filter_node_ptr =
      list_peek_next(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                     &(filter_node_ptr->link));
  }

  if (filter_node_ptr == NULL)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_rx_qos_fltr_modify(): "
            "Couldn't find filters to modify on instance %d. Passed "
            "fltr_handle is 0x%x",
             ds_fwk_inst_ptr->ds_fwk_index, rx_fltr_handle);
    DS_ASSERT(0);
    *ps_errno = DS_EFAULT;
    return DS_IPFLTR_INVALID_HANDLE;
  }

  fltr_handle = ds_fwk_rx_qos_fltr_store_ptrs
                (
                  ds_fwk_inst_ptr,
                  qos_spec_ptr,
                  filter_node_ptr->fltr_buf_ptr->precedence,
                  fltr_compare_f_ptr,
                  ps_errno
                );
  return fltr_handle;

} /* ds_fwk_rx_qos_fltr_modify() */



/*===========================================================================
FUNCTION DS_FWK_RX_QOS_FLTR_DELETE()

DESCRIPTION
  This function deletes all the existing Rx QOS filters identified by the
  specified filter handle from the specified instance.

PARAMETERS
  ds_fwk_inst_ptr      : Iface to delete filters from
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
  ds_fwk_s_type                * ds_fwk_inst_ptr,
  ds_rx_qos_fltr_handle_type     rx_fltr_handle
)
{
  ds_fwk_rx_qos_fltr_node_type  * curr_fltr_node_ptr;
  ds_fwk_rx_qos_fltr_node_type  * next_fltr_node_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"invalid args");
    return;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_rx_qos_fltr_delete(): "
                  "Deleting pointers to Rx fltrs on instance %d",
                  ds_fwk_inst_ptr->ds_fwk_index);

  /*-------------------------------------------------------------------------
    Free Rx filters from instance and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  curr_fltr_node_ptr =
    list_peek_front(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list));
  while (curr_fltr_node_ptr != NULL)
  {
    next_fltr_node_ptr =
      list_peek_next(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                     &(curr_fltr_node_ptr->link));
    if (curr_fltr_node_ptr->fltr_handle == rx_fltr_handle)
    {
      list_pop_item(&(ds_fwk_inst_ptr->rx_qos_fltr_node_list),
                    &(curr_fltr_node_ptr->link));
      PS_MEM_FREE( curr_fltr_node_ptr->fltr_buf_ptr );
      PS_MEM_FREE( curr_fltr_node_ptr );
    }

    curr_fltr_node_ptr = next_fltr_node_ptr;
  } /* while there is a fltr in the list */

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ds_fwk_rx_qos_fltr_delete() */

#endif /* FEATURE_DATA_PS_QOS */

