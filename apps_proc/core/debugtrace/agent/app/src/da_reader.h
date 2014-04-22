#ifndef DA_READER_H
#define DA_READER_H
/**
  @file da_reader.h

  Debug Agent Reader - reads in the debug configuration
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_reader.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "da_main_int.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

typedef enum {
   DA_READER_CONFIG_TYPE_NONE   = 0,
   DA_READER_CONFIG_TYPE_FILE,
   DA_READER_CONFIG_TYPE_BUFFER,
} da_reader_cfg_type_enum;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

da_reader_rval_t da_reader_open_log(da_info_t *da_info_ptr);
da_reader_rval_t da_reader_set_start_file(da_info_t *da_info_ptr,
                                          int start_config);
da_reader_rval_t da_reader_set_file_end(da_info_t *da_info_ptr);
da_reader_rval_t da_reader_init(da_info_t *da_info_ptr,
                                da_reader_cfg_type_enum cfg_type,
                                char *fname);
da_reader_rval_t da_reader_close_all(da_info_t *da_info_ptr);
da_reader_rval_t da_reader_close(da_info_t *da_info_ptr);
da_reader_rval_t da_reader_line(da_info_t *da_info_ptr);

#endif /* #ifndef DA_READER_H */

