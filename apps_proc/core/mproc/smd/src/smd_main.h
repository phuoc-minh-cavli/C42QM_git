#ifndef SMD_MAIN_H
#define SMD_MAIN_H

/*===========================================================================

                    Shared Memory Driver Main Routine Header File


 Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_main.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/14   bc      SMD port searching optimization
                   Modified functions related handling events
                   smd_main became smd_handle_event only handling events
                   Deleted smd_set_stream_closed function
12/12/13   rv      SMEM Partition backwards compatibility is removed.
07/24/13   bt      Make SMD use secure SMEM partitions.
02/11/13   bt      Remove smd_handle_ssr_close.
08/01/12   bt      Remove smd_sm_remote_reset declaration.
05/24/12   bm      Use smem_host_type to identify hosts
01/05/12   hwu     Decouple SMD from SMEM internal header.
07/25/11   rs      Added support for SSR Phase 2
01/06/11   bt      Removed SMD Block and Data Mover protocols.
09/27/10   tl/ih   Added SMD_CMD_PROCESS_PORT to process a specific port
06/08/10   tl      Split SMD task into multiple tasks with different priorities
04/21/10   tl      Implemented SMD Lite API
03/17/08   bfc     Implemented block port close.
02/21/08   bfc     First SMD Block port check in.
02/16/08   bfc     Added a new command for block ports.
10/08/07   hwu     Added is_loopback flag for loopback port.
09/12/07   bfc     smd_main now only processes one event, so the arguement
                   was removed, and the processing combined.
05/17/07   hwu     Added the SMD loopback feature.
12/14/06   hwu     Added new command type for remote processor reset.
                   Added new API smd_set_stream_closed() so startup can
                   forcefully close the SMD streams.
11/14/06   taw     Remove argument to smd_init().
11/03/06   ptm     Add events argument to smd_main.
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
09/05/06   ptm     Initial version - based on smd_task.h#3.
===========================================================================*/

#include "smd_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/*-------------------------------------------------------------------------
 *        Define command data structures
 *-------------------------------------------------------------------------*/
typedef enum
{
  SMD_CMD_OPEN_MEMCPY,
  SMD_CMD_CLOSE,
  SMD_CMD_FLUSH,
  SMD_CMD_IOCTL,
  SMD_CMD_REMOTE_RESET,
  SMD_CMD_REMOTE_RESET_DONE,
  SMD_CMD_LOOPBACK,
  SMD_CMD_OPEN_SMDLITE,
  SMD_CMD_PROCESS_PORT,
} smd_cmd_e_type;

typedef struct
{
  smd_port_id_type port_id;
  boolean          is_loopback;
} smd_cmd_open_memcpy_type;

typedef struct
{
  smd_port_id_type      port_id;
  smd_close_callback_fn close_cb;
  void                  *cb_data;
  boolean               flush;
} smd_cmd_close_type;

typedef struct
{
  smd_port_id_type      port_id;
  smd_flush_callback_fn flush_cb;
  void                  *cb_data;
} smd_cmd_flush_type;

typedef struct
{
  smd_port_id_type     port_id;
  smd_ioctl_cmd_type   cmd;
  smd_ioctl_param_type param;
} smd_cmd_ioctl_type;

typedef struct
{
  smd_port_id_type      port_id;
} smd_cmd_process_port_type;

typedef struct smd_cmd_struct
{
  smd_cmd_e_type cmd;
  union
  {
    smd_cmd_open_memcpy_type open_memcpy;
    smd_cmd_close_type       close;
    smd_cmd_flush_type       flush;
    smd_cmd_ioctl_type       ioctl;
    smd_cmd_process_port_type process_port;
  } data;
} smd_cmd_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_init

DESCRIPTION   Initialize the SMD. Calls init functions for various subsystems,
              and initializes the command queue.

RETURN VALUE  None
===========================================================================*/
void smd_init( void );

/*===========================================================================
FUNCTION      smd_cmd

DESCRIPTION   This routine copies the given cmd buffer into a command queue
              structure, puts the command on the SMD command queue and sets
              the command queue signal for the SMD task.

ARGUMENTS     cmd_ptr - pointer of the command buffer to copy

RETURN VALUE  None

SIDE EFFECTS  Queues command, signals SMD task
===========================================================================*/
void smd_cmd( smd_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION      smd_cmd_context

DESCRIPTION   Sends a command to the particular SMD task context.

ARGUMENTS     cmd_ptr - pointer of the command buffer to copy

              context - pointer to the SMD context

RETURN VALUE  None

SIDE EFFECTS  Queues command, signals SMD task
===========================================================================*/
void smd_cmd_context( smd_cmd_type *cmd_ptr, smd_context_type *context);

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
void smd_cmd_broadcast( smd_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION      smd_handle_event

DESCRIPTION   It handles event caused by remote processor

              When a share memory port is opened, it synchronizes the opening
              with the other processor and is then responsible for sending
              and receiving data and control signals.

ARGUMENTS     context - SMD task context object

RETURN VALUE  None
===========================================================================*/
void smd_handle_event ( smd_context_type * context );

/*===========================================================================
FUNCTION      smd_sm_enter_start

DESCRIPTION   State machine function for entering the START state. Clears
              both of the SMD SMSM state bits, registers the SMSM callback
              initializes the appropriate half of the shared data structures
              and sets the SMD WAIT state in SMSM.

RETURN VALUE  None

SIDE EFFECTS  Sets SMSM SMD WAIT state bit and interrupts the other processor.
===========================================================================*/
void smd_sm_enter_start( void );

/*===========================================================================
FUNCTION      smd_handle_cmd

DESCRIPTION   Dispatch commands queued for the smd task to their handlers

ARGUMENTS     context - SMD task context object

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_handle_cmd( smd_context_type * context );

#ifdef __cplusplus
}
#endif

#endif /* SMD_MAIN_H */
