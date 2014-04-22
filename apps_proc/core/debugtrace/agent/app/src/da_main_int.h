#ifndef DA_MAIN_INT_H
#define DA_MAIN_INT_H
/**
  File: da_main_int.h

  Debug Agent - Configure target for QDSS debugging.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_main_int.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "da_main.h"
#include "da_version.h"
#include "da_swcfg.h"
#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Temporary for development.
 * ------------------------------------------------------------------------*/
#ifndef WINDEV
#define WINDEV 0
#endif

#if 1 == WINDEV
   #define DA_CONT_ON_ERROR 1
   #define DA_OUT_STD_ON 1    // Standard I/O
   #define DA_OUT_LOG_ON 1    // External log file.
   #define DA_OUT_TRACE_ON 0  // SW trace message.
#else
   #define DA_CONT_ON_ERROR 0
   #define DA_OUT_STD_ON 0    // Standard I/O
   #define DA_OUT_LOG_ON 0    // External log file.
   #define DA_OUT_TRACE_ON 0  // SW trace message.
#endif

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define TRUE 1
#define FALSE 0

// Bit mask for output of generated messages.
#define DA_OUT_STD_BIT    0x01
#define DA_OUT_LOG_BIT    0x02
#define DA_OUT_TRACE_BIT  0x04


// Status of reader
typedef enum
{
   DA_READER_RVAL_SUCCESS   = 0,    /**< Function completed successfully. */
   DA_READER_RVAL_DONE      = 1,    /**< Nothing left to process. */
   DA_READER_RVAL_ERR       = 2,    /**< 0xF Error occurred or
                                         unable to fulfill request. */
   DA_READER_RVAL_FILENAME_LEN_ERR  = 3,  /**< 0xE Filename exceeds max length. */
   DA_READER_RVAL_MULTIPLE_FILE_ERR = 4,  /**< 0xD Error procesing multiple files. */
   DA_READER_RVAL_FILE_READ_ERR     = 5   /**< 0xC Error reading the file. */
} da_reader_rval_t;

// State of reader in processing config file.
typedef enum
{
   DA_READER_STATE_FREE         =  0,    // Record free for use.
   DA_READER_STATE_CURRENT      =  1,    // Processing currently in progress.
   DA_READER_STATE_PENDING      =  2,    // File valid, processing pending.
} da_reader_state_t;

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// Debug agent software version
typedef struct da_version_s
{
   int major;
   int minor;
   int build;
} da_version_t;

// Reader - processing context of a configuration file.
typedef struct da_reader_s {

   da_reader_rval_t rdr_rval; // Status of file processing.
   da_reader_state_t rdr_state;  // State of processing file.
   char swe_tag_validated[DA_PARSER_SSID_LAST];
      // Config of SW events allowed (1) or not (0) per subsystem.

   char config_buf[DA_SWCFG_MAX_INPUT_BYTES]; // Contents of file being processed.
   unsigned int curCharIdx;   // Index into config_buf.

   int inLineCnt;             // Input file line being processed.
   FILE *pfInput;             // File pointer to input config file.
   char inFname[DA_SWCFG_MAX_FNAME_BYTES + 1];
      // Name of configuration file specified.
   char inLine[DA_SWCFG_MAX_INLINE_BYTES + 1];
      // Current line read from config file.

   char errMsg[DA_SWCFG_MAX_ERRMSG_BYTES];

   char prevChar;
   da_parser_info_t parserCtxt;

} da_reader_t;

// Debug Agent - overall processing state of debug agent
typedef struct da_info_s {
   da_version_t version;   // Debug agent version
   int outLevel;
      // Error level. 0=None. 1=Errors only. 2=All
   int outFlag;            // Write programs progress to output.
   int exeResult;          // Exe result.
   int curReader;          // Current reader in use.
   int chipid_validated;
      // Used to allow (1) or not (0) hw event config.
   char logFname[DA_SWCFG_MAX_FNAME_BYTES + 1];
      // Name of log file generated.
   FILE *pfLog;            // File pointer to log file.
   da_reader_t reader[DA_SWCFG_MAX_READERS];
   char exeDesc[128];      // Description of execution.
//   void *resp              // Pointer to the response message.
} da_info_t;



/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
// See also da_main.h

da_reader_rval_t da_set_start_file(int start_config);
da_reader_rval_t da_set_file_end(void);

#endif /* #ifndef DA_MAIN_INT_H */

