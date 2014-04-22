/******************************************************************************
  @file: loc_xtra.h
  @brief:   This module handles gpsone XTRA related features .
  

  DESCRIPTION
   Qualcomm Location API XTRA Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_xtra.h#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
5/19/16    yh       LocMW refactoring
04/27/11   spn      Updated for Loc-Api 2.0
======================================================================*/


#ifndef LOC_XTRA_H
#define LOC_XTRA_H

#include "aries_os_api.h"
#include "pdapi_lite.h" //Todo: This has to be removed once pdapi_lite,h is universally added.
typedef struct 
{
   boolean                 v_XtraCmdInProgress;
   pdsm_lite_xtra_cmd_e_type    e_XtraCmd;
   loc_client_handle_type  l_ClientHandle;
   uint32                  q_QmiLocMsgId;     /* Corresponding QMI_LOC Id for PDSM Parameter */
   char*                   pb_XtraData; // Memory will be allocated dynamically for the XTRA file
 
   /* XTRA injection related data structure */
   uint32                  q_TotalPartsToReceive;
   uint32                  q_TotalBytesToReceive;
   uint32                  q_NumOfPartsReceived; 
   uint32                  q_NumOfBytesReceived;

   uint32                  q_TotalPartsToInject;
   uint32                  q_NumOfPartsInjected;
   uint32                  q_NumOfBytesInjected;

   os_TimerMsgType*        pz_XtraProcessTimer;
} locXtra_ModuleDataType;

boolean locXtra_Init (void);

boolean locXtra_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
);

int locXtra_ProcessCmdRequest (const LocMW_LocCmdStructT * pz_Cmd);
void locXtra_ProcessTimerCb ( void );
#endif // LOC_XTRA_H
