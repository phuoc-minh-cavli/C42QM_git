#ifndef DAPAR_HWE_H
#define DAPAR_HWE_H
/**
  @file dapar_hwe.h

  Debug Agent - process hwe instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_hwe.h#1 $

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

typedef struct da_parser_hwe_data_s
{
   da_parser_ssid_t ssid;
   uint8 state;
   int addr_valid;
   unsigned long addr;
   int value_valid;
   unsigned long value;

} dapar_hwe_data_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_hwe_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_HWE_H */

