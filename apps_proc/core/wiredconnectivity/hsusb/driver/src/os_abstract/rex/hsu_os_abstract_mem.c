/*==============================================================================

                 H S U _ O S _ A B S T R A C T _ M E M . C


GENERAL DESCRIPTION
  High-Speed USB OS abstraction layer memoty management API implementation.

EXTERNALIZED FUNCTIONS

  Externalized to outside of the OS abstraction layer:
    hsu_os_cache_flush
    hsu_os_cache_invalidate

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR MODULE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/os_abstract/rex/hsu_os_abstract_mem.c#1 $
  $DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/10   gs      Prepended hsu to function names
11/21/06   ds      Initial version

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

/* AMSS include files. */
#ifndef T_REXNT
//#include <cache_mmu.h>
#endif  /* T_REXNT */

/* High Speed USB subsystem include files. */
#include <jtypes_int.h>
#include <hsu_common_int.h>

/* OS abstraction layer include files. */
#include <port_mem.h>

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Externalized Variables
==============================================================================*/

/*==============================================================================
  File Static Variables
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================

  FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_OS_CACHE_FLUSH

DESCRIPTION
  Flush a memory region from the data cache to the system memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_cache_flush(void *addr, juint32_t size)
{
  HSU_USE_PARAM(addr);
  HSU_USE_PARAM(size);

#if 0 
  /* TODO  */
  #ifndef T_REXNT
  dcache_flush_region(addr, size);
  #else   /* T_REXNT */
  HSU_USE_PARAM(addr);
  HSU_USE_PARAM(size);
  #endif  /* T_REXNT */
#endif /* if 0 */
}

/*===========================================================================
FUNCTION HSU_OS_CACHE_INVALIDATE

DESCRIPTION
  Invalidate a memory region from the data cache, so that further references
  to this memory region will produce an access to the system memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_cache_invalidate(void *addr, juint32_t size)
{
  HSU_USE_PARAM(addr);
  HSU_USE_PARAM(size);

#if 0 
  #ifndef T_REXNT
  dcache_inval_region(addr, size);
  #else   /* T_REXNT */
  HSU_USE_PARAM(addr);
  HSU_USE_PARAM(size);
  #endif  /* T_REXNT */
#endif /* if 0 */
}
