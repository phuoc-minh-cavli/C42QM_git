#ifndef __GVA_ICOMMON_IF_H__
#define __GVA_ICOMMON_IF_H__

/**
  @file  gva_icommon_if.h
  @brief This file contains common interface definitions of the GSM Voice
         Adapter.
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/CT/protected/gva_icommon_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------

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

#ifdef __cplusplus
  #define GVA_INTERNAL extern "C"
#else
  #define GVA_INTERNAL extern
#endif /* __cplusplus */
  /* This marks an internal API that is intended for internal use. */


/****************************************************************************
 * GSM VOICE ADAPTER COMMON DEFINITION                                      *
 ****************************************************************************/

/**
 * Event callback provided by a client of the server.
 *
 * The client returns APR_EOK from the callback if it is successful in
 * queuing the event to its own thread context for processing.
 *
 * All other error codes indicate a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32_t ( *gva_icommon_event_callback_fn_t ) (

  void* session_context,
    /**< Client-provided session_context pointer from the OPEN command. */

  uint32_t event_id,
    /**< Event ID. See GVA_IXXX_EVENT_YYY. */

  void* params,
    /**< Pointer to an associated event data structure. */

  uint32_t size
    /**< Size of the event data structure. */

);

/** Set asid-vsid mapping (Asynchronous API).
 *
 * Voice agent uses this commands to provide active subscription Id and 
 * the corresponding VSID mapped to it.
 *
 * GSM voice adapter will use this mapping to correctly configure vocoder 
 * session set-up.
 */
#define GVA_ICOMMON_CMD_SET_ASID_VSID_MAPPING ( 0x000131D7 )

typedef struct gva_icommon_cmd_set_asid_vsid_mapping_t gva_icommon_cmd_set_asid_vsid_mapping_t;

struct gva_icommon_cmd_set_asid_vsid_mapping_t {

  uint32_t asid;
    /**< Modem Active Subscription ID. */

  uint32_t vsid;
    /**< Voice System ID as defined by DCN 80-NF711-1 Rev E. */

};

#endif /* __GVA_ICOMMON_IF_H__ */

