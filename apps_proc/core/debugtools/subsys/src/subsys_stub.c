/*=============================================================================

                  S U B S Y S T E M   M O D U L E

GENERAL DESCRIPTION
  Implements Subsys framework for booting/shutting down various subsystems.
  This file provides stubs for subsys public apis

EXTERNALIZED FUNCTIONS

  subsys_start(subsys name)
    Starts requested subsystem.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2019 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/subsys_stub.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

01/07/19   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "subsys.h"
#include "subsys_v.h"
#include "pil.h"

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION SUBSYS_START

DESCRIPTION
  Starts specified subsystem from subsys task context. Book keeps how many times
  start request came for given subsystem. Returns success/failure immediately which
  indicates only that request is taken. Actual subsys start status is returned in
  given callback.

DEPENDENCIES

RETURN VALUE
  SUBSYS_SUCCESS
  SUBSYS_INVALID_SUBSYS
  SUBSYS_NOT_ALLOWED

SIDE EFFECTS

=============================================================================*/
subsys_error_type subsys_start(char *str, subsys_start_options_type *options, subsys_start_cb_fn start_cb, void * cb_data)
{
  return SUBSYS_INVALID_SUBSYS;
} /* subsys_start */


