#ifndef DAPAR_SINK_H
#define DAPAR_SINK_H
/**
  @file dapar_sink.h

  Debug Agent - process sink instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_sink.h#1 $

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

// Trace sink IDs
typedef enum
{
   DAPAR_SINKID_ETB           = 0,
   DAPAR_SINKID_DDR           = 1,
   DAPAR_SINKID_TPIU_A        = 2,
   DAPAR_SINKID_TPIU_B        = 3,
   DAPAR_SINKID_USB           = 4,
   DAPAR_SINKID_USB_BUFFERED  = 5,
   DAPAR_SINKID_SD            = 6,
   DAPAR_SINKID_UNKNOWN       = 0xFF
} dapar_sink_id_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_sink_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_SINK_H */

