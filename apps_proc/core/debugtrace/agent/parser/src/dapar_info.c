/*=============================================================================
  FILE: dapar_info.c

  OVERVIEW:     Debug Agent - Process info instructions.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_info.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "DDIChipInfo.h"

#include "da_main.h"
#include "da_parser.h"
#include "dapar_info.h"
#include "dapar_swt.h"
#include "da_cmd_info.h"
#include "da_cmd_swt.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Definitions
 * ------------------------------------------------------------------------*/


da_parser_rval_t dapar_info_h(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_info_get(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_check(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_info_get_version(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_get_chipid(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_get_sinks(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_get_tag(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_info_check_chipid(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_check_tag(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_info_get_tag_ss_type(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_check_tag_ss_type(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_info_get_tag_ss_swe(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_info_check_tag_ss_swe(da_parser_info_t *pPrsr);

//=========================================================================//

typedef struct _info_data_s
{
   da_parser_ssid_t  ssid;
} _info_data_t;

//=========================================================================//

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Param 2
static const da_parser_table_t ptable_info[] =
{
   {"get",        _handler_info_get},
   {"check",      _handler_info_check},     // Validate info
   {"", NULL}
};
// Param 3 - info get
static const da_parser_table_t ptable_info_get[] =
{
   {"version",    _handler_info_get_version},
   {"chipid",     _handler_info_get_chipid},
   {"sinks",      _handler_info_get_sinks},
   {"tag",        _handler_info_get_tag},
   {"", NULL}
};
// Param 3
static const da_parser_table_t ptable_info_check[] =
{
   {"chipid",     _handler_info_check_chipid},
   {"tag",        _handler_info_check_tag},
   {"", NULL}
};

// Param 5 - info get tag subsys
static const da_parser_table_t ptable_info_get_tag_ss[] =
{
   {"type",       _handler_info_get_tag_ss_type},
   {"", NULL}
};
// Param 5 - info check tag subsys
static const da_parser_table_t ptable_info_check_tag_ss[] =
{
   {"type",       _handler_info_check_tag_ss_type},
   {"", NULL}
};
// Param 6 - info get tag subsys type
static const da_parser_table_t ptable_info_get_tag_ss_type[] =
{
   {"event",      _handler_info_get_tag_ss_swe},
   {"", NULL}
};
// Param 6 - info check tag subsys type
static const da_parser_table_t ptable_info_check_tag_ss_type[] =
{
   {"event",      _handler_info_check_tag_ss_swe},
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
   pDat = (_info_data_t *)pPrsr->pData

/*=========================================================================*/
da_parser_rval_t _handler_info_check_chipid(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_cmd_resp_t result;
   char *my_chip_str_ptr;
   char dev_chip_buf[DALCHIPINFO_MAX_ID_LENGTH+1];
      // Added byte to guarantee NULL terminated.

   my_chip_str_ptr = NULL;
   memset (dev_chip_buf, 0, sizeof(dev_chip_buf));

   // Process parameter 4 (chip id name)
   rval = da_parser_get_param(pPrsr, &my_chip_str_ptr);
   if (NULL != my_chip_str_ptr && DA_PARSER_RVAL_SUCCESS == rval)
   {
      // Get chip ID string of device.
      rval = DA_PARSER_RVAL_CMD_FAILED;
      result = da_cmd_info_get_chipid(dev_chip_buf, sizeof(dev_chip_buf)-1);
      if (DA_CMD_RESP_SUCCESS == result)
      {
         // Check the instruction tag with the device's tag
         if (strlen(my_chip_str_ptr) == strlen(dev_chip_buf))
         {
            if (!strcmp(my_chip_str_ptr, dev_chip_buf))
            {
               rval = DA_PARSER_RVAL_SUCCESS;
            }
         }
      }
   }
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      da_set_chipid_validated(1); // Record that chipid is validated.
      // Should the device tag be revealed?
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: Chip ID is valid, %s",
                 my_chip_str_ptr);
   }
   else
   {
      da_set_chipid_validated(0); // Record that chipid is not validated.
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: Check chip ID failed, %s",
                 my_chip_str_ptr);
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_info_check_tag_ss_swe(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_cmd_resp_t result;
   int bMatch;
   _info_data_t *pDat;
   char *my_tag;
   dapar_swt_data_t swt_data;

   _SET_PDAT(info_ck_tag_ss_swe);
   my_tag = NULL;
   memset (&swt_data, 0, sizeof (swt_data));
   result = DA_CMD_RESP_FAILED;

   // Process parameter 7 (tag value)
   rval = da_parser_get_param(pPrsr, &my_tag);
   if (NULL != my_tag && DA_PARSER_RVAL_SUCCESS == rval)
   {
      // Get tag of device.
      swt_data.ssid = pDat->ssid;
      result = da_cmd_swt_ss_get_event_tag(&swt_data);
      if (DA_CMD_RESP_SUCCESS == result)
      {
         bMatch = 0;
         // Check the instruction tag with the device's tag
         if (strlen(my_tag) == sizeof(swt_data.event_tag))
         {
            if (!strncmp(my_tag, swt_data.event_tag,
                         sizeof(swt_data.event_tag)))
            {
               bMatch = 1;
            }
         }
         // Record the result of the tag validation
         if (0 == da_set_swevent_tag_validated(swt_data.ssid, bMatch))
         {
            if (1 == bMatch)
            {
               rval = DA_PARSER_RVAL_SUCCESS;
            }
            else
            {  // Did not match. Report check failed.
               rval = DA_PARSER_RVAL_CMD_FAILED;
            }
         }
         else
         {
            rval = DA_PARSER_RVAL_INTERNAL_ERR;
         }
      }
      else
      {
         rval = DA_PARSER_RVAL_CMD_FAILED;
      }
   }
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: SS%d swevt tag is valid %s",
                 pDat->ssid, my_tag);
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: Check SS%d swevt tag failed %s.",
                 pDat->ssid, my_tag);
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_info_check_tag_ss_type(da_parser_info_t *pPrsr)
{
   return da_parser_param(pPrsr, ptable_info_check_tag_ss_type);
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_info_check_tag(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   da_parser_ssid_t ssid;
   _info_data_t *pDat;

   _SET_PDAT(info_check_tag);

   // Process parameter 4 (subsystem)
   rval = da_parser_subsys(pPrsr, &ssid);
   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      pDat->ssid = ssid;
      da_out_msg(DA_OUT_LEVEL_DEBUG, "SSID=%X", ssid);

      // Process parameter 5
      pPrsr->curParamIdx++;
      rval = da_parser_param(pPrsr, ptable_info_check_tag_ss);
   }
   return rval;
}
/*=========================================================================*/
da_parser_rval_t _handler_info_check(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;
   rval = da_parser_param(pPrsr, ptable_info_check);
   return rval;
}

/*=========================================================================*/
da_parser_rval_t _handler_info_get_tag_ss_swe(da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;

   // TO DO make call to SS to get the its SW event tag.
   // da_cmd_resp_t da_cmd_swt_ss_get_event_tag(dapar_swt_data_t *pDat);

   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: Get SS=%d swevent tag **To Do**",
        ((struct _info_data_s*)pPrsr->pData)->ssid);
   rval = DA_PARSER_RVAL_CMD_FAILED;
   return rval;
}

/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_info_get_tag_ss_type(da_parser_info_t *pPrsr)
{
   return da_parser_param(pPrsr, ptable_info_get_tag_ss_type);
}
/*-------------------------------------------------------------------------*/
da_parser_rval_t _handler_info_get_tag(da_parser_info_t *pPrsr)
{
   return da_parser_param(pPrsr, ptable_info_get_tag_ss);
}
/*=========================================================================*/
da_parser_rval_t _handler_info_get_sinks(da_parser_info_t *pPrsr)
{
   // To do.
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: sinks are TO DO, yet");
   return DA_PARSER_RVAL_CMD_FAILED;
}

// The chip ID string is from DalChipInfoDataLUT in
// \core\systemdrivers\chipinfo\src\DalChipInfoLocal.c
//
da_parser_rval_t _handler_info_get_chipid(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   char chip_id_name[DALCHIPINFO_MAX_ID_LENGTH];

   result = da_cmd_info_get_chipid(chip_id_name, sizeof(chip_id_name));

   // TO DO, Send response back to caller.
   if (DA_CMD_RESP_SUCCESS == result)
   {
      chip_id_name[DALCHIPINFO_MAX_ID_LENGTH-1]='\0';
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: chipID=%s", chip_id_name);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
da_parser_rval_t _handler_info_get_version(da_parser_info_t *pPrsr)
{
   da_cmd_resp_t result;
   uint32 packed_ver;
   result = da_cmd_info_get_version(&packed_ver);

   // TO DO  send QMI indication back to HOST?.

   if (DA_CMD_RESP_SUCCESS == result)
   {
      da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: ver=%d.%d.%d",
             (packed_ver >> 24) & 0xFF,
             (packed_ver >> 16) & 0xFF,
             (packed_ver      ) & 0xFFFF);
      return DA_PARSER_RVAL_SUCCESS;
   }
   else
   {
      return DA_PARSER_RVAL_CMD_FAILED;
   }
}
/*=========================================================================*/
da_parser_rval_t _handler_info_get(da_parser_info_t *pPrsr)
{
   return da_parser_param(pPrsr, ptable_info_get);
}

/*=========================================================================*/
da_parser_rval_t dapar_info_handler(da_parser_info_t *pPrsr)
{
   _info_data_t info_data;

   memset (&info_data, 0, sizeof (info_data));

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* prsr info, no context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }
   pPrsr->pData = (void*)&info_data;

   return da_parser_param(pPrsr, ptable_info);
}
