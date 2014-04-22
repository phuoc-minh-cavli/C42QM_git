#ifndef __TVA_IF_H__
#define __TVA_IF_H__

/**
  @file  tva_if.h
  @brief This is the public header file that clients of TVA should include.
         This file includes all other TVA public header files and contains
         single entry point into the TVA.
*/

/*
  ============================================================================
  Copyright (C) 2015, 2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/protected/tva_if.h#1 $
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
  #define TVA_EXTERNAL extern "C"
#else
  #define TVA_EXTERNAL extern
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
TVA_EXTERNAL uint32_t tva_call (

  uint32_t cmd_id,
  void* params,
  uint32_t size

);

#endif /* __TVA_IF_H__ */

