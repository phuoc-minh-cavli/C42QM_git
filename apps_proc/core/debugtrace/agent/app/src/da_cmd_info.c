/*=============================================================================
  FILE: da_cmd_info.c

  OVERVIEW:     App interface for info commands.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_info.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "da_main.h"
#include "da_parser.h"
#include "da_cmd_info.h"
#include "DDIChipInfo.h"

/*=========================================================================*
 * Info commands
 *=========================================================================*/
da_cmd_resp_t da_cmd_info_get_version(uint32 *ver_mask)
{
   *ver_mask = da_version();
   return DA_CMD_RESP_SUCCESS;
}

/*-------------------------------------------------------------------------*/
/*
The string values are from
see file \core\systemdrivers\chipinfo\src\DalChipInfoLocal.c
static const DalChipInfoDataLUTType DalChipInfoDataLUT[DALCHIPINFO_CHIPID_MAP_SIZE]
MSM8974
APQ8084
MDM9635M
*/
da_cmd_resp_t da_cmd_info_get_chipid(char *chip_id_name, int len)
{
   DALResult dal_rval;
   DALDEVICEHANDLE device_handle;

   memset(chip_id_name, 0, len);
   device_handle = 0;
   dal_rval = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &device_handle);

   if (DAL_SUCCESS == dal_rval && 0 != device_handle)
   {
      dal_rval = DalChipInfo_GetChipIdString(device_handle,
                                             chip_id_name,
                                             len);
      da_out_msg(DA_OUT_LEVEL_ERR, "DA: chipid is %s", chip_id_name);
   }
   return ((DAL_SUCCESS == dal_rval) ? DA_CMD_RESP_SUCCESS :
                                       DA_CMD_RESP_FAILED);
}

