#ifndef MPM_UTILS_H
#define MPM_UTILS_H

/*=============================================================================


GENERAL DESCRIPTION

  This module contains routines for the multiprocessor power manager (MPM).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===============================================================================

                           EDIT HISTORY FOR MODULE
                           
$Header: //components/rel/core.tx/6.0/power/mpm/inc/mpm_utils.h#1 $

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include <stdint.h>

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

=============================================================================*/

void mpm_set_wakeup_timetick(uint64_t wakeup_tick);

#endif /* MPM_UTILS_H */

