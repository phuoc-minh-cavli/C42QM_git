/**
 * @file:  SpmiHal.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/12/26 21:36:05 $
 * $Header: //components/rel/rpm.bf/2.1.3/core/buses/spmi/src/core/hal/SpmiHal.c#1 $
 * $Change: 17934472 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiHal.h"
#include "SpmiOs.h"

//******************************************************************************
// Global Data
//******************************************************************************

void* pmicArbHwioBase = NULL;

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiHal_Init()
{
    if(pmicArbHwioBase != NULL) {
        return SPMI_SUCCESS;
    }

    return SpmiOs_GetPmicArbBaseAddr( &pmicArbHwioBase );
}
