/*=============================================================================
  FILE: da_main.c

  OVERVIEW:     Debug Agent main (entry) module.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_main.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <string.h>
#include <stdarg.h>     // va_...

#include "da_main_int.h"
#include "da_reader.h"
#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define DA_OUT_LEVEL_DEFAULT  DA_OUT_LEVEL_BRIEF   // ERR, BRIEF, DETAIL, DEBUG

/*---------------------------------------------------------------------------
 *
 * ------------------------------------------------------------------------*/
da_info_t da_info;

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

#define DA_IS_OUT(x)   (da_info.outFlag & (x))

/*-------------------------------------------------------------------------*/
void da_pause(void)
{
#if 1 == WINDEV
   if (DA_IS_OUT(DA_OUT_STD_BIT))

   { printf("Press any key to continue");
     getchar();
   }
#endif
}

/*-------------------------------------------------------------------------*/
unsigned int da_version(void)
{
   return (
      ((da_info.version.major<<24) & 0xFF000000) |
      ((da_info.version.minor<<16) & 0x00FF0000) |
      ((da_info.version.build)     & 0x0000FFFF) );
}
/*-------------------------------------------------------------------------*/
int da_out_msg(int level, const char *pFmtStr, ...)
{
   char buffer[1000];

   va_list argp;
   va_start(argp, pFmtStr);
   vsnprintf(&buffer[0], sizeof(buffer), pFmtStr, argp);
   if (da_info.outLevel >= level)
   {
#if 1==WINDEV
      if (DA_IS_OUT(DA_OUT_STD_BIT))
      {
         printf("%s:%d: ",
                da_info.reader[da_info.curReader].inFname,
                da_info.reader[da_info.curReader].inLineCnt);
         printf("%s\n",buffer);
      }
      if (DA_IS_OUT(DA_OUT_LOG_BIT))
      {
         fprintf(da_info.pfLog, "%s\n", buffer);
      }
#else
      if (DA_IS_OUT(DA_OUT_TRACE_BIT))
      {
         /*tracer_data(da_info.pfLog, "%s", buffer); */
      }
#endif
   }
   va_end(argp);
   return 0;
}

// Set agent's output modes.
void da_output_set(void)
{
   if (DA_OUT_STD_ON)   { da_info.outFlag |= DA_OUT_STD_BIT; }
   if (DA_OUT_LOG_ON)   { da_info.outFlag |= DA_OUT_LOG_BIT; }
   if (DA_OUT_TRACE_ON) { da_info.outFlag |= DA_OUT_TRACE_BIT; }
   da_info.outLevel = DA_OUT_LEVEL_DEFAULT;
   return;
}

/*-------------------------------------------------------------------------*/
// RETURN: 0 = successful, 1 = internal error subsystem id not supported.
int da_set_swevent_tag_validated(int subsystem_id, int verified)
{
   if ( (sizeof(da_info.reader[da_info.curReader].swe_tag_validated) /
         sizeof(da_info.reader[da_info.curReader].swe_tag_validated[0]))
        > subsystem_id)
   {
      da_info.reader[da_info.curReader].swe_tag_validated[subsystem_id] = verified;
      return 0;
   }
   else
   {
      return 1;
   }
}
// RETURN: 0 = not validated, 1 = is validated.
int da_is_swevent_tag_validated(int subsystem_id)
{
   if ( (sizeof(da_info.reader[da_info.curReader].swe_tag_validated) /
         sizeof(da_info.reader[da_info.curReader].swe_tag_validated[0]))
        > subsystem_id)
   {
      return da_info.reader[da_info.curReader].swe_tag_validated[subsystem_id];
   }
   else
   {
      return 0;
   }
}
/*-------------------------------------------------------------------------*/
void da_set_chipid_validated(int verified)
{
   da_info.chipid_validated = verified;
   return;
}
int da_is_chipid_validated(void)
{
   return da_info.chipid_validated;
}

/*-------------------------------------------------------------------------*/
/*
   Returns: DA_RVAL_...
            status1
            status2
*/
da_rval_t da_run(da_status_t *pStatus)
{
   int bContinueReading;
   da_reader_rval_t reader_rval;
   da_parser_rval_t parser_rval;

   pStatus->status1 = 0;
   pStatus->status2 = 0;
   parser_rval = DA_PARSER_RVAL_SUCCESS;

   bContinueReading = 1;
   while (bContinueReading)
   {
      reader_rval = da_reader_line(&da_info);
      pStatus->status1 = ((da_info.curReader << 12) & 0xF000) |
                         (da_info.reader[da_info.curReader].inLineCnt & 0x0FFF);
      if (DA_READER_RVAL_SUCCESS == reader_rval)
      {
         parser_rval = da_parser_line(
                          da_info.reader[da_info.curReader].inLine,
                          &(da_info.reader[da_info.curReader].parserCtxt));
      }
      else if (DA_READER_RVAL_DONE == reader_rval)
      {
         da_reader_close(&da_info);
         if (DA_READER_STATE_FREE ==
             da_info.reader[da_info.curReader].rdr_state)
         {
            bContinueReading = 0;
         }
      }
      if ((DA_PARSER_RVAL_SUCCESS != parser_rval) ||
          (DA_READER_RVAL_ERR == reader_rval))
      {
         // Error occurred.
#if (0 == DA_CONT_ON_ERROR)
         // Abort processing.
         bContinueReading = 0;
#else
         //Continue despite error.
         da_out_msg(0, "Err %d, continuing", parser_rval);
#endif
      }
   }

   if (DA_READER_RVAL_DONE == reader_rval ||
       DA_PARSER_RVAL_SUCCESS == parser_rval)
   {
      da_out_msg(0, "Config done.");
   }
   else
   {
      da_out_msg(0, "Agent aborted on err: reader %d, parser %d",
                 reader_rval, parser_rval);
   }

   da_reader_close_all(&da_info);
   da_pause();

   return ((parser_rval << 8) & 0xFF00) |
           (reader_rval & 0x00FF);

}  /* da_run() */

/*-------------------------------------------------------------------------*/
/* Entry point to start debug agent.
   Intended to be called in single sequential executions.
   Return:  0 = Successfully completed configuration
            other = Error occurred, configuration is incomplete.
*/
da_rval_t da_start(int start_config, da_status_t *pStatus)
{
   da_reader_rval_t reader_rval;

   // Initialize debug agent context all to zero.
   memset(&da_info, 0, sizeof(da_info));

   pStatus->status1 = 0;
   pStatus->status2 = 0;

   da_info.version.major = DA_VERSION_MAJOR_NUM;
   da_info.version.minor = DA_VERSION_MINOR_NUM;
   da_info.version.build = DA_VERSION_BUILD_NUM;

   da_output_set();  // Setup types of output generated.

   // Start external log file
   if (da_info.outFlag & DA_OUT_LOG_BIT)
   {
      if (DA_READER_RVAL_ERR == da_reader_open_log(&da_info))
      {
         da_out_msg(0, "Agent unable to open a log file.");
         da_pause();
#if (1 == WINDEV)
         return DA_READER_RVAL_ERR;
//#else  // Proceed without log.
#endif
         da_info.outFlag &= !DA_OUT_LOG_BIT;  // Proceed without log.
      }
   }

   da_out_msg(0, "Agent starting config.");

   reader_rval = da_reader_set_start_file(&da_info, start_config);
   if (DA_READER_RVAL_SUCCESS == reader_rval)
   {
     return da_run(pStatus);
   }
   else
   {
      da_out_msg(0, "Agent aborted reader error %d.", reader_rval);
      da_pause();
      return reader_rval;
   }
}

/*-------------------------------------------------------------------------*/
da_reader_rval_t da_set_start_file(int start_config)
{
   return da_reader_set_start_file(&da_info, start_config);
}
/*-------------------------------------------------------------------------*/
da_reader_rval_t da_set_file_end(void)
{
   return da_reader_set_file_end(&da_info);
}

/*-------------------------------------------------------------------------*/
// Invoke debug agent execution from rcinit.
void da_start_init(void)
{
   da_status_t init_status;
   da_start(DA_START_CONFIG_BUFFER, &init_status);
   return;
}

/*-------------------------------------------------------------------------*/
#if 1 == WINDEV
int main(void)
{
   da_status_t start_status;
   return da_start(DA_START_CONFIG_BUFFER &start_status);
}
#endif

