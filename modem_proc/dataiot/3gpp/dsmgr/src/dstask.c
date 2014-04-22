/*===========================================================================
                      DSTASK.C
DESCRIPTION
This file defines dstask related APIs.

Copyright (c) 2019 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    pk      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "ttl_map.h"


#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "queue.h"
#include "data_msg.h"
#include "dog.h"

#include "modem_mem.h"

#include "task.h"
#include "dog_hb_rex.h"


#include "rcinit_rex.h"

#include "dstaski.h"

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "lte_tlb.h"
#include "dslteps_ut_msg.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */


#include "ds_prof.h"
#include "dsmsgr.h"
#include "ds_pdn_defs.h"
#include "ds_pdn_hdlr.h"
#include "ds_fwk.h"
#include "dss_control.h"
#include "dsati.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds3gmmgsdiif.h"
#include "ds_rmsmi.h"
#include "ds_fwki.h"

#include "ds_appsrv_task.h"
#ifdef QWES_FEATURE_ENABLE
#include "ds_qmi_qteels_client.h"
#endif

#include "ps_proto_init.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/
/*-------------------------------------------------------------------------
  DS command process data block structure used for queues
-------------------------------------------------------------------------*/
typedef struct
{
  q_link_type               link;
  ds_module_enum_type_i       ds_module_id;
  ds_cmd_process_func_type  cb_func_ptr;
}ds_cmd_process_block;

rex_sigs_type   requested_sigs;   /* Signal mask to suspend on           */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
ds_sig_handler_info_type *ds_sig_handler[DS_MAX_SIGNALS] = {NULL};

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data services command queues
---------------------------------------------------------------------------*/
static q_type ds_cmd_free_q = {{NULL}};    /* Queue for storing free command buffers */
static q_type ds_cmd_q = {{NULL}};         /* DS Task command queue */

/*---------------------------------------------------------------------------
  Dog data structures for DS task
---------------------------------------------------------------------------*/
static uint32  ds_task_dog_rpt_id = 0;         /*! dog report ID */


/*---------------------------------------------------------------------------
  TCB pointer for DS task
---------------------------------------------------------------------------*/
static rex_tcb_type* ds_task_ptr = NULL;

/*-------------------------------------------------------------------------
  DS queues to store module command processing callbacks
-------------------------------------------------------------------------*/
static q_type ds_cmd_process_q = {{NULL}};
static q_type ds_cmd_process_free_q = {{NULL}};

/*-------------------------------------------------------------------------
  Array of DS command processing block
-------------------------------------------------------------------------*/
ds_cmd_process_block ds_task_cmd_process_list[DS_MODULE_MAX] = {{{NULL}}};


nv_item_type         ds_nv_item;
nv_stat_enum_type    ds_nv_status;

static nv_cmd_type ds_nv_cmd_buf;

/*===========================================================================

              EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST //#ifdef TEST_FRAMEWORK
/*===========================================================================
FUNCTION      DSI_UT_SEND_SYNC_CNF

DESCRIPTION

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static void dsi_ut_send_sync_cnf(void)
{
  boolean               rval;
  msgr_hdr_struct_type  msg; /* Message with only the header and no body */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rval = ds_msgr_snd_msg_send(DS_UT_DSTASK_SYNC_CNF, &msg, sizeof(msg));
  DS_ASSERT(rval == TRUE);
  return;
} /* dsi_ut_send_sync_cnf() */

/*===========================================================================
FUNCTION      DSI_UT_SYNC_REQ_HDLR

DESCRIPTION

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static boolean dsi_ut_sync_req_hdlr
(
  msgr_umid_type        msgtype,
  const msgr_hdr_struct_type *msg_p
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (msgtype == DS_UT_DSTASK_SYNC_REQ)
  {
    ds_ut_sync_command_hdlr();
  }
  else
  {
    /* We don't expect to receive any other message type
    */
    DS_MSG1(MSG_LEGACY_LOW,
                    "MSG type %d!= DS_UT_DSTASK_SYNC_REQ",msgtype);
    DS_ASSERT(0);
  }

  return TRUE;
} /* dsi_ut_sync_req_hdlr() */

/*===========================================================================
FUNCTION      DSI_UT_DS_CLOSE_READY_HDLR

DESCRIPTION

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static boolean dsi_ut_ds_close_ready_hdlr
(
  msgr_umid_type               msgtype,
  const msgr_hdr_struct_type  *msg_p
)
{
  boolean               rval = FALSE;
  msgr_hdr_struct_type  msg; /* Message with only the header and no body */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (msgtype == LTE_TLB_DS_CTL_READY_IND)
  {
    rval = ds_msgr_snd_msg_send(DS_UT_LTE_TLB_DS_CTL_READY_CNF,
                              &msg,
                              sizeof(msg));
    DS_ASSERT(rval == TRUE);
    return TRUE;
  }
  else
  {
    /* We don't expect to receive any other message type
    */
    DS_ASSERT(0);
  }

  return rval;
} /* dsi_ut_ds_close_ready_hdlr() */
#if 0
/*===========================================================================
FUNCTION      DSI_UT_DS_BEARER_QOS_INFO_IND_HDLR

DESCRIPTION   

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static boolean dsi_ut_ds_bearer_qos_info_ind_hdlr
(
  msgr_umid_type               msgtype,
  const msgr_hdr_struct_type  *msg_p
)
{
  boolean rval = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (msgtype == DS_3GPP_BEARER_QOS_INFO_IND)
  {
    (void)ds_msgr_snd_msg_send(DS_UT_3GPP_BEARER_QOS_INFO_IND,
                              (msgr_hdr_struct_type *)msg_p,
                              sizeof(ds_3gpp_bearer_qos_info_msg_s));
    return TRUE;
  }
  else
  {
    /* We don't expect to receive any other message type
    */
    DS_ASSERT(0);
  }

  return rval;
}/* dsi_ut_ds_bearer_qos_info_ind_hdlr() */
#endif
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

/*===========================================================================
FUNCTION      DSI_POWERUP_INIT

DESCRIPTION   This function performs all the power-up initialization for the 
              Data Services Task.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds_powerup_init( void )
{
  uint8  i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
  -------------------------------------------------------------------------*/
  (void)q_init( &ds_cmd_q );
  (void)q_init( &ds_cmd_free_q );

  /*-----------------------------------------------------------------------
   Initialize DS cmd process queues for module registration
  -----------------------------------------------------------------------*/
  q_init(&ds_cmd_process_q);
  q_init(&ds_cmd_process_free_q);
  for(i = 0; i < DS_MODULE_MAX; i++)
  {
    memset(&ds_task_cmd_process_list[i], 0, sizeof(ds_cmd_process_block));
    ds_task_cmd_process_list[i].ds_module_id = DS_MODULE_INVALID;
    ds_task_cmd_process_list[i].cb_func_ptr  = NULL;

    q_link((void *)&ds_task_cmd_process_list[i], 
		           &(ds_task_cmd_process_list[i].link));
    q_put(&ds_cmd_process_free_q, &ds_task_cmd_process_list[i].link);
  }

  /*-------------------------------------------------------------------------
    Initialize DSSNET6 module.
  -------------------------------------------------------------------------*/
 // dssnet6_init();  // need to check and move under 3gpp init

/* Init 3gpp critical sections 
  ds3gpp_init_all_crit_sect();                                                      directly called */

/*-------------------------------------------------------------------------
    Initialize SIOLIB -- done at task creation time so that the RDM
    registrations are performed before the rdm_issue_opens() call is made.
   -------------------------------------------------------------------------*/
   ds3g_siolib_init();//siolib also need to check

  /* PSM ULOG REG */
  ds_pdn_psm_ulog_register();

  ds_fwk_ulog_register();

//  ds3g_init_crit_sect();  
  //DS_INIT_CRIT_SECTION(&ds3g_timer_crit_sect);


} /* ds_powerup_init() */

/*===========================================================================
FUNCTION      DS_WAIT

DESCRIPTION   This function suspends execution of the Data Services task
              until one (or more) signals from the requested signal mask is
              set.
              Watchdog kicking is performed in this function.

DEPENDENCIES  None

RETURN VALUE  The signal mask returned by rex_wait().

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type  ds_wait
(
  rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
)
{
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/
  do
  {
    set_signals = rex_wait( requested_signal_mask | 
		                    GET_SIG(DS_DOG_HB_REPORT_SIG ));

    if( (set_signals & GET_SIG(DS_DOG_HB_REPORT_SIG )) != 0 )
    {
      /*---------------------------------------------------------------------
        Report back to Dog heart beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      dog_hb_report( ds_task_dog_rpt_id);
      /*---------------------------------------------------------------------
        Processed the DS_DOG_HB_REPORT_SIG so clear it off in set_signals.
      ---------------------------------------------------------------------*/
      (void) rex_clr_sigs( ds_task_ptr, GET_SIG(DS_DOG_HB_REPORT_SIG ) );
      set_signals &= (~(GET_SIG(DS_DOG_HB_REPORT_SIG )));
    }
  } while( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* ds_wait() */

/*===========================================================================
FUNCTION      DS_GET_NEXT_CMD

DESCRIPTION   This function de-queues and returns a command from the 
              Data Services Task's command queues. Command is dequeued from
              the high priority internal command queue, if available,
              otherwise from the regular command queue.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static ds_command_type* ds_get_next_cmd( void )
{
  ds_command_type *cmd_ptr;

  cmd_ptr = (ds_command_type *)q_get( &ds_cmd_q );
  
  return cmd_ptr;
} /* ds_get_next_cmd() */

/*===========================================================================
FUNCTION      DS_PROCESS_CMDS

DESCRIPTION   This function de-queues commands from the Data Services Task's 
              command queue, and dispataches commands to the appropriate
              entity for further processing, if necessary. Commands are
              de-queued until the command queue is empty.

DEPENDENCIES  This function should be called when the DS_CMD_Q_SIG is set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds_process_cmds( void )
{
  ds_command_type           *cmd_ptr = NULL;
  boolean                processed_cmd = FALSE;
  ds_module_enum_type_i    module_id = DS_MODULE_INVALID;
  uint8                  q_count = 0;
  ds_cmd_process_block  *data_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, dispatch the command to the appropriate sub-task.
  -------------------------------------------------------------------------*/
  while( (cmd_ptr = ds_get_next_cmd()) != NULL )
  {
    processed_cmd = FALSE;

#ifdef DS_TASK_DEBUG
    /*-----------------------------------------------------------------------
      Do not Strip.  Here for debugging purposes only.
    -----------------------------------------------------------------------*/
    if ( (cmd_ptr->hdr.cmd_id <= DS_MIN_CMD) ||
         (cmd_ptr->hdr.cmd_id >= DS_MAX_CMD)
       )
    {
      DS_MSG1(MSG_LEGACY_HIGH,
                      "cmd_id %d is not within DS_MIN_CMD and DS_MAX_CMDS",
                      (int)cmd_ptr->hdr.cmd_id);
      DS_ASSERT(0);
    }
#endif /* DS_TASK_DEBUG */

    /*-----------------------------------------------------------------------
      New command processing logic
    -----------------------------------------------------------------------*/
    /* Get module id from command id for new commands */
    module_id 
      = ((uint32)(cmd_ptr->hdr.cmd_id)) / DS_MAX_CMDS_PER_MODULE;
    DS_MSG2(MSG_LEGACY_HIGH,
                    "New DS task command:%d module_id:%d",
                    (uint32)cmd_ptr->hdr.cmd_id, module_id);

    /*---------------------------------------------------------------------
      Call module specific command handler
    ---------------------------------------------------------------------*/
    q_count  = q_cnt(&ds_cmd_process_q);
    while(q_count--)
    {
      data_block = (ds_cmd_process_block*)q_get(&ds_cmd_process_q);
      if(data_block)
      {
        if((data_block->ds_module_id == module_id) &&
           (data_block->cb_func_ptr  != NULL))
        {
          processed_cmd = TRUE;
          data_block->cb_func_ptr(cmd_ptr);
        }
        q_put(&ds_cmd_process_q, &(data_block->link));
      }
    } /* end while */

    if(processed_cmd == FALSE)
    {
      DS_MSG1(MSG_LEGACY_FATAL, "cmd_id:%d not processed",
                      cmd_ptr->hdr.cmd_id);
      DS_ASSERT(0);
    }

    DS_MSG3(MSG_LEGACY_LOW,
                    "Processed cmd %d from DS Cmd buffer - "
                    "cmd_q cnt %d free_q cnt %d",
                    cmd_ptr->hdr.cmd_id,
                    q_cnt(&ds_cmd_q) ,
                    q_cnt(&ds_cmd_free_q) );

    ds_release_cmd_buf(&cmd_ptr);

  } /* while */
  return;
} /* dsi_process_cmds() */


/*===========================================================================

FUNCTION DS_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the DS task and This function notifies the scheduler
  that there is processing to be performed in DS task. 
 
PARAMETERS 
  sig   : Signal to be set.
 
DEPENDENCIES
  None
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_set_signal
(
  ds_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_set_sigs( ds_task_ptr,            
                      (rex_sigs_type) 1 << (rex_sigs_type)( sig ) );
#endif
} /* ds_set_signal() */

/*===========================================================================
FUNCTION      DS_PUT_CMD_EX

DESCRIPTION   This function puts a command on the Data Services Task's
              command queue, and sets the DS_CMD_Q_SIG. The command queue to
              use is passed as an argument.

DEPENDENCIES  The caller of this function should have previously allocated a
              command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds_put_cmd_ex
(
  ds_command_type    *cmd_ptr,                /* Pointer to command to be sent */
  q_type         *cmd_q_ptr                    /* Pointer to command queue */
)
{
#ifdef DS_TASK_DEBUG
  uint32 i;
#endif /* DS_TASK_DEBUG */
#ifdef FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG
  time_type up_time;
#endif /*FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG
  // Get time since phone power up and set timestamp field in command header
  memset( up_time, 0, sizeof(time_type) );
  time_get_uptime_ms(up_time);

  cmd_ptr->hdr.timestamp[0] = up_time[0];
  cmd_ptr->hdr.timestamp[1] = up_time[1];
#endif /*FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG*/
  /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
  -------------------------------------------------------------------------*/
  q_put( cmd_q_ptr, &cmd_ptr->hdr.link );
  (void)rex_set_sigs( ds_task_ptr, GET_SIG(DS_CMD_Q_SIG) );

  DS_MSG3(MSG_LEGACY_HIGH, "Putting cmd %d into DS Cmd buffer - "
                  "cmd_q cnt %d free_q cnt %d", cmd_ptr->hdr.cmd_id,
                  q_cnt(&ds_cmd_q),
                  q_cnt(&ds_cmd_free_q));

} /* ds_put_cmd_ex() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION      DS_TASK

DESCRIPTION   This is the entry point for the Data Services Task. This
              function contains the main processing loop that waits for
              events (signals or commands) and dispatches each event to the
              appropriate entity for further processing.

DEPENDENCIES  None

RETURN VALUE  This function does not return.

SIDE EFFECTS  None
===========================================================================*/
void  ds_task
(
  dword ignored
)
{
  rex_sigs_type   ret_val=0;        /* return value for rex_clr_sigs       */
  rex_sigs_type   set_sigs;         /* Signals set upon return from wait   */
  ds_sig_enum_type sig        = DS_TASK_MIN_SIG;

 
#ifdef FEATURE_DATA_LTE_ESPRESSO_TEST
  boolean         b_first_time_use = TRUE;
#endif /*FEATURE_DATA_LTE_ESPRESSO_TEST */

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  RCEVT_SIGEX_SIGREX term_sig; /* Termination Signal to wait on*/
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_TTL
  TTLBOOTUP(STARTUP_DS_TASK_MS);   //to -Do feature
#endif /* FEATURE_TTL*/

  term_sig.signal = ds_task_ptr = rex_self();
  term_sig.mask = GET_SIG(DS_TASK_STOP_SIG); 

  /* Register for the STOP Signal */
  rcinit_register_term_group(RCINIT_GROUP_5, 
                             RCEVT_SIGEX_TYPE_SIGREX, 
                             &term_sig);
  rcinit_handshake_startup();// moved to end!


  /*-------------------------------------------------------------------------
   Dog heart beat registration.
  -------------------------------------------------------------------------*/
  ds_task_dog_rpt_id = dog_hb_register_rex((rex_sigs_type)GET_SIG(DS_DOG_HB_REPORT_SIG ));

  ds_fwk_init();

    /*-------------------------------------------------------------------------
      Initialize DS Profile
    -------------------------------------------------------------------------*/
    if( ds_profile_init() != DS_PROFILE_RESULT_SUCCESS )
    {
      DS_MSG0(MSG_LEGACY_HIGH,
                      "DS Profile library initialization failed");
    }

    /*-------------------------------------------------------------------------
    PDN_MGR Init functios ()
  -------------------------------------------------------------------------*/
  ds_pdn_hdlr_init();


   /*-------------------------------------------------------------------------
	 DS3GSIOLIB int function
   -------------------------------------------------------------------------*/
   ds3g_siolib_init_timers();

   /*-------------------------------------------------------------------------
    DS_SYS init functions ()
  -------------------------------------------------------------------------*/

    dss_init();
  /*-------------------------------------------------------------------------
    COMMON init functions ()
  -------------------------------------------------------------------------*/
      /*-------------------------------------------------------------------------
    ATCOP init functions ()
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_ATCOP
   dsat_init();
#endif /* FEATURE_DATA_ATCOP */
   ds_pdn_at_init_service_mode();

   ds3g_mmgsdi_init();
  /*-------------------------------------------------------------------------
    RMSM init functions ()
  -------------------------------------------------------------------------*/

   ds_rmsm_init();
   /* NV read and related init functions for pdn_mgr */
   ds_pdn_mgr_init();
    
   /* NV read and related init functions for protocol */
   ps_proto_nv_read();

#ifdef QWES_FEATURE_ENABLE 
  /*--------------------------------------------------------------------------- 
    Initialize QTEELS QMI Client
  ---------------------------------------------------------------------------*/
  qmi_qteels_client_init();

#endif

  /*-------------------------------------------------------------------------
    APPSRV init functions ()
  -------------------------------------------------------------------------*/
  ds_appsrv_init();

	   //Task Ready now...!
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  //rcinit_handshake_startup();
	   if(RCEVT_NULL != rcevt_signal_name("DS:READY"))
	   {
		 DS_MSG0(MSG_LEGACY_HIGH," DS task is ready");
	   }
//	   	DS_MSG0(MSG_LEGACY_HIGH,"RCint handshake");
#else
	   /*-------------------------------------------------------------------------
		 Wait for the task start signal from task controller.
		-------------------------------------------------------------------------*/
	   tmc_task_start();
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  
  while (sig < DS_MAX_SIGNALS)
  {
	requested_sigs |= GET_SIG(sig);
	sig++;
  }
  sig = DS_TASK_MIN_SIG;
#ifdef FEATURE_TTL
   TTLBOOTUP(DS_TASK_READY);
#endif
  /*-------------------------------------------------------------------------
    Main task loop, never exits.
  -------------------------------------------------------------------------*/
  for( ;; )
  {
    /*----------------------------------------------------------------------
      Wait for one of the specified signals to be set. Note that watchdog
      kicking is performed in the wait.
    -----------------------------------------------------------------------*/
    set_sigs = ds_wait( requested_sigs );

    DS_MSG1(MSG_LEGACY_HIGH,"Signal %x",set_sigs);
	
    /*----------------------------------------------------------------------
    We used to individually clear the wrong set of signals and some signals
    were getting lost.  Here, we clear ds_tcb with set_sigs.  set_sigs is
    not altered.
    ----------------------------------------------------------------------*/
    ret_val = rex_clr_sigs( ds_task_ptr, set_sigs );

    if( !(ret_val & set_sigs) )
    {
      DS_MSG0(MSG_LEGACY_LOW," The previously set signal was not set");
    }

  /*----------------------------------------------------------------------
    Sometimes we see signals that were not requested in wait being set.
    So we mask returned sigs with requested sigs
    ----------------------------------------------------------------------*/
    set_sigs &= requested_sigs;


    /*----------------------------------------------------------------------
      If any of the task signals were received, invoke the function to ACK
      task conroller.
    -----------------------------------------------------------------------*/
    if( (set_sigs & (GET_SIG(DS_TASK_STOP_SIG))) != 0 )
    {
      DS_MSG0(MSG_LEGACY_HIGH,"In stop signal");

#ifdef FEATURE_MODEM_RCINIT_PHASE2
      dog_hb_deregister(ds_task_dog_rpt_id);
      rcinit_unregister_term_group(RCINIT_GROUP_5,
	  	                           RCEVT_SIGEX_TYPE_SIGREX, &term_sig);
      rcinit_handshake_term();
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
#else
      tmc_task_stop();
#endif 
      set_sigs &= ~(GET_SIG(DS_TASK_STOP_SIG));
    }

    if( (set_sigs & (GET_SIG(DS_TASK_OFFLINE_SIG))) != 0 )
    {
      //No action!
      set_sigs &= ~(GET_SIG(DS_TASK_OFFLINE_SIG));
    }
    /*----------------------------------------------------------------------
      If the command queue signal was set, clear the signal and invoke the
      function that dispatches commands to the appropriate sub-task.
    -----------------------------------------------------------------------*/
    if((set_sigs & (GET_SIG(DS_CMD_Q_SIG))) != 0 )
    {
      DS_MSG0(MSG_LEGACY_HIGH, " before process cmd");
      ds_process_cmds();
      set_sigs &= ~(GET_SIG(DS_CMD_Q_SIG));
    }
    /*----------------------------------------------------------------------
      If any of the 3G SIOLIB signals were set, clear the signals and invoke
      a function to process the signals.
    -----------------------------------------------------------------------*/
    if((set_sigs & (GET_SIG(DS_DATA_PORT_1ST_SIO_RX_SIG)))  != 0 ||
       (set_sigs & (GET_SIG(DS_DTR_EVENT_SIG)))             != 0 ||
       (set_sigs & (GET_SIG(DS_SIO_RX_PKT_SIG)))            != 0 ||
       (set_sigs & (GET_SIG(DS_SIO_ESCAPE_SIG)))            != 0)
    {
      ds3g_siolib_process_signals( set_sigs );
    }

    /*----------------------------------------------------------------------
      If any of the ATCoP signals were set, clear the signals and invoke a
      function to process the signals.
    -----------------------------------------------------------------------*/
    #ifdef FEATURE_DATA_ATCOP
    if((set_sigs & (GET_SIG(DS_AT_MT_MSG_SIG))) != 0 ||
       (set_sigs & (GET_SIG(DS_AT_SMS_SIG)))    != 0 ||
       (set_sigs & (GET_SIG(DS_AT_CM_MSG_SIG))) != 0)
    {
      dsat_process_async_signal( set_sigs );
    }
    #endif /* FEATURE_DATA_ATCOP */
    while( sig < DS_MAX_SIGNALS )
    {
      if ((set_sigs & GET_SIG(sig))  && (ds_sig_handler[sig] != NULL)
	  	    && (ds_sig_handler[sig]->sig_handler != NULL))
      {
        ds_sig_handler[sig]->sig_handler (
		         sig, ds_sig_handler[sig]-> user_data_ptr);
		set_sigs &= ~(GET_SIG(sig));
      }
	  sig ++;
    }
	sig = DS_TASK_MIN_SIG;

  } /* forever */
} /* ds_task() */

/*===========================================================================
FUNCTION      ds_register_cmd_hdlr

DESCRIPTION   This function registers command processing function for each 
              module.

DEPENDENCIES  None

RETURN VALUE  TRUE: callback registration successful
              TRUE: callback registration Un-successful

SIDE EFFECTS  None
===========================================================================*/
boolean ds_register_cmd_hdlr //ds_register_cmd_hdlr
(
  ds_module_enum_type_i       module_id,
  ds_cmd_process_func_type  cb_func_ptr
)
{
  boolean                ret_val = FALSE;
  ds_cmd_process_block  *cmd_process_block_to_insert = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_LOW,
                  "ds_register_cmd_hdlr module_id:%d ", module_id);

  cmd_process_block_to_insert = (ds_cmd_process_block*)
                                  q_get(&ds_cmd_process_free_q);

  if(cmd_process_block_to_insert !=  NULL)
  {
    cmd_process_block_to_insert->ds_module_id = module_id;
    cmd_process_block_to_insert->cb_func_ptr = cb_func_ptr;
    q_put(&ds_cmd_process_q, &(cmd_process_block_to_insert->link));
    ret_val = TRUE;
  }
  else
  {
    DS_ERROR_LOG_1("No Free  buffer to register module_id:=d ", module_id);
    ret_val = FALSE;
  }

  return ret_val;
}/* ds_register_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS_ALLOCATE_COMMAND_BUF

DESCRIPTION   This function allocates a command buffer from the Data Services 
              Task. The caller of this function must check for a NULL return
              value, and either perform some error recovery or ERR_FATAL,
              as appropriate.

DEPENDENCIES  None

RETURN VALUE  Returns a pointer to a command buffer, if available, or
              NULL if no command buffers are available.

SIDE EFFECTS  None
===========================================================================*/
ds_command_type* ds_allocate_cmd_buf
(
  size_t payload_size  /* number of bytes to allocate for cmd payload */
)
{
  char         *cmd_buf_ptr = NULL;
  ds_command_type  *ds_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate memory for DS command type + command payload
  -------------------------------------------------------------------------*/
   DS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, (sizeof(ds_command_type) + payload_size),
                            char*);
  if(cmd_buf_ptr == NULL)
  {
    DS_MSG2(MSG_LEGACY_FATAL, 
                    "Can't get cmd buf for DS task. "
                    "Heap Memory Allocation failed for client: %d size: %d" ,
                    MODEM_MEM_CLIENT_DATA_CRIT, (sizeof(ds_command_type)));
  }
  else
  {
    ds_cmd_ptr = (ds_command_type*)cmd_buf_ptr;
    memset(ds_cmd_ptr , 0, (sizeof(ds_command_type)+payload_size));
    /*-----------------------------------------------------------------------
      Ensure command payload pointer points to the payload allocated
    -----------------------------------------------------------------------*/
    if(payload_size > 0)
    {
      ds_cmd_ptr->cmd_payload_ptr = (void*)(cmd_buf_ptr + sizeof(ds_command_type));
    }
  }

  /*-------------------------------------------------------------------------
    Note that the cmd_ptr may be NULL if there were no free command buffers
    available. The calling task must either handle the NULL return value
    or ERR_FATAL.
  -------------------------------------------------------------------------*/
  return( ds_cmd_ptr );
}/* ds_allocate_command_buf() */

/*===========================================================================
FUNCTION      DS_RELEASE_CMD_BUF

DESCRIPTION   This function returns a command buffer to the Data Services 
              Task's free queue.

DEPENDENCIES  The caller of this function should have previously allocated 
              a command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds_release_cmd_buf
(
  ds_command_type  **cmd_pptr           /* Pointer to command to be released */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 DS_SYSTEM_HEAP_MEM_FREE(*cmd_pptr);
 return;
} /* ds_release_cmd_buf() */

/*===========================================================================
FUNCTION      DS_GET_COMMAND_QUEUE_CNT

DESCRIPTION   This function returns the count of outstanding commands that 
              are in DS command queue.

DEPENDENCIES  None

RETURN VALUE  Value of type uint8 - Indicates number of outstanding commands 
              in DS command queue.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_command_queue_cnt( void )
{
  uint8 cmd_q_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_q_cnt = (uint8) q_cnt(&ds_cmd_q);

  return (cmd_q_cnt);
} /* ds_get_command_queue_cnt() */

/*===========================================================================
FUNCTION      DS_GET_CUR_FREE_COMMAND_CNT

DESCRIPTION   This function returns the current count of free command
              buffers that are available in the DS task.

DEPENDENCIES  None

RETURN VALUE  Returns a uint8 with the number of outstanding commands 
              in the dstask.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_cur_free_command_cnt( void )
{
  uint8 free_q_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  free_q_cnt = (uint8) q_cnt(&ds_cmd_free_q);

  return (free_q_cnt);
} /* ds_get_cur_cmd_cnt() */

/*===========================================================================
FUNCTION      DS_PUT_CMD

DESCRIPTION   This function puts a command on the Data Services Task's 
              command queue, and sets the DS_CMD_Q_SIG. The command is put
              on the regular external command queue regardless of the calling
              task context, to preserve legacy behavior.

DEPENDENCIES  The caller of this function should have previously allocated a
              command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_put_cmd
(
  ds_command_type    *cmd_ptr                 /* Pointer to command to be sent */
)
{

  ds_put_cmd_ex(cmd_ptr, &ds_cmd_q);

} /* ds_put_command() */


/*===========================================================================
FUNCTION      DS_IS_IN_DS_TASK

DESCRIPTION   Returns TRUE if executing in DS task.

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_is_in_ds_task( void )
{
  return ( ((rex_self() == ds_task_ptr) ? TRUE : FALSE));
} /* dsi_is_in_ds_task() */

/*===========================================================================
FUNCTION      DS_UT_SYNC_COMMAND_HDLR

DESCRIPTION   UT Sync command

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
void ds_ut_sync_command_hdlr( void )  //check in off target
{
  ds_command_type  *cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If there is another CMD pending processing, post DS SYNC CMD
    Else send SYNC CNF MSG
  -------------------------------------------------------------------------*/
  if (q_cnt(&ds_cmd_q) != 0)
  {
    cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    DS_ASSERT(cmd_ptr != NULL);

  //  cmd_ptr->hdr.cmd_id = DS_CMD_UT_SYNC;
    ds_put_cmd(cmd_ptr);
  }
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST //#ifdef TEST_FRAMEWORK
  else
  {
    dsi_ut_send_sync_cnf();
  }
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
} /* ds_ut_sync_cmd_hdlr() */


/*===========================================================================

FUNCTION REG_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void reg_set_sig_handler
(
  ds_sig_enum_type sig,              /* Signal assocaited with handler     */
  ds_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{

 if (sig > DS_TASK_MIN_SIG && sig < DS_MAX_SIGNALS)
 {
    DS_ASSERT( sig_handler != NULL );
 
	if(ds_sig_handler[sig] == NULL)
	{
	/*-------------------------------------------------------------------------
	 Allocate memory for sig handler
	-------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC( ds_sig_handler[sig], sizeof(ds_sig_handler_info_type),
                              ds_sig_handler_info_type*);
	 if(ds_sig_handler[sig] == NULL)
	 {
	   DS_ERROR_LOG_1("reg_set_sig_handler: Can't get memory for signal handler"
					   "Heap Memory Allocation failed for client: =d" ,
					   sig);
	   DS_ASSERT(0);
	  }  
	}

  ds_sig_handler[sig]->sig_handler = sig_handler;
  ds_sig_handler[sig]->user_data_ptr = user_data_ptr;
  }
  return;
} /* reg_set_sig_handler() */


/*===========================================================================

FUNCTION REG_DEREG_SIG_HANDLER()

DESCRIPTION
  This function resets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void reg_dereg_sig_handler
(
  ds_sig_enum_type sig
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_SYSTEM_HEAP_MEM_FREE(ds_sig_handler[sig]);

  ds_sig_handler[sig] = NULL;

} /* reg_dereg_sig_handler() */

/*===========================================================================
FUNCTION  DS_GET_DS_TASK_PTR

DESCRIPTION   Returns DS TCB pointer

RETURN VALUE  ds_task_ptr

DEPENDENCIES  Should be called after ds_tcb_ptr has been inited .

SIDE EFFECTS  None
===========================================================================*/
rex_tcb_type* ds_get_ds_task_ptr( void )
{
  return ds_task_ptr;
} /* ds_get_ds_task_ptr() */

/*===========================================================================

FUNCTION DS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/

nv_stat_enum_type 
ds_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef TEST_FRAMEWORK
  #error code not present
#else
    if(rex_self() != ds_task_ptr)
    {
      return NV_BADTG_S;
    }

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  ds_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
  ds_nv_cmd_buf.tcb_ptr    = ds_task_ptr;           /* Notify this task    */
  ds_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)DS_NV_CMD_SIGNAL; /* With this signal    */
  ds_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  ds_nv_cmd_buf.item       = item_code;             /* Item to get         */
  ds_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  (void) rex_clr_sigs( ds_task_ptr, GET_SIG(DS_NV_CMD_SIGNAL) );
  nv_cmd( &ds_nv_cmd_buf );                       /* Issue the request    */

  (void)ds_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)DS_NV_CMD_SIGNAL );/* Wait for completion*/

  if( ds_nv_cmd_buf.status != NV_DONE_S &&
      ds_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    DS_MSG2(MSG_LEGACY_ERROR, "NV Read Failed Item %d Code %d",
                     ds_nv_cmd_buf.item, ds_nv_cmd_buf.status );
  }
  return( ds_nv_cmd_buf.status );

#endif /* TEST_FRAMEWORK */
} /* dcc_get_nv_item() */


/*===========================================================================
FUNCTION DS_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

DEPENDENCIES
  This function can only be called from DCC task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/

nv_stat_enum_type 
ds_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef TEST_FRAMEWORK
  #error code not present
#else
    if(rex_self() != ds_task_ptr)        
    {
      DS_ERR_FATAL("ds_put_nv_item() not called in dcc task context");
      return NV_BADTG_S;
    }

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  ds_nv_cmd_buf.cmd        = NV_WRITE_F;           /* Write request       */
  ds_nv_cmd_buf.tcb_ptr    = ds_task_ptr;          /* Notify this task    */
  ds_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)DS_NV_CMD_SIGNAL; /* With this signal    */
  ds_nv_cmd_buf.done_q_ptr = NULL;            /* Do not enqueue when done */
  ds_nv_cmd_buf.item       = item_code;            /* Item to put         */
  ds_nv_cmd_buf.data_ptr   = data_ptr;             /* Data to write       */
  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  (void) rex_clr_sigs( ds_task_ptr, GET_SIG(DS_NV_CMD_SIGNAL) );
  nv_cmd( &ds_nv_cmd_buf );
  (void)ds_wait( (rex_sigs_type) 1 <<
                 (rex_sigs_type)DS_NV_CMD_SIGNAL ); /* Wait for completion*/
  if( ds_nv_cmd_buf.status != NV_DONE_S )
  {

    DS_MSG2(MSG_LEGACY_ERROR,"NV Write Failed Item %d Code %d",
                     ds_nv_cmd_buf.item, ds_nv_cmd_buf.status );
  }

  return( ds_nv_cmd_buf.status );
#endif /* TEST_FRAMEWORK */
} /* dcc_put_nv_item() */
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */