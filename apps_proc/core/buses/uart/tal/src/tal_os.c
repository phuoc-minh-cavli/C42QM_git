/*==================================================================================================

FILE: tal_os.c

DESCRIPTION: This module defines the reference implementation of the OS services for the
             Target Abstraction Layer.  It uses the DALSYS APIs.

                      Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                                All Rights Reserved.
                 Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.tx/6.0/buses/uart/tal/src/tal_os.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_os_event_create
   tal_os_event_destroy
   tal_os_event_reset
   tal_os_event_set
   tal_os_event_wait
   tal_os_free
   tal_os_get_property
   tal_os_log
   tal_os_malloc
   tal_os_memcpy
   tal_os_phys_alloc
   tal_os_phys_dealloc
   tal_os_phys_map
   tal_os_phys_unmap
   tal_os_sleep_ms
   tal_os_sync_create
   tal_os_sync_destroy
   tal_os_sync_enter
   tal_os_sync_exit

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "DALSys.h"
#include "tal.h"
#include <stdarg.h>
#include "stringl/stringl.h"
#include "uart_device_props.h"
#include "DALDeviceId.h"
#include "timer.h"


/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
//static DALResult tal_os_dummy_attach(const char *arg, DALDEVICEID device_id, DalDeviceHandle **phandle);

/*==================================================================================================
                                       LOCAL/GLOBAL VARIABLES
==================================================================================================*/

static DALDEVICEID device_array[] = { DALDEVICEID_UARTLG_DEVICE_1,
                                      DALDEVICEID_UARTLG_DEVICE_2,
                                      DALDEVICEID_UARTLG_DEVICE_3,
                                      DALDEVICEID_UARTDM_DEVICE_1,
                                      DALDEVICEID_UARTDM_DEVICE_2,
                                      DALDEVICEID_UARTGSBI_DEVICE_0,
                                      DALDEVICEID_UARTGSBI_DEVICE_1,
                                      DALDEVICEID_UARTGSBI_DEVICE_2,
                                      DALDEVICEID_UARTGSBI_DEVICE_3,
                                      DALDEVICEID_UARTGSBI_DEVICE_4,
                                      DALDEVICEID_UARTGSBI_DEVICE_5,
                                      DALDEVICEID_UARTGSBI_DEVICE_6,
                                      DALDEVICEID_UARTGSBI_DEVICE_7,
                                      DALDEVICEID_UARTGSBI_DEVICE_8,
                                      DALDEVICEID_UARTGSBI_DEVICE_9,
                                      DALDEVICEID_UARTGSBI_DEVICE_10,
                                      DALDEVICEID_UARTGSBI_DEVICE_11,
                                      DALDEVICEID_UARTGSBI_DEVICE_12,
                                      DALDEVICEID_UARTBAM_DEVICE_1,
                                      DALDEVICEID_UARTBAM_DEVICE_2,
                                      DALDEVICEID_UARTBAM_DEVICE_3,
                                      DALDEVICEID_UARTBAM_DEVICE_4,
                                      DALDEVICEID_UARTBAM_DEVICE_5,
                                      DALDEVICEID_UARTBAM_DEVICE_6,
                                      DALDEVICEID_UARTBAM_DEVICE_7,
                                      DALDEVICEID_UARTBAM_DEVICE_8,
                                      DALDEVICEID_UARTBAM_DEVICE_9,
                                      DALDEVICEID_UARTBAM_DEVICE_10,
                                      DALDEVICEID_UARTBAM_DEVICE_11,
                                      DALDEVICEID_UARTBAM_DEVICE_12,
                                      DALDEVICEID_UARTCXM,
                                    };

#define NUM_DEVICES (sizeof(device_array)/sizeof(device_array[0]))
#define DM_IDLE_SIG_MASK            0x00000001
#define SYSTEM_TICK_COUNT           19200
// This global variable is exposed to the DAL framework.  It specifies the devices
// whose attach requests should be routed to this driver.

const DALREG_DriverInfo
DALUart_DriverInfo = { NULL, NUM_DEVICES, device_array };

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: allocate_buffers

DESCRIPTION:

==================================================================================================*/
//static DALResult tal_os_dummy_attach(const char *arg, DALDEVICEID device_id, DalDeviceHandle **phandle)
//{
//   return DAL_SUCCESS;
//}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: tal_os_event_create

==================================================================================================*/
void *tal_os_event_create(void)
{
   qurt_signal_t *signal;
   signal = NULL; 

   signal = (qurt_signal_t *)malloc(sizeof(qurt_signal_t));

   if(signal != NULL)
   {
     qurt_signal_init(signal);
   }

   return(signal);
}

/*==================================================================================================

FUNCTION: tal_os_event_destroy

==================================================================================================*/
void tal_os_event_destroy(void *event_handle)
{
   if(event_handle){
   qurt_signal_destroy(event_handle);
   tal_os_free(event_handle);}
}

/*==================================================================================================

FUNCTION: tal_os_event_reset

==================================================================================================*/
void tal_os_event_reset(void *event_handle)
{
  if(event_handle){
   qurt_signal_clear(event_handle,DM_IDLE_SIG_MASK);}
}

/*==================================================================================================

FUNCTION: tal_os_event_set

==================================================================================================*/
void tal_os_event_set(void *event_handle)
{
   if(event_handle){
   qurt_signal_set(event_handle, DM_IDLE_SIG_MASK);}
}

/*==================================================================================================

FUNCTION: tal_os_event_wait

==================================================================================================*/
TAL_RESULT tal_os_event_wait(void *event_handle, uint32 timeout_ms)
{
   uint32 timer_event = 0;   
   int result;

   if(event_handle == NULL)
   {
     return TAL_ERROR;
   }
 
   result= qurt_signal_wait_timed_ext(event_handle,DM_IDLE_SIG_MASK,QURT_SIGNAL_ATTR_WAIT_ALL,
                          &timer_event,timeout_ms*SYSTEM_TICK_COUNT);                          
   
   return (result == QURT_EOK) ? (TAL_SUCCESS) :((result == QURT_EFAILED_TIMEOUT) ? (TAL_TIMEOUT) : (TAL_ERROR));
}

/*==================================================================================================

FUNCTION: tal_os_free

==================================================================================================*/
void tal_os_free(void *ptr)
{
  if(ptr != NULL)
  {
    free(ptr);
    ptr = NULL;
  }
}

/*==================================================================================================

FUNCTION: tal_os_get_property

==================================================================================================*/
TAL_RESULT tal_os_get_property(void *device_id, const char *name, uint32 *value)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetDALPropertyHandle((DALDEVICEID)device_id, handle);
   if (result != DAL_SUCCESS) { return(TAL_ERROR); }

   result = DALSYS_GetPropertyValue(handle, name, 0, &property_variable);
   if (result != DAL_SUCCESS) { return(TAL_ERROR); }

   if (property_variable.dwType != DALSYS_PROP_TYPE_UINT32) { return(TAL_ERROR); }
   if (property_variable.dwLen  != sizeof(uint32))          { return(TAL_ERROR); }

   *value = property_variable.Val.dwVal;
   return(TAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: tal_os_log

==================================================================================================*/
void tal_os_log(void *device_id, TAL_OS_FLAG flag, const char *fmt, ...)
{
   //uint32 dev_id = (uint32)*device_id;
/*
   if (NULL == huartULog){UART_LOG_INIT();}
    
   switch (flag)
   {
      case ERROR:
           strcat(UART_LOG_PREFIX_1,fmt);
           ULogFront_RealTimePrintf(huartULog, 1, fmt, device_id);
         break;
      case WARNING:           
         strcat(UART_LOG_PREFIX_2,fmt); 
         ULogFront_RealTimePrintf(huartULog, 1, fmt, device_id);
         break;
      case INFO:
         strcat(UART_LOG_PREFIX_3,fmt); 
         ULogFront_RealTimePrintf(huartULog, 1, fmt, device_id); 
         break;
      default:      
          strcat(UART_LOG_PREFIX_3,fmt);
          ULogFront_RealTimePrintf(huartULog, 1, fmt, device_id);

         break;
   }*/
     
   
}

/*==================================================================================================

FUNCTION: tal_os_malloc

==================================================================================================*/
void *tal_os_malloc(uint32 size)
{
   void *ptr;
   ptr = NULL;

   ptr = malloc(size);
   return ptr;
}

/*==================================================================================================

FUNCTION: tal_os_memcpy

==================================================================================================*/
void tal_os_memcpy(void *dst, const void *src, uint32 len)
{
   memscpy(dst, len, src, len);
}

/*==================================================================================================

FUNCTION: tal_os_phys_alloc

==================================================================================================*/
TAL_OS_PHYS *tal_os_phys_alloc(uint32 size)
{
   TAL_OS_PHYS *mem_object;
   qurt_mem_region_t mem_region;
   uint32 result;
   qurt_mem_pool_t         pool = qurt_mem_default_pool;
   qurt_mem_region_attr_t pre_alloc_attr;

   qurt_mem_region_attr_init (&pre_alloc_attr);
   
   pre_alloc_attr.physaddr     = 0xFFFFFFFF;
   pre_alloc_attr.virtaddr     = 0xFFFFFFFF;
   pre_alloc_attr.mapping_type = QURT_MEM_MAPPING_VIRTUAL;
   pre_alloc_attr.region_type  = QURT_MEM_REGION_NORMAL;
   pre_alloc_attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
   pre_alloc_attr.permission   = QURT_PERM_FULL;
   pre_alloc_attr.size         = 0;
   
   
   result = qurt_mem_region_create(&mem_region, size, pool, &pre_alloc_attr);
   if (result != QURT_EOK) { return(NULL); }
   

   result = qurt_mem_region_attr_get(mem_region, &pre_alloc_attr);
   if (result != QURT_EOK) { return(NULL); }   

   mem_object = malloc(sizeof(TAL_OS_PHYS));
   if (mem_object == NULL) { return(NULL); }


   mem_object->handle    = mem_region;
   mem_object->phys_addr = (void *)pre_alloc_attr.physaddr;
   mem_object->virt_addr = (void *)pre_alloc_attr.virtaddr;
   mem_object->size      = pre_alloc_attr.size;
   
   return(mem_object);
}

/*==================================================================================================

FUNCTION: tal_os_phys_dealloc

==================================================================================================*/
void tal_os_phys_dealloc(TAL_OS_PHYS *mem_object)
{
   qurt_mem_region_delete(mem_object->handle);
   tal_os_free(mem_object);
}

/*==================================================================================================

FUNCTION: tal_os_phys_map

==================================================================================================*/
TAL_OS_PHYS *tal_os_phys_map(uint32 phys_addr, uint32 size)
{
   TAL_OS_PHYS *mem_object;
   qurt_addr_t v_addr;
   uint32 result;
   
   result = qurt_mem_map_static_query(&v_addr, phys_addr, size, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE);
   if (result != QURT_EOK) { return(NULL); } 

   mem_object = malloc(sizeof(TAL_OS_PHYS));
   if (mem_object == NULL) { return(NULL); }


   mem_object->handle    = NULL;
   mem_object->phys_addr = (void *)phys_addr;
   mem_object->virt_addr = (void *)v_addr;
   mem_object->size      = size;

   return(mem_object);
}



/*==================================================================================================

FUNCTION: tal_os_phys_unmap

==================================================================================================*/
void tal_os_phys_unmap(TAL_OS_PHYS *mem_object)
{
   tal_os_free(mem_object);
}

/*==================================================================================================

FUNCTION: tal_os_sleep_ms

==================================================================================================*/
void tal_os_sleep_ms(uint32 duration)
{
   timer_sleep(duration,T_MSEC, TRUE);	
}   
/*==================================================================================================

FUNCTION: tal_os_sync_create

==================================================================================================*/
void *tal_os_sync_create(void)
{
   qurt_mutex_t *sync_handle;

   sync_handle = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));
   if (sync_handle != NULL)
   {
      qurt_mutex_init(sync_handle);
   }
   return(sync_handle);
}

/*==================================================================================================

FUNCTION: tal_os_sync_destroy

==================================================================================================*/
void tal_os_sync_destroy(void *sync_handle)
{
   if(sync_handle){
   qurt_mutex_destroy(sync_handle);
   tal_os_free(sync_handle);}
   
}

/*==================================================================================================

FUNCTION: tal_os_sync_enter

==================================================================================================*/
void tal_os_sync_enter(void *sync_handle)
{
   if(sync_handle){
   qurt_mutex_lock(sync_handle);}
}

/*==================================================================================================

FUNCTION: tal_os_sync_exit

==================================================================================================*/
void tal_os_sync_exit(void *sync_handle)
{
   if(sync_handle){
   qurt_mutex_unlock(sync_handle);}
}
