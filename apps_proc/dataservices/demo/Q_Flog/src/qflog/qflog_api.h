/*!
  @file
  qflog_api.h

  @brief
  QFLOG framework functionality
*/
/*===========================================================================
  Copyright (c) 2018  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/

#ifndef __QFLOG_API_H__  // [
#define __QFLOG_API_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"
#include "qapi_socket.h"

void*
qflog_memcpy(void *d, const void *s, size_t n);


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define QFLOG_MEMCPY(d, s, l)    qflog_memcpy((void *)(d), (void *)(s), (int32_t)(l))
#define QFLOG_IP6CPY(a,b)         QFLOG_MEMCPY((a),(b),sizeof(struct in6_addr))

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
typedef unsigned		   int uintptr_t;

/**
   This enumeration represents the valid error codes that can be returned
   by the command functions.  They represent a success, a general error or 
   a usage error.
*/
typedef enum 
{
   QFLOG_STATUS_SUCCESS_E,
   QFLOG_STATUS_ERROR_E,
   QFLOG_STATUS_USAGE_E
} QFLOG_Command_Status_t;

/** 
   This type represents a group handle.                                
*/
typedef void *QFLOG_Group_Handle_t;

/**
   This structure contains the information for a single parameter entered
   into the command line.  It contains the string value (as entered), the
   integer value (if the string could be successfully converted) and a
   boolean flag which indicates if the integer value is valid.
*/
typedef struct QFLOG_Parameter_s
{
   uint8_t *String_Value;
   int32_t  Integer_Value;
   qbool_t  Integer_Is_Valid;
} QFLOG_Parameter_t;


#endif   // ] #ifndef __QCLI_API_H__

