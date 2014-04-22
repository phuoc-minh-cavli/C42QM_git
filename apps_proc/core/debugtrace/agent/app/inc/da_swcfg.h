#ifndef DA_SWCFG_H
#define DA_SWCFG_H
/**
  File: da_swcfg.h

  Software configuration for Debug Agent application (code
  limits and resource constraints).
 */
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_swcfg.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/

// File
#define DA_SWCFG_EFS_FILE_BYTES 3000   // EFS file size.
#define DA_SWCFG_MAX_INPUT_BYTES 8192  // Max bytes per file.
#define DA_SWCFG_MAX_FNAME_BYTES 256   // Max bytes for filename.

#define DA_SWCFG_CONFIG_BUFFER_FNAME   "agent_cfg_buf.h"
#if (1 == WINDEV)
   #define DA_SWCFG_CONFIG_EFS_PATH    ""
#else
   #define DA_SWCFG_CONFIG_EFS_PATH    "/debugagent/"
#endif
#define DA_SWCFG_CONFIG_0_FNAME        "agent_cfg_def.txt"
#define DA_SWCFG_CONFIG_1_FNAME        "agent_cfg1.txt"

#define DA_SWCFG_LOG_FNAME "agent_log.txt"

// Reader
#define DA_SWCFG_MAX_READERS 4        // Maximum depth of open config files.

// Instructions
#define DA_SWCFG_MAX_INLINE_BYTES 128  // Max chars per line
                                       //   Must be at least 1.
#define DA_SWCFG_MAX_PARAMS_PER_LINE 12  // Max parameters per line.
#define DA_SWCFG_MAX_PARAM_BYTES 80      // Max chars per parameter
#define DA_SWCFG_PARAM_DELIMITERS " \t\n"

// Error
#define DA_SWCFG_MAX_ERRMSG_BYTES 128 // Max chars per error message.

#endif /* #ifndef DA_SWCFG_H */

