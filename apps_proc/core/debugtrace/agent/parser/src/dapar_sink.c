/*=============================================================================
  FILE: dapar_sink.c

  OVERVIEW:     Debug Agent - process sink instructions

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_sink.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main.h"
#include "da_parser.h"
#include "dapar_sink.h"
#include "da_cmd_ctrl.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// Argument da_parser_info_t *pPrsr must point to a defined structure.
da_parser_rval_t _handler_sink_set(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_get(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_etb(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_ddr(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_tpiu(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_tpiuA(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_tpiuB(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_sd(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_usb(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_usb_type(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_sink_set_usb_buffered(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Parse tables.
// Param 2
static const da_parser_table_t ptable_sink[] =
{
   {"set",        _handler_sink_set},
   {"get",        _handler_sink_get},
   {"", NULL}
};
// Param 3
static const da_parser_table_t ptable_sink_set[] =
{
   {"etb",        _handler_sink_set_etb},
   {"ddr",        _handler_sink_set_ddr},
   {"tpiuA",      _handler_sink_set_tpiuA},
   {"tpiuB",      _handler_sink_set_tpiuB},
   {"sd",         _handler_sink_set_sd},
   {"usb",        _handler_sink_set_usb},
   {"", NULL}
};
// Param 4
static const da_parser_table_t ptable_sink_set_usb_ext[] =
{
   {"type",       _handler_sink_set_usb_type},
   {"", NULL}
};
// Param
static const da_parser_table_t ptable_sink_set_usb_type[] =
{
   {"buffered",   _handler_sink_set_usb_buffered},
   {"", NULL}
};

/*---------------------------------------------------------------------------
 * Internal Function Definitions
 * ------------------------------------------------------------------------*/
/*=========================================================================*/
da_parser_rval_t _cmd_sink_set_rval(da_cmd_resp_t cmd_result, char *txt)
{
   if (DA_CMD_RESP_SUCCESS == cmd_result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: sink set to %s", txt);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: sink set to %s failed", txt);
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*=========================================================================*/
da_parser_rval_t _handler_sink_set(da_parser_info_t *pPrsr)
{
   return (da_parser_param(pPrsr, ptable_sink_set));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_sink_set_etb(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_sink_set(DAPAR_SINKID_ETB);
   return  (_cmd_sink_set_rval(result, "ETB"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_sink_set_ddr(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_sink_set(DAPAR_SINKID_DDR);
   return  (_cmd_sink_set_rval(result, "DDR"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_sink_set_tpiuA(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_sink_set(DAPAR_SINKID_TPIU_A);
   return  (_cmd_sink_set_rval(result, "TPIU A"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_sink_set_tpiuB(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_sink_set(DAPAR_SINKID_TPIU_B);
   return  (_cmd_sink_set_rval(result, "TPIU B"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_sink_set_sd(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_sink_set(DAPAR_SINKID_SD);
   return  (_cmd_sink_set_rval(result, "SD"));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_sink_set_usb_buffered(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_ctrl_sink_set(DAPAR_SINKID_USB_BUFFERED);
   return  (_cmd_sink_set_rval(result, "USB buffered"));
}
da_parser_rval_t _handler_sink_set_usb_type(da_parser_info_t *pPrsr)
{
   return (da_parser_param(pPrsr, ptable_sink_set_usb_type));
}
da_parser_rval_t _handler_sink_set_usb(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;

   if (pPrsr->paramCnt == pPrsr->curParamIdx)
   {  // End of instruction, process it.
      result = da_cmd_ctrl_sink_set(DAPAR_SINKID_USB);
      return  (_cmd_sink_set_rval(result, "USB"));
   }
   else
   {  // Extended instruction, continue parsing
      return (da_parser_param(pPrsr, ptable_sink_set_usb_ext));
   }
}
/*=========================================================================*/
da_parser_rval_t _handler_sink_get(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   dapar_sink_id_t sinkid;
   result = da_cmd_ctrl_sink_get(&sinkid);

   if (DA_CMD_RESP_SUCCESS == result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: sink=%d *To do*", sinkid);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: sink=%d failed *To do*", sinkid);
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}

/*---------------------------------------------------------------------------
 * External Function Definitions
 * ------------------------------------------------------------------------*/
/*=========================================================================*/
da_parser_rval_t dapar_sink_handler(da_parser_info_t *pPrsr)
{
   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr sink, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }

   pPrsr->pData = NULL;
   return (da_parser_param(pPrsr, ptable_sink));
}
