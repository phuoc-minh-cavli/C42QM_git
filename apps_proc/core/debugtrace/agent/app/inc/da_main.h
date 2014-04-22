#ifndef DA_MAIN_H
#define DA_MAIN_H
/**
   File: da_main.h

   Debug Agent entry point.

 */
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/inc/da_main.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Level of messages generated (larger the value, more messages produced)
#define DA_OUT_LEVEL_ERR    1
#define DA_OUT_LEVEL_BRIEF  2
#define DA_OUT_LEVEL_DETAIL 3
#define DA_OUT_LEVEL_DEBUG  4

//Starting config script options
typedef enum
{
   DA_START_CONFIG_NONE = 0,  // Start without running any config.
   DA_START_CONFIG_BUFFER, // Use preset internal buffered config.
   DA_START_CONFIG_CFG0,   // See DA_SWCFG_CONFIG_0_FNAME; agent_cfg_def.txt
   DA_START_CONFIG_CFG1,   // See DA_SWCFG_CONFIG_1_FNAME; agent_cfg1.txt
} da_start_config_enum;

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/* Return values for Debug Agent function call.
*/
typedef unsigned int da_rval_t;  // upper word da_parser_rval_t +
                                 // lower word da_reader_rval_t

// Debug agent software version
typedef struct da_status_s
{
   unsigned int status1; /**< [15:12]=currentReader, [11:0]=currentLine*/
   unsigned int status2; /**<  Unused */
} da_status_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

int da_out_msg(int level, const char *pFmtStr, ...);

unsigned int da_version(void);

int da_set_swevent_tag_validated(int subsystem_id, int verified);
int da_is_swevent_tag_validated(int subsystem_id);

void da_set_chipid_validated(int verified);
int da_is_chipid_validated(void);

da_rval_t da_start(int start_config, da_status_t *pStatus);

#if 1 == WINDEV
int main(void);
#endif

#endif /* #ifndef DA_MAIN_H */

