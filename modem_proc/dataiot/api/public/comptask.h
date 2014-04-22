#ifndef COMPTASK_H
#define COMPTASK_H

/*===========================================================================
                      COMPTASK.H
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

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION COMP_TASK

DESCRIPTION
  This is the entry point for the Compression Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void  comp_task
(
  dword ignored
    /* lint -esym(715,ignored) */
);

#endif /* COMPTASK_H */
