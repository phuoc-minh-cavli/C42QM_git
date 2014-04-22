/*=============================================================================
  FILE: dapar_odl.c

  OVERVIEW:     Debug Agent - Process on-device logging instructions

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/dapar_odl.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "da_main.h"
#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_odl_h(da_parser_info_t *pPrsr);

da_parser_rval_t _handler_odl_fname(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_odl_start(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_odl_stop(da_parser_info_t *pPrsr);
da_parser_rval_t _handler_odl_bcount(da_parser_info_t *pPrsr);

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
//=========================================================================//

//=========================================================================//

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
// Param 2
static const da_parser_table_t da_parser_table_odl[] =
{
   {"fname",      _handler_odl_fname},
   {"start",      _handler_odl_start},
   {"stop",       _handler_odl_stop},
   {"bcount",     _handler_odl_bcount},    // DDR byte count
   {"", NULL}
};
/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_odl_handler(da_parser_info_t *pPrsr)
{
   pPrsr->pData = NULL;
   return (da_parser_param(pPrsr, da_parser_table_odl));
}
/*=========================================================================*/
da_parser_rval_t _handler_odl_fname(da_parser_info_t *pPrsr)
{
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: odl fname");
   // get fname
   // get loc
   // set cfg
   return DA_PARSER_RVAL_SUCCESS;
}
/*=========================================================================*/
da_parser_rval_t _handler_odl_start(da_parser_info_t *pPrsr)
{
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: odl start");
   // get delay time
   return DA_PARSER_RVAL_SUCCESS;
}
/*=========================================================================*/
da_parser_rval_t _handler_odl_stop(da_parser_info_t *pPrsr)
{
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: odl stopr");
   // get delay time
   return DA_PARSER_RVAL_SUCCESS;
}
/*=========================================================================*/
da_parser_rval_t _handler_odl_bcount(da_parser_info_t *pPrsr)
{
   da_out_msg(DA_OUT_LEVEL_BRIEF, "DA: odl bcount");
   // get value
   // set cfg
   return DA_PARSER_RVAL_SUCCESS;
}
