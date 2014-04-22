#ifndef __VAGENT_IF_H__
#define __VAGENT_IF_H__

/**
  @file  vagent_if.h
  @brief This is the public header file that contains single entry point 
         into the voice agent.*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vagent/inc/protected/vagent_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/

/*----------------------------------------------------------------------------
  Export Macros
----------------------------------------------------------------------------*/

#ifdef __cplusplus
  #define VAGENT_EXTERNAL extern "C"
#else
  #define VAGENT_EXTERNAL extern
#endif /* __cplusplus */
  /* This marks an external API that is intended for public use. */

#ifdef __cplusplus
  #define VAGENT_INTERNAL extern "C"
#else
  #define VAGENT_INTERNAL extern
#endif /* __cplusplus */
  /* This marks an internal API that is intended for internal use. */

/*----------------------------------------------------------------------------
  VAGENT Call function.
----------------------------------------------------------------------------*/

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
  void* params,
  uint32_t size

);

#endif /* __VAGENT_IF_H__ */

