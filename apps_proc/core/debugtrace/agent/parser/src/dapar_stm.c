/*=============================================================================
  FILE: dapar_stm.c

  OVERVIEW:     Debug Agent - process QDSS control instructions

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_stm.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main.h"
#include "da_parser.h"
#include "dapar_stm.h"
#include "da_cmd_ctrl.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Argument da_parser_info_t *pPrsr must point to a defined structure.

da_parser_rval_t _handler_stm_get(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_stm_enable(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_stm_disable(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_stm_get_state(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Parse tables.
// Param 2
static const da_parser_table_t ptable_stm[] =
{
   {"get",        _handler_stm_get},
   {"disable",    _handler_stm_disable},
   {"enable",     _handler_stm_enable},
   {"", NULL}
};
// Param 3
static const da_parser_table_t ptable_stm_get[] =
{
   {"state",     _handler_stm_get_state},
   {"", NULL}
};

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/
/*=========================================================================*/
da_parser_rval_t _cmd_stm_set_rval(da_cmd_resp_t cmd_result, char *txt)
{
   if (DA_CMD_RESP_SUCCESS == cmd_result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: stm set to %s", txt);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: stm set to %s failed", txt);
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_stm_enable(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_stm_set(1);
   return  (_cmd_stm_set_rval(result, "enable"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_stm_disable(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_stm_set(0);
   return  (_cmd_stm_set_rval(result, "disable"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_stm_get_state(da_parser_info_t *pPrsr)
{
   uint8 state;
   if (DA_CMD_RESP_SUCCESS == da_cmd_ctrl_stm_get(&state))
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: STM=%d", state);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: STM get failed");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_stm_get(da_parser_info_t *pPrsr)
{
   return (da_parser_param(pPrsr, ptable_stm_get));
}

/*---------------------------------------------------------------------------
 * External Function Definitions
 * ------------------------------------------------------------------------*/

/*=========================================================================*/
da_parser_rval_t dapar_stm_handler(da_parser_info_t *pPrsr)
{
   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr stm, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
   pPrsr->pData = NULL;
   return (da_parser_param(pPrsr, ptable_stm));
}

