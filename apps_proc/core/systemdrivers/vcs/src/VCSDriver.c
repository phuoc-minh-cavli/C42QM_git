/*
===========================================================================
*/
/**
  @file VCSDriver.c

  Interface implementation file for the VCS device driver.
*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

  ====================================================================

  $Header: //components/rel/core.tx/6.0/systemdrivers/vcs/src/VCSDriver.c#1 $
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

#include "DALDeviceId.h"
#include "DDIVCS.h"
#include "VCSDriver.h"
#include "DDIChipInfo.h"
#include "err.h"

/*=========================================================================
      Data
==========================================================================*/

/*
 * Handle to the VCS DAL.
 */
static DalDeviceHandle *hVCS;
/*
 *
 * Descriptions for each corner.
 *
 * This array needs to get updated whenever a new corner gets added, modified,
 * or removed from the VCS corner enum set in VCSDefs.h.
 * Failure to update both in tandem will result in a runtime fatal error.
 */
static const VCSCornerDescriptionType VCSCornerDescription[] =
{
  {
    VCS_CORNER_OFF,
    RAIL_VOLTAGE_LEVEL_OFF,
    "OFF"
  },
  {
    VCS_CORNER_RETENTION,
    RAIL_VOLTAGE_LEVEL_RETENTION,
    "RETENTION"
  },
  {
    VCS_CORNER_RETENTION_PLUS,
    RAIL_VOLTAGE_LEVEL_RETENTION_HIGH,
    "RETENTION_PLUS"
  },
  {
    VCS_CORNER_LOW_MIN,
    RAIL_VOLTAGE_LEVEL_SVS_MIN,
    "LOW_MIN"
  },
  {
    VCS_CORNER_LOW_MINUS,
    RAIL_VOLTAGE_LEVEL_SVS_LOW,
    "LOW_MINUS"
  },
  {
    VCS_CORNER_LOW,
    RAIL_VOLTAGE_LEVEL_SVS,
    "LOW"
  },
  {
    VCS_CORNER_LOW_PLUS,
    RAIL_VOLTAGE_LEVEL_SVS_HIGH,
    "LOW_PLUS"
  },
  {
    VCS_CORNER_NOMINAL,
    RAIL_VOLTAGE_LEVEL_NOMINAL,
    "NOMINAL"
  },
  {
    VCS_CORNER_NOMINAL_PLUS,
    RAIL_VOLTAGE_LEVEL_NOMINAL_HIGH,
    "NOMINAL_PLUS"
  },
  {
    VCS_CORNER_TURBO,
    RAIL_VOLTAGE_LEVEL_TURBO,
    "TURBO"
  },
};

/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : vcs_init
** =========================================================================*/

void vcs_init
(
  void
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Attach to the VCS DAL.                                                */
  /*-----------------------------------------------------------------------*/

  eResult = DAL_StringDeviceAttach("VCS", &hVCS);

  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: Unable to attach to VCS DAL: %d.", eResult,0,0);
    return;
  }

} /* END vcs_init */

/* =========================================================================
**  Function : vcs_init_thermal
** =========================================================================*/
/*
  Clear out the initial thermal restriction vote on each rail.

  This function gets invoked by the RCINIT framework when the thermal SW
  component has completed initializing.  The agreement with themal SW is that
  by this time it will have issued a voltage vote against each rail to establish
  a voltage corner floor if necessary.

  @param
  None.

  @return
  None.

  @dependencies
  None.
*/

void vcs_init_thermal
(
  void
)
{
  uint32           i;
  VCSRailNodeType *pRail;
  VCSDrvCtxt      *pDrvCtxt;

  pDrvCtxt = VCS_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Remove temporary thermal vote on each rail.                           */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    pRail = &pDrvCtxt->aRails[i];
    if (pRail->pBSPConfig->pNPAActiveFloor !=NULL && pRail->eCornerInitThermal != VCS_CORNER_OFF)
    {
      npa_complete_request(pRail->hClientThermalInit);
    }
  }

} /* END vcs_init_thermal */


/* =========================================================================
**  Function : vcs_cpr_init
** =========================================================================*/

void vcs_cpr_init
(
  void
)
{
  uint32           i;
  VCSRailNodeType *pRail;
  VCSDrvCtxt      *pDrvCtxt;
  npa_resource    *pRailResource;
  /*-----------------------------------------------------------------------*/
  /* Get driver context.                                                   */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = VCS_GetDrvCtxt();
  
  

  /*-----------------------------------------------------------------------*/
  /* Remove CPR disable flag now that CPR initialization has completed.    */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    pRail = &pDrvCtxt->aRails[i];
    pRail->nDisableCPR &= ~VCS_FLAG_DISABLED_BY_CPR;
    pRailResource = pRail->aResources[VCS_NPA_RAIL_RES_IDX_RAIL].handle;
    /*
     * Trigger impulse to invoke CPR if it is now enabled.
     */
    if (!pRail->nDisableCPR)
    {
      /*
       * We update the resource state to 0 in order for the impulse
       * request to make it's way into the driver function.
       * This workaround will be replaced by a new NPA API.
       */
      npa_assign_resource_state(pRailResource, 0);
      npa_issue_impulse_request(pRail->hClientImpulse);
    }
  }

} /* END vcs_cpr_init */


/* =========================================================================
**  Function : VCS_DriverInit
** =========================================================================*/
/**
  Initialize the VCS driver.

  This function initializes the vcs driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_DriverInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult             eResult;
  uint32                i;
  VCSPropertyValueType  PropVal;
  VCSBSPConfigType     *pBSPConfig;

  /*-----------------------------------------------------------------------*/
  /* Initialize the log.                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_GetPropertyValue("VCSLogDefaults", &PropVal);

  if (eResult == DAL_SUCCESS)
  {
    ULogFront_RealTimeInit(
      &pDrvCtxt->hVCSLog,
      "VCS Log",
      ((VCSLogType *)PropVal)->nLogSize,
      ULOG_MEMORY_LOCAL,
      ULOG_LOCK_OS);
  }
  else
  {
    /*
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_WARNING,
      "DALLOG Device VCS: Missing VCS log defaults. Logging is disabled.");
    */
    ULogFront_RealTimeInit(
      &pDrvCtxt->hVCSLog,
      "VCS Log",
      0,
      ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  }

  ULogCore_HeaderSet(pDrvCtxt->hVCSLog, "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Initialize the Rail enum to node map to invalidate all entries.       */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(pDrvCtxt->apRailMap); i++)
  {
    pDrvCtxt->apRailMap[i] = NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the CPU enum to node map to invalidate all entries.        */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(pDrvCtxt->apCPUMap); i++)
  {
    pDrvCtxt->apCPUMap[i] = NULL;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Verify that the corner description array is fully populated.          */
  /*-----------------------------------------------------------------------*/

  if (ARR_SIZE(VCSCornerDescription) != VCS_CORNER_NUM_OF_CORNERS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCSCornerDescription array size doesn't match VCS corner count.",0,0,0);

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize pointer to description of each corner.                     */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pCornerDescription = VCSCornerDescription;

  /*-----------------------------------------------------------------------*/
  /* Initialize the corner string map for debug purposes.                  */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->aszCornerNameMap[VCS_CORNER_OFF]            = "OFF";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_RETENTION]      = "RETENTION";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_RETENTION_PLUS] = "RETENTION_PLUS";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_LOW_MINUS]      = "LOW_MINUS";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_LOW]            = "LOW";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_LOW_PLUS]       = "LOW_PLUS";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_NOMINAL]        = "NOMINAL";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_NOMINAL_PLUS]   = "NOMINAL_PLUS";
  pDrvCtxt->aszCornerNameMap[VCS_CORNER_TURBO]          = "TURBO";
  
  if (ARR_SIZE(VCSCornerDescription) != VCS_CORNER_NUM_OF_CORNERS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCSCornerDescription array size doesn't match VCS corner count.",0,0,0);

    return DAL_ERROR_INTERNAL;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Get the BSP data.                                                     */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_GetPropertyValue("VCSBSPConfig", &PropVal);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCS_GetPropertyValue failed.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the pointer to the BSP config.                                    */
  /*-----------------------------------------------------------------------*/

  pBSPConfig = (VCSBSPConfigType *)PropVal;

  /*-----------------------------------------------------------------------*/
  /* Initialize the rails.                                                 */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitRails(pDrvCtxt, pBSPConfig);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCS_InitRails failed.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the CPUs.                                                  */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitCPUs(pDrvCtxt, pBSPConfig);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: Failed to initialize CPUs.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver.                                 */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitImage(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCS_InitImage failed.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize NPA resources for each rail, CPU, and if applicable.       */
  /*                                                                       */
  /* Create NPA resource node for this rail. The resource name, initial    */
  /* state, and dependencies are defined in the BSP.                       */
  /*                                                                       */
  /* Note: This part needs to happen after the 'set rail corner' function  */
  /*       pointers have been installed, because node creation can happen  */
  /*       right away and trigger the rail's driver function.              */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitNPAResources(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCS_InitRailNPAResources failed.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_DriverInit */


/* =========================================================================
**  Function : VCSStub_DriverInit
** =========================================================================*/
/**
  Initialize the VCS driver stub.

  This function initializes the VCS driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCSStub_DriverInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver stub.                            */
  /*-----------------------------------------------------------------------*/

  eResult = VCSStub_InitImage(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "DALLOG Device VCS: VCSStub_InitImage failed.",0,0,0);

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCSStub_DriverInit */


/* =========================================================================
**  Function : VCS_DriverDeInit
** =========================================================================*/
/**
  De-initialize the VCS driver.

  This function shuts-down the VCS driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult VCS_DriverDeInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Shut-down complete, nothing to do for now.                            */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_DriverDeInit */


/* =========================================================================
**  Function : VCSStub_DriverDeInit
** =========================================================================*/
/**
  De-initialize the VCS driver stub.

  This function shuts-down the VCS driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult VCSStub_DriverDeInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Shut-down complete, nothing to do for now.                            */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCSStub_DriverDeInit */


/*=========================================================================
**  Function : VCS_IsBSPSupported
** =========================================================================*/
/**
 *
  See VCSDriver.h
*/

boolean VCS_IsBSPSupported
(
  VCSHWVersionType *pHWVersion
)
{
  uint32                   nHWVersion, nHWMaxVersion, nHWMinVersion;
  DalChipInfoIdType        eChipId;
  const DalChipInfoIdType *aeChipInfoId;

  /*-----------------------------------------------------------------------*/
  /* Validate argument pointer is usable.                                  */
  /*-----------------------------------------------------------------------*/

  if (pHWVersion == NULL)
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate chip family. If not provided then we do not check.           */
  /*-----------------------------------------------------------------------*/

  if ( (pHWVersion->eChipInfoFamily != 0) &&
       (pHWVersion->eChipInfoFamily != DalChipInfo_ChipFamily()) )
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Ensure the specific chip ID is supported.                             */
  /*-----------------------------------------------------------------------*/

  aeChipInfoId = pHWVersion->aeChipInfoId;

  if (aeChipInfoId != NULL)
  {
    eChipId = DalChipInfo_ChipId();

    while (*aeChipInfoId < DALCHIPINFO_NUM_IDS)
    {
      if (eChipId == *aeChipInfoId)
      {
        break;
      }

      aeChipInfoId++;
    }

    if (*aeChipInfoId == DALCHIPINFO_NUM_IDS)
    {
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Get the actual hardware revision.                                     */
  /*-----------------------------------------------------------------------*/

  nHWVersion = DalChipInfo_ChipVersion();

  nHWMinVersion =
    DALCHIPINFO_VERSION(pHWVersion->Min.nMajor, pHWVersion->Min.nMinor);
  nHWMaxVersion =
    DALCHIPINFO_VERSION(pHWVersion->Max.nMajor, pHWVersion->Max.nMinor);

  /*-----------------------------------------------------------------------*/
  /* No chip version data implies support in all HW versions.              */
  /*-----------------------------------------------------------------------*/

  if ( (nHWMinVersion == 0) && (nHWMaxVersion == 0) )
  {
    return TRUE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the HW version falls within the min/max.                     */
  /*-----------------------------------------------------------------------*/

  if ( (nHWVersion >= nHWMinVersion) && (nHWVersion < nHWMaxVersion) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}

