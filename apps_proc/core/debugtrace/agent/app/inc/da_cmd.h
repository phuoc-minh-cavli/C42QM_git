#ifndef DA_CMD_H
#define DA_CMD_H
/**
  File: da_cmd.h

  Debug Agent - For command interfaces.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_cmd.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  07/08/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
#define DA_CMD_RESP_SUCCESS         0  // Command successfully.
#define DA_CMD_RESP_FAILED          1  // Command failed: error, bad state
#define DA_CMD_RESP_UNSUPPORTED     2  // Not supported: fuse blown
#define DA_CMD_RESP_INVALID_PARAM   3

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

typedef int da_cmd_resp_t;  // DA_CMD_RESP_...

#endif /* #ifndef DA_CMD_H */

