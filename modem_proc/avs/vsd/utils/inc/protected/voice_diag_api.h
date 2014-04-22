#ifndef __VOICE_DIAG_API_H__
#define __VOICE_DIAG_API_H__

/*
  ============================================================================
  Copyright (C) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/inc/protected/voice_diag_api.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include file
****************************************************************************/

#include "apr_comdef.h"

/****************************************************************************
 * CALL API                                                                 *
 ****************************************************************************/
/**
  Provides a single API entry point to the VOICE DIAG utility.
 
  @param[in] index       Command ID.
  @param[in,out] params   Command payload.
  @param[in] size         Size of the command payload.
   
  @return
  APR_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/

int32_t voice_diag_call (
 uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif  /* VOICE_DIAG_API_H */

