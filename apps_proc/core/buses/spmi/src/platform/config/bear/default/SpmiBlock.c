/**
 * @file:  SpmiBlock.c
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2020/01/30 22:49:35 $
 * $Header: //components/rel/core.tx/6.0/buses/spmi/src/platform/config/bear/default/SpmiBlock.c#1 $
 * $Change: 22400605 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiInfo.h"
#include "SpmiTypes.h"
#include "SpmiHal.h"
#include "SpmiGenericConsts.h"
#include "SpmiUtils.h"
#include "SpmiOsTarget.h"

#define SPMI_PMIC_ARB_PHYS_ADDRESS 0x00000000
#define SPMI_BARE_OWNER_NUMBER     0

#define SPMI_MAX_BUSES_SUPPORTED      (1)

static int16 periphMap[SPMI_MAX_PERIPH_ID + 1];
static ChannelMapEntry channelMap[SPMI_MAX_CHANNELS_SUPPORTED];
static SpmiDeviceGenerics generics = INIT_SPMI_DEV_GENERICS;

static SpmiTargetConfig spmiTgt =
{
    .hwioAddrStr    = "PMIC_ARB",
    .interruptVec   = 0,
    .serClkStr      = NULL,
    .ahbClkStr      = NULL,
    .ahbClkId       = NULL,
    .serClkId       = NULL,
    .ahbClkIdInited = FALSE,
    .serClkIdInited = FALSE,
};

SpmiDevInfo spmiDevices[] = {
   {
      .baseAddrs          = (uint8*)SPMI_PMIC_ARB_PHYS_ADDRESS,
      .uDevIdx            = 0,
      .reservedChan       = SPMI_MAX_CHANNELS_SUPPORTED - 1,
      .nextChanIdx        = 0,
      .reservedChanEn     = TRUE,   
      .dynamicChannelMode = FALSE,   
      .periphMap          = periphMap,
      .channelMap         = channelMap,
      .pGenerics          = &generics,
      .target             = (void*) &spmiTgt,    
   },
};

SpmiInfo spmiInfo = {
  .owner              = SPMI_BARE_OWNER_NUMBER,
  .uNumDevices        = SPMI_MAX_BUSES_SUPPORTED,
  .devices            = spmiDevices,
};

SpmiInfo * pSpmiBlockInfo = &spmiInfo;

