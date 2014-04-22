#ifndef __VCSBSP_H__
#define __VCSBSP_H__
/*
===========================================================================
*/
/**
  @file VCSBSP.h

  BSP interface definitions for the DAL VCS device driver.
*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

  ====================================================================

  $Header: //components/rel/core.tx/6.0/systemdrivers/vcs/src/VCSBSP.h#1 $
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
#include "VCSDefs.h"
#include "ClockDefs.h"
#include "DDIChipInfo.h"
#include "npa_resource.h"

/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VCSLogType
 *
 * Default configuration for the VCS log.
 *
 *  nLogSize        - Log size in bytes.  Must be a power of 2.
 */
typedef struct
{
  uint32 nLogSize;
} VCSLogType;


/*
 * VCSPropertyValueType
 *
 * Generic VCS property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *VCSPropertyValueType;


/*
 * VCSHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 *  aeChipInfoId    - Array of chip id's that the configuration supports.
 *                    NULL indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType    eChipInfoFamily;
  const DalChipInfoIdType *aeChipInfoId;

} VCSHWVersionType;

/*
 * VCSCornerVoltageType
 *
 * BSP data structure for describing the voltage for each corner.
 *
 *  eCorner     - The voltage corner
 *  nVoltageUV  - The voltage in uV for this corner
 */
typedef struct
{
  VCSCornerType eCorner;
  uint32        nVoltageUV;
} VCSCornerVoltageType;


/*
 * VCSCornerVoltageRangeType
 *
 * BSP data structure for describing the voltage range for each corner.
 *
 *  eCorner - The voltage corner
 *  nMinUV  - The minimum rail voltage in uV for this corner
 *  nMaxUV  - The maximum rail voltage in uV for this corner
 */
typedef struct
{
  VCSCornerType eCorner;
  uint32        nMinUV;
  uint32        nMaxUV;
} VCSCornerVoltageRangeType;


/*
 * VCSRailCornerConfigType
 *
 * BSP Data structure for describing the rail configuration data.
 *
 *  eCornerMin        - The minimum corner at which the rail is allowed to operate
 *  eCornerMax        - The max corner at which the rail is allowed to operate
 *  eCornerInit       - The rail's initial corner at boot-up
 *  pVoltageRange     - Voltage corner table
 *  nNumVoltageRanges - Total number of corners used
 *  HWVersion         - Supported hardware version info
 */
typedef struct
{
  VCSCornerType              eCornerMin;
  VCSCornerType              eCornerMax;
  VCSCornerType              eCornerInit;
  VCSCornerType              eCornerInitThermal;
  VCSCornerVoltageRangeType *pVoltageRange;
  uint32                     nNumVoltageRanges;
  VCSHWVersionType           HWVersion;
} VCSRailCornerConfigType;


/*
 * VCSCPUConfigType
 *
 * BSP data structure for describing the CPU configuration.
 *
 *  eCPU        - CPU enum
 *  szName      - Name of the NPA client handle for this CPU to vote on the rail
 *  eRail       - Rail enum for this CPU's power source.
 *
 */
typedef struct
{
  ClockCPUType      eCPU;
  const char       *szName;
  VCSRailType       eRail;
} VCSCPUConfigType;

/*
 * VCSNPARemoteDefinitionType
 *
 * BSP Data structure for describing a VCS NPA remote resource.
 *
 *  szName  - Name of remote resource.
 *  szUnits - Name of the resource units or vector resource key.
 */
typedef struct
{
  const char *szName;
  const char *szUnits;
} VCSNPARemoteDefinitionType;


/*
 * VCSNPADefinitionType
 *
 * BSP Data structure for describing a VCS NPA resource.
 *
 *  szName              - Name of local resource.
 *  NPARemoteDefinition - Remote definition for this resource.
 */
typedef struct
{
  const char                 *szName;
  VCSNPARemoteDefinitionType  NPARemoteDefinition;
} VCSNPADefinitionType;

/*
 * VCSRailConfigType
 *
 * BSP Data structure for describing the rail configuration.
 *
 *  eRail             - Rail enum
 *  szName            - Name of rail resource
 *  pCornerConfig     - The rail's configuration data
 *  nNumCornerConfigs - Total number of configurations
 *  szNameDependency  - Name of NPA resource dependency
 *  bEnableCPR        - Enable switch for invoking CPR
 *  bEnableDVS        - Enable switch for dynamic voltage scaling
 */
typedef struct
{
  VCSRailType              eRail;
  const char              *szName;
  VCSRailCornerConfigType *pCornerConfig;
  uint32                   nNumCornerConfigs;
  VCSNPADefinitionType    *pNPAActiveFloor;
  VCSNPADefinitionType    *pNPADependency;
  boolean                  bEnableCPR;
  boolean                  bEnableDVS;
} VCSRailConfigType;


/*
 * VCSBSPConfigType
 *
 * BSP Data structure for list of the rail and CPU configurations.
 *
 *  pRailConfig     - Pointer to array of rail configurations
 *  nNumRailConfigs - Number of rails
 *  pCPUConfig      - Pointer to array of CPU configurations
 *  nNumCPUConfigs  - Number of CPUs
 *  pnCornerPMICMap - Pointer to array of integer PMIC corner enum values.
 */
typedef struct
{
  VCSRailConfigType *pRailConfig;
  uint32             nNumRailConfigs;
  VCSCPUConfigType  *pCPUConfig;
  uint32             nNumCPUConfigs;
  //uint32            *pnCornerPMICMap;
} VCSBSPConfigType;


/*
 * VCSStubType
 *
 * BSP data structure for stubbing out the VCS driver on various platforms.
 *
 * bRUMI   - Stub boolean for RUMI.
 */
typedef struct
{
  boolean bRUMI;
} VCSStubType;

#endif  /* __VCSBSP_H__ */ 
