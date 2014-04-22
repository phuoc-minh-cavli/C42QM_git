/** @file ClockUEFI.c

  This file implements specialized image specific functions such
  as init image, enable DCVS, etc. In case of UEFI, we have few
  place holders for now.

  Copyright (c) 2010-2016,2018 Qualcomm Technologies, Inc.
                   All rights reserved.
                 QUALCOMM Proprietary/GTDR

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/30/14   sj      Fixed 64 bit compilation errors
 05/08/14   sr      Ported to 8994.
 08/09/12   sr      Ported to 8974.
 12/23/10   vs      Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DDIClock.h"
#include "Drivers/ClockDxe/ClockDriver.h"
#include "ClockApps.h"
#include "ClockAVS.h"
#include <Protocol/EFIPlatformInfoTypes.h>

#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>
#include "DDIChipInfo.h"
/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Externals
==========================================================================*/

extern void HAL_clk_MemCoreSave (void);
extern void HAL_clk_MemCoreRestore (void);
extern void HAL_clk_NocDcdEnable (void);
extern void HAL_clk_NocDcdDisable (void);


/*=========================================================================
      Prototypes
==========================================================================*/

void Clock_VDDCXNodeReadyCallback(void *pContext, unsigned int nEventType, void *pNodeName, unsigned int  nNodeNameSize);
void Clock_VDDMXNodeReadyCallback(void *pContext, unsigned int nEventType, void *pNodeName, unsigned int  nNodeNameSize);


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * UEFI Clock Driver local context.
 */
typedef struct
{
  boolean        bLowPowerMode;
  boolean        bClusterGoldInReset;
  ClockNodeType *ClusterSilverId;
  ClockNodeType *ClusterGoldId;
  ClockNodeType *MDSSMDPClockId;
} ClockLocalCtxtType;


/**
 * UEFI Clock Driver NPA context.
 */
typedef struct
{
  boolean                         bDefineResource;
  npa_remote_resource_definition  Clock_NPARemoteResource;
  npa_callback                    fpCallBack;
  npa_client_handle               hClient;
  uint32                          nLowPowerValue;
  ClockDrvCtxt                   *pDrvCtxt;
} ClockNPACtxtType;


/*=========================================================================
      Data
==========================================================================*/

/*
 * Defined in DalClockFwk.c
 */
extern DALREG_DriverInfo DALClock_DriverInfo;

/*
 * gDALModDriverInfoList needs to be created for WM and UEFI - dynamic
 * loading of DAL drivers. This will not work for AMSS image (static loading
 * via DAL based XML files at compile time) so the following can't go in
 * DalClockFwk.c which is common across images
 *
 */
static DALREG_DriverInfo* DALDriverInfoArr[1] = { &DALClock_DriverInfo};
DALREG_DriverInfoList gDALModDriverInfoList= {1, DALDriverInfoArr};

/*-----------------------------------------------------------------------*/
/* Remote node definitions.                                              */
/*-----------------------------------------------------------------------*/

static ClockNPACtxtType Clock_aNPACtxt[] =
{
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/qdss",
      /* .remote_resource_name  = */ "clk0\x01\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "STATE",
      /* .max                   = */ 3,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL
    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 0,
    /* .pDrvCtxt                = */ NULL,
  },
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/dcvs.ena",
      /* .remote_resource_name  = */ "clk0\x02\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_binary_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "Enable",
      /* .max                   = */ 1,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL
    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 1,
    /* .pDrvCtxt                = */ NULL,
  },
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/pcnoc",
      /* .remote_resource_name  = */ "clk1\x00\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "KHz",
      /* .max                   = */ NPA_MAX_STATE,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL

    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 19200,
    /* .pDrvCtxt                = */ NULL,
  },
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/snoc",
      /* .remote_resource_name  = */ "clk1\x01\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "KHz",
      /* .max                   = */ NPA_MAX_STATE,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL
    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 19200,
    /* .pDrvCtxt                = */ NULL,
  },
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/bimc",
      /* .remote_resource_name  = */ "clk2\x00\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "KHz",
      /* .max                   = */ NPA_MAX_STATE,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL

    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 595200, /* SVS Max value taken, how to handle this for PCDDR3 */
    /* .pDrvCtxt                = */ NULL,
  },
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/ce1",
      /* .remote_resource_name  = */ "ce\x00\x00\x00\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "KHz",
      /* .max                   = */ NPA_MAX_STATE,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL

    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 0,
    /* .pDrvCtxt                = */ NULL,
  },
  
  {
    /* .bDefineResource         = */ TRUE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/clk/qpic",
      /* .remote_resource_name  = */ "qpic\x00\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "KHz",
      /* .max                   = */ NPA_MAX_STATE,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL

    },
    /* .fpCallBack              = */ NULL,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ 0,
    /* .pDrvCtxt                = */ NULL,
  },
#if 0 // move these nodes from PMIC to vcs-> rpm
  {
    /* .bDefineResource         = */ FALSE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/pmic/client/rail_cx",
      /* .remote_resource_name  = */ "smpa\x01\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "ModeID",
      /* .max                   = */ CLOCK_VREG_LEVEL_HIGH,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL

    },
    /* .fpCallBack              = */ Clock_VDDCXNodeReadyCallback,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ CLOCK_VREG_LEVEL_LOW_MINUS,
    /* .pDrvCtxt                = */ NULL,
  },
  {
    /* .bDefineResource         = */ FALSE,
    { /* .Clock_NPARemoteResource */
      /* .local_resource_name   = */ "/pmic/client/rail_mx",
      /* .remote_resource_name  = */ "smpa\x02\x00\x00\x00",
      /* .protocol_type         = */ "/protocol/rpm/rpm",
      /* .plugin                = */ &npa_max_plugin,
      /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
      /* .units                 = */ "ModeID",
      /* .max                   = */ CLOCK_VREG_LEVEL_HIGH,
      /* .attributes            = */ 0,
      /* .data                  = */ NULL,
      /* .handle                = */ NULL
    },
    /* .fpCallBack              = */ Clock_VDDMXNodeReadyCallback,
    /* .hClient                 = */ NULL,
    /* .nLowPowerValue          = */ CLOCK_VREG_LEVEL_LOW,
    /* .pDrvCtxt                = */ NULL,
  },
#endif
};

#define   CLOCK_CLUSTER1_FREQUENCY_HZ      1113600 * 1000UL
#define   CLOCK_CLUSTER1_FREQUENCY_HZ_BU   800000  * 1000UL


static ClockSourceFreqConfigType HFPLLCfgCPU1 =
{
  /*                |-.HALConfig------------------------------------------------------------------------------|                             |-.HWVersion----------------------------------------------------------------------------------|              */
  /* .nFreqHz,      {.eSource, .eVCO, .nPreDiv, .nPostDiv, .nL,  .nM, .nN, .nVCOMultiplier, .nAlpha, .nAlphaU } .eVRegLevel,                { { .Min.nMajor, .Min.nMinor }, { .Max.nMajor, .Max.nMinor }, .eChipInfoFamily, .aeChipInfoId }, .bLastEntry */
  CLOCK_CLUSTER1_FREQUENCY_HZ, { HAL_CLK_SOURCE_XO, HAL_CLK_PLL_VCO1, 1, 1,  58, 0, 0, 0, 0x00000000, 0x00 },              CLOCK_VREG_LEVEL_NOMINAL, { { 0, 0 }, { 0xFF, 0xFF }, DALCHIPINFO_FAMILY_UNKNOWN, NULL},                                   0
};

static ClockMuxConfigType MuxCfgHFCPU1 =
{
  /*                |-.HALConfig--------------------------|                              |-.HWVersion----------------------------------------------------------------------------------|                     */
  /* .nFreqHz,      { .eSource, .nDiv2x, .nM, .nN, .n2D   }  .eVRegLevel,                { { .Min.nMajor, .Min.nMinor }, { .Max.nMajor, .Max.nMinor }, .eChipInfoFamily, .aeChipInfoId }, .pSourceFreqConfig */
  CLOCK_CLUSTER1_FREQUENCY_HZ, { HAL_CLK_SOURCE_APCSPLL1, 2, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL, { { 0, 0 }, { 0xFF, 0xFF }, DALCHIPINFO_FAMILY_UNKNOWN, NULL},                                   &HFPLLCfgCPU1
};

#if 0
static ClockMuxConfigType MuxCfgHFCPU1_BU =
{
  /*                |-.HALConfig--------------------------|                              |-.HWVersion----------------------------------------------------------------------------------|                     */
  /* .nFreqHz,      { .eSource, .nDiv2x, .nM, .nN, .n2D   }  .eVRegLevel,                { { .Min.nMajor, .Min.nMinor }, { .Max.nMajor, .Max.nMinor }, .eChipInfoFamily, .aeChipInfoId }, .pSourceFreqConfig */
  CLOCK_CLUSTER1_FREQUENCY_HZ_BU,  { HAL_CLK_SOURCE_GPLL0, 2, 0, 0, 0 },    CLOCK_VREG_LEVEL_LOW, { { 0, 0 }, { 0xFF, 0xFF }, DALCHIPINFO_FAMILY_UNKNOWN, NULL},                                   NULL
};
#endif

static ClockMuxConfigType MuxCfgLowPower =
{
  /*                |-.HALConfig--------------------------|                              |-.HWVersion----------------------------------------------------------------------------------|                     */
  /* .nFreqHz,      { .eSource, .nDiv2x, .nM, .nN, .n2D   }  .eVRegLevel,                { { .Min.nMajor, .Min.nMinor }, { .Max.nMajor, .Max.nMinor }, .eChipInfoFamily, .aeChipInfoId }, .pSourceFreqConfig */
  800000 * 1000UL,  { HAL_CLK_SOURCE_GPLL0, 2, 0, 0, 0 },    CLOCK_VREG_LEVEL_LOW, { { 0, 0 }, { 0xFF, 0xFF }, DALCHIPINFO_FAMILY_UNKNOWN, NULL},                                   NULL
};

ClockLocalCtxtType  ClockLocalCtxt;

#define MDSSMDP_LOW_POWER_FREQUENCY   160000  // KHz
#define MDSSMDP_DEFAULT_FREQUENCY     266670  // KHz

/*
 * The voltage levels specified below were chosen to accommodate all parts of
 * every version. These values come from the characterized silicone and are
 * based on the Slow bin parts.
 */
#define CORE_LOWEST_VOLTAGE           1224000  // reference SSG part @ NOM is the Lowest corner for Vipertooth 1.224V 
#define CORE_NOMINAL_VOLTAGE          1224000  // reference SSG part @ varies MHz - 0.868 V


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_VDDCXNodeReadyCallback
** =========================================================================*/

void Clock_VDDCXNodeReadyCallback(void *pContext, unsigned int nEventType, void *pNodeName, unsigned int nNodeNameSize)
{
  ClockNPACtxtType  *pClockNPACtxtType = (ClockNPACtxtType *)pContext;

  /*-----------------------------------------------------------------------*/
  /* Create NPA client handle for VDD CX                                   */
  /*-----------------------------------------------------------------------*/

  pClockNPACtxtType->hClient = npa_create_sync_client(
    pClockNPACtxtType->Clock_NPARemoteResource.local_resource_name,
    "UEFI_client", NPA_CLIENT_REQUIRED);

} /* END Clock_VDDCXNodeReadyCallback */


/* =========================================================================
**  Function : Clock_VDDMXNodeReadyCallback
** =========================================================================*/

void Clock_VDDMXNodeReadyCallback(void *pContext, unsigned int nEventType, void *pNodeName, unsigned int nNodeNameSize)
{
  ClockNPACtxtType  *pClockNPACtxtType = (ClockNPACtxtType *)pContext;

  /*-----------------------------------------------------------------------*/
  /* Create NPA client handle for VDD MX                                   */
  /*-----------------------------------------------------------------------*/

  pClockNPACtxtType->hClient = npa_create_sync_client(
    pClockNPACtxtType->Clock_NPARemoteResource.local_resource_name,
    "UEFI_client", NPA_CLIENT_REQUIRED);

  /*-----------------------------------------------------------------------*/
  /* Test only.                                                            */
  /*-----------------------------------------------------------------------*/

  #if 0
  {
    DALResult eResult;

    eResult = Clock_EnterLowPowerMode(pClockNPACtxtType->pDrvCtxt);
    eResult = Clock_ExitLowPowerMode(pClockNPACtxtType->pDrvCtxt);
    eResult = eResult;
  }
  #endif

} /* END Clock_VDDMXNodeReadyCallback */


/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/*
  See DalClock.h
*/

DALResult Clock_InitImage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult                     eResult;
  uint32                        i;
  uint32                        nResultFreq;

  /*-----------------------------------------------------------------------*/
  /* Initialze local UEFI Clock Driver context.                            */
  /*-----------------------------------------------------------------------*/

  memset(&ClockLocalCtxt, 0, sizeof(ClockLocalCtxtType));

  /*-----------------------------------------------------------------------*/
  /* Initialize the PLLs                                                   */
  /*-----------------------------------------------------------------------*/
  #if 0
  Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_GPLL0,  0);
  Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_GPLL1,  0);
  Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_GPLL4,  0);
  Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_GPLL5,  0);
  #endif
  /*-----------------------------------------------------------------------*/
  /* Initialize the CPU PLLs.                                              */
  /*-----------------------------------------------------------------------*/
  HAL_clk_ConfigSource(HAL_CLK_SOURCE_APCSPLL1, HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE);

  HAL_clk_ConfigSource(HAL_CLK_SOURCE_APCSPLL1, HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE);

  /*-----------------------------------------------------------------------*/
  /* Get clock IDs for the cluster.                                        */
  /*-----------------------------------------------------------------------*/
  eResult = Clock_GetClockId(pDrvCtxt, "apcs_cluster1_clk", (ClockIdType *)&ClockLocalCtxt.ClusterGoldId);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
#if 0
    Clock_EnableClock(pDrvCtxt, (ClockIdType)ClockLocalCtxt.ClusterGoldId);
    Clock_SetClockConfig(pDrvCtxt, ClockLocalCtxt.ClusterGoldId->pDomain, &MuxCfgHFCPU1);
#endif
  /*-----------------------------------------------------------------------*/
  /* Get the MDSS MDP clock ID.                                            */
  /*-----------------------------------------------------------------------*/
  eResult = Clock_GetClockId(pDrvCtxt, "gcc_mdss_mdp_clk",
    (ClockIdType *)&ClockLocalCtxt.MDSSMDPClockId);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  /*-----------------------------------------------------------------------*/
  /* Set the MDSS MDP clock's frequency.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetClockFrequency(pDrvCtxt,
                                    (ClockIdType)ClockLocalCtxt.MDSSMDPClockId,
                                    MDSSMDP_DEFAULT_FREQUENCY,
                                    CLOCK_FREQUENCY_KHZ_CLOSEST,
                                    &nResultFreq);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the NPA context.                                           */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < SIZEOF_ARRAY(Clock_aNPACtxt); i++)
  {
    if (Clock_aNPACtxt[i].bDefineResource)
    {
      /*-----------------------------------------------------------------------*/
      /* Define the resource and initialize the handle.                        */
      /*-----------------------------------------------------------------------*/
      npa_remote_define_resource(&Clock_aNPACtxt[i].Clock_NPARemoteResource, 0, NULL);
      Clock_aNPACtxt[i].hClient = npa_create_sync_client(
        Clock_aNPACtxt[i].Clock_NPARemoteResource.local_resource_name,
        "UEFI_client", NPA_CLIENT_REQUIRED);
    }
    else
    {
      /*-----------------------------------------------------------------------*/
      /* Create callback events.                                               */
      /*-----------------------------------------------------------------------*/
      npa_resource_available_cb(Clock_aNPACtxt[i].Clock_NPARemoteResource.local_resource_name,
        Clock_aNPACtxt[i].fpCallBack, &Clock_aNPACtxt[i]);
    }
    Clock_aNPACtxt[i].pDrvCtxt = pDrvCtxt;
  }

  pDrvCtxt->pImageCtxt = &ClockLocalCtxt;

  /*-----------------------------------------------------------------------*/
  /* Initialize AVS.                                                       */
  /*-----------------------------------------------------------------------*/
 
  eResult = Clock_InitAVS(pDrvCtxt, NULL);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }
 

  /*-----------------------------------------------------------------------*/
  /* Test only.                                                            */
  /*-----------------------------------------------------------------------*/

#if 0
  eResult = Clock_EnterLowPowerMode(pDrvCtxt);
  eResult = Clock_ExitLowPowerMode(pDrvCtxt);
#endif
  return DAL_SUCCESS;

} /* END Clock_InitImage */


/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_VoltageRequest
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockVRegRequestType  *pCurrentRequest,
  ClockVRegRequestType  *pNewRequest
)
{

  /*-----------------------------------------------------------------------*/
  /* Nothing to do for UEFI just yet.                                      */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_VoltageRequest */


/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
)
{

  /*-----------------------------------------------------------------------*/
  /* Nothing to do for UEFI just yet.                                      */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitVoltage */


/* =========================================================================
**  Function : Clock_EnterLowPowerMode
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnterLowPowerMode
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult           eResult, eReturnedResult = DAL_SUCCESS;
  ClockLocalCtxtType  *pClockLocalCtxt = (ClockLocalCtxtType *)pDrvCtxt->pImageCtxt;
  uint32              i;
  uint32              nResultFreq;

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Do not allow nested entries of the Low Power Mode.                    */
  /*-----------------------------------------------------------------------*/

  if (pClockLocalCtxt->bLowPowerMode == TRUE)
  {
    DALCLOCK_FREE(pDrvCtxt);
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Lower the MDSS MDP clock's frequency.                                 */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetClockFrequency(pDrvCtxt,
                                    (ClockIdType)ClockLocalCtxt.MDSSMDPClockId,
                                    MDSSMDP_LOW_POWER_FREQUENCY,
                                    CLOCK_FREQUENCY_KHZ_CLOSEST,
                                    &nResultFreq);
  if (eReturnedResult == DAL_SUCCESS && eResult != DAL_SUCCESS)
  {
    eReturnedResult = eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Reduce each core's frequency to the lowest possible.                  */
  /*-----------------------------------------------------------------------*/

  Clock_SetClockConfig(pDrvCtxt, pClockLocalCtxt->ClusterGoldId->pDomain, &MuxCfgLowPower);

  /*-----------------------------------------------------------------------*/
  /* Reduce voltage of cluster to the lowest possible.               */
  /*-----------------------------------------------------------------------*/
#if 0 
/* In Qcs405 APC1_L2 minimum supported corner itself NOM only
 * May not need voltage switch and SAW2 is supporting 1 Byte PMIC_DATA causing API failure 
 */
  eResult = Clock_SetVoltageLevel(HAL_AVS_CORE_APC1_L2, CORE_LOWEST_VOLTAGE);
  if (eReturnedResult == DAL_SUCCESS && eResult != DAL_SUCCESS)
  {
    eReturnedResult = eResult;
  }
#endif
  /*-----------------------------------------------------------------------*/
  /* Reduce every bus frequency to the lowest possible.                    */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < SIZEOF_ARRAY(Clock_aNPACtxt); i++)
  {
    if (Clock_aNPACtxt[i].hClient != NULL)
    {
      npa_issue_required_request(Clock_aNPACtxt[i].hClient, Clock_aNPACtxt[i].nLowPowerValue);
    }
  }

  pClockLocalCtxt->bLowPowerMode = TRUE;

  DALCLOCK_FREE(pDrvCtxt);

  return eReturnedResult;

} /* END Clock_EnterLowPowerMode */


/* =========================================================================
**  Function : Clock_ExitLowPowerMode
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ExitLowPowerMode
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult           eResult = DAL_SUCCESS;
  DALResult           eReturnedResult = DAL_SUCCESS;
  ClockLocalCtxtType  *pClockLocalCtxt = (ClockLocalCtxtType *)pDrvCtxt->pImageCtxt;
  uint32              i;
  uint32              nResultFreq;

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Make sure the Low Power Mode is active before exiting it.             */
  /*-----------------------------------------------------------------------*/

  if (pClockLocalCtxt->bLowPowerMode == FALSE)
  {
    DALCLOCK_FREE(pDrvCtxt);
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Restore every bus frequency to its nominal.                           */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < SIZEOF_ARRAY(Clock_aNPACtxt); i++)
  {
    if (Clock_aNPACtxt[i].hClient != NULL)
    {
      npa_complete_request(Clock_aNPACtxt[i].hClient);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Restore voltage of both clusters to the nominal level.                */
  /*-----------------------------------------------------------------------*/
#if 0 
/* In Qcs405 APC1_L2 minimum supported corner itself NOM only
 * May not need voltage switch and SAW2 is supporting 1 Byte PMIC_DATA causing API failure 
 */
  eResult = Clock_SetVoltageLevel(HAL_AVS_CORE_APC1_L2, CORE_NOMINAL_VOLTAGE);
  if (eReturnedResult == DAL_SUCCESS && eResult != DAL_SUCCESS)
  {
      eReturnedResult = eResult;
    }
#endif
  /*-----------------------------------------------------------------------*/
  /* Restore each core's frequency to the nominal.                         */
  /*-----------------------------------------------------------------------*/

  if (eResult != DAL_ERROR)
  {
    MuxCfgHFCPU1.HALConfig.eSource = HAL_CLK_SOURCE_APCSPLL1;
    Clock_SetClockConfig(pDrvCtxt, ClockLocalCtxt.ClusterGoldId->pDomain, &MuxCfgHFCPU1);

    /*-----------------------------------------------------------------------*/
    /* Restore the MDSS MDP clock's frequency.                               */
    /*-----------------------------------------------------------------------*/

    eResult = Clock_SetClockFrequency(pDrvCtxt,
                                      (ClockIdType)ClockLocalCtxt.MDSSMDPClockId,
                                      MDSSMDP_DEFAULT_FREQUENCY,
                                      CLOCK_FREQUENCY_KHZ_CLOSEST,
                                      &nResultFreq);
    if (eReturnedResult == DAL_SUCCESS && eResult != DAL_SUCCESS)
    {
      eReturnedResult = eResult;
    }

    pClockLocalCtxt->bLowPowerMode = FALSE;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return eReturnedResult;

} /* END Clock_ExitLowPowerMode */


/*=========================================================================
**  Function : Clock_NormalizeChipInfo
** =========================================================================*/
/*
  See ClockDriver.h
*/

void Clock_NormalizeChipInfo
(
  HAL_clk_ContextType *HALClkCtxt
)
{
}

/* =========================================================================
**  Function : Clock_GetCpuFrequencyHz
** =========================================================================*/
DALResult Clock_GetCpuFrequencyHz
(
  ClockDrvCtxt *pDrvCtxt,
  uint32 *pnFrequencyHz
)
{
  if (pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize output parameter                                           */
  /*-----------------------------------------------------------------------*/


  *pnFrequencyHz = CLOCK_CLUSTER1_FREQUENCY_HZ;
  return DAL_SUCCESS;
}
