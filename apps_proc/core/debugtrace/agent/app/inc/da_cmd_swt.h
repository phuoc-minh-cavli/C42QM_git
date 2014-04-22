#ifndef DA_CMD_SWT_H
#define DA_CMD_SWT_H
/**
  File: da_cmd_swt.h

  Debug Agent - Interface for SWT commands.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_swt.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  007/08/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_cmd.h"
#include "dapar_swt.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_swt_ss_get_state(dapar_swt_data_t *pDat);
da_cmd_resp_t da_cmd_swt_ss_set_state(dapar_swt_data_t *pDat);

da_cmd_resp_t da_cmd_swt_ss_get_entity(dapar_swt_data_t *pDat);
da_cmd_resp_t da_cmd_swt_ss_set_entity(dapar_swt_data_t *pDat);

da_cmd_resp_t da_cmd_swt_ss_get_event(dapar_swt_data_t *pDat);
da_cmd_resp_t da_cmd_swt_ss_set_event(dapar_swt_data_t *pDat);

da_cmd_resp_t da_cmd_swt_ss_get_event_tag(dapar_swt_data_t *pDat);

#endif /* #ifndef DA_CMD_SWT_H */

