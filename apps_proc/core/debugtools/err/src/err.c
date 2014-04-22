/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error reporting services for both fatal and
  non-fatal errors.  This module is not a task, but rather a set of
  callable procedures which run in the context of the calling task.

Copyright (c) 1992-2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.tx/6.0/debugtools/err/src/err.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   din     Removed dead code and updated file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "err.h"
#include "errlog.h"
#include "erri.h"
#include "msg.h"
#include <stdio.h>
#include "stdlib.h"
#include "DDITimetick.h"
#include "fs_public.h"
#include "dog.h"
#include "err_nv_efs.h"
#include "stringl.h"
#include "tms_utils.h"
#include "tms_dload.h"


#ifdef FEATURE_SMEM
#include "smem.h"
#include "smem_diag.h"
#endif /* FEATURE_SMEM */

#ifdef FEATURE_SMEM_LOG
#include "smem_log.h"
#endif /* FEATURE_SMEM */

#ifdef FEATURE_L4
//#include <l4/cust/msm_syscall.h>
#endif

#include "qurt.h"
#include "qurt_memory.h"
#include "sys_m_internal.h"
#include "tz_syscalls.h"
#include "scm.h"
#include "busywait.h"


/*===========================================================================

                      Prototypes for External functions

===========================================================================*/

extern void sys_m_set_gpio(void);
extern void err_update_image_versioning_info (void);
extern void dog_force_bite(void);
extern void err_action_init(void);
extern err_action_type err_get_auto_action(void);

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
static void err_emergency_error_recovery(void);
static void err_call_tz_invoke_oem_defined_routine(void);

static void err_smem_log(void);
static void err_terminate(void);
static void err_loop_pause(void);
static void err_flush_cache(void);
static void err_log_ssr_failure_reason(void);
void err_reentrancy_violated(void);



/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
#define ERR_DYNAMIC_MSG_SIZE  80
#define ERR_SSR_REASON_SIZE_BYTES 80

unsigned int err_int_flag = 0;

extern char sys_m_sfr_buffer[];
extern int sys_m_sfr_written;
extern void KxCache_DCacheFlush_Lockless(void);

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
/* Used to replace all ERR_FATAL message strings.
 * Reduces code size by eliminating other const strings used for errors.
 */
const char err_generic_msg[] = "Error Fatal, parameters: %d %d %d";
#endif

/* Struct used to hold coredump data */
coredump_type coredump;

/* Ptr used by assembly routines to grab registers */
/*  (update this as needed if struct changes)      */
arch_coredump_type* arch_coredump_ptr = (arch_coredump_type*)(&coredump.arch.regs.array);


static boolean err_services_init_complete = FALSE;

/* SFR (Subsystem restart Failure Reason) decl's */
char* err_ssr_smem_buf_ptr= NULL;
static const char err_sfr_init_string[]="SFR Init: wdog or kernel error suspected.";
boolean err_sfr_locked = FALSE;

static char* err_dynamic_msg_scratch_buf = 0;

/* Wait time before reset actions */
#define ERR_ACTION_WAIT_TIME_USEC 50000

static DalDeviceHandle *phChipPower=NULL;
DALResult dal_attach = DAL_SUCCESS;

static DalDeviceHandle* phTimetickHandle = NULL;


/* The function tables below are processed by the error handler
 * in the following order:
 *   1. err_preflush_internal (one time)
 *   2. err_preflush_external (one time)
 *   3. err_flush_internal (one time)
 *   4. err_postflush_internal (called repeatedly in a loop)
 */

static const err_cb_ptr err_preflush_internal[] =
{

  err_call_tz_invoke_oem_defined_routine,
  sys_m_set_gpio,
  err_smem_log,
  err_log_ssr_failure_reason,
  sys_m_err_notify_peripherals,
  /* NULL must be last in the array */
  NULL
};


static const err_cb_ptr err_flush_internal[] =
{
  sys_m_wait_peripherals,
  err_flush_cache,
  err_terminate,
  err_loop_pause,
  /* NULL must be last in the array */
  NULL
};

#ifdef FEATURE_SMEM_LOG
/* Event defines for using SMEM to log what occurs during sleep and
 * power collapse.
 */
#define ERR_ERROR_FATAL                (SMEM_LOG_ERROR_EVENT_BASE + 1)
#define ERR_ERROR_FATAL_TASK           (SMEM_LOG_ERROR_EVENT_BASE + 2)

#endif /* FEATURE_SMEM_LOG */

#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 400000))
//compiler bug workaround for RVCT 4.1 P 631 upgrade
static volatile int err_always_false = 0;
#endif

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_INITIALIZE_COREDUMP

DESCRIPTION
  Initializes coredump

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void err_initialize_coredump (void)
{
  /* Set type and version values */
  coredump.version = ERR_COREDUMP_VERSION;
  coredump.arch.type = ERR_ARCH_COREDUMP_TYPE;
  coredump.arch.version = ERR_ARCH_COREDUMP_VER;
  coredump.os.type = ERR_OS_COREDUMP_TYPE;
  coredump.os.version = ERR_OS_COREDUMP_VER;
  coredump.err.version = ERR_COREDUMP_VER;

} /* err_initialize_coredump */

/*===========================================================================

FUNCTION ERR_INIT

DESCRIPTION
  This function checks if NV has been built.  If so then it loads up the
  error log cache from NV, it initializes error counts and local variables,
  and it sets error services to online.  If NV has not been built then
  error log is not loaded and error services remain offline and error
  logging does not take place.  Any access to NV is performed here directly,
  without relying on the NV task.  Thus error service can be initialized
  and used early in the life of the DMSS and right after boot.  Note
  that if NV has not been built, error services provide no error logging
  capabilities.  However NV is built once, after initial factory startup,
  and rarely thereafter.  Thus except for the first ever powerup of the
  unit after NV build or re-build, error logging will take place as usual.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

void err_init (void)
{
  dal_attach = DAL_DeviceAttach(DALDEVICEID_CHIPPOWER,&phChipPower);
  if((!phChipPower)||(dal_attach != DAL_SUCCESS))
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,"Failed to register with CHIPPOWER DAL");
    return;
  }

  /* Init Timetick object */
  if(DAL_SUCCESS != DalTimetick_Attach("SystemTimer", &phTimetickHandle))
  {
    ERR_FATAL("ERR SERVICES: DalTimetick_Attach failed.",0,0,0);
  }

   err_ssr_smem_buf_ptr = sys_m_init_sfr_buffer();
   if (NULL != err_ssr_smem_buf_ptr)
   {
     strlcpy (err_ssr_smem_buf_ptr, err_sfr_init_string, ERR_SSR_REASON_SIZE_BYTES);
   }

  err_nv_log_init();

  /* Initialize configuration options which can be controlled via NV settings */
  err_action_init();

  /* We use a scratch buf from the heap to minimize risk of an 
  overwrite corrupting the coredump structure or stack */
  err_dynamic_msg_scratch_buf = (char*)malloc(ERR_DYNAMIC_MSG_SIZE);
  if (err_dynamic_msg_scratch_buf)
  {
    memset (err_dynamic_msg_scratch_buf, 0, ERR_DYNAMIC_MSG_SIZE);
  }

  ERR_LOG_MSG("Err service initialized.");

  err_initialize_coredump();

  err_update_image_versioning_info();

  err_services_init_complete = TRUE;

} /* err_init */

/*===========================================================================

FUNCTION ERR_GET_TIMETICK

DESCRIPTION
  Add timetick value to specified address if timetick handle is initialized

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None .

===========================================================================*/

static void err_get_timetick(DalTimetickTime64Type * tick)
{
  if (NULL != phTimetickHandle)
  {
    /* Best effort, no failure action */
    DalTimetick_GetTimetick64(phTimetickHandle, tick );
  }  
} /* err_get_timetick */


/*===========================================================================

FUNCTION ERR_CALL_NEXT_TO_STM_CB

DESCRIPTION
 Calls err next to STM cb

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/

void err_call_next_to_STM_CB(void)
{
  /* Kick Dog */
  dog_force_kick();
    
  /* Set the start time for postflush callbacks */
  err_get_timetick( &(coredump.err.err_next_to_STM.cb_start_tick) );
    
  /* Flush Cache */
  err_flush_cache();

  if ( coredump.err.err_next_to_STM.err_cb )
  {
    coredump.err.err_current_cb = 
	      coredump.err.err_next_to_STM.err_cb;
    coredump.err.err_next_to_STM.cb_start = TRUE;

    /* Flush Cache */
    err_flush_cache();
 
    coredump.err.err_current_cb();
  }

  /* Kick Dog */
  dog_force_kick();

} /* err_call_next_to_STM_CB */


/*=========================================================================

FUNCTION err_log_ssr_failure_reason

DESCRIPTION
  Used to log a minimal set of failure reason to smem.  Primarily to assist
  locating faulting subsystem in many-subsystem architectures.

DEPENDENCIES
  smem

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void err_log_ssr_failure_reason(void)
{
  if (!err_sfr_locked && (!sys_m_sfr_written))
  {
    word sfr_line = coredump.err.linenum;
    const char *sfr_file_ptr = coredump.err.filename;
    const char *sfr_msg_ptr = coredump.err.message;
    uint32 sfr_p0 = coredump.err.param[0];
    uint32 sfr_p1 = coredump.err.param[1];
    uint32 sfr_p2 = coredump.err.param[2];
    char *sfr_buf_ptr = err_ssr_smem_buf_ptr;
    int sfr_written = 0;

    if (sfr_buf_ptr && sfr_file_ptr)
    {
      /* Write "__MODULE__:__LINE:" */
      sfr_written = tms_utils_fmt(sfr_buf_ptr, ERR_SSR_REASON_SIZE_BYTES, 
		           "%s:%d:", sfr_file_ptr, sfr_line);

      if ((sfr_written > 0) && sfr_msg_ptr)
      {
        /* Append err fatal message */
        if(sfr_written > TMS_UTILS_BUF_SZ_ZERO){
           sfr_buf_ptr += (sfr_written-1); 
	   /*-1 since tms_utils_fmt() API returns written length with NULL 
	    * charectar increment and overwrite previous null-term*/
        }
        sfr_written += tms_utils_fmt(sfr_buf_ptr, 
			(ERR_SSR_REASON_SIZE_BYTES-sfr_written), 
			             sfr_msg_ptr, sfr_p0, sfr_p1, sfr_p2);
      }
      err_sfr_locked = TRUE;
    } /* (sfr_buf_ptr && sfr_file_ptr) */
  } /* (!err_sfr_locked) */

} /* err_log_ssr_failure_reason */

/*=========================================================================

FUNCTION ERR_CALL_TZ_INVOKE_OEM_DEFINED_ROUTINE
DESCRIPTION
  Calls into TZ to invoke OEM defined routine in TZ
===========================================================================*/
static void err_call_tz_invoke_oem_defined_routine(void)
{
  /* TZ SCM Call */
  #define TZBSP_OEM_ERROR_NOTIFICATION_ID 02
  #define TZBSP_SVC_OEM 254
	
  uint32 result = 1;
  struct	 	 
  {	 	 
    uint32   err_fatal;	 	 
  } oem_err_fatal_cmd_buffer; 

  /* Syscall for OEM Error HAndling Request Request */
  (void)scm_call_STM(TZBSP_SVC_OEM, TZBSP_OEM_ERROR_NOTIFICATION_ID, 
       NULL, sizeof(oem_err_fatal_cmd_buffer),
       &result, sizeof(result));
} /* err_call_tz_invoke_oem_defined_routine */


/*=========================================================================

FUNCTION ERR_SMEM_LOG
DESCRIPTION
  Creates error event in SMEM log containing filename and taskname.
===========================================================================*/
static void err_smem_log(void)
{
#ifdef FEATURE_SMEM_LOG
  uint32 name[5];  //this is intentionally uint32, not char, for smem macro
  qurt_thread_t thread_id = coredump.os.thread_id;
  size_t name_len;
  char qurt_task_name[ERR_QURT_TASK_NAME_LEN+1];

  if(thread_id)
  {
	qurt_thread_get_name( &qurt_task_name[0], sizeof(qurt_task_name));

    name_len = MIN( sizeof(name), sizeof(qurt_task_name) );
    strlcpy ((char*)name, qurt_task_name, name_len);
    
    SMEM_LOG_EVENT6_STM (ERR_ERROR_FATAL_TASK,
      name[0], name[1], name[2], name[3], name[4], (uint32) thread_id);

    name_len = 1 + strlen(coredump.err.filename);
    memscpy (name, sizeof(name), coredump.err.filename, name_len);
    SMEM_LOG_EVENT6_STM (ERR_ERROR_FATAL,
      name[0], name[1], name[2], name[3], name[4], coredump.err.linenum);
  }
#endif /* FEATURE_SMEM_LOG */

}

/*===========================================================================

FUNCTION ERROR_FATAL_HANDLER

DESCRIPTION
  This function is invoked from err_fatal_jettison_core. When using JTAG,
  default breakpoint for ERR_FATAL should be placed at this function.
  Will log error to SMEM, kill the PA, and copy the coredump data into
  the err_data structure in unintialized memory.


DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS
  **************************************************************
  ************ THERE IS NO RETURN FROM THIS FUNCTION ***********
  **************************************************************

===========================================================================*/
void err_fatal_handler ( void )
{
  int fptr_index;
  static boolean err_fatal_reentrancy_flag = FALSE ;
  
  if(err_services_init_complete==TRUE)
  {
    if(err_fatal_reentrancy_flag == TRUE)
    {
      /* does not return */
      err_reentrancy_violated();
    }
    else
    {
      err_fatal_reentrancy_flag = TRUE;
    }
  }
  else
  {
     err_initialize_coredump();

     /* Early error, dependencies may not be met */
     err_emergency_error_recovery(); 
  }
  
  err_call_next_to_STM_CB();

  fptr_index=0;
  while(err_preflush_internal[fptr_index] != NULL)
  {
    /* Cycle through internal functions */
    coredump.err.err_current_cb = err_preflush_internal[fptr_index];
    err_flush_cache();
    coredump.err.err_current_cb();
    fptr_index++;
  }

  for(fptr_index=0; fptr_index<ERR_MAX_PREFLUSH_CB; fptr_index++)
  {
    err_get_timetick( 
	&(coredump.err.err_preflush_external[fptr_index].cb_start_tick));
    
    /* Cycle through external functions */
    if(coredump.err.err_preflush_external[fptr_index].err_cb!= 0)
    {
      /* Kick Dog */
      dog_force_kick();

      coredump.err.err_current_cb = 
	      coredump.err.err_preflush_external[fptr_index].err_cb;
      coredump.err.err_preflush_external[fptr_index].cb_start = TRUE;

      err_flush_cache();
      
      coredump.err.err_current_cb();
    }
  }
  
  /* Set the start time for non existent external cb, Can be used to calculate 
   * total time spanned by callbacks */
  err_get_timetick( 
      &(coredump.err.err_preflush_external[ERR_MAX_PREFLUSH_CB].cb_start_tick));

  /* Kick Dog */
  dog_force_kick();

  /* Main loop (cache flush happens here, along with other
   * one-time post-flush operations */
  fptr_index=0;
  while(err_flush_internal[fptr_index] != NULL)
  {
    /* Cycle through internal functions */
    coredump.err.err_current_cb = err_flush_internal[fptr_index];
    coredump.err.err_current_cb();
    fptr_index++;
  }

  #if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 400000))
  /* Compiler bug workaround for RVCT 4.1 P 631 upgrade */
  if (err_always_false)
  {
    return; /* This must NEVER return */
  }
  #endif

} /* err_fatal_handler */


/*===========================================================================

FUNCTION ERR_FATAL_LOCK
DESCRIPTION
  Gets mutex for err_fatal to prevent multiple and/or cascading errors
============================================================================*/
void err_fatal_lock(void)
{
  /* Do INTLOCK SAVE for the locking operation */
  INTLOCK_SAVE(err_int_flag);

  /* Get the start timetick */
  err_get_timetick( &(coredump.err.err_handler_start_time));

  /* Kick Dog */
  dog_force_kick();
} /* err_fatal_lock */


/*===========================================================================

FUNCTION ERR_FATAL_JETTISON_CORE
DESCRIPTION
  Logs fatal error information, including a core dump.
  Not to be called directly by outside code -- for that, use the function
  err_fatal_core_dump().

  NOTE: There is no return from this function.
============================================================================*/
void err_fatal_jettison_core (
  unsigned int line,       /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format,    /* format string */
  uint32 param1,
  uint32 param2,
  uint32 param3
)
{
  /* NOTE: register information should already be saved prior to
   * calling this function.
   */

  /* Get timestamp of error */
  time_get((unsigned long *)&coredump.err.timestamp);

  /* Get thread id(if not pre-filled by err_exception_handler) */
  if (!coredump.os.thread_id) {
  coredump.os.thread_id = (ERR_OS_TCB_TYPE)qurt_thread_get_id();
  }

  /* Store line number */
  coredump.err.linenum = line;

  /* Copy file name */
  if(file_name != 0)
  {
    (void) strlcpy((char *)coredump.err.filename,
                       (char *)file_name,
                       ERR_LOG_MAX_FILE_LEN);
  }

  /* Copy message string */
  if(format != 0)
  {
    (void) strlcpy((char *)coredump.err.message,
                       (char *)format,
                       ERR_LOG_MAX_MSG_LEN);
  }

  coredump.err.param[0]=param1;
  coredump.err.param[1]=param2;
  coredump.err.param[2]=param3;

  /* Call ERR_FATAL handler (no return) */
  err_fatal_handler();

} /* err_fatal_jettison_core */

/*=========================================================================

FUNCTION err_emergency_error_recovery

DESCRIPTION
  Action to be taken when more than one error has occurred, or if an
  error occurs before err_init has completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  No return from this function

===========================================================================*/

static void err_emergency_error_recovery( void )
{
  /* Define action to be taken when multiple crashes occur */

  /* Flush cache */
  err_flush_cache();

  if(!err_services_init_complete)
  {
    /* Notifications have not yet been sent */
    sys_m_set_gpio();
    sys_m_err_notify_peripherals();
  }

  /* Cannot write/clear magic numbers here */

  /* Expire Non secure WDOG bite to let TZ handle reset */
  dog_force_bite();

  /* Spin (will reach this point if not master ERR proc) */
  while(1) 
  {
#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 400000))
    /* Compiler bug workaround for RVCT 4.1 P 631 upgrade */
    if (err_always_false)
    {
      return; /* this must NEVER return */
    }
#endif
  };
} /* err_emergency_error_recovery */


/*=========================================================================

FUNCTION err_terminate

DESCRIPTION
  Invokes dog_force_bite from err prostflush internal callback to 
  let TZ handle reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May not return if action is taken (dload, reset, etc.)

===========================================================================*/
static void err_terminate( void )
{
  err_action_type action;
  
  err_get_timetick( &(coredump.err.err_handler_end_time));
  
  err_flush_cache();

  action = err_get_auto_action();
  
  if ( ERR_RESET == action )
  {
    tms_dload_clear_magic_numbers_STM();
  }
  else if ( ERR_DLOAD == action )
  {
    tms_dload_write_magic_numbers_STM();
  }
  
  err_pause_usec(ERR_ACTION_WAIT_TIME_USEC);
  
  /* Expire Non secure WDOG bite to let TZ handle reset */
  dog_force_bite();

} /* err_terminate */

/*=========================================================================

FUNCTION err_pause_usec

DESCRIPTION
  Pause specified duration of usec. Breaks up pause into smaller chunks
  to prevent warning msg from clk code.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void err_pause_usec(uint32 usec)
{
  int i,kick_ctr=0;

  for(i=0; i<usec; i+=ERR_CLK_PAUSE_SMALL)
  {
    /* Break up call into smaller delays to prevent clk_pause
       warning msg. */

    /* Busywait */
    busywait(ERR_CLK_PAUSE_SMALL);

    /* Make sure dog gets kicked on long delays */
    if(kick_ctr++ >= ERR_CLK_PAUSE_KICK)
    {
      dog_force_kick();  /* ints are disabled, use force_kick */
      kick_ctr = 0;
    }
}
} /* err_pause_usec */


/*=========================================================================

FUNCTION err_loop_pause

DESCRIPTION
  Pause ERR_LOOP_DELAY_USEC; used at end of handling loop.
  Also kicks dog as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void err_loop_pause( void )
{
  static uint32 loop_count=0;

  err_flush_cache();
  
  /* Loop forever until the system is reset */
  while(1)
  {
    /* Delay for specificied time and kick the Dog*/

    /* Pause ERR_LOOP_DELAY_USEC microseconds */
    /* This time interval accounts for one 'loop' of the error handler,
     ignoring overhead from other calls */
    err_pause_usec(ERR_LOOP_DELAY_USEC);
    loop_count++;

    if(loop_count == ERR_LOOP_DOG_FREQ)
    {
      dog_force_kick();  /* Ints are disabled, use force_kick */
      loop_count=0;
    }
  }
} /* err_loop_pause */


/*=========================================================================

FUNCTION err_crash_cb_register

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called after an ERR_FATAL
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.

  !!!These functions MUST NOT call ERR_FATAL/ASSERT under ANY circumstances!!!

DEPENDENCIES
  None

RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_register(err_cb_ptr cb)
{
  int i;
  boolean rval = FALSE;

  for(i=0; i<ERR_MAX_PREFLUSH_CB; i++)
  {
	if(coredump.err.err_preflush_external[i].err_cb == NULL)
  {
	  coredump.err.err_preflush_external[i].err_cb = cb;
    rval = TRUE;
    break;
  }
  }

  return rval;
} /* err_crash_cb_register */


/*=========================================================================

FUNCTION err_crash_cb_dereg

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg(err_cb_ptr cb)
{
  int i;
  boolean rval = FALSE;

  for(i=0; i<ERR_MAX_PREFLUSH_CB; i++)
  {
	if(coredump.err.err_preflush_external[i].err_cb == cb)
  {
	  coredump.err.err_preflush_external[i].err_cb = NULL;
    rval = TRUE;
    break;
  }
  }

  return rval;
} /* err_crash_cb_dereg */

/*=========================================================================

FUNCTION err_crash_cb_reg_next_to_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately after
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback,
  this API was provided only for special case handling to stop ULT Audio Core
  in DPM PL
===========================================================================*/
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb)
{
  if(NULL == coredump.err.err_next_to_STM.err_cb )
  {
    /* Check if already a callback registered*/
    coredump.err.err_next_to_STM.err_cb = cb;
    return TRUE;
  }
  else{
     return FALSE;
  }
}
/*=========================================================================

FUNCTION err_crash_cb_dereg_next_to_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg_next_to_STM(err_cb_ptr cb)
{
  if(coredump.err.err_next_to_STM.err_cb == cb)
  {
    coredump.err.err_next_to_STM.err_cb = NULL;
    return TRUE;
  }
  else{
    return FALSE;
  }
}


/*=========================================================================

FUNCTION err_flush_cache

DESCRIPTION
  Function which will call all necessary cache-flushing APIs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void err_flush_cache(void)
{
  /* Flush the cache */
  qurt_mem_cache_clean(0,0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE);

} /* err_flush_cache */

/*===========================================================================

FUNCTION       err_Fatal_internal3

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_Fatal_internal3 (const err_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 #if defined (FEATURE_SAVE_DEBUG_TRACE)
 msg_const_type msg_blk;
 #endif /* FEATURE_SAVE_DEBUG_TRACE */

 /* Lock Interrupts */
 err_fatal_lock();

/* Capture registers */
 jettison_core();

 /* Kick Dog */
 dog_force_kick();

 coredump.err.compressed_ptr = const_blk;

 #if defined (FEATURE_SAVE_DEBUG_TRACE)
 msg_blk.desc.line    = const_blk->line;
 msg_blk.desc.ss_id   = MSG_SSID_DFLT;
 msg_blk.desc.ss_mask = MSG_LEGACY_FATAL;
 msg_blk.fname        = const_blk->fname;
 msg_blk.fmt          = const_blk->fmt;

 msg_save_3(&msg_blk, code1, code2, code3, NULL);
 #endif /* FEATURE_SAVE_DEBUG_TRACE */
 err_fatal_jettison_core (const_blk -> line,
                          const_blk -> fname,
                          const_blk -> fmt,
                          code1, code2, code3);
} /* err_Fatal_internal3 */

void err_Fatal_internal2 (const err_const_type* const_blk, uint32 code1, uint32 code2)
{
 err_Fatal_internal3(const_blk, code1, code2, 0);

} /* err_Fatal_internal2 */

void err_Fatal_internal1 (const err_const_type* const_blk, uint32 code1)
{
  err_Fatal_internal3(const_blk, code1, 0, 0);

} /* err_Fatal_internal1 */

void err_Fatal_internal0 (const err_const_type* const_blk)
{
  err_Fatal_internal3(const_blk, 0, 0, 0);

} /* err_Fatal_internal0 */

/* Temp function for external reference */
void err_SaveFatal0 (const msg_const_type* const_blk)
{
  err_emergency_error_recovery();
} /* err_SaveFatal0 */


/*===========================================================================

MACRO ERR_FATAL_AUX_MSG

DESCRIPTION
  Accepts a backward compatible, zero-parameter ERR_FATAL string, as well
  as an additional string to fill a dynamic message buffer
  which is stored in the coredump.err.aux_msg.

NOTE
  The SFR created by this message is parsed by some HLOS's/customers.
  As such format maybe important.  Please consult with owners before
  using.

DEPENDENCIES
  None
   
RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  Terminates processing, dumps core

===========================================================================*/

void err_Fatal_aux_msg 
(
  const err_const_type* const_blk, 
  const char * dynamic_msg,
  uint16 size_dynamic_msg
)
{

 #if defined (FEATURE_SAVE_DEBUG_TRACE)
 msg_const_type msg_blk;
 #endif /* FEATURE_SAVE_DEBUG_TRACE */

 /* Lock Interrupts */
 err_fatal_lock();

/* Capture registers */
 jettison_core();

 /* Kick Dog */
 dog_force_kick();

 /* Copy to relevant structures */
 memscpy(coredump.err.aux_msg, sizeof(coredump.err.aux_msg), 
	dynamic_msg, size_dynamic_msg );

 /* Guarantee null termination */
 coredump.err.aux_msg[ERR_LOG_MAX_MSG_LEN-1]=0;

 #ifdef FEATURE_SMEM
 if (err_ssr_smem_buf_ptr&& !err_sfr_locked)
 {
   memscpy(err_ssr_smem_buf_ptr, ERR_SSR_REASON_SIZE_BYTES, 
           coredump.err.aux_msg, sizeof(coredump.err.aux_msg));
   err_sfr_locked = TRUE;
 }
 #endif /* FEATURE_SMEM */

 coredump.err.compressed_ptr = const_blk;

 #if defined (FEATURE_SAVE_DEBUG_TRACE)
 msg_blk.desc.line    = const_blk->line;
 msg_blk.desc.ss_id   = MSG_SSID_DFLT;
 msg_blk.desc.ss_mask = MSG_LEGACY_FATAL;
 msg_blk.fname        = const_blk->fname;
 msg_blk.fmt          = const_blk->fmt;

 msg_save_3(&msg_blk, 0, 0 , 0, NULL);
 #endif /* FEATURE_SAVE_DEBUG_TRACE */
 err_fatal_jettison_core (const_blk -> line,
                          const_blk -> fname,
                          const_blk -> fmt,
                          0, 0, 0);

} /* err_Fatal_aux_msg */

/*=========================================================================

FUNCTION err_reentrancy_violated

DESCRIPTION
  This will only be called when ERR_FATAL is called while processing an
  ERR_FATAL.  It usually means that somone has registered a non-compliant
  callback function using 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define ERR_REENTRANCY_STRING "ERR_FATAL reentrancy violation, remove cb until resolved"
void err_reentrancy_violated(void)
{
 /* Record secondary failure to coredump */
 strlcpy(coredump.err.int_msg, ERR_REENTRANCY_STRING, sizeof(ERR_REENTRANCY_STRING));

 err_emergency_error_recovery();

}
