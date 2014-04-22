/**
  @file vs_vochr.c
  @brief This file contains the definitions of the interfaces for GSM-HR
         vocoder.

*/

/*
  ============================================================================
  Copyright (C) 2013-2015, 2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_vochr.c#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
#include "vs_i.h"


/****************************************************************************
 * FUNCTIONS                                                                *
 ****************************************************************************/

static uint32_t vs_vochr_publish_params (
  void* this
)
{
  //vs_session_object_t* session_obj = (vs_session_object_t*)this;

  //TBD
  return VS_EOK;
}

static uint32_t vs_vochr_set_cached_params_handler (
  void* this
)
{
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  ( void ) vs_voc_action_set_dtx_v2( session_obj );

  return VS_EOK;
}


static uint32_t vs_vochr_set_param_handler (
 void* this,
 uint32_t param_id,
 void* param_data
)
{
  uint32_t rc = VS_EOK;
  //vs_session_object_t* session_obj = (vs_session_object_t*)this;

  switch( param_id )
  {
    default:
      rc = VS_EUNSUPPORTED;
      break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_vochr_init (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;

  session_obj->vocoder.set_param_fn = vs_vochr_set_param_handler;
  session_obj->vocoder.set_cached_param_fn = vs_vochr_set_cached_params_handler;
  session_obj->vocoder.publish_param_fn = vs_vochr_publish_params;

  return rc;
}


