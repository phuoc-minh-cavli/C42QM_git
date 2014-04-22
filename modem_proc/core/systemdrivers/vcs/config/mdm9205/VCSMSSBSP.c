/*
===========================================================================
*/
/**
  @file VCSMSSBSP.c

  BSP data for the MSS VCS driver.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/vcs/config/mdm9205/VCSMSSBSP.c#2 $
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

#include "VCSMSSBSP.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/*
 * Image BSP configuration.
 */
const VCSImageBSPConfigType VCS_ImageBSPConfig =
{
  .bEnableMVC = FALSE,
};
