#ifndef __MCPM_EXTERN_H__
#define __MCPM_EXTERN_H__

/*=========================================================================

           M O D E M   C L O C K   A N D   P O W E R   M A N A G E R 
               
                E X T E R N A L   H E A D E R   F I L E


  GENERAL DESCRIPTION
    This file contains the external interface functions for the client outside of
    MODEM SW and definitions for Modem Clock and Power Manager (MCPM).
  
  EXTERNALIZED FUNCTIONS
    MCPM_Init
      
  INITIALIZATION AND SEQUENCING REQUIREMENTS
    Invoke the MCPM_Init function to initialize the MCPM.
  
         Copyright (c) 2019 by QUALCOMM Technologies Inc.  All Rights Reserved.


==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/mpower.mpss/11.0/api/mcpm_extern.h#6 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
02/11/13   vs      Ported from Dime release branch.


==========================================================================*/ 


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/



#include "comdef.h"
#include "msg.h"

/*=========================================================================
      Typedefs
==========================================================================*/


/*==========================================================================
               FUNCTION DECLARATIONS FOR MODULE
==========================================================================*/


/*==========================================================================

  FUNCTION      MCPM_INIT

  DESCRIPTION   This function is called once at powerup. This function 
                initializes MCPM variables.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

extern void MCPM_Init(void);


#endif /* __MCPM_EXTERN_H__ */

