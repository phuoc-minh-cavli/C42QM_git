/*=============================================================================
  FILE: da_cmd_ctrl_common.c

  OVERVIEW:     Common functions for qdss_control_... type driver APIs:
                Sink, ETM, Funnel, STM and HWE

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_ctrl_common.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  01/03/14   lht    Initial revision.

=============================================================================*/

#include "da_cmd_ctrl_common.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_ctrl_common_control2cmd_rval(int result)
{
   switch (result)
   {
      case QDSS_CONTROL_SUCCESS:     return DA_CMD_RESP_SUCCESS;
      case QDSS_CONTROL_ERROR:       return DA_CMD_RESP_FAILED;
      case QDSS_CONTROL_BAD_STATE:   return DA_CMD_RESP_FAILED;
      case QDSS_CONTROL_UNSUPPORTED: return DA_CMD_RESP_UNSUPPORTED;
      case QDSS_CONTROL_BAD_PARAM:   return DA_CMD_RESP_INVALID_PARAM;
      case QDSS_CONTROL_FUSE_BLOWN:  return DA_CMD_RESP_UNSUPPORTED;
      default:                       return DA_CMD_RESP_FAILED;
   }
}


