#ifndef DAPAR_ETM_H
#define DAPAR_ETM_H
/**
  @file dapar_etm.h

  Debug Agent - process etm instructions.
*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_etm.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// ETM instruction parsed data
typedef struct dapar_etm_data_s
{
   da_parser_ssid_t ssid;
   uint8 state;         // 0 = disable, 1 = enable
   char *cmd_str   ;    // Config ETM options. Null terminated string.
   char settings_str[128];
} dapar_etm_data_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_etm_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_ETM_H */

