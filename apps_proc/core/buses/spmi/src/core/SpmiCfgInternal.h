/**
 * @file:  SpmiCfg.h
 * @brief: This module provides configuration options for the SPMI controller
 * 
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2020/01/30 22:49:35 $
 * $Header: //components/rel/core.tx/6.0/buses/spmi/src/core/SpmiCfgInternal.h#1 $
 * $Change: 22400605 $ 
 * 
 *                              Edit History
 * Date      Description
 * --------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/14/14  Initial Version
 */
#ifndef SPMICFGINTERNAL_H
#define	SPMICFGINTERNAL_H

#include "SpmiBusCfg.h"
#include "SpmiInfo.h"

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiBusCfg_ConfigureChannel(SpmiDevInfo *pDev, SpmiBusCfg_ChannelCfg* chanCfg, boolean autoAssignChan);

#endif
