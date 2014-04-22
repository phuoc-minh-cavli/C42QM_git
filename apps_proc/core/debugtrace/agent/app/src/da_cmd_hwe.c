/*=============================================================================
  FILE: da_cmd_hwe.c

  OVERVIEW:     App interface for hwe commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_hwe.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main.h"
#include "da_cmd_ctrl_common.h"
#include "da_parser.h"
#include "da_cmd_hwe.h"

#include "qdss_control.h"
#include "da_cmd_qmi.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
/*=========================================================================*
 * HWE commands
 *=========================================================================*/
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_hwe_get(uint8 *state)
{
   int result;    //QDSS_CONTROL_...
   result = qdss_control_get_hwe(state);
   return da_cmd_ctrl_common_control2cmd_rval(result);
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_hwe_set(uint8 state)
{
   int result;
   result = qdss_control_set_hwe(state);
   return da_cmd_ctrl_common_control2cmd_rval(result);
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_hwe_set_detect_mask(unsigned long mask)
{
   int result;
   result = qdss_control_set_hwe_detect_mask(mask);
   return da_cmd_ctrl_common_control2cmd_rval(result);
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_hwe_set_trigger_mask(unsigned long mask)
{
   int result;
   result = qdss_control_set_hwe_trigger_mask(mask);
   return da_cmd_ctrl_common_control2cmd_rval(result);
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_hwe_set_reg(dapar_hwe_data_t *pDat)
{
   int result;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR,
                 "DA: cmd hwe_set_reg failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   if (!pDat->addr_valid || !pDat->value_valid)
   {
      return DA_CMD_RESP_INVALID_PARAM;
   }

   if (!da_is_chipid_validated())
   {
      return DA_CMD_RESP_FAILED;
   }

   result = QDSS_CONTROL_ERROR;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         result = qdss_control_set_hwe_reg(pDat->addr, pDat->value);
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         result = da_cmd_qmi_set_hwe_reg(pDat->ssid,
                                         pDat->addr, pDat->value);
         break;
      case DA_PARSER_SSID_RPM:
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      default:
         break;
   }
   return da_cmd_ctrl_common_control2cmd_rval(result);
}


