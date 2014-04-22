/*=============================================================================
  FILE: dapar_cti.c

  OVERVIEW:     Debug Agent - process cti instructions.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_cti.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  11/15/13   lht    Initial revision.

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <string.h>

#include "da_main.h"
#include "da_parser.h"
#include "dapar_cti.h"
#include "da_cmd_cti.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Argument da_parser_info_t *pPrsr must point to a defined structure.

da_parser_rval_t _handler_cti_reset(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_map(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_unmap(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_get(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_cti_block(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_type(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_trigger(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_channel(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_cti_type_input(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_cti_type_output(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Parse tables.
// Param 2
static const da_parser_table_t ptable_cti[] =
{
   {"reset",      _handler_cti_reset},
   {"map",        _handler_cti_map},
   {"unmap",      _handler_cti_unmap},
   {"get",        _handler_cti_get},
   {"", NULL}
};

static const da_parser_table_t ptable_cti_attrib[] =
{
   {"block",      _handler_cti_block},
   {"type",       _handler_cti_type},
   {"trigger",    _handler_cti_trigger},
   {"channel",    _handler_cti_channel},
   {"", NULL}
};

static const da_parser_table_t ptable_cti_type[] =
{
   {"input",      _handler_cti_type_input},
   {"output",     _handler_cti_type_output},
   {"", NULL}
};

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/
/*=========================================================================*/
#define _SET_PDAT(_name) \
   if (NULL == pPrsr->pData)     \
   {                             \
      /* Programming error */    \
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* " #_name ", data missing"); \
      return DA_PARSER_RVAL_INTERNAL_ERR; \
   } \
   pDat = (dapar_cti_data_t *)pPrsr->pData


/*=========================================================================*/
// Parse attributes.
da_parser_rval_t _cti_get_attributes(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   rval = DA_PARSER_RVAL_SUCCESS;
   while (pPrsr->paramCnt > pPrsr->curParamIdx &&
          DA_PARSER_RVAL_SUCCESS == rval)
   {
      rval = da_parser_param(pPrsr, ptable_cti_attrib);
   }
   return rval;
}
/*=========================================================================*/
da_parser_rval_t _handler_cti_block(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;
   da_parser_rval_t rval;
   char *pParam;

   _SET_PDAT(cti_type_output);

   rval = da_parser_get_param(pPrsr, &pParam);
   pPrsr->curParamIdx++;
   if (NULL != pParam && DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->block_len = strlen(pParam);
      pDat->block_name = pParam;
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_type_input(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;

   _SET_PDAT(cti_type_output);
   pDat->type_valid = 1;
   pDat->type = 1;
   return DA_PARSER_RVAL_SUCCESS;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_type_output(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;
   _SET_PDAT(cti_type_output);
   pDat->type_valid = 1;
   pDat->type = 0;
   return DA_PARSER_RVAL_SUCCESS;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_type(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   rval = da_parser_param(pPrsr, ptable_cti_type);
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_trigger(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_cti_data_t *pDat;
   int trigger;

   _SET_PDAT(cti_trigger);

   rval = da_parser_unsigned(pPrsr, &trigger, sizeof(trigger));
   pPrsr->curParamIdx++;
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->trigger_num_valid = 1;
      pDat->trigger_num = trigger;
   }
   else
   {
      pDat->trigger_num_valid = 0;
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_channel(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_cti_data_t *pDat;
   int chan;

   _SET_PDAT(cti_channel);

   rval = da_parser_unsigned(pPrsr, &chan, sizeof(chan));
   pPrsr->curParamIdx++;
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->channel_num_valid = 1;
      pDat->channel_num = chan;
   }
   else
   {
      pDat->channel_num_valid = 0;
   }
   return rval;
}
/*=========================================================================*/
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_get(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;
   da_cmd_resp_t result;
   da_parser_rval_t rval;

   _SET_PDAT(cti_reset);
   rval = _cti_get_attributes(pPrsr);
   if (0 < pDat->block_len && pDat->type_valid)
   {
      result = da_cmd_cti_get(pDat);
      rval = (DA_CMD_RESP_SUCCESS == result) ?
         DA_PARSER_RVAL_SUCCESS : DA_PARSER_RVAL_CMD_FAILED;
   }
   else
   {
      if (DA_PARSER_RVAL_SUCCESS == rval)
      {
         rval = DA_PARSER_RVAL_PARAM_MISSING;
      }
   }
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: CTI get");

   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_unmap(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;
   da_cmd_resp_t result;
   da_parser_rval_t rval;

   _SET_PDAT(cti_reset);
   rval = _cti_get_attributes(pPrsr);
   if (0 < pDat->block_len && pDat->type_valid &&
       pDat->trigger_num_valid && pDat->channel_num_valid)
   {
      result = da_cmd_cti_unmap(pDat);
      rval = (DA_CMD_RESP_SUCCESS == result) ?
         DA_PARSER_RVAL_SUCCESS : DA_PARSER_RVAL_CMD_FAILED;
   }
   else
   {
      if (DA_PARSER_RVAL_SUCCESS == rval)
      {
         rval = DA_PARSER_RVAL_PARAM_MISSING;
      }
   }
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: CTI unmap");

   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_map(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;
   da_cmd_resp_t result;
   da_parser_rval_t rval;

   _SET_PDAT(cti_reset);
   rval = _cti_get_attributes(pPrsr);
   if (0 < pDat->block_len && pDat->type_valid &&
       pDat->trigger_num_valid && pDat->channel_num_valid)
   {
      result = da_cmd_cti_map(pDat);
      rval = (DA_CMD_RESP_SUCCESS == result) ?
         DA_PARSER_RVAL_SUCCESS : DA_PARSER_RVAL_CMD_FAILED;
   }
   else
   {
      if (DA_PARSER_RVAL_SUCCESS == rval)
      {
         rval = DA_PARSER_RVAL_PARAM_MISSING;
      }
   }
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: CTI map is TBD");

   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_cti_reset(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t *pDat;
   da_cmd_resp_t result;
   da_parser_rval_t rval;

   _SET_PDAT(cti_reset);
   rval = _cti_get_attributes(pPrsr);
   if (0 < pDat->block_len)
   {
      result = da_cmd_cti_reset(pDat);
      rval = (DA_CMD_RESP_SUCCESS == result) ?
         DA_PARSER_RVAL_SUCCESS : DA_PARSER_RVAL_CMD_FAILED;
   }
   else
   {
      if (DA_PARSER_RVAL_SUCCESS == rval)
      {
         rval = DA_PARSER_RVAL_PARAM_MISSING;
      }
   }
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: CTI reset");

   return rval;
}
/*=========================================================================*/
da_parser_rval_t dapar_cti_handler(da_parser_info_t *pPrsr)
{
   dapar_cti_data_t cti_data;

   memset (&cti_data, 0, sizeof (cti_data));

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr cti, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
   pPrsr->pData = (void*)&cti_data;

   return (da_parser_param(pPrsr, ptable_cti));
}
