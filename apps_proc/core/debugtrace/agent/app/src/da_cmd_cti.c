/*=============================================================================
  FILE: da_cmd_cti.c

  OVERVIEW:     App interface for CTI commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_cti.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  11/18/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main.h"
#include "da_parser.h"
#include "da_cmd_cti.h"
#include "qdss_control.h"

/*=========================================================================*
 * CTI commands
 *=========================================================================*/

da_cmd_resp_t da_cmd_cti_reset(dapar_cti_data_t *pDat)
{
   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd cti_reset failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   return qdss_control_reset_cti(pDat->block_name);
}

/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_cti_map(dapar_cti_data_t *pDat)
{
   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd cti_map failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   if (pDat->block_len && pDat->type_valid &&
       pDat->trigger_num_valid && pDat->channel_num_valid)
   {
      return qdss_control_map_cti(pDat->block_name, pDat->type,
                                  pDat->trigger_num, pDat->channel_num);
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd cti_map failed, data missing");
      return DA_CMD_RESP_INVALID_PARAM;
   }
}

/*-------------------------------------------------------------------------*/
da_cmd_resp_t da_cmd_cti_unmap(dapar_cti_data_t *pDat)
{
   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd cti_unmap failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   if (pDat->block_len && pDat->type_valid &&
       pDat->trigger_num_valid && pDat->channel_num_valid)
   {
      return qdss_control_unmap_cti(pDat->block_name, pDat->type,
                                    pDat->trigger_num, pDat->channel_num);
   }
   else
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd cti_unmap failed, data missing");
      return DA_CMD_RESP_INVALID_PARAM;
   }

}

/*-------------------------------------------------------------------------*/
//#define DA_CMD_CTI_MAX_TRIGGERS  8
da_cmd_resp_t da_cmd_cti_get(dapar_cti_data_t *pDat)
{
//   unsigned long mappings[DA_CMD_CTI_MAX_TRIGGERS]; // returned results
//   unsigned char length;   // returned number of triggers

   if (NULL == pDat)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: cmd cti_reset failed, data not provided");
      return DA_CMD_RESP_FAILED;
   }

   //return qdss_control_query_cti(pDat->block_name, pDat->type,
   //                              DA_CMD_CTI_MAX_TRIGGERS, &
   //                              &length, &mappings);

   // To do, processing of the returned info.
   return DA_CMD_RESP_UNSUPPORTED;
}

