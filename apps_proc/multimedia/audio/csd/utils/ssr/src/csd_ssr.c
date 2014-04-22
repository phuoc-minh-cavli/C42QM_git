/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 * csd_as_pmem.c
 *
 *  Created on: Aug 15, 2012
 *      Author: sameerk
 *      $Header: //components/rel/audio.tx/2.0/audio/csd/utils/ssr/src/csd_ssr.c#1 $
 *      $DateTime: 2018/12/25 22:39:07 $
 *      $Change: 17929171 $
 *      $Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/12   sgk   Initial Creation
============================================================================*/

/*********************************************************************************************************
 *                                    INCLUDES FOR SSR UTILITY SUPPORT                                   *
 *                                                                                                       *
 *********************************************************************************************************/
#include "csd.h"
#include "csd_ssr.h"  /* SSR Utility Function Declaration */

/*********************************************************************************************************
 *                               CSD INTERNAL SUPPORT DEFINES AND DATA STRUCTURES                        *
 *********************************************************************************************************/

/*********************************************************************************************************
 *              CSD_SSR.c Interfaces that can be called from other CSD Module Files                 *
 *********************************************************************************************************/
/*********************************************************************************************************
 *                                        Internal Functions                                             *
 *********************************************************************************************************/


/** All Functions are Stubs and perform no operation */
/*! \fn uint32_t csd_ssr_init( void )
    \brief This function used to register the csd function with the SSR module.
    \return int32_t
*/
int32_t csd_ssr_init( void )
{
    return CSD_EOK;
}

/*! \fn uint32_t csd_ssr_deinit( void )
    \brief This function used to un-register the csd function with the SSR module.
    \return int32_t
*/
int32_t csd_ssr_deinit()
{
    return CSD_EOK;
}


