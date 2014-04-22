#ifndef __APR_API_I_H__
#define __APR_API_I_H__

/*
  Copyright (C) 2010, 2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //components/rel/audio.tx/2.0/apr/core/inc/apr_api_i.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_api.h"

/*****************************************************************************
 * Core Controls                                                             *
 ****************************************************************************/

APR_INTERNAL int32_t apr_cmd_get_version (
  apr_cmd_get_version_t* params
);

#endif /* __APR_API_I_H__ */

