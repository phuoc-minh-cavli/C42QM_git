
/*===========================================================================

               S M P 2 P   Q U R T

===========================================================================*/
/**
  @file smp2p_qurt.c
*/
/* Abstracts the OS specific implementation of system function implemented by
   QuRT.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and
         group descriptions are maintained in the SMEM_mainpage file. To change
         any of the the file/group text for the PDF, edit the SMEM_mainpage
         file, or contact Tech Pubs.

         The above description for this file is part of the "smem" group
         description in the SMEM_mainpage file.
*/

/*===========================================================================
Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smp2p/src/smp2p_qurt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/15   rv      Moved OS timetick init from smp2p_os_log() to smp2p_init()
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
09/25/13   pa      Remove self-checking code from early smp2p development.
06/25/13   pa      Fix minor bugs in smp2p_os_log.
02/12/13   pa      Convert ASSERT_BRK to ASSERT.
01/14/13   pa      Updated to newer SMEM_* defines SMEM_WCN and SMEM_ADSP.
10/25/12   pa      Initial revision.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h" 
#include "stringl.h" 
#include "qurt.h"
#ifdef SMP2P_DSR_SUPPORTED
#include "dsr.h"
#endif
#include "smp2p_internal.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDITimetick.h"
#include "err.h"
#include "assert.h"

/*===========================================================================
                      LOCAL TYPE DEFINITIONS
===========================================================================*/

/** Defines the header word of the log entry */
typedef union
{
  struct
  {
    /* id of the remote processor */
    uint8  remote_id;

    /* state of the connection */
    uint8  state;

    /* id of the event */
    uint16 event_id;
  } fields;
  uint32 all;
} smp2p_log_header_type;

/** Defines the SMP2P log entry.
 *  Order and size is meant to mimic SMEM log. */
typedef struct
{
  /* timestamp of when the event occurred */
  uint32 timestamp;

  /* header for this log entry */
  smp2p_log_header_type header;

  /* caller data 1 */
  uint32 data1;

  /* caller data 2 */
  uint32 data2;
} smp2p_log_entry_type;

/*===========================================================================
                        EXTERNAL DATA DEFINITIONS
===========================================================================*/

#ifdef FEATURE_SMP2P_LOOPBACK_CLIENT
/** The count of interrupts triggered to the local proc as part of loopback */
extern uint32 smp2p_lb_int_count;
#endif /* FEATURE_SMP2P_LOOPBACK_CLIENT */

/** Main SMP2P info data structure */
extern smp2p_info_type smp2p_info[SMEM_NUM_HOSTS];

/** Pointer to critical section for the SMP2P local log */
extern void * smp2p_log_cs;

/*===========================================================================
                        LOCAL DATA DEFINITIONS
===========================================================================*/

/** The handle for DALIPCINT: outgoing interrupts. */
static DalDeviceHandle * smp2p_intr_out_handle;

/** DAL Timetick handle */
static DalDeviceHandle * smp2p_timetick_handle = NULL;

/** This is the global structure which holds information about interrupts on
 *  this processor. */
smp2p_interrupt_info_type smp2p_interrupt_table[SMEM_NUM_HOSTS];

#ifdef SMP2P_LOG_LOCAL

/** SMP2P local log */
static smp2p_log_entry_type smp2p_log[SMP2P_LOG_NUM_ENTRIES];

/** SMP2P local log index.
 *  Points to the next entry to write to in the log. */
static uint32 smp2p_log_idx = 0;

#endif

#ifdef SMP2P_DSR_SUPPORTED
/** The handle for DSR to handle remote updates. */
static dsr_obj_t *smp2p_dsr_handles[SMEM_NUM_HOSTS];
#endif

/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smsm_dsr
===========================================================================*/
/*!
@brief
  This function is called when an DSR is enqueued from interrupt context.
  This means certain host's state has changed. 

@param dsr_obj    DSR object
@param ctxt       Context associated with DSR
@param payload    payload context

@return None

@note   In interupt context
*/
/*=========================================================================*/
#ifdef SMP2P_DSR_SUPPORTED
static void smp2p_dsr(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  smem_host_type remote = (smem_host_type)(uintptr_t)payload;
  
  if (remote >= SMEM_NUM_HOSTS)
  {
    ERR_FATAL("smp2p_dsr: Invalid remote host %d!", remote, 0, 0);
  }
  smp2pi_isr(remote);
}
#endif

/*===========================================================================
  FUNCTION  smp2p_isr
===========================================================================*/
/**
  Interrupt service routine for SMP2P.  Handles interrupts due to negotiation,
  new entries and received data.

  Negotiation interrupts will not trigger events to the client.
  New entries and new received data will trigger events to a client who is
  registered for that entry.

  @param [in]   source    Source processor ID who triggered the interrupt.

  @return
  None.

  @dependencies
  None.
*/
/*=========================================================================*/
#ifdef SMP2P_DSR_SUPPORTED
static void smp2p_isr(void *param)
{
  smem_host_type remote = (smem_host_type)(uintptr_t)param;
  
  if (remote >= SMEM_NUM_HOSTS)
  {
    ERR_FATAL("smp2p_isr: Invalid remote host %d!", remote, 0, 0);
  }
  
  if (smp2p_dsr_handles[remote] == NULL)
  {
    ERR_FATAL("smp2p_isr: smp2p_dsr_handles[] Invalid remote %d!", remote, 0, 0);
  }

  dsr_enqueue(smp2p_dsr_handles[remote], param);
}
#endif

/*===========================================================================
  FUNCTION  smp2p_target_init_interrupts
===========================================================================*/
/**
  This function initializes the interprocessor interrupts for this target.
  
  @return
  None

  @sideeffect
  Initializes the interrupt table

  @dependencies
  None
*/
/*==========================================================================*/
void smp2p_target_init_interrupts(void)
{
  uint32 i;
  byte *smp2p_intr_enbl_arr;
  DALSYSPropertyVar prop;
  /* The handle is a uint32[2], so it won't be null after declaration. */
  DALSYS_PROPERTY_HANDLE_DECLARE(smp2p_prop_handle);
  
  /* Clear out the interrupt table */
  for( i = 0; i < SMEM_NUM_HOSTS; i++ )
  {
    smp2p_interrupt_table[i].to = SMEM_INVALID_HOST;
    smp2p_interrupt_table[i].processor = (smp2p_os_proc_type)0;
    smp2p_interrupt_table[i].irq_out = SMP2P_NULL_OUT_INTERRUPT;
    smp2p_interrupt_table[i].irq_out_mask = 0;
    smp2p_interrupt_table[i].irq_in = SMP2P_NULL_IN_INTERRUPT;
  }
  
  if (DALSYS_GetDALPropertyHandleStr("/core/mproc/smp2p", smp2p_prop_handle) !=
        DAL_SUCCESS)
  {
    ERR_FATAL("smp2p_target_init_interrupts: Cannot get DALProp handle.", 
              0, 0, 0);
  }
  else if (DALSYS_GetPropertyValue(smp2p_prop_handle, "smp2p_intr_enabled", 0, 
                                   &prop) != DAL_SUCCESS)
  {
    ERR_FATAL("smp2p_target_init_interrupts: Cannot get smp2p_intr_enabled DAL "
              "prop.", 0, 0, 0);
  }
  /* The SMP2P intr enable array will be put in prop.Val.pbVal (array of bytes) */
  smp2p_intr_enbl_arr = prop.Val.pbVal;
  
  /* From/to APPS Processor */
  if (smp2p_intr_enbl_arr[SMEM_APPS])
  {
    smp2p_interrupt_table[SMEM_APPS].to            = SMEM_APPS;
    smp2p_interrupt_table[SMEM_APPS].processor     = SMP2P_APPS_IPCPROC;
    smp2p_interrupt_table[SMEM_APPS].irq_out       = SMP2P_APPS_OUT;
    smp2p_interrupt_table[SMEM_APPS].irq_out_mask  = 0; /* Not used. */
    smp2p_interrupt_table[SMEM_APPS].irq_in        = SMP2P_APPS_IN;
  }
  
  /* From/to Modem processor */
  if (smp2p_intr_enbl_arr[SMEM_MODEM])
  {
    smp2p_interrupt_table[SMEM_MODEM].to           = SMEM_MODEM;
    smp2p_interrupt_table[SMEM_MODEM].processor    = SMP2P_MPSS_IPCPROC;
    smp2p_interrupt_table[SMEM_MODEM].irq_out      = SMP2P_MPSS_OUT;
    smp2p_interrupt_table[SMEM_MODEM].irq_out_mask = SMP2P_MPSS_OUT_M;
    smp2p_interrupt_table[SMEM_MODEM].irq_in       = SMP2P_MPSS_IN;
  }

  /* From/to ADSP processor */
  if (smp2p_intr_enbl_arr[SMEM_ADSP])
  {
    smp2p_interrupt_table[SMEM_ADSP].to            = SMEM_ADSP;
    smp2p_interrupt_table[SMEM_ADSP].processor     = SMP2P_ADSP_IPCPROC;
    smp2p_interrupt_table[SMEM_ADSP].irq_out       = SMP2P_ADSP_OUT;
    smp2p_interrupt_table[SMEM_ADSP].irq_out_mask  = SMP2P_ADSP_OUT_M;
    smp2p_interrupt_table[SMEM_ADSP].irq_in        = SMP2P_ADSP_IN;
  }

  /* From/to WCN processor */
  if (smp2p_intr_enbl_arr[SMEM_WCN])
  {
    smp2p_interrupt_table[SMEM_WCN].to             = SMEM_WCN;
    smp2p_interrupt_table[SMEM_WCN].processor      = SMP2P_WCNSS_IPCPROC;
    smp2p_interrupt_table[SMEM_WCN].irq_out        = SMP2P_WCNSS_OUT;
    smp2p_interrupt_table[SMEM_WCN].irq_out_mask   = SMP2P_WCNSS_OUT_M;
    smp2p_interrupt_table[SMEM_WCN].irq_in         = SMP2P_WCNSS_IN;
  }

  /* From/to RPM processor */
  if (smp2p_intr_enbl_arr[SMEM_RPM])
  {
    smp2p_interrupt_table[SMEM_RPM].to             = SMEM_RPM;
    smp2p_interrupt_table[SMEM_RPM].processor      = SMP2P_RPM_IPCPROC;
    smp2p_interrupt_table[SMEM_RPM].irq_out        = SMP2P_RPM_OUT;
    smp2p_interrupt_table[SMEM_RPM].irq_out_mask   = SMP2P_RPM_OUT_M;
    smp2p_interrupt_table[SMEM_RPM].irq_in         = SMP2P_RPM_IN;
  }
  
}

/*===========================================================================
  FUNCTION  smp2p_os_init_event
===========================================================================*/
/**
  Initializes the OS specific event object.

  @param [in]   os_params   Pointer to the OS specific params struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_init_event(smp2p_os_params_type *os_params)
{
  memset(os_params, 0, sizeof(smp2p_os_params_type));

  qurt_mutex_init(&os_params->mutex);
  qurt_signal_init(&os_params->own_signal);

  os_params->signal = &os_params->own_signal;

  return SMP2P_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION  smp2p_os_send_event
===========================================================================*/
/**
  Triggers the OS specific event object.

  @param [in]   os_params   Pointer to the OS specific params struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_send_event(smp2p_os_params_type *os_params)
{
  qurt_mutex_lock(&os_params->mutex);
  
  if (os_params->signal == NULL)
  {
    qurt_mutex_unlock(&os_params->mutex);
    ERR_FATAL("os_params signal is NULL", 0, 0, 0);
    return SMP2P_STATUS_FAILURE;
  }

  /* Update the flag to indicate event is set and set signal */
  os_params->event_set = TRUE;
  qurt_signal_set(os_params->signal, 1);

  qurt_mutex_unlock(&os_params->mutex);
  
  return SMP2P_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION  smp2p_os_wait
===========================================================================*/
/**
  Blocks on the OS specific event object found in the context.

  @param [in]   ctx   Pointer to the SMP2P context struct

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_wait(smp2p_context_type *ctx)
{
  smp2p_context_type *ctx_arr[1];
  uint32             trigger_idx;
  int32              ret;
  
  ctx_arr[0] = ctx;

  ret = smp2p_os_wait_multiple(ctx_arr, 1, &trigger_idx);
  if (ret != SMP2P_STATUS_SUCCESS || trigger_idx != 0)
  {
    ERR_FATAL("smp2p_os_wait_multiple failed(%d) or invalid indx(%d).",
              ret, trigger_idx, 0);
  }
  
  return ret;
}

/*===========================================================================
  FUNCTION  smp2p_os_wait_multiple
===========================================================================*/
/**
  Blocks on the OS specific events object found in an array of contexts.

  @param [in]   ctx_arr     Array of one or more pointers to SMP2P contexts
                            to wait on.
  @param [in]   ctx_count   Number of valid contexts in ctx array
  @param [out]  trigger_idx Index of context which was triggered

  @return
  SMP2P_STATUS_SUCCESS - Event initialized.
  SMP2P_STATUS_FAILURE - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_os_wait_multiple
(
  smp2p_context_type **ctx_arr,
  uint32               ctx_count,
  uint32              *trigger_idx
)
{
  smp2p_os_params_type *os_params;
  qurt_signal_t        *master_signal;
  int32                 idx;

  if (ctx_arr == NULL || trigger_idx == NULL || ctx_count == 0)
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }
  
  /* Use 0th context's own_signal as master waiting signal for contexts */
  master_signal = &ctx_arr[0]->os_params.own_signal;

  *trigger_idx = 0xFFFFFFFF;

  for (idx = 0; idx < ctx_count; idx++)
  {
    os_params = &ctx_arr[idx]->os_params;

    qurt_mutex_lock(&os_params->mutex);

    if (os_params->waiting == TRUE)
    {
      qurt_mutex_unlock(&os_params->mutex);
      ERR_FATAL("Some thread already wiating on SMP2P event ctx_indx.", idx, 0, 0);
      return SMP2P_STATUS_FAILURE;
    }

    os_params->waiting = TRUE;

    /* Check if event is already set */
    if (os_params->event_set == TRUE)
    {

      qurt_mutex_unlock(&os_params->mutex);
      
      /* Event is already set, no need to wait on signal */
      goto RESTORE_SIGNALS;
    }
    else
    {
      /* Use master signal to wait for event */
      os_params->signal = master_signal;
    }

    qurt_mutex_unlock(&os_params->mutex);
  }

  /* Wait on master signal */
  qurt_signal_wait(master_signal, 1, QURT_SIGNAL_ATTR_WAIT_ANY);

RESTORE_SIGNALS:
  /* Check which event context set the signal */
  for (idx = 0; idx < ctx_count; idx++)
  {
    os_params = &ctx_arr[idx]->os_params;
    
    qurt_mutex_lock(&os_params->mutex);
    
    os_params->waiting = FALSE;

    if (os_params->event_set == TRUE)
    {
      if (*trigger_idx == 0xFFFFFFFF)
      {
        /* Found first signal, so update return index */
        *trigger_idx = idx;
        os_params->event_set = FALSE;
        qurt_signal_clear(os_params->signal, 1);
      }
      else
      {
        /* We already found one signal, so need to restore the signal */
        qurt_signal_set(&os_params->own_signal, 1);
      }
    }
    else if (idx == 0) /* Clear master signal */
    {
      qurt_signal_clear(os_params->signal, 1);
    }

    /* Restore its own signal */
    os_params->signal = &os_params->own_signal;

    qurt_mutex_unlock(&os_params->mutex);
  }

  if (*trigger_idx == 0xFFFFFFFF)
  {
    ERR_FATAL("No event was set but we still not wiating. master_signal=%p", master_signal, 0, 0);
    return SMP2P_STATUS_FAILURE;    
  }

  return SMP2P_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION  smp2p_os_register_interrupt
===========================================================================*/
/**
  This function registers SMP2P interrupts.

  @return
  None
*/
/*=========================================================================*/
void smp2p_os_register_interrupt(void)
{
  smp2p_interrupt_info_type * intr;
  uint32 i;
  /* DAL Incoming Interrupt Controller device handle. */
  DalDeviceHandle * smp2p_intr_in_handle;

  /* Create Interrupt Handler */
  if( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_IPCINT,
                                      &smp2p_intr_out_handle ) )
  {
    ERR_FATAL( "smp2p_register_interrupt: Interrupt Handler creation failed!",
               0, 0, 0);
  }

  /* Create Incoming Interrupt Controller Handle */
  if( ( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER,
                                         &smp2p_intr_in_handle ) )
                                      || (smp2p_intr_in_handle == NULL) )
  {
    ERR_FATAL("smp2p_register_interrupt: Intr Cntrler Handle creation failed!",
              0, 0, 0);
  }

  for (i=0; i<SMEM_NUM_HOSTS; i++)
  {
    /* Register the SMP2P-specific interrupt */
    intr = (smp2p_interrupt_info_type *)&smp2p_interrupt_table[i];

    if (intr->irq_in != SMP2P_NULL_IN_INTERRUPT)
    {
#ifdef SMP2P_DSR_SUPPORTED
      /* Create DSR object for processing the remote host update */
      if (dsr_create(&smp2p_dsr_handles[i], 
                     smp2p_dsr, 
                     NULL, 
                     DSR_PRI_HIGH, 
                     DSR_CONTEXT_ISR) == FALSE)
      {
        ERR_FATAL("smp2p_os_register_interrupt: dsr_create failed! i=%d", i, 0, 0);
      }
#endif
      if( DalInterruptController_RegisterISR(
            smp2p_intr_in_handle,
            (uint32)intr->irq_in,
#ifdef SMP2P_DSR_SUPPORTED            
            (DALIRQ)smp2p_isr,
#else
            (DALIRQ)smp2pi_isr,
#endif
            (DALIRQCtx)intr->to,
            DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER) != DAL_SUCCESS )
      {
        ERR_FATAL("smp2p_register_interrupt: SMP2P ISR for host %d failed to "
                  "register.", i, 0, 0);
      }
    }
  }
}

/*===========================================================================
  FUNCTION  smp2p_os_send_interrupt
===========================================================================*/
/**
  This function sends out the interprocessor interrupts.

  @param[in] dest Destination processor to interrupt

  @return
  None
*/
/*=========================================================================*/
void smp2p_os_send_interrupt(smem_host_type dest)
{
#ifdef FEATURE_SMP2P_LOOPBACK_CLIENT
  /* just count the number of interrupts triggered to the local processor
   * for loopback */
  if (SMP2P_LPBK_HOST == dest)
  {
    smp2p_lb_int_count++;
  }
  /* Send interrupt if it is valid */
  else if (SMP2P_NULL_OUT_INTERRUPT != smp2p_interrupt_table[dest].irq_out)
#endif /* FEATURE_SMP2P_LOOPBACK_CLIENT */
  {
    DalIPCInt_Trigger(smp2p_intr_out_handle,
                      smp2p_interrupt_table[dest].processor,
                      smp2p_interrupt_table[dest].irq_out);
  }
}

/*===========================================================================
  FUNCTION  smp2p_os_cs_create
===========================================================================*/
/**
  Creates the critical section used to acquire access to SMP2P from different
  tasks.

  @return
  Pointer to critical section object

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void * smp2p_os_cs_create(void)
{
  qurt_mutex_t *mutex;

  mutex = smp2p_os_malloc(sizeof(qurt_mutex_t));
  memset(mutex, 0, sizeof(qurt_mutex_t));

  qurt_mutex_init(mutex);

  return ((void *)mutex);
}

/*===========================================================================
  FUNCTION  smp2p_os_cs_lock
===========================================================================*/
/**
  Locks the critical section used by SMP2P.

  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks can lock access to SMP2P.

  @param[in] cs Pointer to the critical section object

  @return
  None

  @dependencies
  None.

  @sideeffects
  Prevents context switch.
*/
/*==========================================================================*/
void smp2p_os_cs_lock(void * cs)
{
  qurt_mutex_t *mutex = (qurt_mutex_t * )cs;

  qurt_mutex_lock(mutex);
}

/*===========================================================================
  FUNCTION  smp2p_os_cs_unlock
===========================================================================*/
/**
  Releases the critical section used by SMP2P.

  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks can unlock access to SMP2P.

  @param[in] cs Pointer to the critical section object

  @return
  None

  @dependencies
  None.

  @sideeffects
  Allows context switch.
*/
/*==========================================================================*/
void smp2p_os_cs_unlock(void * cs)
{
  qurt_mutex_t *mutex = (qurt_mutex_t * )cs;

  qurt_mutex_unlock(mutex);
}

/*===========================================================================
  FUNCTION  smp2p_os_timetick_init
===========================================================================*/
/**
  Creates the handle for the OS-specific timetick handle, if necessary.

  @return
  SMEM_ERROR on failure
  SMEM_SUCCESS on success
*/
/*=========================================================================*/
int32 smp2p_os_timetick_init(void)
{
  DALResult result;
  
  result = DalTimetick_Attach("SystemTimer", &smp2p_timetick_handle);
  if ((result != DAL_SUCCESS) || (smp2p_timetick_handle == NULL))
  {
    ERR("smp2p_timetick: DAL timetick attach failed, result=%d and "
        "smp2p_timetick_handle = 0x%08x", result, (uint32)smp2p_timetick_handle, 0);
    smp2p_timetick_handle = NULL;

    return SMP2P_STATUS_FAILURE; 
  }

  return SMP2P_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION  smp2p_os_timetick_get
===========================================================================*/
/**
  Returns the current timetick value.

  @return
  The current timetick value, measured in ticks.
  0 if an error occurred.
*/
/*=========================================================================*/
uint32 smp2p_os_timetick_get(void)
{
  DalTimetickTime32Type time_ret = 0;
  DALResult result;
  
  if (smp2p_timetick_handle != NULL)
  {
    result = DalTimetick_Get(smp2p_timetick_handle, &time_ret);
    if (result != DAL_SUCCESS)
    {
      ERR("smem_timetick: Cannot get DAL timetick value, result=%d.", result, 0, 0);
      time_ret = 0;
    }
  }

  return (uint32)time_ret;
}

/*===========================================================================
  FUNCTION  smp2p_os_log
===========================================================================*/
/**
  This function writes an event entry into the SMP2P log.

  If SMP2P_LOG_LOCAL is defined, this function writes entries to a local
  circular buffer.  If SMP2P_LOG_SMEM is defined, this function writes
  entries to the SMEM log.  These options are not mutually exclusive.

  @param[in]  event_id  An event identifier for this event
                        For example, an event may be receiving data, or
                        creating a new entry.
  @param[in]  remote_id Identifier of the remote endpoint.  If not applicable,
                        set to -1.
  @param[in]  data1     First data word.  Usually specific to the event_id.
  @param[in]  data2     Second data word.  Usually specific to the event_id.
  @param[in]  flags     Flags param. 
                        SMP2P_FLAGS_STM prevents this function from locking as 
                        well as from logging to SMEM log (which required 
                        interprocessor lock).

  @return
  None.
*/
/*=========================================================================*/
void smp2p_os_log
(
  uint32 event_id,
  int32  remote_id,
  uint32 data1,
  uint32 data2,
  uint32 flags
)
{
#ifdef SMP2P_LOG_LOCAL
  {
    smp2p_log_entry_type   entry;
    smp2p_log_entry_type * entry_ptr;
    uint8 remote_state = (remote_id >= 0 && remote_id < SMEM_NUM_HOSTS) ?
                          smp2p_info[remote_id].state : -1;

    /* fill in the local entry */
    entry.header.fields.event_id  = event_id;
    entry.header.fields.remote_id = remote_id;
    entry.header.fields.state     = remote_state;
    entry.timestamp               = smp2p_os_timetick_get();
    entry.data1                   = data1;
    entry.data2                   = data2;

    if (!(flags & SMP2P_FLAGS_STM))
    {
      /* lock here to protect the global log */
      smp2p_os_cs_lock(smp2p_log_cs);
    }

    /* set the pointer to the entry in question */
    entry_ptr = &smp2p_log[smp2p_log_idx];

    /* update index, and handle wrap around case */
    smp2p_log_idx = (smp2p_log_idx + 1) % SMP2P_LOG_NUM_ENTRIES;

    /* copy the data there */
    smp2p_mem_copy(entry_ptr, sizeof(smp2p_log_entry_type), &entry, 
                   sizeof(entry));

    if (!(flags & SMP2P_FLAGS_STM))
    {
      smp2p_os_cs_unlock(smp2p_log_cs);
    }
  }
#endif

#ifdef SMP2P_LOG_SMEM
  /* If STM (single threaded mode) is specified, skip logging */
  if (!(flags & SMP2P_FLAGS_STM))
  {
    smp2p_log_header_type header;
    uint8 remote_state = (remote_id >= 0 && remote_id < SMEM_NUM_HOSTS) ?
                          smp2p_info[remote_id].state : -1;

    header.fields.event_id   = event_id;
    header.fields.remote_id  = remote_id;
    header.fields.state      = remote_state;
    SMEM_LOG_EVENT(SMP2P_SMEM_LOG_EVENT_ID, header.all, data1, data2);
  }
#endif
}

/*===========================================================================
  FUNCTION  smp2p_os_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c smp2p_os_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void * smp2p_os_malloc(uint32 size)
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
  FUNCTION  smp2p_os_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c smp2p_os_malloc.

  @param[in] ptr    A reference to the region of memory to be freed.

  @return    0 if succesful.
             Negative value if an error occurred.
*/
/*=========================================================================*/
int32 smp2p_os_free(void *ptr)
{
  free(ptr);
  return 0;
}
/*===========================================================================
  FUNCTION  smp2p_mem_copy
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
uint32 smp2p_mem_copy
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
  FUNCTION  smp2p_mem_move
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
  @param[in]  copy_size Number of bytes to copy from source buffer.
   
  @return 
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smp2p_mem_move
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
