#ifndef DA_CMD_ACTION_H
#define DA_CMD_ACTION_H
/**
  File: da_cmd_action.h

  Debug Agent - Interface for action type commands.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_action.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  007/08/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_cmd.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_action_set_file(int start_config);
da_cmd_resp_t da_cmd_action_set_file_end(void);

#endif /* #ifndef DA_CMD_ACTION_H */

