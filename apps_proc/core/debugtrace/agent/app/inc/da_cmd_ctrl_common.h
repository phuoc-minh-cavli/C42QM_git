#ifndef DA_CMD_CTRL_COMMON_H
#define DA_CMD_CTRL_COMMON_H
/**
  File: da_cmd_ctrl_common.h

  Debug Agent - Shared for control and ctrl type driver APIs:
                Sink, ETM, Funnel, STM and HWE.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_ctrl_common.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  01/03/14   lht    Initial revision.

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_cmd.h"
#include "qdss_control.h"  // int result parameter

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_ctrl_common_control2cmd_rval(int result);

#endif /* #ifndef DA_CMD_CTRL_COMMON_H */

