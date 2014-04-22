/**
  @file vs.c
  @brief This file contains the stub definitions of the public interfaces of VS.

  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
   
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/stubs/vs.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* CORE APIs. */
#include "msg.h"
#include "mmstd.h"

/* Self APIs. */
#include "vs.h"

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/
VS_EXTERNAL uint32_t vs_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = VS_EOK;

  switch ( cmd_id )
  {
    default:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "Unsupported command ID(%x)", cmd_id );
        rc = VS_EUNSUPPORTED;
      }
      break;
  }  /* end of switch ( cmd_id )*/

  return rc;
}  /* end of vs_call () */
