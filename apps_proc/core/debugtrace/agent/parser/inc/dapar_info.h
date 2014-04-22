#ifndef DAPAR_INFO_H
#define DAPAR_INFO_H
/**
  @file dapar_info.h

  Debug Agent - process information (info) instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_info.h#1 $

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
// Software trace instruction parsed data.
typedef struct dapar_info_data_s
{
   da_parser_ssid_t ssid;
} dapar_info_data_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_info_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_INFO_H */

