#ifndef ERR_RESET_DETECT_H
#define ERR_RESET_DETECT_H
/*=============================================================================

                         E r r  R e s e t  D e t e c t i o n
                                  H e a d e r  F i l e

 
GENERAL DESCRIPTION
  This file contains the code to detect HW reset of the chip and
  transition into the downloader.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 -2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/debugtools/err_reset_detect.h#1 $ 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/29/14   rks     CR745365:set dload cookie by SCM call
07/25/07   tbg     Initial revision (pulled from boot_hw_reset.h)

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

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
void err_reset_detect_disable(void);

#endif /* ERR_RESET_DETECT_H */
