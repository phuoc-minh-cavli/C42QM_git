#ifndef __VAGENT_H__
#define __VAGENT_H__

/**
  @file  vagent.h
  @brief This is the public header file that contains single entry point 
         for the voice agent.*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vagent/vagent.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/


/*----------------------------------------------------------------------------
 * Include files for Module
 *--------------------------------------------------------------------------*/
#include "vagent_ivoice_if.h"

/*----------------------------------------------------------------------------
  Export Macros
----------------------------------------------------------------------------*/

#ifdef __cplusplus
  #define VAGENT_EXTERNAL extern "C"
#else
  #define VAGENT_EXTERNAL extern
#endif /* __cplusplus */
  /* This marks an external API that is intended for public use. */


/***********************************************************************
  VAGENT SINGLE ENTRY POINT
 ***********************************************************************/

/**
 * Provides a single API entry point into the Voice Agent.
 *
 * @param[in] event_id event identifier to execute.
 * @param[in] params event parameters.
 * @param[in] size Size of the event parameters in bytes.V
 *
 * @return
 *  None
 */
VAGENT_EXTERNAL uint32_t vagent_call (
  uint32_t cmd_id,
    /**< Command ID. */
  void*  payload,
    /**< Reference to command-ID specific payload structure.
      *  Client is responsible allocating memory required for 
      *  command payload. */
  uint32_t size
    /**< Size of the payload structure. */
);

#endif /* __VAGENT_H__ */

