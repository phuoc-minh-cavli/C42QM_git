#ifndef __WVA_IF_H__
#define __WVA_IF_H__

/**
  @file  wva_if.h
  @brief This is the public header file that clients of WVA should include.
         This file includes all other WVA public header files and contains
         single entry point into the WVA.
*/

/*
  ============================================================================
  Copyright (C) 2015, 2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/protected/wva_if.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/


/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/
#include "mmdefs.h"

 /*----------------------------------------------------------------------------
   Export Macros
 ----------------------------------------------------------------------------*/

#ifdef __cplusplus
  #define WVA_EXTERNAL extern "C"
#else
  #define WVA_EXTERNAL extern
#endif /* __cplusplus */
   /* This marks an external API that is intended for public use. */


/**
 * Provides a single API entry point into the GSM Voice Adapter.
 *
 * @param[in] event_id event identifier to execute.
 * @param[in] params event parameters.
 * @param[in] size Size of the event parameters in bytes.V
 *
 * @return
 * None
 */
WVA_EXTERNAL uint32_t wva_call (

  uint32_t cmd_id,
  void* params,
  uint32_t size

);

#endif /* __WVA_IF_H__ */

