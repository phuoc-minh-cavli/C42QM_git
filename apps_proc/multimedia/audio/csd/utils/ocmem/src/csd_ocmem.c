/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 * csd_as_pmem.c
 *
 *  Created on: Aug 15, 2012
 *      Author: sameerk
 *      $Header: //components/rel/audio.tx/2.0/audio/csd/utils/ocmem/src/csd_ocmem.c#1 $
 *      $DateTime: 2018/12/25 22:39:07 $
 *      $Change: 17929171 $
 *      $Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/12   sgk   Initial Creation
============================================================================*/

/*********************************************************************************************************
 *                                    INCLUDES FOR OCMEM SUPPORT                                         *
 *                                 Explicitly for CSD_Proxy Clients                                      *
 *********************************************************************************************************/
#include "csd_ocmem.h"  // OCMEM Specific includes and Defines
/*********************************************************************************************************
 *                               CSD INTERNAL SUPPORT DEFINES AND DATA STRUCTURES                        *
 *********************************************************************************************************/

/*********************************************************************************************************
 *              CSD_OCMEM.c Interfaces that can be called from other CSD Module Files                 *
 *********************************************************************************************************/
/** All Functions on A-Family are Stubs and perform no operation */
int32_t csd_ocmem_dinit(void)
{
    return CSD_EOK; //CSD_EUNSUPPORTED;
}

int32_t csd_ocmem_init(void)
{
    return CSD_EOK; //CSD_EUNSUPPORTED;
}

int32_t csd_ocmem_ioctl(uint32_t nCommand, void* pPayload, uint32_t nSize)
{
    return CSD_EOK; //CSD_EUNSUPPORTED;
}
