/**
 * @file:  SpmiMaster.h
 * @brief: Configures the Spmi Master Controller
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/08/01 23:51:05 $
 * $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/SPMILib/src/core/hal/bear/SpmiMaster.h#1 $
 * $Change: 16788488 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIMASTER_H
#define	SPMIMASTER_H

#include "SpmiBusCfg.h"
#include "SpmiUtils.h"
#include "SpmiInfo.h"

Spmi_Result SpmiMaster_ConfigHW(SpmiDevInfo *pDev, uint8 masterId, SpmiBusCfg_ConfigType cfgType);
void SpmiMaster_LoadRadixTree(SpmiDevInfo *pDev, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries);

#endif

