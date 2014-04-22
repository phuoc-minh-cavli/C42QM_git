#ifndef __VCSMSSBSP_H__
#define __VCSMSSBSP_H__
/*
===========================================================================
*/
/**
  @file VCSMSSBSP.h 
  
  Header description for the MSS VCS driver BSP format.
*/
/*  
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/vcs/hw/mdm9205/mss/inc/VCSMSSBSP.h#1 $
  $DateTime: 2018/09/07 03:22:57 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/22/14   lil     Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "VCSBSP.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VCSImageBSPConfigType
 *
 * BSP data structure for describing the image configuration.
 *
 *  bEnableMVC - MVC enable switch.
 */
typedef struct VCSImageBSPConfigType
{
  boolean bEnableMVC;
} VCSImageBSPConfigType;


#endif  /* __VCSMSSBSP_H__ */ 

