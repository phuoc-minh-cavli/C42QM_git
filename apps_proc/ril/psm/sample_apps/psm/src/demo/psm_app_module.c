/**************************************************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file  psm_app_module.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the Power Saving Mode demo application
 *
 **************************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <float.h>
#include "qapi_socket.h"
#include "qapi_timer.h"
#include "qapi_cli.h"
#include "qflog_utils.h"
#include "psm_app_common.h"

/* DAM thread stack size: 16*1024, can be configured by user */
#define DAM_THREAD_STACK_SIZE 16384
/* DAM thread priority, not configurable */
#define DAM_THREAD_PRIORITY 150
/* Sleep duration in seconds */
#define DAM_SLEEP_DURATION 40
/* Byte pool size */
#define PSM_BYTE_POOL_SIZE 30720*8 

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

void __attribute__((weak)) __stack_chk_fail (void)
{
    return;
}

TX_THREAD *app_thread_handle;
TX_BYTE_POOL *byte_pool_psm;
UCHAR free_memory_psm[PSM_BYTE_POOL_SIZE];

extern int psm_app( void );

/*=============================================================================

FUNCTION psm_sample_app_task_entry

DESCRIPTION
This is the entry point for PSM Sample task

DEPENDENCIES
None.

RETURN VALUE
none

SIDE EFFECTS
None

==============================================================================*/
void psm_sample_app_task_entry(void *thread_ctxt)
{
    LOG_INFO("PSM Sample App task entry \n");
    psm_app();
}

 /**
 @brief This function is the main entry point for the application.
 */
int psm_dam_app_start(void)
{
  int ret;
  CHAR *app_thread_stack_pointer = NULL;

  /* Sleep while device initialization to be done. Especially log
     module need some time to initilize before printing logs*/
  qapi_Timer_Sleep(DAM_SLEEP_DURATION, QAPI_TIMER_UNIT_SEC, true);

  LOG_INFO("psm_dam_app_start entry \n");

  do
  {
    /* Allocate byte_pool_psm (memory heap) */
    ret = txm_module_object_allocate(&byte_pool_psm, sizeof(TX_BYTE_POOL));
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("Allocate byte_pool_psm fail \n");
      break;
    }

    /* Create byte_pool_psm */
    ret = tx_byte_pool_create(byte_pool_psm, "PSM application pool", free_memory_psm, PSM_BYTE_POOL_SIZE);
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("Create byte_pool_psm fail \n");
      break;
    }

    /* Allocate app_thread_handle (App thread) */
    ret = txm_module_object_allocate(&app_thread_handle, sizeof(TX_THREAD));
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("Allocate app_thread_handle fail \n");
      break;
    }
    
    /* Allocate the stack for PSM app_thread_handle */
    tx_byte_allocate(byte_pool_psm, (VOID **)&app_thread_stack_pointer, DAM_THREAD_STACK_SIZE, TX_NO_WAIT);
    
    /* Create app_thread_handle (App thread) */
    ret = tx_thread_create(app_thread_handle, "psm_obj", psm_sample_app_task_entry,
                           0, app_thread_stack_pointer, DAM_THREAD_STACK_SIZE,
                           DAM_THREAD_PRIORITY, DAM_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

    LOG_INFO("tx_thread_create with ret[%d] \n", ret);

    if(ret != TX_SUCCESS)
    {
      if(app_thread_handle)
      {
        txm_module_object_deallocate(app_thread_handle);
      }
      LOG_ERROR("Create app_thread_handle fail \n");
    }
  }while(0);

  LOG_INFO("psm_app_start exit with ret[%d] \n", ret);

  return ret;
}

