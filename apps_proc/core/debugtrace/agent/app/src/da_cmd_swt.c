/*=============================================================================
  FILE: da_cmd_swt.c

  OVERVIEW:     App interface for SWT commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_swt.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <string.h>

#include "da_main.h"
#include "da_parser.h"
#include "da_cmd_swt.h"

// SWT commands
#include "tracer_entity.h"
#include "tracer_config.h"
#include "tracer_config_int.h"

#include "da_cmd_qmi.h"

/*=========================================================================*
 * SWT commands
 *=========================================================================*/
da_cmd_resp_t da_cmd_swt_ss_get_state(dapar_swt_data_t *pDat)
{
   da_cmd_resp_t  result;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd swt_get failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = DA_CMD_RESP_FAILED;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         pDat->state[0] = tcfg_is_op_enabled() ? 0x01 : 0;
         result = DA_CMD_RESP_SUCCESS;
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         result = da_cmd_qmi_get_swt(pDat->ssid, pDat->state);
         break;
      case DA_PARSER_SSID_RSV:
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      case DA_PARSER_SSID_RPM:
      default:
         break;
   }
   return result;
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_swt_ss_set_state(dapar_swt_data_t *pDat)
{
   tracer_cmdresp_t tcmd_rval;

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR,"DA: cmd swt_set failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   tcmd_rval = TRACER_CMDRESP_FAIL;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         tcmd_rval = tracer_cfg_op_ctrl((0 == pDat->state[0]) ?
                        TRACER_OUTPUT_DISABLE : TRACER_OUTPUT_ENABLE);
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         tcmd_rval = da_cmd_qmi_set_swt(pDat->ssid, pDat->state[0]);
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      case DA_PARSER_SSID_RPM:
      default:
         break;
   }
   if (0 == (TRACER_CMDRESP_FAIL & tcmd_rval))
   {
      da_out_msg(DA_OUT_LEVEL_DETAIL, "DA: %s SW trace %s",
                 da_parser_subsys_str(pDat->ssid),
                 (1 == pDat->state[0])? "enabled" : "disabled");
      return DA_CMD_RESP_SUCCESS;
   }
   else
   {
      return DA_CMD_RESP_FAILED;
   }
}
/*=========================================================================*
 * SWT entity commands
 *=========================================================================*/
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_swt_ss_get_entity(dapar_swt_data_t *pDat)
{
   da_cmd_resp_t result;
   uint32 *pState;    // Response. Max of 32 byte mask returned for "all"
                     // Otherwise, first byte contains state value.

   if (NULL == pDat)
   {  // Programming error, data not provided.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd entity_get failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = DA_CMD_RESP_FAILED;
   memset(pDat->state, 0, sizeof (pDat->state));
   pState = pDat->state;

   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         pState[0] = tcfg_is_entity_on(pDat->entid) ? 1 : 0 ;
         result = DA_CMD_RESP_SUCCESS;
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         if (0 == (TRACER_CMDRESP_FAIL &
             da_cmd_qmi_get_entity(pDat->ssid, pDat->entid,
                                   pState, sizeof(pDat->state))))
         {
            result = DA_CMD_RESP_SUCCESS;
         }
         else
         {
            result = DA_CMD_RESP_FAILED;
         }
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      case DA_PARSER_SSID_RPM:
      default:
         break;
   }
   return result;
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_swt_ss_set_entity(dapar_swt_data_t *pDat)
{
   tracer_cmdresp_t tcmd_rval;

   if (NULL == pDat)
   {  // Programming error
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd entity_set failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   tcmd_rval = TRACER_CMDRESP_FAIL;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         tcmd_rval = tracer_cfg_entity_ctrl(pDat->entid,
            (0 == pDat->state[0]) ? TRACER_ENTITY_OFF : TRACER_ENTITY_ON);
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         tcmd_rval = da_cmd_qmi_set_entity(pDat->ssid, pDat->entid, pDat->state[0]);
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      case DA_PARSER_SSID_RPM:
      default:
         break;
   }
   if (0 == (TRACER_CMDRESP_FAIL & tcmd_rval))
   {
      da_out_msg(DA_OUT_LEVEL_DETAIL, "DA: %s SW entity %s",
                 da_parser_subsys_str(pDat->ssid),
                 (1 == pDat->state[0])? "enabled" : "disabled");
      return DA_CMD_RESP_SUCCESS;
   }
   else
   {
      return DA_CMD_RESP_FAILED;
   }
}
/*=========================================================================*
 * SWT event commands
 *=========================================================================*/
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_swt_ss_get_event_tag(dapar_swt_data_t *pDat)
{
   da_cmd_resp_t result;

   if (NULL == pDat)
   {  // Programming error, data not provided.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd swe_get failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = DA_CMD_RESP_FAILED;
   memset (pDat->event_tag, 0, sizeof(pDat->event_tag));

   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         tcfg_cpy_event_tag(pDat->event_tag, sizeof(pDat->event_tag));
         result = DA_CMD_RESP_SUCCESS;
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         if (0 == (TRACER_CMDRESP_FAIL &
             da_cmd_qmi_get_swevt_tag(pDat->ssid, pDat->event_tag,
                                      sizeof(pDat->event_tag))))
         {
            result = DA_CMD_RESP_SUCCESS;
         }
         else
         {
            result = DA_CMD_RESP_FAILED;
         }
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      case DA_PARSER_SSID_RPM:
      default:
         break;
   }
   return result;
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_swt_ss_get_event(dapar_swt_data_t *pDat)
{
   da_cmd_resp_t result;
   uint32 state;

   if (NULL == pDat)
   {  // Programming error, data not provided.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd swe_get failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   result = DA_CMD_RESP_FAILED;
   state = 0;
   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         state = tcfg_is_event_on((tracer_event_id_t)(pDat->evtid)) ? 1 : 0;
         result = DA_CMD_RESP_SUCCESS;
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         if (0 == (TRACER_CMDRESP_FAIL &
             da_cmd_qmi_get_swevt(pDat->ssid, pDat->evtid, &state)))
         {
            result = DA_CMD_RESP_SUCCESS;
         }
         else
         {
            result = DA_CMD_RESP_FAILED;
         }
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      case DA_PARSER_SSID_RPM:
      default:
         break;
   }
   pDat->state[0] = state;
   return result;
}
/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_swt_ss_set_event(dapar_swt_data_t *pDat)
{
   tracer_cmdresp_t tcmd_rval;

   if (NULL == pDat)
   {  // Programming error
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd set swe failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   if (!da_is_swevent_tag_validated(pDat->ssid) &&
       (0xFFFFFFFF != pDat->evtid) &&
       (DA_PARSER_SSID_RPM != pDat->ssid)
      )
   {
      return DA_CMD_RESP_FAILED;
   }

   tcmd_rval = TRACER_CMDRESP_FAIL;

   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_APPS:
         tcmd_rval = tracer_event_ctrl((tracer_event_id_t)(pDat->evtid),
                     (0 == pDat->state[0]) ? TRACER_EVENT_OFF :
                     TRACER_EVENT_ON);
         break;
      case DA_PARSER_SSID_MODEM:
      case DA_PARSER_SSID_ADSP:
      case DA_PARSER_SSID_WCNSS:
         tcmd_rval = da_cmd_qmi_set_swevt(pDat->ssid, pDat->evtid,
                                       pDat->state[0]);
         break;
      case DA_PARSER_SSID_RPM:
         if (0xFFFFFFFF == pDat->evtid)
         {
            for (pDat->rpmevt_group = 0;
                 pDat->rpmevt_group < 16;
                 (pDat->rpmevt_group)++)
            {
               tcmd_rval = da_cmd_qmi_set_rpm_evt(pDat->rpmevt_dest,
                                                  pDat->rpmevt_group,
                                                  pDat->rpmevt_mask);
               if (DA_CMD_RESP_SUCCESS != tcmd_rval)
               {
                  break;
               }
            }
          }
          else
          {
             tcmd_rval = da_cmd_qmi_set_rpm_evt(pDat->rpmevt_dest,
                                            pDat->rpmevt_group,
                                            pDat->rpmevt_mask);
          }
         break;
      case DA_PARSER_SSID_VENUS:
      case DA_PARSER_SSID_SENSOR:
      case DA_PARSER_SSID_AUDIO:
      case DA_PARSER_SSID_GNSS:
      case DA_PARSER_SSID_VPU:
         break;
      case DA_PARSER_SSID_ALL:   // Invalid at this level
      default:
         break;
   }
   return ((0 == (TRACER_CMDRESP_FAIL & tcmd_rval)) ?
           DA_CMD_RESP_SUCCESS : DA_CMD_RESP_FAILED);
}


