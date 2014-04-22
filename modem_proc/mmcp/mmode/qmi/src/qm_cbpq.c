/*===========================================================================

                        QM_CBPQ.C 

DESCRIPTION

  QMI_MMODE source file for cmd_buf pending queue module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/16/13    sm     Initial version
===========================================================================*/

#include "qm_cbpq.h"
#include "queue.h"
#include "msg.h"

//===========================================================================
// GLOBALS
//===========================================================================
struct qm_cbpq_pending_s
{
  q_link_type        link;         /* must be first      */
  qmi_client_handle                clnt;        /* client ID for item */
  qmi_mmode_qcsi_transaction_type *cmd_buf_p;    /* item to queue      */
};

struct qm_cbpq_global_s
{
  q_type net_scan_pend_q;
  #ifndef FEATURE_MMODE_DISABLE_UIM
  q_type get_pref_nwks_pend_q;
  #endif
};

static struct qm_cbpq_global_s qm_cbpq_global;

//===========================================================================
// Function declarations
//===========================================================================

/*===========================================================================
  FUNCTION QM_CBPQ_INIT()

  DESCRIPTION
    This function initializes the cmd_buf pending queue variables

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_init( void )
{
  (void) q_init( &qm_cbpq_global.net_scan_pend_q );
  #ifndef FEATURE_MMODE_DISABLE_UIM
  (void) q_init( &qm_cbpq_global.get_pref_nwks_pend_q );
  #endif
}

/*===========================================================================
  FUNCTION GET_QUEUE_FROM_TYPE()

  DESCRIPTION
    This function gets the queue from global based on the given qtype

  PARAMETERS
    queue_type : type to retrieve the queue for

  RETURN VALUE
    q_type : queue for the given type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static q_type * get_queue_from_type( enum qm_cbpq_qtype_e queue_type )
{
  q_type * ret_val = NULL;

  switch( queue_type )
  {
    case QM_CBPQ_QTYPE_NW_SCAN:
      ret_val = &qm_cbpq_global.net_scan_pend_q;
      break;
    #ifndef FEATURE_MMODE_DISABLE_UIM
    case QM_CBPQ_QTYPE_GET_PREF:
      ret_val = &qm_cbpq_global.get_pref_nwks_pend_q;
      break;
    #endif
    default:
      QM_MSG_ERROR_1("Unhandled queue type %d", queue_type);
      break;
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION QM_CBPQ_ADD()

  DESCRIPTION
    This function adds the provided item to the indicated type.

  PARAMETERS
    queue_type : type for the caller
    queue_item : pointer to element to queue
    clnt      : client id for item to queue

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_add
( 
  enum qm_cbpq_qtype_e queue_type, 
  qmi_mmode_qcsi_transaction_type * queue_item, 
  qmi_client_handle clnt 
)
{
  struct qm_cbpq_pending_s *pending_item;
  q_type                   *q_to_add;
    
  q_to_add = get_queue_from_type( queue_type );
  if ( q_to_add != NULL && queue_item != NULL )
  {
    pending_item = (struct qm_cbpq_pending_s *) modem_mem_alloc( sizeof(struct qm_cbpq_pending_s), MODEM_MEM_CLIENT_QMI_MMODE );
    if ( pending_item != NULL )
    {
      pending_item->clnt = clnt;
      pending_item->cmd_buf_p = queue_item;

      q_put( q_to_add, q_link( &pending_item, &pending_item->link) );
    }
    else
    {
      QM_MSG_ERROR("Failed to allocate memory for queue item");
    }
  }
}

/*===========================================================================
  FUNCTION QM_CBPQ_GET()

  DESCRIPTION
    This function retrieves the first element for the indicated type.

  PARAMETERS
    queue_type : type for the caller

  RETURN VALUE
    void * : pointer to element from queue

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_mmode_qcsi_transaction_type *qm_cbpq_get( enum qm_cbpq_qtype_e queue_type )
{
  struct qm_cbpq_pending_s *pending_item;
  qmi_mmode_qcsi_transaction_type         *ret_val = NULL;
  q_type                   *q_to_get;

  q_to_get = get_queue_from_type( queue_type );

  if ( q_to_get != NULL )
  {
    pending_item = q_get( q_to_get );

    if ( pending_item != NULL )
    {
      ret_val = pending_item->cmd_buf_p;
      modem_mem_free( pending_item, MODEM_MEM_CLIENT_QMI_MMODE );
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION QM_CBPQ_COUNT()

  DESCRIPTION
    This function determines the count of items for indicated type.

  PARAMETERS
    queue_type : type for the caller

  RETURN VALUE
    int : count of items for the type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qm_cbpq_count( enum qm_cbpq_qtype_e queue_type )
{
  q_type * q_to_count = get_queue_from_type( queue_type );

  if ( q_to_count != NULL )
  {
    return q_cnt( q_to_count );
  }
  else
  {
    return 0;
  }
}

/*===========================================================================
  FUNCTION ITEM_IS_INVALID()

  DESCRIPTION
    Comparison function to remove elements from internal qmi_nas pending queues

  PARAMETERS
    pending: pointer to queue element for comparison
    state: state to compare to.
===========================================================================*/
static int item_is_invalid
(
  struct qm_cbpq_pending_s *pending,
  qmi_mmode_svc_info_type  *state
)
{

  qmi_mmode_qcsi_transaction_type *pend_trans;
  
  if( state == NULL ) return FALSE;

  pend_trans = (qmi_mmode_qcsi_transaction_type *)q_check(&(state->pending_req));
  while ( pend_trans != NULL )
  {
    if( pend_trans->req_handle == pending->cmd_buf_p->req_handle )
    {
      return FALSE;
    }

    pend_trans = q_next(&(state->pending_req), &(pend_trans->link));
  }

  QM_MSG_HIGH("Buffer does not match existing client transaction");
  return TRUE;
}

/*===========================================================================
  FUNCTION QM_CBPQ_DEL_INVALID_ITEMS()

  DESCRIPTION
    This function is used to remove invalid items from the indicated type
    
  PARAMETERS
    queue_type : type for the caller

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_del_invalid_items( enum qm_cbpq_qtype_e queue_type )
{
  void * item = NULL;
  q_type * q_to_process = get_queue_from_type( queue_type );
  
  if ( q_to_process != NULL )
  {
    while( (item = q_linear_delete_new( q_to_process,
                                        (q_compare_func_type) item_is_invalid,
                                        &qmi_nas_state,
                                        (q_action_func_type) NULL,
                                        NULL)) != NULL )
    {
      modem_mem_free( item, MODEM_MEM_CLIENT_QMI_MMODE );
    }
  }
}

/*===========================================================================
  FUNCTION CL_ID_BELONGS_TO_CLIENT()

  DESCRIPTION
    Comparison function to remove elements from internal qmi_nas pending queues

  PARAMETERS
    pending: pointer to queue element for comparison
    cl_id: pointer to client id to remove
===========================================================================*/
static int cl_id_belongs_to_client
(
  struct qm_cbpq_pending_s *pending,
  qmi_client_handle         clnt
)
{
  return ( pending->clnt == clnt ) ? TRUE : FALSE;
}

/*===========================================================================
  FUNCTION QM_CBPQ_DEL_CLIENT_ITEMS()

  DESCRIPTION
    This function is used to remove a specific clients items from the indicated type
    
  PARAMETERS
    queue_type : type for the caller
    cl_sp      : client to remove items for 

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_del_client_items( enum qm_cbpq_qtype_e queue_type, qmi_client_handle clnt )
{
  void * item = NULL;
  q_type * q_to_process = get_queue_from_type( queue_type );
  
  if ( q_to_process != NULL )
  {
    while( (item = q_linear_delete_new( q_to_process,
                                        (q_compare_func_type) cl_id_belongs_to_client,
                                         clnt,
                                        (q_action_func_type) NULL,
                                        NULL)) != NULL )
    {
      modem_mem_free( item, MODEM_MEM_CLIENT_QMI_MMODE );
    }
  }
}

/*===========================================================================
FUNCTION QMI_NAS_CMD_BUF_BELONGS_TO_CLIENT_TRANSACTION

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
       the queue.

DEPENDENCIES
  None 

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static int cmd_buf_matches
(
  struct qm_cbpq_pending_s  *pending_ptr,       // searched value
  qmi_mmode_qcsi_transaction_type          *comp_param_ptr   // comparison value
)
{
  return (comp_param_ptr == (qmi_mmode_qcsi_transaction_type *)pending_ptr->cmd_buf_p) ? TRUE : FALSE;
}

/*===========================================================================
  FUNCTION QM_CBPQ_DEL_SPECIFIC_ITEM()

  DESCRIPTION
    This function is used to remove specific item from the indicated type
    
  PARAMETERS
    queue_type : type for the caller
    del_item   : item to remove

  RETURN VALUE
    void * to item removed.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_mmode_qcsi_transaction_type *qm_cbpq_del_specific_item( enum qm_cbpq_qtype_e queue_type, qmi_mmode_qcsi_transaction_type *del_item )
{
  struct qm_cbpq_pending_s * item = NULL;
  qmi_mmode_qcsi_transaction_type *ret_val = NULL;
  q_type * q_to_process = get_queue_from_type( queue_type );
  
  if ( q_to_process != NULL )
  {
    item = (struct qm_cbpq_pending_s*) q_linear_delete_new( q_to_process,
                                                            (q_compare_func_type) cmd_buf_matches,
                                                            del_item,
                                                            (q_action_func_type) NULL,
                                                            NULL);

    if ( item != NULL )
    {
      ret_val = item->cmd_buf_p;
      modem_mem_free( item, MODEM_MEM_CLIENT_QMI_MMODE );
    }
  }

  return ret_val;
}

