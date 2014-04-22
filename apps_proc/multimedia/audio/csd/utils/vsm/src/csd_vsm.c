/*============================================================================

   FILE:       csd_vsm.c

        Copyright © 2014, 2016 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 $Header:
 $Author:

when       who      what, where, why
--------   ---      ----------------------------------------------------------
11/15/16    rk       Abstracting OSAL layer
06/20/12    jk       creation
============================================================================*/

#include "csd_vsm.h"
#include "csd_handle.h"
#include "csd.h"
#include "csd_debug_msg.h"
#include "csd_list.h"
#include "apr_errcodes.h"
#include "apr_lock.h"
#include "csd_error_map.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
struct csd_vsm_session_obj
{
  struct csd_list_node 	node;    
  csd_vsm_session_t session_info;
};

#define VSM_SESSION_LIST_SIZE 10

static struct csd_list  csd_vsm_session_used_list;
static struct csd_list  csd_vsm_session_free_list;

/* Lock management */
static csd_lock_t csd_vsm_update_lock;


static uint32_t csd_vsm_list_init_done = 0;

static struct csd_vsm_session_obj	*csd_vsm_session_list_ptr = NULL; 


/************************************************************/
/* helper functions                                         */
/************************************************************/

bool_t csd_vsm_list_find_session_by_name(struct csd_list_node* node, void* key)
{
  bool_t rc = FALSE;

  struct csd_vsm_session_obj *session = (struct csd_vsm_session_obj *)node->data;
  
  if( NULL != session ) 
  {
    if (strncmp(session->session_info.name, (const char*)key, sizeof(session->session_info.name)) == 0 )
    {
      rc = TRUE;
    }
  }
  return rc;
}

bool_t csd_vsm_list_find_session_by_handle(struct csd_list_node* node, void *key)
{
  bool_t rc = FALSE;

  struct csd_vsm_session_obj *session = (struct csd_vsm_session_obj *)node->data;
  
  if( NULL != session ) 
  {
    if (session->session_info.vc_handle.handle == *(uint32_t*)key ||
        session->session_info.vs_handle.handle == *(uint32_t*)key || 
        session->session_info.vm_handle.handle == *(uint32_t*)key )
    {
      rc = TRUE;
    }
  }
  return rc;
}

uint32_t csd_vsm_add_handles_to_list(csd_vsm_handle_t handle_type, 
                                     csd_vsm_session_state_t session_state, 
                                     csd_vsm_handle_list_t *handle_list,
                                     csd_vsm_session_t session )
{
  uint32_t rc = CSD_EOK;
  uint32_t loc = *(handle_list->ret_write_size)/4;
  switch ( handle_type.handle_type )
  {
    case CSD_VSM_VS_HANDLE:
      if ( session_state == session.state && session.vs_handle.handle != CSD_VSM_NONE_HANDLE )
      {
        handle_list->buffer[loc] = session.vs_handle.handle;
        *( handle_list->ret_write_size ) += 4;
        *( handle_list->ret_total_size ) += 4;
      }
      break;
    case CSD_VSM_VC_HANDLE:
      if ( session_state == session.state && session.vc_handle.handle != CSD_VSM_NONE_HANDLE )
      {
        handle_list->buffer[loc] = session.vc_handle.handle;
        *( handle_list->ret_write_size ) += 4;
        *( handle_list->ret_total_size ) += 4;
      }
      break;
    case CSD_VSM_VM_HANDLE:
      if ( session_state == session.state && session.vm_handle.handle != CSD_VSM_NONE_HANDLE)
      {
        handle_list->buffer[loc] = session.vm_handle.handle;
        *( handle_list->ret_write_size ) += 4;
        *( handle_list->ret_total_size ) += 4;
      }
      break;
    case CSD_VSM_ALL_HANDLE:
       if ( session_state == session.state )
      {
        handle_list->buffer[loc] = session.vs_handle.handle;
        handle_list->buffer[loc+1] = session.vc_handle.handle;
        handle_list->buffer[loc+2] = session.vm_handle.handle;
        *( handle_list->ret_write_size ) += 12;
        *( handle_list->ret_total_size ) += 12;
      }
      break;
    default:
        CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_add_handles_to_list: Invalid Handle Type %d", handle_type.handle_type );
        rc = CSD_EBADPARAM;
      break;
  }

  return rc;
}

uint32_t csd_vsm_add_assoc_handles_to_list(csd_vsm_handle_t handle_type, 
                                     csd_vsm_session_state_t session_state, 
                                     csd_vsm_assoc_handle_list_t *handle_list,
                                     csd_vsm_session_t session )
{
  uint32_t rc = CSD_EOK;
  uint32_t loc = *(handle_list->ret_write_size)/4;
  switch ( handle_type.handle_type )
  {
    case CSD_VSM_VS_HANDLE:
      if ( session_state == session.state && session.vs_handle.handle != CSD_VSM_NONE_HANDLE )
      {
        handle_list->buffer[loc] = session.vs_handle.handle;
        *( handle_list->ret_write_size ) += 4;
        *( handle_list->ret_total_size ) += 4;
      }
      break;
    case CSD_VSM_VC_HANDLE:
      if ( session_state == session.state && session.vc_handle.handle != CSD_VSM_NONE_HANDLE )
      {
        handle_list->buffer[loc] = session.vc_handle.handle;
        *( handle_list->ret_write_size ) += 4;
        *( handle_list->ret_total_size ) += 4;
      }
      break;
    case CSD_VSM_VM_HANDLE:
      if ( session_state == session.state && session.vm_handle.handle != CSD_VSM_NONE_HANDLE )
      {
        handle_list->buffer[loc] = session.vm_handle.handle;
        *( handle_list->ret_write_size ) += 4;
        *( handle_list->ret_total_size ) += 4;
      }
      break;
    case CSD_VSM_ALL_HANDLE:
       if ( session_state == session.state )
      {
        handle_list->buffer[loc] = session.vs_handle.handle;
        handle_list->buffer[loc+1] = session.vc_handle.handle;
        handle_list->buffer[loc+2] = session.vm_handle.handle;
        *( handle_list->ret_write_size ) += 12;
        *( handle_list->ret_total_size ) += 12;
      }
      break;
    default:
        CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_add_assoc_handles_to_list: Invalid Handle Type %d", handle_type.handle_type );
        rc = CSD_EBADPARAM;
      break;
  }

  return rc;
}


/************************************************************/
/* APIs                                                     */
/************************************************************/
uint32_t csd_vsm_init( void )
{
  uint32_t rc = CSD_EOK;
  uint32_t csd_osal_rc = APR_EOK;

  if ( csd_vsm_list_init_done == 0 )
  {
  
    //allocate space for free list
    csd_malloc( VSM_SESSION_LIST_SIZE*sizeof(struct csd_vsm_session_obj), 
                     (void**)&csd_vsm_session_list_ptr );

    if( csd_vsm_session_list_ptr == NULL )
    {
      rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_init: Unable to allocate memory for csd_vsm_session_list_ptr" );
    }
    
    if( rc == CSD_EOK ) 
    {
      csd_memset( csd_vsm_session_list_ptr, 0, 
                     sizeof(struct csd_vsm_session_obj)*VSM_SESSION_LIST_SIZE );
      
      rc = csd_list_init( &csd_vsm_session_free_list );
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_vsm_init: csd_list_init(csd_vsm_session_free_list), rc[0x%x]", rc );
    }
      
    if( CSD_EOK == rc ) 
    {
      rc = csd_list_init_key2( &csd_vsm_session_used_list, &csd_vsm_list_find_session_by_name );
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_vsm_init: csd_list_init_key(csd_vsm_session_used_list), rc[0x%x]", rc );
    }

    if( CSD_EOK == rc ) 
    {
      uint16_t i;
      for( i = 0; i < VSM_SESSION_LIST_SIZE; i++ ) 
      {
        csd_list_add( &csd_vsm_session_free_list, &(csd_vsm_session_list_ptr[i].node), &(csd_vsm_session_list_ptr[i]) );
      }
      CSD_MSG( CSD_PRIO_LOW, "csd_vsm_init: List init success" ); 
    }

    if( CSD_EOK != rc )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_init: List init failed" );
      csd_free( csd_vsm_session_list_ptr );
      csd_vsm_session_list_ptr = NULL;	
      rc = CSD_EFAILED;
    }
  }
  else 
  {
    CSD_MSG( CSD_PRIO_LOW, "csd_vsm_init: List already initialized" );    
  }

  csd_osal_rc = csd_lock_create( &csd_vsm_update_lock );
  rc = csd_map_apr_error_to_csd ( csd_osal_rc );

  return rc;
}

uint32_t csd_vsm_deinit( void )
{
  struct csd_list_node* node = NULL;
  uint32_t count_free, count_used;
   
  if( csd_vsm_list_init_done == 1 )
  {
    count_free = csd_list_get_count( &csd_vsm_session_free_list );
    count_used = csd_list_get_count( &csd_vsm_session_used_list );

    CSD_MSG( CSD_PRIO_LOW, "csd_vsm_deinit: Free list count[%d], Used list count[%d]", count_free, count_used );

    /* Free the node */
    do
    {
      csd_list_get_head( &csd_vsm_session_free_list, &node );
      if( node )
      {
        csd_list_del( &csd_vsm_session_free_list, node );
      }
    }while( node );
    
    csd_list_dinit( &csd_vsm_session_free_list );
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_vsm_deinit: dinit free list success" );

    /* Free the node */
    do
    {
      csd_list_get_head( &csd_vsm_session_used_list, &node );
      if( node )
      {
        csd_list_del( &csd_vsm_session_used_list, node );
      }
    }while( node );
    
    csd_list_dinit( &csd_vsm_session_used_list );
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_vsm_deinit: dinit used list success" );


    if( csd_vsm_session_list_ptr != NULL ) 
    {
      csd_free( csd_vsm_session_list_ptr );
      csd_vsm_session_list_ptr = NULL;
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_vsm_deinit: freed csd_vsm_session_list_ptr" );
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_deinit: Unexpected NULL csd_vsm_session_list_ptr" );
    }

    ( void ) csd_lock_destroy ( csd_vsm_update_lock );
    csd_vsm_list_init_done = 0;
  }
  else
  { 
    CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_deinit: List was not initialized, nothing to free" );
  }
    
  return CSD_EOK;
}

uint32_t csd_vsm_cleanup()
{
  uint32_t rc = CSD_EOK;
  struct csd_list_node* node_ptr = NULL;
  struct csd_vsm_session_obj *obj_ptr = NULL;

  do
  {
    rc = csd_list_read_head( &csd_vsm_session_used_list, &node_ptr );
    if( node_ptr )
    {
      obj_ptr = node_ptr->data;
      if (obj_ptr->session_info.vm_handle.state != CSD_VSM_HANDLE_STATE_UNINITIALIZED )
      {
        csd_close(obj_ptr->session_info.vm_handle.handle);
      }
      if (obj_ptr->session_info.vc_handle.state != CSD_VSM_HANDLE_STATE_UNINITIALIZED )
      {
        csd_close(obj_ptr->session_info.vc_handle.handle);
      }
      if (obj_ptr->session_info.vs_handle.state != CSD_VSM_HANDLE_STATE_UNINITIALIZED )
      {
        csd_close(obj_ptr->session_info.vs_handle.handle);
      }
    }
  }
  while( node_ptr );

  return rc;
}

uint32_t csd_vsm_get_handles( csd_vsm_handle_t handle_type, 
                              csd_vsm_session_state_t session_state, 
                              void *param )
{
  uint32_t rc = CSD_EOK;
  struct csd_list_node* node_ptr = NULL;
  struct csd_vsm_session_obj *obj_ptr = NULL;

  if ( !param )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_get_handles: Failed handle_list is NULL!!" );
    return CSD_EBADPARAM;
  }
  /*get only associative session if handle is set*/
  if ( handle_type.assoc_mode )
  {
    csd_vsm_assoc_handle_list_t *handle_list = (csd_vsm_assoc_handle_list_t*)param;
      
    if ( !handle_list->buffer || !handle_list->ret_write_size || !handle_list->ret_total_size )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_get_handles: Failed handle_list buffer is NULL!!" );
      return CSD_EBADPARAM;
    }

    *( handle_list->ret_total_size ) = 0;
    *( handle_list->ret_write_size ) = 0;

      /*change the search key to by handle*/
    rc = csd_list_set_key2( &csd_vsm_session_used_list, &csd_vsm_list_find_session_by_handle );
    if ( rc == CSD_EOK )
    {
      rc = csd_list_find2( &csd_vsm_session_used_list, &node_ptr, &( handle_list->handle ) );
      if ( rc != CSD_EOK )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_get_handles: Failed to find handle [%d] in current sessions", handle_list->handle );
      }
      else
      {
        obj_ptr = node_ptr->data;
        csd_vsm_add_assoc_handles_to_list( handle_type, session_state, handle_list,
                                     obj_ptr->session_info ); 
      }
    }
    /* set key back */
    rc = csd_list_set_key2( &csd_vsm_session_used_list, &csd_vsm_list_find_session_by_name);     
  }
  /* return all handles*/
  else
  {
    uint32_t i;
    uint32_t count = csd_list_get_count(&csd_vsm_session_used_list);
    csd_vsm_handle_list_t *handle_list = (csd_vsm_handle_list_t*)param;
      
    if ( !handle_list->buffer || !handle_list->ret_write_size || !handle_list->ret_total_size )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_get_handles: Failed handle_list buffer is NULL!!" );
      return CSD_EBADPARAM;
    }

    *( handle_list->ret_total_size ) = 0;
    *( handle_list->ret_write_size ) = 0;

    for ( i=0; i < count; i++)
    {
      csd_list_get_head(&csd_vsm_session_used_list, &node_ptr);
      obj_ptr = node_ptr->data;
      csd_vsm_add_handles_to_list( handle_type, session_state, handle_list,
                                   obj_ptr->session_info );
      csd_list_add_tail(&csd_vsm_session_used_list, node_ptr);
    }
  }

  return rc;
}

uint32_t csd_vsm_update_state( uint32_t handle, 
                               const char* name, 
                               csd_vsm_handle_state_t new_state)
{
  uint32_t rc = CSD_EOK;
  struct csd_list_node* node_ptr = NULL;
  struct csd_vsm_session_obj *obj_ptr = NULL;
  uint32_t type = csd_handle_get_type(handle);

  csd_lock_enter( csd_vsm_update_lock );

  rc = csd_list_find2( &csd_vsm_session_used_list, &node_ptr, (void*)name );
  
  if (rc != CSD_EOK )
  {
    /* if lookup fails add session if it is an open command*/
    if ( new_state == CSD_VSM_HANDLE_STATE_OPEN )
    {
      rc = csd_list_get_head( &csd_vsm_session_free_list, &node_ptr );

      if( rc != CSD_EOK )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_update_state: Failed to get node from the free list, all [%d] free node has been used", VSM_SESSION_LIST_SIZE );
        return CSD_ENORESOURCE;
      }

      obj_ptr = node_ptr->data;
      strlcpy ( obj_ptr->session_info.name,
                name,
                sizeof( obj_ptr->session_info.name ) );

      csd_list_add_tail( &csd_vsm_session_used_list, node_ptr );
    }
    /*lookup failed error condition*/
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_update_state: Failed to get find session");
      return CSD_EFAILED;
    }
  }
  else
  {
    obj_ptr = node_ptr->data;
  }

  switch (type)
  {
    case CSD_OPEN_VOICE_CONTEXT:
      if ( new_state == CSD_VSM_HANDLE_STATE_UNINITIALIZED  )
      {
        obj_ptr->session_info.vc_handle.handle = CSD_VSM_NONE_HANDLE;
      }
      else
      {
        obj_ptr->session_info.vc_handle.handle = handle;
      }  
      obj_ptr->session_info.vc_handle.state = new_state;
      break;
    case CSD_OPEN_VOICE_STREAM:
      if ( new_state == CSD_VSM_HANDLE_STATE_UNINITIALIZED  )
      {
        obj_ptr->session_info.vs_handle.handle = CSD_VSM_NONE_HANDLE;
      }
      else
      {
        obj_ptr->session_info.vs_handle.handle = handle;
      }  
      obj_ptr->session_info.vs_handle.state = new_state;
      break;
    case CSD_OPEN_VOICE_MANAGER:
      if ( new_state == CSD_VSM_HANDLE_STATE_UNINITIALIZED  )
      {
        obj_ptr->session_info.vm_handle.handle = CSD_VSM_NONE_HANDLE;
      }
      else
      {
        obj_ptr->session_info.vm_handle.handle = handle;
      }  
      obj_ptr->session_info.vm_handle.state = new_state;
      break;
    default:
      CSD_MSG( CSD_PRIO_ERROR, "csd_vsm_update_state: Invalid handle type %d", type);
      rc = CSD_EFAILED;
      break;
  }

  /*update session state*/
  if ( obj_ptr->session_info.vs_handle.state == CSD_VSM_HANDLE_STATE_READY &&
       obj_ptr->session_info.vc_handle.state == CSD_VSM_HANDLE_STATE_READY &&
       obj_ptr->session_info.vm_handle.state == CSD_VSM_HANDLE_STATE_READY )
  {
    obj_ptr->session_info.state = CSD_VSM_SESSION_STATE_RUNNING;
  }
  else
  {
    obj_ptr->session_info.state = CSD_VSM_SESSION_STATE_IDLE;
  }

  /*if all handles are uninitalized delete session*/
  if ( obj_ptr->session_info.vs_handle.handle == CSD_VSM_NONE_HANDLE &&
       obj_ptr->session_info.vc_handle.handle == CSD_VSM_NONE_HANDLE &&
       obj_ptr->session_info.vm_handle.handle == CSD_VSM_NONE_HANDLE )
  {
    
    csd_list_del(&csd_vsm_session_used_list, node_ptr);
    csd_memset( &obj_ptr->session_info, 0, sizeof(csd_vsm_session_t) );
    csd_list_add_tail( &csd_vsm_session_free_list, node_ptr );
  }

  csd_lock_leave( csd_vsm_update_lock );

  return rc;
}



