/*
===========================================================================
*/
/**
  @file VCSMSS.c

  Main entry point for the MSS VCS driver.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/vcs/hw/mdm9205/mss/src/VCSMSS.c#2 $
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

#include "DALDeviceId.h"
#include "VCSDriver.h"
#include "VCSMSS.h"

/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Extern Definitions
==========================================================================*/


/*=========================================================================
      Function prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


VCSImageCtxtType VCS_ImageCtxt;


/* =========================================================================
      Prototypes
==========================================================================*/


/* =========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_SetRailMode
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_SetRailMode
(
  VCSDrvCtxt     *pDrvCtxt,
  VCSRailType     eRail,
  VCSRailModeType eMode
)
{
  /*-----------------------------------------------------------------------*/
  /* MSS rail is not present                                               */
  /*-----------------------------------------------------------------------*/

  return DAL_ERROR_NOT_SUPPORTED;

} /* END VCS_SetRailMode */


/* =========================================================================
**  Function : VCS_SetRailVoltage
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_SetRailVoltage
(
  VCSDrvCtxt  *pDrvCtxt,
  VCSRailType  eRail,
  uint32       nVoltageUV
)
{
  /*-----------------------------------------------------------------------*/
  /* MSS rail is not present                                               */
  /*-----------------------------------------------------------------------*/

  return DAL_ERROR_NOT_SUPPORTED;

} /* END VCS_SetRailMode */


/* =========================================================================
**  Function : VCS_InitImage
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitImage
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult            eResult;
  VCSPropertyValueType PropVal;

  /*-----------------------------------------------------------------------*/
  /* Assign the image context.                                             */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pImageCtxt = &VCS_ImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get the image BSP/XML.                                                */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_GetPropertyValue("VCSImageBSPConfig", &PropVal);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCS_GetPropertyValue failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Store the pointer to the image BSP in the image context.              */
  /*-----------------------------------------------------------------------*/

  VCS_ImageCtxt.pBSPConfig = (VCSImageBSPConfigType *)PropVal;

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_InitImage */


/* =========================================================================
**  Function : VCSStub_InitImage
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCSStub_InitImage
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCSStub_InitImage */
