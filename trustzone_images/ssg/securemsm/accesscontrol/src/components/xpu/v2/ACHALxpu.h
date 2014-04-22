#ifndef AC_HAL_XPU_H
#define AC_HAL_XPU_H

/*===========================================================================
Copyright (c) 2010-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssg.tz/1.1.3/securemsm/accesscontrol/src/components/xpu/v2/ACHALxpu.h#1 $
$DateTime: 2019/05/23 05:32:51 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "HALxpu2.h"
#include "HALxpu2Internal.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ACHAL_xpu_GetResourceGroupConfig(a,b,c) HAL_xpu2_GetResourceGroupConfig(a,b,c)
#define ACHAL_xpu_GetDeviceParameters(a,b) HAL_xpu2_GetDeviceParameters(a,b)
#define ACHAL_xpu_ConfigResourceGroup(a,b,c) HAL_xpu2_ConfigResourceGroup(a,b,c)
#define ACHAL_xpu_Type HAL_xpu2_XPU2Type
#define ACHAL_xpu_DeviceType HAL_xpu2_XPUDeviceType
#define ACHAL_xpu_ResourceGroupConfigType HAL_xpu2_ResourceGroupConfigType
#define ACHAL_xpu_XPUDeviceParamsType HAL_xpu2_XPUDeviceParamsType
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef HAL_xpu2_ResourceGroupConfigType ACHAL_xpu_ResourceGroupConfigType;
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /* AC_HAL_XPU_H */
