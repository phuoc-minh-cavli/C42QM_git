#ifndef _POLICYMAN_TASK_H_
#define _POLICYMAN_TASK_H_

/**
  @file policyman_task.h

  @brief Policy Manager task code.
*/

/*
    Copyright (c) 2013 - 2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_task.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "comdef.h"

#include "policyman_i.h"
#include "policyman_policy.h"

#include <sys/types.h>
#include "queue.h"
#include "ref_cnt_obj.h"


/*-------- execute_fn_t --------*/
/**
@brief  Definition of function type that is used to execute a command.

@param[in]  pCmd  Pointer to a policyman_cmd;

@return
  None.
*/
typedef void  (*execute_fn_t)(policyman_cmd_t *pCmd);


/*-------- dtor_fn_t --------*/
/**
@brief  Definition of function type that used to destroy (clean up) a command.

This cleans up any command-specific resources.  Note that any resources held
in POLICYMAN_CMD_HDR below are dealt with by the framework and MUST NOT be
cleaned up in this function.

@param[in]  pCmd  Pointer to the policyman_cmd_t to be destroyed.

@return
  None.
*/
typedef void  (*dtor_fn_t)(policyman_cmd_t *cmd);


/*-------- policyman_signal_t --------*/
/**
@brief  Abstract type for signals used by policyman.

This is used differently depending on whether policyman is built for target
or for the unit test environment.

In unit test, this is a Windows HANDLE.
On target, it can be either
- A REX signal
    REX signals are used in the main task loop, because those are the only
    signals that MSGR works with, and we need MSGR functionality in the main
    loop.  These are used in the policyman_signalset_t.
- A QuRT signal
    QuRT signals are used to block threads calling into policyman APIs when
    necessary, because true kernel objects work better for this.
*/
typedef uint32  policyman_signal_t;

#define POLICYMAN_NULL_SIGNAL ((policyman_signal_t) 0)


/*-------- policyman_tcb_type --------*/
/**
@brief  Define for both platforms, so we can compile across platforms.

@param

@return
  
*/

typedef  rex_tcb_type policyman_tcb_t;


/*-------- POLICYMAN_CMD_HDR --------*/
/**
@brief  This must be the first element in any command structure - it defines the
        base elements necessary for all commands.

@param  _REF_CNT_OBJ  Fields required for reference-counting the commands.
@param  link          Used to link the commands into the command queue.
@param  execute       Pointer to the function to call in order to execute the
                      command.  May not be NULL.
@param  dtor          Pointer to a dtor_fn_t function to call to clean up any
                      resources used by the specific command.  May be NULL if
                      there are no command-specific resources; all resources
                      used in POLICYMAN_CMD_HDR will be cleaned up by the
                      framework.
@param  signal        Signal to set when the command has been executed.
@param  pPolicy       Policy issuing this command.
@param  pMsg          Pointer to a message to be returned to a blocked caller,
                      if command was issued as a result of a blocking call.
*/
#define POLICYMAN_CMD_HDR           \
  _REF_CNT_OBJ;                     \
  q_link_type         link;         \
  execute_fn_t        execute;      \
  dtor_fn_t           dtor;         \
  policyman_signal_t  signal;       \
  policyman_policy_t  *pPolicy;     \
  msgr_hdr_s          *pMsg


/*-------- policyman_cmd_s --------*/
/**
@brief The basic PM command type.

All commands simply extend this; when appropriate, a simple cast between this
and the relevant command type is valid.
*/
struct policyman_cmd_t
{
  POLICYMAN_CMD_HDR;
};



#ifdef __cplusplus
extern "C" {
#endif

/*-------- policyman_cmd_new --------*/
/**
@brief  Allocates a command structure of the requested size and initializes the
        structure of the base command.

PM commands are reference-counted objects.  DO NOT free the command when
you are done with it; rather, call ref_cnt_obj_release() on it to release
your reference.  The command will be cleaned up and freed when its reference
count goes to zero.

@param[in]  size      The size of the command structure - just pass sizeof()
                      your command structure.
@param[in]  execute   Pointer to an execute_fn_t function to call in order to
                      execute the command.  Cannot be NULL.
@param[in]  dtor      Pointer to a dtor_fn_t function to call to clean up any
                      resources allocated by the specific command (resources
                      used by the framework will be cleaned up by the
                      framework).  This will generally be NULL.

@return Pointer to a newly allocated and initialized command.  Cast the return
        value to your command structure.
*/
policyman_cmd_t *
policyman_cmd_new(
  size_t              size,
  execute_fn_t        execute,
  dtor_fn_t           dtor,
  policyman_policy_t  *pPolicy
  );


/*-------- policyman_queue_put_cmd --------*/
/**
@brief  Add a command to the PM command queue.

@param[in]  pCmd  Pointer to the command allocated by a call to policyman_policyman_cmd_new.
            Cast your command structure to policyman_cmd *.

@return
  None.
*/
void
policyman_queue_put_cmd(
  policyman_cmd_t  *pCmd
  );


/*-------- policyman_init_before_start --------*/
/**
@brief  Initialize the PM command queue and signals.  Private function exported
        only for unit tests.

@return
  None.
*/
void
policyman_init_before_start(
  void
  );



/*-------- policyman_client_block_sig_get --------*/
/**
@brief  Return an available signal from the signal pool.  It is a fatal error
        to have no available signals.
*/
policyman_signal_t
policyman_client_block_sig_get(
  void
  );


/*-------- policyman_client_block_sig_return --------*/
/**
@brief  Return a signal to the signal pool, making it available to others again. 
*/
void
policyman_client_block_sig_return(
  policyman_signal_t signal
  );


/*-------- policyman_client_block_signal_wait --------*/
/**
@brief  Called in policyman_get_items_block() to block client threads until
        policyman initialization has completed.
*/
void
policyman_client_block_signal_wait(
  policyman_signal_t  signal
  );



/*-------- policyman_client_block_signal_set --------*/
/**
@brief  Set the signal on which clients wait until policyman initialization
        has completed.
*/
void
policyman_client_block_signal_set(
  policyman_signal_t  signal
  );


/*-------- policyman_blocked_clients_init --------*/
/**
@brief Create a set for blocking clients during initialization.
*/
void
policyman_blocked_clients_init(
  void
);


/*-------- policyman_blocked_clients_release --------*/
/**
@brief  Release the set used for blocked clients.
*/
void policyman_blocked_clients_release(
  void
);


/*-------- policyman_blocked_clients_add --------*/
/**
@brief  Block client during initialization.  Private function to be used by PM task code.

@return
  None
*/
void policyman_blocked_clients_add(
  void
);



#ifdef __cplusplus
}
#endif

#endif /* _POLICYMAN_TASK_H_ */
