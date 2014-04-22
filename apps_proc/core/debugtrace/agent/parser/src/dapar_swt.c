/*=============================================================================
  FILE: dapar_swt.c

  OVERVIEW:     Debug Agent - process SW tracer (swt) instructions.

  DESCRIPTION:
    Software trace instructions (SWT)

    SWT <processor> get
    SWT <processor> set enable|disable
    SWT <processor> <entity id> get
    SWT <processor> <entity id> set enable|disable
    SWT <processor> <entity id> ext swe get
    SWT <processor> <entity id> ext swe set enable|disable

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_swt.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "da_main.h"
#include "da_parser.h"
#include "dapar_swt.h"
#include "da_cmd_swt.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Argument da_parser_info_t *pPrsr must point to a defined structure.

da_parser_rval_t _handler_swt_ss_reset(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_ss_get(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_ss_set(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_ss_enable(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_ss_disable(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_swt_ss_get_state(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_ss_get_event_id(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_ss_set_event_id(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_swt_rpm_set_event_dest(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_rpm_set_event_group(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_swt_rpm_set_event_mask(da_parser_info_t *pPrsr);

//=========================================================================//

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Parse tables.
// Param 3
static const da_parser_table_t ptable_swt_ss[] =
{
   {"reset",      _handler_swt_ss_reset},
   {"get",        _handler_swt_ss_get},
   {"set",        _handler_swt_ss_set},
   {"enable",     _handler_swt_ss_enable},
   {"disable",    _handler_swt_ss_disable},
   {"", NULL}
};
// Param 4
static const da_parser_table_t ptable_swt_ss_get[] =
{
   {"state",      _handler_swt_ss_get_state},
   {"", NULL}
};
// Param 5
static const da_parser_table_t ptable_swt_ss_get_event[] =
{
   {"id",         _handler_swt_ss_get_event_id},
   {"", NULL}
};
// Param 5
static const da_parser_table_t ptable_swt_ss_set_event[] =
{
   {"id",         _handler_swt_ss_set_event_id},
   {"", NULL}
};
static const da_parser_table_t ptable_swt_rpm_set_event[] =
{
   {"destination", _handler_swt_rpm_set_event_dest},
   {"group",       _handler_swt_rpm_set_event_group},
   {"mask",        _handler_swt_rpm_set_event_mask},
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
   pDat = (dapar_swt_data_t *)pPrsr->pData

/*=========================================================================*/
// Parses param for entity name; sets entity ID value in pPrsr->pData.
da_parser_rval_t _swt_parse_entity(da_parser_info_t *pPrsr)
{
   dapar_swt_data_t *pDat;
   char *pParam;
   tracer_ost_entity_id_enum_t entid;
   da_parser_rval_t rval;

   _SET_PDAT(swt_parse_entity);
   pParam = NULL;
   entid = TRACER_ENTITY_NONE;
   rval = da_parser_get_param(pPrsr, &pParam);

   if (NULL != pParam && DA_PARSER_RVAL_SUCCESS == rval)
   {  // All acceptable entities names used in instructions
      if      (!strcasecmp(pParam, "none"))  { entid = TRACER_ENTITY_NONE; }
      else if (!strcasecmp(pParam, "tds"))   { entid = TRACER_ENTITY_TDS;   }
      else if (!strcasecmp(pParam, "event")) { entid = TRACER_ENTITY_SWEVT; }
      else if (!strcasecmp(pParam, "ulog"))  { entid = TRACER_ENTITY_ULOG;  }
      else if (!strcasecmp(pParam, "prof"))  { entid = TRACER_ENTITY_PROF;  }
      else if (!strcasecmp(pParam, "diag"))  { entid = TRACER_ENTITY_DIAG;  }
      else if (!strcasecmp(pParam, "all"))   { entid = TRACER_ENTITY_ALL;  }
      else
      {
         rval = DA_PARSER_RVAL_PARAM_INVALID;
      }
   }
   pDat->entid = entid;
   da_out_msg(DA_OUT_LEVEL_DEBUG, "EntityID=%X", entid);
   return rval;
}

/*=========================================================================*/
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_rpm_set_event_dest(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_swt_data_t *pDat;
   int dest;

   _SET_PDAT(swt_rpm_set_event_dest);

   rval = da_parser_unsigned(pPrsr, &dest, sizeof(dest));
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->rpmevt_dest_valid = 1;
      pDat->rpmevt_dest = dest;
   }
   else
   {
      pDat->rpmevt_dest_valid = 0;
   }
   pPrsr->curParamIdx++;
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_rpm_set_event_group(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_swt_data_t *pDat;
   int group;

   _SET_PDAT(swt_rpm_set_event_group);

   rval = da_parser_unsigned(pPrsr, &group, sizeof(group));
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->rpmevt_group_valid = 1;
      pDat->rpmevt_group = group;
   }
   else
   {
      pDat->rpmevt_group_valid = 0;
   }
   pPrsr->curParamIdx++;
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_rpm_set_event_mask(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_swt_data_t *pDat;
   uint64 mask;

   _SET_PDAT(swt_rpm_set_event_mask);

   rval = da_parser_unsigned(pPrsr, &mask, sizeof(mask));
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->rpmevt_mask_valid = 1;
      pDat->rpmevt_mask = mask;
   }
   else
   {
      pDat->rpmevt_mask_valid = 0;
   }
   pPrsr->curParamIdx++;
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_ss_set(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_set);

   // Parse the entity value; stored in pDat->entid.
   rval = _swt_parse_entity(pPrsr);
   if (DA_PARSER_RVAL_SUCCESS != rval)
   {
      return rval;
   }
   pPrsr->curParamIdx++;   // Done processing param; prep for next one.

   if (DA_PARSER_SSID_RPM == pDat->ssid &&
       TRACER_ENTITY_SWEVT == pDat->entid)
   {  // Extended instructions: config RPM's SW events
      do
      {  // Get all the parameters needed to set RPM's SW events.
         rval = da_parser_param(pPrsr, ptable_swt_rpm_set_event);
      }
      while (DA_PARSER_RVAL_SUCCESS == rval);

      if (pDat->rpmevt_dest_valid && pDat->rpmevt_group_valid &&
          pDat->rpmevt_mask_valid)
      {  // Have all the parameters needed.
         if (DA_CMD_RESP_SUCCESS == da_cmd_swt_ss_set_event(pDat))
         {
            da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: RPM event configured");
            rval= DA_PARSER_RVAL_SUCCESS;
         }
         else
         {
            da_out_msg(DA_OUT_LEVEL_ERR, "DA: RPM event configuration failed");
            rval = DA_PARSER_RVAL_CMD_FAILED;
         }
      }
      else
      {
         rval = DA_PARSER_RVAL_PARAM_MISSING;
      }
   }
   else
   {  // Invalid instruction.
      rval = DA_PARSER_RVAL_FORMAT_ERR;
   }
   return rval;

}
/*=========================================================================*/
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_ss_set_event_id(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_cmd_resp_t result;
   uint32 evtid;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_set_event_id);
   rval = da_parser_unsigned(pPrsr, &evtid, sizeof(evtid));
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->evtid = evtid;
      result = da_cmd_swt_ss_set_event(pDat);
      if (DA_CMD_RESP_SUCCESS == result)
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X SW event 0x%X %s",
              pDat->ssid, pDat->evtid, pDat->state[0]?"enabled":"disabled");
         return DA_PARSER_RVAL_SUCCESS;
      }
      else
      {
         return DA_PARSER_RVAL_CMD_FAILED;
      }
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t __handler_swt_ss_set_entity(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_set_entity);

   if (pPrsr->paramCnt == pPrsr->curParamIdx)
   {  // End of instruction
      result = da_cmd_swt_ss_set_entity(pDat);

      if (DA_CMD_RESP_SUCCESS == result)
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X SW entity %d %s",
              pDat->ssid, pDat->entid, pDat->state[0]?"enabled":"disabled");
         return DA_PARSER_RVAL_SUCCESS;
      }
      else
      {
         da_out_msg(DA_OUT_LEVEL_ERR, "DA: SS%X SW entity%d %s failed",
                 pDat->ssid, pDat->entid, pDat->state[0]?"enabled":"disabled");
         return DA_PARSER_RVAL_CMD_FAILED;
      }
   }
   else
   {  // Extended instruction, continue parsing
      if (TRACER_ENTITY_SWEVT == pDat->entid)
      {
         return da_parser_param(pPrsr, ptable_swt_ss_set_event);
      }
      else
      {
         return DA_PARSER_RVAL_PARAM_INVALID;
      }
   }
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _swt_cmd_ss_set_state(da_parser_info_t *pPrsr, uint32 state)
{
   da_cmd_resp_t result;
   da_parser_rval_t rval;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_set_state);

   pDat->state[0] = state;

   if (pPrsr->paramCnt == pPrsr->curParamIdx)
   {
       result = da_cmd_swt_ss_set_state(pDat);

       if (DA_CMD_RESP_SUCCESS == result)
       {
          da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X SWT %s",
                  pDat->ssid, pDat->state[0]?"enabled":"disabled");
          return DA_PARSER_RVAL_SUCCESS;
       }
       else
       {
          da_out_msg(DA_OUT_LEVEL_ERR, "DA: SS%X SWT %s failed",
                  pDat->ssid, pDat->state[0]?"enable":"disable");
          return DA_PARSER_RVAL_CMD_FAILED;
       }
   }
   else
   {  // Continue parsing of param 4
      rval = _swt_parse_entity(pPrsr);
      if (DA_PARSER_RVAL_SUCCESS != rval)
      {
         return rval;
      }
      pPrsr->curParamIdx++;
      return (__handler_swt_ss_set_entity(pPrsr));
   }
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_ss_enable(da_parser_info_t *pPrsr)
{
   return _swt_cmd_ss_set_state(pPrsr, 1);
}

/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_ss_disable(da_parser_info_t *pPrsr)
{
   return _swt_cmd_ss_set_state(pPrsr, 0);
}
/*=========================================================================*/
da_parser_rval_t _handler_swt_ss_get_event_id(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   uint32 evtid;
   da_parser_rval_t rval;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_get_event_id);

   rval = da_parser_unsigned(pPrsr, &evtid, sizeof(evtid));

   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->evtid = evtid;
      result = da_cmd_swt_ss_get_event(pDat);

      if (DA_CMD_RESP_SUCCESS == result)
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X SW envent0x%X=%d TO DO",
              pDat->ssid, pDat->evtid, pDat->state[0]);
         return DA_PARSER_RVAL_SUCCESS;
      }
      else
      {
         return DA_PARSER_RVAL_CMD_FAILED;
      }
   }

   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t __handler_swt_ss_get_entity(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_get_entity);

   if (pPrsr->paramCnt == pPrsr->curParamIdx)
   {  // End of instruction
      result = da_cmd_swt_ss_get_entity(pDat);

      if (DA_CMD_RESP_SUCCESS == result)
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X SW entity0x%X=%d",
            pDat->ssid, pDat->entid, pDat->state[0]);
         return DA_PARSER_RVAL_SUCCESS;
      }
      else
      {
         da_out_msg(DA_OUT_LEVEL_BRIEF,
            "DA: SS%X get SW entity state failed",
            pDat->ssid, pDat->entid);
         return DA_PARSER_RVAL_CMD_FAILED;
      }
   }
   else
   {  // Extended instruction, continue parsing
      if (TRACER_ENTITY_SWEVT == pDat->entid)
      {
         return da_parser_param(pPrsr, ptable_swt_ss_get_event);
      }
      else
      {
         return DA_PARSER_RVAL_PARAM_INVALID;
      }
   }
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_ss_get_state(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_get_state);

   result = da_cmd_swt_ss_get_state(pDat);

   if (DA_CMD_RESP_SUCCESS == result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%X SWT=%d TO DO",
           pDat->ssid, pDat->state[0]);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_swt_ss_get(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   int hold_idx;

   // Go process parameter 4 (keywords)
   hold_idx = pPrsr->curParamIdx;
   rval = da_parser_param(pPrsr, ptable_swt_ss_get);
   if ((DA_PARSER_RVAL_SUCCESS == rval) ||
       (DA_PARSER_RVAL_CMD_FAILED == rval)
      )
   {  // Done.
      return rval;
   }

   // Check if still on param4
   if ( (DA_PARSER_RVAL_PARAM_INVALID == rval) &&
        (hold_idx == pPrsr->curParamIdx) )
   {  // Process entity id.
      rval = _swt_parse_entity(pPrsr);
      if (DA_PARSER_RVAL_SUCCESS != rval)
      {
         return rval;
      }
      pPrsr->curParamIdx++;
      return (__handler_swt_ss_get_entity(pPrsr));
   }
   return rval;
}
/*=========================================================================*/
// Reset SWT configuration with everything disabled:
// 1) tracer output of all sw instrumented trace is disabled.
// 2) each entity's output is disabled.
// 3) each event ID's output is disabled.
da_cmd_resp_t _swt_ss_reset_all(dapar_swt_data_t *pDat)
{
   int ii;
   for (ii = 1; ii < DA_PARSER_SSID_LAST; ii++)
   {  //Try all subsystems
      pDat->ssid = (DA_PARSER_SSID_APPS  == ii) ? DA_PARSER_SSID_APPS  :
                   (DA_PARSER_SSID_MODEM == ii) ? DA_PARSER_SSID_MODEM :
                   (DA_PARSER_SSID_WCNSS == ii) ? DA_PARSER_SSID_WCNSS :
                   (DA_PARSER_SSID_RPM   == ii) ? DA_PARSER_SSID_RPM   :
                   (DA_PARSER_SSID_ADSP  == ii) ? DA_PARSER_SSID_ADSP  :
                   (DA_PARSER_SSID_VENUS == ii) ? DA_PARSER_SSID_VENUS :
                   (DA_PARSER_SSID_GNSS  == ii) ? DA_PARSER_SSID_GNSS  :
                   (DA_PARSER_SSID_SENSOR== ii) ? DA_PARSER_SSID_SENSOR:
                   (DA_PARSER_SSID_AUDIO == ii) ? DA_PARSER_SSID_AUDIO :
                   (DA_PARSER_SSID_VPU   == ii) ? DA_PARSER_SSID_VPU   :
                                                  DA_PARSER_SSID_RSV;
      da_cmd_swt_ss_set_state(pDat);
      da_cmd_swt_ss_set_entity(pDat);
      da_cmd_swt_ss_set_event(pDat);
   }
   // Always return success. Subsystems may or may not be active.
   return DA_CMD_RESP_SUCCESS;
}
da_parser_rval_t _handler_swt_ss_reset(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   dapar_swt_data_t *pDat;

   _SET_PDAT(swt_ss_reset);
   pDat->entid = TRACER_ENTITY_ALL;
   pDat->evtid = 0xFFFFFFFF;
   pDat->state[0] = 0;  // Disable.
   pDat->rpmevt_dest = 0;
   pDat->rpmevt_mask = 0;
   pDat->rpmevt_group = 0;
   pDat->rpmevt_dest_valid = 1;
   pDat->rpmevt_group_valid = 1;
   pDat->rpmevt_mask_valid = 1;

   switch (pDat->ssid)
   {
      case DA_PARSER_SSID_ALL:
         result = _swt_ss_reset_all(pDat);
         break;
      case DA_PARSER_SSID_RPM:
         result = da_cmd_swt_ss_set_event(pDat);
         break;
      default:
         result = da_cmd_swt_ss_set_state(pDat);
         if (DA_CMD_RESP_SUCCESS == result)
         {
            result = da_cmd_swt_ss_set_entity(pDat);
            if (DA_CMD_RESP_SUCCESS == result)
            {
               // Don't report as failure when SW events not in use.
               // Other failures will be caught by previous two calls.
               da_cmd_swt_ss_set_event(pDat);
            }
         }
         break;
   }
   return (DA_CMD_RESP_SUCCESS == result) ?
           DA_PARSER_RVAL_SUCCESS : DA_PARSER_RVAL_CMD_FAILED;
}
/*---------------------------------------------------------------------------
 * External Function Definitions
 * ------------------------------------------------------------------------*/
da_parser_rval_t dapar_swt_handler(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_parser_ssid_t ssid;
   dapar_swt_data_t swt_data;

   memset (&swt_data, 0, sizeof (swt_data));

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr swt, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
   pPrsr->pData = (void*)&swt_data;

   // Process parameter 2 (subsystem)
   rval = da_parser_subsys(pPrsr, &ssid);
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      swt_data.ssid = ssid;
      da_out_msg(DA_OUT_LEVEL_DEBUG, "SSID=%X", ssid);

      // Process parameter 3
      pPrsr->curParamIdx++;
      rval = da_parser_param(pPrsr, ptable_swt_ss);
   }
   pPrsr->pData = NULL;
   return rval;
}

