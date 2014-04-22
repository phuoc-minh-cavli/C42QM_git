/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              G L O B A L    C O M M A N D    H E A D E R  

GENERAL DESCRIPTION
  This module contains the procedure which processes the global command
  header after a command has been processed.
  
EXTERNALIZED FUNCTIONS
  cmd_done
    Processes global command header.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1990,1991,1992 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 1998,1999      by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/cmd.c_v   1.1   29 Jan 2001 10:53:54   dyang  $
$Header: //components/rel/core.tx/6.0/services/utils/src/cmd.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                     Added support for MSM5000_IRAM_FWD
12/16/98   jct     Removed 80186 support
09/21/98   jct     Incorporate ARM porting changes, updated copyright
07/14/92   jai     Create module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qurt.h"
#include "queue.h"
#include "cmd.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifndef MSM5000_IRAM_REV
/*===========================================================================

FUNCTION CMD_DONE

DESCRIPTION
  This procedure finishes processing a report or command.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cmd_done
( 
  cmd_hdr_type *hdr_ptr
    /* pointer to header of received command */
)
{
  qurt_signal_t *sigs_ptr;
    /* task to signal */
  uint32  sigs_mask;
    /* signal to set */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* temporarily store task to signal and signal to set */
  sigs_ptr = hdr_ptr->sig_ptr;
  sigs_mask = hdr_ptr->sig_mask;

  /* place buffer on done queue if requested */
  if( hdr_ptr->done_q_ptr != NULL )
  {
    /* place command on requested queue */
    q_put( hdr_ptr->done_q_ptr, &hdr_ptr->link );
  }

  /* signal task if requested */
  if( sigs_ptr != NULL )
  {
    ( void )qurt_signal_set( sigs_ptr, sigs_mask );
  }
} /* cmd_done */
#endif /*MSM5000_IRAM_REV*/
