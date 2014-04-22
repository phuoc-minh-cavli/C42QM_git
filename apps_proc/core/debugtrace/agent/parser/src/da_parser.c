/*=============================================================================
  FILE: da_parser.c

  OVERVIEW:     Debug Agent Parser

  Parses one instruction and calls the corresponding command handler.

  DEPENDENCIES: da_parser_info_t *pPrsr must point to a defined structure.

   Main entry point: da_parser_line()
   1. Parses and executes one line (i.e. one instruction).
   1a. Tokenizes the line.
   2b. Parse the first param.
   Parsing of a param: da_parser_param()
   2. Gets the current param and calls its handler function to
      continue processing/
   2a. Get a param.
   2a1. No param.
   2a2. Match param to table entry.
   2a2a. Parameter does not match.
   2a2b. Parameter maches.
   2a2b1. Call the handler for the parameter.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/src/da_parser.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial revision.

=============================================================================*/

#if 1 == WINDEV
   #define strcasecmp(a, b)  stricmp(a, b)
#endif

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "stringl/stringl.h"
#include "com_dtypes.h"

#include "da_swcfg.h"
#include "da_main.h"      // Agent method for outputting messages.

#include "da_parser.h"

/*--------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

// Head parsing table - All possible first parameters.
#include "da_parser_table.h"

/*---------------------------------------------------------------------------
 * Internalized Function Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
// Extract parameter from the current line
// Return number of parameter in pPrsr->paramCnt.
// If count is non-zero, pPrsr->paramList[] is valid.
// DEPENDENCIES:
//    da_parser_info_t *pPrsr must point to a defined structure.
//    char *inLine must point to a null terminated string.
da_parser_rval_t _parser_tokenize(char *inLine, da_parser_info_t *pPrsr)
{
   int idx;    // indexing
   size_t line_len;
   int end_quote_found;
   char *pParam;
   char *pParam2;
   char *pStrTok;
   char buff[DA_SWCFG_MAX_INLINE_BYTES];
   char lastchar;

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR* Parser token, no parse context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }

   pPrsr->paramCnt = 0;
   memset(pPrsr->paramList, 0, sizeof(pPrsr->paramList));
   buff[0]='\0';
   line_len = strlen (inLine);

   pParam = strtok_r(inLine, DA_SWCFG_PARAM_DELIMITERS, &pStrTok);
   while (NULL != pParam)
   {
      if ('#' == pParam[0])
      {  // Comment started, ignore to end of line.
         break;
      }

      // Look for double quoted strings.
      // The double quote characters are stripped off.
      if ('\"' == pParam[0])
      {  // Quote started

         pParam[strlen(pParam)]= ' '; // Undo token delimitier

         // Find end of quote
         lastchar = pParam[0];
         pParam2 = pParam + 1;
         line_len = strlen(pParam2);
         end_quote_found = 0;
         for (idx = 0; idx < line_len; idx++)
         {
            if (('\"' == pParam2[idx]) &&
                ('\\' != lastchar))
            {
               pParam2 += idx;   // Position of end quote
               strtok_r(pParam2, DA_SWCFG_PARAM_DELIMITERS, &pStrTok); //reset
               end_quote_found = 1;
               break;
            }
            lastchar = pParam2[idx];
         }

         if (!end_quote_found)
         {  // Error occurred abort.
            break;
         }

         pParam += 1;       // Remove starting quote char
         pParam2[0] = '\0'; // Remove ending quote char
      }

      if (DA_SWCFG_MAX_PARAMS_PER_LINE <= pPrsr->paramCnt)
      {
         da_out_msg(DA_OUT_LEVEL_ERR, "Agent max param cnt exceeded.");
         return DA_PARSER_RVAL_FORMAT_ERR;
      }
      if (DA_SWCFG_MAX_PARAM_BYTES <= strlen(pParam))
      {
         da_out_msg(DA_OUT_LEVEL_ERR, "Agent max param len exceeded.");
         return DA_PARSER_RVAL_FORMAT_ERR;
      }

      strlcpy(pPrsr->paramList[pPrsr->paramCnt], pParam,
              sizeof(pPrsr->paramList[pPrsr->paramCnt]));
      pPrsr->paramCnt++;

      pParam = strtok_r(NULL, DA_SWCFG_PARAM_DELIMITERS, &pStrTok);
   }

   // Format and output condensed instruction (params only).
   if (0 < pPrsr->paramCnt)
   {
      for (idx=0; idx < pPrsr->paramCnt; idx++)
      {
         if (idx != 0)
         {
            strlcat(buff, " ", sizeof(buff));
         }
         strlcat(buff, pPrsr->paramList[idx], sizeof(buff));
      }
      da_out_msg(DA_OUT_LEVEL_DETAIL, "Instruction: %s", buff);
   }

   return DA_PARSER_RVAL_SUCCESS;
}

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
// Gets the current parameter from the parameter list.
// Returns: DA_PARSER_RVAL_SUCCESS, parameter string in *pParam
//          DA_PARSER_RVAL_CMD_FAILED when pPrsr is invalid
//          DA_PARSER_RVAL_PARAM_MISSING when instruction has no more
//          *pParam is valid only when DA_PARSER_RVAL_SUCCESS
da_parser_rval_t da_parser_get_param(da_parser_info_t *pPrsr, char **pParam)
{
   da_parser_rval_t rval;

   if (NULL == pPrsr)
   {  // Programming error.
      *pParam = NULL;
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR Parser get param, no parse context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }

   if ((0 <= pPrsr->curParamIdx) && (pPrsr->paramCnt > pPrsr->curParamIdx))
   {
      *pParam = pPrsr->paramList[pPrsr->curParamIdx];
      rval = DA_PARSER_RVAL_SUCCESS;
      da_out_msg(DA_OUT_LEVEL_DEBUG, "param: %s", pParam);
   }
   else
   {
      *pParam = NULL;
      rval = DA_PARSER_RVAL_PARAM_MISSING;
      da_out_msg(DA_OUT_LEVEL_DEBUG, "Param missing.");
   }
   return rval;
}
/*-------------------------------------------------------------------------*/
int _is_valid_hexstr (char* pParam, int param_chars)
{
   int ii;
   if (2 > param_chars)  // Prefix "0x" missing
   {
      return 1;
   }
   if ('0' != pParam[0])
   {  // First char not '0'
      return 1;
   }
   if ('x' != pParam[1] && 'X' != pParam[1])
   {  // Second char not 'x'
      return 1;
   }

   // Do some range checking.
   ii = 2;
   while ((16 < param_chars - ii) && ('0' == pParam[ii]))
   {
      ii++;
   }
   if (16 < param_chars - ii)
   {  // Too many parameter
      return 1;
   }
   for (ii = 2; ii < param_chars; ii++)
   {  // Check for valid characters.
      switch (pParam[ii])
      {
         case '0':
         case '1': case '2': case '3':
         case '4': case '5': case '6':
         case '7': case '8': case '9':
         case 'a': case 'A':
         case 'b': case 'B':
         case 'c': case 'C':
         case 'd': case 'D':
         case 'e': case 'E':
         case 'f': case 'F':
            break;
         default:
            return 1;
            //break;
      }
   }
   return 0;
}

/*-------------------------------------------------------------------------*/
int _is_valid_octalstr(char* pParam, int param_chars)
{
   int ii;
   if (2 > param_chars)
   {
      return 1;
   }
   if ('0' != pParam[0])
   {
      return 1;
   }
   ii = 1; // Do some range checking.
   while ((11 < param_chars - ii) && ('0' == pParam[ii]))
   {
      ii++;
   }
   if ( (22 < param_chars - ii) ||
        ((22 == (param_chars-ii)) && ('1' < pParam[ii]))
      )
   {
      return 1;
   }
   else
   {
      for (ii = 1; ii < param_chars; ii++)
      {  // Check for valid characters.
         switch (pParam[ii])
         {
            case '0': case '1':
            case '2': case '3':
            case '4': case '5':
            case '6': case '7':
               break;
            default:
               return 1;
         }
      }
      return 0;
   }
}

/*-------------------------------------------------------------------------*/
int _is_valid_decimalstr(char* pParam, int param_chars)
{
   int ii;

   if (0 < param_chars)
   {
      if ('0' == pParam[0])
      {
         if (1 == param_chars)
         {
            return 0;
         }
         else
         {  // Octal format
            return 1;
         }
      }

      ii = 0; // Do partial check of range.
      while ((20 < param_chars - ii) && ('0' == pParam[ii]))
      {
         ii++;
      }
      if (20 < (param_chars - ii))
      {
         return 1;
      }
      else if (20 == (param_chars-ii))
      {
         if ('1' < pParam[ii])
         {  return 1; }
         else if ('1' == pParam[ii] && '8' < pParam[ii+1])
         {  return 1; }
         else
         {  return 0; }
      }
      else
      {
         for (ii = 0; ii < param_chars; ii++)
         {  // Check for valid characters.
            switch (pParam[ii])
            {
               case '0':
               case '1': case '2': case '3':
               case '4': case '5': case '6':
               case '7': case '8': case '9':
                  break;
               default:
                  return 1;
            }
         }
         return 0;
      }
   }
   return 1;
}
/*-------------------------------------------------------------------------*/
// Convert param to integer value.
da_parser_rval_t da_parser_unsigned(da_parser_info_t *pPrsr,
                                        void *pVal, int valLen)
{
   da_parser_rval_t rval;
   char *pParam;
   int param_chars;
   uint64 my_val; // 64-bit
   uint64 valMax;

   memset(pVal, 0, valLen);
   my_val = 0;

   rval = da_parser_get_param(pPrsr, &pParam);
   if (DA_PARSER_RVAL_SUCCESS != rval)
   {
      return rval;
   }

   // Convert value string to a numerical value.
   if (NULL != pParam)
   {
      param_chars = strlen(pParam);
      if (0 == _is_valid_hexstr(pParam, param_chars))
      {
         my_val = strtoull(pParam, (char**)NULL, 16);
      }
      else if (0 == _is_valid_octalstr(pParam, param_chars))
      {
         my_val = strtoull(pParam, (char**)NULL, 8);
      }
      else if (0 == _is_valid_decimalstr(pParam, param_chars))
      {
         my_val = strtoull(pParam, (char**)NULL, 10);
      }
      else
      {
         rval = DA_PARSER_RVAL_PARAM_INVALID;
      }
   }
      else
      {
      return DA_PARSER_RVAL_PARAM_MISSING;
   }

   // Check bounds of value with storage size.
   valMax = ((1 << (valLen << 3))) - 1;     // 2^(valLen * 8) - 1
   if (valMax < my_val)
   {
      return DA_PARSER_RVAL_PARAM_INVALID;
   }
   if (1 == valLen)
   {
      *((uint8 *)pVal) = (uint8) my_val;
   }
   else if (2 == valLen)
   {
      *((uint16 *)pVal) = (uint16) my_val;
   }
   else if (4 == valLen)
   {
      *((uint32 *)pVal) = (uint32) my_val;
   }
   else if (8 == valLen)
   {
      *((uint64 *)pVal) = my_val;
   }
      else
      {
      rval = DA_PARSER_RVAL_PARAM_INVALID;
   }

   return rval;
}
/*-------------------------------------------------------------------------*/
// Convert ID value to string.
char * da_parser_subsys_str(da_parser_ssid_t ssid)
{
   return (
     (ssid == DA_PARSER_SSID_RSV   ) ? "RSV   " :
     (ssid == DA_PARSER_SSID_APPS  ) ? "APPS  " :
     (ssid == DA_PARSER_SSID_MODEM ) ? "MODEM " :
     (ssid == DA_PARSER_SSID_WCNSS ) ? "WCNSS " :
     (ssid == DA_PARSER_SSID_RPM   ) ? "RPM   " :
     (ssid == DA_PARSER_SSID_ADSP  ) ? "ADSP  " :
     (ssid == DA_PARSER_SSID_VENUS ) ? "VENUS " :
     (ssid == DA_PARSER_SSID_GNSS  ) ? "GNSS  " :
     (ssid == DA_PARSER_SSID_SENSOR) ? "SENSOR" :
     (ssid == DA_PARSER_SSID_AUDIO ) ? "AUDIO " :
     (ssid == DA_PARSER_SSID_VPU   ) ? "VPU   " :
     (ssid == DA_PARSER_SSID_LAST  ) ? "LAST  " :
     (ssid == DA_PARSER_SSID_ALL   ) ? "ALL   " :
                                       "Unknown"
   );
}

// Parse instruction's subsystem name to a parser subsystem ID value.
da_parser_rval_t da_parser_subsys(da_parser_info_t *pPrsr,
                                  da_parser_ssid_t *ssid)
{
   da_parser_rval_t rval;
   char *pParam;
   da_parser_ssid_t iVal;

   iVal = DA_PARSER_SSID_RSV;
   rval = da_parser_get_param(pPrsr, &pParam);

   if (NULL != pParam)
   {
      rval = DA_PARSER_RVAL_SUCCESS;
      // All acceptable subsystem names using in instructions.
      if      (!strcasecmp(pParam, "apss"))   { iVal = DA_PARSER_SSID_APPS;  }
      else if (!strcasecmp(pParam, "apps"))   { iVal = DA_PARSER_SSID_APPS;  }
      else if (!strcasecmp(pParam, "modem"))  { iVal = DA_PARSER_SSID_MODEM; }
      else if (!strcasecmp(pParam, "mpss"))   { iVal = DA_PARSER_SSID_MODEM; }
      else if (!strcasecmp(pParam, "wcnss"))  { iVal = DA_PARSER_SSID_WCNSS; }
      else if (!strcasecmp(pParam, "wcn"))    { iVal = DA_PARSER_SSID_WCNSS; }
      else if (!strcasecmp(pParam, "pronto")) { iVal = DA_PARSER_SSID_WCNSS; }
      else if (!strcasecmp(pParam, "rpm"))    { iVal = DA_PARSER_SSID_RPM;   }
      else if (!strcasecmp(pParam, "adsp"))   { iVal = DA_PARSER_SSID_ADSP;  }
      else if (!strcasecmp(pParam, "video"))  { iVal = DA_PARSER_SSID_VENUS; }
      else if (!strcasecmp(pParam, "venus"))  { iVal = DA_PARSER_SSID_VENUS; }
      else if (!strcasecmp(pParam, "gnss"))   { iVal = DA_PARSER_SSID_GNSS;  }
      else if (!strcasecmp(pParam, "sensor")) { iVal = DA_PARSER_SSID_SENSOR;}
      else if (!strcasecmp(pParam, "audio"))  { iVal = DA_PARSER_SSID_AUDIO; }
      else if (!strcasecmp(pParam, "vpu"))    { iVal = DA_PARSER_SSID_VPU;   }
      else if (!strcasecmp(pParam, "all"))    { iVal = DA_PARSER_SSID_ALL;   }
      else
      {
         rval = DA_PARSER_RVAL_PARAM_INVALID;
      }
   }
   *ssid = iVal;
   return rval;
}

/*-------------------------------------------------------------------------*/
da_parser_rval_t da_parser_param(da_parser_info_t *pPrsr,
                                   const da_parser_table_t *table)
{
   char *pParam;
   da_parser_rval_t rval;

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR, "DA:*ERR Parser param, no parse context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }

   rval = DA_PARSER_RVAL_SUCCESS;
   pParam = NULL;

   da_out_msg(DA_OUT_LEVEL_DEBUG, "Parser param start");

   rval = da_parser_get_param(pPrsr, &pParam);
   if (NULL != pParam)
   {
      while (1)
      {
         da_out_msg(DA_OUT_LEVEL_DEBUG, "Table param: %s", table->name);
         if (!strcmp(table->name,""))
         {
            da_out_msg(DA_OUT_LEVEL_DEBUG, "Param is invalid: %s", pParam);
            return DA_PARSER_RVAL_PARAM_INVALID;
         }
         if (!strcasecmp(table->name, pParam))
         {
            da_out_msg(DA_OUT_LEVEL_DEBUG, "Param found: %s", pParam);
            if (NULL != table->handler)
            {
               da_out_msg(DA_OUT_LEVEL_DEBUG, "Calling handler");
               pPrsr->curParamIdx++;
               rval = table->handler(pPrsr);   //  Call handler for this param
            }
            else
            {  // No further handler, just return.
               rval = DA_PARSER_RVAL_SUCCESS;
            }
            break;
         }
         table = &table[1];
      }
   }
   return rval;
}

/*-------------------------------------------------------------------------*/
// Parse and executes one line (i.e. one instruction).
da_parser_rval_t da_parser_line(char *inLine, da_parser_info_t *pPrsr)
{
   da_parser_rval_t rval;

   if (NULL == pPrsr)
   {  // Programming error.
      da_out_msg(DA_OUT_LEVEL_ERR,"DA:*ERR Parse line, no parse context");
      return DA_PARSER_RVAL_CMD_FAILED;
   }

   memset(pPrsr, 0, sizeof(da_parser_info_t));
   rval = _parser_tokenize(inLine, pPrsr);

   if (DA_PARSER_RVAL_SUCCESS == rval)
   {
      if (0 < pPrsr->paramCnt)
      {
         // Start with the first parameter.
         pPrsr->curParamIdx = 0;
         rval = da_parser_param(pPrsr, da_parser_table_head);
      }
   }
   switch (rval)
   {
      case DA_PARSER_RVAL_FORMAT_ERR:
         da_out_msg(DA_OUT_LEVEL_ERR, "Instruction format error");
         break;
      case DA_PARSER_RVAL_PARAM_MISSING:
         da_out_msg(DA_OUT_LEVEL_ERR, "Instruction missing param #%d", pPrsr->curParamIdx+1);
         break;
      case DA_PARSER_RVAL_PARAM_INVALID:
         da_out_msg(DA_OUT_LEVEL_ERR, "Instruction param%d not supported: %s",
                 pPrsr->curParamIdx+1, pPrsr->paramList[pPrsr->curParamIdx]);
         break;
      case DA_PARSER_RVAL_CMD_FAILED:
         da_out_msg(DA_OUT_LEVEL_ERR, "Command failed",
                 pPrsr->curParamIdx+1, pPrsr->paramList[pPrsr->curParamIdx]);
         break;
   }
   return rval;
}

