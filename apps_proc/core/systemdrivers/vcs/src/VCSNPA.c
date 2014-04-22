/*
===========================================================================
*/
/**
  @file VCSNPA.c 
  
  NPA-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

  ==================================================================== 
  $Header: //components/rel/core.tx/6.0/systemdrivers/vcs/src/VCSNPA.c#1 $
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

#include "comdef.h"
#include "DDIPlatformInfo.h"
#include "VCSDriver.h"
#include "pmapp_npa.h"
#include "DALDeviceId.h"
#include <DALSys.h>
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>


/*=========================================================================
      Macros
==========================================================================*/


#define VCS_NPA_UNITS_RAIL "VReg Corner"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : VCS_NPAVoteDependency
** =========================================================================*/
/**
  Issue an NPA request on the rail resource dependency.

  This function makes an NPA request on a specific rail dependency resource.

  @param *pRail [in] -- Pointer to rail node.
  @param  hClientFrom [in] -- Client handle used by client to send the initial request.
  @param  hClientTo [in] -- Client handle used by VCS to satisfy the dependency.
  @param  eCorner [in] -- Corner vote.

  @dependencies
  None.
*/

static void VCS_NPAVoteDependency
(
  VCSRailNodeType   *pRail,
  npa_client_handle  hClient,
  VCSCornerType      eCorner
)
{
  VCSDrvCtxt    *pDrvCtxt;
  VCSCornerType  eCornerAdjusted = eCorner;
  uint32         nCornerVote;
  /*-----------------------------------------------------------------------*/
  /* Bring votes up to within the supported range if vote is not OFF.      */
  /*-----------------------------------------------------------------------*/

  if (eCorner != VCS_CORNER_OFF)
  {
    eCornerAdjusted = MAX(eCorner, pRail->eCornerMin);

    /*
     * Validate eCornerAdjusted
     */
    if(eCornerAdjusted >= VCS_CORNER_NUM_OF_CORNERS)
    {
      ERR_FATAL(
        "DALLOG Device VCS: Invalid corner[%lu] on rail[%s]",
        eCornerAdjusted,
        pRail->pBSPConfig->szName,0);

      return;
    }

    if (pRail->pVoltageTable)
    {
      for ( ; eCornerAdjusted <= pRail->eCornerMax; eCornerAdjusted++)
      {
    	if (pRail->pVoltageTable->apCornerMap[eCornerAdjusted] != NULL)
        {
          break;
        }
      }

      /*
       * Validate eCornerAdjusted again in the voltage table.
       */
      if (eCornerAdjusted > pRail->eCornerMax)
      {
        ERR_FATAL(
          "DALLOG Device VCS: Request[%lu] on rail[%s] above max corner[%lu]",
          eCornerAdjusted,
          pRail->pBSPConfig->szName,
          pRail->eCornerMax);

        return;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Get the driver context.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = VCS_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Set request attribute to fire-and-forget if we are setting a lower    */
  /* voltage corner.                                                       */
  /*-----------------------------------------------------------------------*/

  if(eCornerAdjusted < pRail->eCorner)
  {
    npa_set_request_attribute(hClient, NPA_REQUEST_FIRE_AND_FORGET);
  }

  /*-----------------------------------------------------------------------*/
  /* Issue the request to satisfy the dependency.                          */
  /* Map the vote to VLVL if the vote is going to RPM (not local).         */
  /*-----------------------------------------------------------------------*/

  if (pRail->pBSPConfig->pNPADependency->NPARemoteDefinition.szName != NULL)
  {
    nCornerVote = pDrvCtxt->pCornerDescription[eCornerAdjusted].eCornerVLVL;
  }
  else
  {
    nCornerVote = eCornerAdjusted;
  }

  npa_issue_scalar_request(hClient, nCornerVote);

} /* END of VCS_NPAVoteDependency */


/* =========================================================================
**  Function : VCS_NPASatisfyRailDependencies
** =========================================================================*/
/**
  Issue NPA requests on the rail's resource dependencies.

  This function makes NPA requests on the rail's resource dependencies.

  @param *pRail [in] -- Pointer to rail node.
  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in] -- Client handle.

  @dependencies
  None.
*/

static void VCS_NPASatisfyRailDependencies
(
  VCSRailNodeType   *pRail,
  npa_resource      *pResource,
  npa_client_handle  hClient
)
{
  VCSCornerType     eCornerRequired, eCornerSuppressible;
  npa_client_handle hClientRequired, hClientSuppressible;

  /*-----------------------------------------------------------------------*/
  /* Determine which client handle to use and which value to send.         */
  /*-----------------------------------------------------------------------*/

  eCornerSuppressible = (VCSCornerType)NPA_RESOURCE_SUPPRESSIBLE_REQUEST(pResource);
  eCornerRequired = (VCSCornerType)NPA_RESOURCE_REQUIRED_REQUEST(pResource);

  hClientSuppressible = pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_SUP].handle;
  hClientRequired = pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_REQ].handle;

  if (hClient->type == NPA_CLIENT_SUPPRESSIBLE)
  {
    VCS_NPAVoteDependency(pRail, hClientSuppressible, eCornerSuppressible);
  }
  
  else if (hClient->type == NPA_CLIENT_REQUIRED)
  {
    VCS_NPAVoteDependency(pRail, hClientRequired, eCornerRequired);
  }
  else if (hClient->type == NPA_CLIENT_IMPULSE)
  {
    VCS_NPAVoteDependency(pRail, hClientSuppressible, eCornerSuppressible);
    VCS_NPAVoteDependency(pRail, hClientRequired, eCornerRequired);
  }

} /* END of VCS_NPASatisfyRailDependencies */

/* =========================================================================
**  Function : VCS_NPARailResourceQuery
** =========================================================================*/
/**
  NPA rail resource query function.

  This function is called to get the following rail information:
    -- Number of corners.
    -- Voltage table for each corner.
    -- Current corner (id / table index).

  @param *pResource  [in]  -- Pointer to the resource in question
  @param  nID        [in]  -- ID of the query.
  @param *pResult    [out] -- Pointer to the data to be filled by this function.

  @return
  npa_query_status - NPA_QUERY_SUCCESS, if query supported.
                   - NPA_QUERY_UNSUPPORTED_QUERY_ID, if query not supported.

  @dependencies
  None.
*/

static npa_query_status VCS_NPARailResourceQuery
(
  npa_resource   *pResource,
  unsigned int    nID,
  npa_query_type *pResult
)
{
  npa_query_status           nStatus = NPA_QUERY_SUCCESS;
  uint32                     nNumCorners, nCorner;
  VCSCornerVoltageRangeType *pVoltageCorner;
  VCSRailNodeType           *pRail = (VCSRailNodeType *)pResource->node->data;

  /*-----------------------------------------------------------------------*/
  /* Validate parameters.                                                  */
  /*-----------------------------------------------------------------------*/

  if (pResource == NULL || pResult == NULL || pRail == NULL)
  {
    return NPA_QUERY_NULL_POINTER;
  }

  /*-----------------------------------------------------------------------*/
  /* Check for the current corner.                                         */
  /*-----------------------------------------------------------------------*/

  if(nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID)
  {
    /*
     * Store current corner request.
     */
    pResult->data.value = pRail->eCorner;
    pResult->type = NPA_QUERY_TYPE_VALUE;
	
    return NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Only process voltage related queries if we have a voltage table.      */
  /*-----------------------------------------------------------------------*/

  if (pRail->pVoltageTable == NULL)
  {
    return NPA_QUERY_UNSUPPORTED_QUERY_ID;
  }

  if (nID == VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES)
  {
    /*
     * Store the number of entries in the corner voltage table.
     */
    pResult->data.value = pRail->pVoltageTable->nNumVoltageRanges;
    pResult->type = NPA_QUERY_TYPE_VALUE;
  }
  else if(nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV)
  {
    /*
     * Store current voltage request.
     */
    pResult->data.value = pRail->nVoltageUV;
    pResult->type = NPA_QUERY_TYPE_VALUE;
  }
  else if (nID >= VCS_NPA_QUERY_VOLTAGE_LEVEL_ID)
  {
    nNumCorners = pRail->pVoltageTable->nNumVoltageRanges;

    if (nID < VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR)
    {
      nCorner = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_ID;
      if (nCorner >= nNumCorners)
      {
        nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      }
      else
      {
        pVoltageCorner = &pRail->pVoltageTable->pVoltageRange[nCorner];
        pResult->data.value = pVoltageCorner->eCorner;
        pResult->type = NPA_QUERY_TYPE_VALUE;
      }
    }
    else if (nID < VCS_NPA_QUERY_VOLTAGE_LEVEL_CEILING)
    {
      nCorner = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR;
      if (nCorner >= nNumCorners)
      {
        nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      }
      else
      {
        pVoltageCorner = &pRail->pVoltageTable->pVoltageRange[nCorner];
        pResult->data.value = pVoltageCorner->nMinUV;
        pResult->type = NPA_QUERY_TYPE_VALUE;
      }
    }
    else
    {
      nCorner = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_CEILING;
      if (nCorner >= nNumCorners)
      {
        nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      }
      else
      {
        pVoltageCorner = &pRail->pVoltageTable->pVoltageRange[nCorner];
        pResult->data.value = pVoltageCorner->nMaxUV;
        pResult->type = NPA_QUERY_TYPE_VALUE;
      }
    }
  }
  else
  {
    nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  }

  return nStatus;

} /* END VCS_NPARailResourceQuery */


/* =========================================================================
**  Function : VCS_NPARailResourceDriverFunc
** =========================================================================*/
/**
  Handle request state changes on a rail's NPA resource.

  This is a generic NPA driver function applicable to all rail resources.
  This function does some common record keeping, and makes use of function
  pointers to invoke rail-specific functions to actually set the voltage.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in] -- Client handle.
  @param  nState [in] -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPARailResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  VCSRailNodeType         *pRail;
  DALResult                eResult;
  VCSDrvCtxt              *pDrvCtxt;
  VCSNPARailEventDataType  RailEventData;
  VCSCornerType            eCornerAggregated;

  pRail = (VCSRailNodeType *)pResource->node->data;
  eCornerAggregated = (VCSCornerType)nState;

  /*-----------------------------------------------------------------------*/
  /* Validate the max rail voltage corner before proceeding                */
  /*-----------------------------------------------------------------------*/

  if(pRail->eCornerMax >= VCS_CORNER_NUM_OF_CORNERS)
  {
    ERR_FATAL(
    "DALLOG Device VCS: Invalid max corner [%lu] on rail[%s]",
    pRail->eCornerMax,
    pRail->pBSPConfig->szName,0);

    return (npa_resource_state)pRail->eCorner;
  }

  /*-----------------------------------------------------------------------*/
  /* Do nothing for init.                                                  */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    return pRail->eCornerInit;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the driver context.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = VCS_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* If any non-HW disable DVS bit is set then set the rail to max.        */
  /*-----------------------------------------------------------------------*/

  if (pRail->nDisableDVS & ~VCS_FLAG_DISABLED_HW_LOGIC)
  {
    eCornerAggregated = pRail->eCornerMax;
  }

  /*-----------------------------------------------------------------------*/
  /* Error if the voltage corner exceeds the current max corner.           */
  /*-----------------------------------------------------------------------*/

  if (eCornerAggregated > pRail->eCornerMax)
  {
    ERR_FATAL(
      "DALLOG Device VCS: Request[%lu] on rail[%s] above max corner[%lu]",
      nState,
      pRail->pBSPConfig->szName,
      pRail->eCornerMax);

    return (npa_resource_state)pRail->eCorner;
  }

  if (eCornerAggregated != VCS_CORNER_OFF)
  {
    /*
     * Round the request up within the supported range.
     */
  eCornerAggregated = MAX(eCornerAggregated, pRail->eCornerMin);

    /*
     * Validate the corner
     */
    if(eCornerAggregated >= VCS_CORNER_NUM_OF_CORNERS)
    {
     ERR_FATAL(
        "DALLOG Device VCS: Invalid corner [%lu] on rail[%s]",
        eCornerAggregated,
        pRail->pBSPConfig->szName,0);

      return (npa_resource_state)pRail->eCorner;
    }

    /*
     * If a voltage table is present then find the min supported corner that
     * is greater than a equal to the request state.
     */
  if (pRail->pVoltageTable)
  {
    for ( ; eCornerAggregated <= pRail->eCornerMax; eCornerAggregated++)
    {
      if (pRail->pVoltageTable->apCornerMap[eCornerAggregated] != NULL)
      {
        break;
      }
    }

    /*
     * Validate that we found a usable entry in the voltage table.
     */
    if (eCornerAggregated > pRail->eCornerMax)
    {
      ERR_FATAL(
        "DALLOG Device VCS: Request[%lu] on rail[%s] above max corner[%lu]",
        nState,
        pRail->pBSPConfig->szName,
        pRail->eCornerMax);

      return (npa_resource_state)pRail->eCorner;
    }
  }
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where request on rail increased.                      */
  /*-----------------------------------------------------------------------*/

  if (eCornerAggregated > pResource->active_state)
  {
    /*
     * Fill out event data for rail state change.
     */
    RailEventData.PreChange.eCorner  = pRail->eCorner;
    RailEventData.PostChange.eCorner = eCornerAggregated;

    /*
     * Notify NPA clients of pre-switch event.
     */
    npa_dispatch_custom_events(
      pResource,
      (npa_event_type)VCS_NPA_RAIL_EVENT_PRE_CHANGE,
      &RailEventData);

    /*
     * Satisfy rail dependency.
     */
    VCS_NPASatisfyRailDependencies(pRail, pResource, hClient);

    /*
     * Increase the rail voltage to requested corner.
     */
    if (pRail->fpSetRailCorner != NULL)
    {
      eResult = pRail->fpSetRailCorner(pRail, eCornerAggregated);
      if (eResult != DAL_SUCCESS)
      {
        ERR_FATAL(
          "DALLOG Device VCS: Unable to set rail[%s] to voltage[%lu]",
          pRail->pBSPConfig->szName,
          eCornerAggregated,0);

        return (npa_resource_state)pRail->eCorner;
      }
    }

    /*
     * Notify NPA clients of post-switch event.
     */
    npa_dispatch_custom_events(
      pResource,
      (npa_event_type)VCS_NPA_RAIL_EVENT_POST_CHANGE,
      &RailEventData);
 
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where request on rail decreased.                      */
  /*-----------------------------------------------------------------------*/

  else if (eCornerAggregated < pResource->active_state)
  { 
    /*
     * Fill out event data for rail state change.
     */
    RailEventData.PreChange.eCorner  = pRail->eCorner;
    RailEventData.PostChange.eCorner = eCornerAggregated;

    /*
     * Notify NPA clients of pre-switch event.
     */
    npa_dispatch_custom_events(
      pResource,
      (npa_event_type)VCS_NPA_RAIL_EVENT_PRE_CHANGE,
      &RailEventData);

    /*
     * Decrease the rail voltage to requested corner.
     */
    if (pRail->fpSetRailCorner != NULL)
    {
      eResult = pRail->fpSetRailCorner(pRail, eCornerAggregated);
      if (eResult != DAL_SUCCESS)
      {
        ERR_FATAL(
          "DALLOG Device VCS: Unable to set rail[%s] to voltage[%lu]",
          pRail->pBSPConfig->szName,
          eCornerAggregated,0);

        return (npa_resource_state)pRail->eCorner;
      }
    }

    /*
     * Satisfy rail dependency.
     */
    VCS_NPASatisfyRailDependencies(pRail, pResource, hClient);

    /*
     * Notify NPA clients of post-switch event.
     */
    npa_dispatch_custom_events(
      pResource,
      (npa_event_type)VCS_NPA_RAIL_EVENT_POST_CHANGE,
      &RailEventData);
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where an impulse wants to force a voltage update.     */
  /*-----------------------------------------------------------------------*/

  else if (hClient->type == NPA_CLIENT_IMPULSE)
  {
    if (pRail->fpSetRailCorner != NULL)
    {
      eResult = pRail->fpSetRailCorner(pRail, eCornerAggregated);
      if (eResult != DAL_SUCCESS)
      {
        ERR_FATAL(
          "DALLOG Device VCS: Unable to set rail[%s] to voltage[%lu]",
          pRail->pBSPConfig->szName,
          eCornerAggregated,0);

        return (npa_resource_state)pRail->eCorner;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where the overall aggregated state didn't change but  */
  /* the aggregated votes for either required or suppressible alone have   */
  /* changed and thus we must update the dependency accordingly.           */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * Satisfy rail dependency.
     */
    VCS_NPASatisfyRailDependencies(pRail, pResource, hClient);
  }

  /*-----------------------------------------------------------------------*/
  /* Log the voltage corner switch.                                        */
  /*-----------------------------------------------------------------------*/

  if (eCornerAggregated != pRail->eCorner)
  {
    if (pRail->nVoltageUV)
    {
      ULOG_RT_PRINTF_4(
        pDrvCtxt->hVCSLog,
        "Rail[%s] Corner[%s -> %s] Voltage[%lu]",
        pRail->pBSPConfig->szName,
        pDrvCtxt->aszCornerNameMap[pRail->eCorner],
        pDrvCtxt->aszCornerNameMap[eCornerAggregated],
        pRail->nVoltageUV);
    }
    else
    {
      ULOG_RT_PRINTF_3(
        pDrvCtxt->hVCSLog,
        "Rail[%s] Corner[%s -> %s]",
        pRail->pBSPConfig->szName,
        pDrvCtxt->aszCornerNameMap[pRail->eCorner],
        pDrvCtxt->aszCornerNameMap[eCornerAggregated]);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update the context.                                                   */
  /*-----------------------------------------------------------------------*/

  pRail->eCorner = eCornerAggregated;

  /*-----------------------------------------------------------------------*/
  /* Return the current rail corner.                                       */
  /*-----------------------------------------------------------------------*/

  return (npa_resource_state)pRail->eCorner;

} /* END VCS_NPARailDriverFunc */

/* =========================================================================
**  Function : VCS_NPARailActiveFloorResourceDriverFunc
** =========================================================================*/
/**
  Handle request state changes on a rail's active floor NPA resource.

  This is a generic NPA driver function applicable to all rail resources.
  This function will update the active floor for the requested rail and
  send an IMPULSE request to the rail driver function if necessary.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in] -- Client handle.
  @param  nState [in] -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPARailActiveFloorResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{

  VCSRailNodeType *pRail;

  pRail = (VCSRailNodeType *)pResource->node->data;

  /*-----------------------------------------------------------------------*/
  /* Do nothing for init.                                                  */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Save the initial min corner.
     */
    pRail->eCornerMinSaved = pRail->eCornerMin;

    return pRail->eCornerMin;
  }

  /*-----------------------------------------------------------------------*/
  /* Error if the voltage corner exceeds the current max corner.           */
  /*-----------------------------------------------------------------------*/

  if (nState > pRail->eCornerMax)
  {
    ERR_FATAL(
      "DALLOG Device VCS: Active floor request[%lu] on rail[%s] above max corner[%lu]",
      nState,
      pRail->pBSPConfig->szName,
      pRail->eCornerMax);

    return (npa_resource_state)pRail->eCornerMin;
  }

  /*-----------------------------------------------------------------------*/
  /* The new active floor corner is the greater between the resource vote  */
  /* and the original active floor.                                        */
  /*-----------------------------------------------------------------------*/

  pRail->eCornerMin = (VCSCornerType)MAX(pRail->eCornerMinSaved, nState);

  /*-----------------------------------------------------------------------*/
  /* Trigger impulse on rail to account for new active floor.              */
  /*-----------------------------------------------------------------------*/

  npa_issue_impulse_request(pRail->hClientImpulse);

  return (npa_resource_state)pRail->eCornerMin;

} /* END VCS_NPARailActiveFloorResourceDriverFunc */

/* =========================================================================
**  Function : VCS_NPARailNodeDriverFunc
** =========================================================================*/
/**
  Handle request for a state change on a rail's resource.

  This is a generic NPA driver function applicable to all rail resources.
  This function will update the requested rail's resource and
  send an IMPULSE request to the rail driver function if necessary.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in] -- Client handle.
  @param  nState [in] -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPARailNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  VCSRailNodeType    *pRail;
  npa_resource_state  nActiveState;
  npa_resource       *pRailResource;
  npa_resource       *pRailActiveFloorResource;

  pRail = (VCSRailNodeType *)pResource->node->data;
  pRailResource =
    pRail->aResources[VCS_NPA_RAIL_RES_IDX_RAIL].handle;
  pRailActiveFloorResource =
    pRail->aResources[VCS_NPA_RAIL_RES_IDX_ACTIVE_FLOOR].handle;
  nActiveState = 0;

  if (pResource == pRailResource)
  {
    nActiveState =
      VCS_NPARailResourceDriverFunc(
        pResource,
        hClient,
        nState);
  }
  else if (pResource == pRailActiveFloorResource)
  {
    nActiveState =
      VCS_NPARailActiveFloorResourceDriverFunc(
        pResource,
        hClient,
        nState);
  }
  else
  {
    ERR_FATAL(
      "DALLOG Device VCS: Unable to process NPA request for rail[%s]",
	  pRail->pBSPConfig->szName,0,0);
  }

  return nActiveState;

} /* END VCS_NPARailNodeDriverFunc */

/* =========================================================================
**  Function : VCS_NPARailNodeAvailableCallback
** =========================================================================*/
/**
  Callback after a rail node is created.

  This function is called by the NPA framework after a rail node
  is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext     [in] -- Context passed in npa_define_node_cb
  @param nEventType    [in] -- Zero.
  @param *pNodeName    [in] -- Name of the node being created.
  @param nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void VCS_NPARailNodeAvailableCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pNodeName,
  unsigned int  nNodeNameSize
)
{
  uint32           i;
  VCSCPUNodeType  *pCPU;
  VCSRailNodeType *pRail;
  npa_resource_definition *pRailActiveFloorResourceDef;

  pRail = (VCSRailNodeType *)pContext;
  pRailActiveFloorResourceDef =
    &pRail->aResources[VCS_NPA_RAIL_RES_IDX_ACTIVE_FLOOR];
  /*-----------------------------------------------------------------------*/
  /* Iterate through list of CPUs on this rail.                            */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pRail->nNumCPUs; i++)
  {
    /*
     * Create a suppressible NPA handle for each CPU to vote on this rail.
     */
    pCPU = pRail->apCPU[i];
    pCPU->NPAHandle =
      npa_create_sync_client(
        pRail->hNode.name, pCPU->pBSPConfig->szName, NPA_CLIENT_SUPPRESSIBLE);

    if (pCPU->NPAHandle == NULL)
    {
      ERR_FATAL(
        "DALLOG Device VCS: Failed to create a NPA client handle on rail for CPU.",0,0,0);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Create an impulse client for this rail node.                          */
  /*-----------------------------------------------------------------------*/

  pRail->hClientImpulse =
    npa_create_sync_client(
      pRail->pBSPConfig->szName,
      pRail->pBSPConfig->szName,
      NPA_CLIENT_IMPULSE);
  if (pRail->hClientImpulse == NULL)
  {
    ERR_FATAL(
      "DALLOG Device VCS: Unable to create impulse client to rail[%s]",
      pRail->pBSPConfig->szName,0,0);
  }
/*-----------------------------------------------------------------------*/
  /* Create a temp thermal init client and vote based on the BSP setting.  */
  /*-----------------------------------------------------------------------*/

  if (pRail->eCornerInitThermal != VCS_CORNER_OFF  && pRailActiveFloorResourceDef != NULL)
  {
    pRail->hClientThermalInit =
      npa_create_sync_client(
        pRailActiveFloorResourceDef->name,
        "VCS Thermal Init",
        NPA_CLIENT_REQUIRED);
    if (pRail->hClientThermalInit == NULL)
    {
      ERR_FATAL(
        "DALLOG Device VCS: Failed to create required client for temporary thermal init on rail[%s].",
        pRailActiveFloorResourceDef->name,0,0);
    }

    /*
     * Issue temporary thermal vote.
     */
    npa_issue_required_request(
      pRail->hClientThermalInit,
      pRail->eCornerInitThermal);
  }

} /* END of VCS_NPARailNodeAvailableCallback */


DALResult VCS_InitNPAResourceRail
(
  VCSRailNodeType *pRail
)
{
  npa_resource_state              anInitialState[VCS_NPA_RAIL_NUM_RESOURCES];
  npa_node_dependency            *pDependency;
  npa_remote_resource_definition *pRemoteResource;
  npa_resource_definition        *pRailResource;
  uint32                          nNumResources;
  /*-----------------------------------------------------------------------*/
  /* Validate pointers.                                                    */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Populate resource.                                                    */
  /*-----------------------------------------------------------------------*/

  nNumResources = 0;

  pRailResource = &pRail->aResources[VCS_NPA_RAIL_RES_IDX_RAIL];
  pRailResource->name       = pRail->pBSPConfig->szName;
  pRailResource->units      = VCS_NPA_UNITS_RAIL;
  pRailResource->max        = VCS_CORNER_MAX;
  pRailResource->plugin     = &npa_max_plugin;
  pRailResource->attributes = NPA_RESOURCE_DEFAULT;
  pRailResource->data       = (npa_user_data)pRail;
  pRailResource->query_fcn  = VCS_NPARailResourceQuery;

  anInitialState[VCS_NPA_RAIL_RES_IDX_RAIL] =
    (npa_resource_state)pRail->eCorner;

  nNumResources++;

  /*-----------------------------------------------------------------------*/
  /* Populate local rail active floor resource.                            */
  /*-----------------------------------------------------------------------*/
 if(pRail->pBSPConfig->pNPAActiveFloor != NULL)
 {
  if (pRail->pBSPConfig->pNPAActiveFloor->NPARemoteDefinition.szName == NULL)
  {
    pRailResource = &pRail->aResources[VCS_NPA_RAIL_RES_IDX_ACTIVE_FLOOR];
    pRailResource->name       = pRail->pBSPConfig->pNPAActiveFloor->szName;
    pRailResource->units      = VCS_NPA_UNITS_RAIL;
    pRailResource->max        = VCS_CORNER_MAX;
    pRailResource->plugin     = &npa_max_plugin;
    pRailResource->attributes = NPA_RESOURCE_DEFAULT |
                                NPA_RESOURCE_IMPULSE_UNCONDITIONAL;
    pRailResource->data       = (npa_user_data)pRail;
    pRailResource->query_fcn  = NULL;

    anInitialState[VCS_NPA_RAIL_RES_IDX_ACTIVE_FLOOR] =
      (npa_resource_state)pRail->eCornerMin;

    nNumResources++;
  }

  /*-----------------------------------------------------------------------*/
  /* Create the remote active floor resource.                              */
  /*-----------------------------------------------------------------------*/

  else
  {
    pRemoteResource =
      &pRail->aRemoteResources[VCS_NPA_RAIL_REM_RES_IDX_ACTIVE_FLOOR];
    pRemoteResource->local_resource_name =
      (char *)pRail->pBSPConfig->pNPAActiveFloor->szName;
    pRemoteResource->remote_resource_name =
      (char *)pRail->pBSPConfig->pNPAActiveFloor->NPARemoteDefinition.szName;
    pRemoteResource->protocol_type = "/protocol/rpm/rpm";
    pRemoteResource->plugin = &npa_max_plugin,
    pRemoteResource->driver_fcn =
      npa_remote_resource_local_aggregation_driver_fcn;
    pRemoteResource->units =
      pRail->pBSPConfig->pNPAActiveFloor->NPARemoteDefinition.szUnits;
    pRemoteResource->max = NPA_MAX_STATE;
    pRemoteResource->attributes = NPA_RESOURCE_REMOTE_NO_INIT;

    npa_remote_define_resource(pRemoteResource, 0, NULL);
  }
}
  /*-----------------------------------------------------------------------*/
  /* Create remote dependency resource.                                    */
  /*-----------------------------------------------------------------------*/
 
  
  if (pRail->pBSPConfig->pNPADependency->NPARemoteDefinition.szName != NULL)
  {
    pRemoteResource =
      &pRail->aRemoteResources[VCS_NPA_RAIL_REM_RES_IDX_DEPENDENCY];
    pRemoteResource->local_resource_name =
      (char *)pRail->pBSPConfig->pNPADependency->szName;
    pRemoteResource->remote_resource_name =
      (char *)pRail->pBSPConfig->pNPADependency->NPARemoteDefinition.szName;
    pRemoteResource->protocol_type = "/protocol/rpm/rpm";
    pRemoteResource->plugin = &npa_max_plugin,
    pRemoteResource->driver_fcn =
      npa_remote_resource_local_aggregation_driver_fcn;
    pRemoteResource->units =
      pRail->pBSPConfig->pNPADependency->NPARemoteDefinition.szUnits;
    pRemoteResource->max = NPA_MAX_STATE;
    pRemoteResource->attributes = NPA_RESOURCE_REMOTE_NO_INIT;
	
    npa_remote_define_resource(pRemoteResource, 0, NULL);
  }

  /*-----------------------------------------------------------------------*/
  /* Create a dependency client for each type supported by NPA.            */
  /*-----------------------------------------------------------------------*/

  pDependency = &pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_REQ];
  pDependency->name = pRail->pBSPConfig->pNPADependency->szName;
  pDependency->client_type = NPA_CLIENT_REQUIRED;

  pDependency = &pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_SUP];
  pDependency->name = pRail->pBSPConfig->pNPADependency->szName;
  pDependency->client_type = NPA_CLIENT_SUPPRESSIBLE;
  
  

  /*-----------------------------------------------------------------------*/
  /* Populate node information.                                            */
  /*-----------------------------------------------------------------------*/

  pRail->hNode.name             = pRail->pBSPConfig->szName;
  pRail->hNode.driver_fcn       = VCS_NPARailNodeDriverFunc;
  pRail->hNode.attributes       = NPA_NODE_DEFAULT;
  pRail->hNode.data             = (npa_user_data)pRail;
  pRail->hNode.dependency_count = VCS_NPA_RAIL_NUM_DEPENDENCIES;
  pRail->hNode.dependencies     = pRail->aDependency;
  pRail->hNode.resource_count   = nNumResources;
  pRail->hNode.resources        = pRail->aResources;
  
/*-----------------------------------------------------------------------*/
  /* Define the rail NPA node.                                             */
  /*-----------------------------------------------------------------------*/
  npa_define_node_cb(
    &pRail->hNode,
    anInitialState,
    VCS_NPARailNodeAvailableCallback,
    pRail);

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_InitNPAResourceRail */


/* =========================================================================
**  Function : VCS_InitNPAResources
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitNPAResources
(
  VCSDrvCtxt *pDrvCtxt
)
{
  uint32           i;
  DALResult        eResult;

  /*-----------------------------------------------------------------------*/
  /* Iterate through all the rails to define rail resources.               */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    eResult = VCS_InitNPAResourceRail(&pDrvCtxt->aRails[i]);
    if (eResult != DAL_SUCCESS)
    {
      ERR_FATAL(
        "DALLOG Device VCS: Failed to create NPA resource for rail.",0,0,0);

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_InitNPAResources */

/* =========================================================================
**  Function : VCS_InitThermal
** =========================================================================*/
/*
  Clear out the initial thermal restriction vote on each rail.

  This function gets invoked by the RCINIT framework when the thermal SW
  component has completed initializing.  The agreement with themal SW is that
  by this time it will have issued a voltage vote against all rails to establish
  a voltage corner floor if necessary.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/

void VCS_InitThermal
(
  void
)
{
  VCSDrvCtxt      *pDrvCtxt;
  VCSRailNodeType *pRail;
  uint32           nCount;

  pDrvCtxt = VCS_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Remove temporary thermal vote on each rail.                           */
  /*-----------------------------------------------------------------------*/

  for (nCount = 0; nCount < pDrvCtxt->nNumRails; nCount++)
  {
    pRail = &pDrvCtxt->aRails[nCount];
    if (pRail->hClientThermalInit)
    {
      npa_complete_request(pRail->hClientThermalInit);
    }
  }

} /* END VCS_InitThermal */
