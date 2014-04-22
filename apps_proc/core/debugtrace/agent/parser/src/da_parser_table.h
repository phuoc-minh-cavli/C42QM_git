#ifndef DA_PARSER_TABLE_H
#define DA_PARSER_TABLE_H
/**
  File: da_parser_table.h

  Debug Agent parser - Definition of content to be parsed. Add
  top-level parameter to the table along with the parameter's
  handler function. Include the header file for the handler
  function declaration and separately add source file for the
  handler function definition.

  This is a sub-file to da_parser.c to isolate construction of
  parsing table info.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/da_parser_table.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Function definitions
 * ------------------------------------------------------------------------*/

// Definition of top level, parameter 1 handlers functions.
#include "dapar_action.h"
#include "dapar_cti.h"
#include "dapar_etm.h"
#include "dapar_hwe.h"
#include "dapar_info.h"
#include "dapar_odl.h"
#include "dapar_sink.h"
#include "dapar_stm.h"
#include "dapar_swt.h"

// Head parsing table - All possible first parameters.
static const da_parser_table_t da_parser_table_head[] =
{
// {"Parameter name", _handler_function}
   {"info",    dapar_info_handler},
   {"reset",   dapar_action_reset_handler},
   {"startup", dapar_action_startup_handler},
   {"delay",   dapar_action_delay_handler},
   {"end",     dapar_action_end_handler},
   {"sink",    dapar_sink_handler}, /* Tracce sink */
   {"etm",     dapar_etm_handler},  /* Processor traces */
   {"stm",     dapar_stm_handler},  /* HW and SW instrumented trace */
   {"hwe",     dapar_hwe_handler},  /* HW event trace */
   {"cti",     dapar_cti_handler},  /* Cross trigger interface */
   {"swt",     dapar_swt_handler},  /* SW trace */
   {"odl",     dapar_odl_handler},  /* On-device logging */
   {"", NULL}
};

#endif /* #ifndef DA_PARSER_TABLE_H */

