/*
==============================================================================

FILE:         VCSBSP.c

DESCRIPTION:
  This file contains VCS BSP data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/vcs/config/mdm9205/VCSBSP.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
11/20/18   shm     First version for 9205

==============================================================================
            Copyright (c) 2018 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "VCSBSP.h"
#include "VCSDriver.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 *  VCS_RailCornerConfigsCX
 *
 *  Set of rail corner configurations.
 */
static VCSRailCornerConfigType VCS_CornerConfigCX[] =
{
  {
    /*.eCornerMin        = */VCS_CORNER_LOW,
    /*.eCornerMax        = */VCS_CORNER_TURBO,
    /*.eCornerInit       = */VCS_CORNER_NOMINAL,
    /*.eCornerInitThermal = */ VCS_CORNER_OFF,
    /*.pVoltageRange     = */NULL, /* We do not know about CX's voltage table */
    /*.nNumVoltageRanges = */0,    /* We do not directly manage CX voltages */
    /*.HWVersion         = */{ {0x00, 0x00}, {0xFF, 0xFF} }
  }
};


/*
 *  VCS_RailCornerConfigsMX
 *
 *  Set of rail corner configurations.
 */
static VCSRailCornerConfigType VCS_CornerConfigMX[] =
{
  {
    /*.eCornerMin        = */VCS_CORNER_LOW,
    /*.eCornerMax        = */VCS_CORNER_TURBO,
    /*.eCornerInit       = */VCS_CORNER_NOMINAL,
    /*.eCornerInitThermal = */ VCS_CORNER_OFF,
    /*.pVoltageRange     = */NULL, /* We do not know about MX's voltage table */
    /*.nNumVoltageRanges = */0,    /* We do not directly manage MX voltages */
    /*.HWVersion         = */{ {0x00, 0x00}, {0xFF, 0xFF} }
  }
};


/*
 * VCS_NPADefinitionDependencyCX
 *
 * Data required to define a an NPA resource in VCS.
 */
static VCSNPADefinitionType VCS_NPADefinitionDependencyCX =
{
  /* .szName = */ VCS_NPA_RESOURCE_VDD_CX_VLVL,
  {
    /* .szName  = */ "ldoa\x01\x00\x00\x00",
    /* .szUnits = */ "vlvl"
  },
};


/*
 * VCS_NPADefinitionActiveFloorCX
 *
 * Data required to define a an NPA resource in VCS.
 */
static VCSNPADefinitionType VCS_NPADefinitionActiveFloorCX =
{
  /* .szName = */ VCS_NPA_RESOURCE_VDD_CX_ACTIVE_FLOOR,
  {
    /* .szName  = */ "ldoa\x01\x00\x00\x00",
    /* .szUnits = */ "vfl"
  },
};


/*
 * VCS_NPADefinitionDependencyMX
 *
 * Data required to define a an NPA resource in VCS.
 */
static VCSNPADefinitionType VCS_NPADefinitionDependencyMX =
{
  /* .szName = */ VCS_NPA_RESOURCE_VDD_MX_VLVL,
  {
    /* .szName  = */ "ldoa\x04\x00\x00\x00",
    /* .szUnits = */ "vlvl"
  },
};


/*
 * VCS_NPADefinitionDependencyMX
 *
 * Data required to define a an NPA resource in VCS.
 */
static VCSNPADefinitionType VCS_NPADependencyMX =
{
  /* .szName = */ VCS_NPA_RESOURCE_VDD_MX,
};


/*
 * VCS_NPADefinitionActiveFloorMX;
 *
 * Data required to define a an NPA resource in VCS.
 */
static VCSNPADefinitionType VCS_NPADefinitionActiveFloorMX =
{
  /* .szName = */ VCS_NPA_RESOURCE_VDD_MX_ACTIVE_FLOOR,
  {
    /* .szName  = */ "ldoa\x04\x00\x00\x00",
    /* .szUnits = */ "vfl"
  },
};


/*
 * VCS_RailConfigs
 *
 * Set of rail configurations.
 */
static VCSRailConfigType VCS_RailConfigs[] =
{
  {
    /*.eRail             = */VCS_RAIL_CX,
    /*.szName            = */VCS_NPA_RESOURCE_VDD_CX,
    /*.pCornerConfig     = */VCS_CornerConfigCX,
    /*.nNumCornerConfigs = */ARR_SIZE(VCS_CornerConfigCX),
    /*.pNPAActiveFloor   = */&VCS_NPADefinitionActiveFloorCX,
    /*.pNPADependency    = */&VCS_NPADefinitionDependencyCX,
    /*.bEnableCPR        = */FALSE,
    /*.bEnableDVS        = */TRUE,
  },
  {
    /*.eRail             = */VCS_RAIL_MX,
    /*.szName            = */VCS_NPA_RESOURCE_VDD_MX,
    /*.pCornerConfig     = */VCS_CornerConfigMX,
    /*.nNumCornerConfigs = */ARR_SIZE(VCS_CornerConfigMX),
    /*.pNPAActiveFloor   = */&VCS_NPADefinitionActiveFloorMX,
    /*.pNPADependency    = */&VCS_NPADefinitionDependencyMX,
    /*.bEnableCPR        = */FALSE,
    /*.bEnableDVS        = */TRUE,
  },
};

/*
 * List of CPUs.
 */
static VCSCPUConfigType VCS_CPUConfigs[] =
{
  {
    /*.eCPU       = */CLOCK_CPU_APPS_C0,
    /*.szName     = */VCS_NPA_RESOURCE_CPU,
    /*.eRail      = */VCS_RAIL_CX,
  },
};


/*
 * VCS Log Default Configuration.
 */
const VCSLogType VCS_LogDefaultConfig[] =
{
  {
    /* .nLogSize = */ 1024,
  }
};


/*
 *  VCS_BSPConfig
 *
 *  List and length of Rail and CPU configurations.
 */
const VCSBSPConfigType VCS_BSPConfig =
{
  /*.pRailConfig     = */VCS_RailConfigs,
  /*.nNumRailConfigs = */ARR_SIZE(VCS_RailConfigs),
  /*.pCPUConfig      = */VCS_CPUConfigs,
  /*.nNumCPUConfigs  = */ARR_SIZE(VCS_CPUConfigs),
};


/*
 * Stub flags.
 */
const VCSStubType VCSStubConfig =
{
  /* .bRUMI   */ FALSE
};
