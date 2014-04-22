#ifndef DAPAR_ODL_H
#define DAPAR_ODL_H
/**
  @file dapar_odl.h

  Debug Agent - process "odl" instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_odl.h#1 $

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

extern da_parser_rval_t dapar_odl_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_ODL_H */

