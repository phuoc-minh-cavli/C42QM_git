#ifndef CLOCKRPMNPA_H
#define CLOCKRPMNPA_H
/*
===========================================================================
*/
/**
  @file ClockRPMNPA.h

  NPA node definitions for the MSM8909 RPM clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/clock/hw/mdm9x05/ClockRPMNPA.h#2 $
  $DateTime: 2019/04/18 22:38:30 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  11/10/12   vtw     Ported from 8974, updated for 8x10.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <DALSys.h>
#include <railway.h>
#include <npa.h>
#include <npa_resource.h>
#include "ClockDriver.h"
#include "rpm.h"
#include "rpm_subsys_state.h"


/*=========================================================================
      Macros
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * BIMCStateType
 *
 * BIMC enum state type
 */
typedef enum
{
  BIMC_ON,           /* BIMC is on */
  BIMC_SAVING,       /* BIMC is in power collapsed, save DEHR */
  BIMC_COLLAPSED,    /* BIMC is in power collapsed */
  BIMC_RESTORING,    /* BIMC is in restoring state */
} BIMCStateType;


/*
 * ClockQDSSSpeedType
 *
 * QDSS enum speed type
 */
typedef enum
{
  CLOCK_QDSS_SPEED_XO,
  CLOCK_QDSS_SPEED_LOW,
  CLOCK_QDSS_SPEED_NORMAL,
  CLOCK_QDSS_NUM_SPEEDS,
} ClockQDSSSpeedType;


/*
 * NPAResourceType
 *
 * Structure containing the NPA clocks resource info
 */
typedef struct
{
  npa_resource_definition Resource;
  npa_node_definition     Node;
} NPAResourceType;


/*
 * ClockResourceType
 *
 * Structure containing the clocks resource info
 */
typedef struct
{
  NPAResourceType         NPANode;
  ClockNodeType           *pClock;
  uint32                  nMinLevel;
  uint32                  nMaxLevel;
  uint32                  nCurLevel;
  uint32                  nSavLevel;
} ClockResourceType;


/*
 * BIMCResourceType *
 * Structure containing the BIMC resource info
 */
typedef struct
{
  boolean                 bPowerCollapsed;  // DDR in PC or self-refresh
  boolean                 bBIMCAppsEnabled;   // last BIMC vote from APSS
  boolean                 bBIMCModemEnabled;  // last BIMC vote from MSS
  BIMCStateType           eXOBIMCState;     // BIMC state in XO shutdown
  BIMCStateType           eBIMCState;
  ClockIdType             nClockDehr;
  ClockPowerDomainIdType  nBIMCPowerID; 
} BIMCResourceType;

/*
 * ClockQDSSResourceType
 *
 * Structure containing the QDSS clocks info
 */
typedef struct
{
  NPAResourceType         NPANode;
  boolean                 bTraceAccess;
  boolean                 bQDSSEnabled;
  ClockVRegLevelType      nQDSSVoltageLevel;
  ClockQDSSLevelType      nQDSSCurrLevel;
  ClockQDSSSpeedType      nQDSSCurrSpeed;
  ClockQDSSSpeedType      nQDSSSaveSpeed;
  ClockNodeType           *pQDSSCfgAHBClk;
  ClockNodeType           *pQDSSDapAHBClk;
  ClockNodeType           *pQDSSEtrUSBClk;
  ClockNodeType           *pQDSSATClk;
  ClockNodeType           *pQDSSPCNocATClk;
  ClockNodeType           *pQDSSTraceClk;
  ClockNodeType           *pQDSSSTMClk;
  ClockNodeType           *pQDSSSTMAXIClk;
  ClockNodeType           *pQDSSDapClk;
  ClockNodeType           *pQDSSTSCTRClk;
  ClockNodeType           *pQDSSTRIGClk;
} ClockQDSSResourceType;


/*
 * ClockCEResourceType
 * Structure containing the Crypto Engine resource info
 */
typedef struct
{
  ClockResourceType      ClkCE;
  ClockNodeType         *pCEAXIClk;   // gcc_crypto_axi_clk
  ClockNodeType         *pCEAHBClk;   // gcc_crypto_ahb_clk
} ClockCEResourceType;

/*
 * ClockQPICResourceType
 * Structure containing the QPIC resource info
 */
typedef struct
{
  ClockResourceType       ClkQPIC;
  ClockResourceType       ClkQPICIO;
  boolean                 bQPICEnable;
  ClockNodeType           *pQPICSysClk;
  ClockNodeType           *pQPICAHBClk;
  ClockNodeType           *pQPICIOClk;
} ClockQPICResourceType;


/*
 * Clock_NPAResourcesType
 *
 * Structure containing the NPA node and resource data for local nodes.
 */
typedef struct
{
  uint32                  vol_level;
  int                     nCXRailID;
  uint32                  eGFXmicroVolts;
  NPAResourceType         CXONPANode;
  NPAResourceType         DCVSEnaNPANode;
  ClockResourceType       CPUClockResource;
  ClockResourceType       PCNOCClockResource;
  BIMCResourceType        BIMCResource;
  ClockResourceType       BIMCClockResource;
  ClockQPICResourceType   QPICClockResources;
  ClockResourceType       APSSAXIASYNCClockResource; 
  ClockResourceType       SPMClockResource;
  ClockResourceType       APSSAHBClockResource;
  ClockQDSSResourceType   QDSSClockResource;
  ClockResourceType       GPLL0ClockResource;
  ClockCEResourceType     CryptoClockResource;
  
} Clock_NPAResourcesType;


/*
 * Clock_DebugCfgType
 *
 * Structure containing the clock resource pointer and the string used in the clockbsp.xml
 */

typedef struct 
{
  ClockResourceType* ClockResDbg;
  char* pDbgDalStrProp;
} Clock_DebugCfgType;
/*=========================================================================
      Prototypes
==========================================================================*/

/*===========================================================================

  FUNCTION      Clock_SetQDSSClocks

  DESCRIPTION   This QDSS function is used for toggle the QDSS clocks ON/OFF

  PARAMETERS    nCurrentLevel - Current QDSS level
                nNewLevel - New QDSS level

  DEPENDENCIES  None.

  RETURN VALUE  None

  SIDE EFFECTS  None.

===========================================================================*/
void Clock_SetQDSSClocks
(
  ClockQDSSResourceType *pQDSSResource,
  ClockQDSSSpeedType eSpeed
);


/*===========================================================================

  FUNCTION      Clock_ChangeQDSSLevel

  DESCRIPTION   This function is used for change QDSS clock to new Level

  PARAMETERS    eCurLevel - (0=Off, 1=Debug, 2=High Speed)
                eNewLevel - (0=Off, 1=Debug, 2=High Speed)

  DEPENDENCIES  None.

  RETURN VALUE  New Level.

  SIDE EFFECTS  None.

===========================================================================*/
ClockQDSSLevelType Clock_ChangeQDSSLevel
(
  ClockQDSSResourceType *pQDSSResource,
  ClockQDSSLevelType    eCurLevel,
  ClockQDSSLevelType    eNewLevel
);


/*===========================================================================

  FUNCTION      Clock_InitNPA

  DESCRIPTION   This function is used setup NPA node to all the clock
                resource

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None

  SIDE EFFECTS  None.

===========================================================================*/
DALResult Clock_InitNPA
(
  void
);


/*===========================================================================

  FUNCTION      Clock_GetNPAResources

  DESCRIPTION   This function is for getting the point to all clocks NPA node

  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  Pointer to all clocks NPA resource

  SIDE EFFECTS  None.

===========================================================================*/
Clock_NPAResourcesType* Clock_GetNPAResources
(
  void
);


/*===========================================================================

  FUNCTION      Clock_SetClockSpeed

  DESCRIPTION   This function is for setting the clock speed
                  
  PARAMETERS    pClockResource [in] - Clock to be change
                nFreqHz [in]        - New clock speed

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/
npa_resource_state Clock_SetClockSpeed
(
  ClockResourceType   *pClockResource,
  uint32              nFreqHz
);

/*===========================================================================
  FUNCTION      Clock_DalDebugInit

  DESCRIPTION   This function is for setting the min and max frequencies of the clocks
                            based on the value provided in the clockchipset.xml
  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/


boolean Clock_DalDebugInit (Clock_NPAResourcesType  *pNPAResources);



/*===========================================================================

  FUNCTION      Clock_GetSubSysState

  DESCRIPTION   This function is for getting the state of the subsystem
                  
  PARAMETERS    rpm_master_client [in] - Subsystem type (APSS, MPSS, etc.)

  DEPENDENCIES  None.

  RETURN VALUE  State of the subsystem

  SIDE EFFECTS  None.

===========================================================================*/

rpm_subSystemState Clock_GetSubSysState
(
  rpm_master_client SubSysType

);


#endif /* #ifndef CLOCKRPMNPA_H */
