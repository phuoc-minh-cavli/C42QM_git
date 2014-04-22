#ifndef DAPAR_ACTION_H
#define DAPAR_ACTION_H
/**
  @file dapar_action.h

  Debug Agent - process "action" instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_action.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

extern da_parser_rval_t dapar_action_reset_handler(da_parser_info_t *pPrsr);
extern da_parser_rval_t dapar_action_startup_handler(da_parser_info_t *pPrsr);
extern da_parser_rval_t dapar_action_delay_handler(da_parser_info_t *pPrsr);
extern da_parser_rval_t dapar_action_end_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_ACTION_H */

