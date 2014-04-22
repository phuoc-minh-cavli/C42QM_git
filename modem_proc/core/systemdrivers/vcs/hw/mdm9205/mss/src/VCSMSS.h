#ifndef VCSMSS_H
#define VCSMSS_H
/*
===========================================================================
*/
/**
  @file VCSMSS.h

  Internal header file for the VCS device driver on the MSS image.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/vcs/hw/mdm9205/mss/src/VCSMSS.h#2 $
  $DateTime: 2019/01/02 21:42:16 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  12/06/18   shm     First version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DDIVCS.h"
#include "VCSMSSBSP.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*
 * NPA client names
 */
#define VCS_NPA_CLIENT_NAME_VDD_MSS_DISABLE_SCALING "/vdd/mss/disable_scaling"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VDD_MSS rail MVC mode supports 4 corners:
 * SVS2, SVS, NOMINAL, TURBO.
 */
#define VCS_MVC_NUM_OF_VOLTAGES 4


/*
 * VCSMVCVoltageType
 *
 *  eCorner    - Rail corner.
 *  nVoltageUV - Rail voltage.
 *  nRegIndex  - Index into the MVC register set.
 */
typedef struct VCSMVCVoltageType
{
  VCSCornerType eCorner;
  uint32        nVoltageUV;
  uint32        nRegIndex;
} VCSMVCVoltageType;


/*
 * MVC Context.
 *
 *  nDisableMVC     - Bitfield flags for disabling MVC.
 *  aMVCVoltage     - MVC voltage table.
 *  apMVCVoltageMap - Map of corner enum to MVC voltage entry.
 */
typedef struct
{
  uint32             nDisableMVC;
  VCSMVCVoltageType  aMVCVoltage[VCS_MVC_NUM_OF_VOLTAGES];
  VCSMVCVoltageType *apMVCVoltageMap[VCS_CORNER_NUM_OF_CORNERS];
} VCSMVCCtxtType;


/**
 * VCS driver image context.
 *
 *  MVCCtxt    - MVC context.
 *  pBSPConfig - Pointer to BSP data.
 */
typedef struct
{
  VCSMVCCtxtType         MVCCtxt;
  VCSImageBSPConfigType *pBSPConfig;
} VCSImageCtxtType;


/*=========================================================================
      Functions
==========================================================================*/


#endif /* !VCSMSS_H */

