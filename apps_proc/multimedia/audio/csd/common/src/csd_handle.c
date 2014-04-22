/*! \file csd_handle.c
    \brief CSD module handle management

    Copyright (C) 2010 - 2012, 2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/src/csd_handle.c#2 $
$DateTime: 2019/01/03 07:23:13 $
$Author: pwbldsvc $
$Change: 17964421 $
$Revision: #2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/16   rk      Abstracting OSAL layer
09/01/14   rp      enhanced the csd init debug buffer
04/23/10   zj      initial release

============================================================================*/
#include "mmdefs.h"
#include "csd.h"
#include "DALSys.h"
#include "csd_list.h"
#include "csd_os_dependencies.h"
#include "csd_debug_msg.h"
#include "csd_debug_info.h"
#include "csd_as_ioctl.h"


/*! \brief The handle obj will store type and client obj ponit 
      It is indexed by index bits. The handle will contains
      both the index and a random number. The index is 10 LSB
      bits. The random number is 20 MSB bits. The valid bits is
      1. And the reseved bit is one 
*/
#define CSD_HANDLE_OBJ_FREE     0
#define CSD_HANDLE_OBJ_BUSY     1

/*
New Handle Bits Design
No of Bits      Bit Index           Information
    10           [0-9]              INDEX
    1            [10]               VALID_MASK
    1            [11]               Buf_Type (PMEM|Heap) for AS/VS
    4            [12-15]            Open_Code (Handle Type AS/AC/VS/VC/VM]
    12           [16-27]            COUNT BITS (Random Number)
    4            [28-31]            RESERVED 
*/
/* NUMBER OF BITS IN THE 32BIT HANDLE */
#define CSD_HANDLE_INDEX_BITS      10    //[0-9]
#define CSD_HANDLE_VALID_BIT        1     //[10]
//                                          //[11] <--> Reserve
#define CSD_HANDLE_TYPE_BITS        4     //[12-15]
#define CSD_HANDLE_COUNT_BITS      12    //[16-27]
#define CSD_HANDLE_RESERVED_BITS    4     //[28-31]

/* START BITINDEX for EACH HANDLES */
#define CSD_HANDLE_INDEXBT_INDEX    0
#define CSD_HANDLE_VALIDBIT_INDEX   10
#define CSD_HANDLE_TYPEBIT_INDEX    12
#define CSD_HANDLE_COUNTBIT_INDEX   16

/* MASKS FOR EACH FIELDS */
#define CSD_HANDLE_INDEX_BITS_MASK  0x000003FF    // ((1<<CSD_HANDLE_INDEX_BITS)-1)
#define CSD_HANDLE_VALID_MASK       0x00000400    // (1<<(CSD_HANDLE_VALIDBIT_INDEX -1))
#define CSD_HANDLE_TYPE_MASK        0x0000F000    // (1<<(CSD_HANDLE_TYPEBIT_INDEX -1))
#define CSD_COUNTBITS_MASK          0x0FFF0000

/* SHIFT_BITS_COUNT */
#define CSD_HANDLE_INDEXBIT_INDEX          0
#define CSD_HANDLE_VALIDBIT_SHIFT_INDEX    10
#define CSD_HANDLE_TYPEBITS_SHIFT_INDEX    12
#define CSD_HANDLE_COUNTBITS_SHIFT_INDEX   16


#define CSD_HANDLE_VALID_BIT_INDEX      (CSD_HANDLE_INDEX_BITS+1)
#define CSD_HANDLE_VALID_BIT_MASK       (1<<(CSD_HANDLE_VALID_BIT_INDEX-1))

#define CSD_HANDLE_BUFTYPE_BIT_INDEX    (CSD_HANDLE_VALID_BIT_INDEX + 1)
#define CSD_HANDLE_BUFTYPE_BIT_MASK     (1<<(CSD_HANDLE_BUFTYPE_BIT_INDEX-1))
#define CSD_HANDLE_OPENCODE_BIT_INDEX   (CSD_HANDLE_BUFTYPE_BIT_INDEX + 1)

static uint32_t	count = 0;

struct csd_handle_obj
{
  struct csd_list_node  node;
  uint32_t              index;
  uint32_t              handle;
  uint32_t	            type;
  void*                 client_obj;
  bool_t                is_ssr;
};

static struct csd_handle_obj *csd_handle_array = NULL;

static struct csd_list  csd_handle_free_list;
static struct csd_list  csd_handle_used_list;
static uint32_t	        csd_handle_size = 0;
static bool_t           is_type_ssr[CSD_OPEN_MAX_NUM];

bool_t csd_list_find_type( struct csd_list_node *node, uint32_t key )
{
  bool_t rc = FALSE;
  struct csd_handle_obj *handle_obj = (struct csd_handle_obj *)node->data;
  if( NULL != handle_obj && handle_obj->type == key )
  {
    rc = TRUE;
  }
  return rc;
}

bool_t csd_list_find_client_obj( struct csd_list_node *node, void *pKeyData )
{
  bool_t rc = FALSE;
  struct csd_handle_obj *handle_obj = (struct csd_handle_obj *)node->data;
  if( NULL != handle_obj && handle_obj->client_obj == pKeyData )
  {
    rc = TRUE;
  }
  return rc;
}

/*! \fn   csd_handle_init
  \brief  This function should only be called once. It initializes
          the handle object arrary
*/
int32_t csd_handle_init(uint32_t num)
{
  int32_t rc;
  uint32_t i;
  if( (1 << CSD_HANDLE_INDEX_BITS) < num )
  {
    return CSD_EBADPARAM;
  }
  
  csd_malloc( num * sizeof(*csd_handle_array), (void **)&csd_handle_array );
  if( csd_handle_array == NULL )
  {
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,CSD_ENORESOURCE,CSD_HANLE_INIT_STAGE,
                      // CSD_INIT_INFO_HANDLE_MALLOC,0,0); 
    return CSD_ENORESOURCE;
  }
  
  csd_memset( csd_handle_array, 0, sizeof(*csd_handle_array)*num );
  /*Handle handle malloc success*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,CSD_EOK,CSD_HANLE_INIT_STAGE,
                      //CSD_INIT_INFO_HANDLE_MALLOC,0,0); 

  rc = csd_list_init( &csd_handle_free_list );
  rc |= csd_list_init_key( &csd_handle_used_list, &csd_list_find_type );
  rc |= csd_list_set_key2( &csd_handle_used_list, &csd_list_find_client_obj );

  if( rc != CSD_EOK )
  {
    csd_list_dinit( &csd_handle_used_list );
    csd_list_dinit( &csd_handle_free_list );
    csd_free( csd_handle_array );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_HANLE_INIT_STAGE,
                       //CSD_INIT_INFO_HANDLE_INIT,0,0); 
    return CSD_EFAILED;
  }
   
  /* CSD handle list init success */
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_HANLE_INIT_STAGE,
                     //CSD_INIT_INFO_HANDLE_INIT,0,0);
  csd_handle_size = num;
  for( i = 0; i < num; i++ )
  {
    csd_handle_array[i].index = i;
    csd_handle_array[i].node.data = &(csd_handle_array[i]);
    csd_list_add_tail( &csd_handle_free_list, &(csd_handle_array[i].node) );
  }
  return CSD_EOK;
}


int32_t csd_handle_dinit(void)
{
  struct csd_list_node *node = NULL;
  uint32_t nHandleCount      = csd_list_get_count( &csd_handle_free_list );

  if( nHandleCount != csd_handle_size )
  {
    /* we have handle leakage */
    CSD_MSG( CSD_PRIO_ERROR, "csd_handle_dinit: Free List Count=[%d] != TotalCount[%d]", nHandleCount, csd_handle_size );
    /* No point in returning Error: Go Ahead and De-Init anyway */
  }
  /* First Free the nodes for those in the free List */
  do
  {
    csd_list_get_head( &csd_handle_free_list, &node );
    if( node )
    {
      csd_list_del( &csd_handle_free_list, node );
    }
  }
  while( node );
  csd_list_dinit( &csd_handle_free_list );
  
  do
  {
    csd_list_get_head( &csd_handle_used_list, &node );
    if( node )
    {
      csd_list_del( &csd_handle_used_list, node );
    }
  }
  while (node);
  csd_list_dinit( &csd_handle_used_list );
  /* csd handle deinit success */
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_HANLE_INIT_STAGE,
                      //CSD_INIT_INFO_HANDLE_INIT,0,0); 

  if( csd_handle_array != NULL )
  {
    csd_free( csd_handle_array );
    csd_handle_array = NULL;
  }
  /* csd handle free success */
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_HANLE_INIT_STAGE,
                     //CSD_INIT_INFO_HANDLE_MALLOC,0,0); 
  return CSD_EOK;
}


uint32_t csd_handle_malloc(uint32_t type, void* obj)
{
  struct csd_list_node* node = NULL;
  struct csd_handle_obj* handle_obj = NULL;
  if( CSD_EOK != csd_list_get_head( &csd_handle_free_list, &node ) )
  {
    return 0;
  }
  if( CSD_EOK != csd_list_add_tail( &csd_handle_used_list, node ) )
  {
    return 0;
  }
  handle_obj = node->data;
  csd_criticalsection_enter();
  count = (count+1) & ((1<<CSD_HANDLE_COUNT_BITS)-1);
  handle_obj->handle = count << CSD_HANDLE_COUNTBITS_SHIFT_INDEX;
  csd_criticalsection_leave();
  handle_obj->type = type;
  handle_obj->client_obj = obj;
  handle_obj->handle |= handle_obj->index;
  handle_obj->handle |= CSD_HANDLE_VALID_MASK;
  handle_obj->handle |= (type << CSD_HANDLE_TYPEBITS_SHIFT_INDEX);
  handle_obj->is_ssr = FALSE;
  
  return handle_obj->handle;
}

uint32_t csd_handle_get_type(uint32_t handle)
{
  uint32_t index = handle & ((1<<CSD_HANDLE_INDEX_BITS)-1);
  struct csd_handle_obj *handle_obj = NULL;
  if (NULL == csd_handle_array)
  {
    return 0;
  }
  if (index >= csd_handle_size)
  {
    return 0;
  }
  handle_obj = &(csd_handle_array[index]);
  if (NULL != handle_obj && handle == handle_obj->handle)
  {
    return handle_obj->type;
  }
  return 0;
}

void* csd_handle_get_obj(uint32_t handle)
{
  uint32_t index = handle & ((1<<CSD_HANDLE_INDEX_BITS)-1);
  struct csd_handle_obj *handle_obj = NULL;
  if (NULL == csd_handle_array)
  {
    return NULL;
  }
  if (index >= csd_handle_size)
  {
    return NULL;
  }
  handle_obj = &(csd_handle_array[index]);
  if (NULL != handle_obj && handle == handle_obj->handle)
  {
    return handle_obj->client_obj;
  }
  return NULL;
}

void csd_handle_free(uint32_t handle)
{
  uint32_t index = handle & ((1<<CSD_HANDLE_INDEX_BITS)-1);
  struct csd_handle_obj *handle_obj = NULL;
  if (NULL == csd_handle_array)
  {
    return;
  }
  if (index >= csd_handle_size)
  {
    return;
  }
  handle_obj = &(csd_handle_array[index]);
  handle_obj->handle &= ~CSD_HANDLE_VALID_MASK;
  handle_obj->type = 0;
  handle_obj->client_obj = NULL;
  handle_obj->is_ssr = FALSE;
  
  csd_list_del(&csd_handle_used_list, &(handle_obj->node));
  csd_list_add_tail(&csd_handle_free_list, &(handle_obj->node));
  return;
}

bool_t csd_handle_is_ssr(uint32_t handle)
{
  uint32_t index = handle & ((1 << CSD_HANDLE_INDEX_BITS) - 1);
  struct csd_handle_obj *handle_obj = NULL;
  bool_t rc = FALSE;
  if (NULL == csd_handle_array)
  {
    return rc;
  }
  if (index < csd_handle_size)
  {
    handle_obj = &(csd_handle_array[index]);
    if ((handle_obj != NULL) && (handle == handle_obj->handle))
    {
      rc =  handle_obj->is_ssr;
    }
  }
  return rc;
}

bool_t csd_ioctl_allow_status_in_ssr(uint32_t cmd)
{
  bool_t rc = FALSE;  
  switch(cmd)
  {
  case CSD_AS_CMD_FREE_PMEM:
  case CSD_AS_CMD_FREE_PMEM_V2:
    rc = TRUE;
    break;
  default:
    rc = FALSE;
    break;
  }
  return rc;
}
   
bool_t csd_handle_is_type_ssr(enum csd_open_code code)
{
  bool_t rc = FALSE;
  if (code < CSD_OPEN_MAX_NUM)
  {
    rc = is_type_ssr[code];
  }
  return rc;
}

int32_t csd_handle_set_type_ssr(enum csd_open_code code, bool_t is_ssr)
{
  struct csd_list_node *node = NULL;
  struct csd_handle_obj *handle_obj = NULL;
  int32_t rc = CSD_EBADPARAM;

  if (CSD_OPEN_MAX_NUM > code)
  {
    if (TRUE == is_ssr)
    {
      //node = csd_handle_used_list.list_head;
      do
      {
        if (CSD_EOK == csd_list_find_next(&csd_handle_used_list, &node, code) && NULL != node)
        {
          handle_obj = node->data;
          handle_obj->is_ssr = TRUE;
          //node = node->next;
        }
        else
        {
          break;
        }
      }
      while (NULL != node);
    }
    is_type_ssr[code] = is_ssr;
    rc = CSD_EOK;
  }
    
  return rc;
}

int32_t csd_handle_set_ssr(uint32_t handle, bool_t bssr)
{
  uint32_t index = handle & ((1 << CSD_HANDLE_INDEX_BITS) - 1);
  struct csd_handle_obj *handle_obj = NULL;
  int32_t rc = CSD_EFAILED;
  if (NULL == csd_handle_array)
  {
    return rc;
  }
  if (index < csd_handle_size)
  {
    handle_obj = &(csd_handle_array[index]);
    if (handle_obj != NULL)
    {
      handle_obj->is_ssr = bssr;
      rc = CSD_EOK;
    }
  }
  return rc;
}

bool_t csd_handle_is_obj_ssr(void *obj)
{
  struct csd_list_node *node = NULL;
  struct csd_handle_obj *handle_obj = NULL;
    bool_t rc = FALSE;
  if (CSD_EOK == csd_list_find2(&csd_handle_used_list, &node, obj) && NULL != node)
  {
    handle_obj = node->data;
    rc = handle_obj->is_ssr;
  }
    return rc;
}

/* TODO: We need to synchronize the access between the SSR notification manager callback
 *       and the public APIs accessed by the client(s). Some notes and questions:
 *       1) Public API calls to csd_open(), csd_close(), csd_read(), csd_write(), csd_ioctl() 
 *          need to be blocked when the SSR callback event occurs and is in the process of 
 *          updating local "flags" that indicate which sub-systems are undergoing SSR. 
 *       2) The SSR callback event handler need to be blocked until any exiting calls to
 *          csd_open(), csd_close(), csd_read(), csd_write(), csd_ioctl() are completed.
 *       3) We could use some form of a counter/semaphore to synchronize the access between
 *          (1) and (2) where the csd APIs check for the counter/semaphore of the SSR
 *          handler, and vice versa.
 *       4) If a sub-system reset occurs while the apps is still in the middle of executing 
 *          one or many csd APIs, the SSR event handling will be blocked until the API completes.
 *          At the same time, any calls made to QDSP inside the csd driver will timeout and 
 *          result in error code that's returned back to the client. Only upon the next public  
 *          API call will the SSR handling be completed and the client be notfied of the correct
*           SSR return code.
 *       5) If SSR occurs during csd_close(), could the driver return prematurely due to a
 *          timeout error from calling the QDSP and not complete all clean-ups? Should we allow
 *          the SSR notification handler to not be blocked by csd_clse()? But even so, there is 
 *          no guarantee that a call to QDSP could be blocked in time and we may still end up 
 *          getting a timeout result from calling QDSP.
 *       6) We need to allow certain CSD IOCTL calls in SSR for the proper clean up.There are 
 *          certain IOCTLS which do need the ADSP to be in Good status. Such IOCTLS can be allowed
 *          to call.
 */
