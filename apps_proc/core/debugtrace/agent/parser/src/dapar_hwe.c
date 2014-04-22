/*=============================================================================
  FILE: dapar_hwe.c

  OVERVIEW:     Debug Agent - process hardware event instructions

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_hwe.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <string.h>

#include "da_main.h"
#include "dapar_hwe.h"
#include "da_cmd_hwe.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Argument da_parser_info_t *pPrsr must point to a defined structure.

da_parser_rval_t _handler_hwe_get(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_disable(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_enable(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_set(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_hwe_get_state(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_set_trigger(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_set_detect(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_hwe_ss_setreg(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_ss_setreg_addr(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_hwe_ss_setreg_value(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Parse tables.
// Param 2
static const da_parser_table_t ptable_hwe[] =
{
   {"get",        _handler_hwe_get},
   {"disable",    _handler_hwe_disable},
   {"enable",     _handler_hwe_enable},
   {"set",        _handler_hwe_set},
   {"", NULL}
};
// Param 3
static const da_parser_table_t ptable_hwe_get[] =
{
   {"state",      _handler_hwe_get_state},
   {"", NULL}
};
// Param 3
static const da_parser_table_t ptable_hwe_ss[] =
{
   {"setreg",     _handler_hwe_ss_setreg},
   {"", NULL}
};
// Param 3
static const da_parser_table_t ptable_hwe_set[] =
{
   {"trigger",    _handler_hwe_set_trigger},
   {"detect",     _handler_hwe_set_detect},
   {"", NULL}
};
// Param 4 and 6
static const da_parser_table_t ptable_hwe_ss_setreg[] =
{
   {"addr",       _handler_hwe_ss_setreg_addr},
   {"value",      _handler_hwe_ss_setreg_value},
   {"", NULL}
};

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

#define _SET_PDAT(_name) \
   if (NULL == pPrsr->pData)     \
   {                             \
      /* Programming error */    \
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* " #_name ", data missing"); \
      return DA_PARSER_RVAL_INTERNAL_ERR; \
   } \
   pDat = (dapar_hwe_data_t *)pPrsr->pData

/*=========================================================================*/
da_parser_rval_t _cmd2parser_rval(da_cmd_resp_t cmd_result, char *txt)
{
   switch(cmd_result)
   {
      case DA_CMD_RESP_SUCCESS:
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: hwe %s succeeded", txt);
         return DA_PARSER_RVAL_SUCCESS;
      case DA_CMD_RESP_INVALID_PARAM:
         da_out_msg(DA_OUT_LEVEL_ERR, "DA: hwe %s param invalid", txt);
         return DA_PARSER_RVAL_PARAM_INVALID;
      default:
         da_out_msg(DA_OUT_LEVEL_ERR, "DA: hwe %s failed", txt);
         return DA_PARSER_RVAL_CMD_FAILED;
   }
}

/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_set_detect(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_cmd_resp_t result;
   unsigned long mask_val;

   rval = da_parser_unsigned(pPrsr, &mask_val, sizeof(mask_val));
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      result = da_cmd_hwe_set_detect_mask(mask_val);
      rval = _cmd2parser_rval(result, "set detect mask");
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_set_trigger(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_cmd_resp_t result;
   unsigned long mask_val;

   rval = da_parser_unsigned(pPrsr, &mask_val, sizeof(mask_val));
   pPrsr->curParamIdx++;
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      result = da_cmd_hwe_set_trigger_mask(mask_val);
      rval = _cmd2parser_rval(result, "set trigger mask");
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_set(da_parser_info_t *pPrsr)
{
   return (da_parser_param(pPrsr, ptable_hwe_set));
}

/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_ss_setreg_addr(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_hwe_data_t *pDat;
   unsigned long addr;

   _SET_PDAT(hwe_ss_setreg);
   addr = 0;
   rval = da_parser_unsigned(pPrsr, &addr, sizeof(addr));
   pPrsr->curParamIdx++;
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->addr = addr;
      pDat->addr_valid = 1;
   }
   return rval;
}
da_parser_rval_t _handler_hwe_ss_setreg_value(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_hwe_data_t *pDat;
   unsigned long value;

   _SET_PDAT(hwe_ss_setreg);
   value = 0;
   rval = da_parser_unsigned(pPrsr, &value, sizeof(value));
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->value = value;
      pDat->value_valid = 1;
   }
   return rval;
}
da_parser_rval_t _handler_hwe_ss_setreg(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_cmd_resp_t result;
   dapar_hwe_data_t *pDat;

   _SET_PDAT(hwe_ss_setreg);

   result = DA_CMD_RESP_SUCCESS;

   // Get both the address and the value.
   rval = da_parser_param(pPrsr, ptable_hwe_ss_setreg);
   if (DA_PARSER_RVAL_SUCCESS != rval)
   {
      return rval;
   }
   rval = da_parser_param(pPrsr, ptable_hwe_ss_setreg);
   if (DA_PARSER_RVAL_SUCCESS != rval)
   {
      return rval;
   }
   if (pDat->addr_valid && pDat->value_valid)
   {
      result = da_cmd_hwe_set_reg(pDat);
      rval = _cmd2parser_rval(result, "set register");
   }
   else
   {
      return DA_PARSER_RVAL_PARAM_MISSING;
   }
   return rval;
}
da_parser_rval_t _handler_hwe_ss(da_parser_info_t *pPrsr)
{
   return (da_parser_param(pPrsr, ptable_hwe_ss));
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_enable(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_hwe_set(1);
   return _cmd2parser_rval(result, "enabled");
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_disable(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   result = da_cmd_hwe_set(0);
   return _cmd2parser_rval(result, "disabled");
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_hwe_get_state(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   uint8 state;
   result = da_cmd_hwe_get(&state);
   return _cmd2parser_rval(result, "get state");
}
da_parser_rval_t _handler_hwe_get(da_parser_info_t *pPrsr)
{
   return (da_parser_param(pPrsr, ptable_hwe_get));
}

/*---------------------------------------------------------------------------
 * External Function Definitions
 * ------------------------------------------------------------------------*/
/*=========================================================================*/
da_parser_rval_t dapar_hwe_handler(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_hwe_data_t hwe_data;
   int hold_idx;
   da_parser_ssid_t ssid;

   memset (&hwe_data, 0, sizeof(hwe_data));

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr hwe, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
   pPrsr->pData = (void*)&hwe_data;

   // Process parameter 2
   // This will take care of all keyword parameters (subsystem parameter
   // is handled next).
   hold_idx = pPrsr->curParamIdx;
   rval = da_parser_param(pPrsr, ptable_hwe);

   if ( (DA_PARSER_RVAL_PARAM_INVALID == rval) &&
        (hold_idx == pPrsr->curParamIdx) )
   {  // Still on param 2, check for subsystem
      rval = da_parser_subsys(pPrsr, &ssid);
      if (DA_PARSER_RVAL_SUCCESS == rval)
      {
         hwe_data.ssid = ssid;
         da_out_msg(DA_OUT_LEVEL_DEBUG, "SSID=%X", ssid);
         pPrsr->curParamIdx++;
         rval = da_parser_param(pPrsr, ptable_hwe_ss);
      }
   }

   return rval;
}

