/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                 P S _ T A S K. C

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  the functions needed to initialize all the modules that execute in PS task
  context and the main task processing loop.


EXTERNALIZED FUNCTIONS
  ps_task()
    Data Protocol Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the PS services can be used unless MC has created the PS task
  and ps_task() has been invoked.

Copyright (c) 2018 - 2022 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/interface/tasks/src/ps_task.c#7 $ $DateTime: 2022/05/24 00:01:24 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ------------------------------------------------------------
05/19/22     rj    FR 78240: Memory optimizations- Disable DHCPv6, PPP, HDLC and SSL
12/30/20     ds    Added XLAT feature.
04/17/20     ds    Code changes to use MUTILS to efficiently utilize DSM items.
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "task.h"
#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "stringl.h"
#include "dog_hb_rex.h"
#include "ds_sys_eventi.h"


#include "ps.h"
#include "ps_svc.h"
#include "ps_taski.h"
#include "ds_system_heap.h"
#include "ds_crit_sect.h"
#include "rmnet_data_ul_legacy.h"
#include "rmnet_opt_defs.h"
#include "data_msg.h"
#include "ds_ipfltr.h"
#include "ds_qos_net_initiated_req.h"
#include "ttl_map.h"
#include "ps_utils_init.h"
#include "ps_sock_mem_pool.h"
#include "ps_stat_commoni.h"
#include "ps_stat_logging.h"
#include "ps_proto_init.h"
#include "ps_addri_v6.h"
#include "ps_ip_fraghdl.h"
#include "ps_hdlc_libi.h"
#include "ps_ppp_task.h"
#include "ps_logging_diag.h"
#include "mutils_dsm.h"
#include "mutils_common.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat_sm.h"
#endif /* #ifdef FEATURE_DATA_PS_464XLAT */


rex_tcb_type  * rex_ps_tcb;

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
  Dog data structure for PS task
---------------------------------------------------------------------------*/
uint32  ps_dog_rpt_id = 0;  /*! dog report ID */ 

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
psi_sig_handler_info_type *psi_sig_handler[PS_MAX_SIGNALS] = {NULL};
ps_cmd_handler_type psi_cmd_handler[PS_MAX_DEFINED_CMD_TYPES] = {default_cmd_handler};

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/


q_type psi_cmd_q;

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/

/*lint -e506 */

rex_sigs_type psi_enabled_sig_mask[PS_MAX_SIG_ARR_LEN];


/*--------------------------------------------------------------------------
  This array stores the extension signals. It extends the signal limit upto 
  the maximum specified via PS_MAX_SIGNALS.  
  This definition is common irrespective of whether FEATURE_REX_SIGS_EXT is
  turned on or off. The array length (PS_EXT_SIG_ARR_LEN) is calculated such
  as to account for the difference.
---------------------------------------------------------------------------*/
/*lint -e506 */
rex_sigs_type ps_ext_sig_mask[PS_EXT_SIG_ARR_LEN];

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PSI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_CMD_Q_SIGNAL.  It retrieves the
  next command from the PS command queue and processes the command by calling
  the registered cmd handler.

DEPENDENCIES
    An item must be on the PS command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_process_cmd
(
  void
)
{
  static byte    ps_cmds_processed_hi_score = 0;
  psi_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &psi_cmd_q ) == 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"psi_process_cmd(): "
                    "No PS command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( psi_cmd_type * ) q_get( &psi_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= PS_MAX_DEFINED_CMD_TYPES )
    {
      DS_MSG1(MSG_LEGACY_ERROR,"psi_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);	  
      ps_free_cmd_buf(cmd_ptr);
      continue;
    }
    processed++;
    psi_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );
	ps_free_cmd_buf(cmd_ptr);
  }

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer ps processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &psi_cmd_q ) == 0 )
  {
#ifdef FEATURE_REX_SIGS_EXT
    PS_CLR_SIGNAL(PS_CMD_Q_SIGNAL);
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2 //Remove the else part
    (void)rex_clr_sigs( rex_ps_tcb, ( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL ) );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
  }

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > ps_cmds_processed_hi_score )
  {
    ps_cmds_processed_hi_score = processed;
    DS_MSG1(MSG_LEGACY_ERROR,"psi_process_cmd(): "
                     "New high # PS commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear ps_cmd_sig from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the PS_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* psi_process_cmd() */

/*===========================================================================

FUNCTION DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean default_sig_handler
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    To avoid lint warning.
  -------------------------------------------------------------------------*/
  (void)user_data_ptr;
  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* default_sig_handler() */


/*===========================================================================

FUNCTION DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void default_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    To avoid lint warning.
  -------------------------------------------------------------------------*/
  (void)user_data_ptr;  
} /* default_cmd_handler() */

/*===========================================================================

FUNCTION PS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS task uses for rex_wait()

DEPENDENCIES
  A valid signal handler should be registered with PS before enabling the
  signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_enable_sig
(
  ps_sig_enum_type sig               /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( psi_sig_handler[sig]->sig_handler == default_sig_handler )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ps_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

  PS_SET_SIG(psi_enabled_sig_mask, sig);

  /*------------------------------------------------------------------------
    To enable multiplexing signal mask.
  -------------------------------------------------------------------------*/
  if((int)sig >= PS_MIN_EXT_SIGNAL)
  {
    PS_SET_SIG(psi_enabled_sig_mask, PS_EXT1_RX_SIGNAL);
#if defined(TEST_FRAMEWORK) && !defined(FEATURE_MODEM_RCINIT_PHASE2)
    #error code not present
#endif
  }

//#if defined(TEST_FRAMEWORK) && !defined(FEATURE_MODEM_RCINIT_PHASE2)
  ps_set_signal(PS_SIG_MASK_CHANGE_SIGNAL);
//#endif
} /* ps_enable_sig() */


/*===========================================================================

FUNCTION PS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ps_disable_sig
(
  ps_sig_enum_type sig                        /* Signal to be disabled     */
)
{
  uint32 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_CLR_SIG(psi_enabled_sig_mask, sig);

  /*-------------------------------------------------------------------------
    Disable the PS_EXT1_RX_SIGNAL signal if all extension signals have been 
    disabled.
  -------------------------------------------------------------------------*/
  /*lint -e506 */
  for(i=PS_SIG_ARR_LEN; i<PS_MAX_SIG_ARR_LEN; i++)
  {
    if(psi_enabled_sig_mask[i] != 0)
    {
      return;
    }
  }  
  PS_CLR_SIG(psi_enabled_sig_mask, PS_EXT1_RX_SIGNAL);
} /* ps_disable_sig() */


/*===========================================================================

FUNCTION PS_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/
ps_sig_handler_type ps_set_sig_handler
(
  ps_sig_enum_type sig,              /* Signal assocaited with handler     */
  ps_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  ps_sig_handler_type old_handler = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 if (sig >= 0 && sig < PS_MAX_SIGNALS)
 {
    DS_ASSERT( sig_handler != NULL );
 
	if(psi_sig_handler[sig] == NULL)
	{
	/*-------------------------------------------------------------------------
	 Allocate memory for PS_RM command type
	-------------------------------------------------------------------------*/
	 DS_SYSTEM_HEAP_MEM_ALLOC(psi_sig_handler[sig], sizeof(psi_sig_handler_info_type),
							 psi_sig_handler_info_type*);
  
	 if(psi_sig_handler[sig] == NULL)
	 {
	   DS_MSG1(MSG_LEGACY_ERROR,"ps_set_sig_handler: Can't get memory for signal handler"
					   "Heap Memory Allocation failed for client: %d" ,
					   sig);
	   DS_ASSERT(0);
	  }  
	}

  old_handler = psi_sig_handler[sig]->sig_handler;

  psi_sig_handler[sig]->sig_handler = sig_handler;
  psi_sig_handler[sig]->user_data_ptr = user_data_ptr;
  }
  return old_handler;
} /* ps_set_sig_handler() */

/*===========================================================================

FUNCTION PS_RESET_SIG_HANDLER()

DESCRIPTION
  This function resets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ps_reset_sig_handler
(
  ps_sig_enum_type sig
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_SYSTEM_HEAP_MEM_FREE(psi_sig_handler[sig]);

} /* ps_set_sig_handler() */



/*===========================================================================

FUNCTION PS_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  NOTE: The passed command will be copied to a PS task
  command item local to the PS Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_send_cmd
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  psi_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal PS task
  -------------------------------------------------------------------------*/
  cmd_ptr = ps_get_cmd_buf();
  if(cmd_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ps_rm_send_cmd(): "
                    "PS_RM task cmd buffers unavailable" );
  }
  else
  {  
    q_link(cmd_ptr, &cmd_ptr->link);
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    q_put( &psi_cmd_q, &cmd_ptr->link );
    ps_set_signal( PS_CMD_Q_SIGNAL );
  }
} /* ps_send_cmd() */


/*===========================================================================

FUNCTION PS_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL
  function pointer is passed the cmd handling is disabled.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/
ps_cmd_handler_type ps_set_cmd_handler
(
  ps_cmd_enum_type cmd,              /* Command name                      */
  ps_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  ps_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = psi_cmd_handler[cmd];

  psi_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                           cmd_handler : default_cmd_handler;

  return old_handler;
} /* ps_set_cmd_handler() */

/*===========================================================================

FUNCTION PS_SET_SIGNAL()

DESCRIPTION
  This function sets a signal to PS task (ps_tcb) for processing.
  - Recommended API for all signals including ext signals.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_set_signal
(
  ps_sig_enum_type sig
)
{
  rex_sigs_type sig_arr[PS_SIG_ARR_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    If PS TCB is not set, just ignore the signal!
  -----------------------------------------------------------------------*/  
  if (rex_ps_tcb == NULL)
  {
    return;
  }
  
  memset(sig_arr, 0, sizeof(sig_arr));

  if((int)sig >= PS_MIN_EXT_SIGNAL)
  {
    PS_SET_EXT_SIG(sig);
    sig = PS_EXT1_RX_SIGNAL;
  }
  PS_SET_SIG(sig_arr, sig);

#ifdef FEATURE_REX_SIGS_EXT
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(FALSE == (rex_set_sigs_ext(rex_ps_tcb, sig_arr)))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ps_set_signal(): "
                    "Failure to set signal %d in ps_tcb", sig);
     DS_ASSERT(0);
   }  
 #endif
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_set_sigs( rex_ps_tcb, sig_arr[0] );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
} /* ps_set_signal */

/*===========================================================================

FUNCTION PS_CLR_SIGNAL()

DESCRIPTION
  This function clears the signal posted onto PS task (ps_tcb).
  - Recommended API for all signals including ext signals.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_clr_signal
(
  ps_sig_enum_type sig
)
{
  rex_sigs_type sig_arr[PS_SIG_ARR_LEN];
  uint32 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    PS_EXT1_RX_SIGNAL cannot be cleared explicitly. Verify.
  -------------------------------------------------------------------------*/
  if(sig == PS_EXT1_RX_SIGNAL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ps_clr_signal(): "
              "PS_EXT1_RX_SIG cannot be cleared explicitly.");
    DS_ASSERT(0);
  }
  
  memset(sig_arr, 0, sizeof(sig_arr));

  if((int)sig >= PS_MIN_EXT_SIGNAL)
  {
    PS_CLR_EXT_SIG(sig);
    /*-----------------------------------------------------------------------
      Clear the PS_EXT1_RX_SIGNAL signal if all extension signals have been 
      cleared.
    -----------------------------------------------------------------------*/
    /*lint -e506 */
    for(i=0; i<PS_EXT_SIG_ARR_LEN; i++)
    {
      if(ps_ext_sig_mask[i] != 0)
      {
        return;
      }
    }
    sig = PS_EXT1_RX_SIGNAL;
  }

  /*-------------------------------------------------------------------------
    Set the signal to be cleared by calling REX api
  -------------------------------------------------------------------------*/
  PS_SET_SIG(sig_arr, sig);
  
#ifdef FEATURE_REX_SIGS_EXT
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(FALSE == (rex_clr_sigs_ext(rex_ps_tcb, sig_arr))) 
  { 
    DS_MSG1(MSG_LEGACY_ERROR,"ps_clr_signal(): "
                    "Failure to clear signal %d in ps_tcb", sig);
     DS_ASSERT(0);
  } 
  #endif
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_clr_sigs( rex_ps_tcb, sig_arr[0] );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
}/* ps_clr_signal */

/*===========================================================================
	
FUNCTION PS_GET_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
  psi_cmd_type * Buffer

SIDE EFFECTS
  None.
	
===========================================================================*/

psi_cmd_type* ps_get_cmd_buf()
{
  psi_cmd_type  *psi_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate memory for PS_RM command type
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(psi_cmd_ptr, sizeof(psi_cmd_type),
                            psi_cmd_type*);
  if(psi_cmd_ptr != NULL)
  {
    memset(psi_cmd_ptr , 0, sizeof(psi_cmd_type));
    /*-----------------------------------------------------------------------
      Ensure command payload pointer points to the payload allocated
  -----------------------------------------------------------------------*/  
   return(psi_cmd_ptr);
  }
  return NULL;
}


/*===========================================================================


FUNCTION PS_FREE_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.
	
===========================================================================*/

void ps_free_cmd_buf(psi_cmd_type *ps_cmd_ptr)
{

 #ifdef FEATURE_Q_NO_SELF_QPTR
	 q_delete(&psi_cmd_q, &(ps_cmd_ptr->link));
 #else
	 q_delete(&(ps_cmd_ptr->link));
 #endif

  DS_SYSTEM_HEAP_MEM_FREE(ps_cmd_ptr);
}

/*===========================================================================
FUNCTION PSI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the data Protocol task. This
  function will perform all first time thru functions for the Protocol task.
  Note that initializations internal to PS should be performed before
  initializing other components that run in PS task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void psi_powerup
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_ERROR,"psi_powerup(): "
                  "Protocol Service Task Powerup");

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to PS Task
  -------------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

  PS_SET_SIG(psi_enabled_sig_mask, PS_TASK_OFFLINE_SIGNAL);
  PS_SET_SIG(psi_enabled_sig_mask, PS_CMD_Q_SIGNAL);
  PS_SET_SIG(psi_enabled_sig_mask, PS_DOG_HB_REPORT_SIGNAL);
  PS_SET_SIG(psi_enabled_sig_mask, PS_SIG_MASK_CHANGE_SIGNAL);

} /* psi_powerup() */

/*===========================================================================
FUNCTION PS_MUTILS_REG()

DESCRIPTION
  This function register with MUTILS library so as to allow DSM item 
  allocations from PS task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_mutils_reg()
{
  mutils_dsm_anysize_reg_param_s   anysize_param;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  DS_MSG0(MSG_LEGACY_MED,"ps_mutils_reg(): "
                  "Register PS client with MUTILS");
  
  /*Register client with MUTILS library now */
  mutils_dsm_anysize_clt_init_param(&anysize_param);

  anysize_param.lock = MUTILS_DSM_ANYSIZE_REG_LOCKLESS;
  
  mutils_dsm_anysize_clt_reg(MUTILS_DSM_CLIENT_PS,&anysize_param);
  
  return;
}
/*===========================================================================
FUNCTION PSI_INIT()

DESCRIPTION
  Initialization function for the data services task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  PS task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void psi_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the command queue for PS_CMD_Q_SIGNAL signal
  -------------------------------------------------------------------------*/
  (void) q_init( &psi_cmd_q );
 /*-------------------------------------------------------------------------
   Initialize Refresh notification framework
 -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  ps_utils_refresh_init();     //Review it once 
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

  /*-------------------------------------------------------------------------
    Initialize pssock module
  -------------------------------------------------------------------------*/
  //ps_sock_init();  Check with Sid
  /*-------------------------------------------------------------------------
    Initialize DSS module
  -------------------------------------------------------------------------*/
  //dss_init(); Check with Sid
  
   ps_utils_powerup();   //Move to DS task 
  /*------------------------------------------------------------------------------
	Initialize the Socket Memory Pool
  -----------------------------------------------------------------------------*/ 
  ps_sock_mem_pool_init();
 
  ds_ipfltr_init();
  ds_fwk_qos_net_initiated_req_init();
  //moved here tbd_aks

#ifdef FEATURE_DATA_PS_HDLC
  ps_hdlc_lib_init();  
#endif

#ifdef FEATURE_DATA_PS_PPP 
  ppp_task_powerup_init();
#endif
  
  /*-------------------------------------------------------------------------
    Initialize EFS config file for DPM
  -------------------------------------------------------------------------*/  
  //ps_dpm_init_efs_config_file();

  /*-------------------------------------------------------------------------
    Initialize DS modules
  -------------------------------------------------------------------------*/
  ps_utils_init();    //Consolidate with DS task api once
  
  //DSNetInit();  Check with Sid
  //DSSockInit();  Check with Sid

#ifdef PS_LOGGING_IOE_DPL
  dpl_init();
#endif //PS_LOGGING_IOE_DPL

  ps_stat_init();
  ps_stat_logging_init();
  rmnet_data_ul_legacy_init();
  rmnet_ul_opt_init();
  
  // Data Protocols Initialization
  ps_proto_init();
  
  ps_addr_v6_init();
  ip_fraghdl_init();
  ps_mutils_reg();

 /*-------------------------------------------------------------------------
    Initialize all protocol modules
  -------------------------------------------------------------------------*/  
#ifdef FEATURE_DATA_PS_464XLAT
  ps_clat_powerup();
#endif /* FEATURE_DATA_PS_464XLAT */

} /* psi_init() */


/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
/*===========================================================================

FUNCTION PS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Protocol Service(s) task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_task
(
  uint32 ignored
    /* Parameter received from Main Control task - ignored */
)
{
  /*lint -e506 */
  rex_sigs_type     loop_sigs[PS_MAX_SIG_ARR_LEN]; /* Signals to be processed
                                           within a task loop */
  /*lint -e506 */
  rex_sigs_type     set_sigs[PS_MAX_SIG_ARR_LEN]; /* Currently set signals */
  boolean           sig_result = FALSE; /*The result value of the extension
                                          signal API is stored in this var.*/
  uint32            loop_cnt;       /* # of times task loop is iterated
                                         on a signal mask                  */
  uint32            i = 0, j = 0;

#if (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE))
  #error code not present
#endif /* (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)) */

#ifdef FEATURE_TTL
   TTLBOOTUP(STARTUP_PS_TASK);	//to -Do feature
#endif /* FEATURE_TTL*/

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    In order to avoid lint warning.
  -------------------------------------------------------------------------*/
  (void)ignored;
  rex_ps_tcb = rex_self();

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /*-------------------------------------------------------------------------
    Dog heart beat registration
  -------------------------------------------------------------------------*/
  ps_dog_rpt_id        = dog_hb_register_rex(
                 (rex_sigs_type)1 << (rex_sigs_type)PS_DOG_HB_REPORT_SIGNAL);

  
  rcinit_handshake_startup();
  psi_init(); /* Refactor! */  

#ifdef FEATURE_TTL
	TTLBOOTUP(STARTUP_PS_TASK_DONE);	 //to -Do feature
#endif /* FEATURE_TTL*/

  memset(set_sigs, 0, sizeof(set_sigs));

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/
    memscpy(loop_sigs, sizeof(loop_sigs), set_sigs, sizeof(set_sigs));

#ifdef FEATURE_REX_SIGS_EXT
  
    sig_result = rex_get_sigs_ext(rex_ps_tcb, set_sigs);

    if(sig_result == FALSE)
    {
      DATA_ERR_FATAL("Failed to obtain the signals from the PS TCB");
      return;
    }
#else

    set_sigs[0] = rex_get_sigs( rex_ps_tcb );
  
#endif
    /*-----------------------------------------------------------------------
      Set the extension signal part in set_sigs array if set.
    -----------------------------------------------------------------------*/
    memscpy(&(set_sigs[PS_SIG_ARR_LEN]), sizeof(set_sigs[PS_SIG_ARR_LEN]), ps_ext_sig_mask, 
             sizeof(ps_ext_sig_mask));
    /*-----------------------------------------------------------------------
      Restore sigs set from the previous processing loop.
      (We copied set_sigs content into loop_sigs above)
    -----------------------------------------------------------------------*/
    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      set_sigs[i] |= loop_sigs[i];
    }

    /*-----------------------------------------------------------------------
      Check if any of the enabled signal is set.
    -----------------------------------------------------------------------*/
    sig_result = FALSE;
    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      set_sigs[i] &= psi_enabled_sig_mask[i];
      if(0 != set_sigs[i])
      {
        sig_result = TRUE;
      }
    }

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( FALSE == sig_result )
    {
#ifdef FEATURE_REX_SIGS_EXT
      sig_result = rex_wait_ext(psi_enabled_sig_mask, set_sigs);
      if(sig_result == FALSE)
      {
        DATA_ERR_FATAL("ps_task():rex_wait_ext() failed.");
        return;
      }
#else
      set_sigs[0] = rex_wait( psi_enabled_sig_mask[0] );
#endif
      /*---------------------------------------------------------------------
        Read extension signal part in set_sigs array if set.
      ---------------------------------------------------------------------*/
      memscpy(&(set_sigs[PS_SIG_ARR_LEN]), sizeof(set_sigs[PS_SIG_ARR_LEN]), ps_ext_sig_mask, 
               sizeof(ps_ext_sig_mask));
      /*-----------------------------------------------------------------------
        We need to process only the enabled signals.
      -----------------------------------------------------------------------*/
      /*lint -e506 */
      for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
      {
        set_sigs[i] &= psi_enabled_sig_mask[i];
      }
    }

#ifdef FEATURE_REX_SIGS_EXT

    sig_result = rex_clr_sigs_ext(rex_ps_tcb, set_sigs);

    if(sig_result == FALSE)
    {
      DS_ERR_FATAL("ps_task():rex_clr_sigs_ext() failed.");
      return;
    }
#else
  
    (void)rex_clr_sigs( rex_ps_tcb, set_sigs[0] );
  
#endif
    /*-----------------------------------------------------------------------
      Now clear the extension signals which we will be processing in this 
      loop.
    -----------------------------------------------------------------------*/
    /*lint -e506 */
    for(i=0, j=PS_SIG_ARR_LEN; i<PS_EXT_SIG_ARR_LEN; i++, j++)
    {
      ps_ext_sig_mask[i] &= ~set_sigs[j];
    }
    /*-----------------------------------------------------------------------
      Clear the signals which don't need to be processed explicitly.
    -----------------------------------------------------------------------*/
    PS_CLR_SIG(set_sigs, PS_SIG_MASK_CHANGE_SIGNAL);
    PS_CLR_SIG(set_sigs, PS_EXT1_RX_SIGNAL); 

    /*-----------------------------------------------------------------------
      Copy set_sigs into loop_sigs for processing and clear set_sigs.
    -----------------------------------------------------------------------*/
    memscpy(loop_sigs, sizeof(loop_sigs), set_sigs, sizeof(set_sigs));
    memset(set_sigs, 0, sizeof(set_sigs));

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than PS_MAX_SIGNALS times, some signal is
      not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    sig_result = TRUE;
    while( sig_result && loop_cnt++ < (int)PS_MAX_SIGNALS )
    {
      DS_ENTER_CRIT_SECTION(&ps_ds_crit_section);

      sig_result = FALSE;
      /*---------------------------------------------------------------------
        If dog heart beat received, ack the beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      if(PS_IS_SET_SIG(loop_sigs, PS_DOG_HB_REPORT_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_DOG_HB_REPORT_SIGNAL);
        dog_hb_report( ps_dog_rpt_id );
      }
      /*---------------------------------------------------------------------
        PS Task command queue signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_CMD_Q_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_CMD_Q_SIGNAL);
        if ( ps_process_cmd() == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_CMD_Q_SIGNAL);
        }
      }
	  else if(PS_IS_SET_SIG(loop_sigs, PS_FWK_DL_SIGNAL_0 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_FWK_DL_SIGNAL_0);
        if ( psi_sig_handler[PS_FWK_DL_SIGNAL_0])
        {
           if ( psi_sig_handler[PS_FWK_DL_SIGNAL_0]->sig_handler
             (
               PS_FWK_DL_SIGNAL_0,
               psi_sig_handler[PS_FWK_DL_SIGNAL_0]->user_data_ptr
             ) == FALSE )
           {
             PS_SET_SIG(set_sigs, PS_FWK_DL_SIGNAL_0);
           }
        }
      }  
      else if(PS_IS_SET_SIG(loop_sigs, PS_FWK_DL_SIGNAL_1 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_FWK_DL_SIGNAL_1);		
        if ( psi_sig_handler[PS_FWK_DL_SIGNAL_1])
        {
          if ( psi_sig_handler[PS_FWK_DL_SIGNAL_1]->sig_handler
             (
               PS_FWK_DL_SIGNAL_1,
               psi_sig_handler[PS_FWK_DL_SIGNAL_1]->user_data_ptr
             ) == FALSE )
          {
            PS_SET_SIG(set_sigs, PS_FWK_DL_SIGNAL_1);
          }
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_FWK_DL_SIGNAL_2 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_FWK_DL_SIGNAL_2);		
        if ( psi_sig_handler[PS_FWK_DL_SIGNAL_2])
        {
          if ( psi_sig_handler[PS_FWK_DL_SIGNAL_2]->sig_handler
             (
               PS_FWK_DL_SIGNAL_2,
               psi_sig_handler[PS_FWK_DL_SIGNAL_2]->user_data_ptr
             ) == FALSE )
          {
            PS_SET_SIG(set_sigs, PS_FWK_DL_SIGNAL_2);
          }
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_FWK_DL_SIGNAL_3 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_FWK_DL_SIGNAL_3);		
        if ( psi_sig_handler[PS_FWK_DL_SIGNAL_3])
        {
          if ( psi_sig_handler[PS_FWK_DL_SIGNAL_3]->sig_handler
             (
               PS_FWK_DL_SIGNAL_3,
               psi_sig_handler[PS_FWK_DL_SIGNAL_3]->user_data_ptr
             ) == FALSE )
          {
            PS_SET_SIG(set_sigs, PS_FWK_DL_SIGNAL_3);
          }
        }
      } 
      else if(PS_IS_SET_SIG(loop_sigs, PS_FWK_DL_SIGNAL_4 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_FWK_DL_SIGNAL_4);		
        if ( psi_sig_handler[PS_FWK_DL_SIGNAL_4])
        {
          if ( psi_sig_handler[PS_FWK_DL_SIGNAL_4]->sig_handler
             (
               PS_FWK_DL_SIGNAL_4,
               psi_sig_handler[PS_FWK_DL_SIGNAL_4]->user_data_ptr
             ) == FALSE )
          {
            PS_SET_SIG(set_sigs, PS_FWK_DL_SIGNAL_4);
          }
        }
      }
	 /*---------------------------------------------------------------------
        RX signal processing
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_RMNET1_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_RMNET1_SIGNAL);
        if ( psi_sig_handler[PS_RX_RMNET1_SIGNAL]->sig_handler
             (
               PS_RX_RMNET1_SIGNAL,
               psi_sig_handler[PS_RX_RMNET1_SIGNAL]->user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_RMNET1_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_SOCKET_RX_Q_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_SOCKET_RX_Q_SIGNAL);
        if ( psi_sig_handler[PS_SOCKET_RX_Q_SIGNAL]->sig_handler
             (
               PS_SOCKET_RX_Q_SIGNAL,
               psi_sig_handler[PS_SOCKET_RX_Q_SIGNAL]->user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_SOCKET_RX_Q_SIGNAL);
        }
      }
       else if(PS_IS_SET_SIG(loop_sigs, PS_DPL_DIAG_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_DPL_DIAG_SIGNAL);
        if ( psi_sig_handler[PS_DPL_DIAG_SIGNAL]->sig_handler
             (
               PS_DPL_DIAG_SIGNAL,
               psi_sig_handler[PS_DPL_DIAG_SIGNAL]->user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPL_DIAG_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
       Incoming TE2 data signal
     ---------------------------------------------------------------------*/
     else if(PS_IS_SET_SIG(loop_sigs, PS_RM_RX_SIGNAL ))
     {
       PS_CLR_SIG(loop_sigs, PS_RM_RX_SIGNAL);
       if ( psi_sig_handler[PS_RM_RX_SIGNAL]->sig_handler
            (
              PS_RM_RX_SIGNAL,
              psi_sig_handler[PS_RM_RX_SIGNAL]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_RM_RX_SIGNAL);
       }
     }
#ifdef FEATURE_MODE_TLB
     else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_0 ) )
     {
       PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_0);
       if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_0]->sig_handler
            (
              PS_3GPP_UM_RX_Q_SIGNAL_0,
              psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_0]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_0);
       }
     }
     else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_1 ) )
     {
       PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_1);
       if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_1]->sig_handler
            (
              PS_3GPP_UM_RX_Q_SIGNAL_1,
              psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_1]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_1);
       }
     }
     else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_2 ) )
     {
       PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_2);
       if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_2]->sig_handler
            (
              PS_3GPP_UM_RX_Q_SIGNAL_2,
              psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_2]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_2);
       }
     }
     else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_3 ) )
     {
       PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_3);
       if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_3]->sig_handler
            (
              PS_3GPP_UM_RX_Q_SIGNAL_3,
              psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_3]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_3);
       }
     }
     else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_4 ) )
     {
       PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_4);
       if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_4]->sig_handler
            (
              PS_3GPP_UM_RX_Q_SIGNAL_4,
              psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_4]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_4);
       }
     }
#endif /* FEATURE_MODE_TLB */
     /*---------------------------------------------------------------------
       Incoming second TE2 data signal
     ---------------------------------------------------------------------*/
     else if(PS_IS_SET_SIG(loop_sigs, PS_RM_2_RX_SIGNAL ))
     {
       PS_CLR_SIG(loop_sigs, PS_RM_2_RX_SIGNAL);
       if ( psi_sig_handler[PS_RM_2_RX_SIGNAL]->sig_handler
            (
              PS_RM_2_RX_SIGNAL,
              psi_sig_handler[PS_RM_2_RX_SIGNAL]->user_data_ptr
            ) == FALSE )
       {
         PS_SET_SIG(set_sigs, PS_RM_2_RX_SIGNAL);
       }
     }
     
      /*---------------------------------------------------------------------
        Incoming PPP data signal
      ---------------------------------------------------------------------*/
#ifdef FEATURE_MODE_TLB
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL ) )
        {
          PS_CLR_SIG(loop_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL);
          if ( psi_sig_handler[PS_3GPP_LO_B_UM_RX_Q_SIGNAL]->sig_handler
               (
                 PS_3GPP_LO_B_UM_RX_Q_SIGNAL,
                 psi_sig_handler[PS_3GPP_LO_B_UM_RX_Q_SIGNAL]->user_data_ptr
               ) == FALSE )
          {
            PS_SET_SIG(set_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL);
          }
        }
#endif /* FEATURE_MODE_TLB */
      /*---------------------------------------------------------------------
        Incoming PPP data signal for the HDR AN Authentication PPP
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_RMSM_AT_UM_RX_Q_SIGNAL ) )
      {
        PS_CLR_SIG(loop_sigs, PS_RMSM_AT_UM_RX_Q_SIGNAL);
        if ( psi_sig_handler[PS_RMSM_AT_UM_RX_Q_SIGNAL]->sig_handler
             (
               PS_RMSM_AT_UM_RX_Q_SIGNAL,
               psi_sig_handler[PS_RMSM_AT_UM_RX_Q_SIGNAL]->user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RMSM_AT_UM_RX_Q_SIGNAL);
        }
      }
      
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
      #error code not present
#endif
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if(PS_IS_SET_SIG(loop_sigs, PS_TASK_OFFLINE_SIGNAL))
      {
        DS_MSG0(MSG_LEGACY_ERROR,"ps_task(): PS_TASK_OFFLINE_SIGNAL received");
        PS_CLR_SIG(loop_sigs, PS_TASK_OFFLINE_SIGNAL);
      }

      /*---------------------------------------------------------------------
        Check if we have processed all the signals.
      ---------------------------------------------------------------------*/
      /*lint -e506 */
      for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
      {
        if(0 != loop_sigs[i])
        {
          sig_result = TRUE;
        }
      }

      DS_LEAVE_CRIT_SECTION(&ps_ds_crit_section);
    }

#if (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE))
    #error code not present
#endif /* (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)) */

    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      if(0 != loop_sigs[i])
      {
        DS_MSG2(MSG_LEGACY_ERROR,"ps_task(): "
                        "Signals not completely processed, loop_sigs[%d] = %d",
                        i, loop_sigs[i]);
        DS_ASSERT(0);
      }
    }
  } /* forever */
} /* ps_task() */

