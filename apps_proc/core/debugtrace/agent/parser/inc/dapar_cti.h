#ifndef DAPAR_CTI_H
#define DAPAR_CTI_H
/**
  @file dapar_cti.h

  Debug Agent - process cti instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_cti.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  11/15/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// CTI instruction parsed data.
typedef struct dapar_cti_data_s
{
   da_parser_ssid_t ssid;
   int block_len;
   char *block_name;       // A null terminated string.
   int type_valid;         // 0 = invalid, 1 = valid
   int type;               // 0 = output, 1 = input
   int trigger_num_valid;  // 0 = invalid, 1 = valid
   int trigger_num;
   int channel_num_valid;  // 0 = invalid, 1 = valid
   int channel_num;
} dapar_cti_data_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_cti_handler(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_CTI_H */

