/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ M O D E M _ T A S K . C

DESCRIPTION
  This is the source file for the QMI Modem Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS 
  qmi_modem_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_modem_task() has been invoked.

Copyright (c) 2011-2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/interface/tasks/src/qmi_modem_task.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    mkumars     First verion of the qmi_modem task module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "task.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#include "qmi_modem_task.h"
#include "qmi_crit_sect.h"
#include "ds_qmux_ext.h"
#include "ds_qmi_svc_ext.h"
#include "amssassert.h"
#include "data_msg.h"

#include "rcinit.h"
#include "rcevt_rex.h"
#include "modem_mem.h"
#include "dstaski.h"
#include "ds_qmi_if.h"
#include "ds_system_heap.h"
#include "ttl_map.h"
#include "qmi_frameworki.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

rex_tcb_type  * rex_qmi_modem_tcb;

/*--------------------------------------------------------------------------
  Dog report id.
---------------------------------------------------------------------------*/

dog_report_type qmi_dog_rpt_id                   = 0;        /* Dog report ID       */

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type qmii_enabled_sig_mask;

q_type qmii_cmd_q;

qmi_crit_sect_type qmi_port_cfg_crit_section;
qmi_crit_sect_type global_qmi_crit_section;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
qmii_sig_handler_info_type *qmii_sig_handler[QMI_MAX_SIGNALS] = {NULL};
qmi_cmd_handler_type qmii_cmd_handler[QMI_MAX_DEFINED_CMD_TYPES] = {qmi_default_cmd_handler};

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/
nv_item_type         qmi_nv_item;
nv_stat_enum_type    qmi_nv_status;
static nv_cmd_type qmi_nv_cmd_buf;




/*===========================================================================
                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION QMI_DEFAULT_SIG_HANDLER()

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
boolean qmi_default_sig_handler
(
  qmi_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"qmi_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------

  Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* qmi_default_sig_handler() */

/*===========================================================================
FUNCTION QMI_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the QMI modem task uses for rex_wait().
 
PARAMETERS 
  sig   : Signal to be enabled.
 
DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg.)
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_enable_sig
(
  qmi_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (qmii_sig_handler[sig] !=NULL) && (qmii_sig_handler[sig]->sig_handler == qmi_default_sig_handler))
  {
    DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"qmi_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }
  QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
  qmii_enabled_sig_mask |= ( (rex_sigs_type) 1 << (rex_sigs_type) sig );  
  QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);
  
  QMI_SET_SIGNAL( QMI_SIG_MASK_CHANGE_SIGNAL );
} /* qmi_enable_sig() */

/*===========================================================================
FUNCTION QMI_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.
 
PARAMETERS 
  sig   : Signal to be disabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_disable_sig
(
  qmi_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
  qmii_enabled_sig_mask &= ~( (rex_sigs_type) 1 << (rex_sigs_type) sig );
  QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);
} /* qmi_disable_sig() */

/*===========================================================================

FUNCTION QMI_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the QMI_MODEM task and This function notifies the scheduler
  that there is processing to be performed in QMI_MODEM task. 
 
PARAMETERS 
  sig   : Signal to be set.
 
DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg.)
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_set_signal
(
  qmi_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( qmii_sig_handler[sig]->sig_handler == qmi_default_sig_handler)
  {
    DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"qmi_set_signal(): "
                    "Signal %d set without a handler", sig );
  }

  QMI_SET_SIGNAL( sig );
} /* qmi_set_signal() */

/*===========================================================================
FUNCTION QMI_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_MODEM task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_clear_signal
(
  qmi_sig_enum_type sig                       /* Signal to be disabled     */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CLR_SIGNAL( sig );
} /* qmi_disable_sig() */

/*===========================================================================
FUNCTION QMI_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.
 
  If the handler is NULL, processing of the signal is defaulted. If the 
  signal handler is already set, then it is updated to use the new signal
  handler.
 
PARAMETERS 
  sig           : Signal for which the handler is to be set.
  sig_handler   : Callback function for handling signal enable.
  user_data_ptr : User data that would be used while the signal
                  handler is called.user data for signal handling 
                  can be set only once while setting the signal handler.
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler, if any was set. NULL otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_sig_handler_type qmi_set_sig_handler
(
  qmi_sig_enum_type sig,             /* Signal assocaited with handler     */
  qmi_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  qmi_sig_handler_type old_handler =NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sig > 0 && sig < QMI_MAX_SIGNALS)
  {

   if(qmii_sig_handler[sig] == NULL)
   {
      /*-------------------------------------------------------------------------
       Allocate memory for PS_RM command type
      -------------------------------------------------------------------------*/
     DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(qmii_sig_handler[sig], sizeof(qmii_sig_handler_info_type),
                                     qmii_sig_handler_info_type*);
   }
   
   old_handler = qmii_sig_handler[sig]->sig_handler; 
   qmii_sig_handler[sig]->sig_handler = ( sig_handler != NULL) ?
                                        sig_handler :
                                        qmi_default_sig_handler;

   qmii_sig_handler[sig]->user_data_ptr = user_data_ptr;
  }
  return old_handler;
} /* qmi_set_sig_handler() */

/*===========================================================================
FUNCTION QMI_GET_CMD_DATA_BUF()

DESCRIPTION
  Allocates a new command buffer to be used by caller to post commands to
  QMI_MODEM task.
 
PARAMETERS 
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  Ptr to new command data buffer.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_cmd_data_buf_type * qmi_get_cmd_data_buf
(
  void
)
{
  qmi_cmd_data_buf_type *new_cmd_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 DS_SYSTEM_HEAP_MEM_ALLOC(new_cmd_ptr ,sizeof(qmi_cmd_data_buf_type), 
                                qmi_cmd_data_buf_type* );

  if(new_cmd_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"qmi_get_cmd_data_buf(): "
                    "QMI cmd data mem alloc error!!");
  }

  return new_cmd_ptr;

} /* qmi_get_cmd_data_buf */

/*===========================================================================
FUNCTION QMI_GET_TASK_CMD_DATA_BUF()

DESCRIPTION
  Allocates a new command buffer to be used by caller to post commands to
  QMI_MODEM task.
 
PARAMETERS 
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  Ptr to new command data buffer.

SIDE EFFECTS
  None.
===========================================================================*/
qmii_cmd_type * qmi_get_task_cmd_data_buf
(
  void
)
{
  qmii_cmd_type *new_cmd_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 DS_SYSTEM_HEAP_MEM_ALLOC(new_cmd_ptr ,sizeof(qmii_cmd_type), 
                          qmii_cmd_type* );

  if(new_cmd_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"qmi_get_cmd_data_buf(): "
                    "QMI cmd data mem alloc error!!");
  }
  else
  {
    memset(new_cmd_ptr , 0, sizeof(qmii_cmd_type));
    /*-----------------------------------------------------------------------
     Ensure command payload pointer points to the payload allocated
     -----------------------------------------------------------------------*/
  }
  return new_cmd_ptr;

} /* qmi_get_cmd_data_buf */

/*===========================================================================
FUNCTION QMI_FREE_CMD_DATA_BUF()

DESCRIPTION
  Frees QMI Modem task command buffer.
 
PARAMETERS 
  None.
 
DEPENDENCIES
  Command buffer should have been allocated by a prior call to
  qmi_get_cmd_data_buf().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_free_cmd_data_buf
(
  void *cmd_ptr                  /* Command buffer to be freed             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
} /* qmi_free_cmd_data_buf */

/*===========================================================================
FUNCTION QMI_FREE_TASK_CMD_DATA_BUF()

DESCRIPTION
  Frees QMI Modem task command buffer.
 
PARAMETERS 
  None.
 
DEPENDENCIES
  Command buffer should have been allocated by a prior call to
  qmi_get_task_cmd_data_buf().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_free_task_cmd_data_buf
(
  qmii_cmd_type *cmd_ptr                  /* Command buffer to be freed             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_Q_NO_SELF_QPTR
	  q_delete(&qmii_cmd_q, &(cmd_ptr->link));
  #else
	  q_delete(&(cmd_ptr->link));
  #endif

  DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
} /* qmi_free_cmd_data_buf */

/*===========================================================================
FUNCTION QMI_SEND_CMD()
  Send command to be processed in QMI_MODEM task.
 
DESCRIPTION
  This method is used to send a command to the QMI_MODEM task for processing. 
  The command handler registered with the command being sent is executed
  when the QMI_MODEM task is run the next time. The QMI_MODEM task is serialized 
  through a command Q, so any pending commands in the QMI_MODEM task would be
  executed before the posted command. 

PARAMETERS
  cmd           - Command to send to QMI MODEM task.
  user_data_ptr - User data sent with the command. The 
                  command handler would be called with this user data.
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void qmi_send_cmd
(
  qmi_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  qmii_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the heap memorye, call error routine if no links
    available, else copy buffer to cmd queue and signal QMI Modem task
  -------------------------------------------------------------------------*/
  cmd_ptr = qmi_get_task_cmd_data_buf();
  if(cmd_ptr == NULL)
  {
     DS_MSG0(MSG_LEGACY_ERROR,"ps_rm_send_cmd(): "
                    "PS_RM task cmd buffers unavailable" );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;
    /*----------------------------------------------------------------------- 
     Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
    q_put( &qmii_cmd_q, &cmd_ptr->link );
    QMI_SET_SIGNAL( QMI_CMD_Q_SIGNAL );	
    QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);
  }
} /* qmi_send_cmd() */

/*===========================================================================
FUNCTION QMI_SET_CMD_HANDLER() 
 
DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL cmd
  handler is passed, then the command handling is defaulted (see 
  qmi_default_cmd_handler). The command handler can be updated on the 
  fly by calling this funciton again.

PARAMETERS 
  cmd         - Command for which we want to set handler.
  cmd_handler - Callback function to be registered as handler.
 
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.
===========================================================================*/
qmi_cmd_handler_type qmi_set_cmd_handler
(
  qmi_cmd_enum_type cmd,              /* Command name                      */
  qmi_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  qmi_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = qmii_cmd_handler[cmd];

  qmii_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                          cmd_handler :
                          qmi_default_cmd_handler;

  return old_handler;
} /* qmi_set_cmd_handler() */

/*===========================================================================
FUNCTION QMI_MODEM_TASK_WAIT()

DESCRIPTION
  This function performs the QMI Modem Task waiting function. This function 
  will not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.
 
PARAMETERS 
  requested_mask : QMI SIG mask.

DEPENDENCIES
  None.

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared.
===========================================================================*/
rex_sigs_type qmi_modem_task_wait
(
  rex_sigs_type requested_mask             /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs;                         /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  set_sigs = rex_get_sigs( rex_qmi_modem_tcb );
#else
  set_sigs = rex_get_sigs( &qmi_modem_tcb );
#endif

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask |
                         (  1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) );

    /*-----------------------------------------------------------------------
      If dog heart beat is received, ack the beat..
    -----------------------------------------------------------------------*/
    if( set_sigs & ( 1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      (void)rex_clr_sigs( rex_qmi_modem_tcb,
                    (  1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) );
#else
      (void)rex_clr_sigs( &qmi_modem_tcb,
                    (  1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) );
#endif
      dog_hb_report( qmi_dog_rpt_id );
    }
  }

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_clr_sigs( rex_qmi_modem_tcb, ( set_sigs & requested_mask ) );
#else
  (void)rex_clr_sigs( &qmi_modem_tcb, ( set_sigs & requested_mask ) );
#endif
  return ( set_sigs & requested_mask );
} /* qmi_modem_task_wait() */

/*===========================================================================
FUNCTION QMI_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.
 
PARAMETERS 
  item_code : Item to get.
  data_ptr  : Pointer where to put the item.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
 
DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

SIDE EFFECTS
  While this function is running all other QMI modem task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.
===========================================================================*/
nv_stat_enum_type qmi_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_qmi_modem_tcb );
#else
  ASSERT( rex_self() == &qmi_modem_tcb );
#endif

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  qmi_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  qmi_nv_cmd_buf.tcb_ptr    = rex_qmi_modem_tcb;     /* Notify this task    */
#else
  qmi_nv_cmd_buf.tcb_ptr    = &qmi_modem_tcb;              /* Notify this task    */
#endif
  qmi_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)QMI_NV_CMD_SIGNAL; /* With this signal    */
  qmi_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  qmi_nv_cmd_buf.item       = item_code;             /* Item to get         */
  qmi_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  QMI_CLR_SIGNAL( QMI_NV_CMD_SIGNAL );             /* Clear signal for NV  */
  nv_cmd( &qmi_nv_cmd_buf );                       /* Issue the request    */

  (void)qmi_modem_task_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)QMI_NV_CMD_SIGNAL );/* Wait for completion*/

  if( qmi_nv_cmd_buf.status != NV_DONE_S &&
      qmi_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    DS_MSG_SPRINTF_3(MSG_LEGACY_ERROR,"NV Read Failed Item %d Code %d %d",
         qmi_nv_cmd_buf.item, qmi_nv_cmd_buf.status,0);
  }
  return( qmi_nv_cmd_buf.status );
} /* qmi_get_nv_item() */

/*===========================================================================
FUNCTION QMI_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.
 
PARAMETERS 
  item_code : Item to set.
  data_ptr  : Pointer to data for item.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
 
DEPENDENCIES
  This function can only be called from QMI Modem task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

SIDE EFFECTS
  While this function is running all other QMI Modem task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.
===========================================================================*/
nv_stat_enum_type qmi_put_nv_item
(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_qmi_modem_tcb );
#else
  ASSERT( rex_self() == &qmi_modem_tcb );
#endif

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  qmi_nv_cmd_buf.cmd        = NV_WRITE_F;           /* Write request       */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  qmi_nv_cmd_buf.tcb_ptr    = rex_qmi_modem_tcb;    /* Notify this task    */
#else
  qmi_nv_cmd_buf.tcb_ptr    = &qmi_modem_tcb;             /* Notify this task    */
#endif
  qmi_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)QMI_NV_CMD_SIGNAL; /* With this signal    */
  qmi_nv_cmd_buf.done_q_ptr = NULL;            /* Do not enqueue when done */
  qmi_nv_cmd_buf.item       = item_code;            /* Item to put         */
  qmi_nv_cmd_buf.data_ptr   = data_ptr;             /* Data to write       */


  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  QMI_CLR_SIGNAL( QMI_NV_CMD_SIGNAL );
  nv_cmd( &qmi_nv_cmd_buf );

  (void)qmi_modem_task_wait( (rex_sigs_type) 1 <<
                 (rex_sigs_type)QMI_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( qmi_nv_cmd_buf.status != NV_DONE_S )
  {
    DS_MSG_SPRINTF_2(MSG_LEGACY_ERROR,"NV Write Failed Item %d Code %d",
         qmi_nv_cmd_buf.item, qmi_nv_cmd_buf.status );
  }

  return( qmi_nv_cmd_buf.status );
} /* qmi_put_nv_item() */


/*===========================================================================

FUNCTION QMI_DEFAULT_CMD_HANDLER()

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
void qmi_default_cmd_handler
(
  qmi_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"qmi_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
} /* qmi_default_cmd_handler() */

/*===========================================================================
FUNCTION QMII_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI task. 
  
  This function performs initialization before QMI task is started.
  Note that initializations internal to QMI should be performed before
  initializing other components that run in QMI task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmii_powerup
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_ERROR,"qmii_powerup(): "
                  "QMI Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/
  QMI_INIT_CRIT_SECTION(&global_qmi_crit_section);
  QMI_INIT_CRIT_SECTION(&qmi_port_cfg_crit_section);

  if (qmux_powerup_init() == FALSE)
  {
    DS_ERROR_FATAL("qmii_powerup(): QMUX Powerup Initialization failed");
    return;
  }

  if (qmi_framework_powerup_init() == FALSE)
  {
    DS_ERROR_FATAL("qmii_powerup(): QMI Framework Powerup Initialization failed");
    return;

  }

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &qmii_cmd_q );

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to QMI Task
  -------------------------------------------------------------------------*/
  qmii_enabled_sig_mask = ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_MODEM_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_MODEM_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_CMD_Q_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_DOG_HB_REPORT_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_SIG_MASK_CHANGE_SIGNAL );

} /* qmii_powerup() */

/*===========================================================================

FUNCTION QMII_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_CMD_Q_SIGNAL.  It retrieves the
  next command from the QMI command queue and processes the command by calling 
  the registered cmd handler.

DEPENDENCIES
    An item must be on the QMI command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qmii_process_cmd
(
  void
)
{
  static byte    qmi_cmds_proc_hi_score = 0;
  qmii_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    ERR_FATAL if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmii_cmd_q ) == 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"qmii_process_cmd(): "
                    "No QMI command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( qmii_cmd_type * ) q_get( &qmii_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= QMI_MAX_DEFINED_CMD_TYPES )
    {
      DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"qmii_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
	  
      qmi_free_task_cmd_data_buf(cmd_ptr);
      continue;
    }
    processed++;
    qmii_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );

#if 1 /* TODO: remove when all MHs take care of their cmd bufs */
    if (NULL != cmd_ptr->user_data_ptr)
    {
      /* Since memory for user data is allocated in QMI task itself */
      qmi_free_cmd_data_buf(cmd_ptr->user_data_ptr);	  
    }
#endif /* TODO: ... */

   qmi_free_task_cmd_data_buf(cmd_ptr);

  }

  /*-------------------------------------------------------------------------
	 Intlock to assure atomic operation below.
  
	 Note this also ensures no other task will be scheduled since
	 q_cnt() and rex_clr_sigs() cannot cause task context switch.
   -------------------------------------------------------------------------*/
   QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);

  /*-------------------------------------------------------------------------
   Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer QMI processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmii_cmd_q ) == 0 )
  {
    QMI_CLR_SIGNAL(QMI_CMD_Q_SIGNAL);
  }
  /*-------------------------------------------------------------------------
	 End atomic operation - Restore interrupts.
   -------------------------------------------------------------------------*/
   QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > qmi_cmds_proc_hi_score )
  {
    qmi_cmds_proc_hi_score = processed;
    DS_MSG1(MSG_LEGACY_ERROR,"qmii_process_cmd(): "
                     "New high # QMI commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear QMI_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the QMI_CMD_SIG is set, leading to ERR_FATAL above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* qmii_process_cmd() */

/*===========================================================================
FUNCTION QMII_INIT()

DESCRIPTION
  Initialization function for the data common ctrl task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  QMI task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmii_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize WWAN interface
    - done after task start so USB enumeration is completed hence
      sio_control_open always works, and so init function can read NV.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Initialize QMUX and QMI services
  -------------------------------------------------------------------------*/  
  ds_register_cmd_hdlr(DS_MODULE_QMI , ds_qmi_process_commands);

  if(qmux_init() != TRUE)
  {
    DS_ERROR_FATAL("qmii_init(): QMUX initialization failed");
  }
} /* qmii_init() */

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================
FUNCTION QMI_MODEM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void qmi_modem_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  rex_sigs_type     set_sigs = 0;   /* Currently set signals               */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */
  uint8             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

#ifdef FEATURE_TTL
   TTLBOOTUP(STARTUP_QMI_MODEM_TASK);	 //to -Do feature
#endif /* FEATURE_TTL*/

  rex_qmi_modem_tcb = rex_self();
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  rcinit_handshake_startup();

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Dog heart beat registration.
  -------------------------------------------------------------------------*/
  qmi_dog_rpt_id = dog_hb_register_rex(
                 (rex_sigs_type)1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL);

  qmii_init();

#ifdef FEATURE_TTL
   TTLBOOTUP(STARTUP_QMI_MODEM_TASK_DONE);	 //to -Do feature
#endif /* FEATURE_TTL*/

  /* Notify that the QMI_MODEM task is ready */
  (void)rcevt_signal_name("QMI_MODEM: READY");

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

    set_sigs |= rex_get_sigs( rex_qmi_modem_tcb );

    set_sigs &= qmii_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( qmii_enabled_sig_mask );
      set_sigs &= qmii_enabled_sig_mask;
    }

    (void)rex_clr_sigs( rex_qmi_modem_tcb, set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) QMI_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) QMI_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If watchdog hearbeat signal is received, just report to dog.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_DOG_HB_REPORT_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL );
        dog_hb_report( qmi_dog_rpt_id );
      }
      /*---------------------------------------------------------------------
       *
        QMI Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_CMD_Q_SIGNAL );

        if ( qmii_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_CMD_Q_SIGNAL );
        }
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/

      /*----------------------------------------------------------------------
        Handle qmux
      -----------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_QMUX_RX_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_QMUX_RX_SIGNAL );

        if ( qmii_sig_handler[QMI_QMUX_RX_SIGNAL]->sig_handler
             (
               QMI_QMUX_RX_SIGNAL ,
               qmii_sig_handler[QMI_QMUX_RX_SIGNAL]->user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_QMUX_RX_SIGNAL);
        }

      }
      else
      {
        /*----------------------------------------------------------------------
          Handle the QMI Service Signal if set
        -----------------------------------------------------------------------*/
        for( i = QMI_QMUX_SERVICE_CTL_SIGNAL; i < QMI_MAX_SIGNALS; i++)
        {
    
          if( loop_sigs & ( (rex_sigs_type) 1 <<
                                 (rex_sigs_type) i ) )
          {
            loop_sigs &= ~( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i );
    
            if ( qmii_sig_handler[i]->sig_handler
                 (
                   i ,
                   qmii_sig_handler[i]->user_data_ptr
                 ) == FALSE )
            {
              DS_MSG1(MSG_LEGACY_ERROR,"qmi_modem_task(): "
                              "QMI_QMUX_SERVICE_SIGNAL %d", i);
              set_sigs |= ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i);
            }
            /*-----------------------------------------------------------------
              allowing one signal type at a time to process which will avoid
              the watchdog timer expiration
            --------------------------------------------------------------------*/
            break;
          }
        }
      }
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MODEM_TASK_OFFLINE_SIGNAL ) )
      {
        DS_MSG0(MSG_LEGACY_ERROR,"qmi_modem_task(): "
                        "QMI_MODEM_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MODEM_TASK_OFFLINE_SIGNAL );
      }
    }

    DS_ASSERT(0 == loop_sigs);
  } /* forever */

} /* qmi_modem_task() */
