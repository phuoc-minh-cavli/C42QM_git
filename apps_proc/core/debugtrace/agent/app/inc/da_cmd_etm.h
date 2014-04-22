#ifndef DA_CMD_ETM_H
#define DA_CMD_ETM_H
/**
  File: da_cmd_etm.h

  Debug Agent - Interface for etmcontrol commands.
*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_etm.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  007/08/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_cmd.h"
#include "dapar_etm.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_etm_get_state(dapar_etm_data_t *pDat);
da_cmd_resp_t da_cmd_etm_set_state(dapar_etm_data_t *pDat);

da_cmd_resp_t da_cmd_etm_get_config(dapar_etm_data_t *pDat);
da_cmd_resp_t da_cmd_etm_set_config(dapar_etm_data_t *pDat);

#endif /* #ifndef DA_CMD_ETM_H */

