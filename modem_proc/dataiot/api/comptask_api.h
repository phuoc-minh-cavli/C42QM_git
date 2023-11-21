#ifndef COMPTASK_API_H
#define COMPTASK_API_H

/*===========================================================================
                      COMPTASK_API.H
DESCRIPTION
This file exposes comp task APIs to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "dsm_queue.h"
#include "task.h"
#include "sys.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define COMP_MAX_CMD_BUF 30               /* max cfg req buffers available */

/*---------------------------------------------------------------------------
  Task Signal Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Task top-level signals.  Number of TX and RX signals are based on maximum
  number of allowed PDCP instances.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef enum
{

  COMP_CMD_Q_SIG                    = 0,
  COMP_DOG_HB_REPORT_SIG            = 1,
  COMP_SNDCP_CMD_Q_SIGNAL           = 2, 
  COMP_SNDCP_LL_DL_Q_SIGNAL         = 3,
  
  COMP0_TX_DATA_SIG                 = 4,
  COMP1_TX_DATA_SIG                 = 5,
  COMP2_TX_DATA_SIG                 = 6,
  COMP3_TX_DATA_SIG                 = 7,

/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  COMP_TASK_OFFLINE_SIG             = 13,
  COMP_TASK_STOP_SIG                = 14,
  COMP_TASK_START_SIG               = 15,
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   DO NOT USE 23 & 24 since these are used by other signals in task.h
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  COMP_MAX_SIGNALS                   
} comp_sig_enum_type;

typedef enum
{
  COMP_MIN_CMD = -1,
  GSNDCP_REG_WM = 0,             /* mode handler to register WM with SNDCP */
  GSNDCP_DEREG_WM = 1,           /* mode handler to deregister WM w /SNDCP */
  COMP_MAX_CMDS
} comp_cmd_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Header type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  q_link_type       link;                               /* Queue link type */
  comp_cmd_enum_type  cmd_id;                    /* Identifies the command */
} comp_cmd_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command type: The command header identifies the command. If the command has
  any parameters, they are specified in the union.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  uint8                   nsapi;
  dsm_watermark_type      *to_sndcp_wm_ptr;
  dsm_watermark_type      *from_sndcp_wm_ptr;
  void                    (*from_sndcp_post_rx_func_ptr)(byte,byte,void *);
  void                    *from_sndcp_rx_func_parm;
  sys_modem_as_id_e_type  subs_id;  
} gsndcp_reg_dereg_wm_data_type;

typedef struct
{
  comp_cmd_hdr_type    hdr;                                /* Command header */

  union
  {
    gsndcp_reg_dereg_wm_data_type     gsndcp_reg_wm;
    gsndcp_reg_dereg_wm_data_type     gsndcp_dereg_wm;
  } cmd;

} comp_cmd_type;

/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*comp_sig_handler_type)
(
  comp_sig_enum_type sig,                  /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

typedef struct
{
  comp_sig_handler_type sig_handler;
  void *user_data_ptr;
} comp_sig_handler_info_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#define COMP_SET_SIGNAL comp_set_signal
#define COMP_CLR_SIGNAL comp_clear_signal

/*===========================================================================
FUNCTION COMP_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the COMP task and notifies the scheduler that there
  is processing to be performed in comp task. 
 
PARAMETERS 
  sig   : Signal to be set.
 
DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg).
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void comp_set_signal
(
  comp_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION COMP_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the COMP task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void comp_clear_signal
(
  comp_sig_enum_type sig                       
);

/*===========================================================================

FUNCTION COMP_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the COMP Task. The
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

comp_cmd_type  *comp_get_cmd_buf
( 
  void 
);

/*===========================================================================

FUNCTION COMP_PUT_CMD

DESCRIPTION
  This function puts a command on the COMP Task's command queue, and
  sets the COMP_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling comp_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  comp_put_cmd
(
  comp_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
);

/*===========================================================================

FUNCTION COMP_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

extern comp_sig_handler_type comp_set_sig_handler
(
  comp_sig_enum_type sig,            /* Signal assocaited with handler     */
  comp_sig_handler_type sig_handler, /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);


/*===========================================================================

FUNCTION COMP_RESERVE_TO_COMP_TX_SIG()

DESCRIPTION
  This function reserves a tx signal for GSNDCP/PDCP and returns it. 

DEPENDENCIES
  None.

RETURN VALUE
  Signal reserved

SIDE EFFECTS
  None.

===========================================================================*/

comp_sig_enum_type comp_reserve_to_comp_tx_sig
(
  void
);

/*===========================================================================

FUNCTION COMP_UNRESERVE_TO_COMP_TX_SIG()

DESCRIPTION
  This function unreserves a tx signal for GSNDCP/PDCP. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void comp_unreserve_to_comp_tx_sig
(
  comp_sig_enum_type sig
);

#endif /* COMPTASK_API_H */
