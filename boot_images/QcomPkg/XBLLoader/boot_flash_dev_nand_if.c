/*=============================================================================

                                  Boot Shared
                          NAND Boot Device Singleton

GENERAL DESCRIPTION
  Singleton for the NAND boot device interface.

Copyright 2010, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   ck      Corrected copyright
09/21/10   plc     Initial port to SB3.0

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_dev_nand_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/* Define the singleton */
BOOT_SINGLETON_DEFINE(boot_flash_dev_nand_if);

