#ifndef DAPAR_STM_H
#define DAPAR_STM_H
/**
  @file dapar_stm.h

  Debug Agent - process stm instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_stm.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_stm_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_STM_H */

