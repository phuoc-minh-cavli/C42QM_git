/*======================================================================
                        qurt_txm_module_mgr.c 

GENERAL DESCRIPTION
  ThreadX Module Manager initialization routine(s)

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2017-22  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 02/06/2017   SB      Initial Version of ThreadX Module manager 
 *                       initialization routine(s)
 ======================================================================*/
#include "tx_api.h"
#include "txm_module.h"
#include "qurt_memory.h"
#include "qurt_system.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_storage_cfg.h"

// For loading demo module only
#include "fs_public.h"
#include "fs_sys_types.h"
#include "err.h"

#include "qurt_shared_memory.h"
#include "qurt_txm_mmu.h"

#include "qurt_txm_uid.h"

#include "qurt_txm_integrity_check.h"

#include "qurt_txm_exception_handler.h"


/* Needed for Memory load(txm_module_manager_memory_load) only, retaining as an example */
// #include "module_code.h"
// extern unsigned char  module_code[];

// For FS max read/write size allowed
uint32 fs_max_rw_size = 0;

/* Define this module's instance.  */
TXM_MODULE_INSTANCE   my_module;

extern unsigned int getOemPoolSize(void);

#define DEMO_MODULE_FILE                        "/core/file_load"
#define NAND_PARTITION_NAME                     "nand_demo_module"
#define TRUE 									1
#define FALSE 									0
#define	G_SESSION_NUM							2
#define	TXM_MODULE_TIMEOUT_TICKS				1920000

void qurt_txm_module_manager_entry(void *arg);
//extern ULONG _txm_module_manager_user_mode_entry(ULONG request, ULONG param_1, ULONG param_2, ULONG param3);

extern int qurt_txm_module_kernel_stack_init(void);

int disable_file_load = 0;
static int gStorage_type = EFS;
#ifdef TX_ENABLE_MODULE_UNLOAD
int gSession_restart = TRUE;
int gSession_num = G_SESSION_NUM;
#endif

#ifdef TX_ENABLE_DAM_EXCEPTION_CB_TEST
#define EXCEPTION_CB_THREAD_PRIORITY			10
#define EXCEPTION_CB_THREAD_STACK_SIZE			1024

typedef enum exception_cb_method_t{
	METHOD_RESTART_MODULE	    = 2,
	METHOD_GET_ERROR_INFO		= 3,
	METHOD_CREATE_THREAD		= 4,
} exception_cb_method ;

int restart_module_on_exception = FALSE;

qurt_txm_module_exception_info error_info_example;
UINT exception_cb_example(ULONG method, ULONG arg2, ULONG arg3, ULONG arg4);
#endif

#ifdef TX_LAST_RAN_THREAD_INFO_TEST

/**
 * This task periodically reads qurt_txm_last_ran_thread_info(). This is testing purpose only
*/
static void last_ran_thread_info_test_example(void* arg)
{
	TXM_MODULE_LAST_RUN_THREAD last_ran_thread_info_ptr;
	while(1)
    {
        qurt_txm_last_ran_thread_info(&my_module, &last_ran_thread_info_ptr);
        qurt_thread_sleep(TXM_MODULE_TIMEOUT_TICKS);
    }
}
#endif

static void qurt_txm_module_manager_entry_util()
{
	int ret = 0;
	void * file_partition_name = NULL;

#ifdef TX_LAST_RAN_THREAD_INFO_TEST
    qurt_thread_attr_t attr;
    qurt_thread_t thread_id;
#endif
	if(disable_file_load) 
		return;
	
#ifdef TX_ENABLE_DAM_EXCEPTION_CB_TEST
	if(restart_module_on_exception==TRUE)
	{
		/* Clearing the mapping created for the previously loaded user module, since we are reloading the same module. */
        ret = qurt_txm_clear_module_mmu(&my_module);
		
		if(ret != QURT_EOK)
	        return;
		
		memset(&my_module,0,sizeof(my_module));
	}
	restart_module_on_exception = FALSE;
#endif	

	/*setting the storage_type to NAND */
    if(gStorage_type == NAND){
	    qurt_txm_module_set_storage_type(&my_module,NAND);
	    file_partition_name = NAND_PARTITION_NAME;
    }
    else 
	{
	
	    /* Storage Type is taken as EFS by default even if we didn't set this
	       but it is recommended to set the storage_type to EFS */
	    qurt_txm_module_set_storage_type(&my_module,EFS);
	    file_partition_name = DEMO_MODULE_FILE;
    }
	
	if(gStorage_type == NAND)
	{ 
	    /* Check if there is any Demo module present and load it if present */	   
  	    flash_ftl_client_t nand_handle;
	    INT nand_status;
		nand_status = flash_ftl_open(&nand_handle,(uint8*)file_partition_name);
		
		if (nand_status!= FLASH_FTL_OK){
	    /* nand_demo_module partition not found*/
			return;
		}
		 /* found nand_demo_module, closing the handle so that Module manager can 
		use it to load the module */
		flash_ftl_close((flash_ftl_client_t)nand_handle);
    }
    else
	{
	    /* Check if there is any Demo module present and load it if present */
	    int filehandle = efs_open(file_partition_name, O_RDONLY);
	    if(filehandle < 0 )
	    {
		    /* DEMO_MODULE_FILE not found or failed to read. */
		    return;
	    }
	    /* found DEMO_MODULE_FILE, closing the file handle so that Module manager can 
		use it to load the module */
	    efs_close(filehandle);
    }
	
gSession_restart_label:

    /* load the module from DEMO_MODULE_FILE */
    if(!disable_file_load)
    {
        ret = txm_module_manager_file_load(&my_module, "my module", NULL,file_partition_name);
    }
    if (ret != TX_SUCCESS)
        return;
	
#ifdef TX_ENABLE_MODULE_UNLOAD
    if(gSession_restart == TRUE)
        gSession_num--;
#endif
	
	  
    ret = qurt_txm_check_if_MBN_type(&my_module,(void *)file_partition_name, TRUE);
  
    /* integrity check needs to be called only if file loaded is of type MBN */
    if(ret == QURT_EOK)
    {
	    ret = qurt_txm_integrity_check(&my_module);
  
	    if(ret != QURT_EOK)
	    {
		    ret = txm_module_manager_unload(&my_module);
		
		    return;
	    }
    }
	
#ifdef TX_DAM_QC_CUSTOMIZATIONS

    /* setup MMU for this module */
    ret = qurt_txm_setup_module_mmu(&my_module);

    if(ret != QURT_EOK)
	    return;
#endif

	/* Check Unique ID for given Module. 
		Proceed further with Module start, only if API returns SUCCESS i.e. Module has Unique ID.
		DO NOT proceed further if API returns failure because Module does not have Unique ID
		and hence unload the Module.
	*/ 
	ret = qurt_txm_module_uid_check(&my_module);
	
	if(ret != QURT_EOK)
	{
		ret = txm_module_manager_unload(&my_module);
		
		return;
	}
	
#ifdef TX_ENABLE_DAM_EXCEPTION_CB_TEST
	/* Register the exception callback to be invoked if the module crashes. */
	ret = qurt_txm_module_exception_notify_cb_register(&my_module,exception_cb_example,METHOD_GET_ERROR_INFO,0,0,0);
	
	if(ret != QURT_EOK)
	{
		ret = txm_module_manager_unload(&my_module);
		
		return;
	}
#endif
	
    /* Start the module.  */
    ret = txm_module_manager_start(&my_module);
  
    if (ret != TX_SUCCESS)
        return;
	

#ifdef TX_LAST_RAN_THREAD_INFO_TEST
    	qurt_thread_attr_init(&attr);
		qurt_thread_attr_set_name(&attr,"THREAD_INFO_TEST");
		qurt_thread_attr_set_priority(&attr,EXCEPTION_CB_THREAD_PRIORITY);
        qurt_thread_attr_set_stack_size(&attr,EXCEPTION_CB_THREAD_STACK_SIZE);
		qurt_thread_create(&thread_id, &attr, last_ran_thread_info_test_example, NULL);
#endif
	  /* The below code is an example of how the module should be unloaded. It is by default not enabled.
         To enable the unloading, enable the TX_ENABLE_MODULE_UNLOAD in the scons. The module can also be 
	     loaded multiple times based on the value of gSession_restart and gSession_num */
#ifdef TX_ENABLE_MODULE_UNLOAD

    qurt_thread_sleep(TXM_MODULE_TIMEOUT_TICKS);

    ret = txm_module_manager_stop(&my_module);
    if(ret != TX_SUCCESS)
        return;

    qurt_thread_sleep(TXM_MODULE_TIMEOUT_TICKS);
  
    /* Clearing the mapping created for the user module */
    qurt_txm_clear_module_mmu(&my_module);

    ret = txm_module_manager_unload(&my_module);
    if (ret != TX_SUCCESS)
        return;

    /* After Module unload, loop back to Module load step for a few times, to demonstrate the 
       module loading-unloading functionality */
    if((gSession_restart == TRUE)&&(gSession_num!=0))
        goto gSession_restart_label;
#if 0
    // This part of code is disabled becasue, even if the Kernel DAM loader is completed,
    // there can be other DAM loaders, which are still using these resources
    /* 
       Session end for all modules/applications and hence, delete the byte pools and mem regions
       that is created for Module manager earlier 
    */

    /* Deleting the byte pool created for the module object pool */
    ret = tx_byte_pool_delete(&_txm_module_manager_object_pool);
    if (ret != TX_SUCCESS)
        return;

    /* Deleting the byte pool created for the module */
    ret = tx_byte_pool_delete(&_txm_module_manager_byte_pool);
    if (ret != TX_SUCCESS)
        return;
  
    qurt_mem_region_delete(txm_op_region_handle);
    qurt_mem_region_delete(txm_mm_region_handle);
#endif
#endif

	
}

#ifdef TX_ENABLE_DAM_EXCEPTION_CB_TEST
static void exception_cb_entry_example(void* arg)
{
	
	restart_module_on_exception = TRUE;
	
	qurt_txm_module_manager_entry_util();
	
}

UINT exception_cb_example(ULONG method, ULONG arg2, ULONG arg3, ULONG arg4)
{
	qurt_thread_t thread_id;
	qurt_thread_attr_t attr;
	switch(method)
	{
		case METHOD_CREATE_THREAD:
		    /*  
			Operations like restarting the module should not be performed from the 
            current executing thread's context. A new thread should be spawned for this.			
			The priority of the newly spawned thread should be greater than 1.
			*/
		    qurt_thread_attr_init(&attr);
			qurt_thread_attr_set_name(&attr,"EXC_CB_THREAD");
			qurt_thread_attr_set_priority(&attr,EXCEPTION_CB_THREAD_PRIORITY);
			qurt_thread_attr_set_stack_size(&attr,EXCEPTION_CB_THREAD_STACK_SIZE);
		    qurt_thread_create(&thread_id, &attr, exception_cb_entry_example, NULL);
		    break;
		
		case METHOD_GET_ERROR_INFO:
		    /* 
			Simple operations like getting the module's error info, can be performed from the
            current executing thread's context. 
			*/
		    qurt_txm_module_get_exception_info(&my_module,&error_info_example);
			break;
		default:
		    break;
			
	}
	return 0;
	
}
#endif


void qurt_txm_module_manager_entry(void *arg)
{
  void * module_ptr     = NULL;
  void * module_obj_ptr = NULL;
  int ret               = 0;
  qurt_mem_region_attr_t mem_attr;
  qurt_mem_region_t      txm_mm_region_handle;
  qurt_mem_region_t      txm_op_region_handle;
  qurt_mem_pool_t        pool_id;
  qurt_addr_t            module_phys_base = NULL;
  unsigned long			 shMemVa = NULL;
  unsigned long 		 shMemSz = NULL;
  qurt_size_t			 oem_pool_size = 0;
  struct fs_statvfs fs_stats = {0};
  qurt_addr_t region_start = 0;

  if(disable_file_load) 
    return;
  	
  /* Query FS Max Read/Write size */
  if(efs_statvfs("/", &fs_stats))
    return;

  /* Store FS Max Read/Write size, for future use */
  fs_max_rw_size = fs_stats.f_maxwrite;
  
   /* Create mem region for Module manager */
  qurt_mem_region_attr_init(&mem_attr);

  /*query Memory region info based on client supplied region ID */
  qurt_get_mem_region_info(MEM_REGION_ID_OEM_POOL, &region_start, &oem_pool_size);
  
  /* Check if OEM pool is not configured or if the OEM pool size is less than 64K. 
     If the OEM pool size is less than 64K, we do no go forward creating the object pool and
	 module code and data region. */
  if((oem_pool_size < 0x10000) || (region_start == 0))
  {
    return;
  }
  
  /* Check if OEM pool size is less that Object pool size or if the oem pool size is
     not 4K aligned. */
  if((oem_pool_size<=TXM_MODULE_OBJECT_POOL_SIZE)||((oem_pool_size&0xFFF)))
  {
      ERR_FATAL("Invalid OEM pool size: oem_pool_size 0x%x, region start: 0x%x, object pool size: 0x%x ",oem_pool_size,region_start,TXM_MODULE_OBJECT_POOL_SIZE);
  }
  
  mem_attr.permission   = QURT_PERM_FULL;
  mem_attr.physaddr     = region_start;
  mem_attr.virtaddr     = QURT_INVALID_ADDR;
  mem_attr.mapping_type = QURT_MEM_MAPPING_PHYS_CONTIGUOUS;	  
  mem_attr.size  		= (((oem_pool_size - TXM_MODULE_OBJECT_POOL_SIZE) + 0xFFF) & ~(0xFFF));
    
  ret = qurt_mem_pool_attach("OEM_POOL", &pool_id); 
  if (ret != QURT_EOK)
      return;

  ret = qurt_mem_region_create(&txm_mm_region_handle, mem_attr.size, pool_id, &mem_attr);
  if (ret != QURT_EOK)
      return;
  
  ret = qurt_mem_region_attr_get(txm_mm_region_handle, &mem_attr);
  if (ret != QURT_EOK)
    return;

  module_ptr = (void *)mem_attr.virtaddr;

  /* Initialize the module manager */
  ret = txm_module_manager_initialize((VOID *) module_ptr, mem_attr.size /* to be uniform with mem region create size request above */);
  if (ret != TX_SUCCESS)
    return;

  /* Create mem region for Module objects */
  qurt_mem_region_attr_init(&mem_attr);
  mem_attr.permission   = QURT_PERM_READ_WRITE;
  mem_attr.physaddr     = QURT_INVALID_ADDR;
  mem_attr.virtaddr     = QURT_INVALID_ADDR;
  mem_attr.size         = TXM_MODULE_OBJECT_POOL_SIZE;
    
  ret = qurt_mem_region_create(&txm_op_region_handle, mem_attr.size, pool_id, &mem_attr);
  if (ret != QURT_EOK)
    return;
  
  ret = qurt_mem_region_attr_get(txm_op_region_handle, &mem_attr);
  if (ret != QURT_EOK)
    return;

  module_obj_ptr = (void *)mem_attr.virtaddr;
  
  /* Create object memory pool for the Modules */
  ret = txm_module_manager_object_pool_create((VOID *)module_obj_ptr, mem_attr.size);
  if (ret != TX_SUCCESS)
    return;

  /* initialize and allocate QAPI KERNEL STACKS slots 
	these stack slots will be used for processing all module qapi calls in kernel context 
  */
  ret = qurt_txm_module_kernel_stack_init();
  if (ret != TX_SUCCESS)
    return;

#if !DISABLE_KERNEL_MODULE_LOADER
  // for aware application, Kernel DAM loader is disabled
  qurt_txm_module_manager_entry_util();
#endif

  return;
}

