#ifndef __SUBSYS_V_H__
#define __SUBSYS_V_H__

/*=============================================================================

        S U B S Y S T E M   M O D U L E   I N T E R N A L   H E A D E R 

GENERAL DESCRIPTION
  subsys internal header file.


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  pil_boot(subsys name)
    pil boots given subsystem.
    
  pil_shutdown(subsys name)
    Shutdowns given subsystem.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016-17 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/subsys_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "comdef.h"
#include "subsys.h"

/*=============================================================================

                            DATA DECLARATIONS
                            
=============================================================================*/

#define MAX_SUBSYS_NAME_SIZE     8
#define SUBSYS_CBS_MAX     4

//This will be internal as client doesnt call MBA boot 
//but Modem boot will trigger MBA boot
#define MBA_SUBSYS_STR           "MBA"

//Subsys Task Signal values
#define SUBSYS_MODEM_START       0x1
#define SUBSYS_MODEM_SHUTDOWN    0x2

//Test and Debug signals
#define SUBSYS_PRINT_DEBUG_LOGS  0x100
#define SUBSYS_TEST_LOAD_NTH_SEG 0x200

/* Enums for subsys state */
typedef enum
{
  SUBSYS_IS_DOWN,
  SUBSYS_LOADING_FAILED = SUBSYS_IS_DOWN,
  SUBSYS_LOADING_STARTED,
  SUBSYS_SHUTTING_DOWN_STARTED,
  SUBSYS_IS_UP
}subsys_status_type;

/* Enums for subsys shutdown status */
//Note: For now keeping these internal as there will be issues due to 
//shutdown as stated below. Later when required can be moved to subsys.h
typedef enum
{
  SUBSYS_STOP_SUCCESS,
  SUBSYS_STOP_FAILURE
}subsys_stop_status_type;

/* Options input for Shutdown function */
typedef struct
{
  uint32 dummy;       /* temporary placeholder */
}subsys_shutdown_options_type;

/* callback function prototype to be used when calling subsys_shutdown() api */
typedef void (*subsys_stop_cb_fn)(void * cb_data, subsys_stop_status_type stop_status);


#define subsys_shutdown_options_init(options) \
{ \
  options.dummy = 0; \
}


/*=============================================================================

                        FUNCTION DECLARATIONS
                        
=============================================================================*/
/*=============================================================================

FUNCTION SUBSYS_INIT

DESCRIPTION
  Initializes Subsys Module

DEPENDENCIES
  
  
RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void subsys_init(void);

/*=============================================================================

FUNCTION SUBSYS_SHUTDOWN

DESCRIPTION
  Shutdowns specified subsystem from subsys task context. Book keeps how many times
  stop request came for given subsystem. Returns success/failure immediately which
  indicates only that request is taken. Actual subsys stop status is returned in
  given callback.

DEPENDENCIES
  Subsystem should be started before shutdown
  
RETURN VALUE
  SUBSYS_SUCCESS
  SUBSYS_INVALID_SUBSYS
  SUBSYS_NOT_ALLOWED

SIDE EFFECTS

=============================================================================*/
//Note: This function is not exposed for now since there will be issues if we do shutdown 
//and start multiple times as clients doesnt handle SSR type of scenarios on TN, TX
subsys_error_type subsys_shutdown(char *str, subsys_shutdown_options_type *options, subsys_stop_cb_fn stop_cb, void *cb_data);


/*=============================================================================

FUNCTION SUBSYS_PRINT_DEBUG_LOGS

DESCRIPTION
  Prints Subsystem Debug Logs. Used for Debugging purposes

DEPENDENCIES
  
  
RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void subsys_print_debug_logs(void);


/*=============================================================================

FUNCTION SUBSYS_LOAD_SEGMENT

DESCRIPTION
  Loads given nth segment of modem in its physical memory. Used for Debugging 
  purposes

DEPENDENCIES
    
RETURN VALUE
  none

SIDE EFFECTS

=============================================================================*/
void subsys_load_segment(uint32 num);

#endif /* __SUBSYS_V_H__ */
