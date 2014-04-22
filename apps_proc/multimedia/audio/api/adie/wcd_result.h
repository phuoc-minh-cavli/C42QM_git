#ifndef WCD_RESULT_INTF_
#define WCD_RESULT_INTF_

/**
@file wcd_result.h
@brief External WCD error/warning codes 
*/
/*-------------------------------------------------------------------------------
    Copyright (c) 2013 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Id: //components/rel/audio.tx/2.0/audio/api/adie/wcd_result.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/01/14   AG      Header update
07/20/13   KNM     Added error code for array index out of bounds.
06/17/13   KNM     Implemented error check for qfuse sense done status.
03/07/13   SSP     Driver Unification Update
03/01/13   KNM     Added error code if ANC is not supported in codec.
12/15/10   SK      Initial revision
===============================================================================*/

/** 
 * \brief The error codes that are returned
 *  by the wcd interface.
 */
typedef enum _wcd_result
{
  E_WCD_ARRAY_INDEX_OUT_OF_BOUNDS = -59,
  
  E_WCD_QFUSE_NOT_DONE_FAILURE = -58,
  
  E_WCD_REACHED_SIDETONE_LIMIT = -57,

  E_WCD_BUS_ERROR = -56,

  E_WCD_ANC_NOT_SUPPORTED = -55,
  
  E_WCD_SIDETONE_NOT_ENABLED = -54,

  E_WCD_SIDETONE_ALREADY_ENABLED = -53,

  E_WCD_INVALID_ANC_CONFIGURATION = -52,

  E_WCD_ANC_ALREADY_ENABLED = -51,

  E_WCD_MISSING_END_OF_STAGE_COMMAND = -50,

  E_WCD_USECASE_STILL_DISABLED = -49,

  E_WCD_UNEXPECTED_COMMAND = -48,

  E_WCD_INVALID_ACDB_COMMAND = -47,

  E_WCD_INVALID_COMMAND_TYPE = -46,

  E_WCD_ACDB_PROFILE_VERSION_INCOMPATIBLE = -45,

  E_WCD_INVALID_PROFILE = -44,

  E_WCD_PROFILE_ALREADY_SET = -43,

  E_WCD_INVALID_CONNECTION_TYPE = -42,

  E_WCD_INVALID_SINK = -41,

  E_WCD_INVALID_SOURCE = -40,

  E_WCD_USECASE_STILL_OPEN = -39,

  E_WCD_USECASE_STILL_ENABLED = -38,
    
  E_WCD_SPEAKER_PROTECTION_CALIBRATION_ERROR = -37,

  E_WCD_UNABLE_TO_READ_CODEC_INFO = -36,

  E_WCD_SLIMBUS_SHARED_CHANNEL_ALREADY_DEFINED = -34,

  E_WCD_SLIMBUS_SHARED_CHANNEL_ERROR = -33,

  E_WCD_INVALID_SLIMBUS_INTERFACE_CONFIGURATION = -32,

  E_WCD_INVALID_BUS_HANDLE = -31,

  E_WCD_INTERRUPT_HANDLER_ERROR = -23,

  E_WCD_PROPERTY_NOT_FOUND = -19,

  E_WCD_NO_CONNECTIONS_DEFINED = -18,

  E_WCD_UNINITIALIZED_DRIVER = -17,

  E_WCD_PROFILE_NOT_SET = -15,

  E_WCD_CONNECTIONS_NOT_DISCONNECTED = -9,

  E_WCD_UNABLE_TO_ROUTE = -8,

  E_WCD_INITIALIZATION_ERROR = -7,

  E_WCD_INVALID_CLIENT_STATE = -6,

  E_WCD_OS_CALL_ERROR = -5,

  E_WCD_INVALID_HANDLE = -4,

  E_WCD_INVALID_PARAMETER_ERROR = -3,
  
  E_WCD_MEMORY_ALLOCATION_ERROR = -2,

  E_WCD_GENERIC_ERROR = -1,

  E_WCD_SUCCESS = 0,

  E_WCD_GENERIC_WARNING = 1,

  E_WCD_CONNECTIONS_BLOCKED = 1,

  E_WCD_EXIT_WORKLOOP = 1,

  E_WCD_EMPTY_LIST = 2,

} wcd_result;


#endif


