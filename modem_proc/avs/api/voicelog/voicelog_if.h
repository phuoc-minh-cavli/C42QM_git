#ifndef __VOICELOG_IF_H__
#define __VOICELOG_IF_H__

/**
  @file  voicelog_if.h
  @brief This is the public header file that clients should include
         for voice logging support on modem sub-system.
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/api/voicelog/voicelog_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"

/* APR APIs. */
#include "apr_comdef.h"
#include "apr_errcodes.h"

/* VOICELOG APIs */
#include "voicelog_ipacket_if.h"
#include "voicelog_ievent_if.h"
#include "voicelog_icommon_if.h"

/****************************************************************************
 * CALL API                                                                 *
 ****************************************************************************/
/**
  Provides a single API entry point to the VOICELOG utility.
 
  @param[in] cmd_id       Command ID.
  @param[in,out] params   Command payload.
  @param[in] size         Size of the command payload.
   
  @return
  APR_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/
APR_EXTERNAL uint32_t voicelog_call (

  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif /* __VOICELOG_IF_H__ */
