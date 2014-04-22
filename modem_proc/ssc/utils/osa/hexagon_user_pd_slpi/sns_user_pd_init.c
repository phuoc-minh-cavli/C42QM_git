/*=============================================================================
  @file sns_user_pd_init.c

  @brief
  PD init for Sensors Root PD.

  Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  INCLUDES
  ===========================================================================*/

#ifndef SSC_TARGET_MDM9205
#include "Diag_LSM.h"
#endif
#ifndef SSC_TARGET_HEXAGON_NO_ISLAND
#include "diag_uimage.h"
#endif
#include "msg_diag_service.h"
#define DBG_MED_PRIO    2

#include "sns_assert.h"
#include "sns_fw_sensor.h"
#include "sns_fw_diag_service.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_timer.h"
#include "sns_types.h"

#ifdef SSC_TARGET_PRAM_AVAILABLE
#include "pram_mgr.h"
#endif /* SSC_TARGET_PRAM_AVAILABLE */


/*=============================================================================
  Forward Declarations
  ===========================================================================*/
extern void sns_fw_init(void);
extern void sns_timer_init(void);
extern void sns_sdc_init(void);
extern void sns_load_sdc_bins(void);

#ifdef SSC_TARGET_MDM9205
extern void vote_for_bimc(void);
#endif

#if defined(SDC_ONLY_IMAGE)
/*=============================================================================
  Static Data
  ===========================================================================*/
static bool sns_init_done = false;

sns_rc sns_user_pd_init()
{
  if(false == sns_init_done)
  {
    sns_sdc_init();
    sns_load_sdc_bins();
    sns_init_done = true;
    return SNS_RC_SUCCESS;
  }
    return SNS_RC_FAILED;
}

#else // full image and not SDC_ONLY_IMAGE

extern bool sns_init_delayed;
extern uint32_t sns_delay_sec;


/*=============================================================================
  Static Data
  ===========================================================================*/
static bool sns_init_done = false;
volatile uint64_t sns_assert_line;
volatile const char* sns_assert_file;

/*=============================================================================
  Public Functions
  ===========================================================================*/

sns_rc sns_user_pd_init()
{
  if(false == sns_init_done)
  {
#ifdef SSC_TARGET_PRAM_AVAILABLE
    // Map in PRAM memory
    sns_pram_addr = pram_acquire_partition(SNS_HEAP_PRAM_NAME, &sns_pram_size);
    // Initialize our Heaps
    sns_heap_init(sns_pram_addr);

    // Adjust the pram_addr and size to take into account the amount used
    // by the heap
    SNS_ASSERT(sns_pram_size > SNS_PRAM_HEAP_SIZE);
    sns_pram_addr = (uint8_t*)sns_pram_addr + SNS_PRAM_HEAP_SIZE;
    sns_pram_size -= SNS_PRAM_HEAP_SIZE;
#else
    sns_pram_addr = NULL;
    sns_pram_size = 0;
    sns_heap_init( NULL );
#endif

/* If enabled, this will delay the framework initialization by 10 seconds.
   This is to easily capture init messages when SSC boots up */
    if(sns_init_delayed)
    {
       const sns_time one_second_in_ticks = 19200000ULL;
       sns_delay_sec = sns_delay_sec > 15 ? 15: sns_delay_sec;
       for(int i = sns_delay_sec; i > 0; i--)
       {
         MSG_1(MSG_SSID_SNS, DBG_MED_PRIO, "init countdown %d ", i);
         /* sns_busy_wait is implemented as a sleep() */
         sns_busy_wait(one_second_in_ticks);
       }
    }

    sns_diag_fw_preinit();

#ifndef SSC_TARGET_MDM9205
    //Init the F3 trace buffer
    void *sns_f3_trace_buffer = sns_malloc(SNS_HEAP_MAIN, SNS_F3_TRACE_SIZE);
    if(NULL != sns_f3_trace_buffer)
    {
      diag_lsm_f3_trace_init( sns_f3_trace_buffer, SNS_F3_TRACE_SIZE );
    }
#else
    vote_for_bimc();
#endif

#ifndef SSC_TARGET_HEXAGON_NO_ISLAND
    // Set diag to exit island mode when island buffer is full
    diag_set_uimage_tx_mode(DIAG_TX_MODE_BUFFERED_THRESH);
#endif

    //Map SDC TCM region
    sns_sdc_init();

    sns_osa_init();

    sns_timer_init();
    sns_fw_init();
    sns_heap_init_delayed();

    sns_init_done = true;
    return SNS_RC_SUCCESS;
  }
  return SNS_RC_FAILED;
}
#endif // full image and not SDC_ONLY_IMAGE

