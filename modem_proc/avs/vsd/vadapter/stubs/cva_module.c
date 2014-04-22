/*
   Copyright (C) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/stubs/cva_module.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* CORE APIs. */
#include "msg.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"

/* SELF APIs. */
#include "cva_if.h"

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/
CVA_EXTERNAL uint32_t cva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;

  switch ( cmd_id )
  {
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;
}

