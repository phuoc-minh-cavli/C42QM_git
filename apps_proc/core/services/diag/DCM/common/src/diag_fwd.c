/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Fwd Task

General Description
This routines in this file pertain to the task called diag_fwd_task
that on the master processor process traffic from slave processors.
This is a more robust design and will help diag_task be more efficient.

Copyright (c) 2010-2016, 2019-2020 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diag_fwd.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
02/01/16   is      Do not read or send data on ADSP channel since ADSP is not available
11/06/15   as      Removed DIAG_CONSUMER_API ifdefs
08/25/15   nk      Defined diag_fwd_task variables in diag_fwd.c and extern them in other files.
07/02/15   is      Check connection status before draining slave data
06/23/15   sa      Access head of the compression buffer within a mutex
05/21/15   rh      Detect completion of drain when buffer is exactly full.
05/13/15   rh      No longer set compressed drain sig when flow controlled
05/04/15   rh      Standardized mutex around total_bytes_in_compressed_buffer.
                   No longer reset ring buffer variables whenever we empty it.
05/04/15   rh      Removed diag_fwd_task suspend
04/16/15   sa      Fixed issues with draining beyond the defined levels and global variable access
                   during mode switches.
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
10/07/14   sa      Protect updating the diag current preset id and diag tx mode drain variables.
05/19/14   sa      Fixed issues with random switching between buffering and
                   streaming modes.
04/30/14   sr      Fixed the issue with running out of DSM items on cmd_sio_tx
                   pool
03/25/14   sa      Fixed diag fwd task hogging the CPU if a flow control is
                   enabled from the USB side.
03/07/14   sa      Renamed DIAG_TX_SLAVE_SIG to DIAG_FWD_TX_SLAVE_SIG
                   Renamed DIAG_RX_RSP_TX_SIG to DIAG_FWD_RX_RSP_TX_SIG
02/26/14   sa      Added support for compression in buffering mode
01/27/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature
12/05/13   sr      Added drain timer for forward channel
10/01/13   sr      Added flow control checks on fwd channel
01/31/12   is      Add flow control on diagcomm_sio_tx_wmq
01/24/12   is      Support for buffered circular mode
11/04/11   is      Support for buffered threshold mode
06/14/11   hm      Migrating to REX opaque TCB APIs
09/14/10   is      New control channel and central routing features
06/16/10   VG      Added DIAG_FWD_RPT_TIMER_SIG to osal_wait
05/26/10   JV      Featurized dog functionality for WM and aDSP
04/06/10   JV      Created
===========================================================================*/

#include "diag_fwd_v.h"
#include "diag_compress_v.h"
#include "diagi_v.h"
#include "diagcomm_v.h"
#include "diagcomm_smd.h"
#include "diag_v.h"
#include "diagdsm_v.h" /* For DSM_DIAG_SMD_TX_USED_CNT() */
#include "diag_cfg.h"  /* For DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT */
#include "diagtune.h"
#include "diagcomm_cmd.h"
#include "diagcompress_bufferi.h"


#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "dog.h"
#include "assert.h"
#endif /* (!DIAG_QDSP6_APPS_PROC) */

#include "diagcomm_dci.h"

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)

osal_tcb_t diag_fwd_task_tcb;
osal_thread_attr_t diag_fwd_task_attr;

#endif

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

unsigned int diag_fwd_drain_timer_len = DIAG_FWD_DRAIN_TIMER_LEN_RT; /* Set to streaming mode timer value*/
/*The forward drain timer is used only in the case where APSS does HDLC encoding*/
osal_timer_t diag_fwd_drain_timer;

#if defined(DIAG_BUFFERING_SUPPORT)
  extern volatile diag_tx_mode_type diag_tx_mode; /* From diagcomm_smd.c */
  extern uint32 total_bytes_in_compressed_buffer; /*From diagcompressed_buffer.c */
  extern volatile uint8 diag_tx_mode_internal;

  static osal_timer_t diag_fwd_tout_timer;
  uint64 diag_fwd_tx_sleep_threshold = DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT;
  osal_timer_cnt_t diag_fwd_tx_sleep_time = DIAG_FWD_TX_SLEEP_TIME_DEFAULT;
  uint32 num_compressed_items_to_drain = DIAG_FWD_NUM_COMPRESSED_ITEMS_TO_DRAIN;   /* Number of Compressed chunks to drain at a time from compression buffer*/
  extern osal_mutex_arg_t diag_compression_mutex;
  extern uint32 diagcompressedbuf_tail;
  extern uint32 diagcompressedbuf_head;
  extern uint16 diagbufc_wrap_length;
  extern osal_mutex_arg_t diag_tx_mode_drain_mutex;
#endif

#if defined (DIAG_DOG_SUPPORT)
dog_report_type diag_fwd_dog_rpt_var = 0;
uint32 diag_fwd_dog_rpt_time_var = 0xffff;

osal_timer_t diag_fwd_rpt_timer;
#endif

void diag_fwd_task_start(uint32 params)
{
   int return_val = 0;
	
   return_val = osal_handshake_start(&diag_fwd_task_tcb);
   ASSERT(OSAL_SUCCESS == return_val);
   
#if defined (DIAG_DOG_SUPPORT)
    
	diag_fwd_dog_rpt_var = dog_register_qurt((osal_thread_t)(diag_fwd_task_tcb.thd_id), DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);

    diag_fwd_dog_rpt_time_var = dog_get_report_period(diag_fwd_dog_rpt_var);


    /* If the return value from dog_get_report_period() was zero, then we MUST
    NOT report to dog directly, as DOG will monitor us in a different manner.
    Hence we won't initialize or start any timer. */
    if (diag_fwd_dog_rpt_time_var > 0)
    {
        /*  Initialize watchdog report timer  */
        return_val = osal_create_timer(&diag_fwd_rpt_timer, osal_thread_self(), DIAG_FWD_RPT_TIMER_SIG);
        ASSERT(OSAL_SUCCESS == return_val);
    }
#endif

#if defined(DIAG_BUFFERING_SUPPORT)
    if( diag_fwd_tx_sleep_time > 0 )
    {
      return_val = osal_create_diag_timer(&diag_fwd_tout_timer, osal_thread_self(), DIAG_FWD_TOUT_TIMER_SIG);
      ASSERT(OSAL_SUCCESS == return_val);
    }
#endif


    /*Create timer to drain out DSMs on forward channel */
    return_val = osal_create_diag_timer(&diag_fwd_drain_timer, osal_thread_self(), DIAG_FWD_DRAIN_TIMER_SIG);
    ASSERT(OSAL_SUCCESS == return_val);

   /* Start the dog timer */
    diag_fwd_kick_watchdog ();
    diag_fwd_handle_sigs();

}

void diag_fwd_kick_watchdog(void)
{
#if defined (DIAG_DOG_SUPPORT)
    int return_val = 0;
    dog_report(diag_fwd_dog_rpt_var);
    return_val = osal_set_timer(&diag_fwd_rpt_timer, diag_fwd_dog_rpt_time_var);
    ASSERT(OSAL_SUCCESS == return_val);
#endif
}


#if defined(DIAG_BUFFERING_SUPPORT)
void diag_fwd_suspend( unsigned int duration )
{
  int return_val = 0;
  osal_sigs_t return_sigs = 0;

  if( duration > 0 )
  {
    #if defined (DIAG_QDSP6_APPS_PROC)
      (void)osal_timed_wait( osal_thread_self(), DIAG_FWD_TOUT_TIMER_SIG, &diag_fwd_tout_timer, duration );

      return_val = osal_reset_sigs( osal_thread_self(), DIAG_FWD_TOUT_TIMER_SIG, &return_sigs );
      ASSERT(OSAL_SUCCESS == return_val);
    #else
      (void)osal_timed_wait( osal_thread_self(), DIAG_FWD_TOUT_TIMER_SIG, &diag_fwd_tout_timer, duration );
	
	  return_val = osal_reset_sigs( osal_thread_self(), DIAG_FWD_TOUT_TIMER_SIG, &return_sigs );
      ASSERT(OSAL_SUCCESS == return_val);
    #endif
  }
} /* diag_fwd_suspend */
#endif /* DIAG_BUFFERING_SUPPORT */

/*===========================================================================

FUNCTION DIAG_SET_FWD_DRAIN_TIMER

DESCRIPTION
  This function sets the forward channel drain timer.

===========================================================================*/
void
diag_set_fwd_drain_timer(void)
{
  int return_val = 0;

  if((osal_get_remaining_time(&diag_fwd_drain_timer) == 0) && (diag_fwd_drain_timer_len > 0))
  {
    return_val = osal_set_timer(&diag_fwd_drain_timer, diag_fwd_drain_timer_len);
    ASSERT(OSAL_SUCCESS == return_val);
  }

} /* diag_set_fwd_drain_timer */

/*===========================================================================

FUNCTION DIAG_CLR_FWD_DRAIN_TIMER

DESCRIPTION
  This function clears the forward channel drain timer.

===========================================================================*/
void
diag_clr_fwd_drain_timer(void)
{
  int return_val = 0;

  if(osal_get_remaining_time(&diag_fwd_drain_timer) == 0)
    return;

  return_val = osal_reset_timer(&diag_fwd_drain_timer);
  ASSERT(OSAL_SUCCESS == return_val);

} /* diag_clr_fwd_drain_timer */

/*===========================================================================

FUNCTION DIAG_CTRL_FWD_DRAIN_TIMER_LEN

DESCRIPTION
  This function modifies the forward channel drain timer length based on
  the timer length value that is passed in

PARAMETERS
  timer_len  Set the fwd drain timer if this value is greater than 0
             else, clear the fwd drain timer

===========================================================================*/
void diag_ctrl_fwd_drain_timer_len(unsigned int timer_len)
{
  diag_fwd_drain_timer_len = timer_len;

  if( timer_len > 0 )
    diag_set_fwd_drain_timer();
  else
    diag_clr_fwd_drain_timer();

} /* diag_ctrl_fwd_drain_timer_len */


/*===========================================================================

FUNCTION DIAG_FWD_HANDLE_SIGS

DESCRIPTION
  Handle all of the signals that interest fwd diag task.  Watch-dog is kicked
  as needed.

===========================================================================*/
void diag_fwd_handle_sigs()
{
  osal_sigs_t sigs = 0;
  osal_sigs_t return_sigs = 0;
  int feature_bit_port1 = 0;
  int feature_bit_port2 = 0;
  int return_val = 0;
  uint32 few_mark = 0;
  static boolean bTailIndexWrap = FALSE;  /* To track if drain index is wrapped */
  static boolean bIsDrainIndexSet = FALSE;  /* To record if the drain index is set */
  static int diagbufc_lower_threshold_index = 0; /* Index till which we need to drain */
  static uint32 diagcompressedbuf_tail_index = 0; /* Initial state of the tail */
  static uint32 diagcompressedbuf_head_index; /* This is index till which the data is to be drained from the compression buffer */
  uint8 tx_mode = 0;
  static boolean bCompressionBufDrainComplete = FALSE;
  uint32 diagcompressionbuf_upper_threshold = 0;
  uint32 diagcompressionbuf_lower_threshold = 0;
  uint32 bytes_in_compression_buffer = 0;

#if defined(DIAG_BUFFERING_SUPPORT)
  uint32 tot_wmq_cnt = 0;
  uint32 num_compressed_items_drained = 0; /* Number of Compressed Chunks drained */
  boolean bDrainSet = FALSE;
#endif

  while (1)
  {
    sigs = osal_thread_wait (osal_thread_self(), (DIAG_FWD_READ_SIG | DIAG_FWD_CMD_READ_SIG | DIAG_FWD_TX_SLAVE_SIG | DIAG_FWD_RPT_TIMER_SIG |
                                                    DIAG_FWD_DRAIN_TIMER_SIG | DIAG_FWD_TX_DRAIN_SIG 
                                                  #ifdef DIAG_CONSUMER_API
                                                  | DIAG_FWD_READ_DCI_SIG | DIAG_FWD_TX_DCI_SLAVE_SIG
                                                  #endif 
                                                  ));

    /* Pet the dog before doing any work. */
    diag_fwd_kick_watchdog ();

    /* Retrieve the mode in which target is configured */
    tx_mode = diag_get_tx_mode();

    if( sigs & DIAG_FWD_CMD_READ_SIG )
    {
      (void)osal_reset_sigs( osal_thread_self(), DIAG_FWD_CMD_READ_SIG, &return_sigs );
      diagcomm_process_cmd();
    }
    else if( sigs & DIAG_FWD_READ_SIG )
    {
      (void)osal_reset_sigs( osal_thread_self(), DIAG_FWD_READ_SIG, &return_sigs );
      diagcomm_process_data();
    }
#ifdef DIAG_CONSUMER_API
      else if (sigs & DIAG_FWD_READ_DCI_SIG)
      {
          (void)osal_reset_sigs( osal_thread_self(), DIAG_FWD_READ_DCI_SIG, &return_sigs );
          diagcomm_process_data_dci();
      }
#endif /* DIAG_CONSUMER_API */

    if( sigs & DIAG_FWD_DRAIN_TIMER_SIG )
    {
      (void)osal_reset_sigs(osal_thread_self(), DIAG_FWD_DRAIN_TIMER_SIG, &return_sigs);

      /* Flush any data pending in SIO_TX pool */
      diagcomm_smd_process_slave_tx(DIAGCOMM_PORT_NONE);
	  diagcomm_send_master_tx(DIAGCOMM_PORT_NONE);
    }
    
    /* Drain the data from the compressed buffer */
    if(sigs & DIAG_FWD_TX_DRAIN_SIG)
    {
      (void) osal_reset_sigs (&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
#if defined(DIAG_BUFFERING_SUPPORT)
      /* If there is space in compression buffer set signal to the compression task to continue processing the slave traffic */
      if(tx_mode != DIAG_TX_MODE_STREAMING && diagcomm_compression_flow_enabled())
      {
        return_val = osal_set_sigs(&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);  
      }      

      /* Take the local snip of global data */
      bDrainSet = diag_tx_mode.drain;
      /* Reset the drain flag if set as the drain logic below will initiate the drain based on the local var*/
      if(TRUE == bDrainSet)
      {
        osal_lock_mutex(&diag_tx_mode_drain_mutex);
        diag_tx_mode.drain = FALSE;
        osal_unlock_mutex(&diag_tx_mode_drain_mutex);
      }
      diagcompressionbuf_upper_threshold = diag_tx_mode.buffered_many_mark;
      diagcompressionbuf_lower_threshold = diag_tx_mode.buffered_few_mark;
     
      osal_lock_mutex(&diag_compression_mutex);
      bytes_in_compression_buffer = total_bytes_in_compressed_buffer;
      osal_unlock_mutex(&diag_compression_mutex);
      
      /* Reset the drain indices if the total bytes in compression buffer crosses the upper threshold and the drain indices are not set yet OR
               if the drain flag is set and total bytes in compression buffer is greater then the lower threshold defined OR
               if the mode is switched to streaming */
      if((tx_mode == DIAG_TX_MODE_BUFFERED_THRESH && bytes_in_compression_buffer >= diagcompressionbuf_upper_threshold
              && bIsDrainIndexSet == FALSE) || 
             (bDrainSet == TRUE && bytes_in_compression_buffer > diagcompressionbuf_lower_threshold) || 
             (tx_mode == DIAG_TX_MODE_STREAMING &&  bytes_in_compression_buffer > 0 ) )
         
      {
        /* Reset the drain wrap flag */
        bTailIndexWrap = FALSE;
        osal_lock_mutex(&diag_compression_mutex);
        diagcompressedbuf_head_index = diagcompressedbuf_head;
        few_mark = diag_tx_mode.buffered_few_mark;
        /* Reset the lower threshold to zero if in streaming mode, to drain
                   the compression buffer completely */
        if(tx_mode == DIAG_TX_MODE_STREAMING)
          few_mark = 0;
        diagbufc_lower_threshold_index = diagcompressedbuf_head_index - few_mark;;
        diagcompressedbuf_tail_index = diagcompressedbuf_tail;
        
        if( diagbufc_lower_threshold_index < 0)
        {
          diagbufc_lower_threshold_index = (DIAGBUFC_SIZE + diagbufc_lower_threshold_index);
          if( DIAGBUFC_SIZE - diagbufc_lower_threshold_index <= diagbufc_wrap_length)
          {
            diagbufc_lower_threshold_index = diagbufc_lower_threshold_index - diagbufc_wrap_length;
          }
        }
        if(diagcompressedbuf_tail_index >= diagbufc_lower_threshold_index)
        {
          bTailIndexWrap = TRUE;
        }
        bIsDrainIndexSet = TRUE;
        bCompressionBufDrainComplete = FALSE;
        osal_unlock_mutex(&diag_compression_mutex);
        ASSERT(diagcompressedbuf_tail <= DIAGBUFC_SIZE);
        ASSERT(diagcompressedbuf_head <= DIAGBUFC_SIZE);
      }
      if(bIsDrainIndexSet == TRUE)
      {
           do
           {
             if(diagcomm_status() && diagcomm_sio_tx_flow_enabled())
             {
               /* Drain the compressed data */
               diagcompressedbuf_drain();
               num_compressed_items_drained++;           
             }
             else
             {
               break;
             }

            if( TRUE == bTailIndexWrap) 
            { 
              if(diagcompressedbuf_tail <= diagcompressedbuf_tail_index)
              {
                if(diagcompressedbuf_tail >= diagbufc_lower_threshold_index)
                {
                  bCompressionBufDrainComplete = TRUE;
                  break;
                }
              }
            }
            else 
            {
              if(diagcompressedbuf_tail >= diagbufc_lower_threshold_index)
              {
                bCompressionBufDrainComplete = TRUE;
                break;
              }
            }
          }while( num_compressed_items_drained < num_compressed_items_to_drain);
          /* Reset the count of compressed chunks drained */
          num_compressed_items_drained = 0;
        

        /* If in buffering mode, set the Compression Signal if there is memory in the compression buffer.
                This is to take care of the case when compression buffer is completely packed and we are not
                able to compress any further. After draining some compressed chunks, we check if there is memory 
                in the compresson buffer and set the compress signalr*/
        if((tx_mode != DIAG_TX_MODE_STREAMING) && (diagcomm_compression_flow_enabled()))
        {
          osal_set_sigs(&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs);
        }
                 
         /* If the expected data is drained from compression buffer*/
         if( TRUE == bCompressionBufDrainComplete)
         {
            /* If the expected data is drained, reset the variables*/
            osal_lock_mutex(&diag_compression_mutex);
            bIsDrainIndexSet = FALSE;
            bTailIndexWrap = FALSE;
            osal_unlock_mutex(&diag_compression_mutex);
            
            if(tx_mode == DIAG_TX_MODE_STREAMING)
            {
              /* if in streaming and still data is present in the compressed buffer*/
              osal_lock_mutex(&diag_compression_mutex);
              if( total_bytes_in_compressed_buffer > 0)
              {
                /*Continue Draining if we can */
				if(diagcomm_status() && diagcomm_sio_tx_flow_enabled())
                  osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
              }
              else
              {
                /*Rest the signal to drain the data from the compressed buffer*/
                (void) osal_reset_sigs (&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);   
                diag_update_mode_info(tx_mode);
                
                diag_tx_mode_internal = DIAG_TX_MODE_STREAMING;
                /* Set signal to diag task to process traffic from slave */
                return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
                ASSERT(OSAL_SUCCESS == return_val);
              }
              osal_unlock_mutex(&diag_compression_mutex);
            }
            else
            {
              /* Switch back to compression */
              osal_set_sigs(&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs);
            }
         }
         else
         {
           /* Continue Draining if we have space */
           if(diagcomm_status() && diagcomm_sio_tx_flow_enabled())
             osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
         }
      }
#endif
    }
    else if( sigs & DIAG_FWD_TX_SLAVE_SIG )
    {
      (void) osal_reset_sigs (osal_thread_self(), DIAG_FWD_TX_SLAVE_SIG, &return_sigs);

       osal_lock_mutex(&diag_compression_mutex);
       bytes_in_compression_buffer = total_bytes_in_compressed_buffer;
       osal_unlock_mutex(&diag_compression_mutex);
       /* If mode is switched to streaming, first drain the data (if any) in compression buffer */
      if(tx_mode == DIAG_TX_MODE_STREAMING && bytes_in_compression_buffer > 0)
      {
        /* Reset the Compression Signal*/
        return_val = osal_reset_sigs(&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        /* Set the drain signal to drain data from Compression Buffer */
        return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        continue;
      }
      
      feature_bit_port1 = DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS);
      feature_bit_port2 = DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS);

      /*Check the flow control variable only if HDLC feature bit is turned on in MPSS or ADSP - since we dont know which port traffic is being handled currently*/
      if( feature_bit_port1 && feature_bit_port2 )
      {
        if( diagcomm_status() && diagcomm_sio_tx_flow_enabled() )
        {
          if( diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1) ) 
          {
            (void)diagcomm_smd_process_slave_tx(DIAGCOMM_PORT_1);
            (void)diagcomm_send_master_tx(DIAGCOMM_PORT_1);
          }	
        }
      }
      else
      {
        /*Backward compatibility case where Apps forwards the slave traffic directly; 
    there is no need to check for flow control here since there will be no DSM allocations from SIO_TX pool*/
        if( diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1) ) 
        {
          (void)diagcomm_smd_process_slave_tx(DIAGCOMM_PORT_1);
          (void)diagcomm_send_master_tx(DIAGCOMM_PORT_1);
        }  
      }

#if defined(DIAG_BUFFERING_SUPPORT) 
      tot_wmq_cnt = diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1);
    
      
      if( (diag_tx_mode.mode == DIAG_TX_MODE_STREAMING) )
      {
        if( diagcomm_status() && diagcomm_sio_tx_flow_enabled() && tot_wmq_cnt )
        {
          osal_set_sigs(osal_thread_self(), DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
        }
      }
#endif
    }
    
  #ifdef DIAG_CONSUMER_API
    if( sigs & DIAG_FWD_TX_DCI_SLAVE_SIG)
    {
       (void) osal_reset_sigs (osal_thread_self(), DIAG_FWD_TX_DCI_SLAVE_SIG, &return_sigs);
       if( diagcomm_smd_get_dci_rx_wmq_cnt(DIAGCOMM_PORT_1) ) 
         diagcomm_smd_fusion_process_slave_tx(DIAGCOMM_PORT_1);	
    }
  #endif /* #ifdef DIAG_CONSUMER_API */
    #if !defined(DIAG_BUFFERING_SUPPORT)
    if( diagcomm_status() && diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1) )
    {
      osal_set_sigs(osal_thread_self(), DIAG_FWD_TX_SLAVE_SIG, &return_sigs);
    }
    #endif /* !DIAG_BUFFERING_SUPPORT */  
  } /* END While */
} /* diag_fwd_handle_sigs */

