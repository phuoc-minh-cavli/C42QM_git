#ifndef CLOCKCONFIG_H
#define CLOCKCONFIG_H
/*
===========================================================================
*/
/**
  @file ClockSBLConfig.h

  Internal header file for the SBL configuration data structures.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/ClockTargetLib/ClockBSP.h#3 $
  $DateTime: 2019/01/22 01:13:44 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBoot.h"
#include "ClockCommon.h"
#include "railway.h"

/*=========================================================================
      References
==========================================================================*/

/*=========================================================================
      Definitions
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/

/* Data structure for SBL configuration data */
typedef struct
{
  /* PLL configurations */
  ClockConfigPLLType PLL0_Cfg;
  ClockConfigPLLType PLL1_Cfg;
  ClockConfigPLLType PLL2_Cfg;
  ClockConfigPLLType BIMCPLL_Cfg;
  //ClockConfigPLLType A53PWRPLL_Cfg;
  ClockConfigPLLType A53PERFPLL_Cfg;
  /* Configurations for CPU */
  ClockAPCSCfgType CPU_Cfg[CLOCK_BOOT_PERF_NUM];

  

  /* PCNOC config data */
  ClockConfigMuxType PCNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Crypto clock config */
  ClockConfigMuxType CE_Cfg;
  ClockConfigMuxType USB_Cfg;

  /* UART clock config */
  ClockConfigMuxType UART_Cfg[CLOCK_BOOT_PERF_NUM];
  
  /*RPM clock config*/
  ClockConfigMuxType RPM_Cfg;

  /* I2C clock config */
  ClockConfigMuxType I2C_Cfg[CLOCK_BOOT_PERF_NUM];

  ClockFreqMuxCfgType SPI_Cfg[4];

  BIMCClockCfgType BIMC_Cfg[5];

} Clock_ConfigType;

/* Data structure for Railway data */
typedef struct
{
  char*             CxRail;
  int               nCxRailId;
  railway_voter_t   CxVoter;
}Clock_RailwayType;

extern Clock_ConfigType *Clock_Config( void );

extern Clock_RailwayType *Clock_RailwayConfig( void );
extern boolean Clock_EnableSource( ClockSourceType eSource );
extern boolean Clock_DisableSource( ClockSourceType eSource );
extern boolean Clock_ConfigureSource( ClockSourceType eSource );

boolean Clock_SourceMapToMux
(
  const ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
);

boolean Clock_MuxMapToSource
(
  ClockConfigMuxType *pConfig,
  uint32 nSource
);

#endif /* !CLOCKCONFIG_H */

