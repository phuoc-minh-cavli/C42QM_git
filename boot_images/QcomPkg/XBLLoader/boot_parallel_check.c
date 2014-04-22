/*=============================================================================

                             boot parallel check  

GENERAL DESCRIPTION
  This file contains the implementation of functions for determining the mode of 
  transfer of pages through BAM in parallel loading and hashing.
  
Copyright 2017,2019 by Qualcomm Technologies, Inc. All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/XBLLoader/boot_parallel_check.c#3 $
  $DateTime: 2019/08/19 04:01:22 $ 
  $Author: pwbldsvc $
  
when       who        what, where, why
--------   ---        ---------------------------------------------------------
08/14/19   aus        Removed unused functions
08/29/17   rohik      Initial revision
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_parallel_check.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
read_state read_parallel = READ_SEQUENTIAL;

