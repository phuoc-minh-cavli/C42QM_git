/**
 * @file:  SpmiOsNhlos.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/06/26 03:18:17 $
 * $Header: //components/rel/core.mpss/3.10/buses/spmi/bear/src/platform/os/SpmiOsNhlos.h#1 $
 * $Change: 16502877 $ 
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
