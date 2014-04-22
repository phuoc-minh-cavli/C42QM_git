/*=============================================================================
  FILE: dapar_etm.c

  OVERVIEW:     Debug Agent - process etm instructions.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_etm.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <string.h>

#include "da_main.h"
#include "dapar_etm.h"
#include "da_cmd_etm.h"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// Argument da_parser_info_t *pPrsr must point to a defined structure.
da_parser_rval_t _handler_etm_ss_get(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_etm_ss_get_state(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_etm_ss_get_config(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_etm_ss_enable(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_etm_ss_disable(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_etm_ss_config(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Parse tables.
// Param 3
static const da_parser_table_t ptable_etm_ss[] =
{
   {"get",        _handler_etm_ss_get},
   {"enable",     _handler_etm_ss_enable},
   {"disable",    _handler_etm_ss_disable},
   {"config",     _handler_etm_ss_config},
   {"", NULL}
};
// Param 4 (state and attribute)
static const da_parser_table_t ptable_etm_ss_get[] =
{
   {"state",      _handler_etm_ss_get_state},
   {"config",     _handler_etm_ss_get_config},
   {"", NULL}
};

/*---------------------------------------------------------------------------
 * Internal Function Definitions
 * ------------------------------------------------------------------------*/
#define _SET_PDAT(_pdat_type, _name) \
   if (NULL == pPrsr->pData)     \
   {                             \
      /* Programming error */    \
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* " #_name ", data missing"); \
      return DA_PARSER_RVAL_INTERNAL_ERR; \
   } \
   pDat = (_pdat_type *)pPrsr->pData

// WARNING: For all internal functions...
//    pPrsr->pData must be assigned to a etm data structure.
//    And the ssid in the etm data structure must be assigned.

/*=========================================================================*/
/*-------------------------------------------------------------------------*/
static da_parser_rval_t _handler_etm_ss_config(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   da_parser_rval_t rval;
   dapar_etm_data_t *pDat;
   char *pParam;

   _SET_PDAT(dapar_etm_data_t, etm_ss_config);

   // Get the configuration string
   rval = da_parser_get_param(pPrsr, &pParam);
   pPrsr->curParamIdx++;

   if (DA_PARSER_RVAL_SUCCESS == rval && NULL != pParam)
   {
      pDat->cmd_str = pParam;
      result = da_cmd_etm_set_config(pDat);
      if (DA_CMD_RESP_SUCCESS == result)
      {
         rval = DA_PARSER_RVAL_SUCCESS;
      }
      else
      {
         rval = DA_PARSER_RVAL_CMD_FAILED;
      }
   }
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X ETM set config %s",
                 pDat->ssid, pDat->cmd_str);
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: SS%X ETM set config failed: %s",
                 pDat->ssid, pDat->cmd_str);
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
static da_parser_rval_t _cmd_etm_set_state(dapar_etm_data_t *pDat)
{
   da_cmd_resp_t result;

   result = da_cmd_etm_set_state(pDat);

   if (DA_CMD_RESP_SUCCESS == result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X ETM %s",
              pDat->ssid, pDat->state?"enabled":"disabled");
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: SS%X ETM %s failed",
              pDat->ssid, pDat->state?"enabled":"disabled");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*-------------------------------------------------------------------------*/
static da_parser_rval_t _handler_etm_ss_enable(da_parser_info_t *pPrsr)
{
   dapar_etm_data_t *pDat;
   _SET_PDAT(dapar_etm_data_t, etm_ss_enable);
   pDat->state = 1;  // Enable ETM
   return (_cmd_etm_set_state(pDat));
}
/*-------------------------------------------------------------------------*/
static da_parser_rval_t _handler_etm_ss_disable(da_parser_info_t *pPrsr)
{
   dapar_etm_data_t *pDat;
   _SET_PDAT(dapar_etm_data_t, etm_ss_disable);
   pDat->state = 0; // Disable ETM
   return (_cmd_etm_set_state(pDat));
}
/*=========================================================================*/
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_etm_ss_get_config(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   da_parser_rval_t rval;
   char *pParam;
   dapar_etm_data_t *pDat;

   _SET_PDAT(dapar_etm_data_t, etm_ss_get_config);

   // Get the string of ETM options being requested.
   rval = da_parser_get_param(pPrsr, &pParam);
   pPrsr->curParamIdx++;

   if (DA_PARSER_RVAL_SUCCESS == rval && NULL != pParam)
   {
      pDat->cmd_str = pParam;
      result = da_cmd_etm_get_config(pDat);
      if (DA_CMD_RESP_SUCCESS == result)
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X ETM config is %s",
                    pDat->ssid, pDat->settings_str);
         rval = DA_PARSER_RVAL_SUCCESS;
      }
      else
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X ETM get config failed; %s",
                    pDat->ssid, pDat->cmd_str);
         rval = DA_PARSER_RVAL_CMD_FAILED;
      }
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
static da_parser_rval_t _handler_etm_ss_get_state(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   dapar_etm_data_t *pDat;

   _SET_PDAT(dapar_etm_data_t, etm_ss_get_state);

   result = da_cmd_etm_get_state(pDat);
   if (DA_CMD_RESP_SUCCESS == result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X ETM=%d",
                 pDat->ssid, pDat->state);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X ETM get state failed.",
                 pDat->ssid);
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*-------------------------------------------------------------------------*/
static da_parser_rval_t _handler_etm_ss_get(da_parser_info_t *pPrsr)
{
   return da_parser_param(pPrsr, ptable_etm_ss_get);
}

/*---------------------------------------------------------------------------
 * External Function Definitions
 * ------------------------------------------------------------------------*/

/*=========================================================================*/
da_parser_rval_t dapar_etm_handler(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_parser_ssid_t ssid;
   dapar_etm_data_t etm_data;

   memset (&etm_data, 0, sizeof (etm_data));

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr etm, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }

   pPrsr->pData = (void*)&etm_data;

   rval = da_parser_subsys(pPrsr, &ssid);
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      etm_data.ssid = ssid;
      da_out_msg(DA_OUT_LEVEL_DEBUG, "SSID=%X", ssid);

      // Move on to param 3
      pPrsr->curParamIdx++;
      rval = da_parser_param(pPrsr, ptable_etm_ss);
   }
   pPrsr->pData = NULL;
   return rval;
}
