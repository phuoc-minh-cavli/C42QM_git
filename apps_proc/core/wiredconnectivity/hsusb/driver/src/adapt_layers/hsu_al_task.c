/*==============================================================================

                          H S U _ A L _ T A S K . C


GENERAL DESCRIPTION
  High-Speed USB Implementation of the Adaptation Layer Command Task.

  The Adaptation Layer Command Task is a Rex task in charge of executing 
  High-Speed USB adaptation layer commands.
  An adaptation layer command is a code section which needs to be executed
  after the occurrence of a Data Mover interrupt, but can't be executed within
  the interrupt context itself (for an example, when this code section 
  is blocking). Therefore, the interrupt queues a command in the commands queue.
  Later on, another task, the adaptation layer task, dequeues this item from 
  the command queue and executes the command outside of the interrupt context.

  This task manages a cyclic queue of commands. Each queue entry contains
  a function address and an actual parameter for this function, which is 
  of type void*.
  When commands are inserted into the queue (using the 'hsu_al_task_enqueue_cmd'
  function), a signal is sent to this task, and wakes it up. Then, this task 
  processes all the queued commands in a FIFO order. For each queued command,
  this task calls the supplied function, passing it its argument.

  While this task is processing command queue items, the Giant mutex is locked.
  When this task finishes processing the queue (i.e., it becomes empty 
  again), this task releases the giant mutex and returns waiting for its
  signal.

  The command queue is implemented using a cyclic buffer which has a head index
  and a tail index. The tail index points to the location of the next inserted
  item, and the head points to the location of the next popped item.
  When the head == tail, the queue is empty, and when head == tail + 1, the 
  queue is full. This way, the queue may be managed without a mutex protection
  since no item counter is needed, and there is no simultaneous access to 
  the same end of the queue.


EXTERNALIZED FUNCTIONS
  Externalized to outside of the High_Speed USB stack adaptation layer:
    hsu_al_task_main

  Externalized internally in the High_Speed USB stack adaptation layer:
    hsu_al_task_enqueue_cmd


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The High-Speed USB adaptation layer task need to be created before 
  using the API defined in this file.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*===========================================================================

EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_task.c#1 $
  $DateTime: 2020/01/30 22:49:35 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/06   ds      Created.

===========================================================================*/


/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

/* AMSS include files. */
#include <dog.h>
//#include <task.h>

/* High Speed USB subsystem include files. */
#include <hsu_common_int.h>
#include <port_int.h>

/* Local include files. */
#include "hsu_al_task_int.h"

#include "hsu_phy_utils_int.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_al_ecm_int.h"
#include "hsu_al_ser_base_int.h"

#include "hsu_core_config_int.h"
#include "jslave_init.h" /* For jslave_wakeup_host */

#include "queue.h"
#include "hsu_log.h"

#include "hsu_conf_sel_i.h" /* For hsu_conf_sel_hs_main_core_enabled */
#include "hsu_bam_sm.h"
#include "qurt_interrupt.h"
#include "timer.h"

static unsigned long hsu_intlock;

/*==============================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

enum
{
  /* The queue must be large enough to contain entries for all of its clients:
  ** Currently, only CDC adaptation layers utilize this queue, and they do so 
  ** only for Rx and Tx commands.
  ** For each CDC adaptation layer (e.g., hsu_al_ser, hsu_al_ecm) there is a 
  ** maximum of 4 entries per instance (up to 2 for Rx, in case of Rx double 
  ** buffering, and up to 2 for Tx if double buffering is supported for Tx).
  ** 
  ** For hsu_al_ecm, there is a maximum of 3 RmNET instances, so this totals to 
  ** 3*4=12 entries.
  **
  ** For hsu_al_ser:
  ** There is a maximum of 2 CDC/OBEX instances, and only one CDC/ACM instance.
  ** So this totals to 12 entries.
  **
  ** Total number of entries is therefore 24, and an elbow room of 8 entries is 
  ** added.
  ** 
  ** To allow al_ecm to enqueue commands and to support small packets over ECM
  ** we increase the queue to 32*6=192
  */
  AL_TASK_COMMAND_QUEUE_SIZE = 192
};

/* NOTE - These signals must not conflict with the High-Speed USB stack 
** OS abstraction layer signals (defined in 'hsu_os_abstract_sync.h'). 
** This is because this task calls user-supplied functions which may in 
** turn call High-Speed USB stack core API, which may internally use the 
** OS abstraction layer API.
*/
enum
{
  AL_TASK_DOG_RPT_TIMER_SIG = 0x00000001,
  AL_TASK_CMD_PENDING_SIG   = 0x00000002,
  AL_TASK_DSM_QUEUE_SIG   = 0x00000004,
  AL_TASK_REMOTE_WAKEUP_SIG   = 0x00000008,
};

/*==============================================================================
  Typedefs
==============================================================================*/

/* AL task command type, as stored in the AL task 
** command queue.
*/
typedef struct 
{
  /* Function to execute. */
  hsu_al_task_cmd_func_type  command_func;

  /* Argument to be passed to the function. */
  void*                      command_arg;

  boolean acquire_giant_mutex;

} al_task_command_type;

/* AL task information. */
typedef struct 
{
#ifdef USB_THREADX
  //qurt_timer_attr_t hsu_al_task_timer_attr;
  //qurt_timer_t hsu_al_task_rpt_timer;
  void * hsu_al_task_timer_attr;
  timer_type hsu_al_task_rpt_timer;
#else


  /* AL task Dog report timer.
  */
  rex_timer_type hsu_al_task_rpt_timer;
#endif

  uint32         hsu_al_task_rpt_id;          /* dog report ID */
  timetick_type  hsu_al_task_rpt_period;      /* dog report Period */

  /* A cyclic queue of commands to be executed by the AL task.
  */
  al_task_command_type  command_queue[AL_TASK_COMMAND_QUEUE_SIZE];

  uint32                command_queue_head;
  uint32                command_queue_tail;

  /* DSM item queue.
  */
  q_type dsm_free_queue;
  q_type dsm_rx_queue;
  q_type rb_free_queue;

  /* Queue for DSM control message receieved to be forward to APP */
  q_type dsm_ecm_control_rx_queue;

  /* Queue for DSM message receieved to be forward to APP for MDM */
  q_type dsm_ser_rx_queue;
  
  // Maintain ACM RX queue separately
  q_type dsm_acm_rx_queue;

  q_type rb_rx_queue;
} al_task_info_type;

typedef struct 
{
  al_task_info_type al_task_info;

} file_static_vars_type;

/*==============================================================================
  Externalized Variables
==============================================================================*/
/* MDM9X15 uses RCINIT mechanism which defines the tcb and stack in tmc*/


// 9x25 BU
//rex_tcb_type   hsu_al_task_tcb;
//unsigned char  hsu_al_task_stack[AL_TASK_STACK_SIZE_IN_BYTES];


/*==============================================================================
  File Static Variables
==============================================================================*/

static file_static_vars_type hsu_al_task;

static file_static_vars_type* const
  file_static_vars_ptr = &hsu_al_task;

qurt_signal_t hsu_al_sigs;
qurt_thread_t hsu_al_id;

/*==============================================================================

                             FUNCTION DEFINITIONS

==============================================================================*/
#ifndef USB_THREADX // rcinit will cover this
boolean hsu_al_thread_create(void)
{
  qurt_thread_attr_t hsu_al_attr;

  qurt_signal_init(&hsu_al_sigs);
  
  qurt_thread_attr_init(&hsu_al_attr);
  qurt_thread_attr_set_name(&hsu_al_attr, "hsu_al_task");
  qurt_thread_attr_set_priority(&hsu_al_attr, 147); //low-priority thread

  qurt_thread_attr_set_stack_size(&hsu_al_attr, AL_TASK_STACK_SIZE_IN_BYTES);

  if(qurt_thread_create(&(hsu_al_id), &hsu_al_attr, hsu_al_task_main, NULL) != QURT_EOK)
  {
     return FALSE;
  }
  return TRUE;
}
#endif



/*===========================================================================
FUNCTION HSU_AL_TASK_ENQUEUE_CMD_EX

DESCRIPTION
  Enters a new command into the High-Speed USB adaptation layer
  command queue, for execution by the High-Speed USB adaptation layer task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_enqueue_cmd_ex(hsu_al_task_cmd_func_type  command_func,
                                void*                      command_arg,
                                boolean                    acquire_giant_mutex)
{
  al_task_info_type* const
    al_task_info_ptr       = &(file_static_vars_ptr->al_task_info);

  al_task_command_type* const
    command_queue_ptr      = al_task_info_ptr->command_queue;

  uint32 const
    command_queue_head     = al_task_info_ptr->command_queue_head;

  uint32* const
    command_queue_tail_ptr = &(al_task_info_ptr->command_queue_tail);


  HSU_ASSERT(command_func != NULL);

  /* Ensure atomic operation of this function, as it modifies global
  ** structures.
  ** There are scenarios in which while an interrupt is executing this function,
  ** another higher priority interrupt is invoked and wants to execute this 
  ** function, too.
  */
  INTLOCK_SAVE(hsu_intlock)
  
  /* Integrity check. */
  HSU_ASSERT(command_queue_ptr[*command_queue_tail_ptr].command_func == NULL);
  HSU_ASSERT(command_queue_ptr[*command_queue_tail_ptr].command_arg  == NULL);

  /* If the command queue is full, generate an assertion. */
  HSU_ASSERT (
               !(
                 ((*command_queue_tail_ptr + 1) == command_queue_head)    ||
                 ((*command_queue_tail_ptr == AL_TASK_COMMAND_QUEUE_SIZE - 1) && 
                  (command_queue_head == 0))
                )
             );

  command_queue_ptr[*command_queue_tail_ptr].command_func = command_func;
  command_queue_ptr[*command_queue_tail_ptr].command_arg  = command_arg;
  command_queue_ptr[*command_queue_tail_ptr].acquire_giant_mutex = acquire_giant_mutex;

  *command_queue_tail_ptr = *command_queue_tail_ptr + 1;

  /* Wrap around. */
  if (*command_queue_tail_ptr == AL_TASK_COMMAND_QUEUE_SIZE)
  {
    *command_queue_tail_ptr = 0;
  }

  INTFREE_RESTORE(hsu_intlock)

  qurt_signal_set(&hsu_al_sigs, AL_TASK_CMD_PENDING_SIG);
}  /* hsu_al_task_enqueue_cmd_ex */

/*===========================================================================
FUNCTION HSU_AL_TASK_ENQUEUE_CMD

DESCRIPTION
  Enters a new command into the High-Speed USB adaptation layer
  command queue, for execution by the High-Speed USB adaptation layer task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_enqueue_cmd(hsu_al_task_cmd_func_type  command_func,
                             void*                      command_arg)
{
  hsu_al_task_enqueue_cmd_ex(command_func, command_arg, TRUE);
}  /* hsu_al_task_enqueue_cmd */

/*===========================================================================
FUNCTION HSU_AL_TASK_DOG_RPT

DESCRIPTION
  This function kicks the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_al_task_dog_rpt(void)
{
  if (file_static_vars_ptr->al_task_info.hsu_al_task_rpt_period > 0)
  {
#ifndef USB_THREADX
    dog_report(file_static_vars_ptr->al_task_info.hsu_al_task_rpt_id); 

    (void)rex_set_timer(&(file_static_vars_ptr->
                        al_task_info.hsu_al_task_rpt_timer),
                        file_static_vars_ptr->al_task_info.hsu_al_task_rpt_period);
#else
    dog_report(file_static_vars_ptr->al_task_info.hsu_al_task_rpt_id);  

    timer_set(&file_static_vars_ptr->al_task_info.hsu_al_task_rpt_timer,
              file_static_vars_ptr->al_task_info.hsu_al_task_rpt_period, 0, T_MSEC);

#endif
  }
} /* hsu_al_task_dog_rpt */


/*===========================================================================
FUNCTION HANDLE_AL_TASK_COMMANDS

DESCRIPTION
  Executes the command queue for the High-Speed USB stack adaptation 
  layer task.

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
static void handle_al_task_commands(void)
{
  al_task_info_type* const
    al_task_info_ptr       = &(file_static_vars_ptr->al_task_info);

  al_task_command_type* const
    command_queue_ptr      = al_task_info_ptr->command_queue;

  uint32* const
    command_queue_head_ptr = &(al_task_info_ptr->command_queue_head);

  uint32* const
    command_queue_tail_ptr = &(al_task_info_ptr->command_queue_tail);

  void*                      command_arg  = NULL;
  hsu_al_task_cmd_func_type  command_func = NULL;
  boolean                    acquire_giant_mutex;

  /* While the command queue is not empty,
  ** execute all the queued commands.
  */
  while(*command_queue_head_ptr != *command_queue_tail_ptr)
  {
    /* Copy command entry data. */
    command_func = command_queue_ptr[*command_queue_head_ptr].command_func;
    command_arg  = command_queue_ptr[*command_queue_head_ptr].command_arg;
    acquire_giant_mutex  = command_queue_ptr[*command_queue_head_ptr].acquire_giant_mutex;

    /* Clear the command entry. */
    command_queue_ptr[*command_queue_head_ptr].command_func = NULL;
    command_queue_ptr[*command_queue_head_ptr].command_arg  = NULL;

    *command_queue_head_ptr = *command_queue_head_ptr + 1;

    if (*command_queue_head_ptr == AL_TASK_COMMAND_QUEUE_SIZE)
    {
      *command_queue_head_ptr = 0;
    }

    if (acquire_giant_mutex == TRUE)
    {
      /* Acquire the Giant Mutex. */
      jsafe_enter();
    }

    /* calling the command function */
    command_func(command_arg);
    if (acquire_giant_mutex == TRUE)
    {
      /* Release the Giant Mutex. */
      jsafe_leave();
    }
  }  /* while (*command ...) */

} /* handle_al_task_commands */


/*===========================================================================
FUNCTION HSU_AL_TASK_MAIN

DESCRIPTION
  The Adaptation Layer Command Task's main function.
  
  Infinitely waits for new AL task commands to be entered into the command
  queue (this event is notified by a Rex signal) and executes them.
  Additionally, this function handles common AMSS signals (Dog 
  task signals, offline, stop).

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_main
(
  void* param
)
{
  #ifndef USB_THREADX
  rex_sigs_type received_signals;
  #else
  uint32 received_signals;  
  #endif
  al_task_info_type* const
    al_task_info_ptr  = &(file_static_vars_ptr->al_task_info);


  HSU_USE_PARAM(param);

  /*-------------------------------*/
  /* Command queue initialization. */ 
  /*-------------------------------*/

  /* Clear the command queue. */
  memset(al_task_info_ptr->command_queue,
         NULL,
         sizeof(al_task_info_ptr->command_queue));

  al_task_info_ptr->command_queue_head = 0;
  al_task_info_ptr->command_queue_tail = 0;

  /*-------------------------------*/
  /*  DSM queue initialization.    */ 
  /*-------------------------------*/

  q_init(&al_task_info_ptr->dsm_free_queue);
  q_init(&al_task_info_ptr->dsm_rx_queue);
  q_init(&al_task_info_ptr->dsm_ecm_control_rx_queue);
  q_init(&al_task_info_ptr->dsm_ser_rx_queue);
  q_init(&al_task_info_ptr->dsm_acm_rx_queue);
  q_init(&al_task_info_ptr->rb_rx_queue);

  qurt_signal_init(&hsu_al_sigs);

  /*-------------------*/
  /* Set the Dog timer */
  /*-------------------*/
  al_task_info_ptr->hsu_al_task_rpt_id = dog_register_qurt(hsu_al_id,
     DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE) ;          /* ID */
  al_task_info_ptr->hsu_al_task_rpt_period = 
    dog_get_report_period(al_task_info_ptr->hsu_al_task_rpt_id ); /* Report Period */

  if (al_task_info_ptr->hsu_al_task_rpt_period > 0)
  {
#ifndef USB_THREADX
    rex_def_timer(&(al_task_info_ptr->hsu_al_task_rpt_timer),
                  &hsu_al_task_tcb,
                  (rex_sigs_type)AL_TASK_DOG_RPT_TIMER_SIG);

    (void)rex_set_timer(&(al_task_info_ptr->hsu_al_task_rpt_timer),
                        al_task_info_ptr->hsu_al_task_rpt_period);
#else
    timer_def_osal(&al_task_info_ptr->hsu_al_task_rpt_timer,
                     NULL, 
                     TIMER_NATIVE_OS_SIGNAL_TYPE,
                     &hsu_al_sigs,
                     AL_TASK_DOG_RPT_TIMER_SIG);

    timer_set(&al_task_info_ptr->hsu_al_task_rpt_timer,
              al_task_info_ptr->hsu_al_task_rpt_period, 0, T_MSEC);

#endif
  }


  /*----------------------------------------------*/
  /* Signal the TMC task that we are ready to go. */
  /*----------------------------------------------*/

  rcinit_handshake_startup();

  /*--------------------------------------*/
  /* Loop forever, handling task signals. */
  /*--------------------------------------*/

  for (;;)
  {
    received_signals = 

    qurt_signal_wait(&hsu_al_sigs, 
               AL_TASK_DSM_QUEUE_SIG | 
               AL_TASK_CMD_PENDING_SIG   |
               AL_TASK_DOG_RPT_TIMER_SIG |
               AL_TASK_REMOTE_WAKEUP_SIG,
               QURT_SIGNAL_ATTR_WAIT_ANY); 
               //TASK_STOP_SIG             |  /* Stop the task */ //TODO: get the eqv
               //TASK_OFFLINE_SIG,            /* Go off line   */ //TODO: get the eqv

   /* Handle a dog report signal. */
   if (received_signals & AL_TASK_DOG_RPT_TIMER_SIG)
   {
      (void)qurt_signal_clear(&hsu_al_sigs, AL_TASK_DOG_RPT_TIMER_SIG);

      /* Kick the watchdog */
      hsu_al_task_dog_rpt();
    }

#ifndef USB_THREADX
    /* Handle a Stop signal. */
    if (received_signals & TASK_STOP_SIG)
    {
      qurt_signal_clear(&hsu_al_sigs, TASK_STOP_SIG);

      if (hsu_conf_sel_hs_main_core_enabled ())
      {	      
        /* Disconnect the USB from the host now. */
        (void) hsu_phy_utils_spoof_disconnect_core(HSU_CORE_CONFIG_USB_IDX);
      }

      if (al_task_info_ptr->hsu_al_task_rpt_period > 0)
      {
        (void)timer_clr(&al_task_info_ptr->hsu_al_task_rpt_timer, TMSEC);
      }
#ifdef DOG_DYNAMIC_API
      dog_deregister(al_task_info_ptr->hsu_al_task_rpt_id);
#endif /* ! DOG_DYNAMIC_API */
    }
#endif //USB_THREADX

#ifndef USB_THREADX

    /* Handle an Offline signal. */
    if (received_signals & TASK_OFFLINE_SIG)
    {
      qurt_signal_clear(&hsu_al_sigs, TASK_OFFLINE_SIG);
    }
#endif //USB_THREADX

    /* Handle a remote wakeup handler */
    if (received_signals & AL_TASK_REMOTE_WAKEUP_SIG)
    {
      (void)qurt_signal_clear(&hsu_al_sigs, AL_TASK_REMOTE_WAKEUP_SIG);
      jsafe_enter();
      //HSU_MSG_HIGH("Handling AL_TASK_REMOTE_WAKEUP_SIG");

      HSU_ULOG(HIGH_MSG, BUS_LOG, "al_task_remote_wakeup_sig");

      (void)jslave_wakeup_host(HSU_CORE_IDX_PRIMARY);
      jsafe_leave();
    }
    
    /* Handle a Command Pending signal. */
    if (received_signals & AL_TASK_CMD_PENDING_SIG)
    {
      (void)qurt_signal_clear(&hsu_al_sigs, AL_TASK_CMD_PENDING_SIG);
      handle_al_task_commands();
    }

    /* Handle a DSM queue Pending signal. */
    if (received_signals & AL_TASK_DSM_QUEUE_SIG)
    {
      (void)qurt_signal_clear(&hsu_al_sigs, AL_TASK_DSM_QUEUE_SIG);

      /* Forward dsm packet in ecm control rx queue */
      while(q_cnt(&al_task_info_ptr->dsm_ecm_control_rx_queue) != 0)
      {
        dsm_item_type * dsm_ptr = (dsm_item_type *)
          q_get(&al_task_info_ptr->dsm_ecm_control_rx_queue);
        HSU_ASSERT(dsm_ptr); /* This should never happen */
        hsu_al_ecm_forward_control_rx_to_app(dsm_ptr);
        dog_report(al_task_info_ptr->hsu_al_task_rpt_id); 
      }
    
      /* Free rb packet in rb free queue */
      while(q_cnt(&al_task_info_ptr->dsm_free_queue) != 0)
      {
        rb_t * rb = (rb_t *)q_get(&al_task_info_ptr->rb_free_queue);
        rb_queue_free(rb, FALSE);
        dog_report(al_task_info_ptr->hsu_al_task_rpt_id); 
      }

      /* Forward dsm packet in rx queue */
      if (hsu_bam_sm_is_rx_forward_to_app_allowed())
      {
        while(q_cnt(&al_task_info_ptr->rb_rx_queue) != 0)
        {
          rb_t * rb = (rb_t *)q_get(&al_task_info_ptr->rb_rx_queue);
          HSU_ASSERT(rb);
          hsu_al_ecm_forward_rx_to_app(rb); /* This should never happen */
          dog_report(al_task_info_ptr->hsu_al_task_rpt_id); 
        }
      }

      /* Forward dsm packet in ser rx queue */
      while(q_cnt(&al_task_info_ptr->dsm_ser_rx_queue) != 0)
      {
        dsm_item_type * dsm_ptr = (dsm_item_type *)
          q_get(&al_task_info_ptr->dsm_ser_rx_queue);
        HSU_ASSERT(dsm_ptr);
        hsu_al_ser_base_forward_rx_to_app(dsm_ptr);
        dog_report(al_task_info_ptr->hsu_al_task_rpt_id); 
      }

      /* Forward dsm packet in acm rx queue */
      while(q_cnt(&al_task_info_ptr->dsm_acm_rx_queue) != 0)
      {
        static jbool_t print_f3 = TRUE;
        hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr;
        dsm_item_type * dsm_ptr = (dsm_item_type *)
          q_check(&al_task_info_ptr->dsm_acm_rx_queue);
        HSU_ASSERT(dsm_ptr);

        ser_if_data_ptr = (hsu_al_ser_base_data_for_ser_if *)dsm_ptr->app_ptr;
        
        if(ser_if_data_ptr->rx_info.inbound_flow_enabled)
        {
          dsm_ptr = (dsm_item_type *)q_get(&al_task_info_ptr->dsm_acm_rx_queue);
          hsu_al_ser_base_forward_rx_to_app(dsm_ptr);
          dog_report(al_task_info_ptr->hsu_al_task_rpt_id); 
          print_f3 = TRUE;
        }
        else
        {
          if(print_f3 == TRUE)
          {
            HSU_MSG_LOW("ACM Inbound Flow Disabled! Cannot forward packets.");
            print_f3 = FALSE;
          }
          break;
        }
      }
    }
  }  /* for (;;) */

}  /* hsu_al_task_main */

/*===========================================================================
FUNCTION hsu_al_task_free_dsm

DESCRIPTION
  Put dsm item into dsm free queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_free_dsm(dsm_item_type * dsm_ptr)
{
  q_link(dsm_ptr, &dsm_ptr->link);
  q_put(&hsu_al_task.al_task_info.dsm_free_queue, &dsm_ptr->link);
  qurt_signal_set(&hsu_al_sigs,AL_TASK_DSM_QUEUE_SIG);
} /* hsu_al_task_free_dsm */

/*===========================================================================
FUNCTION hsu_al_task_free_rb

DESCRIPTION
  Put dsm item into dsm free queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_free_rb(rb_t * rb)
{
  q_link(rb, &rb->link);
  q_put(&hsu_al_task.al_task_info.rb_free_queue, &rb->link);
  qurt_signal_set(&hsu_al_sigs,AL_TASK_DSM_QUEUE_SIG);
} /* hsu_al_task_free_dsm */


/*===========================================================================
FUNCTION hsu_al_task_forward_rx_dsm

DESCRIPTION
  Put dsm item into dsm rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_rx_dsm(rb_t * rb)
{
  q_link(rb, &rb->link);
  q_put(&hsu_al_task.al_task_info.rb_rx_queue, &rb->link);

  qurt_signal_set(&hsu_al_sigs,AL_TASK_DSM_QUEUE_SIG);

} /* hsu_al_task_forward_rx_dsm */

/*===========================================================================
FUNCTION hsu_al_task_forward_ecm_control_rx_dsm

DESCRIPTION
  Put dsm item into ecm control rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_ecm_control_rx_dsm(dsm_item_type * dsm_ptr)
{
  q_link(dsm_ptr, &dsm_ptr->link);
  q_put(&hsu_al_task.al_task_info.dsm_ecm_control_rx_queue, &dsm_ptr->link);

  qurt_signal_set(&hsu_al_sigs,AL_TASK_DSM_QUEUE_SIG);
} /* hsu_al_task_forward_ecm_control_rx_dsm */


/*===========================================================================
FUNCTION hsu_al_task_forward_ser_rx_dsm

DESCRIPTION
  Put dsm item into SER rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_ser_rx_dsm(dsm_item_type * dsm_ptr)
{
  q_link(dsm_ptr, &dsm_ptr->link);
  q_put(&hsu_al_task.al_task_info.dsm_ser_rx_queue, &dsm_ptr->link);

  qurt_signal_set(&hsu_al_sigs,AL_TASK_DSM_QUEUE_SIG);
} /* hsu_al_task_forward_acm_rx_dsm */

/*===========================================================================
FUNCTION hsu_al_task_forward_acm_rx_dsm

DESCRIPTION
  Put dsm item into ACM rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_acm_rx_dsm(dsm_item_type * dsm_ptr)
{
  if(dsm_ptr)
  {
    q_link(dsm_ptr, &dsm_ptr->link);
    q_put(&hsu_al_task.al_task_info.dsm_acm_rx_queue, &dsm_ptr->link);
  }

  qurt_signal_set(&hsu_al_sigs,AL_TASK_DSM_QUEUE_SIG);
} /* hsu_al_task_forward_acm_rx_dsm */


/*===========================================================================
FUNCTION hsu_al_task_signal_remote_wakeup

DESCRIPTION
  Sends signal to AL TASK to trigger remote wake-up

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_signal_remote_wakeup(void)
{
  qurt_signal_set(&hsu_al_sigs,AL_TASK_REMOTE_WAKEUP_SIG);
} /* hsu_al_task_signal_remote_wakeup */

