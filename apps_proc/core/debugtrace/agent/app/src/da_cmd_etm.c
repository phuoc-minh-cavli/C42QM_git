/*=============================================================================
  FILE: da_cmd_etm.c

  OVERVIEW:     App interface for etm commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_etm.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main.h"
#include "da_parser.h"
#include "da_cmd_etm.h"

#include "qdss_control.h"
#include "da_cmd_qmi.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_etm_get_state(dapar_etm_data_t *pDat)
{
   int result;
   uint8 state;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd etm_get failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = QDSS_CONTROL_ERROR;
   state = 0;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         result = qdss_control_get_etm(&state);
         break;
      case DA_PARSER_SSID_MODEM: // fall thru
      case DA_PARSER_SSID_ADSP:  // fall thru
      case DA_PARSER_SSID_WCNSS:
         result = da_cmd_qmi_get_etm(pDat->ssid, &state);
         break;
      case DA_PARSER_SSID_RPM:
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      default:
         break;
   }
   pDat->state = state;
   return (QDSS_CONTROL_SUCCESS == result) ?
           DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;

}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_etm_set_state(dapar_etm_data_t *pDat)
{
   int result;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd etm_set failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = QDSS_CONTROL_ERROR;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         result = qdss_control_set_etm(pDat->state);  // also sets funnel
         break;
      case DA_PARSER_SSID_MODEM:
         result = da_cmd_qmi_set_etm(pDat->ssid, pDat->state);
         if (DA_CMD_RESP_SUCCESS == result)
         {
            result = qdss_control_set_funnel(TFUNNEL_MPSS_PORT, pDat->state);
         }
         break;
      case DA_PARSER_SSID_ADSP:
         result = da_cmd_qmi_set_etm(pDat->ssid, pDat->state);
         if (QDSS_CONTROL_SUCCESS == result)
         {
            result = qdss_control_set_funnel(TFUNNEL_ADSP_PORT, pDat->state);
         }
         break;
      case DA_PARSER_SSID_WCNSS:
         result = da_cmd_qmi_set_etm(pDat->ssid, pDat->state);
         if (QDSS_CONTROL_SUCCESS == result)
         {
            result = qdss_control_set_funnel(TFUNNEL_PRONTO_PORT,
                                             pDat->state);
         }
         break;
      case DA_PARSER_SSID_RPM:
         result = da_cmd_qmi_set_rpm_etm(pDat->state);
         if (QDSS_CONTROL_SUCCESS == result)
         {
            result = qdss_control_set_funnel(TFUNNEL_RPM_PORT,
                                             pDat->state);
         }
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      default:
         break;
   }
   return (QDSS_CONTROL_SUCCESS == result) ?
           DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_etm_get_config(dapar_etm_data_t *pDat)
{
   int result;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd etm_get_config failed, "
                                   "data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = QDSS_CONTROL_ERROR;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_MODEM:    // Fall through
      case DA_PARSER_SSID_ADSP:
         result = da_cmd_qmi_get_etm_config(pDat->ssid,
                     pDat->cmd_str,
                     pDat->settings_str, sizeof(pDat->settings_str));
         break;
      case DA_PARSER_SSID_APPS:     // Fall through
      case DA_PARSER_SSID_WCNSS:    // Fall through
      case DA_PARSER_SSID_RPM:      // Fall through
      case DA_PARSER_SSID_VENUS:    // Fall through
      case DA_PARSER_SSID_SENSOR:   // Fall through
      case DA_PARSER_SSID_AUDIO:    // Fall through
      case DA_PARSER_SSID_GNSS:     // Fall through
      default:
         return DA_CMD_RESP_UNSUPPORTED;
   }

   return (QDSS_CONTROL_SUCCESS == result) ?
           DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_etm_set_config(dapar_etm_data_t *pDat)
{
   int result;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd etm_set_config failed, "
                                   "data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = QDSS_CONTROL_ERROR;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_MODEM:   // Fall through
      case DA_PARSER_SSID_ADSP:    // Fall through
         result = da_cmd_qmi_set_etm_config(pDat->ssid, pDat->cmd_str);
         break;
      case DA_PARSER_SSID_APPS:     // Fall through
      case DA_PARSER_SSID_WCNSS:    // Fall through
      case DA_PARSER_SSID_RPM:      // Fall through
      case DA_PARSER_SSID_VENUS:    // Fall through
      case DA_PARSER_SSID_SENSOR:   // Fall through
      case DA_PARSER_SSID_AUDIO:    // Fall through
      case DA_PARSER_SSID_GNSS:     // Fall through
      default:
         break;
   }

   return (QDSS_CONTROL_SUCCESS == result) ?
           DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED;
}

