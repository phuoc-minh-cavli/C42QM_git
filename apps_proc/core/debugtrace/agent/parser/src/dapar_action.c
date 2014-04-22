/*=============================================================================
  FILE: dapar_action.c

  OVERVIEW:     Debug Agent - Process instructions of some action.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_action.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "DALSys.h"
#include "busywait.h"

#include "da_main.h"
#include "da_parser.h"
#include "da_cmd.h"
#include "da_cmd_action.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_startup_h(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_startup_cfgbuf(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_startup_cfg0(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_startup_cfg1(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Param 2
static const da_parser_table_t ptable_startup[] =
{
   {"agent_cfg_buffer",    _handler_startup_cfgbuf},
   {"agent_cfg_def.txt",   _handler_startup_cfg0},
   {"agent_cfg1.txt",      _handler_startup_cfg1},
   {"", NULL}
};

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

/*=========================================================================*/
da_parser_rval_t dapar_action_reset_handler(da_parser_info_t *pPrsr)
{
   pPrsr->pData = NULL;
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA resetting");
   // To do...
   return DA_PARSER_RVAL_CMD_FAILED;
}
/*=========================================================================*/
da_parser_rval_t _set_start_file(int start_config)
{
   if (DA_CMD_RESP_SUCCESS == da_cmd_action_set_file(start_config))
   {

      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
da_parser_rval_t _handler_startup_cfg1(da_parser_info_t *pPrsr)
{
   return _set_start_file(DA_START_CONFIG_CFG1);
}
da_parser_rval_t _handler_startup_cfg0(da_parser_info_t *pPrsr)
{
   return _set_start_file(DA_START_CONFIG_CFG0);
}
da_parser_rval_t _handler_startup_cfgbuf(da_parser_info_t *pPrsr)
{
   return _set_start_file(DA_START_CONFIG_BUFFER);
}
da_parser_rval_t dapar_action_startup_handler(da_parser_info_t *pPrsr)
{
   return da_parser_param(pPrsr, ptable_startup);
}
/*=========================================================================*/
da_parser_rval_t dapar_action_delay_handler(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   unsigned long time_ms;

   pPrsr->pData = NULL;
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA delaying");

   rval = da_parser_unsigned(pPrsr, &time_ms, sizeof(time_ms));
   if (DA_PARSER_RVAL_SUCCESS != rval)
   {
      return rval;
   }
   busywait(time_ms*1000); //Pause execution
//void DALSYS_BusyWait(uint32 pause_time_us);
//void rex_sleep(unsigned long msecs);

   return DA_PARSER_RVAL_SUCCESS;
}
/*=========================================================================*/
da_parser_rval_t dapar_action_end_handler(da_parser_info_t *pPrsr)
{
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA end instruction.");

   if (DA_CMD_RESP_SUCCESS == da_cmd_action_set_file_end())
   {
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}

