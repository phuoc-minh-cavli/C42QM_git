/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 * csd_ocmem.h
 *
 *  Created on: Jun 6, 2011
 *      $Author: pwbldsvc $ 
 *      $Header: //components/rel/audio.tx/2.0/audio/csd/utils/ocmem/inc/csd_ocmem.h#1 $
 *      $DateTime: 2018/12/25 22:39:07 $
 *      $Change: 17929171 $
 *      $Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/11   sgk   Initial Creation
============================================================================*/
#ifndef CSD_OCMEM_H_
#define CSD_OCMEM_H_

/**************************************************************************
 *                         CSD HEADER DEPENDENCIES                        *
 **************************************************************************/
#include "csd.h"
#include "csd_os_dependencies.h"

/**************************************************************************
 *                   CSD OCMEM Specific Defines               *
 **************************************************************************/
 /*
 * @brief: IOCTL calls to the OCMEM Module;
 */
#define CSD_OCMEM_CMD_ENABLE_REQ                 (1)     /**< Command to request enabling the OCMEM for LPA Stream */
#define CSD_OCMEM_CMD_DISABLE_REQ                (2)     /**< Command to request disabling the OCMEM for LPA Stream */
#define CSD_OCMEM_CMD_SSR_RESTART_START          (3)     /**< Command to handle SSR_RESTART_START state */
#define CSD_OCMEM_CMD_SSR_RESTART_DONE           (4)     /**< Command to handle SSR_RESTART_DONE state */
#define CSD_OCMEM_CMD_VOICE_CALL_START           (5)     /**< Command to handle the voice call start case */
#define CSD_OCMEM_CMD_VOICE_CALL_DONE            (6)     /**< Comamnd to handle the voice call stop case */
#define CSD_OCMEM_CMD_SUSPEND                    (7)     /**< Command to handle the move to SUSPEND state (Power Saving Mode) */
#define CSD_OCMEM_CMD_RESTORE                    (8)     /**< Command to handle the move to RESTORE state  (Power Saving Mode) */
#define CSD_OCMEM_CMD_CLEANUP                    (9)     /**< Command to handle cleanup event (on csd_dinit) */
#define CSD_OCMEM_CMD_DYNAMIC_EVICT              (10)    /**< Command to handle Dynamic Eviction event (on ocmem cb for release resource) */
#define CSD_OCMEM_CMD_DYNAMIC_REINSTATE          (11)    /**< Command to handle Dynamic reinstante event (on ocmem cb for resource avilable) */

/*
 * @brief: Structure defines payload for CSD_OCMEM_CMD_ENABLE
 */
typedef struct csd_ocmem_enable_req
{
   uint32_t nHandle;        /**< Stream Handle requesting for OCMEM Disable >*/
   uint32_t nFormat;        /**< Format of the stream type >*/
}csd_ocmem_enable_req_t;

/*
 * @brief: Structure defines payload for CSD_OCMEM_CMD_DISABLE. 
 */
typedef struct csd_ocmem_disable_req
{
   uint32_t nHandle;        /**< Stream Handle requesting for OCMEM Disable >*/
   uint32_t nFormat;        /**< Format of the stream type >*/
}csd_ocmem_disable_req_t;


/**************************************************************************
 *                   CSD AS OCMEM Interfaces                              *
 **************************************************************************/
int32_t csd_ocmem_init(void);
int32_t csd_ocmem_dinit(void);
int32_t csd_ocmem_ioctl(uint32_t nCommand, void* pPayload, uint32_t nSize);

#endif /* CSD_OCMEM_H_ */
