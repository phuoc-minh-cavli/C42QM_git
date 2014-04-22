/*!
  @file
  pal.h

  @brief
  Console functionality for QCLI
*/
/*===========================================================================
  Copyright (c) 2017-2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/


/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>


#include "stringl.h"
#include "qapi_types.h"

#include "pal.h"
#include "qcli.h"
#include "qcli_api.h"
#include "net_demo.h"
#include "dss_netapp.h"
#include "atfwd_cli_app.h"
#include "version_cli_app.h"

#ifdef FEATURE_QCLI_SOURCE_SHIP
#include "psm_demo.h"
#include "core_demo.h"
#include "mobileap_app.h"
#include "net_debug_cmd.h"
#endif

#include "qurt_error.h"
#include "qurt_thread.h"

#include "qapi.h"
#include "qapi_status.h"
#include "dsm_init.h"
#include "dsm_pool.h"

#include "fs_public.h"
#include "tx_api.h"
#include "txm_module.h"
#include "qurt_system.h"
#include "qurt_shared_memory.h"

#include "qapi_cli.h"
#include "qurt_txm_qapi_fwk.h"

#include "multi_app_test.h"
#include "qurt_txm_integrity_check.h"

cli_ctx_t app_cli_ctx;

extern qurt_txm_module_manager_setup_module_mmu(TXM_MODULE_INSTANCE *module);

extern int qurt_mapping_create_with_asid(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size,
                         qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm, uint32 asid);

#ifdef LWM2M_ENABLE
extern void Initialize_LwM2M_Demo(void);

extern void Initialize_LWM2M_Stub_Demo_v2(void);

extern void Initialize_LWM2M_Ext_Demo_v2(void);

#endif

extern void loc_qcli_iface_init_ex(void);

#ifdef FEATURE_QCA4004
extern void qca4004_driver_qcli_iface_init(void); 
#endif

extern sio_stream_id_type   console_handle ;


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/


/* The following is a simple macro to facilitate printing strings directly
   to the console. As it uses the sizeof operator on the size of the string
   provided, it is intended to be used with string literals and will not
   work correctly with pointers.
*/
#define PAL_CONSOLE_WRITE_STRING_LITERAL(__String__)    do { PAL_Console_Write(sizeof(__String__) - 1, (__String__)); } while(0)

#define DSM_QCLI_SIO_TX_ITEM_POOL ((dsm_mempool_id_type)(&dsm_qcli_sio_tx_item_pool))

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/



/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

static PAL_Context_t PAL_Context;
qurt_signal_t         Start_Thread_Event;


uint8 *dsm_qcli_sio_tx_item_array;
dsm_pool_mgmt_table_type     dsm_qcli_sio_tx_item_pool;

/* Define this module's instance.  */
TXM_MODULE_INSTANCE   qcli_dam_module;
TXM_MODULE_INSTANCE   qcli_dam_lwm2m_module;

bool thrd_running[NO_OF_APPS] = {false};


#define QCLI_DAM_DEMO_MODULE_FILE                        "/datatx/qcli_dam_demo.bin"
#define QCLI_DAM_LWM2M_DEMO_MODULE_FILE                  "/datatx/qcli_dam_lwm2m_demo.bin"


/*-------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

static void Initialize_Samples(void);
static void cli_rx_cb( dsm_item_type ** item_type);
static void QCLI_Thread(void *Param);
static qbool_t PAL_Initialize(void);
void Initialize_Net_Demo(void);
static int spawn_thread(void);

/*-------------------------------------------------------------------------
 * Function Definitions
 *-----------------------------------------------------------------------*/

/**
   @brief This function is responsible for initializing the sample
          applications.
*/
static void Initialize_Samples(void)
{
#ifdef LWM2M_ENABLE
   int fd = 0;
#endif
   Initialize_Net_Demo();
   Initialize_Dss_Netapp_Demo(); 
#ifdef QTI_BRIDGE_ENABLE
#ifndef MINI_QCLI
   Initialize_Atfwd_Cli_App_Demo();
#endif
#endif
#ifdef FEATURE_QCLI_SOURCE_SHIP
   Initialize_CORE_Demo();
#ifndef MINI_QCLI
   Initialize_PSM_Demo();
#ifdef ECM_ENABLE
   Initialize_MobileAP_Demo();
#endif
   Initialize_NetDebug_Demo();
#endif
#endif
#ifdef LWM2M_ENABLE
   fd = efs_open("/datatx/lwm2m_app_autostart", O_RDONLY);
   if(fd > -1)
   {
#ifndef MINI_QCLI
     Initialize_LwM2M_Demo();
     efs_close(fd);
#endif
   }
#endif
#ifndef MINI_QCLI
   loc_qcli_iface_init_ex();
#endif
#ifdef LWM2M_ENABLE
   fd = efs_open("/datatx/lwm2m_app_autostart", O_RDONLY);
   if(fd > -1)
   {
#ifndef MINI_QCLI
     Initialize_LWM2M_Stub_Demo_v2();
     Initialize_LWM2M_Ext_Demo_v2();
     efs_close(fd);
#endif
   }
#endif

#ifndef MINI_QCLI
   Initialize_Version_Cli_App_Demo();
#endif
#ifdef FEATURE_QCA4004
   qca4004_driver_qcli_iface_init();
#endif
}

/**
   @brief This function handles receive callbacks from the SIO.

   @param Num_Bytes[in] is the number of bytes received.
   @param CB_Data[in]   is the application defined callback data.  In this case
                        it is the index of the buffer received on.
*/
static void cli_rx_cb( dsm_item_type **item_type)
{

  uint32 Num_Bytes = 0;
  uint8_t buf[5];

  if(PAL_Context.Rx_Buffers_Free != 0)
  {
     Num_Bytes = dsm_extract(*item_type, 0,&PAL_Context.Rx_Buffer[PAL_Context.Rx_In_Index],PAL_RECIEVE_BUFFER_SIZE);
     dsm_free_packet(item_type);

          /* Note the amount of data that was read. */
          PAL_Context.Rx_Buffer_Length[PAL_Context.Rx_In_Index] = Num_Bytes;

          /***********Check for Ctrl+C  ****************/
          if(*PAL_Context.Rx_Buffer[0] == 3)
          {
            abort_handler();
          }

          PAL_Context.Rx_In_Index ++;
          if(PAL_Context.Rx_In_Index == PAL_RECIEVE_BUFFER_COUNT)
          {
             PAL_Context.Rx_In_Index = 0;
          }

          PAL_ENTER_CRITICAL();
          PAL_Context.Rx_Buffers_Free --;
          PAL_EXIT_CRITICAL();
          /* Signal the event that data was received. */
          qurt_signal_set(&(PAL_Context.Rx_Event), PAL_RECEIVE_EVENT_MASK);

    }
  else
  {
     memset(buf, '\0', sizeof(buf));
     dsm_extract(*item_type, 0,&buf,5);
     dsm_free_packet(item_type);

/***********Check for Ctrl+C  ****************/
     if(buf[0]==3)
     {
         abort_handler();
     }
  }
}
/**
   @brief This function represents the main thread of execution.

   It will finish initialization of the sample and then function as a
   receive thread for the console.
*/
static void QCLI_Thread(void *Param)
{
   uint32_t CurrentIndex;

   /* Initiailze the CLI. */
   if(QCLI_Initialize())
   {
      /* Initialize the samples. */
      Initialize_Samples();

      /* Display the initialize command list. */
      QCLI_Display_Command_List();

      spawn_thread();

      /* Create a receive event. */
      qurt_signal_init(&(PAL_Context.Rx_Event));
      qurt_signal_init(&(Start_Thread_Event));

      /* Loop waiting for received data. */
      while(true)
      {
         /* Wait for data to be received. */
         while(PAL_Context.Rx_Buffers_Free == PAL_RECIEVE_BUFFER_COUNT)
         {
            qurt_signal_wait(&(PAL_Context.Rx_Event), PAL_RECEIVE_EVENT_MASK, QURT_SIGNAL_ATTR_WAIT_ANY);
            qurt_signal_clear(&(PAL_Context.Rx_Event), PAL_RECEIVE_EVENT_MASK);
         }

         CurrentIndex = (uint32_t)(PAL_Context.Rx_Out_Index);

         /* Send the next buffer's data to QCLI for processing. */
         QCLI_Process_Input_Data(PAL_Context.Rx_Buffer_Length[CurrentIndex], PAL_Context.Rx_Buffer[CurrentIndex]);

         /* Adjust the indexes for the received data. */
         PAL_Context.Rx_Out_Index ++;
         if(PAL_Context.Rx_Out_Index == PAL_RECIEVE_BUFFER_COUNT)
         {
            PAL_Context.Rx_Out_Index = 0;
         }

         PAL_ENTER_CRITICAL();
         PAL_Context.Rx_Buffers_Free ++;
         PAL_EXIT_CRITICAL();

         /* Re-queue the buffer with the UART driver. */
         //uart_receive(PAL_Context.Console_UART, (char *)(PAL_Context.Rx_Buffer[CurrentIndex]), PAL_RECIEVE_BUFFER_SIZE, (void *)CurrentIndex);
      }
   }
   else
   {
      PAL_CONSOLE_WRITE_STRING_LITERAL("QCLI initialization failed");
      PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
   }

}

/**
   @brief This function is used to initialize the Platform, predominately
          the console port.

   @return
    - true if the platform was initialized successfully.
    - false if initialization failed.
*/
static qbool_t PAL_Initialize(void)
{
    sio_open_type open_properties;
   sio_open_param_type open_param;
   int filehandle;
   uint8 val = 0;
   qurt_addr_t module_phys_base = NULL;
   struct fs_stat lwm2m_config_fileStat;
   int read_bytes = 0;
   char* read_buf = NULL;

   memset(&PAL_Context, 0, sizeof(PAL_Context));
   PAL_Context.Rx_Buffers_Free = PAL_RECIEVE_BUFFER_COUNT;

   memset (&open_properties, 0, sizeof (open_properties));
   memset (&open_param, 0, sizeof (open_param));
   open_param.dsm_pool_param.dsm_mempool_id  = DSM_QCLI_SIO_TX_ITEM_POOL;
   
   //check whether the file exist and size>0
   if((efs_stat("/datatx/qcli_config", &lwm2m_config_fileStat) < 0) || 
  	  (lwm2m_config_fileStat.st_size == 0))
   {
     LOG_ERROR("LWM2M_LOG: access /datatx/qcli_config failure");
	 return 0;
   }

   read_buf = malloc(lwm2m_config_fileStat.st_size + 1);
   if(read_buf == NULL)
   {
     LOG_ERROR("LWM2M_LOG:  malloc memory failure for read config file!!");
	 return 0;
   }

  filehandle = efs_open( "/datatx/qcli_config", O_RDONLY );

  if(filehandle < 0 )
  {
    free(read_buf);
    return 0;
  }
  else
  {
    memset (read_buf , 0 , lwm2m_config_fileStat.st_size) ;
	read_bytes = efs_read(filehandle, read_buf, lwm2m_config_fileStat.st_size);
  }
  read_buf[read_bytes] = '\0';
  val = read_buf[0];
  
  /* ASCII value of '3' */
	/* Load QCLI DAM if qcli_config has value '3'*/
	if(val == 0x33)
	{
  
	  int ret  = 0;
	  int filehandle = 0xFFFFFFFF;
  
	  /* Check if there is any QCLI DAM LWM2M Demo module present and load it if present */
	  filehandle = efs_open(QCLI_DAM_LWM2M_DEMO_MODULE_FILE, O_RDONLY);
  
	  if(filehandle < 0 )
	  {
		/* QCLI_DAM_LWM2M_DEMO_MODULE_FILE not found or failed to read. */
		LOG_ERROR("pal.c : LWM2M FILE NOT FOUND");
		goto Next;
	  }
  
	  /* found QCLI_DAM_LWM2M_DEMO_MODULE_FILE, closing the file handle so that Module manager can
		 use it to load the module */
	  efs_close(filehandle);
  
  
	  /* load the module from LWM2M_DEMO_MODULE_FILE */
	  ret = txm_module_manager_file_load(&qcli_dam_lwm2m_module, "QCLI module", NULL, QCLI_DAM_LWM2M_DEMO_MODULE_FILE);
  
	  if (ret != TX_SUCCESS)
	  {
	      LOG_ERROR("pal.c : LWM2M FILE NOT loaded");
		  goto Next;
	  }
  
	/* check if the image loaded is of MBN format */
	  ret = qurt_txm_check_if_MBN_type (&qcli_dam_lwm2m_module, (void *)QCLI_DAM_LWM2M_DEMO_MODULE_FILE, TRUE);
	  
	  /* integrity check needs to be called only if file loaded is of type MBN */
	  if(ret == QURT_EOK)
	  {
		ret = qurt_txm_integrity_check(&qcli_dam_lwm2m_module);
	  
		if(ret != QURT_EOK)
		{
		  LOG_ERROR("oem_app_load : Module name %s file Integrity failed \n",QCLI_DAM_LWM2M_DEMO_MODULE_FILE);
		  ret = txm_module_manager_unload(&qcli_dam_lwm2m_module);
		  goto Next;
		}
	  }
  
  
#ifdef TX_DAM_QC_CUSTOMIZATIONS
  
	  ret = qurt_txm_module_manager_setup_module_mmu(&qcli_dam_lwm2m_module);
	  if(ret != QURT_EOK)
		 goto Next;
	  {
		 qurt_size_t size = qcli_dam_lwm2m_module.txm_module_instance_total_ram_usage;
		 /* Align to page size of 4KB */
		 size = (size + 0xfff) & ~0xfff;
  
		 module_phys_base = qurt_lookup_physaddr((qurt_addr_t)qcli_dam_lwm2m_module.txm_module_instance_phys_base);
  
		 ret = qurt_mapping_create_with_asid( (qurt_addr_t)qcli_dam_lwm2m_module.txm_module_instance_virt_base,
									module_phys_base,
									  size, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL,qcli_dam_lwm2m_module.txm_module_instance_asid);
  
		 if(ret != QURT_EOK)
			goto Next;
  
  
		 ret = qurt_mapping_create( (qurt_addr_t)qcli_dam_lwm2m_module.txm_module_instance_virt_base,
									module_phys_base,
									  size, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL);
  
		 if(ret != QURT_EOK)
			goto Next;
  
	  }
#endif
  
	  /* Start the module.	*/
	  ret = txm_module_manager_start(&qcli_dam_lwm2m_module);
	  if (ret != TX_SUCCESS)
		 goto Next;
	  
	}

  /* ASCII value of '2' */
  /* Load QCLI DAM if qcli_config has value '2'*/
  if(val == 0x32)
  {

    int ret  = 0;
    int filehandle = 0xFFFFFFFF;

    /* Check if there is any QCLI DAM Demo module present and load it if present */
    filehandle = efs_open(QCLI_DAM_DEMO_MODULE_FILE, O_RDONLY);

    if(filehandle < 0 )
    {
      /* QCLI_DAM_DEMO_MODULE_FILE not found or failed to read. */
      goto Next;
    }

    /* found QCLI_DAM_DEMO_MODULE_FILE, closing the file handle so that Module manager can
       use it to load the module */
    efs_close(filehandle);


    /* load the module from DEMO_MODULE_FILE */
    ret = txm_module_manager_file_load(&qcli_dam_module, "QCLI module", NULL, QCLI_DAM_DEMO_MODULE_FILE);

    if (ret != TX_SUCCESS)
		goto Next;

  /* check if the image loaded is of MBN format */
    ret = qurt_txm_check_if_MBN_type (&qcli_dam_module, (void *)QCLI_DAM_DEMO_MODULE_FILE, TRUE);
    
    /* integrity check needs to be called only if file loaded is of type MBN */
    if(ret == QURT_EOK)
    {
      ret = qurt_txm_integrity_check(&qcli_dam_module);
    
      if(ret != QURT_EOK)
      {
        LOG_ERROR("oem_app_load : Module name %s file Integrity failed \n",QCLI_DAM_DEMO_MODULE_FILE);
        ret = txm_module_manager_unload(&qcli_dam_module);
        goto Next;
      }
    }


#ifdef TX_DAM_QC_CUSTOMIZATIONS

    ret = qurt_txm_module_manager_setup_module_mmu(&qcli_dam_module);
    if(ret != QURT_EOK)
       goto Next;
    {
       qurt_size_t size = qcli_dam_module.txm_module_instance_total_ram_usage;
       /* Align to page size of 4KB */
       size = (size + 0xfff) & ~0xfff;

       module_phys_base = qurt_lookup_physaddr((qurt_addr_t)qcli_dam_module.txm_module_instance_phys_base);

       ret = qurt_mapping_create_with_asid( (qurt_addr_t)qcli_dam_module.txm_module_instance_virt_base,
                                  module_phys_base,
                                    size, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL,qcli_dam_module.txm_module_instance_asid);

       if(ret != QURT_EOK)
          goto Next;


       ret = qurt_mapping_create( (qurt_addr_t)qcli_dam_module.txm_module_instance_virt_base,
                                  module_phys_base,
                                    size, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL);

       if(ret != QURT_EOK)
          goto Next;

    }
#endif

    /* Start the module.  */
    ret = txm_module_manager_start(&qcli_dam_module);
    if (ret != TX_SUCCESS)
       goto Next;
  }

  
  
  /* *************************************************************************
   
     qcli_config_value and port_id mapping:      
     -------------------------------------------------------------
	 | qcli_config_value  | Port_id             |  Mode           | 
     |	                  |                     |                 |
      -------------------------------------------------------------	 
	 |  1 or, 1;UART      | SIO_PORT_UART_AUX   | QClI_DEMO       |
	 |                    |                     |                 |
	  -------------------------------------------------------------
	 |  1;USB_SER3        | SIO_PORT_USB_SER3   | QClI_DEMO       |
	 |                    |                     |                 |
	  -------------------------------------------------------------
	 |  1;USB_SER5        | SIO_PORT_USB_SER5   | QClI_DEMO       |
	 |                    |                     |                 |
	  -------------------------------------------------------------
	 |  2                 | SIO_PORT_UART_AUX   | QClI_DAM_DEMO   |
	 |                    |                     |                 |
	  -------------------------------------------------------------
	  
	 
  ********************************************************************************/
  
 Next: 
  /* ASCII value of '1'*/
  if(val != 0x31)
  {
    free(read_buf);
    efs_close(filehandle);
    return 0;
  }

  /* At NW Stack Boot up Console Printf
     opens the UART Port ,IF QCLI is also enabled
     close the existing port and use the UART 
     port opened by QCLI to transmit Console Printf
     Data to the Console
  */
  if(console_handle != SIO_NO_STREAM_ID)
  {
    sio_close(console_handle,NULL);
    console_handle = SIO_NO_STREAM_ID;
  }
  
  // Initialize watermark queue for SIO
   dsm_queue_init (&PAL_Context.tx_wm_queue,
                   DSM_QCLI_SIO_TX_ITEM_ARRAY_SIZ,
                   &PAL_Context.tx_q);
   dsm_set_low_wm (&PAL_Context.tx_wm_queue,
                   DSM_QCLI_SIO_TX_ITEM_ARRAY_SIZ);
   dsm_set_hi_wm (&PAL_Context.tx_wm_queue,
                  DSM_QCLI_SIO_TX_ITEM_ARRAY_SIZ);

   /* DSM Memory Pool Initialization...*/
  dsm_qcli_sio_tx_item_array = malloc(DSM_QCLI_SIO_TX_ITEM_ARRAY_SIZ);
  dsm_init_pool(DSM_QCLI_SIO_TX_ITEM_POOL,
  (uint8*)dsm_qcli_sio_tx_item_array,
  DSM_QCLI_SIO_TX_ITEM_ARRAY_SIZ,
  DSM_QCLI_SIO_TX_ITEM_SIZ);

   open_properties.stream_mode  = SIO_GENERIC_MODE;
   open_properties.stream_type  = SIO_STREAM_RXTX;
   open_properties.rx_bitrate   = SIO_BITRATE_115200;
   open_properties.tx_bitrate   = SIO_BITRATE_115200;
#ifdef CHILTEPIN_UART_PORT
   open_properties.port_id   = SIO_PORT_UART_MAIN;
#else
   open_properties.port_id   = SIO_PORT_UART_AUX;
#endif
   open_properties.rx_func_ptr   = cli_rx_cb;
   open_properties.tx_queue   = NULL;
   open_properties.rx_queue   = NULL;
   open_properties.tx_queue    = &PAL_Context.tx_wm_queue;
   open_properties.open_param    = &open_param;
   
   if(read_buf[1] == ';')
   {
     char sec_param[9];
	 int i = 0, j = 0;
	 for (i = 2 ; i <= 9 ; i++)
	 { 
	    sec_param[j++] = read_buf[i];
	 }
	 sec_param[j] = '\0';
	 if (!strcmp("USB_SER3", sec_param))
     {
	    open_properties.port_id   = SIO_PORT_USB_SER3;
     }
	 if (!strcmp("USB_SER5", sec_param))
     {
	    open_properties.port_id   = SIO_PORT_USB_SER5;
     }
   }
   
    PAL_Context.stream_id = sio_open(&open_properties);

    /* Updating the Console Printf Uart Handle
       with the one opened by QCLI
    */
    console_handle =  PAL_Context.stream_id;

   if (PAL_Context.stream_id == SIO_NO_STREAM_ID)
   {
     free(read_buf);
	 efs_close(filehandle);
     return 0;
   }

   efs_close(filehandle);
   free(read_buf);

   return 1;  
}

/**
   @brief This function spawns new threads apart from the QCLI Thread.
*/
static int spawn_thread(void)
{
  qurt_thread_attr_t  Thread_Attribte[NO_OF_APPS];
  qurt_thread_t  Thread_Handle[NO_OF_APPS];
  int   Thread_Result;

  qurt_thread_attr_init(&Thread_Attribte[0]);
  qurt_thread_attr_set_name(&Thread_Attribte[0], "Multi Thread 0");
  qurt_thread_attr_set_stack_size(&Thread_Attribte[0], 16384);
#ifdef FEATURE_9x07_CAT4
  qurt_thread_attr_set_priority(&Thread_Attribte[0], 148);
#else
  qurt_thread_attr_set_priority(&Thread_Attribte[0], 150);
#endif

  Thread_Result = qurt_thread_create(&Thread_Handle[0], &Thread_Attribte[0], multi_app_task_entry_t1, NULL);

  if(Thread_Result != QURT_EOK)
  {
     PAL_CONSOLE_WRITE_STRING_LITERAL("Failed to Start App 0");
     PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
     PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
  }
  else
  {
    PAL_CONSOLE_WRITE_STRING_LITERAL("Successfully Started App 0");
    PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
    PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
    thrd_running[0] = true;
  }

  qurt_thread_attr_init(&Thread_Attribte[1]);
  qurt_thread_attr_set_name(&Thread_Attribte[1], "Multi Thread 1");
  #ifdef FEATURE_9x07_CAT4
  qurt_thread_attr_set_priority(&Thread_Attribte[1], 150);
  #else
  qurt_thread_attr_set_priority(&Thread_Attribte[1], 148);
  #endif
  qurt_thread_attr_set_stack_size(&Thread_Attribte[1], 16384);

  Thread_Result = qurt_thread_create(&Thread_Handle[1], &Thread_Attribte[1], multi_app_task_entry_t2, NULL);

  if(Thread_Result != QURT_EOK)
  {
     PAL_CONSOLE_WRITE_STRING_LITERAL("Failed to Start App 1");
     PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
     PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
  }
  else
  {
    PAL_CONSOLE_WRITE_STRING_LITERAL("Successfully Started App 1");
    PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
    PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
    thrd_running[1] = true;
  }

  return Thread_Result;
}


/**
   @brief This function is the main entry point for the application.
*/
int qcli_app_start(void)
{
   qurt_thread_attr_t Thread_Attribte;
   qurt_thread_t      Thread_Handle;
   int                Result = PAL_Initialize();

   /* Initialize the platform.                                             */
   if(Result)
   {
      /* Start the main demo thread. */
      qurt_thread_attr_init(&Thread_Attribte);
      qurt_thread_attr_set_name(&Thread_Attribte, "QCLI Thread");
      qurt_thread_attr_set_priority(&Thread_Attribte, 152);
      qurt_thread_attr_set_stack_size(&Thread_Attribte, 8192);
      Result = qurt_thread_create(&Thread_Handle, &Thread_Attribte, QCLI_Thread, NULL);
      if(Result != QURT_EOK)
      {
         PAL_CONSOLE_WRITE_STRING_LITERAL("Failed to start QCLI thread.");
         PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
         PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      }
      else
      {
        PAL_CONSOLE_WRITE_STRING_LITERAL("Successfully Started QCLI Thread");
        PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
        PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      }
   }

   /* Register the CLI app handler with kernel */  
   qurt_qapi_register(TXM_QAPI_CLI_BASE + 1, TXM_QAPI_CLI_NUM_IDS, qapi_CLI_Handler);

   memset(&app_cli_ctx, 0x00, sizeof(cli_ctx_t));

   qurt_mutex_init(&app_cli_ctx.cli_app_global_mutex);
   return(QURT_EOK);
}

/**
   @brief This function is used to write a buffer to the console. Note
          that when this function returns, all data from the buffer will
          be written to the console or buffered locally.

   @param Length is the length of the data to be written.
   @param Buffer is a pointer to the buffer to be written to the console.
*/
void PAL_Console_Write(uint32_t Length, const uint8_t *Buffer)
{
   dsm_item_type *item;
   int dsm_item_size = DSM_POOL_ITEM_SIZE(DSM_QCLI_SIO_TX_ITEM_POOL);

   if((Length) && (Buffer))
   {
//     qurt_thread_sleep_ext(1);
     item = dsm_new_buffer (DSM_QCLI_SIO_TX_ITEM_POOL);


     if(item != NULL)
     {
        item->used = Length < dsm_item_size ? Length : dsm_item_size;
        strlcpy((char*)item->data_ptr, (char*)Buffer, item->used+1);
        sio_transmit(PAL_Context.stream_id,item);

     }
   }
}

/**
   @brief This function is indicates to the PAL layer that the application
          should exit. For embedded applications this is typically a reset.
*/
void PAL_Exit(void)
{
   PAL_CONSOLE_WRITE_STRING_LITERAL("Resetting...");
   PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

   /* Wait for the transmit buffers to flush.                           */
  // qurt_thread_sleep_ext(10);

   /* Reset the platform.                                               */
//xxx
   PAL_CONSOLE_WRITE_STRING_LITERAL("Not Implemented.");
   PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
   PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
}
