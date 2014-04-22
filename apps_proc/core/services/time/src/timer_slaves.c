/*=============================================================================
  Copyright (c) 2003-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/**
@file timer_slaves.c
@brief
Implements timer slave tasks

Traditionally, timer task has been used to call the calllback 
functions associated with the timer. This has often led to 
abusive callback holding timer task at ransom, affecting timer 
accuracy. By implementing these slave tasks, the callbacks can 
be offloaded to these slave tasks, and hence timer task is free 
to services other expiring timers. 

In this file, there is support for three slave tasks, which can 
be enabled by defining FEATURE_TIMER_SLAVE1, 
FEATURE_TIMER_SLAVE2 and FEATURE_TIMER_SLAVE3 respectively. 
These defines can be made in <product>.builds file.

INITIALIZATION AND SEQUENCING REQUIREMENTS

timer_slave_cmd_queue_init() should be called before timer_init()

The slave tasks should be started before the timer task in tmc. 
The reason is that there might be expired timers needed to be serviced
when timer task starts. This would require that timer slave task(s) 
are already started to process these timer callbacks.* 
*/ 

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/timer_slaves.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
04/02/2012 rks     Removed False Dependency to TMC.H in CoreBSP-Services
04/12/11   gparikh File created.

=============================================================================*/

#include "timer_slaves.h"
#include "msg.h"
#include "err.h"
#include "qurt.h"
#include "rcevt.h"
#include "rcinit_qurt.h"
#include "DDITimetick.h"
#include "assert.h"

#define TIMER_FREQ_VALUE          19200000

/*defines here*/
#define MAX_TASK_PRIORITY 255

#define MAGIC_INITIALIZER 0xDEADBEEF
#define MAGIC_INITIALIZER_64 0xDEADBEEFD00DFEEDuLL
/*end defines*/

//#define TIMER_SLAVE_PRINT_HEALTH_MSG
#define TIMER_SLAVE_DEBUG_MSG_TH 50

/*static variables here*/

static DalDeviceHandle *hTimerSlavesHandle = NULL;

/*Define timer cmd queue*/
timer_slave_cmd_type timer_slaves_cmd_buf[MAX_SLAVE_TASKS][MAX_TIMER_SLAVE_CMD_ELEMENTS];

/*to store the commands in queue*/
q_type timer_slaves_cmd_q[MAX_SLAVE_TASKS];          

/* Queue for storing free command buffers */
q_type timer_slaves_cmd_free_q[MAX_SLAVE_TASKS];     

uint32 timer_slave_bin_boundaries[MAX_SLAVE_TASKS];

#ifdef FEATURE_DOG
timer_type                    timer_slave_dog_timers[MAX_SLAVE_TASKS];

#ifdef DOG_DYNAMIC_API
/*  Dog variables - now we MAY be dynamic       */
/** @name Dog variables 
@{ */
uint32  dog_timer_slaves_rpt[MAX_SLAVE_TASKS];                 /** Dog report ID. */
uint32  dog_timer_slaves_rpt_time[MAX_SLAVE_TASKS];            /** Report period. */ /* TODO: what is this?*/
 
/** @} */ /* end_name Dog variables */
#endif /* DOG_DYNAMIC_API */
#endif /* FEATURE_DOG */

static uint64 timer_slave_processing_time_warning_threshold = (8*TIMER_FREQ_VALUE)/32768; /* Keeping same ratio as 32kHz case */

#ifndef TIMER_ERROR_FATAL_THRESHOLD_ZEROED_OUT 
static uint64 timer_slave_processing_time_err_fatal_threshold=2*TIMER_FREQ_VALUE;
#else
static uint64 timer_slave_processing_time_err_fatal_threshold=0;
#endif

#ifdef FEATURE_DOG
static qurt_thread_t timer_slaves_qurt_tid[MAX_SLAVE_TASKS];
#endif

/* Signal objects for the signalling */
static qurt_signal_t timer_slaves_qurt_futex[MAX_SLAVE_TASKS];

/*end static variables*/

/*function prototypes here*/

#ifdef FEATURE_DOG
void timer_slave_kick_watch_dog
(
  timer_slave_task_enum_type slave_task_num 
);

void timer_slave_register_dog
(
  timer_slave_task_enum_type timer_slave_num
);
#endif /* FEATURE_DOG */
extern void timer_reset_check_pc_mask_if_set
(
  unsigned int index,
  unsigned int decrease_count
);

/*end function prototypes*/

/*Debug variables here*/

typedef struct
{
  int               pri;
  uint8             bin;
}timer_log_task_pri_type;

#define MAX_TASK_PRI_COUNTER 1000
timer_log_task_pri_type timer_log_task_pri[MAX_TASK_PRI_COUNTER];
uint32 pri_counter = 0;


typedef struct timer_slave_debug_var
{
  timer_ptr_type timer_expired;
  time_timetick_type processing_started;
  time_timetick_type processing_time;
  uint32 fn_address;
  uint8  call_back_type;
}timer_expired_type;

/*the arrays are intentionally defined separately for fine control
 over each array size.*/
#ifdef FEATURE_TIMER_SLAVE1
#define MAX_TIMER_EXPIRED_SLAVE1 30
static timer_expired_type timers_expired_slave1[MAX_TIMER_EXPIRED_SLAVE1];
static uint32 timer_expired_cnt_slave1=0;
static uint32 timer_expired_cnt_global_slave1=0;
#endif

#ifdef FEATURE_TIMER_SLAVE2
#define MAX_TIMER_EXPIRED_SLAVE2 30
static timer_expired_type timers_expired_slave2[MAX_TIMER_EXPIRED_SLAVE2];
static uint32 timer_expired_cnt_slave2=0;
static uint32 timer_expired_cnt_global_slave2=0;
#endif

#ifdef FEATURE_TIMER_SLAVE3
#define MAX_TIMER_EXPIRED_SLAVE3 30
static timer_expired_type timers_expired_slave3[MAX_TIMER_EXPIRED_SLAVE3];
static uint32 timer_expired_cnt_slave3=0;
static uint32 timer_expired_cnt_global_slave3=0;
#endif


/*end debug variables*/

/** 
@ingroup Timer Slaves Functions 
         
@brief 
Kicks the dog.

@return
None

@param[in]                                                               
   timer_slave_task_enum_type - task kicking the dog

@param[out]
None
 
@dependencies  
dog task needs to be started before calling this fn
 
@sideeffects 

*/
#ifdef FEATURE_DOG
void timer_slave_kick_watch_dog
(
  timer_slave_task_enum_type slave_task_num 
)
{
  timer_type * timer_slave_dog_rpt_timer_ptr = NULL;

  /*check for bounds on time_slave_task_num */
  if(slave_task_num >= MAX_SLAVE_TASKS)
  {
    MSG_2(MSG_SSID_ATS, MSG_LEGACY_ERROR,"timer_slave_task_num = %d, max value should be less than = %d"
              ,slave_task_num,MAX_SLAVE_TASKS);
    return;
  }

  timer_slave_dog_rpt_timer_ptr = &timer_slave_dog_timers[slave_task_num];
  
  /*if we got the timer ptr then do the dog reporting*/
  if(timer_slave_dog_rpt_timer_ptr)
  {
    dog_report(dog_timer_slaves_rpt[slave_task_num]);   
    (void) timer_set( timer_slave_dog_rpt_timer_ptr,
                          dog_timer_slaves_rpt_time[slave_task_num],0,T_MSEC );  
  }
  return;  
}
#endif /* FEATURE_DOG */
/** 
@ingroup Timer Slaves Functions 
         
@brief 
Registers the task with dog. Slave tasks when started, are not
registered with dog due to sequence dependency (dog task is started
after the slave tasks). 

@return
None

@param[in]                                                               
   timer_slave_task_enum_type - task to be registered with dog

@param[out]
None
 
@dependencies  
dog task needs to be started before calling this fn
 
@sideeffects 

*/
#ifdef FEATURE_DOG
void timer_slave_register_dog
(
  timer_slave_task_enum_type timer_slave_num
)
{
  dog_timer_slaves_rpt[timer_slave_num]      = dog_register_qurt( timer_slaves_qurt_tid[timer_slave_num], \
                                    DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  dog_timer_slaves_rpt_time[timer_slave_num] = dog_get_report_period( dog_timer_slaves_rpt[timer_slave_num] );
  
  /*kick the watchdog*/
  timer_slave_kick_watch_dog(timer_slave_num);
}
#endif /* FEATURE_DOG */

/** 
@ingroup Timer Slaves Functions 
         
@brief 
timer slave task 1 entry point. This function is called from tmc.

@return
None.

@param[in]                                                               
 None  

@param[out]
 dword - Not used   
 
@dependencies  
Should be called before timer_task
 
@sideeffects 

*/
void timer_slave1_task
(
  /* Unused parameter - required to match task func prototype */
  dword                           ignored    /*lint -esym(715,ignored) */
)
{
#ifdef FEATURE_TIMER_SLAVE1

  /* Signals mask for wait procedure */
  uint32                    sigs;

  /*cmd pointer*/
  timer_slave_cmd_type *cmd_ptr = NULL;

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = 0;

  /*fn address*/
  uint32  fn_address_of_timer = 0;

  /*for debug messages*/
  uint32 print_health_msg_slave1 = 0;

  uint32 timer_after_pc_local_count =0;
  
  rcinit_handshake_startup();

  #ifdef FEATURE_DOG
  timer_def_osal(&timer_slave_dog_timers[TIMER_SLAVE_TASK1],
  			NULL,
  			TIMER_NATIVE_OS_SIGNAL_TYPE,
  			&timer_slaves_qurt_futex[TIMER_SLAVE_TASK1],
  			TIMER_SLAVE_RPT_TIMER_SIG);
  #endif
  
  /* Signal the task in order to prime the timer processing mechanism */
  qurt_signal_set(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK1],TIMER_SLAVE_CMD_Q_SIG );

  for(;;)
  {

    /* Wait for signal */
    sigs = qurt_signal_wait(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK1],
			TIMER_SLAVE_CMD_Q_SIG|TIMER_SLAVE_RPT_TIMER_SIG,
			QURT_SIGNAL_ATTR_WAIT_ANY);

    if(sigs & TIMER_SLAVE_RPT_TIMER_SIG)
    {
      #ifdef FEATURE_DOG
	  qurt_signal_clear(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK1],TIMER_SLAVE_RPT_TIMER_SIG);
      timer_slave_kick_watch_dog(TIMER_SLAVE_TASK1);      
      #ifdef TIMER_SLAVE_PRINT_HEALTH_MSG
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer_debug: slave1 dog timer fired");
      #endif
      #endif
    }

    if ( sigs & TIMER_SLAVE_CMD_Q_SIG )
    {
      /* Clear the signal */
	  qurt_signal_clear(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK1],TIMER_SLAVE_CMD_Q_SIG);
      
      timer_after_pc_local_count=0;
      /*loop through the command queue and process all the commands*/
      while((cmd_ptr = (timer_slave_cmd_type *) q_get(&timer_slaves_cmd_q[TIMER_SLAVE_TASK1])) != NULL)
      {

        timers_expired_slave1[timer_expired_cnt_slave1].timer_expired = cmd_ptr->timer_slave_operation.timer;
        timers_expired_slave1[timer_expired_cnt_slave1].call_back_type = 0;
        timers_expired_slave1[timer_expired_cnt_slave1].fn_address = 0;
        timers_expired_slave1[timer_expired_cnt_slave1].processing_time = MAGIC_INITIALIZER_64;

        /*log the time when processing started*/
        DalTimetick_GetTimetick64(hTimerSlavesHandle, &cb_processing_started);

        /*Log the time to indicate the start of processing of this timer*/
        timers_expired_slave1[timer_expired_cnt_slave1].processing_started = cb_processing_started;
    
        switch (cmd_ptr->call_back_type) 
        {
          /*TODO: need to fix this*/
          case TIMER_SLAVE_SET_SIG:
             /*as of now setting the signal through slave task is not supported. 
              It is done by timer task. This is just a place holder*/            
          break;
          case TIMER_SLAVE_CB_TYPE1:
            if(cmd_ptr->timer_slave_operation.func_cb1)
            {
              timers_expired_slave1[timer_expired_cnt_slave1].call_back_type = 
              (timers_expired_slave1[timer_expired_cnt_slave1].call_back_type | 0x2);
              timers_expired_slave1[timer_expired_cnt_slave1].fn_address = (uint32)cmd_ptr->timer_slave_operation.func_cb1;
              
              fn_address_of_timer = (uint32)cmd_ptr->timer_slave_operation.func_cb1;
             
              cmd_ptr->timer_slave_operation.func_cb1(cmd_ptr->timer_slave_operation.data);
            }
                       
            break;
          case TIMER_SLAVE_CB_TYPE2:
            if(cmd_ptr->timer_slave_operation.func_cb2)
            {
               timers_expired_slave1[timer_expired_cnt_slave1].call_back_type = 
               (timers_expired_slave1[timer_expired_cnt_slave1].call_back_type | 0x4); 
               timers_expired_slave1[timer_expired_cnt_slave1].fn_address = (uint32)cmd_ptr->timer_slave_operation.func_cb2;
               
               fn_address_of_timer = (uint32)cmd_ptr->timer_slave_operation.func_cb2;

               cmd_ptr->timer_slave_operation.func_cb2(cmd_ptr->timer_slave_operation.run_time,cmd_ptr->timer_slave_operation.data);
            }
          break;

          default:
            MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer_slave1_debug: command type not supported");
          break;
        }/*end switch*/

        /*return the memory to free command queue for reuse*/
        q_put( &timer_slaves_cmd_free_q[TIMER_SLAVE_TASK1], &cmd_ptr->link );

        /*log the cb ended time*/
        DalTimetick_GetTimetick64(hTimerSlavesHandle, &cb_processing_ended);
 
        /*to take care of the wrap around case*/
        if(cb_processing_ended >= cb_processing_started)
        {
          cb_processing_time = cb_processing_ended - cb_processing_started;       
        }
        else /*if it is a wrap around case*/
        {  
          cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
        }

        timers_expired_slave1[timer_expired_cnt_slave1].processing_time = cb_processing_time;
        timer_expired_cnt_slave1 = (timer_expired_cnt_slave1+1)%MAX_TIMER_EXPIRED_SLAVE1;
        timer_expired_cnt_global_slave1++;

        #ifdef TIMER_SLAVE_PRINT_HEALTH_MSG
        if(!(timer_expired_cnt_global_slave1%TIMER_SLAVE_DEBUG_MSG_TH))
        {
          print_health_msg_slave1=1;
        }
        else
        {
          print_health_msg_slave1 =0;
        }
        #endif

        /*generate a warningif the processing time > warning threshold*/
        if((cb_processing_time > timer_slave_processing_time_warning_threshold)||print_health_msg_slave1)
        {
          MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_slave_processing_time_warning_threshold);          
                    
        }

        /*generate a warningif the processing time > warning threshold*/
        if(timer_slave_processing_time_err_fatal_threshold != 0 
           && cb_processing_time > timer_slave_processing_time_err_fatal_threshold)
        {
          ERR_FATAL("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_slave_processing_time_warning_threshold);          
                    
        }
        timer_after_pc_local_count++;
      }/*end while*/
      // DID we see this happened after powercollapse
      timer_reset_check_pc_mask_if_set(1,timer_after_pc_local_count);
    }/*end TIMER_SLAVE1_CMD_Q*/    

  }/*end for*/

#endif /* FEATURE_TIMER_SLAVE1 */
} /* timer_slave1_task */

/** 
@ingroup Timer Slaves Functions 
         
@brief 
timer slave task 2 entry point. This function is called from tmc.

@return
None.

@param[in]                                                               
 dword - Not used  

@param[out]
 None   
 
@dependencies  
Should be called before timer_task
 
@sideeffects 

*/
void timer_slave2_task
(
  /* Unused parameter - required to match task func prototype */
  dword                           ignored    /*lint -esym(725,ignored) */
)
{
#ifdef FEATURE_TIMER_SLAVE2

  /* Signals for wait procedure */
  uint32                   sigs;

  /*cmd pointer*/
  timer_slave_cmd_type *cmd_ptr = NULL;

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = 0;

  /*fn address*/
  uint32  fn_address_of_timer = 0;

  /*for debug message*/
  uint32 print_health_msg_slave2 = 0;

  uint32 timer_after_pc_local_count =0;

  rcinit_handshake_startup();

  #ifdef FEATURE_DOG
  timer_def_osal(&timer_slave_dog_timers[TIMER_SLAVE_TASK2],
  			NULL,
  			TIMER_NATIVE_OS_SIGNAL_TYPE,
  			&timer_slaves_qurt_futex[TIMER_SLAVE_TASK2],
  			TIMER_SLAVE_RPT_TIMER_SIG);  
  #endif /* FEATURE_DOG */
  
  /* Signal the task in order to prime the timer processing mechanism */
  qurt_signal_set(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK2],TIMER_SLAVE_CMD_Q_SIG );

  for(;;)
  {

    /* Wait for signal */
    sigs = qurt_signal_wait(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK2],
			TIMER_SLAVE_CMD_Q_SIG|TIMER_SLAVE_RPT_TIMER_SIG,
			QURT_SIGNAL_ATTR_WAIT_ANY);	

    if(sigs & TIMER_SLAVE_RPT_TIMER_SIG)
    {
      #ifdef FEATURE_DOG
	  qurt_signal_clear(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK2],TIMER_SLAVE_RPT_TIMER_SIG);
      timer_slave_kick_watch_dog(TIMER_SLAVE_TASK2);    
      #ifdef TIMER_SLAVE_PRINT_HEALTH_MSG
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer_debug: slave2 dog timer fired");
      #endif
      #endif
    }

    if ( sigs & TIMER_SLAVE_CMD_Q_SIG )
    {
      /* Clear the timer expiry signal */
	  qurt_signal_clear(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK2],TIMER_SLAVE_CMD_Q_SIG);
      timer_after_pc_local_count =0;
      /*loop through the command queue and process all the commands*/
      while((cmd_ptr = (timer_slave_cmd_type *) q_get(&timer_slaves_cmd_q[TIMER_SLAVE_TASK2])) != NULL)
      {
        timers_expired_slave2[timer_expired_cnt_slave2].timer_expired = cmd_ptr->timer_slave_operation.timer;
        timers_expired_slave2[timer_expired_cnt_slave2].call_back_type = 0;
        timers_expired_slave2[timer_expired_cnt_slave2].fn_address = 0;
        timers_expired_slave2[timer_expired_cnt_slave2].processing_time = MAGIC_INITIALIZER_64;

        /*log the time when processing started*/
        DalTimetick_GetTimetick64(hTimerSlavesHandle, &cb_processing_started);

        /*Log the time to indicate the start of processing of this timer*/
        timers_expired_slave2[timer_expired_cnt_slave2].processing_started = cb_processing_started;

        switch (cmd_ptr->call_back_type) 
        {
          /*TODO: need to fix this*/
          case TIMER_SLAVE_SET_SIG:
             /*as of now setting the signal through slave task is not supported. 
              It is done by timer task. This is just a place holder*/
          break;
          case TIMER_SLAVE_CB_TYPE1:
            if(cmd_ptr->timer_slave_operation.func_cb1)
            {
              timers_expired_slave2[timer_expired_cnt_slave2].call_back_type = 
              (timers_expired_slave2[timer_expired_cnt_slave2].call_back_type | 0x2);
              timers_expired_slave2[timer_expired_cnt_slave2].fn_address = (uint32)cmd_ptr->timer_slave_operation.func_cb1;
              
              fn_address_of_timer = (uint32)cmd_ptr->timer_slave_operation.func_cb1;

              cmd_ptr->timer_slave_operation.func_cb1(cmd_ptr->timer_slave_operation.data);
            }
                       
            break;
          case TIMER_SLAVE_CB_TYPE2:
            if(cmd_ptr->timer_slave_operation.func_cb2)
            {
              timers_expired_slave2[timer_expired_cnt_slave2].call_back_type = 
              (timers_expired_slave2[timer_expired_cnt_slave2].call_back_type | 0x4); 
              timers_expired_slave2[timer_expired_cnt_slave2].fn_address = (uint32)cmd_ptr->timer_slave_operation.func_cb2;
               
              fn_address_of_timer = (uint32)cmd_ptr->timer_slave_operation.func_cb2;

              cmd_ptr->timer_slave_operation.func_cb2(cmd_ptr->timer_slave_operation.run_time,cmd_ptr->timer_slave_operation.data);
            }
          break;

          default:
            MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer_slave2_debug: command type not supported");
          break;
        }/*end switch*/

        /*return the memory to free command queue for reuse*/
        q_put( &timer_slaves_cmd_free_q[TIMER_SLAVE_TASK2], &cmd_ptr->link );        

         /*log the cb ended time*/
        DalTimetick_GetTimetick64(hTimerSlavesHandle, &cb_processing_ended);
 
        /*to take care of the wrap around case*/
        if(cb_processing_ended >= cb_processing_started)
        {
          cb_processing_time = cb_processing_ended - cb_processing_started;       
        }
        else /*if it is a wrap around case*/
        {  
          cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
        }

        timers_expired_slave2[timer_expired_cnt_slave2].processing_time = cb_processing_time;
        timer_expired_cnt_slave2 = (timer_expired_cnt_slave2+1)%MAX_TIMER_EXPIRED_SLAVE2;
        timer_expired_cnt_global_slave2++;

        #ifdef TIMER_SLAVE_PRINT_HEALTH_MSG
        if(!(timer_expired_cnt_global_slave2%TIMER_SLAVE_DEBUG_MSG_TH))
        {
          print_health_msg_slave2=1;
        }
        else
        {
          print_health_msg_slave2 =0;
        }
        #endif

        /*generate a warningif the processing time > warning threshold*/
        if((cb_processing_time > timer_slave_processing_time_warning_threshold)||print_health_msg_slave2)
        {
          MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_slave_processing_time_warning_threshold);          
                    
        }

        /*generate a warningif the processing time > warning threshold*/
        if(timer_slave_processing_time_err_fatal_threshold != 0 
           && cb_processing_time > timer_slave_processing_time_err_fatal_threshold)
        {
          ERR_FATAL("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_slave_processing_time_warning_threshold);          
                    
        }
        timer_after_pc_local_count++;
      }/*end while*/
      timer_reset_check_pc_mask_if_set(2,timer_after_pc_local_count);
    }/*end TIMER_SLAVE2_CMD_Q*/

  }/*end for*/

#endif /* FEATURE_TIMER_SLAVE2*/

} /* timer_slave2_task */

/** 
@ingroup Timer Slaves Functions 
         
@brief 
timer slave task 3 entry point. This function is called from tmc.

@return
None.

@param[in]                                                               
 dword - Not used  

@param[out]
 None   
 
@dependencies  
Should be called before timer_task
 
@sideeffects 

*/
void timer_slave3_task
(
  /* Unused parameter - required to match task func prototype */
  dword                           ignored    /*lint -esym(735,ignored) */
)
{

#ifdef FEATURE_TIMER_SLAVE3
  /* Signals for wait procedure */
  uint32                   sigs;

  /*cmd pointer*/
  timer_slave_cmd_type *cmd_ptr = NULL;

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = 0;

  /*fn address*/
  uint32  fn_address_of_timer = 0;

  /*for debug message*/
  uint32 print_health_msg_slave3 = 0;

  uint32 timer_after_pc_local_count =0;

  rcinit_handshake_startup();

  #ifdef FEATURE_DOG
  timer_def_osal(&timer_slave_dog_timers[TIMER_SLAVE_TASK3],
  			NULL,
  			TIMER_NATIVE_OS_SIGNAL_TYPE,
  			&timer_slaves_qurt_futex[TIMER_SLAVE_TASK3],
  			TIMER_SLAVE_RPT_TIMER_SIG);    
  #endif /* FEATURE_DOG */
  
  /* Signal the task in order to prime the timer processing mechanism */
  qurt_signal_set(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK3],TIMER_SLAVE_CMD_Q_SIG );

  for(;;)
  {

    /* Wait for some signal */
    sigs = qurt_signal_wait(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK3],
			TIMER_SLAVE_CMD_Q_SIG|TIMER_SLAVE_RPT_TIMER_SIG,
			QURT_SIGNAL_ATTR_WAIT_ANY);	


    if(sigs & TIMER_SLAVE_RPT_TIMER_SIG)
    {
      #ifdef FEATURE_DOG
      qurt_signal_clear(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK3],TIMER_SLAVE_RPT_TIMER_SIG);
      timer_slave_kick_watch_dog(TIMER_SLAVE_TASK3);      
      #ifdef TIMER_SLAVE_PRINT_HEALTH_MSG
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer_debug: slave3 dog timer fired");
      #endif
      #endif
    }

    if ( sigs & TIMER_SLAVE_CMD_Q_SIG )
    {
      /* Clear the timer expiry signal */
      qurt_signal_clear(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK3],TIMER_SLAVE_CMD_Q_SIG);
      timer_after_pc_local_count =0;
      /*loop through the command queue and process all the commands*/
      while((cmd_ptr = (timer_slave_cmd_type *) q_get(&timer_slaves_cmd_q[TIMER_SLAVE_TASK3])) != NULL)
      {
        timers_expired_slave3[timer_expired_cnt_slave3].timer_expired = cmd_ptr->timer_slave_operation.timer;
        timers_expired_slave3[timer_expired_cnt_slave3].call_back_type = 0;
        timers_expired_slave3[timer_expired_cnt_slave3].fn_address = 0;
        timers_expired_slave3[timer_expired_cnt_slave3].processing_time = MAGIC_INITIALIZER_64;

        /*log the time when processing started*/
        DalTimetick_GetTimetick64(hTimerSlavesHandle, &cb_processing_started);

        /*Log the time to indicate the start of processing of this timer*/
        timers_expired_slave3[timer_expired_cnt_slave3].processing_started = cb_processing_started;

        switch (cmd_ptr->call_back_type) 
        {
          /*TODO: need to fix this*/
          case TIMER_SLAVE_SET_SIG:
            /*as of now setting the signal through slave task is not supported. 
              It is done by timer task. This is just a place holder*/
          break;
          case TIMER_SLAVE_CB_TYPE1:
            if(cmd_ptr->timer_slave_operation.func_cb1)
            {
              timers_expired_slave3[timer_expired_cnt_slave3].call_back_type = 
              (timers_expired_slave3[timer_expired_cnt_slave3].call_back_type | 0x2);
              timers_expired_slave3[timer_expired_cnt_slave3].fn_address = (uint32)cmd_ptr->timer_slave_operation.func_cb1;
              
              fn_address_of_timer = (uint32)cmd_ptr->timer_slave_operation.func_cb1;

              cmd_ptr->timer_slave_operation.func_cb1(cmd_ptr->timer_slave_operation.data);
            }
                       
            break;
          case TIMER_SLAVE_CB_TYPE2:
            if(cmd_ptr->timer_slave_operation.func_cb2)
            {
              timers_expired_slave3[timer_expired_cnt_slave3].call_back_type = 
              (timers_expired_slave3[timer_expired_cnt_slave3].call_back_type | 0x4); 
              timers_expired_slave3[timer_expired_cnt_slave3].fn_address = (uint32)cmd_ptr->timer_slave_operation.func_cb2;
               
              fn_address_of_timer = (uint32)cmd_ptr->timer_slave_operation.func_cb2;
              cmd_ptr->timer_slave_operation.func_cb2(cmd_ptr->timer_slave_operation.run_time,cmd_ptr->timer_slave_operation.data);
            }
          break;

          default:
            MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer_slave3_debug: command type not supported");
          break;
        }/*end switch*/

        /*return the memory to free command queue for reuse*/
        q_put( &timer_slaves_cmd_free_q[TIMER_SLAVE_TASK3], &cmd_ptr->link );

         /*log the cb ended time*/
        DalTimetick_GetTimetick64(hTimerSlavesHandle, &cb_processing_ended);
 
        /*to take care of the wrap around case*/
        if(cb_processing_ended >= cb_processing_started)
        {
          cb_processing_time = cb_processing_ended - cb_processing_started;       
        }
        else /*if it is a wrap around case*/
        {  
          cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
        }

        timers_expired_slave3[timer_expired_cnt_slave3].processing_time = cb_processing_time;
        timer_expired_cnt_slave3 = (timer_expired_cnt_slave3+1)%MAX_TIMER_EXPIRED_SLAVE3;
        timer_expired_cnt_global_slave3++;

        #ifdef TIMER_SLAVE_PRINT_HEALTH_MSG
        if(!(timer_expired_cnt_global_slave3%TIMER_SLAVE_DEBUG_MSG_TH))
        {
          print_health_msg_slave3=1;
        }
        else
        {
          print_health_msg_slave3 =0;
        }
        #endif

        /*generate a warningif the processing time > warning threshold*/
        if((cb_processing_time > timer_slave_processing_time_warning_threshold)||print_health_msg_slave3)
        {
          MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_slave_processing_time_warning_threshold);          
                    
        }

        /*generate a warningif the processing time > warning threshold*/
        if(timer_slave_processing_time_err_fatal_threshold != 0
           && cb_processing_time > timer_slave_processing_time_err_fatal_threshold)
        {
          ERR_FATAL("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_slave_processing_time_warning_threshold);          
                    
        }
        timer_after_pc_local_count++;
      }/*end while*/
      timer_reset_check_pc_mask_if_set(3,timer_after_pc_local_count);
    }/*end TIMER_SLAVE3_CMD_Q*/

  }/*end for*/

#endif /*FEATURE_TIMER_SLAVE3*/
} /* timer_slave3_task */


/** 
@ingroup Timer Slaves Functions 
         
@brief 
Initializes timer slave commmand queues

@return
None.

@param[in]                                                               
 None  

@param[out]
 None   
 
@dependencies  
Should be called before timer slave tasks are started
 
@sideeffects 

*/
void timer_slave_cmd_queue_init
(
  void
)
{
  int  tasks,queue_elements;

  for(tasks=0; tasks < MAX_SLAVE_TASKS; tasks++)
  {
    /*init the timer queue*/
    (void) q_init(&timer_slaves_cmd_q[tasks]);
    (void) q_init(&timer_slaves_cmd_free_q[tasks]);
  }

  /*loop through the slave tasks*/
  for(tasks = 0; tasks < MAX_SLAVE_TASKS; tasks++)
  {
    /*initialize the queue elements and place all the queue elements
     in free cmd queue*/
    for( queue_elements = 0; queue_elements < MAX_TIMER_SLAVE_CMD_ELEMENTS; queue_elements++ )
    {
      (void)q_link( &timer_slaves_cmd_buf[tasks][queue_elements], 
                    &timer_slaves_cmd_buf[tasks][queue_elements].link );
      q_put( &timer_slaves_cmd_free_q[tasks], 
             &timer_slaves_cmd_buf[tasks][queue_elements].link );
    }

    /*initialize the dog rpt timers to zero*/
    #ifdef FEATURE_DOG
    dog_timer_slaves_rpt[tasks] = 0;
    dog_timer_slaves_rpt_time[tasks] = 0;
    #endif /* FEATURE_DOG */

    /*initialize the bin boundaries to max*/
    timer_slave_bin_boundaries[tasks]=MAX_TASK_PRIORITY;    
  }

  /*At this point all the boundaries should be set at MAX.*/

  /*if FEATURE_TIMER_SLAVE2 is defined, then make a bin with boundaries [MAX/2, MAX]*/
  #ifdef FEATURE_TIMER_SLAVE2
  timer_slave_bin_boundaries[TIMER_SLAVE_TASK1]=MAX_TASK_PRIORITY/MAX_SLAVE_TASKS;
  #endif

  /*if FEATURE_TIMER_SLAVE3 is defined, then make bin with boundaries, [MAX/3, 2*MAX/3, MAX]*/
  #ifdef FEATURE_TIMER_SLAVE3
  timer_slave_bin_boundaries[TIMER_SLAVE_TASK1]=MAX_TASK_PRIORITY/MAX_SLAVE_TASKS;
  timer_slave_bin_boundaries[TIMER_SLAVE_TASK2]=(2*MAX_TASK_PRIORITY)/MAX_SLAVE_TASKS;
  #endif

  /*dummy assignments, so that the compiler does not optimize out
   these variables*/
  timer_slave_processing_time_warning_threshold = timer_slave_processing_time_warning_threshold;
  timer_slave_processing_time_err_fatal_threshold=timer_slave_processing_time_err_fatal_threshold;


}

/** 
@ingroup Timer Slaves Functions 
         
@brief 
Allocates memory from the command buffer of the specified
slave task

@return
None.

@param[in]                                                               
timer_slave_task_enum_type - slave task from which the memory is 
                             to be obtained

@param[out]
timer_slave_cmd_type* - pointer to cmd buffer memory
 
@dependencies  
None
 
@sideeffects 

*/
timer_slave_cmd_type  *timer_slave_get_cmd_buf
( 
  timer_slave_task_enum_type time_slave_task_num 
)
{
  timer_slave_cmd_type    *cmd_ptr=NULL;                           /* Pointer to command */
  static uint32           timer_slave_q_cnt=0;
  static uint32           timer_slave_free_q_cnt=0;
  static uint32           timer_slave_num_queried=0xFF;

  /*check for bounds on time_slave_task_num */
  if(time_slave_task_num >= MAX_SLAVE_TASKS)
  {
    MSG_2(MSG_SSID_ATS, MSG_LEGACY_ERROR,"time_slave_task_num = %d, max value should be less than = %d"
              ,time_slave_task_num,MAX_SLAVE_TASKS);
    return NULL;
  }

  /* Get a command buffer from the free command queue. */
  if(NULL == (cmd_ptr = q_get( &timer_slaves_cmd_free_q[time_slave_task_num] )) )
  {
    /*-----------------------------------------------------------------------
        No free command buffers available, something is seriously wrong.
    -----------------------------------------------------------------------*/
    timer_slave_num_queried=time_slave_task_num;
    timer_slave_q_cnt =  q_cnt(&timer_slaves_cmd_q[time_slave_task_num]);
    timer_slave_free_q_cnt = q_cnt(&timer_slaves_cmd_free_q[time_slave_task_num]);
    
    ERR_FATAL("No items on time slave task free cmd q on %d,Q_ele=%d,Free_Q=%d", 
              timer_slave_num_queried, 
              timer_slave_q_cnt, 
              timer_slave_free_q_cnt);
  }  
 
  return( cmd_ptr );

} /* timer_slave_get_cmd_buf */

/** 
@ingroup Timer Slaves Functions 
         
@brief 
Post commands on appropriate slave task

@return
None.

@param[in]                                                               
* timer_slave_post_cmd_params_type* - ptr to command
*                                     structure 

@param[out]
timer_slave_operation_status_type - status of the operation
 
@dependencies  
None
 
@sideeffects 

*/
timer_slave_operation_status_type timer_slave_post_cmd
(  
  timer_slave_post_cmd_params_type* slave_cmd_ptr
)
{
  timer_slave_cmd_type* timer_slave_cmd_ptr = NULL;
  timer_slave_operation_status_type status = TIMER_SLAVE_RESULT_PASS;
  uint8 timer_slave_task_num;

  if(NULL == slave_cmd_ptr)
  {
    return TIMER_SLAVE_RESULT_FAIL;
  }

  timer_slave_task_num = slave_cmd_ptr->timer_slave_task_num;

  /*check for bounds on time_slave_task_num */
  if(timer_slave_task_num >= MAX_SLAVE_TASKS)
  {
    ERR_FATAL("time_slave_task_num = %d, max value should be less than = %d"
              ,timer_slave_task_num,MAX_SLAVE_TASKS,0);
    return TIMER_SLAVE_RESULT_FAIL;
  }

  /*get the cmd q buffer from the appropriate queue*/
  timer_slave_cmd_ptr = timer_slave_get_cmd_buf((timer_slave_task_enum_type)timer_slave_task_num);
  
  if(NULL == timer_slave_cmd_ptr)
  {
    ERR_FATAL("timer_slave_task_cmd_ptr == NULL",0,0,0);
    status= TIMER_SLAVE_RESULT_FAIL;
	return TIMER_SLAVE_RESULT_FAIL;
  }

  /*fill in the data for the cmd*/
  timer_slave_cmd_ptr->timer_slave_operation.timer = slave_cmd_ptr->timer_ext;
  timer_slave_cmd_ptr->call_back_type = slave_cmd_ptr->cb_type;
  timer_slave_cmd_ptr->timer_slave_operation.sig_obj = slave_cmd_ptr->sig_obj;
  timer_slave_cmd_ptr->timer_slave_operation.sig_mask = slave_cmd_ptr->sig_mask;
  timer_slave_cmd_ptr->timer_slave_operation.func_cb1 = slave_cmd_ptr->cb1;
  timer_slave_cmd_ptr->timer_slave_operation.func_cb2 = slave_cmd_ptr->cb2;
  timer_slave_cmd_ptr->timer_slave_operation.run_time = slave_cmd_ptr->run_time;
  timer_slave_cmd_ptr->timer_slave_operation.data = slave_cmd_ptr->data;

  /*link the cmd*/
  q_link(timer_slave_cmd_ptr, &timer_slave_cmd_ptr->link);

  /*put the message on queue and signal the task*/
  q_put(&timer_slaves_cmd_q[timer_slave_task_num] , &timer_slave_cmd_ptr->link);

  /*set the signal on the queue*/
  qurt_signal_set(&timer_slaves_qurt_futex[timer_slave_task_num],TIMER_SLAVE_CMD_Q_SIG);
    
  return status;
}

/** 
@ingroup Timer Slaves Functions 
         
@brief 
Gets the priorities boundaries that decide the
assignment of slave task

@return
dword The cutoff priority for the bin specified

@param[in]                                                               
  timer_slave_task_enum_type - bin specified

@param[out]
None
 
@dependencies  
None
 
@sideeffects 

*/
dword timer_slave_get_priority_boundary
(
  timer_slave_task_enum_type bin
)
{
  if(bin < MAX_SLAVE_TASKS)
  {
    return timer_slave_bin_boundaries[bin];
  }

  return 0;
}

/** 
@ingroup Timer Slaves Functions 
         
@brief 
Assigns slave task to the timer during timer_def

@return
None

@param[in]                                                               
   timer_ptr_type - pointer to timer that needs to be
                assigned a slave

@param[out]
None
 
@dependencies  
FEATURE_TIMER_SLAVE1 should be defined
 
@sideeffects 

*/
uint8 timer_assign_slave_task_to_timer ( void )
{ 
#ifdef FEATURE_TIMER_SLAVE1
  
  uint8 slave_bin = 0;
    
  /*query for the tcb's priority*/
  int task_priority = qurt_thread_get_priority(qurt_thread_get_id());
    
  /*for REX/L4 targets, the priority are in a reverse order*/
  #ifndef FEATURE_QDSP6
  task_priority = MAX_TASK_PRIORITY - task_priority;  
  #endif
      
  /*assign the slave task to the timer during timer def*/
  for(slave_bin=TIMER_SLAVE_TASK1; slave_bin < MAX_SLAVE_TASKS; slave_bin++)
  {
    if(task_priority <= timer_slave_get_priority_boundary
                        ((timer_slave_task_enum_type)slave_bin))
    {
      timer_log_task_pri[pri_counter].pri=task_priority;
      timer_log_task_pri[pri_counter].bin=slave_bin;
      pri_counter = (pri_counter+1)%MAX_TASK_PRI_COUNTER;
      break;
    }  
  }

  return slave_bin;

#endif
}
/** 

@ingroup Timer Slaves Functions

@brief 

Initializes slave tasks 

@return

None

@param[in]                                                               
None

@param[out]
None

@dependencies  

FEATURE_TIMER_SLAVE<X> should be defined to
start the timer_slave<x> task

@sideeffects 
Should be started before timer task
*/
void timer_slave_task1_init
(
  void
)
{
  RCINIT_INFO handle = rcinit_lookup("tmr_slave1");

  #ifdef FEATURE_DOG
  timer_slaves_qurt_tid[TIMER_SLAVE_TASK1] = qurt_thread_get_id();
  #endif
  
  qurt_signal_init(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK1]);

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, timer_slave1_task);
  }
}

void timer_slave_task2_init
(
  void
)
{
  RCINIT_INFO handle = rcinit_lookup("tmr_slave2");

  #ifdef FEATURE_DOG
  timer_slaves_qurt_tid[TIMER_SLAVE_TASK2] = qurt_thread_get_id();
  #endif
  
  qurt_signal_init(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK2]);


  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, timer_slave2_task);
  }
}

void timer_slave_task3_init
(
  void
)
{
  RCINIT_INFO handle = rcinit_lookup("tmr_slave3");

  #ifdef FEATURE_DOG
  timer_slaves_qurt_tid[TIMER_SLAVE_TASK3] = qurt_thread_get_id();
  #endif
  
  qurt_signal_init(&timer_slaves_qurt_futex[TIMER_SLAVE_TASK3]);


  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, timer_slave3_task);
  }
}

/*=============================================================================

FUNCTION TIMER_SLAVES_GET_TIMETICK_HANDLE

DESCRIPTION
 This function is called from time_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_slaves_get_timetick_handle(void **hTimerHandlePtr)
{
  DalTimetick_Attach("SystemTimer", &hTimerSlavesHandle);
  ASSERT(hTimerSlavesHandle != NULL);
  if(hTimerHandlePtr != NULL)
  {
    *hTimerHandlePtr = hTimerSlavesHandle;
  }
}

