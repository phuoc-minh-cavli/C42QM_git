#ifndef PM_APP_CLK__H
#define PM_APP_CLK__H

/** @file pm_app_clk.h
*
*  This file contains application layer APIs to support PMIC
*  Clock peripheral.
*/
/*
*  Copyright (c) 2013-2019 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.3/core/api/systemdrivers/pmic/pm_app_clk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/15   kt      Created. 
========================================================================== */
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_clk.h"

/*===========================================================================

                        API

===========================================================================*/
/** @addtogroup pm_app_clk
@{ */

/**
*  An app level API to get the currently configured XO warmup
*  time delay in micro seconds.
*  
* @param[out] warmup_time_usec Pointer to return the configured 
*       warmup time delay value in micro seconds.
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_app_clk_get_xo_warmup_time(uint32 *warmup_time_usec);

/** @} */ /* end_addtogroup pm_app_clk */
#endif /* PM_APP_CLK__H */
