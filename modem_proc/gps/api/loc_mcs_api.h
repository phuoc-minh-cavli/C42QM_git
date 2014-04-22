#ifndef LOC_MCS_API_H
#define LOC_MCS_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Location MCS API Header File

GENERAL DESCRIPTION
This file contains API definitions between Location MCS module and
other modules outside GNSS.

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/api/loc_mcs_api.h#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *------------------------------------------------------------------------*/
#include "comdef.h"
#include "cgps_api.h"


/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Structures and Enumerated Types Definitions
 * -----------------------------------------------------------------------*/

typedef enum
{
  LOC_MCS_SYS_WWAN_STATE_NONE = -1,
  LOC_MCS_SYS_WWAN_STATE_PTW_EVENT,
  LOC_MCS_SYS_WWAN_STATE_T3324_DURATION_EVENT,
  LOC_MCS_SYS_WWAN_STATE_T3324_EXPIRED_EVENT
} loc_mcs__SysWwanStateUpdateEventType;

typedef struct
{
  /* Subscription ID (0 = AS_ID_1, 1 = AS_ID_2, 2 = AS_ID_3) */
  /* Must be less than build definition of MAX_AS_IDS, i.e. 1, for 9x05 */
  uint8 b_SubscrID;

  /* WWAN state update event for above subscription ID */
  loc_mcs__SysWwanStateUpdateEventType  e_SysWwanStateUpdateEvent;
  uint32                                q_EdrxDuration;
  uint32                                q_T3324Duration;
} loc_mcs_SysWwanStateUpdateStructType;


/*--------------------------------------------------------------------------
 * Function Definitions
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Function: loc_mcs_SysWwanStateUpdate
 *
 * Description:
 *   This API is called by ML1 to send SYS WWAN State Update to GNSS
 *
 * Parameters: cpz_SysWwanStateUpdate - SYS WWAN State Update structure
 *   
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 * -----------------------------------------------------------------------*/

extern void loc_mcs_SysWwanStateUpdate(const loc_mcs_SysWwanStateUpdateStructType *cpz_SysWwanStateUpdate);


#endif /* #ifndef LOC_MCS_API_H */
