#ifndef PS_RM_TASK_H
#define PS_RM_TASK_H
/*===========================================================================

                        P S _ R M _ T A S K . H

DESCRIPTION

  Rm Data processing Task services header file. All definitions, structures,
  and functions needed for performing PS_RM task specific services.
  
EXTERNALIZED FUNCTIONS  
  
  ps_rm_enable_sig()
    Adds specific signal for PS_RM task to process.

  ps_rm_disable_sig()
    Removes specific signal from PS_RM task processing.

  ps_rm_set_sig_handler()
    Sets signal handler for specific signal processed in PS_RM task.

  ps_rm_send_cmd()
    Post a command to be handled in PS_RM task.

  ps_rm_set_cmd_handler()
    Sets command handler for command handled in PS_RM task.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/tasks/inc/ps_rm_taski.h#1 $ 
  $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "amssassert.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "rex.h"
#include "queue.h"

/*===========================================================================

                      EXTERN DECLARATIONS

===========================================================================*/

extern rex_tcb_type           ps_rm_tcb;



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type    * rex_ps_rm_tcb;
#endif

#define PS_RM_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
  
#define PS_RM_SET_SIGNAL( sig ) \
  (void)rex_set_sigs( &ps_rm_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#define PS_RM_CLR_SIGNAL( sig ) \
  (void)rex_clr_sigs( &ps_rm_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

/*--------------------------------------------------------------------------
  PS_RM Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  PS_RM_CMD_Q_SIGNAL                =  1,   /* PS_RM command Q signal          */
  PS_RM_DOG_HB_REPORT_SIGNAL        =  2,   /* Dog heart beat report signal       */
  PS_RM_SIG_MASK_CHANGE_SIGNAL      =  3,   /* Indicates sig mask has changed  */

  PS_RM_RMNET_RX_DATA_Q_SIGNAL_1    =  4,   /* RMNET --> PS_RM data sig        */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_2    =  5,   /* RMNET --> PS_RM data sig        */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_3    =  6,   /* RMNET --> PS_RM data sig        */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_4    =  7,   /* RMNET --> PS_RM data sig        */
  
  /*-------------------------------------------------------------------------
   BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
  -------------------------------------------------------------------------*/
  PS_RM_TASK_OFFLINE_SIGNAL         =  13,	 /* Reserved TASK_OFFLINE_SIG	  */
  PS_RM_TASK_STOP_SIGNAL            =  14,	 /* Reserved TASK_STOP_SIG		  */
  PS_RM_TASK_START_SIGNAL           =  15,	 /* Reserved TASK_START_SIG 	  */
  
  PS_RM_MAX_SIGNALS                          /* use next value for enum       */
} ps_rm_sig_enum_type;


/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_RM_MIN_CMD                      = -1, 
  /* Always must be last */
  PS_RM_MAX_DEFINED_CMD_TYPES        = 1  //J-F-C
} ps_rm_cmd_enum_type;

/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ps_rm_sig_handler_type)
(
  ps_rm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*ps_rm_cmd_handler_type)
(
  ps_rm_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/

typedef struct
{
  ps_rm_sig_handler_type sig_handler;
  void *user_data_ptr;
} ps_rm_sig_handler_info_type;

/*--------------------------------------------------------------------------
  PS_RM Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct     
{
  q_link_type       link;		     /* For linking onto cmd queue             */
  ps_rm_cmd_enum_type cmd;         /* Actual command to be processed         */
  void              *user_data_ptr; /* Command specific user parameters    */
} ps_rm_cmd_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION PS_RM_DEFAULT_SIG_HANDLER()

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean 
ps_rm_default_sig_handler
(
  ps_rm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================

FUNCTION PS_RM_DEFAULT_CMD_HANDLER()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ps_rm_default_cmd_handler
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION PS_RM_POWERUP()

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_powerup
(
  void
);

/*===========================================================================

FUNCTION PS_RM_PROCESS_CMD()

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
ps_rm_process_cmd
(  
  void
);

/*===========================================================================

FUNCTION PS_RM_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS_RMtask uses for rex_wait().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_enable_sig
(
  ps_rm_sig_enum_type sig                       /* Signal to be enabled      */
);

/*===========================================================================

FUNCTION PS_RM_DISABLE_SIG()

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

void 
ps_rm_disable_sig
(
  ps_rm_sig_enum_type sig                       /* Signal to be disabled     */
);

/*===========================================================================

FUNCTION PS_RM_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.

DEPENDENCIES
  None.

RETURN VALUE

 Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_sig_handler_type 
ps_rm_set_sig_handler
(
  ps_rm_sig_enum_type sig,             /* Signal assocaited with handler     */
  ps_rm_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);

/*===========================================================================

FUNCTION PS_RM_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS_RM task context.  The cmd is
  processed by calling the registered cmd handler, if any.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_send_cmd
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================

FUNCTION PS_RM_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_cmd_handler_type 
ps_rm_set_cmd_handler
(
  ps_rm_cmd_enum_type cmd,             /* Command name                      */
  ps_rm_cmd_handler_type cmd_handler   /* Cmd handler function pointer      */
);


/*===========================================================================

FUNCTION PS_RM_GET_CMD_BUF()

DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
  ps_rm_cmd_type * Buffer

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_cmd_type* ps_rm_get_cmd_buf
(

);

/*===========================================================================
	
FUNCTION PS_RM_GET_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
  void 

SIDE EFFECTS
  None.
	
===========================================================================*/

void ps_rm_free_cmd_buf
(
 ps_rm_cmd_type *cmd_ptr

);

/*===========================================================================

FUNCTION PS_RM_WAIT()

DESCRIPTION
  This function performs the PS_RM Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/

rex_sigs_type
ps_rm_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);

#ifdef __cplusplus
}
#endif


#endif /* PS_RM_TASK_H */
