/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Compress Task 

General Description
This routines in this file pertain to the task called diag_compress_task
that on the master processor process diag traffic generated on master 
processsor and slave processors.

diag_compress_task is supposed to de-queue the DSM items from the slave 
processors and the master proc and compress the same in a compression buffer.
  
Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diag_compress.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/16   nk      Changes to support ThreadX
05/04/15   rh      Manually undefine watchdog #define in case we want to add
                   a watchdog back to the compression task in the future
05/04/15   rh      Standardized mutex around total_bytes_in_compressed_buffer
04/16/15   sa      Fixed race conditions during mode switching from buffering to streaming 
                   and vice versa.
02/26/14   sa      Created
===========================================================================*/

#include "diag_compress_v.h"
#include "diag_fwd_v.h"
#include "diagi_v.h"
#include "diagcomm_v.h"
#include "diagcomm_smd.h"
#include "dog.h"
#include "assert.h"
#include "diag_cfg.h"

#if defined (DIAG_DOG_SUPPORT)
#undef DIAG_DOG_SUPPORT
#endif

#if defined(DIAG_BUFFERING_SUPPORT) 
  
  static osal_timer_t diag_compress_tout_timer;
  uint32 diag_compress_tx_sleep_threshold = DIAG_COMPRESS_TX_SLEEP_THRESHOLD_DEFAULT;
  osal_timer_cnt_t diag_compress_tx_sleep_time = DIAG_COMPRESS_TX_SLEEP_TIME_DEFAULT;
  uint32 diag_compress_tx_dsm_cnt = 0;   /* Number of DSM items sent for compresion */
  extern volatile diag_tx_mode_type diag_tx_mode; /* From diagcomm_smd.c */
  extern uint32 total_bytes_in_compressed_buffer; /*From diagcompressed_buffer.c */
  extern volatile uint8 diag_tx_mode_internal;
  extern osal_mutex_arg_t diag_compression_mutex;  
  extern uint32 diagcompressedbuf_head;
  uint32 num_dsm_items_to_process = DIAG_NUM_DSM_ITEMS_TO_PROCESS;  /* Number of DSM items to process at a time in compression Mode */


#if defined (DIAG_DOG_SUPPORT)
dog_report_type diag_compress_dog_rpt_var = 0;
uint32 diag_compress_dog_rpt_time_var = 0xffff;

osal_timer_t diag_compress_rpt_timer;
#endif

/*===========================================================================

FUNCTION DIAG_COMPRESS_TASK_START

DESCRIPTION
  This function contains the diagnostic compression task. This task compresses the TX traffic
  from the slave processor and apss processor

  This procedure does not return.

===========================================================================*/
void diag_compress_task_start(uint32 params)
{
  int return_val = 0;
  
  return_val = osal_handshake_start(&diag_compress_task_tcb);
  ASSERT(OSAL_SUCCESS == return_val);

#if defined (DIAG_DOG_SUPPORT)

  diag_compress_dog_rpt_var = dog_register_qurt((osal_thread_t )diag_compress_task_tcb.thd_id, DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);

  diag_compress_dog_rpt_time_var = dog_get_report_period(diag_compress_dog_rpt_var);


  /* If the return value from dog_get_report_period() was zero, then we MUST
  NOT report to dog directly, as DOG will monitor us in a different manner.
  Hence we won't initialize or start any timer. */
  if (diag_compress_dog_rpt_time_var > 0)
  {
    /*  Initialize watchdog report timer  */
    return_val = osal_create_timer(&diag_compress_rpt_timer, &diag_compress_task_tcb, DIAG_COMPRESS_RPT_TIMER_SIG);
    ASSERT(OSAL_SUCCESS == return_val);
  }
#endif

  /* diag_compress_tout_timer is used to suspend the compression task for a defined duration to 
     avoid starving the lower prio task */
  if( diag_compress_tx_sleep_time > 0 )
  {
      return_val = osal_create_diag_timer(&diag_compress_tout_timer, osal_thread_self(), DIAG_COMPRESS_TOUT_TIMER_SIG);
      ASSERT(OSAL_SUCCESS == return_val);
  }
 
   /* Start the dog timer */
  diag_compress_kick_watchdog ();
  diag_compress_handle_sigs();

}


/*===========================================================================

FUNCTION DIAG_COMPRESS_KICK_WATCHDOG

DESCRIPTION
  This function kicks the watchdog for the diagnostic compression task

===========================================================================*/
void diag_compress_kick_watchdog(void)
{
#if defined (DIAG_DOG_SUPPORT)
  int return_val = 0;
  dog_report(diag_compress_dog_rpt_var);
  return_val = osal_set_timer(&diag_compress_rpt_timer, diag_compress_dog_rpt_time_var);
  ASSERT(OSAL_SUCCESS == return_val);
#endif
}


/*===========================================================================

FUNCTION DIAG_COMPRESS_SUSPEND

DESCRIPTION
  This function suspends the diag compression task
  for a specified time interval

===========================================================================*/
void diag_compress_suspend( unsigned int duration )
{
  int return_val = 0;
  osal_sigs_t return_sigs = 0;
  
  if( duration > 0 )
  {
    (void)osal_timed_wait( osal_thread_self(), DIAG_COMPRESS_TOUT_TIMER_SIG, &diag_compress_tout_timer, duration );
    return_val = osal_reset_sigs( osal_thread_self(), DIAG_COMPRESS_TOUT_TIMER_SIG, &return_sigs );
    ASSERT(OSAL_SUCCESS == return_val);
  }
} /* diag_compress_suspend */


void diag_compress_handle_sigs()
{
  osal_sigs_t sigs = 0;
  osal_sigs_t return_sigs = 0;
  uint32 num_dsm_items_compressed = 0;
  uint32 num_dsm_in_rx_queue = 0;
  uint8 tx_mode = 0;

  while (1)
  {
    sigs = osal_thread_wait (&diag_compress_task_tcb, (DIAG_COMPRESS_RPT_TIMER_SIG | DIAG_TX_COMPRESS_SIG ));

    /* Pet the dog before doing any work. */
    diag_compress_kick_watchdog ();
        
    if(sigs & DIAG_TX_COMPRESS_SIG)
    {
    
      (void) osal_reset_sigs (&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs);

      tx_mode = diag_get_tx_mode();
	  
      diag_tx_mode_internal = tx_mode;
      
      if(DIAG_TX_MODE_STREAMING == tx_mode)
      {
         continue;
      }

      /* Signal the drain task if the total bytes in the compressed buffer reach the upper threshold in threshold buffering
           or the drain command is triggered*/
      osal_lock_mutex(&diag_compression_mutex);
      if( (tx_mode == DIAG_TX_MODE_BUFFERED_THRESH ) && (total_bytes_in_compressed_buffer >= diag_tx_mode.buffered_many_mark))
      {
          /*  Signal the drain signal in the fwd task to start draining when it gets scheduled*/
          osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
      }
      osal_unlock_mutex(&diag_compression_mutex);
      /* If flow control is enabled for the SIO TX pool and the compressed buffer is full,
             hold on the compression */
      /* Continue to wait for the compress signal if there is no memory in the compression buffer */
      if((tx_mode == DIAG_TX_MODE_BUFFERED_THRESH) && (diagcomm_compression_flow_enabled() == FALSE ))
      {
        continue;
      }
      
      num_dsm_in_rx_queue = diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1);
      if(num_dsm_in_rx_queue > 0)       
      {
        /* Else set the compress signal */         
          osal_set_sigs(&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs); 
      }       
      /* Process the DSM items coming from the peripherals and Apss local traffic */
      if(diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1))
        num_dsm_items_compressed = diagcomm_smd_process_slave_tx (DIAGCOMM_PORT_1);
     
      diag_compress_tx_dsm_cnt += num_dsm_items_compressed;
     
       /* To ensure lower priority tasks are not starved by DIAG COMPRESS task, force task
                 suspension when # of DSMs drained passes the threshold. When buffered data
                 gets drained, data is drained to USB a lot faster than in pure streaming mode. */
      if( diag_compress_tx_sleep_time > 0 )
      {
        if( diag_compress_tx_dsm_cnt >= diag_compress_tx_sleep_threshold )
        {
          diag_compress_suspend( diag_compress_tx_sleep_time );
          diag_compress_tx_dsm_cnt = 0;
        }
      }
    }    
  } /* END While */
} /* diag_compress_handle_sigs */

#endif
