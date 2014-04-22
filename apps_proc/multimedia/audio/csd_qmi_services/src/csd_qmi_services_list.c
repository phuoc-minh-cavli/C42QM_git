/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S _ L I S T . C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file for queues.


Copyright (c) 2011, 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services_list.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/15/16    rk      Abstracting OSAL layer
07/03/13    vk      Changes to remove compiler warnings
09/12/11    akj     Created file (initial version)

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "csd_qmi_services_common.h"
#include "csd_list.h"
#include "csd_status.h"
#include "csd_debug_msg.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
struct qmi_csd_services_client_info_obj
{
  uint32_t    index;
  struct csd_list_node 	node;    
  struct csd_cmd_token_param_t qmi_client_info;
/*
  uint32_t  cmd_type;     //csd_comment type
  uint32_t  cmd_token;
  uint32_t  cmd_resp_ptr;
  client_info_type *clnt_info;
  qmi_req_handle req_handle;
  int32_t msg_id;
  service_context_type *service_cookie;
*/
};

static struct qmi_csd_services_client_info_obj	*qmi_csd_services_client_info_list_ptr = NULL; 

static struct csd_list qmi_csd_services_client_info_free_list;
static struct csd_list qmi_csd_services_client_info_used_list;

static uint32_t qmi_csd_list_init_done = 0;
static uint32_t qmi_csd_cmd_token_count = 0;

#define QMI_SERVICE_CLIENT_INFO_LIST_SIZE 20

/*=============================================================================
              QMI Services List Find Type
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_vc_list_init
=============================================================================*/
/*!
@brief
  This function is used to find the key value in the list

*/
/*=========================================================================*/
bool_t qmi_services_list_find_type(struct csd_list_node* node, uint32_t key)
{
  bool_t rc = FALSE;

  struct qmi_csd_services_client_info_obj *service_obj = (struct qmi_csd_services_client_info_obj *)node->data;
  
  if( NULL != service_obj ) 
  {
    if( service_obj->qmi_client_info.cmd_token == (key ^ service_obj->index) )
    {
      rc = TRUE;
    }
  }
  return rc;
}
/*=============================================================================
              QMI CSD Client Info List Initialization Function
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_client_info_list_init
=============================================================================*/
/*!
@brief
  This function is used to Initialize the qmi message information which can be
  used during the callback to the QMI client for the Asyncroneous calls of CSD
  This funtion is called when the csd_init called.

*/
/*=========================================================================*/
uint32_t qmi_csd_client_info_list_init(void)
{	
  uint32_t csd_rc = CSD_EOK;

  if( qmi_csd_list_init_done == 0 )
  {
    /* Intialise the token counter for the list */
    qmi_csd_cmd_token_count = 0;

    csd_malloc( QMI_SERVICE_CLIENT_INFO_LIST_SIZE*sizeof(*qmi_csd_services_client_info_list_ptr), 
                   (void**)&qmi_csd_services_client_info_list_ptr );

    if( qmi_csd_services_client_info_list_ptr == NULL )
    {
      csd_rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_client_info_list_init: Unable to allocate memory for qmi_csd_services_client_info_list_ptr" );
    }
  
    if( csd_rc == CSD_EOK )
    {
      csd_memset( qmi_csd_services_client_info_list_ptr, 0, 
                     sizeof(*qmi_csd_services_client_info_list_ptr)*QMI_SERVICE_CLIENT_INFO_LIST_SIZE );
    
      csd_rc = csd_list_init( &qmi_csd_services_client_info_free_list );
      CSD_MSG( CSD_PRIO_MEDIUM, "qmi_csd_client_info_list_init: csd_list_init(qmi_csd_services_client_info_free_list), csd_rc[0x%x]", csd_rc );
    }
    
    if( CSD_EOK == csd_rc ) 
    {
      csd_rc = csd_list_init_key( &qmi_csd_services_client_info_used_list, &qmi_services_list_find_type );
      CSD_MSG( CSD_PRIO_MEDIUM, "qmi_csd_client_info_list_init: csd_list_init_key(qmi_csd_services_client_info_used_list), csd_rc[0x%x]", csd_rc );
    }

    if( CSD_EOK == csd_rc ) 
    {
      uint16_t i;
      for( i = 0; i < QMI_SERVICE_CLIENT_INFO_LIST_SIZE; i++ ) 
      {
        qmi_csd_services_client_info_list_ptr[i].index = 0;
        qmi_csd_services_client_info_list_ptr[i].node.data = &(qmi_csd_services_client_info_list_ptr[i]);
        csd_list_add_tail( &qmi_csd_services_client_info_free_list, &(qmi_csd_services_client_info_list_ptr[i].node) );
      }
      CSD_MSG( CSD_PRIO_LOW, "qmi_csd_client_info_list_init: List init success" ); 
      qmi_csd_list_init_done = 1;
    }

    if( CSD_EOK != csd_rc )
    {
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_client_info_list_init: List init failed" );
      csd_free( qmi_csd_services_client_info_list_ptr );
      qmi_csd_services_client_info_list_ptr = NULL;	
      csd_rc = CSD_EFAILED;
    }
  }
  else 
  {
    CSD_MSG( CSD_PRIO_LOW, "qmi_csd_client_info_list_init: List already initialized" );    
  }

  return csd_rc;
}

/*=============================================================================
              QMI CSD Client Info List Initialization Function
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_client_info_list_dinit
=============================================================================*/
/*!
@brief
  This function is used for deinitializing the qmi_csd_client_info list which
  stores the client msg information used for the callback function. This 
  function is called in CSD_Dinit.

*/
/*=========================================================================*/
uint32_t qmi_csd_client_info_list_dinit(void)
{
  struct csd_list_node* node = NULL;
  uint32_t count_free, count_used;
   
  if( qmi_csd_list_init_done == 1 )
  {
    count_free = csd_list_get_count( &qmi_csd_services_client_info_free_list );
    count_used = csd_list_get_count( &qmi_csd_services_client_info_used_list );

    CSD_MSG( CSD_PRIO_LOW, "qmi_csd_client_info_list_dinit: Free list count[%d], Used list count[%d]", count_free, count_used );

    /* Free the node */
    do
    {
      csd_list_get_head( &qmi_csd_services_client_info_free_list, &node );
      if( node )
      {
        csd_list_del( &qmi_csd_services_client_info_free_list, node );
      }
    }while( node );
    
    csd_list_dinit( &qmi_csd_services_client_info_free_list );
    CSD_MSG( CSD_PRIO_MEDIUM, "qmi_csd_client_info_list_dinit: dinit free list success" );

    /* Free the node */
    do
    {
      csd_list_get_head( &qmi_csd_services_client_info_used_list, &node );
      if( node )
      {
        csd_list_del( &qmi_csd_services_client_info_used_list, node );
      }
    }while( node );
    
    csd_list_dinit( &qmi_csd_services_client_info_used_list );
    CSD_MSG( CSD_PRIO_MEDIUM, "qmi_csd_client_info_list_dinit: dinit used list success" );

    /* Reset the token counter values */
    qmi_csd_cmd_token_count = 0;

    if( qmi_csd_services_client_info_list_ptr != NULL ) 
    {
      csd_free( qmi_csd_services_client_info_list_ptr );
      qmi_csd_services_client_info_list_ptr = NULL;
      CSD_MSG( CSD_PRIO_MEDIUM, "qmi_csd_client_info_list_dinit: freed qmi_csd_services_client_info_list_ptr" );
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_client_info_list_dinit: Unexpected NULL qmi_csd_services_client_info_list_ptr" );
    }
    qmi_csd_list_init_done = 0;
  }
  else
  { 
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_client_info_list_dinit: List was not initialized, nothing to free" );
  }
    
  return CSD_EOK;
}
/*=============================================================================
              QMI CSD Client Info Set List function
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_client_info_set_list
=============================================================================*/
/*!
@brief
  This function adds the client data to the link list
@param[in]   client_info        QMI client data required during the callback

@retval    CSD_EOK              Success
@retval    CSD_EFAILED          Error, No empty node in free list
*/
/*=========================================================================*/
uint32_t qmi_csd_client_info_set_list( csd_cmd_token_param_t *client_info)
{	
  struct qmi_csd_services_client_info_obj* obj_ptr;
  struct csd_list_node* node_ptr = NULL;
  uint32_t csd_rc = CSD_EFAILED;

  csd_rc = csd_list_get_head( &qmi_csd_services_client_info_free_list, &node_ptr );
  
  if( csd_rc != CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_client_info_set_list: Failed to get node from the free list, all [%d] free node has been used", QMI_SERVICE_CLIENT_INFO_LIST_SIZE );
    return CSD_ENORESOURCE;
  }
  
  obj_ptr = node_ptr->data;
  
  obj_ptr->index                          = ++qmi_csd_cmd_token_count;
  obj_ptr->qmi_client_info.cmd_token      = client_info->cmd_token;
  obj_ptr->qmi_client_info.cmd_type       = client_info->cmd_type;
  obj_ptr->qmi_client_info.clnt_info      = client_info->clnt_info;
  obj_ptr->qmi_client_info.req_handle     = client_info->req_handle;
  obj_ptr->qmi_client_info.msg_id         = client_info->msg_id;
  obj_ptr->qmi_client_info.service_cookie = client_info->service_cookie;
  obj_ptr->qmi_client_info.cmd_resp_ptr   = client_info->cmd_resp_ptr;

  client_info->cmd_token = obj_ptr->index ^ client_info->cmd_token;
  
  csd_list_add_tail( &qmi_csd_services_client_info_used_list, node_ptr );

  return csd_rc;
}

/*=============================================================================
              QMI CSD Client Info Get list function
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_client_info_get_list
=============================================================================*/
/*!
@brief
  This function gets the list from the token id provide by the client
@param[in]   client_info        QMI client data required during the callback

@retval    CSD_EOK              Success
@retval    CSD_EFAILED          Error, No node selected for the cmd_token.
*/
/*=========================================================================*/
uint32_t qmi_csd_client_info_get_list ( csd_cmd_token_param_t *client_info )
{
  struct qmi_csd_services_client_info_obj* obj_ptr;
  struct csd_list_node* node_ptr = NULL;
  uint32_t csd_rc;

  csd_rc = csd_list_find( &qmi_csd_services_client_info_used_list, &node_ptr, client_info->cmd_token );
  if( csd_rc != CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_client_info_set_list: failed to get the node in used list from the given csd_token [0x%x]", client_info->cmd_token );
    return csd_rc;
  }
  
  obj_ptr                     = node_ptr->data;
  client_info->cmd_token      = obj_ptr->qmi_client_info.cmd_token;
  client_info->cmd_type       = obj_ptr->qmi_client_info.cmd_type; 
  client_info->clnt_info      = obj_ptr->qmi_client_info.clnt_info; 
  client_info->req_handle     = obj_ptr->qmi_client_info.req_handle; 
  client_info->msg_id         = obj_ptr->qmi_client_info.msg_id; 
  client_info->service_cookie = obj_ptr->qmi_client_info.service_cookie;
  client_info->cmd_resp_ptr   = obj_ptr->qmi_client_info.cmd_resp_ptr;

  csd_list_del( &qmi_csd_services_client_info_used_list, node_ptr );

  csd_memset( &obj_ptr->qmi_client_info, 0, sizeof(struct csd_cmd_token_param_t) );
  csd_list_add_head( &qmi_csd_services_client_info_free_list, node_ptr );

  return csd_rc;
}


