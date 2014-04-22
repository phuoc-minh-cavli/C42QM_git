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

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/CT/protected/gva_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/


/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "gva_icommon_if.h"


/*----------------------------------------------------------------------------
  GVA Call function.
----------------------------------------------------------------------------*/

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

