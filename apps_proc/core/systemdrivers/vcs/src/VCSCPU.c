/*
===========================================================================
*/
/**
  @file VCSCPU.c 
  
  CPU-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

  ==================================================================== 
  $Header: //components/rel/core.tx/6.0/systemdrivers/vcs/src/VCSCPU.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/22/14   lil     Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include <DALSys.h>
#include "VCSDriver.h"
#include "DALDeviceId.h"
#include <npa_resource.h>


/*=========================================================================
      Externs
==========================================================================*/


/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_InitCPUs
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitCPUs
(
  VCSDrvCtxt       *pDrvCtxt,
  VCSBSPConfigType *pBSPConfig
)
{
  DALResult         eResult;
  uint32            i, j, k, nSize;
  VCSCPUNodeType   *pCPU;
  VCSRailNodeType  *pRail;

  /*-----------------------------------------------------------------------*/
  /* Nothing to do if no CPUs present.                                     */
  /*-----------------------------------------------------------------------*/

  if (pBSPConfig->nNumCPUConfigs == 0)
  {
    return DAL_SUCCESS;
  }

  pDrvCtxt->nNumCPUs = pBSPConfig->nNumCPUConfigs;

  /*-----------------------------------------------------------------------*/
  /* Allocate memory for the CPU nodes.                                    */
  /*-----------------------------------------------------------------------*/

  nSize = pDrvCtxt->nNumCPUs * sizeof (*pDrvCtxt->aCPUs);

  pDrvCtxt->aCPUs = (VCSCPUNodeType*)malloc(nSize);
   
  if (pDrvCtxt->aCPUs == DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: Unable to allocate %lu bytes for %lu CPUs.",
      nSize,
      pDrvCtxt->nNumCPUs,0);

    return eResult;
  }

  memset((void *)pDrvCtxt->aCPUs, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Initialize the CPU nodes.                                             */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumCPUs; i++)
  {
    pCPU = &pDrvCtxt->aCPUs[i];

    /*
     * Link the BSP data.`
     */
    pCPU->pBSPConfig = &pBSPConfig->pCPUConfig[i];

    /*
     * Save CPU enum.
     */
    pCPU->eCPU = pCPU->pBSPConfig->eCPU;

    /*
     * Create entry in CPU enum to node map.
     */
    pDrvCtxt->apCPUMap[pCPU->eCPU] = pCPU;

    /*
     * Increment the count for the rail powering this CPU.
     */
    pRail = pDrvCtxt->apRailMap[pCPU->pBSPConfig->eRail];
    if (pRail == NULL)
    {
      return DAL_ERROR_INTERNAL;
    }

    pRail->nNumCPUs++;
    
  }

  /*-----------------------------------------------------------------------*/
  /* Connect the Rails to each of the CPUs which it powers.                */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    pRail = &pDrvCtxt->aRails[i];

    /*
     * Skip this rail if there are no CPU's to map.
     */
    if (pRail->nNumCPUs == 0)
    {
      continue;
    }

    /*
     * Allocate storage for pointers to CPU nodes.
     */
    nSize = pRail->nNumCPUs * sizeof (*pRail->apCPU);

    pRail->apCPU = (VCSCPUNodeType**)malloc(nSize);
    if (pRail->apCPU == NULL)
    {
      ERR_FATAL(
        "DALLOG Device VCS: Unable to allocate %lu bytes for %lu CPUs.",
        nSize,
        pDrvCtxt->nNumCPUs,0);

      return eResult;
    }

    memset((void *)pRail->apCPU, 0x0, nSize);

    /*
     * Iterate through CPU list and store pointers to those on this rail.
     */
    for (j = 0, k = 0; j < pRail->nNumCPUs; j++, k++)
    {
      for ( ; k < pDrvCtxt->nNumCPUs; k++)
      {
        pCPU = &pDrvCtxt->aCPUs[j];
        if (pCPU->pBSPConfig->eRail == pRail->eRail)
        {
          pRail->apCPU[j] = pCPU;
          break;
        }
      }
    }
  }

  return DAL_SUCCESS;

} /* END VCS_InitCPUs */


/* =========================================================================
**  Function : VCS_SetCPUCorner
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_SetCPUCorner
(
  VCSDrvCtxt   *pDrvCtxt,
  ClockCPUType  eCPU,
  VCSCornerType eCornerRequest
)
{
  VCSRailNodeType *pRail;
  VCSCPUNodeType  *pCPU;
  //VCSCornerType    eCornerStart;
  npa_resource    *pRailResource;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (eCPU >= CLOCK_CPU_NUM_OF_CPUS ||
      eCornerRequest >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Verify the CPU is supported.                                          */
  /*-----------------------------------------------------------------------*/

  pCPU = pDrvCtxt->apCPUMap[eCPU];
  if (pCPU == NULL)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  /*-----------------------------------------------------------------------*/
  /* Short circuit if the new request is same as current.                  */
  /*-----------------------------------------------------------------------*/

  if (pCPU->eCorner == eCornerRequest)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Get handle to rail node.                                              */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[pCPU->pBSPConfig->eRail];
  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail with a critical section.                    */
  /*-----------------------------------------------------------------------*/

  pRailResource = pRail->aResources[VCS_NPA_RAIL_RES_IDX_RAIL].handle;
  npa_resource_lock(pRailResource);

  /*-----------------------------------------------------------------------*/
  /* Keep track of the rail corner pre-CPU vote.                           */
  /*-----------------------------------------------------------------------*/

  //eCornerStart = pRail->eCorner;

  /*-----------------------------------------------------------------------*/
  /* Make a corner request with the provided NPA handle.                   */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(pCPU->NPAHandle, eCornerRequest);

  /*-----------------------------------------------------------------------*/
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pCPU->eCorner = eCornerRequest;

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRailResource);

  return DAL_SUCCESS;

} /* END VCS_SetCPUCorner */
