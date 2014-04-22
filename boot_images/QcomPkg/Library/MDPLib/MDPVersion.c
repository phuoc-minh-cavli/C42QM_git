/*=============================================================================
 
  File: MDPLIb.c
 
  Source file for MDP functions
  
 
  Copyright (c) 2011-2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#include "MDPLib.h"
#include "MDPLib_i.h"
#include "MDPSystem.h"

/*=========================================================================
     Default Defines
==========================================================================*/

  
              
/*=========================================================================
     Local Static Variables
==========================================================================*/



/*=========================================================================
     Local Static Functions
==========================================================================*/

/* MDP5XX Resolution Capabilities
 */
static const MDP_ResolutionCaps sResolutionCaps_MDP5XX =
{
    2,     /* uMinLayerWidth  */
    2,     /* uMinLayerHeight */
    2048,  /* uMaxLayerWidth  */
    2048,  /* uMaxLayerHeight */
};


/**************************************************************
* MDP5XX PP (Ping-Pong) Capabilities
*************************************************************/
static const MDP_PingPongCaps sPPCaps_MDP5XX[HAL_MDP_PINGPONG_MAX] =
{
    /* bSupported, uMaxWidth, uPPFeatureFlags */
    { FALSE,       0,         0 },   // HAL_MDP_PING_PONG_NONE
    { TRUE,        2560,      0 },   // HAL_MDP_PING_PONG_0
    { TRUE,        2560,      0 },   // HAL_MDP_PING_PONG_1
    { TRUE,        2560,      0 },   // HAL_MDP_PING_PONG_2
    { TRUE,        2560,      0 },   // HAL_MDP_PING_PONG_3
    { FALSE,       0,         0 },   // HAL_MDP_PING_PONG_4
};


/* Slave PingPong Buffer ID for PP split for all MDP versions
*/
static const HAL_MDP_PingPongId *PingPongSplitSlaveMapInfo[] =
{
    NULL,                          /* MDP 1.0.0 */
    NULL,                          /* MDP 1.1.0 */
    NULL,                          /* MDP 1.2.0 */
    NULL,                          /* MDP 1.3.0 */
    NULL,                          /* MDP 1.4.0 */
    NULL,                          /* MDP 1.5.0 */
    NULL,                          /* MDP 1.6.0 */
    NULL,                          /* MDP 1.7.0 */
    NULL,                          /* MDP 1.8.0 */
    NULL,                          /* MDP 1.9.0 */
    NULL,                          /* MDP 1.10.0*/
    NULL,                          /* MDP 1.11.0 */
    NULL,                          /* MDP 1.12.0 */
    NULL,                          /* MDP 1.13.0 */
    NULL,                          /* MDP 1.14.0 */    
    NULL,                          /* MDP 1.15.0 */
    NULL,                          /* MDP 1.16.0 */
    NULL,                          /* MDP 1.17.0 */
};

/* MDP5XX Device Capabilities
 */
static const MDP_DeviceCaps sDeviceCaps_MDP5XX =
{
  &sResolutionCaps_MDP5XX,
  &sPPCaps_MDP5XX[0]
};

/* MDP Device caps  for all MDP versions
 */
static const MDP_DeviceCaps *DeviceCapInfo[] =
{
  NULL,                          /* MDP 1.0.0 */
  NULL,                          /* MDP 1.1.0 */
  NULL,                          /* MDP 1.2.0 */
  NULL,                          /* MDP 1.3.0 */
  NULL,                          /* MDP 1.4.0 */
  NULL,                          /* MDP 1.5.0 */
  NULL,                          /* MDP 1.6.0 */
  NULL,                          /* MDP 1.7.0 */
  NULL,                          /* MDP 1.8.0 */
  NULL,                          /* MDP 1.9.0 */
  NULL,                          /* MDP 1.10.0*/
  NULL,                          /* MDP 1.11.0*/
  NULL,                          /* MDP 1.12.0*/
  NULL,                          /* MDP 1.13.0*/ 
  NULL,                          /* MDP 1.14.0*/
  NULL,                          /* MDP 1.15.0*/
  &sDeviceCaps_MDP5XX,           /* MDP 1.16.0*/
  &sDeviceCaps_MDP5XX            /* MDP 1.17.0*/
};

/*=========================================================================
      Public APIs
==========================================================================*/

MDP_Status MDPInitHwPrivateInfo(MDP_HwPrivateInfo *pHwPrivateInfo)
{
  MDP_Status          eStatus             = MDP_STATUS_OK;
  HAL_MDP_HwInfo      sHwInfo; 

  if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ReadVersionInfo(&(pHwPrivateInfo->sMDPVersionInfo), &sHwInfo))
  {
     eStatus = MDP_STATUS_HW_ERROR;
  }
  else
  {    
    // MDP Device Caps
    pHwPrivateInfo->pDeviceCaps = DeviceCapInfo[pHwPrivateInfo->sMDPVersionInfo.uMinorVersion];

    // MDP PingPong buffer split slave ID mapping table
    pHwPrivateInfo->pPPSplitSlaveMap = PingPongSplitSlaveMapInfo[pHwPrivateInfo->sMDPVersionInfo.uMinorVersion];
  }

  return eStatus;
}

