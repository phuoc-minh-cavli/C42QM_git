#ifndef DA_CMD_CTI_H
#define DA_CMD_CTI_H
/**
  File: da_cmd_cti.h

  Debug Agent - Interface for CTI commands.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_cti.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  11/20/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_cmd.h"
#include "dapar_cti.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_cti_reset(dapar_cti_data_t *pDat);
da_cmd_resp_t da_cmd_cti_map(dapar_cti_data_t *pDat);
da_cmd_resp_t da_cmd_cti_unmap(dapar_cti_data_t *pDat);
da_cmd_resp_t da_cmd_cti_get(dapar_cti_data_t *pDat);

#endif /* #ifndef DA_CMD_CTI_H */

