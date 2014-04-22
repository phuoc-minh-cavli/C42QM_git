/*=============================================================================
  FILE: da_cmd_action.c

  OVERVIEW:     App interface for action type commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_action.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  03/03/14   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main_int.h"
#include "da_cmd.h"

/*=========================================================================*
 *
 *=========================================================================*/
da_cmd_resp_t da_cmd_action_set_file(int start_config)
{
   if (DA_READER_RVAL_SUCCESS == da_set_start_file(start_config))
   {
      return DA_CMD_RESP_SUCCESS;
   }
   else
   {
      return DA_CMD_RESP_UNSUPPORTED;
   }
}

/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_action_set_file_end(void)
{
   if (DA_READER_RVAL_SUCCESS == da_set_file_end())
   {
      return DA_CMD_RESP_SUCCESS;
   }
   else
   {
      return DA_CMD_RESP_FAILED;
   }
}

