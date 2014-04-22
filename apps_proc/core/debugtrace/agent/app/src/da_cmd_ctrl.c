/*=============================================================================
  FILE: da_cmd_ctrl.c

  OVERVIEW:     App interface for miscellaneous control commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_ctrl.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_parser.h"
#include "da_cmd_ctrl.h"

#include "qdss_control.h"

/*=========================================================================*
 * Sink commands
 *=========================================================================*/

da_cmd_resp_t da_cmd_ctrl_sink_set(dapar_sink_id_t sinkid)
{
   int result;
   uint8 sink_id;

   result = QDSS_CONTROL_SUCCESS;
   sink_id = 0;
   switch (sinkid)   // Convert debug agent values to TMC values.
   {
      case DAPAR_SINKID_ETB:
         sink_id = TMC_TRACESINK_ETB;
         break;
      case DAPAR_SINKID_DDR:
         sink_id = TMC_TRACESINK_RAM;
         break;
      case DAPAR_SINKID_TPIU_A:
         sink_id = TMC_TRACESINK_TPIU;
         break;
      case DAPAR_SINKID_USB:
         sink_id = TMC_TRACESINK_USB;
         break;
      case DAPAR_SINKID_USB_BUFFERED:
         sink_id = TMC_TRACESINK_USB_BUFFERED;
         break;
      case DAPAR_SINKID_SD:
         sink_id = TMC_TRACESINK_SD;
         break;
      case DAPAR_SINKID_TPIU_B:  // not available
         sink_id = 0;
         result = QDSS_CONTROL_UNSUPPORTED;
      case DAPAR_SINKID_UNKNOWN: // fall through
      default:
         sink_id = 0;
         result = QDSS_CONTROL_BAD_PARAM;
         break;
   }
   if (QDSS_CONTROL_SUCCESS == result)
   {
      // Note: The return value is mixed between QDSS_CONTROL_...
      // and DAL_ values. Therefore, cannot distinguish between
      // type of errors.
      result = qdss_control_set_sink(sink_id);
   }
   return (QDSS_CONTROL_SUCCESS == result) ?
      DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;
}

/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_ctrl_sink_get(dapar_sink_id_t *sinkid)
{
   uint8 sink_id;

   // Note: The return value is mixed between QDSS_CONTROL_...
   // and DAL_ values. Therefore, cannot distinguish between
   // type of errors.
   if (QDSS_CONTROL_SUCCESS != qdss_control_get_sink(&sink_id))
   {
      return DA_CMD_RESP_FAILED;
   }
   switch (sink_id) // Convert TMC values to debug agent values.
   {
      case TMC_TRACESINK_ETB:
         *sinkid = DAPAR_SINKID_ETB;
         break;
      case TMC_TRACESINK_RAM:
         *sinkid = DAPAR_SINKID_DDR;
         break;
      case TMC_TRACESINK_TPIU:
         *sinkid = DAPAR_SINKID_TPIU_A;
         break;
      case TMC_TRACESINK_USB:
         *sinkid = DAPAR_SINKID_USB;
         break;
      case TMC_TRACESINK_USB_BUFFERED:    // not available
         *sinkid = DAPAR_SINKID_UNKNOWN;
         break;
      case TMC_TRACESINK_SD:
         *sinkid = DAPAR_SINKID_SD;
         break;
      default:
         *sinkid = DAPAR_SINKID_UNKNOWN;
         break;
   }
   return DA_CMD_RESP_SUCCESS;
}

/*=========================================================================*
 * STM commands
 *=========================================================================*/

da_cmd_resp_t da_cmd_ctrl_stm_get(uint8 *state)
{
   // Note: The return value is mixed between QDSS_CONTROL_...
   // and DAL_ values. Therefore, cannot distinguish between
   // type of errors.
   return (QDSS_CONTROL_SUCCESS == qdss_control_get_stm(state)) ?
      DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;
}

/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_ctrl_stm_set(uint8 state)
{
   // Note: The return value is mixed between QDSS_CONTROL_...
   // and DAL_ values. Therefore, cannot distinguish between
   // type of errors.
   return (QDSS_CONTROL_SUCCESS == qdss_control_set_stm(state)) ?
      DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;
}

