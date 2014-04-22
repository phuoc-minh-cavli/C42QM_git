/*
   DESCRIPTION
   This file contains the stub definitions of voice diag utility

   Copyright (C) 2017- 2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/stubs/voice_diag.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* CORE APIs. */
#include "msg.h"
#include "mmstd.h"
#include "drv_api.h"

/* APR APIs. */
#include "apr_errcodes.h"

/* VSD APIs. */
#include "voice_diag_api.h"

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/
int32_t voice_diag_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( cmd_id )
  {
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "voice_diag_call(): Unsupported callindex (%x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }
  return rc;
}
