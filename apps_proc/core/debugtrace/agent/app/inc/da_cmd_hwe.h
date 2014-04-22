#ifndef DA_CMD_HWE_H
#define DA_CMD_HWE_H
/**
  File: da_cmd_hwe.h

  Debug Agent - Interface for HW event control commands.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd_hwe.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  007/08/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "da_cmd.h"
#include "dapar_hwe.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_cmd_resp_t da_cmd_hwe_set(uint8 state);
da_cmd_resp_t da_cmd_hwe_get(uint8 *state);
da_cmd_resp_t da_cmd_hwe_set_trigger_mask(unsigned long mask);
da_cmd_resp_t da_cmd_hwe_set_detect_mask(unsigned long mask);

da_cmd_resp_t da_cmd_hwe_set_reg(dapar_hwe_data_t *pDat);

#endif /* #ifndef DA_CMD_HWE_H */

