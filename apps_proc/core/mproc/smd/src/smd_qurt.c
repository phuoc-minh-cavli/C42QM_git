/*=========================================================================

                    Shared Memory Driver REX Source File

Define the normal REX task entry point and the OS support routines. The OS
support routines are implemented using REX functions.

 Copyright (c)  2005-2014,2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_qurt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/05   ptm     Initial version - derived from smd_interface.c#8
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "stringl.h"
#include "stdint.h"
#ifdef SMD_DSR_SUPPORTED
#include "dsr.h"
#endif
#if defined(FEATURE_RCINIT)
#include "rcevt.h"
#include "rcinit.h"
#endif

#include "DALDeviceId.h"
#include "DDIIPCInt.h"
#include "DDIInterruptController.h"

#include "smsm.h"

#include "smd_os.h"
#include "smd.h"
#include "smd_internal.h"
#include "smd_proc.h"
#include "smd_os_common.h"
#include "smd_main.h"
#include "smd_dsm.h"
#include "smd_lite.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#if defined (SMD_APPS_PROC)
#define SMD_SMSM_HOST_ENTRY SMSM_APPS_STATE
#elif defined (SMD_MODEM_PROC)
#define SMD_SMSM_HOST_ENTRY SMSM_MODEM_STATE
#elif defined (SMD_LPASS_PROC)
#define SMD_SMSM_HOST_ENTRY SMSM_Q6_STATE
#elif defined (SMD_RIVA_PROC)
#define SMD_SMSM_HOST_ENTRY SMSM_RIVA_STATE
#else
#define SMD_SMSM_HOST_ENTRY SMSM_INVALID_ENTRY
#endif

#define SMD_HIGH_THREAD_STACK_SIZE   4096
#define SMD_LOW_THREAD_STACK_SIZE    4096

#define SMD_DPC_RING_SIZE            0x10 /* Should be multiples of 2 */
#define SMD_DPC_RING_MASK            0xF  /* SMD_DPC_RING_SIZE - 1 */

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

/**
 * SMD context structure for each running task.
 *
 * Each task gets its own context structure, which includes all the
 * information the task needs to run. Each channel is assigned to a particular
 * context (and therefore to a particular task, at a particular priority) by
 * smd_assign_context().
 */
static smd_context_type smd_context[SMD_PRIORITY_COUNT][SMEM_NUM_HOSTS];

static struct
{
  q_type cmd_queue;
  void  *cmd_queue_cs;
}smd_cmd_queue[SMD_PRIORITY_COUNT];

/* The critical section for SMD Lite running in IST context */
static void *smd_isr_cs = NULL;

/* The critical section structure for the DTR function assignment */
static void *smd_dtr_cs = NULL;

/* The critical section structure for the IOCTL command count */
static void *smd_ioctl_cs = NULL;

/* The critical section structure for the Watermark Queue assignment */
static void *smd_dsm_cs = NULL;

/* The critical section that ensures incoming SMD interrupts are 
 * enabled/disabled atomically. */
static void *smd_inc_intr_cs = NULL;

/* DAL Outgoing Interrupt (IPCINT) device handle. */
static DalDeviceHandle *smd_intr_out_handle;

/* DAL Incoming Interrupt Controller device handle. */
static DalDeviceHandle *smd_intr_in_handle;

/** Table defining interrupts for each channel in the target */
smd_interrupt_info_type smd_interrupt_table[SMEM_NUM_HOSTS];

/** HIGH and LOW thread handles */
static qurt_thread_t smd_high_thread_handle;
static qurt_thread_t smd_low_thread_handle;

/** HIGH and LOW thread stacks */
#ifndef SMD_OS_QURT_THREADX
static char *smd_high_thread_stack = NULL;
static char *smd_low_thread_stack = NULL;
#endif

#ifdef SMD_DSR_SUPPORTED
/** The handle for DSR to handle remote updates. */
static dsr_obj_t *smd_dsr_handles[SMEM_NUM_HOSTS];

/** DSR for SMD DPC */
typedef struct {
  void (*func)(unsigned long);
  unsigned long param;
} smd_dpc_item_type;

static dsr_obj_t *smd_dpc_dsr_handle;
static smd_dpc_item_type smd_dpc_ring[SMD_DPC_RING_SIZE];
static uint32 smd_dpc_rd_indx;
static uint32 smd_dpc_wr_indx;
static void *smd_dpc_cs;
#endif 

void smd_task( dword params );
/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_handle_event_multi_proc
===========================================================================*/
/**
@brief        Call 'smd_handle_event' for all the processors that caused event
              Early return if no processor bit is on
              
@param[in]    event_sig   signal that has processor bit. 

              context     Pointer to the array of context 
@return       None
*/
/*=========================================================================*/
static void smd_handle_event_multi_proc
(
  smd_context_type *context,
  uint32 event_sig
)
{
  int i;
  /* Get bits indicate which proc caused an event */
  uint32 proc_bit = (event_sig >> SMD_EVENT_HOST_BIT_SHIFT) & 
                     SMD_ALL_PROC_MASK;
  
  /* Checking process_loopback in any one context is sufficient
   * since the only difference in this context is sorted open_list by host */
  if ( !proc_bit )
  {
    return;
  }
                     
  for (i = 0; i < SMEM_NUM_HOSTS; ++i)
  {    
    if (proc_bit & SMD_PROC_SIG_MASK)
    {
      smd_handle_event(&context[i]);  
    }
    proc_bit >>= 1;
  }
  
  if (context[SMD_THIS_HOST].process_loopback)
  {
#ifdef SMD_NWAY_LOOP_SERVER
    smd_loopback_server_update();
#endif

#ifdef SMD_NWAY_LOOP_CLIENT
    smd_loopback_client_update();
#endif
  }
}

/*===========================================================================
FUNCTION      smd_init_critical_sections
===========================================================================*/
/**
@brief        Sets up critical sections

              Initializes critical sections for DTR, IOCT and DSM.

@return       None
*/
/*=========================================================================*/
static void smd_init_critical_sections( void )
{
  /* Initialize critical section for DTR */
  smd_dtr_cs = smd_cs_create();
  
  /* Initialize critical section for IOCTL */
  smd_ioctl_cs = smd_cs_create();

  /* Initialize critical section for DSM */
  smd_dsm_cs = smd_cs_create();

  /* Initialize critical section for SMD Lite in IST context */
  smd_isr_cs = smd_cs_create();
  
  /* Initialize critical section for incoming SMD interrupt enable/disable. */
  smd_inc_intr_cs = smd_cs_event_create();
}

/*===========================================================================
FUNCTION      smd_dpc_worker

DESCRIPTION   This function calls user dpc functions

RETURN VALUE  None
===========================================================================*/
#ifdef SMD_DSR_SUPPORTED
static void smd_dpc_worker(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  void (*func)(unsigned long) = NULL;
  unsigned long param = 0;
  
  do {
    smd_cs_lock(smd_dpc_cs, TRUE);
    
    if (smd_dpc_rd_indx != smd_dpc_wr_indx)
    {
       func  = smd_dpc_ring[smd_dpc_rd_indx].func;
       param = smd_dpc_ring[smd_dpc_rd_indx].param;
       
       smd_dpc_rd_indx = (smd_dpc_rd_indx + 1) & SMD_DPC_RING_MASK;
    }
    smd_cs_lock(smd_dpc_cs, FALSE);
    
    if (func)
    {
      func(param);
    }
    
  } while(func);
}
#endif

/*===========================================================================
FUNCTION      smd_dpc_init

DESCRIPTION   Initialize the DPC

RETURN VALUE  None
===========================================================================*/
#ifdef SMD_DSR_SUPPORTED
static void smd_dpc_init(void)
{
  smd_dpc_cs = smd_cs_create();
  smd_dpc_rd_indx = 0;
  smd_dpc_wr_indx = 0;
  memset(smd_dpc_ring, 0, sizeof(smd_dpc_ring));

  /* Create DSR object for processing dpc */
  if (dsr_create(&smd_dpc_dsr_handle, 
                 smd_dpc_worker, 
                 NULL, 
                 DSR_PRI_HIGH, 
                 DSR_CONTEXT_WORKLOOP) == FALSE)
  {
    ERR_FATAL("smd_dpc_init: dsr_create failed!", 0, 0, 0);
  }
}
#endif

/*===========================================================================
  FUNCTION  smd_task_init
===========================================================================*/
/**
  This is the pre-initialization function for smd. It performs all
  initializations required to queue commands to the smd tasks.
  As a side note, based on the availability of the RCINIT Feature,
  it will also spawn the SMD default task which is responsible to
  spawn other SMD priority tasks.
  (This function may be called from the smd default task during other
  initialization if it is not called from RCINIT)

  @return        None

  @dependencies
  If RCINIT feature is enabled, then the properties for 'smdtask'
  are required to be set in mproc/smd/build/Sconscript.
*/
/*=========================================================================*/
void smd_task_init(void)
{
  qurt_signal_t *signal;
  int i;
  int j;
#ifdef FEATURE_RCINIT
  RCINIT_INFO handle = rcinit_lookup("smdtask");
#endif

  /* Initialize the critical sections */
  smd_init_critical_sections();

  /* Initialize the DPC */
#ifdef SMD_DSR_SUPPORTED
  smd_dpc_init();
#endif
  
  /* Initialize the SMD subsystems */
  smd_init();
  smd_wm_init();

  /* Initialize SMD context */
  for(i = 0; i < SMD_PRIORITY_COUNT; i++)
  {
    (void) q_init(&smd_cmd_queue[i].cmd_queue);
    smd_cmd_queue[i].cmd_queue_cs = smd_cs_create();

    signal = smd_malloc(sizeof(qurt_signal_t));
    memset(signal, 0, sizeof(qurt_signal_t));

    qurt_signal_init(signal);
    
    for(j = 0; j < SMEM_NUM_HOSTS; ++j)
    {
      smd_context[i][j].priority = (smd_priority_type)i;
      smd_context[i][j].cmd_queue = &smd_cmd_queue[i].cmd_queue;
      smd_context[i][j].cmd_queue_cs = smd_cmd_queue[i].cmd_queue_cs;
      smd_context[i][j].open_list = NULL;
      smd_context[i][j].os.signal = signal;
      smd_context[i][j].os.is_ist = FALSE;
      smd_context[i][j].process_loopback = FALSE;
    }
  }

  /* Handle IST-priority SMD Lite from the ISR */
  for(j = 0; j < SMEM_NUM_HOSTS; ++j)
  {
    smd_context[SMD_PRIORITY_IST][j].os.is_ist = TRUE;
  }

  /* Initialize SMD Lite API */
  smdl_init(0);

  smd_target_init();

#ifdef FEATURE_RCINIT
  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, smd_task);
  }
#endif
}

/*===========================================================================
FUNCTION      smd_task_loop
===========================================================================*/
/**
@brief        Runs the SMD main loop for a particular task context.

              This function waits for a SMD signal to be sent to this task,
              then calls smd_main() to service the channels maintained by this
              context.

@param[in]    context    Pointer to the array of SMD context structure 
                         for the task

@return       This function never returns.
*/
/*=========================================================================*/
static void smd_task_loop(smd_context_type *context)
{
  unsigned int smd_wait_sigs;
  unsigned int sigs;

  smd_wait_sigs = SMD_EVENT_APPS_SIG |
                  SMD_EVENT_MODEM_SIG |
                  SMD_EVENT_ADSP_SIG |
                  SMD_EVENT_WCN_SIG |
                  SMD_EVENT_RPM_SIG |
                  SMD_CMD_SIG |
                  SMD_SMSM_EVENT_SIG;

  /*
     Process the command queue, and other event updates. Note, the events
     or commands could have arrived before the task was started (Thus
     the signals might not have been set). Thus we need to first check
     these before waiting on the signals.
     
     Although context[SMD_THIS_HOST] is never used, it is okay to pass
     this as arg because cmd_queue in context is a pointer pointing to
     same smd_cmd_queue per priority
  */
  smd_handle_cmd(&context[SMD_THIS_HOST]);
  sigs = SMD_ALL_PROC_MASK << SMD_EVENT_HOST_BIT_SHIFT;
  smd_handle_event_multi_proc(context, sigs);
  
  while (1)
  {
    sigs = qurt_signal_wait(context->os.signal, smd_wait_sigs, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal_clear(context->os.signal, sigs);

    if ((sigs & SMD_CMD_SIG) != 0)
    {
      /* Process the command queue without iterating through every port. This
       * signal is sent from smd_cmd().
       * 
       * Although context[SMD_THIS_HOST] is never used, it is okay to pass
       * this as arg because cmd_queue in context is a pointer pointing to
       * same smd_cmd_queue per priority
       */
      smd_handle_cmd(&context[SMD_THIS_HOST]);
    }
    
    /* Checking for updates from other processors (only relevant ports). 
     * This signal is sent from smd_isr(). */          
    smd_handle_event_multi_proc(context, sigs);
  } /* while loop of task loop */
} /* smd_task_loop */

/*===========================================================================
FUNCTION      smd_handle_ist_events

DESCRIPTION   Handle events to IST-priority channels

PARAMETERS    event - an event mask, which directly translates to rex signals

RETURN VALUE  None
===========================================================================*/
static void smd_handle_ist_events(uint32 event)
{
  smd_context_type *ist_context;

  /* Handle IST-priority SMD Lite from the current task, which could be a
   * client (asking for the port to be opened or closed) or from the ISR
   * itself. To protect the integrity of the data structures, we need to
   * lock a critical section first.
   */
  smd_cs_lock(smd_isr_cs, TRUE);

  if ((event & SMD_CMD_SIG) != 0)
  {
    /* Process the command queue without iterating through every port. This
     * event is sent from smd_cmd().
     * Although smd_context[][SMD_THIS_HOST] will never be used, it is okay
     * to pass as arg since all contexts per priority points 
     * to same cmd_queue */
    smd_handle_cmd(&smd_context[SMD_PRIORITY_IST][SMD_THIS_HOST]);
  }

  /* Iterate through ports that are related to proper processors, 
   * checking for updates from other processors.
   * This event is sent from smd_isr(). */
  ist_context = (smd_context_type *)&smd_context[SMD_PRIORITY_IST];
  smd_handle_event_multi_proc(ist_context, event);

  smd_cs_lock(smd_isr_cs, FALSE);
}

/*===========================================================================
FUNCTION      smd_dsr

DESCRIPTION   This function is called when an DSR is enqueued.

RETURN VALUE  None
===========================================================================*/
#ifdef SMD_DSR_SUPPORTED
static void smd_dsr(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  smem_host_type remote_host = (smem_host_type)(uintptr_t)payload;
  uint32 proc_bit;
  
  ASSERT(remote_host < SMEM_NUM_HOSTS);

  proc_bit = smdi_get_proc_bit_mask(remote_host);
  smd_handle_ist_events(proc_bit << SMD_EVENT_HOST_BIT_SHIFT);
}
#endif

/*===========================================================================
FUNCTION      smd_isr

DESCRIPTION   Interrupt service routine for SMD. This interrupt is received
              when the other processor wants the SMD task to service some
              channel. The service required may be an update of control
              signals, data has been read and more should be sent, or data
              has been written and needs to be read.

              The ISR sends event sig to all relevent SMD tasks. Each task is
              scheduled at the appropriate priority by the RTOS and checks the
              channels assigned to it for work to be done.

RETURN VALUE  None
===========================================================================*/
static void smd_isr( smem_host_type remote_host )
{
  uint32 proc_bit;
  uint32 event_sig;
  int    i;

  ASSERT(remote_host < SMEM_NUM_HOSTS);
  
  proc_bit  = smdi_get_proc_bit_mask(remote_host);
  event_sig = proc_bit << SMD_EVENT_HOST_BIT_SHIFT;

#ifdef SMD_DSR_SUPPORTED
  if (smd_dsr_handles[remote_host] == NULL)
  {
    ERR_FATAL("smd_isr: smd_dsr_handles[] Invalid remote %d!", remote_host, 0, 0);
  }
#endif /* SMD_DSR_SUPPORTED */
    
  for (i = 0; i < SMD_PRIORITY_COUNT; i++)
  {
#ifdef SMD_DSR_SUPPORTED
    if (i == SMD_PRIORITY_IST)
    {
      dsr_enqueue(smd_dsr_handles[remote_host], (void*)remote_host);
      continue;
    }
#endif /* SMD_DSR_SUPPORTED */

    if (smd_context[i][remote_host].open_list || 
        smd_context[i][remote_host].process_loopback)
    {
      smd_event_send_smd_context(&smd_context[i][remote_host], event_sig);
    }
  }
} /* smd_isr */

/*===========================================================================
  FUNCTION      smd_register_interrupt
===========================================================================*/
/**
  Registers the SMD ISR for the incoming interrupt specified by intr.

  @param[in]  intr    The internal smd_interrupt_table entry of the
                      interrupt to register for.

  @return
  SMD_STATUS_SUCCESS        Interrupt registered successfully.
  SMD_STATUS_INVALID_PARAM  The interrupt ID was invalid, and not registered.
  SMD_STATUS_NOT_INIT       Internal structures and handles are not 
                            initialized yet.
*/
/*=========================================================================*/
static int32 smd_register_interrupt
(
  smd_interrupt_info_type *intr
)
{
  int32 ret_val;
  
  if( intr->irq_in != SMD_NULL_IN_INTERRUPT )
  {
    if( DalInterruptController_RegisterISR(
            smd_intr_in_handle,
            (uint32)intr->irq_in,
            (DALIRQ)smd_isr,
            (DALIRQCtx)intr->to,
            DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER ) != DAL_SUCCESS )
    {
      ret_val = SMD_STATUS_NOT_INIT;
    }
    ret_val = SMD_STATUS_SUCCESS;
  }
  else
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION      smd_deregister_interrupt
===========================================================================*/
/**
  Deregisters the SMD ISR which was previously registered for the incoming 
  interrupt specified by intr.

  @param[in]  intr    The internal smd_interrupt_table entry of the
                      interrupt to deregister.

  @return
  SMD_STATUS_SUCCESS        Interrupt deregistered successfully.
  SMD_STATUS_INVALID_PARAM  The interrupt ID was invalid, and not deregistered.
  SMD_STATUS_NOT_INIT       Internal structures and handles are not initialized yet, 
                            or interrupt was not registered.

*/
/*=========================================================================*/
static int32 smd_deregister_interrupt
(
  smd_interrupt_info_type *intr
)
{
  int32 ret_val;
  
  if( intr->irq_in != SMD_NULL_IN_INTERRUPT )
  {
    if( DalInterruptController_Unregister(
            smd_intr_in_handle,
            (uint32)intr->irq_in ) != DAL_SUCCESS )
    {
      ret_val = SMD_STATUS_NOT_INIT;
    }
    ret_val = SMD_STATUS_SUCCESS;
  }
  else
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  return ret_val;
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_event_send

DESCRIPTION   Send an event to the specified processor.

PARAMETERS    info - the port information pointer

RETURN VALUE  None
===========================================================================*/
void smd_event_send(smd_stream_info_type *info)
{
  /* make sure memory is consistent before sending interrupt */
  SMD_MEMORY_BARRIER();

  /* interrupt the other side for receive (read data I sent) */
  smd_send_interrupt(PORT_GET_REMOTE(info->port_id));
} /* smd_event_send */

/*===========================================================================
FUNCTION      smd_event_send_smd

DESCRIPTION   Send an event to the smd thread that handles a particular port

PARAMETERS    port_id - port id to send the event to

RETURN VALUE  None
===========================================================================*/
void smd_event_send_smd(smd_port_id_type port_id)
{
  smd_info_type *info;
  smem_host_type remote_host;
  uint32 event_sig;
  uint32 proc_bit;

  info = smdi_get_info(port_id);
  remote_host = smd_get_other_host( SMD_THIS_HOST, info->channel_type );
  proc_bit = smdi_get_proc_bit_mask( remote_host );
  event_sig = proc_bit << SMD_EVENT_HOST_BIT_SHIFT;

  smd_event_send_smd_context(info->context, event_sig);
} /* smd_event_send_smd */

/*===========================================================================
FUNCTION      smd_event_send_smd_context

DESCRIPTION   Send an event to the smd thread specified by context

PARAMETERS    context - SMD context to send the event to

              event - an event mask, which directly translates to rex signals

RETURN VALUE  None
===========================================================================*/
void smd_event_send_smd_context(smd_context_type *context, uint32 event)
{
  ASSERT(context != NULL);
  
  if(context->os.is_ist == FALSE)
  {
    qurt_signal_set(context->os.signal, event);
  }
  else
  {
    smd_handle_ist_events(event);
  }
} /* smd_event_send_smd_context */

/*===========================================================================
FUNCTION      smd_dtr_lock
===========================================================================*/
/**
@brief        Locks/unlocks DTR callback assignment critical section

              This function protects DTR callback assignment using critical
              sections.

@param[in]    lock    TRUE locks; FALSE unlocks

@return       None
*/
/*=========================================================================*/
void smd_dtr_lock(boolean lock)
{
  smd_cs_lock(smd_dtr_cs, lock);
}

/*===========================================================================
FUNCTION      smd_ioctl_lock
===========================================================================*/
/**
@brief        Locks/unlocks IOCTL command count critical section

              This function protects IOCTL command count using critical
              sections.

@param[in]    lock    TRUE locks; FALSE unlocks

@return       None
*/
/*=========================================================================*/
void smd_ioctl_lock(boolean lock)
{
  smd_cs_lock(smd_ioctl_cs, lock);
}

/*===========================================================================
FUNCTION      smd_dsm_lock
===========================================================================*/
/**
@brief        Locks/unlocks Watermark Queue assignment critical section

              This function protects Watermark Queue assignment using critical
              sections.

@param[in]    lock    TRUE locks; FALSE unlocks

@return       None
*/
/*=========================================================================*/
void smd_dsm_lock(boolean lock)
{
  smd_cs_lock(smd_dsm_cs, lock);
}

/*===========================================================================
FUNCTION      smd_cs_lock

DESCRIPTION   lock/unlock a critical section

PARAMETERS    cs - pointer the the critical section

              lock - lock or unlock the critical section

RETURN VALUE  None.
===========================================================================*/
void smd_cs_lock(void *cs, boolean lock)
{
  qurt_mutex_t *mutex = (qurt_mutex_t * )cs;

  if (lock)
  {
    qurt_mutex_lock(mutex);
  }
  else
  {
    qurt_mutex_unlock(mutex);
  }
}/* smd_cs_lock */

/*===========================================================================
FUNCTION      smd_cs_create

DESCRIPTION   Initializes a Critical Section

PARAMETERS    None

RETURN VALUE  The critical section.
===========================================================================*/
void *smd_cs_create(void)
{
  qurt_mutex_t *mutex;
  
  mutex = smd_malloc(sizeof(qurt_mutex_t));
  memset(mutex, 0, sizeof(qurt_mutex_t));
  
  qurt_mutex_init(mutex);
  
  return ((void *)mutex);
}/* smd_cs_create */

/*===========================================================================
FUNCTION      smd_cs_destroy

DESCRIPTION   Destroys a Critical Section

PARAMETERS    None

RETURN VALUE  none.
===========================================================================*/
void smd_cs_destroy(void *cs)
{
  qurt_mutex_t *mutex = (qurt_mutex_t * )cs;
  
  qurt_mutex_destroy(mutex);
  
  free(mutex);
}/* smd_cs_destroy */

/*===========================================================================
FUNCTION      smd_event_start_loopback

DESCRIPTION   This function sets the SMSM_SMD_LOOPBACK state for the
              local host.

PARAMETERS    None

RETURN VALUE  None
===========================================================================*/
void smd_event_start_loopback(void)
{
  /* let the other host know the loopback starts  */
  smsm_state_set(SMD_SMSM_HOST_ENTRY, SMSM_SMD_LOOPBACK);
} /* smd_event_start_loopback */

/*===========================================================================
FUNCTION smd_smsm_cb

DESCRIPTION
  This function is called when the remote processor changes its SMSM
  states.

PARAMETERS
  host            Which host the state change is for
  prev_state      State that host was previously in
  curr_state      State that host is currently in

DEPENDENCIES
  None

RETURN VALUE
  None

CONTEXT
  In ISR context.

===========================================================================*/
static void smd_smsm_cb
(
  smsm_entry_type  host,
  uint32           prev,
  uint32           curr,
  void            *data
)
{
  smd_cmd_type cmd;

  /* the other host asserts SMSM_SMD_LOOPBACK */
  if ((curr & SMSM_SMD_LOOPBACK) && (!(prev & SMSM_SMD_LOOPBACK)))
  {
    cmd.cmd = SMD_CMD_LOOPBACK;
    smd_cmd(&cmd);
  }
} /* smsm_smsm_cb */

/*===========================================================================
FUNCTION      SMD_TARGET_INIT

DESCRIPTION   Performs target specific initialization for SMD.

RETURN VALUE
===========================================================================*/
void smd_target_init(void)
{
  uint32 i;
  int32  ret_val;

  /* Create Outgoing IPCINT Interrupt Handle */
  if( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_IPCINT,
                                       &smd_intr_out_handle ) )
  {
    ERR_FATAL("SMD Target Init: IPCINT Handler creation failed!", 0, 0, 0);
  }

  /* Create Incoming Interrupt Controller Handle */
  if( ( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER,
                                         &smd_intr_in_handle ) )
          || (smd_intr_in_handle == NULL) )
  {
    ERR_FATAL("SMD Target Init: Interrupt Controller Handle creation failed!",
              0, 0, 0);
  }

  /* Populate SMD interrupt table */
  smd_target_init_interrupts(SMD_THIS_HOST);
  
  /* Make sure all SMD channels are closed on the local side.  This is to 
   * support distributed subsystem restart. */
  smdi_close_channels();

  /* Register incoming SMD interrupts */
  for( i = 0; i < SMEM_NUM_HOSTS; i++ )
  {
    smd_interrupt_info_type *intr = &(smd_interrupt_table[i]);
    
#ifdef SMD_DSR_SUPPORTED
    if (intr->irq_in != SMD_NULL_IN_INTERRUPT)
    {
      /* Create DSR object for processing the remote host update */
      if (dsr_create(&smd_dsr_handles[i], 
                     smd_dsr, 
                     NULL, 
                     DSR_PRI_HIGH, 
                     DSR_CONTEXT_ISR) == FALSE)
      {
        ERR_FATAL("smd_target_init: dsr_create failed! i=%d", i, 0, 0);
      }
    }
#endif
    ret_val = smd_register_interrupt(intr);
    /* If its not a valid interrupt, that's fine, ignore it. */
    if ( ret_val == SMD_STATUS_NOT_INIT )
    {
      ERR_FATAL("smd_target_init: SMD ISR for host %d failed to register.",
                  i, 0, 0);
    }
  }
}

/*===========================================================================
FUNCTION      smd_assign_context
===========================================================================*/
/**
@brief        Assigns an SMD context to the provided SMD port, based on the
              requested priority.

              This function must be called after populating the info structure
              from the API function that opens the SMD port, before calling
              smd_event_send_smd() to notify the particular task responsible
              for handling the port.

@param[in,out]  info    Pointer to the SMD info structure on which to assign
                        the SMD context. This function will populate
                        info->context with the appropriate task context.

@param[in]    priority  The priority requested by the client. If SMD is
                        implemented using multiple tasks with different OS
                        priorities for ports of different priorities, this
                        function will use this parameter to assign the context
                        to the appropriate task.

@return       None
*/
/*=========================================================================*/
void smd_assign_context( smd_info_type * info, smd_priority_type priority )
{
  smem_host_type remote_host;
  
  if(priority >= SMD_PRIORITY_COUNT)
  {
    ERR_FATAL("smd_assign_context: priority %d > SMD_PRIORITY_COUNT %d info %x", priority, SMD_PRIORITY_COUNT, info);
    return;
  }

  remote_host = smd_get_other_host(SMD_THIS_HOST, info->channel_type);
  if (remote_host == SMEM_INVALID_HOST)
  {
    ERR_FATAL("smd_assign_context: Invalid host found in this channel",
              0, 0, 0);
  }
  info->context = &smd_context[priority][remote_host];
}

/*===========================================================================
FUNCTION      smd_context_get_default
===========================================================================*/
/**
@brief        Returns a pointer to the default SMD context.

              This function is used by smd_cmd() to get the default SMD
              context used for handling loopback servers.

@return       Default SMD context
*/
/*=========================================================================*/
smd_context_type * smd_context_get_default(void)
{
  /* Although context[SMD_THIS_HOST] is never used, it is okay to pass
   * this as arg because cmd_queue in context is a pointer pointing to
   * same smd_cmd_queue per priority */  
  return &smd_context[SMD_PRIORITY_DEFAULT][SMD_THIS_HOST];
}

/*===========================================================================
FUNCTION      smd_cmd_broadcast
===========================================================================*/
/**
@brief        Send an SMD command to all SMD tasks

              This function iterates through the list of SMD tasks and calls
              smd_cmd_context() to send the command to each task.

              This function is implemented by the OS-specific code, which
              alone has visibility into the true number of tasks and which
              ports they correspond to.

@param[in]    cmd_ptr    Pointer of the command buffer to send

@return       None
*/
/*=========================================================================*/
void smd_cmd_broadcast(smd_cmd_type *cmd_ptr)
{
  int i;

  /* Although context[SMD_THIS_HOST] is never used, it is okay to pass
   * this as arg because cmd_queue in context is a pointer pointing to
   * same smd_cmd_queue per priority */
  for(i = 0; i < SMD_PRIORITY_COUNT; i++)
  {
    smd_cmd_context(cmd_ptr, &smd_context[i][SMD_THIS_HOST]);
  }
}

/*===========================================================================
FUNCTION      smd_task

DESCRIPTION   REX version of Shared Memory Driver's Task.

              Initializes data structures such as ISRs and critical sections.
              Then calls smd_main to handle normal operations.

RETURN VALUE  Never returns
===========================================================================*/
void smd_task(dword params)
{
  qurt_thread_attr_t attr;
  uint16  priority;
/* Keep Lint happy */
#ifdef _lint
  params = params;
#endif

#if defined(FEATURE_RCINIT)
  rcinit_handshake_startup();
#endif

  smsm_state_set(SMD_SMSM_HOST_ENTRY, SMSM_SMDINIT);
  
  /* Checking if SMSM_SMD_LOOPBACK is set on APPS side, if it is set,
     starting loop back. This scenario can happen when APPS initially
     set SMSM_SMD_LOOPBACK, and then Modem SSR happen, now again when
     loop back start on APPS via setting SMSM_SMD_LOOPBACK bit, Modem
     will not get smd_smsm_cb because bit transition from 1->1. */
  if (SMSM_SMD_LOOPBACK & smsm_state_get(SMSM_APPS_STATE))
  {
    smd_cmd_type cmd;
    cmd.cmd = SMD_CMD_LOOPBACK;
    smd_cmd( &cmd );
  }
  
  /* Create high priority thread */
  qurt_thread_attr_init(&attr);
  qurt_thread_attr_set_name(&attr, "SMD_HIGH");
  /* SMD_HIGH priority should be greater then smdtask thread */
  priority = rcinit_lookup_prio("smdtask") - 1;
  qurt_thread_attr_set_priority(&attr, priority);
  qurt_thread_attr_set_stack_size(&attr, SMD_HIGH_THREAD_STACK_SIZE);
  
#ifndef SMD_OS_QURT_THREADX
  smd_high_thread_stack = smd_malloc(SMD_HIGH_THREAD_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&attr, smd_high_thread_stack);
#endif
  
  qurt_thread_create(&smd_high_thread_handle, 
                     &attr, 
                     (void (*)(void *))smd_task_loop, 
                     &smd_context[SMD_PRIORITY_HIGH]);

  /* Create low priority thread */
  qurt_thread_attr_init(&attr);
  qurt_thread_attr_set_name(&attr, "SMD_LOW");
  /* SMD_LOW priority should be greater then diag thread */
  priority = rcinit_lookup_prio("diag") - 1;
  qurt_thread_attr_set_priority(&attr, priority);
  qurt_thread_attr_set_stack_size(&attr, SMD_LOW_THREAD_STACK_SIZE);

#ifndef SMD_OS_QURT_THREADX
  smd_low_thread_stack = smd_malloc(SMD_LOW_THREAD_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&attr, smd_low_thread_stack);
#endif

  qurt_thread_create(&smd_low_thread_handle, 
                     &attr, 
                     (void (*)(void *))smd_task_loop, 
                     &smd_context[SMD_PRIORITY_LOW]);

  /* register a callback to process the other host's state changes */
  smsm_cb_register(
#if defined(SMD_APPS_PROC)
                   SMSM_MODEM_STATE,
#elif defined(SMD_MODEM_PROC)
                   SMSM_APPS_STATE,
#else
#error "Trying to register SMSM callback with invalid entry"
#endif
                   SMSM_SMD_LOOPBACK,
                   smd_smsm_cb,
                   NULL);

  /* Handle default-priority SMD from this task */
  smd_task_loop((smd_context_type *)&smd_context[SMD_PRIORITY_DEFAULT]);
}

/*===========================================================================
  FUNCTION  smd_send_interrupt
===========================================================================*/
/**
  Send an interrupt to the target processor in the specified channel.

  @param[in] host - Remote host to which the interrupt is to be sent.

  @return
  NA.

  @dependencies
  None.

  @sideeffects
  Interrupts the target processor.
*/
/*=========================================================================*/
void smd_send_interrupt( smem_host_type host )
{
  smd_interrupt_info_type *intr;

  if((uint32)host >= SMEM_NUM_HOSTS)
  {
    /* We got an invalid host, exit without trying to send interrupt */
    SMD_MSG_ERROR("SMD:%s:%d: Invalid host\n", __FUNCTION__, __LINE__, 0);
    return;
  }

  intr = &smd_interrupt_table[ host ];

  if( DALIPCINT_PROC_NULL != intr->processor &&
      SMD_NULL_OUT_INTERRUPT != intr->irq_out )
  {
    DalIPCInt_Trigger(smd_intr_out_handle, intr->processor, intr->irq_out);
  }
}

/*===========================================================================
FUNCTION      smd_cs_event_create
===========================================================================*/
/**
  Initializes a Critical Section suitable for being locked from
  taskless SMDL's interrupt context.

  @return   The critical section, as a void pointer.
===========================================================================*/
void *smd_cs_event_create(void)
{
  /* On Rex OSes, interrupt context is also handled in a thread. */
  return smd_cs_create();
}

/*===========================================================================
  FUNCTION      smd_enable_intr
===========================================================================*/
/**
  Enables the SMD ISR for the specified processor's incoming interrupt, if it 
  was previously disabled.
  
  This function will do nothing if the interrupt is already enabled.

  @param[in]  host    The smem_host_type for the processor whose incoming 
                      interrupt to enable.

  @return
  SMD_STATUS_SUCCESS        Interrupt enabled successfully, or was already 
                            enabled.
  SMD_STATUS_INVALID_PARAM  The host or interrupt ID was invalid, and not 
                            registered.
  SMD_STATUS_NOT_INIT       Interrupt structures and handles are not 
                            initialized yet.
  SMD_STATUS_ERROR          Not able to generate Soft-trigger.

  @sideeffects
  Will soft-trigger the interrupt, and hence the ISR, upon re-enablement.
*/
/*=========================================================================*/
int32 smd_enable_intr(smem_host_type host)
{
  int32 ret_val;
  
  if (host >= SMEM_NUM_HOSTS)
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  else if (smd_intr_in_handle == NULL)
  {
    ret_val = SMD_STATUS_NOT_INIT;
  }
  else 
  {
    smd_interrupt_info_type *intr;
    smd_cs_lock(smd_inc_intr_cs, TRUE);
    intr = &(smd_interrupt_table[host]);
    if (intr->disabled)
    {
      /* (Re)register the incoming interrupt. */
      if ((ret_val = smd_register_interrupt(intr)) == SMD_STATUS_SUCCESS)
      {
        /* Soft-trigger the interrupt now, since we probably missed one while 
         * it was previously disabled.  Assert, because at this point the 
         * intr and handle have already been validated. */
        if (DalInterruptController_InterruptTrigger(smd_intr_in_handle,
                  (uint32)intr->irq_in) != DAL_SUCCESS)
        {
          ret_val = SMD_STATUS_ERROR;
        }
        /* Track that the interrupt is not disabled. */
        intr->disabled = FALSE;
      }
    }
    else
    {
      /* The interrupt is already enabled. */
      ret_val = SMD_STATUS_SUCCESS;
    }
    smd_cs_lock(smd_inc_intr_cs, FALSE);
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION      smd_disable_intr
===========================================================================*/
/**
  Disables the SMD ISR for the specified processor's incoming interrupt, if it 
  was previously enabled.
  
  This function will do nothing if the interrupt is already disabled.

  @param[in]  host    The smem_host_type for the processor whose incoming 
                      interrupt to enable.

  @return
  SMD_STATUS_SUCCESS        Interrupt disabled successfully, or was already 
                            disabled.
  SMD_STATUS_INVALID_PARAM  The host or interrupt ID was invalid.
  SMD_STATUS_NOT_INIT       Interrupt structures and handles are not 
                            initialized yet.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smd_disable_intr(smem_host_type host)
{
  int32 ret_val;
  
  if (host >= SMEM_NUM_HOSTS)
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  else if (smd_intr_in_handle == NULL)
  {
    ret_val = SMD_STATUS_NOT_INIT;
  }
  else
  {
    smd_interrupt_info_type *intr;
    smd_cs_lock(smd_inc_intr_cs, TRUE);
    intr = &(smd_interrupt_table[host]);
    if (!intr->disabled)
    {
      /* Deregister the incoming interrupt. */
      if ((ret_val = smd_deregister_interrupt(intr)) == SMD_STATUS_SUCCESS)
      {
        /* Track that the interrupt has been disabled. */
        intr->disabled = TRUE;
      }
    }
    else
    {
      /* The interrupt is already disabled. */
      ret_val = SMD_STATUS_SUCCESS;
    }
    smd_cs_lock(smd_inc_intr_cs, FALSE);
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION  smd_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c smd_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void *smd_malloc(uint32 size)
{
  void *ptr;
  
  ptr = malloc(size);
  
  if (ptr == NULL)
  {
    ERR_FATAL("malloc failed! size=%d.", size, 0, 0);
  }
  
  return ptr;
}

/*===========================================================================
  FUNCTION  smd_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c smd_malloc.

  @param[in] pMem    The reference to the region of memory to be freed.

  @return    0 if succesful.
             Negative value if an error occurred.
*/
/*=========================================================================*/
int32 smd_free(void *ptr)
{
  free(ptr);
  return 0;
}

/*===========================================================================
  FUNCTION  smd_target_init_interrupts
===========================================================================*/
/**
  This function initializes the interprocessor interrupts for this target.
  
  @param[in]  this_host   source of the interrupts

  @return
  None

  @sideeffect
  Initializes the interrupt table

  @dependencies
  None
*/
/*==========================================================================*/
void smd_target_init_interrupts( smem_host_type this_host )
{
  uint32 i;
  uint32 smd_intr_enable_bits;
  DALSYSPropertyVar prop;
  /* The handle is a uint32[2], so it won't be null after declaration. */
  DALSYS_PROPERTY_HANDLE_DECLARE(smd_prop_handle);
  
  /* Clear out the interrupt table */
  for( i = 0; i < SMEM_NUM_HOSTS; i++ )
  {
    smd_interrupt_table[i].to = SMEM_INVALID_HOST;
    smd_interrupt_table[i].processor = (smd_os_proc_type)0;
    smd_interrupt_table[i].irq_out = SMD_NULL_OUT_INTERRUPT;
    smd_interrupt_table[i].irq_out_mask = 0;
    smd_interrupt_table[i].irq_in = SMD_NULL_IN_INTERRUPT;
    smd_interrupt_table[i].disabled = FALSE;
  }
  
  if (DALSYS_GetDALPropertyHandleStr("/core/mproc/smd", smd_prop_handle) !=
        DAL_SUCCESS)
  {
    ERR_FATAL("smd_target_init_interrupts: Cannot get DALProp handle.", 
              0, 0, 0);
  }
  else if (DALSYS_GetPropertyValue(smd_prop_handle, "smd_intr_enabled", 0, 
                                   &prop) != DAL_SUCCESS)
  {
    ERR_FATAL("smd_target_init_interrupts: Cannot get smd_intr_enabled DAL "
              "prop.", 0, 0, 0);
  }
  /* Each bit in smd_intr_enable_bits represents supported processor.*/
  smd_intr_enable_bits = prop.Val.dwVal;
  
  /* Copy interrupt enable to check supported processors in this target
     This will be used in smdl_open function in smd_lite_api.c */
  smd_supported_proc = smd_intr_enable_bits;
  
  /* From/to APPS Processor */
  if (smd_intr_enable_bits & SMD_APPS_BIT_MASK)
  {
    smd_interrupt_table[SMEM_APPS].to            = SMEM_APPS;
    smd_interrupt_table[SMEM_APPS].processor     = SMD_APPS_IPCPROC;
    smd_interrupt_table[SMEM_APPS].irq_out       = SMD_APPS_OUT;
    smd_interrupt_table[SMEM_APPS].irq_out_mask  = 0; /* Not used. */
    smd_interrupt_table[SMEM_APPS].irq_in        = SMD_APPS_IN;
  }
  
  /* From/to Modem processor */
  if (smd_intr_enable_bits & SMD_MODEM_BIT_MASK)
  {
    smd_interrupt_table[SMEM_MODEM].to           = SMEM_MODEM;
    smd_interrupt_table[SMEM_MODEM].processor    = SMD_MPSS_IPCPROC;
    smd_interrupt_table[SMEM_MODEM].irq_out      = SMD_MPSS_OUT;
    smd_interrupt_table[SMEM_MODEM].irq_out_mask = SMD_MPSS_OUT_M;
    smd_interrupt_table[SMEM_MODEM].irq_in       = SMD_MPSS_IN;
  }

  /* From/to ADSP Q6 (LPASS) processor */
  if (smd_intr_enable_bits & SMD_ADSP_BIT_MASK)
  {
    smd_interrupt_table[SMEM_Q6].to              = SMEM_Q6;
    smd_interrupt_table[SMEM_Q6].processor       = SMD_ADSP_IPCPROC;
    smd_interrupt_table[SMEM_Q6].irq_out         = SMD_ADSP_OUT;
    smd_interrupt_table[SMEM_Q6].irq_out_mask    = SMD_ADSP_OUT_M;
    smd_interrupt_table[SMEM_Q6].irq_in          = SMD_ADSP_IN;
  }

  /* From/to RIVA processor */
  if (smd_intr_enable_bits & SMD_WCNSS_BIT_MASK)
  {
    smd_interrupt_table[SMEM_RIVA].to            = SMEM_RIVA;
    smd_interrupt_table[SMEM_RIVA].processor     = SMD_WCNSS_IPCPROC;
    smd_interrupt_table[SMEM_RIVA].irq_out       = SMD_WCNSS_OUT;
    smd_interrupt_table[SMEM_RIVA].irq_out_mask  = SMD_WCNSS_OUT_M;
    smd_interrupt_table[SMEM_RIVA].irq_in        = SMD_WCNSS_IN;
  }

  /* From/to RPM processor */
  if (smd_intr_enable_bits & SMD_RPM_BIT_MASK)
  {
    smd_interrupt_table[SMEM_RPM].to             = SMEM_RPM;
    smd_interrupt_table[SMEM_RPM].processor      = SMD_RPM_IPCPROC;
    smd_interrupt_table[SMEM_RPM].irq_out        = SMD_RPM_OUT;
    smd_interrupt_table[SMEM_RPM].irq_out_mask   = SMD_RPM_OUT_M;
    smd_interrupt_table[SMEM_RPM].irq_in         = SMD_RPM_IN;
  }
  
}

/*===========================================================================
  FUNCTION  smd_string_copy
===========================================================================*/
/**
  Copies the source string into the destination buffer until 
  size is reached, or until a '\0' is encountered.  If valid,
  the destination string will always be NULL deliminated.
  
  @param[in] dst    The destination string, contents will be updated.
  @param[in] src    The source string
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
char *smd_string_copy(char *dst, const char *src, uint32 size)
{
  strlcpy(dst, src, size);

  return dst;
}

/*===========================================================================
  FUNCTION  smd_mem_copy
===========================================================================*/
/**
  Copies bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond the size of 
  the destination buffer.

  The result of calling this on overlapping source and destination
  buffers is undefined.
  
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.

  @return
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smd_mem_copy
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
)
{
  uint32 ret_size;

  ret_size = memscpy(dst, dst_size, src, copy_size);
  return ret_size;
}

/*===========================================================================
  FUNCTION  smd_mem_move
===========================================================================*/
/**
  Moves bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond 
  the size of the destination buffer. 

  This function should be used in preference to memscpy() if there
  is the possiblity of source and destination buffers overlapping.
  The result of the operation is defined to be as if the copy were from
  the source to a temporary buffer that overlaps neither source nor
  destination, followed by a copy from that temporary buffer to the
  destination.
   
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size  Number of bytes to copy from source buffer.
   
  @return 
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smd_mem_move
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
)
{
  uint32 ret_size;

  ret_size = memsmove(dst, dst_size, src, copy_size);
  return ret_size;
}

/*===========================================================================
FUNCTION      smd_copy_from_smd_chnl_uncached
===========================================================================*/
/**
  Copies the data from uncached SMD channel FIFO. 

  @return   Destination pointer.
===========================================================================*/
void *smd_copy_from_smd_chnl_uncached
(
  void         *dest, 
  const void   *src, 
  unsigned int size
)
{
  memscpy(dest, size, src, size);
  return dest;
}

/*===========================================================================
FUNCTION      smd_dpc_queue
===========================================================================*/
/**
  Accepts a function pointer which will be executed out of the 
  calling task's context, hence the name Deferred Procedure Call (DPC).
  
  @param[in]  func     Pointer to the function to execute.
  @param[in]  param    Parameter to pass to the function.
  
  @return   None.
===========================================================================*/
void smd_dpc_queue
(
  void (*func)(unsigned long),
  unsigned long param
)
{
#ifdef SMD_DSR_SUPPORTED
  if (func == NULL)
  {
    return;
  }
  smd_cs_lock(smd_dpc_cs, TRUE);
  
  if (((smd_dpc_wr_indx + 1) & SMD_DPC_RING_MASK) != smd_dpc_rd_indx)
  {
    smd_dpc_ring[smd_dpc_wr_indx].func  = func;
    smd_dpc_ring[smd_dpc_wr_indx].param = param;
    
    smd_dpc_wr_indx = (smd_dpc_wr_indx + 1) & SMD_DPC_RING_MASK;
    
    dsr_enqueue(smd_dpc_dsr_handle, NULL);
  }
  else
  {
    ERR_FATAL("smd_dpc_queue: SMD_DPC_RING_SIZE(%d) is not sufficient!", 
        SMD_DPC_RING_SIZE, 0, 0);
  }
  
  smd_cs_lock(smd_dpc_cs, FALSE);
#else 
  boolean rc;

  if (func == NULL)
  {
    return;
  }
  
  rc = rexl4_queue_dpc(func, param);
  ASSERT(rc != FALSE);
#endif
}
