#ifndef DA_CMD_INFO_H
#define DA_CMD_INFO_H
/**
  File: da_cmd_info.h

  Debug Agent - Interface for info commands.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_info.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  007/08/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "da_cmd.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_info_get_version(uint32 *version);
da_cmd_resp_t da_cmd_info_get_chipid(char *chip_id_name, int len);

#endif /* #ifndef DA_CMD_INFO_H */

