#ifndef DA_PARSER_H
#define DA_PARSER_H
/**
  File: da_parser.h

  Debug Agent Parser - Parse one instruction and calls the
  corresponding command handler.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/parser/inc/da_parser.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdlib.h>

#include "da_swcfg.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Returned values (4-LSB nibble)
typedef enum
{
   DA_PARSER_RVAL_SUCCESS = 0,       /**< Processed successfully.*/
   DA_PARSER_RVAL_INTERNAL_ERR,      /**< Internal processing error */
   DA_PARSER_RVAL_FORMAT_ERR,        /**< Not the expected format for the instruction */
   DA_PARSER_RVAL_PARAM_MISSING,     /**< Expected parameter not provided */
   DA_PARSER_RVAL_PARAM_INVALID,     /**< Parameter not supported */
   DA_PARSER_RVAL_CMD_FAILED         /**< Command execution failed. */
} da_parser_rval_t;

// Subsystem IDs (values match qdss's diag interface)
typedef enum
{
   DA_PARSER_SSID_RSV    =  0,
   DA_PARSER_SSID_APPS   =  1,
   DA_PARSER_SSID_MODEM  =  2,
   DA_PARSER_SSID_WCNSS  =  3,  // Pronto
   DA_PARSER_SSID_RPM    =  4,
   DA_PARSER_SSID_ADSP   =  5,  // ADSP
   DA_PARSER_SSID_VENUS  =  6,
   DA_PARSER_SSID_GNSS   =  7,
   DA_PARSER_SSID_SENSOR =  8,
   DA_PARSER_SSID_AUDIO  =  9,
   DA_PARSER_SSID_VPU    = 10,
   DA_PARSER_SSID_LAST,
   DA_PARSER_SSID_ALL    = 0xFF, // Reserved for internal use.
} da_parser_ssid_t;

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// Parser context
typedef struct da_parser_info_s {
   da_parser_rval_t prsr_rval; // Status of parser.
   int paramCnt;     // Number of parameters in paramList.
   int curParamIdx;  // Index of current paramList being parsed.
   char paramList[DA_SWCFG_MAX_PARAMS_PER_LINE][DA_SWCFG_MAX_PARAM_BYTES];
                     // Ordered list of param strings for current instruction.
   void *pData;      // Handle to handler specific command data.
} da_parser_info_t;

// Parsing table
typedef struct da_parser_table_s
{
   char name[DA_SWCFG_MAX_PARAM_BYTES];   // Text of a parameter
   da_parser_rval_t (*handler)(da_parser_info_t *pPrsr);
                                          // Processing of that parameter.
} da_parser_table_t;


/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Parse one line of ASCII text.

  @param inLine [in]: Null terminated ASCII text line to be parsed.
  @param pPrsr [in]: Pointer to control structure where parser will
                     store information.

  @return  DA_PARSER_RVAL_SUCCESS : line sucessfully processed.
           DA_PARSER_RVAL_... :  error occurred.
 */
da_parser_rval_t da_parser_line(char *inLine, da_parser_info_t *pPrsr);

/*-------------------------------------------------------------------------*/
/**
  @brief Process the parameter corresponding to
         pPrsr->paramList[pPrsr->curParamIdx].

  Uses cmd_table to find the parameter and call the corresponding handler.

  @param pPrsr[in]: Pointer to control structure containing parser info.
  @param cmd_table [in]: Table of valid params and corresponding handlers.

  @return  DA_PARSER_RVAL_SUCCESS : parameter successfully processed.
           DA_PARSER_RVAL_... : error occurred.
 */
da_parser_rval_t da_parser_param(da_parser_info_t *pPrsr,
                                 const da_parser_table_t *table);

/*-------------------------------------------------------------------------*/
da_parser_rval_t da_parser_get_param(da_parser_info_t *pPrsr, char **pParam);
/*-------------------------------------------------------------------------*/
/**
  @brief Process the parameter corresponding to
         pPrsr->paramList[pPrsr->curParamIdx] as a subsystem name and
         convert to the subsystem id value.

  @param pPrsr[in]: Pointer to control structure containing parser info.
  @param ssid [in/out]: Assigns the subsystem id value to the location
         provided.

  @return  DA_PARSER_RVAL_SUCCESS : parameter successfully processed.
           DA_PARSER_RVAL_... : error occurred.
 */
da_parser_rval_t da_parser_subsys(da_parser_info_t *pPrsr,
                                  da_parser_ssid_t *ssid);

char * da_parser_subsys_str(da_parser_ssid_t ssid);

/*-------------------------------------------------------------------------*/
/**
  @brief Process the parameter corresponding to
         pPrsr->paramList[pPrsr->curParamIdx] as a unsigned integer and
         converts to the numeric value. Converts a maximum of a
         32-bit value.

  @param pPrsr[in]: Pointer to control structure containing parser info.
  @param value [in/out]: Assigns the decimal value to the location provided.
  @param valLen [in]: Sizeof the location. Typically 1, 2, or 4.

  @return  DA_PARSER_RVAL_SUCCESS : parameter successfully processed.
           DA_PARSER_RVAL_... : error occurred.
 */
da_parser_rval_t da_parser_unsigned(da_parser_info_t *pPrsr,
                                    void *pVal,
                                    int valLen);

#endif /* #ifndef DA_PARSER_H */

