/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 * csd_resourcemanager.h
 *
 *  Created on: Jun 6, 2011
 *      $Author: pwbldsvc $ 
 *      $Header: //components/rel/audio.tx/2.0/audio/csd/utils/resourcemanager/inc/csd_resourcemanager.h#1 $
 *      $DateTime: 2018/12/25 22:39:07 $
 *      $Change: 17929171 $
 *      $Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/11   sgk   Initial Creation
============================================================================*/
#ifndef CSD_RESOURCEMANAGER_H_
#define CSD_RESOURCEMANAGER_H_

/**************************************************************************
 *                         CSD HEADER DEPENDENCIES                        *
 **************************************************************************/
#include "csd.h"
#include "csd_as.h"
#include "csd_ac.h"
#include "csd_vs.h"
#include "csd_vc.h"
#include "csd_vm.h"
#include "csd_as_ioctl.h"       // CSD_AS IOCTL Interfaces
#include "csd_ac_ioctl.h"       // CSD_AC IOCTL Interfaces
#include "csd_dev_ioctl.h"      // CSD_DEV IOCTL Interfaces
#include "csd_vs_ioctl.h"       // CSD_VS IOCTL Interfaces
#include "csd_vc_ioctl.h"       // CSD_VC IOCTL Interfaces
#include "csd_vm_ioctl.h"       // CSD_VM IOCTL Interfaces

#include "csd_list.h"
#include "csd_os_dependencies.h"

/**************************************************************************
 *                   CSD RESOURCE MANAGER Specific Defines               *
 **************************************************************************/
#define CSD_RM_POOL_SIZE                 (64)               // This Should be same as MAX_CSD_HANDLES in Module
#define CSD_RM_CMD_MODE_PREPROCESS       (0)                // CMD Processing Notification before csd call
#define CSD_RM_CMD_MODE_POSTPROCESS      (1)                // CMD Processing Notification after csd call
#define CSD_RM_IDLE_TIMEOUT              (10*1000*1000)     // Idle time wait for WorkLoop thread in (micro)usec
#define CSD_RM_WAIT_TIMEOUT              (10*1000*1000)     // Wait Time before Processing SUSPEND in (micro)usec
#define CSD_RM_WKLOOP_PRIORITY           (29)              // Priority of the Workloop thread
#define CSD_RM_MAX_WKLOOP_EVENTS         (5)                // Maximum Events that can be registerd with Workloop
/*
 * @brief: Enum defines to indicate state of the ResourceManager
 */
typedef enum csd_rm_state
{
    CSD_RM_STATE_INIT      = 1,             // RM is Initialized and no active elements in RM List
    CSD_RM_STATE_ACTIVE    = 2,             // RM is Initialized and AS/DEV/VM active elements are in RM List
    CSD_RM_STATE_SUSPENDED = 4,             // RM is Initialized and DEV active elements in RM List are Suspended
    CSD_RM_STATE_IN_SSR    = 8,             // RM is Initialized and in SSR, all affected active elements in RM List are Cleared (TBD)
    CSD_RM_STATE_UNKNOWN   = 0xFF,          // RM is not Initialized or is already DeInited.
}csd_rm_state_e;

/*
 * @brief: Enum defines to indicate Command type passed to the ResourceManager
 */
typedef enum csd_rm_command_type
{
    CSD_CMD_OPEN      = 0,
    CSD_CMD_CLOSE     = 1,
    CSD_CMD_READ      = 2,
    CSD_CMD_WRITE     = 3,
    CSD_CMD_IOCTL     = 4,
    CSD_CMD_OTHER     = 5,
}csd_rm_command_type_e;

/*
 * @brief: Enum defines to indicate state of CSD objects held in the ResourceManager
 */
typedef enum csd_rm_handle_state
{
    CSD_RM_OBJ_STATE_INACTIVE   = 0,
    CSD_RM_OBJ_STATE_ACTIVE     = 1,
    CSD_RM_OBJ_STATE_UNKNOWN    = 0xFF,
}csd_rm_handle_state_e;
 
/*
 * @brief: Structure defines to necesary details (information) per csd data object
 *          as maintained in the ResourceManager list
 */
typedef struct csd_rm_state_info
{
   uint32_t nHandle;       // Handle of the client used for allocation/De-allocation
   uint32_t nIndex;        // Index of the Handle for Fast Lookup using Indexing
   uint32_t nType;         // Handle Type (AS/AC/DEV/VS/VC/VM) as used in the enum
   uint32_t nState;        // State of the Handle defined @ csd_rm_handle_state_e
   uint32_t nDevCount;     // Count of the Devices Enabled @ dev_cmd_enable
}csd_rm_state_info_t;

/*
 * @brief: Structure defines the per csd node object 
 *          as maintained in the ResourceManager list.
 */
typedef struct csd_rm_state_node
{
   csd_rm_state_info_t sRMInfo; /**< Info Containing the RM Details for Allocation */
   struct csd_list_node node;   /**< CSD List for Easy Access of Active Nodes */
}csd_rm_state_node_t;

/*
 * @brief: Callback function prototype used for the Workloop
 */
typedef DALResult (*csd_rm_workloop_cb)( DALSYSEventHandle evt, void * client_data);
 /*
 * @brief: Enum to define the list of commands to be processed by Workloop
 *          (Cmd List can be extended as necessary)
 */
 
 typedef enum csd_rm_workloop_cmd
 {
    CSD_RM_WORKLOOP_INVALID_CMD         = 0,
    CSD_RM_WORKLOOP_START_TIMEOUT       = 1,
    CSD_RM_WORKLOOP_ABORT_TIMEOUT       = 2,
    CSD_RM_WORKLOOP_TERMINATE           = 3,
    CSD_RM_WORKLOOP_SSR_RESET_NOTIFY    = 4,
    CSD_RM_WORKLOOP_SSR_READY_NOTIFY    = 5,
 }csd_rm_workloop_cmd_e;

/*
 * @brief: Structure defines to necesary details (information) of workloop
 *          as maintained in the ResourceManager
 */
#if 0
typedef struct csd_rm_workloop
{
    DALSYSWorkLoopHandle    workloop;       /**< WorkLoop Thread Handle */
    DALSYSEventHandle       init_evt;       /**< signal to start the workloop Timer :: DALSYS_EVENT_ATTR_WORKLOOP_EVENT */
    DALSYSEventObj          init_obj;       /**< Associated Object for Init Event */
    DALSYSEventHandle       term_evt;       /**< signal to terminate the workloop :: DALSYS_EVENT_ATTR_WORKLOOP_EVENT */
    DALSYSEventObj          term_obj;       /**< Associated Object for Term Event */
    DALSYSEventHandle       time_evt;       /**< signal to indicate timeout to the workloop :: DALSYS_EVENT_ATTR_TIMEOUT_EVENT */
    DALSYSEventObj          time_obj;       /**< Associated Object for Time Event */
    DALSYSEventHandle       cmd_evt;        /**< signal to indicate command to the workloop :: DALSYS_EVENT_ATTR_CLIENT_DEFAULT */
    DALSYSEventObj          cmd_obj;        /**< Associated Object for CMD Event */
    DALSYSEventHandle       ssr_evt;       /**< signal to indicate SSR notification to the workloop :: DALSYS_EVENT_ATTR_WORKLOOP_EVENT */
    DALSYSEventObj          ssr_obj;       /**< Associated Object for SSR Event */
    DALSYSSyncHandle        lock;           /**< Sync Object for WorkLoop:: Not Needed */
    DALSYSSyncObj           lock_obj;       /**< Associated Object for Lock Handle */
	csd_rm_workloop_cb      cb;             /**< Callback registered with the Workloop */ 
	void*                   client_data;    /**< Data Expected in the callback of Workloop */
    csd_rm_workloop_cmd_e   eCmd;           /**< Workloop Command type to be processed */
}csd_rm_workloop_t;
#endif
/**************************************************************************
 *                   CSD Resource Manager Interfaces                      *
 **************************************************************************/
int32_t csd_rm_init(uint32_t nMaxHandles);
int32_t csd_rm_dinit(void);

int32_t csd_rm_process_csd_command(csd_rm_command_type_e eCmdType, uint32_t nHandle, uint32_t nCmd, uint32_t nCmdStatus, uint32_t nMode);
int32_t csd_rm_csd_command_begin(csd_rm_command_type_e eCmdType, uint32_t nCmd, uint32_t nCmdStatus);
int32_t csd_rm_csd_command_end(csd_rm_command_type_e eCmdType, uint32_t nCmd, uint32_t nCmdStatus);

#endif /* CSD_RESOURCEMANAGER_H_ */
