#ifndef __GVA_IF_H__
#define __GVA_IF_H__

/**
  @file  gva_if.h
  @brief This is the public header file that clients of GVA should include.
         This file includes all other GVA public header files and contains
         single entry point into the GVA.
*/

/*
  ============================================================================
  Copyright (C) 2015, 2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/protected/gva_if.h#1 $
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
  #define GVA_EXTERNAL extern "C"
#else
  #define GVA_EXTERNAL extern
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
GVA_EXTERNAL uint32_t gva_call (

  uint32_t cmd_id,
  void* params,
  uint32_t size

);

#endif /* __GVA_IF_H__ */

