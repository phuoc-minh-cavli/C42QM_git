/*===========================================================================

                    Shared Memory Diag Source File

    Define the APIs exported for DIAG.

 Copyright (c) 2006-2007, 2009, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/src/smem_diag.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/26/11   rs      Moved definition of OS header file to after Target header 
                   to allow redefition of macros during initial OS bringup
04/30/10   rs      Clear Lint warnings
06/10/09   bfc     CMI updates.
06/01/09   hwu     changed to use smem_alloc.
03/11/09   tl      Windows Mobile 6 warning cleanup
07/21/07   ptm     Change code to use memcpy instead of strncpy
03/14/07   bfc     Removed the dependency to hs.h
11/28/06   ptm     Initial version - derived from smem.c#13.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include <string.h>

#include "smem_target.h"
#include "smem_os.h"
#include "smem.h"
#include "smem_v.h"
#include "smem_diag.h"

/*===========================================================================
                              GLOBAL DATA DECLARATIONS 
===========================================================================*/

static char *smem_diag_buffer;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smem_diag_get_message
                                                                             
DESCRIPTION   Copy a diag message from shared memory.

ARGUMENTS     char * message - pointer to destination string
              uint32 len - length of buffer pointed to by message
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
void smem_diag_get_message( char *message, uint32 len )
{
  if( message == NULL )
  {
    return;
  }

  if( smem_diag_buffer == NULL )
  {
    smem_diag_buffer = (char *) smem_alloc( SMEM_DIAG_ERR_MESSAGE,
                                            SMEM_DIAG_BUFFER_LEN );

    if( smem_diag_buffer == NULL )
    {
      return;
    }
  }

  len = MIN( len, SMEM_DIAG_BUFFER_LEN );

  memcpy( message, smem_diag_buffer, len );
  message[len-1] = '\0';
} /* smem_diag_get_message */

/*===========================================================================
FUNCTION      smem_diag_set_message
                                                                             
DESCRIPTION   Copy a diag message into shared memory.

ARGUMENTS     char * message - pointer to string to copy to shared memory.
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  Writes message in shared memory and does memory barrier so the
              the message is guaranteed to be in shared memory on return.
===========================================================================*/
void smem_diag_set_message( char *message )
{
  uint32 len;

  if( message == NULL )
  {
    return;
  }

  if( smem_diag_buffer == NULL )
  {
    smem_diag_buffer = (char *) smem_alloc( SMEM_DIAG_ERR_MESSAGE,
                                            SMEM_DIAG_BUFFER_LEN );

    if( smem_diag_buffer == NULL )
    {
      return;
    }
  }

  len = strlen( message );

  len = MIN( len, SMEM_DIAG_BUFFER_LEN );

  memcpy( smem_diag_buffer, message, len );
  smem_diag_buffer[SMEM_DIAG_BUFFER_LEN-1] = '\0';

  /* Be sure message is in memory before returning */
  SMEM_MEMORY_BARRIER();
} /* smem_diag_set_message */
