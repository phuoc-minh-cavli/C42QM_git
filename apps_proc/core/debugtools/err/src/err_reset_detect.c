/*=============================================================================

                        E r r  R e s e t  D e t e c t i o n
 
GENERAL DESCRIPTION
  This file contains the code to detect HW reset of the chip and
  transition into the downloader.

EXTERNALIZED FUNCTIONS 
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2007-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/err/src/err_reset_detect.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
10/29/14   rks     CR745365:set dload cookie by SCM call
10/11/12   mcg     Overhauled to use tms_dload and eliminate SPLASH ui support 
06/24/10   niting  Added Sahara memory dump support by moving magic numbers 
                   to a specified memory location.
07/25/07   tbg     Initial revision (pulled from boot_hw_reset.c)
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "err_reset_detect.h"
#include "tms_dload.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/



/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION  ERR_RESET_DETECT_DISABLE

DESCRIPTION
  This function will clear hw reset detect flags.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void err_reset_detect_disable(void)
{
  tms_dload_clear_magic_numbers_STM();
}


