#ifndef DAPAR_SWT_H
#define DAPAR_SWT_H
/**
  @file dapar_swt.h

  Debug Agent - process SW tracer (swt) instructions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/dapar_swt.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "da_parser.h"
#include "tracer_entity.h"
#include "tracer_event_ids.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// Software trace instruction parsed data.
typedef struct dapar_swt_data_s
{
   da_parser_ssid_t ssid;              // Subystem ID.
   tracer_ost_entity_id_enum_t entid;  // Entity ID
   uint32 evtid;                       // Event ID
   uint32 state[8];        //0=disable, 1=enable.
                           //Use first element as value.
                           //Get all entity states uses 64 bytes as mask.
   char event_tag[12];     // 12 byte tag, not null terminated.
   int rpmevt_dest_valid;  // rpmevt_dest value is valid (1) or not (0)
   int rpmevt_dest;        // Log destination of RPM's events
   int rpmevt_group_valid; // rpmevt_group value is valid (1) or not (0)
   uint8 rpmevt_group;     // RPM event group
   int rpmevt_mask_valid;  // rpmevt_mask value is valid (1) or not (0)
   uint64 rpmevt_mask;     // Configuration mask for the RPM event group.
} dapar_swt_data_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_parser_rval_t dapar_swt_handler(da_parser_info_t *pPrsr);
da_parser_rval_t dapar_swt_parse_entity(da_parser_info_t *pPrsr);

#endif /* #ifndef DAPAR_SWT_H */

