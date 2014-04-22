/*=============================================================================
  FILE: da_reader.c

  OVERVIEW:     Debug Agent Reader

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_reader.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdio.h>
#include "stringl/stringl.h"

#include "da_main_int.h"
#include "da_reader.h"
#include "fs_errno.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "tracer_event_ids.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#include "agent_cfg_buf.h"    // Built-in Buffered configuration script

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

int _read_char(da_info_t *da_info_ptr)
{
   if (strlen(da_info_ptr->reader[da_info_ptr->curReader].config_buf) >
       da_info_ptr->reader[da_info_ptr->curReader].curCharIdx)
   {
      return da_info_ptr->reader[da_info_ptr->curReader].config_buf[
             (da_info_ptr->reader[da_info_ptr->curReader].curCharIdx)++];
   }
   else
   {
      return EOF;
   }
}

/*-------------------------------------------------------------------------*/
/*
   RETURN:  the next line from the "current" configuration file.
     DA_READER_RVAL_SUCCESS = the input line, inLine, is valid
     DA_READER_RVAL_ERR = Input line too long.
     DA_READER_RVAL_DONE = no input line available,
                           end of file has been reached.
*/
da_reader_rval_t da_reader_line(da_info_t *da_info_ptr)
{
   int inChar;
   int charCnt;
   char prevChar;


   memset (da_info_ptr->reader[da_info_ptr->curReader].inLine, '\0',
           sizeof(da_info_ptr->reader[da_info_ptr->curReader].inLine));
   charCnt = 0;
   prevChar = da_info_ptr->reader[da_info_ptr->curReader].prevChar;

   while(EOF != (inChar = _read_char(da_info_ptr)))
   {
      if ('\r' == inChar)
      {
         break;
      }
      if (('\n' == inChar) && ('\r' != prevChar))
      {
         break;
      }
      else if (('\n' != inChar) || ('\r' != prevChar))
      {
         if (DA_SWCFG_MAX_INLINE_BYTES > charCnt)
         {
            da_info_ptr->reader[da_info_ptr->curReader].
               inLine[charCnt] = inChar;
            charCnt++;
         }
      }
      prevChar = inChar;
   }

   da_info_ptr->reader[da_info_ptr->curReader].prevChar = inChar;
   da_info_ptr->reader[da_info_ptr->curReader].inLineCnt++;

   if (DA_SWCFG_MAX_INLINE_BYTES < charCnt)
   {
      da_info_ptr->reader[da_info_ptr->curReader].
         inLine[DA_SWCFG_MAX_INLINE_BYTES] = '\0';
      da_out_msg(DA_OUT_LEVEL_ERR,
                 "Agent max line len of %d chars exceeded.",
                 DA_SWCFG_MAX_INLINE_BYTES);
      return DA_READER_RVAL_ERR;
   }
   else
   {
      da_info_ptr->reader[da_info_ptr->curReader].inLine[charCnt]='\0';
   }
   if ((0 == charCnt) && (EOF == inChar))
   {
      return DA_READER_RVAL_DONE;
   }
   else
   {
      return DA_READER_RVAL_SUCCESS;
   }
}

/*-------------------------------------------------------------------------*/
/*
   RETURN:
      DA_READER_RVAL_SUCCESS
*/
da_reader_rval_t da_reader_close(da_info_t *da_info_ptr)
{
   if (NULL != da_info_ptr->reader[da_info_ptr->curReader].pfInput)
   {
      fclose(da_info_ptr->reader[da_info_ptr->curReader].pfInput);
   }
   da_info_ptr->reader[da_info_ptr->curReader].rdr_state =
      DA_READER_STATE_FREE;
   if (0 == da_info_ptr->curReader)
   {
      if (NULL != da_info_ptr->pfLog)
      {
         fclose(da_info_ptr->pfLog);
      }
   }
   else
   {
      da_info_ptr->curReader -= 1;
      if (DA_READER_STATE_PENDING ==
          da_info_ptr->reader[da_info_ptr->curReader].rdr_state)
      {
         da_info_ptr->reader[da_info_ptr->curReader].rdr_state =
            DA_READER_STATE_CURRENT;
      }
   }
   return DA_READER_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
da_reader_rval_t da_reader_close_all(da_info_t *da_info_ptr)
{
   da_reader_rval_t reader_rval;
   reader_rval = DA_READER_RVAL_SUCCESS;
   while (DA_READER_STATE_FREE !=
          da_info_ptr->reader[da_info_ptr->curReader].rdr_state)
   {
      reader_rval = da_reader_close(da_info_ptr);
   }
   return reader_rval;
}

/*-------------------------------------------------------------------------*/
/* Initialize for reading a config file.

   RETURN:
      DA_READER_RVAL_SUCCESS
      DA_READER_RVAL_ERR
      DA_READER_RVAL_FILENAME_LEN_ERR
      DA_READER_RVAL_MULTIPLE_FILE_ERR
      DA_READER_RVAL_FILE_READ_ERR
*/
da_reader_rval_t da_reader_init(da_info_t *da_info_ptr,
                                da_reader_cfg_type_enum cfg_type,
                                char *fname)
{
   da_reader_t *pReader;
   int file_bytes_read;

   // Find a reader
   switch (da_info_ptr->reader[da_info_ptr->curReader].rdr_state)
   {
      case DA_READER_STATE_CURRENT:
         // Current Reader in use, change to pending.
         da_info_ptr->reader[da_info_ptr->curReader].rdr_state = DA_READER_STATE_PENDING;
         // Go to next Reader.
         da_info_ptr->curReader++;
         break;
      case DA_READER_STATE_FREE:
         // Current reader is available (not in use).
         break;

      default: // DA_READER_STATE_PENDING
         // All other cases should never occur.
         da_out_msg(DA_OUT_LEVEL_ERR,
                 "Inclusion of cfg file during invalid state.");
         return DA_READER_RVAL_ERR;
         //break;
   }
   if (DA_SWCFG_MAX_READERS <= da_info_ptr->curReader)
   {
      da_out_msg(DA_OUT_LEVEL_ERR,
              "Cfg files supported exceeded (max is %d).",
              DA_SWCFG_MAX_READERS);
      return DA_READER_RVAL_MULTIPLE_FILE_ERR;
   }
   if (DA_READER_STATE_FREE !=
       da_info_ptr->reader[da_info_ptr->curReader].rdr_state)
   {
      da_out_msg(DA_OUT_LEVEL_ERR,
              "Reading of multiple cfg files failed.");
      return DA_READER_RVAL_MULTIPLE_FILE_ERR;
   }

   // Assign, open, and prepare for reading.
   pReader = &(da_info_ptr->reader[da_info_ptr->curReader]);
   memset(pReader, 0, sizeof(da_reader_t));
   if (strlcpy(pReader->inFname, fname, sizeof(pReader->inFname)) >=
       sizeof(pReader->inFname))
   {
      da_out_msg(DA_OUT_LEVEL_ERR, "Agent max cfg name len exceeded.");
      return DA_READER_RVAL_FILENAME_LEN_ERR;
   }

   switch (cfg_type)
   {
      #if 0
      case DA_READER_CONFIG_TYPE_FILE:
      #if (1 == WINDEV)
         memset(pReader, 0, sizeof(da_reader_t));
         if (NULL == (pReader->pfInput = fopen(fname, "r")))
         {
            da_out_msg(DA_OUT_LEVEL_ERR, "Cfg file not found.");
            return DA_READER_RVAL_FILE_READ_ERR;
         }
         else
         {
            da_out_msg(DA_OUT_LEVEL_DETAIL, "Cfg file opened");
            file_bytes_read = fread(pReader->config_buf, sizeof(char),
               sizeof(pReader->config_buf) - 1, pReader->pfInput);
            if (!feof(pReader->pfInput))
            {
               da_out_msg(DA_OUT_LEVEL_ERR, "Cfg file too big.");
               da_reader_close(da_info_ptr);
               return DA_READER_RVAL_FILE_READ_ERR;
            }
            pReader->config_buf[file_bytes_read] = '\0';
         }
      #else
         // Open the file and read into the buffer.
         if (-1 ==
             (file_bytes_read = efs_get(fname, pReader->config_buf,
                                        DA_SWCFG_EFS_FILE_BYTES))
            )
         {
            da_out_msg(DA_OUT_LEVEL_ERR, "Cfg file read error %d", efs_errno);
            return DA_READER_RVAL_FILE_READ_ERR;
         }
         else
         {
            da_out_msg(DA_OUT_LEVEL_DETAIL, "Cfg file opened %d",
                       file_bytes_read);
         }
      #endif
         break;
      #endif
      case DA_READER_CONFIG_TYPE_BUFFER:
         if (strlcat(pReader->config_buf, configbuffer,
                     sizeof(pReader->config_buf))
             >= sizeof (pReader->config_buf))
         {
            da_out_msg(DA_OUT_LEVEL_ERR, "Cfg buffer read error");
            return DA_READER_RVAL_FILE_READ_ERR;
         }
         else
         {
            da_out_msg(DA_OUT_LEVEL_DETAIL, "Using buffered cfg");
         }
         break;

      default:
         pReader->config_buf[0] = '#';
         da_out_msg(DA_OUT_LEVEL_DETAIL, "No Cfg file specified.");
         break;
   }
   da_info_ptr->reader[da_info_ptr->curReader].rdr_state =
      DA_READER_STATE_CURRENT;

   return DA_READER_RVAL_SUCCESS;

}  // da_reader_init()

/*-------------------------------------------------------------------------*/
/*
   Returns: DA_READER_RVAL_...
*/
da_reader_rval_t da_reader_set_start_file(da_info_t *da_info_ptr,
                                          int start_config)
{
   char fname[DA_SWCFG_MAX_FNAME_BYTES + 1];
   da_reader_cfg_type_enum reader_cfg_type;

   // Set the config file.
   memset (fname, 0, sizeof(fname));
   switch (start_config)
   {
      case DA_START_CONFIG_BUFFER:
         reader_cfg_type = DA_READER_CONFIG_TYPE_BUFFER;
         if (strlcat(fname,
             DA_SWCFG_CONFIG_EFS_PATH DA_SWCFG_CONFIG_BUFFER_FNAME,
             sizeof(fname)) >=  sizeof(fname))
         {
            return (DA_READER_RVAL_FILENAME_LEN_ERR);
         }
         break;
      case DA_START_CONFIG_CFG0:
         reader_cfg_type = DA_READER_CONFIG_TYPE_FILE;
         if (strlcat(fname,
             DA_SWCFG_CONFIG_EFS_PATH DA_SWCFG_CONFIG_0_FNAME,
             sizeof(fname)) >= sizeof(fname))
         {
            return (DA_READER_RVAL_FILENAME_LEN_ERR);
         }
         break;
      case DA_START_CONFIG_CFG1:
         reader_cfg_type = DA_READER_CONFIG_TYPE_FILE;
         if (strlcat(fname,
             DA_SWCFG_CONFIG_EFS_PATH DA_SWCFG_CONFIG_1_FNAME,
             sizeof(fname)) >= sizeof(fname))
         {
            return (DA_READER_RVAL_FILENAME_LEN_ERR);
         }
         break;
      default:
         return DA_READER_RVAL_ERR;
   }

   return da_reader_init(da_info_ptr, reader_cfg_type, fname);
}

/*-------------------------------------------------------------------------*/
/*
   Returns: DA_READER_RVAL_...
*/
da_reader_rval_t da_reader_set_file_end(da_info_t *da_info_ptr)
{
   da_info_ptr->reader[da_info_ptr->curReader].curCharIdx =
      strlen(da_info_ptr->reader[da_info_ptr->curReader].config_buf);
   return DA_READER_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
// Create filename for log file based on config filename and open.
/*
   RETURN:
      DA_READER_RVAL_SUCCESS
      DA_READER_RVAL_ERR
*/
da_reader_rval_t da_reader_open_log(da_info_t *da_info_ptr)
{
   if (strlcpy(da_info_ptr->logFname, DA_SWCFG_LOG_FNAME,
               sizeof(da_info_ptr->logFname))
       >= sizeof(da_info_ptr->logFname))
   {
      return (DA_READER_RVAL_FILENAME_LEN_ERR);
   }

   if (NULL == (da_info_ptr->pfLog = fopen(da_info_ptr->logFname,"w")))
   {
      da_info_ptr->outLevel &= (~DA_OUT_LOG_BIT);
      da_out_msg(DA_OUT_LEVEL_ERR,
                 "No log generated, cannot open file.");
      return DA_READER_RVAL_ERR;
   }
   return DA_READER_RVAL_SUCCESS;
}

