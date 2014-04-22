#ifndef DA_CMD_CTRL_H
#define DA_CMD_CTRL_H
/**
  File: da_cmd_ctrl.h

  Debug Agent - Interface for qdss control commands.
      Sink
      STM
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_ctrl.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  007/08/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "da_cmd.h"
#include "dapar_sink.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_ctrl_sink_set(dapar_sink_id_t sinkid);
da_cmd_resp_t da_cmd_ctrl_sink_get(dapar_sink_id_t *sinkid);

da_cmd_resp_t da_cmd_ctrl_stm_set(uint8 state);
da_cmd_resp_t da_cmd_ctrl_stm_get(uint8 *state);

#endif /* #ifndef DA_CMD_CTRL_H */

