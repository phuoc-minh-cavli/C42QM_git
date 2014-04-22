/*===========================================================================

            S M E M _ S T U B S . C 

DESCRIPTION

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/src/smem_log_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/09    ih     Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_target.h"
#include "smem_log.h"

/*===========================================================================
                    LOCAL DATA DECLARATIONS 
===========================================================================*/

/*===========================================================================
FUNCTION      smem_log_event

DESCRIPTION   Write an entry in the shared memory event log.

              The log idx is treated with extreme caution to prevent writing
              outside of the event array.

ARGUMENTS     area - the logging area to write to
              id - id tag for the event
              reserved - must be 0
              data1, data2, data3 - event related data items

              Uses spin lock for exclusive access to event log

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_event ( smem_log_area area, uint32 id, uint32 reserved,
    uint32 data1, uint32 data2, uint32 data3 )
{
} /* smem_log_event */

/*===========================================================================
FUNCTION      smem_log_event6

DESCRIPTION   Write two entries in the shared memory event log - allows caller
              to write more data items than smem_log_event.

              This function is not supported on static logs; attempts to use
              this function to log to static logs will instead be redirected
              to the mproc log.

              The log idx is treated with extreme caution to prevent writing
              outside of the event array.

ARGUMENTS     area - the logging area to write to
              id - id tag for the event
              reserved - must be 0
              data1, data2, data3, data4, data5, data6 - event related data
                                                         items

              Uses spin lock for exclusive access to event log

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_event6
(
  smem_log_area area,
  uint32 id,
  uint32 reserved,
  uint32 data1,
  uint32 data2,
  uint32 data3,
  uint32 data4,
  uint32 data5,
  uint32 data6
)
{
} /* smem_log_event */

/*===========================================================================
FUNCTION      smem_log_event_static

DESCRIPTION   A stubb for this depreciated function call.  This reduces 
              compilation dependencies between SMEM LOG and its users.
      
ARGUMENTS     id - Not used
              data1, data2, data3 - Not used

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_event_static ( uint32 id, uint32 data1, uint32 data2, uint32 data3 )
{
}
