/*
   Copyright (C) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/stubs/tva_module.c#1 $
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
#include "tva_if.h"

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/
TVA_EXTERNAL uint32_t tva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  switch ( cmd_id )
  {
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "tva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;
}

