/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          System Monitor non-init data
                
GENERAL DESCRIPTION
The scatter load
  file is set up to put any *_nzi.o files in an uninitialized portion of RAM.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2006-20017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------

 

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys_m_internal.h"

 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#pragma arm section zidata = ".nzi"


char sys_m_sfr_buffer[SYS_M_SFR_LENGTH];




#pragma arm section zidata
