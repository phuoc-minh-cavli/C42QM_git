#ifndef XOFESTRM_DATA_H
#define XOFESTRM_DATA_H

/*============================================================================
  FILE:         xofestrm_data.h

  OVERVIEW:     This file contains the data types for the XOFESTRM module.

  DEPENDENCIES: None
  Copyright (c) 2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/xofestrm/inc/xofestrm_data.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-08-02  rb  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "aries_os_api.h"
#include "xofestrm_api.h"
#include "tcxomgr.h"

/* filter update period for slope error estimation (in ms) */
#define XOFESTRM_PERIOD_GPS_OFF        1000
/* slope error estimation time/temp database size */
#define XOFESTRM_DB_SIZE               10
#define XOFESTRM_INVALID_TEMP ((tcxomgr_temp_type)0x80000000)
/* Time for which rpush information is valid */
#define XOFESTRM_RPUSH_TIME_THRESH     60 /* seconds */
/* Temperature range for which rpush information is valid */
#define XOFESTRM_RPUSH_TEMP_THRESH     4096   /* 4 deg C in 2^-10 deg C units */
/* Reset timer for Idle RPUSH */
#define XOFESTRM_IDLE_RESET_TIME       6000

static const uint8 ROT_NO_INFO = 0;

#define XOFESTRM_MAX_FREQ_UNC  (FLT)C_MAX_CLK_FREQ_UNC

#endif

