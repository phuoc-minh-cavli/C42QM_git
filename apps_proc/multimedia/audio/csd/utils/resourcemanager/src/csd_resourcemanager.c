/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 * csd_resourcemanager.c
 *
 *  Created on: Jun 6, 2011
 *      $Author: pwbldsvc $ 
 *      $Header: //components/rel/audio.tx/2.0/audio/csd/utils/resourcemanager/src/csd_resourcemanager.c#1 $
 *      $DateTime: 2018/12/25 22:39:07 $
 *      $Change: 17929171 $
 *      $Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/28/12   jk       resourcemanager stub file
============================================================================*/

/*********************************************************************************************************
 *                         IMPLEMENTATIONS FOR CSD_SERVER PMEM ALLOCATION                                *
 *                                 Explicitly for CSD_Proxy Clients                                      *
 *********************************************************************************************************/
#include "csd_resourcemanager.h"    // CSD PMEM Defines and Other Dependency Includes

/* stubs returning CSD_EOK */
int32_t csd_rm_init(uint32_t nMaxHandles)
{
   return CSD_EOK;
}
int32_t csd_rm_dinit(void)
{
   return CSD_EOK;
}

int32_t csd_rm_process_csd_command(csd_rm_command_type_e eCmdType, 
                                   uint32_t nHandle, 
                                   uint32_t nCmd, 
                                   uint32_t nCmdStatus, 
                                   uint32_t nMode)
{
   return CSD_EOK;
}

int32_t csd_rm_csd_command_begin(csd_rm_command_type_e eCmdType, 
                                 uint32_t nCmd, 
                                 uint32_t nCmdStatus)
{
   return CSD_EOK;
}
int32_t csd_rm_csd_command_end(csd_rm_command_type_e eCmdType, 
                               uint32_t nCmd, 
                               uint32_t nCmdStatus)
{
   return CSD_EOK;
}

