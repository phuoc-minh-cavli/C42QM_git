/**
 * @file:  SpmiOsNhlos.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2020/01/30 22:49:35 $
 * $Header: //components/rel/core.tx/6.0/buses/spmi/src/platform/os/tn/SpmiOsNhlos.h#1 $
 * $Change: 22400605 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIOSNHLOS_H
#define	SPMIOSNHLOS_H

#include "SpmiTypes.h"
#include "DALSysTypes.h"

Spmi_Result SpmiOsNhlos_GetDalProp(char* propId, DALSYSPropertyVar* val);

#endif
