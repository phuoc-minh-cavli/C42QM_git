 /**===========================================================================
 **
 ** FILE 
 **     mba_debug.c
 ** 
 ** GENERAL DESCRIPTION 
 **     This file provides the APIs to use QTimers within
 **     the Modem subsystem. All QTimer functionality is based
 **     on a global system counter which runs at 19.2 Mhz.                                        
 ** 
 **==========================================================================*/


/*=============================================================================


                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/securemsm/mba/modem_stub.c#2 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/01/12   mm      Initial Revision for MSM8974
 
=============================================================================*/


/*=============================================================================
                                MODULE INCLUDES
                          ADD NEW ONES UNDER THIS LINE    
=============================================================================*/
#include <IxErrno.h>
#include <string.h>
#include <mba_ac.h>
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Stub function - RC Init stub */
void XPUInit(void)
{
  return; //sys_m_xpu_handler_register(modem_xpus_open);
}

/* Stub function */
uint8 mba_oem_seccrtl_allow_unlock_xpu(void)
{
  return FALSE;
}