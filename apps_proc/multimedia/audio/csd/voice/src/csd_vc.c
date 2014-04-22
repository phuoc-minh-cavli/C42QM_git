/*-----------------------------------------------------------------------------
   Copyright (C) 2013, 2016 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/voice/src/csd_vc.c#4 $
  $DateTime: 2019/03/14 20:28:08 $
  $Author: pwbldsvc $
  $Change: 18597212 $
  $Revision: #4 $
  =========================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
       when        who       what, where, why
      --------    -----      ---------------------------------------------------
      11/15/16     rk        Abstracting OSAL layer
	  05/16/16	   rk        send VP3 command from vc-apr-cb to avoid a race condition
							 causes reponse sent twice for device_config
	  01/06/15     rk        Moved tx dtmf detection event QMI CB processing to voc_comman_task 
      08/25/14     jk        Added per vocoder calibration support  
      09/09/14     rp        Fixed vocproc mode to External EC mix for  valid EC referance device 
      09/01/14     rp        enhanced the csd init debug buffer 
      05/30/14     rp        Added topology feature id 
      04/16/14     jk        added support for Voice Session Manager
      02/06/14     vk        modified code for proper csd/acdb/apr error mapping
      08/08/13     jy        Allow voice call when no calibration data in ACDB
      07/03/13     vk        Porting changes from mainline related to CR499559 and removal of compiler warnings
      06/05/13     aim       Added support for unified CSD msg logging.
      06/03/13     unni      Removed csd_vc_close propagating failure due to
                             csd_voc_commmon_remove_device_info
      05/31/13     rp        Added valid device check during the csd_vc_close
      04/23/13     unni      dynamic afe port manager support
      04/16/13     ts        added CSD_VC_IOCTL_SET_VOLUME_STEP API
      03/22/13     jy        fix imvalid device pair resulting in set device config crash
      02/27/12     jk        fixed mem dereg issues in vc_close   
  =========================================================================*/

#include "csd_vc_private.h"
#include "csd_debug_info.h"
#include "csd_os_dependencies.h"
#include "csd_dev_common.h"
#include "csd_error_map.h"
#include "csd_vsm.h"

static uint32_t   csd_vc_apr_handle;        
static char_t     csd_vc_cvp_dest_dns[] = "qcom.audio.cvp";
static char_t     csd_vc_mvm_dest_dns[] = "qcom.audio.mvm";
static char_t     csd_vc_dns[] = "qcom.apps.csd_vc";
static uint16_t   csd_vc_src_addr;
static uint16_t   csd_vc_cvp_dest_addr;
static uint16_t   csd_vc_mvm_dest_addr;      /* for mvm look up */
static csd_lock_t csd_vc_task_lock;            /* Lock management */
static apr_list_t csd_vc_free_cmd_list;
static apr_list_t csd_vc_active_cmd_list;
static csd_vc_cmd_list_item_t csd_vc_cmd_pool[ CSD_VC_NUM_CMDS ];

static apr_list_t csd_vc_free_sess_list;
static apr_list_t csd_vc_active_sess_list;
static apr_list_t csd_vc_track_dispatched_cmd_list; /* this keeps cmd sent for open + close that are awaiting a response */
static csd_vc_session_item_t csd_vc_sess_pool[ CSD_VC_NUM_SESSIONS ];

static apr_objmgr_object_t csd_vc_object_table[ CSD_VC_MAX_OBJECTS_V ];
static apr_objmgr_t csd_vc_apr_objmgr;

static uint32_t token_counter = 0;
static struct csd_vtable csd_vc_vtable;
static uint32_t h_csd_voc_common;  /* Handle to csd voice common */

/*
 ** End Internal Data Structures
*/

/*
 ** Start Internal functions
*/
static void csd_vc_task_lock_fn ( void )
{
  ( void ) csd_lock_enter( csd_vc_task_lock );
}

static void csd_vc_task_unlock_fn ( void ) 
{
  ( void ) csd_lock_leave( csd_vc_task_lock );
}

static int32_t csd_vc_get_device_ids( void* params )
{
  csd_vc_session_item_t* psession = NULL;
  csd_vc_ioctl_get_device_ids_t *get_device_id = ( csd_vc_ioctl_get_device_ids_t * ) params;
  
  if ( !get_device_id || !get_device_id->csd_vc_handle || 
       !get_device_id->ret_rx_dev_id || !get_device_id->ret_tx_dev_id )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_vc_get_device_ids: Bad param passed to CSD");
    return CSD_EBADPARAM;
  }  
  
  /* extract psession */
  psession = csd_voc_common_obtain_session_from_handle ( get_device_id->csd_vc_handle, &csd_vc_apr_objmgr );
  if ( !psession )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_get_device_ids: Unable to obtain session from vc handle[0x%x]",  get_device_id->csd_vc_handle );
    return CSD_EFAILED;
  }

  *get_device_id->ret_rx_dev_id = psession->rx_dev_id;
  *get_device_id->ret_tx_dev_id = psession->tx_dev_id;

  CSD_MSG(CSD_PRIO_LOW,"csd_vc_get_device_ids: rx_dev_id[0x%x] tx_dev_id[0x%x]",
          psession->rx_dev_id, psession->tx_dev_id );
  
  return CSD_EOK;
}

static int32_t csd_vc_clean_cal_resources( csd_vc_session_item_t  *psess )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;

  if( !psess )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_clean_cal_resources: Input arg[*psess] is NULL" );
    return CSD_EBADPARAM;
  }
  
  switch( psess->sess_cal.state )
  {
    case CSD_VC_ENUM_CAL_REGISTERED:
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_clean_cal_resources: In state[CSD_VC_ENUM_CAL_REGISTERED]" );
      if( psess->sess_cal.static_table.if_registered )
      {
        uint32_t cmd = VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA;
        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vc_src_addr, psess->h_apr_obj,
                                     csd_vc_cvp_dest_addr, psess->h_cvp,
                                     0, cmd,
                                     APR_NULL_V, 0 );                                 
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_clean_cal_resources: VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA sent, apr_rc[0x%x]", apr_rc );
      }
      if( psess->sess_cal.dynamic_table.if_registered )
      {
        uint32_t cmd = VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA;
        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vc_src_addr, psess->h_apr_obj,
                                     csd_vc_cvp_dest_addr, psess->h_cvp,
                                     0, cmd,
                                     APR_NULL_V, 0 );                                 
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_clean_cal_resources: VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA sent, apr_rc[0x%x]", apr_rc );
      }
      if( psess->sess_cal.devcfg_table.if_registered )
      {
        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vc_src_addr, psess->h_apr_obj,
                                     csd_vc_cvp_dest_addr, psess->h_cvp,
                                     0, VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG,
                                     APR_NULL_V, 0 );                                 
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_clean_cal_resources: VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG sent, apr_rc[0x%x]", apr_rc );
      }
    break;
    
    case CSD_VC_ENUM_CAL_MEM_MAPPED:
    {
      vss_imemory_cmd_unmap_t cvp_cmd;
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_clean_cal_resources: In state[CSD_VC_ENUM_CAL_MEM_MAPPED]" );
      
      cvp_cmd.mem_handle = psess->sess_cal.descriptor_mapped_handle;      
      psess->memmap_client = CSD_VC_ENUM_CLIENT_CAL;
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_mvm_dest_addr, APR_NULL_V,
                                   0, VSS_IMEMORY_CMD_UNMAP,
                                   &cvp_cmd, sizeof( vss_imemory_cmd_unmap_t ) );
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_clean_cal_resources: VSS_IMEMORY_CMD_UNMAP sent, apr_rc[0x%x], mem_handle[0x%x]",
               apr_rc, cvp_cmd.mem_handle );
    }
    break;
        
    case CSD_VC_ENUM_CAL_MEM_ALLOCATED:
    {
      vss_imemory_table_t* pMemoryTable = NULL;
      vss_imemory_table_t* pNextMemoryTable = NULL;
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_clean_cal_resources: In state[CSD_VC_ENUM_CAL_MEM_ALLOCATED]" );
      /* Free cal data buffer */
      csd_mem_free( psess->sess_cal.data_memory_handle );
      /* Free cal table buffer which contains addr of cal data buffer */
      pMemoryTable = (vss_imemory_table_t*)psess->sess_cal.pva_descriptor_table;
      do
      {
        if( pMemoryTable->next_table_descriptor.mem_address )
        {
          vss_imemory_table_t** ppNextMemoryTable = 
            (vss_imemory_table_t**)( pMemoryTable + CSD_MEM_PAGE_SIZE - sizeof( vss_imemory_block_t ) );

          pNextMemoryTable = ( vss_imemory_table_t* )*ppNextMemoryTable;
        }
        else
        {
          pNextMemoryTable = NULL;
        }
        
        csd_mem_free( psess->sess_cal.descriptor_memory_handle );
        pMemoryTable = pNextMemoryTable;
      } while ( pNextMemoryTable );

      memset( &psess->sess_cal, 0, sizeof(csd_vc_session_cal) );
      psess->sess_cal.state = CSD_VC_ENUM_CAL_UNINITIALIZED;
    }
    /* Fall through */
    case CSD_VC_ENUM_CAL_UNINITIALIZED:
    {
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_clean_cal_resources: In state[CSD_VC_ENUM_CAL_UNINITIALIZED]" );
      if( psess->bclosing_sequence )
      {
        csd_event_signal ( psess->csd_vc_event );
      }
      else
      {
        csd_vc_cmd_list_item_t* pcmd = NULL;
        ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
        if ( NULL != pcmd )
        {
          pcmd->type = CSD_VC_CMD_SEND_CALIBRATION;
          pcmd->psess = psess;
          pcmd->client_token = NULL;
          ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
        }      
      }
    }   
    
    default:
    break;
  }
  rc = csd_map_apr_error_to_csd ( apr_rc );
  return rc;
}

static int32_t csd_vc_send_cal_fn( csd_vc_session_item_t  *psess )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint32_t tmp_size = 0;
  csd_voc_common_get_oemdb_t cal_arg;
  uint32_t nContigLen  = 0;

  if( !psess )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_cal_fn: Input arg[*psess] is NULL" );
    return CSD_EBADPARAM;
  }
  
  cal_arg.common.rx_device_id = psess->rx_dev_id;
  cal_arg.common.rx_samp_rate = psess->rx_dev_sr;
  cal_arg.common.tx_device_id = psess->tx_dev_id;
  cal_arg.common.tx_samp_rate = psess->tx_dev_sr;
  
  cal_arg.volume.rx_device_id = psess->rx_dev_id;
  cal_arg.volume.tx_device_id = psess->tx_dev_id;
  cal_arg.volume.feature_id = psess->sess_fid.volume_feature_id;
  
  cal_arg.dev_cfg.rx_device_id = psess->rx_dev_id;
  cal_arg.dev_cfg.tx_device_id = psess->tx_dev_id;
  
  switch( psess->sess_cal.state )
  {
    case CSD_VC_ENUM_CAL_UNINITIALIZED:
    {
      uint32_t nTraverseSize = 0;
      uint8_t* pTraverse = NULL;
      int64_t pPhysical = NULL;
      uint8_t* pVAMemoryTableTraverse = NULL;
      vss_imemory_table_t* vss_mem_tbl = NULL;
      vss_imemory_block_t* vss_mem_block = NULL;
#if DESCRIPTOR_TABLE_LARGER_THAN_4K        
      uint8_t* pNewVAMemoryTable = NULL;
#endif
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_send_cal_fn: In state[CSD_VC_ENUM_CAL_UNINITIALIZED]" );
      /* Check ACDB for common cal data size */
      tmp_size = csd_voc_common_get_calibration_size( CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_SIZE, &cal_arg );  
      if( tmp_size != 0 )
      {
        psess->sess_cal.static_table.size = ( tmp_size/CSD_MEM_PAGE_SIZE ) * CSD_MEM_PAGE_SIZE + CSD_MEM_PAGE_SIZE;
      }  
      /* Check ACDB for volume cal data size */
      tmp_size = csd_voc_common_get_calibration_size( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_SIZE, &cal_arg );
      if( tmp_size != 0 )
      {
        psess->sess_cal.dynamic_table.size = ( tmp_size/CSD_MEM_PAGE_SIZE ) * CSD_MEM_PAGE_SIZE + CSD_MEM_PAGE_SIZE;
      }        
      /* Check ACDB for devcfg cal data size */
      tmp_size = csd_voc_common_get_calibration_size( CSD_VOC_COMMON_GET_OEMDB_DEV_CFG_SIZE, &cal_arg );
      if( tmp_size != 0 )
      {
        psess->sess_cal.devcfg_table.size = ( tmp_size/CSD_MEM_PAGE_SIZE ) * CSD_MEM_PAGE_SIZE + CSD_MEM_PAGE_SIZE;
      }    
  
      tmp_size = psess->sess_cal.static_table.size + psess->sess_cal.dynamic_table.size + psess->sess_cal.devcfg_table.size;  
      if( !tmp_size )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_cal_fn: Cal data not available in ACDB, skipping" );
        rc = CSD_EFAILED;
        break;
      }
      
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_send_cal_fn: Allocating memory with size[0x%x]", tmp_size );
      csd_mem_alloc( tmp_size, &psess->sess_cal.data_memory_handle, &psess->sess_cal.static_table.p_vir_addr );
      if( !psess->sess_cal.static_table.p_vir_addr )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_cal_fn: Failed to allocate memory with size[0x%x]", tmp_size );
        rc = CSD_EFAILED;
        break;
      }
      psess->sess_cal.dynamic_table.p_vir_addr = psess->sess_cal.static_table.p_vir_addr + psess->sess_cal.static_table.size;
      psess->sess_cal.devcfg_table.p_vir_addr = psess->sess_cal.dynamic_table.p_vir_addr + psess->sess_cal.dynamic_table.size;
      psess->sess_cal.state = CSD_VC_ENUM_CAL_MEM_ALLOCATED;
/* Memory Table Buffer Layout constructed according to specs by vss_imemory_public_if.h with the 
   virtual address append at the end. 

   The memory table may require more than 4K to describe non-contiguous VC calibration buffer.
   
   The table descriptor only holds the physical address of the next memory table. CSD will reserve 
   the last 12 bytes of the memory table to populate the virtual address of the next memory table 
   to avoid translating the address during free.

    <------------------- 12 bytes ------------------------>
     ------------------------------------------------------
    | vss_imemory_table_descriptor_t next_table_descriptor
     ------------------------------------------------------
    | vss_imemory_block_t block_0
     ------------------------------------------------------
    | vss_imemory_block_t block_1                         |
     ------------------------------------------------------
    | ...                                                 |
     ------------------------------------------------------
    | vss_imemory_block_t block_339                       |
     ------------------------------------------------------
    | vss_imemory_table_t* pVANextMemoryTable             |
     ------------------------------------------------------
    <----------8 bytes----------> ( Assuming 64 bit addressing )
*/      
      /* allocate first memory table here */
      csd_mem_alloc( CSD_MEM_PAGE_SIZE, &psess->sess_cal.descriptor_memory_handle, &psess->sess_cal.pva_descriptor_table );
      if( !psess->sess_cal.pva_descriptor_table )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_cal_fn: Failed to allocate memory with size[0x%x]", CSD_MEM_PAGE_SIZE );
        rc = CSD_EFAILED;
        break;
      }      
      
      memset( ( void* )psess->sess_cal.pva_descriptor_table, 0, CSD_MEM_PAGE_SIZE );
      pVAMemoryTableTraverse = psess->sess_cal.pva_descriptor_table;
      vss_mem_tbl = (vss_imemory_table_t*)pVAMemoryTableTraverse;            
      /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
      pVAMemoryTableTraverse += sizeof(vss_imemory_table_descriptor_t);
      /* set the next table description to 0 at the moment */
      vss_mem_tbl->next_table_descriptor.mem_address = 0;
      vss_mem_tbl->next_table_descriptor.mem_size = 0;        
      /* determine the amount of memory blocks there are in the non-contiguous memory */
      nTraverseSize = tmp_size;
      pTraverse = psess->sess_cal.static_table.p_vir_addr;            
    
      while( nTraverseSize > 0 )
      {
        csd_get_mem_info( psess->sess_cal.data_memory_handle, (uint64_t*)&pPhysical, &nContigLen );
                    
        if( psess->sess_cal.num_of_memory_block == 0 )
        {
          psess->sess_cal.static_table.phy_addr = pPhysical;
        }

        nTraverseSize = nTraverseSize - nContigLen; /* update the leftover size to be traversed */
        pTraverse += nContigLen; /* update pointer to the virual address of the next memory block */
        vss_mem_block = (vss_imemory_block_t*)pVAMemoryTableTraverse;
            
        /* Populate the memory memory block */
        vss_mem_block->mem_address = pPhysical;
        vss_mem_block->mem_size = nContigLen;
        CSD_MSG( CSD_PRIO_LOW, "csd_vc_send_cal_fn: [%dth]Mem block filled in descriptor table, phy addr[0x%llx], size[0x%x]",
                 psess->sess_cal.num_of_memory_block, vss_mem_block->mem_address, vss_mem_block->mem_size );

        pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof(vss_imemory_block_t); /* advance to the next memory block */            
        psess->sess_cal.num_of_memory_block++;
#if DESCRIPTOR_TABLE_LARGER_THAN_4K            
        if( psess->sess_cal.num_of_memory_block >= ( CSD_PMEM_PAGE_SIZE - sizeof(vss_imemory_table_descriptor_t) - sizeof(vss_imemory_table_t*)/sizeof(vss_imemory_block_t) ) )
        {
          /* allocate the new memory table */
          pNewVAMemoryTable = (uint8_t*)CSD_PMEM_ALLOC_EXT( CSD_PMEM_PAGE_SIZE, PMEM_TYPE_ID, PMEM_NON_CONTIG, PMEM_ALIGNMENT_4K );
          /* copy the VA of the next memory table at the end of this current one */
          ( void ) csd_memscpy ( pVAMemoryTableTraverse, sizeof ( vss_imemory_table_t * ), &pNewVAMemoryTable, sizeof ( vss_imemory_table_t * ) );
          /* get the Physical Address for the Memory Table */
          CSD_MEM_OFFSET( pNewVAMemoryTable, CSD_PMEM_PAGE_SIZE, &vss_mem_tbl->next_table_descriptor.mem_address, &nContigLen );
          vss_mem_tbl->next_table_descriptor.mem_size = nContigLen;
          pVAMemoryTableTraverse = pNewVAMemoryTable;
          vss_mem_tbl = ( vss_imemory_table_t* ) pVAMemoryTableTraverse;
          /* set the next table description to 0 at the moment */
          vss_mem_tbl->next_table_descriptor.mem_address = 0;
          vss_mem_tbl->next_table_descriptor.mem_size = 0;
          /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
          pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_table_descriptor_t );
        }
#endif
      } /* end while( nTraverseSize > 0 ) */
    }
    /* Fall through */
    case CSD_VC_ENUM_CAL_MEM_ALLOCATED:
    {
      vss_imemory_cmd_map_physical_t cvp_cmd;
      
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_send_cal_fn: In state[CSD_VC_ENUM_CAL_MEM_ALLOCATED]" );
      /* get the Physical Address for the descriptor Table */
      csd_get_mem_info( psess->sess_cal.descriptor_memory_handle,
                        (uint64_t*)&cvp_cmd.table_descriptor.mem_address, &nContigLen );

      cvp_cmd.table_descriptor.mem_size = psess->sess_cal.num_of_memory_block * sizeof(vss_imemory_block_t) + sizeof(vss_imemory_table_descriptor_t);
      cvp_cmd.is_cached         = TRUE;
      cvp_cmd.cache_line_size   = CSD_VOC_COMMON_CACHE_LINE_SIZE;
      cvp_cmd.access_mask       = 1;
      cvp_cmd.page_align        = CSD_VOC_COMMON_PAGE_ALIGN;
      cvp_cmd.min_data_width    = CSD_VOC_COMMON_MIN_DATA_WIDTH;
      cvp_cmd.max_data_width    = CSD_VOC_COMMON_MAX_DATA_WIDTH;

      psess->memmap_client = CSD_VC_ENUM_CLIENT_CAL;
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_mvm_dest_addr, APR_NULL_V,
                                   psess->set_dev_token, VSS_IMEMORY_CMD_MAP_PHYSICAL,
                                   &cvp_cmd, sizeof(vss_imemory_cmd_map_physical_t) );
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_send_cal_fn: VSS_IMEMORY_CMD_MAP_PHYSICAL sent, apr_rc[0x%x], mem_addr[0x%llx], mem_size[0x%x]",
               apr_rc, cvp_cmd.table_descriptor.mem_address, cvp_cmd.table_descriptor.mem_size );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;
      
    case CSD_VC_ENUM_CAL_MEM_MAPPED:
    {
      int32_t static_table_rc = CSD_EFAILED;
      int32_t dynamic_table_rc = CSD_EFAILED;
      int32_t devcfg_table_rc = CSD_EFAILED;  
      aprv2_packet_t* papr_packet = NULL;
      uint32_t static_table_column_size = 0;
      uint32_t dynamic_table_column_size = 0;
      uint32_t static_table_size = 0;
      uint32_t dynamic_table_size = 0;
      uint32_t devcfg_table_size = 0;      
      uint32_t cal_cmd_static;
      uint32_t cal_cmd_dynamic;

      cal_cmd_static = VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA;
      cal_cmd_dynamic = VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA;
 
      CSD_MSG( CSD_PRIO_LOW, "csd_vc_send_cal_fn: In state[CSD_VC_ENUM_CAL_MEM_MAPPED]" );
      /* Get cal dta from ACDB */
      if( psess->sess_cal.static_table.size )
      {
        static_table_rc = csd_voc_common_get_calibration( CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC, &cal_arg,
                                                          (uint8_t *)psess->sess_cal.static_table.p_vir_addr,
                                                          psess->sess_cal.static_table.size, &static_table_size );                                                                  
        static_table_column_size = csd_voc_common_get_cal_column_info_size( CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO_SIZE,
                                                                            psess->rx_dev_id, psess->tx_dev_id );                                                              
      }
      if( psess->sess_cal.dynamic_table.size )
      {
        dynamic_table_rc = csd_voc_common_get_calibration( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC, &cal_arg,
                                                          (uint8_t *)psess->sess_cal.dynamic_table.p_vir_addr,
                                                          psess->sess_cal.dynamic_table.size, &dynamic_table_size );
        dynamic_table_column_size = csd_voc_common_get_cal_column_info_size( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO_SIZE, 
                                                                             psess->rx_dev_id, psess->tx_dev_id );
      }
      if( psess->sess_cal.devcfg_table.size )
      {
        devcfg_table_rc = csd_voc_common_get_calibration( CSD_VOC_COMMON_GET_OEMDB_DEV_CFG, &cal_arg,
                                                          (uint8_t *)psess->sess_cal.devcfg_table.p_vir_addr,
                                                          psess->sess_cal.devcfg_table.size, &devcfg_table_size );
      }
                  
      if( static_table_rc == CSD_EOK )
      { /* Register Common Calibration */
        vss_ivocproc_cmd_register_static_calibration_data_t cvp_reg_common_cmd;        
        cvp_reg_common_cmd.cal_mem_handle  = psess->sess_cal.descriptor_mapped_handle;
        cvp_reg_common_cmd.cal_mem_size    = static_table_size;

        if( psess->sess_cal.num_of_memory_block > 1 )
        { /* if the memory is non-contiguous CVD expects an offset */
          cvp_reg_common_cmd.cal_mem_address = 0; /* zero offset since common calibration is stored at the start of the buffer */
        }
        else
        { /* if the memory is contiguous CVD expects a physical address */
          cvp_reg_common_cmd.cal_mem_address = ( uint64_t )psess->sess_cal.static_table.phy_addr;
        }        
            
        apr_rc = __aprv2_cmd_alloc_ext( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                    csd_vc_src_addr, psess->h_apr_obj,
                                    csd_vc_cvp_dest_addr, psess->h_cvp,
                                    psess->set_dev_token, cal_cmd_static,
                                    sizeof(vss_ivocproc_cmd_register_static_calibration_data_t) + static_table_column_size,
                                    &papr_packet );

        if( APR_EOK == apr_rc && papr_packet != NULL )
        { /* copy the param_data into apr buffer */
          uint8_t* pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
          uint32_t psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( papr_packet->header );
          uint32_t column_write_size = 0;          
          csd_memscpy( pdata, psize,
                      &cvp_reg_common_cmd,
                      sizeof( vss_ivocproc_cmd_register_static_calibration_data_t ) );
          pdata += sizeof( vss_ivocproc_cmd_register_static_calibration_data_t ) - sizeof(uint32_t);

          csd_voc_common_get_cal_column_info( CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO,
                                              pdata, static_table_column_size + sizeof(uint32_t),
                                              &column_write_size, psess->rx_dev_id,psess->tx_dev_id  );

          apr_rc = __aprv2_cmd_forward( csd_vc_apr_handle, papr_packet );
          CSD_MSG( CSD_PRIO_HIGH, "csd_vc_send_cal_fn: VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA sent, apr_rc[0x%x]", apr_rc );
        }
        else  
        {
          CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_cal_fn: __aprv2_cmd_alloc_ext failed, apr_rc[0x%x]", apr_rc );
        }
      }
      
      if( dynamic_table_rc == CSD_EOK )
      { /* Register Volume Calibration */      
        vss_ivocproc_cmd_register_dynamic_calibration_data_t cvp_reg_dynamic_cmd;        
        cvp_reg_dynamic_cmd.cal_mem_handle  = psess->sess_cal.descriptor_mapped_handle;        
        cvp_reg_dynamic_cmd.cal_mem_size    = dynamic_table_size;

        if( psess->sess_cal.num_of_memory_block > 1 )
        { /* if the memory is non-contiguous CVD expects an offset */
          cvp_reg_dynamic_cmd.cal_mem_address = (uint64_t)( psess->sess_cal.dynamic_table.p_vir_addr - psess->sess_cal.static_table.p_vir_addr );
        }
        else
        { /* if the memory is contiguous CVD expects a physical address */
          cvp_reg_dynamic_cmd.cal_mem_address = (uint64_t)( 
            psess->sess_cal.static_table.phy_addr + ( psess->sess_cal.dynamic_table.p_vir_addr - psess->sess_cal.static_table.p_vir_addr ) );
        }

        apr_rc = __aprv2_cmd_alloc_ext( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                    csd_vc_src_addr, psess->h_apr_obj,
                                    csd_vc_cvp_dest_addr, psess->h_cvp,
                                    psess->set_dev_token, cal_cmd_dynamic,
                                    sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t) + dynamic_table_column_size,
                                    &papr_packet );

        if( APR_EOK == apr_rc && papr_packet != NULL )
        { /* copy the param_data into apr buffer */
          uint8_t* pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
          uint32_t psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( papr_packet->header );
          uint32_t column_write_size = 0;
          csd_memscpy( pdata, psize,
                      &cvp_reg_dynamic_cmd,
                      sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t) );
          pdata += sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t) - sizeof(uint32_t);

          csd_voc_common_get_cal_column_info( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO, pdata, 
                                              dynamic_table_column_size + sizeof(uint32_t), &column_write_size, psess->rx_dev_id, psess->tx_dev_id );

          apr_rc = __aprv2_cmd_forward( csd_vc_apr_handle, papr_packet );
          CSD_MSG( CSD_PRIO_HIGH, "csd_vc_send_cal_fn: VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA sent, apr_rc[0x%x]", apr_rc );
        }
        else  
        {
          CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_cal_fn: __aprv2_cmd_alloc_ext failed, apr_rc[0x%x]", apr_rc );
        }
      }
      
      if( devcfg_table_rc == CSD_EOK )
      { /* Register Device Configuration */      
        vss_ivocproc_cmd_register_device_config_t cvp_reg_devcfg_cmd;
        cvp_reg_devcfg_cmd.mem_handle  = psess->sess_cal.descriptor_mapped_handle;
        cvp_reg_devcfg_cmd.mem_size    = devcfg_table_size;

        if ( psess->sess_cal.num_of_memory_block > 1 )
        { /* if the memory is non-contiguous CVD expects an offset */
          cvp_reg_devcfg_cmd.mem_address = (uint64_t)( psess->sess_cal.devcfg_table.p_vir_addr - psess->sess_cal.static_table.p_vir_addr );
        }
        else
        { /* if the memory is contiguous CVD expects a physical address */
          cvp_reg_devcfg_cmd.mem_address = (uint64_t)( 
            psess->sess_cal.static_table.phy_addr + ( psess->sess_cal.devcfg_table.p_vir_addr - psess->sess_cal.static_table.p_vir_addr ) );
        }

        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vc_src_addr, psess->h_apr_obj,
                                     csd_vc_cvp_dest_addr, psess->h_cvp,
                                     psess->set_dev_token, VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG,
                                     &cvp_reg_devcfg_cmd, sizeof(vss_ivocproc_cmd_register_device_config_t) );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_send_cal_fn: VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG sent, apr_rc[0x%x]", apr_rc );
      }
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;
    
    default:
    break;
  }
    
  if( ( rc != CSD_EOK ) && ( psess->sess_cal.state < CSD_VC_ENUM_CAL_MEM_MAPPED ))
  {
    csd_vc_cmd_list_item_t* pcmd = NULL;
    /* obtain a free command item */
    ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
    if( pcmd ) 
    {
      pcmd->type = CSD_VC_CMD_SEND_VP3;
      pcmd->psess = psess;
      pcmd->client_token = psess->set_dev_token;
      ( void )apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
    }
  }
  
  return rc;
}

static int32_t csd_vc_send_vp3_fn( csd_vc_session_item_t *psess )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint32_t size = 0;
  csd_voc_common_process_oemdb_vp3_t vp3_cmd;
  csd_vc_cmd_list_item_t* pcmd = NULL; 

  vp3_cmd.tx_device_id = psess->tx_dev_id;
  vp3_cmd.rx_device_id = psess->rx_dev_id;
  
  switch( psess->sess_vp3.state )
  {
    case CSD_VC_ENUM_VP3_MEM_MAPPED: 
      rc = csd_voc_common_process_vp3( CSD_VOC_COMMON_GET_OEMDB_VP3_INFO, vp3_cmd, NULL, NULL, &size );
      if( rc != CSD_EOK )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_vp3_fn: Unable to get VP3 info, rc[0x%x], skipping VP3 processing", rc );    
        break;
      }
    /* Fall through */
    case CSD_VC_ENUM_VP3_INFO_RECEIVED:
      rc = csd_voc_common_process_vp3( CSD_VOC_COMMON_GET_OEMDB_VP3_DATA, vp3_cmd,
                                       (uint8_t *)psess->sess_vp3.pva_data, VP3_MAXIMUM_BUFFER_SIZE, &size );
      if( rc != CSD_EOK )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_vp3_fn: Unable to get VP3 data, rc[0x%x], skipping VP3 processing", rc );
        break;
      }
    /* Fall through */
    case CSD_VC_ENUM_VP3_DATA_RECEIVED:
    {
      vss_ivp3_cmd_set_data_t cmd;
       
      cmd.mem_handle = psess->sess_vp3.descriptor_mapped_handle;
      cmd.mem_address = psess->sess_vp3.phy_addr_data;
      cmd.mem_size = size;

      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vc_src_addr, psess->h_apr_obj,
                                       csd_vc_cvp_dest_addr, psess->h_cvp,
                                       0, VSS_IVP3_CMD_SET_DATA,
                                       &cmd, sizeof(vss_ivp3_cmd_set_data_t) );
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_send_vp3_fn: VSS_IVP3_CMD_SET_DATA sent, apr_rc[0x%x], mem_handle[0x%x], mem_addr[0x%llx], mem_size[%d]", 
               apr_rc, cmd.mem_handle, cmd.mem_address, cmd.mem_size );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;
    
    default:
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_send_vp3_fn: Unable to proceed due to VP3 state[%d], skipping VP3 processing", psess->sess_vp3.state );
      rc = CSD_EFAILED;
    break;
  }
     
  /* obtain a free command item */
  ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
  if( pcmd ) 
  {
      pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
      pcmd->psess = psess;
      pcmd->cmd.rsp_cmd.status_id = CSD_EOK; /* Client should get success response even vp3/cal failed */
      pcmd->client_token = psess->set_dev_token;
      ( void )apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
    }
  
  return rc;
}

static int32_t csd_vc_receive_vp3_fn( csd_vc_session_item_t *psess )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  
  switch( psess->sess_vp3.state )
  {
    case CSD_VC_ENUM_VP3_MEM_MAPPED:    
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vc_src_addr, psess->h_apr_obj,
                                       csd_vc_cvp_dest_addr, psess->h_cvp,
                                       0, VSS_IVP3_CMD_GET_SIZE,
                                       NULL, 0 );
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_receive_vp3_fn: VSS_IVP3_CMD_GET_SIZE sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd ( apr_rc );
    break;
      
    case CSD_VC_ENUM_VP3_INFO_RECEIVED:
    {
      vss_ivp3_cmd_get_data_t cmd;
      
      if( !psess->sess_vp3.size_data || psess->sess_vp3.size_data > VP3_MAXIMUM_BUFFER_SIZE )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_receive_vp3_fn: Unable to proceed due to received VP3 data size[%d], skipping", psess->sess_vp3.size_data );
        rc = CSD_EFAILED;
        psess->sess_vp3.state = CSD_VC_ENUM_VP3_MEM_MAPPED;
        break;
      }
      
      cmd.mem_handle = psess->sess_vp3.descriptor_mapped_handle;
      cmd.mem_address = psess->sess_vp3.phy_addr_data;
      cmd.mem_size = psess->sess_vp3.size_data;
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vc_src_addr, psess->h_apr_obj,
                                       csd_vc_cvp_dest_addr, psess->h_cvp,
                                       0, VSS_IVP3_CMD_GET_DATA,
                                       &cmd, sizeof(vss_ivp3_cmd_get_data_t) );
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_receive_vp3_fn: VSS_IVP3_CMD_GET_DATA sent, apr_rc[0x%x], mem_handle[0x%x], mem_addr[0x%llx], mem_size[%d]", 
               apr_rc, cmd.mem_handle, cmd.mem_address, cmd.mem_size );
      rc = csd_map_apr_error_to_csd ( apr_rc );
    }
    break;
    
    case CSD_VC_ENUM_VP3_DATA_RECEIVED:
    {
      csd_voc_common_process_oemdb_vp3_t vp3_cmd;

      vp3_cmd.tx_device_id = psess->tx_dev_id;
      vp3_cmd.rx_device_id = psess->rx_dev_id;
      /* Let the client thread continue execution */
      csd_event_signal( psess->csd_vc_event );
      
      csd_voc_common_process_vp3( CSD_VOC_COMMON_SET_OEMDB_VP3_DATA, vp3_cmd,
                                  (uint8 *)psess->sess_vp3.pva_data, psess->sess_vp3.size_data, NULL );
      psess->sess_vp3.state = CSD_VC_ENUM_VP3_MEM_MAPPED;
    }
    break;
      
    default: 
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_receive_vp3_fn: Not able to proceed due to VP3 state[%d], skipping", psess->sess_vp3.state );
      rc = CSD_EFAILED;
    break;
  }
  
  if( rc != CSD_EOK )
  {
    /* Let the client thread continue execution */
    csd_event_signal( psess->csd_vc_event );
  }
  
  return rc;
}

static int32_t csd_vc_set_cal_fid_fn( csd_vc_session_item_t *psess )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  
  switch( psess->sess_fid.set_feature_id_state )
  {
    case CSD_VC_ENUM_FID_SET_INITIALIZE:    
    {
      /* Disable VC to begin re-registration of calibration */
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vc_src_addr, psess->h_apr_obj,
                                       csd_vc_cvp_dest_addr, psess->h_cvp,
                                       0, VSS_IVOCPROC_CMD_DISABLE,
                                       NULL, 0 );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_set_cal_fid_fn: VSS_IVOCPROC_CMD_DISABLE sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd ( apr_rc );
    }
    break;

    case CSD_VC_ENUM_FID_VC_DISABLED:
    {
      if( psess->sess_cal.dynamic_table.if_registered)
      {
        uint32_t cmd = VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA;

        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                         csd_vc_src_addr, psess->h_apr_obj,
                                         csd_vc_cvp_dest_addr, psess->h_cvp,
                                         0, cmd,
                                         APR_NULL_V, 0 );                                 
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_set_cal_fid_fn: VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA sent, apr_rc[0x%x]", apr_rc );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
    }
    break;

    case CSD_VC_ENUM_FID_VOL_DEREGISTERED:
    {
      uint32_t dynamic_table_column_size = 0;
      aprv2_packet_t* papr_packet = NULL;
      vss_ivocproc_cmd_register_dynamic_calibration_data_t cvp_reg_dynamic_cmd;
      uint32_t cmd = VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA;

      if( psess->sess_cal.dynamic_table.size )
      {
        dynamic_table_column_size = csd_voc_common_get_cal_column_info_size( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO_SIZE, 
                                                                             psess->rx_dev_id, psess->tx_dev_id );
      }

      /* Register Volume Calibration */      
      cvp_reg_dynamic_cmd.cal_mem_handle  = psess->sess_cal.descriptor_mapped_handle;        
      cvp_reg_dynamic_cmd.cal_mem_size    = psess->sess_fid.volume_cal_size_used;

      if( psess->sess_cal.num_of_memory_block > 1 )
      { /* if the memory is non-contiguous CVD expects an offset */
        cvp_reg_dynamic_cmd.cal_mem_address = (uint64_t)( psess->sess_cal.dynamic_table.p_vir_addr - psess->sess_cal.static_table.p_vir_addr );
      }
      else
      { /* if the memory is contiguous CVD expects a physical address */
        cvp_reg_dynamic_cmd.cal_mem_address = (uint64_t)( 
          psess->sess_cal.static_table.phy_addr + psess->sess_cal.dynamic_table.p_vir_addr - psess->sess_cal.static_table.p_vir_addr );
      }

      apr_rc = __aprv2_cmd_alloc_ext( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                      csd_vc_src_addr, psess->h_apr_obj,
                                      csd_vc_cvp_dest_addr, psess->h_cvp,
                                      psess->set_dev_token, cmd,
                                      sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t) + dynamic_table_column_size,
                                      &papr_packet );

      if( APR_EOK == apr_rc && papr_packet != NULL )
      { /* copy the param_data into apr buffer */
        uint32_t column_write_size = 0;
        uint8_t* pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
        uint32_t psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(papr_packet->header);
        csd_memscpy( pdata, psize, &cvp_reg_dynamic_cmd, sizeof(vss_ivocproc_cmd_register_volume_calibration_data_t) );
        pdata += sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t) - sizeof(uint32_t);

        csd_voc_common_get_cal_column_info( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO, pdata, 
                                            dynamic_table_column_size + sizeof(uint32_t), &column_write_size, 
                                            psess->rx_dev_id, psess->tx_dev_id );

        apr_rc = __aprv2_cmd_forward( csd_vc_apr_handle, papr_packet );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_set_cal_fid_fn: VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA sent, apr_rc[0x%x]", rc );
      }
      else  
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_set_cal_fid_fn: __aprv2_cmd_alloc_ext failed, apr_rc[0x%x]", apr_rc );
      }
      rc = csd_map_apr_error_to_csd ( apr_rc );
    }
    break;

    case CSD_VC_ENUM_FID_VOL_REGISTERED:
    {
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                    csd_vc_src_addr, psess->h_apr_obj,
                                    csd_vc_cvp_dest_addr, psess->h_cvp,
                                    0, VSS_IVOCPROC_CMD_ENABLE,
                                    NULL, 0 );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_set_cal_fid_fn: VSS_IVOCPROC_CMD_ENABLE sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd ( apr_rc );
    }
    break;

    default: 
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_set_cal_fid_fn: Unknown set_feature_id_state[%d], skipping", psess->sess_fid.set_feature_id_state );
      rc = CSD_EFAILED;
    break;
  }

  return rc;
}

uint32_t csd_vc_update_state (csd_vc_session_item_t  *psess, uint32_t cmd)
{
  uint32_t rc = CSD_EOK;
  switch( psess->sess_state.state )
  {
    case CSD_VSM_HANDLE_STATE_UNINITIALIZED:
      /* if session is opened transition*/
      if( cmd == VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          rc = csd_vsm_update_state( psess->h_csd, psess->vc_name, psess->sess_state.state );
      }
      break;
    case CSD_VSM_HANDLE_STATE_OPEN:
       /* if session is opened transition*/
      if( cmd == APRV2_IBASIC_CMD_DESTROY_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_UNINITIALIZED;
         rc = csd_vsm_update_state( psess->h_csd, psess->vc_name, psess->sess_state.state );
      }
      else
      {
        if( psess->sess_state.vc_configured && psess->sess_state.vc_enabled )
        {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_READY;
         rc =  csd_vsm_update_state( psess->h_csd, psess->vc_name, psess->sess_state.state );
        }
      }
      break;
    case CSD_VSM_HANDLE_STATE_READY:
       /* if session is opened transition*/
      if( cmd == APRV2_IBASIC_CMD_DESTROY_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_UNINITIALIZED;
          rc = csd_vsm_update_state( psess->h_csd, psess->vc_name, psess->sess_state.state );
      }
      else
      {
        if( !psess->sess_state.vc_configured || !psess->sess_state.vc_enabled )
        {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          rc = csd_vsm_update_state( psess->h_csd, psess->vc_name, psess->sess_state.state );
        }
      }
      break;
    default:
       CSD_MSG( CSD_PRIO_ERROR, "csd_vc_update_state: Unknown state[%d]", psess->sess_state.state );
       rc = CSD_EBADSTATE;
  }
  return rc;
}

bool_t csd_vc_session_exsists(const char* session_name)
{
  bool_t rc = FALSE;
  csd_vc_session_item_t *link;
  csd_vc_session_item_t *pivot;
   
  /* check if the vs is belonged to this vm */
  if ( !( apr_list_peak_head ( &csd_vc_active_sess_list, ( apr_list_node_t ** )&link) ) )
  {    
    do 
    {
      if ( strncmp( session_name, link->vc_name, link->vc_name_size )== 0 )
      {
        rc = TRUE;
        break;
      }
      pivot = link;
    } while ( ! ( apr_list_get_next ( &csd_vc_active_sess_list,
                                      ( apr_list_node_t* )pivot, ( apr_list_node_t** )&link ) ));
  }
  return rc;
}

uint32_t csd_vc_command_fn( uint32_t param )
{
  int32_t rc = CSD_EOK;
  csd_vc_cmd_list_item_t* pcmd = NULL;

  while( !( apr_list_remove_head( &csd_vc_active_cmd_list, ( apr_list_node_t** )&pcmd ) ) )
  {
    if( pcmd )
    {
      switch( pcmd->type )
      {
        case CSD_VC_CMD_SEND_CALIBRATION:
          rc = csd_vc_send_cal_fn( pcmd->psess );
        break;

        case CSD_VC_CMD_SEND_VP3:
          rc = csd_vc_send_vp3_fn( pcmd->psess );
        break;
        
        case CSD_VC_CMD_RECEIVE_VP3:
          rc = csd_vc_receive_vp3_fn( pcmd->psess );
        break;

        case CSD_VC_CMD_SET_CAL_FID:
          rc = csd_vc_set_cal_fid_fn( pcmd->psess );
        break;

        case CSD_VC_CMD_CLEAN_CALIBRATION_RESOURCES:
          rc = csd_vc_clean_cal_resources( pcmd->psess );
        break;

        case CSD_VC_CMD_UPDATE_STATE:
          rc = csd_vc_update_state( pcmd->psess, pcmd->cmd.update_state_cmd);
        break;
 
        case CSD_VC_CMD_SEND_RSP_CALLBACK:
        {
          csd_vc_event_command_rsp_t event_rsp;
          if( pcmd->psess->csd_vc_cb )
          {
            event_rsp.status_id = pcmd->cmd.rsp_cmd.status_id;
            pcmd->psess->csd_vc_cb ( pcmd->client_token, CSD_VC_EVENTID_COMMAND_RSP,
                                     &event_rsp, sizeof(event_rsp), pcmd->psess->csd_vc_cb_data );
          }
          else
          {
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_command_fn: CSD_VC_CMD_SEND_RSP_CALLBACK failed, no callback has been registered!" );
          }
        }
        break;

        case CSD_VC_CMD_SEND_IVPCM_EVT_NOTIFY:
        {
          csd_vc_ivpcm_evt_notify_v2_t event_rsp;
          if( pcmd->psess->csd_vc_cb )
          {
            csd_memscpy( &event_rsp, sizeof(event_rsp),
                         &( pcmd->cmd.notify_cmd ), sizeof( csd_vc_ivpcm_evt_notify_v2_t ) );
            pcmd->psess->csd_vc_cb ( pcmd->client_token, CSD_VC_IVPCM_EVT_NOTIFY_V2,
                                     &event_rsp, sizeof(event_rsp), pcmd->psess->csd_vc_cb_data );
          }
        }
        break;

        case CSD_VC_CMD_SEND_MAP_MEM_RSP:
        {
          csd_vc_rsp_map_t mem_map_rsp;
          if( pcmd->psess->csd_vc_cb )
          {
            csd_memscpy( &mem_map_rsp,sizeof(mem_map_rsp),
                        &( pcmd->cmd.rspmap_cmd ), sizeof( csd_vc_rsp_map_t ) );
            pcmd->psess->csd_vc_cb ( pcmd->client_token, CSD_VC_COMMAND_RSP_MAP,
                                     &mem_map_rsp, sizeof(mem_map_rsp), pcmd->psess->csd_vc_cb_data );
          }
        }
        break;

        case CSD_VC_CMD_SEND_STATE_NOTIFY_CALLBACK:
        {
          if( pcmd->psess->csd_vc_cb )
          {
            csd_vc_event_state_notify_t event_state;
            event_state.state_id = pcmd->cmd.state_cmd.state_id;
            pcmd->psess->csd_vc_cb( pcmd->client_token, CSD_VC_EVENTID_STATE_NOTIFY,
                                    &event_state, sizeof(event_state), pcmd->psess->csd_vc_cb_data );
          }
        }
        break;

        case CSD_VC_CMD_SEND_TX_DTMF_DETECTED:
        {
          if ( NULL != pcmd->cmd.dtmf_tx_detected && pcmd->psess->csd_vc_cb )
          {
            pcmd->psess->csd_vc_cb( pcmd->client_token, CSD_VC_EVENTID_TX_DTMF_DETECTED,
                               pcmd->cmd.dtmf_tx_detected, sizeof ( csd_vc_event_tx_dtmf_detected_t ), pcmd->psess->csd_vc_cb_data );
          }
        }
        break;

        default:
        break;
      } /* End of Switch */
    } 
    ( void ) apr_list_add_tail ( &csd_vc_free_cmd_list, &pcmd->link );
  } /* End while cmds */
  return rc;
}

static void csd_vc_get_cmd_node ( uint32_t cmd_token, csd_vc_cmd_list_item_t **pcmd_node )
{
  csd_vc_cmd_list_item_t* pcmd_pivot = NULL;
  csd_vc_cmd_list_item_t* pcmd = NULL;

  *pcmd_node = NULL;

  if ( !( apr_list_peak_head ( &csd_vc_track_dispatched_cmd_list, ( apr_list_node_t ** )&pcmd ) ) )
  {
    pcmd_pivot = pcmd;

    do
    {
      if ( pcmd->cmd_token == cmd_token )
      {
        *pcmd_node = pcmd; /* if the token is matching then populate the pcmd_node */
        return;
      }
      else
      {
        pcmd_pivot = pcmd; /* update the pivot pointer */
      }

    } while ( ! ( apr_list_get_next ( &csd_vc_track_dispatched_cmd_list,
              ( apr_list_node_t* )pcmd_pivot, ( apr_list_node_t** )&pcmd ) ) );
  }
}

static int32_t csd_vc_apr_callback (
  aprv2_packet_t* packet,
  void* dispatch_data )
{
  csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;
  aprv2_ibasic_rsp_result_t  *rsp;
  csd_vc_session_item_t  *psess = NULL;
  apr_objmgr_object_t* apr_obj = NULL;
  csd_vc_cmd_list_item_t* pcmd = NULL;
  csd_vc_cmd_list_item_t* pcmd2 = NULL;

  if( !packet )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: Input arg[*packet] is NULL" );
    return APR_EFAILED;
  }
  
  ( void ) apr_objmgr_find_object( &csd_vc_apr_objmgr, packet->dst_port, &apr_obj );
  if( !apr_obj )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: apr_objmgr_find_object failed with port[0x%x]", packet->dst_port );
    __aprv2_cmd_free( csd_vc_apr_handle, packet );
    return APR_EOK; /* This may not be APR issue */
  }
  
  psess = apr_obj->any.ptr;
  if( !psess )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: Unable to find session in the object, obj address[0x%p]", ((void*)apr_obj) );
    __aprv2_cmd_free( csd_vc_apr_handle, packet );
    return APR_EOK; /* This may not be APR issue */
  }
    
  switch( packet->opcode )
  {
    case APRV2_IBASIC_RSP_RESULT:
    {
      rsp = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet );
      if( rsp )
      {
        switch( rsp->opcode )
        {
          case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2:
            if( APR_EOK == rsp->status )
            {
              CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 success, CVP handle[0x%x]", packet->src_port );
              psess->h_cvp = packet->src_port;            
            }
            else
            {
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 failed reported by CVP, rc[0x%x]", rsp->status );
            }
            csd_event_signal( psess->csd_vc_event ); /* we need to keep track of the tokens */
          break;

          case APRV2_IBASIC_CMD_DESTROY_SESSION:
            CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: Received APRV2_IBASIC_CMD_DESTROY_SESSION response, apr_rc[0x%x]", rsp->status );
            csd_event_signal( psess->csd_vc_event ); /* we need to keep track of the tokens */
          break;

          case VSS_IVOCPROC_CMD_SET_DEVICE_V2:
            ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
            if( pcmd )
            {
              if( APR_EOK == rsp->status )
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_SET_DEVICE_V2 success" );
                /*send update*/
                ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd2 );
                if( pcmd2 )
                {
                    psess->sess_state.vc_configured = TRUE;
                    pcmd2->type = CSD_VC_CMD_UPDATE_STATE;
                    pcmd2->cmd.update_state_cmd = rsp->opcode;
                    pcmd2->psess = psess;
                    ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd2->link );
                }
                
                pcmd->type = CSD_VC_CMD_CLEAN_CALIBRATION_RESOURCES;             
              }
              else
              {
                CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_SET_DEVICE_V2 failed reported by CVP, rc[0x%x]", rsp->status );
                pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
                pcmd->cmd.rsp_cmd.status_id = rsp->status;
              }                    
              pcmd->psess = psess;
              pcmd->client_token = psess->set_dev_token;

              ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
              /* signal the csd common thread to process the cmd */
              sigs |= CSD_VOC_COMMON_SIG_CMD;
              ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
            }
          break;

          case VSS_IMEMORY_CMD_MAP_PHYSICAL: /* this is an error case */
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IMEMORY_CMD_MAP_PHYSICAL failed reported by MVM, apr_rc[%d]", rsp->status );
            if( psess->memmap_client == CSD_VC_ENUM_CLIENT_VP3 )
            {
              psess->sess_vp3.descriptor_mapped_handle = 0;
              csd_event_signal( psess->csd_vc_event );
            }
            else
            {
              ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
              if( pcmd )
              {
                if( psess->memmap_client == CSD_VC_ENUM_CLIENT_EXTERNAL )                     
                { 
                  pcmd->type = CSD_VC_CMD_SEND_MAP_MEM_RSP;
                  pcmd->psess = psess;
                  pcmd->cmd.rspmap_cmd.mem_handle = 0;
                  pcmd->client_token = packet->token;
                }
                else if( psess->memmap_client == CSD_VC_ENUM_CLIENT_CAL )
                { /* Force success response to client for cal issues */                      
                  pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
                  pcmd->psess = psess;
                  pcmd->client_token = psess->set_dev_token;
                  pcmd->cmd.rsp_cmd.status_id = CSD_EOK;
                }
                ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
                /* signal the csd common thread to process the cmd */
                sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
              }
            }
          break;
            
          case VSS_IMEMORY_CMD_UNMAP:              
            CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: Received VSS_IMEMORY_CMD_UNMAP response, apr_rc[0x%x]", rsp->status );
            if( psess->memmap_client == CSD_VC_ENUM_CLIENT_VP3 )
            {
              csd_event_signal( psess->csd_vc_event );
            }
            else
            {
              ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
              if( pcmd != NULL ) 
              {
                if( psess->memmap_client == CSD_VC_ENUM_CLIENT_CAL )
                {
                  psess->sess_cal.state = CSD_VC_ENUM_CAL_MEM_ALLOCATED;
                  pcmd->type = CSD_VC_CMD_CLEAN_CALIBRATION_RESOURCES;
                }
                else if( psess->memmap_client == CSD_VC_ENUM_CLIENT_EXTERNAL )
                {
                  pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
                }
                pcmd->psess = psess;
                pcmd->client_token = NULL;                      
                ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
                sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
              }
            }
          break;

          case VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG:
            if( APR_EOK == rsp->status )
            {
              CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG success" );
              psess->sess_cal.devcfg_table.if_registered = TRUE;
              psess->sess_cal.state = CSD_VC_ENUM_CAL_REGISTERED;
            }
            else
            {
                 CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG failed, opcode[0x%x], rc[0x%x]", rsp->opcode, rsp->status );
            }
          break;

          case VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2:
          case VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA:
            if( APR_EOK == rsp->status )
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA success");
                psess->sess_cal.static_table.if_registered = TRUE;
                psess->sess_cal.state = CSD_VC_ENUM_CAL_REGISTERED;
              }
            else
              {
                 CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA failed, opcode[0x%x], rc[0x%x]", rsp->opcode, rsp->status );
              }
          break;

          case VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA:
          case VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA:
            if( APR_EOK == rsp->status )
            {
              CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA success" );
              psess->sess_cal.dynamic_table.if_registered = TRUE;
                psess->sess_cal.state = CSD_VC_ENUM_CAL_REGISTERED;

                if (CSD_VC_ENUM_FID_VOL_DEREGISTERED == psess->sess_fid.set_feature_id_state)
                {
                  psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_VOL_REGISTERED;
                  ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
                  if( pcmd != NULL ) 
                  {
                    pcmd->type = CSD_VC_CMD_SET_CAL_FID;
                    pcmd->psess = psess;
                    pcmd->client_token = NULL;                      
                    ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );

                    sigs |= CSD_VOC_COMMON_SIG_CMD;
                    ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
                  }
                }
              else
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG success" );
                psess->sess_cal.devcfg_table.if_registered = TRUE;
                psess->sess_cal.state = CSD_VC_ENUM_CAL_REGISTERED;
              }
            }
            else
            { /* No action is required, memory will be cleaned up at next SET_DEVICE_CFG or CLOSE call */
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA failed, opcode[0x%x], rc[0x%x]", rsp->opcode, rsp->status );
            }                               
          break;

          case VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA:
          case VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA:
          case VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA:
          case VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA:
          case VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG:
            if( APR_EOK == rsp->status )
            {
              if( rsp->opcode == VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG )
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG success" );
                psess->sess_cal.devcfg_table.if_registered = FALSE;
              }
              else if( rsp->opcode == VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA ||
                       rsp->opcode == VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA )
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA success" );
                psess->sess_cal.static_table.if_registered = FALSE;
              }
              else
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA success" );
                psess->sess_cal.dynamic_table.if_registered = FALSE;
              }

              if( !psess->sess_cal.devcfg_table.if_registered &&
                  !psess->sess_cal.static_table.if_registered &&
                  !psess->sess_cal.dynamic_table.if_registered )
              { 
                psess->sess_cal.state = CSD_VC_ENUM_CAL_MEM_MAPPED;          
                ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
                if( pcmd != NULL ) 
                {
                  pcmd->type = CSD_VC_CMD_CLEAN_CALIBRATION_RESOURCES;
                  pcmd->psess = psess;
                  pcmd->client_token = NULL;                      
                  ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
                  
                  sigs |= CSD_VOC_COMMON_SIG_CMD;
                  ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
                }
              }

              if (!psess->sess_cal.dynamic_table.if_registered && CSD_VC_ENUM_FID_VC_DISABLED == psess->sess_fid.set_feature_id_state)
              {
                psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_VOL_DEREGISTERED;
                ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
                if( pcmd != NULL ) 
                {
                  pcmd->type = CSD_VC_CMD_SET_CAL_FID;
                  pcmd->psess = psess;
                  pcmd->client_token = NULL;                      
                  ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
                  
                  sigs |= CSD_VOC_COMMON_SIG_CMD;
                  ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
                }
              }
            }
            else
            { /* if de-registeration failed, skip the rest of cleanup/re-register steps */
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: Deregistartion failed, opcode[0x%x], rc[0x%x]", rsp->opcode, rsp->status );
              if( psess->bclosing_sequence )
              {
                csd_event_signal( psess->csd_vc_event );
              }
              else
              {
                ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
                if( pcmd != NULL) 
                {
                  pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
                  pcmd->psess = psess;
                  pcmd->cmd.rsp_cmd.status_id = CSD_EOK; /* Client should get success response even cal failed */
                  pcmd->client_token = psess->set_dev_token;
                  ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
              
                  sigs |= CSD_VOC_COMMON_SIG_CMD;
                  ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
                }
              }
            }
          break;

          case VSS_IVOCPROC_CMD_ENABLE:
          case VSS_IVOCPROC_CMD_DISABLE:
          case VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX:
          case VSS_IVOCPROC_CMD_SET_MUTE:
          case VSS_IVOLUME_CMD_MUTE_V2:
          case VSS_ICOMMON_CMD_SET_UI_PROPERTY:
          case VSS_IVOCPROC_CMD_SET_TX_DTMF_DETECTION:
          case VSS_IVOLUME_CMD_SET_STEP:
          case VSS_IVPCM_CMD_START_V2:
          case VSS_IVPCM_CMD_STOP:
          case VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS:
            ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
            if( pcmd ) 
            {
              if( VSS_IVOCPROC_CMD_DISABLE == rsp->opcode )
              {
                if( APR_EOK == rsp->status )
                {
                   psess->sess_state.vc_enabled = FALSE;
                  /*send update*/
                  ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd2 );
                  if( pcmd2 )
                  {
                    psess->sess_state.vc_configured = TRUE;
                    pcmd2->type = CSD_VC_CMD_UPDATE_STATE;
                    pcmd2->cmd.update_state_cmd = rsp->opcode;
                    pcmd2->psess = psess;
                    ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd2->link );
       
                  }
                }
                  
              if ((CSD_VC_ENUM_FID_SET_INITIALIZE == psess->sess_fid.set_feature_id_state )
                  && psess->sess_cal.dynamic_table.if_registered )
                {
                  /* Should ONLY happen if VSS_IVOCPROC_CMD_DISABLE callback is received */
                  psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_VC_DISABLED;
                  pcmd->type = CSD_VC_CMD_SET_CAL_FID;
                 
                }
                else
                {
                  pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
                }
              }
              else if (VSS_IVOCPROC_CMD_ENABLE == rsp->opcode)
              {
                if( APR_EOK == rsp->status )
                {
                   psess->sess_state.vc_enabled = TRUE;
                  /*send update*/
                  ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd2 );
                  if( pcmd2 )
                  {
                    psess->sess_state.vc_configured = TRUE;
                    pcmd2->type = CSD_VC_CMD_UPDATE_STATE;
                    pcmd2->cmd.update_state_cmd = rsp->opcode;
                    pcmd2->psess = psess;
                    ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd2->link );
                  }
                }
                if (CSD_VC_ENUM_FID_VOL_REGISTERED == psess->sess_fid.set_feature_id_state )
                {
                  psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_UNINIITALIZED;
                }
                pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
              }
              else
              {
                pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
              }

              pcmd->psess = psess;
              pcmd->cmd.rsp_cmd.status_id = rsp->status;
              pcmd->client_token = packet->token;

              /*if API called during closing sequence signal instead of sending call back */
              if( psess->bclosing_sequence )
              {
                  CSD_MSG( CSD_PRIO_MEDIUM, "csd_vc_apr_callback : internal API call %d recieved signaling", rsp->opcode);
                  csd_event_signal ( psess->csd_vc_event );
              }
              else
              {
                ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
              }
              /* signal the csd common thread to process the cmd */
              sigs |= CSD_VOC_COMMON_SIG_CMD;
              ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
            }
          break;

          case VSS_IVP3_CMD_GET_SIZE: /* by definition a failure case */
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVP3_CMD_GET_SIZE failed reported by CVP, apr_rc[%d]", rsp->status );
            psess->sess_vp3.size_data = 0;
            csd_event_signal( psess->csd_vc_event );
          break;

          case VSS_IVP3_CMD_GET_DATA:
            if( rsp->status != APR_EOK )
            {
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVP3_CMD_GET_DATA failed reported by CVP, apr_rc[%d]", rsp->status );
              psess->sess_vp3.state = CSD_VC_ENUM_VP3_MEM_MAPPED;
              csd_event_signal( psess->csd_vc_event );
            }
            else
            {
              ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
              if( pcmd )
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVP3_CMD_GET_DATA success" );
                pcmd->type = CSD_VC_CMD_RECEIVE_VP3;
                pcmd->psess = psess;
                pcmd->client_token = 0;
                psess->sess_vp3.state = CSD_VC_ENUM_VP3_DATA_RECEIVED;

                ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
                /* signal the csd common thread to process the cmd */
                sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
              }
            }
          break;
          
          case VSS_IVP3_CMD_SET_DATA:
            if( rsp->status != APR_EOK )
            {
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: VSS_IVP3_CMD_SET_DATA failed reported by CVP, apr_rc[%d]", rsp->status );
            }
            else
            {
              CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVP3_CMD_SET_DATA success" );
            }
          break;
          
          default:
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: Unsupported rsp opcode[0x%x]", rsp->opcode );
          break;
        } /* switch */
      }
    }
    break;

    case VSS_IVOCPROC_EVT_TX_DTMF_DETECTED:
    {
         csd_vc_cmd_list_item_t* pcmd = NULL; /* pointer to tracked command item */
          /* obtain a free command item */
          ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
          if ( pcmd != NULL )
          {
            pcmd->type = CSD_VC_CMD_SEND_TX_DTMF_DETECTED;
            pcmd->cmd.dtmf_tx_detected = APRV2_PKT_GET_PAYLOAD( csd_vc_event_tx_dtmf_detected_t, packet);
            pcmd->client_token = packet->token;
			pcmd->psess = psess;
            ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
            /* signal the csd common thread to process the cmd */
            sigs |= CSD_VOC_COMMON_SIG_CMD;
            ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
          }
          CSD_MSG(CSD_PRIO_LOW,
                        "csd_vc_apr_callback: TX DTMF detected " );
    }
    break;

    case VSS_ICOMMON_RSP_GET_UI_PROPERTY:
    {
      csd_vc_cmd_list_item_t* pcmd_cb = NULL; /* pointer to callback command item*/
      vss_icommon_rsp_get_ui_property_t *get_ui_rsp = NULL;
      /* retreive the client's command request based on unique token */
      ( void )csd_vc_get_cmd_node( packet->token, &pcmd );

      get_ui_rsp = ( vss_icommon_rsp_get_ui_property_t * )APRV2_PKT_GET_PAYLOAD( uint8_t, packet );

      if ( ( NULL != pcmd) &&
           ( NULL != get_ui_rsp ) &&
           ( 0 == get_ui_rsp->status ) &&
           ( NULL != pcmd->cmd.getui_cmd.param_data ) &&
           ( get_ui_rsp->module_id == pcmd->cmd.getui_cmd.module_id ) &&
           ( get_ui_rsp->param_id == pcmd->cmd.getui_cmd.param_id ) &&
           ( get_ui_rsp->param_size <= pcmd->cmd.getui_cmd.param_size ) )
      {
        csd_memscpy ( pcmd->cmd.getui_cmd.param_data,
                      pcmd->cmd.getui_cmd.param_size,
                      ((uint8_t*)get_ui_rsp + sizeof ( vss_icommon_rsp_get_ui_property_t )),/* refer to the vss_common_public_if.h */
                      get_ui_rsp->param_size );
      }

      csd_event_signal ( psess->csd_vc_event ); /* Bring GET UI Call Out of Wait State */
      /* obtain a free command item to send response to csd client */
      ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, ( ( apr_list_node_t**) &pcmd_cb ) );
      if( pcmd_cb ) 
      {
        pcmd_cb->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
        pcmd_cb->psess = psess;

        if ( NULL != get_ui_rsp )
        {
          pcmd_cb->cmd.rsp_cmd.status_id= get_ui_rsp->status;
        }
        else
        {
          pcmd_cb->cmd.rsp_cmd.status_id = CSD_EFAILED;
        }

        if ( NULL != pcmd ) 
        {
          pcmd_cb->client_token = pcmd->client_token;
        }
        else
        {
          pcmd_cb->client_token = 0;
        }

        ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd_cb->link );
        /* signal the csd common thread to process the cmd */
        sigs |= CSD_VOC_COMMON_SIG_CMD;
        ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
      }

      /* return the tracked cmd item back to free queue */
      if ( NULL != pcmd )
      { /* return the list item back into the free cmd list */
        ( void ) apr_list_delete ( &csd_vc_track_dispatched_cmd_list, ( apr_list_node_t* )&pcmd->link );
        ( void ) apr_list_add_tail ( &csd_vc_free_cmd_list, ( apr_list_node_t* )&pcmd->link );
      }
    }
    break;

    case VSS_IMEMORY_RSP_MAP: /* by definition a successful response */
    {
      vss_imemory_rsp_map_t* pmemmap_rsp = (vss_imemory_rsp_map_t*)APRV2_PKT_GET_PAYLOAD( uint8_t, packet );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IMEMORY_CMD_MAP success, received mem_handle[0x%x]", pmemmap_rsp->mem_handle );
      if( psess->memmap_client == CSD_VC_ENUM_CLIENT_VP3 )
      {
        psess->sess_vp3.descriptor_mapped_handle = pmemmap_rsp->mem_handle;
        psess->sess_vp3.state = CSD_VC_ENUM_VP3_MEM_MAPPED;
        csd_event_signal( psess->csd_vc_event );
      }
      else
      {
        ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
        if( NULL != pcmd )
        {
          if( psess->memmap_client == CSD_VC_ENUM_CLIENT_EXTERNAL )                     
          { 
            pcmd->type = CSD_VC_CMD_SEND_MAP_MEM_RSP;
            pcmd->cmd.rspmap_cmd.mem_handle = pmemmap_rsp->mem_handle;
          }
          else if( psess->memmap_client == CSD_VC_ENUM_CLIENT_CAL )
          {
            psess->sess_cal.descriptor_mapped_handle = pmemmap_rsp->mem_handle;
            psess->sess_cal.state = CSD_VC_ENUM_CAL_MEM_MAPPED;
            pcmd->type = CSD_VC_CMD_SEND_CALIBRATION;
			
            ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd2 );
            if( pcmd2 )
			{
              pcmd2->type = CSD_VC_CMD_SEND_VP3;
              pcmd2->psess = psess;
              pcmd2->client_token = packet->token;
            }
          }
          pcmd->psess = psess;
          pcmd->client_token = packet->token;
        
          ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
		  if( pcmd2 )
		  {
		    ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd2->link );
		  }
          /* signal the csd common thread to process the cmd */
          sigs |= CSD_VOC_COMMON_SIG_CMD;
          ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
        }
      }
    }
    break;

    case APRV2_IBASIC_EVT_ACCEPTED:
    break;

    case VSS_IVPCM_EVT_NOTIFY_V2:
    {
      vss_ivpcm_evt_notify_v2_t *vss_ivpcm_evt = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );
      if( NULL != vss_ivpcm_evt )
      {
        ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
        if( pcmd ) 
        {
          csd_memscpy( &( pcmd->cmd.notify_cmd ), sizeof( pcmd->cmd.notify_cmd ),
                       vss_ivpcm_evt , sizeof(vss_ivpcm_evt_notify_v2_t) );
          pcmd->type = CSD_VC_CMD_SEND_IVPCM_EVT_NOTIFY;
          pcmd->psess = psess;
          pcmd->client_token = packet->token;
    
          ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
          /* signal the csd common thread to process the cmd */
          sigs |= CSD_VOC_COMMON_SIG_CMD;
          ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
        }
      }
    }
    break;

    case VSS_IVP3_RSP_GET_SIZE:
    { /* by definition a success rsp */
      vss_ivp3_rsp_get_size_t* size_rsp = (vss_ivp3_rsp_get_size_t*)APRV2_PKT_GET_PAYLOAD( uint8_t, packet );
      
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_apr_callback: VSS_IVP3_CMD_GET_SIZE success, size[%d]", size_rsp->size );
      ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
      if( NULL != pcmd )
      {
        pcmd->type = CSD_VC_CMD_RECEIVE_VP3;
        psess->sess_vp3.size_data = size_rsp->size;
        psess->sess_vp3.state = CSD_VC_ENUM_VP3_INFO_RECEIVED;
        pcmd->psess = psess;
        pcmd->client_token = packet->token;
   
        ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
        /* signal the csd common thread to process the cmd */
        sigs |= CSD_VOC_COMMON_SIG_CMD;
        ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
      }
    }
    break;
    
    default:
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_apr_callback: Unsupported packet opcode[0x%x]", packet->opcode );
    break;
  } /* switch( packet->opcode ) */

  __aprv2_cmd_free( csd_vc_apr_handle, packet );
  /* Note that rc needs to be APR_EOK in order to prevent APR from freeing the packet a second time */
  return APR_EOK;
}

/*
 ** End Internal functions
*/


/*
 ** Start External APIs
*/

CSD_INTERNAL int32_t csd_vc_init(struct csd_vtable** vtable ) 
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint8_t i = 0;
  uint8_t checkpoint = 0;

  rc = csd_voc_common_get_session( csd_vc_command_fn,
                                   &h_csd_voc_common );

  if ( ( CSD_EOK == rc )
       && ( vtable ) ) 
  {
    checkpoint = 1;
    /* CSD vc get session success */
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_VC_INIT_STAGE,
                       //CSD_INIT_VC_SESSION_GET,0,0); 
    csd_vc_vtable.open_fn = ( csd_open_fn ) csd_vc_open;
    csd_vc_vtable.close_fn = ( csd_close_fn )csd_vc_close;
    csd_vc_vtable.ioctl_fn = ( csd_ioctl_fn )csd_vc_ioctl;
    csd_vc_vtable.read_fn = ( csd_read_fn )csd_vc_read;
    csd_vc_vtable.write_fn = ( csd_write_fn ) csd_vc_write;
    *vtable = &csd_vc_vtable;
  }
  else
  {
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_VC_INIT_STAGE,
                       //CSD_INIT_VC_SESSION_GET,0,0); 
  }

  if ( CSD_EOK == rc ) 
  {
    apr_rc = __aprv2_cmd_local_dns_lookup( csd_vc_cvp_dest_dns,
                                       sizeof(csd_vc_cvp_dest_dns),
                                       &csd_vc_cvp_dest_addr);

    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                         //CSD_INIT_VC_DNS_CVP_LOOK_UP,0,0); 

    if ( APR_EOK == apr_rc )
    {
      apr_rc = __aprv2_cmd_local_dns_lookup( csd_vc_mvm_dest_dns,
                                       sizeof(csd_vc_mvm_dest_dns),
                                       &csd_vc_mvm_dest_addr);
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_DNS_MVM_LOOK_UP,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      apr_rc = __aprv2_cmd_register2( &csd_vc_apr_handle,
                                csd_vc_dns,
                                sizeof ( csd_vc_dns ),
                                0,
                                csd_vc_apr_callback,
                                NULL,
                                &csd_vc_src_addr );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_APR_REGISTER,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 2;
      apr_rc = csd_lock_create( &csd_vc_task_lock );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_APR_LOCK_CREATE,0,0); 
    }

    if ( CSD_OSAL_EOK == apr_rc ) 
    {
      checkpoint = 3;
      apr_rc = apr_list_init( &csd_vc_free_cmd_list, csd_vc_task_lock_fn, csd_vc_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_FREE_CMD_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      checkpoint = 4;
      apr_rc = apr_list_init( &csd_vc_active_cmd_list, csd_vc_task_lock_fn, csd_vc_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_ACTIVE_CMD_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      checkpoint = 5;
      apr_rc = apr_list_init( &csd_vc_track_dispatched_cmd_list, csd_vc_task_lock_fn, csd_vc_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_DISPATCH_CMD_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      checkpoint = 6;
      for ( i = 0; i < CSD_VC_NUM_CMDS; ++i ) 
      {
        if ( APR_EOK != ( apr_rc = apr_list_add_tail( &csd_vc_free_cmd_list,
                                                ( apr_list_node_t* ) &csd_vc_cmd_pool[i] ) ) )
        {
          break;
        }
      }
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_FREE_CMD_LIST_ADD,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      apr_rc = apr_list_init( &csd_vc_free_sess_list, csd_vc_task_lock_fn, csd_vc_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_FREE_SESS_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 7;
      apr_rc = apr_list_init( &csd_vc_active_sess_list, csd_vc_task_lock_fn, csd_vc_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_ACTIVE_SESS_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 8;
      for ( i = 0; i < CSD_VC_NUM_SESSIONS; ++i ) 
      {
        if ( APR_EOK != ( apr_rc = apr_list_add_tail( &csd_vc_free_sess_list,
                              ( ( apr_list_node_t* ) &(csd_vc_sess_pool[i].link) ) ) ) )
        {
          break;
        }
      }
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                         //CSD_INIT_VC_FREE_SESS_LIST_ADD,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      /* Initialize the object manager. */
      apr_objmgr_setup_params_t params;

      params.table = csd_vc_object_table;
      params.total_bits = CSD_VC_HANDLE_TOTAL_BITS_V;
      params.index_bits = CSD_VC_HANDLE_INDEX_BITS_V;
      params.lock_fn = csd_vc_task_lock_fn;
      params.unlock_fn = csd_vc_task_unlock_fn;
      apr_rc = apr_objmgr_construct( &csd_vc_apr_objmgr, &params );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VC_INIT_STAGE,
                         //CSD_INIT_VC_OBJECT_MGR_INIT,0,0); 
    }

    if ( APR_EOK != apr_rc ) 
    {
      /* free resources here*/
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_init: error during initialization checkpoint[%d]", checkpoint );
      switch ( checkpoint ) 
      {
        case 8:
          ( void ) apr_list_destroy ( &csd_vc_active_sess_list );
          /*csd vc active session list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_ACTIVE_SESS_LIST_INIT,0,0); 
        case 7:
          ( void ) apr_list_destroy ( &csd_vc_free_sess_list );
          /*csd vc free session list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_FREE_SESS_LIST_INIT,0,0); 
        case 6:
          ( void ) apr_list_destroy ( &csd_vc_track_dispatched_cmd_list );
          /*csd vc dispatch list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_DISPATCH_CMD_LIST_INIT,0,0); 
        case 5:
          ( void ) apr_list_destroy ( &csd_vc_free_cmd_list );
          /*csd vc free command list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_FREE_CMD_LIST_INIT,0,0); 
        case 4:
          ( void ) apr_list_destroy ( &csd_vc_active_cmd_list );
          /*csd vc active command list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_ACTIVE_CMD_LIST_INIT,0,0); 
        case 3:
          ( void ) csd_lock_destroy ( csd_vc_task_lock );
          /*csd vc apr lock destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_APR_LOCK_CREATE,0,0); 
        case 2:
          ( void ) __aprv2_cmd_deregister( csd_vc_apr_handle);
          /*csd vc apr de-registration done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_APR_REGISTER,0,0); 
        case 1:
          ( void ) csd_voc_common_free_session ( &h_csd_voc_common );
          /*csd vc release session done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                           //CSD_INIT_VC_SESSION_GET,0,0); 
        break;
        default:
        break;
      }/* switch */
    }
    rc = csd_map_apr_error_to_csd ( apr_rc );
  }

  CSD_MSG( CSD_PRIO_LOW,"Exit, rc[0x%x]",rc );

  return rc;
}

CSD_INTERNAL int32_t csd_vc_deinit ( void ) {

  ( void ) apr_list_destroy ( &csd_vc_active_sess_list );
  /*csd vc active session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                    // CSD_INIT_VC_ACTIVE_SESS_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vc_free_sess_list );
  /*csd vc free session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_FREE_SESS_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vc_track_dispatched_cmd_list );
  /*csd vc dispatch list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_DISPATCH_CMD_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vc_free_cmd_list );
  /*csd vc free command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_FREE_CMD_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vc_active_cmd_list );
  /*csd vc active command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_ACTIVE_CMD_LIST_INIT,0,0); 

  ( void ) csd_lock_destroy ( csd_vc_task_lock );
  /*csd vc apr lock destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_APR_LOCK_CREATE,0,0); 

  ( void ) __aprv2_cmd_deregister( csd_vc_apr_handle);
  /*csd vc apr de-registration done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_APR_REGISTER,0,0); 

  ( void ) csd_voc_common_free_session ( &h_csd_voc_common );
  /*csd vc release session done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_SESSION_GET,0,0); 

  /* Release the object manager. */
  ( void ) apr_objmgr_destruct( &csd_vc_apr_objmgr );
  /*csd vc destroy object manager done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VC_INIT_STAGE,
                     //CSD_INIT_VC_OBJECT_MGR_INIT,0,0); 

  return CSD_EOK;
}

CSD_INTERNAL int32_t csd_vc_open (
  uint32_t open_id,
  void* params,
  uint32_t size
) 
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vc_session_item_t* psess = NULL;
  apr_objmgr_object_t* objmgr_obj = NULL;
  csd_vc_open_full_control_t* csd_vc_cmd = ( csd_vc_open_full_control_t* )params;
  uint8_t checkpoint = 0;

  /*validate session is unique*/
  if( csd_vc_session_exsists(csd_vc_cmd->name) )
  {
    rc = CSD_EALREADY;
     CSD_MSG( CSD_PRIO_ERROR, "csd_vc_open: Failed!! Session with same name already exsists");
  }
  else
  {

    /* Get a session */
    apr_rc = apr_list_remove_head( &csd_vc_free_sess_list, (apr_list_node_t**)&psess );
    if( apr_rc == APR_EOK && psess )
    {
      checkpoint = 1;
      memset( (void*)psess, 0, sizeof(csd_vc_session_item_t) );
      ( void ) apr_objmgr_alloc_object( &csd_vc_apr_objmgr, &objmgr_obj );
    rc = csd_map_apr_error_to_csd( apr_rc );
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_open: Unable to get a free session, apr_rc[0x%x]", apr_rc );
      rc = CSD_ENORESOURCE;
    }
    
    if( apr_rc == APR_EOK && objmgr_obj )
    {
      checkpoint = 2;
      apr_rc = csd_event_create( &psess->csd_vc_event );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_open: Unable to alloc a new objuect" );
      rc = CSD_ENORESOURCE;
    }

    if( rc == CSD_EOK && objmgr_obj && psess ) 
    {
      checkpoint = 3;
      objmgr_obj->any.ptr = psess;
      psess->h_apr_obj = objmgr_obj->handle;    
      ( void ) apr_list_add_tail( &csd_vc_active_sess_list, &psess->link );
    }

    if( rc == CSD_EOK ) 
    {
      vss_ivocproc_cmd_create_full_control_session_v2_t cvp_cmd;
      aprv2_packet_t* papr_packet = NULL;

      cvp_cmd.direction        = csd_vc_cmd->direction;
      cvp_cmd.profile_id       = csd_vc_cmd->network_id;
      cvp_cmd.rx_port_id       = VSS_IVOCPROC_PORT_ID_NONE;
      cvp_cmd.rx_topology_id   = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
      cvp_cmd.tx_port_id       = VSS_IVOCPROC_PORT_ID_NONE;
      cvp_cmd.tx_topology_id   = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
      cvp_cmd.vocproc_mode     = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
      cvp_cmd.ec_ref_port_id   = VSS_IVOCPROC_PORT_ID_NONE;

      apr_rc = __aprv2_cmd_alloc_ext( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                  csd_vc_src_addr, psess->h_apr_obj,
                                  csd_vc_cvp_dest_addr, NULL,
                                  NULL, VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2,
                                  sizeof(vss_ivocproc_cmd_create_full_control_session_v2_t) + csd_vc_cmd->name_size,
                                  &papr_packet );

      if( apr_rc == APR_EOK && papr_packet )
      { /* copy the param_data into apr buffer */
        uint8_t* pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
        uint32_t psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(papr_packet->header);
        csd_memscpy( pdata, psize,
                    &cvp_cmd, sizeof ( vss_ivocproc_cmd_create_full_control_session_v2_t ) ); /* copy the cmd structure */
        pdata += sizeof(vss_ivocproc_cmd_create_full_control_session_v2_t);
        psize -= sizeof(vss_ivocproc_cmd_create_full_control_session_v2_t);
        csd_memscpy( pdata, psize, csd_vc_cmd->name, csd_vc_cmd->name_size );
        apr_rc = __aprv2_cmd_forward( csd_vc_apr_handle, papr_packet );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_open: VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 sent, apr_rc[0x%x], sess_name[%s]", rc, csd_vc_cmd->name );
      }
      
      if( apr_rc == APR_EOK )
      {
        apr_rc = csd_event_wait( psess->csd_vc_event );
      }
      rc = csd_map_apr_error_to_csd( apr_rc );

      if( psess->h_cvp != NULL )
      {
        psess->h_csd = csd_handle_malloc( CSD_OPEN_VOICE_CONTEXT, ( void* )&objmgr_obj->handle );
        psess->csd_vc_cb = ( csd_vc_event_callback_fn_t )csd_vc_cmd->callback_fn;
        psess->csd_vc_cb_data = csd_vc_cmd->cb_data;

        strlcpy ( psess->vc_name, csd_vc_cmd->name, sizeof( psess->vc_name ) );
        psess->vc_name_size = csd_vc_cmd->name_size;
        /*Initialize the psession device ID*/
        psess->tx_dev_id = CSD_DEV_ID_INVALID;
        psess->rx_dev_id = CSD_DEV_ID_INVALID;
        psess->ec_ref_id = CSD_DEV_ID_INVALID;
        psess->sess_fid.volume_feature_id = ACDB_VOCVOL_FID_DEFAULT;
        psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_UNINIITALIZED;
        psess->sess_fid.volume_cal_size_used = 0;
      psess->sess_fid.topology_feature_id = (uint32_t)CSD_FID_TOPOLOGY_DEFAULT;
        /*update state*/
       csd_vc_update_state (psess, VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 );
      }
      else
      {
        rc = CSD_EFAILED;
      }
    }

    /* VP3 memory allocation and mapping */
    if( 0 )
    {
      int32_t vp3_rc;
      uint32_t nContigLen  = 0;
      
      switch( psess->sess_vp3.state )
      {
        case CSD_VC_ENUM_VP3_UNINITIALIZED:
        {            
          vss_imemory_table_t* vss_mem_tbl = NULL;
          vss_imemory_block_t* vss_mem_block = NULL;
          /* Allocate buffer for VP3 data, maximum 1K */
          csd_mem_alloc( VP3_MAXIMUM_BUFFER_SIZE, &psess->sess_vp3.data_memory_handle, &psess->sess_vp3.pva_data );
          if( !psess->sess_vp3.pva_data )
          {
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_open: Failed to allocate memory with size[%d]", VP3_MAXIMUM_BUFFER_SIZE );
            break;
          }
          memset( ( void* )psess->sess_vp3.pva_data, 0, VP3_MAXIMUM_BUFFER_SIZE );
          csd_get_mem_info( psess->sess_vp3.data_memory_handle, (uint64_t*)&psess->sess_vp3.phy_addr_data, 
                            &nContigLen );
          psess->sess_vp3.state = CSD_VC_ENUM_VP3_MEM_ALLOCATED;
          /* Allocate buffer for descriptor table, maximum 1K */
          csd_mem_alloc( VP3_MAXIMUM_BUFFER_SIZE, &psess->sess_vp3.descriptor_memory_handle, &psess->sess_vp3.pva_descriptor_table );
          if( !psess->sess_vp3.pva_descriptor_table )
          {
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_open: Failed to allocate memory with size[%d]", VP3_MAXIMUM_BUFFER_SIZE );
            break;
          }
          
          memset( ( void* )psess->sess_vp3.pva_descriptor_table, 0, VP3_MAXIMUM_BUFFER_SIZE );
          vss_mem_tbl = (vss_imemory_table_t*)psess->sess_vp3.pva_descriptor_table;
          vss_mem_tbl->next_table_descriptor.mem_address = 0;
          vss_mem_tbl->next_table_descriptor.mem_size = 0;  

          vss_mem_block = (vss_imemory_block_t*)( psess->sess_vp3.pva_descriptor_table + sizeof(vss_imemory_table_descriptor_t) );
          vss_mem_block->mem_address = psess->sess_vp3.phy_addr_data;
          vss_mem_block->mem_size = nContigLen;
          CSD_MSG( CSD_PRIO_LOW, "csd_vc_open: Mem block filled in descriptor table, phy addr[0x%llx], size[%d]", vss_mem_block->mem_address, nContigLen );
        }
        /* Fall through */
        case CSD_VC_ENUM_VP3_MEM_ALLOCATED:
        {
          vss_imemory_cmd_map_physical_t cvp_cmd;
          csd_get_mem_info( psess->sess_vp3.descriptor_memory_handle,
                            (uint64_t*)&cvp_cmd.table_descriptor.mem_address, &nContigLen );
          
          cvp_cmd.table_descriptor.mem_size = sizeof(vss_imemory_block_t) + sizeof(vss_imemory_table_descriptor_t);
          cvp_cmd.is_cached         = TRUE;
          cvp_cmd.cache_line_size   = CSD_VOC_COMMON_CACHE_LINE_SIZE;
          cvp_cmd.access_mask       = 1;
          cvp_cmd.page_align        = CSD_VOC_COMMON_PAGE_ALIGN;
          cvp_cmd.min_data_width    = CSD_VOC_COMMON_MIN_DATA_WIDTH;
          cvp_cmd.max_data_width    = CSD_VOC_COMMON_MAX_DATA_WIDTH;

          psess->memmap_client = CSD_VC_ENUM_CLIENT_VP3;
          
          vp3_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                           csd_vc_src_addr, psess->h_apr_obj,
                                           csd_vc_mvm_dest_addr, APR_NULL_V,
                                           0, VSS_IMEMORY_CMD_MAP_PHYSICAL,
                                           &cvp_cmd, sizeof(vss_imemory_cmd_map_physical_t) );
          CSD_MSG( CSD_PRIO_HIGH, "csd_vc_open: VSS_IMEMORY_CMD_MAP_PHYSICAL sent, apr_rc[0x%x], mem_addr[0x%llX], mem_size[%d]",
                   vp3_rc, cvp_cmd.table_descriptor.mem_address, cvp_cmd.table_descriptor.mem_size );
          
          if( vp3_rc == APR_EOK )
          {
            vp3_rc = csd_event_wait( psess->csd_vc_event );
          }
        }
        break;
        
        default:
        break;
      }
    }
    
    if( rc != CSD_EOK )
    { /* free resources if error occurs */
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_open: Error occured, cleanup with checkpoint[%d]", checkpoint );
      switch( checkpoint )
      {
        case 3:
          ( void ) apr_list_delete( &csd_vc_active_sess_list, &psess->link );
          ( void ) csd_event_destroy( psess->csd_vc_event );
        /* Fall through */
        case 2:
           ( void ) apr_objmgr_free_object( &csd_vc_apr_objmgr, objmgr_obj->handle );
        /* Fall through */
        case 1:
          ( void ) apr_list_add_tail( &csd_vc_free_sess_list, &psess->link );
        break;
      }
    }
  }
  *csd_vc_cmd->ret_status = rc;
  if( rc == CSD_EOK )
  {
    return psess->h_csd;
  }
  else
  {
    return 0;/* return a 0 handle if there is an error */
  }
}

CSD_INTERNAL int32_t csd_vc_close( uint32_t handle ) 
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vc_session_item_t* psess = NULL;
  csd_vc_cmd_list_item_t* pcmd = NULL;

  psess = csd_voc_common_obtain_session_from_handle( handle, &csd_vc_apr_objmgr );
  if( !psess ) 
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_close: Unable to retrieve session with handle[0x%x]", handle );
    return CSD_EBADPARAM;
  }
  
  /* Destroy server session */
  if( FALSE == csd_handle_is_ssr( handle ) ) 
  {
    psess->bclosing_sequence = TRUE;      
      
    /* if we are in ready state during a close we need to disable first */
    if( psess->sess_state.state == CSD_VSM_HANDLE_STATE_READY )
    {
       apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   0, VSS_IVOCPROC_CMD_DISABLE,
                                   NULL, 0 );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOCPROC_CMD_DISABLE sent, apr_rc[0x%x]", apr_rc );
      apr_rc = csd_event_wait ( psess->csd_vc_event );
      if ( APR_EOK != apr_rc )
      {
        CSD_MSG( CSD_PRIO_ERROR,
                 "csd_vc_close: csd_event_wait failed when waiting for disable clean-up, apr_rc[0x%x]",
                 apr_rc);
      }

    }
    if( psess->sess_cal.state != CSD_VC_ENUM_CAL_UNINITIALIZED )
    {
      ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
      if( pcmd != NULL ) 
      {            
        csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;
        pcmd->type = CSD_VC_CMD_CLEAN_CALIBRATION_RESOURCES;
        pcmd->psess = psess;

        ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
        /* signal the csd common thread to process the cmd */
        sigs |= CSD_VOC_COMMON_SIG_CMD;
        ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);

        apr_rc = csd_event_wait ( psess->csd_vc_event );
        if ( APR_EOK != apr_rc )
        {
          CSD_MSG( CSD_PRIO_ERROR,
                "csd_vc_close: csd_event_wait failed when waiting for calibration clean-up, apr_rc[0x%x]",
                apr_rc);
        }
      }
    }
    
    /* Cleanup VP3 memory */
    switch( psess->sess_vp3.state )
    {
      case CSD_VC_ENUM_VP3_MEM_MAPPED:
      {
        vss_imemory_cmd_unmap_t cmd;
        cmd.mem_handle = psess->sess_vp3.descriptor_mapped_handle;
        psess->memmap_client = CSD_VC_ENUM_CLIENT_VP3;

        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                         csd_vc_src_addr, psess->h_apr_obj,
                                         csd_vc_mvm_dest_addr, APR_NULL_V,
                                         0, VSS_IMEMORY_CMD_UNMAP,
                                         &cmd, sizeof(vss_imemory_cmd_unmap_t) );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_close: VSS_IMEMORY_CMD_UNMAP sent, apr_rc[%d], mem_handle[0x%x]", rc, cmd.mem_handle );
        apr_rc = csd_event_wait( psess->csd_vc_event );

        if ( APR_EOK != apr_rc )
        {
          CSD_MSG( CSD_PRIO_ERROR,
                "csd_vc_close: csd_event_wait failed when waiting for Unmap for VP3, apr_rc[0x%x]",
                apr_rc);
        }
        psess->sess_vp3.state = CSD_VC_ENUM_VP3_MEM_ALLOCATED;
      }
      /* Fall through */
      case CSD_VC_ENUM_VP3_MEM_ALLOCATED:
        csd_mem_free( psess->sess_vp3.data_memory_handle );
        csd_mem_free( psess->sess_vp3.descriptor_memory_handle );
        psess->sess_vp3.state = CSD_VC_ENUM_VP3_UNINITIALIZED;
      break;
      
      default:
        CSD_MSG( CSD_PRIO_LOW, "csd_vc_close: No processing required at VP3 state[%d]", psess->sess_vp3.state );
      break;
    }
    
    /* Destroy server session */
    apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                 csd_vc_src_addr, psess->h_apr_obj,
                                 csd_vc_cvp_dest_addr, psess->h_cvp,
                                 NULL, APRV2_IBASIC_CMD_DESTROY_SESSION,
                                 NULL, 0 );
    CSD_MSG( CSD_PRIO_HIGH, "csd_vc_close: APRV2_IBASIC_CMD_DESTROY_SESSION sent, apr_rc[0x%x], CVP handle[0x%x]", apr_rc, psess->h_cvp );
    if( apr_rc == APR_EOK )
    {
      apr_rc = csd_event_wait( psess->csd_vc_event );
    }
    if ( APR_EOK != apr_rc )
    {
      CSD_MSG( CSD_PRIO_ERROR,
            "csd_vc_close: csd_event_wait failed when waiting for APR Destroy Session, apr_rc[0x%x]",
            apr_rc);
    }
  }
  rc = csd_map_apr_error_to_csd ( apr_rc );

  if( rc == CSD_EOK )
  {
     /*update state*/
     psess->sess_state.vc_configured = FALSE;
     psess->sess_state.vc_enabled = FALSE;
     csd_vc_update_state (psess, APRV2_IBASIC_CMD_DESTROY_SESSION);
  }

  /* free resources associated with session */
  if( CSD_DEV_ID_INVALID != psess->tx_dev_id )
  {
    csd_voc_commmon_remove_device_info( psess->tx_dev_id );
  }
  if( CSD_DEV_ID_INVALID != psess->rx_dev_id )
  {
    csd_voc_commmon_remove_device_info( psess->rx_dev_id );
  }
  if ( CSD_DEV_ID_INVALID != psess->ec_ref_id )
  {
    csd_voc_commmon_remove_device_info( psess->ec_ref_id );
  }

  ( void ) csd_event_destroy( psess->csd_vc_event );
  ( void ) csd_handle_free( psess->h_csd );
  ( void ) apr_objmgr_free_object( &csd_vc_apr_objmgr, psess->h_apr_obj );
  ( void ) apr_list_delete( &csd_vc_active_sess_list, &( psess->link ) );
  ( void ) apr_list_add_tail( &csd_vc_free_sess_list, &( psess->link ) );
    
  return rc;
}

uint32_t  csd_vc_get_server_handle( uint32_t  h_csd_vc )
{
  csd_vc_session_item_t  *psess = (csd_vc_session_item_t*)h_csd_vc;
  if( psess != NULL)
  {
    return psess->h_cvp;
  }
  return NULL;
}

CSD_INTERNAL int32_t csd_vc_ioctl(
   uint32_t handle,
   uint32_t code,
   void* params,
   uint32_t size )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vc_session_item_t  *psess = NULL;

  if( NULL == params )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: Input arg[*params] is NULL" );
    return CSD_EBADPARAM;
  }

  if( NULL == handle && CSD_VC_IOCTL_GET_DEVICE_IDS == code )
  {
    return csd_vc_get_device_ids( params );
  }

  /* extract psession from handle */
  psess = csd_voc_common_obtain_session_from_handle( handle, &csd_vc_apr_objmgr );
  if( !psess )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: Unable to retrieve active sess from handle[0x%x]", handle );
    return CSD_EBADPARAM;
  }
  
  switch( code )
  {
    case CSD_VC_IOCTL_SET_UI_PROPERTY: /* One or more */
    {
      csd_vc_ioctl_set_ui_property_t *csd_vc_prop = (csd_vc_ioctl_set_ui_property_t*)params;
      vss_icommon_cmd_set_ui_property_t cvp_cmd;
      aprv2_packet_t* papr_packet = NULL;

      cvp_cmd.module_id  = csd_vc_prop->module_id;
      cvp_cmd.param_id   = csd_vc_prop->param_id;
      cvp_cmd.param_size = csd_vc_prop->param_size;
      cvp_cmd.reserved   = 0;

      apr_rc = __aprv2_cmd_alloc_ext( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                  csd_vc_src_addr, psess->h_apr_obj,
                                  csd_vc_cvp_dest_addr, psess->h_cvp,
                                  csd_vc_prop->cmd_token, VSS_ICOMMON_CMD_SET_UI_PROPERTY,
                                  sizeof(vss_icommon_cmd_set_ui_property_t) + cvp_cmd.param_size,
                                  &papr_packet );

      if( APR_EOK == apr_rc && papr_packet != NULL )
      { /* copy the param_data into apr buffer */
        uint8_t* pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
        uint32_t psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(papr_packet->header);
        csd_memscpy( pdata, psize,
                     &cvp_cmd, sizeof(vss_icommon_cmd_set_ui_property_t) ); /* copy the cmd structure */
        pdata += sizeof(vss_icommon_cmd_set_ui_property_t);
        psize -= sizeof(vss_icommon_cmd_set_ui_property_t);
        csd_memscpy( pdata, psize, csd_vc_prop->param_data, csd_vc_prop->param_size );
        apr_rc = __aprv2_cmd_forward( csd_vc_apr_handle, papr_packet );
      }

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: CSD_VC_IOCTL_SET_UI_PROPERTY sent, apr_rc[0x%x], module id[0x%x], "
               "param_id[0x%x], param_size[0x%x]", apr_rc, cvp_cmd.module_id, cvp_cmd.param_id, cvp_cmd.param_size );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_GET_UI_PROPERTY:
    {
      csd_vc_ioctl_get_ui_property_t *csd_vc_prop = (csd_vc_ioctl_get_ui_property_t* )params;
      vss_icommon_cmd_get_ui_property_t cvp_cmd;
      csd_vc_cmd_list_item_t* pcmd = NULL; /* use this to keep track of the buffers*/

      apr_rc = apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
      if( APR_EOK == apr_rc && NULL != pcmd )
      {
        csd_memscpy ( &pcmd->cmd, sizeof(pcmd->cmd),
                      csd_vc_prop, sizeof(csd_vc_ioctl_get_ui_property_t) );
        pcmd->cmd_token = token_counter;
        pcmd->client_token = csd_vc_prop->cmd_token; 
        apr_rc = apr_list_add_tail( &csd_vc_track_dispatched_cmd_list, &pcmd->link );

        cvp_cmd.module_id  = csd_vc_prop->module_id;
        cvp_cmd.param_id   = csd_vc_prop->param_id;
        cvp_cmd.param_size = csd_vc_prop->param_size;

        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vc_src_addr, psess->h_apr_obj,
                                     csd_vc_cvp_dest_addr, psess->h_cvp,
                                     token_counter++, VSS_ICOMMON_CMD_GET_UI_PROPERTY,
                                     &cvp_cmd, sizeof(vss_icommon_cmd_get_ui_property_t) );
                                     
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_ICOMMON_CMD_GET_UI_PROPERTY sent, apr_rc[0x%x], module id[0x%x], "
                 "param_id[0x%x], param_size[0x%x]", apr_rc, cvp_cmd.module_id, cvp_cmd.param_id, cvp_cmd.param_size );
        if( APR_EOK == apr_rc )
        {
          apr_rc = csd_event_wait( psess->csd_vc_event );
        }
      }
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;
    
    case CSD_VC_IOCTL_SET_RX_VOLUME_STEP:
    case CSD_VC_IOCTL_SET_VOLUME_STEP:
    {
      csd_vc_ioctl_set_volume_step_t *csd_vc_vol_step = ( csd_vc_ioctl_set_volume_step_t* )params;
      csd_vc_ioctl_set_rx_volume_step_t *csd_vc_rx_vol_step = (csd_vc_ioctl_set_rx_volume_step_t*)params;
      vss_ivolume_cmd_set_step_t cvp_cmd;
      uint32_t token;

      if( code == CSD_VC_IOCTL_SET_RX_VOLUME_STEP )
      {
        cvp_cmd.direction = VSS_IVOLUME_DIRECTION_RX;
        cvp_cmd.value = csd_vc_rx_vol_step->vol_step;
        cvp_cmd.ramp_duration_ms = csd_vc_rx_vol_step->ramp_duration;
        token = csd_vc_rx_vol_step->cmd_token;
      }
      else
      {
        cvp_cmd.direction = csd_vc_vol_step->direction;
        cvp_cmd.value = csd_vc_vol_step->value;
        cvp_cmd.ramp_duration_ms = csd_vc_vol_step->ramp_duration_ms;
        token = csd_vc_vol_step->cmd_token;
      }
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   token, VSS_IVOLUME_CMD_SET_STEP,
                                   &cvp_cmd, sizeof(vss_ivolume_cmd_set_step_t) );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOLUME_CMD_SET_STEP sent, apr_rc[0x%x], direction[%d], value[%d], ramp_duration_ms[%d]",
               apr_rc, cvp_cmd.direction, cvp_cmd.value, cvp_cmd.ramp_duration_ms );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_SET_RX_VOLUME_INDEX:
    {
      csd_vc_ioctl_set_rx_volume_index_t *csd_vc_rx_vol = ( csd_vc_ioctl_set_rx_volume_index_t* )params;
      vss_ivocproc_cmd_set_rx_volume_index_t cvp_cmd;
      
      cvp_cmd.vol_index = csd_vc_rx_vol->vol_index;
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   csd_vc_rx_vol->cmd_token, VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX,
                                   &cvp_cmd, sizeof(vss_ivocproc_cmd_set_rx_volume_index_t) );
      
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX sent, apr_rc[0x%x], vol_index[%d]", 
               apr_rc, cvp_cmd.vol_index );   
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS:
    {
      csd_vc_ioctl_set_number_of_steps_t *csd_vc_set_num_vol_step = ( csd_vc_ioctl_set_number_of_steps_t* )params;
      vss_ivolume_cmd_set_number_of_steps_t cvp_cmd;
      
      cvp_cmd.value = csd_vc_set_num_vol_step->value;

      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   csd_vc_set_num_vol_step->cmd_token,
                                   VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS,
                                   &cvp_cmd, sizeof(vss_ivolume_cmd_set_number_of_steps_t) );
                                   
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS sent, apr_rc[0x%x], value[%d]",
               apr_rc, cvp_cmd.value );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_SET_MUTE_V2:
    {
      csd_vc_ioctl_set_mute_v2_t *csd_vc_mute = (csd_vc_ioctl_set_mute_v2_t*)params;
      vss_ivolume_cmd_mute_v2_t cvp_cmd;

      cvp_cmd.direction = csd_vc_mute->direction;
      cvp_cmd.mute_flag = csd_vc_mute->mute_flag;
      cvp_cmd.ramp_duration_ms = csd_vc_mute->ramp_duration;
      cvp_cmd.ramp_duration_ms = csd_vc_mute->ramp_duration;

      rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   csd_vc_mute->cmd_token, VSS_IVOLUME_CMD_MUTE_V2,
                                   &cvp_cmd, sizeof(vss_ivolume_cmd_mute_v2_t) );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOLUME_CMD_MUTE_V2 sent, apr_rc[0x%x], direction[%d], mute_flag[%d], ramp_duration_ms[%d]",
               apr_rc, cvp_cmd.direction, cvp_cmd.mute_flag, cvp_cmd.ramp_duration_ms );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_ENABLE: 
    {
      uint32_t* cmd_token = ( uint32_t* )params;
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   *cmd_token, VSS_IVOCPROC_CMD_ENABLE,
                                   NULL, 0 );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOCPROC_CMD_ENABLE sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_DISABLE:
    {
      uint32_t* cmd_token = ( uint32_t* )params;
      csd_vc_cmd_list_item_t* pcmd = NULL;

      /* When VC Disable is called by client, it's usually for a device switch.  Set to default Feature ID for Volume Calibration */
      psess->sess_fid.volume_feature_id = ACDB_VOCVOL_FID_DEFAULT;

      /* Get VP3 data first */      
      ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
      if( pcmd != NULL ) 
      {
        csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;
        pcmd->type = CSD_VC_CMD_RECEIVE_VP3;
        pcmd->psess = psess;

        ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
        /* signal the csd common thread to process the cmd */
        sigs |= CSD_VOC_COMMON_SIG_CMD;
        ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);

        ( void ) csd_event_wait ( psess->csd_vc_event );
      }    
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   *cmd_token, VSS_IVOCPROC_CMD_DISABLE,
                                   NULL, 0 );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOCPROC_CMD_DISABLE sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_SET_DEVICE_CONFIG:
    case CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2:
    {
      csd_vc_ioctl_set_device_config_v2_t *csd_vc_dev_config  = (csd_vc_ioctl_set_device_config_v2_t*)params;
      vss_ivocproc_cmd_set_device_v2_t cvp_cmd;
      csd_voc_common_check_dev_pair_t dev_pair;
      uint32_t ec_ref_topology_id = 0;
      cvp_cmd.rx_port_id       = VSS_IVOCPROC_PORT_ID_NONE;
      cvp_cmd.rx_topology_id   = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
      cvp_cmd.tx_port_id       = VSS_IVOCPROC_PORT_ID_NONE;
      cvp_cmd.tx_topology_id   = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
      cvp_cmd.vocproc_mode     = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
      cvp_cmd.ec_ref_port_id   = VSS_IVOCPROC_PORT_ID_NONE;
      
      /* check if tx & rx devices are valid pair */
      dev_pair.rx_device_id = csd_vc_dev_config->rx_dev_id;
      dev_pair.tx_device_id = csd_vc_dev_config->tx_dev_id;

      rc = csd_voc_common_check_device_pair( &dev_pair );
      if( CSD_EOK == rc )
      {
        /* RX check */
        if( psess->rx_dev_id == csd_vc_dev_config->rx_dev_id )
        {
          cvp_cmd.rx_topology_id = psess->rx_topology_id;
          cvp_cmd.rx_port_id = psess->rx_port_id;
        }
        else
        {        
          rc = csd_voc_commmon_get_device_info( csd_vc_dev_config->rx_dev_id,
                                                (uint32_t*)&cvp_cmd.rx_topology_id,
                                                (uint16_t*)&cvp_cmd.rx_port_id );
          
          if( rc == CSD_EOK )
          {
            rc = csd_voc_commmon_is_valid_sample_rate(csd_vc_dev_config->rx_dev_id,
                                                      csd_vc_dev_config->rx_dev_sr);
            if( CSD_EOK != rc )
            {
              /* Remove pre-fetched rx dev info as sample rate validation failed */
              ( void )csd_voc_commmon_remove_device_info( csd_vc_dev_config->rx_dev_id );
            }
          }
        }
      }
       
      if( rc == CSD_EOK )
      {
        /* TX check */
        if( psess->tx_dev_id == csd_vc_dev_config->tx_dev_id )
        {
          cvp_cmd.tx_topology_id = psess->tx_topology_id;
          cvp_cmd.tx_port_id = psess->tx_port_id;
        }
        else
        {          
          rc = csd_voc_commmon_get_device_info( csd_vc_dev_config->tx_dev_id,
                                                (uint32_t*)&cvp_cmd.tx_topology_id,
                                                (uint16_t*)&cvp_cmd.tx_port_id );
          
          if( rc == CSD_EOK )
          {
            rc = csd_voc_commmon_is_valid_sample_rate( csd_vc_dev_config->tx_dev_id,
                                                       csd_vc_dev_config->tx_dev_sr);
            if( CSD_EOK != rc )
            {
              /* Remove pre-fetched Tx dev info as sample rate validation failed */
              ( void )csd_voc_commmon_remove_device_info( csd_vc_dev_config->tx_dev_id );
              if( psess->rx_dev_id != csd_vc_dev_config->rx_dev_id )
              {
                /* Remove pre-fetched Rx also as set device is failing*/
                ( void )csd_voc_commmon_remove_device_info( csd_vc_dev_config->rx_dev_id );
              }
            }
          }
        }
      }
       
      if( rc == CSD_EOK )
      {
        if( code == CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2 )
        {
          /* EC refernce check */
          if( csd_vc_dev_config->ec_ref_dev_id == CSD_DEV_ID_INVALID )
          {
            cvp_cmd.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
            cvp_cmd.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;
          }
          else if( psess->ec_ref_id == csd_vc_dev_config->ec_ref_dev_id )
          {       
            cvp_cmd.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING;
            cvp_cmd.ec_ref_port_id = psess->ec_port_id;
          }
          else
          {                
            rc = csd_voc_commmon_get_device_info( csd_vc_dev_config->ec_ref_dev_id,
                                                  (uint32_t*)&ec_ref_topology_id,
                                                  (uint16_t*)&cvp_cmd.ec_ref_port_id );
            if( rc != CSD_EOK )
            {
              /* Remove pre-fetched Tx and Rx also as set device is failing*/
              ( void )csd_voc_commmon_remove_device_info( csd_vc_dev_config->tx_dev_id );
              ( void )csd_voc_commmon_remove_device_info( csd_vc_dev_config->rx_dev_id );
            }
            cvp_cmd.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING;
          }
        }
        else
        {
          cvp_cmd.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
          cvp_cmd.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;
        }
      }

      /* Only if eveything is successful, update the session parameter */
      if( rc == CSD_EOK )
      {
        if( psess->rx_dev_id != CSD_DEV_ID_INVALID &&
            psess->rx_dev_id != csd_vc_dev_config->rx_dev_id )
        {
          ( void )csd_voc_commmon_remove_device_info( psess->rx_dev_id );
        }
        
        if( psess->tx_dev_id != CSD_DEV_ID_INVALID &&
            psess->tx_dev_id != csd_vc_dev_config->tx_dev_id )
        {
          ( void )csd_voc_commmon_remove_device_info( psess->tx_dev_id );
        }
        
        if( psess->ec_ref_id != CSD_DEV_ID_INVALID &&
            psess->ec_ref_id != csd_vc_dev_config->ec_ref_dev_id )
        {
          ( void )csd_voc_commmon_remove_device_info( psess->ec_ref_id );
        }

        psess->rx_topology_id = cvp_cmd.rx_topology_id;
        psess->rx_port_id = cvp_cmd.rx_port_id;
        psess->rx_dev_id = csd_vc_dev_config->rx_dev_id;
        psess->rx_dev_sr = csd_vc_dev_config->rx_dev_sr;
      
        psess->tx_topology_id = cvp_cmd.tx_topology_id;
        psess->tx_port_id = cvp_cmd.tx_port_id;
        psess->tx_dev_id = csd_vc_dev_config->tx_dev_id;
        psess->tx_dev_sr = csd_vc_dev_config->tx_dev_sr;
        
        psess->ec_port_id = cvp_cmd.ec_ref_port_id;        
        psess->ec_ref_id = csd_vc_dev_config->ec_ref_dev_id;

        /* Check the Feature id Topology*/
        if( psess->sess_fid.topology_feature_id == (uint32_t)CSD_FID_TOPOLOGY_NONE )
        {
            CSD_MSG( CSD_PRIO_MEDIUM, "csd_vc_ioctl: Topology FID set,Sending VSS_IVOCPROC_TOPOLOGY_ID_NONE for"
                                      "Tx an Rx devices with SET_DEVICE_config command");
           /*Update the Topology to None*/
            cvp_cmd.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
            cvp_cmd.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;

            /*set default fid topology value in the session*/
            psess->sess_fid.topology_feature_id = (uint32_t)CSD_FID_TOPOLOGY_DEFAULT;
        }
      }
      
      
      if( CSD_EOK == rc )
      {
        psess->set_dev_token = csd_vc_dev_config->cmd_token;
        apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vc_src_addr, psess->h_apr_obj,
                                     csd_vc_cvp_dest_addr, psess->h_cvp,
                                     csd_vc_dev_config->cmd_token,
                                     VSS_IVOCPROC_CMD_SET_DEVICE_V2,
                                     &cvp_cmd, sizeof(vss_ivocproc_cmd_set_device_v2_t) );
                                     
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOCPROC_CMD_SET_DEVICE_V2 sent, apr_rc[0x%x] token[0x%x], rx_port_id[0x%x], "
                 "rx_topology_id[0x%x], tx_port_id[0x%x], tx_topology_id[0x%x], ec_ref_port_id[0x%x], vocproc_mode[0x%x]",
                 apr_rc, csd_vc_dev_config->cmd_token, cvp_cmd.rx_port_id, cvp_cmd.rx_topology_id, cvp_cmd.tx_port_id, 
                 cvp_cmd.tx_topology_id, cvp_cmd.ec_ref_port_id, cvp_cmd.vocproc_mode );
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
    }
    break;

    case CSD_VC_IOCTL_SET_TX_DTMF_DETECTION:
    {
      csd_vc_ioctl_set_tx_dtmf_detection_t *csd_vc_tx_dtmf_detect = ( csd_vc_ioctl_set_tx_dtmf_detection_t* )params;
      vss_ivocproc_cmd_set_tx_dtmf_detection_t cvp_cmd;

      cvp_cmd.enable = csd_vc_tx_dtmf_detect->enable;

      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   csd_vc_tx_dtmf_detect->cmd_token,
                                   VSS_IVOCPROC_CMD_SET_TX_DTMF_DETECTION,
                                   &cvp_cmd, sizeof(vss_ivocproc_cmd_set_tx_dtmf_detection_t) );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVOCPROC_CMD_SET_TX_DTMF_DETECTION sent, apr_rc[0x%x], enable[%d]",
               apr_rc, cvp_cmd.enable );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER_V2:
    {
      csd_vc_ioctl_ivpcm_exchange_buffer_v2_t *ivpcm_exchange_buffer = ( csd_vc_ioctl_ivpcm_exchange_buffer_v2_t* )params;
      vss_ivpcm_evt_push_buffer_v2_t cvp_cmd;

      cvp_cmd.tap_point = ivpcm_exchange_buffer->tap_point;
      cvp_cmd.push_buf_mask = ivpcm_exchange_buffer->push_buf_mask;
      cvp_cmd.out_buf_mem_address = ( ( uint64_t )( ivpcm_exchange_buffer->out_buf_mem_address_msw ) << 32 ) |
                                      ( ivpcm_exchange_buffer->out_buf_mem_address_lsw );
      cvp_cmd.out_buf_mem_size = ivpcm_exchange_buffer->out_buf_mem_size;
      cvp_cmd.in_buf_mem_address = ( ( uint64_t )( ivpcm_exchange_buffer->in_buf_mem_address_msw ) << 32 ) |
                                     ( ivpcm_exchange_buffer->in_buf_mem_address_lsw );
      cvp_cmd.in_buf_mem_size = ivpcm_exchange_buffer->in_buf_mem_size;
      cvp_cmd.sampling_rate = ivpcm_exchange_buffer->sampling_rate;
      cvp_cmd.num_in_channels = ivpcm_exchange_buffer->num_in_channels;    

      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   0, VSS_IVPCM_EVT_PUSH_BUFFER_V2,
                                   &cvp_cmd, sizeof(vss_ivpcm_evt_push_buffer_v2_t) );
      
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVPCM_EVT_PUSH_BUFFER_V2 sent to CVD, apr_rc[0x%x] tap_point[0x%x], push_buf_mask[0x%x],"
               "out_buf_mem_address[0x%016llX], out_buf_mem_size[0x%x], in_buf_mem_address[0x%016llX], in_buf_mem_size[0x%x],"
               " sampling_rate[0x%x], num_in_channels[0x%x]",
               apr_rc, cvp_cmd.tap_point, cvp_cmd.push_buf_mask, cvp_cmd.out_buf_mem_address, cvp_cmd.out_buf_mem_size,
               cvp_cmd.in_buf_mem_address, cvp_cmd.in_buf_mem_size, cvp_cmd.sampling_rate, cvp_cmd.num_in_channels );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_IVPCM_CMD_START_V2:
    {
      csd_vc_ioctl_ivpcm_cmd_start_v2_t *ivpcm_cmd_start = ( csd_vc_ioctl_ivpcm_cmd_start_v2_t* )params;
      uint32_t num_tap_points;
      uint32_t tap_point_index;
      uint32_t target_payload_len;
      vss_ivpcm_tap_point_t vss_tap_point;
      aprv2_packet_t *dst_packet = NULL;
      uint8_t *dst_payload;
  
      num_tap_points = ivpcm_cmd_start->num_tap_points;
      target_payload_len = sizeof( uint32_t )*2 + num_tap_points * sizeof(vss_ivpcm_tap_point_t);

      apr_rc = __aprv2_cmd_alloc_ext( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                  csd_vc_src_addr, psess->h_apr_obj,
                                  csd_vc_cvp_dest_addr, psess->h_cvp,
                                  ivpcm_cmd_start->cmd_token, VSS_IVPCM_CMD_START_V2,
                                  target_payload_len, &dst_packet );
      if( dst_packet != NULL )
      {
        uint32_t dst_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(dst_packet->header);
        dst_payload = APRV2_PKT_GET_PAYLOAD( uint8_t, dst_packet );
        /* copy mem handle and num of tap points */
        csd_memscpy ( dst_payload, dst_size,
                      &ivpcm_cmd_start->mem_handle, sizeof( ivpcm_cmd_start->mem_handle) );
        dst_payload += sizeof( ivpcm_cmd_start->mem_handle );
        dst_size -= sizeof( ivpcm_cmd_start->mem_handle );
        csd_memscpy( dst_payload, dst_size,
                     &num_tap_points, sizeof( num_tap_points ) );
        dst_payload += sizeof( num_tap_points );
        dst_size -= sizeof( num_tap_points );
  
        for( tap_point_index = 0; tap_point_index < num_tap_points; tap_point_index++ )
        {
          vss_tap_point.tap_point = ivpcm_cmd_start->tap_points[tap_point_index].tap_point;
          vss_tap_point.direction = ivpcm_cmd_start->tap_points[tap_point_index].direction;
          vss_tap_point.sampling_rate = ivpcm_cmd_start->tap_points[tap_point_index].sampling_rate;
          vss_tap_point.duration = ivpcm_cmd_start->tap_points[tap_point_index].duration;
  
          /* copy each tap point structure to destination payload */
          csd_memscpy( dst_payload, dst_size,
                       &vss_tap_point, sizeof(vss_tap_point) );
          dst_payload += sizeof( vss_tap_point );
          dst_size -= sizeof( num_tap_points);
        }

        apr_rc = __aprv2_cmd_forward( csd_vc_apr_handle, dst_packet );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVPCM_CMD_START_V2 sent, apr_rc[0x%x]", rc );
      }
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_IVPCM_CMD_STOP:
    {
      uint32_t* cmd_token = ( uint32_t* )params;

      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_cvp_dest_addr, psess->h_cvp,
                                   *cmd_token, VSS_IVPCM_CMD_STOP,
                                   NULL, 0 );
                                   
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IVPCM_CMD_STOP sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_CMD_MAP_MEMORY_V2:
    {
      csd_vc_ioctl_cmd_map_memory_v2_t *map_memory_t = ( csd_vc_ioctl_cmd_map_memory_v2_t* )params;
      vss_imemory_cmd_map_physical_t cvp_cmd;
      
      cvp_cmd.table_descriptor.mem_address = ( ( uint64_t )map_memory_t->table_descriptor.mem_address_msw << 32) |
                                               ( map_memory_t->table_descriptor.mem_address_lsw );
      cvp_cmd.table_descriptor.mem_size = map_memory_t->table_descriptor.mem_size;
      cvp_cmd.is_cached = map_memory_t->is_cached;
      cvp_cmd.cache_line_size = map_memory_t->cache_line_size;
      cvp_cmd.access_mask = map_memory_t->access_mask;
      cvp_cmd.page_align = map_memory_t->page_align;
      cvp_cmd.min_data_width = map_memory_t->min_data_width;
      cvp_cmd.max_data_width = map_memory_t->max_data_width;

      psess->memmap_client = CSD_VC_ENUM_CLIENT_EXTERNAL;
      
      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_mvm_dest_addr, APR_NULL_V,
                                   map_memory_t->cmd_token, VSS_IMEMORY_CMD_MAP_PHYSICAL,
                                   &cvp_cmd, sizeof(vss_imemory_cmd_map_physical_t) );
                                   
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IMEMORY_CMD_MAP_PHYSICAL sent, apr_rc[0x%x]", apr_rc );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_CMD_UNMAP_MEMORY_V2:
    {
      csd_vc_ioctl_cmd_unmap_memory_v2_t *unmap_memory_t = ( csd_vc_ioctl_cmd_unmap_memory_v2_t* )params;
      vss_imemory_cmd_unmap_t cvp_cmd;
      
      cvp_cmd.mem_handle = unmap_memory_t->mem_handle;
      psess->memmap_client = CSD_VC_ENUM_CLIENT_EXTERNAL;

      apr_rc = __aprv2_cmd_alloc_send( csd_vc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vc_src_addr, psess->h_apr_obj,
                                   csd_vc_mvm_dest_addr, APR_NULL_V,
                                   unmap_memory_t->cmd_token, VSS_IMEMORY_CMD_UNMAP,
                                   &cvp_cmd, sizeof(vss_imemory_cmd_unmap_t) );
                                   
      CSD_MSG( CSD_PRIO_HIGH, "csd_vc_ioctl: VSS_IMEMORY_CMD_UNMAP sent, apr_rc[0x%x], mem_handle[0x%x]",
               apr_rc, cvp_cmd.mem_handle );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }
    break;

    case CSD_VC_IOCTL_GET_CVP_HANDLE:
    {
      csd_vc_ioctl_get_cvp_handle_t *cvp_handle = (csd_vc_ioctl_get_cvp_handle_t*)params;
      *cvp_handle->ret_cvp_handle = psess->h_cvp;
      /* if no handle exsists return CSD_ENORESOURCE (resource not available */
      if( !*cvp_handle->ret_cvp_handle)
      {
        rc = CSD_ENORESOURCE;
      }
    }
    break;

    case CSD_VC_IOCTL_GET_NUMBER_OF_VOLUME_STEPS: /* GL this needs to be revisited */
    {
      csd_vc_cmd_list_item_t* pcmd = NULL;
      csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;

      /* if volume cal is not done yet return an error */
      if( !psess->sess_cal.dynamic_table.if_registered )
      {
        rc = CSD_ENOTREADY;
      }
      else
      {
        csd_vc_ioctl_get_number_of_steps_t *csd_vc_rx_volume_steps = ( csd_vc_ioctl_get_number_of_steps_t* )params;
        *csd_vc_rx_volume_steps->num_steps = 
          csd_voc_common_get_calibration_size( CSD_VOC_COMMON_GET_OEMDB_VOL_TABLE_STEP_SIZE, NULL );

        ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
        if( pcmd != NULL) 
        {
          pcmd->type = CSD_VC_CMD_SEND_RSP_CALLBACK;
          pcmd->psess = psess;
          pcmd->cmd.rsp_cmd.status_id = CSD_EOK;
          pcmd->client_token = csd_vc_rx_volume_steps->cmd_token;
  
          ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
          /* signal the csd common thread to process the cmd */
          sigs |= CSD_VOC_COMMON_SIG_CMD;
          ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
        }
      }
    }
    break;

    case CSD_VC_IOCTL_STORE_VP3_DATA:
    {
      uint32_t* cmd_token = ( uint32_t* )params;
     
      csd_vc_cmd_list_item_t* pcmd = NULL;
      ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
      if( pcmd != NULL ) 
      {
        csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;
        pcmd->type = CSD_VC_CMD_RECEIVE_VP3;
        pcmd->psess = psess;
        pcmd->client_token = *cmd_token;

        ( void ) apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
        /* signal the csd common thread to process the cmd */
        sigs |= CSD_VOC_COMMON_SIG_CMD;
        ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);

        ( void ) csd_event_wait ( psess->csd_vc_event );
      }    
    }
    break;
    
    case CSD_VC_IOCTL_GET_AFE_PORT:
    {
      csd_vc_ioctl_get_afe_port_t *get_afe_port = (csd_vc_ioctl_get_afe_port_t*)params;
      if( psess->rx_port_id != VSS_IVOCPROC_PORT_ID_NONE &&
          psess->tx_port_id != VSS_IVOCPROC_PORT_ID_NONE )
      {
        *get_afe_port->ret_rx_afe_port = psess->rx_port_id;
        *get_afe_port->ret_tx_afe_port = psess->tx_port_id;
        *get_afe_port->ret_ec_afe_port = psess->ec_port_id;
        
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_vc_ioctl: CSD_VC_IOCTL_GET_AFE_PORT success, rx_port_id[0x%x], tx_port_id[0x%x], ec_port_id[0x%x]",
                 psess->rx_port_id, psess->tx_port_id, psess->ec_port_id );
      }
      else
      {
        rc = CSD_EFAILED;
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: CSD_VC_IOCTL_GET_AFE_PORT failed, rx_port_id[0x%x], tx_port_id[0x%x]",
                 psess->rx_port_id, psess->tx_port_id );
      }
    }
    break;
    
    case CSD_VC_IOCTL_SET_CAL_FID:
    {
      csd_vc_ioctl_set_cal_fid_t *csd_vc_set_cal_fid = (csd_vc_ioctl_set_cal_fid_t*)params;
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: start");
      /* Only Volume Calibration and Topology  Feature ID are supported right now -
          Check parameters to see if we are setting Volume FID and Topology */
      /* only one Feature ID supported at a time*/
      if ( 1 == csd_vc_set_cal_fid->num_fid_in_table)
      {
        switch(csd_vc_set_cal_fid->fid_table[0].cal_table)
        {
          case CSD_VC_IOCTL_ENUM_CAL_VOLUME:
          {
            if (CSD_VC_ENUM_FID_UNINIITALIZED == psess->sess_fid.set_feature_id_state)
            {
              /* If volume calibration is register, re-register */
              if( psess->sess_cal.dynamic_table.if_registered )
              {
                /* Check if the Feature ID exists in ACDB before we do anything */
                int32_t dynamic_table_rc = CSD_EFAILED;
                csd_voc_common_get_oemdb_t cal_arg;

                cal_arg.volume.rx_device_id = psess->rx_dev_id;
                cal_arg.volume.tx_device_id = psess->tx_dev_id;
                cal_arg.volume.feature_id = csd_vc_set_cal_fid->fid_table[0].feature_id;

                if( psess->sess_cal.dynamic_table.size )
                {
                  dynamic_table_rc = csd_voc_common_get_calibration( CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC, &cal_arg,
                                                    (uint8_t *)psess->sess_cal.dynamic_table.p_vir_addr,
                                                    psess->sess_cal.dynamic_table.size, &psess->sess_fid.volume_cal_size_used );
                }

                if( dynamic_table_rc == CSD_EOK )
                {
                  /* When VC Disable is called by client, it's usually for a device switch.
                     Set to default Feature ID for Volume Calibration */
                  csd_vc_cmd_list_item_t* pcmd = NULL;    
                  /* obtain a free command item */
                  ( void ) apr_list_remove_head( &csd_vc_free_cmd_list, (apr_list_node_t**)&pcmd );
                  if( pcmd ) 
                  {
                    csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;
                    pcmd->type = CSD_VC_CMD_SET_CAL_FID;
                    psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_SET_INITIALIZE;
                    psess->sess_fid.volume_feature_id = csd_vc_set_cal_fid->fid_table[0].feature_id;
                    pcmd->psess = psess;
                    pcmd->client_token = csd_vc_set_cal_fid->cmd_token;
                    ( void )apr_list_add_tail( &csd_vc_active_cmd_list, &pcmd->link );
                    /* signal the csd common thread to process the cmd */
                    sigs |= CSD_VOC_COMMON_SIG_CMD;
                    ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
                  }
                }
                else
                {
                  rc = CSD_EBADPARAM;
                  CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: Feature ID[0x%x] does not exist",
                           csd_vc_set_cal_fid->fid_table[0].feature_id );
                }
              }
              else
              {
                csd_vc_event_command_rsp_t event_rsp;
                CSD_MSG( CSD_PRIO_MEDIUM, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: VC not registered, caching Volume feature ID" );
                psess->sess_fid.set_feature_id_state = CSD_VC_ENUM_FID_SET_INITIALIZE;
                psess->sess_fid.volume_feature_id = csd_vc_set_cal_fid->fid_table[0].feature_id;
                
                if( psess->csd_vc_cb )
                {
                  event_rsp.status_id = CSD_EOK;
                  psess->csd_vc_cb ( csd_vc_set_cal_fid->cmd_token, CSD_VC_EVENTID_COMMAND_RSP,
                                  &event_rsp, sizeof(event_rsp), psess->csd_vc_cb_data );
                }
              }
            }
            else
            {
              rc = CSD_ENOTREADY;
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: Not in CSD_VC_ENUM_FID_UNINIITALIZED "
                          "state. psess->sess_fid.set_feature_id_state=[0x%x]", psess->sess_fid.set_feature_id_state );
            }
            break;
          }

          case CSD_VC_IOCTL_ENUM_TOPOLOGY:
          {
            /*Currently support only two values 0- CSD_FID_TOPOLOGY_DEFAULT, 
                     . 1- CSD_FID_TOPOLOGY_NONE*/
            CSD_MSG( CSD_PRIO_MEDIUM, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: CSD_VC_IOCTL_ENUM_TOPOLOGY" );
            if( csd_vc_set_cal_fid->fid_table[0].feature_id > CSD_FID_TOPOLOGY_NONE)
            {
              CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: InValid Topology "
                          " Feature ID value =[0x%x]", csd_vc_set_cal_fid->fid_table[0].feature_id );
              rc = CSD_EBADPARAM;
            }
            else
            {
              csd_vc_event_command_rsp_t event_rsp;
              /*Update the Toplogy feature ID*/
              psess->sess_fid.topology_feature_id = (uint32_t)csd_vc_set_cal_fid->fid_table[0].feature_id;

              if(psess->csd_vc_cb )
              {
                event_rsp.status_id = CSD_EOK;
                psess->csd_vc_cb ( csd_vc_set_cal_fid->cmd_token, CSD_VC_EVENTID_COMMAND_RSP,
                                 &event_rsp, sizeof(event_rsp), psess->csd_vc_cb_data );
              }
            }
            break;
          }
          default:
             rc = CSD_EBADPARAM;
            CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: Invalid Feature ID =[0x%x]", csd_vc_set_cal_fid->fid_table[0].cal_table );
            break;
        }

      }
      else
      {
        rc = CSD_EUNSUPPORTED;
        CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: CSD_VC_IOCTL_SET_CAL_FID: "
                                  "un supported number of feature IDs in the table" );
      }
    }
    break;
    
    default: /* All others not supported for FC1 */
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_vc_ioctl: Unsupported code[0x%x]", code ); 
    break;
  } /* switch code */

  return rc;
}

CSD_INTERNAL int32_t csd_vc_write (
  uint32_t handle,
  void* params,
  uint32_t size
)
{
   return 0;
}

CSD_INTERNAL int32_t csd_vc_read (
  uint32_t handle,
  void* params,
  uint32_t size
)
{
   return 0;
}

/*
 ** End External APIs
*/
