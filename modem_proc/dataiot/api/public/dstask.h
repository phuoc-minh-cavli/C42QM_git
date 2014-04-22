#ifndef DSTASK_H
#define DSTASK_H
/*===========================================================================
/*===========================================================================
                      DSTASK.H
DESCRIPTION
This file exposes dstask to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    PK      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_TASK

DESCRIPTION
  This is the entry point for the Data Services Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds_task
(
  dword ignored
    /* lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

#endif /* DSTASK_H */
