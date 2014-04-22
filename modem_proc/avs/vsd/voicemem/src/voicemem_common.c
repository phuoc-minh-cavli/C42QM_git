/*
  Copyright (C) 2009-2019 QUALCOMM Technologies Inc.
  All rights reserved.
  Qualcomm Technologies Inc. Confidential and Proprietary.

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/voicemem/src/voicemem_common.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmstd.h"
#include "msg.h"
#include "err.h"
#if( defined WINSIM || defined AVS_MPSS_TEST )
  #include <stdlib.h>
  #include "qurt_elite.h"
#endif
#include "apr_errcodes.h"
#include "apr_memmgr.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
#include "smem.h"
#include "qurt.h"
#endif
#include "voicemem.h"
#include "drv_api.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
#define UNCOMMENT 0

#define VOICEMEM_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "VOICEMEM: Error[0x%08x], voicemem_init_ref_count=%d", \
                            rc, voicemem_init_ref_count, 0 ); } }

/* Size of the reserved SMEM_VOICE region. */
#define VOICEMEM_SMEM_HEAP_SIZE ( 81920 )

/* Flag to indicate SMEM to allocate cached memory. */
#define VOICEMEM_SMEM_ALLOC_FLAG_CACHED ( 0x80000000 )

/* Size of VOICEMEM internal book-keeping heap. */
#define VOICEMEM_INTERNAL_HEAP_SIZE_V ( 2048 )

#define VOICEMEM_HANDLE_TOTAL_BITS_V ( 16 )
#define VOICEMEM_HANDLE_INDEX_BITS_V ( 5 ) /* 5 bits = 32 handles. */
#define VOICEMEM_MAX_OBJECTS_V ( 1 << VOICEMEM_HANDLE_INDEX_BITS_V )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef struct voicemem_region_object_t
{
  uint32_t handle;
  void* base_virt_addr;
  uint32_t size;
  uint8_t client_name[31];
}
  voicemem_region_object_t;



/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/* Object manager lock. */
static apr_lock_t voicemem_objmgr_lock;

/* Object manager. */
static apr_objmgr_object_t voicemem_object_table[ VOICEMEM_MAX_OBJECTS_V ];
static apr_objmgr_t voicemem_objmgr;

/* VOICEMEM internal book-keeping heap manager. */
static uint8_t voicemem_internal_heap_pool[ VOICEMEM_INTERNAL_HEAP_SIZE_V ];
static apr_memmgr_type voicemem_internal_heapmgr;

/* SMEM_VOICE heap virtual address. */
static void* voicemem_heap_virt_addr = NULL;
static uint64_t voicemem_smem_heap_virt_addr = 0;
static uint64_t voicemem_smem_heap_phys_addr = 0;

/* VOICEMEM initialization reference count. */
static uint32_t voicemem_init_ref_count = 0;

static uint64_t mem_page[20];
static uint8_t mem_page_usage[20];

/* ADSP mailbox memory address. */
static uint64_t voicemem_mbox_pcie_iova = 0;
static uint64_t voicemem_mbox_adsp_iova = 0;

/****************************************************************************
 * HELPER FUNCTIONS                                                         *
 ****************************************************************************/

VSD_PRIVATE void voicemem_objmgr_lock_fn ( void )
{
  ( void ) apr_lock_enter( voicemem_objmgr_lock );
}


VSD_PRIVATE void voicemem_objmgr_unlock_fn ( void )
{
  ( void ) apr_lock_leave( voicemem_objmgr_lock );
}

VSD_PRIVATE uint32_t voicemem_create_region_object (
  voicemem_region_object_t** ret_region_obj
)
{
  uint32_t rc;
  uint32_t checkpoint = 0;
  voicemem_region_object_t* region_obj;
  apr_objmgr_object_t* objmgr_obj;

  for ( ;; )
  {
    if ( ret_region_obj == NULL )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    { /* Allocate memory for the new region object. */
      region_obj = apr_memmgr_malloc( &voicemem_internal_heapmgr,
                                      sizeof( voicemem_region_object_t ) );
      if ( region_obj == NULL )
      {
        rc = VSD_ENORESOURCE;
        break;
      }
      checkpoint = 1;
    }

    { /* Allocate a new handle for the region object. */
      rc = apr_objmgr_alloc_object( &voicemem_objmgr, &objmgr_obj );
      if ( rc ) break;
      checkpoint = 2;
    }

    /* Link the region object to the handle */
    objmgr_obj->any.ptr = region_obj;

    { /* Initialize the region object. */
      mmstd_memset( region_obj, 0, sizeof( voicemem_region_object_t ) );
      region_obj->handle = objmgr_obj->handle;
    }

    *ret_region_obj = region_obj;

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 2:
    ( void ) apr_objmgr_free_object( &voicemem_objmgr, objmgr_obj->handle );
    /*-fallthru */

  case 1:
    apr_memmgr_free( &voicemem_internal_heapmgr, region_obj );
    /*-fallthru */

  default:
    break;
  }

  return rc;
}


VSD_PRIVATE uint32_t voicemem_destroy_region_object (
  voicemem_region_object_t* region_obj
)
{
  if ( region_obj == NULL )
  {
    return VSD_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &voicemem_objmgr, region_obj->handle );
  apr_memmgr_free( &voicemem_internal_heapmgr, region_obj );

  return VSD_EOK;
}


VSD_PRIVATE uint32_t voicemem_get_region_object (
  uint32_t handle,
  voicemem_region_object_t** ret_region_obj
)
{
  uint32_t rc;
  apr_objmgr_object_t* objmgr_obj;
  voicemem_region_object_t* region_obj;

  for ( ;; )
  {
    if ( ret_region_obj == NULL )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    rc = apr_objmgr_find_object( &voicemem_objmgr, handle, &objmgr_obj );
    if ( rc ) break;

    region_obj = ( ( voicemem_region_object_t* ) objmgr_obj->any.ptr );
    if ( region_obj == NULL )
    {
      rc = VSD_EFAILED;
      break;
    }

    *ret_region_obj = region_obj;

    return VSD_EOK;
  }

  return rc;
}

/****************************************************************************
 * COMMAND PROCESSING FUNCTIONS                                             *
 ****************************************************************************/

VSD_PRIVATE uint32_t voicemem_cmd_init ( void )
{
  int32_t i = 0;

#ifdef AVS_BEAR_FAMILY
  int32_t rc = 0;
  smem_alloc_params_type smem_params;
#endif

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== voicemem_cmd_init() ======" );

  if ( voicemem_init_ref_count > 0 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "voicemem_cmd_init(): VOICEMEM already initialized: ref_count = %d",
           voicemem_init_ref_count );
    voicemem_init_ref_count++;
    return VSD_EOK;
  }

  { /* Initialize object manager lock. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &voicemem_objmgr_lock );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;

    params.table = voicemem_object_table;
    params.total_bits = VOICEMEM_HANDLE_TOTAL_BITS_V;
    params.index_bits = VOICEMEM_HANDLE_INDEX_BITS_V;
    params.lock_fn = voicemem_objmgr_lock_fn;
    params.unlock_fn = voicemem_objmgr_unlock_fn;
    ( void ) apr_objmgr_construct( &voicemem_objmgr, &params );
  }

  { /* Initialize VOICEMEM internal book-keeping heap manager. */
    apr_memmgr_init_heap( &voicemem_internal_heapmgr, (void*) &voicemem_internal_heap_pool,
                          sizeof( voicemem_internal_heap_pool ), NULL, NULL );
  }

  { /* Allocate SMEM voice heap and initialize the VOICEMEM SMEM heap manager. */
    uint32_t smem_heap_size;
    /* NOTE: The 80 KB SMEM_VOICE region is shared between voice and ECall. ECall
     * allocates the first 8 KB. SMEM does not do heap management and there is no
     * smem_free() (hence the need for this VOICEMEM driver) therefore when a client
     * allocates a certain size from one of the predefined SMEM regions, other
     * clients that subsequently allocate from the same SMEM region will get the
     * same address and size allocated by the first client. Therefore it is imperative
     * that VOICEMEM is initialized BEFORE ECall, so that we can get the full 80 KB.
     */

#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 

    { /* Initialize SMEM. */
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "voicemem_cmd_init(): initializing SMEM" );
      ( void ) smem_init( );
    }

#ifdef AVS_BEAR_FAMILY
    /* Allocate SMEM heap.
     *
     * NOTE : Remote host is the proc which is going to access the shared memory.
     * Host proc and remote proc cannot be the same, so SMEM_MODEM will not work.
     * Populating SMEM_APPS as the remote host to make the smem alloc successful.
     */
    mmstd_memset( (void*)(&smem_params), 0, sizeof( smem_alloc_params_type ) );
    smem_params.remote_host = SMEM_APPS;
    smem_params.smem_type = SMEM_VOICE;
    smem_params.size = VOICEMEM_SMEM_HEAP_SIZE;
    smem_params.buffer = NULL;
    smem_params.flags = VOICEMEM_SMEM_ALLOC_FLAG_CACHED;
    /* Allocating cached SMEM. */
    rc = smem_alloc_ex(&smem_params);
    if ( rc != SMEM_STATUS_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "voicemem_cmd_init(): Failed to allocate SMEM heap rc %d", rc );
      return VSD_EFAILED;
    }
    voicemem_smem_heap_virt_addr = (uint64_t) smem_params.buffer;
    voicemem_smem_heap_virt_addr = ( voicemem_smem_heap_virt_addr + 4096 - 1 ) &  ~( 4096 - 1 );
    voicemem_smem_heap_phys_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t) voicemem_smem_heap_virt_addr );

    /* NOTE: smem_alloc_ex() API updates the size param with the allocated size
     * if the current allocation is successful or the buffer size if it was previously allocated. */
    smem_heap_size = smem_params.size;

#else /* AVS_BEAR_FAMILY */

    /* Allocate SMEM heap. */
    voicemem_smem_heap_virt_addr = (uint64_t) smem_alloc( SMEM_VOICE, VOICEMEM_SMEM_HEAP_SIZE );
    if ( voicemem_smem_heap_virt_addr == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_init(): Failed to allocate SMEM_VOICE heap" );
      return VSD_EFAILED;
    }
    voicemem_smem_heap_virt_addr = ( voicemem_smem_heap_virt_addr + 4096 - 1 ) &  ~( 4096 - 1 );
    voicemem_smem_heap_phys_addr = ( uint64_t) qurt_lookup_physaddr( (qurt_addr_t) voicemem_smem_heap_virt_addr );

    ( void ) smem_get_addr( SMEM_VOICE, &smem_heap_size );

#endif /* AVS_BEAR_FAMILY */

#else /* !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) */

    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "voicemem_cmd_init(): initializing virtual memory" );
    voicemem_heap_virt_addr = malloc( VOICEMEM_SMEM_HEAP_SIZE );
    voicemem_smem_heap_virt_addr = (uint64_t) voicemem_heap_virt_addr;
    if ( voicemem_smem_heap_virt_addr == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_init(): Failed to allocate SMEM_VOICE heap" );
      return VSD_EFAILED;
    }

    voicemem_smem_heap_virt_addr = ( voicemem_smem_heap_virt_addr + 4096 - 1 ) &  ~( 4096 - 1 );
    voicemem_smem_heap_phys_addr = voicemem_smem_heap_virt_addr;
    smem_heap_size = VOICEMEM_SMEM_HEAP_SIZE;

#endif/* !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) */

    /* Check that the amount of SMEM_VOICE heap allocated matches what we asked for. */
    if ( smem_heap_size < VOICEMEM_SMEM_HEAP_SIZE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "voicemem_cmd_init(): Insufficient SMEM_VOICE heap allocated: requested %d, allocated %d",
             VOICEMEM_SMEM_HEAP_SIZE, smem_heap_size );
      /* smem_free() is not actually implemented. */
      //smem_free( voicemem_smem_heap_virt_addr );
      return VSD_EFAILED;
    }

    for( i = 0; i < 20; i++ )
    {
      mem_page_usage[i] = 0;
      mem_page[i] = ( uint32_t ) voicemem_smem_heap_phys_addr + ( i << 12 );
/*      MSG_5( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "voicemem_cmd_init(): mem_page[%d] = LSW[0x%08X]MSW[0x%08X] mem_page_usage[%d] = %d",
             i, (uint32_t)mem_page[i], (uint32_t)(mem_page[i] >> 32), i, mem_page_usage[i] ); 
*/
    }
    /* The first 8 KB are reserved for ECall, so mark them as used. */
    mem_page_usage[0] = 1; mem_page_usage[1] = 1;
  }

  voicemem_init_ref_count++;

  return VSD_EOK;
}


VSD_PRIVATE uint32_t voicemem_cmd_deinit ( void )
{
  if ( voicemem_init_ref_count == 0 )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "voicemem_cmd_deinit(): VOICEMEM not initialized." );
    return VSD_EOK;
  }

  voicemem_init_ref_count--;

  if ( voicemem_init_ref_count == 0 )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "voicemem_cmd_deinit(): de-initializing VOICEMEM." );

#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
    /* NOTE: smem_free is not implemented. */
    //smem_free( voicemem_smem_heap_virt_addr );
#else
    free( voicemem_heap_virt_addr );
#endif

    /* There is no smem_deinit. */
    { /* Destroy the object manager. */
      ( void ) apr_objmgr_destruct( &voicemem_objmgr );
    }

    { /* Destroy the object manager lock. */
      ( void ) apr_lock_destroy( voicemem_objmgr_lock );
    }
  }

  return VSD_EOK;
}


VSD_PRIVATE uint32_t voicemem_cmd_alloc_default (
  voicemem_cmd_alloc_default_t* params
)
{
  uint32_t rc;
  uint32_t i = 0;
  uint32_t num_blocks = 0;
  uint32_t start_index = 0;
  voicemem_region_object_t* region_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_cmd_alloc_default(): null parameter." );
      rc = VSD_EBADPARAM;
      break;
    }

    if ( ( params->size == 0 ) ||
         ( params->ret_voicemem_handle == NULL ) ||
         ( params->ret_virt_addr == NULL ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_cmd_alloc_default(): BAD parameter." );
      rc = VSD_EBADPARAM;
      break;
    }

    if ( mmstd_strnlen( ( char_t* ) params->client_name, sizeof( params->client_name ) ) == 0 )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_cmd_alloc_default(): no client name string provided" );
      rc = VSD_EBADPARAM;
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "voicemem_cmd_alloc_default(): size=%d, client_name=%.20s",
           params->size, params->client_name );

/*
    for( i = 0; i < 20 ; i++ )
    {
      MSG_5( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "voicemem_cmd_alloc_default(): START: mem_page[%d] = LSW[0x%08X]MSW[0x%08X] mem_page_usage[%d] = %d",
             i, (uint32_t)mem_page[i], (uint32_t)(mem_page[i] >> 32), i, mem_page_usage[i] ); 
    }
*/
    num_blocks = params->size / 4096;
    if( ( params->size % 4096 ) != 0 )
    {
       num_blocks++;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "voicemem_cmd_alloc_default(): num_blocks to allocate = %d", num_blocks );

    /* TODO: If there are multiple clients using VOICEMEM, this logic breaks in the following scenario:
     *   -> client 1 allocates 4 KB
     *   -> client 2 allocates 4 KB
     *   -> client 1 de-allocates 4 KB
     *   -> client 1 allocates 8 KB
     * Currently VOICEMEM only has one client (IMS) which allocates one region. At any point in time
     * when this is no longer true, this logic needs to be fixed.
     */
    for( i = 2 ; i < 20; i++ )
    {
      if ( mem_page_usage[i] == 0 )
      {
        start_index = i;
        break;
      }
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "voicemem_cmd_alloc_default(): start_index = %d", start_index );

    if ( ( 20 - start_index ) < num_blocks )
    {
       MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "voicemem_cmd_alloc_default(): falied: not enough memory left");
       rc = VSD_ENEEDMORE;
       break;
    }

    rc = voicemem_create_region_object( &region_obj );
    if ( NULL == region_obj )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "voicemem_cmd_alloc_default(): create region obj failed: rc = 0x%08X", rc );
      break;
    }

    region_obj->base_virt_addr = ( uint8_t* ) voicemem_smem_heap_virt_addr + ( start_index << 12 );
    region_obj->size = params->size;
    ( void ) mmstd_memcpy( region_obj->client_name, sizeof( params->client_name ),
                           params->client_name, sizeof( params->client_name ) );

    /* Mark mem pages as used. */
    while ( num_blocks && (start_index < 20) )
    {
      mem_page_usage[start_index] = 1;
      start_index++;
      num_blocks--;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "voicemem_cmd_alloc_default: handle[0x%08X] base_virt_addr[0x%08X] size[%d]",
           region_obj->handle, region_obj->base_virt_addr, region_obj->size );

    *( params->ret_voicemem_handle ) = region_obj->handle;
    *( params->ret_virt_addr ) = region_obj->base_virt_addr;

/*
    for( i = 0; i < 20 ; i++ )
    {
      MSG_5( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "voicemem_cmd_alloc_default(): END: mem_page[%d] = LSW[0x%08X]MSW[0x%08X] mem_page_usage[%d] = %d",
             i, (uint32_t)mem_page[i], (uint32_t)(mem_page[i] >> 32), i, mem_page_usage[i] ); 
    }
*/
    break;
  }

  return rc;
}


VSD_PRIVATE uint32_t voicemem_cmd_cache_invalidate (
  voicemem_cmd_cache_invalidate_t* params
)
{
  uint32_t rc = VSD_EOK;
#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
  int qurt_mem_rc;
#endif

  if ( params == NULL )
  {
    return VSD_EBADPARAM;
  }

#if UNCOMMENT
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "voicemem_cmd_cache_invalidate(): handle = 0x%08x, virt_addr[MSW] = 0x%08x, virt_addr[LSW] = 0x%08x, size = %d",
         params->voicemem_handle, (uint32_t)((uint64_t)(params->virt_addr) >> 32), (uint32_t)(params->virt_addr), params->size );
#endif

#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
  qurt_mem_rc = qurt_mem_cache_clean( (qurt_addr_t) params->virt_addr,
                  params->size, QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE );
  if ( qurt_mem_rc != QURT_EOK )
  {
    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_cmd_cache_invalidate(): failed for virt_addr[MSW] = 0x%08x, virt_addr[LSW] = 0x%08x,  size[%d] result[0x%08X]",
           (uint32_t)((uint64_t)(params->virt_addr) >> 32), (uint32_t)(params->virt_addr) , params->size, qurt_mem_rc );
    rc = VSD_EFAILED;
  }
#endif

  return rc;
}


VSD_PRIVATE uint32_t voicemem_cmd_cache_flush (
  voicemem_cmd_cache_flush_t* params
)
{
  uint32_t rc = VSD_EOK;
#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
  int qurt_mem_rc;
#endif

  if ( params == NULL )
  {
    return VSD_EBADPARAM;
  }

#if UNCOMMENT
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW, 
         "voicemem_cmd_cache_flush(): handle = 0x%08X, virt_addr[MSW] = 0x%08x, virt_addr[LSW] = 0x%08x, size = %d",
         params->voicemem_handle, (uint32_t)((uint64_t)(params->virt_addr) >> 32), (uint32_t)(params->virt_addr), params->size );
#endif

#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
  qurt_mem_rc = qurt_mem_cache_clean( (qurt_addr_t) params->virt_addr,
                  params->size, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE );
  if ( qurt_mem_rc != QURT_EOK )
  {
    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_cmd_cache_invalidate(): failed for virt_addr[MSW] = 0x%08x, virt_addr[LSW] = 0x%08x, size[%d] result[0x%08X]",
           (uint32_t)((uint64_t)(params->virt_addr) >> 32), (uint32_t)(params->virt_addr), params->size, qurt_mem_rc );
    rc = VSD_EFAILED;
  }
#endif

  return rc;
}


VSD_PRIVATE uint32_t voicemem_cmd_free (
  voicemem_cmd_free_t* params
)
{
  uint32_t rc;
  uint32_t j;
  uint32_t num_blocks;
  voicemem_region_object_t* region_obj;
#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
  qurt_addr_t virtual_address;
  qurt_addr_t phys_addr;
  uint32_t i;
#else
  uint8_t* virtual_address;
#endif
  

  if ( params == NULL )
  {
    rc = VSD_EBADPARAM;
    return rc;
  }

  /* Find the VOICEMEM region. */
  rc = voicemem_get_region_object( params->voicemem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_free(): Invalid voicemem_handle = 0x%08X",
           params->voicemem_handle );
    return rc;
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "voicemem_cmd_free(): region_obj: handle = 0x%08X, base_virt_addr = 0x%08x, size = %d",
                                         region_obj->handle, region_obj->base_virt_addr, region_obj->size );
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "voicemem_cmd_free(): client_name = %.20s",
         region_obj->client_name );

/*
  for( i = 0; i < 20 ; i++ )
  {
    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "voicemem_cmd_free(): at start: mem_page[%d] = 0x%0X16 mem_page_usage[%d] = %d",
           i, mem_page[i], i, mem_page_usage[i] );
  }
*/

  /* Deallocate the SMEM memory. */
  num_blocks = region_obj->size / 4096;

  if( ( region_obj->size % 4096 ) != 0 )
  {
    num_blocks++;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "voicemem_cmd_free(): num_blocks to free = %d", num_blocks );

#if !defined ( WINSIM ) && !defined ( AVS_MPSS_TEST ) 
  virtual_address = ( qurt_addr_t ) ( region_obj->base_virt_addr );

  for ( i = 0; i < num_blocks; i++ )
  {
    phys_addr = qurt_lookup_physaddr( virtual_address );
    if ( phys_addr == 0 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_free(): qurt_lookup_physaddr() failed for virt_addr = 0x%08x",
             ( qurt_addr_t ) virtual_address );
      rc = APR_EBADPARAM;
      return rc;
    }

    for ( j = 0; j < 20; j++ )
    {
      if ( mem_page[j] == ( uint64_t ) phys_addr )
      {
        mem_page_usage[j] = 0;
      }
    }
    virtual_address += 4096;
  }
#else
  virtual_address = ( uint8_t* ) ( region_obj->base_virt_addr );

  for ( j = 0; j < 20; j++ )
  {
    if ( mem_page[j] == ( uint64_t ) virtual_address )
    {
      mem_page_usage[j] = 0;
      break;
    }
  }
  virtual_address += 4096;
#endif
/*
  for ( i = 0; i < 20 ; i++ )
  {
    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW, "voicemem_cmd_free(): at end: mem_page[%d] = 0x%0X16 mem_page_usage[%d] = %d",
           i, mem_page[i], i, mem_page_usage[i] );
  }
*/

  /* Destroy the region object. */
  ( void ) voicemem_destroy_region_object( region_obj );

  return VSD_EOK;
}

VSD_PRIVATE uint32_t voicemem_cmd_set_mailbox_memory_config (
  voicemem_cmd_set_mailbox_memory_config_t* params
)
{
 
  if ( ( params->mailbox_mem_address_pcie == NULL ) ||
        ( params->mailbox_mem_address_adsp == NULL ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_set_mailbox_memory_config(): Invalid IOVA" );
    return VSD_EFAILED;
  }
  /* Check that the amount of SMEM_VOICE heap allocated matches what we asked for. */
  if ( params->mem_size < VOICEMEM_SMEM_HEAP_SIZE )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_cmd_set_mailbox_memory_config(): Insufficient ADSP mailbox memory allocated: requested %d, allocated %d",
              VOICEMEM_SMEM_HEAP_SIZE, params->mem_size );
    return VSD_EFAILED;
  }
  voicemem_mbox_pcie_iova = params->mailbox_mem_address_pcie;
  voicemem_mbox_adsp_iova = params->mailbox_mem_address_adsp;
  
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "voicemem_cmd_set_mailbox_memory_config(): before aligning: pcie_iova[MSW] = 0x%08x, pcie_iova[LSW] = 0x%08x, "
                                         "adsp_iova[MSW] = 0x%08x, adsp_iova[LSW] = 0x%08x",
                                         (uint32_t)(voicemem_mbox_pcie_iova >> 32), (uint32_t)(voicemem_mbox_pcie_iova), 
                                         (uint32_t)(voicemem_mbox_adsp_iova >> 32), (uint32_t)(voicemem_mbox_adsp_iova));
  voicemem_mbox_adsp_iova = ( voicemem_mbox_adsp_iova + 4096 - 1 ) &  ~( 4096 - 1 );
  voicemem_mbox_pcie_iova = ( voicemem_mbox_pcie_iova + 4096 - 1 ) &  ~( 4096 - 1 );
  
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "voicemem_cmd_set_mailbox_memory_config(): after aligning: pcie_iova[MSW] = 0x%08x, pcie_iova[LSW] = 0x%08x, "
                                         "adsp_iova[MSW] = 0x%08x, adsp_iova[LSW] = 0x%08x",
                                         (uint32_t)(voicemem_mbox_pcie_iova >> 32), (uint32_t)(voicemem_mbox_pcie_iova), 
                                         (uint32_t)(voicemem_mbox_adsp_iova >> 32), (uint32_t)(voicemem_mbox_adsp_iova));
  return VSD_EOK;
}

 uint64_t voicemem_cmd_get_iova (
   voicemem_cmd_get_iova_t* params
)
{
  uint32_t rc = VSD_EOK;
  voicemem_region_object_t* region_obj;

  for ( ;; )
  {
    rc = voicemem_get_region_object( params->mem_handle, &region_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_get_iova(): Invalid voicemem_handle = 0x%08X",
            params->mem_handle );
      rc = VSD_EHANDLE;
      break;
    }
    if ( ( params->smem_addr - voicemem_smem_heap_phys_addr ) <= VOICEMEM_SMEM_HEAP_SIZE )
    {
      if( NULL != params->adsp_iova )
      {
        *(params->adsp_iova) = ( params->smem_addr -  voicemem_smem_heap_phys_addr ) + voicemem_mbox_adsp_iova;
      }
      if( NULL !=  params->pcie_iova )
      {
        *(params->pcie_iova) = ( params->smem_addr -  voicemem_smem_heap_phys_addr ) + voicemem_mbox_pcie_iova;
      }
      rc = VSD_EOK;
    }
    else
    {
      rc = VSD_EBADPARAM;
    }
    break;
  }
  VOICEMEM_PANIC_ON_ERROR( rc );
  return rc;
}

VSD_PUBLIC vsd_status_t voicemem_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  if ( ( ( size > 0 ) && ( params == NULL ) ) ||
       ( ( size == 0 ) && ( params != NULL ) ) )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_call(): cmd_id = 0x%08X params = 0x%08X size = %d is invalid",
           cmd_id, params, size );
    return VSD_EBADPARAM;
  }

  if ( ( voicemem_init_ref_count == 0 ) && ( cmd_id != VOICEMEM_CMD_INIT ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicemem_call(): cmd_id = 0x%08X called before VOICEMEM has been initialized", cmd_id );
    return VSD_ENOTREADY;
  }

  switch ( cmd_id )
  {
    case DRV_CMDID_INIT:
      rc = voicemem_cmd_init();
      break;

    case DRV_CMDID_DEINIT:
      rc = voicemem_cmd_deinit();
      break;

    case VOICEMEM_CMD_ALLOC_DEFAULT:
      rc = voicemem_cmd_alloc_default( ( voicemem_cmd_alloc_default_t* ) params );
      break;

    case VOICEMEM_CMD_CACHE_INVALIDATE:
      rc = voicemem_cmd_cache_invalidate( ( voicemem_cmd_cache_invalidate_t* ) params );
      break;

    case VOICEMEM_CMD_CACHE_FLUSH:
      rc = voicemem_cmd_cache_flush( ( voicemem_cmd_cache_flush_t* ) params );
      break;
    
    case VOICEMEM_CMD_SET_MAILBOX_MEMORY_CONFIG:
      rc = voicemem_cmd_set_mailbox_memory_config( ( voicemem_cmd_set_mailbox_memory_config_t* ) params );
      break;

    case VOICEMEM_CMD_GET_IOVA:
      rc = voicemem_cmd_get_iova( ( voicemem_cmd_get_iova_t* ) params );
      break;
    
    case VOICEMEM_CMD_FREE:
      rc = voicemem_cmd_free( ( voicemem_cmd_free_t* ) params );
      break;

    default:
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Unsupported cmd_id (0x%08X)", cmd_id );
      rc = VSD_EUNSUPPORTED;
      break;
  }

  return rc;
}

